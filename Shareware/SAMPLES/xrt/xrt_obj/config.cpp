// config.cpp : implementation file
//

#include "stdafx.h"
#include "xrt_obj.h"

#include "config.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define TAB_STOP    60
/////////////////////////////////////////////////////////////////////////////
// CXRTConfigView

IMPLEMENT_DYNCREATE(CXRTConfigView, CFormView)

CXRTConfigView::CXRTConfigView()
    : CFormView(CXRTConfigView::IDD)
{
    //{{AFX_DATA_INIT(CXRTConfigView)
    m_uiTimeout = 0 ;
    m_fShowChanges = FALSE;
    //}}AFX_DATA_INIT
    
    m_pClipboardObjectView = NULL ;
}

CXRTConfigView::~CXRTConfigView()
{
}

BOOL CXRTConfigView::PreCreateWindow( CREATESTRUCT &cs ) 
{
    cs.style   &=~(LONG)WS_HSCROLL ;
    cs.style   &=~(LONG)WS_VSCROLL ;

    return CFormView::PreCreateWindow( cs ) ;
}


void CXRTConfigView::OnDestroy()
{
/*
    if (m_pClipboardObjectView != NULL)
    {
        if (m_pClipboardObjectView->m_pXRTDataObject != NULL)
            m_pClipboardObjectView->m_pXRTDataObject->Release() ;
        m_pClipboardObjectView->m_pXRTDataObject = NULL ;
    }
*/    
    if (!(((CXRTApp*)AfxGetApp())->m_fEmbedding || ((CXRTApp*)AfxGetApp())->m_fAutomation) )
    {
        CString szKey ; szKey.LoadString( IDS_INI_CONFIGWNDPOS ) ;                                           
        SavePosition( GetParent(), szKey ) ;        
    }
    
    CFormView::OnDestroy();
}

void CXRTConfigView::OnInitialUpdate()
{
    CXRTData* pXRTData = GetDocument() ;
    ASSERT( pXRTData ) ;
    
    Ctl3dSubclassDlg( m_hWnd, CTL3D_ALL ) ;
    CString szKey ; szKey.LoadString( IDS_INI_CONFIGWNDPOS ) ;

    CFormView::OnInitialUpdate() ;

    m_lbInstruments.SetFont( ((CXRTApp*)AfxGetApp())->m_pFont, TRUE ) ;    
    COLINFO rgCol[]=
    {
        60,     0,      DT_LEFT,
        60,     0,      DT_RIGHT,
        60,     0,      DT_RIGHT,
        60,     0,      DT_RIGHT,
        60,     0,      DT_RIGHT,
        60,     0,      DT_LEFT,
    } ;
    
    m_lbInstruments.SetColumnsEx( sizeof(rgCol)/sizeof(rgCol[0]), rgCol ) ;
    m_lbInstruments.SetWindowText( "Symbol\tHigh\tLow\tLast\tVolume\t" ) ;
    
    UINT n ;
    int  i ;
    char szItem[256] ;
    LPXRTSTOCK    lpQuote ;
    for (n = 0 ; n < pXRTData->m_lpQuotes->cStocks ; n ++ )
    {   
        lpQuote = &pXRTData->m_lpQuotes->rgStocks[n] ;
        wsprintf( szItem, "%s\t%lu.%02lu\t%lu.%02lu\t%lu.%02lu\t%lu",     
                    (LPSTR)lpQuote->szSymbol,
                    lpQuote->ulHigh/1000L, lpQuote->ulHigh%1000L,
                    lpQuote->ulLow/1000L, lpQuote->ulLow%1000L,
                    lpQuote->ulLast/1000L, lpQuote->ulLast%1000L,
                    lpQuote->ulVolume
                     ) ;
    
        i = m_lbInstruments.AddString( szItem ) ;
        m_lbInstruments.SetItemData( i, (ULONG)(LPVOID)&pXRTData->m_lpQuotes->rgStocks[n] ) ;
    }
    
    UpdateData( FALSE ) ;
    SetTimer( 42, pXRTData->m_uiTimeout, NULL ) ;

    RestorePosition( GetParent(), szKey ) ;
}


void CXRTConfigView::OnUpdate(CView*, LPARAM, CObject*)
{
    CXRTData* pXRTData = GetDocument() ;
    ASSERT( pXRTData ) ;
    m_uiTimeout = pXRTData->m_uiTimeout ;

    if (m_fShowChanges)
        m_lbInstruments.InvalidateRect( NULL, FALSE ) ;              
              
}

void CXRTConfigView::OnClickedSet()
{
    CXRTData* pXRTData = GetDocument() ;
    ASSERT( pXRTData ) ;  
    
    UpdateData() ; 
    pXRTData->m_uiTimeout = m_uiTimeout ;
    SetTimer( 42, pXRTData->m_uiTimeout, NULL ) ;
}

void CXRTConfigView::OnClickedShowChanges()
{
    UpdateData() ;

}

void CXRTConfigView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CXRTConfigView)
    DDX_Control(pDX, IDC_INSTRUMENTS, m_lbInstruments);
    DDX_Text(pDX, IDC_TIMEOUT, m_uiTimeout);
    DDV_MinMaxUInt(pDX, m_uiTimeout, 0, 10000);
    DDX_Check(pDX, IDC_SHOWCHANGES, m_fShowChanges);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXRTConfigView, CFormView)
    //{{AFX_MSG_MAP(CXRTConfigView)
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_SET, OnClickedSet)
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    ON_WM_TIMER()
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
    ON_BN_CLICKED(IDC_SHOWCHANGES, OnClickedShowChanges)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXRTConfigView message handlers

