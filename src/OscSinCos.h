/*****************************************************************************

        OscSinCos.h
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



#if ! defined (OscSinCos_HEADER_INCLUDED)
#define	OscSinCos_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"def.h"



template <class T>
class OscSinCos
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	T	DataType;

						OscSinCos ();

	FORCEINLINE void
						set_step (double angle_rad);

	FORCEINLINE DataType
						get_cos () const;
	FORCEINLINE DataType
						get_sin () const;
	FORCEINLINE void
						step ();
	FORCEINLINE void
						clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	DataType			_pos_cos;		// Current phase expressed with sin and cos. [-1 ; 1]
	DataType			_pos_sin;		// -
	DataType			_step_cos;		// Phase increment per step, [-1 ; 1]
	DataType			_step_sin;		// -



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						OscSinCos (const OscSinCos &other);
	OscSinCos &		operator = (const OscSinCos &other);
	bool				operator == (const OscSinCos &other);
	bool				operator != (const OscSinCos &other);

};	// class OscSinCos



#include	"OscSinCos.hpp"



#endif	// OscSinCos_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
