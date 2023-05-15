/*****************************************************************************

        Ay2Wav.cpp
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

#include	"Ay2Wav.h"
#include	"DcKiller.h"
#include	"DynArray.h"
#include	"fnc.h"
#include	"SpeakerAmp.h"
#include	"SpeakerFilter.h"
#include	"WavWritter.h"

#include	<cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Ay2Wav::Ay2Wav ()
:	_file_reader ()
,	_emul ()
,	_param ()
{
	// Nothing
}



int	Ay2Wav::run (int argc, char *argv [])
{
	assert (argc > 0);
	assert (argv != 0);

	// Resets emulator
	_emul.reset ();

	// Parses commande line
	int				ret_val = _param.parse_cmd_line (argc, argv);
	std::string		filename_ay3;
	if (ret_val == 0)
	{
		const bool		debug_mode_flag = _param.is_debug_mode_active ();
		_emul.set_debug_mode (debug_mode_flag);
		const bool		port_read_flag = _param.is_port_read_allowed ();
		_emul.allow_port_reading (port_read_flag);

		filename_ay3 = _param.use_ay3_filename ();

		// Reads .AY file
		ret_val = _file_reader.read_file (filename_ay3.c_str ());
	}

	int				song_index = -1;
	if (ret_val == 0)
	{
		// Gets song index to play
		const int		nbr_songs = _file_reader.get_nbr_songs ();
		song_index = _param.get_song_index ();
		if (song_index < 0)
		{
			song_index = _file_reader.get_def_song ();
		}

		if (song_index < 0 || song_index >= nbr_songs)
		{
			printf ("Error: song index out of range [1 ; %d].\n", nbr_songs);
			ret_val = -1;
		}
	}

	MixConv		conv = MixConv_STEREO;
	double		interrupt_freq = 0;
	double		sample_freq = 0;
	if (ret_val == 0)
	{
		// Prints information
		const std::string &	author = _file_reader.get_author ();
		printf ("Author: %s\n", author.c_str ());
		const std::string &	misc_info = _file_reader.get_misc_info ();
		printf ("Info: %s\n", misc_info.c_str ());

		const int		nbr_songs = _file_reader.get_nbr_songs ();
		const std::string &	song_name = _file_reader.get_song_name (song_index);
		printf ("Song %d/%d: %s\n", song_index + 1, nbr_songs, song_name.c_str ());

		// Copies data blocks
		const int		nbr_blocks = _file_reader.get_nbr_blocks (song_index);
		for (int block_cnt = 0; block_cnt < nbr_blocks; ++block_cnt)
		{
			const int		len = _file_reader.get_block_len (song_index, block_cnt);
			const UInt16	addr = _file_reader.get_block_addr (song_index, block_cnt);
			const UByte *	ptr = _file_reader.get_block_ptr (song_index, block_cnt);
			_emul.write_block_to_mem (ptr, addr, len);
		}

		// Sets interrupt rate
		interrupt_freq = _param.get_interrupt_freq ();
		_emul.set_interrupt_rate (interrupt_freq);

		// Emulation type
		const Parameters::ForceEmul	force_emul = _param.get_emulated_hardware ();
		Emulator::TypeEmul	type_emul = Emulator::TypeEmul_AUTO;
		switch (force_emul)
		{
		case	Parameters::ForceEmul_AUTO:
			type_emul = Emulator::TypeEmul_AUTO;
			break;
		case	Parameters::ForceEmul_ZX:
			type_emul = Emulator::TypeEmul_ZX;
			break;
		case	Parameters::ForceEmul_CPC:
			type_emul = Emulator::TypeEmul_CPC;
			break;
		case	Parameters::ForceEmul_ZCPC:
			type_emul = Emulator::TypeEmul_ZCPC;
			break;
		default:
			assert (false);
			break;
		}

		// Starts song
		sample_freq = _param.get_sample_freq ();
		conv = _param.get_channel_conversion ();
		const UInt16	sp = _file_reader.get_sp (song_index);
		const UInt16	interrupt = _file_reader.get_interrupt (song_index);
		const UInt16	regfill = _file_reader.get_init_reg_val (song_index);
		UInt16			init = _file_reader.get_init (song_index);
		if (init == 0)
		{
			// If INIT equal to ZERO then place to first CALL instruction address
			// of first AY file block instead of INIT (see next f) and g) steps)
			init = _file_reader.get_block_addr (song_index, 0);
		}
		ret_val = _emul.start (sample_freq, conv, init, interrupt, sp, regfill, type_emul);
		if (ret_val != 0)
		{
			printf ("Error: cannot start emulation correctly.\n");
		}
	}

	double			skip_duration = 0;
	double			rec_duration = 0;
	if (ret_val == 0)
	{
		int				nbr_frames = _file_reader.get_song_duration (song_index);
		const double	song_duration = nbr_frames / interrupt_freq;

		printf ("Song duration: ");
		if (nbr_frames == 0)
		{
			printf ("unknown\n");
		}
		else
		{
			printf ("%.2f seconds\n", song_duration);
		}

		// Finds song duration and skip time
		skip_duration = _param.get_skip_time ();
		rec_duration = _param.get_specified_duration ();
		if (_param.is_duration_ay ())
		{
			if (nbr_frames > 0)
			{
				const double	rem_time = song_duration - skip_duration;
				if (rem_time > 0)
				{
					rec_duration = rem_time;
				}
				else
				{
					printf ("Error: skip time is too long, out of the song.\n");
					ret_val = -1;
				}
			}
		}
	}
	if (ret_val == 0)
	{
		printf ("Recording duration: %.2f seconds\n", rec_duration);
		if (skip_duration > 0)
		{
			printf ("Skipping: %.2f seconds\n", skip_duration);
		}
	}

	int				nbr_chn = 2;
	int				nbr_files = 0;
	int				nbr_spec_files = 0;
	if (ret_val == 0)
	{
		// Finds the required number of channels and files
		nbr_chn = Emulator::get_nbr_audio_chn_for (conv);
		nbr_spec_files = _param.get_nbr_out_files ();
		if (nbr_spec_files <= 1)
		{
			if (nbr_spec_files == 0 && conv == MixConv_SEPARATED)
			{
				nbr_files = nbr_chn;
			}
			else
			{
				nbr_files = 1;
			}
		}
		else
		{
			nbr_files = nbr_chn;
			if (nbr_spec_files > nbr_files)
			{
				printf (
					"Error: too many specified output files (max %d).\n",
					nbr_files
				);
				ret_val = -1;
			}
		}
	}

	DynArray <std::string>	filename_arr;
	if (ret_val == 0)
	{
		// Find all filenames
		filename_arr.resize (nbr_files);
		for (int file_index = 0; file_index < nbr_files; ++file_index)
		{
			if (file_index < nbr_spec_files)
			{
				filename_arr [file_index] = _param.use_out_filename (file_index);
			}
			else
			{
				std::string		fname = filename_ay3;
				if (nbr_files > 1)
				{
					fname += '.';
					fname += static_cast <char> ('1' + file_index);
				}
				fname += ".wav";
				filename_arr [file_index] = fname;
			}
		}
	}

	DynArray <WavWritter>	wav_writter_arr;
	if (ret_val == 0)
	{
		// Prepares files
		wav_writter_arr.resize (nbr_files);
		int				nbr_chn_per_file = nbr_chn;
		if (nbr_files > 1)
		{
			nbr_chn_per_file = 1;
		}
		const WavResol	resol = _param.get_resolution ();

		for (int file_cnt = 0; file_cnt < nbr_files && ret_val == 0; ++file_cnt)
		{
			WavWritter &	ww = wav_writter_arr [file_cnt];
			const std::string &	filename = filename_arr [file_cnt];
			int				chn_index = 0;
			if (nbr_chn_per_file == 1)
			{
				chn_index = file_cnt;
			}
			ret_val = ww.open (
				filename.c_str (),
				nbr_chn_per_file,
				chn_index,
				resol,
				sample_freq
			);
		}
	}

	if (ret_val == 0)
	{
		const int		filter_quality = _param.get_speaker_filter_quality ();
		const bool		filter_flag = (filter_quality >= 0);

		// Prepares buffers & cie
		AudioBufArray	buf_arr (nbr_chn);
		DynArray <float *>	buf_ptr_arr (nbr_chn);
		DynArray <DcKiller>	dc_killer_arr (nbr_chn);
		SpeakerFilter	filter_arr [Emulator::MAX_NBR_AUDIO_CHN];
		long				buf_size = 16384;
		for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
		{
			if (filter_flag)
			{
				SpeakerFilter &	filter = filter_arr [chn_cnt];
				filter.set_quality (filter_quality);
				buf_size = filter.get_block_len ();
				buf_ptr_arr [chn_cnt] = filter.get_input_buffer ();
				buf_arr [chn_cnt].resize (buf_size);
			}

			else
			{
				buf_arr [chn_cnt].resize (buf_size);
				buf_ptr_arr [chn_cnt] = &buf_arr [chn_cnt] [0];
			}

			DcKiller &		dc_killer = dc_killer_arr [chn_cnt];
			dc_killer.set_sample_freq (sample_freq);
			dc_killer.set_cutoff_freq (10.0f);
		}

		// Sets channel activity
		const int		channel_activity = _param.get_channels ();
		_emul.set_channel_activity (channel_activity);

		// Skips audio
		const long		skip_len = round_long (skip_duration * sample_freq);
		if (skip_len > 0)
		{
			printf ("Skipping");
			long				work_pos = 0;
			while (work_pos < skip_len)
			{
				long				work_len = skip_len - work_pos;
				work_len = compute_min (work_len, buf_size);
				_emul.generate_audio (&buf_ptr_arr [0], work_len);
				printf (".");
				fflush (stdout);
				work_pos += work_len;
			}
			printf ("\n");
		}

		// Records audio
		const long		total_len = round_long (rec_duration * sample_freq);
		long				dot_len = 16384;
		long				dot_pos = dot_len;
		printf ("Generating");
		long				work_pos = 0;
		do
		{
			long				work_len = total_len - work_pos;
			work_len = compute_min (work_len, buf_size);

			// Generates sound
			_emul.generate_audio (&buf_ptr_arr [0], work_len);

			// Removes DC
			for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
			{
				DcKiller &		dc_killer = dc_killer_arr [chn_cnt];
				float *			buf_ptr = buf_ptr_arr [chn_cnt];
				dc_killer.process_block (buf_ptr, work_len);
			}

			// Speaker
			if (filter_flag)
			{
				using namespace std;

				for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
				{
					// Distortion
					float *			buf_ptr = buf_ptr_arr [chn_cnt];
					SpeakerAmp::process_block (buf_ptr, work_len);

					// Filter
					SpeakerFilter &	filter = filter_arr [chn_cnt];
					filter.process (work_len);
					const float *	src_ptr = filter.get_output_buffer ();
					float *			dest_ptr = &buf_arr [chn_cnt] [0];
					memcpy (dest_ptr, src_ptr, sizeof (*dest_ptr) * work_len);
				}
			}

			// Saves data
			for (int file_cnt = 0
			;	file_cnt < nbr_files && ret_val == 0
			;	++file_cnt)
			{
				WavWritter &	ww = wav_writter_arr [file_cnt];
				ret_val = ww.write_block (buf_arr, work_len);
			}

			work_pos += work_len;

			if (work_pos >= dot_pos)
			{
				dot_pos += dot_len;
				printf (".");
				fflush (stdout);
			}
		}
		while (work_pos < total_len && ret_val == 0);

		printf ("\n");
	}

	if (true)
	{
		// Closes files
		for (int file_cnt = 0; file_cnt < nbr_files; ++file_cnt)
		{
			WavWritter &	ww = wav_writter_arr [file_cnt];
			if (ww.is_open ())
			{
				const int		ret_val_close = ww.close ();
				if (ret_val == 0 && ret_val_close != 0)
				{
					ret_val = ret_val_close;
				}
			}
		}
	}

	// For debugging / ripping purpose
	const DebugInfo &	debug_info = _emul.use_debug_info ();
	if (debug_info.is_debug_mode_active ())
	{
		dump_uninitialised_read_addresses (debug_info);
		dump_written_ports (debug_info);
		dump_read_ports (debug_info);
	}

	return (ret_val);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Ay2Wav::dump_uninitialised_read_addresses (const DebugInfo &debug_info)
{
	assert (&debug_info != 0);

	if (debug_info.is_any_addr_read_wo_init ())
	{
		printf ("List of addresses read but not initialised:\n");
		for (long addr = 0; addr < 65536L; ++ addr)
		{
			if (debug_info.is_addr_read_wo_init (addr))
			{
				const long		pc = debug_info.get_pc_for_addr_read_wo_init (addr);
				printf (
					"   #%04X, PC = #%04X\n",
					static_cast <int> (addr),
					static_cast <int> (pc)
				);
			}
		}
		printf ("End of address list.\n");
	}
}



void	Ay2Wav::dump_written_ports (const DebugInfo &debug_info)
{
	assert (&debug_info != 0);

	if (debug_info.is_any_port_written ())
	{
		printf ("List of unexpected written ports:\n");
		for (int port = 0; port < 256; ++ port)
		{
			if (debug_info.is_port_written (port))
			{
				const long		pc = debug_info.get_pc_for_port_written (port);
				printf ("   #%02X, PC = #%04X\n", port, static_cast <int> (pc));
			}
		}
		printf ("End of written port list.\n");
	}
}



void	Ay2Wav::dump_read_ports (const DebugInfo &debug_info)
{
	assert (&debug_info != 0);

	if (debug_info.is_any_port_read ())
	{
		printf ("List of read ports:\n");
		for (int port = 0; port < 256; ++ port)
		{
			if (debug_info.is_port_read (port))
			{
				const long		pc = debug_info.get_pc_for_port_read (port);
				printf ("   #%02X, PC = #%04X\n", port, static_cast <int> (pc));
			}
		}
		printf ("End of unexpected read port list.\n");
	}
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
