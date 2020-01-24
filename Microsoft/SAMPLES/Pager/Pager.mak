# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Pager - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Pager - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Pager - Win32 Release" && "$(CFG)" != "Pager - Win32 Debug" &&\
 "$(CFG)" != "Pager - Win32 Unicode Release" && "$(CFG)" !=\
 "Pager - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Pager.mak" CFG="Pager - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Pager - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Pager - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Pager - Win32 Unicode Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Pager - Win32 Unicode Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "Pager - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "Pager - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Pager.dll" "$(OUTDIR)\regsvr32.trg" ".\Pager.tlb"

CLEAN : 
	-@erase "$(INTDIR)\PageObj.obj"
	-@erase "$(INTDIR)\Pager.obj"
	-@erase "$(INTDIR)\Pager.pch"
	-@erase "$(INTDIR)\Pager.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\Pager.dll"
	-@erase "$(OUTDIR)\Pager.exp"
	-@erase "$(OUTDIR)\Pager.lib"
	-@erase "$(OUTDIR)\regsvr32.trg"
	-@erase ".\Pager.h"
	-@erase ".\Pager.tlb"
	-@erase ".\Pager_i.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_WINDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/Pager.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Pager.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Pager.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/Pager.pdb" /machine:I386 /def:".\Pager.def"\
 /out:"$(OUTDIR)/Pager.dll" /implib:"$(OUTDIR)/Pager.lib" 
DEF_FILE= \
	".\Pager.def"
LINK32_OBJS= \
	"$(INTDIR)\PageObj.obj" \
	"$(INTDIR)\Pager.obj" \
	"$(INTDIR)\Pager.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\Pager.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build
OutDir=.\Release
TargetPath=.\Release\Pager.dll
InputPath=.\Release\Pager.dll
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Pager - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Pager.dll" "$(OUTDIR)\Pager.bsc" "$(OUTDIR)\regsvr32.trg"\
 ".\Pager.tlb"

CLEAN : 
	-@erase "$(INTDIR)\PageObj.obj"
	-@erase "$(INTDIR)\PageObj.sbr"
	-@erase "$(INTDIR)\Pager.obj"
	-@erase "$(INTDIR)\Pager.pch"
	-@erase "$(INTDIR)\Pager.res"
	-@erase "$(INTDIR)\Pager.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Pager.bsc"
	-@erase "$(OUTDIR)\Pager.dll"
	-@erase "$(OUTDIR)\Pager.exp"
	-@erase "$(OUTDIR)\Pager.ilk"
	-@erase "$(OUTDIR)\Pager.lib"
	-@erase "$(OUTDIR)\Pager.pdb"
	-@erase "$(OUTDIR)\regsvr32.trg"
	-@erase ".\Pager.h"
	-@erase ".\Pager.tlb"
	-@erase ".\Pager_i.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Pager.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Pager.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Pager.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\PageObj.sbr" \
	"$(INTDIR)\Pager.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Pager.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/Pager.pdb" /debug /machine:I386 /def:".\Pager.def"\
 /out:"$(OUTDIR)/Pager.dll" /implib:"$(OUTDIR)/Pager.lib" 
DEF_FILE= \
	".\Pager.def"
LINK32_OBJS= \
	"$(INTDIR)\PageObj.obj" \
	"$(INTDIR)\Pager.obj" \
	"$(INTDIR)\Pager.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\Pager.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build
OutDir=.\Debug
TargetPath=.\Debug\Pager.dll
InputPath=.\Debug\Pager.dll
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Unicode Release"
# PROP BASE Intermediate_Dir "Unicode Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseU"
# PROP Intermediate_Dir "ReleaseU"
# PROP Target_Dir ""
OUTDIR=.\ReleaseU
INTDIR=.\ReleaseU
# Begin Custom Macros
OutDir=.\ReleaseU
# End Custom Macros

ALL : "$(OUTDIR)\Pager.dll" "$(OUTDIR)\regsvr32.trg" ".\Pager.tlb"

CLEAN : 
	-@erase "$(INTDIR)\PageObj.obj"
	-@erase "$(INTDIR)\Pager.obj"
	-@erase "$(INTDIR)\Pager.pch"
	-@erase "$(INTDIR)\Pager.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\Pager.dll"
	-@erase "$(OUTDIR)\Pager.exp"
	-@erase "$(OUTDIR)\Pager.lib"
	-@erase "$(OUTDIR)\regsvr32.trg"
	-@erase ".\Pager.h"
	-@erase ".\Pager.tlb"
	-@erase ".\Pager_i.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D "WIN32" /D\
 "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/Pager.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\ReleaseU/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Pager.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Pager.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/Pager.pdb" /machine:I386 /def:".\Pager.def"\
 /out:"$(OUTDIR)/Pager.dll" /implib:"$(OUTDIR)/Pager.lib" 
