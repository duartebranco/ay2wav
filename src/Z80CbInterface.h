/*****************************************************************************

        Z80CbInterface.h
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



#if ! defined (Z80CbInterface_HEADER_INCLUDED)
#define	Z80CbInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"def.h"



class Z80CbInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual			~Z80CbInterface () {}

	UByte				read_z80_mem (UInt16 addr);
	void				write_z80_mem (UInt16 addr, UByte data);

	UByte				read_z80_port (UInt16 port);
	void				write_z80_port (UInt16 port, UByte data);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual UByte	do_read_z80_mem (UInt16 addr) = 0;
	virtual void	do_write_z80_mem (UInt16 addr, UByte data) = 0;

	virtual UByte	do_read_z80_port (UInt16 port) = 0;
	virtual void	do_write_z80_port (UInt16 port, UByte data) = 0;



};	// class Z80CbInterface



//#include	"Z80CbInterface.hpp"



#endif	// Z80CbInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
