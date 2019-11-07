// xrt_obj.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "xrt_obj.h"

#include "mainfrm.h"
#include "doc.h"
#include "object.h"
#include "config.h"
#include "mdi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXRTApp

BEGIN_MESSAGE_MAP(CXRTApp, CWinApp)
    //{{AFX_MSG_MAP(CXRTApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXRTApp construction

CXRTApp::CXRTApp()
{
    m_pFont = NULL ;
    m_pFontBold = NULL ;
    m_pObjectViewTemplate = NULL ;
    m_pConfigViewTemplate = NULL ;
    m_pXRTDataDoc = NULL ;    

    m_pIClassFactory = NULL ;
    m_dwRegCO = 0 ;    
    m_cLock = 0 ;
    m_cObj = 0 ;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXRTApp object

CXRTApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CXRTApp initialization

BOOL CXRTApp::InitInstance()
{
    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

    SetMessageQueue( 96 ) ;

    m_pFont = ReallyCreateFont( NULL, "MS Sans Serif", "Regular", 8, 0 ) ;
    m_pFontBold = ReallyCreateFont( NULL, "MS Sans Serif", "Bold", 8, 0 ) ;
                   
    TEXTMETRIC  tm ;   
    CDC dc ;
    HDC hdc = GetDC( NULL ) ;
    dc.Attach( hdc ) ;
    
    m_pFont = dc.SelectObject( m_pFont ) ;
    dc.GetTextMetrics( &tm ) ;
    m_pFont = dc.SelectObject( m_pFont ) ;
    dc.Detach(); 
    ReleaseDC( NULL, hdc ) ;
    
    m_cyFont = tm.tmHeight + tm.tmExternalLeading ;                   
    
    if (FAILED(OleInitialize(NULL)))
    {
        TRACE( "OleInitialize Failed!\r\n" ) ;
        return FALSE;
    }
    Ctl3dRegister(AfxGetInstanceHandle()) ;
    Ctl3dAutoSubclass(AfxGetInstanceHandle()) ;

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    m_pConfigViewTemplate = new CMultiDocTemplate(IDR_CONFIGVIEW,
            RUNTIME_CLASS(CXRTData),
            RUNTIME_CLASS(CConfigMDIChild),        
            RUNTIME_CLASS(CXRTConfigView)) ;                   
    AddDocTemplate( m_pConfigViewTemplate );

    m_pObjectViewTemplate = new CMultiDocTemplate(IDR_OBJECTFRAME,
            RUNTIME_CLASS(CXRTData),
            RUNTIME_CLASS(CObjectMDIChild),        
            RUNTIME_CLASS(CXRTObjectView)) ;
    AddDocTemplate( m_pObjectViewTemplate );
                   
    // create main MDI Frame window
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
    {
        TRACE( "LoadFrame failed\r\n" ) ;
        delete pMainFrame ;
        return FALSE;
    }
    m_pMainWnd = pMainFrame;

    m_fAutomation = m_fEmbedding = FALSE ;

    char szEmbedding[] = "Embedding";  
    int  nEmb = lstrlen( szEmbedding ) ;
    char szAutomation[] = "Automation";
    int  nAuto = lstrlen( szAutomation ) ;
    for (LPSTR p = m_lpCmdLine ; *p ; p++)
    {
        if (0 == _fstrnicmp( szEmbedding, p, nEmb ))
            m_fEmbedding = TRUE ;
        if (0 == _fstrnicmp( szAutomation, p, nAuto ))
            m_fAutomation = TRUE ;
    }


    if (m_fAutomation || m_fEmbedding)
    {
        // don't show ourselves. 
        if (m_fAutomation)
            TRACE( "Started with /Automation\r\n" ) ;
            
        if (m_fEmbedding)
            TRACE( "Started with /Embedding\r\n" ) ;
//
// In a 'retail' release all this would be un-commented so
// that the window never appears...
//             
//#ifdef _DEBUG
//
        m_cLock-- ;
        m_fAutomation = m_fEmbedding = FALSE ;
    }
//
//#else
//
//    }
//    else    
//    
//#endif     
//
    {                                                       
        // "/Automation" was not on the command line.  The user wants us
        // visible.
        //
        // Add an additional lock so that we don't shut down

        m_cLock++ ;

        CString szKey ;
        szKey.LoadString( IDS_INI_MAINWNDPOS ) ;
        pMainFrame->ShowWindow( SW_SHOWNORMAL ) ;
        RestorePosition( pMainFrame, szKey ) ;
        
        pMainFrame->UpdateWindow();
        TRACE( "Main window updated\r\n" ) ;
    }


    m_pIClassFactory = (LPCLASSFACTORY)new CImpIClassFactory ;
    if (m_pIClassFactory == NULL)
    {
        TRACE( "new CImpClassFactory failed!" ) ;
        m_pMainWnd->PostMessage( WM_CLOSE ) ;
        return TRUE ;
    }
    m_pIClassFactory->AddRef() ;
                
    TRACE( "Registering CLSID_StocksRUsStockQuotes {%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\r\n",
                CLSID_StocksRUsStockQuotes.Data1, CLSID_StocksRUsStockQuotes.Data2, CLSID_StocksRUsStockQuotes.Data3,
                CLSID_StocksRUsStockQuotes.Data4[0], CLSID_StocksRUsStockQuotes.Data4[1],
                CLSID_StocksRUsStockQuotes.Data4[2], CLSID_StocksRUsStockQuotes.Data4[3],
                CLSID_StocksRUsStockQuotes.Data4[4], CLSID_StocksRUsStockQuotes.Data4[5],
                CLSID_StocksRUsStockQuotes.Data4[6], CLSID_StocksRUsStockQuotes.Data4[7]);
    
    HRESULT hr ;
    hr = CoRegisterClassObject( CLSID_StocksRUsStockQuotes, 
                                (LPUNKNOWN)m_pIClassFactory,
                                CLSCTX_LOCAL_SERVER,
                                REGCLS_MULTIPLEUSE, &m_dwRegCO ) ;
    if (FAILED( hr ))
    {   
        TRACE( "CoRegisterClassObject failed" ) ;
        m_pMainWnd->PostMessage( WM_CLOSE ) ;
    }

    // Force the creation of our single CDocument (CXRTData) object.
    // This call will also create one view on the doc, the Object Config
    // View (CXRTConfigView).
    //
    OnFileNew(); 
    

    return TRUE;
}

int CXRTApp::ExitInstance()
{  
    if (m_dwRegCO != 0)
        CoRevokeClassObject( m_dwRegCO ) ;

    OleUninitialize() ;      
    
    if (m_pFont)
        delete m_pFont ;

    if (m_pFontBold)
        delete m_pFontBold ;
    
    Ctl3dUnregister( AfxGetInstanceHandle() ) ;
     
    return CWinApp::ExitInstance() ;
}

void CXRTApp::OnFileNew()
{
    OpenDocumentFile(NULL);
}

CDocument* CXRTApp::OpenDocumentFile(LPCSTR lpszFileName)
{
    if (m_pXRTDataDoc == NULL)
    {
        // This is the first time through
        // This creates the Object Configuration MDI Child
        m_pXRTDataDoc = (CXRTData*)m_pConfigViewTemplate->OpenDocumentFile(NULL);
        if (m_pXRTDataDoc == NULL)
        {
            AfxMessageBox("Could not create XRT data document" );
            return NULL;
        }
        
        return m_pXRTDataDoc ;
    }
    
    CFrameWnd* pNewFrame = m_pObjectViewTemplate->CreateNewFrame(m_pXRTDataDoc, NULL);
    if (pNewFrame == NULL)
        return m_pXRTDataDoc;
        
    m_pObjectViewTemplate->InitialUpdateFrame(pNewFrame, m_pXRTDataDoc);

    return m_pXRTDataDoc;
}

CXRTObjectView* CXRTApp::CreateXRTObjectView() 
{   
    CXRTObjectView* pObj = NULL ;
    
    CFrameWnd* pNewFrame = m_pObjectViewTemplate->CreateNewFrame( m_pXRTDataDoc, NULL ) ;

    CWnd* pWnd = pNewFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST);
    if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CXRTObjectView)))
        pObj = (CXRTObjectView*)pWnd ;
     
    m_pObjectViewTemplate->InitialUpdateFrame(pNewFrame, m_pXRTDataDoc);
    
    return pObj ;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    CButton m_Cancel;
    CStatic m_WrittenBy;
    CStatic m_Version;
    CStatic m_Group;
    CStatic m_Copyright;
    CStatic m_AppName;
    CStatic m_AllRightsReserved;
    //}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}
 
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    DDX_Control(pDX, IDCANCEL, m_Cancel);
    DDX_Control(pDX, IDC_WRITTENBY, m_WrittenBy);
    DDX_Control(pDX, IDC_VERSION, m_Version);
    DDX_Control(pDX, IDC_GROUP, m_Group);
    DDX_Control(pDX, IDC_COPYRIGHT, m_Copyright);
    DDX_Control(pDX, IDC_APPNAME, m_AppName);
    DDX_Control(pDX, IDC_ALLRIGHTSRESERVED, m_AllRightsReserved);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CXRTApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}



