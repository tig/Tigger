; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSumInfoDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "prop.h"
VbxHeaderFile=prop.h
VbxImplementationFile=prop.cpp

ClassCount=6
Class1=CPropApp
Class2=CPropDoc
Class3=CPropView
Class4=CMainFrame
Class5=CAboutDlg

ResourceCount=3
Resource1=IDR_MAINFRAME
Resource2=IDD_SUMINFO
Class6=CSumInfoDlg
Resource3=IDD_ABOUTBOX

[CLS:CPropApp]
Type=0
HeaderFile=prop.h
ImplementationFile=prop.cpp
Filter=N
LastObject=ID_APP_ABOUT

[CLS:CPropDoc]
Type=0
HeaderFile=propdoc.h
ImplementationFile=propdoc.cpp
Filter=N
LastObject=ID_FILE_SUMMARYINFORMATION

[CLS:CPropView]
Type=0
HeaderFile=propview.h
ImplementationFile=propview.cpp
Filter=Child Window
LastObject=ID_APP_ABOUT

[CLS:CMainFrame]
Type=0
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp
Filter=T
LastObject=CMainFrame

[CLS:CAboutDlg]
Type=0
HeaderFile=prop.cpp
ImplementationFile=prop.cpp
Filter=Dialog
LastObject=CAboutDlg

[ACL:IDR_MAINFRAME]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_NEXT_PANE
Command5=ID_PREV_PANE
CommandCount=5
Class=CPropView

[CLS:CSumInfoDlg]
Type=0
HeaderFile=propdoc.h
ImplementationFile=propdoc.cpp
Filter=D
LastObject=CSumInfoDlg

[MNU:IDR_MAINFRAME]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_SUMMARYINFORMATION
Command6=ID_FILE_MRU_FILE1
Command7=ID_APP_EXIT
Command8=ID_APP_ABOUT
CommandCount=8
Class=CPropView

[DLG:IDD_SUMINFO]
Type=1
ControlCount=23
Control1=IDC_STATIC,static,1342308352
Control2=IDD_SUMINFO,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDC_DIRECTORY,static,1342308480
Control5=IDC_STATIC,static,1342308352
Control6=IDC_TITLE,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_SUBJECT,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_AUTHOR,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_KEYWORDS,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_COMMENTS,edit,1352732740
Control15=IDC_STATIC,static,1342308352
Control16=IDC_TEMPLATE,edit,1350631552
Control17=IDC_STATIC,static,1342308352
Control18=IDC_LASTAUTHOR,edit,1350631552
Control19=IDC_STATIC,static,1342308352
Control20=IDC_REVNUMBER,edit,1350631552
Control21=IDOK,button,1342242817
Control22=IDCANCEL,button,1342242816
Control23=IDC_DETAILS,button,1342242816
Class=CSumInfoDlg

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352
Class=CAboutDlg

