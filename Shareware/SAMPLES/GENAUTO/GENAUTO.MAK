# Microsoft Visual C++ generated build script - Do not modify

PROJ = GENAUTO
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG
R_RCDEFINES = /dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = Z:\GENAUTO\
USEMFC = 0
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
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /Od /D "_AFXDLL" /D "_DEBUG" /FR /GA /GEf /Fd"GENAUTO.PDB"
CFLAGS_R_WEXE = /nologo /Gs /G2 /W3 /AL /O1 /D "NDEBUG" /D "_AFXDLL" /FR /GA /GEf
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:12288 /ALIGN:16 /ONERROR:NOEXE /CO
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:12288 /ALIGN:16 /ONERROR:NOEXE
LIBS_D_WEXE = mfc250d oldnames libw llibcew commdlg.lib shell.lib  compobj storage ole2 ole2disp ole2nls mfcoleui mfco250d
LIBS_R_WEXE = mfc250  oldnames libw llibcew commdlg.lib shell.lib  compobj storage ole2 ole2disp ole2nls mfcoleui mfco250
RCFLAGS = /nologo /z
RESFLAGS = /nologo /t
RUNFLAGS = 
DEFFILE = GENAUTO.DEF
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
		GENAUTO.SBR \
		MAINFRM.SBR \
		DOC.SBR \
		VIEW.SBR \
		IPFRAME.SBR \
		SRVRITEM.SBR


GENAUTO_RCDEP = z:\genauto\res\genauto.ico \
	z:\genauto\res\doc.ico \
	z:\genauto\res\sizehorz.cur \
	z:\genauto\res\genauto.rc2


STDAFX_DEP = z:\genauto\stdafx.h


GENAUTO_DEP = z:\genauto\stdafx.h \
	z:\genauto\genauto.h \
	z:\genauto\mainfrm.h \
	z:\genauto\ipframe.h \
	z:\genauto\doc.h \
	z:\genauto\view.h


MAINFRM_DEP = z:\genauto\stdafx.h \
	z:\genauto\genauto.h \
	z:\genauto\mainfrm.h


DOC_DEP = z:\genauto\stdafx.h \
	z:\genauto\genauto.h \
	z:\genauto\doc.h \
	z:\genauto\srvritem.h


VIEW_DEP = z:\genauto\stdafx.h \
	z:\genauto\genauto.h \
	z:\genauto\doc.h \
	z:\genauto\view.h


IPFRAME_DEP = z:\genauto\stdafx.h \
	z:\genauto\genauto.h \
	z:\genauto\ipframe.h


SRVRITEM_DEP = z:\genauto\stdafx.h \
	z:\genauto\genauto.h \
	z:\genauto\doc.h \
	z:\genauto\view.h \
	z:\genauto\srvritem.h


all:	$(PROJ).EXE $(PROJ).BSC

GENAUTO.RES:	GENAUTO.RC $(GENAUTO_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r GENAUTO.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

GENAUTO.OBJ:	GENAUTO.CPP $(GENAUTO_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c GENAUTO.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

DOC.OBJ:	DOC.CPP $(DOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DOC.CPP

VIEW.OBJ:	VIEW.CPP $(VIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c VIEW.CPP

IPFRAME.OBJ:	IPFRAME.CPP $(IPFRAME_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IPFRAME.CPP

SRVRITEM.OBJ:	SRVRITEM.CPP $(SRVRITEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRVRITEM.CPP


$(PROJ).EXE::	GENAUTO.RES

$(PROJ).EXE::	STDAFX.OBJ GENAUTO.OBJ MAINFRM.OBJ DOC.OBJ VIEW.OBJ IPFRAME.OBJ SRVRITEM.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
GENAUTO.OBJ +
MAINFRM.OBJ +
DOC.OBJ +
VIEW.OBJ +
IPFRAME.OBJ +
SRVRITEM.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
d:\ole2\rel\+
d:\lib\+
e:\xl4sdk\lib\+
d:\msvc\ole2\samples\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) GENAUTO.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	GENAUTO.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) GENAUTO.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
