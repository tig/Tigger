; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFListView
LastTemplate=CDocument
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "iniedit.h"
VbxHeaderFile=iniedit.h
VbxImplementationFile=iniedit.cpp

ClassCount=10
Class1=CINIeditDoc
Class2=CINIeditApp
Class3=CAboutDlg
Class4=CINIeditView
Class5=CMainFrame

ResourceCount=5
Resource1=IDR_MAINFRAME
Resource2=IDD_ABOUTBOX
Class6=CINIfilesDoc
Class7=CINIfilesView
Resource3=IDR_FILESTYPE
Resource4=IDR_INITYPE
Class8=CTreeView
Resource5=IDD_FILELIST
Class9=CFListView
Class10=CFileListDoc

[CLS:CINIeditDoc]
Type=0
HeaderFile=inieddoc.h
ImplementationFile=inieddoc.cpp

[CLS:CINIeditApp]
Type=0
HeaderFile=iniedit.h
ImplementationFile=iniedit.cpp
Filter=N
LastObject=ID_FILE_NEW

[CLS:CAboutDlg]
Type=0
HeaderFile=iniedit.cpp
ImplementationFile=iniedit.cpp

[CLS:CINIeditView]
Type=0
HeaderFile=iniedvw.h
ImplementationFile=iniedvw.cpp
LastObject=CINIeditView

[CLS:CMainFrame]
Type=0
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp
LastObject=CMainFrame

[ACL:IDR_MAINFRAME]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
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
Command15=ID_CONTEXT_HELP
Command16=ID_HELP
CommandCount=16
Class=?

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[CLS:CINIfilesDoc]
Type=0
HeaderFile=filesdoc.h
ImplementationFile=filesdoc.cpp
Filter=Not a Window
LastObject=CINIfilesDoc

[CLS:CINIfilesView]
Type=0
HeaderFile=filesvw.h
ImplementationFile=filesvw.cpp
Filter=MDI Child Frame

[MNU:IDR_MAINFRAME]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_HELP_INDEX
Command9=ID_HELP_USING
Command10=ID_APP_ABOUT
CommandCount=10
Class=?

[MNU:IDR_FILESTYPE]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT
Command4=ID_FILE_PRINT_PREVIEW
Command5=ID_FILE_PRINT_SETUP
Command6=ID_FILE_MRU_FILE1
Command7=ID_APP_EXIT
Command8=ID_VIEW_TOOLBAR
Command9=ID_VIEW_STATUS_BAR
Command10=ID_WINDOW_NEW
Command11=ID_WINDOW_CASCADE
Command12=ID_WINDOW_TILE_HORZ
Command13=ID_WINDOW_ARRANGE
Command14=ID_HELP_INDEX
Command15=ID_HELP_USING
Command16=ID_APP_ABOUT
CommandCount=16

[MNU:IDR_INITYPE]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_PRINT
Command5=ID_FILE_PRINT_PREVIEW
Command6=ID_FILE_PRINT_SETUP
Command7=ID_FILE_MRU_FILE1
Command8=ID_APP_EXIT
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_VIEW_TOOLBAR
Command14=ID_VIEW_STATUS_BAR
Command15=ID_WINDOW_NEW
Command16=ID_WINDOW_CASCADE
Command17=ID_WINDOW_TILE_HORZ
Command18=ID_WINDOW_ARRANGE
Command19=ID_HELP_INDEX
Command20=ID_HELP_USING
Command21=ID_APP_ABOUT
CommandCount=21
Class=?

[CLS:CTreeView]
Type=0
HeaderFile=treeview.h
ImplementationFile=treeview.cpp
Filter=C
LastObject=CTreeView

[DLG:IDD_FILELIST]
Type=1
ControlCount=1
Control1=IDC_FILELIST,listbox,1352667393
Class=CFListView

[CLS:CFileListDoc]
Type=0
HeaderFile=flistdoc.h
ImplementationFile=flistdoc.cpp
Filter=Not a Window
LastObject=CFileListDoc

[CLS:CFListView]
Type=0
HeaderFile=flistvw.h
ImplementationFile=flistvw.cpp
Filter=C
LastObject=CFListView

