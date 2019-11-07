# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (PPC) Dynamic-Link Library" 0x0702
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=XRTTick - Win32 (PPC) Debug
!MESSAGE No configuration specified.  Defaulting to XRTTick - Win32 (PPC)\
 Debug.
!ENDIF 

!IF "$(CFG)" != "XRTTick - Win32 Release" && "$(CFG)" !=\
 "XRTTick - Win32 Debug" && "$(CFG)" != "XRTTick - Win32 (PPC) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "XRTTick.mak" CFG="XRTTick - Win32 (PPC) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XRTTick - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XRTTick - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XRTTick - Win32 (PPC) Debug" (based on\
 "Win32 (PPC) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "XRTTick - Win32 Debug"

!IF  "$(CFG)" == "XRTTick - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP BASE Target_Ext "ocx"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# PROP Target_Ext "ocx"
OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\XRTTick.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Release\XRTTick.lib"
	-@erase ".\Release\OFFSTAGE.OBJ"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\XRTTick.obj"
	-@erase ".\Release\xrttippg.obj"
	-@erase ".\Release\TICKCTL.OBJ"
	-@erase ".\Release\XRTTick.res"
	-@erase ".\Release\XRTTick.tlb"
	-@erase ".\Release\XRTTick.exp"
	-@erase ".\Release\regsvr32.trg"
	-@erase ".\Release\XRTTick.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/XRTTick.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/XRTTick.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/XRTTick.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/XRTTick.pdb" /machine:I386 /def:".\XRTTick.def"\
 /out:"$(OUTDIR)/XRTTick.ocx" /implib:"$(OUTDIR)/XRTTick.lib" 
DEF_FILE= \
	".\XRTTick.def"
LINK32_OBJS= \
	"$(INTDIR)/OFFSTAGE.OBJ" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/XRTTick.obj" \
	"$(INTDIR)/xrttippg.obj" \
	"$(INTDIR)/TICKCTL.OBJ" \
	"$(INTDIR)/XRTTick.res"

"$(OUTDIR)\XRTTick.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Release
TargetPath=.\Release\XRTTick.ocx
InputPath=.\Release\XRTTick.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "XRTTick - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP BASE Target_Ext "ocx"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# PROP Target_Ext "ocx"
OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\XRTTick.ocx" "$(OUTDIR)\XRTTick.bsc" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\XRTTick.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\XRTTick.bsc"
	-@erase ".\Debug\OFFSTAGE.SBR"
	-@erase ".\Debug\TICKCTL.SBR"
	-@erase ".\Debug\StdAfx.sbr"
	-@erase ".\Debug\XRTTick.sbr"
	-@erase ".\Debug\xrttippg.sbr"
	-@erase ".\Debug\XRTTick.ilk"
	-@erase ".\Debug\xrttippg.obj"
	-@erase ".\Debug\OFFSTAGE.OBJ"
	-@erase ".\Debug\TICKCTL.OBJ"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\XRTTick.obj"
	-@erase ".\Debug\XRTTick.res"
	-@erase ".\Debug\XRTTick.tlb"
	-@erase ".\Debug\XRTTick.lib"
	-@erase ".\Debug\XRTTick.exp"
	-@erase ".\Debug\XRTTick.pdb"
	-@erase ".\Debug\regsvr32.trg"
	-@erase ".\Debug\XRTTick.ocx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W4 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W4 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/XRTTick.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/XRTTick.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/XRTTick.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/OFFSTAGE.SBR" \
	"$(INTDIR)/TICKCTL.SBR" \
	"$(INTDIR)/StdAfx.sbr" \
	"$(INTDIR)/XRTTick.sbr" \
	"$(INTDIR)/xrttippg.sbr"

"$(OUTDIR)\XRTTick.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/XRTTick.pdb" /debug /machine:I386 /def:".\XRTTick.def"\
 /out:"$(OUTDIR)/XRTTick.ocx" /implib:"$(OUTDIR)/XRTTick.lib" 
DEF_FILE= \
	".\XRTTick.def"
