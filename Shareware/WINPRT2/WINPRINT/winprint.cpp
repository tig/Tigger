// winprint.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "winprint.h"

#include "mainfrm.h"
#include "winprdoc.h"
#include "cntritem.h"
#include "cfgview.h"
#include "queview.h"
#include "cfglstvw.h"

#include "cfgframe.h"
#include "cfgview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinPrintApp

BEGIN_MESSAGE_MAP(CWinPrintApp, CWinApp)
    //{{AFX_MSG_MAP(CWinPrintApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
    // Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinPrintApp construction

CWinPrintApp::CWinPrintApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWinPrintApp object

CWinPrintApp theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.
static const CLSID BASED_CODE clsid =
{ 0x0002A5F0, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0, 0x46 } };

/////////////////////////////////////////////////////////////////////////////
// CWinPrintApp initialization

BOOL CWinPrintApp::InitInstance()
{
    // Initialize OLE 2.0 libraries
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

    m_pFont = ReallyCreateFont( NULL, _T("MS Sans Serif"), _T("Regular"), 10, 0 ) ;
    m_pFontBold = ReallyCreateFont( NULL, _T("MS Sans Serif"), _T("Bold"), 10, 0 ) ;
                   
    LoadBitmaps( FALSE ) ;                   

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
    
    Ctl3dRegister(AfxGetInstanceHandle()) ;
    Ctl3dAutoSubclass(AfxGetInstanceHandle()) ;

    CMainFrame::Initialize() ;

    LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CWinPrintDoc),
        RUNTIME_CLASS(CMainFrame),     // main SDI frame window
        RUNTIME_CLASS(CConfigListView));
    pDocTemplate->SetContainerInfo(IDR_CNTR_INPLACE);
    AddDocTemplate(pDocTemplate);

    // create main window
    if (m_nCmdShow == SW_SHOWNORMAL)
        m_nCmdShow = CMainFrame::m_nDefCmdShow ;

    OnFileNew() ;
    int nCmdShow = m_nCmdShow;

    // create splash window and update registry if not run with /Embedding
    BOOL bRunAutomated = RunAutomated();

    // setup main window
    nCmdShow = !bRunAutomated ? m_nCmdShow : SW_HIDE;
    m_nCmdShow = SW_HIDE;

    //((CMainFrame*)m_pMainWnd)->InitialShowWindow(nCmdShow);
    if (!bRunAutomated)
    {
        m_pMainWnd->UpdateWindow();

        if (!m_pMainWnd->IsIconic() && m_lpCmdLine[0] == 0 &&
            m_splash.Create(m_pMainWnd))
        {
            m_splash.ShowWindow(SW_SHOW);
            m_splash.UpdateWindow();
        }
        m_dwSplashTime = ::GetCurrentTime();
    }

    if (m_lpCmdLine[0] != '\0')
    {
        // TODO: add command line processing here
    }
    
    m_nCmdShow = nCmdShow;
    OpenDocumentFile("Z:\\source\\kindel\\winprt2\\winprt20\\winprt20.cfg");
    m_pMainWnd->UpdateWindow();


    CWinPrintDoc* pDoc = (CWinPrintDoc*)((CFrameWnd*)m_pMainWnd)->GetActiveDocument() ;
    ASSERT(pDoc) ;
    
    CCreateContext context ;
    context.m_pCurrentFrame = NULL ;
    context.m_pCurrentDoc = pDoc ;
    context.m_pNewViewClass = RUNTIME_CLASS(CConfigView) ;
    context.m_pNewDocTemplate = NULL ;
    context.m_pLastView = NULL ;

    POSITION pos ;
    pos = pDoc->GetStartPosition();
    while (pos != NULL)
    {
        CWinPrintCntrItem* pDocItem = (CWinPrintCntrItem*)pDoc->GetNextItem(pos);
        ASSERT_VALID(pDocItem);
        if (pDocItem->m_itemType == CWinPrintCntrItem::typeConfiguration)
        {
            CConfigFrame* pTestFrame = new CConfigFrame ;
            if (pTestFrame == NULL)
            {
                TRACE("Warning: create of CConfigFrame failed\n") ;
                return FALSE ;
            }
            ASSERT(pTestFrame ->IsKindOf(RUNTIME_CLASS(CConfigFrame)));
            
            pTestFrame->m_strTitle = pDocItem->m_strConfigName ;
            
            // create new from resource
            if (!pTestFrame ->LoadFrame(IDR_CONFIGFRAME,
                   WS_OVERLAPPEDWINDOW,
                    NULL, &context))
            {
                TRACE0("Warning: CDocTemplate couldn't create a frame\n");
                // frame will be deleted in PostNcDestroy cleanup
                return FALSE ;
            }

            if (pDocItem->m_windowPlacement.length == sizeof(WINDOWPLACEMENT))
                pTestFrame->SetWindowPlacement( &pDocItem->m_windowPlacement ) ;

            pTestFrame->m_fInitialUpdate = TRUE ;
            pDocTemplate->InitialUpdateFrame( pTestFrame, ((CFrameWnd*)m_pMainWnd)->GetActiveDocument(), TRUE ) ;
            pTestFrame->m_fInitialUpdate = FALSE ;
        }
    }        

    ((CFrameWnd*)m_pMainWnd)->ActivateFrame() ;
    return TRUE;
}

