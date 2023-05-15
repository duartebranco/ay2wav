/*****************************************************************************

        WavWritter.cpp
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

#include	"fnc.h"
#include	"WavWritter.h"

#include	<algorithm>

#include	<cassert>
#include	<cstdio>

namespace std { }



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



WavWritter::WavWritter ()
:	_f_ptr (0)
,	_chn_index (0)
,	_nbr_chn (1)
,	_nbr_spl (0)
,	_resol (WavResol_I16)
,	_data_buf ()
,	_spl_size (2)
,	_frame_size (2)
,	_clip_min (-32768L)
,	_clip_max (32767L)
,	_scale (1.0)
,	_big_endian_flag (false)
,	_header ()
{
	int				x = 1;
	_big_endian_flag = (*reinterpret_cast <UByte *> (&x) == 0);
}



WavWritter::~WavWritter ()
{
	if (_f_ptr != 0)
	{
		close ();
	}
}



int	WavWritter::open (const char *filename_0, int nbr_chn, int chn_index, WavResol resol, double sample_freq)
{
	assert (! is_open ());
	assert (filename_0 != 0);
	assert (filename_0 [0] != '\0');
	assert (nbr_chn > 0);
	assert (chn_index >= 0);
	assert (resol >= 0);
	assert (resol < WavResol_NBR_ELT);

	using namespace std;

	_resol = resol;
	_nbr_chn = nbr_chn;
	_chn_index = chn_index;
	_spl_size = 0;
	_nbr_spl = 0;
	_f_ptr = fopen (filename_0, "wb");
	int				ret_val = (_f_ptr != 0) ? 0 : -1;
	if (ret_val != 0)
	{
		printf ("Error: cannot open file %s\n", filename_0);
	}

	if (ret_val == 0)
	{
		switch (_resol)
		{
		case	WavResol_I08:
			_spl_size = 1;
			_clip_min = -128L;
			_clip_max = 127L;
			_scale = _clip_min / -32768.0f;
			break;
		case	WavResol_I16:
			_spl_size = 2;
			_clip_min = -32768L;
			_clip_max = 32767L;
			_scale = _clip_min / -32768.0f;
			break;
		case	WavResol_I24:
			_spl_size = 3;
			_clip_min = -8388608;
			_clip_max = 8388607;
			_scale = _clip_min / -32768.0f;
			break;
		case	WavResol_F32:
			_spl_size = 4;
			_scale = 1.0f / 32768.0f;
			break;
		default:
			assert (false);
			break;
		}
		_frame_size = _spl_size * _nbr_chn;
	}

#if 1

	if (ret_val == 0)
	{
		_header._riff [0] = 'R';
		_header._riff [1] = 'I';
		_header._riff [2] = 'F';
		_header._riff [3] = 'F';
		_header._riff_len = 0;	// Later

		_header._wave [0] = 'W';
		_header._wave [1] = 'A';
		_header._wave [2] = 'V';
		_header._wave [3] = 'E';

		_header._fmt [0] = 'f';
		_header._fmt [1] = 'm';
		_header._fmt [2] = 't';
		_header._fmt [3] = ' ';
		_header._fmt_len = conv_le_32 (16);

		const long		fs_int = round_long (sample_freq);
		int				format = 1;	// WAVE_FORMAT_PCM
		if (_resol == WavResol_F32)
		{
			format = 3;	// WAVE_FORMAT_IEEE_FLOAT
		}

		_header._format_tag        = conv_le_16 (format);
		_header._channels          = conv_le_16 (nbr_chn);
		_header._samples_per_sec   = conv_le_32 (fs_int);
		_header._avg_bytes_per_sec = conv_le_32 (fs_int * _frame_size);
		_header._block_align       = conv_le_16 (_frame_size);
		_header._bits_per_sample   = conv_le_16 (_spl_size * 8);

		_header._data [0] = 'd';
		_header._data [1] = 'a';
		_header._data [2] = 't';
		_header._data [3] = 'a';
		_header._data_len = 0;	// Later

		ret_val = write_header ();
	}

#endif

	return (ret_val);
}



int	WavWritter::write_block (const AudioBufArray &buf, long nbr_spl)
{
	assert (is_open ());
	assert (&buf != 0);
	assert (nbr_spl > 0);

	int				ret_val = 0;

	const long		buf_len = nbr_spl * _frame_size;
	_data_buf.resize (buf_len);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
		{
			const int		out_pos = (pos * _nbr_chn + chn_cnt) * _spl_size;
			const int		chn = _chn_index + chn_cnt;
			const float		spl_flt = buf [chn] [pos] * _scale;

			long				spl_int = 0;
			if (_resol != WavResol_F32)
			{
				spl_int = round_long (spl_flt);
				spl_int = clip_spl (spl_int, _clip_min, _clip_max);
			}

			switch (_resol)
			{
			case	WavResol_I08:
				_data_buf [out_pos] = static_cast <UByte> (spl_int + 0x80);
				break;
			case	WavResol_I16:
				_data_buf [out_pos    ] = static_cast <UByte> (spl_int);
				_data_buf [out_pos + 1] = static_cast <UByte> (spl_int >> 8);
				break;
			case	WavResol_I24:
				_data_buf [out_pos    ] = static_cast <UByte> (spl_int);
				_data_buf [out_pos + 1] = static_cast <UByte> (spl_int >> 8);
				_data_buf [out_pos + 2] = static_cast <UByte> (spl_int >> 16);
				break;
			case	WavResol_F32:
				*reinterpret_cast <float *> (&_data_buf [out_pos]) = spl_flt;
				if (_big_endian_flag)
				{
					std::swap (_data_buf [out_pos    ], _data_buf [out_pos + 3]);
					std::swap (_data_buf [out_pos + 1], _data_buf [out_pos + 2]);
				}
				break;
			default:
				assert (false);
				break;
			}
		}
	}

	const long		nbr_spl_written =
		fwrite (&_data_buf [0], _frame_size, nbr_spl, _f_ptr);
	if (nbr_spl_written != nbr_spl)
	{
		printf ("Error: failed to write sample data into file.\n");
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		_nbr_spl += nbr_spl;
	}

	return (ret_val);
}



int	WavWritter::close ()
{
	assert (is_open ());

	using namespace std;

	int				ret_val = 0;

#if 1

	ret_val = write_header ();

#endif

	const int		ret_val_fclose = fclose (_f_ptr);
	_f_ptr = 0;
	if (ret_val_fclose != 0)
	{
		printf ("Error: cannot close file successfully.\n");
		if (ret_val == 0)
		{
			ret_val = -1;
		}
	}

	DataBuffer ().swap (_data_buf);

	return (ret_val);
}



bool	WavWritter::is_open () const
{
	return (_f_ptr != 0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	WavWritter::write_header ()
{
	assert (is_open ());

	using namespace std;

	int				ret_val = 0;

	const long		data_size = _nbr_spl * _frame_size;
	const long		file_size = sizeof (WavHeader) - 8 + data_size;
	_header._data_len = conv_le_32 (data_size);
	_header._riff_len = conv_le_32 (file_size);

	const long		old_pos = ftell (_f_ptr);
	if (old_pos == -1)
	{
		ret_val = -1;
	}

	if (ret_val == 0 && fseek (_f_ptr, 0, SEEK_SET) != 0)
	{
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		const int		nbr_obj_written =
			fwrite (&_header, sizeof (_header), 1, _f_ptr);
		if (nbr_obj_written != 1)
		{
			ret_val = -1;
		}
	}

	if (ret_val == 0 && fseek (_f_ptr, old_pos, SEEK_SET) != 0)
	{
		ret_val = -1;
	}

	if (ret_val != 0)
	{
		printf ("Error: cannot write header \n");
	}

	return (ret_val);
}



UInt16	WavWritter::conv_le_16 (UInt16 val) const
{
	if (_big_endian_flag)
	{
		val =   ((val >> 8) & 0xFF)
		      + ((val & 0xFF) << 8);
	}

	return (val);
}



UInt32	WavWritter::conv_le_32 (UInt32 val) const
{
	if (_big_endian_flag)
	{
		val =   ((val >> 24) & 0x00FFUL)
		      + ((val >>  8) & 0xFF00UL)
		      + ((val & 0xFF00UL) <<  8)
		      + ((val & 0x00FFUL) << 24);
	}

	return (val);
}



long	WavWritter::clip_spl (long x, long mi, long ma)
{
	return (  (x < mi)
	        ? mi
	        : (  (ma < x)
	           ? ma
	           : x       )
	);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
