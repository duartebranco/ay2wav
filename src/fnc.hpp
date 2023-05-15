/*****************************************************************************

        fnc.hpp
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



#if defined (fnc_CURRENT_CODEHEADER)
	#error Recursive inclusion of fnc code header.
#endif
#define	fnc_CURRENT_CODEHEADER

#if ! defined (fnc_CODEHEADER_INCLUDED)
#define	fnc_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cassert>
#include	<cmath>

namespace std { }



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



long	round_long (double x)
{
	using namespace std;

	return (static_cast <long> (floor (x + 0.5)));
}



inline bool	is_pow2 (long x)
{
	assert (x > 0);

	return  ((x & -x) == x);
}



inline int	get_next_pow2 (long x)
{
	--x;

	int				p = 0;
	while ((x & ~0xFFFFL) != 0)
	{
		p += 16;
		x >>= 16;
	}
	while ((x & ~0xFL) != 0)
	{
		p += 4;
		x >>= 4;
	}
	while (x > 0)
	{
		++p;
		x >>= 1;
	}

	return (p);
}



template <class T>
T	compute_min (const T &a, const T &b)
{
	assert (&a != 0);
	assert (&b != 0);

	return ((a < b) ? a : b);
}



template <class T>
T	compute_max (const T &a, const T &b)
{
	assert (&a != 0);
	assert (&b != 0);

	return ((b < a) ? a : b);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



#endif	// fnc_CODEHEADER_INCLUDED

#undef fnc_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
