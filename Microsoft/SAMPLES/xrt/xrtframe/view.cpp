// WOSA/XRT XRTFrame Sample Application Version 1.00.007
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// view.cpp : implementation of the CStocksView class
//
// The view is setup like this:
//
// +----------------------------------------------+
// | Listbox of Request Objects                   |
// |                                              |
// |     struct m_Requests                        |
// |                                              |
// |                                              |
// |                                              |
// +=============== Splitter =====================+
// | Listbox of DataItem Objects                  |
// |                                              |
// |     struct m_Data                            |
// |                                              |
// |                                              |
// |                                              |
// +----------------------------------------------+
//

#include "stdafx.h"
#include "xrtframe.h"

#include "doc.h"
#include "view.h"

#include "srvritem.h"
#include "request.h"
#include "properti.h"
#include "property.h"
#include "dataitem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataObjectView

IMPLEMENT_DYNCREATE(CDataObjectView, CView)
#define new DEBUG_NEW

#define IDLB_REQUESTS   42
#define IDLB_DATA       43

BEGIN_MESSAGE_MAP(CDataObjectView, CView)
    //{{AFX_MSG_MAP(CDataObjectView)
    ON_WM_CREATE()
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    ON_WM_SIZE()
    ON_COMMAND(ID_EDIT_REMOVEITEM, OnRemoveItem)
    ON_UPDATE_COMMAND_UI(ID_EDIT_REMOVEITEM, OnUpdateRemoveItem)
    ON_COMMAND(ID_EDIT_CHANGEITEM, OnChangeItem)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CHANGEITEM, OnUpdateChangeItem)
    ON_COMMAND(ID_EDIT_ADDITEM, OnAddItem)
    ON_UPDATE_COMMAND_UI(ID_EDIT_ADDITEM, OnUpdateAddItem)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_COMMAND(ID_OPTIONS_PROPERTIES, OnOptionsProperties)
    //}}AFX_MSG_MAP
    ON_LBN_DBLCLK( IDLB_REQUESTS, OnDblClkRequests )
    ON_CONTROL( CLBN_COLUMNCHANGE, IDLB_REQUESTS, OnColumnChangeRequests ) 
    ON_CONTROL( CLBN_COLUMNCHANGE, IDLB_DATA, OnColumnChangeData ) 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataObjectView construction/destruction

CDataObjectView::CDataObjectView()
{
    m_fInitialUpdate = FALSE ;
}
 
CDataObjectView::~CDataObjectView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDataObjectView setup stuff
void CDataObjectView::DoInitialUpdate()
{
    CDataObject* pDoc = GetDocument();

    m_fInitialUpdate = TRUE ;
    CDC *pDC = GetDC() ;
    LOGFONT logfont = pDoc->m_logfont;
    logfont.lfHeight = -::MulDiv(-logfont.lfHeight,
        pDC->GetDeviceCaps(LOGPIXELSY), 72);
   
    if (m_font.GetSafeHandle() != NULL)
        m_font.DeleteObject() ;
         
    m_font.CreateFontIndirect( &logfont ) ;
    m_Requests.lb.SetFont( &m_font, TRUE ) ;
    m_Data.lb.SetFont( &m_font, TRUE ) ;
    
    logfont.lfHeight = -::MulDiv(7, pDC->GetDeviceCaps(LOGPIXELSY), 72);
    lstrcpy(logfont.lfFaceName, "Small Fonts" ) ;

    ReleaseDC(pDC) ;                        
    
    COLINFO* pColInfo ;
    int nCols ;
    CString str ;
    
    // Setup the columns for the Requests list
    pColInfo = pDoc->GetRequestsColInfo( &nCols, str ) ;
    m_Requests.lb.SetColumnsEx( nCols, pColInfo ) ;
    m_Requests.lb.SetWindowText( str ) ;

    // Setup the columns for the DataItem list
    pColInfo = pDoc->GetDataColInfo( &nCols, str ) ;
    m_Data.lb.SetColumnsEx( nCols, pColInfo ) ;
    m_Data.lb.SetWindowText( str ) ;

}
 
