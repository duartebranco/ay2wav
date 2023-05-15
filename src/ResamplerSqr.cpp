/*****************************************************************************

        ResamplerSqr.cpp
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

#include	"ResamplerSqr.h"

#include	<cassert>
#include	<cmath>
#include	<cstring>

namespace std { }



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ResamplerSqr::ResamplerSqr ()
:	_buf ()
,	_capacity (0)
,	_buf_len (0)
,	_rate_frac (0)
,	_write_pos (0)
,	_nbr_spl_buf (0)
,	_pos_frac (0)
,	_cur_val (0)
{
	if (! _blep_ok_flag)
	{
		build_tables ();
	}
}



/*
==============================================================================
Name: set_resampling_rate
Description:
	Set the resampling rate.
Input parameters:
	- rate: Rate of the resampling (output / input). Currently this resampler
		is only a downsampler, so rate has to be in the range [0 ; 1]
Throws: Nothing
==============================================================================
*/

void	ResamplerSqr::set_resampling_rate (double rate)
{
	assert (rate > 0);
	assert (rate < 1);

	using namespace std;

	_rate_frac = static_cast <UInt32> (floor (rate * 4294967296.0));	// 2^32
}



void	ResamplerSqr::set_buf_len (long len)
{
	assert (len > 0);

	_capacity = len;
	_buf_len = _capacity + PHASE_LEN;	// For the working zone within the circular buffer
	_buf.resize (_buf_len + PHASE_LEN - 1);	// For the post-unrolling
}



void	ResamplerSqr::clear_buffers ()
{
	_write_pos = 0;
	_nbr_spl_buf = 0;
	_pos_frac = 0;
	_cur_val = 0;
}



/*
==============================================================================
Name: process_input_samples
Description:
	Feeds the resampler with sample data. Depending how much room is available,
	all data could not be processed. The resampler returns the number of
	effectively processed samples.
Input parameters:
	- data_ptr: Pointer on input data.
	- nbr_spl: Maximum number of samples to process. > 0.
Returns: Number of processed samples. >= 0.
Throws: Nothing
==============================================================================
*/

long	ResamplerSqr::process_input_samples (const Int16 data_ptr [], long nbr_spl)
{
	assert (_capacity > 0);
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	long				ay3_pos = 0;
	while (ay3_pos < nbr_spl && _nbr_spl_buf < _capacity)
	{
		const int		val = data_ptr [ay3_pos];
		if (val != _cur_val)
		{
			const int		dif = val - _cur_val;
			_buf [_write_pos] += dif;
			_cur_val = val;

			// Band-limits the step
			const int		lerp_bits = 32 - NBR_PHASES_L2;
			const int		phase = _pos_frac >> lerp_bits;
			const long		lerp_mask = (1L << lerp_bits) - 1;
			const long		lerp_val = _pos_frac & lerp_mask;
			const float		lerp = lerp_val * (NBR_PHASES / 4294967296.0f);	// 2^32
			const float *	phase_data = _blep_phase [phase];
			const float *	lerp_data = _blep_lerp [phase];
			for (int k = 0; k < PHASE_LEN; ++k)
			{
				const float		step = phase_data [k] + lerp * lerp_data [k];
				const float		step_scaled = step * dif;
				_buf [_write_pos + k] += step_scaled;
			}
		}

		++ ay3_pos;

		_pos_frac += _rate_frac;
		if (_pos_frac < _rate_frac)
		{
			++ _write_pos;
			++ _nbr_spl_buf;

			// We reached the end of the buffer ?
			if (_write_pos >= _buf_len)
			{
				using namespace std;

				_write_pos -= _buf_len;
				memcpy (
					&_buf [0],
					&_buf [_buf_len],
					(PHASE_LEN - 1) * sizeof (_buf [0])
				);
			}

			// Sets the first new sample to the current value
			_buf [_write_pos] += _cur_val;

			// The last sample of the "work-in-progress" zone is new, so we have
			// to clear it before mixing anything in it.
			_buf [_write_pos + (PHASE_LEN - 1)] = 0;
		}
	}

	return (ay3_pos);
}