LINK32_OBJS= \
	"$(INTDIR)/xrttippg.obj" \
	"$(INTDIR)/OFFSTAGE.OBJ" \
	"$(INTDIR)/TICKCTL.OBJ" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/XRTTick.obj" \
	"$(INTDIR)/XRTTick.res"

"$(OUTDIR)\XRTTick.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
OutDir=.\Debug
TargetPath=.\Debug\XRTTick.ocx
InputPath=.\Debug\XRTTick.ocx
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "XRTTick - Win32 (PPC) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "XRTTick_"
# PROP BASE Intermediate_Dir "XRTTick_"
# PROP BASE Target_Dir ""
# PROP BASE Target_Ext "ocx"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PPCDbg"
# PROP Intermediate_Dir "PPCDbg"
# PROP Target_Dir ""
# PROP Target_Ext "ocx"
OUTDIR=.\PPCDbg
INTDIR=.\PPCDbg
# Begin Custom Macros
OutDir=.\PPCDbg
# End Custom Macros

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

ALL : "$(OUTDIR)\XRTTick.ocx" "$(OUTDIR)\regsvr32.trg"

CLEAN : 
	-@erase ".\PPCDbg\vc40.pdb"
	-@erase ".\PPCDbg\XRTTick.pch"
	-@erase ".\PPCDbg\XRTTick.lib"
	-@erase ".\PPCDbg\OFFSTAGE.OBJ"
	-@erase ".\PPCDbg\XRTTick.obj"
	-@erase ".\PPCDbg\StdAfx.obj"
	-@erase ".\PPCDbg\TICKCTL.OBJ"
	-@erase ".\PPCDbg\xrttippg.obj"
	-@erase ".\PPCDbg\XRTTick.res"
	-@erase ".\PPCDbg\XRTTick.tlb"
	-@erase ".\PPCDbg\XRTTick.exp"
	-@erase ".\PPCDbg\XRTTick.pdb"
	-@erase ".\PPCDbg\regsvr32.trg"
	-@erase ".\PPCDbg\XRTTick.ocx"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /PPC32
# ADD MTL /nologo /D "_DEBUG" /PPC32
MTL_PROJ=/nologo /D "_DEBUG" /PPC32 
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /WX /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/XRTTick.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PPCDbg/
CPP_SBRS=

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/XRTTick.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/XRTTick.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:PPC
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:PPC
LINK32_FLAGS=/nologo /subsystem:windows /dll /pdb:"$(OUTDIR)/XRTTick.pdb"\
 /debug /machine:PPC /def:".\XRTTick.def" /out:"$(OUTDIR)/XRTTick.ocx"\
 /implib:"$(OUTDIR)/XRTTick.lib" 
DEF_FILE= \
	".\XRTTick.def"
LINK32_OBJS= \
	"$(INTDIR)/OFFSTAGE.OBJ" \
	"$(INTDIR)/XRTTick.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/TICKCTL.OBJ" \
	"$(INTDIR)/xrttippg.obj" \
	"$(INTDIR)/XRTTick.res"

"$(OUTDIR)\XRTTick.ocx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Registering OLE control...
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ENDIF 

################################################################################
# Begin Target

# Name "XRTTick - Win32 Release"
# Name "XRTTick - Win32 Debug"
# Name "XRTTick - Win32 (PPC) Debug"

!IF  "$(CFG)" == "XRTTick - Win32 Release"

!ELSEIF  "$(CFG)" == "XRTTick - Win32 Debug"

!ELSEIF  "$(CFG)" == "XRTTick - Win32 (PPC) Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "XRTTick - Win32 Release"

!ELSEIF  "$(CFG)" == "XRTTick - Win32 Debug"

!ELSEIF  "$(CFG)" == "XRTTick - Win32 (PPC) Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "XRTTick - Win32 Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"
   $(CPP) /nologo /MD /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/XRTTick.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "XRTTick - Win32 Debug"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W4 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/XRTTick.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\XRTTick.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "XRTTick - Win32 (PPC) Debug"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	
