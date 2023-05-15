/*****************************************************************************

        Ay38910.h
        Copyright (c) 2006 Laurent de Soras

Some emulation tricks taken from the MAME project - http://www.mame.net.

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



#if ! defined (Ay38910_HEADER_INCLUDED)
#define	Ay38910_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"def.h"



class Ay38910CbInterface;

class Ay38910
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum {			NBR_CHN		= 3	};
	enum {			CLK_PER_SPL	= 8	};	// 8 clocks per output sample.
	enum {			MAX_OUTPUT	= 0x4000	};	// Must be stored as an Int16

	enum Reg
	{
		Reg_A_PER_LSB = 0,
		Reg_A_PER_MSB,
		Reg_B_PER_LSB,
		Reg_B_PER_MSB,
		Reg_C_PER_LSB,
		Reg_C_PER_MSB,
		Reg_N_PER,
		Reg_ENABLE,
		Reg_A_VOL,
		Reg_B_VOL,
		Reg_C_VOL,
		Reg_E_PER_LSB,
		Reg_E_PER_MSB,
		Reg_E_SHAPE,
		Reg_PORT_A,
		Reg_PORT_B,

		Reg_NBR_ELT
	};

						Ay38910 ();
	virtual			~Ay38910 () {}

	void				set_callback (Ay38910CbInterface &cb);

	void				reset ();
	void				latch_address (Reg reg);
	void				write_data (int data);
	int				read_data ();
	void				process_block (Int16 *chn_ptr_arr [NBR_CHN], long nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum {			ENV_RESOL	= 2	};	// To handle double-speed when envelope period is 0.

	class Channel
	{
	public:
							Channel ();
		void				reset ();

		int				_reg_vol;
		int				_enable_mask;
		int				_period;		// Unit: output sample period
		int				_count;		// > 0, wraps when it reaches 0
		int				_step;		// 1 if channel is active, 0 if inactive
		int				_vol;			// [0 ; MAX_OUTPUT]
		UByte				_output;
		bool				_env_flag;
	};

	inline void		write_reg (int reg, int val);

	inline void		update_enable ();
	inline void		update_noise_period ();
	inline void		update_env_period ();
	inline void		update_env_shape ();
	inline void		update_chn_period (Channel &chn, Reg reg_fine, Reg reg_coarse);
	inline void		update_chn_vol (Channel &chn);
	inline void		update_chn_activity (Channel &chn);

	inline int		process_chn (Channel &chn, int noise_mask);
	inline void		process_noise ();
	inline void		process_envelope ();
	inline void		set_env_volume ();

	static void		build_vol_table ();

	Ay38910CbInterface *
						_cb_ptr;
	Reg				_register_latch;
	UByte				_reg_arr [16];
	int				_last_enable;	// Previous value of Mixer Enable

	Channel			_chn_arr [NBR_CHN];

	long				_rnd_state;
	int				_noise_period;	// Unit: output sample period
	int				_noise_count;	// > 0, wraps when it reaches 0
	int				_noise_step;	// 1 if noise is active, 0 if inactive
	UByte				_noise_state;	// 0x00 or 0xFF
	UByte				_noise_out;		// State filtered with the mixer enable mask

	int				_env_period;	// Unit: output sample period / ENV_RESOL
	int				_env_count;		// > 0, wraps when it reaches 0
	int				_env_vol;
	bool				_env_hold_flag;
	bool				_env_alternate_flag;
	bool				_env_continue_flag;
	int				_env_attack;
	int				_env_val;		// [0 ; 31]

	static int		_vol_table [32];
	static bool		_vol_table_init_flag;
	static const UByte
						_reg_mask_arr [16];



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						Ay38910 (const Ay38910 &other);
	Ay38910 &		operator = (const Ay38910 &other);
	bool				operator == (const Ay38910 &other);
	bool				operator != (const Ay38910 &other);

};	// class Ay38910



//#include	"Ay38910.hpp"



#endif	// Ay38910_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
