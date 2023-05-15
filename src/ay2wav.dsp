# Microsoft Developer Studio Project File - Name="ay2wav" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ay2wav - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ay2wav.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ay2wav.mak" CFG="ay2wav - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ay2wav - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ay2wav - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ay2wav - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GR /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "ay2wav - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ay2wav - Win32 Release"
# Name "ay2wav - Win32 Debug"
# Begin Group "z80"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\z80\Codes.h
# End Source File
# Begin Source File

SOURCE=.\z80\CodesCB.h
# End Source File
# Begin Source File

SOURCE=.\z80\CodesED.h
# End Source File
# Begin Source File

SOURCE=.\z80\CodesXCB.h
# End Source File
# Begin Source File

SOURCE=.\z80\CodesXX.h
# End Source File
# Begin Source File

SOURCE=.\z80\Tables.h
# End Source File
# Begin Source File

SOURCE=.\z80\Z80.c
# End Source File
# Begin Source File

SOURCE=.\z80\Z80.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AudioBuf.h
# End Source File
# Begin Source File

SOURCE=.\AudioBufArray.h
# End Source File
# Begin Source File

SOURCE=.\Ay2Wav.cpp
# End Source File
# Begin Source File

SOURCE=.\Ay2Wav.h
# End Source File
# Begin Source File

SOURCE=.\Ay38910.cpp
# End Source File
# Begin Source File

SOURCE=.\Ay38910.h
# End Source File
# Begin Source File

SOURCE=.\Ay38910CbInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\Ay38910CbInterface.h
# End Source File
# Begin Source File

SOURCE=.\AyFileReader.cpp
# End Source File
# Begin Source File

SOURCE=.\AyFileReader.h
# End Source File
# Begin Source File

SOURCE=.\ConvolverFFT.cpp
# End Source File
# Begin Source File

SOURCE=.\ConvolverFFT.h
# End Source File
# Begin Source File

SOURCE=.\DcKiller.cpp
# End Source File
# Begin Source File

SOURCE=.\DcKiller.h
# End Source File
# Begin Source File

SOURCE=.\DebugInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugInfo.h
# End Source File
# Begin Source File

SOURCE=.\def.h
# End Source File
# Begin Source File

SOURCE=.\DynArray.h
# End Source File
# Begin Source File

SOURCE=.\DynArray.hpp
# End Source File
# Begin Source File

SOURCE=.\Emulator.cpp
# End Source File
# Begin Source File

SOURCE=.\Emulator.h
# End Source File
# Begin Source File

SOURCE=.\FFTReal.h
# End Source File
# Begin Source File

SOURCE=.\FFTReal.hpp
# End Source File
# Begin Source File

SOURCE=.\fnc.h
# End Source File
# Begin Source File

SOURCE=.\fnc.hpp
# End Source File
# Begin Source File

SOURCE=.\Intel8255.cpp
# End Source File
# Begin Source File

SOURCE=.\Intel8255.h
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\MixConv.h
# End Source File
# Begin Source File

SOURCE=.\OscSinCos.h
# End Source File
# Begin Source File

SOURCE=.\OscSinCos.hpp
# End Source File
# Begin Source File

SOURCE=.\Parameters.cpp
# End Source File
# Begin Source File

SOURCE=.\Parameters.h
# End Source File
# Begin Source File

SOURCE=.\ResamplerSqr.cpp
# End Source File
# Begin Source File

SOURCE=.\ResamplerSqr.h
# End Source File
# Begin Source File

SOURCE=.\RingFifo.h
# End Source File
# Begin Source File

SOURCE=.\RingFifo.hpp
# End Source File
# Begin Source File

SOURCE=.\SpeakerAmp.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeakerAmp.h
# End Source File
# Begin Source File

SOURCE=.\SpeakerFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeakerFilter.h
# End Source File
# Begin Source File

SOURCE=.\Tests.cpp
# End Source File
# Begin Source File

SOURCE=.\Tests.h
# End Source File
# Begin Source File

SOURCE=.\WavResol.h
# End Source File
# Begin Source File

SOURCE=.\WavWritter.cpp
# End Source File
# Begin Source File

SOURCE=.\WavWritter.h
# End Source File
# Begin Source File

SOURCE=.\Z80CbInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\Z80CbInterface.h
# End Source File
# Begin Source File

SOURCE=.\Z80Wrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\Z80Wrapper.h
# End Source File
# End Target
# End Project
