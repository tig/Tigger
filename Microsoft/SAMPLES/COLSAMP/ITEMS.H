// items.h : header file
//
// CItems, CEnumItems definition 
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

#ifndef _ITEMS_H_
#define _ITEMS_H_

/////////////////////////////////////////////////////////////////////////////
// CItems command target
class CColSampDoc ;

class CItems : public CCmdTarget
{   
    DECLARE_DYNCREATE(CItems)
public:
    CItems();         // protected constructor used by dynamic creation
    CItems( const CColSampDoc* pDoc );

// Attributes
private:
    CColSampDoc*    m_pDoc ;        // Owning document, NULL if  none.
    
public:
    CColSampDoc* GetDoc() ;

// Operations
public:

// Implementation
public:
    virtual ~CItems();
    virtual void OnFinalRelease();

protected:
    // Generated message map functions
    //{{AFX_MSG(CItems)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    DECLARE_DISPATCH_MAP()

public:    
    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CItems)
    afx_msg void Remove(const VARIANT FAR& Item);
    afx_msg VARIANT Add(const VARIANT FAR& vtName, const VARIANT FAR& vtValue );
    afx_msg long GetCount();
    //}}AFX_DISPATCH
    afx_msg LPDISPATCH GetItem(const VARIANT FAR& ItemNameOrIndex);
    
protected:
    afx_msg LPUNKNOWN _NewEnum();
};
  
  
// Declaraction of the class that implements IEnumVARIANT
//
class CEnumItems : public CCmdTarget
{   
public:
    CEnumItems();         // protected constructor used by dynamic creation
    CEnumItems( CColSampDoc* pDoc );

// Attributes
private:
    CColSampDoc*    m_pDoc ;        // Owning document, NULL if  none.
    
// Operations
public:

// Implementation
public:
    virtual ~CEnumItems();
    virtual void OnFinalRelease();

protected:
    DECLARE_INTERFACE_MAP()

public:    
    // The following block get's expanded by the C++ preprocessor to this:
    //
    //  class FAR XEnumVARIANT : public IEnumVARIANT
    //  {
    //  public:
    //      STDMETHOD_(ULONG, AddRef)(); 
    //      STDMETHOD_(ULONG, Release)();
    //      STDMETHOD(QueryInterface)(REFIID iid, LPVOID far* ppvObj); 
    //      XEnumVARIANT() ;      
    //      POSITION   m_posCurrent ; 
    //      ...
    //
    //  } m_xEnumVARIANT ;
    //  friend class XEnumVARIANT ;
    //      
    BEGIN_INTERFACE_PART(EnumVARIANT, IEnumVARIANT)
        XEnumVARIANT() ;        // constructor to set m_posCurrent
        POSITION   m_posCurrent ; // Next() requires that we keep track of our current item

        STDMETHOD(Next)(THIS_ unsigned long celt, VARIANT FAR* rgvar, unsigned long FAR* pceltFetched);
        STDMETHOD(Skip)(THIS_ unsigned long celt) ;
        STDMETHOD(Reset)(THIS) ;
        STDMETHOD(Clone)(THIS_ IEnumVARIANT FAR* FAR* ppenum) ;
    END_INTERFACE_PART(EnumVARIANT)    

} ;

/////////////////////////////////////////////////////////////////////////////
#endif //_ITEMS_H_
