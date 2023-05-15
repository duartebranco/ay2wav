/*****************************************************************************

        ResamplerSqr.h
        Copyright (c) 2006 Laurent de Soras

This resampler is optimised for "square" signals, with a lot of flat steps
(constant values).

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



#if ! defined (ResamplerSqr_HEADER_INCLUDED)
#define	ResamplerSqr_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"def.h"

#include	<vector>



class ResamplerSqr
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						ResamplerSqr ();
	virtual			~ResamplerSqr () {}

	void				set_resampling_rate (double rate);
	void				set_buf_len (long len);
	void				clear_buffers ();

	long				process_input_samples (const Int16 data_ptr [], long nbr_spl);
	long				get_available_output_samples () const;
	void				pop_output_samples (float data_ptr [], long nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum {			NBR_PHASES_L2	= 5	};
	enum {			NBR_PHASES		= 1 << NBR_PHASES_L2	};
	enum {			PHASE_LEN		= 16	};

	typedef	std::vector <float>	SplArray;

	static void		build_tables ();
	
	SplArray			_buf;
	long				_capacity;
	long				_buf_len;
	UInt32			_rate_frac;		// Fractional part of the rate * 2^32

	long				_write_pos;		// Within the final buffer, always ahead from _read_pos
	long				_nbr_spl_buf;	// Number of stored samples (finished)
	UInt32			_pos_frac;		// Position of the latest input sample relative to the current output sample
	Int16				_cur_val;		// Latest input value

	static const float
						_blep_linear [NBR_PHASES * PHASE_LEN + 1];
	static float	_blep_phase [NBR_PHASES] [PHASE_LEN];
	static float	_blep_lerp [NBR_PHASES] [PHASE_LEN];
	static bool		_blep_ok_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						ResamplerSqr (const ResamplerSqr &other);
	ResamplerSqr &	operator = (const ResamplerSqr &other);
	bool				operator == (const ResamplerSqr &other);
	bool				operator != (const ResamplerSqr &other);

};	// class ResamplerSqr



//#include	"ResamplerSqr.hpp"



#endif	// ResamplerSqr_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
