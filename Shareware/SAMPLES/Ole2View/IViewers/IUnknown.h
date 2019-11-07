// IUnknown.h : header file
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
// CIUnknownViewer command target

class CIUnknownViewer : public CInterfaceViewer
{
	DECLARE_DYNCREATE(CIUnknownViewer)
protected:
	CIUnknownViewer();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIUnknownViewer)
	public:
//	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL
    virtual HRESULT OnView(HWND hwndParent, REFIID riid, LPUNKNOWN punk);

// Implementation
protected:
	virtual ~CIUnknownViewer();

	// Generated message map functions
	//{{AFX_MSG(CIUnknownViewer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CIUnknownViewer)
};

/////////////////////////////////////////////////////////////////////////////
// CIUnknownDlg dialog

class CIUnknownDlg : public CDialog
{
// Construction
public:
	CIUnknownDlg(CWnd* pParent = NULL);   // standard constructor

    IUnknown*       m_punk ;
    IID             m_iid ;

// Dialog Data
	//{{AFX_DATA(CIUnknownDlg)
	enum { IDD = IDD_IUNKNOWN };
	CButton	m_btnViewTypeInfo;
	CString	m_strIID;
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIUnknownDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIUnknownDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnViewTypeInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
