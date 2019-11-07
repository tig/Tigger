# Microsoft Visual C++ generated build script - Do not modify

PROJ = WINPRT20
DEBUG = 1
PROGTYPE = 1
CALLER = ..\winprint\winprint.exe
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = Z:\SOURCE\KINDEL\WINPRT2\WINPRT20\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = /YcSTDAFX.H
CUSEPCHFLAG = 
CPPUSEPCHFLAG = /YuSTDAFX.H
FIRSTC =             
FIRSTCPP = STDAFX.CPP  
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /Zp1 /W4 /WX /Zi /AL /Od /D "_DEBUG" /D "_USRDLL" /D "_WINDLL" /FR /GD /Fd"WINPRT20.PDB" 
CFLAGS_R_WDLL = /nologo /Gs /G2 /Zp1 /W3 /AL /Ox /D "NDEBUG" /D "_USRDLL" /D "_WINDLL" /FR /GD 
LFLAGS_D_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /NOE /PACKC:61440 /CO /ALIGN:16
LFLAGS_R_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /NOE /PACKC:61440 /ALIGN:16
LIBS_D_WDLL = oc25 lafxctld lafxdwd libw ldllcew commdlg shell compobj storage ole2 ole2disp typelib oldnames
LIBS_R_WDLL = oc25 lafxctl lafxdw libw ldllcew commdlg shell compobj storage ole2 ole2disp typelib oldnames
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = WINPRT20.DEF
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
SBRS = STDAFX.SBR \
		WINPRT20.SBR \
		ENGINE.SBR \
		CONTROL.SBR \
		CTLPPG.SBR \
		HEADER.SBR \
		HEADERSE.SBR \
		CPROP.SBR \
		PAGE.SBR \
		FTD.SBR \
		INPUT.SBR \
		WINTIME.SBR


WINPRT20_RCDEP = z:\source\kindel\winprt2\winprt20\winprt20.ico \
	z:\source\kindel\winprt2\winprt20\winprint.ico \
	z:\source\kindel\winprt2\winprt20\landscap.ico \
	z:\source\kindel\winprt2\winprt20\noorient.ico \
	z:\source\kindel\winprt2\winprt20\portrait.ico \
	z:\source\kindel\winprt2\winprt20\control.bmp \
	z:\source\kindel\winprt2\winprt20\winprt20.rc2 \
	z:\source\kindel\winprt2\winprt20\winprt20.tlb


STDAFX_DEP = z:\source\kindel\winprt2\winprt20\stdafx.h \
	d:\msvc\cdk16\include\afxctl.h \
	d:\msvc\cdk16\include\olectl.h \
	d:\msvc\cdk16\include\olectlid.h \
	d:\msvc\cdk16\include\oleconn.h \
	d:\msvc\cdk16\include\oletype.h \
	d:\msvc\cdk16\include\oledisp.h \
	d:\msvc\cdk16\include\oleprsh.h \
	d:\msvc\cdk16\include\olelic.h \
	d:\msvc\cdk16\include\olebind.h \
	d:\msvc\cdk16\include\afxctl.inl


WINPRT20_DEP = z:\source\kindel\winprt2\winprt20\stdafx.h \
	d:\msvc\cdk16\include\afxctl.h \
	d:\msvc\cdk16\include\olectl.h \
	d:\msvc\cdk16\include\olectlid.h \
	d:\msvc\cdk16\include\oleconn.h \
	d:\msvc\cdk16\include\oletype.h \
	d:\msvc\cdk16\include\oledisp.h \
	d:\msvc\cdk16\include\oleprsh.h \
	d:\msvc\cdk16\include\olelic.h \
	d:\msvc\cdk16\include\olebind.h \
	d:\msvc\cdk16\include\afxctl.inl \
	z:\source\kindel\winprt2\winprt20\winprt20.h \
	z:\source\kindel\winprt2\winprt20\cprop.h


