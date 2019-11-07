# Microsoft Visual C++ generated build script - Do not modify

PROJ = COLDLL
DEBUG = 1
PROGTYPE = 1
CALLER = e:\source\ole2view.exe
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\RESEARCH\COLLECT\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = APP.CPP     
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /W3 /Zi /ALw /Od /D "_DEBUG" /FR /GD /Fd"COLDLL.PDB"
CFLAGS_R_WDLL = /nologo /W3 /ALw /O1 /D "NDEBUG" /FR /GD 
LFLAGS_D_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /MAP:FULL
LIBS_D_WDLL = oldnames libw ldllcew ole2 compobj ole2disp typelib commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WDLL = oldnames libw ldllcew ole2 compobj ole2disp typelib commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = COLDLL.DEF
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
SBRS = APP.SBR \
		APPCF.SBR \
		COLLECT.SBR \
		ENUMVAR.SBR \
		ITEM.SBR \
		MAIN.SBR


APP_DEP = d:\msvc\include\ole2.h \
	d:\msvc\include\compobj.h \
	d:\msvc\include\scode.h \
	d:\msvc\include\initguid.h \
	d:\msvc\include\coguid.h \
	d:\msvc\include\oleguid.h \
	d:\msvc\include\dvobj.h \
	d:\msvc\include\storage.h \
	d:\msvc\include\moniker.h \
	d:\msvc\include\dispatch.h \
	d:\msvc\include\variant.h \
	e:\source\research\collect\app.h \
	e:\source\research\collect\tlb.h


APPCF_DEP = d:\msvc\include\ole2.h \
	d:\msvc\include\compobj.h \
	d:\msvc\include\scode.h \
	d:\msvc\include\initguid.h \
	d:\msvc\include\coguid.h \
	d:\msvc\include\oleguid.h \
	d:\msvc\include\dvobj.h \
	d:\msvc\include\storage.h \
	d:\msvc\include\moniker.h \
	e:\source\research\collect\app.h \
	d:\msvc\include\dispatch.h \
	e:\source\research\collect\tlb.h


COLLECT_DEP = d:\msvc\include\ole2.h \
	d:\msvc\include\compobj.h \
	d:\msvc\include\scode.h \
	d:\msvc\include\initguid.h \
	d:\msvc\include\coguid.h \
	d:\msvc\include\oleguid.h \
	d:\msvc\include\dvobj.h \
	d:\msvc\include\storage.h \
	d:\msvc\include\moniker.h \
	d:\msvc\include\dispatch.h \
	d:\msvc\include\variant.h \
	e:\source\research\collect\app.h \
	e:\source\research\collect\tlb.h


ENUMVAR_DEP = d:\msvc\include\ole2.h \
	d:\msvc\include\compobj.h \
	d:\msvc\include\scode.h \
	d:\msvc\include\initguid.h \
	d:\msvc\include\coguid.h \
	d:\msvc\include\oleguid.h \
	d:\msvc\include\dvobj.h \
	d:\msvc\include\storage.h \
	d:\msvc\include\moniker.h \
	d:\msvc\include\dispatch.h \
	d:\msvc\include\variant.h \
	e:\source\research\collect\app.h \
	e:\source\research\collect\tlb.h


ITEM_DEP = d:\msvc\include\ole2.h \
	d:\msvc\include\compobj.h \
	d:\msvc\include\scode.h \
	d:\msvc\include\initguid.h \
	d:\msvc\include\coguid.h \
	d:\msvc\include\oleguid.h \
	d:\msvc\include\dvobj.h \
	d:\msvc\include\storage.h \
	d:\msvc\include\moniker.h \
	d:\msvc\include\dispatch.h \
	d:\msvc\include\variant.h \
	e:\source\research\collect\app.h \
	e:\source\research\collect\tlb.h


MAIN_DEP = d:\msvc\include\ole2.h \
	d:\msvc\include\compobj.h \
	d:\msvc\include\scode.h \
	d:\msvc\include\initguid.h \
	d:\msvc\include\coguid.h \
	d:\msvc\include\oleguid.h \
	d:\msvc\include\dvobj.h \
	d:\msvc\include\storage.h \
	d:\msvc\include\moniker.h \
	d:\msvc\include\olenls.h \
	d:\msvc\include\dispatch.h \
	d:\msvc\include\variant.h \
	e:\source\research\collect\app.h \
	e:\source\research\collect\tlb.h


APP_RCDEP = e:\source\research\collect\app.h \
	d:\msvc\include\ole2.h \
	d:\msvc\include\dispatch.h \
	e:\source\research\collect\tlb.h \
	e:\source\research\collect\app.ico


all:	$(PROJ).DLL $(PROJ).BSC

APP.OBJ:	APP.CPP $(APP_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c APP.CPP

APPCF.OBJ:	APPCF.CPP $(APPCF_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c APPCF.CPP

COLLECT.OBJ:	COLLECT.CPP $(COLLECT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLLECT.CPP

ENUMVAR.OBJ:	ENUMVAR.CPP $(ENUMVAR_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ENUMVAR.CPP

ITEM.OBJ:	ITEM.CPP $(ITEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ITEM.CPP

MAIN.OBJ:	MAIN.CPP $(MAIN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAIN.CPP

APP.RES:	APP.RC $(APP_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r APP.RC


$(PROJ).DLL::	APP.RES

$(PROJ).DLL::	APP.OBJ APPCF.OBJ COLLECT.OBJ ENUMVAR.OBJ ITEM.OBJ MAIN.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
APP.OBJ +
APPCF.OBJ +
COLLECT.OBJ +
ENUMVAR.OBJ +
ITEM.OBJ +
MAIN.OBJ +
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
	$(RC) $(RESFLAGS) APP.RES $@
	@copy $(PROJ).CRF MSVC.BND
	implib /nowep $(PROJ).LIB $(PROJ).DLL

$(PROJ).DLL::	APP.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) APP.RES $@

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
