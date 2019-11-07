// view.cpp
//
// Implements CListBoxView which is an override of CView with appropriate
// owner draw listbox and COleDropTarget support.
//
// CConfigListView and CQueueView are derived from this class
//


#include "stdafx.h"
#include "view.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListBoxView

IMPLEMENT_DYNAMIC(CListBoxView, CView)

BEGIN_MESSAGE_MAP(CListBoxView, CView)
    //{{AFX_MSG_MAP(CListBoxView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxView construction/destruction

CListBoxView::CListBoxView()
{
}

int CListBoxView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

//    COLINFO* pColInfo ;
//    int nCols ;
//    CString str ;

    CRect rect ;
    GetClientRect( &rect ) ;
    DWORD dw = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
                                CLBS_LIKEBUTTON |
                                LBS_OWNERDRAWFIXED | 
                                LBS_HASSTRINGS |
                                LBS_NOTIFY |
                                LBS_DISABLENOSCROLL | 
                                LBS_NOINTEGRALHEIGHT ;

    // Create the Requests listbox
    //                          
    if (!m_lb.Create( dw, rect, this, 1 ))
    {
        TRACE( "Listbox failed to create!" ) ;
        return -1 ;
    }

    //m_lb.SetLikeButtonStyle( TRUE ) ;
    //m_lb.SetFont( theApp.m_pFont ) ;

    // pColInfo = ???
    // m_lb.SetColumnsEx( nCols, pColInfo ) ;
    // m_lb.SetWindowText( str ) ;
    
    m_dropTarget.Register( this ) ;
    return 0;
}

void CListBoxView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS)
{
    CDC             dc ;             
    COLORREF        rgbBack ;
    RECT            rcFocus ;
    BOOL            fSelected ;
    int             x, y, cy ;
    CString         strItem ;

    if (lpDIS->itemID == LB_ERR)
        return ;                           
        
    m_lb.GetText( lpDIS->itemID, strItem ) ;
                         
    dc.Attach( lpDIS->hDC ) ;
    
    rcFocus = lpDIS->rcItem ;    
    if (fSelected = (lpDIS->itemState & ODS_SELECTED) ? TRUE : FALSE)
    {
        dc.SetTextColor( GetSysColor( COLOR_HIGHLIGHTTEXT ) ) ;
        dc.SetBkColor( rgbBack = GetSysColor( COLOR_HIGHLIGHT ) ) ;
    }
    else
    {
        dc.SetTextColor( GetSysColor( COLOR_WINDOWTEXT ) ) ;
        dc.SetBkColor( rgbBack = GetSysColor( COLOR_WINDOW ) ) ;
    }
    
    // if we are loosing focus, remove the focus rect before
    // drawing.
    //
    if ((lpDIS->itemAction) & (ODA_FOCUS))
        if (!((lpDIS->itemState) & (ODS_FOCUS)))
            dc.DrawFocusRect( &rcFocus ) ;
    
    y = lpDIS->rcItem.top ;
    x = lpDIS->rcItem.left ;
    cy = 0 ; //(rcFocus.bottom - rcFocus.top - m_cyFont) / 2 ;

    dc.ExtTextOut( x, y, ETO_OPAQUE, &lpDIS->rcItem, NULL, 0, NULL ) ;
    m_lb.ColumnTextOut( &dc, x+2, y+cy, &rcFocus, strItem );

    // if we are gaining focus draw the focus rect after drawing
    // the text.
    if ((lpDIS->itemAction) & (ODA_FOCUS))
        if ((lpDIS->itemState) & (ODS_FOCUS))
             dc.DrawFocusRect( &rcFocus ) ;
    
    if (fSelected)
    {
        dc.SetTextColor( GetSysColor( COLOR_WINDOWTEXT ) ) ;
        dc.SetBkColor( GetSysColor( COLOR_WINDOW ) ) ;
    }

    dc.Detach() ;
} 

void CListBoxView::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMIS)
{
    m_lb.MeasureItem( lpMIS ) ;
    lpMIS->itemHeight += 1 ;
}

void CListBoxView::OnSize(UINT nType, int cx, int cy)
{   
    m_lb.SetWindowPos( NULL, -1, -1, cx+2, cy+2, SWP_NOZORDER ) ;        
}

/////////////////////////////////////////////////////////////////
// DragDrop code
//
DROPEFFECT CListBoxView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
    return OnDragOver(pDataObject, dwKeyState, point);
}


DROPEFFECT CListBoxView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{   
    DROPEFFECT de = DROPEFFECT_NONE;

    // check for force link
    if ((dwKeyState & (MK_CONTROL|MK_SHIFT)) == (MK_CONTROL|MK_SHIFT))
        de = DROPEFFECT_LINK;
    // check for force copy
    else if ((dwKeyState & MK_CONTROL) == MK_CONTROL)
        de = DROPEFFECT_COPY;
    // check for force move
    else if ((dwKeyState & MK_ALT) == MK_ALT)
        de = DROPEFFECT_MOVE;
    // default -- recommended action is move
    else
        de = DROPEFFECT_MOVE;

    return de;
}
 
void CListBoxView::OnDragLeave()
{
    CView::OnDragLeave() ;
}

BOOL CListBoxView::OnDrop(COleDataObject* pDataObject,DROPEFFECT dropEffect, CPoint point)
{
/*
    LPDATAOBJECT    pDO = pDataObject->m_lpDataObject ;

    pDO->AddRef() ;
    IDisplayInterface( theApp.m_pMainWnd->GetSafeHwnd(),
                       pDO, 
                       (LPIID)&IID_IDataObject, 
                       (LPTSTR)(LPCSTR)"Drag and Drop data object" ) ;
    pDO->Release() ;
*/

    return FALSE ;
}


