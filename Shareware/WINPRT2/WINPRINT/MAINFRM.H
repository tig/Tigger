// mainfrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MAINFRM_H__
#define __MAINFRM_H__

class CConfigListView ;
class CQueueView ;

class CMainFrame : public CFrameWnd
{
friend class CWinPrintApp ;

protected: // create from serialization only
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

    BOOL SavePosition() ;
    BOOL RestorePosition() ;

// Attributes
public:
    CConfigListView*    m_pConfigListView ;
    CQueueView*         m_pQueueView ;

// Operations
public:
    static void Initialize();
    static void Terminate();

// Implementation
public:
    virtual ~CMainFrame();
    virtual BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
                CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
    void ActivateFrame(int nCmdShow);
    
    CStatusBar  m_wndStatusBar;
    CToolBar    m_wndToolBar;
    CSplitterWnd m_wndSplitter ; 

    static int  m_nDefCmdShow;
    static int  m_nDefCmdShowOld;
    BOOL        m_fSaveMinPos  ;
    BOOL        m_fTopMost    ;
// Generated message map functions
protected:
    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSysColorChange();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnOptionsPreferences();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
