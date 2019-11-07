# Microsoft Visual C++ generated build script - Do not modify

PROJ = TEST
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
ORIGIN = MSVCNT
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\XRT\EXCEL5\TEST\ 
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = /YcSTDAFX.H
CUSEPCHFLAG = 
CPPUSEPCHFLAG = /YuSTDAFX.H
FIRSTC = 
FIRSTCPP = STDAFX.CPP
RC = rc
CFLAGS_D_WEXE32 = /nologo /D_X86_ /W3 /FR /D_DEBUG /Zi /D_WINDOWS /Fd"TEST.PDB" /Fp"TEST.PCH"
CFLAGS_R_WEXE32 = /nologo /D_X86_ /W3 /FR /Ox /DNDEBUG /D_WINDOWS /Fp"TEST.PCH"
LFLAGS_D_WEXE32 = /NOLOGO /DEBUG /DEBUGTYPE:cv /SUBSYSTEM:windows nafxcwd.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib ..\xrtxll32.lib
LFLAGS_R_WEXE32 = /NOLOGO /SUBSYSTEM:windows nafxcw.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib ..\xrtxll32.lib
LFLAGS_D_LIB32 = /NOLOGO
LFLAGS_R_LIB32 = /NOLOGO
LIBS_D_WEXE32 = 
LIBS_R_WEXE32 = 
RCFLAGS32 = 
D_RCDEFINES32 = -d_DEBUG
R_RCDEFINES32 = -dNDEBUG
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
SBRS = STDAFX.SBR \
		TEST.SBR \
		MAINFRM.SBR \
		TESTDOC.SBR \
		TESTVIEW.SBR


TEST_RCDEP =  \
	e:\source\xrt\excel5\test\res\test.ico \
	e:\source\xrt\excel5\test\res\test.rc2


STDAFX_DEP =  \
	e:\source\xrt\excel5\test\stdafx.h


TEST_DEP =  \
	e:\source\xrt\excel5\test\stdafx.h \
	e:\source\xrt\excel5\test\test.h \
	e:\source\xrt\excel5\test\mainfrm.h \
	e:\source\xrt\excel5\test\testdoc.h \
	e:\source\xrt\excel5\test\testview.h


MAINFRM_DEP =  \
	e:\source\xrt\excel5\test\stdafx.h \
	e:\source\xrt\excel5\test\test.h \
	e:\source\xrt\excel5\test\mainfrm.h


TESTDOC_DEP =  \
	e:\source\xrt\excel5\test\stdafx.h \
	e:\source\xrt\excel5\test\test.h \
	e:\source\xrt\excel5\test\testdoc.h


TESTVIEW_DEP =  \
	e:\source\xrt\excel5\test\stdafx.h \
	e:\source\xrt\excel5\test\test.h \
	e:\source\xrt\excel5\test\testdoc.h \
	e:\source\xrt\excel5\test\testview.h


all:	$(PROJ).EXE $(PROJ).BSC

TEST.RES:	TEST.RC $(TEST_RCDEP)
	$(RC) $(RCFLAGS32) $(RCDEFINES) -r TEST.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

TEST.OBJ:	TEST.CPP $(TEST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TEST.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

TESTDOC.OBJ:	TESTDOC.CPP $(TESTDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TESTDOC.CPP

TESTVIEW.OBJ:	TESTVIEW.CPP $(TESTVIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TESTVIEW.CPP


$(PROJ).EXE:	TEST.RES

$(PROJ).EXE:	STDAFX.OBJ TEST.OBJ MAINFRM.OBJ TESTDOC.OBJ TESTVIEW.OBJ $(OBJS_EXT) $(LIBS_EXT)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ 
TEST.OBJ 
MAINFRM.OBJ 
TESTDOC.OBJ 
TESTVIEW.OBJ 
$(OBJS_EXT)
-OUT:$(PROJ).EXE
$(MAPFILE_OPTION)
TEST.RES
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
