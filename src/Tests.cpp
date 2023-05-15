/*****************************************************************************

        Tests.cpp
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

#include	"Ay38910.h"
#include	"Ay38910CbInterface.h"
#include	"Tests.h"
#include	"Z80CbInterface.h"
#include	"Z80Wrapper.h"

#include	<cassert>
#include	<cstdio>
#include	<cstring>

namespace std { }



class DummyZ80Cb
:	public Z80CbInterface
{
public:
	DummyZ80Cb ()
	:	_mem ()
	{
		using namespace std;
		memset (&_mem [0], 0, sizeof (_mem));
		_mem [0] = 0x00;	// LOOP: NOP
		_mem [1] = 0x18;	// JR LOOP
		_mem [2] = 0xFD;
	}
	~DummyZ80Cb ()
	{
		// Nothing
	}

protected:
	virtual UByte	do_read_z80_mem (UInt16 addr)
	{
		printf ("Reading #%04X: #%02X\n", static_cast <int> (addr), static_cast <int> (_mem [addr]));
		return (_mem [addr]);
	}

	virtual void	do_write_z80_mem (UInt16 addr, UByte data)
	{
		_mem [addr] = data;
		printf ("Writing #%04X: #%02X\n", static_cast <int> (addr), static_cast <int> (_mem [addr]));
	}

	virtual UByte	do_read_z80_port (UInt16 port)
	{
		return (0);
	}

	virtual void	do_write_z80_port (UInt16 port, UByte data)
	{
		// Nothing
	}

private:
	UByte				_mem [65536];
};



class DummyAy3Cb
:	public Ay38910CbInterface
{
protected:
	virtual void	do_signal_ay3_state_about_to_change () { }

	virtual void	do_write_ay3_port_a (UByte data) { }
	virtual UByte	do_read_ay3_port_a () { return (0); }

	virtual void	do_write_ay3_port_b (UByte data) { }
	virtual UByte	do_read_ay3_port_b () { return (0); }
};



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	Tests::test_z80 ()
{
	int				ret_val = 0;

	DummyZ80Cb		z80_cb;
	Z80Wrapper		z80_w;

	z80_w.set_callback (z80_cb);
	for (long i = 0; i < 10; ++i)
	{
		const long		nbr_cycles = z80_w.exec_single (0);
		using namespace std;
		printf ("Step %ld: %ld cycles\n", i, nbr_cycles);
	}
	printf ("\n");

	return (ret_val);
}



int	Tests::test_ay3 ()
{
	int				ret_val = 0;

	DummyAy3Cb		ay_cb;
	Ay38910			ay;
	ay.set_callback (ay_cb);
	ay.reset ();

	const long		nbr_spl = 300;
	Int16				chn_data [3] [nbr_spl];
	Int16 *			chn_ptr_arr [3] = { chn_data [0], chn_data [1], chn_data [2] };

#if 1

	ay.latch_address (Ay38910::Reg_ENABLE);
	ay.write_data (0xEA);
	ay.latch_address (Ay38910::Reg_A_VOL);
	ay.write_data (0x10);
	ay.latch_address (Ay38910::Reg_B_VOL);
	ay.write_data (0x0F);

	ay.latch_address (Ay38910::Reg_A_PER_LSB);
	ay.write_data (5);
	ay.latch_address (Ay38910::Reg_A_PER_MSB);
	ay.write_data (0);

	ay.latch_address (Ay38910::Reg_N_PER);
	ay.write_data (1);

	ay.latch_address (Ay38910::Reg_E_PER_LSB);
	ay.write_data (1);
	ay.latch_address (Ay38910::Reg_E_PER_MSB);
	ay.write_data (0);
	ay.latch_address (Ay38910::Reg_E_SHAPE);
	ay.write_data (0x08);

#else

	ay.latch_address (Ay38910::Reg_ENABLE);
	ay.write_data (0xFE);
	ay.latch_address (Ay38910::Reg_A_VOL);
	ay.write_data (0x0F);
	ay.latch_address (Ay38910::Reg_A_PER_LSB);
	ay.write_data (10);
	ay.latch_address (Ay38910::Reg_A_PER_MSB);
	ay.write_data (1);

#endif

	ay.process_block (chn_ptr_arr, nbr_spl);

	for (long i = 0; i < nbr_spl; ++i)
	{
		printf ("%5d %5d %5d\n", chn_data [0] [i], chn_data [1] [i], chn_data [2] [i]);
	}
	printf ("\n");

	return (ret_val);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
