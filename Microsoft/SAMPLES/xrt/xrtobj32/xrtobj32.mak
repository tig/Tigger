# Microsoft Visual C++ generated build script - Do not modify

PROJ = XRTOBJ32
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
ORIGIN = MSVCNT
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\XRT\XRTOBJ32\ 
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = CLSID.C
FIRSTCPP = XRTOBJ32.CPP
RC = rc
CFLAGS_D_WEXE32 = /nologo /Gz /W3 /Zi /YX"precomp.h" /D "_DEBUG" /D "_X86_" /D "_WINDOWS" /D "WIN32" /D "_INC_OLE" /FR /ML /Fd"XRTOBJ32.PDB"       /Fp"XRTOBJ32.PCH"
CFLAGS_R_WEXE32 = /nologo /Gz /W3 /YX"precomp.h" /O2 /D "NDEBUG" /D "_X86_" /D "_WINDOWS" /D "WIN32" /D "_INC_OLE" /FR /ML /Fp"XRTOBJ32.PCH"
LFLAGS_D_WEXE32 = /NOLOGO /VERSION:0,1 /MAP:"xrtobj32.map" /DEBUG /DEBUGTYPE:both /DEF:"xrtobj32.def" /SUBSYSTEM:windows user32.lib gdi32.lib winspool.lib advapi32.lib shell32.lib compob32.lib ole232.lib commnot.lib uuid.lib storag32.lib
LFLAGS_R_WEXE32 = /NOLOGO /VERSION:0,1 /DEF:"xrtobj32.def" /SUBSYSTEM:windows user32.lib gdi32.lib winspool.lib advapi32.lib shell32.lib compob32.lib ole232.lib commnot.lib uuid.lib storag32.lib
LFLAGS_D_LIB32 = /NOLOGO
LFLAGS_R_LIB32 = /NOLOGO
LIBS_D_WEXE32 = 
LIBS_R_WEXE32 = 
RCFLAGS32 = 
D_RCDEFINES32 = -d_DEBUG
R_RCDEFINES32 = -dNDEBUG
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE32)
LFLAGS = $(LFLAGS_D_WEXE32)
LIBS = $(LIBS_D_WEXE32)
LFLAGS_LIB=$(LFLAGS_D_LIB32)
MAPFILE_OPTION = -map:$(PROJ).map
RCDEFINES = $(D_RCDEFINES32)
!else
CFLAGS = $(CFLAGS_R_WEXE32)
LFLAGS = $(LFLAGS_R_WEXE32)
LIBS = $(LIBS_R_WEXE32)
MAPFILE_OPTION = 
LFLAGS_LIB=$(LFLAGS_R_LIB32)
RCDEFINES = $(R_RCDEFINES32)
!endif
SBRS = XRTOBJ32.SBR \
		IENUMFE.SBR \
		OBJECT.SBR \
		CLSID.SBR \
		SIMOBJ.SBR \
		IPERFILE.SBR


XRTOBJ32_DEP =  \
	e:\source\xrt\xrtobj32\precomp.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	..\\wosaxrt.h \
	e:\source\xrt\xrtobj32\clsid.h \
	e:\source\xrt\xrtobj32\xrtobj32.h \
	e:\source\xrt\xrtobj32\object.h \
	e:\source\xrt\xrtobj32\simobj.h \
	e:\source\xrt\xrtobj32\debug.h


XRTOBJ32_RCDEP =  \
	e:\source\xrt\xrtobj32\xrtobj.ico


IENUMFE_DEP =  \
	e:\source\xrt\xrtobj32\precomp.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	..\\wosaxrt.h \
	e:\source\xrt\xrtobj32\xrtobj32.h \
	e:\source\xrt\xrtobj32\ienumfe.h


OBJECT_DEP =  \
	e:\source\xrt\xrtobj32\precomp.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	..\\wosaxrt.h \
	e:\source\xrt\xrtobj32\xrtobj32.h \
	e:\source\xrt\xrtobj32\object.h \
	e:\source\xrt\xrtobj32\simobj.h \
	e:\source\xrt\xrtobj32\ienumfe.h \
	e:\source\xrt\xrtobj32\iperfile.h


CLSID_DEP =  \
	e:\source\xrt\xrtobj32\precomp.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	..\\wosaxrt.h \
	e:\source\xrt\xrtobj32\clsid.h


SIMOBJ_DEP =  \
	e:\source\xrt\xrtobj32\precomp.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	..\\wosaxrt.h \
	e:\source\xrt\xrtobj32\xrtobj32.h \
	e:\source\xrt\xrtobj32\simobj.h \
	e:\source\xrt\xrtobj32\object.h


IPERFILE_DEP =  \
	e:\source\xrt\xrtobj32\precomp.h \
	e:\cairo\h\compobj.h \
	e:\cairo\h\scode.h \
	e:\cairo\h\initguid.h \
	e:\cairo\h\coguid.h \
	e:\cairo\h\ole2.h \
	e:\cairo\h\oleguid.h \
	e:\cairo\h\dvobj.h \
	e:\cairo\h\storage.h \
	e:\cairo\h\moniker.h \
	..\\wosaxrt.h \
	e:\source\xrt\xrtobj32\xrtobj32.h \
	e:\source\xrt\xrtobj32\clsid.h \
	e:\source\xrt\xrtobj32\iperfile.h


all:	$(PROJ).EXE $(PROJ).BSC

XRTOBJ32.OBJ:	XRTOBJ32.CPP $(XRTOBJ32_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c XRTOBJ32.CPP

XRTOBJ32.RES:	XRTOBJ32.RC $(XRTOBJ32_RCDEP)
	$(RC) $(RCFLAGS32) $(RCDEFINES) -r XRTOBJ32.RC

IENUMFE.OBJ:	IENUMFE.CPP $(IENUMFE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IENUMFE.CPP

OBJECT.OBJ:	OBJECT.CPP $(OBJECT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c OBJECT.CPP

CLSID.OBJ:	CLSID.C $(CLSID_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c CLSID.C

SIMOBJ.OBJ:	SIMOBJ.CPP $(SIMOBJ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SIMOBJ.CPP

IPERFILE.OBJ:	IPERFILE.CPP $(IPERFILE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IPERFILE.CPP


$(PROJ).EXE:	XRTOBJ32.RES

$(PROJ).EXE:	XRTOBJ32.OBJ IENUMFE.OBJ OBJECT.OBJ CLSID.OBJ SIMOBJ.OBJ IPERFILE.OBJ $(OBJS_EXT) $(LIBS_EXT)
	echo >NUL @<<$(PROJ).CRF
XRTOBJ32.OBJ 
IENUMFE.OBJ 
OBJECT.OBJ 
CLSID.OBJ 
SIMOBJ.OBJ 
IPERFILE.OBJ 
$(OBJS_EXT)
-OUT:$(PROJ).EXE
$(MAPFILE_OPTION)
XRTOBJ32.RES
$(LIBS)
$(LIBS_EXT)
$(DEFFILE_OPTION) -implib:$(PROJ).lib
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
