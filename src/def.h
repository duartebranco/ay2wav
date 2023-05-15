/*****************************************************************************

        def.h
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



#if ! defined (def_HEADER_INCLUDED)
#define	def_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

// Defines LSB_FIRST or MSB_FIRST. Please adapt Z80.h depending on your target
// architecture.
#include	"z80/Z80.h"

const double	PI		= 3.1415926535897932384626433832795;
const double	SQRT2	= 1.41421356237309514547462185873883;

#if defined (_MSC_VER)

	#define	FORCEINLINE	__forceinline

#else

	#define	FORCEINLINE	inline

#endif

typedef	signed char		SByte;
typedef	unsigned char	UByte;
typedef	signed short	Int16;
typedef	unsigned short	UInt16;
typedef	signed int		Int32;
typedef	unsigned int	UInt32;



#endif	// def_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
