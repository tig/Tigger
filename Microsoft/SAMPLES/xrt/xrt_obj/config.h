// config.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXRTConfigView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CXRTConfigView : public CFormView
{
    DECLARE_DYNCREATE(CXRTConfigView)
protected:
    CXRTConfigView();           // protected constructor used by dynamic creation

// Form Data
public:
    //{{AFX_DATA(CXRTConfigView)
    enum { IDD = IDR_CONFIGVIEW };
    CColumnListBox    m_lbInstruments;
    UINT     m_uiTimeout;
    BOOL    m_fShowChanges;
    //}}AFX_DATA

    CXRTObjectView*     m_pClipboardObjectView ;
    
// Attributes
public:
    CXRTData* GetDocument();

// Operations
public:

// Implementation
protected:
    virtual ~CXRTConfigView();
    virtual BOOL PreCreateWindow( CREATESTRUCT &cs )  ;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnInitialUpdate();
    virtual void OnUpdate(CView*, LPARAM, CObject*) ;
    // Generated message map functions
    //{{AFX_MSG(CXRTConfigView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClickedSet();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnEditCopy();
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnEditClear();
    afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
    afx_msg void OnClickedShowChanges();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in object.cpp
inline CXRTData* CXRTConfigView::GetDocument()
   { return (CXRTData*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
