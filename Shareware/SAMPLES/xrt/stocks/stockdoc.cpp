// stockdoc.cpp : implementation of the CStocksDoc class
//

#include "stdafx.h"
#include "stocks.h"
#include "item.h"
#include "items.h"
#include "stockdoc.h"
#include "srvritem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStocksDoc

IMPLEMENT_DYNCREATE(CStocksDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(CStocksDoc, COleServerDoc)
    //{{AFX_MSG_MAP(CStocksDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CStocksDoc, COleServerDoc)
    //{{AFX_DISPATCH_MAP(CStocksDoc)
    DISP_PROPERTY_EX(CStocksDoc, "Items", GetItems, SetItems, VT_DISPATCH)
    DISP_FUNCTION(CStocksDoc, "ShowWindow", ShowWindow, VT_BOOL, VTS_BOOL)
    DISP_FUNCTION(CStocksDoc, "CreateItem", CreateItem, VT_DISPATCH, VTS_BSTR VTS_VARIANT)
    //}}AFX_DISPATCH_MAP
    //DISP_DEFVALUE(CStocksDoc, "Items")
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStocksDoc construction/destruction

CStocksDoc::CStocksDoc()
{
    EnableAutomation();

    // TODO: add one-time construction code here
    m_pItems = NULL ;
    
    AfxOleLockApp();
}

CStocksDoc::~CStocksDoc()
{   
    AfxOleUnlockApp();
}

void CStocksDoc::OnFinalRelease()
{
    CDocument::OnFinalRelease() ;
}

BOOL CStocksDoc::OnNewDocument()
{
    if (!COleServerDoc::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CStocksDoc server implementation

COleServerItem* CStocksDoc::OnGetEmbeddedItem()
{
    // OnGetEmbeddedItem is called by the framework to get the COleServerItem
    //  that is associated with the document.  It is only called when necessary.

    CStocksSrvrItem* pItem = new CStocksSrvrItem(this);
    ASSERT_VALID(pItem);
    return pItem;
}

/////////////////////////////////////////////////////////////////////////////
// CStocksDoc serialization

void CStocksDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }
}

/////////////////////////////////////////////////////////////////////////////
// CStocksDoc diagnostics

#ifdef _DEBUG
void CStocksDoc::AssertValid() const
{
    COleServerDoc::AssertValid();
}

void CStocksDoc::Dump(CDumpContext& dc) const
{
    COleServerDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStocksDoc commands

BOOL CStocksDoc::ShowWindow( BOOL fShow )
{
    int nshow = SW_SHOW ;
    
    if (fShow == FALSE)
        nshow = SW_HIDE ;

    POSITION pos = GetFirstViewPosition();
    CView* pView = GetNextView(pos);
    if (pView != NULL)
    {
        CFrameWnd* pFrameWnd = pView->GetParentFrame();
    
        fShow = pFrameWnd->IsWindowVisible() ;
        
        pFrameWnd->ActivateFrame(nshow);
        pFrameWnd = pFrameWnd->GetParentFrame();
        if (pFrameWnd != NULL)
            pFrameWnd->ActivateFrame(nshow);
    }   
    return fShow ;
}

LPDISPATCH CStocksDoc::GetItems()
{   
    if (m_pItems == NULL)
        m_pItems = new CStockItems ;
    
    LPDISPATCH lpDisp = m_pItems->GetIDispatch(TRUE) ;
    return lpDisp ;
}

void CStocksDoc::SetItems(LPDISPATCH newValue)
{
    CStockItems* pItems = (CStockItems*)CCmdTarget::FromIDispatch(newValue) ;
    if (pItems != NULL && pItems->IsKindOf(RUNTIME_CLASS(CStockItems)))
    {
        if (m_pItems != NULL)
        {
            LPDISPATCH lpDisp = m_pItems->GetIDispatch(FALSE) ;
            if (lpDisp) lpDisp->Release() ;
            delete m_pItems ;
        }
        m_pItems = pItems ;         
    }
}


LPDISPATCH CStocksDoc::CreateItem(LPCSTR Name, const VARIANT FAR& Value)
{
    CStockItem* pItem ;  
    LPDISPATCH lpDisp = NULL ;
    
    pItem = new CStockItem ;
    
    if (pItem != NULL)
    {
        pItem->m_szName = Name ;
        pItem->SetValue( Value ) ;

        lpDisp = pItem->GetIDispatch(FALSE) ;
    }
    
    // BUGBUG:  Implement dispatch exception if lpDisp == NULL
    return lpDisp ;
}
