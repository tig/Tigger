# Microsoft Visual C++ generated build script - Do not modify

PROJ = TESTAP32
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
ORIGIN = MSVCNT
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
FIRSTC = DEFGUID.C
FIRSTCPP = IADVSINK.CPP
RC = rc
CFLAGS_D_WEXE32 = /nologo /Gz /W3 /Zi /YX"precomp.h" /D "_DEBUG" /D "_X86_" /D "_WINDOWS" /D "WIN32" /D "_INC_OLE" /FR /ML /Fd"TESTAP32.PDB"     /Fp"TESTAP32.PCH"
CFLAGS_R_WEXE32 = /nologo /Gz /W3 /YX"precomp.h" /O2 /D "NDEBUG" /D "_X86_" /D "_WINDOWS" /D "WIN32" /D "_INC_OLE" /FR /ML /Fp"TESTAP32.PCH"
LFLAGS_D_WEXE32 = /NOLOGO /VERSION:0,1 /MAP:"testap32.map" /DEBUG /DEBUGTYPE:both /SUBSYSTEM:windows uuid.lib uuid.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib ole232.lib compob32.lib
LFLAGS_R_WEXE32 = /NOLOGO /VERSION:0,1 /SUBSYSTEM:windows uuid.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib ole232.lib compob32.lib
LFLAGS_D_LIB32 = /NOLOGO
LFLAGS_R_LIB32 = /NOLOGO
LIBS_D_WEXE32 = 
LIBS_R_WEXE32 = 
RCFLAGS32 = 
D_RCDEFINES32 = /d_DEBUG /dWIN32
R_RCDEFINES32 = /dNDEBUG /dWIN32
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE32)
LFLAGS = $(LFLAGS_D_WEXE32)
LIBS = $(LIBS_D_WEXE32)
LFLAGS_LIB=$(LFLAGS_D_LIB32)
MAPFILE_OPTION = -map:$(PROJ).map
RCDEFINES = $(D_RCDEFINES32)
!else
CFLAGS = $(CFLAGS_R_WEXE32)
LFLAGS = $(LFLAGS_R_WEXE32)
LIBS = $(LIBS_R_WEXE32)
MAPFILE_OPTION = 
LFLAGS_LIB=$(LFLAGS_R_LIB32)
RCDEFINES = $(R_RCDEFINES32)
!endif
SBRS = IADVSINK.SBR \
		IDATAOBJ.SBR \
		PRECOMP.SBR \
		TESTAPP.SBR \
		UTIL.SBR \
		DEFGUID.SBR


IADVSINK_DEP =  \
	e:\source\xrt\test\precomp.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	e:\source\xrt\test\xrttest.h \
	..\\wosaxrt.h \
	e:\source\xrt\test\util.h \
	e:\source\xrt\test\idataobj.h


IDATAOBJ_DEP =  \
	e:\source\xrt\test\precomp.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	e:\source\xrt\test\xrttest.h \
	..\\wosaxrt.h \
	e:\source\xrt\test\util.h \
	e:\source\xrt\test\idataobj.h


PRECOMP_DEP =  \
	e:\source\xrt\test\precomp.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h


TESTAPP_DEP =  \
	e:\source\xrt\test\precomp.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	e:\source\xrt\test\xrttest.h \
	..\\wosaxrt.h \
	e:\source\xrt\test\util.h \
	e:\source\xrt\test\idataobj.h \
	e:\source\xrt\test\debug.h


UTIL_DEP =  \
	e:\source\xrt\test\precomp.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	e:\source\xrt\test\xrttest.h \
	..\\wosaxrt.h \
	e:\source\xrt\test\util.h


DEFGUID_DEP =  \
	e:\source\xrt\test\precomp.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h


XRTTEST_RCDEP =  \
	e:\source\xrt\test\xrttest.rc2


all:	$(PROJ).EXE $(PROJ).BSC

IADVSINK.OBJ:	IADVSINK.CPP $(IADVSINK_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c IADVSINK.CPP

IDATAOBJ.OBJ:	IDATAOBJ.CPP $(IDATAOBJ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IDATAOBJ.CPP

PRECOMP.OBJ:	PRECOMP.CPP $(PRECOMP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PRECOMP.CPP

TESTAPP.OBJ:	TESTAPP.CPP $(TESTAPP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TESTAPP.CPP

UTIL.OBJ:	UTIL.CPP $(UTIL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c UTIL.CPP

DEFGUID.OBJ:	DEFGUID.C $(DEFGUID_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c DEFGUID.C

XRTTEST.RES:	XRTTEST.RC $(XRTTEST_RCDEP)
	$(RC) $(RCFLAGS32) $(RCDEFINES) -r XRTTEST.RC


$(PROJ).EXE:	XRTTEST.RES

$(PROJ).EXE:	IADVSINK.OBJ IDATAOBJ.OBJ PRECOMP.OBJ TESTAPP.OBJ UTIL.OBJ DEFGUID.OBJ $(OBJS_EXT) $(LIBS_EXT)
	echo >NUL @<<$(PROJ).CRF
IADVSINK.OBJ 
IDATAOBJ.OBJ 
PRECOMP.OBJ 
TESTAPP.OBJ 
UTIL.OBJ 
DEFGUID.OBJ 
$(OBJS_EXT)
-OUT:$(PROJ).EXE
$(MAPFILE_OPTION)
XRTTEST.RES
$(LIBS)
$(LIBS_EXT)
$(DEFFILE_OPTION) -implib:$(PROJ).lib
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
