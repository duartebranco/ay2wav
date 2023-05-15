/*****************************************************************************

        Emulator.cpp
        Copyright (c) 2006 Laurent de Soras

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

#include	"Emulator.h"
#include	"fnc.h"

#include	<cassert>
#include	<cmath>
#include	<cstring>

namespace std { }



static const UInt16	Emulator_zx_port_mask = 0xC002U;
static const UInt16	Emulator_zx_port_reg  = 0xFFFD & Emulator_zx_port_mask;
static const UInt16	Emulator_zx_port_data = 0xBFFD & Emulator_zx_port_mask;



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: ctor
Description:
	Creates the emulator and sets it to the reset state.
Throws: ?
==============================================================================
*/

Emulator::Emulator ()
:	_mem ()
,	_ay3 ()
,	_z80 ()
,	_i8255 (_ay3)
,	_type (TypeHrdw_UNKNOWN)
,	_interrupt_freq (50)
,	_clock_rate_z80 (4000000L)
,	_clock_rate_ay3 (0)
,	_sample_freq (0)
,	_rspl_rate (0)
,	_clk_cnt_z80 (0)
,	_snd_dmp_clk_z80 (0)
,	_nxt_int_clk_z80 (0)
,	_res_clk_ay3 (0)
,	_interrupt_per (0)
,	_chn_activity (0xFF)
,	_conv (MixConv_MONO)
,	_ay3_out_fifo_arr ()
,	_rspl_arr ()
,	_debug_info ()
{
	_z80.set_callback (*this);
	_ay3.set_callback (*this);

	reset ();
}




/*
==============================================================================
Name: reset
Description:
	Call this to initialise emulator.
	Memory is cleared ; you can fill it with 	write_block_to_mem()
	Interrupt rate is set to 50 Hz.
	Z80 frequency is set to 4 MHz.
	AY-3-8910 frequency is not set.
	Hardware type is undetermined.
Throws: Nothing
==============================================================================
*/

void	Emulator::reset ()
{
	_i8255.reset ();
	_z80.reset ();
	_ay3.reset ();
	_type = TypeHrdw_UNKNOWN;
	_interrupt_freq = 50;
	_clock_rate_z80 = 4000000L;
	_clock_rate_ay3 = 0;
	_sample_freq = 0;
	_rspl_rate = 0;
	_clk_cnt_z80 = 0;
	_snd_dmp_clk_z80 = 0;
	_nxt_int_clk_z80 = 0;
	_res_clk_ay3 = 0;
	_chn_activity = 0xFF;
	_conv = MixConv_MONO;
	update_interrupt_period ();
	for (int ay3_chn = 0; ay3_chn < Ay38910::NBR_CHN; ++ay3_chn)
	{
		_ay3_out_fifo_arr [ay3_chn].clear ();
	}
	for (int audio_chn = 0; audio_chn < MAX_NBR_AUDIO_CHN; ++audio_chn)
	{
		_rspl_arr [audio_chn].clear_buffers ();
	}

	using namespace std;

	// Fill #0000-#00FF range with #C9 value
	memset (&_mem [0x0000], 0xC9, 0x0100);	// #C9: RET

	// Fill #0100-#3FFF range with #FF value
	memset (&_mem [0x0100], 0xFF, 0x3F00);	// #FF: RST #38

	// Fill #4000-#FFFF range with #00 value
	memset (&_mem [0x4000], 0x00, 0xC000);

	_debug_info.clear ();

	// Place to #0038 address #FB value
	_mem [0x0038] = 0xFB;						// #FB: EI
	_debug_info.set_addr_as_init (0x0038);	// EI
	_debug_info.set_addr_as_init (0x0039);	// RET
}



/*
==============================================================================
Name: write_block_to_mem
Description:
	Stores a block of data into the emulator's memory. If the block goes out of
	the valid address space, it is automatically truncated.
Input parameters:
	- data_ptr: Pointer on the data to store.
	- addr: Address where it will be loaded in the Z80 address space. Range:
		[0x0000 ; 0xFFFF]
	- len: Length of the block, > 0.
Throws: Nothing
==============================================================================
*/

