// stockvw.cpp : implementation of the CStocksView class
//

#include "stdafx.h"
#include "stocks.h"

#include "stockdoc.h"
#include "stockvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStocksView

IMPLEMENT_DYNCREATE(CStocksView, CView)

BEGIN_MESSAGE_MAP(CStocksView, CView)
	//{{AFX_MSG_MAP(CStocksView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStocksView construction/destruction

CStocksView::CStocksView()
{
	// TODO: add construction code here
}

CStocksView::~CStocksView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CStocksView drawing

void CStocksView::OnDraw(CDC* pDC)
{
	CStocksDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CStocksView diagnostics

#ifdef _DEBUG
void CStocksView::AssertValid() const
{
	CView::AssertValid();
}

void CStocksView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CStocksDoc* CStocksView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CStocksDoc)));
	return (CStocksDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStocksView message handlers
