# Microsoft Visual C++ generated build script - Do not modify

PROJ = STOCKS
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG
R_RCDEFINES = /dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\XRT\STOCKS\
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
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /Od /D "_AFXDLL" /D "_DEBUG" /FR /GA /GEf /Fd"STOCKS.PDB"
CFLAGS_R_WEXE = /nologo /Gs /G2 /W3 /AL /O1 /D "NDEBUG" /D "_AFXDLL" /FR /GA /GEf
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:12288 /ALIGN:16 /ONERROR:NOEXE /CO
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:12288 /ALIGN:16 /ONERROR:NOEXE
LIBS_D_WEXE = mfc250d oldnames libw llibcew commdlg.lib shell.lib  compobj storage ole2 ole2disp ole2nls mfcoleui mfco250d
LIBS_R_WEXE = mfc250  oldnames libw llibcew commdlg.lib shell.lib  compobj storage ole2 ole2disp ole2nls mfcoleui mfco250
RCFLAGS = /nologo /z
RESFLAGS = /nologo /t
RUNFLAGS = 
DEFFILE = STOCKS.DEF
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
		STOCKS.SBR \
		MAINFRM.SBR \
		STOCKDOC.SBR \
		STOCKVW.SBR \
		IPFRAME.SBR \
		SRVRITEM.SBR \
		ITEM.SBR \
		ITEMS.SBR


STOCKS_RCDEP = e:\source\xrt\stocks\res\stocks.ico \
	e:\source\xrt\stocks\res\stockdoc.ico \
	e:\source\xrt\stocks\res\stocks.rc2


STDAFX_DEP = e:\source\xrt\stocks\stdafx.h


STOCKS_DEP = e:\source\xrt\stocks\stdafx.h \
	e:\source\xrt\stocks\stocks.h \
	e:\source\xrt\stocks\mainfrm.h \
	e:\source\xrt\stocks\ipframe.h \
	e:\source\xrt\stocks\stockdoc.h \
	e:\source\xrt\stocks\stockvw.h


MAINFRM_DEP = e:\source\xrt\stocks\stdafx.h \
	e:\source\xrt\stocks\stocks.h \
	e:\source\xrt\stocks\mainfrm.h


STOCKDOC_DEP = e:\source\xrt\stocks\stdafx.h \
	e:\source\xrt\stocks\stocks.h \
	e:\source\xrt\stocks\item.h \
	e:\source\xrt\stocks\items.h \
	e:\source\xrt\stocks\stockdoc.h \
	e:\source\xrt\stocks\srvritem.h


STOCKVW_DEP = e:\source\xrt\stocks\stdafx.h \
	e:\source\xrt\stocks\stocks.h \
	e:\source\xrt\stocks\stockdoc.h \
	e:\source\xrt\stocks\stockvw.h


IPFRAME_DEP = e:\source\xrt\stocks\stdafx.h \
	e:\source\xrt\stocks\stocks.h \
	e:\source\xrt\stocks\ipframe.h


SRVRITEM_DEP = e:\source\xrt\stocks\stdafx.h \
	e:\source\xrt\stocks\stocks.h \
	e:\source\xrt\stocks\stockdoc.h \
	e:\source\xrt\stocks\srvritem.h


ITEM_DEP = e:\source\xrt\stocks\stdafx.h \
	e:\source\xrt\stocks\stocks.h \
	e:\source\xrt\stocks\item.h \
	e:\source\xrt\stocks\items.h


ITEMS_DEP = e:\source\xrt\stocks\stdafx.h \
	e:\source\xrt\stocks\stocks.h \
	e:\source\xrt\stocks\item.h \
	e:\source\xrt\stocks\items.h


all:	$(PROJ).EXE $(PROJ).BSC

STOCKS.RES:	STOCKS.RC $(STOCKS_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r STOCKS.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

STOCKS.OBJ:	STOCKS.CPP $(STOCKS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c STOCKS.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

STOCKDOC.OBJ:	STOCKDOC.CPP $(STOCKDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c STOCKDOC.CPP

STOCKVW.OBJ:	STOCKVW.CPP $(STOCKVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c STOCKVW.CPP

IPFRAME.OBJ:	IPFRAME.CPP $(IPFRAME_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IPFRAME.CPP

SRVRITEM.OBJ:	SRVRITEM.CPP $(SRVRITEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRVRITEM.CPP

ITEM.OBJ:	ITEM.CPP $(ITEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ITEM.CPP

ITEMS.OBJ:	ITEMS.CPP $(ITEMS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ITEMS.CPP


$(PROJ).EXE::	STOCKS.RES

$(PROJ).EXE::	STDAFX.OBJ STOCKS.OBJ MAINFRM.OBJ STOCKDOC.OBJ STOCKVW.OBJ IPFRAME.OBJ \
	SRVRITEM.OBJ ITEM.OBJ ITEMS.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
STOCKS.OBJ +
MAINFRM.OBJ +
STOCKDOC.OBJ +
STOCKVW.OBJ +
IPFRAME.OBJ +
SRVRITEM.OBJ +
ITEM.OBJ +
ITEMS.OBJ +
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
	$(RC) $(RESFLAGS) STOCKS.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	STOCKS.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) STOCKS.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
