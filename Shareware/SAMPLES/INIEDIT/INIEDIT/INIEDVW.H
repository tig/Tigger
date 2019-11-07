// iniedvw.h : interface of the CINIeditView class
//
/////////////////////////////////////////////////////////////////////////////
         
#ifndef _INIEDVW_H_         
#define _INIEDVW_H_
        
#include "inieddoc.h"
         
class CINIeditView : public CView
{
protected: // create from serialization only
    CINIeditView();
    DECLARE_DYNCREATE(CINIeditView)

// Attributes
public:
    CINIeditDoc* GetDocument();

// Operations
public:

// Implementation
public:
    virtual ~CINIeditView();
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

// Generated message map functions
protected:
    //{{AFX_MSG(CINIeditView)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in iniedvw.cpp
inline CINIeditDoc* CINIeditView::GetDocument()
   { return (CINIeditDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif // _INIEDVW_H_

