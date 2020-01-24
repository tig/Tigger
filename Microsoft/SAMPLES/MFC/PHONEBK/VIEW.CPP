#include <afxwin.h>
#include "resource.h"
#include "view.h"

CTheApp     theApp ;

// CMainWindow member functions
//

// Message map
BEGIN_MESSAGE_MAP(CMainWindow, CFrameWnd)
    ON_WM_PAINT()
    ON_COMMAND(IDM_ABOUT, OnAbout)
END_MESSAGE_MAP()


// CMainWindow:: contructor
CMainWindow::CMainWindow()
{
    LoadAccelTable( "MainAccelTable" ) ;
    Create( NULL, "Hello Foundation Application", WS_OVERLAPPEDWINDOW, 
            rectDefault, NULL, "MainMenu" ) ;

}

void CMainWindow::OnPaint()
{
    CString     s = "Hello, Windows!" ;
    CPaintDC    dc( this ) ;
    CRect       rect ;

    GetClientRect( rect ) ;
    dc.SetTextAlign( TA_BASELINE | TA_CENTER )  ;
    dc.SetBkMode( TRANSPARENT ) ;
    dc.TextOut( rect.right / 2, rect.bottom / 2, s, s.GetLength() ) ;
}
  
void CMainWindow::OnAbout()
{
    CModalDialog    about( "AboutBox", this ) ;
    about.DoModal() ;
}


// CTheApp member functions
//
//
BOOL CTheApp::InitInstance()
{
    // Construct a window object in the heap
    m_pMainWnd = new CMainWindow() ;

    // Show the window
    m_pMainWnd->ShowWindow( m_nCmdShow ) ;

    // Paint the window
    m_pMainWnd->UpdateWindow() ;

    return TRUE ;
}
