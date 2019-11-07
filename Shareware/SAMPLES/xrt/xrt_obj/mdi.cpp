// mdi.cpp : implementation file
//

#include "stdafx.h"
#include "xrt_obj.h"
#include "mdi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigMDIChild

IMPLEMENT_DYNCREATE(CConfigMDIChild, CMDIChildWnd)

CConfigMDIChild::CConfigMDIChild()
{
}

CConfigMDIChild::~CConfigMDIChild()
{
}

BOOL CConfigMDIChild::PreCreateWindow( CREATESTRUCT &cs ) 
{
    cs.style   &=~(LONG)FWS_ADDTOTITLE ;
    
    return CMDIChildWnd::PreCreateWindow( cs ) ;
}

BEGIN_MESSAGE_MAP(CConfigMDIChild, CMDIChildWnd)
    //{{AFX_MSG_MAP(CConfigMDIChild)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SYSCOMMAND()
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CConfigMDIChild::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    GetSystemMenu( FALSE )->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND | MF_GRAYED ) ;

    return 0;
}

void CConfigMDIChild::OnDestroy()
{
    CMDIChildWnd::OnDestroy();
    
}

void CConfigMDIChild::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID != SC_CLOSE)
        CMDIChildWnd::OnSysCommand(nID, lParam);
}

void CConfigMDIChild::OnSize(UINT nType, int cx, int cy)
{
    CMDIChildWnd::OnSize(nType, cx, cy);

/*
    CWnd* pWnd = GetDescendantWindow(AFX_IDW_PANE_FIRST);
    if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CXRTObjectView)) && pWnd->IsWindowVisible())
    {
        pWnd->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE |SWP_NOZORDER ) ;
    }
*/    
}


/////////////////////////////////////////////////////////////////////////////
// CObjectMDIChild
BEGIN_MESSAGE_MAP(CObjectMDIChild, CMDIChildWnd)
    //{{AFX_MSG_MAP(CObjectMDIChild)
    ON_WM_CREATE()
    ON_WM_SYSCOMMAND()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


IMPLEMENT_DYNCREATE(CObjectMDIChild, CMDIChildWnd)

CObjectMDIChild::CObjectMDIChild()
{
}

CObjectMDIChild::~CObjectMDIChild()
{
}
       
BOOL CObjectMDIChild::PreCreateWindow( CREATESTRUCT &cs ) 
{
    cs.style   &=~(LONG)FWS_ADDTOTITLE ;
//    cs.style |= WS_MINIMIZE ;
    return CMDIChildWnd::PreCreateWindow( cs ) ;
}

int CObjectMDIChild::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    GetSystemMenu( FALSE )->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND | MF_GRAYED ) ;
    return 0;
}


void CObjectMDIChild::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID != SC_CLOSE)
        CMDIChildWnd::OnSysCommand(nID, lParam);
}

