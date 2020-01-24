; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CIUnknownDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "iviewers.h"
LastPage=0

ClassCount=16
Class1=CAboutDlg
Class2=CDataObjectViewer
Class3=CDispatchViewer
Class4=CDispatchDlg
Class5=CIPersistViewer
Class6=CIPersistDlg
Class7=CIPersistStreamDlg
Class8=CIUnknownViewer
Class9=CIUnknownDlg
Class10=CInterfaceViewer
Class11=CIViewersApp
Class12=CTypeLibODLView
Class13=CTypeLibTreeView
Class14=CTypeLibViewer
Class15=CTypeLibWnd
Class16=CUniformDataTransfer

ResourceCount=10
Resource1=IDD_IPERSIST
Resource2=IDR_MAINFRAME
Resource3=IDD_DISPATCH
Resource4=IDD_IDATAOBJDLG
Resource5=IDD_TYPELIB
Resource6=IDD_IUNKNOWN
Resource7=IDD_ABOUTBOX
Resource8=IDD_DATAOBJECT
Resource9=IDD_IPERSISTSTREAM
Resource10=IDD_IPERSISTSTREAMINIT

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=AboutDlg.h
ImplementationFile=AboutDlg.cpp

[CLS:CDataObjectViewer]
Type=0
BaseClass=CInterfaceViewer
HeaderFile=dataobj.h
ImplementationFile=dataobj.cpp

[CLS:CDispatchViewer]
Type=0
BaseClass=CInterfaceViewer
HeaderFile=dispatch.h
ImplementationFile=dispatch.cpp

[CLS:CDispatchDlg]
Type=0
BaseClass=CDialog
HeaderFile=dispatch.h
ImplementationFile=dispatch.cpp

[CLS:CIPersistViewer]
Type=0
BaseClass=CInterfaceViewer
HeaderFile=IPersist.h
ImplementationFile=IPersist.cpp

[CLS:CIPersistDlg]
Type=0
BaseClass=CDialog
HeaderFile=IPersist.h
ImplementationFile=IPersist.cpp

[CLS:CIPersistStreamDlg]
Type=0
BaseClass=CDialog
HeaderFile=IPersist.h
ImplementationFile=IPersist.cpp
LastObject=IDC_INITNEW
Filter=D
VirtualFilter=dWC

[CLS:CIUnknownViewer]
Type=0
BaseClass=CInterfaceViewer
HeaderFile=IUnknown.h
ImplementationFile=IUnknown.cpp

[CLS:CIUnknownDlg]
Type=0
BaseClass=CDialog
HeaderFile=IUnknown.h
ImplementationFile=IUnknown.cpp
Filter=D
VirtualFilter=dWC
LastObject=CIUnknownDlg

[CLS:CInterfaceViewer]
Type=0
BaseClass=CCmdTarget
HeaderFile=iviewer.h
ImplementationFile=iviewer.cpp

[CLS:CIViewersApp]
Type=0
BaseClass=CWinApp
HeaderFile=IViewers.h
ImplementationFile=IViewers.cpp

[CLS:CTypeLibODLView]
Type=0
BaseClass=CRichEditView
HeaderFile=tlbodl.h
ImplementationFile=tlbodl.cpp

[CLS:CTypeLibTreeView]
Type=0
BaseClass=CView
HeaderFile=tlbtree.h
ImplementationFile=tlbtree.cpp

[CLS:CTypeLibViewer]
Type=0
BaseClass=CInterfaceViewer
HeaderFile=typelib.h
ImplementationFile=typelib.cpp

[CLS:CTypeLibWnd]
Type=0
BaseClass=CFrameWnd
HeaderFile=typelib.h
ImplementationFile=typelib.cpp

[CLS:CUniformDataTransfer]
Type=0
BaseClass=CDialog
HeaderFile=UniformDataTransfer.h
ImplementationFile=UniformDataTransfer.cpp

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=8
Control1=IDC_STATIC,static,1342177283
Control2=IDC_APPNAME,static,1342308352
Control3=IDC_COPYRIGHT,static,1342308352
Control4=IDCANCEL,button,1342373889
Control5=IDC_ALLRIGHTSRESERVED,static,1342308352
Control6=IDC_WRITTENBY,static,1342308352
Control7=IDC_GROUP,static,1342308352
Control8=IDC_VERSION,static,1342308352

[DLG:IDD_DISPATCH]
Type=1
Class=CDispatchDlg
ControlCount=4
Control1=IDCANCEL,button,1342242816
Control2=IDC_STATIC,static,1342308352
Control3=IDC_VIEWTYPEINFO,button,1342242816
Control4=IDC_TYPEINFOCOUNT,static,1342308352

[DLG:IDD_IPERSIST]
Type=1
Class=CIPersistDlg
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_CLASSNAME,static,1342308352
Control5=IDC_CLSID,static,1342308352

[DLG:IDD_IPERSISTSTREAM]
Type=1
Class=CIPersistStreamDlg
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_CLASSNAME,static,1342308352
Control5=IDC_CLSID,static,1342308352
Control6=IDC_ISDIRTY,button,1342242816
Control7=IDC_ISDIRTYTXT,static,1342308352
Control8=IDC_GETSIZEMAX,button,1342242816
Control9=IDC_SIZEMAX,static,1342308352

[DLG:IDD_IUNKNOWN]
Type=1
Class=CIUnknownDlg
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_INTERFACENAME,static,1342308352
Control5=IDC_IID,static,1342308352
Control6=IDC_VIEWTYPEINFO,button,1476460544

[DLG:IDD_DATAOBJECT]
Type=1
Class=CUniformDataTransfer
ControlCount=4
Control1=IDC_DATAFORMATS,SysListView32,1350631445
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_DUMMY,static,1073876992

[DLG:IDD_IDATAOBJDLG]
Type=1
Class=?
ControlCount=12
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_UPDATEDISPLAY,button,1342242819
Control4=IDC_ADVISEDATA,edit,1352730820
Control5=IDC_STATIC,static,1342308352
Control6=IDC_GETDATA,listbox,1352728833
Control7=IDC_DOGETDATA,button,1342242816
Control8=IDC_SETUPADVISE,button,1342242816
Control9=IDC_PRIMEFIRST,button,1342242819
Control10=IDCANCEL,button,1342242817
Control11=IDC_CLEAROUTPUT,button,1342242816
Control12=IDC_FORMATETC,listbox,1352733009

[DLG:IDD_TYPELIB]
Type=1
Class=?
ControlCount=0

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_CLOSE
Command2=ID_VIEW_GROUPBYTYPEKIND
Command3=ID_VIEW_TOOLBAR
Command4=ID_VIEW_STATUS_BAR
Command5=ID_APP_ABOUT
CommandCount=5

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_VIEW_GROUPBYTYPEKIND
Command2=ID_APP_ABOUT
CommandCount=2

[DLG:IDD_IPERSISTSTREAMINIT]
Type=1
Class=CIPersistStreamDlg
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_CLASSNAME,static,1342308352
Control5=IDC_CLSID,static,1342308352
Control6=IDC_ISDIRTY,button,1342242816
Control7=IDC_ISDIRTYTXT,static,1342308352
Control8=IDC_GETSIZEMAX,button,1342242816
Control9=IDC_SIZEMAX,static,1342308352
Control10=IDC_INITNEW,button,1342242816

