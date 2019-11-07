# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=StgEdit - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to StgEdit - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "StgEdit - Win32 Release" && "$(CFG)" !=\
 "StgEdit - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "STGEDIT.MAK" CFG="StgEdit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "StgEdit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "StgEdit - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "StgEdit - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "StgEdit - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\STGEDIT.exe"

CLEAN : 
	-@erase ".\Release\STGEDIT.exe"
	-@erase ".\Release\StgEditItem.obj"
	-@erase ".\Release\STGEDIT.pch"
	-@erase ".\Release\MAINFRM.OBJ"
	-@erase ".\Release\STDAFX.OBJ"
	-@erase ".\Release\PROPSET.OBJ"
	-@erase ".\Release\splitter.obj"
	-@erase ".\Release\ContentsView.obj"
	-@erase ".\Release\StgEditView.obj"
	-@erase ".\Release\StgEditDoc.obj"
	-@erase ".\Release\StgEdit.obj"
	-@erase ".\Release\StreamView.obj"
	-@erase ".\Release\StgEdit.res"
	-@erase ".\Release\PropView.obj"
	-@erase ".\Release\PropSetView.obj"
	-@erase ".\Release\StorageView.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/STGEDIT.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/StgEdit.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/STGEDIT.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/STGEDIT.pdb" /machine:I386 /out:"$(OUTDIR)/STGEDIT.exe" 
LINK32_OBJS= \
	"$(INTDIR)/StgEditItem.obj" \
	"$(INTDIR)/MAINFRM.OBJ" \
	"$(INTDIR)/STDAFX.OBJ" \
	"$(INTDIR)/PROPSET.OBJ" \
	"$(INTDIR)/splitter.obj" \
	"$(INTDIR)/ContentsView.obj" \
	"$(INTDIR)/StgEditView.obj" \
	"$(INTDIR)/StgEditDoc.obj" \
	"$(INTDIR)/StgEdit.obj" \
	"$(INTDIR)/StreamView.obj" \
	"$(INTDIR)/PropView.obj" \
	"$(INTDIR)/PropSetView.obj" \
	"$(INTDIR)/StorageView.obj" \
	"$(INTDIR)/StgEdit.res"

"$(OUTDIR)\STGEDIT.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\STGEDIT.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\STGEDIT.pch"
	-@erase ".\Debug\STGEDIT.exe"
	-@erase ".\Debug\StgEditDoc.obj"
	-@erase ".\Debug\StreamView.obj"
	-@erase ".\Debug\MAINFRM.OBJ"
	-@erase ".\Debug\STDAFX.OBJ"
	-@erase ".\Debug\StgEditView.obj"
	-@erase ".\Debug\PROPSET.OBJ"
	-@erase ".\Debug\StgEditItem.obj"
	-@erase ".\Debug\ContentsView.obj"
	-@erase ".\Debug\StgEdit.obj"
	-@erase ".\Debug\splitter.obj"
	-@erase ".\Debug\StgEdit.res"
	-@erase ".\Debug\PropView.obj"
	-@erase ".\Debug\PropSetView.obj"
	-@erase ".\Debug\StorageView.obj"
	-@erase ".\Debug\STGEDIT.ilk"
	-@erase ".\Debug\STGEDIT.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/STGEDIT.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/StgEdit.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/STGEDIT.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/STGEDIT.pdb" /debug /machine:I386 /out:"$(OUTDIR)/STGEDIT.exe" 
LINK32_OBJS= \
	"$(INTDIR)/StgEditDoc.obj" \
	"$(INTDIR)/StreamView.obj" \
	"$(INTDIR)/MAINFRM.OBJ" \
	"$(INTDIR)/STDAFX.OBJ" \
	"$(INTDIR)/StgEditView.obj" \
	"$(INTDIR)/PROPSET.OBJ" \
	"$(INTDIR)/StgEditItem.obj" \
	"$(INTDIR)/ContentsView.obj" \
	"$(INTDIR)/StgEdit.obj" \
	"$(INTDIR)/splitter.obj" \
	"$(INTDIR)/PropView.obj" \
	"$(INTDIR)/PropSetView.obj" \
	"$(INTDIR)/StorageView.obj" \
	"$(INTDIR)/StgEdit.res"

"$(OUTDIR)\STGEDIT.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "StgEdit - Win32 Release"
# Name "StgEdit - Win32 Debug"

!IF  "$(CFG)" == "StgEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "StgEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StgEdit.CPP
DEP_CPP_STGED=\
	".\MAINFRM.H"\
	".\splitter.h"\
	".\StgEdit.H"\
	".\StgEditDoc.H"\
	".\StgEditView.H"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "StgEdit - Win32 Release"


"$(INTDIR)\StgEdit.obj" : $(SOURCE) $(DEP_CPP_STGED) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"


"$(INTDIR)\StgEdit.obj" : $(SOURCE) $(DEP_CPP_STGED) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STGED) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STDAFX.CPP
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "StgEdit - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/STGEDIT.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\STGEDIT.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/STGEDIT.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\STGEDIT.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MAINFRM.CPP
DEP_CPP_MAINF=\
	".\ContentsView.h"\
	".\MAINFRM.H"\
	".\splitter.h"\
	".\StgEdit.H"\
	".\StorageView.h"\
	".\StreamView.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "StgEdit - Win32 Release"


"$(INTDIR)\MAINFRM.OBJ" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"


"$(INTDIR)\MAINFRM.OBJ" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StgEditDoc.CPP
DEP_CPP_STGEDI=\
	".\StgEdit.H"\
	".\StgEditDoc.H"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "StgEdit - Win32 Release"


"$(INTDIR)\StgEditDoc.obj" : $(SOURCE) $(DEP_CPP_STGEDI) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"


