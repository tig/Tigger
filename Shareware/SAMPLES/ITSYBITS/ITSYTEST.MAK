# Microsoft Visual C++ generated build script - Do not modify

PROJ = ITSYTEST
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\SOURCE\!\ITSYBITS\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = ITSYTEST.C  
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G3 /W3 /Zi /YX /Od /D "_DEBUG" /FR /GA /Fd"ITSYTEST.PDB" /Fp"ITSYTEST.PCH"
CFLAGS_R_WEXE = /nologo /G3 /W3 /YX /O1 /D "NDEBUG" /FR /GA /Fp"ITSYTEST.PCH" 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO 
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE 
LIBS_D_WEXE = oldnames libw slibcew 
LIBS_R_WEXE = oldnames libw slibcew 
RCFLAGS = /nologo 
RESFLAGS = /nologo 
RUNFLAGS = 
DEFFILE = ITSYTEST.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = ITSYTEST.SBR \
		ITSYBITS.SBR


ITSYTEST_DEP = c:\source\!\itsybits\itsybits.h


ITSYBITS_DEP = c:\source\!\itsybits\itsybits.h


ITSYTEST_RCDEP = c:\source\!\itsybits\itsytest.rcv \
	c:\source\!\itsybits\itsytest.ico


all:	$(PROJ).EXE $(PROJ).BSC

ITSYTEST.OBJ:	ITSYTEST.C $(ITSYTEST_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c ITSYTEST.C

ITSYBITS.OBJ:	ITSYBITS.C $(ITSYBITS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ITSYBITS.C

ITSYTEST.RES:	ITSYTEST.RC $(ITSYTEST_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r ITSYTEST.RC


$(PROJ).EXE::	ITSYTEST.RES

$(PROJ).EXE::	ITSYTEST.OBJ ITSYBITS.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
ITSYTEST.OBJ +
ITSYBITS.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) ITSYTEST.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	ITSYTEST.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) ITSYTEST.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
