/*****************************************************************************

        DebugInfo.cpp
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

#include	"DebugInfo.h"

#include	<cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DebugInfo::DebugInfo ()
:	_mem_rw_flag_arr ()
,	_mem_ro_pc_arr ()
,	_port_out_pc_arr ()
,	_port_in_pc_arr ()
,	_debug_flag (false)
,	_port_in_flag (true)
,	_one_mem_ro_flag (false)
,	_one_port_out_flag (false)
,	_one_port_in_flag (false)
{
	clear ();
}



void	DebugInfo::clear ()
{
	for (long addr = 0; addr < 65536L; ++addr)
	{
		_mem_rw_flag_arr [addr] = false;
		_mem_ro_pc_arr [addr] = -1;
	}

	for (int port = 0; port < 256; ++port)
	{
		_port_in_pc_arr [port] = -1;
		_port_out_pc_arr [port] = -1;
	}

	_one_mem_ro_flag = false;
	_one_port_out_flag = false;
	_one_port_in_flag = false;
}



void	DebugInfo::set_debug_mode (bool flag)
{
	_debug_flag = flag;
}



bool	DebugInfo::is_debug_mode_active () const
{
	return (_debug_flag);
}



void	DebugInfo::allow_port_reading (bool flag)
{
	_port_in_flag = flag;
}



bool	DebugInfo::is_port_reading_allowed () const
{
	return (_port_in_flag);
}



void	DebugInfo::set_addr_as_init (UInt16 addr)
{
	assert (! is_addr_read_wo_init (addr));

	_mem_rw_flag_arr [addr] = true;
}



bool	DebugInfo::is_addr_init (UInt16 addr) const
{
	return (_mem_rw_flag_arr [addr]);
}



void	DebugInfo::set_addr_as_read_wo_init (UInt16 addr, UInt16 pc)
{
	assert (! is_addr_init (addr));

	if (_mem_ro_pc_arr [addr] < 0)
	{
		_mem_ro_pc_arr [addr] = pc;
		_one_mem_ro_flag = true;
	}
}



bool	DebugInfo::is_any_addr_read_wo_init () const
{
	return (_one_mem_ro_flag);
}



bool	DebugInfo::is_addr_read_wo_init (UInt16 addr) const
{
	return (_mem_ro_pc_arr [addr] >= 0);
}



UInt16	DebugInfo::get_pc_for_addr_read_wo_init (UInt16 addr) const
{
	assert (is_addr_read_wo_init (addr));

	return (static_cast <UInt16> (_mem_ro_pc_arr [addr]));
}



void	DebugInfo::set_port_as_written (UByte port, UInt16 pc)
{
	if (_port_out_pc_arr [port] < 0)
	{
		_port_out_pc_arr [port] = pc;
		_one_port_out_flag = true;
	}
}



bool	DebugInfo::is_any_port_written () const
{
	return (_one_port_out_flag);
}



bool	DebugInfo::is_port_written (UByte port) const
{
	return (_port_out_pc_arr [port] >= 0);
}



UInt16	DebugInfo::get_pc_for_port_written (UByte port) const
{
	assert (is_port_written (port));

	return (static_cast <UInt16> (_port_out_pc_arr [port]));
}



void	DebugInfo::set_port_as_read (UByte port, UInt16 pc)
{
	if (_port_in_pc_arr [port] < 0)
	{
		_port_in_pc_arr [port] = pc;
		_one_port_in_flag = true;
	}
}



bool	DebugInfo::is_any_port_read () const
{
	return (_one_port_in_flag);
}



bool	DebugInfo::is_port_read (UByte port) const
{
	return (_port_in_pc_arr [port] >= 0);
}



UInt16	DebugInfo::get_pc_for_port_read (UByte port) const
{
	assert (is_port_read (port));

	return (static_cast <UInt16> (_port_in_pc_arr [port]));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
