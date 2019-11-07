; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CWinPrintView
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

[CLS:CWinPrintDoc]
Type=0
HeaderFile=winprdoc.h
ImplementationFile=winprdoc.cpp
Filter=N

[CLS:CWinPrintView]
Type=0
HeaderFile=winprvw.h
ImplementationFile=winprvw.cpp
Filter=Child Window

[CLS:CMainFrame]
Type=0
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp
Filter=T

[CLS:CAboutDlg]
Type=0
HeaderFile=winprint.cpp
ImplementationFile=winprint.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=-1,static,1342177283
Control2=-1,static,1342308352
Control3=-1,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[MNU:IDR_MAINFRAME]
Type=1
Class=CWinPrintView
Command1=ID_APP_ABOUT
Command2=ID_APP_EXIT
Command3=ID_EDIT_COPY
Command4=ID_EDIT_CUT
Command5=ID_EDIT_PASTE
Command6=ID_EDIT_UNDO
Command7=ID_FILE_MRU_FILE1
Command8=ID_FILE_NEW
Command9=ID_FILE_OPEN
Command13=ID_FILE_SAVE
Command14=ID_FILE_SAVE_AS
Command15=ID_VIEW_STATUS_BAR
Command16=ID_VIEW_TOOLBAR
Command21=ID_HELP_INDEX
Command22=ID_HELP_USING
CommandCount=22


[ACL:IDR_MAINFRAME]
Type=1
Class=CWinPrintView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14
