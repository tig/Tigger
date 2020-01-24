// autoddoc.h : interface of the CAutoDudeDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _AUTODOC_H_
#define _AUTODOC_H_

class CAutoDudeDoc : public CDocument
{
protected: // create from serialization only
    CAutoDudeDoc();
    DECLARE_DYNCREATE(CAutoDudeDoc)

// Attributes
public:

// Operations
public: 
    void SetText( const CString& sz )
        { m_Text = sz ;  UpdateAllViews( NULL ) ; }   
        
    CString GetText( void )
        { return m_Text ; } 

private:
    CString     m_Text ;

// Implementation
public:
    virtual ~CAutoDudeDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
protected:
    virtual BOOL    OnNewDocument();

// Generated message map functions
protected:
    //{{AFX_MSG(CAutoDudeDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif //_AUTODOC_H_
