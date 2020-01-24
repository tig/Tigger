# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (PPC) Application" 0x0701
# TARGTYPE "Win32 (PPC) Dynamic-Link Library" 0x0702
# TARGTYPE "Power Macintosh Dynamic-Link Library" 0x0402
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Power Macintosh Application" 0x0401

!IF "$(CFG)" == ""
CFG=IViewers - Win32 (PPC) Debug
!MESSAGE No configuration specified.  Defaulting to IViewers - Win32 (PPC)\
 Debug.
!ENDIF 

!IF "$(CFG)" != "Ole2View - Win32 Release" && "$(CFG)" !=\
 "Ole2View - Win32 Debug" && "$(CFG)" != "IViewers - Win32 Release" && "$(CFG)"\
 != "IViewers - Win32 Debug" && "$(CFG)" != "Ole2View - Power Macintosh Debug"\
 && "$(CFG)" != "IViewers - Power Macintosh Debug" && "$(CFG)" !=\
 "Ole2View - Win32 (PPC) Debug" && "$(CFG)" != "IViewers - Win32 (PPC) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ole2view.mak" CFG="IViewers - Win32 (PPC) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Ole2View - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Ole2View - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "IViewers - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IViewers - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Ole2View - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
!MESSAGE "IViewers - Power Macintosh Debug" (based on\
 "Power Macintosh Dynamic-Link Library")