ENGINE_DEP = z:\source\kindel\winprt2\winprt20\stdafx.h \
	d:\msvc\cdk16\include\afxctl.h \
	d:\msvc\cdk16\include\olectl.h \
	d:\msvc\cdk16\include\olectlid.h \
	d:\msvc\cdk16\include\oleconn.h \
	d:\msvc\cdk16\include\oletype.h \
	d:\msvc\cdk16\include\oledisp.h \
	d:\msvc\cdk16\include\oleprsh.h \
	d:\msvc\cdk16\include\olelic.h \
	d:\msvc\cdk16\include\olebind.h \
	d:\msvc\cdk16\include\afxctl.inl \
	z:\source\kindel\winprt2\winprt20\winprt20.h \
	z:\source\kindel\winprt2\winprt20\cprop.h \
	z:\source\kindel\winprt2\winprt20\engine.h


CONTROL_DEP = z:\source\kindel\winprt2\winprt20\stdafx.h \
	d:\msvc\cdk16\include\afxctl.h \
	d:\msvc\cdk16\include\olectl.h \
	d:\msvc\cdk16\include\olectlid.h \
	d:\msvc\cdk16\include\oleconn.h \
	d:\msvc\cdk16\include\oletype.h \
	d:\msvc\cdk16\include\oledisp.h \
	d:\msvc\cdk16\include\oleprsh.h \
	d:\msvc\cdk16\include\olelic.h \
	d:\msvc\cdk16\include\olebind.h \
	d:\msvc\cdk16\include\afxctl.inl \
	z:\source\kindel\winprt2\winprt20\winprt20.h \
	z:\source\kindel\winprt2\winprt20\cprop.h \
	z:\source\kindel\winprt2\winprt20\headerse.h \
	z:\source\kindel\winprt2\winprt20\control.h \
	z:\source\kindel\winprt2\winprt20\header.h \
	z:\source\kindel\winprt2\winprt20\ftd.h \
	z:\source\kindel\winprt2\winprt20\page.h \
	z:\source\kindel\winprt2\winprt20\input.h \
	z:\source\kindel\winprt2\winprt20\ctlppg.h


CTLPPG_DEP = z:\source\kindel\winprt2\winprt20\stdafx.h \
	d:\msvc\cdk16\include\afxctl.h \
	d:\msvc\cdk16\include\olectl.h \
	d:\msvc\cdk16\include\olectlid.h \
	d:\msvc\cdk16\include\oleconn.h \
	d:\msvc\cdk16\include\oletype.h \
	d:\msvc\cdk16\include\oledisp.h \
	d:\msvc\cdk16\include\oleprsh.h \
	d:\msvc\cdk16\include\olelic.h \
	d:\msvc\cdk16\include\olebind.h \
	d:\msvc\cdk16\include\afxctl.inl \
	z:\source\kindel\winprt2\winprt20\winprt20.h \
	z:\source\kindel\winprt2\winprt20\cprop.h \
	z:\source\kindel\winprt2\winprt20\ctlppg.h


HEADER_DEP = z:\source\kindel\winprt2\winprt20\stdafx.h \
	d:\msvc\cdk16\include\afxctl.h \
	d:\msvc\cdk16\include\olectl.h \
	d:\msvc\cdk16\include\olectlid.h \
	d:\msvc\cdk16\include\oleconn.h \
	d:\msvc\cdk16\include\oletype.h \
	d:\msvc\cdk16\include\oledisp.h \
	d:\msvc\cdk16\include\oleprsh.h \
	d:\msvc\cdk16\include\olelic.h \
	d:\msvc\cdk16\include\olebind.h \
	d:\msvc\cdk16\include\afxctl.inl \
	z:\source\kindel\winprt2\winprt20\winprt20.h \
	z:\source\kindel\winprt2\winprt20\cprop.h \
	z:\source\kindel\winprt2\winprt20\headerse.h \
	z:\source\kindel\winprt2\winprt20\header.h


