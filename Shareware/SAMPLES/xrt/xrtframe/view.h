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
//

#ifndef _VIEW_H_
#define _VIEW_H_

#include "collist.h"

// CDataObjetView::OnUpdate lHint flags
//
#define UPDATE_ALL                  0       
#define UPDATE_ADDREQUEST           1       // Request object was added
#define UPDATE_CHANGEREQUEST        2       // Request object was changed
#define UPDATE_REMOVEREQUEST        3       // Request object was deleted
#define UPDATE_REQUEST_COLCHANGE    4   
#define UPDATE_CHANGEPROP           5

#define UPDATE_MARKETCHANGE         10       // Market changed
#define UPDATE_DATA_COLCHANGE       11
#define UPDATE_ADDDATAITEM          12

#define UPDATE_TIMEOUT              15

class CProperty ;
class CDataItem ;
class CDataObjectView : public CView
{
protected: // create from serialization only
    CDataObjectView();
    DECLARE_DYNCREATE(CDataObjectView)

// Attributes
public:
    CDataObject* GetDocument();
    CFont   m_font ;

    struct tagRequests
    {
        CColumnListBox  lb ; 
    } m_Requests ;

    struct tagData
    {
        CColumnListBox  lb ; 
    } m_Data ;
    
// Operations
public:
    void SetSelection( CRequest* pRequest ) ;
    void SetSelection( CProperty* pProperty ) ;
    void SetSelection( CDataItem* pDataItem ) ;

// Implementation
public:
    ~CDataObjectView() ;
    
    virtual void OnDraw( CDC* pDC ) ;

    void DoInitialUpdate() ;
    virtual void OnInitialUpdate() ;

    void DoUpdateRequests( CView* pSender, LPARAM lHint, CObject* pHint );
    void DoUpdateData( CView* pSender, LPARAM lHint, CObject* pHint );
    virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) ;

    void DoDrawItemRequests( LPDRAWITEMSTRUCT lpDIS ) ;
    void DoDrawItemData( LPDRAWITEMSTRUCT lpDIS ) ;    
    
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    BOOL  m_fInitialUpdate ;
// Generated message map functions
protected:
    //{{AFX_MSG(CDataObjectView)
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
    afx_msg void OnEditCopy();
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnOptionsProperties();
    //}}AFX_MSG
    virtual void OnDblClkRequests();
    virtual void OnColumnChangeRequests();
    virtual void OnColumnChangeData();
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in stockvw.cpp
inline CDataObject* CDataObjectView::GetDocument()
   { return (CDataObject*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeRequestDlg dialog

class CChangeRequestDlg : public CDialog
{
// Construction
public:
    CChangeRequestDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CChangeRequestDlg)
    enum { IDD = IDD_ADDITEM };
    CString m_strRequest;
    //}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Generated message map functions
    //{{AFX_MSG(CChangeRequestDlg)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CDataObjPropsDlg dialog

class CDataObjPropsDlg : public CDialog
{
// Construction
public:
    CDataObjPropsDlg(CWnd* pParent = NULL); // standard constructor

// Dialog Data
    //{{AFX_DATA(CDataObjPropsDlg)
	enum { IDD = IDD_DATAOBJECTPROPS };
    CString m_strName;
    CString m_strUsername;
    CString m_strPassword;
    UINT    m_uiUpdateFrequency;
	BOOL	m_fMostRecentOnly;
	//}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Generated message map functions
    //{{AFX_MSG(CDataObjPropsDlg)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


#endif // _VIEW_

