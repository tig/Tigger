// cfglstvw.h : interface of the CConfigListView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CFGLSTVW_H__
#define __CFGLSTVW_H__

#include "view.h"
class CConfigFrame ;

class CWinPrintCntrItem;

class CConfigListView : public CListBoxView
{
protected: // create from serialization only
    CConfigListView();
    DECLARE_DYNCREATE(CConfigListView)

// Constants
public:
    enum UpdateFlags
    {
        updateDefault =     0x0000,
        updateReset =       0x0001,
    } ;

// Attributes
public:
    CWinPrintDoc* GetDocument();
    
// Operations
public:

// Implementation
public:
    virtual ~CConfigListView();
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual void OnInitialUpdate(); // called first time after construct
    virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) ;

// Generated message map functions
protected:
    //{{AFX_MSG(CConfigListView)
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    afx_msg void OnDblClk();
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in winprvw.cpp
inline CWinPrintDoc* CConfigListView::GetDocument()
   { return (CWinPrintDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif // __CFGLSTVW_H__
