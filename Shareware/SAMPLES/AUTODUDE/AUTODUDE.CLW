; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAutoDudeApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "autodude.h"
VbxHeaderFile=autodude.h
VbxImplementationFile=autodude.cpp

ClassCount=5
Class1=CAutoDudeApp
Class2=CAutoDudeDoc
Class3=CAutoDudeView
Class4=CMainFrame
Class5=CAboutDlg

ResourceCount=2
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME

[CLS:CAutoDudeApp]
Type=0
HeaderFile=autodude.h
ImplementationFile=autodude.cpp
Filter=N
LastObject=ID_APP_ABOUT

[CLS:CAutoDudeDoc]
Type=0
HeaderFile=autoddoc.h
ImplementationFile=autoddoc.cpp
Filter=N
LastObject=ID_FILE_NEW

[CLS:CAutoDudeView]
Type=0
HeaderFile=autodvw.h
ImplementationFile=autodvw.cpp
Filter=Child Window

[CLS:CMainFrame]
Type=0
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp
Filter=T
LastObject=CMainFrame

[CLS:CAboutDlg]
Type=0
HeaderFile=autodude.cpp
ImplementationFile=autodude.cpp
Filter=D

[ACL:IDR_MAINFRAME]
Type=1
Class=CAutoDudeView
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
Command13=ID_PREV_PANE
CommandCount=13
Command4=ID_EDIT_UNDO

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_VERSION,static,1342308352
Class=CAboutDlg

[MNU:IDR_MAINFRAME]
Type=1
Command1=ID_APP_EXIT
Command2=ID_VIEW_TOOLBAR
Command3=ID_VIEW_STATUS_BAR
Command4=ID_APP_ABOUT
CommandCount=4
Class=CAutoDudeView