BOOL CWinPrintApp::OnIdle(LONG lCount)
{
    // call base class idle first
    BOOL bResult = CWinApp::OnIdle(lCount);

    // then do our work
    if (m_splash.m_hWnd != NULL)
    {
        if (::GetCurrentTime() - m_dwSplashTime > 2500)
        {
            // timeout expired, destroy the splash window
            m_splash.DestroyWindow();
            m_pMainWnd->UpdateWindow();

            // NOTE: don't set bResult to FALSE,
            //  CWinApp::OnIdle may have returned TRUE
        }
        else
        {
            // check again later...
            bResult = TRUE;
        }
    }

    return bResult;
}

BOOL CWinPrintApp::PreTranslateMessage(MSG* pMsg)
{
    BOOL bResult = CWinApp::PreTranslateMessage(pMsg);

    if (m_splash.m_hWnd != NULL &&
        (pMsg->message == WM_KEYDOWN ||
         pMsg->message == WM_SYSKEYDOWN ||
         pMsg->message == WM_LBUTTONDOWN ||
         pMsg->message == WM_RBUTTONDOWN ||
         pMsg->message == WM_MBUTTONDOWN ||
         pMsg->message == WM_NCLBUTTONDOWN ||
         pMsg->message == WM_NCRBUTTONDOWN ||
         pMsg->message == WM_NCMBUTTONDOWN))
    {
        m_splash.DestroyWindow();
        m_pMainWnd->UpdateWindow();
    }

    return bResult;
}

int CWinPrintApp::ExitInstance()
{  
    //OleUninitialize() ;      
    
    if (m_pFont)
        delete m_pFont ;

    if (m_pFontBold)
        delete m_pFontBold ;
    
    FreeBitmaps() ;

    Ctl3dUnregister( AfxGetInstanceHandle() ) ;

    CMainFrame::Terminate();
     
    return CWinApp::ExitInstance() ;
}

void CWinPrintApp::LoadBitmaps( BOOL fFree )
{
    COLORREF    rgbNorm, rgbHigh ;

    rgbNorm  = GetSysColor( COLOR_WINDOW ) ;
    rgbHigh = GetSysColor( COLOR_HIGHLIGHT ) ;

    if (fFree)
        FreeBitmaps() ;                      
           
    for (int i = IDB_FIRST ; i <= IDB_LAST ; i++)
    {
        m_rgbmNorm[BMINDEX(i)] = bmLoadAndTranslate( i, rgbNorm ) ;
        m_rgbmHigh[BMINDEX(i)] = bmLoadAndTranslate( i, rgbHigh ) ;
    }
}
                        
void FreeABitmap( CBitmap* p ) 
{
    if (p)      
        delete p ;
}
                        
void CWinPrintApp::FreeBitmaps()
{
    for (int i = IDB_FIRST ; i <= IDB_LAST ; i++)
    {   
        FreeABitmap( m_rgbmNorm[BMINDEX(i)]) ;
        FreeABitmap( m_rgbmHigh[BMINDEX(i)]) ;
    }
}

void CWinPrintApp::OnAppAbout()
{
    CAboutBox about;
    about.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CWinPrintApp commands