void	Emulator::write_block_to_mem (const UByte data_ptr [], UInt16 addr, long len)
{
	assert (data_ptr != 0);
	assert (len > 0);

	if (addr + len > 65536L)
	{
		len = 65536L - addr;
	}

	for (long pos = 0; pos < len; ++pos)
	{
		_mem [addr + pos] = data_ptr [pos];
		_debug_info.set_addr_as_init (addr + pos);
	}
}



/*
==============================================================================
Name: write_word_to_mem
Description:
	Writes a word into the emulator's memory. If the word goes out of the valid
	address space, it is automatically truncated.
Input parameters:
	- addr: Address where to load the word. Range: [0x0000 ; 0xFFFF]
	- data: Word to write ([0x0000 ; 0xFFFF]).
Throws: Nothing
==============================================================================
*/

void	Emulator::write_word_to_mem (UInt16 addr, UInt16 data)
{
	_mem [addr + 0] = data & 0xFF;
	_debug_info.set_addr_as_init (addr + 0);
	if (addr <= 0xFFFEL)
	{
		_mem [addr + 1] = data >> 8;
		_debug_info.set_addr_as_init (addr + 1);
	}
}



/*
==============================================================================
Name: set_debug_mode
Description:
	Enable or disable debug mode.
Input parameters:
	- flag: true to enable.
Throws: Nothing
==============================================================================
*/

void	Emulator::set_debug_mode (bool flag)
{
	_debug_info.set_debug_mode (flag);
}



/*
==============================================================================
Name: allow_port_reading
Description:
	Allows or prohibit reading of AY3 ports
Input parameters:
	- flag: true to allow.
Throws: Nothing
==============================================================================
*/

void	Emulator::allow_port_reading (bool flag)
{
	_debug_info.allow_port_reading (flag);
}



/*
==============================================================================
Name: use_debug_info
Description:
	Access the debugging information (read only)
Returns: Reference on the debugging information.
Throws: Nothing
==============================================================================
*/

const DebugInfo &	Emulator::use_debug_info () const
{
	return (_debug_info);
}



/*
==============================================================================
Name: set_interrupt_rate
Description:
	Change the interrupt frequency. This function can be called at any time.
Input parameters:
	- inter_freq: The interrupt frequency, in Hz. > 0.
Throws: Nothing
==============================================================================
*/

void	Emulator::set_interrupt_rate (double inter_freq)
{
	assert (inter_freq > 0);

	_interrupt_freq = inter_freq;
	update_interrupt_period ();
}



/*
==============================================================================
Name: start
Description:
	Begins the emulation.
	Call this function after the memory filling. Runs the program until exact
	machine type is detected or too much time has been spent.
	After a successful return, the emulator is ready to generate audio.
	If the function fails, restart all from reset().
Input parameters:
	- sample_freq: Sampling frequency of the audio data to generate. Hz, > 0.
	- conv: Output audio format.
	- init: address of the Z80 initialisation routine.
	- interrupt: address of the Z80 interrupt routine.
	- sp: Address of the Z80 stack.
	- regfill: Filling value for the Z80 common registers.
	- type_emul: hardware emulation type
Returns:
	0: Everything is OK, hardware type was detected after a reasonable time.
	negative number: detection error
Throws: ?
==============================================================================
*/

int	Emulator::start (double sample_freq, MixConv conv, UInt16 init, UInt16 interrupt, UInt16 sp, UInt16 regfill, TypeEmul type_emul)
{
	assert (sample_freq > 0);
	assert (conv >= 0);
	assert (conv < MixConv_NBR_ELT);
	assert (init != 0);
	assert (type_emul >= 0);
	assert (type_emul < TypeEmul_NBR_ELT);

	_sample_freq = sample_freq;
	_conv = conv;

	prepare (init, interrupt, sp, regfill);

	switch (type_emul)
	{
	case	Emulator::TypeEmul_AUTO:
		_type = TypeHrdw_UNKNOWN;
		break;
	case	Emulator::TypeEmul_ZX:
		set_type (TypeHrdw_ZX, _clk_z80_zx, _clk_ay3_zx);
		break;
	case	Emulator::TypeEmul_CPC:
		set_type (TypeHrdw_CPC, _clk_z80_cpc, _clk_ay3_cpc);
		break;
	case	Emulator::TypeEmul_ZCPC:
		set_type (TypeHrdw_ZX, _clk_z80_cpc, _clk_ay3_cpc * 2);
		break;
	default:
		assert (false);
		break;
	}

	// Machine detection should be done within about 1 second, otherwise
	// something must be wrong.
	const long		max_delay = 4000000L;	// Z80 clock cycles

	while (_type == TypeHrdw_UNKNOWN && _clk_cnt_z80 < max_delay)
	{
		execute_z80_instruction (_nxt_int_clk_z80);
	}

	assert (is_playing () || _type == TypeHrdw_UNKNOWN);

	return ((_type != TypeHrdw_UNKNOWN) ? 0 : -1);
}



