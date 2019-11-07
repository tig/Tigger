// cfgview.cpp : implementation of the CConfigView class
//

#include "stdafx.h"
#include "winprint.h"

#include "winprdoc.h"
#include "cntritem.h"
#include "cfgview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigView

IMPLEMENT_DYNCREATE(CConfigView, CView)

BEGIN_MESSAGE_MAP(CConfigView, CView)
    //{{AFX_MSG_MAP(CConfigView)
    ON_WM_SETFOCUS()
    ON_WM_SIZE()
    ON_COMMAND(ID_CANCEL_EDIT, OnCancelEdit)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_SETCURSOR()
    ON_WM_RBUTTONDOWN()
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigView construction/destruction

CConfigView::CConfigView()
{
    m_pItem = NULL ;
}

CConfigView::~CConfigView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CConfigView drawing

void CConfigView::OnDraw(CDC* pDC)
{
    CWinPrintDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    ASSERT(m_pItem) ;    
    if (m_pItem)
    {        
        CSize size;
        CRect rect ;
        if (m_pItem->GetExtent(&size))
        {
            // OLE returns the extent in HIMETRIC units -- we need pixels
            CClientDC dc(NULL);
            dc.HIMETRICtoDP(&size);
        }
        rect.top = rect.left = 0 ;
        rect.bottom = size.cy ;
        rect.right = size.cx ;
        m_pItem->Draw(pDC, rect);
    }
}

void CConfigView::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    CWinPrintDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    CString strTitle ;
    GetParent()->GetWindowText( strTitle ) ;
    CWinPrintCntrItem* pItem = NULL ;
    POSITION pos = pDoc->GetStartPosition();
    while (pos)
    {
        pItem = (CWinPrintCntrItem*)pDoc->GetNextItem(pos);
        if (pItem && pItem->m_strConfigName == strTitle)
        {
            ASSERT(pItem->m_pView == NULL) ;
            ASSERT(m_pItem == NULL) ;
            
            m_pItem = pItem ;
            break ;
        }
    }
    
    ASSERT( m_pItem ) ;
    
    if (m_pItem)
    {
        m_pItem->DoVerb(GetKeyState(VK_CONTROL) < 0 ?
            OLEIVERB_PRIMARY : OLEIVERB_INPLACEACTIVATE , this);
    
        COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
        ASSERT(pActiveItem == m_pItem) ;
        
//        if (pActiveItem != NULL)
//        {
//            CRect rc ;
//            GetClientRect(&rc) ;
//            pActiveItem->SetItemRects(&rc,&rc);
//        }
        
        GetClientRect(&m_pItem->m_rect) ;
        CSize size = m_pItem->m_rect.Size() ;
        CClientDC dc(NULL);
        dc.DPtoHIMETRIC(&size);
        m_pItem->SetExtent( size ) ;

        GetParent()->SetWindowPos( m_pItem->m_fTopMost ? &wndTopMost : &wndNoTopMost, 0,0,0,0, 
            SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE ) ;
    }
}

void CConfigView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    switch (lHint)
    {
    case CWinPrintDoc::updateWindow:    // invalidate entire window
        Invalidate();
        break;
    case CWinPrintDoc::updateItem:      // invalidate single item
        {
            CRect rect;
            GetClientRect(&rect) ;
            InvalidateRect(rect);
        }
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CConfigView printing

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CConfigView::IsSelected(const CObject* pDocItem) const
{
    return pDocItem == m_pItem;
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.
void CConfigView::OnCancelEdit()
{
    // Close any in-place active item on this view.
    COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
    if (pActiveItem != NULL)
    {
        pActiveItem->Close();
    }
    ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}

// Special handling of OnSetFocus and OnSize are required for a container
//  when an object is being edited in-place.
void CConfigView::OnSetFocus(CWnd* pOldWnd)
{
    COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
    if (pActiveItem != NULL &&
        (pActiveItem->GetItemState() == COleClientItem::activeUIState ||
         pActiveItem->GetItemState() == COleClientItem::activeState))
    {
        // need to set focus to this item if it is in the same view
        CWnd* pWnd = pActiveItem->GetInPlaceWindow();
        if (pWnd != NULL)
        {
            pWnd->SetFocus();   // don't call the base class
            return;
        }
    }

    CView::OnSetFocus(pOldWnd);
}

void CConfigView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    if (m_pItem)
    {
        GetClientRect(&m_pItem->m_rect) ;
        CSize size = m_pItem->m_rect.Size() ;
        CClientDC dc(NULL);
        dc.DPtoHIMETRIC(&size);
        m_pItem->SetExtent( size ) ;
    }
    
    COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
    if (pActiveItem != NULL)
    {
        CRect rc ;
        GetClientRect(&rc) ;
        pActiveItem->SetItemRects(&rc,&rc);
    }
}

void CConfigView::SetItem(CWinPrintCntrItem* pItem)
{
    // close in-place active item
    if (pItem == NULL || m_pItem != pItem)
    {
        COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
        if (pActiveItem != NULL && pActiveItem != pItem)
            pActiveItem->Close();
    }
    // update view to new selection
    if (m_pItem != pItem)
    {
        if (m_pItem != NULL)
            OnUpdate(NULL, CWinPrintDoc::updateItem, m_pItem);

        m_pItem = pItem;
        if (m_pItem != NULL)
            OnUpdate(NULL, CWinPrintDoc::updateItem, m_pItem);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CConfigView diagnostics

#ifdef _DEBUG
void CConfigView::AssertValid() const
{
    CView::AssertValid();
}

void CConfigView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CWinPrintDoc* CConfigView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinPrintDoc)));
    return (CWinPrintDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CConfigView message handlers

void CConfigView::OnLButtonDown(UINT nFlags, CPoint point)
{

    CView::OnLButtonDown(nFlags, point);
}

void CConfigView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    OnLButtonDown(nFlags, point);

    if (m_pItem != NULL)
    {
        m_pItem->DoVerb(GetKeyState(VK_CONTROL) < 0 ?
            OLEIVERB_PRIMARY : OLEIVERB_INPLACEACTIVATE , this);
    }

    COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
    if (pActiveItem != NULL)
    {
        CRect rc ;
        GetClientRect(&rc) ;
        pActiveItem->SetItemRects(&rc,&rc);
    }

    CView::OnLButtonDblClk(nFlags, point);
}

BOOL CConfigView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (pWnd == this && m_pItem != NULL)
    {
    }

    return CView::OnSetCursor(pWnd, nHitTest, message);
}


void CConfigView::OnRButtonDown(UINT nFlags, CPoint point) 
{
    MessageBeep(0);
    
    CView::OnRButtonDown(nFlags, point);
}


void CConfigView::OnDestroy() 
{
    if (m_pItem)
    {
        m_pItem->m_windowPlacement.length = sizeof(WINDOWPLACEMENT) ;
        GetParent()->GetWindowPlacement( &m_pItem->m_windowPlacement ) ;
        
        if (m_pItem->IsInPlaceActive())
        {
            m_pItem->Deactivate() ;
        }            
    }
    
    CView::OnDestroy();
}

