; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CColumnListBox
LastTemplate=CView
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "xrt_obj.h"
VbxHeaderFile=xrt_obj.h
VbxImplementationFile=xrt_obj.cpp

ClassCount=9
Class1=CXRTConfigView
Class2=CXRTData
Class3=CMainFrame
Class4=CConfigMDIChild
Class5=CObjectMDIChild
Class6=CXRTObjectView
Class7=CXRTApp
Class8=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class9=CColumnListBox
Resource3=IDR_CONFIGVIEW

[CLS:CXRTConfigView]
Type=0
HeaderFile=config.h
ImplementationFile=config.cpp
Filter=D
LastObject=CXRTConfigView

[CLS:CXRTData]
Type=0
HeaderFile=doc.h
ImplementationFile=doc.cpp

[CLS:CMainFrame]
Type=0
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp

[CLS:CConfigMDIChild]
Type=0
HeaderFile=mdi.h
ImplementationFile=mdi.cpp

[CLS:CObjectMDIChild]
Type=0
HeaderFile=mdi.h
ImplementationFile=mdi.cpp
Filter=MDI Child Frame
LastObject=CObjectMDIChild

[CLS:CXRTObjectView]
Type=0
HeaderFile=object.h
ImplementationFile=object.cpp
LastObject=CXRTObjectView
Filter=W

[CLS:CXRTApp]
Type=0
HeaderFile=xrt_obj.h
ImplementationFile=xrt_obj.cpp

[CLS:CAboutDlg]
Type=0
HeaderFile=xrt_obj.cpp
ImplementationFile=xrt_obj.cpp

[MNU:IDR_MAINFRAME]
Type=1
Command1=ID_APP_EXIT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_CLEAR
Command4=ID_WINDOW_CASCADE
Command5=ID_WINDOW_TILE_HORZ
Command6=ID_WINDOW_ARRANGE
Command7=ID_APP_ABOUT
CommandCount=7
Class=?

[ACL:IDR_MAINFRAME]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13
Class=?

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=8
Control1=IDC_STATIC,static,1342177283
Control2=IDC_APPNAME,static,1342308352
Control3=IDC_COPYRIGHT,static,1342308352
Control4=IDCANCEL,button,1342373889
Control5=IDC_ALLRIGHTSRESERVED,static,1342308352
Control6=IDC_WRITTENBY,static,1342308352
Control7=IDC_GROUP,static,1342308352
Control8=IDC_VERSION,static,1342308352
Class=CAboutDlg

[DLG:IDR_CONFIGVIEW]
Type=1
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_TIMEOUT,edit,1350631552
Control3=IDC_SET,button,1342242817
Control4=IDC_INSTRUMENTS,listbox,1352733011
Control5=IDC_STATIC,static,1342308352
Control6=IDC_SHOWCHANGES,button,1342242819
Class=CXRTConfigView

[CLS:CColumnListBox]
Type=0
HeaderFile=collist.h
ImplementationFile=collist.cpp
LastObject=CColumnListBox

