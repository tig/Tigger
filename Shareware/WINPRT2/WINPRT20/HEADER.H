// header.h : header file
//
#ifndef __HEADER_H__
#define __HEADER_H__

#include "headerse.h" 

/////////////////////////////////////////////////////////////////////////////
// CHeader command target
class CWinPrintControl ;

class CHeader : public CControlProperty
{   
    DECLARE_DYNCREATE(CHeader)
public:
    CHeader();
    virtual ~CHeader() ;
    
// Attributes
public:
    CWinPrintControl*   m_pControl ;

// Operations
public:
    virtual void OnResetState() ;
    BOOL Recalc( CDC* pDC ) ;
    void Draw( CDC *pDC, const CRect& rc ) ;

// Overrides

    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CHeader)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual void DoPropExchange(CPropExchange* pPX); 

    // Generated message map functions
    //{{AFX_MSG(CHeader)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CHeader)
    float m_flHeight;
    afx_msg void OnHeightChanged();
    afx_msg LPDISPATCH GetLeft();
    afx_msg void SetLeft(LPDISPATCH newValue);
    afx_msg LPDISPATCH GetCenter();
    afx_msg void SetCenter(LPDISPATCH newValue);
    afx_msg LPDISPATCH GetRight();
    afx_msg void SetRight(LPDISPATCH newValue);
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()

    DECLARE_CONTROLPROP(CHeader)

public:
    int             m_iHeight ;     // Calculated 
    CHeaderSection  m_Left ;
    CHeaderSection  m_Center ;
    CHeaderSection  m_Right ;
};

/////////////////////////////////////////////////////////////////////////////
    
#endif

