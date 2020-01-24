# Microsoft Visual C++ generated build script - Do not modify

PROJ = XRTXLL32
DEBUG = 0
PROGTYPE = 1
CALLER = 
ARGS = 
DLLS = 
ORIGIN = MSVCNT
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\XRT\XRTXLL\ 
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = CLSID.C
FIRSTCPP = DDESRV.CPP
RC = rc
CFLAGS_D_WDLL32 = /nologo /Zp1 /W3 /Zi /YX /D "_DEBUG" /D "_X86_" /D "WIN32" /D "_INC_OLE" /FR /ML /Fd"XRTXLL32.PDB"         /Fp"XRTXLL32.PCH"
CFLAGS_R_WDLL32 = /nologo /Zp1 /W3 /YX /O2 /D "NDEBUG" /D "_X86_" /D "WIN32" /D "_INC_OLE" /FR /ML /Fp"XRTXLL32.PCH"
LFLAGS_D_WDLL32 = /NOLOGO /VERSION:0,1 /MAP:"xrtxll32.map" /DEBUG /DEBUGTYPE:both /DEF:"xrtxll32.def" /SUBSYSTEM:windows user32.lib gdi32.lib advapi32.lib ole232.lib compob32.lib uuid.lib g:\xl4nt\xlcall\xlcall.lib
LFLAGS_R_WDLL32 = /NOLOGO /VERSION:0,1 /DEF:"xrtxll32.def" /SUBSYSTEM:windows user32.lib gdi32.lib advapi32.lib ole232.lib compob32.lib uuid.lib g:\xl4nt\xlcall\xlcall.lib
LFLAGS_D_LIB32 = /NOLOGO
LFLAGS_R_LIB32 = /NOLOGO
LIBS_D_WDLL32 = 
LIBS_R_WDLL32 = 
RCFLAGS32 = 
D_RCDEFINES32 = -d_DEBUG
R_RCDEFINES32 = -dNDEBUG
DEFFILE = XRTXLL32.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WDLL32)
LFLAGS = $(LFLAGS_D_WDLL32)
LIBS = $(LIBS_D_WDLL32)
LFLAGS_LIB=$(LFLAGS_D_LIB32)
MAPFILE_OPTION = -map:$(PROJ).map
DEFFILE_OPTION = 
RCDEFINES = $(D_RCDEFINES32)
!else
CFLAGS = $(CFLAGS_R_WDLL32)
LFLAGS = $(LFLAGS_R_WDLL32)
LIBS = $(LIBS_R_WDLL32)
MAPFILE_OPTION = 
DEFFILE_OPTION = -def:$(DEFFILE)
LFLAGS_LIB=$(LFLAGS_R_LIB32)
RCDEFINES = $(R_RCDEFINES32)
!endif
SBRS = CLSID.SBR \
		DDESRV.SBR \
		FN.SBR \
		FRAMEWRK.SBR \
		IADVSINK.SBR \
		WRAPPER.SBR \
		XRTXLL.SBR


CLSID_DEP =  \
	e:\source\xrt\xrtxll\precomp.h \
	g:\xl4nt\xlcall\xlcall.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	e:\source\xrt\xrtxll\clsid.h


DDESRV_DEP =  \
	e:\source\xrt\xrtxll\precomp.h \
	g:\xl4nt\xlcall\xlcall.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	e:\source\xrt\xrtxll\wrapper.h \
	e:\source\xrt\xrtxll\fn.h \
	e:\source\xrt\xrtxll\ddesrv.h \
	e:\source\xrt\xrtxll\debug.h


FN_DEP =  \
	e:\source\xrt\xrtxll\precomp.h \
	g:\xl4nt\xlcall\xlcall.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	e:\source\xrt\xrtxll\framewrk.h \
	..\\wosaxrt.h \
	e:\source\xrt\xrtxll\clsid.h \
	e:\source\xrt\xrtxll\fn.h \
	e:\source\xrt\xrtxll\iadvsink.h \
	e:\source\xrt\xrtxll\ddesrv.h \
	e:\source\xrt\xrtxll\debug.h


FRAMEWRK_DEP =  \
	g:\xl4nt\xlcall\xlcall.h \
	e:\source\xrt\xrtxll\framewrk.h


IADVSINK_DEP =  \
	e:\source\xrt\xrtxll\precomp.h \
	g:\xl4nt\xlcall\xlcall.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	..\\wosaxrt.h \
	e:\source\xrt\xrtxll\fn.h \
	e:\source\xrt\xrtxll\iadvsink.h


WRAPPER_DEP =  \
	e:\source\xrt\xrtxll\precomp.h \
	g:\xl4nt\xlcall\xlcall.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	e:\source\xrt\xrtxll\wrapper.h


XRTXLL_DEP =  \
	e:\source\xrt\xrtxll\precomp.h \
	g:\xl4nt\xlcall\xlcall.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	e:\source\xrt\xrtxll\framewrk.h \
	e:\source\xrt\xrtxll\debug.h \
	e:\source\xrt\xrtxll\clsid.h \
	e:\source\xrt\xrtxll\fn.h \
	e:\source\xrt\xrtxll\ddesrv.h \
	e:\source\xrt\xrtxll\wrapper.h


XRTXLL_RCDEP =  \
	e:\source\xrt\xrtxll\resource.h


all:	$(PROJ).DLL $(PROJ).BSC

CLSID.OBJ:	CLSID.C $(CLSID_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c CLSID.C

DDESRV.OBJ:	DDESRV.CPP $(DDESRV_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c DDESRV.CPP

FN.OBJ:	FN.CPP $(FN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FN.CPP

FRAMEWRK.OBJ:	FRAMEWRK.C $(FRAMEWRK_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c FRAMEWRK.C

IADVSINK.OBJ:	IADVSINK.CPP $(IADVSINK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IADVSINK.CPP

WRAPPER.OBJ:	WRAPPER.C $(WRAPPER_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c WRAPPER.C

XRTXLL.OBJ:	XRTXLL.CPP $(XRTXLL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c XRTXLL.CPP

XRTXLL.RES:	XRTXLL.RC $(XRTXLL_RCDEP)
	$(RC) $(RCFLAGS32) $(RCDEFINES) -r XRTXLL.RC


$(PROJ).DLL:	XRTXLL.RES

$(PROJ).DLL:	CLSID.OBJ DDESRV.OBJ FN.OBJ FRAMEWRK.OBJ IADVSINK.OBJ WRAPPER.OBJ XRTXLL.OBJ $(OBJS_EXT) $(LIBS_EXT) $(DEFFILE) XRTXLL.RES
	echo >NUL @<<$(PROJ).CRF
CLSID.OBJ 
DDESRV.OBJ 
FN.OBJ 
FRAMEWRK.OBJ 
IADVSINK.OBJ 
WRAPPER.OBJ 
XRTXLL.OBJ 
$(OBJS_EXT)
-DLL -OUT:$(PROJ).DLL
$(MAPFILE_OPTION)
XRTXLL.RES
$(LIBS)
$(LIBS_EXT)
$(DEFFILE_OPTION) -implib:$(PROJ).lib
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