!MESSAGE "Ole2View - Win32 (PPC) Debug" (based on "Win32 (PPC) Application")
!MESSAGE "IViewers - Win32 (PPC) Debug" (based on\
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
# PROP Target_Last_Scanned "Ole2View - Win32 Debug"

!IF  "$(CFG)" == "Ole2View - Win32 Release"

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

ALL : "IViewers - Win32 Release" "$(OUTDIR)\ole2view.exe"\
 "$(OUTDIR)\ole2view.hlp"

CLEAN : 
	-@erase ".\Release\ole2view.exe"
	-@erase ".\Release\regview.obj"
	-@erase ".\Release\ole2view.pch"
	-@erase ".\Release\AboutDlg.obj"
	-@erase ".\Release\obj_vw.obj"
	-@erase ".\Release\util.obj"
	-@erase ".\Release\Ole2View.obj"
	-@erase ".\Release\doc.obj"
	-@erase ".\Release\ServerInfoDlg.obj"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\guids.obj"
	-@erase ".\Release\MainFrm.obj"
	-@erase ".\Release\Ole2View.res"
	-@erase ".\Release\ole2view.hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ole2view.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Ole2View.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ole2view.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"uuid2.lib"
LINK32_FLAGS=version.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/ole2view.pdb" /machine:I386 /nodefaultlib:"uuid2.lib"\
 /out:"$(OUTDIR)/ole2view.exe" 
LINK32_OBJS= \
	"$(INTDIR)/regview.obj" \
	"$(INTDIR)/AboutDlg.obj" \
	"$(INTDIR)/obj_vw.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/Ole2View.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/ServerInfoDlg.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/guids.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/Ole2View.res" \
	".\IViewers\Release\IViewers.lib"

"$(OUTDIR)\ole2view.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

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

ALL : "IViewers - Win32 Debug" "$(OUTDIR)\ole2view.exe"\
 "$(OUTDIR)\ole2view.bsc" "$(OUTDIR)\ole2view.hlp"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\ole2view.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\ole2view.bsc"
	-@erase ".\Debug\guids.sbr"
	-@erase ".\Debug\Ole2View.sbr"
	-@erase ".\Debug\AboutDlg.sbr"
	-@erase ".\Debug\MainFrm.sbr"
	-@erase ".\Debug\util.sbr"
	-@erase ".\Debug\ServerInfoDlg.sbr"
	-@erase ".\Debug\regview.sbr"
	-@erase ".\Debug\doc.sbr"
	-@erase ".\Debug\StdAfx.sbr"
	-@erase ".\Debug\obj_vw.sbr"
	-@erase ".\Debug\ole2view.exe"
	-@erase ".\Debug\doc.obj"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\obj_vw.obj"
	-@erase ".\Debug\guids.obj"
	-@erase ".\Debug\Ole2View.obj"
	-@erase ".\Debug\AboutDlg.obj"
	-@erase ".\Debug\MainFrm.obj"
	-@erase ".\Debug\util.obj"
	-@erase ".\Debug\ServerInfoDlg.obj"
	-@erase ".\Debug\regview.obj"
	-@erase ".\Debug\Ole2View.res"
	-@erase ".\Debug\ole2view.ilk"
	-@erase ".\Debug\ole2view.pdb"
	-@erase ".\Debug\ole2view.hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W4 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W4 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/ole2view.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Ole2View.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ole2view.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/guids.sbr" \
	"$(INTDIR)/Ole2View.sbr" \
	"$(INTDIR)/AboutDlg.sbr" \
	"$(INTDIR)/MainFrm.sbr" \
	"$(INTDIR)/util.sbr" \
	"$(INTDIR)/ServerInfoDlg.sbr" \
	"$(INTDIR)/regview.sbr" \
	"$(INTDIR)/doc.sbr" \
	"$(INTDIR)/StdAfx.sbr" \
	"$(INTDIR)/obj_vw.sbr"

"$(OUTDIR)\ole2view.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 version.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=version.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/ole2view.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/ole2view.exe" 
LINK32_OBJS= \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/obj_vw.obj" \
	"$(INTDIR)/guids.obj" \
	"$(INTDIR)/Ole2View.obj" \
	"$(INTDIR)/AboutDlg.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/ServerInfoDlg.obj" \
	"$(INTDIR)/regview.obj" \
	"$(INTDIR)/Ole2View.res" \
	".\IViewers\Debug\IViewers.lib"

"$(OUTDIR)\ole2view.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "IViewers - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "IViewers\Release"
# PROP BASE Intermediate_Dir "IViewers\Release"
# PROP BASE Target_Dir "IViewers"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "IViewers\Release"
# PROP Intermediate_Dir "IViewers\Release"
# PROP Target_Dir "IViewers"
OUTDIR=.\IViewers\Release
INTDIR=.\IViewers\Release

ALL : "$(OUTDIR)\IViewers.dll"

CLEAN : 
	-@erase ".\IViewers\Release\IViewers.dll"
	-@erase ".\IViewers\Release\UniformDataTransfer.obj"
	-@erase ".\IViewers\Release\IViewers.pch"
	-@erase ".\IViewers\Release\iadvsink.obj"
	-@erase ".\IViewers\Release\guid.obj"
	-@erase ".\IViewers\Release\dispatch.obj"
	-@erase ".\IViewers\Release\idataobj.obj"
	-@erase ".\IViewers\Release\typelib.obj"
	-@erase ".\IViewers\Release\IPersist.obj"
	-@erase ".\IViewers\Release\AboutDlg.obj"
	-@erase ".\IViewers\Release\dataobj.obj"
	-@erase ".\IViewers\Release\tlbodl.obj"
	-@erase ".\IViewers\Release\iviewer.obj"
	-@erase ".\IViewers\Release\tlbtree.obj"
	-@erase ".\IViewers\Release\IViewers.obj"
	-@erase ".\IViewers\Release\util.obj"
	-@erase ".\IViewers\Release\IUnknown.obj"
	-@erase ".\IViewers\Release\StdAfx.obj"
	-@erase ".\IViewers\Release\tree.obj"
	-@erase ".\IViewers\Release\IViewers.res"
	-@erase ".\IViewers\Release\IViewers.lib"
	-@erase ".\IViewers\Release\IViewers.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/IViewers.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\IViewers\Release/
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/IViewers.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/IViewers.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 version.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=version.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/IViewers.pdb" /machine:I386 /def:".\IViewers\IViewers.def"\
 /out:"$(OUTDIR)/IViewers.dll" /implib:"$(OUTDIR)/IViewers.lib" 
DEF_FILE= \
	".\IViewers\IViewers.def"
LINK32_OBJS= \
	"$(INTDIR)/UniformDataTransfer.obj" \
	"$(INTDIR)/iadvsink.obj" \
	"$(INTDIR)/guid.obj" \
	"$(INTDIR)/dispatch.obj" \
	"$(INTDIR)/idataobj.obj" \
	"$(INTDIR)/typelib.obj" \
	"$(INTDIR)/IPersist.obj" \
	"$(INTDIR)/AboutDlg.obj" \
	"$(INTDIR)/dataobj.obj" \
	"$(INTDIR)/tlbodl.obj" \
	"$(INTDIR)/iviewer.obj" \
	"$(INTDIR)/tlbtree.obj" \
	"$(INTDIR)/IViewers.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/IUnknown.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/tree.obj" \
	"$(INTDIR)/IViewers.res"

"$(OUTDIR)\IViewers.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "IViewers\Debug"
# PROP BASE Intermediate_Dir "IViewers\Debug"
# PROP BASE Target_Dir "IViewers"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "IViewers\Debug"
# PROP Intermediate_Dir "IViewers\Debug"
# PROP Target_Dir "IViewers"
OUTDIR=.\IViewers\Debug
INTDIR=.\IViewers\Debug

ALL : "$(OUTDIR)\IViewers.dll"

CLEAN : 
	-@erase ".\IViewers\Debug\vc40.pdb"
	-@erase ".\IViewers\Debug\IViewers.pch"
	-@erase ".\IViewers\Debug\vc40.idb"
	-@erase ".\IViewers\Debug\IViewers.dll"
	-@erase ".\IViewers\Debug\dataobj.obj"
	-@erase ".\IViewers\Debug\iviewer.obj"
	-@erase ".\IViewers\Debug\tlbtree.obj"
	-@erase ".\IViewers\Debug\iadvsink.obj"
	-@erase ".\IViewers\Debug\IPersist.obj"
	-@erase ".\IViewers\Debug\AboutDlg.obj"
	-@erase ".\IViewers\Debug\StdAfx.obj"
	-@erase ".\IViewers\Debug\dispatch.obj"
	-@erase ".\IViewers\Debug\idataobj.obj"
	-@erase ".\IViewers\Debug\IViewers.obj"
	-@erase ".\IViewers\Debug\util.obj"
	-@erase ".\IViewers\Debug\UniformDataTransfer.obj"
	-@erase ".\IViewers\Debug\tree.obj"
	-@erase ".\IViewers\Debug\typelib.obj"
	-@erase ".\IViewers\Debug\IUnknown.obj"
	-@erase ".\IViewers\Debug\tlbodl.obj"
	-@erase ".\IViewers\Debug\guid.obj"
	-@erase ".\IViewers\Debug\IViewers.res"
	-@erase ".\IViewers\Debug\IViewers.ilk"
	-@erase ".\IViewers\Debug\IViewers.lib"
	-@erase ".\IViewers\Debug\IViewers.exp"
	-@erase ".\IViewers\Debug\IViewers.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W4 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W4 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL"\
 /Fp"$(INTDIR)/IViewers.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\IViewers\Debug/
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
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/IViewers.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/IViewers.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 version.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=version.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/IViewers.pdb" /debug /machine:I386\
 /def:".\IViewers\IViewers.def" /out:"$(OUTDIR)/IViewers.dll"\
 /implib:"$(OUTDIR)/IViewers.lib" 
DEF_FILE= \
	".\IViewers\IViewers.def"
LINK32_OBJS= \
	"$(INTDIR)/dataobj.obj" \
	"$(INTDIR)/iviewer.obj" \
	"$(INTDIR)/tlbtree.obj" \
	"$(INTDIR)/iadvsink.obj" \
	"$(INTDIR)/IPersist.obj" \
	"$(INTDIR)/AboutDlg.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/dispatch.obj" \
	"$(INTDIR)/idataobj.obj" \
	"$(INTDIR)/IViewers.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/UniformDataTransfer.obj" \
	"$(INTDIR)/tree.obj" \
	"$(INTDIR)/typelib.obj" \
	"$(INTDIR)/IUnknown.obj" \
	"$(INTDIR)/tlbodl.obj" \
	"$(INTDIR)/guid.obj" \
	"$(INTDIR)/IViewers.res"

"$(OUTDIR)\IViewers.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Ole2View"
# PROP BASE Intermediate_Dir "Ole2View"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PMacDbg"
# PROP Intermediate_Dir "PMacDbg"
# PROP Target_Dir ""
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "IViewers - Power Macintosh Debug" "$(OUTDIR)\ole2view.exe"\
 "$(OUTDIR)\ole2view.trg" "$(OUTDIR)\ole2view.hlp"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\ole2view.pch"
	-@erase ".\PMacDbg\ole2view.ilk"
	-@erase ".\PMacDbg\AboutDlg.obj"
	-@erase ".\PMacDbg\doc.obj"
	-@erase ".\PMacDbg\util.obj"
	-@erase ".\PMacDbg\obj_vw.obj"
	-@erase ".\PMacDbg\Ole2View.obj"
	-@erase ".\PMacDbg\guids.obj"
	-@erase ".\PMacDbg\MainFrm.obj"
	-@erase ".\PMacDbg\StdAfx.obj"
	-@erase ".\PMacDbg\ServerInfoDlg.obj"
	-@erase ".\PMacDbg\regview.obj"
	-@erase ".\PMacDbg\Ole2ViewMac.rsc"
	-@erase ".\PMacDbg\Ole2View.rsc"
	-@erase ".\PMacDbg\ole2view.pdb"
	-@erase ".\PMacDbg\ole2view.trg"
	-@erase ".\PMacDbg\ole2view.exe"
	-@erase ".\PMacDbg\ole2view.hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /WX /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W4 /WX /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W4 /WX /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/ole2view.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PMacDbg/
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
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/Ole2View.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ole2view.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 version.lib /nologo /mac:bundle /debug /machine:MPPC
# SUBTRACT BASE LINK32 /incremental:no
# ADD LINK32 /nologo /mac:bundle /debug /machine:MPPC
# SUBTRACT LINK32 /incremental:no
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="????"\
 /incremental:yes /pdb:"$(OUTDIR)/ole2view.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/ole2view.exe" 
LINK32_OBJS= \
	"$(INTDIR)/AboutDlg.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/obj_vw.obj" \
	"$(INTDIR)/Ole2View.obj" \
	"$(INTDIR)/guids.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/ServerInfoDlg.obj" \
	"$(INTDIR)/regview.obj" \
	".\IViewers\PMacDbg\IViewers.lib" \
	".\PMacDbg\Ole2ViewMac.rsc" \
	".\PMacDbg\Ole2View.rsc"

"$(OUTDIR)\ole2view.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\IViewers\PMacDbg\IViewers.dll" \
	"$(OUTDIR)/ole2view.exe"

"$(OUTDIR)\ole2view.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\IViewers\PMacDbg\IViewers.dll\
 "$(MFILE32_DEST):IViewers.dll">"$(OUTDIR)\ole2view.trg"
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\ole2view.exe\
 "$(MFILE32_DEST):ole2view.exe">"$(OUTDIR)\ole2view.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\IViewers\PMacDbg\IViewers.dll\
 "$(MFILE32_DEST):IViewers.dll">"$(OUTDIR)\ole2view.trg"
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\ole2view.exe\
 "$(MFILE32_DEST):ole2view.exe">"$(OUTDIR)\ole2view.trg"

!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "IViewers\PMacDbg"
# PROP BASE Intermediate_Dir "IViewers\PMacDbg"
# PROP BASE Target_Dir "IViewers"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "IViewers\PMacDbg"
# PROP Intermediate_Dir "IViewers\PMacDbg"
# PROP Target_Dir "IViewers"
OUTDIR=.\IViewers\PMacDbg
INTDIR=.\IViewers\PMacDbg

ALL : "$(OUTDIR)\IViewers.dll" "$(OUTDIR)\IViewers.trg"

CLEAN : 
	-@erase ".\IViewers\PMacDbg\vc40.pdb"
	-@erase ".\IViewers\PMacDbg\IViewers.pch"
	-@erase ".\IViewers\PMacDbg\IViewers.ilk"
	-@erase ".\IViewers\PMacDbg\typelib.obj"
	-@erase ".\IViewers\PMacDbg\IPersist.obj"
	-@erase ".\IViewers\PMacDbg\AboutDlg.obj"
	-@erase ".\IViewers\PMacDbg\UniformDataTransfer.obj"
	-@erase ".\IViewers\PMacDbg\dataobj.obj"
	-@erase ".\IViewers\PMacDbg\IUnknown.obj"
	-@erase ".\IViewers\PMacDbg\iviewer.obj"
	-@erase ".\IViewers\PMacDbg\tlbtree.obj"
	-@erase ".\IViewers\PMacDbg\tlbodl.obj"
	-@erase ".\IViewers\PMacDbg\dispatch.obj"
	-@erase ".\IViewers\PMacDbg\idataobj.obj"
	-@erase ".\IViewers\PMacDbg\tree.obj"
	-@erase ".\IViewers\PMacDbg\guid.obj"
	-@erase ".\IViewers\PMacDbg\StdAfx.obj"
	-@erase ".\IViewers\PMacDbg\IViewers.obj"
	-@erase ".\IViewers\PMacDbg\util.obj"
	-@erase ".\IViewers\PMacDbg\iadvsink.obj"
	-@erase ".\IViewers\PMacDbg\IViewers.rsc"
	-@erase ".\IViewers\PMacDbg\IViewersMac.rsc"
	-@erase ".\IViewers\PMacDbg\IViewers.lib"
	-@erase ".\IViewers\PMacDbg\IViewers.exp"
	-@erase ".\IViewers\PMacDbg\IViewers.pdb"
	-@erase ".\IViewers\PMacDbg\IViewers.trg"
	-@erase ".\IViewers\PMacDbg\IViewers.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /WX /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W4 /WX /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W4 /WX /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS"\
 /D "WIN32" /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "_AFXDLL"\
 /Fp"$(INTDIR)/IViewers.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\IViewers\PMacDbg/
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
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/IViewers.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/IViewers.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 version.lib /nologo /mac:nobundle /dll /debug /machine:MPPC
# SUBTRACT BASE LINK32 /incremental:no
# ADD LINK32 /nologo /mac:nobundle /dll /debug /machine:MPPC
# SUBTRACT LINK32 /incremental:no
LINK32_FLAGS=/nologo /mac:nobundle /mac:type="shlb" /mac:creator="cfmg"\
 /mac:init="WlmConnectionInit" /mac:term="WlmConnectionTerm" /dll\
 /incremental:yes /pdb:"$(OUTDIR)/IViewers.pdb" /debug /machine:MPPC\
 /def:".\IViewers\IViewers.def" /out:"$(OUTDIR)/IViewers.dll"\
 /implib:"$(OUTDIR)/IViewers.lib" 
DEF_FILE= \
	".\IViewers\IViewers.def"
LINK32_OBJS= \
	"$(INTDIR)/typelib.obj" \
	"$(INTDIR)/IPersist.obj" \
	"$(INTDIR)/AboutDlg.obj" \
	"$(INTDIR)/UniformDataTransfer.obj" \
	"$(INTDIR)/dataobj.obj" \
	"$(INTDIR)/IUnknown.obj" \
	"$(INTDIR)/iviewer.obj" \
	"$(INTDIR)/tlbtree.obj" \
	"$(INTDIR)/tlbodl.obj" \
	"$(INTDIR)/dispatch.obj" \
	"$(INTDIR)/idataobj.obj" \
	"$(INTDIR)/tree.obj" \
	"$(INTDIR)/guid.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/IViewers.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/iadvsink.obj" \
	"$(INTDIR)/IViewers.rsc" \
	"$(INTDIR)/IViewersMac.rsc"

"$(OUTDIR)\IViewers.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/IViewers.dll"

"$(OUTDIR)\IViewers.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\IViewers\PMacDbg\IViewers.dll\
 "$(MFILE32_DEST):IViewers.dll">"$(OUTDIR)\IViewers.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\IViewers\PMacDbg\IViewers.dll\
 "$(MFILE32_DEST):IViewers.dll">"$(OUTDIR)\IViewers.trg"

!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Ole2View"
# PROP BASE Intermediate_Dir "Ole2View"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PPCDebug"
# PROP Intermediate_Dir "PPCDebug"
# PROP Target_Dir ""
OUTDIR=.\PPCDebug
INTDIR=.\PPCDebug

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

ALL : "IViewers - Win32 (PPC) Debug" "$(OUTDIR)\Ole2View.exe"\
 "$(OUTDIR)\Ole2View.bsc" "$(OUTDIR)\Ole2View.hlp"

CLEAN : 
	-@erase ".\PPCDebug\vc40.pdb"
	-@erase ".\PPCDebug\Ole2View.pch"
	-@erase ".\PPCDebug\Ole2View.bsc"
	-@erase ".\PPCDebug\util.sbr"
	-@erase ".\PPCDebug\StdAfx.sbr"
	-@erase ".\PPCDebug\obj_vw.sbr"
	-@erase ".\PPCDebug\MainFrm.sbr"
	-@erase ".\PPCDebug\Ole2View.sbr"
	-@erase ".\PPCDebug\ServerInfoDlg.sbr"
	-@erase ".\PPCDebug\regview.sbr"
	-@erase ".\PPCDebug\doc.sbr"
	-@erase ".\PPCDebug\AboutDlg.sbr"
	-@erase ".\PPCDebug\guids.sbr"
	-@erase ".\PPCDebug\Ole2View.exe"
	-@erase ".\PPCDebug\doc.obj"
	-@erase ".\PPCDebug\AboutDlg.obj"
	-@erase ".\PPCDebug\guids.obj"
	-@erase ".\PPCDebug\util.obj"
	-@erase ".\PPCDebug\StdAfx.obj"
	-@erase ".\PPCDebug\obj_vw.obj"
	-@erase ".\PPCDebug\MainFrm.obj"
	-@erase ".\PPCDebug\Ole2View.obj"
	-@erase ".\PPCDebug\ServerInfoDlg.obj"
	-@erase ".\PPCDebug\regview.obj"
	-@erase ".\PPCDebug\Ole2View.res"
	-@erase ".\PPCDebug\Ole2View.pdb"
	-@erase ".\PPCDebug\Ole2View.hlp"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /PPC32
# ADD MTL /nologo /D "_DEBUG" /PPC32
MTL_PROJ=/nologo /D "_DEBUG" /PPC32 
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Ole2View.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PPCDebug/
CPP_SBRS=.\PPCDebug/

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Ole2View.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Ole2View.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/util.sbr" \
	"$(INTDIR)/StdAfx.sbr" \
	"$(INTDIR)/obj_vw.sbr" \
	"$(INTDIR)/MainFrm.sbr" \
	"$(INTDIR)/Ole2View.sbr" \
	"$(INTDIR)/ServerInfoDlg.sbr" \
	"$(INTDIR)/regview.sbr" \
	"$(INTDIR)/doc.sbr" \
	"$(INTDIR)/AboutDlg.sbr" \
	"$(INTDIR)/guids.sbr"

"$(OUTDIR)\Ole2View.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:PPC
# ADD LINK32 version.lib /nologo /subsystem:windows /debug /machine:PPC
LINK32_FLAGS=version.lib /nologo /subsystem:windows\
 /pdb:"$(OUTDIR)/Ole2View.pdb" /debug /machine:PPC /out:"$(OUTDIR)/Ole2View.exe"\
 
LINK32_OBJS= \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/AboutDlg.obj" \
	"$(INTDIR)/guids.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/obj_vw.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/Ole2View.obj" \
	"$(INTDIR)/ServerInfoDlg.obj" \
	"$(INTDIR)/regview.obj" \
	"$(INTDIR)/Ole2View.res" \
	".\IViewers\PPCDbg\IViewers.lib"

"$(OUTDIR)\Ole2View.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "IViewers\PPCDbg"
# PROP BASE Intermediate_Dir "IViewers\PPCDbg"
# PROP BASE Target_Dir "IViewers"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "IViewers\PPCDbg"
# PROP Intermediate_Dir "IViewers\PPCDbg"
# PROP Target_Dir "IViewers"
OUTDIR=.\IViewers\PPCDbg
INTDIR=.\IViewers\PPCDbg

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

ALL : "$(OUTDIR)\IViewers.dll"

CLEAN : 
	-@erase ".\IViewers\PPCDbg\vc40.pdb"
	-@erase ".\IViewers\PPCDbg\IViewers.pch"
	-@erase ".\IViewers\PPCDbg\IViewers.dll"
	-@erase ".\IViewers\PPCDbg\UniformDataTransfer.obj"
	-@erase ".\IViewers\PPCDbg\util.obj"
	-@erase ".\IViewers\PPCDbg\idataobj.obj"
	-@erase ".\IViewers\PPCDbg\dataobj.obj"
	-@erase ".\IViewers\PPCDbg\iviewer.obj"
	-@erase ".\IViewers\PPCDbg\StdAfx.obj"
	-@erase ".\IViewers\PPCDbg\tree.obj"
	-@erase ".\IViewers\PPCDbg\IPersist.obj"
	-@erase ".\IViewers\PPCDbg\AboutDlg.obj"
	-@erase ".\IViewers\PPCDbg\guid.obj"
	-@erase ".\IViewers\PPCDbg\typelib.obj"
	-@erase ".\IViewers\PPCDbg\dispatch.obj"
	-@erase ".\IViewers\PPCDbg\IViewers.obj"
	-@erase ".\IViewers\PPCDbg\tlbtree.obj"
	-@erase ".\IViewers\PPCDbg\tlbodl.obj"
	-@erase ".\IViewers\PPCDbg\IUnknown.obj"
	-@erase ".\IViewers\PPCDbg\iadvsink.obj"
	-@erase ".\IViewers\PPCDbg\IViewers.res"
	-@erase ".\IViewers\PPCDbg\IViewers.lib"
	-@erase ".\IViewers\PPCDbg\IViewers.exp"
	-@erase ".\IViewers\PPCDbg\IViewers.pdb"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /PPC32
# ADD MTL /nologo /D "_DEBUG" /PPC32
MTL_PROJ=/nologo /D "_DEBUG" /PPC32 
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/IViewers.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\IViewers\PPCDbg/
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/IViewers.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/IViewers.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:PPC
# ADD LINK32 version.lib /nologo /subsystem:windows /dll /debug /machine:PPC
LINK32_FLAGS=version.lib /nologo /subsystem:windows /dll\
 /pdb:"$(OUTDIR)/IViewers.pdb" /debug /machine:PPC\
 /def:".\IViewers\IViewers.def" /out:"$(OUTDIR)/IViewers.dll"\
 /implib:"$(OUTDIR)/IViewers.lib" 
DEF_FILE= \
	".\IViewers\IViewers.def"
LINK32_OBJS= \
	"$(INTDIR)/UniformDataTransfer.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/idataobj.obj" \
	"$(INTDIR)/dataobj.obj" \
	"$(INTDIR)/iviewer.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/tree.obj" \
	"$(INTDIR)/IPersist.obj" \
	"$(INTDIR)/AboutDlg.obj" \
	"$(INTDIR)/guid.obj" \
	"$(INTDIR)/typelib.obj" \
	"$(INTDIR)/dispatch.obj" \
	"$(INTDIR)/IViewers.obj" \
	"$(INTDIR)/tlbtree.obj" \
	"$(INTDIR)/tlbodl.obj" \
	"$(INTDIR)/IUnknown.obj" \
	"$(INTDIR)/iadvsink.obj" \
	"$(INTDIR)/IViewers.res"

"$(OUTDIR)\IViewers.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "Ole2View - Win32 Release"
# Name "Ole2View - Win32 Debug"
# Name "Ole2View - Power Macintosh Debug"
# Name "Ole2View - Win32 (PPC) Debug"

!IF  "$(CFG)" == "Ole2View - Win32 Release"

!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\hlp\Ole2View.hpj

!IF  "$(CFG)" == "Ole2View - Win32 Release"

# Begin Custom Build - Making help file...
OutDir=.\Release
ProjDir=.
TargetName=ole2view
InputPath=.\hlp\Ole2View.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

# Begin Custom Build - Making help file...
OutDir=.\Debug
ProjDir=.
TargetName=ole2view
InputPath=.\hlp\Ole2View.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

# Begin Custom Build - Making help file...
OutDir=.\PMacDbg
ProjDir=.
TargetName=ole2view
InputPath=.\hlp\Ole2View.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

# Begin Custom Build - Making help file...
InputPath=.\hlp\Ole2View.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\util.cpp

!IF  "$(CFG)" == "Ole2View - Win32 Release"

DEP_CPP_UTIL_=\
	".\stdafx.h"\
	".\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

DEP_CPP_UTIL_=\
	".\stdafx.h"\
	".\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"

"$(INTDIR)\util.sbr" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

DEP_CPP_UTIL_=\
	".\stdafx.h"\
	".\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

DEP_CPP_UTIL_=\
	".\stdafx.h"\
	".\util.h"\
	".\comcat.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

"$(INTDIR)\util.sbr" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Ole2View - Win32 Release"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	{$(INCLUDE)}"\comcat.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ole2view.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ole2view.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	{$(INCLUDE)}"\comcat.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W4 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/ole2view.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ole2view.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	{$(INCLUDE)}"\comcat.h"\
	
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W4 /WX /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)/ole2view.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ole2view.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	".\comcat.h"\
	
