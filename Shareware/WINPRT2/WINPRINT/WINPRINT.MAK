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
PROJPATH = Z:\SOURCE\KINDEL\WINPRT2\WINPRINT\
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
CFLAGS_D_WEXE = /nologo /G2 /W4 /WX /Zi /AL /Od /D "_DEBUG" /D "_AFXDLL" /FR /GA /GEf /Fd"WINPRINT.PDB" 
CFLAGS_R_WEXE = /nologo /Gs /G2 /W4 /WX /AL /O1 /D "NDEBUG" /D "_AFXDLL" /FR /GA /GEf 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:12288 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:12288 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = mfc250d mfco250d oldnames libw llibcew compobj storage ole2 ole2disp ole2nls mfcoleui ctl3d commdlg.lib shell.lib 
LIBS_R_WEXE = mfc250 mfco250 oldnames libw llibcew compobj storage ole2 ole2disp ole2nls mfcoleui ctl3d commdlg.lib shell.lib 
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
		CNTRITEM.SBR \
		CFGVIEW.SBR \
		QUEVIEW.SBR \
		CFGLSTVW.SBR \
		UTIL.SBR \
		SHADOW.SBR \
		COLLIST.SBR \
		VIEW.SBR \
		CFGFRAME.SBR \
		ABOUTBOX.SBR \
		ITSYBITS.SBR


WINPRINT_RCDEP = z:\source\kindel\winprt2\winprint\res\winprint.ico \
	z:\source\kindel\winprt2\winprint\res\idr_prin.ico \
	z:\source\kindel\winprt2\winprint\res\na.bmp \
	z:\source\kindel\winprt2\winprint\res\question.bmp \
	z:\source\kindel\winprt2\winprint\res\portrait.bmp \
	z:\source\kindel\winprt2\winprint\res\landscap.bmp \
	z:\source\kindel\winprt2\winprint\res\sizehorz.cur \
	z:\source\kindel\winprt2\winprint\res\winprint.rc2


STDAFX_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h


WINPRINT_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\winprint.h \
	z:\source\kindel\winprt2\winprint\util.h \
	z:\source\kindel\winprt2\winprint\shadow.h \
	z:\source\kindel\winprt2\winprint\aboutbox.h \
	z:\source\kindel\winprt2\winprint\mainfrm.h \
	z:\source\kindel\winprt2\winprint\winprdoc.h \
	z:\source\kindel\winprt2\winprint\cntritem.h \
	z:\source\kindel\winprt2\winprint\cfgview.h \
	z:\source\kindel\winprt2\winprint\queview.h \
	z:\source\kindel\winprt2\winprint\view.h \
	z:\source\kindel\winprt2\winprint\collist.h \
	z:\source\kindel\winprt2\winprint\cfglstvw.h \
	z:\source\kindel\winprt2\winprint\cfgframe.h


MAINFRM_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\winprint.h \
	z:\source\kindel\winprt2\winprint\util.h \
	z:\source\kindel\winprt2\winprint\shadow.h \
	z:\source\kindel\winprt2\winprint\aboutbox.h \
	z:\source\kindel\winprt2\winprint\mainfrm.h \
	z:\source\kindel\winprt2\winprint\queview.h \
	z:\source\kindel\winprt2\winprint\view.h \
	z:\source\kindel\winprt2\winprint\collist.h \
	z:\source\kindel\winprt2\winprint\cfglstvw.h


WINPRDOC_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\winprint.h \
	z:\source\kindel\winprt2\winprint\util.h \
	z:\source\kindel\winprt2\winprint\shadow.h \
	z:\source\kindel\winprt2\winprint\aboutbox.h \
	z:\source\kindel\winprt2\winprint\winprdoc.h \
	z:\source\kindel\winprt2\winprint\cntritem.h


CNTRITEM_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\winprint.h \
	z:\source\kindel\winprt2\winprint\util.h \
	z:\source\kindel\winprt2\winprint\shadow.h \
	z:\source\kindel\winprt2\winprint\aboutbox.h \
	z:\source\kindel\winprt2\winprint\winprdoc.h \
	z:\source\kindel\winprt2\winprint\cntritem.h


CFGVIEW_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\winprint.h \
	z:\source\kindel\winprt2\winprint\util.h \
	z:\source\kindel\winprt2\winprint\shadow.h \
	z:\source\kindel\winprt2\winprint\aboutbox.h \
	z:\source\kindel\winprt2\winprint\winprdoc.h \
	z:\source\kindel\winprt2\winprint\cntritem.h \
	z:\source\kindel\winprt2\winprint\cfgview.h


