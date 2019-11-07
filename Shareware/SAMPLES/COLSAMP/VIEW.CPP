// view.cpp : implementation of the CStocksView class
//

#include "stdafx.h"
#include "colsamp.h"

#include "item.h"
#include "items.h"

#include "doc.h"
#include "view.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColSampView

IMPLEMENT_DYNCREATE(CColSampView, CView)
#define new DEBUG_NEW

BEGIN_MESSAGE_MAP(CColSampView, CView)
    //{{AFX_MSG_MAP(CColSampView)
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
    ON_WM_SHOWWINDOW()
    //}}AFX_MSG_MAP
    ON_LBN_DBLCLK( 42, OnDblClk )
    ON_CONTROL( CLBN_COLUMNCHANGE, 42, OnColumnChange ) 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColSampView construction/destruction

CColSampView::CColSampView()
{
    m_fInitialUpdate = FALSE ;
}

CColSampView::~CColSampView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CColSampView drawing

void CColSampView::OnDraw(CDC* pDC)
{
    CColSampDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

//    pDC->TextOut( 10, 10, "Hello!" ) ;
}

void CColSampView::OnInitialUpdate()
{
    m_fInitialUpdate = TRUE ;

    CColSampDoc* pDoc = GetDocument();

    CDC *pDC = GetDC() ;
    LOGFONT logfont = pDoc->m_logfont;
    logfont.lfHeight = -::MulDiv(-logfont.lfHeight,
        pDC->GetDeviceCaps(LOGPIXELSY), 72);
    ReleaseDC(pDC) ;                        

    m_font.CreateFontIndirect( &logfont ) ;
    m_lb.SetFont( &m_font, TRUE ) ;

    m_lb.SetColumnsEx( pDoc->GetNumColumns(), pDoc->GetColInfo() ) ;
    m_lb.SetWindowText( pDoc->GetColTitleString()  ) ;

    OnUpdate( this, 0, NULL ) ;
}

void CColSampView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
    CColSampDoc* pDoc = GetDocument();

    if (m_fInitialUpdate == FALSE)
    {
        m_fInitialUpdate = TRUE ;
        CDC *pDC = GetDC() ;
        LOGFONT logfont = pDoc->m_logfont;
        logfont.lfHeight = -::MulDiv(-logfont.lfHeight,
            pDC->GetDeviceCaps(LOGPIXELSY), 72);
        ReleaseDC(pDC) ;                        
        
        m_font.CreateFontIndirect( &logfont ) ;
        m_lb.SetFont( &m_font, TRUE ) ;
        
        m_lb.SetColumnsEx( pDoc->GetNumColumns(), pDoc->GetColInfo() ) ;
        m_lb.SetWindowText( pDoc->GetColTitleString()  ) ;
    }  

    m_lb.SetRedraw( FALSE ) ;            
    
    POSITION pos;
    CItem* pItem ;
    UINT i, k ;
    // lHint of 0 means the reason for the change cannot
    // be easily quantified.  Removes have lHint = 0
    // lHint of 1 means an item's name or value has changed
    // lHint of 2 means an item has been added
    if (lHint == 1 && pHint != NULL)
    {                  
        // find the item in the oblist
        #if 0
        pos = pDoc->m_lstDocItems->Find( pHint ) ;
        if (pos != NULL)
        {
            // Invalidate just it's rectangle
        }
        else
        {
            TRACE("Could not find pHint in m_lstDocItems!" ) ;
        }    
        #endif
    }                    
    else if (lHint == 2 && pHint != NULL)
    {
        i = m_lb.AddString("") ;
        ASSERT (LB_ERR != i) ;
        m_lb.SetItemData( i, (DWORD)pHint) ;
    }
    else
    {
        UINT cItems = m_lb.GetCount() ;
    
        ASSERT( cItems != LB_ERR ) ;
        
        for( i = 0, pos = pDoc->m_lstDocItems.GetHeadPosition(); pos != NULL; )
        {
            pItem = (CItem*)pDoc->m_lstDocItems.GetNext( pos ) ;
                
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


/////////////////////////////////////////////////////////////////////////////
// CColSampView diagnostics

#ifdef _DEBUG
void CColSampView::AssertValid() const
{
    CView::AssertValid();
}

void CColSampView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CColSampDoc* CColSampView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CColSampDoc)));
    return (CColSampDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CColSampView message handlers

int CColSampView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    TRACE( "CColSampView::OnCreate\r\n" ) ;
    
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
    
    CColSampDoc* pDoc = GetDocument();

    CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject( ANSI_VAR_FONT ) ) ;
    m_lb.SetFont( pFont ) ;
    m_lb.SetColumnsEx( pDoc->GetNumColumns(), pDoc->GetColInfo() ) ;
    m_lb.SetWindowText( pDoc->GetColTitleString()  ) ;

    return 0;
}


