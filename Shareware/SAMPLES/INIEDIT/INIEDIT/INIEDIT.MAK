# Microsoft Visual C++ generated build script - Do not modify

PROJ = INIEDIT
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG
R_RCDEFINES = /dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\SOURCE\CPP\INIEDIT2\INIEDIT\
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
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /Od /D "_DEBUG" /FR /GA /Fd"INIEDIT.PDB"
CFLAGS_R_WEXE = /nologo /Gs /G2 /W3 /AM /O1 /D "NDEBUG" /FR /GA
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE
LIBS_D_WEXE = mafxcwd oldnames libw mlibcew commdlg olesvr olecli shell 
LIBS_R_WEXE = mafxcw oldnames libw mlibcew commdlg olesvr olecli shell 
RCFLAGS = /nologo /z
RESFLAGS = /nologo /t
RUNFLAGS = 
DEFFILE = INIEDIT.DEF
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
SBRS = STDAFX.SBR \
		INIEDIT.SBR \
		INIEDVW.SBR \
		INIFILE.SBR \
		MAINFRM.SBR \
		INIEDDOC.SBR \
		FLISTVW.SBR \
		FLISTDOC.SBR


STDAFX_DEP = c:\source\cpp\iniedit2\iniedit\stdafx.h


INIEDIT_DEP = c:\source\cpp\iniedit2\iniedit\stdafx.h \
	c:\source\cpp\iniedit2\iniedit\iniedit.h \
	c:\source\cpp\iniedit2\iniedit\inifile.h \
	c:\source\cpp\iniedit2\iniedit\mainfrm.h \
	c:\source\cpp\iniedit2\iniedit\inieddoc.h \
	c:\source\cpp\iniedit2\iniedit\iniedvw.h \
	c:\source\cpp\iniedit2\iniedit\flistdoc.h \
	c:\source\cpp\iniedit2\iniedit\flistvw.h


INIEDVW_DEP = c:\source\cpp\iniedit2\iniedit\stdafx.h \
	c:\source\cpp\iniedit2\iniedit\iniedit.h \
	c:\source\cpp\iniedit2\iniedit\inifile.h \
	c:\source\cpp\iniedit2\iniedit\inieddoc.h \
	c:\source\cpp\iniedit2\iniedit\iniedvw.h


INIFILE_DEP = c:\source\cpp\iniedit2\iniedit\stdafx.h \
	c:\source\cpp\iniedit2\iniedit\inifile.h


MAINFRM_DEP = c:\source\cpp\iniedit2\iniedit\stdafx.h \
	c:\source\cpp\iniedit2\iniedit\iniedit.h \
	c:\source\cpp\iniedit2\iniedit\inifile.h \
	c:\source\cpp\iniedit2\iniedit\mainfrm.h


INIEDDOC_DEP = c:\source\cpp\iniedit2\iniedit\stdafx.h \
	c:\source\cpp\iniedit2\iniedit\iniedit.h \
	c:\source\cpp\iniedit2\iniedit\inifile.h \
	c:\source\cpp\iniedit2\iniedit\inieddoc.h


INIEDIT_RCDEP = c:\source\cpp\iniedit2\iniedit\res\iniedit.ico \
	c:\source\cpp\iniedit2\iniedit\res\inieddoc.ico \
	c:\source\cpp\iniedit2\iniedit\res\idr_file.ico \
	c:\source\cpp\iniedit2\iniedit\res\iniedit.rc2


FLISTVW_DEP = c:\source\cpp\iniedit2\iniedit\stdafx.h \
	c:\source\cpp\iniedit2\iniedit\iniedit.h \
	c:\source\cpp\iniedit2\iniedit\inifile.h \
	c:\source\cpp\iniedit2\iniedit\flistdoc.h \
	c:\source\cpp\iniedit2\iniedit\flistvw.h


FLISTDOC_DEP = c:\source\cpp\iniedit2\iniedit\stdafx.h \
	c:\source\cpp\iniedit2\iniedit\iniedit.h \
	c:\source\cpp\iniedit2\iniedit\inifile.h \
	c:\source\cpp\iniedit2\iniedit\flistdoc.h


all:	$(PROJ).EXE $(PROJ).BSC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

INIEDIT.OBJ:	INIEDIT.CPP $(INIEDIT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c INIEDIT.CPP

INIEDVW.OBJ:	INIEDVW.CPP $(INIEDVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c INIEDVW.CPP

INIFILE.OBJ:	INIFILE.CPP $(INIFILE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c INIFILE.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

INIEDDOC.OBJ:	INIEDDOC.CPP $(INIEDDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c INIEDDOC.CPP

INIEDIT.RES:	INIEDIT.RC $(INIEDIT_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r INIEDIT.RC

FLISTVW.OBJ:	FLISTVW.CPP $(FLISTVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FLISTVW.CPP

FLISTDOC.OBJ:	FLISTDOC.CPP $(FLISTDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FLISTDOC.CPP


$(PROJ).EXE::	INIEDIT.RES

$(PROJ).EXE::	STDAFX.OBJ INIEDIT.OBJ INIEDVW.OBJ INIFILE.OBJ MAINFRM.OBJ INIEDDOC.OBJ \
	FLISTVW.OBJ FLISTDOC.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
INIEDIT.OBJ +
INIEDVW.OBJ +
INIFILE.OBJ +
MAINFRM.OBJ +
INIEDDOC.OBJ +
FLISTVW.OBJ +
FLISTDOC.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) INIEDIT.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	INIEDIT.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) INIEDIT.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
