/*****************************************************************************

        RingFifo.h
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



#if ! defined (RingFifo_HEADER_INCLUDED)
#define	RingFifo_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<vector>



template <class T>
class RingFifo
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	T	EltType;

						RingFifo ();
	virtual			~RingFifo () {}

	void				set_size (long sz);
	void				clear ();
	long				get_nbr_elt () const;
	long				get_room () const;
	void				push (const EltType data_ptr [], long len);
	void				pop (EltType data_ptr [], long len);

	void				get_writing_info (long &max_len, EltType * &data_ptr);
	void				notify_writing (long len);

	void				get_reading_info (long &max_len, const EltType * &data_ptr) const;
	void				notify_reading (long len);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	std::vector <EltType>	VectType;

	long				get_read_pos () const;

	VectType			_data;
	long				_write_pos;
	long				_nbr_elt;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						RingFifo (const RingFifo &other);
	RingFifo &		operator = (const RingFifo &other);
	bool				operator == (const RingFifo &other);
	bool				operator != (const RingFifo &other);

};	// class RingFifo



#include	"RingFifo.hpp"



#endif	// RingFifo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
