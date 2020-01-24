// WOSA/XRT XRTTick Sample OLE Control Version 1.00.001
//
// Copyright (c) 1993-94 Microsoft Corporation, All Rights Reserved.
//
//      This is a part of the Microsoft Source Code Samples. 
//      This source code is only intended as a supplement to 
//      Microsoft Development Tools and/or WinHelp documentation.
//      See these sources for detailed information regarding the 
//      Microsoft samples programs.
//
// xrttippg.cpp : Implementation of the CTickPropPage property page class.

#include "stdafx.h"
#include "xrttick.h"
#include "xrttippg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CTickPropPage, COlePropertyPage)
#define new DEBUG_NEW 


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CTickPropPage, COlePropertyPage)
    //{{AFX_MSG_MAP(CTickPropPage)
    ON_BN_CLICKED(IDC_ABOUT, OnAbout)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CTickPropPage, "XRTTick.TickerPropPage.1",
    0x5aabe534, 0xdaf0, 0x11cd, 0x84, 0x45, 0x0, 0xaa, 0x0, 0x33, 0x97, 0x10)


/////////////////////////////////////////////////////////////////////////////
// CTickPropPage::CTickPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CTickPropPage

BOOL CTickPropPage::CTickPropPageFactory::UpdateRegistry(BOOL bRegister)
{
    if (bRegister)
        return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
            m_clsid, IDS_XRTTICKER_PPG);
    else
        return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CTickPropPage::CTickPropPage - Constructor

