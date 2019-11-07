// coltedoc.h : interface of the CColtestDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CColtestDoc : public CDocument
{
protected: // create from serialization only
    CColtestDoc();
    DECLARE_DYNCREATE(CColtestDoc)

// Attributes
public:
    CString m_strFileSpec ;

// Operations
public:

// Implementation
public:
    virtual ~CColtestDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
protected:
    virtual BOOL    OnNewDocument();
    virtual BOOL    OnOpenDocument(const char* pszPathName) ;
    
// Generated message map functions
protected:
    //{{AFX_MSG(CColtestDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
