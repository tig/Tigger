# Microsoft Visual C++ generated build script - Do not modify

PROJ = PROP
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG
R_RCDEFINES = /dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\RESEARCH\PROP\
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
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /Od /D "_DEBUG" /FR /GA /Fd"PROP.PDB"
CFLAGS_R_WEXE = /nologo /Gs /G2 /W3 /AM /O1 /D "NDEBUG" /FR /GA
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = mafxcwd oldnames libw mlibcew ole2 storage commdlg.lib shell.lib 
LIBS_R_WEXE = mafxcw oldnames libw mlibcew ole2 storage commdlg.lib shell.lib 
RCFLAGS = /nologo /z
RESFLAGS = /nologo /t
RUNFLAGS = 
DEFFILE = PROP.DEF
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
		PROP.SBR \
		MAINFRM.SBR \
		PROPDOC.SBR \
		PROPVIEW.SBR \
		PROPSET.SBR \
		CLSID.SBR


PROP_RCDEP = e:\source\research\prop\res\prop.ico \
	e:\source\research\prop\res\prop.rc2


STDAFX_DEP = e:\source\research\prop\stdafx.h


PROP_DEP = e:\source\research\prop\stdafx.h \
	e:\source\research\prop\prop.h \
	e:\source\research\prop\mainfrm.h \
	e:\source\research\prop\propdoc.h \
	e:\source\research\prop\propview.h


MAINFRM_DEP = e:\source\research\prop\stdafx.h \
	e:\source\research\prop\prop.h \
	e:\source\research\prop\mainfrm.h


PROPDOC_DEP = e:\source\research\prop\stdafx.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	e:\source\research\prop\prop.h \
	e:\source\research\prop\propdoc.h


PROPVIEW_DEP = e:\source\research\prop\stdafx.h \
	e:\source\research\prop\prop.h \
	e:\source\research\prop\propdoc.h \
	e:\source\research\prop\propview.h


PROPSET_DEP = e:\source\research\prop\stdafx.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	d:\ole2\rel\variant.h \
	e:\source\research\prop\propset.h


CLSID_DEP = d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	e:\source\research\prop\clsid.h


all:	$(PROJ).EXE $(PROJ).BSC

PROP.RES:	PROP.RC $(PROP_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r PROP.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

PROP.OBJ:	PROP.CPP $(PROP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PROP.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

PROPDOC.OBJ:	PROPDOC.CPP $(PROPDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PROPDOC.CPP

PROPVIEW.OBJ:	PROPVIEW.CPP $(PROPVIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PROPVIEW.CPP

PROPSET.OBJ:	PROPSET.CPP $(PROPSET_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PROPSET.CPP

CLSID.OBJ:	CLSID.C $(CLSID_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c CLSID.C


$(PROJ).EXE::	PROP.RES

$(PROJ).EXE::	STDAFX.OBJ PROP.OBJ MAINFRM.OBJ PROPDOC.OBJ PROPVIEW.OBJ PROPSET.OBJ \
	CLSID.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
PROP.OBJ +
MAINFRM.OBJ +
PROPDOC.OBJ +
PROPVIEW.OBJ +
PROPSET.OBJ +
CLSID.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
d:\ole2\rel\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) PROP.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	PROP.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) PROP.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