/*
==============================================================================
Name: get_type
Description:
	Returns the hardware type, if known.
Returns: The hardware type.
Throws: Nothing
==============================================================================
*/

Emulator::TypeHrdw	Emulator::get_type () const
{
	return (_type);
}



/*
==============================================================================
Name: set_channel_activity
Description:
	Sets the AY-3-8910 channel activity. It can be changed at any time.
Input parameters:
	- mask: A bit mask for the channels. A bit set to 1 means the channel is
		active, to 0 means it is silent. A = bit 0, B = bit 1 and C = bit 2.
Throws: Nothing
==============================================================================
*/

void	Emulator::set_channel_activity (int mask)
{
	_chn_activity = mask;
}



/*
==============================================================================
Name: generate_audio
Description:
	Generates audio data. Call this function as many time you want, but after
	a start().
Input parameters:
	- nbr_spl: Number of samples to generate.
Output parameters:
	- data_ptr_arr: Array of pointers on the zone where samples should be
		generated. The number of channels depends on the conv parameter passed
		previously to the start() function.
Throws: ?
==============================================================================
*/

void	Emulator::generate_audio (float **data_ptr_arr, long nbr_spl)
{
	assert (is_playing ());
	assert (data_ptr_arr != 0);
	assert (data_ptr_arr [0] != 0);
	assert (nbr_spl > 0);

	long				work_pos = 0;
	do
	{
		// Converts AY3 signals into audio signals
		const long		max_len = nbr_spl - work_pos;
		const long		work_len =
			conv_ay3_signal_to_audio (data_ptr_arr, work_pos, max_len);
		work_pos += work_len;

		// Generates new AY3 signals if there is room enough in the AY3 FIFO
		const Ay3OutFifo &	ay3_fifo = _ay3_out_fifo_arr [0];
		if (ay3_fifo.get_room () > ay3_fifo.get_nbr_elt ())
		{
			generate_ay3_signal ();
		}

		if (ay3_fifo.get_room () > ay3_fifo.get_nbr_elt ())
		{
			const double	refresh_per_s = _audio_data_refresh_per * 0.5;
			const double	refresh_per_clk = refresh_per_s * _clock_rate_z80;
			long				stop_clk =
				_clk_cnt_z80 + static_cast <long> (refresh_per_clk);
			do
			{
				execute_z80_instruction (stop_clk);
			}
			while (_clk_cnt_z80 < stop_clk);
		}

		check_and_fix_counters ();
	}
	while (work_pos < nbr_spl);
}



/*
==============================================================================
Name: get_nbr_audio_chn_for
Description:
	Gives the number of output audio channels required for a given AY-3-8910
	ouput mixing/conversion.
Input parameters:
	- conv: The conversion
Returns: Number of channels required, > 0.
Throws: Nothing
==============================================================================
*/

