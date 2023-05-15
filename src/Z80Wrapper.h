/*****************************************************************************

        Z80Wrapper.h
        Copyright (c) 2006 Laurent de Soras

Because of the memory/port access implementation, this wrapper cannot be
used concurrently (multiple wrappers running simultaneously in different
threads).

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



#if ! defined (Z80Wrapper_HEADER_INCLUDED)
#define	Z80Wrapper_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"z80/z80.h"



class Z80CbInterface;

class Z80Wrapper
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Reg8
	{
		Reg8_A = 0,
		Reg8_F,
		Reg8_B,
		Reg8_C,
		Reg8_D,
		Reg8_E,
		Reg8_H,
		Reg8_L,
		Reg8_A1,
		Reg8_F1,
		Reg8_B1,
		Reg8_C1,
		Reg8_D1,
		Reg8_E1,
		Reg8_H1,
		Reg8_L1,
		Reg8_IFF,
		Reg8_I,
		Reg8_R,

		Reg8_NBR_ELT
	};

	enum Reg16
	{
		Reg16_AF = 0,
		Reg16_BC,
		Reg16_DE,
		Reg16_HL,
		Reg16_IX,
		Reg16_IY,
		Reg16_PC,
		Reg16_SP,
		Reg16_AF1,
		Reg16_BC1,
		Reg16_DE1,
		Reg16_HL1,

		Reg16_NBR_ELT
	};

						Z80Wrapper ();
	virtual			~Z80Wrapper ();

	void				set_callback (Z80CbInterface &cb);
	void				reset ();
	long				exec_single (long clk_before_next_interrupt);
	void				interrupt (UByte vec);
	void				interrupt_non_maskable ();
	UByte &			use_reg_8 (Reg8 reg);
	UInt16 &			use_reg_16 (Reg16 reg);

	// For internal use only
	static Z80CbInterface *
						_cur_cb_ptr;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum {			CYCLE_MAX	= 1000	};

	Z80CbInterface *
						_cb_ptr;		// 0 = not initialised
	Z80				_z80;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						Z80Wrapper (const Z80Wrapper &other);
	Z80Wrapper &	operator = (const Z80Wrapper &other);
	bool				operator == (const Z80Wrapper &other);
	bool				operator != (const Z80Wrapper &other);

};	// class Z80Wrapper



//#include	"Z80Wrapper.hpp"



#endif	// Z80Wrapper_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
