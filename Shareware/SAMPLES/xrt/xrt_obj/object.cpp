// object.cpp : implementation of the CXRTObjectView class
//

#include "stdafx.h"
#include "xrt_obj.h"
#include "dataobj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXRTObjectView

IMPLEMENT_DYNCREATE(CXRTObjectView, CView)

BEGIN_MESSAGE_MAP(CXRTObjectView, CView)
    //{{AFX_MSG_MAP(CXRTObjectView)
    ON_WM_DESTROY()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXRTObjectView construction/destruction

CXRTObjectView::CXRTObjectView()
{   
    m_pXRTDataObject = NULL ;
    m_fStatic = FALSE ;  
}

CXRTObjectView::~CXRTObjectView()
{
}

int CXRTObjectView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    RECT rect ;
    rect.top = lpCreateStruct->y ;
    rect.left = lpCreateStruct->x ;
    rect.bottom = lpCreateStruct->cy ;
    rect.right = lpCreateStruct->cx ;
    if (!m_lb.Create( WS_CHILD | WS_VISIBLE | 
                                WS_BORDER |
                                WS_VSCROLL |
                                #if 1
                                LBS_OWNERDRAWFIXED | 
                                LBS_HASSTRINGS |
                                #else
                                LBS_USETABSTOPS |
                                #endif
                                LBS_NOTIFY |
                                LBS_DISABLENOSCROLL | 
//                                LBS_SORT |
                                LBS_NOINTEGRALHEIGHT,
                                rect, this, 42 ))
    {
        TRACE( "Listbox failed to create!" ) ;
        return -1 ;
    }
    
    // Create font
    m_lb.SetFont( ((CXRTApp*)AfxGetApp())->m_pFont ) ;
    m_lb.SetWindowText( "Col A\tCol B\tCol C" ) ;

    int n = 60 ;
    m_lb.SetTabStops( 1, &n) ;                     
    m_lb.AddString( "A1\tB1\tC1" ) ;
    m_lb.AddString( "A2\t\t" ) ;
    m_lb.AddString( "A3\t\t" ) ;
    m_lb.AddString( "A4\tB4\t" ) ;
    m_lb.AddString( "A5\tB5\t" ) ;
    m_lb.AddString( "A6\tB6\tC6" ) ;
    m_lb.AddString( "A7\tB7\tC7" ) ;
    m_lb.AddString( "\t\t" ) ;
    m_lb.AddString( "\t\tC9" ) ;
    m_lb.AddString( "\tB10\tC10" ) ;
    
                         
    return 0;
}

void CXRTObjectView::OnDestroy()
{
    if (m_pXRTDataObject != NULL)
    {
        TRACE( "~CXRTObjectView: m_pXRTDataObject is not NULL.\n" ) ;
        m_pXRTDataObject->AddRef() ;
        TRACE( "~CXRTObjectView: RefCount = %d\n", m_pXRTDataObject->Release() ) ;
        // ((CXRTApp*)AfxGetApp())->ObjectCreated( FALSE ) ;  // could shutdown app
        // Don't worry about trying to ::Release it (doing so
        // might cause a call back!)
        //
        // delete m_pXRTDataObject ;
        m_pXRTDataObject = NULL ;
    }
    
    CView::OnDestroy();
}

void CXRTObjectView::OnInitialUpdate()
{
    CXRTData* pXRTData = GetDocument() ;
    ASSERT( pXRTData ) ;
    
    CView::OnInitialUpdate() ;
  
// This is NOP because when OnInitialUpdate is called, we haven't 
// created the dataobject yet.
//
//    if (m_pXRTDataObject != NULL)
//        m_pXRTDataObject->DataHasChanged( TRUE ) ;

}


void CXRTObjectView::OnUpdate(CView*, LPARAM l, CObject*)
{
    //CXRTData* pXRTData = GetDocument() ;

    if (m_pXRTDataObject != NULL && m_fStatic == FALSE)
        m_pXRTDataObject->DataHasChanged( FALSE ) ;
        
    if (m_pXRTDataObject != NULL && l != 0 && m_fStatic == TRUE)
        m_pXRTDataObject->DataHasChanged( TRUE ) ;
}

