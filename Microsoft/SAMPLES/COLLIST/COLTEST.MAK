# Microsoft Visual C++ generated build script - Do not modify

PROJ = COLTEST
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG
R_RCDEFINES = /dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\COLLIST\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = /YcSTDAFX.H
CUSEPCHFLAG = 
CPPUSEPCHFLAG = /YuSTDAFX.H
FIRSTC = WINTIME.C   
FIRSTCPP = STDAFX.CPP  
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /Od /D "_DEBUG" /FR /GA /Fd"COLTEST.PDB" 
CFLAGS_R_WEXE = /nologo /Gs /G3 /W3 /AM /O1 /D "NDEBUG" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = mafxcwd oldnames libw mlibcew commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WEXE = mafxcw oldnames libw mlibcew commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo /z
RESFLAGS = /nologo /t
RUNFLAGS = 
DEFFILE = COLTEST.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = STDAFX.SBR \
		COLTEST.SBR \
		MAINFRM.SBR \
		COLTEDOC.SBR \
		COLTEVW.SBR \
		WINTIME.SBR \
		COLLIST.SBR


COLTEST_RCDEP = e:\source\collist\res\coltest.ico \
	e:\source\collist\res\coltedoc.ico \
	e:\source\collist\res\coltest.rc2


STDAFX_DEP = e:\source\collist\stdafx.h


COLTEST_DEP = e:\source\collist\stdafx.h \
	e:\source\collist\coltest.h \
	e:\source\collist\collist.h \
	e:\source\collist\mainfrm.h \
	e:\source\collist\coltedoc.h \
	e:\source\collist\coltevw.h


MAINFRM_DEP = e:\source\collist\stdafx.h \
	e:\source\collist\coltest.h \
	e:\source\collist\collist.h \
	e:\source\collist\mainfrm.h


COLTEDOC_DEP = e:\source\collist\stdafx.h \
	e:\source\collist\coltest.h \
	e:\source\collist\collist.h \
	e:\source\collist\coltedoc.h


COLTEVW_DEP = e:\source\collist\stdafx.h \
	e:\source\collist\coltest.h \
	e:\source\collist\collist.h \
	e:\source\collist\coltedoc.h \
	e:\source\collist\coltevw.h \
	e:\source\collist\wintime.h


WINTIME_DEP = e:\source\collist\wintime.h


COLLIST_DEP = e:\source\collist\stdafx.h \
	e:\source\collist\collist.h


all:	$(PROJ).EXE $(PROJ).BSC

COLTEST.RES:	COLTEST.RC $(COLTEST_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r COLTEST.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

COLTEST.OBJ:	COLTEST.CPP $(COLTEST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLTEST.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

COLTEDOC.OBJ:	COLTEDOC.CPP $(COLTEDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLTEDOC.CPP

COLTEVW.OBJ:	COLTEVW.CPP $(COLTEVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLTEVW.CPP

WINTIME.OBJ:	WINTIME.C $(WINTIME_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c WINTIME.C

COLLIST.OBJ:	COLLIST.CPP $(COLLIST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLLIST.CPP


$(PROJ).EXE::	COLTEST.RES

$(PROJ).EXE::	STDAFX.OBJ COLTEST.OBJ MAINFRM.OBJ COLTEDOC.OBJ COLTEVW.OBJ WINTIME.OBJ \
	COLLIST.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
COLTEST.OBJ +
MAINFRM.OBJ +
COLTEDOC.OBJ +
COLTEVW.OBJ +
WINTIME.OBJ +
COLLIST.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
c:\lib\+
d:\ole2\rel\+
e:\progole2\lib\+
e:\xlsdk\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) COLTEST.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	COLTEST.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) COLTEST.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
