// headerse.h : header file
//

#ifndef __HEADERSE_H__
#define __HEADERSE_H__

/////////////////////////////////////////////////////////////////////////////
// CHeaderSection command target
class CHeader ;

class CHeaderSection : public CControlProperty
{   
    DECLARE_DYNCREATE(CHeaderSection)
public:
    CHeaderSection( );           // protected constructor used by dynamic creation
    virtual ~CHeaderSection();

// Attributes
public:

// Operations
public:
    CHeader*    m_pHeader ;
    virtual void OnResetState() ;
   
    CString& GetExpandedString() ;
    LPSTR ExpandMeta( char c, LPSTR pD ) ;
        
// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CHeaderSection)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual void DoPropExchange(CPropExchange* pPX);

    // Generated message map functions
    //{{AFX_MSG(CHeaderSection)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CHeaderSection)
    CString m_strText;
    afx_msg void OnTextChanged();
    afx_msg LPFONTDISP GetFont();
    afx_msg void SetFont(LPFONTDISP newValue);
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()

    DECLARE_CONTROLPROP(CHeaderSection)

    CFontHolder m_Font ;
    void OnFontChanged() ;
    BEGIN_INTERFACE_PART( FontNotify, IPropertyNotifySink ) 
        STDMETHOD(OnChanged)(DISPID dispid);
        STDMETHOD(OnRequestEdit)(DISPID dispid);
    END_INTERFACE_PART( FontNotify )

};

/////////////////////////////////////////////////////////////////////////////
    
#endif //__HEADERSE_H__

