// winprvw.cpp : implementation of the CWinPrintView class
//

#include "stdafx.h"
#include "winprint.h"

#include "winprdoc.h"
#include "winprvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinPrintView

IMPLEMENT_DYNCREATE(CWinPrintView, CView)

BEGIN_MESSAGE_MAP(CWinPrintView, CView)
	//{{AFX_MSG_MAP(CWinPrintView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinPrintView construction/destruction

CWinPrintView::CWinPrintView()
{
	// TODO: add construction code here
}

CWinPrintView::~CWinPrintView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWinPrintView drawing

void CWinPrintView::OnDraw(CDC* pDC)
{
	CWinPrintDoc* pDoc = GetDocument();

	// TODO: add draw code here
}



/////////////////////////////////////////////////////////////////////////////
// CWinPrintView diagnostics

#ifdef _DEBUG
void CWinPrintView::AssertValid() const
{
	CView::AssertValid();
}

void CWinPrintView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWinPrintDoc* CWinPrintView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinPrintDoc)));
	return (CWinPrintDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinPrintView message handlers
