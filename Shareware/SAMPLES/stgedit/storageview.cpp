// StorageView.cpp : implementation file
//

#include "stdafx.h"
#include "stgedit.h"
#include "StorageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStorageView

IMPLEMENT_DYNCREATE(CStorageView, CFormView)

CStorageView::CStorageView()
	: CFormView(CStorageView::IDD)
{
	//{{AFX_DATA_INIT(CStorageView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CStorageView::~CStorageView()
{
}

void CStorageView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStorageView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStorageView, CFormView)
	//{{AFX_MSG_MAP(CStorageView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStorageView diagnostics

#ifdef _DEBUG
void CStorageView::AssertValid() const
{
	CFormView::AssertValid();
}

void CStorageView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStorageView message handlers