# ADD CPP /Yc"stdafx.h"
BuildCmds= \
	$(CPP) /nologo /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Ole2View.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Ole2View.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\regview.cpp

!IF  "$(CFG)" == "Ole2View - Win32 Release"

DEP_CPP_REGVI=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\regview.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\obj_vw.h"\
	

"$(INTDIR)\regview.obj" : $(SOURCE) $(DEP_CPP_REGVI) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

DEP_CPP_REGVI=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\regview.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\obj_vw.h"\
	

"$(INTDIR)\regview.obj" : $(SOURCE) $(DEP_CPP_REGVI) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"

"$(INTDIR)\regview.sbr" : $(SOURCE) $(DEP_CPP_REGVI) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

DEP_CPP_REGVI=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\regview.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\obj_vw.h"\
	

"$(INTDIR)\regview.obj" : $(SOURCE) $(DEP_CPP_REGVI) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

DEP_CPP_REGVI=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\regview.h"\
	".\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\obj_vw.h"\
	

"$(INTDIR)\regview.obj" : $(SOURCE) $(DEP_CPP_REGVI) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

"$(INTDIR)\regview.sbr" : $(SOURCE) $(DEP_CPP_REGVI) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ole2View.cpp

!IF  "$(CFG)" == "Ole2View - Win32 Release"

