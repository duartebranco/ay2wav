/*****************************************************************************

        Intel8255.cpp
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

#include	"Ay38910.h"
#include	"Intel8255.h"

#include	<cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Intel8255::Intel8255 (Ay38910 &ay3)
:	_ay3 (ay3)
,	_cmd (0)
,	_port_a_out (0)
,	_port_a_in (0)
{
	assert (&ay3 != 0);
}



void	Intel8255::reset ()
{
	_cmd = 0;
	_port_a_out = 0;
}



void	Intel8255::write_port_a (UByte data)
{
	_port_a_out = data;
}



UByte	Intel8255::read_port_a () const
{
	return (_port_a_in);
}



void	Intel8255::write_port_c (UByte data)
{
	data &= 0xC0;

	if (_cmd == 0x80 && data == 0)
	{
		_ay3.write_data (_port_a_out);
	}
	else if (_cmd == 0xC0 && data == 0)
	{
		const int		reg_index = _port_a_out & 15;
		const Ay38910::Reg	reg = static_cast <Ay38910::Reg> (reg_index);
		_ay3.latch_address (reg);
	}
	else if (data == 0x40)
	{
		_port_a_in = _ay3.read_data ();
	}

	_cmd = data;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