DEF_FILE= \
	".\Pager.def"
LINK32_OBJS= \
	"$(INTDIR)\PageObj.obj" \
	"$(INTDIR)\Pager.obj" \
	"$(INTDIR)\Pager.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\Pager.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build
OutDir=.\ReleaseU
TargetPath=.\ReleaseU\Pager.dll
InputPath=.\ReleaseU\Pager.dll
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Unicode Debug"
# PROP BASE Intermediate_Dir "Unicode Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Target_Dir ""
OUTDIR=.\DebugU
INTDIR=.\DebugU
# Begin Custom Macros
OutDir=.\DebugU
# End Custom Macros

ALL : "$(OUTDIR)\Pager.dll" "$(OUTDIR)\regsvr32.trg" ".\Pager.tlb"

CLEAN : 
	-@erase "$(INTDIR)\PageObj.obj"
	-@erase "$(INTDIR)\Pager.obj"
	-@erase "$(INTDIR)\Pager.pch"
	-@erase "$(INTDIR)\Pager.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Pager.dll"
	-@erase "$(OUTDIR)\Pager.exp"
	-@erase "$(OUTDIR)\Pager.ilk"
	-@erase "$(OUTDIR)\Pager.lib"
	-@erase "$(OUTDIR)\Pager.pdb"
	-@erase "$(OUTDIR)\regsvr32.trg"
	-@erase ".\Pager.h"
	-@erase ".\Pager.tlb"
	-@erase ".\Pager_i.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32"\
 /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/Pager.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\DebugU/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Pager.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Pager.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/Pager.pdb" /debug /machine:I386 /def:".\Pager.def"\
 /out:"$(OUTDIR)/Pager.dll" /implib:"$(OUTDIR)/Pager.lib" 
DEF_FILE= \
	".\Pager.def"
LINK32_OBJS= \
	"$(INTDIR)\PageObj.obj" \
	"$(INTDIR)\Pager.obj" \
	"$(INTDIR)\Pager.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\Pager.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build
OutDir=.\DebugU
TargetPath=.\DebugU\Pager.dll
InputPath=.\DebugU\Pager.dll
SOURCE=$(InputPath)

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   regsvr32 /s /c "$(TargetPath)"
   echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"

# End Custom Build

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

# Name "Pager - Win32 Release"
# Name "Pager - Win32 Debug"
# Name "Pager - Win32 Unicode Release"
# Name "Pager - Win32 Unicode Debug"

!IF  "$(CFG)" == "Pager - Win32 Release"

!ELSEIF  "$(CFG)" == "Pager - Win32 Debug"

!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	{$(INCLUDE)}"\atlbase.h"\
	{$(INCLUDE)}"\atlcom.h"\
	{$(INCLUDE)}"\atlimpl.cpp"\
	

!IF  "$(CFG)" == "Pager - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_WINDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/Pager.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Pager.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Pager - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Pager.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Pager.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D "WIN32" /D\
 "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/Pager.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Pager.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32"\
 /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/Pager.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Pager.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Pager.cpp
DEP_CPP_PAGER=\
	".\PageObj.h"\
	".\Pager.h"\
	".\Pager_i.c"\
	".\StdAfx.h"\
	{$(INCLUDE)}"\atlbase.h"\
	{$(INCLUDE)}"\atlcom.h"\
	

!IF  "$(CFG)" == "Pager - Win32 Release"


"$(INTDIR)\Pager.obj" : $(SOURCE) $(DEP_CPP_PAGER) "$(INTDIR)"\
 "$(INTDIR)\Pager.pch" ".\Pager.h" ".\Pager_i.c"


!ELSEIF  "$(CFG)" == "Pager - Win32 Debug"


"$(INTDIR)\Pager.obj" : $(SOURCE) $(DEP_CPP_PAGER) "$(INTDIR)"\
 "$(INTDIR)\Pager.pch" ".\Pager.h" ".\Pager_i.c"