"$(INTDIR)\StgEditDoc.obj" : $(SOURCE) $(DEP_CPP_STGEDI) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STGEDI) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StgEditView.CPP

!IF  "$(CFG)" == "StgEdit - Win32 Release"

DEP_CPP_STGEDIT=\
	".\ContentsView.h"\
	".\PropSetView.h"\
	".\PropView.h"\
	".\StgEdit.H"\
	".\StgEditDoc.H"\
	".\StgEditItem.H"\
	".\StgEditView.H"\
	".\StorageView.h"\
	".\StreamView.h"\
	".\MAINFRM.H"\
	".\splitter.h"\
	".\PROPSET.H"\
	".\StdAfx.h"\
	

"$(INTDIR)\StgEditView.obj" : $(SOURCE) $(DEP_CPP_STGEDIT) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"

DEP_CPP_STGEDIT=\
	".\ContentsView.h"\
	".\PropSetView.h"\
	".\PropView.h"\
	".\StgEdit.H"\
	".\StgEditDoc.H"\
	".\StgEditItem.H"\
	".\PROPSET.H"\
	".\StgEditView.H"\
	".\StorageView.h"\
	".\StreamView.h"\
	".\MAINFRM.H"\
	".\splitter.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\StgEditView.obj" : $(SOURCE) $(DEP_CPP_STGEDIT) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STGEDIT) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StgEdit.rc
DEP_RSC_STGEDIT_=\
	".\res\StgEdit.ico"\
	".\res\StgEditDoc.ico"\
	".\res\Toolbar.bmp"\
	".\res\bitmap1.bmp"\
	".\res\StgEdit.rc2"\
	

"$(INTDIR)\StgEdit.res" : $(SOURCE) $(DEP_RSC_STGEDIT_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ContentsView.CPP
DEP_CPP_CONTE=\
	".\ContentsView.h"\
	".\StgEdit.H"\
	".\PROPSET.H"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "StgEdit - Win32 Release"


"$(INTDIR)\ContentsView.obj" : $(SOURCE) $(DEP_CPP_CONTE) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"


"$(INTDIR)\ContentsView.obj" : $(SOURCE) $(DEP_CPP_CONTE) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CONTE) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StgEditItem.CPP
DEP_CPP_STGEDITI=\
	".\StgEdit.H"\
	".\StgEditItem.H"\
	".\PROPSET.H"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "StgEdit - Win32 Release"


"$(INTDIR)\StgEditItem.obj" : $(SOURCE) $(DEP_CPP_STGEDITI) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"


"$(INTDIR)\StgEditItem.obj" : $(SOURCE) $(DEP_CPP_STGEDITI) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STGEDITI) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PROPSET.CPP
DEP_CPP_PROPS=\
	".\oleprop.h"\
	".\PROPSET.H"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "StgEdit - Win32 Release"


"$(INTDIR)\PROPSET.OBJ" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"


"$(INTDIR)\PROPSET.OBJ" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StreamView.CPP
DEP_CPP_STREA=\
	".\ContentsView.h"\
	".\StgEdit.H"\
	".\StgEditItem.H"\
	".\StreamView.h"\
	".\oleprop.h"\
	".\PROPSET.H"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "StgEdit - Win32 Release"


"$(INTDIR)\StreamView.obj" : $(SOURCE) $(DEP_CPP_STREA) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"


"$(INTDIR)\StreamView.obj" : $(SOURCE) $(DEP_CPP_STREA) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STREA) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\splitter.cpp
DEP_CPP_SPLIT=\
	".\splitter.h"\
	".\StdAfx.h"\
	".\StgEdit.H"\
	

!IF  "$(CFG)" == "StgEdit - Win32 Release"


"$(INTDIR)\splitter.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"


"$(INTDIR)\splitter.obj" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PropView.cpp
DEP_CPP_PROPV=\
	".\ContentsView.h"\
	".\PropView.h"\
	".\StgEdit.H"\
	".\StgEditItem.H"\
	".\oleprop.h"\
	".\PROPSET.H"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "StgEdit - Win32 Release"


"$(INTDIR)\PropView.obj" : $(SOURCE) $(DEP_CPP_PROPV) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"


"$(INTDIR)\PropView.obj" : $(SOURCE) $(DEP_CPP_PROPV) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPV) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PropSetView.cpp

!IF  "$(CFG)" == "StgEdit - Win32 Release"

DEP_CPP_PROPSE=\
	".\ContentsView.h"\
	".\PropSetView.h"\
	".\StgEdit.H"\
	".\StgEditItem.H"\
	".\oleprop.h"\
	".\PROPSET.H"\
	".\StdAfx.h"\
	
NODEP_CPP_PROPSE=\
	".\CPropertyView"\
	

"$(INTDIR)\PropSetView.obj" : $(SOURCE) $(DEP_CPP_PROPSE) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"

DEP_CPP_PROPSE=\
	".\ContentsView.h"\
	".\PropSetView.h"\
	".\StgEdit.H"\
	".\StgEditItem.H"\
	".\PROPSET.H"\
	".\oleprop.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\PropSetView.obj" : $(SOURCE) $(DEP_CPP_PROPSE) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PROPSE) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StorageView.cpp

!IF  "$(CFG)" == "StgEdit - Win32 Release"


"$(INTDIR)\StorageView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STGEDIT.pch"


!ELSEIF  "$(CFG)" == "StgEdit - Win32 Debug"

DEP_CPP_STORA=\
	".\StorageView.h"\
	".\StdAfx.h"\
	".\StgEdit.H"\
	

"$(INTDIR)\StorageView.obj" : $(SOURCE) $(DEP_CPP_STORA) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STORA) "$(INTDIR)"\
 "$(INTDIR)\STGEDIT.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