DEP_CPP_OLE2V=\
	".\stdafx.h"\
	".\IViewers\iview.h"\
	".\Ole2View.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\obj_vw.h"\
	".\regview.h"\
	".\AboutDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\util.h"\
	

"$(INTDIR)\Ole2View.obj" : $(SOURCE) $(DEP_CPP_OLE2V) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

DEP_CPP_OLE2V=\
	".\stdafx.h"\
	".\IViewers\iview.h"\
	".\Ole2View.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\obj_vw.h"\
	".\regview.h"\
	".\AboutDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\util.h"\
	

"$(INTDIR)\Ole2View.obj" : $(SOURCE) $(DEP_CPP_OLE2V) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"

"$(INTDIR)\Ole2View.sbr" : $(SOURCE) $(DEP_CPP_OLE2V) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

DEP_CPP_OLE2V=\
	".\stdafx.h"\
	".\IViewers\iview.h"\
	".\Ole2View.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\obj_vw.h"\
	".\regview.h"\
	".\AboutDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\util.h"\
	

"$(INTDIR)\Ole2View.obj" : $(SOURCE) $(DEP_CPP_OLE2V) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

DEP_CPP_OLE2V=\
	".\stdafx.h"\
	".\IViewers\iview.h"\
	".\Ole2View.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\obj_vw.h"\
	".\regview.h"\
	".\AboutDlg.h"\
	".\comcat.h"\
	".\util.h"\
	