void CColSampView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS)
{
    if (lpDIS->itemID == LB_ERR)
        return ;

    CItem*          pItem ;
    CDC             dc ;             
    COLORREF        rgbBack ;
    RECT            rcFocus ;
    BOOL            fSelected ;
    int             x, y, cy ;

    char            szItem[256] ;

    if (NULL == (pItem = (CItem*)lpDIS->itemData))
        return ;

    wsprintf( szItem, "%d\t%s\t%s\t%s", lpDIS->itemID, (LPCSTR)pItem->m_szName,
                (LPCSTR)VTtoString(pItem->m_vtValue.vt), 
                (LPCSTR)VariantToString(pItem->m_vtValue) ) ;
                         
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

//    int nTab = MulDiv( TAB_STOP, GetTextMetricsCorrectly( dc.m_hDC, NULL ), 4)  ;
//    dc.TabbedTextOut( x + 2, y + cy, szItem, lstrlen(szItem), 1, &nTab, 0 ) ;

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


void CColSampView::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    m_lb.MeasureItem( lpMeasureItemStruct ) ;
//    lpMeasureItemStruct->itemHeight = m_cyFont ;
}

void CColSampView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    
    if (m_lb.m_hWnd && IsWindow( m_lb.m_hWnd ))
    {
        m_lb.SetWindowPos( NULL, -1, -1, cx+2, cy+2, SWP_NOZORDER ) ;        

        CColSampDoc* pDoc = GetDocument();
        
        if (pDoc)
        {
            COLINFO* rgColInfo ;
            m_lb.GetColumnsEx( &rgColInfo ) ;
            pDoc->SetColInfo( rgColInfo, FALSE ) ;
        }
    }
}

void CColSampView::OnRemoveItem()
{
    UINT i = m_lb.GetCurSel() ;
    if (i == LB_ERR)
        return ;

    CColSampDoc* pDoc = GetDocument();
        
    CItem* pItem = (CItem*)m_lb.GetItemData( i ) ;
    if (pItem)
    {
        pDoc->RemoveDocItem( pItem ) ;

        LPDISPATCH lpDisp = pItem->GetIDispatch(FALSE) ;
        if (lpDisp) 
            lpDisp->Release() ;
    }
}

void CColSampView::OnUpdateRemoveItem(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( LB_ERR != m_lb.GetCurSel() ) ;
}

void CColSampView::OnDblClk()
{
    OnChangeItem() ;
}

void CColSampView::OnColumnChange()
{
    CColSampDoc* pDoc = GetDocument();
    
    COLINFO* rgColInfo ;
    m_lb.GetColumnsEx( &rgColInfo ) ;
    
    pDoc->SetColInfo( rgColInfo ) ;
}