typedef struct tagVS_VERSION
  {
    WORD wTotLen;
    WORD wValLen;
    char szSig[16];
    VS_FIXEDFILEINFO vffInfo;
  } VS_VERSION;


BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    char szVersion[64] ;  
    wsprintf( szVersion, "Version ERROR!" ) ;

    char szExe[256] ;
    GetModuleFileName( AfxGetInstanceHandle(), szExe, 255 ) ;
    
    DWORD dwVerHnd ;                             
    DWORD dwVerInfoSize = GetFileVersionInfoSize( szExe, &dwVerHnd);

    if (dwVerInfoSize)
    {
        VS_VERSION*   pVI;             // Pointer to block to hold info

        pVI = (VS_VERSION*)(char*)new char[dwVerInfoSize] ;

        // Get the info and fill in the pertinent dialog components
        if(GetFileVersionInfo( szExe, 0L, dwVerInfoSize, pVI ))
        {                    
            if (HIWORD(pVI->vffInfo.dwFileVersionLS) > 0)
                wsprintf( szVersion,
                      "Version %d.%02d.%03d - %s",
                        HIWORD(pVI->vffInfo.dwFileVersionMS),
                        LOWORD(pVI->vffInfo.dwFileVersionMS),
                        HIWORD(pVI->vffInfo.dwFileVersionLS),
                        (LPSTR)__DATE__  );
            else
                wsprintf( szVersion,
                      "Version %d.%02d - %s",
                        HIWORD(pVI->vffInfo.dwFileVersionMS),
                        LOWORD(pVI->vffInfo.dwFileVersionMS),
                        (LPSTR)__DATE__  );
                                    
        }

        // Let go of the memory
        delete pVI ;
    }

    m_Cancel.SetFont( theApp.m_pFont ) ;

    m_AppName.SetWindowText( AfxGetAppName() ) ;   
    m_AppName.SetFont( theApp.m_pFont ) ;

    m_Version.SetWindowText( szVersion ) ;
    m_Version.SetFont( theApp.m_pFont ) ;

    m_Copyright.SetFont( theApp.m_pFont ) ;
    m_AllRightsReserved.SetFont( theApp.m_pFont ) ;
    m_WrittenBy.SetFont( theApp.m_pFont ) ;
    m_Group.SetFont( theApp.m_pFont ) ;

    DlgCenter( m_hWnd, ::GetParent( m_hWnd ), FALSE ) ;
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CXRTApp::ObjectCreated( BOOL fCreated )
{
    if (fCreated)
        m_cObj++ ;
    else
    {
        m_cObj-- ;
        
        if ( m_cObj == 0 && m_cLock == 0 && IsWindow( m_pMainWnd->m_hWnd ) )
            m_pMainWnd->PostMessage( WM_CLOSE ) ;
    }
}

