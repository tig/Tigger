// doc.cpp : implementation of the CGenAutoDoc class
//
// CGenAutoDoc implementation  
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  December 8, 1993   cek     First implementation.
//
// This class implements an automation interface.  The following
// methods and properties are defined on GenAuto.Document.1
// (Note:  those marked with a '*' are not implemented yet).
// (Note:  '!' indicates "Value" property)
//
//  Document Object
//      Methods:
//          document.Activate
//          document.Close
//          document.Close( saveChanges, fileName )*
//          document.NewWindow*
//          document.Save*
//          document.SaveAs*
//  
//      Properties:
//          Application [out]
//          Visible [in/out]*
//          FullName [out]*
//          Name [out]*
//          Saved [out]*
//          Title [in/out]*
//          
#include "stdafx.h"
#include "genauto.h"
#include "doc.h"
#include "srvritem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenAutoDoc

IMPLEMENT_DYNCREATE(CGenAutoDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(CGenAutoDoc, COleServerDoc)
    //{{AFX_MSG_MAP(CGenAutoDoc)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CGenAutoDoc, COleServerDoc)
    //{{AFX_DISPATCH_MAP(CGenAutoDoc)
    DISP_PROPERTY_EX(CGenAutoDoc, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
    DISP_PROPERTY_EX(CGenAutoDoc, "Visible", GetVisible, SetVisible, VT_BOOL)
    DISP_PROPERTY_EX(CGenAutoDoc, "Title", GetTitle, SetTitle, VT_BSTR)
    DISP_FUNCTION(CGenAutoDoc, "Activate", Activate, VT_EMPTY, VTS_NONE)
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenAutoDoc construction/destruction

CGenAutoDoc::CGenAutoDoc()
{
    EnableAutomation();
    
    // determine default font for document
    memset(&m_logfont, 0, sizeof m_logfont);
    m_logfont.lfHeight = -10;
    static char BASED_CODE szArial[] = "Arial";
    lstrcpy(m_logfont.lfFaceName, szArial);
    m_logfont.lfOutPrecision = OUT_TT_PRECIS;
    m_logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    m_logfont.lfQuality = PROOF_QUALITY;
    m_logfont.lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
    
    EnableCompoundFile( FALSE ) ;
    
    AfxOleLockApp();
}

CGenAutoDoc::~CGenAutoDoc()
{   
    AfxOleUnlockApp();
}

CFont* CGenAutoDoc::SelectDocFont(CDC* pDC, CFont& font)
{
    // convert points in m_logfont.lfHeight to logical
    LOGFONT logfont = m_logfont;
    logfont.lfHeight = -::MulDiv(-logfont.lfHeight,
        pDC->GetDeviceCaps(LOGPIXELSY), 72);

    // create the font object
    if (!font.CreateFontIndirect(&logfont))
        return NULL;

    // select the font
    return pDC->SelectObject(&font);
}
                       
int CGenAutoDoc::GetWidth( CDC* pDC ) 
{       
    return 100 ;
}
                       
void CGenAutoDoc::CalcDisplaySize( CDC* pDC, CSize& size ) 
{
    // select correct font for the document
    CFont font;
    CFont* pOldFont = SelectDocFont(pDC, font);
    
    size.cx = GetWidth( pDC ) ;

    TEXTMETRIC tm ;
    pDC->GetTextMetrics( &tm ) ;

    size.cy = tm.tmHeight * 4 + 2 ;
    
    // restore state of the dc
    if (pOldFont != NULL)
        pDC->SelectObject(pOldFont);
}

int CGenAutoDoc::Draw(CDC *pDC, CPoint ptStart)
{
    CFont font;
    CFont* pOldFont = SelectDocFont(pDC, font);
    if (pOldFont == NULL)
        return -1;
                                  
    TEXTMETRIC tm ;
    pDC->GetTextMetrics( &tm ) ;
                                  
    RECT rc ;                             
    rc.left = ptStart.x ;
    rc.top = ptStart.y ;
    rc.bottom = ptStart.y + tm.tmHeight ;
    rc.right = rc.left ;
         

    pDC->TextOut( 10, 10, "Document has no implementation" ) ;
   
    // restore state of the dc
    pDC->SelectObject(pOldFont);
    return 0 ;
}

void CGenAutoDoc::OnFinalRelease()
{
    COleServerDoc::OnFinalRelease() ;
}

BOOL CGenAutoDoc::OnNewDocument()
{
    if (!COleServerDoc::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGenAutoDoc server implementation

COleServerItem* CGenAutoDoc::OnGetEmbeddedItem()
{
    // OnGetEmbeddedItem is called by the framework to get the COleServerItem
    //  that is associated with the document.  It is only called when necessary.

    CGenAutoSrvrItem* pItem = new CGenAutoSrvrItem(this);
    ASSERT_VALID(pItem);
    return pItem;
}

/////////////////////////////////////////////////////////////////////////////
// CGenAutoDoc serialization

void CGenAutoDoc::Serialize(CArchive& ar)
{
    TRACE("CGenAutoDoc::Serialize\r\n" ) ;

    if (ar.IsStoring())
    {
        TRACE( "   Write\r\n" ) ;
        ar.Write(&m_logfont,sizeof(LOGFONT));
    }
    else
    {
        TRACE( "   Read\r\n" ) ;
        ar.Read(&m_logfont,sizeof(LOGFONT));
    }
}

/////////////////////////////////////////////////////////////////////////////
// CGenAutoDoc diagnostics

#ifdef _DEBUG
void CGenAutoDoc::AssertValid() const
{
    COleServerDoc::AssertValid();
}

void CGenAutoDoc::Dump(CDumpContext& dc) const
{
    COleServerDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGenAutoDoc automation methods commands
//
LPDISPATCH CGenAutoDoc::GetApplication()
{
    LPDISPATCH  lpDisp = NULL ;
    CApplication* pApp = new CApplication ;
    
    if (pApp)
        lpDisp = pApp->GetIDispatch(FALSE) ;
    
    return lpDisp ;
}

BOOL CGenAutoDoc::GetVisible()
{
    POSITION pos = GetFirstViewPosition();
    CView* pView = GetNextView(pos);
    if (pView != NULL)
    {
        return pView->IsWindowVisible() ;
    }   
    
    return FALSE ;
}

void CGenAutoDoc::SetVisible(BOOL fVisible)
{
    POSITION pos = GetFirstViewPosition();
    CView* pView = GetNextView(pos);
    if (pView != NULL)
    {
        // Get the MDI child
        CFrameWnd* pFrameWnd = pView->GetParentFrame();

        if (fVisible)
        {    
            if (!pFrameWnd->IsWindowVisible())
                pFrameWnd->ShowWindow(SW_SHOWNA);
            
            pFrameWnd = pFrameWnd->GetParentFrame() ;                
            if (pFrameWnd && !pFrameWnd->IsWindowVisible())
                pFrameWnd->ShowWindow(SW_SHOWNA);
        }
        else
        {
            pFrameWnd->ShowWindow( SW_HIDE ) ;
        }        
    }   
}

void CGenAutoDoc::Activate()
{
    SetVisible( TRUE ) ;
    
    POSITION pos = GetFirstViewPosition();
    CView* pView = GetNextView(pos);
    if (pView)
    {
        pView->ShowWindow( SW_SHOW ) ;
        CFrameWnd* pFrameWnd = pView->GetParentFrame();
        if (pFrameWnd)
            pFrameWnd->BringWindowToTop() ;
    }
}
/////////////////////////////////////////////////////////////////////////////
// CDocuments

IMPLEMENT_DYNCREATE(CDocuments, CCmdTarget)

CDocuments::CDocuments()
{
    EnableAutomation();
    AfxOleLockApp();
}

CDocuments::~CDocuments()
{
    AfxOleUnlockApp();
}

void CDocuments::OnFinalRelease()
{
    // When the last reference for an automation object is released
    //  OnFinalRelease is called.  This implementation deletes the 
    //  object.  Add additional cleanup required for your object before
    //  deleting it from memory.

    delete this;
}

BEGIN_MESSAGE_MAP(CDocuments, CCmdTarget)
    //{{AFX_MSG_MAP(CDocuments)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDocuments, CCmdTarget)
    //{{AFX_DISPATCH_MAP(CDocuments)
    DISP_PROPERTY_EX(CDocuments, "Count", GetCount, SetNotSupported, VT_I2)
    DISP_FUNCTION(CDocuments, "Add", Add, VT_VARIANT, VTS_NONE)
    DISP_FUNCTION(CDocuments, "Item", GetItem, VT_DISPATCH, VTS_VARIANT )
    DISP_FUNCTION(CDocuments, "Open", Open, VT_VARIANT, VTS_VARIANT)
    //}}AFX_DISPATCH_MAP
    DISP_DEFVALUE(CDocuments,"Item")
    DISP_PROPERTY_EX_ID(CDocuments, "_NewEnum", DISPID_NEWENUM, _NewEnum, SetNotSupported, VT_UNKNOWN)
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocuments message handlers
//
//  object.Add()
//      Creates a new Document object.  The new Document becomes the
//      active document.
//      Returns the newly created Document.
//
VARIANT CDocuments::Add()
{
    VARIANT va;
    VariantInit( &va ) ;
    POSITION pos ;   
    
    va.vt = VT_EMPTY ;

    // HACK:  Assume only one doc template for now...
    pos = AfxGetApp()->m_templateList.GetHeadPosition() ; 
    if (pos != NULL)
    {
        CDocTemplate* pTemplate = (CDocTemplate*)AfxGetApp()->m_templateList.GetNext( pos ) ;
        CDocument* pItem = pTemplate->OpenDocumentFile(NULL, TRUE );    // make visible
        
        if (pItem != NULL)
        {
            va.vt = VT_DISPATCH ;
            va.pdispVal = pItem->GetIDispatch(TRUE) ;
        }    
    }
    return va;
}

// object.Open( fileName )
//      Opens a document
//
//      fileName is required.
//
//      Returns the Document object
//
VARIANT CDocuments::Open(const VARIANT FAR& fileName)
{
    VARIANT va;
    VariantInit( &va ) ;
    CDocument* pItem ;
    
    va.vt = VT_EMPTY ;
    
    if (fileName.vt == VT_BSTR)
    {
        pItem = AfxGetApp()->OpenDocumentFile( fileName.bstrVal )  ;
        if (pItem != NULL)
        {
            va.vt = VT_DISPATCH ;
            va.pdispVal = pItem->GetIDispatch(TRUE) ;
        }    
    }
    return va;
}

// object.Count
//      Returns the number of open documents
//
short CDocuments::GetCount()
{
    // HACK:  Assume only one doc template for now...
    POSITION pos = AfxGetApp()->m_templateList.GetHeadPosition() ;
    CDocTemplate* pTemplate = (CDocTemplate*)AfxGetApp()->m_templateList.GetNext( pos ) ;
    pos = pTemplate->GetFirstDocPosition() ;
    short n = 0 ;
    while (pos != NULL && pTemplate->GetNextDoc( pos ))
        n++ ;
    return n ;
}

// object.Item( index )
//      Returns the Document specified by index.  index can either
//      be a document name or number
//
LPDISPATCH CDocuments::GetItem( const VARIANT FAR& vt )
{   
    CDocument* pItem = NULL ;  
    LPDISPATCH lpDisp = NULL ;

    // HACK:  Assume only one doc template for now...
    POSITION pos = AfxGetApp()->m_templateList.GetHeadPosition() ;
    CDocTemplate* pTemplate = (CDocTemplate*)AfxGetApp()->m_templateList.GetNext( pos ) ;
    pos = pTemplate->GetFirstDocPosition() ;
    
    if (vt.vt == VT_BSTR)
    {
        // for each document compare...
        while (pos != NULL)
        {
            pItem = pTemplate->GetNextDoc( pos ) ;
            if (pItem)
            {
                if (lstrcmpi( pItem->GetTitle(), vt.bstrVal ) == 0)
                    break ;
            }
        }
    }
    else
    {
        // coerce to VT_I4
        VARIANT va ;
        VariantInit( &va );
        if (SUCCEEDED(VariantChangeType( &va, (VARIANT FAR*)&vt, 0, VT_I4 )))
        {
            pItem = pTemplate->GetNextDoc( pos ) ;
            while(va.lVal)
            {
                if (pos == NULL)
                {
                    // off the end..
                    pItem = NULL ;
                    break ;
                }
                else
                {
                    pItem = pTemplate->GetNextDoc( pos ) ;
                }
                va.lVal-- ;
            }
        }
    }                 
    
    if (pItem != NULL)
        lpDisp = pItem->GetIDispatch(TRUE) ;        // AddRef

    // BUGBUG:  Implement dispatch exception if lpDisp == NULL
    return lpDisp ;
}

// object.Title
//      Get/Set property
//
BSTR CGenAutoDoc::GetTitle()
{
    CString s;
    s = COleServerDoc::GetTitle( ) ;
    return s.AllocSysString();
}

void CGenAutoDoc::SetTitle(LPCSTR lpszNewValue)
{
    COleServerDoc::SetTitle( lpszNewValue ) ;
}

// _NewEnum is a hidden method called by VBA during "For Each"
// processing
//
LPUNKNOWN CDocuments::_NewEnum()
{
    CEnumDocs* pIEV = NULL ;

    pIEV = new CEnumDocs ;
    
    if (pIEV)
    {
        pIEV->m_xEnumVARIANT.Reset() ;
        return &pIEV->m_xEnumVARIANT ;
    }
    
    return NULL ;
}

//----------------------------------------------------------------------------
// Implementation of CEnumDocs which implements the IEnumVARIANT interface
// used by _NewEnum
//
BEGIN_INTERFACE_MAP(CEnumDocs, CCmdTarget)
    INTERFACE_PART(CEnumDocs, IID_IEnumVARIANT, EnumVARIANT)
END_INTERFACE_MAP()

CEnumDocs::CEnumDocs()
{
    //TRACE("CEnumVARAINT::CEnumDocs()\r\n" ) ;
    
    AfxOleLockApp();
}

CEnumDocs::~CEnumDocs()
{
    //TRACE("CEnumVARAINT::~CEnumDocs()\r\n" ) ;
    AfxOleUnlockApp();
}

void CEnumDocs::OnFinalRelease()
{
    // When the last reference for an automation object is released
    //  OnFinalRelease is called.  This implementation deletes the 
    //  object.  Add additional cleanup required for your object before
    //  deleting it from memory.
    //TRACE("CEnumDocs::OnFinalRelease\r\n" ) ;
    delete this;
}

//----------------------------------------------------------------------------
// Implementation of the nested class XEnumVARIANT (declared through the
// BEGIN/END_INTERFACE_PART macros in the declaration of CEnumDocs)
//
// This class implements the IEnumVARIANT interface requried by the 
// _NewEnum property of the collection
//
CEnumDocs::XEnumVARIANT::XEnumVARIANT()
{
    m_posCurrent = NULL ;
}

STDMETHODIMP_(ULONG) CEnumDocs::XEnumVARIANT::AddRef()
{   
    METHOD_PROLOGUE(CEnumDocs, EnumVARIANT)
    return pThis->ExternalAddRef() ;
}   

STDMETHODIMP_(ULONG) CEnumDocs::XEnumVARIANT::Release()
{   
    METHOD_PROLOGUE(CEnumDocs, EnumVARIANT)
    return pThis->ExternalRelease() ;
}   

STDMETHODIMP CEnumDocs::XEnumVARIANT::QueryInterface( REFIID iid, void FAR* FAR* ppvObj )
{   
    METHOD_PROLOGUE(CEnumDocs, EnumVARIANT)
    return (HRESULT)pThis->ExternalQueryInterface( (void FAR*)&iid, ppvObj) ;
}   

// IEnumVARIANT::Next
// 
STDMETHODIMP CEnumDocs::XEnumVARIANT::Next( ULONG celt, VARIANT FAR* rgvar, ULONG FAR* pceltFetched)
{
    // This sets up the "pThis" pointer so that it points to our
    // containing CItems instance
    //
    METHOD_PROLOGUE(CEnumDocs, EnumVARIANT)

    HRESULT hr;
    ULONG   l ;
    CDocument*  pItem = NULL ;
    POSITION pos = AfxGetApp()->m_templateList.GetHeadPosition() ;
    CDocTemplate* pTemplate = (CDocTemplate*)AfxGetApp()->m_templateList.GetNext( pos ) ;

    // pceltFetched can legally == 0
    //                                           
    if (pceltFetched != NULL)
        *pceltFetched = 0;
    else if (celt > 1)
    {   
        TRACE("XEnumVARIANT::Next() celt > 1 and pceltFetched == NULL!\r\n", celt ) ;
        return ResultFromScode( E_INVALIDARG ) ;   
    }

    for (l=0; l < celt; l++)
        VariantInit( &rgvar[l] ) ;

    // Retrieve the next celt elements.
    hr = NOERROR ;
    for (l = 0 ; m_posCurrent != NULL && celt != 0 ; l++)
    {   
        pItem = pTemplate->GetNextDoc( m_posCurrent ) ;
        celt-- ;
        if (pItem)
        {
            //TRACE( "   Setting rgvar[%d]\r\n", (int)l ) ;
            rgvar[l].vt = VT_DISPATCH ;
            rgvar[l].pdispVal = pItem->GetIDispatch( TRUE ) ;
            if (pceltFetched != NULL)
                (*pceltFetched)++ ;
        }
        else 
        {
            TRACE("GetNext failed in IEnumVARIANT::Next\r\n" ) ;
            return ResultFromScode( E_UNEXPECTED ) ;
        }
    }
    
    if (celt != 0)
    {
        //TRACE( "   End of list...celt == %d\r\n", (int)celt ) ;
        hr = ResultFromScode( S_FALSE ) ;
    }
    
    return hr ;
}

// IEnumVARIANT::Skip
//
STDMETHODIMP CEnumDocs::XEnumVARIANT::Skip(unsigned long celt) 
{
    METHOD_PROLOGUE(CEnumDocs, EnumVARIANT)
    POSITION pos = AfxGetApp()->m_templateList.GetHeadPosition() ;
    CDocTemplate* pTemplate = (CDocTemplate*)AfxGetApp()->m_templateList.GetNext( pos ) ;

    //TRACE("XEnumVARIANT::Skip( %l )\r\n", celt ) ;
    
    while (m_posCurrent != NULL && celt--)
        pTemplate->GetNextDoc( m_posCurrent ) ;
    
    return (celt == 0 ? NOERROR : ResultFromScode( S_FALSE )) ;
}

STDMETHODIMP CEnumDocs::XEnumVARIANT::Reset()
{
    METHOD_PROLOGUE(CEnumDocs, EnumVARIANT)
    //TRACE("XEnumVARIANT::Reset()\r\n") ;
    POSITION pos = AfxGetApp()->m_templateList.GetHeadPosition() ;
    CDocTemplate* pTemplate = (CDocTemplate*)AfxGetApp()->m_templateList.GetNext( pos ) ;
    m_posCurrent = pTemplate->GetFirstDocPosition() ;
    
    return NOERROR ;
}

STDMETHODIMP CEnumDocs::XEnumVARIANT::Clone(IEnumVARIANT FAR* FAR* ppenum) 
{
    METHOD_PROLOGUE(CEnumDocs, EnumVARIANT)   
    //TRACE("XEnumVARIANT::Clone()\r\n" ) ;

    CEnumDocs* p = new CEnumDocs ;
    if (p)
    {
        p->m_xEnumVARIANT.m_posCurrent = m_posCurrent ;
        return NOERROR ;    
    }
    else
        return ResultFromScode( E_OUTOFMEMORY ) ;
}
