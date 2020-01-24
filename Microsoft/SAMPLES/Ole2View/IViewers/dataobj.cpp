// dataobj.cpp : implementation file
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
#include "dataobj.h"
#include "util.h"
#include "UniformDataTransfer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataObjectViewer

IMPLEMENT_DYNCREATE(CDataObjectViewer, CInterfaceViewer)
#define new DEBUG_NEW

CDataObjectViewer::CDataObjectViewer()
{
}

CDataObjectViewer::~CDataObjectViewer()
{
}

BEGIN_MESSAGE_MAP(CDataObjectViewer, CInterfaceViewer)
	//{{AFX_MSG_MAP(CDataObjectViewer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_OLECREATE(CDataObjectViewer, "Ole2View UDT Interface Viewers", 0x28d8aba0, 0x4b78, 0x11ce, 0xb2, 0x7d, 0x0, 0xaa, 0x0, 0x1f, 0x73, 0xc1)

/////////////////////////////////////////////////////////////////////////////
// CDataObjectViewer View imp.

HRESULT CDataObjectViewer::OnView(HWND hwndParent, REFIID riid, LPUNKNOWN punk)
{
    SCODE sc = NOERROR ;

	ASSERT(hwndParent);
    ASSERT(punk);

    if (riid != IID_IDataObject)
    {
        AfxMessageBox( _T("IDataObject interface viewer only supports IID_IDataObject") ) ;
        return E_INVALIDARG ;
    }

    IDataObject* pdataobj = NULL ;
    sc = punk->QueryInterface( riid, (void**)&pdataobj ) ;
    if (FAILED(sc))
    {
        AfxMessageBox( _T("Object does not support IDataObject") ) ;
        return E_UNEXPECTED ;
    }
    
    CUniformDataTransfer* pdlg ;
    pdlg = new CUniformDataTransfer( CWnd::FromHandle(hwndParent), pdataobj, riid, _T("") ) ;
    ASSERT(pdlg);
    if (pdlg)
    {
        pdlg->DoModal() ;
        delete pdlg ;
    }
    
    pdataobj->Release() ;

    return sc ;
}

