/*****************************************************************************

        DcKiller.cpp
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

#include	"DcKiller.h"

#include	<cassert>
#include	<cmath>

namespace std { }



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DcKiller::DcKiller ()
:	_sample_freq (44100.0f)
,	_cutoff_freq (10.0f)
,	_coef_x ()
,	_coef_y ()
,	_mem_x (0)
,	_mem_y (0)
,	_anti_denormal (1e-20f)
{
	update_eq ();
	clear_buffers ();
}



void	DcKiller::set_sample_freq (float sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	update_eq ();
}



void	DcKiller::set_cutoff_freq (float f_0)
{
	assert (f_0 > 0);

	_cutoff_freq = f_0;
	update_eq ();
}



void	DcKiller::clear_buffers ()
{
	_mem_x = 0;
	_mem_y = 0;
}



void	DcKiller::process_block (float data_ptr [], long nbr_spl)
{
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	long				work_pos = 0;
	do
	{
		const long		max_block_len = 64;
		long				block_len = nbr_spl - work_pos;
		if (block_len > max_block_len)
		{
			block_len = max_block_len;
		}

		const long		block_end = work_pos + block_len;
		long				pos = work_pos;
		do
		{
			const float		x = data_ptr [pos];
			const float		y =        x * _coef_x [0]
									 + _mem_x * _coef_x [1]
									 - _mem_y * _coef_y;
			_mem_x = x;
			_mem_y = y;

			data_ptr [pos] = y;

			++ pos;
		}
		while (pos < block_end);
		work_pos = pos;

		// Trick to avoid denormals in filter memory
		_mem_y += _anti_denormal;
		_anti_denormal = -_anti_denormal;
	}
	while (work_pos < nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DcKiller::update_eq ()
{
	assert (_cutoff_freq < _sample_freq * 0.5f);

	using namespace std;

	// Equation in s-plane
	const double	b_0 = 0;
	const double	b_1 = 1;
	const double	a_0 = 1;
	const double	a_1 = 1;

	// s to z bilinear transform
	const double	pi = 3.1415926535897932384626433832795;
	const double	inv_k = tan (_cutoff_freq * pi / _sample_freq);
	assert (inv_k != 0);
	const double	k = 1 / inv_k;

	const double	b1k = b_1 * k;
	const double	b1z = b_0 - b1k; 
	const double	b0z = b_0 + b1k; 

	const double	a1k = a_1 * k;
	const double	a1z = a_0 - a1k;
	const double	a0z = a_0 + a1k;

	// IIR coefficients in z-plane
	assert (a0z != 0);
	const double	mult = 1 / a0z;

	_coef_x [0] = static_cast <float> (b0z * mult);
	_coef_x [1] = static_cast <float> (b1z * mult);
	_coef_y     = static_cast <float> (a1z * mult);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
