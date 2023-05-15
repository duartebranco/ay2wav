/*****************************************************************************

        Ay38910CbInterface.cpp
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

#include	"Ay38910CbInterface.h"

#include	<cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: signal_ay3_state_about_to_change
Description:
	Indicates that the AY-3-8910 registers are about to be modified. This is
	a good place to render remaining audio data, from the latest rendering
	moment up to "now".
Throws: Nothing
==============================================================================
*/

void	Ay38910CbInterface::signal_ay3_state_about_to_change ()
{
	do_signal_ay3_state_about_to_change ();
}



/*
==============================================================================
Name: write_ay3_port_a
Description:
	Handles Port A data writing.
Input parameters:
	- data: The byte to write on port A.
Throws: Nothing
==============================================================================
*/

void	Ay38910CbInterface::write_ay3_port_a (UByte data)
{
	do_write_ay3_port_a (data);
}



/*
==============================================================================
Name: read_ay3_port_a
Description:
	Handles Port A data reading.
Returns: The byte which will be read by the AY-3-8910
Throws: Nothing
==============================================================================
*/

UByte	Ay38910CbInterface::read_ay3_port_a ()
{
	return (do_read_ay3_port_a ());
}



/*
==============================================================================
Name: write_ay3_port_b
Description:
	Handles Port B data writing.
Input parameters:
	- data: The byte to write on port B.
Throws: Nothing
==============================================================================
*/

void	Ay38910CbInterface::write_ay3_port_b (UByte data)
{
	do_write_ay3_port_b (data);
}



/*
==============================================================================
Name: read_ay3_port_b
Description:
	Handles Port B data reading.
Returns: The byte which will be read by the AY-3-8910
Throws: Nothing
==============================================================================
*/

UByte	Ay38910CbInterface::read_ay3_port_b ()
{
	return (do_read_ay3_port_b ());
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
