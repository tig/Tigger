// view.h : interface of the CColSampView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _VIEW_H_
#define _VIEW_H_

#include "collist.h"

class CColSampView : public CView
{
protected: // create from serialization only
    CColSampView();
    DECLARE_DYNCREATE(CColSampView)

// Attributes
public:
    CColSampDoc* GetDocument();
    CColumnListBox  m_lb ; 
    CFont m_font ;

// Operations
public:

// Implementation
public:
    virtual ~CColSampView();
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
    //{{AFX_MSG(CColSampView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnRemoveItem();
    afx_msg void OnUpdateRemoveItem(CCmdUI* pCmdUI);
    afx_msg void OnChangeItem();
    afx_msg void OnUpdateChangeItem(CCmdUI* pCmdUI);
    afx_msg void OnAddItem();
    afx_msg void OnUpdateAddItem(CCmdUI* pCmdUI);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
    virtual void OnDblClk();
    virtual void OnColumnChange();
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in stockvw.cpp
inline CColSampDoc* CColSampView::GetDocument()
   { return (CColSampDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeItemDlg dialog

class CChangeItemDlg : public CDialog
{
// Construction
public:
    CChangeItemDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CChangeItemDlg)
    enum { IDD = IDD_ADDITEM };
    CString m_szName;
    CString m_szValue;
    CString m_szType;
    //}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Generated message map functions
    //{{AFX_MSG(CChangeItemDlg)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
#endif // _VIEW_