QUEVIEW_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\winprint.h \
	z:\source\kindel\winprt2\winprint\util.h \
	z:\source\kindel\winprt2\winprint\shadow.h \
	z:\source\kindel\winprt2\winprint\aboutbox.h \
	z:\source\kindel\winprt2\winprint\winprdoc.h \
	z:\source\kindel\winprt2\winprint\cntritem.h \
	z:\source\kindel\winprt2\winprint\queview.h \
	z:\source\kindel\winprt2\winprint\view.h \
	z:\source\kindel\winprt2\winprint\collist.h


CFGLSTVW_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\winprint.h \
	z:\source\kindel\winprt2\winprint\util.h \
	z:\source\kindel\winprt2\winprint\shadow.h \
	z:\source\kindel\winprt2\winprint\aboutbox.h \
	z:\source\kindel\winprt2\winprint\winprdoc.h \
	z:\source\kindel\winprt2\winprint\cntritem.h \
	z:\source\kindel\winprt2\winprint\cfglstvw.h \
	z:\source\kindel\winprt2\winprint\view.h


UTIL_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\winprint.h \
	z:\source\kindel\winprt2\winprint\util.h \
	z:\source\kindel\winprt2\winprint\shadow.h \
	z:\source\kindel\winprt2\winprint\aboutbox.h


SHADOW_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\shadow.h


COLLIST_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\collist.h \
	z:\source\kindel\winprt2\winprint\util.h


VIEW_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\view.h \
	z:\source\kindel\winprt2\winprint\collist.h


CFGFRAME_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\itsybits.h \
	z:\source\kindel\winprt2\winprint\winprint.h \
	z:\source\kindel\winprt2\winprint\util.h \
	z:\source\kindel\winprt2\winprint\shadow.h \
	z:\source\kindel\winprt2\winprint\aboutbox.h \
	z:\source\kindel\winprt2\winprint\winprdoc.h \
	z:\source\kindel\winprt2\winprint\cfgframe.h \
	z:\source\kindel\winprt2\winprint\cfgview.h \
	z:\source\kindel\winprt2\winprint\cntritem.h


ABOUTBOX_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\aboutbox.h


ITSYBITS_DEP = z:\source\kindel\winprt2\winprint\stdafx.h \
	d:\inc\ctl3d.h \
	z:\source\kindel\winprt2\winprint\winprint.h \
	z:\source\kindel\winprt2\winprint\util.h \
	z:\source\kindel\winprt2\winprint\shadow.h \
	z:\source\kindel\winprt2\winprint\aboutbox.h \
	z:\source\kindel\winprt2\winprint\itsybits.h


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

CNTRITEM.OBJ:	CNTRITEM.CPP $(CNTRITEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CNTRITEM.CPP

CFGVIEW.OBJ:	CFGVIEW.CPP $(CFGVIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CFGVIEW.CPP

QUEVIEW.OBJ:	QUEVIEW.CPP $(QUEVIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c QUEVIEW.CPP

CFGLSTVW.OBJ:	CFGLSTVW.CPP $(CFGLSTVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CFGLSTVW.CPP

UTIL.OBJ:	UTIL.CPP $(UTIL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c UTIL.CPP

SHADOW.OBJ:	SHADOW.CPP $(SHADOW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SHADOW.CPP

COLLIST.OBJ:	COLLIST.CPP $(COLLIST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLLIST.CPP

VIEW.OBJ:	VIEW.CPP $(VIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c VIEW.CPP

CFGFRAME.OBJ:	CFGFRAME.CPP $(CFGFRAME_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CFGFRAME.CPP

ABOUTBOX.OBJ:	ABOUTBOX.CPP $(ABOUTBOX_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ABOUTBOX.CPP

ITSYBITS.OBJ:	ITSYBITS.CPP $(ITSYBITS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ITSYBITS.CPP


$(PROJ).EXE::	WINPRINT.RES

$(PROJ).EXE::	STDAFX.OBJ WINPRINT.OBJ MAINFRM.OBJ WINPRDOC.OBJ CNTRITEM.OBJ CFGVIEW.OBJ \
	QUEVIEW.OBJ CFGLSTVW.OBJ UTIL.OBJ SHADOW.OBJ COLLIST.OBJ VIEW.OBJ CFGFRAME.OBJ ABOUTBOX.OBJ \
	ITSYBITS.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
WINPRINT.OBJ +
MAINFRM.OBJ +
WINPRDOC.OBJ +
CNTRITEM.OBJ +
CFGVIEW.OBJ +
QUEVIEW.OBJ +
CFGLSTVW.OBJ +
UTIL.OBJ +
SHADOW.OBJ +
COLLIST.OBJ +
VIEW.OBJ +
CFGFRAME.OBJ +
ABOUTBOX.OBJ +
ITSYBITS.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
D:\MSVC\CDK16\LIB\+
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
