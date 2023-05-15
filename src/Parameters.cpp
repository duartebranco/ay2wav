/*****************************************************************************

        Parameters.cpp
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

#include	"Parameters.h"

#include	<cassert>
#include	<cctype>
#include	<cstdio>
#include	<cstdlib>
#include	<cstring>



template <class T>
static T		Parameters_get_max (const T &a, const T &b)
{
	return ((a < b) ? b : a);
}

template <class T>
static T		Parameters_get_min (const T &a, const T &b)
{
	return ((a < b) ? a : b);
}



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Parameters::Parameters ()
:	_debug_mode_active_flag (false)
,	_port_read_flag (true)
,	_ay3_filename ()
,	_song_index (-1)
,	_out_filename_arr ()
,	_conversion (MixConv_STEREO)
,	_channels (0x7)
,	_resol (WavResol_I16)
,	_sample_freq (44100.0)
,	_interrupt_freq (50.0)
,	_duration_ay_flag (true)
,	_duration (90.0)
,	_skip_time (0)
,	_emul_hw (ForceEmul_AUTO)
,	_speaker_filter_quality (-1)
{
	// Nothing
}



int	Parameters::parse_cmd_line (int argc, char *argv [])
{
	int				ret_val = 0;

	if (argc < 2)
	{
		if (ret_val != 0)
		{
			printf ("Error: too few arguments.\n");
		}
		ret_val = -1;
	}

	OptList			opt_list;
	if (ret_val == 0)
	{
		_ay3_filename.erase (_ay3_filename.begin (), _ay3_filename.end ());
		_song_index = -1;
		_out_filename_arr.clear ();
		_conversion = MixConv_STEREO;
		_channels = 0x7;
		_resol = WavResol_I16;
		_sample_freq = 44100.0;
		_interrupt_freq = 50.0;
		_duration_ay_flag = true;
		_duration = 90.0;
		_skip_time = 0;

		_ay3_filename = argv [argc - 1];

		ret_val = collect_options (opt_list, argv, argc - 1);
		if (ret_val != 0)
		{
			printf ("Error: syntax.\n");
		}
	}

	if (ret_val == 0)
	{
		typedef	int	(Parameters::*OptProcFnc) (const FullOpt &);
		static const OptProcFnc	opt_proc_fnc_arr [Opt_NBR_ELT] =
		{
			&Parameters::process_opt_song_index,
			&Parameters::process_opt_out_file,
			&Parameters::process_opt_chn_mix,
			&Parameters::process_opt_chn_act,
			&Parameters::process_opt_resol,
			&Parameters::process_opt_spl_freq,
			&Parameters::process_opt_inter_freq,
			&Parameters::process_opt_duration,
			&Parameters::process_opt_skip,
			&Parameters::process_opt_debug,
			&Parameters::process_opt_emul_type,
			&Parameters::process_opt_speaker
		};

		const int		nbr_opt = opt_list.size ();
		for (int opt_index = 0; opt_index < nbr_opt && ret_val == 0; ++opt_index)
		{
			const FullOpt &	opt = opt_list [opt_index];
			assert (opt._opt >= 0);
			assert (opt._opt < Opt_NBR_ELT);

			const OptProcFnc	opt_proc = opt_proc_fnc_arr [opt._opt];
			assert (opt_proc != 0);
			ret_val = (this->*opt_proc) (opt);
		}
	}

	if (ret_val != 0)
	{
		print_usage (argv [0]);
	}

	return (ret_val);
}



void	Parameters::print_usage (const char *progname_0) const
{
	assert (progname_0 != 0);

	using namespace std;

	printf (
"AY2Wav - A .AY to .WAV file converter\n"
"\n"
"Written by Laurent de Soras, 2006\n"
"Z80 emulator by Marat Fayzullin, (c) 1994-2002\n"
"\n"
".AY files are music files from Amstrad CPC and Sinclair Spectrum computers.\n"
"They contain music data as well as the code required to play it back on the\n"
"original computer. This program emulates the Zilog Z80 microprocessor and the\n"
"General Instruments AY-3-8910/8912 (or Yamaha YM2149) Programmable Sound\n"
"Generator, in order to render an audio sample file in the Wav format.\n"
"\n"
"Usage:\n"
"\n"
"   %s [options] <ay_filename>\n"
"\n"
"Options:\n"
"\n"
"   -s <song_number>\n"
"      Selects the song number, ranging from 1 to N, where N is the number of\n"
"      songs stored in the .AY file.\n"
"      Default is the \"main\" song, indicated in the .AY file.\n"
"\n"
"   -o <out_filename> [<out_filename2> [<out_filename3>]]\n"
"      Selects a filename for the output. One can have multiple filenames to\n"
"      split multichannel streams into mono files.\n"
"      Default is 1 file based on the input filename, except when rendering\n"
"      with -c separated, where there are 3 mono files, numbered.\n"
"\n"
"   -c mono | stereo | separated\n"
"      Selects the internal channel mixing for the AY-3-8910.\n"
"      - mono mixes the ABC channels into one single channel\n"
"      - stereo sends A to left channel, C to right channel and B is mixed on\n"
"         the center (half-volume).\n"
"      - separated doesn't mix anything and dumps the AY-3-8910 outputs\n"
"         individually.\n"
"      Default is stereo.\n"
"\n"
"   -p [A][B][C]\n"
"      Selects AY-3-8910 channels to activate; others are muted. This option\n"
"      does not applies when dumping sound to separated channels.\n"
"      Default is ABC.\n"
"\n"
"   -r 8 | 16 | 24 | 32f\n"
"      Selects the output resolution: 8, 16 or 24 bits integer, 32f for 32-bit\n"
"      floating point (scale: 0 dB = +/-1.0).\n"
"      Default is 16 bits.\n"
"\n"
"   -f <sample_rate>\n"
"      Selects the output sample rate, in Hz. Valid range: [%.0f ; %.0f].\n"
"      Default is 44100 Hz.\n"
"\n"
"   -i <interrupt_rate>\n"
"      Selects the interrupt rate, in Hz. This affects the speed of the song.\n"
"      Valid range: [%.0f ; %.0f]\n"
"      Default is 50 Hz.\n"
"\n"
"   -d <duration_s> | ay\n"
"      Selects the playback duration, in seconds, or read it in the .AY file,\n"
"      if it is configured. Valid range: [%.0f ; %.0f]\n"
"      Default is 90 seconds.\n"
"\n"
"   -k <duration_s>\n"
"      Skips the beginning of the song before recording it. The duration is in\n"
"      seconds. Then it will record sound during a fixed amount of time, except\n"
"      if the -d option has been selected with parameter \"ay\", whereas the song\n"
"      always terminates at the same timestamp (thus reducing the recording\n"
"      duration).\n"
"      Default is 0 second.\n"
"\n"
"   -b off | on | noin\n"
"      Activate or deactivate the debugging mode. At the end of the processing,\n"
"      it will list addresses which have been read without being written or\n"
"      initialised first. It will also list non-AY3 accessed ports. noin mode\n"
"      prohibits reading of ports, because some AY file players do not support\n"
"      this feature.\n"
"      Default is off.\n"
"\n"
"   -e auto | zx | cpc | zcpc\n"
"      Indicates the machine type for the emulation. Because AY files were\n"
"      originally designed for Spectrum computers only, they haven\'t any\n"
"      information about the hardware they were actually designed for or ripped\n"
"      from. This option can force a specific hardware to be emulated.\n"
"      - auto performs an automatic detection\n"
"      - zx forces emulation of a Sinclair Spectrum\n"
"      - cpc is for an Amstrad CPC\n"
"      - zcpc is for early musics ripped from Amstrad CPC and patched to use\n"
"      the Spectrum hardware. However this trick detunes them. This option\n"
"      fixes this problem.\n"
"      Default is auto.\n"
"\n"
"   -a off | 1 | 2 | 3\n"
"      Activates or deactivates the Amstrad CPC speaker emulation. 3 sets the\n"
"      best fidelity.\n"
"      Default is off.\n"
"\n",
		progname_0,
		_min_sample_freq,
		_max_sample_freq,
		_min_interrupt_freq,
		_max_interrupt_freq,
		_min_duration,
		_max_duration
	);
}



bool	Parameters::is_debug_mode_active () const
{
	return (_debug_mode_active_flag);
}



bool	Parameters::is_port_read_allowed () const
{
	return (_port_read_flag);
}



const std::string &	Parameters::use_ay3_filename () const
{
	return (_ay3_filename);
}



// Has to be checked for the upper bound
int	Parameters::get_song_index () const
{
	return (_song_index);
}



int	Parameters::get_nbr_out_files () const
{
	return (_out_filename_arr.size ());
}



const std::string &	Parameters::use_out_filename (int index) const
{
	assert (index >= 0);
	assert (index < get_nbr_out_files ());

	return (_out_filename_arr [index]);
}



MixConv	Parameters::get_channel_conversion () const
{
	return (_conversion);
}



int	Parameters::get_channels () const
{
	return (_channels);
}



WavResol	Parameters::get_resolution () const
{
	return (_resol);
}



double	Parameters::get_sample_freq () const
{
	return (_sample_freq);
}



double	Parameters::get_interrupt_freq () const
{
	return (_interrupt_freq);
}



bool	Parameters::is_duration_ay () const
{
	return (_duration_ay_flag);
}



double	Parameters::get_specified_duration () const
{
	return (_duration);
}



double	Parameters::get_skip_time () const
{
	return (_skip_time);
}



Parameters::ForceEmul	Parameters::get_emulated_hardware () const
{
	return (_emul_hw);
}



int	Parameters::get_speaker_filter_quality () const
{
	return (_speaker_filter_quality);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	Parameters::collect_options (OptList &opt_list, char *argv [], long opt_len) const
{
	assert (&opt_list != 0);
	assert (argv != 0);
	assert (opt_len >= 0);

	int				ret_val = 0;

	opt_list.clear ();
	std::vector <std::string>	cur_arg_list;

	for (int opt_index = opt_len - 1
	;	opt_index > 0 && ret_val == 0
	;	-- opt_index)
	{
		const Truc *	opt_ptr = find_option (argv, opt_index);
		if (opt_ptr == 0)
		{
			cur_arg_list.insert (cur_arg_list.begin (), argv [opt_index]);
		}
		else
		{
			const int		arg_list_size = cur_arg_list.size ();
			if (   arg_list_size < opt_ptr->_nbr_arg_min
			    || arg_list_size > opt_ptr->_nbr_arg_max)
			{
				ret_val = -1;
			}
			else
			{
				FullOpt			full_opt;
				full_opt._opt = opt_ptr->_opt;
				full_opt._arg_list = cur_arg_list;
				opt_list.insert (opt_list.begin (), full_opt);

				cur_arg_list.clear ();
			}
		}
	}

	return (ret_val);
}



// Returns 0 if not found
const Parameters::Truc *	Parameters::find_option (char *argv [], int arg_index) const
{
	assert (argv != 0);
	assert (arg_index > 0);

	const Truc *	opt_ptr = 0;

	for (int opt_index = 0; opt_index < Opt_NBR_ELT && opt_ptr == 0; ++opt_index)
	{
		using namespace std;

		const Truc &	opt = _opt_table [opt_index];
		const char *	arg_0 = argv [arg_index];
		assert (arg_0 != 0);
		if (strcmp (arg_0, opt._opt_name_0) == 0)
		{
			opt_ptr = &opt;
		}
	}

	return (opt_ptr);
}



int	Parameters::process_opt_song_index (const FullOpt &opt)
{
	assert (&opt != 0);

	int				ret_val = conv_int (_song_index, opt._arg_list [0]);
	-- _song_index;
	if (ret_val == 0 && _song_index < 0)
	{
		ret_val = -1;
		printf ("Error: song index cannot be negative or null.\n");
	}

	return (ret_val);
}



int	Parameters::process_opt_out_file (const FullOpt &opt)
{
	assert (&opt != 0);

	_out_filename_arr = opt._arg_list;

	return (0);
}



int	Parameters::process_opt_chn_mix (const FullOpt &opt)
{
	assert (&opt != 0);

	int				ret_val = 0;

	const char *		arg_0 = opt._arg_list [0].c_str ();
	if (strcmp_ci (arg_0, "mono"))
	{
		_conversion = MixConv_MONO;
	}
	else if (strcmp_ci (arg_0, "stereo"))
	{
		_conversion = MixConv_STEREO;
	}
	else if (strcmp_ci (arg_0, "separated"))
	{
		_conversion = MixConv_SEPARATED;
	}
	else
	{
		printf ("Error: unknown mix specification.\n");
		ret_val = -1;
	}

	return (ret_val);
}



int	Parameters::process_opt_chn_act (const FullOpt &opt)
{
	assert (&opt != 0);

	int				ret_val = 0;

	_channels = 0;
	const long		len = opt._arg_list [0].length ();
	for (long i = 0; i < len && ret_val == 0; ++i)
	{
		using namespace std;
		const char		c = toupper (opt._arg_list [0] [i]);
		const int		k = c - 'A';
		if (k < 0 || k > 2)
		{
			printf ("Error: allowed channels are A, B and C.\n");
			ret_val = -1;
		}
		_channels |= 1 << k;
	}

	return (ret_val);
}



int	Parameters::process_opt_resol (const FullOpt &opt)
{
	assert (&opt != 0);

	int				ret_val = 0;

	const char *		arg_0 = opt._arg_list [0].c_str ();
	if (strcmp_ci (arg_0, "8"))
	{
		_resol = WavResol_I08;
	}
	else if (strcmp_ci (arg_0, "16"))
	{
		_resol = WavResol_I16;
	}
	else if (strcmp_ci (arg_0, "32"))
	{
		_resol = WavResol_I24;
	}
	else if (strcmp_ci (arg_0, "32f"))
	{
		_resol = WavResol_F32;
	}
	else
	{
		printf ("Error: unknown output resolution.\n");
		ret_val = -1;
	}

	return (ret_val);
}



int	Parameters::process_opt_spl_freq (const FullOpt &opt)
{
	assert (&opt != 0);

	int				ret_val = conv_double (_sample_freq, opt._arg_list [0]);
	if (ret_val == 0 && (   _sample_freq < _min_sample_freq
				            || _sample_freq > _max_sample_freq))
	{
		printf (
			"Error: Sample frequency is out of range [%f ; %f].\n",
			_min_sample_freq,
			_max_sample_freq
		);
		ret_val = -1;
	}

	return (ret_val);
}



int	Parameters::process_opt_inter_freq (const FullOpt &opt)
{
	assert (&opt != 0);

	int				ret_val = conv_double (_interrupt_freq, opt._arg_list [0]);
	if (ret_val == 0 && (   _interrupt_freq < _min_interrupt_freq
	                     || _interrupt_freq > _max_interrupt_freq))
	{
		printf (
			"Error: Sample frequency is out of range [%f ; %f].\n",
			_min_interrupt_freq,
			_max_interrupt_freq
		);
		ret_val = -1;
	}

	return (ret_val);
}



int	Parameters::process_opt_duration (const FullOpt &opt)
{
	assert (&opt != 0);

	int				ret_val = 0;

	if (strcmp_ci (opt._arg_list [0].c_str (), "ay"))
	{
		_duration = 90.0;
		_duration_ay_flag = true;
	}
	else
	{
		ret_val = conv_double (_duration, opt._arg_list [0]);
		_duration_ay_flag = false;
		if (ret_val == 0 && (   _duration < _min_duration
		                     || _duration > _max_duration))
		{
			printf (
				"Error: Song duration is out of range [%f ; %f].\n",
				_min_duration,
				_max_duration
			);
			ret_val = -1;
		}
	}

	return (ret_val);
}



int	Parameters::process_opt_skip (const FullOpt &opt)
{
	assert (&opt != 0);

	int				ret_val = conv_double (_skip_time, opt._arg_list [0]);
	if (ret_val == 0 && (   _skip_time < _min_skip_time
	                     || _skip_time > _max_skip_time))
	{
		printf (
			"Error: Skip time is out of range [%f ; %f].\n",
			_min_skip_time,
			_max_skip_time
		);
		ret_val = -1;
	}

	return (ret_val);
}



int	Parameters::process_opt_debug (const FullOpt &opt)
{
	assert (&opt != 0);

	int				ret_val = 0;

	const char *		arg_0 = opt._arg_list [0].c_str ();
	if (strcmp_ci (arg_0, "off"))
	{
		_debug_mode_active_flag = false;
		_port_read_flag = true;
	}
	else if (strcmp_ci (arg_0, "on"))
	{
		_debug_mode_active_flag = true;
		_port_read_flag = true;
	}
	else if (strcmp_ci (arg_0, "noin"))
	{
		_debug_mode_active_flag = true;
		_port_read_flag = false;
	}
	else
	{
		printf ("Error: unknown debug mode.\n");
		ret_val = -1;
	}

	return (ret_val);
}



int	Parameters::process_opt_emul_type (const FullOpt &opt)
{
	assert (&opt != 0);

	int				ret_val = 0;

	const char *		arg_0 = opt._arg_list [0].c_str ();
	if (strcmp_ci (arg_0, "auto"))
	{
		_emul_hw = ForceEmul_AUTO;
	}
	else if (strcmp_ci (arg_0, "zx"))
	{
		_emul_hw = ForceEmul_ZX;
	}
	else if (strcmp_ci (arg_0, "cpc"))
	{
		_emul_hw = ForceEmul_CPC;
	}
	else if (strcmp_ci (arg_0, "zcpc"))
	{
		_emul_hw = ForceEmul_ZCPC;
	}
	else
	{
		printf ("Error: unknown debug mode.\n");
		ret_val = -1;
	}

	return (ret_val);
}



int	Parameters::process_opt_speaker (const FullOpt &opt)
{
	assert (&opt != 0);

	int				ret_val = 0;

	const char *		arg_0 = opt._arg_list [0].c_str ();
	if (strcmp_ci (arg_0, "off"))
	{
		_speaker_filter_quality = -1;
	}
	else
	{
		ret_val = conv_int (_speaker_filter_quality, opt._arg_list [0]);
		if (ret_val == 0)
		{
			-- _speaker_filter_quality;
		}
	}

	if (ret_val != 0)
	{
		printf ("Error: unknown speaker emulation mode.\n");
		ret_val = -1;
	}

	return (ret_val);
}



bool	Parameters::strcmp_ci (const char *s1_0, const char *s2_0)
{
	using namespace std;

	long				pos = 0;
	bool				ok_flag = true;
	do
	{
		ok_flag = (toupper (s1_0 [pos]) == toupper (s2_0 [pos]));
		++pos;
	}
	while (ok_flag && s1_0 [pos-1] != '\0');

	return (ok_flag);
}



int	Parameters::conv_int (int &result, const std::string &s)
{
	assert (&result != 0);
	assert (&s != 0);

	int				ret_val = 0;

	const char *	s_0 = s.c_str ();
	const char *	test_0 = s_0;
	result = strtol (s_0, const_cast <char **> (&test_0), 10);
	if (test_0 == s_0)
	{
		ret_val = -1;
	}

	return (ret_val);
}



int	Parameters::conv_double (double &result, const std::string &s)
{
	assert (&result != 0);
	assert (&s != 0);

	int				ret_val = 0;

	const char *	s_0 = s.c_str ();
	const char *	test_0 = s_0;
	result = strtod (s_0, const_cast <char **> (&test_0));
	if (test_0 == s_0)
	{
		ret_val = -1;
	}

	return (ret_val);
}



const Parameters::Truc	Parameters::_opt_table [Opt_NBR_ELT] =
{
	{ Opt_SONG_INDEX, "-s", 1, 1 },
	{ Opt_OUTFILE,    "-o", 1, 3 },
	{ Opt_CHN_MIX,    "-c", 1, 1 },
	{ Opt_CHN_ACT,    "-p", 1, 1 },
	{ Opt_RESOL,      "-r", 1, 1 },
	{ Opt_SPL_FREQ,   "-f", 1, 1 },
	{ Opt_INTER_FREQ, "-i", 1, 1 },
	{ Opt_DURATION,   "-d", 1, 1 },
	{ Opt_SKIP,       "-k", 1, 1 },
	{ Opt_DEBUG,      "-b", 1, 1 },
	{ Opt_EMUL_TYPE,  "-e", 1, 1 },
	{ Opt_SPEAKER,    "-a", 1, 1 }
};



const double	Parameters::_max_sample_freq = 125000.0;
const double	Parameters::_min_sample_freq = 4000.0;
const double	Parameters::_max_interrupt_freq = 300.0;
const double	Parameters::_min_interrupt_freq = 1.0;
const double	Parameters::_max_duration = 36000.0;
const double	Parameters::_min_duration = 0.0;
const double	Parameters::_max_skip_time = _max_duration;
const double	Parameters::_min_skip_time = _min_duration;



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
