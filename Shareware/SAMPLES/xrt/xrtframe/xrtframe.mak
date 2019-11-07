# Microsoft Visual C++ generated build script - Do not modify

PROJ = XRTFRAME
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG
R_RCDEFINES = /dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = Z:\XRTFRAME\
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
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /Od /D "_AFXDLL" /D "_DEBUG" /FR /GA /GEf /Fd"XRTFRAME.PDB"
CFLAGS_R_WEXE = /nologo /Gs /G2 /W3 /AL /O1 /D "NDEBUG" /D "_AFXDLL" /FR /GA /GEf
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:12288 /ALIGN:16 /ONERROR:NOEXE /CO
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:12288 /ALIGN:16 /ONERROR:NOEXE
LIBS_D_WEXE = mfc250d oldnames libw llibcew commdlg.lib shell.lib  compobj storage ole2 ole2disp ole2nls mfcoleui mfco250d
LIBS_R_WEXE = mfc250  oldnames libw llibcew commdlg.lib shell.lib  compobj storage ole2 ole2disp ole2nls mfcoleui mfco250
RCFLAGS = /nologo /z
RESFLAGS = /nologo /t
RUNFLAGS = 
DEFFILE = XRTFRAME.DEF
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
		XRTFRAME.SBR \
		MAINFRM.SBR \
		DOC.SBR \
		VIEW.SBR \
		IPFRAME.SBR \
		SRVRITEM.SBR \
		COLLIST.SBR \
		REQUEST.SBR \
		REQUESTS.SBR \
		PROPERTY.SBR \
		PROPERTI.SBR \
		DATAITEM.SBR \
		SIMOBJ.SBR \
		DATAITMS.SBR \
		WOSAXRT.SBR


XRTFRAME_RCDEP = z:\xrtframe\res\xrtframe.ico \
	z:\xrtframe\res\doc.ico \
	z:\xrtframe\res\sizehorz.cur \
	z:\xrtframe\res\xrtframe.rc2


STDAFX_DEP = z:\xrtframe\stdafx.h


XRTFRAME_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\mainfrm.h \
	z:\xrtframe\ipframe.h \
	z:\xrtframe\doc.h \
	z:\xrtframe\collist.h \
	z:\xrtframe\simobj.h \
	z:\xrtframe\wosaxrt.h \
	z:\xrtframe\view.h


MAINFRM_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\simobj.h \
	z:\xrtframe\mainfrm.h


DOC_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\wosaxrt.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\mainfrm.h \
	z:\xrtframe\collist.h \
	z:\xrtframe\doc.h \
	z:\xrtframe\simobj.h \
	z:\xrtframe\srvritem.h \
	z:\xrtframe\request.h \
	z:\xrtframe\requests.h \
	z:\xrtframe\property.h \
	z:\xrtframe\properti.h \
	z:\xrtframe\dataitem.h \
	z:\xrtframe\dataitms.h \
	z:\xrtframe\view.h


VIEW_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\doc.h \
	z:\xrtframe\collist.h \
	z:\xrtframe\simobj.h \
	z:\xrtframe\wosaxrt.h \
	z:\xrtframe\view.h \
	z:\xrtframe\srvritem.h \
	z:\xrtframe\request.h \
	z:\xrtframe\properti.h \
	z:\xrtframe\property.h \
	z:\xrtframe\dataitem.h


IPFRAME_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\ipframe.h


SRVRITEM_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\doc.h \
	z:\xrtframe\collist.h \
	z:\xrtframe\simobj.h \
	z:\xrtframe\wosaxrt.h \
	z:\xrtframe\view.h \
	z:\xrtframe\srvritem.h \
	z:\xrtframe\dataitem.h \
	z:\xrtframe\request.h


COLLIST_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\collist.h


REQUEST_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\request.h \
	z:\xrtframe\properti.h \
	z:\xrtframe\property.h \
	z:\xrtframe\dataitem.h \
	z:\xrtframe\doc.h \
	z:\xrtframe\collist.h \
	z:\xrtframe\simobj.h \
	z:\xrtframe\wosaxrt.h \
	z:\xrtframe\view.h


