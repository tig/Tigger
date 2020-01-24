// object.h : interface of the CXRTObjectView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "dataobj.h"

class CXRTObjectView : public CView
{
protected: // create from serialization only
    CXRTObjectView();
    DECLARE_DYNCREATE(CXRTObjectView)

// Attributes
public:
    LPCXRTDataObject    m_pXRTDataObject ;
    CXRTData*           GetDocument();

    BOOL                m_fStatic ;   // does not get updated unless forced (for clipboard)
    
    CColumnListBox      m_lb ;
    
// Operations
public:
    HRESULT CreateDataObject( LPUNKNOWN punkOuter, LPUNKNOWN *ppUnk ) ;
    HRESULT DestroyDataObject( LPUNKNOWN pUnk ) ;    
    
    virtual void OnInitialUpdate();
    virtual void OnUpdate(CView*, LPARAM, CObject*) ;
// Implementation
public:
    virtual ~CXRTObjectView();
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
    //{{AFX_MSG(CXRTObjectView)
    afx_msg void OnDestroy();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in object.cpp
inline CXRTData* CXRTObjectView::GetDocument()
   { return (CXRTData*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif 

