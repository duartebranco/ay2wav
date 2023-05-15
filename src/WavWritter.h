/*****************************************************************************

        WavWritter.h
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



#if ! defined (WavWritter_HEADER_INCLUDED)
#define	WavWritter_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"AudioBufArray.h"
#include	"def.h"
#include	"WavResol.h"

#include	<vector>

#include	<cstdio>



class WavWritter
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						WavWritter ();
	virtual			~WavWritter ();

	int				open (const char *filename_0, int nbr_chn, int chn_index, WavResol resol, double sample_freq);
	int				write_block (const AudioBufArray &buf, long nbr_spl);
	int				close ();
	bool				is_open () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	struct WavHeader
	{
		char				_riff [4];
		UInt32			_riff_len;

		char				_wave [4];

		char				_fmt [4];
		UInt32			_fmt_len;
		Int16				_format_tag;
		UInt16			_channels;
		UInt32			_samples_per_sec;
		UInt32			_avg_bytes_per_sec;
		UInt16			_block_align;
		UInt16			_bits_per_sample;

		char				_data [4];
		UInt32			_data_len;
	};

	typedef	std::vector <UByte>	DataBuffer;

	int				write_header ();
	UInt16			conv_le_16 (UInt16 val) const;
	UInt32			conv_le_32 (UInt32 val) const;

	static inline long
						clip_spl (long x, long mi, long ma);

	FILE *			_f_ptr;
	int				_chn_index;
	int				_nbr_chn;
	long				_nbr_spl;
	WavResol			_resol;
	DataBuffer		_data_buf;
	int				_spl_size;
	int				_frame_size;
	long				_clip_min;
	long				_clip_max;
	float				_scale;
	bool				_big_endian_flag;
	WavHeader		_header;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						WavWritter (const WavWritter &other);
	WavWritter &	operator = (const WavWritter &other);
	bool				operator == (const WavWritter &other);
	bool				operator != (const WavWritter &other);

};	// class WavWritter



//#include	"WavWritter.hpp"



#endif	// WavWritter_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
