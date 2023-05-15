/*****************************************************************************

        Emulator.h
        Copyright (c) 2006 Laurent de Soras

This is the main object of the emulation. It could be reused for any other
form of emulation (sound file player, etc).

CPC: F_z80 = 4000000 Hz, F_ay3 = 1000000 Hz
ZX : F_z80 = 3494400 Hz, F_ay3 = 1773400 Hz

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



#if ! defined (Emulator_HEADER_INCLUDED)
#define	Emulator_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"Ay38910.h"
#include	"Ay38910CbInterface.h"
#include	"DebugInfo.h"
#include	"def.h"
#include	"Intel8255.h"
#include	"MixConv.h"
#include	"ResamplerSqr.h"
#include	"RingFifo.h"
#include	"Z80CbInterface.h"
#include	"Z80Wrapper.h"



class Emulator
:	public Z80CbInterface
,	public Ay38910CbInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum {			MAX_NBR_AUDIO_CHN	= 3	};

	enum TypeEmul
	{
		TypeEmul_AUTO = 0,
		TypeEmul_ZX,
		TypeEmul_CPC,
		TypeEmul_ZCPC,

		TypeEmul_NBR_ELT
	};

	enum TypeHrdw
	{
		TypeHrdw_UNKNOWN = 0,
		TypeHrdw_CPC,
		TypeHrdw_ZX
	};

						Emulator ();
	virtual			~Emulator () {}

	void				reset ();
	void				write_block_to_mem (const UByte data_ptr [], UInt16 addr, long len);
	void				write_word_to_mem (UInt16 addr, UInt16 data);

	void				set_debug_mode (bool flag);
	void				allow_port_reading (bool flag);
	const DebugInfo &
						use_debug_info () const;

	void				set_interrupt_rate (double inter_freq);
	int				start (double sample_freq, MixConv conv, UInt16 init, UInt16 interrupt, UInt16 sp, UInt16 regfill, TypeEmul type_emul);
	TypeHrdw			get_type () const;
	void				set_channel_activity (int mask);
	void				generate_audio (float **data_ptr_arr, long nbr_spl);

	static int		get_nbr_audio_chn_for (MixConv conv);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// Z80CbInterface
	virtual UByte	do_read_z80_mem (UInt16 addr);
	virtual void	do_write_z80_mem (UInt16 addr, UByte data);

	virtual UByte	do_read_z80_port (UInt16 port);
	virtual void	do_write_z80_port (UInt16 port, UByte data);

	// Ay38910CbInterface
	virtual void	do_signal_ay3_state_about_to_change ();

	virtual void	do_write_ay3_port_a (UByte data);
	virtual UByte	do_read_ay3_port_a ();

	virtual void	do_write_ay3_port_b (UByte data);
	virtual UByte	do_read_ay3_port_b ();



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	RingFifo <Int16>	Ay3OutFifo;
	typedef	UByte	MemZone    [65536L];
	typedef	bool	MemFlagArr [65536L];

	void				prepare (UInt16 init, UInt16 interrupt, UInt16 sp, UInt16 regfill);
	void				execute_z80_instruction (long stop_clk);
	void				detect_type (UInt16 port);
	void				set_type (TypeHrdw type, double clock_rate_z80, double clock_rate_ay3);
	void				write_port_cpc (UInt16 port, UByte data);
	void				write_port_zx (UInt16 port, UByte data);
	void				start_playback ();
	bool				is_playing () const;
	void				generate_ay3_signal ();
	long				conv_ay3_signal_to_audio (float **data_ptr_arr, long work_pos, long max_len);
	void				update_interrupt_period ();
	void				check_and_fix_counters ();

	MemZone			_mem;
	Ay38910			_ay3;
	Z80Wrapper		_z80;
	Intel8255		_i8255;				// Used only on CPC machines
	TypeHrdw			_type;				// Machine type (CPC or ZX), TypeHrdw_UNKNOWN before detection
	double			_interrupt_freq;	// Interrupt rate, Hz
	double			_clock_rate_z80;	// Hz, > 0
	double			_clock_rate_ay3;	// Hz, > 0. 0 = not playing yet
	double			_sample_freq;		// Hz, > 0
	double			_rspl_rate;			// Resampling ratio (AY3 output -> final sample), 0 = not playing yet
	long				_clk_cnt_z80;		// Z80 current clock counter, >= 0
	long				_snd_dmp_clk_z80;	// Z80 clock counter value at the latest sound dump, in [0 ; _clk_cnt_z80]
	long				_nxt_int_clk_z80;	// Timestamp of the next interrupt
	double			_res_clk_ay3;		// Residual AY3 clock count, after the last sample of the latest sound dump (may be fractionnal), >= 0
	long				_rem_clk_z80_int;	// Remaining Z80 clock cycles before next interrupt
	long				_interrupt_per;	// Interrupt period, Z80 clock cycles, > 0
	int				_chn_activity;		// Mask for channel acitivity. 0x01 = A, 0x02 = B, 0x04 = C
	MixConv			_conv;
	Ay3OutFifo		_ay3_out_fifo_arr [Ay38910::NBR_CHN];
	ResamplerSqr	_rspl_arr [MAX_NBR_AUDIO_CHN];
	DebugInfo		_debug_info;

	static const double					// Minimum time between two audio block generations. s, > 0
						_audio_data_refresh_per;
	static const double
						_clk_z80_zx;
	static const double
						_clk_z80_cpc;
	static const double
						_clk_ay3_zx;
	static const double
						_clk_ay3_cpc;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						Emulator (const Emulator &other);
	Emulator &		operator = (const Emulator &other);
	bool				operator == (const Emulator &other);
	bool				operator != (const Emulator &other);

};	// class Emulator



//#include	"Emulator.hpp"



#endif	// Emulator_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
