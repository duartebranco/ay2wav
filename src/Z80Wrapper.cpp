/*****************************************************************************

        Z80Wrapper.cpp
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

#include	"Z80CbInterface.h"
#include	"Z80Wrapper.h"

#include	<cassert>



// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Callbacks from wrapped Z80 emulator



void WrZ80(word addr, byte value)
{
	assert (Z80Wrapper::_cur_cb_ptr != 0);

	Z80Wrapper::_cur_cb_ptr->write_z80_mem (addr, value);
}



byte RdZ80(word addr)
{
	assert (Z80Wrapper::_cur_cb_ptr != 0);

	return (Z80Wrapper::_cur_cb_ptr->read_z80_mem (addr));
}



void OutZ80(word port, byte value)
{
	assert (Z80Wrapper::_cur_cb_ptr != 0);

	Z80Wrapper::_cur_cb_ptr->write_z80_port (port, value);
}



byte InZ80(word port)
{
	assert (Z80Wrapper::_cur_cb_ptr != 0);

	return (Z80Wrapper::_cur_cb_ptr->read_z80_port (port));
}



void PatchZ80(Z80 *R)
{
	// Nothing
}



word LoopZ80(Z80 *R)
{
	return (INT_QUIT);
}



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Z80Wrapper::Z80Wrapper ()
:	_cb_ptr (0)
,	_z80 ()
{
	_z80.R = 0x00;
	_z80.IPeriod = CYCLE_MAX;
	_z80.IAutoReset = 1;
	_z80.TrapBadOps = 1;
	_z80.Trap = 0x0000;
	_z80.Trace = 0;
	_z80.User = reinterpret_cast <void *> (this);

	reset ();
}



Z80Wrapper::~Z80Wrapper ()
{
	// Nothing
}



void	Z80Wrapper::set_callback (Z80CbInterface &cb)
{
	assert (&cb != 0);

	_cb_ptr = &cb;
}



void	Z80Wrapper::reset ()
{
	_cur_cb_ptr = _cb_ptr;

	ResetZ80 (&_z80);

	_cur_cb_ptr = 0;
}



// clk_before_next_interrupt helps to optimise HALT instruction execution.
// Set clk_before_next_interrupt to 0 if you don't know when next interrupt
// will happen.
long	Z80Wrapper::exec_single (long clk_before_next_interrupt)
{
	_cur_cb_ptr = _cb_ptr;

	long				nbr_cycles = 1 - ExecZ80 (&_z80, 1);
	assert (nbr_cycles > 0);

	if ((_z80.IFF & IFF_HALT) != 0 && clk_before_next_interrupt > 0)
	{
		const int		nbr_instr = (clk_before_next_interrupt - 1) / nbr_cycles;
		nbr_cycles *= 1 + nbr_instr;
	}

	_cur_cb_ptr = 0;

	return (nbr_cycles);
}



void	Z80Wrapper::interrupt (UByte vec)
{
	_cur_cb_ptr = _cb_ptr;

	IntZ80 (&_z80, vec);

	_cur_cb_ptr = 0;
}



void	Z80Wrapper::interrupt_non_maskable ()
{
	_cur_cb_ptr = _cb_ptr;

	IntZ80 (&_z80, INT_NMI);

	_cur_cb_ptr = 0;
}



UByte &	Z80Wrapper::use_reg_8 (Reg8 reg)
{
	assert (reg >= 0);
	assert (reg < Reg8_NBR_ELT);

	UByte *			r_ptr = 0;

	switch (reg)
	{
	case	Reg8_A:	r_ptr = &_z80.AF.B.h;	break;
	case	Reg8_F:	r_ptr = &_z80.AF.B.l;	break;
	case	Reg8_B:	r_ptr = &_z80.BC.B.h;	break;
	case	Reg8_C:	r_ptr = &_z80.BC.B.l;	break;
	case	Reg8_D:	r_ptr = &_z80.DE.B.h;	break;
	case	Reg8_E:	r_ptr = &_z80.DE.B.l;	break;
	case	Reg8_H:	r_ptr = &_z80.HL.B.h;	break;
	case	Reg8_L:	r_ptr = &_z80.HL.B.l;	break;
	case	Reg8_A1:	r_ptr = &_z80.AF1.B.h;	break;
	case	Reg8_F1:	r_ptr = &_z80.AF1.B.l;	break;
	case	Reg8_B1:	r_ptr = &_z80.BC1.B.h;	break;
	case	Reg8_C1:	r_ptr = &_z80.BC1.B.l;	break;
	case	Reg8_D1:	r_ptr = &_z80.DE1.B.h;	break;
	case	Reg8_E1:	r_ptr = &_z80.DE1.B.l;	break;
	case	Reg8_H1:	r_ptr = &_z80.HL1.B.h;	break;
	case	Reg8_L1:	r_ptr = &_z80.HL1.B.l;	break;
	case	Reg8_IFF:	r_ptr = &_z80.IFF;	break;
	case	Reg8_I:	r_ptr = &_z80.I;	break;
	case	Reg8_R:	r_ptr = &_z80.R;	break;
	default:
		assert (false);
		break;
	}
	assert (r_ptr != 0);

	return (*r_ptr);
}



UInt16 &	Z80Wrapper::use_reg_16 (Reg16 reg)
{
	assert (reg >= 0);
	assert (reg < Reg16_NBR_ELT);

	UInt16 *			r_ptr = 0;

	switch (reg)
	{
	case	Reg16_AF:	r_ptr = &_z80.AF.W;	break;
	case	Reg16_BC:	r_ptr = &_z80.BC.W;	break;
	case	Reg16_DE:	r_ptr = &_z80.DE.W;	break;
	case	Reg16_HL:	r_ptr = &_z80.HL.W;	break;
	case	Reg16_IX:	r_ptr = &_z80.IX.W;	break;
	case	Reg16_IY:	r_ptr = &_z80.IY.W;	break;
	case	Reg16_PC:	r_ptr = &_z80.PC.W;	break;
	case	Reg16_SP:	r_ptr = &_z80.SP.W;	break;
	case	Reg16_AF1:	r_ptr = &_z80.AF1.W;	break;
	case	Reg16_BC1:	r_ptr = &_z80.BC1.W;	break;
	case	Reg16_DE1:	r_ptr = &_z80.DE1.W;	break;
	case	Reg16_HL1:	r_ptr = &_z80.HL1.W;	break;
	default:
		assert (false);
		break;
	}
	assert (r_ptr != 0);

	return (*r_ptr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Z80CbInterface *	Z80Wrapper::_cur_cb_ptr = 0;



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
