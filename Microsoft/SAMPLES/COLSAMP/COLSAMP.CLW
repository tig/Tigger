; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CColSampView
LastTemplate=CCmdTarget
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "colsamp.h"
VbxHeaderFile=colsamp.h
VbxImplementationFile=colsamp.cpp
LastPage=0

ClassCount=11
Class1=CColumnListBox
Class2=CColSampApp
Class3=CAboutDlg
Class4=CColSampDoc
Class5=CInPlaceFrame
Class6=CItem
Class7=CItems
Class8=CMainFrame
Class9=CColSampView

ResourceCount=6
Resource1=IDR_COLSAMPTYPE
Resource2=IDD_ABOUTBOX
Resource3=IDR_MAINFRAME
Resource4=IDD_ADDITEM
Resource5=IDR_COLSAMPTYPE_SRVR_EMB
Class10=CChangeItemDlg
Resource6=IDR_COLSAMPTYPE_SRVR_IP
Class11=CColSampAppProxy

[CLS:CColumnListBox]
Type=0
HeaderFile=collist.h
ImplementationFile=collist.cpp

[CLS:CColSampApp]
Type=0
HeaderFile=colsamp.h
ImplementationFile=colsamp.cpp
Filter=N

[CLS:CAboutDlg]
Type=0
HeaderFile=colsamp.cpp
ImplementationFile=colsamp.cpp

[CLS:CColSampDoc]
Type=0
HeaderFile=doc.h
ImplementationFile=doc.cpp
Filter=N

[CLS:CInPlaceFrame]
Type=0
HeaderFile=ipframe.h
ImplementationFile=ipframe.cpp

[CLS:CItem]
Type=0
HeaderFile=item.h
ImplementationFile=item.cpp

[CLS:CItems]
Type=0
HeaderFile=items.h
ImplementationFile=items.cpp

[CLS:CMainFrame]
Type=0
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp

[CLS:CColSampView]
Type=0
HeaderFile=view.h
ImplementationFile=view.cpp
Filter=C
LastObject=CColSampView

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=7
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDCANCEL,button,1342373889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_MRU_FILE1
Command4=ID_APP_EXIT
Command5=ID_VIEW_TOOLBAR
Command6=ID_VIEW_STATUS_BAR
Command7=ID_HELP_INDEX
Command8=ID_HELP_USING
Command9=ID_APP_ABOUT
CommandCount=9

[MNU:IDR_COLSAMPTYPE]
Type=1
Class=CColSampView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_MRU_FILE1
Command7=ID_APP_EXIT
Command8=ID_EDIT_CHANGEITEM
Command9=ID_EDIT_ADDITEM
Command10=ID_EDIT_REMOVEITEM
Command11=ID_VIEW_TOOLBAR
Command12=ID_VIEW_STATUS_BAR
Command13=ID_WINDOW_NEW
Command14=ID_WINDOW_CASCADE
Command15=ID_WINDOW_TILE_HORZ
Command16=ID_WINDOW_ARRANGE
Command17=ID_HELP_INDEX
Command18=ID_HELP_USING
Command19=ID_APP_ABOUT
CommandCount=19

[MNU:IDR_COLSAMPTYPE_SRVR_EMB]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_UPDATE
Command5=ID_FILE_SAVE_COPY_AS
Command6=ID_FILE_MRU_FILE1
Command7=ID_APP_EXIT
Command8=ID_EDIT_CHANGEITEM
Command9=ID_EDIT_ADDITEM
Command10=ID_EDIT_REMOVEITEM
Command11=ID_VIEW_TOOLBAR
Command12=ID_VIEW_STATUS_BAR
Command13=ID_WINDOW_NEW
Command14=ID_WINDOW_CASCADE
Command15=ID_WINDOW_TILE_HORZ
Command16=ID_WINDOW_ARRANGE
Command17=ID_HELP_INDEX
Command18=ID_HELP_USING
Command19=ID_APP_ABOUT
CommandCount=19

[MNU:IDR_COLSAMPTYPE_SRVR_IP]
Type=1
Class=?
Command1=ID_EDIT_CHANGEITEM
Command2=ID_EDIT_ADDITEM
Command3=ID_EDIT_REMOVEITEM
Command4=ID_HELP_INDEX
Command5=ID_HELP_USING
Command6=ID_APP_ABOUT
CommandCount=6

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_REMOVEITEM
Command5=ID_HELP
Command6=ID_CONTEXT_HELP
Command7=ID_NEXT_PANE
Command8=ID_PREV_PANE
Command9=ID_EDIT_ADDITEM
Command10=ID_EDIT_CHANGEITEM
CommandCount=10

[ACL:IDR_COLSAMPTYPE_SRVR_IP]
Type=1
Class=?
Command1=ID_EDIT_REMOVEITEM
Command2=ID_HELP
Command3=ID_CONTEXT_HELP
Command4=ID_EDIT_ADDITEM
Command5=ID_EDIT_CHANGEITEM
CommandCount=5

[ACL:IDR_COLSAMPTYPE_SRVR_EMB]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_UPDATE
Command4=ID_EDIT_REMOVEITEM
Command5=ID_HELP
Command6=ID_CONTEXT_HELP
Command7=ID_NEXT_PANE
Command8=ID_PREV_PANE
Command9=ID_EDIT_ADDITEM
Command10=ID_EDIT_CHANGEITEM
CommandCount=10

[DLG:IDD_ADDITEM]
Type=1
Class=CChangeItemDlg
ControlCount=8
Control1=IDC_STATIC,static,1342308352
Control2=IDC_NAME,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_TYPE,combobox,1344342275
Control5=IDC_STATIC,static,1342308352
Control6=IDC_VALUE,edit,1350631552
Control7=IDOK,button,1342242817
Control8=IDCANCEL,button,1342242816

[CLS:CChangeItemDlg]
Type=0
HeaderFile=view.h
ImplementationFile=view.cpp
Filter=D
LastObject=IDC_TYPE

[CLS:CColSampAppProxy]
Type=0
HeaderFile=colsamp.h
ImplementationFile=colsamp.cpp
Filter=C

