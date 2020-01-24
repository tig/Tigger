# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=msntick - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to msntick - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "msntick - Win32 Release" && "$(CFG)" !=\
 "msntick - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "msntick.mak" CFG="msntick - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msntick - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "msntick - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "msntick - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\msntick.exe"

CLEAN : 
	-@erase "$(INTDIR)\msntick.obj"
	-@erase "$(INTDIR)\msntick.pch"
	-@erase "$(INTDIR)\msntick.res"
	-@erase "$(INTDIR)\msntickDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\ticker.obj"
	-@erase "$(OUTDIR)\msntick.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/msntick.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/msntick.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/msntick.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/msntick.pdb" /machine:I386 /out:"$(OUTDIR)/msntick.exe" 
LINK32_OBJS= \
	"$(INTDIR)\msntick.obj" \
	"$(INTDIR)\msntick.res" \
	"$(INTDIR)\msntickDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\ticker.obj"

"$(OUTDIR)\msntick.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "msntick - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\msntick.exe"

CLEAN : 
	-@erase "$(INTDIR)\msntick.obj"
	-@erase "$(INTDIR)\msntick.pch"
	-@erase "$(INTDIR)\msntick.res"
	-@erase "$(INTDIR)\msntickDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\ticker.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\msntick.exe"
	-@erase "$(OUTDIR)\msntick.ilk"
	-@erase "$(OUTDIR)\msntick.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/msntick.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/msntick.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/msntick.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/msntick.pdb" /debug /machine:I386 /out:"$(OUTDIR)/msntick.exe" 
LINK32_OBJS= \
	"$(INTDIR)\msntick.obj" \
	"$(INTDIR)\msntick.res" \
	"$(INTDIR)\msntickDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\ticker.obj"

"$(OUTDIR)\msntick.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "msntick - Win32 Release"
# Name "msntick - Win32 Debug"

!IF  "$(CFG)" == "msntick - Win32 Release"

!ELSEIF  "$(CFG)" == "msntick - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "msntick - Win32 Release"

!ELSEIF  "$(CFG)" == "msntick - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msntick.cpp
DEP_CPP_MSNTI=\
	".\msntick.h"\
	".\msntickDlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\msntick.obj" : $(SOURCE) $(DEP_CPP_MSNTI) "$(INTDIR)"\
 "$(INTDIR)\msntick.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\msntickDlg.cpp
DEP_CPP_MSNTIC=\
	".\msntick.h"\
	".\msntickDlg.h"\
	".\StdAfx.h"\
	".\ticker.h"\
	

"$(INTDIR)\msntickDlg.obj" : $(SOURCE) $(DEP_CPP_MSNTIC) "$(INTDIR)"\
 "$(INTDIR)\msntick.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "msntick - Win32 Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/msntick.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\msntick.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "msntick - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/msntick.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\msntick.pch" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msntick.rc
DEP_RSC_MSNTICK=\
	".\res\msntick.ico"\
	".\res\msntick.rc2"\
	

"$(INTDIR)\msntick.res" : $(SOURCE) $(DEP_RSC_MSNTICK) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ticker.cpp
DEP_CPP_TICKE=\
	".\StdAfx.h"\
	".\ticker.h"\
	

"$(INTDIR)\ticker.obj" : $(SOURCE) $(DEP_CPP_TICKE) "$(INTDIR)"\
 "$(INTDIR)\msntick.pch"


# End Source File
# End Target
# End Project
################################################################################
################################################################################
# Section OLE Controls
# 	{3472D900-5A27-11CF-8B11-00AA00C00903}
# End Section
################################################################################
################################################################################
# Section msntick : {315DF7E0-5A27-11CF-8B11-00AA00C00903}
# 	2:5:Class:CTicker
# 	2:10:HeaderFile:ticker.h
# 	2:8:ImplFile:ticker.cpp
# End Section
################################################################################
################################################################################
# Section msntick : {3472D900-5A27-11CF-8B11-00AA00C00903}
# 	0:8:Button.h:G:\Source\msntick\Button.h
# 	0:10:Button.cpp:G:\Source\msntick\Button.cpp
# 	2:21:DefaultSinkHeaderFile:ticker.h
# 	2:16:DefaultSinkClass:CTicker
# End Section
################################################################################