void CColSampView::OnChangeItem()
{
    CChangeItemDlg Dlg;
    CColSampDoc* pDoc = GetDocument();
    CItem* pItem = pDoc->GetDocItem( m_lb.GetCurSel() ) ;
    if (pItem)
    {   
        VARIANT v ;
        VariantInit( &v ) ;

        if (pItem->m_vtValue.vt != VT_ERROR && FAILED(VariantChangeType( &v, &pItem->m_vtValue, 0, VT_BSTR )))
        {
            AfxMessageBox( "Cannot convert Value to a string.  It is not possible to change this value." ) ;
            return ;
        }
        
        Dlg.m_szName = pItem->m_szName ;
        if (pItem->m_vtValue.vt == VT_ERROR)
        {
            Dlg.m_szValue = "" ;
        }
        else
        {
            Dlg.m_szValue = v.bstrVal ;
        }
        
        Dlg.m_szType = VTtoString( pItem->m_vtValue.vt ) ;
        VariantClear( &v ) ;
                
        if (IDOK == Dlg.DoModal())
        {
            VARTYPE vt = StringToVT( Dlg.m_szType ) ;
            if (vt == -1)
            {
                AfxMessageBox( "Invalid type.  Item not changed." ) ;
                return ;
            }
            
            v.vt = VT_BSTR ;
            v.bstrVal = Dlg.m_szValue.AllocSysString() ;
            
            if (FAILED(VariantChangeType( &pItem->m_vtValue, &v, 0, vt )))
            {   
                char szBuf[256] ;
                wsprintf( szBuf, "Cannot convert Value to a %s.  Item not changed.", (LPSTR)VTtoString( vt ) ) ;
                AfxMessageBox( szBuf ) ;
                VariantClear( &v ) ;
                return ;
            }
            VariantClear( &v ) ;
            pItem->m_szName = Dlg.m_szName ;
            pDoc->UpdateAllViews( NULL, 1, pItem ) ;
            pDoc->SetModifiedFlag(TRUE) ;
        }
    }
}

void CColSampView::OnUpdateChangeItem(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( LB_ERR != m_lb.GetCurSel() ) ;
}

/////////////////////////////////////////////////////////////////////////////
// CChangeItemDlg dialog


CChangeItemDlg::CChangeItemDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CChangeItemDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CChangeItemDlg)
    m_szName = "";
    m_szValue = "";
    m_szType = "";
    //}}AFX_DATA_INIT
}

void CChangeItemDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CChangeItemDlg)
    DDX_Text(pDX, IDC_NAME, m_szName);
    DDX_Text(pDX, IDC_VALUE, m_szValue);
    DDX_CBString(pDX, IDC_TYPE, m_szType);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChangeItemDlg, CDialog)
    //{{AFX_MSG_MAP(CChangeItemDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChangeItemDlg message handlers

void CColSampView::OnAddItem()
{
    CChangeItemDlg Dlg;
    CColSampDoc* pDoc = GetDocument();

    CItem* pItem = new CItem( pDoc ) ;

    if (pItem)
    {   
        VARIANT v ;
        VariantInit( &v ) ;

        if (pItem->m_vtValue.vt != VT_ERROR && FAILED(VariantChangeType( &v, &pItem->m_vtValue, 0, VT_BSTR )))
        {
            AfxMessageBox( "Cannot convert Value to a string.  It is not possible to add this value." ) ;
            delete pItem ;
            return ;
        }
        
        Dlg.m_szName = pItem->m_szName ;
        if (pItem->m_vtValue.vt == VT_ERROR)
        {
            Dlg.m_szValue = "" ;
        }
        else
        {
            Dlg.m_szValue = v.bstrVal ;
        }
        
        Dlg.m_szType = VTtoString( pItem->m_vtValue.vt ) ;
        VariantClear( &v ) ;
                
        if (IDOK == Dlg.DoModal())
        {
            VARTYPE vt = StringToVT( Dlg.m_szType ) ;
            if (vt == -1)
            {
                AfxMessageBox( "Invalid type.  Item not added." ) ;
                delete pItem ;
                return ;
            }
            
            v.vt = VT_BSTR ;
            v.bstrVal = Dlg.m_szValue.AllocSysString() ;
            
            if (FAILED(VariantChangeType( &pItem->m_vtValue, &v, 0, vt )))
            {   
                char szBuf[256] ;
                wsprintf( szBuf, "Cannot convert Value to a %s.  Item not added.", (LPSTR)VTtoString( vt ) ) ;
                AfxMessageBox( szBuf ) ;
                VariantClear( &v ) ;
                delete pItem ;
                return ;
            }
            
            VariantClear( &v ) ;
            pItem->m_szName = Dlg.m_szName ;
            
            pDoc->AddDocItem( pItem ) ;
        }
        else
            delete pItem ;
    }
}

void CColSampView::OnUpdateAddItem(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( TRUE ) ;
}

void CColSampView::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CView::OnShowWindow(bShow, nStatus);
}
