// autodvw.cpp : implementation of the CAutoDudeView class
//

#include "stdafx.h"
#include "autodude.h"

#include "autoddoc.h"
#include "autodvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoDudeView

IMPLEMENT_DYNCREATE(CAutoDudeView, CView)

BEGIN_MESSAGE_MAP(CAutoDudeView, CView)
    //{{AFX_MSG_MAP(CAutoDudeView)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoDudeView construction/destruction

CAutoDudeView::CAutoDudeView()
{
    // TODO: add construction code here
}

CAutoDudeView::~CAutoDudeView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CAutoDudeView drawing

void CAutoDudeView::OnDraw(CDC* pDC)
{
    CAutoDudeDoc* pDoc = GetDocument();

    // TODO: add draw code here
    pDC->TextOut( 10, 10, pDoc->GetText() ) ;
}



/////////////////////////////////////////////////////////////////////////////
// CAutoDudeView diagnostics

#ifdef _DEBUG
void CAutoDudeView::AssertValid() const
{
    CView::AssertValid();
}

void CAutoDudeView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CAutoDudeDoc* CAutoDudeView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAutoDudeDoc)));
    return (CAutoDudeDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAutoDudeView message handlers
