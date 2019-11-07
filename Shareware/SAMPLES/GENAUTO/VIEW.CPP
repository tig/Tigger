// view.cpp : implementation of the CGenAutoView class
//

#include "stdafx.h"
#include "genauto.h"

#include "doc.h"
#include "view.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenAutoView

IMPLEMENT_DYNCREATE(CGenAutoView, CView)
#define new DEBUG_NEW

BEGIN_MESSAGE_MAP(CGenAutoView, CView)
    //{{AFX_MSG_MAP(CGenAutoView)
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenAutoView construction/destruction

CGenAutoView::CGenAutoView()
{
    m_fInitialUpdate = FALSE ;
}

CGenAutoView::~CGenAutoView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGenAutoView drawing

void CGenAutoView::OnDraw(CDC* pDC)
{
    CGenAutoDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    pDC->TextOut( 10, 10, "View has no implementation" ) ;
}

void CGenAutoView::OnInitialUpdate()
{
    m_fInitialUpdate = TRUE ;

    CGenAutoDoc* pDoc = GetDocument();

    CDC *pDC = GetDC() ;
    LOGFONT logfont = pDoc->m_logfont;
    logfont.lfHeight = -::MulDiv(-logfont.lfHeight,
        pDC->GetDeviceCaps(LOGPIXELSY), 72);
    ReleaseDC(pDC) ;                        

    OnUpdate( this, 0, NULL ) ;
}

void CGenAutoView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
    CGenAutoDoc* pDoc = GetDocument();

    if (m_fInitialUpdate == FALSE)
    {
        m_fInitialUpdate = TRUE ;
        CDC *pDC = GetDC() ;
        LOGFONT logfont = pDoc->m_logfont;
        logfont.lfHeight = -::MulDiv(-logfont.lfHeight,
            pDC->GetDeviceCaps(LOGPIXELSY), 72);
        ReleaseDC(pDC) ;                        
    }  

}


/////////////////////////////////////////////////////////////////////////////
// CGenAutoView diagnostics

#ifdef _DEBUG
void CGenAutoView::AssertValid() const
{
    CView::AssertValid();
}

void CGenAutoView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CGenAutoDoc* CGenAutoView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGenAutoDoc)));
    return (CGenAutoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGenAutoView message handlers

int CGenAutoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    TRACE( "CGenAutoView::OnCreate\r\n" ) ;
    
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

