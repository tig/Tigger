// propview.h : interface of the CPropView class
//
/////////////////////////////////////////////////////////////////////////////

class CPropView : public CView
{
protected: // create from serialization only
	CPropView();
	DECLARE_DYNCREATE(CPropView)

// Attributes
public:
	CPropDoc* GetDocument();

// Operations
public:

// Implementation
public:
	virtual ~CPropView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CPropView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG	// debug version in propview.cpp
inline CPropDoc* CPropView::GetDocument()
   { return (CPropDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
