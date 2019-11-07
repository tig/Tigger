# Microsoft Visual C++ generated build script - Do not modify

PROJ = XRT_XL4
DEBUG = 1
PROGTYPE = 1
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\XRT\XRT_XL4\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = FRAMEWRK.C  
FIRSTCPP =             
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /W3 /Zi /AMw /Od /D "_DEBUG" /FR /GD /Fd"XRT_XL4.PDB"
CFLAGS_R_WDLL = /nologo /G2 /W3 /AMw /O1 /D "NDEBUG" /FR /GD 
LFLAGS_D_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /MAP:FULL
LIBS_D_WDLL = oldnames libw mdllcew xlcall ole2 compobj 
LIBS_R_WDLL = oldnames libw mdllcew xlcall ole2 compobj 
RCFLAGS = /nologo 
RESFLAGS = /nologo 
RUNFLAGS = 
DEFFILE = XRT_XL4.DEF
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
SBRS = FRAMEWRK.SBR \
		GENERIC.SBR \
		CLSID.SBR


FRAMEWRK_DEP = e:\xl4sdk\include\xlcall.h \
	e:\source\xrt\xrt_xl4\framewrk.h


GENERIC_DEP = d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	e:\xl4sdk\include\xlcall.h \
	e:\source\xrt\xrt_xl4\framewrk.h \
	e:\source\xrt\xrt_xl4\generic.h \
	e:\source\xrt\xrt_xl4\debug.h


CLSID_DEP = d:\ole2\rel\compobj.h


CLSID_DEP = d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	e:\source\xrt\xrt_xl4\clsid.h


all:	$(PROJ).DLL $(PROJ).BSC

FRAMEWRK.OBJ:	FRAMEWRK.C $(FRAMEWRK_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c FRAMEWRK.C

GENERIC.OBJ:	GENERIC.C $(GENERIC_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c GENERIC.C

XRT_XL4.RES:	XRT_XL4.RC $(XRT_XL4_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r XRT_XL4.RC

CLSID.OBJ:	CLSID.C $(CLSID_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c CLSID.C


$(PROJ).DLL::	XRT_XL4.RES

$(PROJ).DLL::	FRAMEWRK.OBJ GENERIC.OBJ CLSID.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
FRAMEWRK.OBJ +
GENERIC.OBJ +
CLSID.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
d:\ole2\rel\+
d:\lib\+
e:\xl4sdk\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) XRT_XL4.RES $@
	@copy $(PROJ).CRF MSVC.BND
	implib /nowep $(PROJ).LIB $(PROJ).DLL

$(PROJ).DLL::	XRT_XL4.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) XRT_XL4.RES $@

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
