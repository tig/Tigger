// mainfrm.h : interface of the CConfigFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CFGFRAME_H__
#define __CFGFRAME_H__

class CConfigView ;

class CConfigFrame : public CFrameWnd
{
friend class CWinPrintApp ;

public: // create from serialization only
    CConfigFrame();
    DECLARE_DYNCREATE(CConfigFrame)

// Attributes
public:
    BOOL  m_fInitialUpdate ;

// Operations
public:

// Implementation
public:
    BOOL Create(LPCSTR lpszClassName,
    LPCSTR lpszWindowName,
    DWORD dwStyle,
    const RECT& rect,
    CWnd* pParentWnd,
    LPCSTR lpszMenuName,
    DWORD dwExStyle,
    CCreateContext* pContext) ;

    virtual ~CConfigFrame();
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
                        CWnd* pParentWnd, CCreateContext* pContext);
#if 0                
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
#endif
    virtual void ActivateFrame(int nCmdShow = -1);
    
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
    CToolBar    m_wndToolBar;
    void OnSysSaveMinPos() ;
    void OnSysAlwaysOnTop();

// Generated message map functions
protected:
    //{{AFX_MSG(CConfigFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSysColorChange();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