int	Emulator::get_nbr_audio_chn_for (MixConv conv)
{
	assert (conv >= 0);
	assert (conv < MixConv_NBR_ELT);

	int				nbr_chn = 0;

	switch (conv)
	{
	case	MixConv_MONO:
		nbr_chn = 1;
		break;
	case	MixConv_STEREO:
		nbr_chn = 2;
		break;
	case	MixConv_SEPARATED:
		nbr_chn = 3;
		break;
	default:
		assert (false);
		break;
	}

	return (nbr_chn);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



UByte	Emulator::do_read_z80_mem (UInt16 addr)
{
	if (   _debug_info.is_debug_mode_active ()
	    && ! _debug_info.is_addr_init (addr))
	{
		const UInt16	pc = _z80.use_reg_16 (Z80Wrapper::Reg16_PC);
		_debug_info.set_addr_as_read_wo_init (addr, pc);
	}

	return (_mem [addr]);
}



void	Emulator::do_write_z80_mem (UInt16 addr, UByte data)
{
	_mem [addr] = data;

	if (   _debug_info.is_debug_mode_active ()
	    && ! _debug_info.is_addr_read_wo_init (addr))
	{
		_debug_info.set_addr_as_init (addr);
	}
}



UByte	Emulator::do_read_z80_port (UInt16 port)
{
	UByte				data = 0xFF;

	bool				expected_flag = false;

	if (   _type == TypeHrdw_ZX
	    || _type == TypeHrdw_UNKNOWN)
	{
		if ((port & Emulator_zx_port_mask) == Emulator_zx_port_reg)
		{
			data = _ay3.read_data ();
			expected_flag = _debug_info.is_port_reading_allowed ();
		}
	}

	else	// TypeHrdw_CPC
	{
		const int		high_byte = port >> 8;
		if (high_byte == 0xF4)
		{
			data = _i8255.read_port_a ();
			expected_flag = _debug_info.is_port_reading_allowed ();
		}
	}

	if (! expected_flag && _debug_info.is_debug_mode_active ())
	{
		const int		high_byte = port >> 8;
		const UInt16	pc = _z80.use_reg_16 (Z80Wrapper::Reg16_PC);
		_debug_info.set_port_as_read (high_byte, pc);
	}

	return (data);
}



void	Emulator::do_write_z80_port (UInt16 port, UByte data)
{
	if (_type == TypeHrdw_UNKNOWN)
	{
		detect_type (port);
	}

	if (_type == TypeHrdw_CPC)
	{
		write_port_cpc (port, data);
	}
	else if (_type == TypeHrdw_ZX)
	{
		write_port_zx (port, data);
	}
}



void	Emulator::do_signal_ay3_state_about_to_change ()
{
	try
	{
		if (is_playing ())
		{
			generate_ay3_signal ();
		}
	}
	catch (...)
	{
		assert (false);
	}
}



void	Emulator::do_write_ay3_port_a (UByte data)
{
	// Nothing
}



UByte	Emulator::do_read_ay3_port_a ()
{
	return (0);
}



void	Emulator::do_write_ay3_port_b (UByte data)
{
	// Nothing
}



UByte	Emulator::do_read_ay3_port_b ()
{
	return (0);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Emulator::prepare (UInt16 init, UInt16 interrupt, UInt16 sp, UInt16 regfill)
{
	assert (init != 0);

	// If INTERRUPT equal to ZERO then place at ZERO address next player:
	if (interrupt == 0)
	{
		static const UByte	code [] =
		{
			0xF3,					// DI
			0xCD,	0x00, 0x00,	// CALL INIT
			0xED, 0x5E,			// LOOP: IM 2
			0xFB,					// EI
			0x76,					// HALT
			0x18, 0xFA			// JR LOOP
		};
		const long		len = sizeof (code) / sizeof (code [0]);
		write_block_to_mem (code, 0, len);
	}

	// If INTERRUPT not equal to ZERO then place at ZERO address next player:
	else
	{
		static const UByte	code [] =
		{
			0xF3,					// DI
			0xCD,	0x00, 0x00,	// CALL INIT
			0xED, 0x56,			// LOOP: IM 1
			0xFB,					// EI
			0x76,					// HALT
			0xCD, 0x00, 0x00,	// CALL INTERRUPT
			0x18, 0xF7			// JR LOOP
		};
		const long		len = sizeof (code) / sizeof (code [0]);
		write_block_to_mem (code, 0, len);
		write_word_to_mem (9, interrupt);
	}

	write_word_to_mem (2, init);

	// Load all common lower registers with LoReg value (including AF register)
	// Load all common higher registers with HiReg value
	_z80.use_reg_16 (Z80Wrapper::Reg16_AF) = regfill;
	_z80.use_reg_16 (Z80Wrapper::Reg16_BC) = regfill;
	_z80.use_reg_16 (Z80Wrapper::Reg16_DE) = regfill;
	_z80.use_reg_16 (Z80Wrapper::Reg16_HL) = regfill;
	_z80.use_reg_16 (Z80Wrapper::Reg16_IX) = regfill;
	_z80.use_reg_16 (Z80Wrapper::Reg16_IY) = regfill;
	_z80.use_reg_16 (Z80Wrapper::Reg16_AF1) = regfill;
	_z80.use_reg_16 (Z80Wrapper::Reg16_BC1) = regfill;
	_z80.use_reg_16 (Z80Wrapper::Reg16_DE1) = regfill;
	_z80.use_reg_16 (Z80Wrapper::Reg16_HL1) = regfill;

	// Load into I register 3 (this player version)
	_z80.use_reg_8 (Z80Wrapper::Reg8_I) = 3;

	// Load to SP stack value from points data of this song
	_z80.use_reg_16 (Z80Wrapper::Reg16_SP) = sp;

	// Load to PC ZERO value
	_z80.use_reg_16 (Z80Wrapper::Reg16_PC) = 0;

	// Disable Z80 interrupts and set IM0 mode
	_z80.use_reg_8 (Z80Wrapper::Reg8_IFF) = 0;
}



void	Emulator::execute_z80_instruction (long stop_clk)
{
	assert (_clock_rate_z80 > 0);

	if (stop_clk > _nxt_int_clk_z80)
	{
		stop_clk = _nxt_int_clk_z80;
	}

	if (_clk_cnt_z80 >= _nxt_int_clk_z80)
	{
		_nxt_int_clk_z80 += _interrupt_per;

		_z80.interrupt (0x38);
	}

	const long		max_nbr_cycles = stop_clk - _clk_cnt_z80;
	const long		nbr_cycles = _z80.exec_single (max_nbr_cycles);
	_clk_cnt_z80 += nbr_cycles;
}



void	Emulator::detect_type (UInt16 port)
{
	const int		high_byte = port >> 8;
	const UInt16	masked_port = port & Emulator_zx_port_mask;

	if (high_byte == 0xF4 || high_byte == 0xF6)
	{
		set_type (TypeHrdw_CPC, _clk_z80_cpc, _clk_ay3_cpc);
	}

	else if (   masked_port == Emulator_zx_port_reg
	         || masked_port == Emulator_zx_port_data)
	{
		set_type (TypeHrdw_ZX, _clk_z80_zx, _clk_ay3_zx);
	}
}



void	Emulator::set_type (TypeHrdw type, double clock_rate_z80, double clock_rate_ay3)
{
	assert (type == TypeHrdw_CPC || type == TypeHrdw_ZX);
	assert (clock_rate_z80 > 0);
	assert (clock_rate_ay3 > 0);

	_type = type;
	_clock_rate_z80 = clock_rate_z80;
	_clock_rate_ay3 = clock_rate_ay3;

	start_playback ();
}



void	Emulator::write_port_cpc (UInt16 port, UByte data)
{
	const int		high_byte = port >> 8;
	if (high_byte == 0xF4)
	{
		_i8255.write_port_a (data);
	}
	else if (high_byte == 0xF6)
	{
		_i8255.write_port_c (data);
	}
	else if (_debug_info.is_debug_mode_active ())
	{
		const int		high_byte = port >> 8;
		const UInt16	pc = _z80.use_reg_16 (Z80Wrapper::Reg16_PC);
		_debug_info.set_port_as_written (high_byte, pc);
	}
}



void	Emulator::write_port_zx (UInt16 port, UByte data)
{
	const UInt16	masked_port = port & Emulator_zx_port_mask;
	if (masked_port == Emulator_zx_port_reg)
	{
		const int		reg_index = data & 15;
		const Ay38910::Reg	reg = static_cast <Ay38910::Reg> (reg_index);
		_ay3.latch_address (reg);
	}
	else if (masked_port == Emulator_zx_port_data)
	{
		_ay3.write_data (data);
	}
	else if (_debug_info.is_debug_mode_active ())
	{
		const int		high_byte = port >> 8;
		const UInt16	pc = _z80.use_reg_16 (Z80Wrapper::Reg16_PC);
		_debug_info.set_port_as_written (high_byte, pc);
	}
}



void	Emulator::start_playback ()
{
	assert (_clock_rate_z80 > 0);
	assert (_clock_rate_ay3 > 0);
	assert (_sample_freq > 0);

	update_interrupt_period ();

	const double	ay3_out_rate = _clock_rate_ay3 / Ay38910::CLK_PER_SPL;
	_rspl_rate = _sample_freq / ay3_out_rate;

	_snd_dmp_clk_z80 = _clk_cnt_z80;
	_res_clk_ay3 = 0;

	const long		security = 1000;

	const long		ay3_fifo_size =
		static_cast <long> (_audio_data_refresh_per * ay3_out_rate) + security;
	for (int ay3_chn = 0; ay3_chn < Ay38910::NBR_CHN; ++ay3_chn)
	{
		_ay3_out_fifo_arr [ay3_chn].set_size (ay3_fifo_size);
	}

	const long		audio_buf_size =
		static_cast <long> (_audio_data_refresh_per * _sample_freq) + security;
	for (int audio_chn = 0; audio_chn < MAX_NBR_AUDIO_CHN; ++audio_chn)
	{
		ResamplerSqr &	rspl = _rspl_arr [audio_chn];
		rspl.set_buf_len (audio_buf_size);
		rspl.set_resampling_rate (_rspl_rate);
	}
}



bool	Emulator::is_playing () const
{
	return (_clock_rate_ay3 > 0);
}



void	Emulator::generate_ay3_signal ()
{
	assert (is_playing ());

	using namespace std;

	// Computes the available signal length
	const long		nbr_z80_cycles = _clk_cnt_z80 - _snd_dmp_clk_z80;
	const double	clock_ratio = _clock_rate_ay3 / _clock_rate_z80;
	const double	nbr_ay3_cycles = nbr_z80_cycles * clock_ratio + _res_clk_ay3;
	const long		len = static_cast <long> (
		floor (nbr_ay3_cycles / Ay38910::CLK_PER_SPL)
	);

	// Generates signal and stores it
	long				work_pos = 0;
	while (work_pos < len)
	{
		int				ay3_chn;

		Int16 *			data_ptr_arr [Ay38910::NBR_CHN];
		long				work_len = len - work_pos;
		for (ay3_chn = 0; ay3_chn < Ay38910::NBR_CHN; ++ay3_chn)
		{
			long				max_len;
			_ay3_out_fifo_arr [ay3_chn].get_writing_info (
				max_len,
				data_ptr_arr [ay3_chn]
			);
			assert (max_len > 0);
			if (work_len > max_len)
			{
				work_len = max_len;
			}
		}

		_ay3.process_block (data_ptr_arr, work_len);

		for (ay3_chn = 0; ay3_chn < Ay38910::NBR_CHN; ++ay3_chn)
		{
			_ay3_out_fifo_arr [ay3_chn].notify_writing (work_len);
		}
		work_pos += work_len;
	}

	// Computes remaining AY-3-8910 cycles
	const double	ay3_cycles_consumed =
		static_cast <double> (len) * Ay38910::CLK_PER_SPL;
	_res_clk_ay3 = nbr_ay3_cycles - ay3_cycles_consumed;
	_snd_dmp_clk_z80 = _clk_cnt_z80;
}



long	Emulator::conv_ay3_signal_to_audio (float **data_ptr_arr, long work_pos, long max_len)
{
	assert (data_ptr_arr != 0);
	assert (data_ptr_arr [0] != 0);
	assert (work_pos >= 0);
	assert (max_len > 0);

	const long	max_ay3_block_len = 1024;
	Int16			ay3_buf_arr [Ay38910::NBR_CHN] [max_ay3_block_len];

	const int	vol_a = (_chn_activity >> 0) & 1;
	const int	vol_b = (_chn_activity >> 1) & 1;
	const int	vol_c = (_chn_activity >> 2) & 1;

	long			done_len = 0;
	bool			cont_flag = true;
	while (done_len < max_len && cont_flag)
	{
		int				ay3_chn;

		// Get available AY3 data
		long				avl_ay3;
		const Int16 *	ay3_data_ptr_arr [Ay38910::NBR_CHN];
		for (ay3_chn = 0; ay3_chn < Ay38910::NBR_CHN; ++ay3_chn)
		{
			Ay3OutFifo &	ay3_fifo = _ay3_out_fifo_arr [ay3_chn];
			ay3_fifo.get_reading_info (avl_ay3, ay3_data_ptr_arr [ay3_chn]);
		}
		if (avl_ay3 > max_ay3_block_len)
		{
			avl_ay3 = max_ay3_block_len;
		}

		if (avl_ay3 <= 0)
		{
			cont_flag = false;	// We need more AY3 data
		}

		else
		{
			// Converts if required. ay3_data_ptr_arr will store input pointers
			// for correspounding audio channels
			const int		nbr_chn = get_nbr_audio_chn_for (_conv);
			long				pos;
			switch (_conv)
			{
			case	MixConv_MONO:
				for (pos = 0; pos < avl_ay3; ++pos)
				{
					const long		val =   ay3_data_ptr_arr [0] [pos] * vol_a
										      + ay3_data_ptr_arr [1] [pos] * vol_b
										      + ay3_data_ptr_arr [2] [pos] * vol_c;
					ay3_buf_arr [0] [pos] = static_cast <Int16> (val >> 1);	// To avoid overflows
				}
				ay3_data_ptr_arr [0] = ay3_buf_arr [0];
				break;

			case	MixConv_STEREO:
				for (pos = 0; pos < avl_ay3; ++pos)
				{
					const int		mid = (ay3_data_ptr_arr [1] [pos] >> 1) * vol_b;
					ay3_buf_arr [0] [pos] = ay3_data_ptr_arr [0] [pos] * vol_a + mid;
					ay3_buf_arr [1] [pos] = ay3_data_ptr_arr [2] [pos] * vol_c + mid;
				}
				ay3_data_ptr_arr [0] = ay3_buf_arr [0];
				ay3_data_ptr_arr [1] = ay3_buf_arr [1];
				break;

			case	MixConv_SEPARATED:
				// Ignores the channel activity.
				break;

			default:
				assert (false);
				break;
			}

			long				nbr_out_spl = max_len - done_len;
			for (int audio_chn = 0; audio_chn < nbr_chn; ++audio_chn)
			{
				ResamplerSqr &	rspl = _rspl_arr [audio_chn];

				// Feeds the sampling rate converter
				const long		nbr_ay3_spl_done = rspl.process_input_samples (
					ay3_data_ptr_arr [audio_chn],
					avl_ay3
				);
				if (nbr_ay3_spl_done < avl_ay3)
				{
					assert (audio_chn == 0);	// Converters should be internally synchronised
					avl_ay3 = nbr_ay3_spl_done;
				}

				// Outputs audio samples
				long			avl_out = rspl.get_available_output_samples ();
				if (avl_out < nbr_out_spl)
				{
					assert (audio_chn == 0);	// Converters should be internally synchronised
					nbr_out_spl = avl_out;
				}
				if (nbr_out_spl > 0)
				{
					float *			data_ptr =
						&data_ptr_arr [audio_chn] [work_pos + done_len];
					rspl.pop_output_samples (data_ptr, nbr_out_spl);
				}
			}

			// Validates processed AY3 samples
			for (ay3_chn = 0; ay3_chn < Ay38910::NBR_CHN; ++ay3_chn)
			{
				Ay3OutFifo &	ay3_fifo = _ay3_out_fifo_arr [ay3_chn];
				ay3_fifo.notify_reading (avl_ay3);
			}

			// Validates output samples
			done_len += nbr_out_spl;
		}
	}

	return (done_len);
}



void	Emulator::update_interrupt_period ()
{
	assert (_clock_rate_z80 > 0);
	assert (_interrupt_freq > 0);

	_interrupt_per = round_long (_clock_rate_z80 / _interrupt_freq);

	if (_nxt_int_clk_z80 - _clk_cnt_z80 > _interrupt_per)
	{
		_nxt_int_clk_z80 = _clk_cnt_z80 + _interrupt_per;
	}
}



// Avoids overflow on the Z80 clock cycle counter
void	Emulator::check_and_fix_counters ()
{
	const long		limit = LONG_MAX / 2;

	if (_clk_cnt_z80 > limit)
	{
		_clk_cnt_z80 -= limit;
		_snd_dmp_clk_z80 -= limit;
		_nxt_int_clk_z80 -= limit;
	}
}



const double	Emulator::_audio_data_refresh_per = 0.025;
const double	Emulator::_clk_z80_zx = 3494400;
const double	Emulator::_clk_z80_cpc = 4000000;
const double	Emulator::_clk_ay3_zx = 1773400;
const double	Emulator::_clk_ay3_cpc = 1000000;



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
