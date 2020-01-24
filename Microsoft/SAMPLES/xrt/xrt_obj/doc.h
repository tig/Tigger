// doc.h : interface of the CXRTData class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _DOC_H_
#define _DOC_H_

class CXRTData : public CDocument
{
protected: // create from serialization only
    CXRTData();
    DECLARE_DYNCREATE(CXRTData)

// Attributes
public:
    UINT  m_uiTimeout ;  
    char  m_sz[256] ;
    
    LPXRTSTOCKS m_lpQuotes ;
    UINT m_uiQuoteProb, m_uiElementProb ;    
// Operations
public:
    void GetNewData( ) ;

// Implementation
public:
    virtual ~CXRTData();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
protected:
    virtual BOOL    OnNewDocument();

// Generated message map functions
protected:
    //{{AFX_MSG(CXRTData)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