REQUESTS_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\requests.h \
	z:\xrtframe\request.h \
	z:\xrtframe\property.h \
	z:\xrtframe\properti.h \
	z:\xrtframe\doc.h \
	z:\xrtframe\collist.h \
	z:\xrtframe\simobj.h \
	z:\xrtframe\wosaxrt.h


PROPERTY_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\doc.h \
	z:\xrtframe\collist.h \
	z:\xrtframe\simobj.h \
	z:\xrtframe\wosaxrt.h \
	z:\xrtframe\view.h \
	z:\xrtframe\property.h


PROPERTI_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\doc.h \
	z:\xrtframe\collist.h \
	z:\xrtframe\simobj.h \
	z:\xrtframe\wosaxrt.h \
	z:\xrtframe\property.h \
	z:\xrtframe\properti.h


DATAITEM_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\doc.h \
	z:\xrtframe\collist.h \
	z:\xrtframe\simobj.h \
	z:\xrtframe\wosaxrt.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\request.h \
	z:\xrtframe\properti.h \
	z:\xrtframe\property.h \
	z:\xrtframe\dataitem.h \
	z:\xrtframe\view.h


SIMOBJ_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\doc.h \
	z:\xrtframe\collist.h \
	z:\xrtframe\simobj.h \
	z:\xrtframe\wosaxrt.h


DATAITMS_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\xrtframe.h \
	z:\xrtframe\dataitms.h \
	z:\xrtframe\dataitem.h \
	z:\xrtframe\doc.h \
	z:\xrtframe\collist.h \
	z:\xrtframe\simobj.h \
	z:\xrtframe\wosaxrt.h \
	z:\xrtframe\property.h \
	z:\xrtframe\properti.h


WOSAXRT_DEP = z:\xrtframe\stdafx.h \
	z:\xrtframe\wosaxrt.h


all:	$(PROJ).EXE $(PROJ).BSC

XRTFRAME.RES:	XRTFRAME.RC $(XRTFRAME_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r XRTFRAME.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

XRTFRAME.OBJ:	XRTFRAME.CPP $(XRTFRAME_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c XRTFRAME.CPP

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

COLLIST.OBJ:	COLLIST.CPP $(COLLIST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLLIST.CPP

REQUEST.OBJ:	REQUEST.CPP $(REQUEST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c REQUEST.CPP

REQUESTS.OBJ:	REQUESTS.CPP $(REQUESTS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c REQUESTS.CPP

PROPERTY.OBJ:	PROPERTY.CPP $(PROPERTY_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PROPERTY.CPP

PROPERTI.OBJ:	PROPERTI.CPP $(PROPERTI_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PROPERTI.CPP

DATAITEM.OBJ:	DATAITEM.CPP $(DATAITEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DATAITEM.CPP

SIMOBJ.OBJ:	SIMOBJ.CPP $(SIMOBJ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SIMOBJ.CPP

DATAITMS.OBJ:	DATAITMS.CPP $(DATAITMS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DATAITMS.CPP

WOSAXRT.OBJ:	WOSAXRT.CPP $(WOSAXRT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WOSAXRT.CPP


$(PROJ).EXE::	XRTFRAME.RES

$(PROJ).EXE::	STDAFX.OBJ XRTFRAME.OBJ MAINFRM.OBJ DOC.OBJ VIEW.OBJ IPFRAME.OBJ SRVRITEM.OBJ \
	COLLIST.OBJ REQUEST.OBJ REQUESTS.OBJ PROPERTY.OBJ PROPERTI.OBJ DATAITEM.OBJ SIMOBJ.OBJ \
	DATAITMS.OBJ WOSAXRT.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
XRTFRAME.OBJ +
MAINFRM.OBJ +
DOC.OBJ +
VIEW.OBJ +
IPFRAME.OBJ +
SRVRITEM.OBJ +
COLLIST.OBJ +
REQUEST.OBJ +
REQUESTS.OBJ +
PROPERTY.OBJ +
PROPERTI.OBJ +
DATAITEM.OBJ +
SIMOBJ.OBJ +
DATAITMS.OBJ +
WOSAXRT.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
D:\MSVC\CDK16\LIB\+
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
	$(RC) $(RESFLAGS) XRTFRAME.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	XRTFRAME.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) XRTFRAME.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
