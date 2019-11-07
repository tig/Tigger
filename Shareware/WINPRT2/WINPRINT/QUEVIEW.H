// queview.h : interface of the CQueueView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __QUEVIEW_H__
#define __QUEVIEW_H__

#include "view.h"

class CWinPrintCntrItem;
class CWinPrintDoc ;

class CQueueView : public CListBoxView
{
protected: // create from serialization only
    CQueueView();
    DECLARE_DYNCREATE(CQueueView)

// Attributes
public:
    CWinPrintDoc* GetDocument();

// Operations
public:

// Implementation
public:
    virtual ~CQueueView();
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual void OnInitialUpdate(); // called first time after construct

// Generated message map functions
protected:
    //{{AFX_MSG(CQueueView)
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in winprvw.cpp
inline CWinPrintDoc* CQueueView::GetDocument()
   { return (CWinPrintDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif // __QUEVIEW_H__

