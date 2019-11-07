// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "xrt_obj.h"

#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    // TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}
 
BOOL CMainFrame::PreCreateWindow( CREATESTRUCT &cs ) 
{
    cs.style   &=~(LONG)FWS_ADDTOTITLE ;
    return CMDIFrameWnd::PreCreateWindow( cs ) ;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

void CMainFrame::OnDestroy()
{   
    if (!(((CXRTApp*)AfxGetApp())->m_fEmbedding || ((CXRTApp*)AfxGetApp())->m_fAutomation) )
    {
        CString szKey ;
        szKey.LoadString( IDS_INI_MAINWNDPOS ) ;
        SavePosition( this, szKey) ;            
    }
    
    CMDIFrameWnd::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


