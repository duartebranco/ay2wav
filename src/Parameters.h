/*****************************************************************************

        Parameters.h
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



#if ! defined (Parameters_HEADER_INCLUDED)
#define	Parameters_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"MixConv.h"
#include	"WavResol.h"

#include	<string>
#include	<vector>



class Parameters
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum ForceEmul
	{
		ForceEmul_AUTO = 0,
		ForceEmul_ZX,
		ForceEmul_CPC,
		ForceEmul_ZCPC,

		ForceEmul_NBR_ELT
	};

						Parameters ();
	virtual			~Parameters () {}

	int				parse_cmd_line (int argc, char *argv []);
	void				print_usage (const char *progname_0) const;

	bool				is_debug_mode_active () const;
	bool				is_port_read_allowed () const;
	const std::string &
						use_ay3_filename () const;

	int				get_song_index () const;
	int				get_nbr_out_files () const;
	const std::string &
						use_out_filename (int index) const;
	MixConv			get_channel_conversion () const;
	int				get_channels () const;
	WavResol			get_resolution () const;
	double			get_sample_freq () const;
	double			get_interrupt_freq () const;
	bool				is_duration_ay () const;
	double			get_specified_duration () const;
	double			get_skip_time () const;
	ForceEmul		get_emulated_hardware () const;
	int				get_speaker_filter_quality () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Opt
	{
		Opt_SONG_INDEX = 0,
		Opt_OUTFILE,
		Opt_CHN_MIX,
		Opt_CHN_ACT,
		Opt_RESOL,
		Opt_SPL_FREQ,
		Opt_INTER_FREQ,
		Opt_DURATION,
		Opt_SKIP,
		Opt_DEBUG,
		Opt_EMUL_TYPE,
		Opt_SPEAKER,

		Opt_NBR_ELT
	};

	struct Truc
	{
		Opt				_opt;
		const char *	_opt_name_0;
		int				_nbr_arg_min;
		int				_nbr_arg_max;
	};

	struct FullOpt
	{
		Opt				_opt;
		std::vector <std::string>
							_arg_list;
	};

	typedef	std::vector <FullOpt>	OptList;

	int				collect_options (OptList &opt_list, char *argv [], long opt_len) const;
	const Truc *	find_option (char *argv [], int arg_index) const;

	int				process_opt_song_index (const FullOpt &opt);
	int				process_opt_out_file (const FullOpt &opt);
	int				process_opt_chn_mix (const FullOpt &opt);
	int				process_opt_chn_act (const FullOpt &opt);
	int				process_opt_resol (const FullOpt &opt);
	int				process_opt_spl_freq (const FullOpt &opt);
	int				process_opt_inter_freq (const FullOpt &opt);
	int				process_opt_duration (const FullOpt &opt);
	int				process_opt_skip (const FullOpt &opt);
	int				process_opt_debug (const FullOpt &opt);
	int				process_opt_emul_type (const FullOpt &opt);
	int				process_opt_speaker (const FullOpt &opt);

	static bool		strcmp_ci (const char *s1_0, const char *s2_0);
	static int		conv_int (int &result, const std::string &s);
	static int		conv_double (double &result, const std::string &s);

	bool				_debug_mode_active_flag;
	bool				_port_read_flag;
	std::string		_ay3_filename;
	int				_song_index;			// -1: default song
	std::vector <std::string>
						_out_filename_arr;
	MixConv			_conversion;
	int				_channels;				// bit 0 = A, bit 1 = B, bit 2 = C
	WavResol			_resol;
	double			_sample_freq;			// Hz
	double			_interrupt_freq;		// Hz
	bool				_duration_ay_flag;
	double			_duration;				// s
	double			_skip_time;				// s
	ForceEmul		_emul_hw;
	int				_speaker_filter_quality;	// [0 - 2], -1 = off

	static const Truc
						_opt_table [Opt_NBR_ELT];
	static const double
						_max_sample_freq;
	static const double
						_min_sample_freq;
	static const double
						_max_interrupt_freq;
	static const double
						_min_interrupt_freq;
	static const double
						_max_duration;
	static const double
						_min_duration;
	static const double
						_max_skip_time;
	static const double
						_min_skip_time;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						Parameters (const Parameters &other);
	Parameters &	operator = (const Parameters &other);
	bool				operator == (const Parameters &other);
	bool				operator != (const Parameters &other);

};	// class Parameters



//#include	"Parameters.hpp"



#endif	// Parameters_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
