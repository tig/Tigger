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
PROJPATH = Z:\OLE_IN~2\XRT\XRT_1.01\OLESAMP\XRTSAMP\XRTFRAME\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = STDAFX.CPP  
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /YX"STDAFX.H" /Od /D "_DEBUG" /D "_AFXDLL" /FR /GA /GEf /Fd"XRTFRAME.PDB" /Fp"XRTFRAME.PCH" /Fp"XRTFRAME.PCH"
CFLAGS_R_WEXE = /nologo /Gs /G2 /W3 /AL /YX"STDAFX.H" /O1 /D "NDEBUG" /D "_AFXDLL" /FR /GA /GEf /Fp"XRTFRAME.PCH"
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
		DATAITEM.SBR \
		DATAITMS.SBR \
		DOC.SBR \
		IPFRAME.SBR \
		MAINFRM.SBR \
		PROPERTI.SBR \
		PROPERTY.SBR \
		REQUEST.SBR \
		REQUESTS.SBR \
		SIMOBJ.SBR \
		SRVRITEM.SBR \
		COLLIST.SBR \
		VIEW.SBR \
		WOSAXRT.SBR \
		XRTFRAME.SBR


STDAFX_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h


DATAITEM_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\doc.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\collist.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\simobj.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\wosaxrt.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\request.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\properti.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\property.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\dataitem.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\view.h


DATAITMS_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\dataitms.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\dataitem.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\doc.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\collist.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\simobj.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\wosaxrt.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\property.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\properti.h


DOC_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\wosaxrt.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\mainfrm.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\collist.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\doc.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\simobj.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\srvritem.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\request.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\requests.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\property.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\properti.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\dataitem.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\dataitms.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\view.h


IPFRAME_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\ipframe.h


MAINFRM_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\simobj.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\mainfrm.h


PROPERTI_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\doc.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\collist.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\simobj.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\wosaxrt.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\property.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\properti.h


PROPERTY_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\doc.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\collist.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\simobj.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\wosaxrt.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\view.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\property.h


REQUEST_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\request.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\properti.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\property.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\dataitem.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\doc.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\collist.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\simobj.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\wosaxrt.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\view.h


REQUESTS_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\requests.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\request.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\property.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\properti.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\doc.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\collist.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\simobj.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\wosaxrt.h


SIMOBJ_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\doc.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\collist.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\simobj.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\wosaxrt.h


SRVRITEM_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\doc.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\collist.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\simobj.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\wosaxrt.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\view.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\srvritem.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\dataitem.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\request.h


COLLIST_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\collist.h


VIEW_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\doc.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\collist.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\simobj.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\wosaxrt.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\view.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\srvritem.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\request.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\properti.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\property.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\dataitem.h


WOSAXRT_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\wosaxrt.h


XRTFRAME_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\stdafx.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\xrtframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\mainfrm.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\ipframe.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\doc.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\collist.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\simobj.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\wosaxrt.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\view.h


XRTFRAME_RCDEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\res\xrtframe.ico \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\res\doc.ico \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\res\sizehorz.cur \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrtframe\res\xrtframe.rc2


all:	$(PROJ).EXE $(PROJ).BSC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

DATAITEM.OBJ:	DATAITEM.CPP $(DATAITEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DATAITEM.CPP

DATAITMS.OBJ:	DATAITMS.CPP $(DATAITMS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DATAITMS.CPP

DOC.OBJ:	DOC.CPP $(DOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DOC.CPP

IPFRAME.OBJ:	IPFRAME.CPP $(IPFRAME_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IPFRAME.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

PROPERTI.OBJ:	PROPERTI.CPP $(PROPERTI_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PROPERTI.CPP

PROPERTY.OBJ:	PROPERTY.CPP $(PROPERTY_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PROPERTY.CPP

REQUEST.OBJ:	REQUEST.CPP $(REQUEST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c REQUEST.CPP

REQUESTS.OBJ:	REQUESTS.CPP $(REQUESTS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c REQUESTS.CPP

SIMOBJ.OBJ:	SIMOBJ.CPP $(SIMOBJ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SIMOBJ.CPP

SRVRITEM.OBJ:	SRVRITEM.CPP $(SRVRITEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRVRITEM.CPP

COLLIST.OBJ:	COLLIST.CPP $(COLLIST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLLIST.CPP

VIEW.OBJ:	VIEW.CPP $(VIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c VIEW.CPP

WOSAXRT.OBJ:	WOSAXRT.CPP $(WOSAXRT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WOSAXRT.CPP

XRTFRAME.OBJ:	XRTFRAME.CPP $(XRTFRAME_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c XRTFRAME.CPP

XRTFRAME.RES:	XRTFRAME.RC $(XRTFRAME_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r XRTFRAME.RC


$(PROJ).EXE::	XRTFRAME.RES

$(PROJ).EXE::	STDAFX.OBJ DATAITEM.OBJ DATAITMS.OBJ DOC.OBJ IPFRAME.OBJ MAINFRM.OBJ \
	PROPERTI.OBJ PROPERTY.OBJ REQUEST.OBJ REQUESTS.OBJ SIMOBJ.OBJ SRVRITEM.OBJ COLLIST.OBJ \
	VIEW.OBJ WOSAXRT.OBJ XRTFRAME.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
DATAITEM.OBJ +
DATAITMS.OBJ +
DOC.OBJ +
IPFRAME.OBJ +
MAINFRM.OBJ +
PROPERTI.OBJ +
PROPERTY.OBJ +
REQUEST.OBJ +
REQUESTS.OBJ +
SIMOBJ.OBJ +
SRVRITEM.OBJ +
COLLIST.OBJ +
VIEW.OBJ +
WOSAXRT.OBJ +
XRTFRAME.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
C:\MSVC\CDK16\LIB\+
c:\msvc\lib\+
c:\msvc\mfc\lib\+
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
