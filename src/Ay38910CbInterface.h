/*****************************************************************************

        Ay38910CbInterface.h
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



#if ! defined (Ay38910CbInterface_HEADER_INCLUDED)
#define	Ay38910CbInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"def.h"



class Ay38910CbInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual			~Ay38910CbInterface () {}

	void				signal_ay3_state_about_to_change ();

	void				write_ay3_port_a (UByte data);
	UByte				read_ay3_port_a ();

	void				write_ay3_port_b (UByte data);
	UByte				read_ay3_port_b ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void	do_signal_ay3_state_about_to_change () = 0;

	virtual void	do_write_ay3_port_a (UByte data) = 0;
	virtual UByte	do_read_ay3_port_a () = 0;

	virtual void	do_write_ay3_port_b (UByte data) = 0;
	virtual UByte	do_read_ay3_port_b () = 0;



};	// class Ay38910CbInterface



//#include	"Ay38910CbInterface.hpp"



#endif	// Ay38910CbInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
