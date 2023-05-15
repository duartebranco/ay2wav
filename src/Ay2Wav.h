/*****************************************************************************

        Ay2Wav.h
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



#if ! defined (Ay2Wav_HEADER_INCLUDED)
#define	Ay2Wav_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"AyFileReader.h"
#include	"Emulator.h"
#include	"Parameters.h"

#include	<vector>

#include	<cstdio>



class Ay2Wav
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						Ay2Wav ();
	virtual			~Ay2Wav () {}

	int				run (int argc, char *argv []);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static void		dump_uninitialised_read_addresses (const DebugInfo &debug_info);
	static void		dump_written_ports (const DebugInfo &debug_info);
	static void		dump_read_ports (const DebugInfo &debug_info);

	AyFileReader	_file_reader;
	Emulator			_emul;
	Parameters		_param;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						Ay2Wav (const Ay2Wav &other);
	Ay2Wav &			operator = (const Ay2Wav &other);
	bool				operator == (const Ay2Wav &other);
	bool				operator != (const Ay2Wav &other);

};	// class Ay2Wav



//#include	"Ay2Wav.hpp"



#endif	// Ay2Wav_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
