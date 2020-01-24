// WOSA/XRT XRTFrame Sample Application Version 1.00.007
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// view.h : interface of the CDataObjectView and CDataView class
//
// We have 2 view implementations on our CDataObject document class:
//  One for our "Request list" and one for the real-time data.
// 
//  view.* implements CDataObjectView
//  dataview.* implements CDataView
//

#ifndef _DATAVIEW_H_
#define _DATAVIEW_H_

#include "collist.h"
class CProperty ;
class CDataItem ;

class CDataView : public CView
{
protected: // create from serialization only
    CDataView();
    ~CDataView() ;
    DECLARE_DYNCREATE(CDataView)

// Attributes
public:
    CDataObject* GetDocument();
    CColumnListBox  m_lb ; 
    CFont m_font ;

    CFont m_smallfont ;
    int m_cySmallFont ;

// Operations
public:
    void SetSelection( CDataItem* pDataItem ) ;
    void SetSelection( CProperty* pProperty ) ;
    void ChangeUpdateFrequencyTimer( UINT uiFreq ) ;

// Implementation
public:
    virtual void OnDraw( CDC* pDC );
    virtual void OnInitialUpdate() ;
    virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) ;
        
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    BOOL  m_fInitialUpdate ;
    UINT  m_nIDTimerEvent ;
    
// Generated message map functions
protected:
    //{{AFX_MSG(CDataView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnEditCopy();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    //}}AFX_MSG
    virtual void OnColumnChange();
    DECLARE_MESSAGE_MAP()
    
    friend class CDataObject ;
};

#ifndef _DEBUG  // debug version in stockvw.cpp
inline CDataObject* CDataView::GetDocument()
   { return (CDataObject*)m_pDocument; }
#endif

#endif // _DATAVIEW_H_