void CDataObjectView::OnInitialUpdate()
{
    DoInitialUpdate() ;
    OnUpdate( this, UPDATE_ALL, NULL ) ;
}

void CDataObjectView::DoUpdateRequests( CView* pSender, LPARAM lHint, CObject* pHint )
{
    if (lHint == UPDATE_MARKETCHANGE || lHint == UPDATE_ADDDATAITEM)
        return ;

    CDataObject* pDoc = GetDocument();

    m_Requests.lb.SetRedraw( FALSE ) ;            
    
    POSITION pos;
    CRequest* pItem ;
    UINT i, k ;
    // lHint of UPDATE_ALL means the reason for the change cannot
    // be easily quantified.  
    if (lHint == UPDATE_CHANGEREQUEST && pHint != NULL)
    {                  
        // find the item in the oblist
        #if 0
        pos = pDoc->m_lstRequests->Find( pHint ) ;
        if (pos != NULL)
        {
            // Invalidate just it's rectangle
        }
        else
        {
            TRACE("Could not find pHint in m_lstRequests!" ) ;
        }    
        #endif
    }                    
    else if (lHint == UPDATE_ADDREQUEST && pHint != NULL)
    {
        i = m_Requests.lb.AddString("") ;
        ASSERT (LB_ERR != i) ;
        m_Requests.lb.SetItemData( i, (DWORD)pHint) ;
    }
    else if (lHint == UPDATE_CHANGEPROP)
    {   
        // A CProperty changed
        //
                
    }
    else
    {
        UINT cItems = m_Requests.lb.GetCount() ;
    
        ASSERT( cItems != LB_ERR ) ;
        
        for( i = 0, pos = pDoc->m_lstRequests.GetHeadPosition(); pos != NULL; )
        {
            pItem = (CRequest*)pDoc->m_lstRequests.GetNext( pos ) ;
                
            if (i >= cItems)
            {
                k = m_Requests.lb.AddString("") ;
                if (k != i)
                {
                    TRACE("Somethings wrong in OnUpdateRequests") ;
                    break ;
                }
            }
            m_Requests.lb.SetItemData( i, (DWORD)pItem ) ;
            i++ ;
        }
             
        for (k = i ; k < cItems ; k++)
        {
            m_Requests.lb.DeleteString( k ) ;
        }
    }
  
    // TODO:  Make the invalidation smarter
    m_Requests.lb.Invalidate( FALSE ) ;   
    m_Requests.lb.SetRedraw( TRUE ) ;
}

void CDataObjectView::DoUpdateData( CView* pSender, LPARAM lHint, CObject* pHint )
{
    CDataObject* pDoc = GetDocument();

    m_Data.lb.SetRedraw( FALSE ) ;            

    POSITION pos;
    CDataItem* pItem ;
    UINT i, k ;
    // lHint of UPDATE_ALL means the reason for the change cannot
    // be easily quantified.  
    if (lHint == UPDATE_ADDDATAITEM && pHint != NULL)
    {
        i = m_Data.lb.AddString("") ;
        ASSERT (LB_ERR != i) ;
        m_Data.lb.SetItemData( i, (DWORD)pHint) ;
    }
    else if (lHint == UPDATE_MARKETCHANGE)
    {
    
    }
    else 
    {
        UINT cItems = m_Data.lb.GetCount() ;
    
        ASSERT( cItems != LB_ERR ) ;
        
        for( i = 0, pos = pDoc->m_lstDataItems.GetHeadPosition(); pos != NULL; )
        {
            pItem = (CDataItem*)pDoc->m_lstDataItems.GetNext( pos ) ;
                
            if (i >= cItems)
            {
                k = m_Data.lb.AddString("") ;
                if (k != i)
                {
                    TRACE("Somethings wrong in OnUpdateData") ;
                    break ;
                }
            }
            m_Data.lb.SetItemData( i, (DWORD)pItem ) ;
            i++ ;
        }
             
        for (k = i ; k < cItems ; k++)
        {
            m_Data.lb.DeleteString( k ) ;
        }
    }
  
    // TODO:  Make the invalidation smarter
    m_Data.lb.Invalidate( FALSE ) ;   
    m_Data.lb.SetRedraw( TRUE ) ;

}

