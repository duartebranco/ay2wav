/*****************************************************************************

        main.cpp
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

#include	"Ay2Wav.h"

#include	"Tests.h"

#include	<cassert>
#include	<cmath>

namespace std { }



static int	check_endian ();



int main (int argc, char *argv [])
{
	int				ret_val = check_endian ();

#if 0

	Tests::test_ay3 ();

#else

	Ay2Wav			ay32wav;
	ret_val = ay32wav.run (argc, argv);

#endif

	return (ret_val);
}



static int	check_endian ()
{
	const int		x = 1;
	const char		c = *reinterpret_cast <const char *> (&x);
	const bool		lsb_first_flag = (c != 0);
	bool				ok_flag = false;

#if defined (MSB_FIRST) && ! defined (LSB_FIRST)
	ok_flag = ! lsb_first_flag;
#elif defined (LSB_FIRST) && ! defined (MSB_FIRST)
	ok_flag = lsb_first_flag;
#else
	#error Endianess has to be defined.
#endif

	assert (ok_flag);

	return (ok_flag ? 0 : -1);
}