HEADERSE_DEP = z:\source\kindel\winprt2\winprt20\stdafx.h \
	d:\msvc\cdk16\include\afxctl.h \
	d:\msvc\cdk16\include\olectl.h \
	d:\msvc\cdk16\include\olectlid.h \
	d:\msvc\cdk16\include\oleconn.h \
	d:\msvc\cdk16\include\oletype.h \
	d:\msvc\cdk16\include\oledisp.h \
	d:\msvc\cdk16\include\oleprsh.h \
	d:\msvc\cdk16\include\olelic.h \
	d:\msvc\cdk16\include\olebind.h \
	d:\msvc\cdk16\include\afxctl.inl \
	z:\source\kindel\winprt2\winprt20\winprt20.h \
	z:\source\kindel\winprt2\winprt20\cprop.h \
	z:\source\kindel\winprt2\winprt20\wintime.h \
	z:\source\kindel\winprt2\winprt20\control.h \
	z:\source\kindel\winprt2\winprt20\header.h \
	z:\source\kindel\winprt2\winprt20\headerse.h \
	z:\source\kindel\winprt2\winprt20\ftd.h \
	z:\source\kindel\winprt2\winprt20\page.h \
	z:\source\kindel\winprt2\winprt20\input.h


CPROP_DEP = z:\source\kindel\winprt2\winprt20\stdafx.h \
	d:\msvc\cdk16\include\afxctl.h \
	d:\msvc\cdk16\include\olectl.h \
	d:\msvc\cdk16\include\olectlid.h \
	d:\msvc\cdk16\include\oleconn.h \
	d:\msvc\cdk16\include\oletype.h \
	d:\msvc\cdk16\include\oledisp.h \
	d:\msvc\cdk16\include\oleprsh.h \
	d:\msvc\cdk16\include\olelic.h \
	d:\msvc\cdk16\include\olebind.h \
	d:\msvc\cdk16\include\afxctl.inl \
	z:\source\kindel\winprt2\winprt20\winprt20.h \
	z:\source\kindel\winprt2\winprt20\cprop.h \
	z:\source\kindel\winprt2\winprt20\propset.h \
	z:\source\kindel\winprt2\winprt20\ctlimpl.h


PAGE_DEP = z:\source\kindel\winprt2\winprt20\stdafx.h \
	d:\msvc\cdk16\include\afxctl.h \
	d:\msvc\cdk16\include\olectl.h \
	d:\msvc\cdk16\include\olectlid.h \
	d:\msvc\cdk16\include\oleconn.h \
	d:\msvc\cdk16\include\oletype.h \
	d:\msvc\cdk16\include\oledisp.h \
	d:\msvc\cdk16\include\oleprsh.h \
	d:\msvc\cdk16\include\olelic.h \
	d:\msvc\cdk16\include\olebind.h \
	d:\msvc\cdk16\include\afxctl.inl \
	z:\source\kindel\winprt2\winprt20\winprt20.h \
	z:\source\kindel\winprt2\winprt20\cprop.h \
	z:\source\kindel\winprt2\winprt20\control.h \
	z:\source\kindel\winprt2\winprt20\header.h \
	z:\source\kindel\winprt2\winprt20\headerse.h \
	z:\source\kindel\winprt2\winprt20\ftd.h \
	z:\source\kindel\winprt2\winprt20\page.h \
	z:\source\kindel\winprt2\winprt20\input.h


FTD_DEP = z:\source\kindel\winprt2\winprt20\stdafx.h \
	d:\msvc\cdk16\include\afxctl.h \
	d:\msvc\cdk16\include\olectl.h \
	d:\msvc\cdk16\include\olectlid.h \
	d:\msvc\cdk16\include\oleconn.h \
	d:\msvc\cdk16\include\oletype.h \
	d:\msvc\cdk16\include\oledisp.h \
	d:\msvc\cdk16\include\oleprsh.h \
	d:\msvc\cdk16\include\olelic.h \
	d:\msvc\cdk16\include\olebind.h \
	d:\msvc\cdk16\include\afxctl.inl \
	z:\source\kindel\winprt2\winprt20\winprt20.h \
	z:\source\kindel\winprt2\winprt20\cprop.h \
	z:\source\kindel\winprt2\winprt20\control.h \
	z:\source\kindel\winprt2\winprt20\header.h \
	z:\source\kindel\winprt2\winprt20\headerse.h \
	z:\source\kindel\winprt2\winprt20\ftd.h \
	z:\source\kindel\winprt2\winprt20\page.h \
	z:\source\kindel\winprt2\winprt20\input.h


