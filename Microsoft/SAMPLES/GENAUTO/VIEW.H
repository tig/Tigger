// view.h : interface of the CGenAutoView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _VIEW_H_
#define _VIEW_H_

class CGenAutoView : public CView
{
protected: // create from serialization only
    CGenAutoView();
    DECLARE_DYNCREATE(CGenAutoView)

// Attributes
public:
    CGenAutoDoc* GetDocument();
    CFont m_font ;

// Operations
public:

// Implementation
public:
    virtual ~CGenAutoView();
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual void OnInitialUpdate() ;
    virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) ;
        
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    BOOL  m_fInitialUpdate ;
// Generated message map functions
protected:
    //{{AFX_MSG(CGenAutoView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in stockvw.cpp
inline CGenAutoDoc* CGenAutoView::GetDocument()
   { return (CGenAutoDoc*)m_pDocument; }
#endif

#endif // _VIEW_

