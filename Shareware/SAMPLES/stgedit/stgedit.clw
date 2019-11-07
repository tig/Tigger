; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CStorageView
LastTemplate=CFormView
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "stgedit.h"
LastPage=0

ClassCount=9
Class1=CContentsView
Class2=CMainFrame
Class3=CSplitter
Class4=CStgEditApp
Class5=CAboutDlg
Class6=CStgEditDoc
Class7=CStgEditView
Class8=CStreamView

ResourceCount=3
Resource1=IDR_MAINFRAME
Resource2=IDD_ABOUTBOX
Class9=CStorageView
Resource3=IDD_STORAGE

[CLS:CContentsView]
Type=0
BaseClass=CView
HeaderFile=contentsview.h
ImplementationFile=contentsview.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp

[CLS:CSplitter]
Type=0
BaseClass=CSplitterWnd
HeaderFile=splitter.h
ImplementationFile=splitter.cpp
LastObject=CSplitter

[CLS:CStgEditApp]
Type=0
BaseClass=CWinApp
HeaderFile=stgedit.h
ImplementationFile=stgedit.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=stgedit.cpp
ImplementationFile=stgedit.cpp
LastObject=CAboutDlg

[CLS:CStgEditDoc]
Type=0
BaseClass=CDocument
HeaderFile=stgeditdoc.h
ImplementationFile=stgeditdoc.cpp

[CLS:CStgEditView]
Type=0
BaseClass=CView
HeaderFile=stgeditview.h
ImplementationFile=stgeditview.cpp
Filter=C
VirtualFilter=VWC
LastObject=CStgEditView

[CLS:CStorageView]
Type=0
HeaderFile=StorageView.h
ImplementationFile=StorageView.cpp
BaseClass=CFormView
Filter=D

[CLS:CStreamView]
Type=0
BaseClass=CContentsView
HeaderFile=streamview.h
ImplementationFile=streamview.cpp
LastObject=CStreamView

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_EDIT_COPY
Command3=ID_APP_ABOUT
CommandCount=3

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_NEW
Command3=ID_FILE_MRU_FILE1
Command4=ID_APP_EXIT
Command5=ID_EDIT_COPY
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_APP_ABOUT
CommandCount=8

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_FILE_OPEN
Command3=ID_EDIT_CUT
Command4=ID_NEXT_PANE
Command5=ID_PREV_PANE
Command6=ID_EDIT_COPY
CommandCount=6

[DLG:IDD_STORAGE]
Type=1
Class=CStorageView
ControlCount=32
Control1=IDC_STATIC,static,1342308352
Control2=IDC_CLSID,static,1342312576
Control3=IDC_STATIC,static,1342308352
Control4=IDC_NAME,static,1342312576
Control5=IDC_STATIC,static,1342308352
Control6=IDC_MTIME,static,1342312576
Control7=IDC_STATIC,static,1342308352
Control8=IDC_CTIME,static,1342312576
Control9=IDC_STATIC,static,1342308352
Control10=IDC_ATIME,static,1342312576
Control11=IDC_STGM_READ2,button,1476460547
Control12=IDC_STGM_READWRITE,button,1476460547
Control13=IDC_STGM_TRANSACTED,button,1476460547
Control14=IDC_STGM_CREATE,button,1476460547
Control15=IDC_STGM_CONVERT,button,1476460547
Control16=IDC_STGM_FAILIFTHERE,button,1476460547
Control17=IDC_STGM_SHARE_DENY_NONE,button,1476460547
Control18=IDC_STGM_SHARE_DENY_READ,button,1476460547
Control19=IDC_STGM_SHARE_DENY_WRITE,button,1476460547
Control20=IDC_STGM_SHARE_EXCLUSIVE,button,1476460547
Control21=IDC_STGM_PRIORITY,button,1476460547
Control22=IDC_STGM_DELETEONRELEASE,button,1476460547
Control23=IDC_STATIC,button,1342177287
Control24=IDC_STATSTATE_DOC,button,1476460547
Control25=IDC_STATSTATE_CONVERT,button,1476460547
Control26=IDC_STATSTATE_FILESTGSAME,button,1476460547
Control27=IDC_STATIC,button,1342177287
Control28=IDC_STGFMT_DOCUMENT,button,1476460547
Control29=IDC_STGFMT_DIRECTORY,button,1476460547
Control30=IDC_STGFMT_CATALOG,button,1476460547
Control31=IDC_STGFMT_FILE,button,1476460547
Control32=IDC_STATIC,button,1342177287

