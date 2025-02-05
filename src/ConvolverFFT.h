/*****************************************************************************

        ConvolverFFT.h
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



#if ! defined (ConvolverFFT_HEADER_INCLUDED)
#define	ConvolverFFT_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"DynArray.h"
#include	"FFTReal.h"

#include	<memory>



class ConvolverFFT
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	float	DataType;

						ConvolverFFT ();
						ConvolverFFT (long nbr_spl, const DataType impulse_ptr [], float vol);

	bool				is_valid () const;
	void				set_impulse (long nbr_spl, const DataType impulse_ptr [], float vol);

	long				get_block_len () const;
	DataType *		get_input_buffer () const;
	const DataType *
						get_output_buffer () const;

	void				process ();

	void				clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	FFTReal <DataType>	FftType;
	typedef	std::unique_ptr <FftType>	FFTRealAPtr;

	FFTRealAPtr		_fft_aptr;
	DynArray <DataType>
						_impulse_freq;
	DynArray <DataType>
						_input_buf;
	DynArray <DataType>					// For data in freq domain or 0-padded
						_temp_buf;			// impulse in time domain.
	DynArray <DataType>
						_output_buf;
	long				_impulse_len;		// > 0. 0 = not initialized
	long				_fft_len;			// > _impulse_len. 0 = not initialized
	long				_block_len;			// > 0. 0 = not initialized
	long				_rem_len;			// > 0. 0 = not initialized



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						ConvolverFFT (const ConvolverFFT &other);
	ConvolverFFT &	operator = (const ConvolverFFT &other);
	bool				operator == (const ConvolverFFT &other);
	bool				operator != (const ConvolverFFT &other);

};	// class ConvolverFFT



//#include	"ConvolverFFT.hpp"



#endif	// ConvolverFFT_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
