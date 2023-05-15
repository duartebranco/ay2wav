/*****************************************************************************

        Ay38910.cpp
        Copyright (c) 2006 Laurent de Soras

Important notes about AY-3-8910 functioning. Most of them are taken from the
MAME emulator:

* Tone and noise mixing

The 8910 has three outputs, each output is the mix of one of the three tone
generators and of the (single) noise generator. The two are mixed BEFORE
going into the DAC. The formula to mix each channel is:

	(ToneOn | ToneDisable) & (NoiseOn | NoiseDisable).

Note that this means that if both tone and noise are disabled, the output
is 1, not 0, and can be modulated changing the volume.

* Tone, noise and envelope periods

Careful studies of the chip output prove that it counts up from 0 until the
counter becomes greater or equal to the period. This is important when the
program is rapidly changing the period to modulate the sound.

Also, note that period = 0 is the same as period = 1. However, this does NOT
apply to the Envelope period. In that case, period = 0 is half as period = 1.

Counters do not seem modified if the channel is disabled. This is important
when the program is rapidly modulating the volume.

* Clocks

The step clock for the tone and noise generators is the chip clock divided
by 8; for the envelope generator of the AY-3-8910, it is half that much
(clock / 16), but the envelope of the YM2149 goes twice as fast, therefore
again clock / 8.

The MAME implementation uses a sub-sample (DAC-wise) accuracy to locate the
edge of the waveforms. This makes a difference when cutting a waveform after
having set its period to a smaller value that it had previously. However it's
difficult to evaluate if the real chip exhibits this behaviour or not. This
implementation does not have the sub-sample accuracy.

* Envelope counter

The envelope counter on the AY-3-8910 has 16 steps. On the YM2149 it has
twice the steps, happening twice as fast. Since the end result is just a
smoother curve, we always use the YM2149 behaviour.

* Random generator

The Random Number Generator of the 8910 is a 17-bit shift register. The input
to the shift register is bit 0 XOR bit 3 (bit 0 is the output). This was
verified on AY-3-8910 and YM2149 chips.

The following is a fast way to compute bit 17 = bit 0 ^ bit 3. Instead of
doing all the logic operations, we only check bit 0, relying on the fact that
after three shifts of the register, what now is bit 3 will become bit 0, and
will invert, if necessary, bit 14, which previously was bit 17. This version
is called the "Galois configuration".

* Volume levels

The AY-3-8910 has 16 levels, in a logarithmic scale (3 dB per step). The
YM2149 still has 16 levels for the tone generators, but 32 for the envelope
generator (1.5 dB per step).

* I/O Ports

Even if the port is set as output, we still need to return the external data.
Some games, like Kidniki, need this to work.

--- Legal stuff ---

This file is part of AY2Wav.

AY2Wav is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

AY2Wav is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with AY2Wav; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130) // "'operator' : logical operation on address of string constant"
	#pragma warning (1 : 4223) // "nonstandard extension used : non-lvalue array converted to pointer"
	#pragma warning (1 : 4705) // "statement has no effect"
	#pragma warning (1 : 4706) // "assignment within conditional expression"
	#pragma warning (4 : 4786) // "identifier was truncated to '255' characters in the debug information"
	#pragma warning (4 : 4800) // "forcing value to bool 'true' or 'false' (performance warning)"
	#pragma warning (4 : 4355) // "'this' : used in base member initializer list"
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"Ay38910.h"
#include	"Ay38910CbInterface.h"
#include	"fnc.h"

#include	<cassert>
#include	<cstring>

namespace std { }



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: ctor
Description:
	The AY-4-8910 is created and reset. There is no callback.
Throws: Nothing.
==============================================================================
*/

