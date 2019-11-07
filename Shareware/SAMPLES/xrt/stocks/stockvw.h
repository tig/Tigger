// stockvw.h : interface of the CStocksView class
//
/////////////////////////////////////////////////////////////////////////////

class CStocksView : public CView
{
protected: // create from serialization only
	CStocksView();
	DECLARE_DYNCREATE(CStocksView)

// Attributes
public:
	CStocksDoc* GetDocument();

// Operations
public:

// Implementation
public:
	virtual ~CStocksView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CStocksView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in stockvw.cpp
inline CStocksDoc* CStocksView::GetDocument()
   { return (CStocksDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
