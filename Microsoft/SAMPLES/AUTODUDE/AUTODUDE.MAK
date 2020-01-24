# Microsoft Visual C++ generated build script - Do not modify

PROJ = AUTODUDE
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = Yo!  Mamma!
DLLS = 
D_RCDEFINES = /d_DEBUG
R_RCDEFINES = /dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\AUTODUDE\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = /YcSTDAFX.H
CUSEPCHFLAG = 
CPPUSEPCHFLAG = /YuSTDAFX.H
FIRSTC = CLSID.C     
FIRSTCPP = STDAFX.CPP  
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /Od /D "_DEBUG" /FR /GA /Fd"AUTODUDE.PDB"
CFLAGS_R_WEXE = /nologo /Gs /G2 /W3 /AM /O1 /D "NDEBUG" /FR /GA
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = mafxcwd oldnames libw mlibcew ole2 ole2disp commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WEXE = mafxcw oldnames libw mlibcew ole2 ole2disp commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo /z
RESFLAGS = /nologo /t
RUNFLAGS = 
DEFFILE = AUTODUDE.DEF
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
		AUTODUDE.SBR \
		MAINFRM.SBR \
		AUTODDOC.SBR \
		AUTODVW.SBR \
		CLSID.SBR \
		DISPAUTO.SBR \
		IDATA.SBR


AUTODUDE_RCDEP = e:\source\autodude\res\autodude.ico \
	e:\source\autodude\res\autodude.rc2


STDAFX_DEP = e:\source\autodude\stdafx.h


AUTODUDE_DEP = e:\source\autodude\stdafx.h \
	e:\source\autodude\autodude.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	d:\ole2\rel\dispatch.h \
	d:\ole2\rel\variant.h \
	e:\source\autodude\dispauto.h \
	e:\source\autodude\clsid.h \
	e:\source\autodude\mainfrm.h \
	e:\source\autodude\autoddoc.h \
	e:\source\autodude\autodvw.h


MAINFRM_DEP = e:\source\autodude\stdafx.h \
	e:\source\autodude\autodude.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	d:\ole2\rel\dispatch.h \
	d:\ole2\rel\variant.h \
	e:\source\autodude\dispauto.h \
	e:\source\autodude\clsid.h \
	e:\source\autodude\mainfrm.h


AUTODDOC_DEP = e:\source\autodude\stdafx.h \
	e:\source\autodude\autodude.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	d:\ole2\rel\dispatch.h \
	d:\ole2\rel\variant.h \
	e:\source\autodude\dispauto.h \
	e:\source\autodude\clsid.h \
	e:\source\autodude\autoddoc.h


AUTODVW_DEP = e:\source\autodude\stdafx.h \
	e:\source\autodude\autodude.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	d:\ole2\rel\dispatch.h \
	d:\ole2\rel\variant.h \
	e:\source\autodude\dispauto.h \
	e:\source\autodude\clsid.h \
	e:\source\autodude\autoddoc.h \
	e:\source\autodude\autodvw.h


CLSID_DEP = d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	e:\source\autodude\clsid.h


DISPAUTO_DEP = e:\source\autodude\stdafx.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	d:\ole2\rel\olenls.h \
	d:\ole2\rel\dispatch.h \
	d:\ole2\rel\variant.h \
	e:\source\autodude\autodude.h \
	e:\source\autodude\dispauto.h \
	e:\source\autodude\clsid.h \
	e:\source\autodude\autoddoc.h


IDATA_DEP = e:\source\autodude\stdafx.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	d:\ole2\rel\olenls.h \
	d:\ole2\rel\dispatch.h \
	d:\ole2\rel\variant.h \
	e:\source\autodude\dispauto.h \
	e:\source\autodude\clsid.h


all:	$(PROJ).EXE $(PROJ).BSC

AUTODUDE.RES:	AUTODUDE.RC $(AUTODUDE_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r AUTODUDE.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

AUTODUDE.OBJ:	AUTODUDE.CPP $(AUTODUDE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c AUTODUDE.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

AUTODDOC.OBJ:	AUTODDOC.CPP $(AUTODDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c AUTODDOC.CPP

AUTODVW.OBJ:	AUTODVW.CPP $(AUTODVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c AUTODVW.CPP

CLSID.OBJ:	CLSID.C $(CLSID_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c CLSID.C

DISPAUTO.OBJ:	DISPAUTO.CPP $(DISPAUTO_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DISPAUTO.CPP

IDATA.OBJ:	IDATA.CPP $(IDATA_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IDATA.CPP


$(PROJ).EXE::	AUTODUDE.RES

$(PROJ).EXE::	STDAFX.OBJ AUTODUDE.OBJ MAINFRM.OBJ AUTODDOC.OBJ AUTODVW.OBJ CLSID.OBJ \
	DISPAUTO.OBJ IDATA.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
AUTODUDE.OBJ +
MAINFRM.OBJ +
AUTODDOC.OBJ +
AUTODVW.OBJ +
CLSID.OBJ +
DISPAUTO.OBJ +
IDATA.OBJ +
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
	$(RC) $(RESFLAGS) AUTODUDE.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	AUTODUDE.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) AUTODUDE.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
