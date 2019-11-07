# Microsoft Visual C++ generated build script - Do not modify

PROJ = XRTVIEW
DEBUG = 0
PROGTYPE = 1
CALLER = e:\source\ole2view.exe g:\xrtframe\testit1.xrf
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = Z:\XRTVIEW\
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
CFLAGS_D_WDLL = /nologo /W3 /FR /G2 /Zi /D_DEBUG /Od /GD /ALw /Fd"XRTVIEW.PDB"
CFLAGS_R_WDLL = /nologo /W3 /FR /O1 /DNDEBUG /GD /ALw
LFLAGS_D_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /MAP:FULL
LIBS_D_WDLL = oldnames libw ldllcew ole2 ole2disp commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WDLL = oldnames libw ldllcew ole2 ole2disp commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = XRTVIEW.DEF
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
SBRS = IADVSINK.SBR \
		IDATAOBJ.SBR \
		PRECOMP.SBR \
		UTIL.SBR \
		XRTVIEW.SBR


IADVSINK_DEP = z:\xrtview\precomp.h \
	z:\xrtview\wosaxrt.h \
	z:\xrtview\xrtview.h \
	z:\xrtview\util.h \
	z:\xrtview\idataobj.h


IDATAOBJ_DEP = z:\xrtview\precomp.h \
	z:\xrtview\wosaxrt.h \
	z:\xrtview\xrtview.h \
	z:\xrtview\util.h \
	z:\xrtview\idataobj.h


PRECOMP_DEP = z:\xrtview\precomp.h


UTIL_DEP = z:\xrtview\precomp.h \
	z:\xrtview\xrtview.h \
	z:\xrtview\util.h


XRTVIEW_DEP = z:\xrtview\precomp.h \
	z:\xrtview\xrtview.h \
	z:\xrtview\util.h


XRTVIEW_RCDEP = z:\xrtview\xrtview.rc2


all:	$(PROJ).DLL $(PROJ).BSC

IADVSINK.OBJ:	IADVSINK.CPP $(IADVSINK_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c IADVSINK.CPP

IDATAOBJ.OBJ:	IDATAOBJ.CPP $(IDATAOBJ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IDATAOBJ.CPP

PRECOMP.OBJ:	PRECOMP.CPP $(PRECOMP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PRECOMP.CPP

UTIL.OBJ:	UTIL.CPP $(UTIL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c UTIL.CPP

XRTVIEW.OBJ:	XRTVIEW.CPP $(XRTVIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c XRTVIEW.CPP

XRTVIEW.RES:	XRTVIEW.RC $(XRTVIEW_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r XRTVIEW.RC


$(PROJ).DLL::	XRTVIEW.RES

$(PROJ).DLL::	IADVSINK.OBJ IDATAOBJ.OBJ PRECOMP.OBJ UTIL.OBJ XRTVIEW.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
IADVSINK.OBJ +
IDATAOBJ.OBJ +
PRECOMP.OBJ +
UTIL.OBJ +
XRTVIEW.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
$(MAPFILE)
D:\MSVC\CDK16\LIB\+
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
	$(RC) $(RESFLAGS) XRTVIEW.RES $@
	@copy $(PROJ).CRF MSVC.BND
	implib /nowep $(PROJ).LIB $(PROJ).DLL

$(PROJ).DLL::	XRTVIEW.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) XRTVIEW.RES $@

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
