# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=OLECam - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to OLECam - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "OLECam - Win32 Release" && "$(CFG)" !=\
 "OLECam - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "OLECam.mak" CFG="OLECam - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OLECam - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "OLECam - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "OLECam - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "OLECam - Win32 Release"

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

ALL : "$(OUTDIR)\OLECam.exe" "$(OUTDIR)\OLECam.tlb"

CLEAN : 
	-@erase ".\Release\OLECam.exe"
	-@erase ".\Release\Doc.obj"
	-@erase ".\Release\OLECam.pch"
	-@erase ".\Release\dataobj.obj"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\MainFrm.obj"
	-@erase ".\Release\guids.obj"
	-@erase ".\Release\util.obj"
	-@erase ".\Release\view.obj"
	-@erase ".\Release\OLECam.res"
	-@erase ".\Release\OLECam.obj"
	-@erase ".\Release\OLECam.tlb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/OLECam.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c\
 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/OLECam.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OLECam.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/OLECam.pdb" /machine:I386 /out:"$(OUTDIR)/OLECam.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Doc.obj" \
	"$(INTDIR)/dataobj.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/guids.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/OLECam.obj" \
	"$(INTDIR)/OLECam.res"

"$(OUTDIR)\OLECam.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"

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

ALL : "$(OUTDIR)\OLECam.exe" "$(OUTDIR)\OLECam.bsc" "$(OUTDIR)\OLECam.tlb"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\OLECam.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\OLECam.bsc"
	-@erase ".\Debug\util.sbr"
	-@erase ".\Debug\StdAfx.sbr"
	-@erase ".\Debug\Doc.sbr"
	-@erase ".\Debug\guids.sbr"
	-@erase ".\Debug\dataobj.sbr"
	-@erase ".\Debug\view.sbr"
	-@erase ".\Debug\MainFrm.sbr"
	-@erase ".\Debug\OLECam.sbr"
	-@erase ".\Debug\OLECam.exe"
	-@erase ".\Debug\MainFrm.obj"
	-@erase ".\Debug\util.obj"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\Doc.obj"
	-@erase ".\Debug\guids.obj"
	-@erase ".\Debug\dataobj.obj"
	-@erase ".\Debug\view.obj"
	-@erase ".\Debug\OLECam.res"
	-@erase ".\Debug\OLECam.obj"
	-@erase ".\Debug\OLECam.ilk"
	-@erase ".\Debug\OLECam.pdb"
	-@erase ".\Debug\OLECam.tlb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/OLECam.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/OLECam.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OLECam.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/util.sbr" \
	"$(INTDIR)/StdAfx.sbr" \
	"$(INTDIR)/Doc.sbr" \
	"$(INTDIR)/guids.sbr" \
	"$(INTDIR)/dataobj.sbr" \
	"$(INTDIR)/view.sbr" \
	"$(INTDIR)/MainFrm.sbr" \
	"$(INTDIR)/OLECam.sbr"

"$(OUTDIR)\OLECam.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/OLECam.pdb" /debug /machine:I386 /out:"$(OUTDIR)/OLECam.exe" 
LINK32_OBJS= \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/Doc.obj" \
	"$(INTDIR)/guids.obj" \
	"$(INTDIR)/dataobj.obj" \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/OLECam.obj" \
	"$(INTDIR)/OLECam.res"

"$(OUTDIR)\OLECam.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "OLECam - Win32 Release"
# Name "OLECam - Win32 Debug"

!IF  "$(CFG)" == "OLECam - Win32 Release"

!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "OLECam - Win32 Release"

!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "OLECam - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/OLECam.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\OLECam.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/OLECam.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\OLECam.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\StdAfx.h"\
	".\OLECam.h"\
	".\MainFrm.h"\
	

!IF  "$(CFG)" == "OLECam - Win32 Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"

"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Doc.cpp
DEP_CPP_DOC_C=\
	".\StdAfx.h"\
	".\OLECam.h"\
	".\DataObj.h"\
	".\Doc.h"\
	

