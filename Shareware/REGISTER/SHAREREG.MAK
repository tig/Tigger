# Microsoft Visual C++ generated build script - Do not modify

PROJ = SHAREREG
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\KINDEL\REGISTER\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = REG.C       
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /Od /D "_DEBUG" /FR /GA /Fd"SHAREREG.PDB"
CFLAGS_R_WEXE = /nologo /G2 /W3 /AM /O1 /D "NDEBUG" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = oldnames libw mlibcew reglib\reglib.lib commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WEXE = oldnames libw mlibcew reglib\reglib.lib commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = SHAREREG.DEF
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
SBRS = REG.SBR \
		SHADOW.SBR \
		SHAREREG.SBR \
		TOOLBAR.SBR \
		UTILS.SBR


REG_DEP = d:\inc\ceklib.h \
	d:\inc\ceklibd.h \
	e:\source\kindel\register\sharereg.h \
	d:\inc\wdb.h \
	e:\source\kindel\register\resource.h


SHADOW_DEP = e:\source\kindel\register\sharereg.h \
	d:\inc\wdb.h


SHAREREG_DEP = d:\inc\ceklib.h \
	d:\inc\ceklibd.h \
	e:\source\kindel\register\resource.h \
	e:\source\kindel\register\sharereg.h \
	d:\inc\wdb.h


TOOLBAR_DEP = e:\source\kindel\register\sharereg.h \
	d:\inc\wdb.h \
	e:\source\kindel\register\resource.h


UTILS_DEP = e:\source\kindel\register\sharereg.h \
	d:\inc\wdb.h


SHAREREG_RCDEP = e:\source\kindel\register\resource.h \
	e:\source\kindel\register\sharereg.ico \
	e:\source\kindel\register\about.dlg


all:	$(PROJ).EXE $(PROJ).BSC

REG.OBJ:	REG.C $(REG_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c REG.C

SHADOW.OBJ:	SHADOW.C $(SHADOW_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SHADOW.C

SHAREREG.OBJ:	SHAREREG.C $(SHAREREG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SHAREREG.C

TOOLBAR.OBJ:	TOOLBAR.C $(TOOLBAR_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c TOOLBAR.C

UTILS.OBJ:	UTILS.C $(UTILS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c UTILS.C

SHAREREG.RES:	SHAREREG.RC $(SHAREREG_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r SHAREREG.RC


$(PROJ).EXE::	SHAREREG.RES

$(PROJ).EXE::	REG.OBJ SHADOW.OBJ SHAREREG.OBJ TOOLBAR.OBJ UTILS.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
REG.OBJ +
SHADOW.OBJ +
SHAREREG.OBJ +
TOOLBAR.OBJ +
UTILS.OBJ +
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
	$(RC) $(RESFLAGS) SHAREREG.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	SHAREREG.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) SHAREREG.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
