// winprdoc.h : interface of the CWinPrintDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __WINPRDOC_H__
#define __WINPRDOC_H__

class CWinPrintDoc : public COleDocument
{
friend class CWinPrintCntrItem ;
protected: // create from serialization only
    CWinPrintDoc();
    DECLARE_DYNCREATE(CWinPrintDoc)

// Attributes
public:
    enum OnUpdateHints
    {  
        updateWindow =      0,
        updateItem =        1
    } ;
    
// Operations
public:

// Implementation
public:
    virtual ~CWinPrintDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    //{{AFX_MSG(CWinPrintDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CWinPrintDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif //__WINPRDOC_H__
