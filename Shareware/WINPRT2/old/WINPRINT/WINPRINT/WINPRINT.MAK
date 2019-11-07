# Microsoft Visual C++ generated build script - Do not modify

PROJ = WINPRINT
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG
R_RCDEFINES = /dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\!\WINPRT2\WINPRINT\WINPRINT\
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
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /Od /D "_DEBUG" /FR /GA /Fd"WINPRINT.PDB"
CFLAGS_R_WEXE = /nologo /Gs /G2 /W3 /AM /O1 /D "NDEBUG" /FR /GA
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE
LIBS_D_WEXE = mafxcwd oldnames libw mlibcew commdlg olesvr olecli shell 
LIBS_R_WEXE = mafxcw oldnames libw mlibcew commdlg olesvr olecli shell 
RCFLAGS = /nologo /z
RESFLAGS = /nologo /t
RUNFLAGS = 
DEFFILE = WINPRINT.DEF
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
		WINPRINT.SBR \
		MAINFRM.SBR \
		WINPRDOC.SBR \
		WINPRVW.SBR


WINPRINT_RCDEP = c:\!\winprt2\winprint\winprint\res\winprint.ico \
	c:\!\winprt2\winprint\winprint\res\toolbar.bmp \
	c:\!\winprt2\winprint\winprint\res\winprint.rc2


STDAFX_DEP = c:\!\winprt2\winprint\winprint\stdafx.h


WINPRINT_DEP = c:\!\winprt2\winprint\winprint\stdafx.h \
	c:\!\winprt2\winprint\winprint\winprint.h \
	c:\!\winprt2\winprint\winprint\mainfrm.h \
	c:\!\winprt2\winprint\winprint\winprdoc.h \
	c:\!\winprt2\winprint\winprint\winprvw.h


MAINFRM_DEP = c:\!\winprt2\winprint\winprint\stdafx.h \
	c:\!\winprt2\winprint\winprint\winprint.h \
	c:\!\winprt2\winprint\winprint\mainfrm.h


WINPRDOC_DEP = c:\!\winprt2\winprint\winprint\stdafx.h \
	c:\!\winprt2\winprint\winprint\winprint.h \
	c:\!\winprt2\winprint\winprint\winprdoc.h


WINPRVW_DEP = c:\!\winprt2\winprint\winprint\stdafx.h \
	c:\!\winprt2\winprint\winprint\winprint.h \
	c:\!\winprt2\winprint\winprint\winprdoc.h \
	c:\!\winprt2\winprint\winprint\winprvw.h


all:	$(PROJ).EXE $(PROJ).BSC

WINPRINT.RES:	WINPRINT.RC $(WINPRINT_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r WINPRINT.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

WINPRINT.OBJ:	WINPRINT.CPP $(WINPRINT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WINPRINT.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

WINPRDOC.OBJ:	WINPRDOC.CPP $(WINPRDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WINPRDOC.CPP

WINPRVW.OBJ:	WINPRVW.CPP $(WINPRVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WINPRVW.CPP


$(PROJ).EXE::	WINPRINT.RES

$(PROJ).EXE::	STDAFX.OBJ WINPRINT.OBJ MAINFRM.OBJ WINPRDOC.OBJ WINPRVW.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
WINPRINT.OBJ +
MAINFRM.OBJ +
WINPRDOC.OBJ +
WINPRVW.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) WINPRINT.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	WINPRINT.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) WINPRINT.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
