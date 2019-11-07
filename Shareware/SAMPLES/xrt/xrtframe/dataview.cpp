// WOSA/XRT XRTFrame Sample Application Version 1.00.007
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// dataview.cpp : implementation of the CStocksView class
//

#include "stdafx.h"
#include "xrtframe.h"

#include "doc.h"
#include "dataview.h"
#include "dataitem.h"

#include "srvritem.h"
#include "property.h"
#include "properti.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataView

IMPLEMENT_DYNCREATE(CDataView, CView)
#define new DEBUG_NEW

BEGIN_MESSAGE_MAP(CDataView, CView)
    //{{AFX_MSG_MAP(CDataView)
    ON_WM_CREATE()
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    ON_WM_SIZE()
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_WM_TIMER()
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    //}}AFX_MSG_MAP
    ON_CONTROL( CLBN_COLUMNCHANGE, 42, OnColumnChange ) 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataView construction/destruction

CDataView::CDataView()
{
    m_fInitialUpdate = FALSE ;
    m_nIDTimerEvent = 0 ;
}

CDataView::~CDataView()
{
    if (m_nIDTimerEvent != 0 )
        KillTimer( m_nIDTimerEvent ) ;
}

/////////////////////////////////////////////////////////////////////////////
// CDataView drawing
void CDataView::OnInitialUpdate()
{
    m_fInitialUpdate = TRUE ;

    CDataObject* pDoc = GetDocument();

    CDC *pDC = GetDC() ;
    LOGFONT logfont = pDoc->m_logfont;
    logfont.lfHeight = -::MulDiv(-logfont.lfHeight,
        pDC->GetDeviceCaps(LOGPIXELSY), 72);
    m_font.CreateFontIndirect( &logfont ) ;
    m_lb.SetFont( &m_font, TRUE ) ;
    
    logfont.lfHeight = -::MulDiv(7, pDC->GetDeviceCaps(LOGPIXELSY), 72);
    lstrcpy(logfont.lfFaceName, "Small Fonts" ) ;
    
    m_smallfont.CreateFontIndirect( &logfont ) ;
    CFont* pFont = pDC->SelectObject( &m_smallfont ) ;
    TEXTMETRIC tm ;
    pDC->GetTextMetrics( &tm ) ;
    m_cySmallFont = tm.tmHeight ;
    pDC->SelectObject( pFont ) ;        
    ReleaseDC(pDC) ;                        

    COLINFO* pColInfo ;
    int nCols ;
    CString str ;
    pColInfo = pDoc->GetDataColInfo( &nCols, str ) ;
    m_lb.SetColumnsEx( nCols, pColInfo ) ;
    m_lb.SetWindowText( str ) ;

    OnUpdate( this, UPDATE_ALL, NULL ) ;
}

void CDataView::ChangeUpdateFrequencyTimer( UINT uiFreq )
{  
    if (m_nIDTimerEvent) KillTimer( m_nIDTimerEvent ) ;
    if (uiFreq != 0)
        m_nIDTimerEvent = SetTimer( m_nIDTimerEvent, uiFreq, NULL ) ;
}

void CDataView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
    CDataObject* pDoc = GetDocument();

    if (m_fInitialUpdate == FALSE)
    {
        m_fInitialUpdate = TRUE ;
        CDC *pDC = GetDC() ;
        LOGFONT logfont = pDoc->m_logfont;
        logfont.lfHeight = -::MulDiv(-logfont.lfHeight,
            pDC->GetDeviceCaps(LOGPIXELSY), 72);
        m_font.CreateFontIndirect( &logfont ) ;
        m_lb.SetFont( &m_font, TRUE ) ;
        
        logfont.lfHeight = -::MulDiv(7, pDC->GetDeviceCaps(LOGPIXELSY), 72);
        lstrcpy(logfont.lfFaceName, "Small Fonts" ) ;
    
        m_smallfont.CreateFontIndirect( &logfont ) ;
        CFont* pFont = pDC->SelectObject( &m_smallfont ) ;
        TEXTMETRIC tm ;
        pDC->GetTextMetrics( &tm ) ;
        m_cySmallFont = tm.tmHeight ;
        pDC->SelectObject( pFont ) ;        
        ReleaseDC(pDC) ;                        

        COLINFO* pColInfo ;
        int nCols ;
        CString str ;
        pColInfo = pDoc->GetDataColInfo( &nCols, str ) ;
        m_lb.SetColumnsEx( nCols, pColInfo ) ;
        m_lb.SetWindowText( str ) ;
    }  

    m_lb.SetRedraw( FALSE ) ;            

    POSITION pos;
    CDataItem* pItem ;
    UINT i, k ;
    // lHint of 0 means the reason for the change cannot
    // be easily quantified.  Removes have lHint = 0
    // lHint of 11 means an item's name or value has changed
    // lHint of 12 means an item has been added
    if (lHint == 11 && pHint != NULL)
    {                  
        // find the item in the oblist
        #if 0
        pos = pDoc->m_lstDataItems->Find( pHint ) ;
        if (pos != NULL)
        {
            // Invalidate just it's rectangle
        }
        else
        {
            TRACE("Could not find pHint in m_lstDataItems!" ) ;
        }    
        #endif
    }                    
    else if (lHint == 12 && pHint != NULL)
    {
        i = m_lb.AddString("") ;
        ASSERT (LB_ERR != i) ;
        m_lb.SetItemData( i, (DWORD)pHint) ;
    }
    else if (lHint == 13)
    {   
        // A CProperty changed
        //
                
    }
    else
    {
        UINT cItems = m_lb.GetCount() ;
    
        ASSERT( cItems != LB_ERR ) ;
        
        for( i = 0, pos = pDoc->m_lstDataItems.GetHeadPosition(); pos != NULL; )
        {
            pItem = (CDataItem*)pDoc->m_lstDataItems.GetNext( pos ) ;
                
            if (i >= cItems)
            {
                k = m_lb.AddString("") ;
                if (k != i)
                {
                    TRACE("Somethings wrong in OnUpdate") ;
                    break ;
                }
            }
            m_lb.SetItemData( i, (DWORD)pItem ) ;
            i++ ;
        }
             
        for (k = i ; k < cItems ; k++)
        {
            m_lb.DeleteString( k ) ;
        }
    }
  
    // TODO:  Make the invalidation smarter
    m_lb.Invalidate( FALSE ) ;   

    m_lb.SetRedraw( TRUE ) ;
}