/*
==============================================================================
Name: get_available_output_samples
Description:
	Finds the number of available output samples.
Returns: Number of output samples. >= 0.
Throws: Nothing
==============================================================================
*/

long	ResamplerSqr::get_available_output_samples () const
{
	assert (_capacity > 0);

	return (_nbr_spl_buf);
}



/*
==============================================================================
Name: pop_output_samples
Description:
	Extract output samples.
Input parameters:
	- data_ptr: Location where to store the extracted data.
	- nbr_spl: Number of output samples to extract. Must be less or equal to
		the value returned by get_available_output_samples().
Throws: Nothing
==============================================================================
*/

void	ResamplerSqr::pop_output_samples (float data_ptr [], long nbr_spl)
{
	assert (_capacity > 0);
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	const long		read_pos = (_write_pos - _nbr_spl_buf + _buf_len) % _buf_len;
	const long		available = _buf_len - read_pos;
	long				work_len = nbr_spl;
	if (work_len > available)
	{
		work_len = available;
	}
	if (work_len > _nbr_spl_buf)
	{
		work_len = _nbr_spl_buf;
	}

	using namespace std;

	memcpy (data_ptr, &_buf [read_pos], work_len * sizeof (data_ptr [0]));
	_nbr_spl_buf -= work_len;

	if (nbr_spl > work_len)
	{
		pop_output_samples (data_ptr + work_len, nbr_spl - work_len);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ResamplerSqr::build_tables ()
{
	for (int phase = 0; phase < NBR_PHASES; ++phase)
	{
		for (int pos = 0; pos < PHASE_LEN; ++pos)
		{
			const int		l_pos = (pos + 1) * NBR_PHASES - phase;
			const float		cur = _blep_linear [l_pos    ];
			const float		nxt = _blep_linear [l_pos - 1];
			_blep_phase [phase] [pos] = cur;
			_blep_lerp [phase] [pos] = nxt - cur;
		}
	}

	_blep_ok_flag = true;
}



/*
	% Matlab script for minimum-phase Band-Limited stEP (minBLEP)

	l = 16;	% Phase length
	p = 32;	% Number of phases
	o = 16;	% Oversampling for cepstrum calculation
	x = fir1 (l*p-1, 0.9/p);
	[y, ym] = rceps([x zeros(1, l*p*(o-1))]);
	x1 = ym(1:l*p);
	x2 = filter(1, [1 -1], x1);
	blep = x2 - 1;

	% Display
	w = linspace (0, 1, 2001);
	h = 20 * log10 (abs (freqz (x, [1], w * pi)));
	subplot (2, 1, 1); plot (w * p,  h); axis ([0   p   -100 3   ]); grid on;
	subplot (2, 1, 2); plot (       x2); axis ([0 l*p-1    0 1.25]); grid on;

	% Average group delay : 1.81 samples
*/
const float	ResamplerSqr::_blep_linear [NBR_PHASES * PHASE_LEN + 1] =
{
	-0.99992660288172f, -0.99981561577959f, -0.99966556974798f, -0.99946553712269f,
	-0.99920824636533f, -0.99888245480242f, -0.99847896860637f, -0.99798581335699f,
	-0.99739225257197f, -0.99668509003802f, -0.99585176576183f, -0.99487734993192f,
	-0.99374719994839f, -0.99244447525103f, -0.99095256327333f, -0.98925296007842f,
	-0.98732750104909f, -0.98515644655985f, -0.98272049961565f, -0.97999899405143f,
	-0.97697172277360f, -0.97361721733324f, -0.96991446428052f, -0.96584131478369f,
	-0.96137616386459f, -0.95649650683789f, -0.95118060044286f, -0.94540613019535f,
	-0.93915182235478f, -0.93239616520656f, -0.92511894172122f, -0.91729997715011f,
	-0.90892060148364f, -0.89996243868593f, -0.89040883605013f, -0.88024372074705f,
	-0.86945301979456f, -0.85802358154069f, -0.84594457337519f, -0.83320643483550f,
	-0.81980222136255f, -0.80572655603210f, -0.79097690394626f, -0.77555251773160f,
	-0.75945565649989f, -0.74269054332732f, -0.72526455179219f, -0.70718718801809f,
	-0.68847124913739f, -0.66913181824567f, -0.64918737632814f, -0.62865878484962f,
	-0.60757033135543f, -0.58594868589417f, -0.56382388013309f, -0.54122824579337f,
	-0.51819734421179f, -0.49476890272227f, -0.47098370780432f, -0.44688454226995f,
	-0.42251703708770f, -0.39792859555584f, -0.37316918862399f, -0.34829025182659f,
	-0.32334541810751f, -0.29838938936537f, -0.27347861858528f, -0.24867017232805f,
	-0.22402238092007f, -0.19959370630468f, -0.17544336936602f, -0.15163022390820f,
	-0.12821335708397f, -0.10525096215731f, -0.08280090729948f, -0.06091960672528f,
	-0.03966256526211f, -0.01908326103461f,  0.00076631609735f,  0.01983675243180f,
	 0.03808066265949f,  0.05545373928103f,  0.07191419221606f,  0.08742376225400f,
	 0.10194715226994f,  0.11545300852152f,  0.12791334276612f,  0.13930447585324f,
	 0.14960643953509f,  0.15880386858750f,  0.16688536790905f,  0.17384434220316f,
	 0.17967832195181f,  0.18438973038528f,  0.18798517217879f,  0.19047614429686f,
	 0.19187829302663f,  0.19221207185289f,  0.19150196545750f,  0.18977708989707f,
	 0.18707037636614f,  0.18341910733174f,  0.17886405630633f,  0.17344996127652f,
	 0.16722462657138f,  0.16023934415088f,  0.15254796982102f,  0.14420730455721f,
	 0.13527615438704f,  0.12581567781970f,  0.11588843179011f,  0.10555868597871f,
	 0.09489145545692f,  0.08395278510624f,  0.07280877552505f,  0.06152584832575f,
	 0.05016977877253f,  0.03880595687153f,  0.02749844039918f,  0.01631022298442f,
	 0.00530231484864f, -0.00546597786242f, -0.01593794268251f, -0.02605916823779f,
	-0.03577840084108f, -0.04504723226916f, -0.05382091426934f, -0.06205801530921f,
	-0.06972118140831f, -0.07677675147267f, -0.08319545725933f, -0.08895199355296f,
	-0.09402565714331f, -0.09839987373771f, -0.10206277866940f, -0.10500670208247f,
	-0.10722869116140f, -0.10872995109030f, -0.10951630522380f, -0.10959758816150f,
	-0.10898804211530f, -0.10770566310057f, -0.10577253814401f, -0.10321415208833f,
	-0.10005967522811f, -0.09634124048039f, -0.09209419030416f, -0.08735633091707f,
	-0.08216814327334f, -0.07657201752686f, -0.07061243130027f, -0.06433516866348f,
	-0.05778747238538f, -0.05101725654837f, -0.04407324425096f, -0.03700418624927f,
	-0.02985899637712f, -0.02268598675749f, -0.01553300934454f, -0.00844671361774f,
	-0.00147269867553f,  0.00534520282711f,  0.01196475741555f,  0.01834601458937f,
	 0.02445111639923f,  0.03024493979385f,  0.03569487338992f,  0.04077140982944f,
	 0.04544788311008f,  0.04970100714425f,  0.05351057230754f,  0.05685993092805f,
	 0.05973565447890f,  0.06212796730078f,  0.06403036455059f,  0.06543999316184f,
	 0.06635722859277f,  0.06678600129676f,  0.06673333157503f,  0.06620960306932f,
	 0.06522805958119f,  0.06380503186955f,  0.06195940411300f,  0.05971280227660f,
	 0.05708903775126f,  0.05411426364983f,  0.05081640039447f,  0.04722526381075f,
	 0.04337197621782f,  0.03928907078916f,  0.03500989404110f,  0.03056869433784f,
	 0.02600002511989f,  0.02133882784996f,  0.01661984597454f,  0.01187771129616f,
	 0.00714637580570f,  0.00245920694240f, -0.00215155870234f, -0.00665470764595f,
	-0.01102057010911f, -0.01522089483740f, -0.01922933793724f, -0.02302131320085f,
	-0.02657444211445f, -0.02986837332480f, -0.03288518923423f, -0.03560919204189f,
	-0.03802726656041f, -0.04012863391337f, -0.04190517248625f, -0.04335114093399f,
	-0.04446345826275f, -0.04524139609562f, -0.04568681732303f, -0.04580383703549f,
	-0.04559902017372f, -0.04508101305893f, -0.04426070399702f, -0.04315083099633f,
	-0.04176611214442f, -0.04012283648702f, -0.03823897067386f, -0.03613373825598f,
	-0.03382770672071f, -0.03134235835266f, -0.02870016073421f, -0.02592413276921f,
	-0.02303790358558f, -0.02006528001779f, -0.01703030151355f, -0.01395681713002f,
	-0.01086854429647f, -0.00778866199161f, -0.00473987856757f, -0.00174404463673f,
	 0.00117776757375f,  0.00400562969186f,  0.00672066396550f,  0.00930538396827f,
	 0.01174358326253f,  0.01402064652774f,  0.01612341520861f,  0.01804046506619f,
	 0.01976194624368f,  0.02127982664765f,  0.02258770752343f,  0.02368103509181f,
	 0.02455689442038f,  0.02521419185518f,  0.02565342876230f,  0.02587685543059f,
	 0.02588822461996f,  0.02569291698226f,  0.02529767517130f,  0.02471070303712f,
	 0.02394138411900f,  0.02300035995576f,  0.02189923902783f,  0.02065066031643f,
	 0.01926799794234f,  0.01776541403110f,  0.01615756171250f,  0.01445962915498f,
	 0.01268704254296f,  0.01085550360888f,  0.00898069603776f,  0.00707832140972f,
	 0.00516381493877f,  0.00325238620657f,  0.00135875006115f, -0.00050282318215f,
	-0.00231875874411f, -0.00407611629806f, -0.00576282264875f, -0.00736759978448f,
	-0.00888017886115f, -0.01029121591941f, -0.01159248465446f, -0.01277677643666f,
	-0.01383806856163f, -0.01477140602888f, -0.01557304435979f, -0.01624031401693f,
	-0.01677174044794f, -0.01716689478726f, -0.01742649502363f, -0.01755224611844f,
	-0.01754692426568f, -0.01741420699130f, -0.01715874024352f, -0.01678595800815f,
	-0.01630213259793f, -0.01571418551449f, -0.01502972444076f, -0.01425685020102f,
	-0.01340418609141f, -0.01248068625857f, -0.01149566166036f, -0.01045859245973f,
	-0.00937915168021f, -0.00826702152290f, -0.00713191449852f, -0.00598339410791f,
	-0.00483089538857f, -0.00368355344412f, -0.00255022823024f, -0.00143934612719f,
	-0.00035893365900f,  0.00068352474831f,  0.00168104286069f,  0.00262725939723f,
	 0.00351638582348f,  0.00434331987939f,  0.00510358653144f,  0.00579343839074f,
	 0.00640978829593f,  0.00695029402174f,  0.00741327956179f,  0.00779780208061f,
	 0.00810356147328f,  0.00833095132838f,  0.00848096036728f,  0.00855521222807f,
	 0.00855586354081f,  0.00848563628819f,  0.00834771439135f,  0.00814576866157f,
	 0.00788385041881f,  0.00756640721416f,  0.00719817232778f,  0.00678417186414f,
	 0.00632961274267f,  0.00583988566980f,  0.00532045699494f,  0.00477687274953f,
	 0.00421465807952f,  0.00363932401503f,  0.00305627389149f,  0.00247081080195f,
	 0.00188804848056f,  0.00131291784787f,  0.00075008253299f,  0.00020394697669f,
	-0.00032141918280f, -0.00082226632838f, -0.00129522709731f, -0.00173729209946f,
	-0.00214585854136f, -0.00251869865033f, -0.00285399872222f, -0.00315032466919f,
	-0.00340665504957f, -0.00362234521699f, -0.00379715350864f, -0.00393120115064f,
	-0.00402498801966f, -0.00407934526978f, -0.00409543964773f, -0.00407472040175f,
	-0.00401891634748f, -0.00392998280164f, -0.00381009738170f, -0.00366161147023f,
	-0.00348704680544f, -0.00328905056888f, -0.00307038962846f, -0.00283390502436f,
	-0.00258250037396f, -0.00231909428768f, -0.00204660445565f, -0.00176790218407f,
	-0.00148579815064f, -0.00120300503302f, -0.00092212936776f, -0.00064564377122f,
	-0.00037588332066f, -0.00011502320119f,  0.00013492546670f,  0.00037213689751f,
	 0.00059497720807f,  0.00080202332829f,  0.00099206762043f,  0.00116412846693f,
	 0.00131744703241f,  0.00145148578265f,  0.00156591692981f,  0.00166061222021f,
	 0.00173562914625f,  0.00179119930087f,  0.00182771854574f,  0.00184573742963f,
	 0.00184595498181f,  0.00182920754260f,  0.00179646038876f,  0.00174878937033f,
	 0.00168736682799f,  0.00161343624602f,  0.00152829645866f,  0.00143327635103f,
	 0.00132972517065f,  0.00121899376475f,  0.00110243390809f,  0.00098138473162f,
	 0.00085717561847f,  0.00073111025777f,  0.00060446560545f,  0.00047846887388f,
	 0.00035429204242f,  0.00023302605408f,  0.00011567844226f,  0.00000315359734f,
	-0.00010373940157f, -0.00020429708962f, -0.00029790538309f, -0.00038404657456f,
	-0.00046228355117f, -0.00053227187357f, -0.00059374996962f, -0.00064655670332f,
	-0.00069062294998f, -0.00072598606246f, -0.00075277338890f, -0.00077120608893f,
	-0.00078157202190f, -0.00078422204376f, -0.00077954092474f, -0.00076794743261f,
	-0.00074987436980f, -0.00072577943642f, -0.00069613561584f, -0.00066144733719f,
	-0.00062224128312f, -0.00057907607267f, -0.00053252428151f, -0.00048317146874f,
	-0.00043159242358f, -0.00037834827617f, -0.00032396942435f, -0.00026896214336f,
	-0.00021380547392f, -0.00015896703548f, -0.00010490652970f, -0.00005208749102f,
	-0.00000097312050f,  0.00004797775416f,  0.00009432680104f,  0.00013767220630f,
	 0.00017766459338f,  0.00021401917600f,  0.00024651556360f,  0.00027499635627f,
	 0.00029934925740f,  0.00031950076442f,  0.00033539547282f,  0.00034700290407f,
	 0.00035430774669f,  0.00035733637247f,  0.00035615305850f,  0.00035089286055f,
	 0.00034174402418f,  0.00032896946360f,  0.00031286327783f,  0.00029376136370f,
	 0.00027198311456f,  0.00024785285246f,  0.00022164958046f,  0.00019365941829f,
	 0.00016414071251f,  0.00013340269370f,  0.00010176573745f,  0.00006964061712f,
	 0.00003745582096f,  0.00000572076744f, -0.00002508487106f, -0.00005444924270f,
	-0.00008195295895f, -0.00010717527521f, -0.00012980870361f, -0.00014951826767f,
	-0.00016605521031f, -0.00017909076841f, -0.00018836832377f, -0.00019354439753f,
	-0.00019440831065f, -0.00019072963710f, -0.00018252998373f, -0.00016988972301f,
	-0.00015322730799f, -0.00013301125527f, -0.00011002844420f, -0.00008499335342f,
	-0.00005884931254f, -0.00003231725288f, -0.00000630819680f,  0.00001855577226f,
	 0.00004137490260f,  0.00006152291822f,  0.00007795299705f,  0.00008994380453f,
	 0.00009632085321f,  0.00009652606426f,  0.00008970575371f,  0.00007617322204f,
	 0.00005614996615f,  0.00003168087458f,  0.00000464725063f, -0.00002058829463f,
	-0.00004052069523f, -0.00004804035226f, -0.00003878788736f,  0.00000145819633f,
	 0.0f	// Added for the phase/lerp decomposition
};



float	ResamplerSqr::_blep_phase [NBR_PHASES] [PHASE_LEN];
float	ResamplerSqr::_blep_lerp [NBR_PHASES] [PHASE_LEN];
bool	ResamplerSqr::_blep_ok_flag = false;



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
