# Microsoft Visual C++ generated build script - Do not modify

PROJ = LEDVBX
DEBUG = 1
PROGTYPE = 2
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\SOURCE\!\LED\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = LEDVBX.C    
FIRSTCPP =             
RC = rc
CFLAGS_D_VEXE = /nologo /W3 /FR /G2 /Zi /D_DEBUG /Zp /Od /GD /ALw /Gc
CFLAGS_R_VEXE = /nologo /W3 /FR /G2 /DNDEBUG /Gs /Zp /Ox /GD /ALw /Gc
LFLAGS_D_VEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16
LFLAGS_R_VEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16
LIBS_D_VEXE = oldnames libw commdlg shell olecli olesvr ldllcew
LIBS_R_VEXE = oldnames libw commdlg shell olecli olesvr ldllcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = LEDVBX.DEF
OBJS_EXT = 
LIBS_EXT = ..\..\..\VB\CDK\VBAPI.LIB 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_VEXE)
LFLAGS = $(LFLAGS_D_VEXE)
LIBS = $(LIBS_D_VEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_VEXE)
LFLAGS = $(LFLAGS_R_VEXE)
LIBS = $(LIBS_R_VEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = LEDVBX.SBR


LEDVBX_DEP = c:\msvc\include\vbapi.h \
	c:\source\!\led\ledvbx.h


VBAPI_DEP = 

LEDVBX_RCDEP = c:\source\!\led\ledvbx.h \
	c:\source\!\led\ledvbcu.bmp \
	c:\source\!\led\ledvbcd.bmp \
	c:\source\!\led\ledvbmu.bmp \
	c:\source\!\led\ledvbeu.bmp


all:	$(PROJ).VBX $(PROJ).BSC

LEDVBX.OBJ:	LEDVBX.C $(LEDVBX_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c LEDVBX.C

LEDVBX.RES:	LEDVBX.RC $(LEDVBX_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r LEDVBX.RC


$(PROJ).VBX::	LEDVBX.RES

$(PROJ).VBX::	LEDVBX.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
LEDVBX.OBJ +
$(OBJS_EXT)
$(PROJ).VBX
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
..\..\..\VB\CDK\VBAPI.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) LEDVBX.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).VBX::	LEDVBX.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) LEDVBX.RES $@

run: $(PROJ).VBX
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