void CDataView::SetSelection( CProperty* pProperty ) 
{
    UINT cItems = m_lb.GetCount() ;
    ASSERT( cItems != LB_ERR ) ;
    CDataItem*   pItem ;
    
    for (UINT i = 0 ; i < cItems ; i++)
    {
        pItem = (CDataItem*)m_lb.GetItemData( i ) ;
        if (pItem->m_pProperties->Find( pProperty ))
        {
            m_lb.SetCurSel( i ) ;
            return ;
        }
    }
}

void CDataView::SetSelection( CDataItem* pDataItem ) 
{
    UINT cItems = m_lb.GetCount() ;
    ASSERT( cItems != LB_ERR ) ;
    CDataItem*   pItem ;
    
    for (UINT i = 0 ; i < cItems ; i++)
    {
        pItem = (CDataItem*)m_lb.GetItemData( i ) ;
        if (pItem == pDataItem)
        {
            m_lb.SetCurSel( i ) ;
            return ;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// CDataView diagnostics

#ifdef _DEBUG
void CDataView::AssertValid() const
{
    CView::AssertValid();
}

void CDataView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CDataObject* CDataView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDataObject)));
    return (CDataObject*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDataView message handlers

int CDataView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    TRACE( "CDataView::OnCreate\r\n" ) ;
    
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rect ;
    GetClientRect( &rect ) ;

    DWORD dw = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
                                CLBS_LIKEBUTTON |
                                LBS_OWNERDRAWFIXED | 
                                LBS_HASSTRINGS |
                                LBS_NOTIFY |
                                LBS_DISABLENOSCROLL | 
                                LBS_NOINTEGRALHEIGHT ;
                          
    if (!m_lb.Create( dw, rect, this, 42 ))
    {
        TRACE( "Listbox failed to create!" ) ;
        return -1 ;
    }
    
    m_lb.SetLikeButtonStyle( TRUE ) ;
    
    CDataObject* pDoc = GetDocument();

    CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject( ANSI_VAR_FONT ) ) ;
    m_lb.SetFont( pFont ) ;

    COLINFO* pColInfo ;
    int nCols ;
    CString str ;
    pColInfo = pDoc->GetDataColInfo( &nCols, str ) ;
    m_lb.SetColumnsEx( nCols, pColInfo ) ;
    m_lb.SetWindowText( str ) ;
    
    CDC *pDC = GetDC() ;
    LOGFONT logfont ;
    pFont->GetObject( sizeof(LOGFONT), &logfont );

    logfont.lfHeight = -::MulDiv(7, pDC->GetDeviceCaps(LOGPIXELSY), 72);
    lstrcpy(logfont.lfFaceName, "Small Fonts" ) ;

    m_smallfont.CreateFontIndirect( &logfont ) ;
    pFont = pDC->SelectObject( &m_smallfont ) ;
    TEXTMETRIC tm ;
    pDC->GetTextMetrics( &tm ) ;
    m_cySmallFont = tm.tmHeight ;
    pDC->SelectObject( pFont ) ;
    m_smallfont.DeleteObject() ;

    ReleaseDC(pDC) ;

    return 0;
}

