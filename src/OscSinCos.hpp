/*****************************************************************************

        OscSinCos.hpp
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



#if defined (OscSinCos_CURRENT_CODEHEADER)
	#error Recursive inclusion of OscSinCos code header.
#endif
#define	OscSinCos_CURRENT_CODEHEADER

#if ! defined (OscSinCos_CODEHEADER_INCLUDED)
#define	OscSinCos_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cmath>

namespace std { }



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
OscSinCos <T>::OscSinCos ()
:	_pos_cos (1)
,	_pos_sin (0)
,	_step_cos (1)
,	_step_sin (0)
{
	// Nothing
}



template <class T>
void	OscSinCos <T>::set_step (double angle_rad)
{
	using namespace std;

	_step_cos = static_cast <DataType> (cos (angle_rad));
	_step_sin = static_cast <DataType> (sin (angle_rad));
}



template <class T>
typename OscSinCos <T>::DataType	OscSinCos <T>::get_cos () const
{
	return (_pos_cos);
}



template <class T>
typename OscSinCos <T>::DataType	OscSinCos <T>::get_sin () const
{
	return (_pos_sin);
}



template <class T>
void	OscSinCos <T>::step ()
{
	const DataType	old_cos = _pos_cos;
	const DataType	old_sin = _pos_sin;

	_pos_cos = old_cos * _step_cos - old_sin * _step_sin;
	_pos_sin = old_cos * _step_sin + old_sin * _step_cos;
}



template <class T>
void	OscSinCos <T>::clear_buffers ()
{
	_pos_cos = static_cast <DataType> (1);
	_pos_sin = static_cast <DataType> (0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



#endif	// OscSinCos_CODEHEADER_INCLUDED

#undef OscSinCos_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