"$(INTDIR)\Ole2View.obj" : $(SOURCE) $(DEP_CPP_OLE2V) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

"$(INTDIR)\Ole2View.sbr" : $(SOURCE) $(DEP_CPP_OLE2V) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\obj_vw.cpp

!IF  "$(CFG)" == "Ole2View - Win32 Release"

DEP_CPP_OBJ_V=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\obj_vw.h"\
	".\ServerInfoDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\regview.h"\
	

"$(INTDIR)\obj_vw.obj" : $(SOURCE) $(DEP_CPP_OBJ_V) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

DEP_CPP_OBJ_V=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\obj_vw.h"\
	".\ServerInfoDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\regview.h"\
	

"$(INTDIR)\obj_vw.obj" : $(SOURCE) $(DEP_CPP_OBJ_V) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"

"$(INTDIR)\obj_vw.sbr" : $(SOURCE) $(DEP_CPP_OBJ_V) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

DEP_CPP_OBJ_V=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\obj_vw.h"\
	".\ServerInfoDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\regview.h"\
	

"$(INTDIR)\obj_vw.obj" : $(SOURCE) $(DEP_CPP_OBJ_V) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

DEP_CPP_OBJ_V=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\obj_vw.h"\
	".\ServerInfoDlg.h"\
	".\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\regview.h"\
	

"$(INTDIR)\obj_vw.obj" : $(SOURCE) $(DEP_CPP_OBJ_V) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

"$(INTDIR)\obj_vw.sbr" : $(SOURCE) $(DEP_CPP_OBJ_V) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "Ole2View - Win32 Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\MainFrm.h"\
	".\obj_vw.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\doc.h"\
	".\regview.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\MainFrm.h"\
	".\obj_vw.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\doc.h"\
	".\regview.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"

"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\MainFrm.h"\
	".\obj_vw.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\doc.h"\
	".\regview.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\MainFrm.h"\
	".\obj_vw.h"\
	".\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\doc.h"\
	".\regview.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\doc.cpp

!IF  "$(CFG)" == "Ole2View - Win32 Release"

DEP_CPP_DOC_C=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\doc.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\regview.h"\
	".\obj_vw.h"\
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

DEP_CPP_DOC_C=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\doc.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\regview.h"\
	".\obj_vw.h"\
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"

"$(INTDIR)\doc.sbr" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

DEP_CPP_DOC_C=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\doc.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\regview.h"\
	".\obj_vw.h"\
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

DEP_CPP_DOC_C=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\doc.h"\
	".\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\regview.h"\
	".\obj_vw.h"\
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

"$(INTDIR)\doc.sbr" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ole2View.rc

!IF  "$(CFG)" == "Ole2View - Win32 Release"

DEP_RSC_OLE2VI=\
	".\res\Ole2View.ico"\
	".\res\automati.ico"\
	".\res\dragdrop.ico"\
	".\res\noicon.ico"\
	".\res\icon1.ico"\
	".\res\ole2_1.ico"\
	".\res\ico00001.ico"\
	".\res\ico00002.ico"\
	".\res\object.ico"\
	".\res\toolbar.bmp"\
	".\res\automati.bmp"\
	".\res\containe.bmp"\
	".\res\bmp00002.bmp"\
	".\res\object.bmp"\
	".\res\object32.bmp"\
	".\res\control.bmp"\
	".\res\obj_c32.bmp"\
	".\res\object_i.bmp"\
	".\res\bmp00001.bmp"\
	".\res\insrt132.bmp"\
	".\res\insrt_32.bmp"\
	".\res\question.bmp"\
	".\res\questn32.bmp"\
	".\res\imagelis.bmp"\
	".\res\bitmap1.bmp"\
	".\res\checkbut.bmp"\
	".\res\Ole2View.rc2"\
	

"$(INTDIR)\Ole2View.res" : $(SOURCE) $(DEP_RSC_OLE2VI) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Ole2View.res" /d "NDEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

DEP_RSC_OLE2VI=\
	".\res\Ole2View.ico"\
	".\res\automati.ico"\
	".\res\dragdrop.ico"\
	".\res\noicon.ico"\
	".\res\icon1.ico"\
	".\res\ole2_1.ico"\
	".\res\ico00001.ico"\
	".\res\ico00002.ico"\
	".\res\object.ico"\
	".\res\toolbar.bmp"\
	".\res\automati.bmp"\
	".\res\containe.bmp"\
	".\res\bmp00002.bmp"\
	".\res\object.bmp"\
	".\res\object32.bmp"\
	".\res\control.bmp"\
	".\res\obj_c32.bmp"\
	".\res\object_i.bmp"\
	".\res\bmp00001.bmp"\
	".\res\insrt132.bmp"\
	".\res\insrt_32.bmp"\
	".\res\question.bmp"\
	".\res\questn32.bmp"\
	".\res\imagelis.bmp"\
	".\res\bitmap1.bmp"\
	".\res\checkbut.bmp"\
	".\res\Ole2View.rc2"\
	

"$(INTDIR)\Ole2View.res" : $(SOURCE) $(DEP_RSC_OLE2VI) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Ole2View.res" /d "_DEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

# PROP Intermediate_Dir "PMacDbg"
DEP_RSC_OLE2VI=\
	".\res\Ole2View.ico"\
	".\res\automati.ico"\
	".\res\dragdrop.ico"\
	".\res\noicon.ico"\
	".\res\icon1.ico"\
	".\res\ole2_1.ico"\
	".\res\ico00001.ico"\
	".\res\ico00002.ico"\
	".\res\object.ico"\
	".\res\toolbar.bmp"\
	".\res\automati.bmp"\
	".\res\containe.bmp"\
	".\res\bmp00002.bmp"\
	".\res\object.bmp"\
	".\res\object32.bmp"\
	".\res\control.bmp"\
	".\res\obj_c32.bmp"\
	".\res\object_i.bmp"\
	".\res\bmp00001.bmp"\
	".\res\insrt132.bmp"\
	".\res\insrt_32.bmp"\
	".\res\question.bmp"\
	".\res\questn32.bmp"\
	".\res\imagelis.bmp"\
	".\res\bitmap1.bmp"\
	".\res\checkbut.bmp"\
	".\res\Ole2View.rc2"\
	
INTDIR_SRC=.\PMacDbg
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409

".\PMacDbg\Ole2View.rsc" : $(SOURCE) $(DEP_RSC_OLE2VI) "$(INTDIR_SRC)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR_SRC)/Ole2View.rsc" /d "_MAC" /d "_MPPC_"\
 /d "_DEBUG" /d "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