INPUT_DEP = z:\source\kindel\winprt2\winprt20\stdafx.h \
	d:\msvc\cdk16\include\afxctl.h \
	d:\msvc\cdk16\include\olectl.h \
	d:\msvc\cdk16\include\olectlid.h \
	d:\msvc\cdk16\include\oleconn.h \
	d:\msvc\cdk16\include\oletype.h \
	d:\msvc\cdk16\include\oledisp.h \
	d:\msvc\cdk16\include\oleprsh.h \
	d:\msvc\cdk16\include\olelic.h \
	d:\msvc\cdk16\include\olebind.h \
	d:\msvc\cdk16\include\afxctl.inl \
	z:\source\kindel\winprt2\winprt20\winprt20.h \
	z:\source\kindel\winprt2\winprt20\cprop.h \
	z:\source\kindel\winprt2\winprt20\input.h


WINTIME_DEP = z:\source\kindel\winprt2\winprt20\stdafx.h \
	d:\msvc\cdk16\include\afxctl.h \
	d:\msvc\cdk16\include\olectl.h \
	d:\msvc\cdk16\include\olectlid.h \
	d:\msvc\cdk16\include\oleconn.h \
	d:\msvc\cdk16\include\oletype.h \
	d:\msvc\cdk16\include\oledisp.h \
	d:\msvc\cdk16\include\oleprsh.h \
	d:\msvc\cdk16\include\olelic.h \
	d:\msvc\cdk16\include\olebind.h \
	d:\msvc\cdk16\include\afxctl.inl \
	z:\source\kindel\winprt2\winprt20\winprt20.h \
	z:\source\kindel\winprt2\winprt20\cprop.h \
	z:\source\kindel\winprt2\winprt20\wintime.h


all:	$(PROJ).DLL $(PROJ).BSC

WINPRT20.RES:	WINPRT20.RC $(WINPRT20_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r WINPRT20.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

WINPRT20.OBJ:	WINPRT20.CPP $(WINPRT20_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WINPRT20.CPP

ENGINE.OBJ:	ENGINE.CPP $(ENGINE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ENGINE.CPP

CONTROL.OBJ:	CONTROL.CPP $(CONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CONTROL.CPP

CTLPPG.OBJ:	CTLPPG.CPP $(CTLPPG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CTLPPG.CPP

HEADER.OBJ:	HEADER.CPP $(HEADER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c HEADER.CPP

HEADERSE.OBJ:	HEADERSE.CPP $(HEADERSE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c HEADERSE.CPP

CPROP.OBJ:	CPROP.CPP $(CPROP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CPROP.CPP

PAGE.OBJ:	PAGE.CPP $(PAGE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PAGE.CPP

FTD.OBJ:	FTD.CPP $(FTD_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FTD.CPP

INPUT.OBJ:	INPUT.CPP $(INPUT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c INPUT.CPP

WINTIME.OBJ:	WINTIME.CPP $(WINTIME_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WINTIME.CPP


$(PROJ).DLL::	WINPRT20.RES

$(PROJ).DLL::	STDAFX.OBJ WINPRT20.OBJ ENGINE.OBJ CONTROL.OBJ CTLPPG.OBJ HEADER.OBJ \
	HEADERSE.OBJ CPROP.OBJ PAGE.OBJ FTD.OBJ INPUT.OBJ WINTIME.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
WINPRT20.OBJ +
ENGINE.OBJ +
CONTROL.OBJ +
CTLPPG.OBJ +
HEADER.OBJ +
HEADERSE.OBJ +
CPROP.OBJ +
PAGE.OBJ +
FTD.OBJ +
INPUT.OBJ +
WINTIME.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
$(MAPFILE)
D:\MSVC\CDK16\LIB\+
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
	$(RC) $(RESFLAGS) WINPRT20.RES $@
	@copy $(PROJ).CRF MSVC.BND
	implib /nowep $(PROJ).LIB $(PROJ).DLL

$(PROJ).DLL::	WINPRT20.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) WINPRT20.RES $@

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
