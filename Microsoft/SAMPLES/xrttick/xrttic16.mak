# Microsoft Visual C++ generated build script - Do not modify

PROJ = XRTTICK
DEBUG = 0
PROGTYPE = 1
CALLER = e:\msvc\cdk16\bin\tstcon16.exe
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = Z:\OLE_IN~2\XRT\XRT_1.01\OLESAMP\XRTSAMP\XRTTICK\
USEMFC = 0
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
CFLAGS_D_WDLL = /nologo /G2 /Zp1 /W3 /WX /Zi /ALw /Od /D "_DEBUG" /D "_AFXDLL" /D "_WINDLL" /D "_AFXCTL" /FR /GD /Fd"XRTTICK.PDB" /GEf
CFLAGS_R_WDLL = /nologo /Gs /G2 /Zp1 /W3 /ALw /Ox /D "NDEBUG" /D "_AFXDLL" /D "_WINDLL" /D "_AFXCTL" /FR /GD /GEf
LFLAGS_D_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /NOE /PACKC:61440 /CO /ALIGN:16
LFLAGS_R_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /NOE /PACKC:61440 /ALIGN:16
LIBS_D_WDLL = ocs25d oc25d libw ldllcew compobj storage ole2 ole2disp typelib oldnames commdlg.lib shell.lib 
LIBS_R_WDLL = ocs25 oc25 libw ldllcew compobj storage ole2 ole2disp typelib oldnames commdlg.lib shell.lib 
RCFLAGS = /nologo /i tlb16
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = XRTTICK.DEF
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
		XRTTICK.SBR \
		TICKCTL.SBR \
		XRTTIPPG.SBR \
		OFFSTAGE.SBR


XRTTICK_RCDEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\tickectl.bmp \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\ticker.ico \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\xrttick.rc2 \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\tlb16\xrttick.tlb


STDAFX_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\stdafx.h \
	c:\msvc\cdk16\include\afxctl.h \
	c:\msvc\cdk16\include\olectl.h \
	c:\msvc\cdk16\include\olectlid.h \
	c:\msvc\cdk16\include\afxctl.inl


XRTTICK_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\stdafx.h \
	c:\msvc\cdk16\include\afxctl.h \
	c:\msvc\cdk16\include\olectl.h \
	c:\msvc\cdk16\include\olectlid.h \
	c:\msvc\cdk16\include\afxctl.inl \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\xrttick.h


TICKCTL_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\stdafx.h \
	c:\msvc\cdk16\include\afxctl.h \
	c:\msvc\cdk16\include\olectl.h \
	c:\msvc\cdk16\include\olectlid.h \
	c:\msvc\cdk16\include\afxctl.inl \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\xrttick.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\tickctl.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\wosaxrt.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\xrttippg.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\offstage.h


XRTTIPPG_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\stdafx.h \
	c:\msvc\cdk16\include\afxctl.h \
	c:\msvc\cdk16\include\olectl.h \
	c:\msvc\cdk16\include\olectlid.h \
	c:\msvc\cdk16\include\afxctl.inl \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\xrttick.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\xrttippg.h


OFFSTAGE_DEP = z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\stdafx.h \
	c:\msvc\cdk16\include\afxctl.h \
	c:\msvc\cdk16\include\olectl.h \
	c:\msvc\cdk16\include\olectlid.h \
	c:\msvc\cdk16\include\afxctl.inl \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\xrttick.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\tickctl.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\wosaxrt.h \
	z:\ole_in~2\xrt\xrt_1.01\olesamp\xrtsamp\xrttick\offstage.h


all:	$(PROJ).DLL $(PROJ).BSC

XRTTICK.RES:	XRTTICK.RC $(XRTTICK_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r XRTTICK.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

XRTTICK.OBJ:	XRTTICK.CPP $(XRTTICK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c XRTTICK.CPP

TICKCTL.OBJ:	TICKCTL.CPP $(TICKCTL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TICKCTL.CPP

XRTTIPPG.OBJ:	XRTTIPPG.CPP $(XRTTIPPG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c XRTTIPPG.CPP

OFFSTAGE.OBJ:	OFFSTAGE.CPP $(OFFSTAGE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c OFFSTAGE.CPP


$(PROJ).DLL::	XRTTICK.RES

$(PROJ).DLL::	STDAFX.OBJ XRTTICK.OBJ TICKCTL.OBJ XRTTIPPG.OBJ OFFSTAGE.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
XRTTICK.OBJ +
TICKCTL.OBJ +
XRTTIPPG.OBJ +
OFFSTAGE.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
$(MAPFILE)
C:\MSVC\CDK16\LIB\+
c:\msvc\lib\+
c:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) XRTTICK.RES $@
	@copy $(PROJ).CRF MSVC.BND
	implib /nowep $(PROJ).LIB $(PROJ).DLL

$(PROJ).DLL::	XRTTICK.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) XRTTICK.RES $@

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
