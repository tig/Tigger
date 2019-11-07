// flistdoc.h : header file
//

#ifndef _FLISTDOC_H_
#define _FLISTDOC_H_

/////////////////////////////////////////////////////////////////////////////
// CFListDoc document

class CFListDoc : public CDocument
{
protected:
    CFListDoc();         
    DECLARE_DYNCREATE(CFListDoc)
    
// Attributes
public:

// Operations
public:
    UINIfileList*  GetFileList();
    
// Implementation
protected:
    virtual ~CFListDoc();
    virtual BOOL OnNewDocument();
            

    // Generated message map functions
protected:
    //{{AFX_MSG(CFListDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    
public:
    UINIfileList*    m_pINIfiles ;
    
};

#endif // _FLISTDOC_H_

