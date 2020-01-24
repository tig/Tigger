// inieddoc.h : interface of the CINIeditDoc class
//
/////////////////////////////////////////////////////////////////////////////
               
#ifndef _INIEDDOC_H_
#define _INIEDDOC_H_
               
class CINIeditDoc : public CDocument
{
protected: 
    CINIeditDoc();
    DECLARE_DYNCREATE(CINIeditDoc)

// Attributes
public:

// Operations
public:

// Implementation
public:
    virtual ~CINIeditDoc();
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
    DECLARE_MESSAGE_MAP()
        
private:
    UINIfile*   m_pINIfile ;

};

/////////////////////////////////////////////////////////////////////////////
#endif // _INIEDDOC_H_
