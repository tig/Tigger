# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (PPC) Application" 0x0701

!IF "$(CFG)" == ""
CFG=XRTFrm32 - Win32 (PPC) Debug
!MESSAGE No configuration specified.  Defaulting to XRTFrm32 - Win32 (PPC)\
 Debug.
!ENDIF 

!IF "$(CFG)" != "XRTFrm32 - Win32 Debug" && "$(CFG)" !=\
 "XRTFrm32 - Win32 Release" && "$(CFG)" != "XRTFrm32 - Win32 (PPC) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "XRTFrm32.MAK" CFG="XRTFrm32 - Win32 (PPC) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XRTFrm32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "XRTFrm32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "XRTFrm32 - Win32 (PPC) Debug" (based on "Win32 (PPC) Application")
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
# PROP Target_Last_Scanned "XRTFrm32 - Win32 Debug"

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\XRTFrm32.exe"

CLEAN : 
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\vc40.idb"
	-@erase ".\WinDebug\XRTFrm32.exe"
	-@erase ".\WinDebug\dataitms.obj"
	-@erase ".\WinDebug\srvritem.obj"
	-@erase ".\WinDebug\properti.obj"
	-@erase ".\WinDebug\simobj.obj"
	-@erase ".\WinDebug\wosaxrt.obj"
	-@erase ".\WinDebug\collist.obj"
	-@erase ".\WinDebug\doc.obj"
	-@erase ".\WinDebug\view.obj"
	-@erase ".\WinDebug\stdafx.obj"
	-@erase ".\WinDebug\property.obj"
	-@erase ".\WinDebug\dataitem.obj"
	-@erase ".\WinDebug\xrtframe.obj"
	-@erase ".\WinDebug\requests.obj"
	-@erase ".\WinDebug\ipframe.obj"
	-@erase ".\WinDebug\mainfrm.obj"
	-@erase ".\WinDebug\request.obj"
	-@erase ".\WinDebug\xrtframe.res"
	-@erase ".\WinDebug\XRTFrm32.ilk"
	-@erase ".\WinDebug\XRTFrm32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/XRTFrm32.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
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
# ADD RSC /l 0x409 /d "_DEBUG" /d "WIN32" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/xrtframe.res" /d "_DEBUG" /d "WIN32" /d\
 "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/XRTFrm32.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/XRTFrm32.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/XRTFrm32.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dataitms.obj" \
	"$(INTDIR)/srvritem.obj" \
	"$(INTDIR)/properti.obj" \
	"$(INTDIR)/simobj.obj" \
	"$(INTDIR)/wosaxrt.obj" \
	"$(INTDIR)/collist.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/property.obj" \
	"$(INTDIR)/dataitem.obj" \
	"$(INTDIR)/xrtframe.obj" \
	"$(INTDIR)/requests.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/request.obj" \
	"$(INTDIR)/xrtframe.res"

"$(OUTDIR)\XRTFrm32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\XRTFrm32.exe"

CLEAN : 
	-@erase ".\WinRel\XRTFrm32.exe"
	-@erase ".\WinRel\view.obj"
	-@erase ".\WinRel\XRTFrm32.pch"
	-@erase ".\WinRel\simobj.obj"
	-@erase ".\WinRel\collist.obj"
	-@erase ".\WinRel\doc.obj"
	-@erase ".\WinRel\stdafx.obj"
	-@erase ".\WinRel\dataitem.obj"
	-@erase ".\WinRel\xrtframe.obj"
	-@erase ".\WinRel\requests.obj"
	-@erase ".\WinRel\mainfrm.obj"
	-@erase ".\WinRel\properti.obj"
	-@erase ".\WinRel\request.obj"
	-@erase ".\WinRel\dataitms.obj"
	-@erase ".\WinRel\srvritem.obj"
	-@erase ".\WinRel\ipframe.obj"
	-@erase ".\WinRel\wosaxrt.obj"
	-@erase ".\WinRel\property.obj"
	-@erase ".\WinRel\xrtframe.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W4 /WX /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W4 /WX /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/XRTFrm32.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\WinRel/
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
# ADD RSC /l 0x409 /d "NDEBUG" /d "WIN32" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/xrtframe.res" /d "NDEBUG" /d "WIN32" /d\
 "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/XRTFrm32.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/XRTFrm32.pdb" /machine:I386 /out:"$(OUTDIR)/XRTFrm32.exe" 