void CXRTApp::LockServer( BOOL fLock )
{
    if (fLock)
        m_cLock++ ;
    else
    {
        m_cLock-- ;
        
        if ( m_cObj == 0 && m_cLock == 0 && IsWindow( m_pMainWnd->m_hWnd ))
            m_pMainWnd->PostMessage( WM_CLOSE ) ;    
    }
}

/////////////////////////////////////////////////////////////////////////////
// CXRTApp commands
   
// IClassFactory Implementation
// CImpIClassFactory::CImpIClassFactory
// CImpIClassFactory::~CImpIClassFactory
//
//
CImpIClassFactory::CImpIClassFactory(void)
{
    m_cRef=0L;
    return;
}

CImpIClassFactory::~CImpIClassFactory(void)
{
    return ;
}

// CImpIClassFactory::QueryInterface
// CImpIClassFactory::AddRef
// CImpIClassFactory::Release
//
// Purpose:
//  IUnknown members for CImpIClassFactory object.
//
STDMETHODIMP CImpIClassFactory::QueryInterface( REFIID riid, LPVOID FAR *ppv )
{
    *ppv = NULL ;    
    
    //Any interface on this object is the object pointer.
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
        *ppv = (LPVOID)this;
   
    // If we actually assign an interface to ppv we need to AddRef it
    // since we're returning a new pointer.
    //
    if (NULL != *ppv)
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }

    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) CImpIClassFactory::AddRef(void)
{
    return ++m_cRef;
}


