# Microsoft Visual C++ generated build script - Do not modify

PROJ = COLSAMP
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG
R_RCDEFINES = /dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = Z:\COLSAMP\
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
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /Od /D "_AFXDLL" /D "_DEBUG" /FR /GA /GEf /Fd"COLSAMP.PDB"
CFLAGS_R_WEXE = /nologo /Gs /G2 /W3 /AL /O1 /D "NDEBUG" /D "_AFXDLL" /FR /GA /GEf
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:12288 /ALIGN:16 /ONERROR:NOEXE /CO
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:12288 /ALIGN:16 /ONERROR:NOEXE
LIBS_D_WEXE = mfc250d oldnames libw llibcew commdlg.lib shell.lib  compobj storage ole2 ole2disp ole2nls mfcoleui mfco250d
LIBS_R_WEXE = mfc250  oldnames libw llibcew commdlg.lib shell.lib  compobj storage ole2 ole2disp ole2nls mfcoleui mfco250
RCFLAGS = /nologo /z
RESFLAGS = /nologo /t
RUNFLAGS = 
DEFFILE = COLSAMP.DEF
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
		COLSAMP.SBR \
		MAINFRM.SBR \
		DOC.SBR \
		VIEW.SBR \
		IPFRAME.SBR \
		SRVRITEM.SBR \
		ITEM.SBR \
		ITEMS.SBR \
		COLLIST.SBR


COLSAMP_RCDEP = z:\colsamp\res\colsamp.ico \
	z:\colsamp\res\doc.ico \
	z:\colsamp\res\sizehorz.cur \
	z:\colsamp\res\colsamp.rc2


STDAFX_DEP = z:\colsamp\stdafx.h


COLSAMP_DEP = z:\colsamp\stdafx.h \
	z:\colsamp\colsamp.h \
	z:\colsamp\mainfrm.h \
	z:\colsamp\ipframe.h \
	z:\colsamp\doc.h \
	z:\colsamp\collist.h \
	z:\colsamp\view.h


MAINFRM_DEP = z:\colsamp\stdafx.h \
	z:\colsamp\colsamp.h \
	z:\colsamp\mainfrm.h


DOC_DEP = z:\colsamp\stdafx.h \
	z:\colsamp\colsamp.h \
	z:\colsamp\item.h \
	z:\colsamp\items.h \
	z:\colsamp\doc.h \
	z:\colsamp\collist.h \
	z:\colsamp\srvritem.h


VIEW_DEP = z:\colsamp\stdafx.h \
	z:\colsamp\colsamp.h \
	z:\colsamp\item.h \
	z:\colsamp\items.h \
	z:\colsamp\doc.h \
	z:\colsamp\collist.h \
	z:\colsamp\view.h


IPFRAME_DEP = z:\colsamp\stdafx.h \
	z:\colsamp\colsamp.h \
	z:\colsamp\ipframe.h


SRVRITEM_DEP = z:\colsamp\stdafx.h \
	z:\colsamp\colsamp.h \
	z:\colsamp\doc.h \
	z:\colsamp\collist.h \
	z:\colsamp\view.h \
	z:\colsamp\srvritem.h


ITEM_DEP = z:\colsamp\stdafx.h \
	z:\colsamp\colsamp.h \
	z:\colsamp\item.h \
	z:\colsamp\items.h \
	z:\colsamp\doc.h \
	z:\colsamp\collist.h


ITEMS_DEP = z:\colsamp\stdafx.h \
	z:\colsamp\colsamp.h \
	z:\colsamp\item.h \
	z:\colsamp\items.h \
	z:\colsamp\doc.h \
	z:\colsamp\collist.h


COLLIST_DEP = z:\colsamp\stdafx.h \
	z:\colsamp\collist.h


all:	$(PROJ).EXE $(PROJ).BSC

COLSAMP.RES:	COLSAMP.RC $(COLSAMP_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r COLSAMP.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

COLSAMP.OBJ:	COLSAMP.CPP $(COLSAMP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLSAMP.CPP

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

ITEM.OBJ:	ITEM.CPP $(ITEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ITEM.CPP

ITEMS.OBJ:	ITEMS.CPP $(ITEMS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ITEMS.CPP

COLLIST.OBJ:	COLLIST.CPP $(COLLIST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLLIST.CPP


$(PROJ).EXE::	COLSAMP.RES

$(PROJ).EXE::	STDAFX.OBJ COLSAMP.OBJ MAINFRM.OBJ DOC.OBJ VIEW.OBJ IPFRAME.OBJ SRVRITEM.OBJ \
	ITEM.OBJ ITEMS.OBJ COLLIST.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
COLSAMP.OBJ +
MAINFRM.OBJ +
DOC.OBJ +
VIEW.OBJ +
IPFRAME.OBJ +
SRVRITEM.OBJ +
ITEM.OBJ +
ITEMS.OBJ +
COLLIST.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
c:\lib\+
d:\ole2\lib\+
e:\progole2\lib\+
e:\xlsdk\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) COLSAMP.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	COLSAMP.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) COLSAMP.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
