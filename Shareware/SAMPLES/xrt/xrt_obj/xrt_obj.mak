# Microsoft Visual C++ generated build script - Do not modify

PROJ = XRT_OBJ
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG
R_RCDEFINES = /dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\SOURCE\XRT\XRT_OBJ\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = /YcSTDAFX.H
CUSEPCHFLAG = 
CPPUSEPCHFLAG = /YuSTDAFX.H
FIRSTC = CLSID.C     
FIRSTCPP = STDAFX.CPP  
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /WX /Zi /AM /Od /D "_DEBUG" /FR /Zn /GA /Fd"XRT_OBJ.PDB" 
CFLAGS_R_WEXE = /nologo /Gs /G2 /W3 /AM /O1 /D "NDEBUG" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = mafxcwd oldnames libw mlibcew ctl3d ole2 ole2disp storage.lib commdlg.lib olecli.lib olesvr.lib shell.lib ver.lib 
LIBS_R_WEXE = mafxcw oldnames libw mlibcew ctl3d ole2 ole2disp storage.lib commdlg.lib olecli.lib olesvr.lib shell.lib ver.lib 
RCFLAGS = /nologo /z
RESFLAGS = /nologo /t
RUNFLAGS = 
DEFFILE = XRT_OBJ.DEF
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
		XRT_OBJ.SBR \
		MAINFRM.SBR \
		DOC.SBR \
		OBJECT.SBR \
		UTIL.SBR \
		CONFIG.SBR \
		MDI.SBR \
		DATAOBJ.SBR \
		CLSID.SBR \
		IENUMFE.SBR \
		IPERSTOR.SBR \
		COLLIST.SBR \
		IOLEOBJ.SBR \
		IPERFILE.SBR


XRT_OBJ_RCDEP = e:\source\xrt\xrt_obj\wosaxrt.ico \
	e:\source\xrt\xrt_obj\res\doc.ico \
	e:\source\xrt\xrt_obj\res\sizehorz.cur \
	e:\source\xrt\xrt_obj\res\xrt_obj.rc2


STDAFX_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h


XRT_OBJ_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h \
	e:\source\xrt\xrt_obj\xrt_obj.h \
	d:\ole2\include\dispatch.h \
	e:\source\xrt\xrt_obj\clsid.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\xrt_obj\util.h \
	e:\source\xrt\xrt_obj\mainfrm.h \
	e:\source\xrt\xrt_obj\doc.h \
	e:\source\xrt\xrt_obj\collist.h \
	e:\source\xrt\xrt_obj\object.h \
	e:\source\xrt\xrt_obj\dataobj.h \
	e:\source\xrt\xrt_obj\config.h \
	e:\source\xrt\xrt_obj\mdi.h


MAINFRM_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h \
	e:\source\xrt\xrt_obj\xrt_obj.h \
	d:\ole2\include\dispatch.h \
	e:\source\xrt\xrt_obj\clsid.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\xrt_obj\util.h \
	e:\source\xrt\xrt_obj\mainfrm.h \
	e:\source\xrt\xrt_obj\doc.h \
	e:\source\xrt\xrt_obj\collist.h \
	e:\source\xrt\xrt_obj\object.h \
	e:\source\xrt\xrt_obj\dataobj.h


DOC_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h \
	e:\source\xrt\xrt_obj\xrt_obj.h \
	d:\ole2\include\dispatch.h \
	e:\source\xrt\xrt_obj\clsid.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\xrt_obj\util.h \
	e:\source\xrt\xrt_obj\mainfrm.h \
	e:\source\xrt\xrt_obj\doc.h \
	e:\source\xrt\xrt_obj\collist.h \
	e:\source\xrt\xrt_obj\object.h \
	e:\source\xrt\xrt_obj\dataobj.h


OBJECT_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h \
	e:\source\xrt\xrt_obj\xrt_obj.h \
	d:\ole2\include\dispatch.h \
	e:\source\xrt\xrt_obj\clsid.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\xrt_obj\util.h \
	e:\source\xrt\xrt_obj\mainfrm.h \
	e:\source\xrt\xrt_obj\doc.h \
	e:\source\xrt\xrt_obj\collist.h \
	e:\source\xrt\xrt_obj\object.h \
	e:\source\xrt\xrt_obj\dataobj.h