LINK32_OBJS= \
	"$(INTDIR)/view.obj" \
	"$(INTDIR)/simobj.obj" \
	"$(INTDIR)/collist.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/dataitem.obj" \
	"$(INTDIR)/xrtframe.obj" \
	"$(INTDIR)/requests.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/properti.obj" \
	"$(INTDIR)/request.obj" \
	"$(INTDIR)/dataitms.obj" \
	"$(INTDIR)/srvritem.obj" \
	"$(INTDIR)/ipframe.obj" \
	"$(INTDIR)/wosaxrt.obj" \
	"$(INTDIR)/property.obj" \
	"$(INTDIR)/xrtframe.res"

"$(OUTDIR)\XRTFrm32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "XRTFrm32"
# PROP BASE Intermediate_Dir "XRTFrm32"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "XRTFrm32"
# PROP Intermediate_Dir "XRTFrm32"
# PROP Target_Dir ""
OUTDIR=.\XRTFrm32
INTDIR=.\XRTFrm32

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

ALL : "$(OUTDIR)\XRTFrm32.exe"

CLEAN : 
	-@erase ".\XRTFrm32\vc40.pdb"
	-@erase ".\XRTFrm32\XRTFrm32.pch"
	-@erase ".\XRTFrm32\XRTFrm32.exe"
	-@erase ".\XRTFrm32\stdafx.obj"
	-@erase ".\XRTFrm32\xrtframe.obj"
	-@erase ".\XRTFrm32\mainfrm.obj"
	-@erase ".\XRTFrm32\ipframe.obj"
	-@erase ".\XRTFrm32\srvritem.obj"
	-@erase ".\XRTFrm32\collist.obj"
	-@erase ".\XRTFrm32\view.obj"
	-@erase ".\XRTFrm32\wosaxrt.obj"
	-@erase ".\XRTFrm32\property.obj"
	-@erase ".\XRTFrm32\request.obj"
	-@erase ".\XRTFrm32\properti.obj"
	-@erase ".\XRTFrm32\dataitem.obj"
	-@erase ".\XRTFrm32\requests.obj"
	-@erase ".\XRTFrm32\doc.obj"
	-@erase ".\XRTFrm32\simobj.obj"
	-@erase ".\XRTFrm32\dataitms.obj"
	-@erase ".\XRTFrm32\xrtframe.res"
	-@erase ".\XRTFrm32\XRTFrm32.pdb"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /PPC32
# ADD MTL /nologo /D "_DEBUG" /PPC32
MTL_PROJ=/nologo /D "_DEBUG" /PPC32 
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/XRTFrm32.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\XRTFrm32/
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
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "WIN32"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/xrtframe.res" /d "_DEBUG" /d "_AFXDLL" /d\
 "WIN32" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/XRTFrm32.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:PPC
# ADD LINK32 /nologo /subsystem:windows /debug /machine:PPC
LINK32_FLAGS=/nologo /subsystem:windows /pdb:"$(OUTDIR)/XRTFrm32.pdb" /debug\
 /machine:PPC /out:"$(OUTDIR)/XRTFrm32.exe" 
LINK32_OBJS= \
	".\XRTFrm32\stdafx.obj" \
	".\XRTFrm32\xrtframe.obj" \
	".\XRTFrm32\mainfrm.obj" \
	".\XRTFrm32\ipframe.obj" \
	".\XRTFrm32\srvritem.obj" \
	".\XRTFrm32\collist.obj" \
	".\XRTFrm32\view.obj" \
	".\XRTFrm32\wosaxrt.obj" \
	".\XRTFrm32\property.obj" \
	".\XRTFrm32\request.obj" \
	".\XRTFrm32\properti.obj" \
	".\XRTFrm32\dataitem.obj" \
	".\XRTFrm32\requests.obj" \
	".\XRTFrm32\doc.obj" \
	".\XRTFrm32\simobj.obj" \
	".\XRTFrm32\dataitms.obj" \
	".\XRTFrm32\xrtframe.res"

"$(OUTDIR)\XRTFrm32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "XRTFrm32 - Win32 Debug"
# Name "XRTFrm32 - Win32 Release"
# Name "XRTFrm32 - Win32 (PPC) Debug"

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"
   $(CPP) /nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/XRTFrm32.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W4 /WX /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/XRTFrm32.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\XRTFrm32.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD CPP /Yc"stdafx.h"
BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/XRTFrm32.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\XRTFrm32.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xrtframe.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_XRTFR=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\mainfrm.h"\
	".\ipframe.h"\
	".\doc.h"\
	".\view.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\xrtframe.obj" : $(SOURCE) $(DEP_CPP_XRTFR) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_XRTFR=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\mainfrm.h"\
	".\ipframe.h"\
	".\doc.h"\
	".\view.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\xrtframe.obj" : $(SOURCE) $(DEP_CPP_XRTFR) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_XRTFR=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\mainfrm.h"\
	".\ipframe.h"\
	".\doc.h"\
	".\view.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\xrtframe.obj" : $(SOURCE) $(DEP_CPP_XRTFR) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\simobj.h"\
	".\mainfrm.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\simobj.h"\
	".\mainfrm.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\simobj.h"\
	".\mainfrm.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ipframe.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_IPFRA=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\ipframe.h"\
	

