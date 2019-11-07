; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CWinPrintDoc
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "winprint.h"
VbxHeaderFile=winprint.h
VbxImplementationFile=winprint.cpp

ClassCount=5
Class1=CWinPrintApp
Class2=CWinPrintDoc
Class3=CWinPrintView
Class4=CMainFrame
Class5=CAboutDlg

ResourceCount=2
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME

[CLS:CWinPrintApp]
Type=0
HeaderFile=winprint.h
ImplementationFile=winprint.cpp
Filter=N
LastObject=CWinPrintApp

[CLS:CWinPrintDoc]
Type=0
HeaderFile=winprdoc.h
ImplementationFile=winprdoc.cpp
Filter=Not a Window
LastObject=CWinPrintDoc

[CLS:CWinPrintView]
Type=0
HeaderFile=winprvw.h
ImplementationFile=winprvw.cpp
Filter=Child Window
LastObject=CWinPrintView

[CLS:CMainFrame]
Type=0
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp
Filter=T
LastObject=CMainFrame

[CLS:CAboutDlg]
Type=0
HeaderFile=winprint.cpp
ImplementationFile=winprint.cpp
Filter=D
LastObject=CAboutDlg

[ACL:IDR_MAINFRAME]
Type=1
Class=CWinPrintView
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
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[MNU:IDR_MAINFRAME]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_APP_EXIT
Command6=ID_EDIT_UNDO
Command7=ID_EDIT_CUT
Command8=ID_EDIT_COPY
Command9=ID_EDIT_PASTE
Command10=ID_VIEW_TOOLBAR
Command11=ID_VIEW_STATUS_BAR
Command12=ID_HELP_INDEX
Command13=ID_HELP_USING
Command14=ID_APP_ABOUT
CommandCount=14
Class=CWinPrintView

