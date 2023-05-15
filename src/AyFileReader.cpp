/*****************************************************************************

        AyFileReader.cpp
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

#include	"AyFileReader.h"

#include	<cassert>
#include	<cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



AyFileReader::AyFileReader ()
:	_author ()
,	_misc_info ()
,	_song_arr ()
,	_def_song (0)
{
	// Nothing
}



int	AyFileReader::read_file (const char *filename_0)
{
	assert (filename_0 != 0);
	assert (filename_0 [0] != '\0');

	int				ret_val = 0;

	using namespace std;

	FILE *			f_ptr = fopen (filename_0, "rb");
	if (f_ptr == 0)
	{
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		ret_val = fseek (f_ptr, 0, SEEK_END);
	}

	long				file_len = 0;
	if (ret_val == 0)
	{
		file_len = ftell (f_ptr);
		ret_val = fseek (f_ptr, 0, SEEK_SET);
	}

	std::vector <UByte>	data_arr (file_len);
	if (ret_val == 0)
	{
		const long		read_bytes = fread (&data_arr [0], 1, file_len, f_ptr);
		if (read_bytes != file_len)
		{
			ret_val = -1;
		}
	}

	if (ret_val == 0)
	{
		ret_val = parse_file (data_arr);
	}

	if (f_ptr != 0)
	{
		fclose (f_ptr);
		f_ptr = 0;
	}

	if (ret_val != 0)
	{
		printf ("Error: cannot read file \"%s\".\n", filename_0);
	}

	return (ret_val);
}



const std::string &	AyFileReader::get_author () const
{
	return (_author);
}



const std::string &	AyFileReader::get_misc_info () const
{
	return (_misc_info);
}



const int	AyFileReader::get_nbr_songs () const
{
	return (_song_arr.size ());
}



const int	AyFileReader::get_def_song () const
{
	return (_def_song);
}



const std::string &	AyFileReader::get_song_name (int song) const
{
	assert (song >= 0);
	assert (song < get_nbr_songs ());

	return (_song_arr [song]._name);
}



const long	AyFileReader::get_song_duration (int song) const
{
	assert (song >= 0);
	assert (song < get_nbr_songs ());

	return (_song_arr [song]._duration);
}



const long	AyFileReader::get_fade_length (int song) const
{
	assert (song >= 0);
	assert (song < get_nbr_songs ());

	return (_song_arr [song]._fade_len);
}



UInt16	AyFileReader::get_init_reg_val (int song) const
{
	assert (song >= 0);
	assert (song < get_nbr_songs ());

	return (_song_arr [song]._init_reg_val);
}



UInt16	AyFileReader::get_sp (int song) const
{
	assert (song >= 0);
	assert (song < get_nbr_songs ());

	return (_song_arr [song]._sp);
}



UInt16	AyFileReader::get_init (int song) const
{
	assert (song >= 0);
	assert (song < get_nbr_songs ());

	return (_song_arr [song]._init);
}



UInt16	AyFileReader::get_interrupt (int song) const
{
	assert (song >= 0);
	assert (song < get_nbr_songs ());

	return (_song_arr [song]._interrupt);
}



int	AyFileReader::get_nbr_blocks (int song) const
{
	assert (song >= 0);
	assert (song < get_nbr_songs ());

	return (_song_arr [song]._block_arr.size ());
}



int	AyFileReader::get_block_len (int song, int block) const
{
	assert (song >= 0);
	assert (song < get_nbr_songs ());
	assert (block >= 0);
	assert (block < get_nbr_blocks (song));

	return (_song_arr [song]._block_arr [block]._len);
}



UInt16	AyFileReader::get_block_addr (int song, int block) const
{
	assert (song >= 0);
	assert (song < get_nbr_songs ());
	assert (block >= 0);
	assert (block < get_nbr_blocks (song));

	return (_song_arr [song]._block_arr [block]._addr);
}



const UByte *	AyFileReader::get_block_ptr (int song, int block) const
{
	assert (song >= 0);
	assert (song < get_nbr_songs ());
	assert (block >= 0);
	assert (block < get_nbr_blocks (song));

	return (&_song_arr [song]._block_arr [block]._data_arr [0]);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	AyFileReader::parse_file (const std::vector <UByte> &data_arr)
{
	assert (&data_arr != 0);

	int				ret_val = 0;

	const long		file_len = data_arr.size ();

	if (   file_len < 20
	    || data_arr [0] != 'Z'
	    || data_arr [1] != 'X'
	    || data_arr [2] != 'A'
	    || data_arr [3] != 'Y'
	    || data_arr [4] != 'E'
	    || data_arr [5] != 'M'
	    || data_arr [6] != 'U'
	    || data_arr [7] != 'L')
	{
		printf ("Error: this is not a valid .AY file.\n");
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		const long		author_pos = read_ptr (data_arr, 12);
		const long		misc_pos = read_ptr (data_arr, 14);
		_author = read_string (data_arr, author_pos);
		_misc_info = read_string (data_arr, misc_pos);
		const int		nbr_songs = data_arr [16] + 1;
		_def_song = data_arr [17];
		_song_arr.resize (nbr_songs);
		const long		song_struct_pos = read_ptr (data_arr, 18);

		for (int song_cnt = 0; song_cnt < nbr_songs; ++song_cnt)
		{
			SongInfo &		song = _song_arr [song_cnt];
			const long		tssp = song_struct_pos + song_cnt * 4;
			const long		name_pos = read_ptr (data_arr, tssp);
			const long		data_pos = read_ptr (data_arr, tssp + 2);
			song._name = read_string (data_arr, name_pos);
			song._duration = read_word (data_arr, data_pos + 4);
			song._fade_len = read_word (data_arr, data_pos + 6);
			song._init_reg_val = read_word (data_arr, data_pos + 8);
			const long		point_pos = read_ptr (data_arr, data_pos + 10);
			const long		addr_pos = read_ptr (data_arr, data_pos + 12);

			song._sp = read_word (data_arr, point_pos + 0);
			song._init = read_word (data_arr, point_pos + 2);
			song._interrupt = read_word (data_arr, point_pos + 4);

			song._block_arr.clear ();
			int				block_cnt = 0;
			bool				cont_flag = true;
			while (cont_flag)
			{
				const long		ap = addr_pos + block_cnt * 6;
				const UInt16	block_addr = read_word (data_arr, ap);
				if (block_addr == 0)
				{
					cont_flag = false;
				}
				else
				{
					SongInfo::BlockInfo	block;
					block._addr = block_addr;
					block._len = read_word (data_arr, ap + 2);
					block._data_arr.clear ();
					const long		offset = read_ptr (data_arr, ap + 4);
					for (int d = 0
					;	   d < block._len
						&& block_addr + d < 0x10000L
						&& offset + d < file_len
					; ++d)
					{
						const UByte		val = data_arr [offset + d];
						block._data_arr.push_back (val);
					}

					song._block_arr.push_back (block);
					++ block_cnt;
				}
			}
		}
	}

	return (ret_val);
}



UInt16	AyFileReader::read_word (const std::vector <UByte> &data_arr, long pos)
{
	assert (&data_arr != 0);
	assert (pos >= 0);
	assert (pos <= static_cast <long> (data_arr.size ()) - 2);

	const UInt16	val = (data_arr [pos] << 8) + data_arr [pos + 1];

	return (val);
}



long	AyFileReader::read_ptr (const std::vector <UByte> &data_arr, long pos)
{
	assert (&data_arr != 0);
	assert (pos >= 0);
	assert (pos <= static_cast <long> (data_arr.size ()) - 2);

	Int16				val = static_cast <Int16> (read_word (data_arr, pos));
	val += pos;

	return (val);
}



std::string	AyFileReader::read_string (const std::vector <UByte> &data_arr, long pos)
{
	assert (&data_arr != 0);
	assert (pos >= 0);
	assert (pos <= static_cast <long> (data_arr.size ()) - 1);

	std::string		str;
	const long		file_len = data_arr.size ();
	while (pos < file_len && data_arr [pos] != '\0')
	{
		str += static_cast <char> (data_arr [pos]);
		++ pos;
	}

	return (str);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
