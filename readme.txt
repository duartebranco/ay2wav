==============================================================================

        AY2Wav - A .AY to .WAV file converter
        v1.10

        Copyright (c) 2006-2007 Laurent de Soras
        Included Z80 emulator by Marat Fayzullin, (c) 1994-2002

==============================================================================



Contents:

1. Legal
2. What is AY2Wav ?
3. Using AY2Wav
4. Compilation
5. History
6. Contact



1. Legal
--------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Check the file license.txt to get full information about the license.



2. What is AY2Wav ?
-------------------

.AY files are music files from Amstrad CPC and Sinclair Spectrum computers.
They contain music data as well as the code required to play it back on the
original computer. This program emulates the Zilog Z80 microprocessor and the
General Instruments AY-3-8910/8912 (or Yamaha YM2149) Programmable Sound
Generator, in order to render an audio sample file in the Wav format.

Currently, AY2wav does not play "beeper" tunes from Spectrum computers, only
tunes using the PSG processor.

The program features a high quality resampling system, something which is
missing in most AY players or CPC/ZX emulators.



3. Using AY2Wav
---------------

AY2Wav runs from the command line. Full C++ source code is provided, as well
as a compiled version for Microsoft Windows (Win32).

Command:

   ay2wav [options] <ay_filename>

Options:

   -s <song_number>
      Selects the song number, ranging from 1 to N, where N is the number of
      songs stored in the .AY file.
      Default is the "main" song, indicated in the .AY file.

   -o <out_filename> [<out_filename2> [<out_filename3>]]
      Selects a filename for the output. One can have multiple filenames to
      split multichannel streams into mono files.
      Default is 1 file based on the input filename, except when rendering
      with -c separate, where there are 3 mono files, numbered.

   -c mono | stereo | separated
      Selects the internal channel mixing for the AY-3-8910.
      - mono mixes the ABC channels into one single channel
      - stereo sends A to left channel, C to right channel and B is mixed on
         the center (half-volume).
      - separated doesn't mix anything and dumps the AY-3-8910 outputs
         individually.
      Default is stereo.

   -p [A][B][C]
      Selects AY-3-8910 channels to activate; others are muted. This option
      does not applies when dumping sound to separated channels.
      Default is ABC.

   -r 8 | 16 | 24 | 32f
      Selects the output resolution: 8, 16 or 24 bits integer, 32f for 32-bit
      floating point (scale: 0 dB = +/-1.0).
      Default is 16 bits.

   -f <sample_rate>
      Selects the output sample rate, in Hz. Valid range: [4000 ; 125000].
      Default is 44100 Hz.

   -i <interrupt_rate>
      Selects the interrupt rate, in Hz. This affects the speed of the song.
      Valid range: [1 ; 300]
      Default is 50 Hz.

   -d <duration_s> | ay
      Selects the playback duration, in seconds, or read it in the .AY file,
      if it is configured. Valid range: [0 ; 36000]
      Default is 90 seconds.

   -k <duration_s>
      Skips the beginning of the song before recording it. The duration is in
      seconds. Then it will record sound during a fixed amount of time, except
      if the -d option has been selected with parameter "ay", whereas the song
      always terminates at the same timestamp (thus reducing the recording
      duration).
      Default is 0 second.

   -b off | on | noin
      Activate or deactivate the debugging mode. At the end of the processing
      it will list addresses which have been read without being written or
      initialised first. It will also list non-AY3 accessed ports. noin mode
      prohibits reading of ports, because some AY file players do not support
      this feature.
      Default is off.

   -e auto | zx | cpc | zcpc
      Indicates the machine type for the emulation. Because AY files were
      originally designed for Spectrum computers only, they haven't any
      information about the hardware they were actually designed for or ripped
      from. This option can force a specific hardware to be emulated.
      - auto performs an automatic detection
      - zx forces emulation of a Sinclair Spectrum
      - cpc is for an Amstrad CPC
      - zcpc is for early musics ripped from Amstrad CPC and patched to use
      the Spectrum hardware. However this trick detunes them. This option
      fixes this problem.
      Default is auto.

   -a off | 1 | 2 | 3
      Activates or deactivates the Amstrad CPC speaker emulation. 3 sets the
      best fidelity. Note: the emulation is correct only for 44.1 kHz output.
      Default is off.



4. Compilation and testing
--------------------------

Drop the following files into your project or makefile :
*.cpp
*.h
*.hpp
z80/*.h
z80/*.cpp

AY2Wav may be compiled in two versions: release and debug. Debug version
has checks that could slow down the code. Define NDEBUG to set the Release
mode. For example, the command line to compile AY2Wav on GCC would look like:

Debug mode:
g++ -o ay2wav_debug.exe -Wall *.cpp z80/*.c

Release mode:
g++ -o ay2wav_release.exe -DNDEBUG -O3 *.cpp z80/*.c

Important: AY2Wav is portable and should compile and work on any system.
However you should define a few symbols in order to accomodate with your
system specific features. Check and modify def.h, as well as z80/Z80.h in
order to specify the endianess of your computer (LSB_FIRST: little endian,
MSB_FIRST: big endian).

It is possible to isolate the emulator only, for example to build a real-
time AY file player, or to include it in a plug-in. The main interface is
located in Emulator.h.



5. History
----------

v1.10 (2007.04.04)
    - Added AY3 port reading ability.
    - Added a debug mode checking if memory has been fully initialised and
      if only AY3 ports addressed.
    - Added an option to specify emulated hardware, so it can play correctly
      early AY files ripped from CPC, when AY players weren't supporting CPC
      hardware.
    - Added Amstrad CPC speaker emulation.
    - Use of a more recent version of the Z80 emulator by Marat Fayzullin
    - Fixed a waveform discontinuity problem in the AY3 emulation.
    - Fixed problem with song selection.
    - Fixed envelope initialisation error.

v1.00 (2006.07.15)
    - Initial release.



6. Contact
----------

Please address any comment, bug report or flame to:

Laurent de Soras
laurent.de.soras@club-internet.fr
http://ldesoras.free.fr

