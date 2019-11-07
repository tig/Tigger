// IUnknown.cpp : implementation file
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
#include "IUnknown.h"
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIUnknownViewer

IMPLEMENT_DYNCREATE(CIUnknownViewer, CInterfaceViewer)
#define new DEBUG_NEW

CIUnknownViewer::CIUnknownViewer()
{
}

CIUnknownViewer::~CIUnknownViewer()
{
}

BEGIN_MESSAGE_MAP(CIUnknownViewer, CInterfaceViewer)
	//{{AFX_MSG_MAP(CIUnknownViewer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// {7CE551EA-F85C-11ce-9059-080036F12502}
IMPLEMENT_OLECREATE(CIUnknownViewer, "Ole2View Default Interface Viewer", 0x7ce551ea, 0xf85c, 0x11ce, 0x90, 0x59, 0x8, 0x0, 0x36, 0xf1, 0x25, 0x2) ;

/////////////////////////////////////////////////////////////////////////////
// CIUnknownViewer View imp.

// 

HRESULT CIUnknownViewer::OnView(HWND hwndParent, REFIID riid, LPUNKNOWN punk)
{
    SCODE sc = NOERROR ;

	ASSERT(hwndParent);
//    ASSERT(punk);

    CIUnknownDlg dlg(CWnd::FromHandle(hwndParent)) ;
    dlg.m_punk = punk ;
    dlg.m_iid = riid ;

    OLECHAR   wcsIID[40] ;
    ::StringFromGUID2(riid, wcsIID, 40);
    dlg.m_strIID = wcsIID ;

    TCHAR  szName[256] ;
    TCHAR  szKey[256] ;
    LONG cb = sizeof(szName)/sizeof(TCHAR);
    wsprintf(szKey, _T("Interface\\%s"), dlg.m_strIID) ;
    if (RegQueryValue(HKEY_CLASSES_ROOT, szKey, szName, &cb) != ERROR_SUCCESS)
        lstrcpy(szName, _T("<no name in registry>")) ;
    dlg.m_strName = szName ;

    dlg.DoModal() ;

    return sc ;
}
/////////////////////////////////////////////////////////////////////////////
// CIUnknownDlg dialog


CIUnknownDlg::CIUnknownDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIUnknownDlg::IDD, pParent)
{
    m_punk = NULL ;
    m_iid = GUID_NULL ;

	//{{AFX_DATA_INIT(CIUnknownDlg)
	m_strIID = _T("");
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void CIUnknownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIUnknownDlg)
	DDX_Control(pDX, IDC_VIEWTYPEINFO, m_btnViewTypeInfo);
	DDX_Text(pDX, IDC_IID, m_strIID);
	DDX_Text(pDX, IDC_INTERFACENAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIUnknownDlg, CDialog)
	//{{AFX_MSG_MAP(CIUnknownDlg)
	ON_BN_CLICKED(IDC_VIEWTYPEINFO, OnViewTypeInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIUnknownDlg message handlers

BOOL CIUnknownDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    CenterWindow() ;

    // Check to see if this interface has a TypeLib associated with it.
    // If it does, then enable the View Type Info button.
    //
    CString str("Interface\\") ;
    str += m_strIID + _T("\\TypeLib");
    LONG cb = 0;
    if (RegQueryValue(HKEY_CLASSES_ROOT, str, NULL, &cb) == ERROR_SUCCESS)
        m_btnViewTypeInfo.EnableWindow(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIUnknownDlg::OnViewTypeInfo() 
{
    USES_CONVERSION;
    GUID tlibid ;
    TCHAR szid[40] ;
    CString str("Interface\\") ;
    str += m_strIID + _T("\\TypeLib");
    LONG cb = sizeof(szid)/sizeof(TCHAR);
    if (RegQueryValue(HKEY_CLASSES_ROOT, str, szid, &cb) != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("Could not read TypeLib key in registry."));
        return ;
    }

    ::CLSIDFromString(T2OLE(szid), &tlibid);

    HRESULT hr;
    ITypeLib* ptlb = NULL ;
    LCID      lcid = GetUserDefaultLCID() ;
    SHORT     wVerMajor = -1;
    while (FAILED(hr = LoadRegTypeLib(tlibid, wVerMajor, 0, lcid, &ptlb)))
    {
        if (wVerMajor == -1)
            wVerMajor = 64 ;

        if (wVerMajor == 0)
        {
            TCHAR szTemp[256] ;
            wsprintf(szTemp, _T("LoadRegTypeLib(%s, [-1 through 64], %u, %lu, ...) failed."),
                        szid, 0, lcid);
            ErrorMessage( szTemp, hr ) ;
            return ;
        }
        wVerMajor--;
    }

    ASSERT(ptlb);

    // Find our IID coclass...
    //
    ITypeInfo* pti = NULL ;
    if (SUCCEEDED(hr = ptlb->GetTypeInfoOfGuid(m_iid, &pti)))
    {
        IInterfaceViewer* pview = NULL ;
        hr = CoCreateInstance(CLSID_ITypeLibViewer, NULL, CLSCTX_SERVER, IID_IInterfaceViewer, (void**)&pview);
        if (SUCCEEDED(hr))
        {
            pview->View(GetSafeHwnd(), IID_ITypeInfo, pti) ;
            pview->Release() ;
        }
    
        pti->Release();
    }
    else
    {
        ErrorMessage(_T("ITypeLib::GetTypeInfoOfGuid failed."), hr ) ;
    }

	ptlb->Release();
}
