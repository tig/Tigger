// IPersist.cpp : implementation file
//
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1993 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "iviewers.h"
#include "iview.h"
#include "iviewer.h"
#include "IPersist.h"
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPersistViewer

IMPLEMENT_DYNCREATE(CIPersistViewer, CInterfaceViewer)
#define new DEBUG_NEW

CIPersistViewer::CIPersistViewer()
{
}

CIPersistViewer::~CIPersistViewer()
{
}

BEGIN_MESSAGE_MAP(CIPersistViewer, CInterfaceViewer)
	//{{AFX_MSG_MAP(CIPersistViewer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// {7CE551EB-F85C-11ce-9059-080036F12502}
IMPLEMENT_OLECREATE(CIPersistViewer, "Ole2View IPersist Interface Viewers", 0x7ce551eb, 0xf85c, 0x11ce, 0x90, 0x59, 0x8, 0x0, 0x36, 0xf1, 0x25, 0x2) ;

/////////////////////////////////////////////////////////////////////////////
// CIPersistViewer View imp.

// 

HRESULT CIPersistViewer::OnView(HWND hwndParent, REFIID riid, LPUNKNOWN punk)
{
    HRESULT hr = NOERROR ;

	ASSERT(hwndParent);
    ASSERT(punk);

    IPersist* pp = NULL ;
    if (FAILED(hr = punk->QueryInterface(IID_IPersist, (void**)&pp)))
    {
        AfxMessageBox("QueryInterface for IID_IPersist failed!  This is bad.");
    }
    else if (pp == NULL)
    {
        AfxMessageBox("QueryInterface for IID_IPersist succeeded, but the returned pointer is NULL!  This is bad.");
    }
    
    CIPersistDlg* pdlg = NULL ;
    CLSID clsid ;
    if (punk && riid == IID_IPersistStream)
    {
        pdlg = new CIPersistStreamDlg(IDD_IPERSISTSTREAM,CWnd::FromHandle(hwndParent)) ;
    }
    else if (punk && (riid == IID_IPersistStreamInit || riid == IID_IPersistMemory))
    {
        pdlg = new CIPersistStreamDlg(IDD_IPERSISTSTREAMINIT,CWnd::FromHandle(hwndParent)) ;
    }
    else
        pdlg = new CIPersistDlg(IDD_IPERSIST,CWnd::FromHandle(hwndParent)) ;

    pdlg->m_punk = punk ;
    pdlg->m_iid = riid ;
    if (pp != NULL && SUCCEEDED(hr = pp->GetClassID(&clsid)))
    {
        OLECHAR   wcsCLSID[40] ;
        ::StringFromGUID2(clsid, wcsCLSID, 40);
        pdlg->m_strCLSID = wcsCLSID ;
        TCHAR  szName[256] ;
        TCHAR  szKey[256] ;
        LONG cb = sizeof(szName)/sizeof(TCHAR);
        wsprintf(szKey, _T("CLSID\\%s"), pdlg->m_strCLSID) ;
        if (RegQueryValue(HKEY_CLASSES_ROOT, szKey, szName, &cb) != ERROR_SUCCESS)
            lstrcpy(szName, _T("<no name in registry>")) ;
        pdlg->m_strName = szName ;
    }
    else
    {
        pdlg->m_strCLSID = _T("<IPersist::GetClassID failed>") ;
        pdlg->m_strName = _T("") ;
    }
    if (pp!=NULL)
        pp->Release();

    pdlg->DoModal() ;

    return hr ;
}

/////////////////////////////////////////////////////////////////////////////
// CIPersistDlg dialog
CIPersistDlg::CIPersistDlg(UINT uiID /*=0*/, CWnd* pParent /*=NULL*/)
	: CDialog(uiID, pParent)
{
    m_punk = NULL ;
    m_iid = GUID_NULL ;

	//{{AFX_DATA_INIT(CIPersistDlg)
	m_strCLSID = _T("");
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void CIPersistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPersistDlg)
	DDX_Text(pDX, IDC_CLSID, m_strCLSID);
	DDX_Text(pDX, IDC_CLASSNAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIPersistDlg, CDialog)
	//{{AFX_MSG_MAP(CIPersistDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPersistDlg message handlers
