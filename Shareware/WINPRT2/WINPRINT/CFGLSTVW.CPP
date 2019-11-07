// cfglstvw.cpp : implementation of the CConfigListView class
//

#include "stdafx.h"
#include "winprint.h"

#include "winprdoc.h"
#include "cntritem.h"
#include "cfglstvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigListView

IMPLEMENT_DYNCREATE(CConfigListView, CListBoxView)

BEGIN_MESSAGE_MAP(CConfigListView, CListBoxView)
    //{{AFX_MSG_MAP(CConfigListView)
    ON_WM_SETFOCUS()
    ON_WM_SIZE()
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
    ON_LBN_DBLCLK( 1, OnDblClk )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigListView construction/destruction

CConfigListView::CConfigListView()
{
    // TODO: add construction code here
}

CConfigListView::~CConfigListView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CConfigListView drawing

void CConfigListView::OnDraw(CDC* pDC)
{
    CWinPrintDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

}

void CConfigListView::OnInitialUpdate()
{
    CListBoxView::OnInitialUpdate();

    OnUpdate( this, updateReset, NULL ) ;
}

void CConfigListView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
    CWinPrintDoc* pDoc = GetDocument();
    if (lHint == updateReset)
    {
        POSITION pos ;
        
        m_lb.ResetContent() ;
        
        pos = pDoc->GetStartPosition();
        while (pos != NULL)
        {
            CWinPrintCntrItem* pDocItem = (CWinPrintCntrItem*)pDoc->GetNextItem(pos);
            ASSERT_VALID(pDocItem);
            if (pDocItem->m_itemType == CWinPrintCntrItem::typeConfiguration)
                m_lb.AddString( pDocItem->m_strConfigName ) ;
        }        
    }
}

void CConfigListView::OnSetFocus(CWnd* pOldWnd)
{
    CListBoxView::OnSetFocus(pOldWnd);
}

void CConfigListView::OnSize(UINT nType, int cx, int cy)
{
    CListBoxView::OnSize(nType, cx, cy);
}


/////////////////////////////////////////////////////////////////////////////
// CConfigListView diagnostics

#ifdef _DEBUG
void CConfigListView::AssertValid() const
{
    CListBoxView::AssertValid();
}

void CConfigListView::Dump(CDumpContext& dc) const
{
    CListBoxView::Dump(dc);
}

CWinPrintDoc* CConfigListView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinPrintDoc)));
    return (CWinPrintDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConfigListView message handlers

int CConfigListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CListBoxView::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_lb.SetFont( theApp.m_pFont ) ;    
    m_lb.SetWindowText("Configuration\tPrinter") ;
    
    return 0;
}

void CConfigListView::OnDblClk()
{
}
