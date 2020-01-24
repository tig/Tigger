; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CObjTreeView
LastTemplate=CPropertyPage
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ole2view.h"
LastPage=0

ClassCount=12
Class1=CAboutDlg
Class2=COle2ViewDoc
Class3=CMainFrame
Class4=CObjTreeView
Class5=COle2ViewApp
Class6=CRegistryView

ResourceCount=9
Resource1=IDD_REGVIEW
Resource2=IDR_MAINFRAME
Resource3=IDD_ABOUTBOX
Resource4=IDM_INTERFACE
Resource5=IDD_SERVERINFO
Class7=CServerInfoDlg
Resource6=IDM_OBJECT
Class8=CNetOleConfig
Resource7=IDD_ACTIVATIONTAB
Class9=CClassConfigDlg
Class10=CClassActivationPropPage
Resource8=IDD_CLASSACCESSPERMS
Class11=CClassAccessPropPage
Class12=CClassLaunchPropPage
Resource9=IDD_CLASSLAUNCHPERMS

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=AboutDlg.h
ImplementationFile=AboutDlg.cpp
LastObject=ID_VIEW_HIDDENCOMCATS

[CLS:COle2ViewDoc]
Type=0
BaseClass=CDocument
HeaderFile=doc.h
ImplementationFile=doc.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
VirtualFilter=fWC
LastObject=ID_OBJECT_CREATE

[CLS:CObjTreeView]
Type=0
BaseClass=CTreeView
HeaderFile=obj_vw.h
ImplementationFile=obj_vw.cpp
Filter=C
VirtualFilter=VWC
LastObject=ID_OBJECTS_NETOLE

[CLS:COle2ViewApp]
Type=0
BaseClass=CWinApp
HeaderFile=Ole2View.h
ImplementationFile=Ole2View.cpp
LastObject=COle2ViewApp

[CLS:CRegistryView]
Type=0
BaseClass=CFormView
HeaderFile=regview.h
ImplementationFile=regview.cpp

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

[DLG:IDD_REGVIEW]
Type=1
Class=CRegistryView
ControlCount=4
Control1=IDC_OBJICON,static,1342177283
Control2=IDC_OBJECTNAME,static,1342308492
Control3=IDC_OBJECTCLSID,static,1342308492
Control4=IDC_TREEVIEW,SysTreeView32,1350631442

[MNU:IDM_OBJECT]
Type=1
Class=CObjTreeView
Command1=ID_OBJECT_CREATE
Command2=ID_OBJECT_CREATE_ON
Command3=ID_OBJECT_RELEASE
Command4=ID_OBJECTS_NETOLE
Command5=ID_OBJECT_DELETE
CommandCount=5

[MNU:IDM_INTERFACE]
Type=1
Class=?
Command1=ID_OBJECT_VIEW
Command2=ID_OBJECT_DELETE
CommandCount=2

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_BINDTOAFILE
Command2=ID_FILE_VIEWTYPELIB
Command3=ID_FILE_RUNREGEDIT
Command4=ID_APP_EXIT
Command5=ID_IFACES_USEINPROCSERVER
Command6=ID_IFACES_USEINPROCHANDLER
Command7=ID_IFACES_USELOCALSERVER
Command8=ID_OBJECT_CREATE
Command9=ID_OBJECT_CREATE_ON
Command10=ID_OBJECT_RELEASE
Command11=ID_OBJECT_VIEW
Command12=ID_OBJECTS_NETOLE
Command13=ID_OBJECT_DELETE
Command14=ID_VIEW_HIDDENCOMCATS
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_VIEW_REFRESH
Command18=ID_HELP_INDEX
Command19=ID_HELP_USING
Command20=ID_APP_ABOUT
CommandCount=20

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_FILE_NEW
Command3=ID_FILE_OPEN
Command4=ID_FILE_SAVE
Command5=ID_EDIT_PASTE
Command6=ID_EDIT_UNDO
Command7=ID_EDIT_CUT
Command8=ID_HELP_INDEX
Command9=ID_VIEW_REFRESH
Command10=ID_NEXT_PANE
Command11=ID_PREV_PANE
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_EDIT_CUT
Command15=ID_EDIT_UNDO
CommandCount=15

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_BINDTOAFILE
Command2=ID_FILE_VIEWTYPELIB
Command3=ID_FILE_RUNREGEDIT
Command4=ID_OBJECT_DELETE
Command5=ID_IFACES_USEINPROCSERVER
Command6=ID_IFACES_USEINPROCHANDLER
Command7=ID_IFACES_USELOCALSERVER
Command8=ID_OBJECT_CREATE
Command9=ID_OBJECT_RELEASE
Command10=ID_OBJECT_VIEW
Command11=ID_APP_ABOUT
CommandCount=11

[DLG:IDD_SERVERINFO]
Type=1
Class=CServerInfoDlg
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_MACHINE,edit,1350631552
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816

[CLS:CServerInfoDlg]
Type=0
HeaderFile=ServerInfoDlg.h
ImplementationFile=ServerInfoDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=ID_OBJECT_ATSTORAGE

[CLS:CNetOleConfig]
Type=0
HeaderFile=NetOleConfig.h
ImplementationFile=NetOleConfig.cpp
BaseClass=CCmdTarget
Filter=N
LastObject=CNetOleConfig

[DLG:IDD_ACTIVATIONTAB]
Type=1
Class=CClassActivationPropPage
ControlCount=3
Control1=IDC_ATSTORAGE,button,1342242819
Control2=IDC_STATIC,static,1342308352
Control3=IDC_REMOTESERVERNAME,edit,1350631552

[CLS:CClassConfigDlg]
Type=0
HeaderFile=ClassConfigDlg.h
ImplementationFile=ClassConfigDlg.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CClassConfigDlg

[CLS:CClassActivationPropPage]
Type=0
HeaderFile=ClassActivationPropPage.h
ImplementationFile=ClassActivationPropPage.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CClassActivationPropPage

[DLG:IDD_CLASSLAUNCHPERMS]
Type=1
Class=CClassLaunchPropPage
ControlCount=0

[DLG:IDD_CLASSACCESSPERMS]
Type=1
Class=CClassAccessPropPage
ControlCount=0

[CLS:CClassAccessPropPage]
Type=0
HeaderFile=ClassAccessPropPage.h
ImplementationFile=ClassAccessPropPage.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CClassAccessPropPage

[CLS:CClassLaunchPropPage]
Type=0
HeaderFile=ClassLaunchPropPage.h
ImplementationFile=ClassLaunchPropPage.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CClassLaunchPropPage