void CDataObjectView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
    if (m_fInitialUpdate == FALSE)
        DoInitialUpdate() ;

    if (lHint == 0 || (lHint >= UPDATE_ADDREQUEST && lHint <=UPDATE_CHANGEPROP))
        DoUpdateRequests( pSender, lHint, pHint ) ;

    if (lHint == 0 || (lHint >= UPDATE_MARKETCHANGE && lHint <= UPDATE_ADDDATAITEM))
        DoUpdateData( pSender, lHint, pHint ) ;
}

void CDataObjectView::SetSelection( CRequest* pRequest ) 
{
    UINT cItems = m_Requests.lb.GetCount() ;
    ASSERT( cItems != LB_ERR ) ;
    CRequest*   pItem ;
    
    for (UINT i = 0 ; i < cItems ; i++)
    {
        pItem = (CRequest*)m_Requests.lb.GetItemData( i ) ;
        if (pItem == pRequest)
        {
            m_Requests.lb.SetCurSel( i ) ;
            return ;
        }
    }
}

void CDataObjectView::SetSelection( CProperty* pProperty ) 
{
    UINT cItems = m_Data.lb.GetCount() ;
    ASSERT( cItems != LB_ERR ) ;
    CDataItem*   pItem ;
    
    for (UINT i = 0 ; i < cItems ; i++)
    {
        pItem = (CDataItem*)m_Data.lb.GetItemData( i ) ;
        if (pItem->m_pValueProps->Find( pProperty ) != NULL)
        {
            m_Data.lb.SetCurSel( i ) ;
            return ;
        }
    }
}

void CDataObjectView::SetSelection( CDataItem* pDataItem ) 
{
    UINT cItems = m_Data.lb.GetCount() ;
    ASSERT( cItems != LB_ERR ) ;
    CDataItem*   pItem ;
    
    for (UINT i = 0 ; i < cItems ; i++)
    {
        pItem = (CDataItem*)m_Data.lb.GetItemData( i ) ;
        if (pItem == pDataItem)
        {
            m_Data.lb.SetCurSel( i ) ;
            return ;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// CDataObjectView diagnostics

#ifdef _DEBUG
void CDataObjectView::AssertValid() const
{
    CView::AssertValid();
}

void CDataObjectView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CDataObject* CDataObjectView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDataObject)));
    return (CDataObject*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDataObjectView message handlers

int CDataObjectView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    TRACE( "CDataObjectView::OnCreate\r\n" ) ;
    
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    CDataObject* pDoc = GetDocument();

    COLINFO* pColInfo ;
    int nCols ;
    CString str ;

    CRect rect ;
    GetClientRect( &rect ) ;
    DWORD dw = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
                                CLBS_LIKEBUTTON |
                                LBS_OWNERDRAWFIXED | 
                                LBS_HASSTRINGS |
                                LBS_NOTIFY |
                                LBS_DISABLENOSCROLL | 
                                LBS_NOINTEGRALHEIGHT ;

    CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject( ANSI_VAR_FONT ) ) ;

    // Create the Requests listbox
    //                          
    if (!m_Requests.lb.Create( dw, rect, this, IDLB_REQUESTS ))
    {
        TRACE( "Listbox failed to create!" ) ;
        return -1 ;
    }
    //m_Requests.lb.SetLikeButtonStyle( TRUE ) ;
    m_Requests.lb.SetFont( pFont ) ;
    pColInfo = pDoc->GetRequestsColInfo( &nCols, str ) ;
    m_Requests.lb.SetColumnsEx( nCols, pColInfo ) ;
    m_Requests.lb.SetWindowText( str ) ;
    
    // Create the DataItem listbox
    //                          
    if (!m_Data.lb.Create( dw, rect, this, IDLB_DATA ))
    {
        TRACE( "Listbox failed to create!" ) ;
        return -1 ;
    }
    //m_Data.lb.SetLikeButtonStyle( TRUE ) ;
    m_Data.lb.SetFont( pFont ) ;
    pColInfo = pDoc->GetDataColInfo( &nCols, str ) ;
    m_Data.lb.SetColumnsEx( nCols, pColInfo ) ;
    m_Data.lb.SetWindowText( str ) ;
    
    return 0;
}

