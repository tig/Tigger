; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTickCtrl
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "xrttick.h"
LastPage=0
CDK=Y

ClassCount=4
Class1=CTickCtrl
Class2=CTickPropPage
Class3=CDataObjectPropPage
Class4=CXRTPropPage

ResourceCount=4
Resource1=IDD_PROPPAGE_WOSAXRT
Resource2=IDD_PROPPAGE_XRTTICKER
Resource3=IDD_PROPPAGE_DATAOBJECT
Resource4=IDD_ABOUTBOX_XRTTICKER

[CLS:CTickCtrl]
Type=0
BaseClass=COleControl
HeaderFile=TICKCTL.H
ImplementationFile=TICKCTL.CPP
LastObject=CTickCtrl
Filter=W
VirtualFilter=WC

[CLS:CTickPropPage]
Type=0
BaseClass=COlePropertyPage
HeaderFile=xrttippg.h
ImplementationFile=xrttippg.cpp

[CLS:CDataObjectPropPage]
Type=0
BaseClass=COlePropertyPage
HeaderFile=xrttippg.h
ImplementationFile=xrttippg.cpp
Filter=D
VirtualFilter=idWC

[CLS:CXRTPropPage]
Type=0
BaseClass=COlePropertyPage
HeaderFile=xrttippg.h
ImplementationFile=xrttippg.cpp
Filter=D
VirtualFilter=idWC

[DLG:IDD_PROPPAGE_XRTTICKER]
Type=1
Class=CTickPropPage
ControlCount=11
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDIT1,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT2,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT3,edit,1350631552
Control8=IDC_STATIC,static,1342308352
Control9=IDC_ABOUT,button,1342242816
Control10=IDC_CACHEON,button,1342242819
Control11=IDC_OFFSETVALUES,button,1342242819

[DLG:IDD_PROPPAGE_DATAOBJECT]
Type=1
Class=CDataObjectPropPage
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=ID_SERVERNAME,edit,1350631552
Control3=IDC_ACTIVE,button,1342242819
Control4=IDC_STARTSTOP,button,1342242816

[DLG:IDD_PROPPAGE_WOSAXRT]
Type=1
Class=CXRTPropPage
ControlCount=8
Control1=IDC_MAKESERVERVISIBLE,button,1342242819
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308354
Control4=IDC_REQUEST,edit,1350631552
Control5=IDC_STATIC,static,1342308354
Control6=IDC_NAMEPROPERTY,edit,1350631552
Control7=IDC_STATIC,static,1342308354
Control8=IDC_VALUEPROPERTY,edit,1350631552

[DLG:IDD_ABOUTBOX_XRTTICKER]
Type=1
Class=?
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