int CXRTConfigView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFormView::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

#ifdef _DEBUG
CXRTData* CXRTConfigView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CXRTData)));
    return (CXRTData*) m_pDocument;
}
#endif


void CXRTConfigView::OnSize(UINT nType, int cx, int cy)
{
    if (m_lbInstruments.GetSafeHwnd() != NULL)             
        m_lbInstruments.SetWindowPos( NULL, 4, 56, cx-8, cy-58, SWP_NOACTIVATE |SWP_NOZORDER ) ;
}


void CXRTConfigView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS)
{
    if (lpDIS->itemID == LB_ERR)
        return ;

    CXRTData*       pDoc ;
    LPXRTSTOCK    lpQuote ;
    
    CDC             dc ;             
    COLORREF        rgbBack ;
    RECT            rcFocus ;
    BOOL            fSelected ;
    int             x, y, cy ;

    char    szItem[256] ;

    lpQuote = (LPXRTSTOCK)lpDIS->itemData ;

    wsprintf( szItem, "%s\t%lu.%02lu\t%lu.%02lu\t%lu.%02lu\t%lu",     
                (LPSTR)lpQuote->szSymbol,
                lpQuote->ulHigh/1000L, lpQuote->ulHigh%1000L,
                lpQuote->ulLow/1000L, lpQuote->ulLow%1000L,
                lpQuote->ulLast/1000L, lpQuote->ulLast%1000L,
                lpQuote->ulVolume
                 ) ;
                         
    dc.Attach( lpDIS->hDC ) ;
    
    pDoc = GetDocument() ;
    
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
        
    cy = (rcFocus.bottom - rcFocus.top - ((CXRTApp*)AfxGetApp())->m_cyFont) / 2 ;

    dc.ExtTextOut( x, y, ETO_OPAQUE, &lpDIS->rcItem, NULL, 0, NULL ) ;

//    int nTab = MulDiv( TAB_STOP, GetTextMetricsCorrectly( dc.m_hDC, NULL ), 4)  ;
//    dc.TabbedTextOut( x + 2, y + cy, szItem, lstrlen(szItem), 1, &nTab, 0 ) ;

    m_lbInstruments.ColumnTextOut( &dc, x+2, y+cy, &rcFocus, CString(szItem) );

     
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


void CXRTConfigView::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    lpMeasureItemStruct->itemHeight = ((CXRTApp*)AfxGetApp())->m_cyFont ;
}


void CXRTConfigView::OnTimer(UINT nIDEvent)
{
    GetDocument()->GetNewData() ;
}

void CXRTConfigView::OnEditCopy()
{       
    HRESULT  hr ;
    
    ASSERT( ((CXRTApp*)AfxGetApp())->m_pIClassFactory ) ;
    
    if (m_pClipboardObjectView == NULL)
    {
        m_pClipboardObjectView = ((CXRTApp*)AfxGetApp())->CreateXRTObjectView() ;
        if (m_pClipboardObjectView == NULL)
        {
            TRACE( "could not create a new CXRTObjectView for clipboard" ) ;
            return  ;
        }
        
        // CXRTObjectView::CreateDataObject creates the actual OLE2 object and
        // returns a poitner to it's IUnknown.
        //
        LPUNKNOWN pUnk ;    
        hr = m_pClipboardObjectView->CreateDataObject( NULL, &pUnk ) ;
        if (FAILED(hr) || pUnk == NULL)
        {
            delete m_pClipboardObjectView ;
            m_pClipboardObjectView = NULL ;
            return ;
        }   
        
        hr = pUnk->QueryInterface( IID_IUnknown, (LPVOID FAR*)&m_pClipboardObjectView->m_pXRTDataObject ) ;
        if (FAILED(hr)) // QueryInterface
        {
            m_pClipboardObjectView->DestroyDataObject( pUnk ) ;
            delete m_pClipboardObjectView ;
            m_pClipboardObjectView = NULL ;
            return ;
        }
        
        // CreateDataObject did an AddRef so we must Release
        pUnk->Release() ;
        
        m_pClipboardObjectView->GetParent()->SetWindowText( "Clipboard Object" ) ;
        m_pClipboardObjectView->m_fStatic = TRUE ;
    }
         
    LPDATAOBJECT    lpDataObj ;
    hr = m_pClipboardObjectView->m_pXRTDataObject->QueryInterface( IID_IDataObject, (LPVOID FAR*)&lpDataObj ) ;
    if (FAILED(hr)) // QueryInterface
    {
        TRACE( "QueryInterface on clipboard dataobject failed to return pIDataObject\n" ) ;
        return ;
    }
    
    m_pClipboardObjectView->OnUpdate( this, 1, NULL ) ;
    OleSetClipboard( lpDataObj ) ;
    lpDataObj->Release() ;
}

void CXRTConfigView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( TRUE ) ;
}

void CXRTConfigView::OnEditClear()
{
    // TODO: Add your command handler code here
    
}

void CXRTConfigView::OnUpdateEditClear(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( FALSE ) ;
    
}