UTIL_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h \
	e:\source\xrt\xrt_obj\xrt_obj.h \
	d:\ole2\include\dispatch.h \
	e:\source\xrt\xrt_obj\clsid.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\xrt_obj\util.h \
	e:\source\xrt\xrt_obj\mainfrm.h \
	e:\source\xrt\xrt_obj\doc.h \
	e:\source\xrt\xrt_obj\collist.h \
	e:\source\xrt\xrt_obj\object.h \
	e:\source\xrt\xrt_obj\dataobj.h \
	e:\source\xrt\xrt_obj\shadow.h


CONFIG_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h \
	e:\source\xrt\xrt_obj\xrt_obj.h \
	d:\ole2\include\dispatch.h \
	e:\source\xrt\xrt_obj\clsid.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\xrt_obj\util.h \
	e:\source\xrt\xrt_obj\mainfrm.h \
	e:\source\xrt\xrt_obj\doc.h \
	e:\source\xrt\xrt_obj\collist.h \
	e:\source\xrt\xrt_obj\object.h \
	e:\source\xrt\xrt_obj\dataobj.h \
	e:\source\xrt\xrt_obj\config.h


MDI_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h \
	e:\source\xrt\xrt_obj\xrt_obj.h \
	d:\ole2\include\dispatch.h \
	e:\source\xrt\xrt_obj\clsid.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\xrt_obj\util.h \
	e:\source\xrt\xrt_obj\mainfrm.h \
	e:\source\xrt\xrt_obj\doc.h \
	e:\source\xrt\xrt_obj\collist.h \
	e:\source\xrt\xrt_obj\object.h \
	e:\source\xrt\xrt_obj\dataobj.h \
	e:\source\xrt\xrt_obj\mdi.h


DATAOBJ_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h \
	e:\source\xrt\xrt_obj\xrt_obj.h \
	d:\ole2\include\dispatch.h \
	e:\source\xrt\xrt_obj\clsid.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\xrt_obj\util.h \
	e:\source\xrt\xrt_obj\mainfrm.h \
	e:\source\xrt\xrt_obj\doc.h \
	e:\source\xrt\xrt_obj\collist.h \
	e:\source\xrt\xrt_obj\object.h \
	e:\source\xrt\xrt_obj\dataobj.h \
	e:\source\xrt\xrt_obj\ienumfe.h \
	e:\source\xrt\xrt_obj\iperstor.h \
	e:\source\xrt\xrt_obj\iperfile.h \
	e:\source\xrt\xrt_obj\ioleobj.h


CLSID_DEP = d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	e:\source\xrt\xrt_obj\clsid.h


IENUMFE_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h \
	e:\source\xrt\xrt_obj\xrt_obj.h \
	d:\ole2\include\dispatch.h \
	e:\source\xrt\xrt_obj\clsid.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\xrt_obj\util.h \
	e:\source\xrt\xrt_obj\mainfrm.h \
	e:\source\xrt\xrt_obj\doc.h \
	e:\source\xrt\xrt_obj\collist.h \
	e:\source\xrt\xrt_obj\object.h \
	e:\source\xrt\xrt_obj\dataobj.h \
	e:\source\xrt\xrt_obj\ienumfe.h


IPERSTOR_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h \
	e:\source\xrt\xrt_obj\xrt_obj.h \
	d:\ole2\include\dispatch.h \
	e:\source\xrt\xrt_obj\clsid.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\xrt_obj\util.h \
	e:\source\xrt\xrt_obj\mainfrm.h \
	e:\source\xrt\xrt_obj\doc.h \
	e:\source\xrt\xrt_obj\collist.h \
	e:\source\xrt\xrt_obj\object.h \
	e:\source\xrt\xrt_obj\dataobj.h \
	e:\source\xrt\xrt_obj\iperstor.h


