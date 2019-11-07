// queview.cpp : implementation of the CQueueView class
//

#include "stdafx.h"
#include "winprint.h"

#include "winprdoc.h"
#include "cntritem.h"
#include "queview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQueueView

IMPLEMENT_DYNCREATE(CQueueView, CListBoxView)

BEGIN_MESSAGE_MAP(CQueueView, CListBoxView)
    //{{AFX_MSG_MAP(CQueueView)
    ON_WM_SETFOCUS()
    ON_WM_SIZE()
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueueView construction/destruction

CQueueView::CQueueView()
{
    // TODO: add construction code here
}

CQueueView::~CQueueView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CQueueView drawing

void CQueueView::OnDraw(CDC* pDC)
{
    CWinPrintDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

}

void CQueueView::OnInitialUpdate()
{
    CListBoxView::OnInitialUpdate();


}

void CQueueView::OnSetFocus(CWnd* pOldWnd)
{
    CListBoxView::OnSetFocus(pOldWnd);
}

void CQueueView::OnSize(UINT nType, int cx, int cy)
{
    CListBoxView::OnSize(nType, cx, cy);
}

/////////////////////////////////////////////////////////////////////////////
// CQueueView diagnostics

#ifdef _DEBUG
void CQueueView::AssertValid() const
{
    CListBoxView::AssertValid();
}

void CQueueView::Dump(CDumpContext& dc) const
{
    CListBoxView::Dump(dc);
}

CWinPrintDoc* CQueueView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinPrintDoc)));
    return (CWinPrintDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CQueueView message handlers

int CQueueView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CListBoxView::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    m_lb.SetFont( theApp.m_pFont ) ;    
    m_lb.SetWindowText("Filename\tPages\tPrinter\tConfiguration") ;
    
    return 0;
}
