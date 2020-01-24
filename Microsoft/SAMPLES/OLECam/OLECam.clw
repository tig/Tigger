; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDoc
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "OLECam.h"
ODLFile=OLECam.odl
LastPage=0

ClassCount=5
Class1=CApp
Class2=CDoc
Class3=CSrvView
Class4=CMainFrame

ResourceCount=8
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource7=IDD_OLECam_FORM
Resource8=IDD_DEFAULT
Class5=CAboutDlg

[CLS:CApp]
Type=0
HeaderFile=OLECam.h
ImplementationFile=OLECam.cpp
Filter=N

[CLS:CDoc]
Type=0
HeaderFile=Doc.h
ImplementationFile=Doc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC
LastObject=CDoc

[CLS:CSrvView]
Type=0
HeaderFile=View.h
ImplementationFile=view.cpp
Filter=D

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T



[CLS:CAboutDlg]
Type=0
HeaderFile=OLECam.cpp
ImplementationFile=OLECam.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_SETDEFAULTIMAGE
Command2=ID_APP_EXIT
Command3=ID_APP_ABOUT
CommandCount=3

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
CommandCount=13
Command4=ID_EDIT_UNDO
Command13=ID_PREV_PANE

[DLG:IDD_OLECam_FORM]
Type=1
Class=CSrvView
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_IMAGE,edit,1350631552
Control3=IDC_BROWSE,button,1342242816
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_APPLY,button,1342242817

[DLG:IDD_DEFAULT]
Type=1
ControlCount=5
Control1=65535,static,1342308352
Control2=IDC_IMAGE,edit,1350631552
Control3=IDC_BROWSE,button,1342242816
Control4=IDOK,button,1342242817
Control5=IDCANCEL,button,1342242816