# ADD CPP /Yc"stdafx.h"
BuildCmds= \
	$(CPP) /nologo /MDd /W3 /WX /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/XRTTick.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\XRTTick.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\XRTTick.cpp

!IF  "$(CFG)" == "XRTTick - Win32 Release"

DEP_CPP_XRTTI=\
	".\StdAfx.h"\
	".\XRTTick.h"\
	

"$(INTDIR)\XRTTick.obj" : $(SOURCE) $(DEP_CPP_XRTTI) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"


!ELSEIF  "$(CFG)" == "XRTTick - Win32 Debug"

DEP_CPP_XRTTI=\
	".\StdAfx.h"\
	".\XRTTick.h"\
	

"$(INTDIR)\XRTTick.obj" : $(SOURCE) $(DEP_CPP_XRTTI) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"

"$(INTDIR)\XRTTick.sbr" : $(SOURCE) $(DEP_CPP_XRTTI) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"


!ELSEIF  "$(CFG)" == "XRTTick - Win32 (PPC) Debug"

DEP_CPP_XRTTI=\
	".\StdAfx.h"\
	".\XRTTick.h"\
	

"$(INTDIR)\XRTTick.obj" : $(SOURCE) $(DEP_CPP_XRTTI) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\XRTTick.def

!IF  "$(CFG)" == "XRTTick - Win32 Release"

!ELSEIF  "$(CFG)" == "XRTTick - Win32 Debug"

!ELSEIF  "$(CFG)" == "XRTTick - Win32 (PPC) Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\XRTTick.rc

!IF  "$(CFG)" == "XRTTick - Win32 Release"

DEP_RSC_XRTTIC=\
	".\tickectl.bmp"\
	".\ticker.ico"\
	".\cursor1.cur"\
	".\xrttick.rc2"\
	
NODEP_RSC_XRTTIC=\
	".\Release\XRTTick.tlb"\
	

"$(INTDIR)\XRTTick.res" : $(SOURCE) $(DEP_RSC_XRTTIC) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/XRTTick.res" /i "Release" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "XRTTick - Win32 Debug"

DEP_RSC_XRTTIC=\
	".\tickectl.bmp"\
	".\ticker.ico"\
	".\cursor1.cur"\
	".\xrttick.rc2"\
	".\Debug\XRTTick.tlb"\
	

"$(INTDIR)\XRTTick.res" : $(SOURCE) $(DEP_RSC_XRTTIC) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/XRTTick.res" /i "Debug" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "XRTTick - Win32 (PPC) Debug"

DEP_RSC_XRTTIC=\
	".\tickectl.bmp"\
	".\ticker.ico"\
	".\cursor1.cur"\
	".\xrttick.rc2"\
	
NODEP_RSC_XRTTIC=\
	".\PPCDbg\XRTTick.tlb"\
	

"$(INTDIR)\XRTTick.res" : $(SOURCE) $(DEP_RSC_XRTTIC) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.tlb"
   $(RSC) /l 0x409 /fo"$(INTDIR)/XRTTick.res" /i "PPCDbg" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\XRTTick.odl

!IF  "$(CFG)" == "XRTTick - Win32 Release"


"$(OUTDIR)\XRTTick.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "NDEBUG" /tlb "$(OUTDIR)/XRTTick.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "XRTTick - Win32 Debug"


"$(OUTDIR)\XRTTick.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/XRTTick.tlb" /win32 $(SOURCE)


!ELSEIF  "$(CFG)" == "XRTTick - Win32 (PPC) Debug"

"$(OUTDIR)\XRTTick.tlb" : $(SOURCE) "$(OUTDIR)"
   $(MTL) /nologo /D "_DEBUG" /tlb "$(OUTDIR)/XRTTick.tlb" /PPC32 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TICKCTL.CPP

!IF  "$(CFG)" == "XRTTick - Win32 Release"

DEP_CPP_TICKC=\
	".\StdAfx.h"\
	".\XRTTick.h"\
	".\TICKCTL.H"\
	".\xrttippg.h"\
	".\OFFSTAGE.H"\
	".\WOSAXRT.H"\
	
