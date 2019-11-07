// mdi.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigMDIChild frame

class CConfigMDIChild : public CMDIChildWnd
{
    DECLARE_DYNCREATE(CConfigMDIChild)
protected:
    CConfigMDIChild();          // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
    virtual ~CConfigMDIChild();
    virtual BOOL PreCreateWindow( CREATESTRUCT &cs )  ;

    // Generated message map functions
    //{{AFX_MSG(CConfigMDIChild)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CObjectMDIChild frame

class CObjectMDIChild : public CMDIChildWnd
{
    DECLARE_DYNCREATE(CObjectMDIChild)
protected:
    CObjectMDIChild();          // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
    virtual ~CObjectMDIChild();
    virtual BOOL PreCreateWindow( CREATESTRUCT &cs )  ;

    // Generated message map functions
    //{{AFX_MSG(CObjectMDIChild)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