// Called by CIClassFactory::CreateInstance() to create a XRT object
//
// The object is implemented by CXRTDataObject (DATAOBJ.CPP).
//
// CXRTDataObject is given a pointer back to us so that it can
// call ::DestroyDataObject when it get's released.
//
HRESULT CXRTObjectView::CreateDataObject( LPUNKNOWN punkOuter, LPUNKNOWN *ppUnk )
{
    m_pXRTDataObject = new CXRTDataObject( punkOuter, this ) ;
    
    if (m_pXRTDataObject == NULL)
        return ResultFromScode( E_OUTOFMEMORY ) ;
        
    if (m_pXRTDataObject->Init() == FALSE)
    {
        ASSERT( 0 ) ;
        delete m_pXRTDataObject ;
        m_pXRTDataObject = NULL ;
        return ResultFromScode( E_OUTOFMEMORY ) ;
    }
        
    *ppUnk = m_pXRTDataObject ;
    m_pXRTDataObject->AddRef() ;
    
    // Tell the app that an object was created, so it can increment
    // it's lock count.
    //
    ((CXRTApp*)AfxGetApp())->ObjectCreated( TRUE ) ; 

    m_pXRTDataObject->DataHasChanged( TRUE ) ;
    
    return ResultFromScode( S_OK ) ;             
}

HRESULT CXRTObjectView::DestroyDataObject( LPUNKNOWN pUnk )
{
    // if pUnk is NULL then the caller is going to delete
    // otherwise we need to do it.
    //
    if (pUnk != NULL)
        delete (CXRTDataObject*)pUnk ;
    m_pXRTDataObject = NULL ;

    // Cause the window to be destroyed (but only after we've
    // gotten the heck out of here).
    //
    GetParent()->PostMessage( WM_CLOSE, 0, 0L ) ;    
    ((CXRTApp*)AfxGetApp())->ObjectCreated( FALSE ) ;  // could shutdown app
    return ResultFromScode( S_OK ) ;
}

/////////////////////////////////////////////////////////////////////////////
// CXRTObjectView drawing

void CXRTObjectView::OnDraw(CDC* pDC)
{
/*
    CXRTData* pDoc = GetDocument();
    UINT    cyFont = ((CXRTApp*)AfxGetApp())->m_cyFont ;
    UINT    y = cyFont ;

    ((CXRTApp*)AfxGetApp())->m_pFont = pDC->SelectObject( ((CXRTApp*)AfxGetApp())->m_pFont ) ;

    pDC->TextOut( 2, y = cyFont, CString("m_sz = ") + pDoc->m_sz ) ;
    char szBuf[256] ;         
    
    if (m_pXRTDataObject != NULL)
    {
        // Do an addref/release to get the reference count.
        m_pXRTDataObject->AddRef() ;
        wsprintf( szBuf, "Reference Count = %d", m_pXRTDataObject->Release() ) ;
        pDC->TextOut( 2, y+=cyFont, szBuf, lstrlen( szBuf ) ) ;
    }
    else
    {
        pDC->TextOut( 2, y += cyFont, CString("m_pXRTDataObject == NULL") ) ;
    }
    
    ((CXRTApp*)AfxGetApp())->m_pFont = pDC->SelectObject( ((CXRTApp*)AfxGetApp())->m_pFont ) ;
*/    
}

 
/////////////////////////////////////////////////////////////////////////////
// CXRTObjectView diagnostics

#ifdef _DEBUG
void CXRTObjectView::AssertValid() const
{
    CView::AssertValid();
}

void CXRTObjectView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CXRTData* CXRTObjectView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CXRTData)));
    return (CXRTData*) m_pDocument;
}

#endif //_DEBUG



void CXRTObjectView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    m_lb.SetWindowPos( NULL, -1, -1, cx+2, cy+2, SWP_NOZORDER ) ;        
}

void CXRTObjectView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    m_lb.DrawItem(lpDrawItemStruct);
}

void CXRTObjectView::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    m_lb.MeasureItem(lpMeasureItemStruct);
}