STDMETHODIMP_(ULONG) CImpIClassFactory::Release(void)
{
    ULONG           cRefT;

    cRefT=--m_cRef;

    //Free the object if the reference and lock counts are zero.
    //
    if (0L==m_cRef)
        delete this;

    return cRefT;
}


// CImpIClassFactory::CreateInstance
//
// Purpose:
//  Instantiates an object supported by this class factory.  That
//  object must at least support IUnknown.
//
//  Derived classes should override this function.
//
// Parameters:
//  punkOuter       LPUNKNOWN to the controlling IUnknown if we are
//                  being used in an aggregation.
//  riid            REFIID identifying the interface the caller desires
//                  to have for the new object.
//  ppvObj          LPVOID FAR * in which to store the desired interface
//                  pointer for the new object.
//
// Return Value:
//  HRESULT         NOERROR if successful, otherwise contains E_NOINTERFACE
//                  if we cannot support the requested interface.
//
STDMETHODIMP CImpIClassFactory::CreateInstance(LPUNKNOWN punkOuter,
                                    REFIID riid, LPVOID FAR *ppvObj)
{
    HRESULT         hr ;
    
    *ppvObj = NULL ;
    
    hr = ResultFromScode(E_OUTOFMEMORY) ;
    
    // Verify that if there is a controlling unknown it's asking for
    // IUnknown
    if (NULL != punkOuter && !IsEqualIID( riid, IID_IUnknown ))
        return ResultFromScode( E_NOINTERFACE ) ;

    // Create a new CXRTObjectView view and call it's
    // Init routhine with punkOuter
    // 
    CXRTObjectView* pXRTObjView = ((CXRTApp*)AfxGetApp())->CreateXRTObjectView() ;
    if (pXRTObjView == NULL)
    {
        TRACE( "could not create a new CXRTObjectView" ) ;
        return ResultFromScode( E_FAIL ) ;
    }
    
    // CXRTObjectView::CreateDataObject creates the actual OLE2 object and
    // returns a poitner to it's IUnknown.
    //
    LPUNKNOWN pUnk ;    
    hr = pXRTObjView->CreateDataObject( punkOuter, &pUnk ) ;
    if (FAILED(hr) || pUnk == NULL)
        return hr ;
        
    hr = pUnk->QueryInterface( riid, ppvObj ) ;
    if (FAILED(hr)) // QueryInterface
    {
        pXRTObjView->DestroyDataObject( pUnk ) ;
        return hr ;
    }
    
    // CreateDataObject did an AddRef so we must Release
    pUnk->Release() ;
    
    return hr ;
}
    

// CImpIClassFactory::LockServer
//
// Purpose:
//  Increments or decrements the lock count of the serving IClassFactory
//  object.  When the lock count is zero and the reference count is zero
//  we get rid of this object.
//
//  DLL objects should override this to affect their DLL ref count.
//
// Parameters:
//  fLock           BOOL specifying whether to increment or decrement the
//                  lock count.
//
// Return Value:
//  HRESULT         NOERROR always.
//
STDMETHODIMP CImpIClassFactory::LockServer(BOOL fLock)
{
    if (fLock)
        ((CXRTApp*)AfxGetApp())->LockServer( fLock) ;  // could cause app shutdown
    
    return NOERROR;
}
   