CTickPropPage::CTickPropPage() :
    COlePropertyPage(IDD, IDS_XRTTICKER_PPG_CAPTION)
{
    //{{AFX_DATA_INIT(CTickPropPage)
    m_strText = _T("");
    m_lSpeed = 0;
    m_lTickWidth = 0;
    m_fCacheOn = FALSE;
    m_fOffsetValues = FALSE;
    //}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CTickPropPage::DoDataExchange - Moves data between page and properties

void CTickPropPage::DoDataExchange(CDataExchange* pDX)
{
    //{{AFX_DATA_MAP(CTickPropPage)
    DDP_Text(pDX, IDC_EDIT1, m_strText, _T("Text") );
    DDX_Text(pDX, IDC_EDIT1, m_strText);
    DDP_Text(pDX, IDC_EDIT2, m_lSpeed, _T("ScrollSpeed") );
    DDX_Text(pDX, IDC_EDIT2, m_lSpeed);
    DDV_MinMaxLong(pDX, m_lSpeed, 1, 10000);
    DDP_Text(pDX, IDC_EDIT3, m_lTickWidth, _T("ScrollWidth") );
    DDX_Text(pDX, IDC_EDIT3, m_lTickWidth);
    DDV_MinMaxLong(pDX, m_lTickWidth, 1, 100);
    DDP_Check(pDX, IDC_CACHEON, m_fCacheOn, _T("CacheOn") );
    DDX_Check(pDX, IDC_CACHEON, m_fCacheOn);
    DDP_Check(pDX, IDC_OFFSETVALUES, m_fOffsetValues, _T("OffsetValues") );
    DDX_Check(pDX, IDC_OFFSETVALUES, m_fOffsetValues);
    //}}AFX_DATA_MAP
    DDP_PostProcessing(pDX);     
}


/////////////////////////////////////////////////////////////////////////////
// CTickPropPage message handlers

void CTickPropPage::OnAbout() 
{
    CDialog dlgAbout(IDD_ABOUTBOX_XRTTICKER,this);
    dlgAbout.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CDataObjectPropPage dialog

IMPLEMENT_DYNCREATE(CDataObjectPropPage, COlePropertyPage)
#define new DEBUG_NEW 


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CDataObjectPropPage, COlePropertyPage)
    //{{AFX_MSG_MAP(CDataObjectPropPage)
	ON_BN_CLICKED(IDC_STARTSTOP, OnStartstop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDataObjectPropPage, "XRTTick.DataObjectPropPage.1",
    0xd50c40b0, 0xe21c, 0x11cd, 0x84, 0x45, 0x0, 0xaa, 0x0, 0x33, 0x97, 0x10)


/////////////////////////////////////////////////////////////////////////////
// CDataObjectPropPage::CDataObjectPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CDataObjectPropPage

BOOL CDataObjectPropPage::CDataObjectPropPageFactory::UpdateRegistry(BOOL bRegister)
{
    if (bRegister)
        return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
            m_clsid, IDS_DATAOBJECT_PPG);
    else
        return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CDataObjectPropPage::CDataObjectPropPage - Constructor

CDataObjectPropPage::CDataObjectPropPage() :
    COlePropertyPage(IDD, IDS_DATAOBJECT_PPG_CAPTION)
{
    //{{AFX_DATA_INIT(CDataObjectPropPage)
	m_fActive = FALSE;
	m_strDataObjectName = _T("");
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CDataObjectPropPage::DoDataExchange - Moves data between page and properties

void CDataObjectPropPage::DoDataExchange(CDataExchange* pDX)
{
    // NOTE: ClassWizard will add DDP, DDX, and DDV calls here
    //    DO NOT EDIT what you see in these blocks of generated code !
    //{{AFX_DATA_MAP(CDataObjectPropPage)
	DDX_Control(pDX, IDC_STARTSTOP, m_btnStartStop);
	DDP_Check(pDX, IDC_ACTIVE, m_fActive, _T("DataObjectActive") );
	DDX_Check(pDX, IDC_ACTIVE, m_fActive);
	DDP_Text(pDX, ID_SERVERNAME, m_strDataObjectName, _T("DataObjectName") );
	DDX_Text(pDX, ID_SERVERNAME, m_strDataObjectName);
	//}}AFX_DATA_MAP
    DDP_PostProcessing(pDX);

    if (pDX->m_bSaveAndValidate == FALSE)
    {
        C_DXRTTicker  ticker ;
        ULONG ulObjects = NULL ;
        IDispatch** prgDisp ;
        prgDisp = GetObjectArray(&ulObjects);
        while (ulObjects--)
        {
            ticker.AttachDispatch(prgDisp[ulObjects], FALSE) ;
            IUnknown* p = ticker.GetDataObject();
            if (p)
            {
                p->Release() ;
                m_btnStartStop.SetWindowText(_T("&Stop Now")) ;
            }
            else
                m_btnStartStop.SetWindowText(_T("&Start Now")) ;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
// CDataObjectPropPage message handlers

void CDataObjectPropPage::OnStartstop()
{
    C_DXRTTicker  ticker ;
    ULONG ulObjects = NULL ;
    IDispatch** prgDisp ;
    prgDisp = GetObjectArray(&ulObjects);
    while (ulObjects--)
    {
        try
        {
            ticker.AttachDispatch(prgDisp[ulObjects], FALSE) ;
            IUnknown* p = ticker.GetDataObject();
            if (p)
            {
                p->Release() ;
                ticker.StopDataObject() ;
                UpdateData(FALSE);
            }
            else
            {
                UpdateData(TRUE);
                if (ticker.StartDataObject())
                {
                    if (ticker.GetAutoStartDataObject() || ticker.InitializeDataObject())
                        m_btnStartStop.SetWindowText(_T("&Stop Now")) ;
                }
            }
        }
        catch(COleDispatchException* e)
        {
            CString str ;
            str = e->m_strDescription + _T(" (") + e->m_strSource + _T(")") ;
            AfxMessageBox( str ) ;
            e->Delete() ;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
// CDataObjectPropPage dialog

IMPLEMENT_DYNCREATE(CXRTPropPage, COlePropertyPage)
#define new DEBUG_NEW 


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CXRTPropPage, COlePropertyPage)
    //{{AFX_MSG_MAP(CXRTPropPage)
    // NOTE - ClassWizard will add and remove message map entries
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CXRTPropPage, "XRTTick.XRTPropPage.1",
    0xd50c40b1, 0xe21c, 0x11cd, 0x84, 0x45, 0x0, 0xaa, 0x0, 0x33, 0x97, 0x10)


/////////////////////////////////////////////////////////////////////////////
// CXRTPropPage::CXRTPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CXRTPropPage

BOOL CXRTPropPage::CXRTPropPageFactory::UpdateRegistry(BOOL bRegister)
{
    if (bRegister)
        return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
            m_clsid, IDS_XRT_PPG);
    else
        return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CXRTPropPage::CXRTPropPage - Constructor

CXRTPropPage::CXRTPropPage() :
    COlePropertyPage(IDD, IDS_XRT_PPG_CAPTION)
{
    //{{AFX_DATA_INIT(CXRTPropPage)
    m_fDataObjectVisible = FALSE;
    m_strRequest = _T("");
    m_strNameProperty = _T("");
    m_strValueProperty = _T("");
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CXRTPropPage::DoDataExchange - Moves data between page and properties

void CXRTPropPage::DoDataExchange(CDataExchange* pDX)
{
    // NOTE: ClassWizard will add DDP, DDX, and DDV calls here
    //    DO NOT EDIT what you see in these blocks of generated code !
    //{{AFX_DATA_MAP(CXRTPropPage)
	DDP_Check(pDX, IDC_MAKESERVERVISIBLE, m_fDataObjectVisible, _T("DataObjectVisible") );
	DDX_Check(pDX, IDC_MAKESERVERVISIBLE, m_fDataObjectVisible);
	DDP_Text(pDX, IDC_REQUEST, m_strRequest, _T("DataObjectRequest") );
	DDX_Text(pDX, IDC_REQUEST, m_strRequest);
	DDP_Text(pDX, IDC_NAMEPROPERTY, m_strNameProperty, _T("DataObjectNameProperty") );
	DDX_Text(pDX, IDC_NAMEPROPERTY, m_strNameProperty);
	DDP_Text(pDX, IDC_VALUEPROPERTY, m_strValueProperty, _T("DataObjectValueProperty") );
	DDX_Text(pDX, IDC_VALUEPROPERTY, m_strValueProperty);
	//}}AFX_DATA_MAP
    DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CXRTPropPage message handlers


/////////////////////////////////////////////////////////////////////////////
// C_DXRTTicker properties

BOOL C_DXRTTicker::GetEnabled()
{
	BOOL result;
	GetProperty(DISPID_ENABLED, VT_BOOL, (void*)&result);
	return result;
}

void C_DXRTTicker::SetEnabled(BOOL propVal)
{
	SetProperty(DISPID_ENABLED, VT_BOOL, propVal);
}

long C_DXRTTicker::GetScrollSpeed()
{
	long result;
	GetProperty(0x1, VT_I4, (void*)&result);
	return result;
}

void C_DXRTTicker::SetScrollSpeed(long propVal)
{
	SetProperty(0x1, VT_I4, propVal);
}

long C_DXRTTicker::GetScrollWidth()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

void C_DXRTTicker::SetScrollWidth(long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}

long C_DXRTTicker::GetMaxCacheSize()
{
	long result;
	GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}

void C_DXRTTicker::SetMaxCacheSize(long propVal)
{
	SetProperty(0x3, VT_I4, propVal);
}

CString C_DXRTTicker::GetDataObjectName()
{
	CString result;
	GetProperty(0x4, VT_BSTR, (void*)&result);
	return result;
}

void C_DXRTTicker::SetDataObjectName(LPCTSTR propVal)
{
	SetProperty(0x4, VT_BSTR, propVal);
}

BOOL C_DXRTTicker::GetDataObjectActive()
{
	BOOL result;
	GetProperty(0x5, VT_BOOL, (void*)&result);
	return result;
}

void C_DXRTTicker::SetDataObjectActive(BOOL propVal)
{
	SetProperty(0x5, VT_BOOL, propVal);
}

BOOL C_DXRTTicker::GetDataObjectVisible()
{
	BOOL result;
	GetProperty(0x6, VT_BOOL, (void*)&result);
	return result;
}

void C_DXRTTicker::SetDataObjectVisible(BOOL propVal)
{
	SetProperty(0x6, VT_BOOL, propVal);
}

BOOL C_DXRTTicker::GetAutoStartDataObject()
{
	BOOL result;
	GetProperty(0x7, VT_BOOL, (void*)&result);
	return result;
}

void C_DXRTTicker::SetAutoStartDataObject(BOOL propVal)
{
	SetProperty(0x7, VT_BOOL, propVal);
}

BOOL C_DXRTTicker::GetCacheOn()
{
	BOOL result;
	GetProperty(0x8, VT_BOOL, (void*)&result);
	return result;
}

void C_DXRTTicker::SetCacheOn(BOOL propVal)
{
	SetProperty(0x8, VT_BOOL, propVal);
}

BOOL C_DXRTTicker::GetOffsetValues()
{
	BOOL result;
	GetProperty(0x9, VT_BOOL, (void*)&result);
	return result;
}

void C_DXRTTicker::SetOffsetValues(BOOL propVal)
{
	SetProperty(0x9, VT_BOOL, propVal);
}

CString C_DXRTTicker::GetDataObjectNameProperty()
{
	CString result;
	GetProperty(0xa, VT_BSTR, (void*)&result);
	return result;
}

void C_DXRTTicker::SetDataObjectNameProperty(LPCTSTR propVal)
{
	SetProperty(0xa, VT_BSTR, propVal);
}

CString C_DXRTTicker::GetDataObjectValueProperty()
{
	CString result;
	GetProperty(0xb, VT_BSTR, (void*)&result);
	return result;
}

void C_DXRTTicker::SetDataObjectValueProperty(LPCTSTR propVal)
{
	SetProperty(0xb, VT_BSTR, propVal);
}

CString C_DXRTTicker::GetDataObjectRequest()
{
	CString result;
	GetProperty(0xc, VT_BSTR, (void*)&result);
	return result;
}

void C_DXRTTicker::SetDataObjectRequest(LPCTSTR propVal)
{
	SetProperty(0xc, VT_BSTR, propVal);
}

LPUNKNOWN C_DXRTTicker::GetDataObject()
{
	LPUNKNOWN result;
	GetProperty(0xd, VT_UNKNOWN, (void*)&result);
	return result;
}

void C_DXRTTicker::SetDataObject(LPUNKNOWN propVal)
{
	SetProperty(0xd, VT_UNKNOWN, propVal);
}

LPFONTDISP C_DXRTTicker::GetFont()
{
	LPFONTDISP result;
	GetProperty(DISPID_FONT, VT_DISPATCH, (void*)&result);
	return result;
}

void C_DXRTTicker::SetFont(LPFONTDISP propVal)
{
	SetProperty(DISPID_FONT, VT_DISPATCH, propVal);
}

OLE_COLOR C_DXRTTicker::GetForeColor()
{
	OLE_COLOR result;
	GetProperty(DISPID_FORECOLOR, VT_I4, (void*)&result);
	return result;
}

void C_DXRTTicker::SetForeColor(OLE_COLOR propVal)
{
	SetProperty(DISPID_FORECOLOR, VT_I4, propVal);
}

OLE_COLOR C_DXRTTicker::GetBackColor()
{
	OLE_COLOR result;
	GetProperty(DISPID_BACKCOLOR, VT_I4, (void*)&result);
	return result;
}

void C_DXRTTicker::SetBackColor(OLE_COLOR propVal)
{
	SetProperty(DISPID_BACKCOLOR, VT_I4, propVal);
}

CString C_DXRTTicker::GetText()
{
	CString result;
	GetProperty(DISPID_TEXT, VT_BSTR, (void*)&result);
	return result;
}

void C_DXRTTicker::SetText(LPCTSTR propVal)
{
	SetProperty(DISPID_TEXT, VT_BSTR, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// C_DXRTTicker operations

void C_DXRTTicker::Clear()
{
	InvokeHelper(0xe, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

BOOL C_DXRTTicker::StartDataObject()
{
	BOOL result;
	InvokeHelper(0xf, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL C_DXRTTicker::StopDataObject()
{
	BOOL result;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL C_DXRTTicker::InitializeDataObject()
{
	BOOL result;
	InvokeHelper(0x11, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

void C_DXRTTicker::Tick(long ID, const VARIANT& Name, const VARIANT& Value)
{
	static BYTE parms[] =
		VTS_I4 VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ID, &Name, &Value);
}

BOOL C_DXRTTicker::DataObjectAdvise()
{
	BOOL result;
	InvokeHelper(0x13, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL C_DXRTTicker::DataObjectUnAdvise()
{
	BOOL result;
	InvokeHelper(0x14, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

void C_DXRTTicker::AboutBox()
{
	InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}
