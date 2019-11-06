# Microsoft Developer Studio Project File - Name="id3lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=id3lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "id3lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "id3lib.mak" CFG="id3lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "id3lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "id3lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "id3lib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "id3lib - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "id3lib - Win32 Release"
# Name "id3lib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\adler32.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\compress.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\crc32.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\deflate.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\gzio.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_dll_wrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_error.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_field.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_field_binary.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_field_integer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_field_string_ascii.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_field_string_unicode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_frame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_frame_parse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_frame_render.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_header.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_header_frame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_header_tag.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_int28.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_misc_support.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_tag.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_tag_file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_tag_find.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_tag_parse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_tag_parse_lyrics3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_tag_parse_v1.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_tag_render.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_tag_sync.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\infblock.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\infcodes.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\inffast.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\inflate.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\infutil.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\trees.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\uncompr.c
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\zutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\deflate.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_error.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_externals.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_field.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_frame.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_header.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_header_frame.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_header_tag.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_int28.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_misc_support.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_tag.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_types.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3_version.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\id3lib.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\infblock.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\infcodes.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\inffast.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\inffixed.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\inftrees.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\infutil.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\trees.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\zconf.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\zlib.h
# End Source File
# Begin Source File

SOURCE=..\..\ID3Lib\source\library\zutil.h
# End Source File
# End Group
# End Target
# End Project
