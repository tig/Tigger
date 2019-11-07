// coltevw.h : interface of the CColtestView class
//
/////////////////////////////////////////////////////////////////////////////

class CColtestView : public CView
{
protected: // create from serialization only
    CColtestView();
    DECLARE_DYNCREATE(CColtestView)

// Attributes
public:
    CColumnListBox m_lb ; 
    CFont           m_Font ;
    CColtestDoc* GetDocument();

// Operations
public:

// Implementation
public:
    virtual ~CColtestView();
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual void OnInitialUpdate() ;
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
    //{{AFX_MSG(CColtestView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in coltevw.cpp
inline CColtestDoc* CColtestView::GetDocument()
   { return (CColtestDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
