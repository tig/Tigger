# Microsoft Visual C++ generated build script - Do not modify

PROJ = XRTXLL
DEBUG = 1
PROGTYPE = 1
CALLER = d:\excel\exceld.exe
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\XRT\EXCEL4\
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


XRTXLL_DEP = e:\source\xrt\excel4\precomp.h \
	e:\xlsdk\include\xlcall.h \
	d:\msvc\include\ole2.h \
	d:\msvc\include\compobj.h \
	d:\msvc\include\scode.h \
	d:\msvc\include\initguid.h \
	d:\msvc\include\coguid.h \
	d:\msvc\include\oleguid.h \
	d:\msvc\include\dvobj.h \
	d:\msvc\include\storage.h \
	d:\msvc\include\moniker.h \
	e:\source\xrt\excel4\framewrk.h \
	e:\source\xrt\excel4\debug.h \
	e:\source\xrt\excel4\clsid.h \
	e:\source\xrt\excel4\fn.h \
	e:\source\xrt\excel4\ddesrv.h \
	e:\source\xrt\excel4\wrapper.h


FN_DEP = e:\source\xrt\excel4\precomp.h \
	e:\xlsdk\include\xlcall.h \
	d:\msvc\include\ole2.h \
	d:\msvc\include\compobj.h \
	d:\msvc\include\scode.h \
	d:\msvc\include\initguid.h \
	d:\msvc\include\coguid.h \
	d:\msvc\include\oleguid.h \
	d:\msvc\include\dvobj.h \
	d:\msvc\include\storage.h \
	d:\msvc\include\moniker.h \
	e:\source\xrt\excel4\framewrk.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\excel4\clsid.h \
	e:\source\xrt\excel4\fn.h \
	e:\source\xrt\excel4\iadvsink.h \
	e:\source\xrt\excel4\ddesrv.h \
	e:\source\xrt\excel4\debug.h


FRAMEWRK_DEP = e:\xlsdk\include\xlcall.h \
	e:\source\xrt\excel4\framewrk.h


CLSID_DEP = d:\msvc\include\compobj.h \
	d:\msvc\include\scode.h \
	d:\msvc\include\initguid.h \
	d:\msvc\include\coguid.h \
	e:\source\xrt\excel4\clsid.h


IADVSINK_DEP = e:\source\xrt\excel4\precomp.h \
	e:\xlsdk\include\xlcall.h \
	d:\msvc\include\ole2.h \
	d:\msvc\include\compobj.h \
	d:\msvc\include\scode.h \
	d:\msvc\include\initguid.h \
	d:\msvc\include\coguid.h \
	d:\msvc\include\oleguid.h \
	d:\msvc\include\dvobj.h \
	d:\msvc\include\storage.h \
	d:\msvc\include\moniker.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\excel4\fn.h \
	e:\source\xrt\excel4\iadvsink.h


WRAPPER_DEP = e:\source\xrt\excel4\wrapper.h


DDESRV_DEP = e:\source\xrt\excel4\precomp.h \
	e:\xlsdk\include\xlcall.h \
	d:\msvc\include\ole2.h \
	d:\msvc\include\compobj.h \
	d:\msvc\include\scode.h \
	d:\msvc\include\initguid.h \
	d:\msvc\include\coguid.h \
	d:\msvc\include\oleguid.h \
	d:\msvc\include\dvobj.h \
	d:\msvc\include\storage.h \
	d:\msvc\include\moniker.h \
	e:\source\xrt\excel4\wrapper.h \
	e:\source\xrt\excel4\fn.h \
	e:\source\xrt\excel4\ddesrv.h \
	e:\source\xrt\excel4\debug.h


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
d:\ole2\lib\+
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