DEP_RSC_OLE2VI=\
	".\res\Ole2View.ico"\
	".\res\automati.ico"\
	".\res\dragdrop.ico"\
	".\res\noicon.ico"\
	".\res\icon1.ico"\
	".\res\ole2_1.ico"\
	".\res\ico00001.ico"\
	".\res\ico00002.ico"\
	".\res\object.ico"\
	".\res\toolbar.bmp"\
	".\res\automati.bmp"\
	".\res\containe.bmp"\
	".\res\bmp00002.bmp"\
	".\res\object.bmp"\
	".\res\object32.bmp"\
	".\res\control.bmp"\
	".\res\obj_c32.bmp"\
	".\res\object_i.bmp"\
	".\res\bmp00001.bmp"\
	".\res\insrt132.bmp"\
	".\res\insrt_32.bmp"\
	".\res\question.bmp"\
	".\res\questn32.bmp"\
	".\res\imagelis.bmp"\
	".\res\bitmap1.bmp"\
	".\res\checkbut.bmp"\
	".\res\Ole2View.rc2"\
	

"$(INTDIR)\Ole2View.res" : $(SOURCE) $(DEP_RSC_OLE2VI) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Ole2View.res" /d "_DEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "IViewers"

!IF  "$(CFG)" == "Ole2View - Win32 Release"

"IViewers - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\ole2view.mak CFG="IViewers - Win32 Release" 

!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

"IViewers - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\ole2view.mak CFG="IViewers - Win32 Debug" 

!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

"IViewers - Power Macintosh Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\ole2view.mak\
 CFG="IViewers - Power Macintosh Debug" 

!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

"IViewers - Win32 (PPC) Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\ole2view.mak CFG="IViewers - Win32 (PPC) Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\AboutDlg.cpp

!IF  "$(CFG)" == "Ole2View - Win32 Release"

DEP_CPP_ABOUT=\
	".\stdafx.h"\
	".\AboutDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\AboutDlg.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

DEP_CPP_ABOUT=\
	".\stdafx.h"\
	".\AboutDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\AboutDlg.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"

"$(INTDIR)\AboutDlg.sbr" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

DEP_CPP_ABOUT=\
	".\stdafx.h"\
	".\AboutDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\AboutDlg.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

DEP_CPP_ABOUT=\
	".\stdafx.h"\
	".\AboutDlg.h"\
	".\comcat.h"\
	

"$(INTDIR)\AboutDlg.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

"$(INTDIR)\AboutDlg.sbr" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ole2View.r

!IF  "$(CFG)" == "Ole2View - Win32 Release"

!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

# PROP Intermediate_Dir "PMacDbg"
INTDIR_SRC=.\PMacDbg
# ADD MRC /o"PMacDbg/Ole2ViewMac.rsc"

".\PMacDbg\Ole2ViewMac.rsc" : $(SOURCE) "$(INTDIR_SRC)"
   $(MRC) /o"$(INTDIR_SRC)/Ole2ViewMac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG"\
 /l 0x409 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\guids.cpp

!IF  "$(CFG)" == "Ole2View - Win32 Release"

DEP_CPP_GUIDS=\
	".\stdafx.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\guids.obj" : $(SOURCE) $(DEP_CPP_GUIDS) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

DEP_CPP_GUIDS=\
	".\stdafx.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\guids.obj" : $(SOURCE) $(DEP_CPP_GUIDS) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"

"$(INTDIR)\guids.sbr" : $(SOURCE) $(DEP_CPP_GUIDS) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

DEP_CPP_GUIDS=\
	".\stdafx.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\guids.obj" : $(SOURCE) $(DEP_CPP_GUIDS) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

DEP_CPP_GUIDS=\
	".\stdafx.h"\
	".\comcat.h"\
	

"$(INTDIR)\guids.obj" : $(SOURCE) $(DEP_CPP_GUIDS) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

"$(INTDIR)\guids.sbr" : $(SOURCE) $(DEP_CPP_GUIDS) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ServerInfoDlg.cpp

!IF  "$(CFG)" == "Ole2View - Win32 Release"

DEP_CPP_SERVE=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\ServerInfoDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\regview.h"\
	".\obj_vw.h"\
	

"$(INTDIR)\ServerInfoDlg.obj" : $(SOURCE) $(DEP_CPP_SERVE) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 Debug"

DEP_CPP_SERVE=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\ServerInfoDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\regview.h"\
	".\obj_vw.h"\
	

"$(INTDIR)\ServerInfoDlg.obj" : $(SOURCE) $(DEP_CPP_SERVE) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"

"$(INTDIR)\ServerInfoDlg.sbr" : $(SOURCE) $(DEP_CPP_SERVE) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Power Macintosh Debug"

DEP_CPP_SERVE=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\ServerInfoDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\regview.h"\
	".\obj_vw.h"\
	

"$(INTDIR)\ServerInfoDlg.obj" : $(SOURCE) $(DEP_CPP_SERVE) "$(INTDIR)"\
 "$(INTDIR)\ole2view.pch"


!ELSEIF  "$(CFG)" == "Ole2View - Win32 (PPC) Debug"

DEP_CPP_SERVE=\
	".\stdafx.h"\
	".\Ole2View.h"\
	".\ServerInfoDlg.h"\
	".\comcat.h"\
	".\IViewers\iview.h"\
	".\util.h"\
	".\MainFrm.h"\
	".\doc.h"\
	".\regview.h"\
	".\obj_vw.h"\
	

"$(INTDIR)\ServerInfoDlg.obj" : $(SOURCE) $(DEP_CPP_SERVE) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

"$(INTDIR)\ServerInfoDlg.sbr" : $(SOURCE) $(DEP_CPP_SERVE) "$(INTDIR)"\
 "$(INTDIR)\Ole2View.pch"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "IViewers - Win32 Release"
# Name "IViewers - Win32 Debug"
# Name "IViewers - Power Macintosh Debug"
# Name "IViewers - Win32 (PPC) Debug"

!IF  "$(CFG)" == "IViewers - Win32 Release"

!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\IViewers\ReadMe.txt

!IF  "$(CFG)" == "IViewers - Win32 Release"

!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\IViewers.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_IVIEW=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\typelib.h"\
	".\IViewers\iview.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iviewer.h"\
	

"$(INTDIR)\IViewers.obj" : $(SOURCE) $(DEP_CPP_IVIEW) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_IVIEW=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\typelib.h"\
	".\IViewers\iview.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iviewer.h"\
	

"$(INTDIR)\IViewers.obj" : $(SOURCE) $(DEP_CPP_IVIEW) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_IVIEW=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\typelib.h"\
	".\IViewers\iview.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iviewer.h"\
	

"$(INTDIR)\IViewers.obj" : $(SOURCE) $(DEP_CPP_IVIEW) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_IVIEW=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\typelib.h"\
	".\IViewers\iview.h"\
	".\IViewers\util.h"\
	".\comcat.h"\
	".\IViewers\iviewer.h"\
	

"$(INTDIR)\IViewers.obj" : $(SOURCE) $(DEP_CPP_IVIEW) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\IViewers.def

!IF  "$(CFG)" == "IViewers - Win32 Release"

!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\StdAfx.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_STDAF=\
	".\IViewers\StdAfx.h"\
	{$(INCLUDE)}"\comcat.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)/IViewers.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IViewers.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_STDAF=\
	".\IViewers\StdAfx.h"\
	{$(INCLUDE)}"\comcat.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W4 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL"\
 /Fp"$(INTDIR)/IViewers.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IViewers.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_STDAF=\
	".\IViewers\StdAfx.h"\
	{$(INCLUDE)}"\comcat.h"\
	
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W4 /WX /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D\
 "WIN32" /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "_AFXDLL"\
 /Fp"$(INTDIR)/IViewers.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IViewers.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_STDAF=\
	".\IViewers\StdAfx.h"\
	".\comcat.h"\
	
