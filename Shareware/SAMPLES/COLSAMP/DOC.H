// doc.h : interface of the CStocksDoc class
//
// CColSampDoc definition
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

#ifndef _DOC_H_
#define _DOC_H_

#include "collist.h"

class CColSampSrvrItem;
class CItems ; 
class CItem ;

class CColSampDoc : public COleServerDoc
{
protected: // create from serialization only
    CColSampDoc();
    DECLARE_DYNCREATE(CColSampDoc)

// Attributes
public:
    CColSampSrvrItem* GetEmbeddedItem()
        { return (CColSampSrvrItem*)COleServerDoc::GetEmbeddedItem(); }

public:
    long        m_nUpperBound ;
    CObList     m_lstDocItems ;        // the 'contents' of the document

    LOGFONT     m_logfont ;

// Operations
    int         GetDocItemCount( void ) ;
    CItem*      GetDocItem( int index ) ;
    CItem*      GetDocItem( const CString& rszName ) ;
    
    BOOL        AddDocItem( CItem* pItem ) ;
    BOOL        ChangeDocItem( int index, const CString& rszName, const VARIANT FAR& vt ) ;
    BOOL        ChangeDocItem( CItem* pItem, const CString& rszName, const VARIANT FAR& vt ) ;
    BOOL        ChangeDocItem( CItem* pItem, const CString& rszName ) ;
    BOOL        ChangeDocItem( CItem* pItem, const VARIANT FAR& vt ) ;

    BOOL        RemoveDocItem( int index ) ;
    BOOL        RemoveDocItem( CItem* pItem ) ;
    BOOL        RemoveDocItem( const CString& str ) ;

    COLINFO*    GetColInfo() ;
    void        SetColInfo( COLINFO* pColInfo, BOOL fDirty = TRUE ) ;
    int         GetNumColumns() ;
    CString     GetColTitleString() ;
    CFont*      SelectDocFont(CDC* pDC, CFont& font) ;
    int         GetWidth( CDC* pDC ) ;
    void        CalcDisplaySize( CDC* pDC, CSize& sizeItem ) ;        
    int         Draw( CDC* pDC, CPoint ptStart ) ;
// Implementation
public:
    virtual ~CColSampDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
    virtual COleServerItem* OnGetEmbeddedItem();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    virtual BOOL OnNewDocument();
    virtual void OnFinalRelease();

    COLINFO* m_rgColInfo ;

// Generated message map functions
protected:
    //{{AFX_MSG(CColSampDoc)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CColSampDoc)
    afx_msg LPDISPATCH GetApplication();
    afx_msg BOOL GetVisible();
    afx_msg void SetVisible(BOOL bNewValue);
    afx_msg LPDISPATCH GetItems( const VARIANT FAR& vt );
    afx_msg void Activate();
    afx_msg BSTR GetTitle();
    afx_msg void SetTitle(LPCSTR lpszNewValue);
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDocuments command target
//
// This class implements the "Documents collection object"
//
class CDocuments : public CCmdTarget
{   
    DECLARE_DYNCREATE(CDocuments)
public:
    CDocuments();           

// Attributes
public:

// Operations
public:

// Implementation
public:
    virtual ~CDocuments();
    virtual void OnFinalRelease();

protected:
    // Generated message map functions
    //{{AFX_MSG(CDocuments)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CDocuments)
    afx_msg short GetCount();
    afx_msg VARIANT Add();
    afx_msg LPDISPATCH GetItem(const VARIANT FAR& ItemNameOrIndex);
    afx_msg VARIANT Open(const VARIANT FAR& fileName);
    //}}AFX_DISPATCH
    afx_msg LPUNKNOWN _NewEnum();

    DECLARE_DISPATCH_MAP()
};

// Declaraction of the class that implements IEnumVARIANT
//
class CEnumDocs : public CCmdTarget
{   
public:
    CEnumDocs();         // protected constructor used by dynamic creation

// Attributes
private:
    
// Operations
public:

// Implementation
public:
    virtual ~CEnumDocs();
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
#endif // _DOC_H_