NODEP_CPP_TICKC=\
	".\DataObjppg.h"\
	

"$(INTDIR)\TICKCTL.OBJ" : $(SOURCE) $(DEP_CPP_TICKC) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"


!ELSEIF  "$(CFG)" == "XRTTick - Win32 Debug"

DEP_CPP_TICKC=\
	".\StdAfx.h"\
	".\XRTTick.h"\
	".\TICKCTL.H"\
	".\xrttippg.h"\
	".\OFFSTAGE.H"\
	".\WOSAXRT.H"\
	
NODEP_CPP_TICKC=\
	".\CTickCtrl"\
	

"$(INTDIR)\TICKCTL.OBJ" : $(SOURCE) $(DEP_CPP_TICKC) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"

"$(INTDIR)\TICKCTL.SBR" : $(SOURCE) $(DEP_CPP_TICKC) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"


!ELSEIF  "$(CFG)" == "XRTTick - Win32 (PPC) Debug"

DEP_CPP_TICKC=\
	".\StdAfx.h"\
	".\XRTTick.h"\
	".\TICKCTL.H"\
	".\xrttippg.h"\
	".\OFFSTAGE.H"\
	

"$(INTDIR)\TICKCTL.OBJ" : $(SOURCE) $(DEP_CPP_TICKC) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OFFSTAGE.CPP

!IF  "$(CFG)" == "XRTTick - Win32 Release"

DEP_CPP_OFFST=\
	".\StdAfx.h"\
	".\XRTTick.h"\
	".\TICKCTL.H"\
	".\OFFSTAGE.H"\
	".\WOSAXRT.H"\
	

"$(INTDIR)\OFFSTAGE.OBJ" : $(SOURCE) $(DEP_CPP_OFFST) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"


!ELSEIF  "$(CFG)" == "XRTTick - Win32 Debug"

DEP_CPP_OFFST=\
	".\StdAfx.h"\
	".\XRTTick.h"\
	".\TICKCTL.H"\
	".\OFFSTAGE.H"\
	".\WOSAXRT.H"\
	

"$(INTDIR)\OFFSTAGE.OBJ" : $(SOURCE) $(DEP_CPP_OFFST) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"

"$(INTDIR)\OFFSTAGE.SBR" : $(SOURCE) $(DEP_CPP_OFFST) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"


!ELSEIF  "$(CFG)" == "XRTTick - Win32 (PPC) Debug"

DEP_CPP_OFFST=\
	".\StdAfx.h"\
	".\XRTTick.h"\
	".\TICKCTL.H"\
	".\OFFSTAGE.H"\
	

"$(INTDIR)\OFFSTAGE.OBJ" : $(SOURCE) $(DEP_CPP_OFFST) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xrttippg.cpp

!IF  "$(CFG)" == "XRTTick - Win32 Release"

DEP_CPP_XRTTIP=\
	".\StdAfx.h"\
	".\XRTTick.h"\
	".\xrttippg.h"\
	

"$(INTDIR)\xrttippg.obj" : $(SOURCE) $(DEP_CPP_XRTTIP) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"


!ELSEIF  "$(CFG)" == "XRTTick - Win32 Debug"

DEP_CPP_XRTTIP=\
	".\StdAfx.h"\
	".\XRTTick.h"\
	".\xrttippg.h"\
	
NODEP_CPP_XRTTIP=\
	".\COlePropertyPage"\
	

"$(INTDIR)\xrttippg.obj" : $(SOURCE) $(DEP_CPP_XRTTIP) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"

"$(INTDIR)\xrttippg.sbr" : $(SOURCE) $(DEP_CPP_XRTTIP) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"


!ELSEIF  "$(CFG)" == "XRTTick - Win32 (PPC) Debug"

DEP_CPP_XRTTIP=\
	".\StdAfx.h"\
	".\XRTTick.h"\
	".\xrttippg.h"\
	

"$(INTDIR)\xrttippg.obj" : $(SOURCE) $(DEP_CPP_XRTTIP) "$(INTDIR)"\
 "$(INTDIR)\XRTTick.pch"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