Ay38910::Ay38910 ()
:	_cb_ptr (0)
,	_register_latch (static_cast <Reg> (0))
,	_reg_arr ()
,	_last_enable (-1)
,	_rnd_state (1)
,	_noise_period (1)
,	_noise_count (1)
,	_noise_step (0)
,	_noise_state (0)
,	_noise_out (0)
,	_env_period (1)
,	_env_count (1)
,	_env_vol (0)
,	_env_hold_flag (true)
,	_env_alternate_flag (false)
,	_env_continue_flag (false)
,	_env_attack (0)
,	_env_val (0x1F)
{
	using namespace std;

	for (int chn_cnt = 0; chn_cnt < NBR_CHN; ++chn_cnt)
	{
		Channel &		chn = _chn_arr [chn_cnt];
		chn._enable_mask = 1 << chn_cnt;
		chn._reg_vol = Reg_A_VOL + chn_cnt;
	}

	if (! _vol_table_init_flag)
	{
		build_vol_table ();
	}

	reset ();
}



/*
==============================================================================
Name: set_callback
Description:
	Sets the callback for writing to or reading ports A and B. Callback object
	is not mandatory.
Input/output parameters:
	- cb: Callback object.
Throws: Nothing
==============================================================================
*/

void	Ay38910::set_callback (Ay38910CbInterface &cb)
{
	assert (&cb != 0);

	_cb_ptr = &cb;
}



/*
==============================================================================
Name: reset
Description:
	Resets the AY-3-8910.
Throws: Nothing.
==============================================================================
*/

void	Ay38910::reset ()
{
	_register_latch = static_cast <Reg> (0);
	memset (&_reg_arr, 0, sizeof (_reg_arr));
	_last_enable = 0;
	for (int chn = 0; chn < NBR_CHN; ++chn)
	{
		_chn_arr [chn].reset ();
	}

	_rnd_state = 1;
	_noise_period = 1;
	_noise_count = 1;
	_noise_step = 1;
	_noise_state = 0xFF;
	_noise_out = 0;

	_env_period = 1;
	_env_count = 1;
	_env_vol = 0;
	_env_hold_flag = true;
	_env_alternate_flag = false;
	_env_continue_flag = false;
	_env_attack = 0;
	_env_val = 0x1F;

	if (_cb_ptr != 0)
	{
		_cb_ptr->write_ay3_port_a (0xFF);
		_cb_ptr->write_ay3_port_b (0xFF);
	}
}



/*
==============================================================================
Name: latch_address
Description:
	Selects the current register for reading/writing.
Input parameters:
	- reg: The register index.
Throws: Nothing
==============================================================================
*/

void	Ay38910::latch_address (Reg reg)
{
	assert (reg >= 0);
	assert (reg < Reg_NBR_ELT);

	_register_latch = reg;
}



/*
==============================================================================
Name: write_data
Description:
	Write the data into the current register.
Input parameters:
	- data: Byte to write [0x00 ; 0xFF]
Throws: Nothing
==============================================================================
*/

void	Ay38910::write_data (int data)
{
	assert (data >= 0);
	assert (data <= 255);

	write_reg (_register_latch, data);
}



/*
==============================================================================
Name: read_data
Description:
	Read the current register value
Returns: The value (byte)
Throws: Nothing
==============================================================================
*/

int	Ay38910::read_data ()
{
	switch (_register_latch)
	{
	case	Reg_PORT_A:
		if (_cb_ptr != 0)
		{
			_reg_arr [Reg_PORT_A] = _cb_ptr->read_ay3_port_a ();
		}
		break;
	case	Reg_PORT_B:
		if (_cb_ptr != 0)
		{
			_reg_arr [Reg_PORT_B] = _cb_ptr->read_ay3_port_b ();
		}
		break;
	default:
		// Nothing
		break;
	}

	return (_reg_arr [_register_latch]);
}



/*
==============================================================================
Name: process_block
Description:
	Generates a block of audio data. Data is in the range [0 ; MAX_OUTPUT]
Input parameters:
	- nbr_spl: Number of samples to produce.
Output parameters:
	- chn_ptr_arr: Array of 3 pointers on the data for channels A, B and C.
Throws: Nothing
==============================================================================
*/