"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_IPFRA=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\ipframe.h"\
	

"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_IPFRA=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\ipframe.h"\
	

"$(INTDIR)\ipframe.obj" : $(SOURCE) $(DEP_CPP_IPFRA) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\srvritem.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_SRVRI=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\view.h"\
	".\srvritem.h"\
	".\dataitem.h"\
	".\request.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\srvritem.obj" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_SRVRI=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\view.h"\
	".\srvritem.h"\
	".\dataitem.h"\
	".\request.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\srvritem.obj" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_SRVRI=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\view.h"\
	".\srvritem.h"\
	".\dataitem.h"\
	".\request.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\srvritem.obj" : $(SOURCE) $(DEP_CPP_SRVRI) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\xrtframe.rc

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"


"$(INTDIR)\xrtframe.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_RSC_XRTFRA=\
	".\RES\XRTFRAME.ICO"\
	".\RES\DOC.ICO"\
	".\RES\TOOLBAR.BMP"\
	".\RES\ITOOLBAR.BMP"\
	".\RES\SIZEHORZ.CUR"\
	".\RES\XRTFRAME.RC2"\
	

"$(INTDIR)\xrtframe.res" : $(SOURCE) $(DEP_RSC_XRTFRA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_RSC_XRTFRA=\
	".\RES\XRTFRAME.ICO"\
	".\RES\DOC.ICO"\
	".\RES\TOOLBAR.BMP"\
	".\RES\ITOOLBAR.BMP"\
	".\RES\SIZEHORZ.CUR"\
	".\RES\XRTFRAME.RC2"\
	

"$(INTDIR)\xrtframe.res" : $(SOURCE) $(DEP_RSC_XRTFRA) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/xrtframe.res" /d "_DEBUG" /d "_AFXDLL" /d\
 "WIN32" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\collist.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_COLLI=\
	".\stdafx.h"\
	".\collist.h"\
	

"$(INTDIR)\collist.obj" : $(SOURCE) $(DEP_CPP_COLLI) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_COLLI=\
	".\stdafx.h"\
	".\collist.h"\
	

"$(INTDIR)\collist.obj" : $(SOURCE) $(DEP_CPP_COLLI) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_COLLI=\
	".\stdafx.h"\
	".\collist.h"\
	

"$(INTDIR)\collist.obj" : $(SOURCE) $(DEP_CPP_COLLI) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\view.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_VIEW_=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\view.h"\
	".\srvritem.h"\
	".\request.h"\
	".\properti.h"\
	".\property.h"\
	".\dataitem.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_VIEW_=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\view.h"\
	".\srvritem.h"\
	".\request.h"\
	".\properti.h"\
	".\property.h"\
	".\dataitem.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_VIEW_=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\view.h"\
	".\srvritem.h"\
	".\request.h"\
	".\properti.h"\
	".\property.h"\
	".\dataitem.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\view.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wosaxrt.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_WOSAX=\
	".\stdafx.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\wosaxrt.obj" : $(SOURCE) $(DEP_CPP_WOSAX) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_WOSAX=\
	".\stdafx.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\wosaxrt.obj" : $(SOURCE) $(DEP_CPP_WOSAX) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_WOSAX=\
	".\stdafx.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\wosaxrt.obj" : $(SOURCE) $(DEP_CPP_WOSAX) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\property.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_PROPE=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\view.h"\
	".\property.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\property.obj" : $(SOURCE) $(DEP_CPP_PROPE) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_PROPE=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\view.h"\
	".\property.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\property.obj" : $(SOURCE) $(DEP_CPP_PROPE) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_PROPE=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\view.h"\
	".\property.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\property.obj" : $(SOURCE) $(DEP_CPP_PROPE) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\request.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_REQUE=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\request.h"\
	".\properti.h"\
	".\property.h"\
	".\dataitem.h"\
	".\doc.h"\
	".\view.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\request.obj" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_REQUE=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\request.h"\
	".\properti.h"\
	".\property.h"\
	".\dataitem.h"\
	".\doc.h"\
	".\view.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\request.obj" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_REQUE=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\request.h"\
	".\properti.h"\
	".\property.h"\
	".\dataitem.h"\
	".\doc.h"\
	".\view.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\request.obj" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\properti.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_PROPER=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\property.h"\
	".\properti.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\properti.obj" : $(SOURCE) $(DEP_CPP_PROPER) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_PROPER=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\property.h"\
	".\properti.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\properti.obj" : $(SOURCE) $(DEP_CPP_PROPER) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_PROPER=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\property.h"\
	".\properti.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\properti.obj" : $(SOURCE) $(DEP_CPP_PROPER) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dataitem.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_DATAI=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\collist.h"\
	".\request.h"\
	".\properti.h"\
	".\property.h"\
	".\dataitem.h"\
	".\view.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\dataitem.obj" : $(SOURCE) $(DEP_CPP_DATAI) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_DATAI=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\collist.h"\
	".\request.h"\
	".\properti.h"\
	".\property.h"\
	".\dataitem.h"\
	".\view.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\dataitem.obj" : $(SOURCE) $(DEP_CPP_DATAI) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_DATAI=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\collist.h"\
	".\request.h"\
	".\properti.h"\
	".\property.h"\
	".\dataitem.h"\
	".\view.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\dataitem.obj" : $(SOURCE) $(DEP_CPP_DATAI) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\requests.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_REQUES=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\requests.h"\
	".\request.h"\
	".\property.h"\
	".\properti.h"\
	".\doc.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\requests.obj" : $(SOURCE) $(DEP_CPP_REQUES) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_REQUES=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\requests.h"\
	".\request.h"\
	".\property.h"\
	".\properti.h"\
	".\doc.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\requests.obj" : $(SOURCE) $(DEP_CPP_REQUES) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_REQUES=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\requests.h"\
	".\request.h"\
	".\property.h"\
	".\properti.h"\
	".\doc.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\requests.obj" : $(SOURCE) $(DEP_CPP_REQUES) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\doc.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_DOC_C=\
	".\stdafx.h"\
	".\wosaxrt.h"\
	".\xrtframe.h"\
	".\mainfrm.h"\
	".\collist.h"\
	".\doc.h"\
	".\srvritem.h"\
	".\request.h"\
	".\requests.h"\
	".\property.h"\
	".\properti.h"\
	".\simobj.h"\
	".\dataitem.h"\
	".\dataitms.h"\
	".\view.h"\
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_DOC_C=\
	".\stdafx.h"\
	".\wosaxrt.h"\
	".\xrtframe.h"\
	".\mainfrm.h"\
	".\collist.h"\
	".\doc.h"\
	".\srvritem.h"\
	".\request.h"\
	".\requests.h"\
	".\property.h"\
	".\properti.h"\
	".\simobj.h"\
	".\dataitem.h"\
	".\dataitms.h"\
	".\view.h"\
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_DOC_C=\
	".\stdafx.h"\
	".\wosaxrt.h"\
	".\xrtframe.h"\
	".\mainfrm.h"\
	".\collist.h"\
	".\doc.h"\
	".\srvritem.h"\
	".\request.h"\
	".\requests.h"\
	".\property.h"\
	".\properti.h"\
	".\simobj.h"\
	".\dataitem.h"\
	".\dataitms.h"\
	".\view.h"\
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\simobj.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_SIMOB=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\simobj.h"\
	".\collist.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\simobj.obj" : $(SOURCE) $(DEP_CPP_SIMOB) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_SIMOB=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\simobj.h"\
	".\collist.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\simobj.obj" : $(SOURCE) $(DEP_CPP_SIMOB) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_SIMOB=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\doc.h"\
	".\simobj.h"\
	".\collist.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\simobj.obj" : $(SOURCE) $(DEP_CPP_SIMOB) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dataitms.cpp

!IF  "$(CFG)" == "XRTFrm32 - Win32 Debug"

DEP_CPP_DATAIT=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\dataitms.h"\
	".\dataitem.h"\
	".\doc.h"\
	".\property.h"\
	".\properti.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\dataitms.obj" : $(SOURCE) $(DEP_CPP_DATAIT) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 Release"

DEP_CPP_DATAIT=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\dataitms.h"\
	".\dataitem.h"\
	".\doc.h"\
	".\property.h"\
	".\properti.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\dataitms.obj" : $(SOURCE) $(DEP_CPP_DATAIT) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"


!ELSEIF  "$(CFG)" == "XRTFrm32 - Win32 (PPC) Debug"

DEP_CPP_DATAIT=\
	".\stdafx.h"\
	".\xrtframe.h"\
	".\dataitms.h"\
	".\dataitem.h"\
	".\doc.h"\
	".\property.h"\
	".\properti.h"\
	".\collist.h"\
	".\simobj.h"\
	".\wosaxrt.h"\
	

"$(INTDIR)\dataitms.obj" : $(SOURCE) $(DEP_CPP_DATAIT) "$(INTDIR)"\
 "$(INTDIR)\XRTFrm32.pch"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
