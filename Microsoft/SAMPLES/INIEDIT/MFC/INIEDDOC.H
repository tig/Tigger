// inieddoc.h : interface of the CINIeditDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CINIeditDoc : public CDocument
{
protected: // create from serialization only
    CINIeditDoc();
    DECLARE_DYNCREATE(CINIeditDoc)

// Attributes
public:

// Operations
public:

// Implementation
public:
    virtual ~CINIeditDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
protected:
    virtual BOOL    OnNewDocument();

// Generated message map functions
protected:
    //{{AFX_MSG(CINIeditDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    
private:
    UINIfile*   m_pINIfile ;
        
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
