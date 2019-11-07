// StorageView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStorageView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CStorageView : public CFormView
{
protected:
	CStorageView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CStorageView)

// Form Data
public:
	//{{AFX_DATA(CStorageView)
	enum { IDD = IDD_STORAGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStorageView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CStorageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CStorageView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