COLLIST_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h \
	e:\source\xrt\xrt_obj\collist.h


IOLEOBJ_DEP = e:\source\xrt\xrt_obj\stdafx.h \
	d:\ole2\include\ole2.h \
	d:\ole2\include\compobj.h \
	d:\ole2\include\scode.h \
	d:\ole2\include\initguid.h \
	d:\ole2\include\coguid.h \
	d:\ole2\include\oleguid.h \
	d:\ole2\include\dvobj.h \
	d:\ole2\include\storage.h \
	d:\ole2\include\moniker.h \
	d:\ole2\include\olenls.h \
	d:\ole2\include\variant.h \
	c:\inc\ctl3d.h \
	e:\source\xrt\xrt_obj\xrt_obj.h \
	d:\ole2\include\dispatch.h \
	e:\source\xrt\xrt_obj\clsid.h \
	e:\source\xrt\\wosaxrt.h \
	e:\source\xrt\xrt_obj\util.h \
	e:\source\xrt\xrt_obj\mainfrm.h \
	e:\source\xrt\xrt_obj\doc.h \
	e:\source\xrt\xrt_obj\collist.h \
	e:\source\xrt\xrt_obj\object.h \
	e:\source\xrt\xrt_obj\dataobj.h \
	e:\source\xrt\xrt_obj\ioleobj.h


IPERFILE_DEP = e:\source\xrt\xrt_obj\clsid.h \
	e:\source\xrt\xrt_obj\iperfile.h


all:	$(PROJ).EXE $(PROJ).BSC

XRT_OBJ.RES:	XRT_OBJ.RC $(XRT_OBJ_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r XRT_OBJ.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

XRT_OBJ.OBJ:	XRT_OBJ.CPP $(XRT_OBJ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c XRT_OBJ.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

DOC.OBJ:	DOC.CPP $(DOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DOC.CPP

OBJECT.OBJ:	OBJECT.CPP $(OBJECT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c OBJECT.CPP

UTIL.OBJ:	UTIL.CPP $(UTIL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c UTIL.CPP

CONFIG.OBJ:	CONFIG.CPP $(CONFIG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CONFIG.CPP

MDI.OBJ:	MDI.CPP $(MDI_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MDI.CPP

DATAOBJ.OBJ:	DATAOBJ.CPP $(DATAOBJ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DATAOBJ.CPP

CLSID.OBJ:	CLSID.C $(CLSID_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c CLSID.C

IENUMFE.OBJ:	IENUMFE.CPP $(IENUMFE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IENUMFE.CPP

IPERSTOR.OBJ:	IPERSTOR.CPP $(IPERSTOR_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IPERSTOR.CPP

COLLIST.OBJ:	COLLIST.CPP $(COLLIST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COLLIST.CPP

IOLEOBJ.OBJ:	IOLEOBJ.CPP $(IOLEOBJ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IOLEOBJ.CPP

IPERFILE.OBJ:	IPERFILE.CPP $(IPERFILE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IPERFILE.CPP


$(PROJ).EXE::	XRT_OBJ.RES

$(PROJ).EXE::	STDAFX.OBJ XRT_OBJ.OBJ MAINFRM.OBJ DOC.OBJ OBJECT.OBJ UTIL.OBJ CONFIG.OBJ \
	MDI.OBJ DATAOBJ.OBJ CLSID.OBJ IENUMFE.OBJ IPERSTOR.OBJ COLLIST.OBJ IOLEOBJ.OBJ IPERFILE.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
XRT_OBJ.OBJ +
MAINFRM.OBJ +
DOC.OBJ +
OBJECT.OBJ +
UTIL.OBJ +
CONFIG.OBJ +
MDI.OBJ +
DATAOBJ.OBJ +
CLSID.OBJ +
IENUMFE.OBJ +
IPERSTOR.OBJ +
COLLIST.OBJ +
IOLEOBJ.OBJ +
IPERFILE.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
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
	$(RC) $(RESFLAGS) XRT_OBJ.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	XRT_OBJ.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) XRT_OBJ.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
