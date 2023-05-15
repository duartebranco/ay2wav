/*****************************************************************************

        AyFileReader.h
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



#if ! defined (AyFileReader_HEADER_INCLUDED)
#define	AyFileReader_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"def.h"

#include	<string>
#include	<vector>



class AyFileReader
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						AyFileReader ();
	virtual			~AyFileReader () {}

	int				read_file (const char *filename_0);

	const std::string &
						get_author () const;
	const std::string &
						get_misc_info () const;

	const int		get_nbr_songs () const;
	const int		get_def_song () const;
	const std::string &
						get_song_name (int song) const;
	const long		get_song_duration (int song) const;
	const long		get_fade_length (int song) const;
	UInt16			get_init_reg_val (int song) const;
	UInt16			get_sp (int song) const;
	UInt16			get_init (int song) const;
	UInt16			get_interrupt (int song) const;

	int				get_nbr_blocks (int song) const;
	int				get_block_len (int song, int block) const;
	UInt16			get_block_addr (int song, int block) const;
	const UByte *	get_block_ptr (int song, int block) const;




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int				parse_file (const std::vector <UByte> &data_arr);
	UInt16			read_word (const std::vector <UByte> &data_arr, long pos);
	long				read_ptr (const std::vector <UByte> &data_arr, long pos);
	std::string		read_string (const std::vector <UByte> &data_arr, long pos);

	class SongInfo
	{
	public:
		class BlockInfo
		{
		public:
			std::vector <UByte>
								_data_arr;
			int				_len;
			UInt16			_addr;
		};

		std::string		_name;
		long				_duration;
		long				_fade_len;
		UInt16			_init_reg_val;
		UInt16			_sp;
		UInt16			_init;
		UInt16			_interrupt;
		std::vector <BlockInfo>
							_block_arr;
	};

	std::string		_author;
	std::string		_misc_info;
	std::vector <SongInfo>
						_song_arr;
	int				_def_song;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						AyFileReader (const AyFileReader &other);
	AyFileReader &	operator = (const AyFileReader &other);
	bool				operator == (const AyFileReader &other);
	bool				operator != (const AyFileReader &other);

};	// class AyFileReader



//#include	"AyFileReader.hpp"



#endif	// AyFileReader_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
