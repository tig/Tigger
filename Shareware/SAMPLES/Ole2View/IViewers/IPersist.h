// IPersist.h : header file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1993 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
// CIPersistViewer command target

class CIPersistViewer : public CInterfaceViewer
{
	DECLARE_DYNCREATE(CIPersistViewer)
protected:
	CIPersistViewer();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPersistViewer)
	public:
//	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL
    virtual HRESULT OnView(HWND hwndParent, REFIID riid, LPUNKNOWN punk);

// Implementation
protected:
	virtual ~CIPersistViewer();

	// Generated message map functions
	//{{AFX_MSG(CIPersistViewer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CIPersistViewer)
};

/////////////////////////////////////////////////////////////////////////////
// CIPersistDlg dialog

class CIPersistDlg : public CDialog
{
// Construction
public:
	CIPersistDlg(UINT uiID = 0, CWnd* pParent = NULL);   // standard constructor

    IUnknown*       m_punk ;
    IID             m_iid ;

// Dialog Data
	//{{AFX_DATA(CIPersistDlg)
	enum { IDD = IDD_IPERSIST };
	CString	m_strCLSID;
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPersistDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIPersistDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CIPersistStreamDlg dialog
class CIPersistStreamDlg : public CIPersistDlg
{
// Construction
public:
	CIPersistStreamDlg(UINT uiID = 0, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIPersistStreamDlg)
	enum { IDD = IDD_IPERSISTSTREAMINIT };
	CString	m_strIsDirty;
	CString	m_strSizeMax;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPersistStreamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIPersistStreamDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnIsDirty();
	afx_msg void OnGetSizeMax();
	afx_msg void OnInitNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

