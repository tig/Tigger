// mainwnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMainWnd frame

class CMainWnd : public CFrameWnd
{
    DECLARE_DYNCREATE(CMainWnd)

public:
    CMainWnd() ;
    
    BOOL SavePosition() ;
    BOOL RestorePosition() ;

// Attributes
public:

// Operations
public:

// Implementation
protected:
    virtual ~CMainWnd();

    // Generated message map functions
    //{{AFX_MSG(CMainWnd)
	afx_msg void OnDestroy();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
