// flistvw.h : header file
//
          
#ifndef _FLISTVW_H_
#define _FLISTVW_H_          

/////////////////////////////////////////////////////////////////////////////
// CFListView form view

#include "flistdoc.h"

class CFListView : public CView
{
protected:
    CFListView();           
    DECLARE_DYNCREATE(CFListView)

// Attributes
public:
    CFListDoc* GetDocument() ;
    CListBox*  GetListBox() ;
    
// Operations
public:

// Implementation
public:
    virtual ~CFListView();
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    
    
    
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Printing support
protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

private:
    CListBox    m_listboxFiles;

protected:
    // Generated message map functions
    //{{AFX_MSG(CFListView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in iniedvw.cpp
inline CFListDoc* CFListView::GetDocument()
   { return (CFListDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

#endif //_FLISTVW_H_
