// cfgview.h : interface of the CConfigView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CFGVIEW_H__
#define __CFGVIEW_H__

class CWinPrintCntrItem;

class CConfigView : public CView
{
protected: // create from serialization only
    CConfigView();
    DECLARE_DYNCREATE(CConfigView)

// Attributes
public:
    
    CWinPrintDoc* GetDocument();

    // m_pItem holds the selection to the CWinPrintCntrItem for this
    // view.
    CWinPrintCntrItem* m_pItem;

// Operations
public:
    void SetItem(CWinPrintCntrItem* pItem);

// Implementation
public:
    virtual ~CConfigView();
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual void OnInitialUpdate(); // called first time after construct
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

    // OLE Container support
    virtual BOOL IsSelected(const CObject* pDocItem) const;

// Generated message map functions
protected:
    //{{AFX_MSG(CConfigView)
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnCancelEdit();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in winprvw.cpp
inline CWinPrintDoc* CConfigView::GetDocument()
   { return (CWinPrintDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif //__CFGVIEW_H__