!IF  "$(CFG)" == "OLECam - Win32 Release"


"$(INTDIR)\Doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"


"$(INTDIR)\Doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"

"$(INTDIR)\Doc.sbr" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\view.cpp

!IF  "$(CFG)" == "OLECam - Win32 Release"

DEP_CPP_VIEW_=\
	".\StdAfx.h"\
	".\OLECam.h"\
	".\Doc.h"\
	".\View.h"\
	

"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"

DEP_CPP_VIEW_=\
	".\StdAfx.h"\
	".\OLECam.h"\
	".\Doc.h"\
	".\View.h"\
	
NODEP_CPP_VIEW_=\
	".\CSrvView"\
	

"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"

"$(INTDIR)\view.sbr" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dataobj.cpp
DEP_CPP_DATAO=\
	".\StdAfx.h"\
	".\util.h"\
	".\DataObj.h"\
	".\guids.h"\
	

!IF  "$(CFG)" == "OLECam - Win32 Release"


"$(INTDIR)\dataobj.obj" : $(SOURCE) $(DEP_CPP_DATAO) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"


"$(INTDIR)\dataobj.obj" : $(SOURCE) $(DEP_CPP_DATAO) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"

"$(INTDIR)\dataobj.sbr" : $(SOURCE) $(DEP_CPP_DATAO) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\util.cpp

!IF  "$(CFG)" == "OLECam - Win32 Release"

DEP_CPP_UTIL_=\
	".\StdAfx.h"\
	".\util.h"\
	
NODEP_CPP_UTIL_=\
	".\lpszFace"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"

DEP_CPP_UTIL_=\
	".\StdAfx.h"\
	".\util.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"

"$(INTDIR)\util.sbr" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\guids.cpp
DEP_CPP_GUIDS=\
	".\StdAfx.h"\
	".\guids.h"\
	

!IF  "$(CFG)" == "OLECam - Win32 Release"


"$(INTDIR)\guids.obj" : $(SOURCE) $(DEP_CPP_GUIDS) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"


"$(INTDIR)\guids.obj" : $(SOURCE) $(DEP_CPP_GUIDS) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"

"$(INTDIR)\guids.sbr" : $(SOURCE) $(DEP_CPP_GUIDS) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OLECam.rc
DEP_RSC_OLECA=\
	".\res\OLECam.ico"\
	".\res\Doc.ico"\
	".\res\OLECam.rc2"\
	

!IF  "$(CFG)" == "OLECam - Win32 Release"


"$(INTDIR)\OLECam.res" : $(SOURCE) $(DEP_RSC_OLECA) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/OLECam.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"


"$(INTDIR)\OLECam.res" : $(SOURCE) $(DEP_RSC_OLECA) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/OLECam.res" /i "Debug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OLECam.cpp

!IF  "$(CFG)" == "OLECam - Win32 Release"

DEP_CPP_OLECAM=\
	".\StdAfx.h"\
	".\OLECam.h"\
	".\MainFrm.h"\
	".\Doc.h"\
	".\View.h"\
	
NODEP_CPP_OLECAM=\
	".\;"\
	

"$(INTDIR)\OLECam.obj" : $(SOURCE) $(DEP_CPP_OLECAM) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"

DEP_CPP_OLECAM=\
	".\StdAfx.h"\
	".\OLECam.h"\
	".\MainFrm.h"\
	".\Doc.h"\
	".\View.h"\
	

"$(INTDIR)\OLECam.obj" : $(SOURCE) $(DEP_CPP_OLECAM) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"

"$(INTDIR)\OLECam.sbr" : $(SOURCE) $(DEP_CPP_OLECAM) "$(INTDIR)"\
 "$(INTDIR)\OLECam.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OLECam.odl

!IF  "$(CFG)" == "OLECam - Win32 Release"


"$(OUTDIR)\OLECam.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/OLECam.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "OLECam - Win32 Debug"


"$(OUTDIR)\OLECam.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/OLECam.tlb" /win32 $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
