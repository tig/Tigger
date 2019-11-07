// winprvw.h : interface of the CWinPrintView class
//
/////////////////////////////////////////////////////////////////////////////

class CWinPrintView : public CView
{
protected: // create from serialization only
	CWinPrintView();
	DECLARE_DYNCREATE(CWinPrintView)

// Attributes
public:
	CWinPrintDoc* GetDocument();

// Operations
public:

// Implementation
public:
	virtual ~CWinPrintView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CWinPrintView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG	// debug version in winprvw.cpp
inline CWinPrintDoc* CWinPrintView::GetDocument()
   { return (CWinPrintDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
