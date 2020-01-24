# Microsoft Visual C++ generated build script - Do not modify

PROJ = XRTXLL
DEBUG = 0
PROGTYPE = 1
CALLER = d:\xl4\excel.exe
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\XRT\EXCEL5\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = FRAMEWRK.C  
FIRSTCPP = XRTXLL.CPP  
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /W3 /WX /Zi /AMw /YX"precomp.h" /Od /D "_DEBUG" /FR /GD /Fd"XRTXLL.PDB"   /Fp"XRTXLL.PCH"
CFLAGS_R_WDLL = /nologo /G2 /W3 /WX /AMw /YX"precomp.h" /O1 /D "NDEBUG" /FR /GD /Fp"XRTXLL.PCH"
LFLAGS_D_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /MAP:FULL
LIBS_D_WDLL = oldnames libw mdllcew xlcall ole2 compobj ddeml.lib 
LIBS_R_WDLL = oldnames libw mdllcew xlcall ole2 compobj commdlg.lib ddeml.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = XRTXLL.DEF
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
SBRS = XRTXLL.SBR \
		FN.SBR \
		FRAMEWRK.SBR \
		CLSID.SBR \
		IADVSINK.SBR \
		WRAPPER.SBR \
		DDESRV.SBR


XRTXLL_DEP = e:\source\xrt\excel5\precomp.h \
	e:\xlsdk\include\xlcall.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	e:\source\xrt\excel5\framewrk.h \
	e:\source\xrt\excel5\debug.h \
	e:\source\xrt\excel5\clsid.h \
	e:\source\xrt\excel5\fn.h \
	e:\source\xrt\excel5\ddesrv.h \
	e:\source\xrt\excel5\wrapper.h


FN_DEP = e:\source\xrt\excel5\precomp.h \
	e:\xlsdk\include\xlcall.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	e:\source\xrt\excel5\framewrk.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\excel5\clsid.h \
	e:\source\xrt\excel5\fn.h \
	e:\source\xrt\excel5\iadvsink.h \
	e:\source\xrt\excel5\ddesrv.h \
	e:\source\xrt\excel5\debug.h


FRAMEWRK_DEP = e:\xlsdk\include\xlcall.h \
	e:\source\xrt\excel5\framewrk.h


CLSID_DEP = d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	e:\source\xrt\excel5\clsid.h


IADVSINK_DEP = e:\source\xrt\excel5\precomp.h \
	e:\xlsdk\include\xlcall.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\excel5\fn.h \
	e:\source\xrt\excel5\iadvsink.h


WRAPPER_DEP = e:\source\xrt\excel5\wrapper.h


DDESRV_DEP = e:\source\xrt\excel5\precomp.h \
	e:\xlsdk\include\xlcall.h \
	d:\ole2\rel\ole2.h \
	d:\ole2\rel\compobj.h \
	d:\ole2\rel\scode.h \
	d:\ole2\rel\initguid.h \
	d:\ole2\rel\coguid.h \
	d:\ole2\rel\oleguid.h \
	d:\ole2\rel\dvobj.h \
	d:\ole2\rel\storage.h \
	d:\ole2\rel\moniker.h \
	e:\source\xrt\excel5\wrapper.h \
	e:\source\xrt\excel5\fn.h \
	e:\source\xrt\excel5\ddesrv.h \
	e:\source\xrt\excel5\debug.h


all:	$(PROJ).DLL $(PROJ).BSC

XRTXLL.OBJ:	XRTXLL.CPP $(XRTXLL_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c XRTXLL.CPP

FN.OBJ:	FN.CPP $(FN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FN.CPP

FRAMEWRK.OBJ:	FRAMEWRK.C $(FRAMEWRK_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c FRAMEWRK.C

CLSID.OBJ:	CLSID.C $(CLSID_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c CLSID.C

IADVSINK.OBJ:	IADVSINK.CPP $(IADVSINK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IADVSINK.CPP

WRAPPER.OBJ:	WRAPPER.C $(WRAPPER_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c WRAPPER.C

DDESRV.OBJ:	DDESRV.CPP $(DDESRV_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DDESRV.CPP


$(PROJ).DLL::	XRTXLL.OBJ FN.OBJ FRAMEWRK.OBJ CLSID.OBJ IADVSINK.OBJ WRAPPER.OBJ DDESRV.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
XRTXLL.OBJ +
FN.OBJ +
FRAMEWRK.OBJ +
CLSID.OBJ +
IADVSINK.OBJ +
WRAPPER.OBJ +
DDESRV.OBJ +
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
	$(RC) $(RESFLAGS) $@
	implib /nowep $(PROJ).LIB $(PROJ).DLL


run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
