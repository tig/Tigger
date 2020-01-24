; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "coltest.h"
VbxHeaderFile=coltest.h
VbxImplementationFile=coltest.cpp

ClassCount=5
Class1=CColtestApp
Class2=CColtestDoc
Class3=CColtestView
Class4=CMainFrame
Class5=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_CTTYPE
Resource3=IDR_MAINFRAME

[CLS:CColtestApp]
Type=0
HeaderFile=coltest.h
ImplementationFile=coltest.cpp
Filter=N

[CLS:CColtestDoc]
Type=0
HeaderFile=coltedoc.h
ImplementationFile=coltedoc.cpp
Filter=N
LastObject=ID_FILE_NEW

[CLS:CColtestView]
Type=0
HeaderFile=coltevw.h
ImplementationFile=coltevw.cpp
Filter=W
LastObject=CColtestView

[CLS:CMainFrame]
Type=0
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp
Filter=T
LastObject=CMainFrame

[CLS:CAboutDlg]
Type=0
HeaderFile=coltest.cpp
ImplementationFile=coltest.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=-1,static,1342177283
Control2=-1,static,1342308352
Control3=-1,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[ACL:IDR_MAINFRAME]
Type=1
Class=CColtestView
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

[MNU:IDR_CTTYPE]
Type=1
Command1=ID_FILE_OPEN
Command2=ID_FILE_CLOSE
Command3=ID_APP_EXIT
Command4=ID_WINDOW_NEW
Command5=ID_WINDOW_CASCADE
Command6=ID_WINDOW_TILE_HORZ
Command7=ID_WINDOW_ARRANGE
Command8=ID_APP_ABOUT
CommandCount=8
Class=CColtestView

[MNU:IDR_MAINFRAME]
Type=1
Command1=ID_FILE_OPEN
Command2=ID_APP_EXIT
Command3=ID_APP_ABOUT
CommandCount=3
Class=CMainFrame

