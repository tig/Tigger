# Microsoft Visual C++ generated build script - Do not modify

PROJ = XRTTEST
DEBUG = 1
PROGTYPE = 1
CALLER = e:\source\ole2view.exe
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
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
FIRSTCPP = IDATAOBJ.CPP
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /W3 /WX /Zi /AMw /YX"precomp.h" /Od /D "_DEBUG" /D "_USRDLL" /FR /GD /Fd"xrttest.PDB"  /Fp"xrttest.PCH"
CFLAGS_R_WDLL = /nologo /G2 /W3 /WX /AMw /YX"precomp.h" /O1 /D "NDEBUG" /D "_USRDLL" /FR /GD /Fp"xrttest.PCH"
LFLAGS_D_WDLL = /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO /MAP /MAP:FULL
LFLAGS_R_WDLL = /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /MAP /MAP:FULL
LIBS_D_WDLL = oldnames libw mdllcew OLE2 ole2disp typelib ver.lib 
LIBS_R_WDLL = oldnames libw mdllcew OLE2 ole2disp typelib ver.lib 
RCFLAGS = /nologo 
RESFLAGS = /nologo 
RUNFLAGS = 
DEFFILE = XRTTEST.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WDLL)
LFLAGS = $(LFLAGS_D_WDLL)
LIBS = $(LIBS_D_WDLL)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WDLL)
LFLAGS = $(LFLAGS_R_WDLL)
LIBS = $(LIBS_R_WDLL)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = IDATAOBJ.SBR \
		UTIL.SBR \
		XRTTEST.SBR \
		PRECOMP.SBR \
		IADVSINK.SBR


IDATAOBJ_DEP = e:\source\xrt\test\precomp.h \
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
	d:\ole2\rel\variant.h \
	e:\source\xrt\test\xrttest.h \
	d:\ole2\rel\dispatch.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\test\util.h \
	e:\source\xrt\test\idataobj.h


UTIL_DEP = e:\source\xrt\test\precomp.h \
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
	d:\ole2\rel\variant.h \
	e:\source\xrt\test\xrttest.h \
	d:\ole2\rel\dispatch.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\test\util.h


XRTTEST_DEP = e:\source\xrt\test\precomp.h \
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
	d:\ole2\rel\variant.h \
	e:\source\xrt\test\xrttest.h \
	d:\ole2\rel\dispatch.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\test\util.h


XRTTEST_RCDEP = e:\source\xrt\test\xrttest.rc2


PRECOMP_DEP = e:\source\xrt\test\precomp.h \
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
	d:\ole2\rel\variant.h


IADVSINK_DEP = e:\source\xrt\test\precomp.h \
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
	d:\ole2\rel\variant.h \
	e:\source\xrt\test\xrttest.h \
	d:\ole2\rel\dispatch.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\test\util.h \
	e:\source\xrt\test\idataobj.h


all:	$(PROJ).DLL $(PROJ).BSC

IDATAOBJ.OBJ:	IDATAOBJ.CPP $(IDATAOBJ_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c IDATAOBJ.CPP

UTIL.OBJ:	UTIL.CPP $(UTIL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c UTIL.CPP

XRTTEST.OBJ:	XRTTEST.CPP $(XRTTEST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c XRTTEST.CPP

XRTTEST.RES:	XRTTEST.RC $(XRTTEST_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r XRTTEST.RC

PRECOMP.OBJ:	PRECOMP.CPP $(PRECOMP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PRECOMP.CPP

IADVSINK.OBJ:	IADVSINK.CPP $(IADVSINK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IADVSINK.CPP


$(PROJ).DLL::	XRTTEST.RES

$(PROJ).DLL::	IDATAOBJ.OBJ UTIL.OBJ XRTTEST.OBJ PRECOMP.OBJ IADVSINK.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
IDATAOBJ.OBJ +
UTIL.OBJ +
XRTTEST.OBJ +
PRECOMP.OBJ +
IADVSINK.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
c:\lib\+
d:\ole2\rel\+
e:\progole2\lib\+
e:\xlsdk\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) XRTTEST.RES $@
	@copy $(PROJ).CRF MSVC.BND
	implib /nowep $(PROJ).LIB $(PROJ).DLL

$(PROJ).DLL::	XRTTEST.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) XRTTEST.RES $@

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
