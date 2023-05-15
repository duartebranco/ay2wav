/*****************************************************************************

        DebugInfo.h
        Copyright (c) 2007 Laurent de Soras

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



#if ! defined (DebugInfo_HEADER_INCLUDED)
#define	DebugInfo_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"def.h"



class DebugInfo
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						DebugInfo ();
	virtual			~DebugInfo () {}

	void				clear ();

	void				set_debug_mode (bool flag);
	bool				is_debug_mode_active () const;

	void				allow_port_reading (bool flag);
	bool				is_port_reading_allowed () const;

	void				set_addr_as_init (UInt16 addr);
	bool				is_addr_init (UInt16 addr) const;

	void				set_addr_as_read_wo_init (UInt16 addr, UInt16 pc);
	bool				is_any_addr_read_wo_init () const;
	bool				is_addr_read_wo_init (UInt16 addr) const;
	UInt16			get_pc_for_addr_read_wo_init (UInt16 addr) const;

	void				set_port_as_written (UByte port, UInt16 pc);
	bool				is_any_port_written () const;
	bool				is_port_written (UByte port) const;
	UInt16			get_pc_for_port_written (UByte port) const;

	void				set_port_as_read (UByte port, UInt16 pc);
	bool				is_any_port_read () const;
	bool				is_port_read (UByte port) const;
	UInt16			get_pc_for_port_read (UByte port) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	bool	MemFlagArr [65536L];
	typedef	long	MemAdrArr [65536L];
	typedef	long	PortAdrArr [256];

	MemFlagArr		_mem_rw_flag_arr;	// Read addresses with write first.
	MemAdrArr		_mem_ro_pc_arr;	// PC values for read addresses without having been initialised. -1 = no read yet
	PortAdrArr		_port_out_pc_arr;	// PC values for written ports
	PortAdrArr		_port_in_pc_arr;	// PC values for read ports

	bool				_debug_flag;
	bool				_port_in_flag;

	bool				_one_mem_ro_flag;
	bool				_one_port_out_flag;
	bool				_one_port_in_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						DebugInfo (const DebugInfo &other);
	DebugInfo &		operator = (const DebugInfo &other);
	bool				operator == (const DebugInfo &other);
	bool				operator != (const DebugInfo &other);

};	// class DebugInfo



//#include	"DebugInfo.hpp"



#endif	// DebugInfo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
