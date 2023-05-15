/*****************************************************************************

        ConvolverFFT.cpp
        Copyright (c) 2002 Ohm Force

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

#include	"ConvolverFFT.h"
#include	"fnc.h"

#include	<cassert>
#include	<cstring>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ConvolverFFT::ConvolverFFT ()
:	_fft_aptr ()
,	_impulse_freq ()
,	_input_buf ()
,	_temp_buf ()
,	_output_buf ()
,	_impulse_len (0)
,	_fft_len (0)
,	_block_len (0)
,	_rem_len (0)
{
}



ConvolverFFT::ConvolverFFT (long nbr_spl, const DataType impulse_ptr [], float vol)
:	_fft_aptr ()
,	_impulse_freq ()
,	_input_buf ()
,	_temp_buf ()
,	_output_buf ()
,	_impulse_len (0)
,	_fft_len (0)
,	_block_len (0)
,	_rem_len (0)
{
	set_impulse (nbr_spl, impulse_ptr, vol);
}



bool	ConvolverFFT::is_valid () const
{
	return (_fft_len > 0);
}



/*
==============================================================================
Name : set_impulse
Description :
	Set a new impulse. Object reallocations are done only if the FFT length
	had to change since the last time. Buffer is not cleared if FFT size
	doesn't change.
	Buffers addresses should be invalidated by client application.
Input parameters :
	- nbr_spl : Impulse length, in samples. > 1.
	- impulse_ptr : Pointer on the impulse. Impulse data is copied into the
		class data.
Throws : std::vector exception, only if FFT length changed.
==============================================================================
*/

void	ConvolverFFT::set_impulse (long nbr_spl, const DataType impulse_ptr [], float vol)
{
	assert (nbr_spl > 1);
	assert (impulse_ptr != 0);
	assert (vol != 0);

	_impulse_len = nbr_spl;
	const long		half_len_p2 = get_next_pow2 (_impulse_len - 1);
	const long		old_fft_len = _fft_len;
	_fft_len = 2L << half_len_p2;
	assert (_fft_len > _impulse_len);
	assert (_fft_len >= 2 * (_impulse_len - 1));

	// Reallocations, if required
	if (_fft_len != old_fft_len)
	{
		_fft_aptr = FFTRealAPtr (new FftType (_fft_len));
		_impulse_freq.resize (_fft_len);
		_input_buf.resize (_fft_len);
		_temp_buf.resize (_fft_len);
		_output_buf.resize (_fft_len);

		_block_len = _fft_len - _impulse_len + 1;
		_rem_len = _fft_len - _block_len;

		clear_buffers ();
	}

	// Copy impulse and pad it with 0 to the right
	using namespace std;
	memcpy (
		&_temp_buf [0],
		impulse_ptr,
		_impulse_len * sizeof (_temp_buf [0])
	);
	memset (
		&_temp_buf [_impulse_len],
		0,
		(_fft_len - _impulse_len) * sizeof (_temp_buf [0])
	);

	// Frequency domain
	_fft_aptr->do_fft (&_impulse_freq [0], &_temp_buf [0]);
	_fft_aptr->rescale (&_impulse_freq [0]);

	for (long pos = 0; pos < _fft_len; ++pos)
	{
		_impulse_freq [pos] *= vol;
	}

	assert (is_valid ());
}



long	ConvolverFFT::get_block_len () const
{
	assert (is_valid ());

	return (_block_len);
}



/*
==============================================================================
Name : get_input_buffer
Description :
	Once the impulse set, get the input buffer for data. The address remains
	constant between two impulse changes of the same length.
Returns: Address of the buffer.
Throws : Nothing
==============================================================================
*/

ConvolverFFT::DataType *	ConvolverFFT::get_input_buffer () const
{
	assert (is_valid ());

	return (const_cast <DataType *> (&_input_buf [_rem_len]));
}



/*
==============================================================================
Name : get_output_buffer
Description :
	Once the impulse set, get the output buffer for data. The address remains
	constant between two impulse changes of the same length.
Returns: Address of the buffer.
Throws : Nothing
==============================================================================
*/

const ConvolverFFT::DataType *	ConvolverFFT::get_output_buffer () const
{
	assert (is_valid ());

	return (&_output_buf [_rem_len]);
}



/*
==============================================================================
Name : process
Description :
	First, user should fill the public input buffer with block_len samples.
	In return, block_len samples are calculated in the output buffer.
Throws : Nothing
==============================================================================
*/

void	ConvolverFFT::process ()
{
	assert (is_valid ());

	// Data in frequency domain
	_fft_aptr->do_fft (&_temp_buf [0], &_input_buf [0]);

	// Save old data
	using namespace std;
	memcpy (
		&_input_buf [0],
		&_input_buf [_block_len],
		_rem_len * sizeof (_input_buf [0])
	);

	// Multiply signals in frequency domain
	const long		half_fft_len = _fft_len >> 1;
	for (long bin = 1; bin < half_fft_len; ++bin)
	{
		const long		bin_imag = bin + half_fft_len;

		const float		h_real = _impulse_freq [bin];
		const float		h_imag = _impulse_freq [bin_imag];

		const float		x_real = _temp_buf [bin];
		const float		x_imag = _temp_buf [bin_imag];

		const float		y_real = x_real * h_real - x_imag * h_imag;
		const float		y_imag = x_imag * h_real + x_real * h_imag;

		_temp_buf [bin] = y_real;
		_temp_buf [bin_imag] = y_imag;
	}
	_temp_buf [0] *= _impulse_freq [0];
	_temp_buf [half_fft_len] *= _impulse_freq [half_fft_len];

	// Back to time domain
	_fft_aptr->do_ifft (&_temp_buf [0], &_output_buf [0]);
}



void	ConvolverFFT::clear_buffers ()
{
	using namespace std;

	const long		len = _input_buf.size ();
	memset (&_input_buf [0], 0, len * sizeof (_input_buf [0]));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
