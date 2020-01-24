// item.h : header file
//
// CItem definition  
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  December 8, 1993   cek     First implementation.
//

#ifndef _ITEM_H_
#define _ITEM_H_

class CItems ;
class CColSampDoc ;
/////////////////////////////////////////////////////////////////////////////
// CItem command target

class CItem : public CCmdTarget
{   
    DECLARE_SERIAL(CItem)
public:
    CItem();           // protected constructor used by dynamic creation
    CItem( CColSampDoc* pDoc ) ;
    CItem( CItems* pParent ) ;
    CItems*         m_pParent ;
    CColSampDoc*    m_pDoc ;
// Attributes  
private:
    
public:
    VARIANT m_vtValue ;

// Operations
public:
    // Helpers required by CColSampSrvrItem
    void CalcDisplaySize( CDC* pDC, CSize& sizeItem ) ;  // get bounding rect
    int Draw( CDC* pDC, CPoint ptStart, int n = -1 ) ;
// Implementation
public:
    virtual ~CItem();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
    virtual void OnFinalRelease();

protected:
    // Generated message map functions
    //{{AFX_MSG(CItem)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    DECLARE_OLECREATE(CItem)

public:
    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CItem)
    afx_msg VARIANT GetValue();
    afx_msg void SetValue(const VARIANT FAR& newValue);
    afx_msg BSTR GetName();
    afx_msg void SetName(LPCSTR lpszNewValue);
    afx_msg LPDISPATCH GetApplication();
    afx_msg LPDISPATCH GetDocument();
    afx_msg LPDISPATCH GetParent();
    //}}AFX_DISPATCH
    CString m_szName;
    DECLARE_DISPATCH_MAP()
};

LPSTR VTtoString( const VARTYPE vt ) ;
VARTYPE StringToVT( const CString& rstr ) ;
LPSTR VariantToString( const VARIANT &v )  ;

/////////////////////////////////////////////////////////////////////////////
#endif //_ITEM_H_


 