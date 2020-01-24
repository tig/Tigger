// StgEditView.h : interface of the CStgEditView class
//
/////////////////////////////////////////////////////////////////////////////

class CStgEditView : public CView
{
protected: // create from serialization only
	CStgEditView();
	DECLARE_DYNCREATE(CStgEditView)

// Attributes
public:
	CStgEditDoc* GetDocument();
	CTreeCtrl	m_tree ;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStgEditView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStgEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CImageList	 m_images ;

// Generated message map functions
protected:
	//{{AFX_MSG(CStgEditView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
    afx_msg void OnTreeSelchanged(NMHDR* pNMHDR, LRESULT* pResult) ;
    afx_msg void OnTreeItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) ;
    afx_msg void OnTreeItemReturn(NMHDR* pNMHDR, LRESULT* pResult) ;
    afx_msg void OnTreeDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) ;
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in StgEditView.cpp
inline CStgEditDoc* CStgEditView::GetDocument()
   { return (CStgEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
