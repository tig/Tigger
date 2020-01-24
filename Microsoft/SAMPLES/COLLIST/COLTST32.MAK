# Microsoft Visual C++ generated build script - Do not modify

PROJ = COLTST32
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
ORIGIN = MSVCNT
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\COLLIST\ 
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = WINTIME.C
FIRSTCPP = COLLIST.CPP
RC = rc
CFLAGS_D_WEXE32 = /nologo /W3 /Zi /YX /D "_DEBUG" /D "_X86_" /D "_WINDOWS" /D "WIN32" /FR /ML /Fd"COLTST32.PDB"  /Fp"COLTST32.PCH"
CFLAGS_R_WEXE32 = /nologo /W3 /YX /O2 /D "NDEBUG" /D "_X86_" /D "_WINDOWS" /D "WIN32" /FR /ML /Fp"COLTST32.PCH"
LFLAGS_D_WEXE32 = /NOLOGO /DEBUG /DEBUGTYPE:cv /SUBSYSTEM:windows user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib nafxcwd.lib
LFLAGS_R_WEXE32 = /NOLOGO /SUBSYSTEM:windows user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib nafxcw.lib
LFLAGS_D_LIB32 = /NOLOGO
LFLAGS_R_LIB32 = /NOLOGO
LIBS_D_WEXE32 = 
LIBS_R_WEXE32 = 
RCFLAGS32 = 
D_RCDEFINES32 = /d_DEBUG /dWIN32
R_RCDEFINES32 = /dNDEBUG /dWIN32
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE32)
LFLAGS = $(LFLAGS_D_WEXE32)
LIBS = $(LIBS_D_WEXE32)
LFLAGS_LIB=$(LFLAGS_D_LIB32)
MAPFILE_OPTION = 
RCDEFINES = $(D_RCDEFINES32)
!else
CFLAGS = $(CFLAGS_R_WEXE32)
LFLAGS = $(LFLAGS_R_WEXE32)
LIBS = $(LIBS_R_WEXE32)
MAPFILE_OPTION = 
LFLAGS_LIB=$(LFLAGS_R_LIB32)
RCDEFINES = $(R_RCDEFINES32)
!endif
SBRS = COLLIST.SBR \
		COLTEDOC.SBR \
		COLTEST.SBR \
		COLTEVW.SBR \
		MAINFRM.SBR \
		STDAFX.SBR


COLLIST_DEP =  \
	e:\source\collist\stdafx.h \
	e:\source\collist\collist.h


COLTEDOC_DEP =  \
	e:\source\collist\stdafx.h \
	e:\source\collist\coltest.h \
	e:\source\collist\wintime.h \
	e:\source\collist\collist.h \
	e:\source\collist\coltedoc.h


COLTEST_DEP =  \
	e:\source\collist\stdafx.h \
	e:\source\collist\coltest.h \
	e:\source\collist\wintime.h \
	e:\source\collist\collist.h \
	e:\source\collist\mainfrm.h \
	e:\source\collist\coltedoc.h \
	e:\source\collist\coltevw.h


COLTEVW_DEP =  \
	e:\source\collist\stdafx.h \
	e:\source\collist\coltest.h \
	e:\source\collist\wintime.h \
	e:\source\collist\collist.h \
	e:\source\collist\coltedoc.h \
	e:\source\collist\coltevw.h


MAINFRM_DEP =  \
	e:\source\collist\stdafx.h \
	e:\source\collist\coltest.h \
	e:\source\collist\wintime.h \
	e:\source\collist\collist.h \
	e:\source\collist\mainfrm.h


STDAFX_DEP =  \
	e:\source\collist\stdafx.h


COLTEST_RCDEP =  \
	e:\source\collist\res\coltest.ico \
	e:\source\collist\res\coltedoc.ico \
	e:\source\collist\res\sizehorz.cur \
	e:\source\collist\res\coltest.rc2


all:	$(PROJ).EXE $(PROJ).BSC

COLLIST.OBJ:	COLLIST.CPP $(COLLIST_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c COLLIST.CPP

COLTEDOC.OBJ:	COLTEDOC.CPP $(COLTEDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLTEDOC.CPP

COLTEST.OBJ:	COLTEST.CPP $(COLTEST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLTEST.CPP

COLTEVW.OBJ:	COLTEVW.CPP $(COLTEVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLTEVW.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c STDAFX.CPP

COLTEST.RES:	COLTEST.RC $(COLTEST_RCDEP)
	$(RC) $(RCFLAGS32) $(RCDEFINES) -r COLTEST.RC


$(PROJ).EXE:	COLTEST.RES

$(PROJ).EXE:	COLLIST.OBJ COLTEDOC.OBJ COLTEST.OBJ COLTEVW.OBJ MAINFRM.OBJ STDAFX.OBJ $(OBJS_EXT) $(LIBS_EXT)
	echo >NUL @<<$(PROJ).CRF
COLLIST.OBJ 
COLTEDOC.OBJ 
COLTEST.OBJ 
COLTEVW.OBJ 
MAINFRM.OBJ 
STDAFX.OBJ 
$(OBJS_EXT)
-OUT:$(PROJ).EXE
$(MAPFILE_OPTION)
COLTEST.RES
$(LIBS)
$(LIBS_EXT)
$(DEFFILE_OPTION) -implib:$(PROJ).lib
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