BOOL CIPersistDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    CenterWindow() ;
        
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CIPersistStreamDlg dialog
//
// This handy dandy dialog works for IPersistStream, IPersistStreamInit, and
// IPeristMemory.  Each of these interfaces inherits from IPerist, and have very
// similar vtable layouts.  We take advantage of this to use a common implementation
// for all.
//
CIPersistStreamDlg::CIPersistStreamDlg(UINT uiID /*=0*/, CWnd* pParent /*=NULL*/)
	: CIPersistDlg(uiID, pParent)
{
	//{{AFX_DATA_INIT(CIPersistStreamDlg)
	m_strIsDirty = _T("???");
	m_strSizeMax = _T("???");
	//}}AFX_DATA_INIT
}

void CIPersistStreamDlg::DoDataExchange(CDataExchange* pDX)
{
	CIPersistDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPersistStreamDlg)
	DDX_Text(pDX, IDC_ISDIRTYTXT, m_strIsDirty);
	DDX_Text(pDX, IDC_SIZEMAX, m_strSizeMax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIPersistStreamDlg, CDialog)
	//{{AFX_MSG_MAP(CIPersistStreamDlg)
	ON_BN_CLICKED(IDC_ISDIRTY, OnIsDirty)
	ON_BN_CLICKED(IDC_GETSIZEMAX, OnGetSizeMax)
	ON_BN_CLICKED(IDC_INITNEW, OnInitNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPersistStreamDlg message handlers
BOOL CIPersistStreamDlg::OnInitDialog() 
{
	CIPersistDlg::OnInitDialog();

    if (m_iid == IID_IPersistMemory)
        SetWindowText(_T("IPersistMemory Interface Viewer"));
        
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIPersistStreamDlg::OnIsDirty() 
{
    HRESULT hr = NOERROR ;
    IPersistStream* pips; 
    if (SUCCEEDED(hr = m_punk->QueryInterface(m_iid, (void**)&pips)))
    {
        pips->IsDirty() ?  m_strIsDirty = _T("TRUE") : m_strIsDirty = _T("FALSE");
        pips->Release() ;
        UpdateData(FALSE);
    }
    else
    {
        ErrorMessage( _T("QueryInterface failed for IID_IPersistStream[Init]"), hr );
    }
}

void CIPersistStreamDlg::OnGetSizeMax() 
{
    HRESULT hr = NOERROR ;
    IPersistStream* pips; 
    if (SUCCEEDED(hr = m_punk->QueryInterface(m_iid, (void**)&pips)))
    {
        ULARGE_INTEGER uli;
        if (FAILED(hr = pips->GetSizeMax(&uli)))
        {
            ErrorMessage( _T("IPersistStream::GetSizeMax failed"), hr );
        }
        else
        {
            m_strSizeMax.Format(_T("%d bytes"), uli.LowPart);
            UpdateData(FALSE);
        }
        pips->Release() ;
    }
    else
    {
        ErrorMessage( _T("QueryInterface failed for IID_IPersistStream[Init]"), hr );
    }
}

void CIPersistStreamDlg::OnInitNew() 
{
    ASSERT(m_iid == IID_IPersistStreamInit || m_iid == IID_IPersistMemory );

    HRESULT hr = NOERROR ;
    // IPersistStreamInit and IPersistMemory have the same vtable layout,
    // although the parameters to Save, Load, and GetSizeMax are different.
    // We can ignore that here because we just call InitNew which is the
    // same for both!
    IPersistStreamInit* pips; 
    if (SUCCEEDED(hr = m_punk->QueryInterface(m_iid, (void**)&pips)))
    {
        if (FAILED(hr = pips->InitNew()))
        {
            if (m_iid == IID_IPersistStreamInit)
                ErrorMessage( _T("IPersistStreamInit::InitNew failed"), hr );
            else
                ErrorMessage( _T("IPersistMemory::InitNew failed"), hr );

        }
        pips->Release() ;
    }
    else
    {
        if (m_iid == IID_IPersistStreamInit)
            ErrorMessage( _T("QueryInterface failed for IID_IPersistStreamInit"), hr );
        else
            ErrorMessage( _T("QueryInterface failed for IID_IPersistMemory"), hr );
    }
}