void CDataObjectView::OnDraw( CDC* pDC )
{
} 

void CDataObjectView::DoDrawItemRequests( LPDRAWITEMSTRUCT lpDIS )
{
    CDC             dc ;             
    COLORREF        rgbBack ;
    RECT            rcFocus ;
    BOOL            fSelected ;
    int             x, y, cy ;
    char            szItem[256] ;

    *szItem = '\0' ;

    CRequest* pItem ;
    if (NULL == (pItem = (CRequest*)lpDIS->itemData))
        return ;
    wsprintf( szItem, "%lu\t%s", pItem->m_dwID, (LPCSTR)pItem->m_strRequest ) ;

    CProperty* pProp ;
    POSITION pos ;
    for (pos = pItem->m_pProperties->m_lstProps.GetHeadPosition(); pos != NULL ;)
    {
        pProp = (CProperty*)pItem->m_pProperties->m_lstProps.GetNext(pos) ;                     
        if (pProp)
        {
            lstrcat( szItem, "\t" ) ;
            lstrcat( szItem, pProp->m_strName ) ;
        }
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
    m_Requests.lb.ColumnTextOut( &dc, x+2, y+cy, &rcFocus, CString(szItem) );

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

void CDataObjectView::DoDrawItemData( LPDRAWITEMSTRUCT lpDIS )
{
    CDC             dc ;             
    COLORREF        rgbBack ;
    RECT            rcFocus ;
    BOOL            fSelected ;
    int             x, y, cy ;
    char            szItem[256] ;
//    char            szNames[256] ;

    *szItem = '\0' ;
//    *szNames = '\0' ;
    
    CDataItem*          pItem ;
    if (NULL == (pItem = (CDataItem*)lpDIS->itemData))
        return ;

    VARIANT va ;
    VariantInit(&va) ;

    CProperty* pProperty ;
    POSITION pos ;             
    ASSERT(pItem->m_pValueProps) ;
    for (pos = pItem->m_pValueProps->m_lstProps.GetHeadPosition() ; pos != NULL ;)
    {
        pProperty = (CProperty*)pItem->m_pValueProps->m_lstProps.GetNext(pos) ;
        ASSERT(pProperty) ;

//        lstrcat( szNames, pProperty->m_strName ) ;

        if (SUCCEEDED(VariantChangeType( &va, &pProperty->m_varValue, 0, VT_BSTR)))
        {
            lstrcat( szItem, va.bstrVal ) ;
            VariantClear( &va ) ;
        }
        else
            lstrcat( szItem, "<type!>" ) ;
        
        if (pos != NULL )
        {
//            lstrcat( szNames, "\t" ) ;
            lstrcat( szItem, "\t" ) ;
        }
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
//    m_Data.lb.ColumnTextOut( &dc, x+2, y+cy, &rcFocus, CString(szNames) );
//    cy = (rcFocus.bottom - rcFocus.top) / 2 ;
//    RECT    rc = rcFocus ;
//    rc.top += cy ;
    m_Data.lb.ColumnTextOut( &dc, x+2, y+cy, &rcFocus, CString(szItem) );
     
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

void CDataObjectView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS)
{
    if (lpDIS->itemID == LB_ERR)
        return ;
        
    if (nIDCtl == IDLB_REQUESTS)
        DoDrawItemRequests( lpDIS );
    else
        DoDrawItemData( lpDIS ) ;
}


void CDataObjectView::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMIS)
{
    if (nIDCtl == IDLB_REQUESTS)
        m_Requests.lb.MeasureItem( lpMIS ) ;
    else
    {
        m_Data.lb.MeasureItem( lpMIS ) ;
        lpMIS->itemHeight *= 2 ;
    }
}

void CDataObjectView::OnSize(UINT nType, int cx, int cy)
{   

//    UINT    nSplitPos = 0;
    int y = -1 ;    
    CView::OnSize(nType, cx, cy);
    CDataObject* pDoc = GetDocument();
//    if (pDoc)
//        nSplitPos = pDoc->m_nSplitterPos ;
        
    if (m_Requests.lb.m_hWnd && IsWindow( m_Requests.lb.m_hWnd ))
    {
        m_Requests.lb.SetWindowPos( NULL, -1, y, cx+2, (cy/2)+1, SWP_NOZORDER ) ;        
        y += (cy / 2)+1 ;
        if (pDoc)
        {
            COLINFO* rgColInfo ;
            int nCols ;
            CString str ;
            nCols = m_Requests.lb.GetColumnsEx( &rgColInfo ) ;
            m_Requests.lb.GetWindowText( str ) ;
            pDoc->SetRequestsColInfo( nCols, rgColInfo, str, FALSE ) ;
        }
    }

    if (m_Data.lb.m_hWnd && IsWindow( m_Data.lb.m_hWnd ))
    {
        m_Data.lb.SetWindowPos( NULL, -1, y-1, cx+2, (cy/2)+2, SWP_NOZORDER ) ;        
        if (pDoc)
        {
            COLINFO* rgColInfo ;
            int nCols ;
            CString str ;
            nCols = m_Data.lb.GetColumnsEx( &rgColInfo ) ;
            m_Data.lb.GetWindowText( str ) ;
            pDoc->SetDataColInfo( nCols, rgColInfo, str, FALSE ) ;
        }
    }
}

//=======================================================================
// Request list box specific handlers
//
void CDataObjectView::OnRemoveItem()
{
    UINT i = m_Requests.lb.GetCurSel() ;
    if (i == LB_ERR)
        return ;

    CDataObject* pDoc = GetDocument();
        
    CRequest* pItem = (CRequest*)m_Requests.lb.GetItemData( i ) ;
    if (pItem)
    {
        pDoc->RemoveRequest( pItem ) ;
        pItem->ExternalRelease() ;
    }
}

void CDataObjectView::OnUpdateRemoveItem(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( LB_ERR != m_Requests.lb.GetCurSel() ) ;
}

void CDataObjectView::OnDblClkRequests()
{
    OnChangeItem() ;
}

void CDataObjectView::OnColumnChangeRequests()
{
    CDataObject* pDoc = GetDocument();
    if (pDoc)
    {
        COLINFO* rgColInfo ;
        int nCols ;
        CString str ;
            
        nCols = m_Requests.lb.GetColumnsEx( &rgColInfo ) ;
        m_Requests.lb.GetWindowText( str ) ;
        pDoc->SetRequestsColInfo( nCols, rgColInfo, str, TRUE ) ;
    }
}

void CDataObjectView::OnChangeItem()
{
    CChangeRequestDlg Dlg;
    CDataObject* pDoc = GetDocument();
    
    CRequest* pItem = pDoc->GetRequest( m_Requests.lb.GetCurSel() ) ;
    if (pItem)
    {   
        Dlg.m_strRequest = pItem->m_strRequest ;
        if (IDOK == Dlg.DoModal())
            pDoc->ChangeRequest( pItem, Dlg.m_strRequest ) ;
    }
}

void CDataObjectView::OnUpdateChangeItem(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( LB_ERR != m_Requests.lb.GetCurSel() ) ;
}

/////////////////////////////////////////////////////////////////////////////
// CChangeRequestDlg dialog
//
// TODO:  Enhance this dialog to allow the editing of properties.
//        it currently only allows the editing of the request string.
//
CChangeRequestDlg::CChangeRequestDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CChangeRequestDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CChangeRequestDlg)
    m_strRequest = "";
    //}}AFX_DATA_INIT
}

void CChangeRequestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CChangeRequestDlg)
    DDX_Text(pDX, IDC_NAME, m_strRequest);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChangeRequestDlg, CDialog)
    //{{AFX_MSG_MAP(CChangeRequestDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChangeRequestDlg message handlers

void CDataObjectView::OnAddItem()
{
    CChangeRequestDlg Dlg;
    CDataObject* pDoc = GetDocument();

    CRequest* pItem = new CRequest( pDoc ) ;

    if (pItem)
    {
        Dlg.m_strRequest = pItem->m_strRequest ;
        
//        Dlg.m_strProps = ((CProperty*)pItem->m_Properties->m_lstProps.GetHead())->m_strName ;
        
        if (IDOK == Dlg.DoModal())
        {   
            pItem->m_strRequest = Dlg.m_strRequest ;
            ASSERT(pItem->m_pProperties) ;
            
            pDoc->AddRequest( pItem ) ;
        }
        else
            delete pItem ;
    }

}

void CDataObjectView::OnUpdateAddItem(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( TRUE ) ;
}

//=======================================================================
// DataItemlist box specific handlers
//
void CDataObjectView::OnColumnChangeData()
{
    CDataObject* pDoc = GetDocument();
    if (pDoc)
    {
        COLINFO* rgColInfo ;
        int nCols ;
        CString str ;
            
        nCols = m_Data.lb.GetColumnsEx( &rgColInfo ) ;
        m_Data.lb.GetWindowText( str ) ;
        pDoc->SetDataColInfo( nCols, rgColInfo, str, TRUE ) ;
    }
}