"$(INTDIR)\Pager.sbr" : $(SOURCE) $(DEP_CPP_PAGER) "$(INTDIR)"\
 "$(INTDIR)\Pager.pch" ".\Pager.h" ".\Pager_i.c"


!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Release"


"$(INTDIR)\Pager.obj" : $(SOURCE) $(DEP_CPP_PAGER) "$(INTDIR)"\
 "$(INTDIR)\Pager.pch" ".\Pager.h" ".\Pager_i.c"


!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Debug"


"$(INTDIR)\Pager.obj" : $(SOURCE) $(DEP_CPP_PAGER) "$(INTDIR)"\
 "$(INTDIR)\Pager.pch" ".\Pager.h" ".\Pager_i.c"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Pager.def

!IF  "$(CFG)" == "Pager - Win32 Release"

!ELSEIF  "$(CFG)" == "Pager - Win32 Debug"

!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Pager.rc

!IF  "$(CFG)" == "Pager - Win32 Release"

DEP_RSC_PAGER_=\
	".\PageObj.rgs"\
	
NODEP_RSC_PAGER_=\
	".\Release\Pager.tlb"\
	

"$(INTDIR)\Pager.res" : $(SOURCE) $(DEP_RSC_PAGER_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Pager - Win32 Debug"

DEP_RSC_PAGER_=\
	".\PageObj.rgs"\
	
NODEP_RSC_PAGER_=\
	".\Debug\Pager.tlb"\
	

"$(INTDIR)\Pager.res" : $(SOURCE) $(DEP_RSC_PAGER_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Release"

DEP_RSC_PAGER_=\
	".\PageObj.rgs"\
	
NODEP_RSC_PAGER_=\
	".\ReleaseU\Pager.tlb"\
	

"$(INTDIR)\Pager.res" : $(SOURCE) $(DEP_RSC_PAGER_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Debug"

DEP_RSC_PAGER_=\
	".\PageObj.rgs"\
	
NODEP_RSC_PAGER_=\
	".\DebugU\Pager.tlb"\
	

"$(INTDIR)\Pager.res" : $(SOURCE) $(DEP_RSC_PAGER_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Pager.idl

!IF  "$(CFG)" == "Pager - Win32 Release"

# Begin Custom Build
InputPath=.\Pager.idl

BuildCmds= \
	midl Pager.idl \
	

"Pager.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Pager.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Pager_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Pager - Win32 Debug"

# Begin Custom Build
InputPath=.\Pager.idl

BuildCmds= \
	midl Pager.idl \
	

"Pager.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Pager.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Pager_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Release"

# Begin Custom Build
InputPath=.\Pager.idl

BuildCmds= \
	midl Pager.idl \
	

"Pager.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Pager.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Pager_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Debug"

# Begin Custom Build
InputPath=.\Pager.idl

BuildCmds= \
	midl Pager.idl \
	

"Pager.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Pager.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Pager_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PageObj.cpp
DEP_CPP_PAGEO=\
	".\PageObj.h"\
	".\Pager.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"\atlbase.h"\
	{$(INCLUDE)}"\atlcom.h"\
	

!IF  "$(CFG)" == "Pager - Win32 Release"


"$(INTDIR)\PageObj.obj" : $(SOURCE) $(DEP_CPP_PAGEO) "$(INTDIR)"\
 "$(INTDIR)\Pager.pch" ".\Pager.h"


!ELSEIF  "$(CFG)" == "Pager - Win32 Debug"


"$(INTDIR)\PageObj.obj" : $(SOURCE) $(DEP_CPP_PAGEO) "$(INTDIR)"\
 "$(INTDIR)\Pager.pch" ".\Pager.h"

"$(INTDIR)\PageObj.sbr" : $(SOURCE) $(DEP_CPP_PAGEO) "$(INTDIR)"\
 "$(INTDIR)\Pager.pch" ".\Pager.h"


!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Release"


"$(INTDIR)\PageObj.obj" : $(SOURCE) $(DEP_CPP_PAGEO) "$(INTDIR)"\
 "$(INTDIR)\Pager.pch" ".\Pager.h"


!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Debug"


"$(INTDIR)\PageObj.obj" : $(SOURCE) $(DEP_CPP_PAGEO) "$(INTDIR)"\
 "$(INTDIR)\Pager.pch" ".\Pager.h"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PageObj.rgs

!IF  "$(CFG)" == "Pager - Win32 Release"

!ELSEIF  "$(CFG)" == "Pager - Win32 Debug"

!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "Pager - Win32 Unicode Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
