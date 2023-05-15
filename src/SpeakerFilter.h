/*****************************************************************************

        SpeakerFilter.h
        Copyright (c) 2007 Laurent de Soras

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



#if ! defined (SpeakerFilter_HEADER_INCLUDED)
#define	SpeakerFilter_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"ConvolverFFT.h"



class SpeakerFilter
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						SpeakerFilter ();
	virtual			~SpeakerFilter () {}

	void				set_quality (int quality);

	long				get_block_len () const;
	float *			get_input_buffer () const;
	const float *	get_output_buffer () const;

	void				process (long nbr_spl);

	void				clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	ConvolverFFT	_convolver;

	static const float
						_impulse_128 [128];
	static const float
						_impulse_512 [512];
	static const float
						_impulse_2048 [2048];



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						SpeakerFilter (const SpeakerFilter &other);
	SpeakerFilter&	operator = (const SpeakerFilter &other);
	bool				operator == (const SpeakerFilter &other);
	bool				operator != (const SpeakerFilter &other);

};	// class SpeakerFilter



//#include	"SpeakerFilter.hpp"



#endif	// SpeakerFilter_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
