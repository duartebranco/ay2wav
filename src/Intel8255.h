/*****************************************************************************

        Intel8255.h
        Copyright (c) 2006 Laurent de Soras

Emulation restricted to the communication between Z80 and AY-3-8910 as
implemented in the Amstrad CPC.

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



#if ! defined (Intel8255_HEADER_INCLUDED)
#define	Intel8255_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"def.h"



class Ay38910;

class Intel8255
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit			Intel8255 (Ay38910 &ay3);
	virtual			~Intel8255 () {}

	void				reset ();

	void				write_port_a (UByte data);
	UByte				read_port_a () const;
	void				write_port_c (UByte data);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	Ay38910 &		_ay3;
	UByte				_cmd;
	UByte				_port_a_out;
	UByte				_port_a_in;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						Intel8255 ();
						Intel8255 (const Intel8255 &other);
	Intel8255 &		operator = (const Intel8255 &other);
	bool				operator == (const Intel8255 &other);
	bool				operator != (const Intel8255 &other);

};	// class Intel8255



//#include	"Intel8255.hpp"



#endif	// Intel8255_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