void CDataObjectView::OnEditCopy()
{
    UINT i = m_Data.lb.GetCurSel() ;
    if (i == LB_ERR)
        return ;
    
    CDataObject* pDoc = GetDocument();
    CDataItem* pDataItem = (CDataItem*)m_Data.lb.GetItemData(i) ;
    ASSERT( pDataItem ) ;

    pDataItem->Copy() ; // call the automation fn
}

void CDataObjectView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( (LB_ERR != m_Data.lb.GetCurSel()) ) ;
}

void CDataObjectView::OnOptionsProperties()
{
    CDataObject* pDoc = GetDocument();
    if (!pDoc)
        return ;

    CDataObjPropsDlg    dlg(this) ;
        
    dlg.m_strName = pDoc->m_strName ;
    dlg.m_strUsername = pDoc->m_strUsername ;
    dlg.m_strPassword = pDoc->m_strPassword ;
    dlg.m_uiUpdateFrequency = pDoc->m_uiUpdateFrequency ;
    dlg.m_fMostRecentOnly = pDoc->m_fMostRecentOnly ;

    if (IDOK == dlg.DoModal())        
    {
        pDoc->m_strName = dlg.m_strName ;
        pDoc->m_strUsername = dlg.m_strUsername ;
        pDoc->m_strPassword = dlg.m_strPassword ;
        
        if (pDoc->m_uiUpdateFrequency != dlg.m_uiUpdateFrequency)
        {
            pDoc->SetUpdateFrequency( dlg.m_uiUpdateFrequency ) ;
        }
        pDoc->m_fMostRecentOnly = dlg.m_fMostRecentOnly ;
    }
}


/////////////////////////////////////////////////////////////////////////////
// CDataObjPropsDlg dialog
CDataObjPropsDlg::CDataObjPropsDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CDataObjPropsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDataObjPropsDlg)
    m_strName = "";
    m_strUsername = "";
    m_strPassword = "";
    m_uiUpdateFrequency = 0;
    m_fMostRecentOnly = FALSE;
    //}}AFX_DATA_INIT
}

void CDataObjPropsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDataObjPropsDlg)
    DDX_Text(pDX, IDC_NAME, m_strName);
    DDX_Text(pDX, IDC_USERNAME, m_strUsername);
    DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
    DDX_Text(pDX, IDC_UPDATEFREQ, m_uiUpdateFrequency);
    DDX_Check(pDX, IDC_MOSTRECENTONLY, m_fMostRecentOnly);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDataObjPropsDlg, CDialog)
    //{{AFX_MSG_MAP(CDataObjPropsDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