void	Ay38910::process_block (Int16 *chn_ptr_arr [NBR_CHN], long nbr_spl)
{
	assert (chn_ptr_arr != 0);
	assert (chn_ptr_arr [0] != 0);
	assert (chn_ptr_arr [1] != 0);
	assert (chn_ptr_arr [2] != 0);
	assert (nbr_spl > 0);

	Int16 *			buf_a_ptr = chn_ptr_arr [0];
	Int16 *			buf_b_ptr = chn_ptr_arr [1];
	Int16 *			buf_c_ptr = chn_ptr_arr [2];

	long				pos = 0;
	do
	{
		const int		out_a = process_chn (_chn_arr [0], 0x08);
		const int		out_b = process_chn (_chn_arr [1], 0x10);
		const int		out_c = process_chn (_chn_arr [2], 0x20);

		buf_a_ptr [pos] = static_cast <Int16> (out_a);
		buf_b_ptr [pos] = static_cast <Int16> (out_b);
		buf_c_ptr [pos] = static_cast <Int16> (out_c);

		process_noise ();
		process_envelope ();

		++ pos;
	}
	while (pos < nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Ay38910::Channel::Channel ()
:	_reg_vol (-1)
,	_enable_mask (-1)
,	_period (1)
,	_count (1)
,	_step (0)
,	_vol (0)
,	_output (0)
,	_env_flag (false)
{
	// Nothing
}



void	Ay38910::Channel::reset ()
{
	assert (_reg_vol >= 0);
	assert (_enable_mask >= 0);

	_period = 1;
	_count = 1;
	_step = 0;
	_vol = 0;
	_output = 0;
	_env_flag = false;
}



void	Ay38910::write_reg (int reg, int val)
{
	assert (reg >= 0);
	assert (reg < Reg_NBR_ELT);
	assert (val >= 0x00);
	assert (val <= 0xFF);

	if (reg < Reg_PORT_A && val != _reg_arr [reg] && _cb_ptr != 0)
	{
		_cb_ptr->signal_ay3_state_about_to_change ();
	}

	const int		mask = _reg_mask_arr [reg];
	_reg_arr [reg] = val & mask;

	switch (reg)
	{
	case	Reg_A_PER_LSB:
	case	Reg_A_PER_MSB:
		update_chn_period (_chn_arr [0], Reg_A_PER_LSB, Reg_A_PER_MSB);
		break;

	case	Reg_B_PER_LSB:
	case	Reg_B_PER_MSB:
		update_chn_period (_chn_arr [1], Reg_B_PER_LSB, Reg_B_PER_MSB);
		break;

	case	Reg_C_PER_LSB:
	case	Reg_C_PER_MSB:
		update_chn_period (_chn_arr [2], Reg_C_PER_LSB, Reg_C_PER_MSB);
		break;

	case	Reg_N_PER:
		update_noise_period ();
		break;

	case	Reg_ENABLE:
		update_enable ();
		break;

	case	Reg_A_VOL:
		update_chn_vol (_chn_arr [0]);
		break;

	case	Reg_B_VOL:
		update_chn_vol (_chn_arr [1]);
		break;

	case	Reg_C_VOL:
		update_chn_vol (_chn_arr [2]);
		break;

	case	Reg_E_PER_LSB:
	case	Reg_E_PER_MSB:
		update_env_period ();
		break;

	case	Reg_E_SHAPE:
		update_env_shape ();
		break;

	case	Reg_PORT_A:
		if ((_reg_arr [Reg_ENABLE] & 0x40) != 0 && _cb_ptr != 0)
		{
			_cb_ptr->write_ay3_port_a (_reg_arr [Reg_PORT_A]);
		}
		break;

	case	Reg_PORT_B:
		if ((_reg_arr [Reg_ENABLE] & 0x80) != 0 && _cb_ptr != 0)
		{
			_cb_ptr->write_ay3_port_b (_reg_arr [Reg_PORT_B]);
		}
		break;

	default:
		assert (false);
		break;
	}
}



void	Ay38910::update_enable ()
{
	const int		enable = _reg_arr [Reg_ENABLE];

	update_chn_activity (_chn_arr [0]);
	update_chn_activity (_chn_arr [1]);
	update_chn_activity (_chn_arr [2]);

	_noise_step = ((enable & 0x38) == 0x38) ? 0 : 1;	// All off
	_noise_out = (_noise_state | enable);

	if (   (_last_enable < 0)
	    || ((_last_enable & 0x40) != (enable & 0x40)))
	{
		// Write out 0xFF if port set to input
		if (_cb_ptr != 0)
		{
			_cb_ptr->write_ay3_port_a (
				  ((enable & 0x40) != 0)
				? _reg_arr [Reg_PORT_A]
				: 0xFF
			);
		}
	}

	if (   (_last_enable < 0)
	    || ((_last_enable & 0x80) != (enable & 0x80)))
	{
		// Write out 0xFF if port set to input
		if (_cb_ptr != 0)
		{
			_cb_ptr->write_ay3_port_b (
				  ((enable & 0x80) != 0)
				? _reg_arr [Reg_PORT_B]
				: 0xFF
			);
		}
	}

	_last_enable = enable;
}



void	Ay38910::update_noise_period ()
{
	const int		old_period = _noise_period;

	_noise_period = _reg_arr [Reg_N_PER];
	_noise_period = compute_max (_noise_period, 1);

	const int		offset = _noise_period - old_period;
	_noise_count += offset;
	_noise_count = compute_max (_noise_count, 1);
}



void	Ay38910::update_env_period ()
{
	const int		old_period = _env_period;

	_env_period = ((_reg_arr [Reg_E_PER_MSB] << 8) + _reg_arr [Reg_E_PER_LSB]);
	_env_period *= ENV_RESOL;
	_env_period = compute_max (_env_period, 1);

	const int		offset = _env_period - old_period;
	_env_count += offset;
	_env_count = compute_max (_env_count, 1);
}



void	Ay38910::update_env_shape ()
{
	_env_attack = ((_reg_arr [Reg_E_SHAPE] & 0x04) != 0) ? 0x1F : 0x00;

	// If Continue = 0, map the shape to the equivalent one which has Continue = 1
	if ((_reg_arr [Reg_E_SHAPE] & 0x08) == 0)
	{
		_env_hold_flag = true;
		_env_alternate_flag = (_env_attack != 0);
	}
	else
	{
		_env_hold_flag      = ((_reg_arr [Reg_E_SHAPE] & 0x01) != 0);
		_env_alternate_flag = ((_reg_arr [Reg_E_SHAPE] & 0x02) != 0);
	}

	_env_count = _env_period;
	_env_val = 0x1F;
	_env_continue_flag = false;
	set_env_volume ();
}



void	Ay38910::update_chn_period (Channel &chn, Reg reg_fine, Reg reg_coarse)
{
	assert (&chn != 0);
	assert (reg_fine >= 0);
	assert (reg_fine < Reg_NBR_ELT);
	assert (reg_coarse >= 0);
	assert (reg_coarse < Reg_NBR_ELT);

	const int		old_period = chn._period;

	chn._period = (_reg_arr [reg_coarse] << 8) + _reg_arr [reg_fine];
	chn._period = compute_max (chn._period, 1);

	const int		offset = chn._period - old_period;
	chn._count += offset;
	chn._count = compute_max (chn._count, 1);
}



void	Ay38910::update_chn_vol (Channel &chn)
{
	assert (&chn != 0);

	const int		vol = _reg_arr [chn._reg_vol];

	chn._env_flag = ((vol & 0x10) != 0);
	if (chn._env_flag)
	{
		chn._vol = _env_vol;
	}
	else
	{
		int				table_index = 0;
		if (vol > 0)
		{
			table_index = vol * 2 + 1;
		}
		chn._vol = _vol_table [table_index];
	}

	update_chn_activity (chn);
}



void	Ay38910::update_chn_activity (Channel &chn)
{
	assert (&chn != 0);

	if ((_reg_arr [Reg_ENABLE] & chn._enable_mask) != 0)
	{
		chn._step = 0;
		chn._output = 1;
	}

	else
	{
		chn._step = (_reg_arr [chn._reg_vol] != 0) ? 1 : 0;
	}
}



int	Ay38910::process_chn (Channel &chn, int noise_mask)
{
	assert (&chn != 0);
	assert (noise_mask != 0);
	assert (chn._step   == 0 || chn._step   == 1);
	assert (chn._output == 0 || chn._output == 1);

	int				out = 0;

	if ((_noise_out & noise_mask) != 0)
	{
		out = chn._output * chn._vol;
	}

	-- chn._count;
	if (chn._count <= 0)
	{
		assert (chn._count == 0);
		chn._count = chn._period;
		chn._output ^= chn._step;
	}

	return (out);
}



void	Ay38910::process_noise ()
{
	_noise_count -= _noise_step;
	if (_noise_count <= 0)
	{
		// Is noise output going to change ?
		if (((_rnd_state + 1) & 2) != 0)	// (bit 0 ^ bit 1) ?
		{
			_noise_state = ~_noise_state;
			_noise_out = (_noise_state | _reg_arr [Reg_ENABLE]);
		}

		if (_rnd_state & 1)
		{
			_rnd_state ^= 0x24000;
		}
		_rnd_state >>= 1;
		_noise_count += _noise_period;
	}
}



void	Ay38910::process_envelope ()
{
	if (! _env_continue_flag)
	{
		_env_count -= ENV_RESOL;
		if (_env_count <= 0)
		{
			do
			{
				--_env_val;
				_env_count += _env_period;
			}
			while (_env_count <= 0);

			// Check envelope current position
			if (_env_val < 0)
			{
				if (_env_hold_flag)
				{
					if (_env_alternate_flag)
					{
						_env_attack ^= 0x1F;
					}
					_env_continue_flag = true;
					_env_val = 0;
				}
				else
				{
					// If _env_val has looped an odd number of times
					// (usually 1), invert the output.
					if (_env_alternate_flag && (_env_val & 0x20) != 0)
					{
 						_env_attack ^= 0x1F;
					}

					_env_val &= 0x1F;
				}
			}

			set_env_volume ();
		}
	}
}



void	Ay38910::set_env_volume ()
{
	_env_vol = _vol_table [_env_val ^ _env_attack];

	for (int chn = 0; chn < NBR_CHN; ++chn)
	{
		if (_chn_arr [chn]._env_flag)
		{
			_chn_arr [chn]._vol = _env_vol;
		}
	}
}



/*
The AY3 data sheet specifies 1.5 dB/step. From measure tests, it seems that
the difference is more something fading from 0.5 dB/step at maximum volume
up to 1.5 dB/step at minimum volume.
*/

void	Ay38910::build_vol_table ()
{
	_vol_table_init_flag = true;

	double			out = MAX_OUTPUT;
	const double	div_beg = 1.05925372517729;	// = 10 ^ (0.5/20) -> 0.5 dB
	const double	div_end = 1.18850222743702;	// = 10 ^ (1.5/20) -> 1.5 dB
	double			div = div_beg;
	double			div_step = (div_end - div_beg) / 32;
	for (int i = 31; i > 0; --i)
	{
		_vol_table [i] = static_cast <int> (round_long (out));

		out /= div;
		div += div_step;
	}
	_vol_table [0] = 0;
}



int	Ay38910::_vol_table [32];
bool	Ay38910::_vol_table_init_flag = false;

const UByte	Ay38910::_reg_mask_arr [16] =
{
	0xFF, 0x0F,	// Tone A Period
	0xFF, 0x0F,	// Tone B Period
	0xFF, 0x0F,	// Tone C Period
	0x1F,			// Noise Period
	0xFF,			// Mixer Control-I/O Enable
	0x1F,			// Channel A Amplitude
	0x1F,			// Channel B Amplitude
	0x1F,			// Channel C Amplitude
	0xFF, 0xFF,	// Envelope Period
	0x0F,			// Envelope Shape/Cycle
	0xFF,			// Port A
	0xFF,			// Port B
};



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