void CDataView::OnDraw( CDC* pDC )
{
    CFont* pFont = pDC->SelectObject( &m_smallfont ) ;
    pDC->SetBkColor( GetSysColor( COLOR_ACTIVECAPTION ) ) ;
    pDC->SetTextColor( GetSysColor( COLOR_CAPTIONTEXT ) ) ;

    CRect rc ;
    GetClientRect( &rc ) ;
    rc.bottom = m_cySmallFont ;

    char szReq[] = "  DataItem Objects" ;
        
//    CSize size = pDC->GetTextExtent( szReq, lstrlen(szReq) ) ;
//    pDC->ExtTextOut( (rc.right - rc.left - size.cx)/2, 0, ETO_OPAQUE, &rc, szReq, lstrlen(szReq), NULL ) ;
    pDC->ExtTextOut( 0, 0, ETO_OPAQUE, &rc, szReq, lstrlen(szReq), NULL ) ;
    pDC->SelectObject( pFont ) ;
} 


void CDataView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS)
{
    if (lpDIS->itemID == LB_ERR)
        return ;

    CDC             dc ;             
    COLORREF        rgbBack ;
    RECT            rcFocus ;
    BOOL            fSelected ;
    int             x, y, cy ;

    char            szItem[256] ;

    *szItem = '\0' ;

    CDataItem*          pItem ;
    if (NULL == (pItem = (CDataItem*)lpDIS->itemData))
        return ;

    int n, c ;
    VARIANT va ;
    VariantInit(&va) ;
    CProperties*    pProps = pItem->m_pProperties ;
    c = pProps->m_lstProps.GetCount() ;
    for( n = 0; n < c; )
    {
        if (SUCCEEDED(VariantChangeType( &va, &pItem->m_rgvarValue[n], 0, VT_BSTR )))
        {                        
            lstrcat( szItem, va.bstrVal ) ;
            VariantClear( &va ) ;
        }
        else
        {
            lstrcat( szItem, "<type!>" ) ;
        }
        
        if (++n != c )
            lstrcat( szItem, "\t" ) ;
    }

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

    m_lb.ColumnTextOut( &dc, x+2, y+cy, &rcFocus, CString(szItem) );
     
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


void CDataView::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    m_lb.MeasureItem( lpMeasureItemStruct ) ;
}

void CDataView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    if (m_lb.m_hWnd && IsWindow( m_lb.m_hWnd ))
    {
        m_lb.SetWindowPos( NULL, -1, m_cySmallFont, cx+2, cy- m_cySmallFont + 1, SWP_NOZORDER ) ;        

        CDataObject* pDoc = GetDocument();
        if (pDoc)
        {
            COLINFO* rgColInfo ;
            int nCols ;
            CString str ;
            
            nCols = m_lb.GetColumnsEx( &rgColInfo ) ;
            m_lb.GetWindowText( str ) ;
            pDoc->SetDataColInfo( nCols, rgColInfo, str, FALSE ) ;
        }
    }
}

void CDataView::OnColumnChange()
{
    CDataObject* pDoc = GetDocument();
    if (pDoc)
    {
        COLINFO* rgColInfo ;
        int nCols ;
        CString str ;
            
        nCols = m_lb.GetColumnsEx( &rgColInfo ) ;
        m_lb.GetWindowText( str ) ;
        pDoc->SetDataColInfo( nCols, rgColInfo, str, TRUE ) ;
    }
}

void CDataView::OnEditCopy()
{

    UINT i = m_lb.GetCurSel() ;
    if (i == LB_ERR)
        return ;

    CDataObject* pDoc = GetDocument();
    CDataItem* pDataItem = (CDataItem*)m_lb.GetItemData(i) ;
    ASSERT( pDataItem ) ;
    
    // get a server item suitable to generate the clipboard data
    CDataItemServerItem* pItem = pDataItem->m_pServerItem;
    if (pItem == NULL)
        pItem = new CDataItemServerItem(GetDocument(), pDataItem);
    ASSERT_VALID(pItem);

    pItem->CopyToClipboard(TRUE);
    
}

void CDataView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( LB_ERR != m_lb.GetCurSel() ) ;
}


void CDataView::OnTimer(UINT nIDEvent)
{
    CDataObject* pDoc = GetDocument();

    // Now's the time to send out all our data...
    POSITION pos = pDoc->GetStartPosition();
    COleServerItem* pItem;
    while ((pItem = pDoc->GetNextServerItem(pos)) != NULL)
    {
        if (pItem->IsKindOf(RUNTIME_CLASS(CDataItemServerItem)) ||
            pItem->IsKindOf(RUNTIME_CLASS(CDataObjectSvrItem)))
            pItem->OnUpdate(NULL, 0, NULL, DVASPECT_CONTENT ) ;
    }
    
    pDoc->ClearDataItems() ;

    CView::OnTimer(nIDEvent);
}