# ADD CPP /Yc"stdafx.h"
BuildCmds= \
	$(CPP) /nologo /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/IViewers.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IViewers.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\IViewers.rc

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_RSC_IVIEWE=\
	".\IViewers\res\images.bmp"\
	".\IViewers\res\toolbars.bmp"\
	".\IViewers\res\bitmap1.bmp"\
	".\IViewers\res\ico00001.ico"\
	".\IViewers\res\Ole2View.ico"\
	".\IViewers\res\IViewers.rc2"\
	

"$(INTDIR)\IViewers.res" : $(SOURCE) $(DEP_RSC_IVIEWE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/IViewers.res" /i "IViewers" /d "NDEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_RSC_IVIEWE=\
	".\IViewers\res\images.bmp"\
	".\IViewers\res\toolbars.bmp"\
	".\IViewers\res\bitmap1.bmp"\
	".\IViewers\res\ico00001.ico"\
	".\IViewers\res\Ole2View.ico"\
	".\IViewers\res\IViewers.rc2"\
	

"$(INTDIR)\IViewers.res" : $(SOURCE) $(DEP_RSC_IVIEWE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/IViewers.res" /i "IViewers" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_RSC_IVIEWE=\
	".\IViewers\res\images.bmp"\
	".\IViewers\res\toolbars.bmp"\
	".\IViewers\res\bitmap1.bmp"\
	".\IViewers\res\ico00001.ico"\
	".\IViewers\res\Ole2View.ico"\
	".\IViewers\res\IViewers.rc2"\
	

"$(INTDIR)\IViewers.rsc" : $(SOURCE) $(DEP_RSC_IVIEWE) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/IViewers.rsc" /i "IViewers" /d "_MAC" /d\
 "_MPPC_" /d "_DEBUG" /d "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_RSC_IVIEWE=\
	".\IViewers\res\images.bmp"\
	".\IViewers\res\toolbars.bmp"\
	".\IViewers\res\bitmap1.bmp"\
	".\IViewers\res\ico00001.ico"\
	".\IViewers\res\Ole2View.ico"\
	".\IViewers\res\IViewers.rc2"\
	

"$(INTDIR)\IViewers.res" : $(SOURCE) $(DEP_RSC_IVIEWE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/IViewers.res" /i "IViewers" /d "_DEBUG" /d\
 "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\typelib.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_TYPEL=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\typelib.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\util.h"\
	".\IViewers\tlbtree.h"\
	".\IViewers\tlbodl.h"\
	".\IViewers\AboutDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\typelib.obj" : $(SOURCE) $(DEP_CPP_TYPEL) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_TYPEL=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\typelib.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\util.h"\
	".\IViewers\tlbtree.h"\
	".\IViewers\tlbodl.h"\
	".\IViewers\AboutDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\typelib.obj" : $(SOURCE) $(DEP_CPP_TYPEL) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_TYPEL=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\typelib.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\util.h"\
	".\IViewers\tlbtree.h"\
	".\IViewers\tlbodl.h"\
	".\IViewers\AboutDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\typelib.obj" : $(SOURCE) $(DEP_CPP_TYPEL) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_TYPEL=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\typelib.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\util.h"\
	".\IViewers\tlbtree.h"\
	".\IViewers\tlbodl.h"\
	".\IViewers\AboutDlg.h"\
	".\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\typelib.obj" : $(SOURCE) $(DEP_CPP_TYPEL) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\tree.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_TREE_=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\tree.obj" : $(SOURCE) $(DEP_CPP_TREE_) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_TREE_=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\tree.obj" : $(SOURCE) $(DEP_CPP_TREE_) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_TREE_=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\tree.obj" : $(SOURCE) $(DEP_CPP_TREE_) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_TREE_=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	".\comcat.h"\
	

"$(INTDIR)\tree.obj" : $(SOURCE) $(DEP_CPP_TREE_) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\tlbtree.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_TLBTR=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\typelib.h"\
	".\IViewers\tlbtree.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\tlbtree.obj" : $(SOURCE) $(DEP_CPP_TLBTR) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_TLBTR=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\typelib.h"\
	".\IViewers\tlbtree.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\tlbtree.obj" : $(SOURCE) $(DEP_CPP_TLBTR) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_TLBTR=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\typelib.h"\
	".\IViewers\tlbtree.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\tlbtree.obj" : $(SOURCE) $(DEP_CPP_TLBTR) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_TLBTR=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\typelib.h"\
	".\IViewers\tlbtree.h"\
	".\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\tlbtree.obj" : $(SOURCE) $(DEP_CPP_TLBTR) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\tlbodl.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_TLBOD=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\util.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\typelib.h"\
	".\IViewers\tlbodl.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\tlbodl.obj" : $(SOURCE) $(DEP_CPP_TLBOD) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_TLBOD=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\util.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\typelib.h"\
	".\IViewers\tlbodl.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\tlbodl.obj" : $(SOURCE) $(DEP_CPP_TLBOD) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_TLBOD=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\util.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\typelib.h"\
	".\IViewers\tlbodl.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\tlbodl.obj" : $(SOURCE) $(DEP_CPP_TLBOD) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_TLBOD=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\util.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\typelib.h"\
	".\IViewers\tlbodl.h"\
	".\comcat.h"\
	

"$(INTDIR)\tlbodl.obj" : $(SOURCE) $(DEP_CPP_TLBOD) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\iviewer.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_IVIEWER=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iviewer.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\iviewer.obj" : $(SOURCE) $(DEP_CPP_IVIEWER) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_IVIEWER=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iviewer.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\iviewer.obj" : $(SOURCE) $(DEP_CPP_IVIEWER) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_IVIEWER=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iviewer.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\iviewer.obj" : $(SOURCE) $(DEP_CPP_IVIEWER) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_IVIEWER=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iviewer.h"\
	".\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\iviewer.obj" : $(SOURCE) $(DEP_CPP_IVIEWER) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\IUnknown.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_IUNKN=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\IUnknown.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\IUnknown.obj" : $(SOURCE) $(DEP_CPP_IUNKN) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_IUNKN=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\IUnknown.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\IUnknown.obj" : $(SOURCE) $(DEP_CPP_IUNKN) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_IUNKN=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\IUnknown.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\IUnknown.obj" : $(SOURCE) $(DEP_CPP_IUNKN) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_IUNKN=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\IUnknown.h"\
	".\IViewers\util.h"\
	".\comcat.h"\
	

"$(INTDIR)\IUnknown.obj" : $(SOURCE) $(DEP_CPP_IUNKN) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\idataobj.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_IDATA=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\util.h"\
	".\IViewers\idataobj.h"\
	".\IViewers\iadvsink.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\idataobj.obj" : $(SOURCE) $(DEP_CPP_IDATA) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_IDATA=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\util.h"\
	".\IViewers\idataobj.h"\
	".\IViewers\iadvsink.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\idataobj.obj" : $(SOURCE) $(DEP_CPP_IDATA) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_IDATA=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\util.h"\
	".\IViewers\idataobj.h"\
	".\IViewers\iadvsink.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\idataobj.obj" : $(SOURCE) $(DEP_CPP_IDATA) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_IDATA=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\util.h"\
	".\IViewers\idataobj.h"\
	".\IViewers\iadvsink.h"\
	".\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\idataobj.obj" : $(SOURCE) $(DEP_CPP_IDATA) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\iadvsink.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_IADVS=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\util.h"\
	".\IViewers\idataobj.h"\
	".\IViewers\iadvsink.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\iadvsink.obj" : $(SOURCE) $(DEP_CPP_IADVS) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_IADVS=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\util.h"\
	".\IViewers\idataobj.h"\
	".\IViewers\iadvsink.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\iadvsink.obj" : $(SOURCE) $(DEP_CPP_IADVS) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_IADVS=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\util.h"\
	".\IViewers\idataobj.h"\
	".\IViewers\iadvsink.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\iadvsink.obj" : $(SOURCE) $(DEP_CPP_IADVS) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_IADVS=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\util.h"\
	".\IViewers\idataobj.h"\
	".\IViewers\iadvsink.h"\
	".\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\iadvsink.obj" : $(SOURCE) $(DEP_CPP_IADVS) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\guid.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_GUID_=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\guid.obj" : $(SOURCE) $(DEP_CPP_GUID_) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_GUID_=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\guid.obj" : $(SOURCE) $(DEP_CPP_GUID_) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_GUID_=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\guid.obj" : $(SOURCE) $(DEP_CPP_GUID_) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_GUID_=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\guid.obj" : $(SOURCE) $(DEP_CPP_GUID_) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\dispatch.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_DISPA=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\dispatch.obj" : $(SOURCE) $(DEP_CPP_DISPA) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_DISPA=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\dispatch.obj" : $(SOURCE) $(DEP_CPP_DISPA) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_DISPA=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\dispatch.obj" : $(SOURCE) $(DEP_CPP_DISPA) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_DISPA=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\util.h"\
	".\comcat.h"\
	

"$(INTDIR)\dispatch.obj" : $(SOURCE) $(DEP_CPP_DISPA) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\dataobj.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_DATAO=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\dataobj.h"\
	".\IViewers\util.h"\
	".\IViewers\UniformDataTransfer.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iviewer.h"\
	

"$(INTDIR)\dataobj.obj" : $(SOURCE) $(DEP_CPP_DATAO) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_DATAO=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\dataobj.h"\
	".\IViewers\util.h"\
	".\IViewers\UniformDataTransfer.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iviewer.h"\
	

"$(INTDIR)\dataobj.obj" : $(SOURCE) $(DEP_CPP_DATAO) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_DATAO=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\dataobj.h"\
	".\IViewers\util.h"\
	".\IViewers\UniformDataTransfer.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iviewer.h"\
	

"$(INTDIR)\dataobj.obj" : $(SOURCE) $(DEP_CPP_DATAO) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_DATAO=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\dataobj.h"\
	".\IViewers\util.h"\
	".\IViewers\UniformDataTransfer.h"\
	".\comcat.h"\
	".\IViewers\iviewer.h"\
	

"$(INTDIR)\dataobj.obj" : $(SOURCE) $(DEP_CPP_DATAO) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\util.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_UTIL_=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_UTIL_=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_UTIL_=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_UTIL_=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	".\comcat.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\AboutDlg.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_ABOUT=\
	".\IViewers\StdAfx.h"\
	".\IViewers\AboutDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\AboutDlg.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_ABOUT=\
	".\IViewers\StdAfx.h"\
	".\IViewers\AboutDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\AboutDlg.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_ABOUT=\
	".\IViewers\StdAfx.h"\
	".\IViewers\AboutDlg.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\AboutDlg.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_ABOUT=\
	".\IViewers\StdAfx.h"\
	".\IViewers\AboutDlg.h"\
	".\comcat.h"\
	

"$(INTDIR)\AboutDlg.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\UniformDataTransfer.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_UNIFO=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\UniformDataTransfer.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\UniformDataTransfer.obj" : $(SOURCE) $(DEP_CPP_UNIFO) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_UNIFO=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\UniformDataTransfer.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\UniformDataTransfer.obj" : $(SOURCE) $(DEP_CPP_UNIFO) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_UNIFO=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\UniformDataTransfer.h"\
	{$(INCLUDE)}"\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\UniformDataTransfer.obj" : $(SOURCE) $(DEP_CPP_UNIFO) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_UNIFO=\
	".\IViewers\StdAfx.h"\
	".\IViewers\util.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\UniformDataTransfer.h"\
	".\comcat.h"\
	".\IViewers\iview.h"\
	

"$(INTDIR)\UniformDataTransfer.obj" : $(SOURCE) $(DEP_CPP_UNIFO) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\IViewers.r

!IF  "$(CFG)" == "IViewers - Win32 Release"

!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

# ADD MRC /o"IViewers\PMacDbg/IViewersMac.rsc"

"$(INTDIR)\IViewersMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/IViewersMac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l\
 0x409 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IViewers\IPersist.cpp

!IF  "$(CFG)" == "IViewers - Win32 Release"

DEP_CPP_IPERS=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\IPersist.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\IPersist.obj" : $(SOURCE) $(DEP_CPP_IPERS) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 Debug"

DEP_CPP_IPERS=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\IPersist.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\IPersist.obj" : $(SOURCE) $(DEP_CPP_IPERS) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Power Macintosh Debug"

DEP_CPP_IPERS=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\IPersist.h"\
	".\IViewers\util.h"\
	{$(INCLUDE)}"\comcat.h"\
	

"$(INTDIR)\IPersist.obj" : $(SOURCE) $(DEP_CPP_IPERS) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "IViewers - Win32 (PPC) Debug"

DEP_CPP_IPERS=\
	".\IViewers\StdAfx.h"\
	".\IViewers\IViewers.h"\
	".\IViewers\iview.h"\
	".\IViewers\iviewer.h"\
	".\IViewers\IPersist.h"\
	".\IViewers\util.h"\
	".\comcat.h"\
	

"$(INTDIR)\IPersist.obj" : $(SOURCE) $(DEP_CPP_IPERS) "$(INTDIR)"\
 "$(INTDIR)\IViewers.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
################################################################################
# Section IViewers : {3B7C8860-D78F-101B-B9B5-04021C009402}
# 	0:8:Font.cpp:G:\SOURCE\Ole2View\IViewers\Font.cpp
# 	0:9:Picture.h:G:\SOURCE\Ole2View\IViewers\Picture.h
# 	0:6:Font.h:G:\SOURCE\Ole2View\IViewers\Font.h
# 	0:11:Picture.cpp:G:\SOURCE\Ole2View\IViewers\Picture.cpp
# 	0:12:Richtext.cpp:G:\SOURCE\Ole2View\IViewers\Richtext.cpp
# 	0:10:Richtext.h:G:\SOURCE\Ole2View\IViewers\Richtext.h
# 	2:21:DefaultSinkHeaderFile:richtext.h
# 	2:16:DefaultSinkClass:CRichText
# End Section
################################################################################
################################################################################
# Section IViewers : {7BF80981-BF32-101A-8BBB-00AA00300CAB}
# 	2:5:Class:CPicture
# 	2:10:HeaderFile:picture.h
# 	2:8:ImplFile:picture.cpp
# End Section
################################################################################
################################################################################
# Section OLE Controls
# 	{3B7C8860-D78F-101B-B9B5-04021C009402}
# End Section
################################################################################
################################################################################
# Section IViewers : {3B7C8861-D78F-101B-B9B5-04021C009402}
# 	2:5:Class:CRichText
# 	2:10:HeaderFile:richtext.h
# 	2:8:ImplFile:richtext.cpp
# End Section
################################################################################
################################################################################
# Section IViewers : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:COleFont
# 	2:10:HeaderFile:font.h
# 	2:8:ImplFile:font.cpp
# End Section
################################################################################
################################################################################
# Section Ole2View : {E91E9D17-F8F7-11CE-9059-080036F12502}
# 	0:12:Ole2View.cpp:G:\SOURCE\Ole2View\Ole2View1.cpp
# 	1:13:IDD_ABOUTBOX1:116
# 	2:16:Resource Include:resource.h
# 	2:25:FUNC: RegisterIViewersDLL:RegisterIViewersDLL
# 	2:17:FUNC: FreeABitmap:FreeABitmap
# 	2:11:VAR: g_osvi:g_osvi
# 	2:10:ENUM: enum:enum
# 	2:16:CLASS: CAboutDlg:CAboutDlg
# 	2:11:VAR: theApp:theApp
# 	2:19:Application Include:ole2view.h
# 	2:12:IDD_ABOUTBOX:IDD_ABOUTBOX1
# 	2:12:Ole2View.cpp:Ole2View1.cpp
# End Section
################################################################################
