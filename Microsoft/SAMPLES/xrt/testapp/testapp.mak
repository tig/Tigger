# Microsoft Visual C++ generated build script - Do not modify

PROJ = TESTAPP
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\XRT\TEST\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = IADVSINK.CPP
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /YX"PRECOMP.H" /Od /D "_DEBUG" /FR /GA /Fd"TESTAPP.PDB" /Fp"TESTAPP.PCH"
CFLAGS_R_WEXE = /nologo /W3 /AM /YX"PRECOMP.H" /O1 /D "NDEBUG" /FR /GA /Fp"TESTAPP.PCH"
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = oldnames libw mlibcew ole2 ver.lib 
LIBS_R_WEXE = oldnames libw mlibcew ole2 ver.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = TESTAPP.DEF
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
SBRS = IADVSINK.SBR \
		IDATAOBJ.SBR \
		PRECOMP.SBR \
		UTIL.SBR \
		TESTAPP.SBR


IADVSINK_DEP = e:\source\xrt\test\precomp.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	e:\source\xrt\test\xrttest.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\test\util.h \
	e:\source\xrt\test\idataobj.h


IDATAOBJ_DEP = e:\source\xrt\test\precomp.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	e:\source\xrt\test\xrttest.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\test\util.h \
	e:\source\xrt\test\idataobj.h


PRECOMP_DEP = e:\source\xrt\test\precomp.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h


UTIL_DEP = e:\source\xrt\test\precomp.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	e:\source\xrt\test\xrttest.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\test\util.h


TESTAPP_DEP = e:\source\xrt\test\precomp.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	e:\source\xrt\test\xrttest.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\test\util.h \
	e:\source\xrt\test\idataobj.h \
	e:\source\xrt\test\debug.h


XRTTEST_RCDEP = e:\source\xrt\test\xrttest.rc2


all:	$(PROJ).EXE $(PROJ).BSC

IADVSINK.OBJ:	IADVSINK.CPP $(IADVSINK_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c IADVSINK.CPP

IDATAOBJ.OBJ:	IDATAOBJ.CPP $(IDATAOBJ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IDATAOBJ.CPP

PRECOMP.OBJ:	PRECOMP.CPP $(PRECOMP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PRECOMP.CPP

UTIL.OBJ:	UTIL.CPP $(UTIL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c UTIL.CPP

TESTAPP.OBJ:	TESTAPP.CPP $(TESTAPP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TESTAPP.CPP

XRTTEST.RES:	XRTTEST.RC $(XRTTEST_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r XRTTEST.RC


$(PROJ).EXE::	XRTTEST.RES

$(PROJ).EXE::	IADVSINK.OBJ IDATAOBJ.OBJ PRECOMP.OBJ UTIL.OBJ TESTAPP.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
IADVSINK.OBJ +
IDATAOBJ.OBJ +
PRECOMP.OBJ +
UTIL.OBJ +
TESTAPP.OBJ +
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
	$(RC) $(RESFLAGS) XRTTEST.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	XRTTEST.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) XRTTEST.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
