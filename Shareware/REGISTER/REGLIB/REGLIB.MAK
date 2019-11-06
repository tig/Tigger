# Microsoft Visual C++ generated build script - Do not modify

PROJ = REGLIB
DEBUG = 0
PROGTYPE = 1
CALLER = ..\sharereg.exe
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\KINDEL\REGISTER\REGLIB\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = REGLIB.C    
FIRSTCPP =             
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /W3 /Zi /ALw /Od /D "_DEBUG" /FR /GD /Fd"REGLIB.PDB"
CFLAGS_R_WDLL = /nologo /G2 /W3 /ALw /O1 /D "NDEBUG" /FR /GD 
LFLAGS_D_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /NOE /ALIGN:16 /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /NOE /ALIGN:16 /MAP:FULL
LIBS_D_WDLL = oldnames libw commdlg shell olecli olesvr ldllcew
LIBS_R_WDLL = oldnames libw commdlg shell olecli olesvr ldllcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = REGLIB.DEF
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
SBRS = REGLIB.SBR \
		WEP.SBR


REGLIB_DEP = d:\inc\wdb.h \
	e:\source\kindel\register\reglib\reglib.h \
	e:\source\kindel\register\reglib\version.h


WEP_DEP = 

REGLIB_RCDEP = e:\source\kindel\register\reglib\isz.h \
	e:\source\kindel\register\reglib\reglib.rcv \
	e:\source\kindel\register\reglib\reglib.ico


all:	$(PROJ).DLL $(PROJ).BSC

REGLIB.OBJ:	REGLIB.C $(REGLIB_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c REGLIB.C

WEP.OBJ:	WEP.C $(WEP_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c WEP.C

REGLIB.RES:	REGLIB.RC $(REGLIB_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r REGLIB.RC


$(PROJ).DLL::	REGLIB.RES

$(PROJ).DLL::	REGLIB.OBJ WEP.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
REGLIB.OBJ +
WEP.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
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
	$(RC) $(RESFLAGS) REGLIB.RES $@
	@copy $(PROJ).CRF MSVC.BND
	implib /nowep $(PROJ).LIB $(PROJ).DLL

$(PROJ).DLL::	REGLIB.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) REGLIB.RES $@

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
