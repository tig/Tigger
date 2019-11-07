// autodvw.h : interface of the CAutoDudeView class
//
/////////////////////////////////////////////////////////////////////////////
         
#ifndef _AUTODVW_H_
#define _AUTODVW_H_         
class CAutoDudeView : public CView
{
protected: // create from serialization only
    CAutoDudeView();
    DECLARE_DYNCREATE(CAutoDudeView)

// Attributes
public:
    CAutoDudeDoc* GetDocument();

// Operations
public:

// Implementation
public:
    virtual ~CAutoDudeView();
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
    //{{AFX_MSG(CAutoDudeView)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in autodvw.cpp
inline CAutoDudeDoc* CAutoDudeView::GetDocument()
   { return (CAutoDudeDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif // _AUTODVW_H_
