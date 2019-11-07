# Microsoft Visual C++ generated build script - Do not modify

PROJ = XRT_OBJ
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\XRT\OBJECT\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = CLSID.C     
FIRSTCPP = XRT_OBJ.CPP 
RC = rc
CFLAGS_D_WEXE = /nologo /W3 /FR /G2 /Zi /D_DEBUG /Od /AM /GA /Fd"XRT_OBJ.PDB"
CFLAGS_R_WEXE = /nologo /W3 /FR /O1 /DNDEBUG /AM /GA
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = mafxcwd oldnames libw mlibcew ole2 commdlg.lib shell.lib 
LIBS_R_WEXE = mafxcw oldnames libw mlibcew ole2 commdlg.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = XRT_OBJ.DEF
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
SBRS = XRT_OBJ.SBR \
		CLSID.SBR \
		DATAOBJ.SBR \
		IENUMFE.SBR \
		RENDER.SBR \
		MAINWND.SBR


XRT_OBJ_DEP = e:\source\xrt\object\dataobj.h \
	e:\source\xrt\object\stdafx.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	e:\source\xrt\object\clsid.h \
	e:\source\xrt\object\xrt_obj.h


XRT_OBJ_RCDEP = e:\source\xrt\object\xrt_obj.ico


CLSID_DEP = d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	e:\source\xrt\object\clsid.h


DATAOBJ_DEP = e:\source\xrt\object\dataobj.h \
	e:\source\xrt\object\stdafx.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	e:\source\xrt\object\clsid.h


IENUMFE_DEP = e:\source\xrt\object\dataobj.h \
	e:\source\xrt\object\stdafx.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	e:\source\xrt\object\clsid.h


RENDER_DEP = e:\source\xrt\object\dataobj.h \
	e:\source\xrt\object\stdafx.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	e:\source\xrt\object\clsid.h


all:	$(PROJ).EXE $(PROJ).BSC

XRT_OBJ.OBJ:	XRT_OBJ.CPP $(XRT_OBJ_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c XRT_OBJ.CPP

XRT_OBJ.RES:	XRT_OBJ.RC $(XRT_OBJ_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r XRT_OBJ.RC

CLSID.OBJ:	CLSID.C $(CLSID_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c CLSID.C

DATAOBJ.OBJ:	DATAOBJ.CPP $(DATAOBJ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DATAOBJ.CPP

IENUMFE.OBJ:	IENUMFE.CPP $(IENUMFE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IENUMFE.CPP

RENDER.OBJ:	RENDER.CPP $(RENDER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c RENDER.CPP

MAINWND.OBJ:	MAINWND.CPP $(MAINWND_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINWND.CPP


$(PROJ).EXE::	XRT_OBJ.RES

$(PROJ).EXE::	XRT_OBJ.OBJ CLSID.OBJ DATAOBJ.OBJ IENUMFE.OBJ RENDER.OBJ MAINWND.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
XRT_OBJ.OBJ +
CLSID.OBJ +
DATAOBJ.OBJ +
IENUMFE.OBJ +
RENDER.OBJ +
MAINWND.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
c:\lib\+
d:\ole2\rel\+
e:\progole2\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) XRT_OBJ.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	XRT_OBJ.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) XRT_OBJ.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
