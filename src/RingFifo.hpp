/*****************************************************************************

        RingFifo.hpp
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



#if defined (RingFifo_CURRENT_CODEHEADER)
	#error Recursive inclusion of RingFifo code header.
#endif
#define	RingFifo_CURRENT_CODEHEADER

#if ! defined (RingFifo_CODEHEADER_INCLUDED)
#define	RingFifo_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
RingFifo <T>::RingFifo ()
:	_data ()
,	_write_pos (0)
,	_nbr_elt (0)
{
	// Nothing
}



// Stored elements will be lost
template <class T>
void	RingFifo <T>::set_size (long sz)
{
	assert (sz > 0);

	_data.resize (sz);
	clear ();
}



template <class T>
void	RingFifo <T>::clear ()
{
	_write_pos = 0;
	_nbr_elt = 0;
}



template <class T>
long	RingFifo <T>::get_nbr_elt () const
{
	assert (_data.size () > 0);

	return (_nbr_elt);
}



template <class T>
long	RingFifo <T>::get_room () const
{
	assert (_data.size () > 0);

	const long		room = _data.size () - _nbr_elt;
	assert (room >= 0);

	return (room);
}



template <class T>
void	RingFifo <T>::push (const EltType data_ptr [], long len)
{
	assert (_data.size () > 0);
	assert (data_ptr != 0);
	assert (len > 0);
	assert (len <= get_room ());

	const long		end_pos = _write_pos + len;
	const long		capacity = _data.size ();
	long				seg_len = len;
	if (end_pos >= capacity)
	{
		seg_len = capacity - _write_pos;
	}

	for (long pos = 0; pos < seg_len; ++pos)
	{
		_data [_write_pos + pos] = data_ptr [pos];
	}
	_write_pos = (_write_pos + seg_len) % capacity;

	if (seg_len < len)
	{
		push (data_ptr + seg_len, len - seg_len);
	}
}



template <class T>
void	RingFifo <T>::pop (EltType data_ptr [], long len)
{
	assert (_data.size () > 0);
	assert (data_ptr != 0);
	assert (len > 0);
	assert (len <= get_nbr_elt ());

	const long		capacity = _data.size ();
	const long		read_pos = get_read_pos ();
	const long		end_pos = read_pos + len;
	long				seg_len = len;
	if (end_pos >= capacity)
	{
		seg_len = capacity - read_pos;
	}

	for (long pos = 0; pos < seg_len; ++pos)
	{
		data_ptr [pos] = _data [read_pos + pos];
	}
	_nbr_elt -= seg_len;

	if (seg_len < len)
	{
		pop (data_ptr + seg_len, len - seg_len);
	}
}



template <class T>
void	RingFifo <T>::get_writing_info (long &max_len, EltType * &data_ptr)
{
	assert (&max_len != 0);
	assert (&data_ptr != 0);

	data_ptr = &_data [_write_pos];
	max_len = _data.size () - _write_pos;
	const long		available = get_room ();
	if (max_len > available)
	{
		max_len = available;
	}
}



template <class T>
void	RingFifo <T>::notify_writing (long len)
{
	assert (len > 0);
	assert (len <= get_room ());
	assert (len <= static_cast <long> (_data.size ()) - _write_pos);

	const long		capacity = _data.size ();
	_write_pos = (_write_pos + len) % capacity;
	_nbr_elt += len;
}



template <class T>
void	RingFifo <T>::get_reading_info (long &max_len, const EltType * &data_ptr) const
{
	assert (&max_len != 0);
	assert (&data_ptr != 0);

	const long		read_pos = get_read_pos ();
	data_ptr = &_data [read_pos];
	max_len = _data.size () - read_pos;
	const long		available = get_nbr_elt ();
	if (max_len > available)
	{
		max_len = available;
	}
}



template <class T>
void	RingFifo <T>::notify_reading (long len)
{
	assert (len > 0);
	assert (len <= get_nbr_elt ());
	assert (len <= static_cast <long> (_data.size ()) - get_read_pos ());

	_nbr_elt -= len;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
long	RingFifo <T>::get_read_pos () const
{
	const long		capacity = _data.size ();
	const long		read_pos = (_write_pos - _nbr_elt + capacity) % capacity;

	return (read_pos);
}



#endif	// RingFifo_CODEHEADER_INCLUDED

#undef RingFifo_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
