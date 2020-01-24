// items.cpp : implementation file
//
// CItems, CEnumItems implementation  
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
// The CItems class implements an automation interface.  The following
// methods and properties are defined on Items
// (Note:  those marked with a '*' are not implemented yet).
// (Note:  '!' indicates "Value" property)
//
//  Items Object
//      Methods:
//          items.Item !
//          items.Item( index )
//          items.Item( string )
//          items.Add
//          items.Add( name, value )
//          items.Remove( index ) ;
//          items.Remove( name )
//          items._NewEnum [hidden]
//
//      Properties:
//          Application [out]*
//          Document [out]* 
//          Parent [out*]
//          Name [in/out]*
//          Count
//          
// CEnumItems implements IEnumVARIANT for the Items collection.
//

#include "stdafx.h"
#include "colsamp.h" 
#include "item.h"
#include "items.h"

#include "doc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Items

IMPLEMENT_DYNCREATE(CItems, CCmdTarget)
#define new DEBUG_NEW

BEGIN_INTERFACE_MAP(CEnumItems, CCmdTarget)
    INTERFACE_PART(CEnumItems, IID_IEnumVARIANT, EnumVARIANT)
END_INTERFACE_MAP()

CItems::CItems()
{
    EnableAutomation();
    
    // To keep the application running as long as an OLE automation 
    //  object is active, the constructor calls AfxOleLockApp.
    
    m_pDoc = NULL ;
    
    AfxOleLockApp();
}

CItems::CItems( const CColSampDoc* pDoc )
{
    EnableAutomation();
    
    // To keep the application running as long as an OLE automation 
    //  object is active, the constructor calls AfxOleLockApp.
    
    m_pDoc = (CColSampDoc*)pDoc ;
    
    AfxOleLockApp();
}

CItems::~CItems()
{
    // To terminate the application when all objects created with
    //  with OLE automation, the destructor calls AfxOleUnlockApp.
    
    AfxOleUnlockApp();
}

void CItems::OnFinalRelease()
{
    // When the last reference for an automation object is released
    //  OnFinalRelease is called.  This implementation deletes the 
    //  object.  Add additional cleanup required for your object before
    //  deleting it from memory.

    //TRACE("CItems::OnFinalRelease\r\n" ) ;
    delete this;
}

CColSampDoc* CItems::GetDoc() 
{   
    return m_pDoc ;
}

BEGIN_MESSAGE_MAP(CItems, CCmdTarget)
    //{{AFX_MSG_MAP(CItems)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CItems, CCmdTarget)
    //{{AFX_DISPATCH_MAP(CItems)
    DISP_FUNCTION(CItems, "Remove", Remove, VT_EMPTY, VTS_VARIANT)
    DISP_FUNCTION(CItems, "Add", Add, VT_VARIANT, VTS_VARIANT VTS_VARIANT)
    DISP_PROPERTY_EX(CItems, "Count", GetCount, SetNotSupported, VT_I4)
    //}}AFX_DISPATCH_MAP
    DISP_FUNCTION(CItems, "Item", GetItem, VT_DISPATCH, VTS_VARIANT )
    DISP_DEFVALUE(CItems,"Item")
    DISP_PROPERTY_EX_ID(CItems, "_NewEnum", DISPID_NEWENUM, _NewEnum, SetNotSupported, VT_UNKNOWN)
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItems message handlers
LPDISPATCH CItems::GetItem( const VARIANT FAR& vt )
{   
    CItem* pItem = NULL ;  
    LPDISPATCH lpDisp = NULL ;
    
    if (vt.vt == VT_BSTR)
    {
        pItem = m_pDoc->GetDocItem( CString(vt.bstrVal) ) ;
    }
    else
    {
        // coerce to VT_I4
        VARIANT va ;
        VariantInit( &va );
        if (SUCCEEDED(VariantChangeType( &va, (VARIANT FAR*)&vt, 0, VT_I4 )))
        {
            pItem = m_pDoc->GetDocItem( (int)va.lVal ) ;
        }
    }                 
    
    if (pItem != NULL)
        lpDisp = pItem->GetIDispatch(TRUE) ;        // AddRef
    

    // BUGBUG:  Implement dispatch exception if lpDisp == NULL
    return lpDisp ;
}

LPUNKNOWN CItems::_NewEnum()
{
    CEnumItems* pIEV = NULL ;

    pIEV = new CEnumItems( m_pDoc ) ;
    
    if (pIEV)
    {
        pIEV->m_xEnumVARIANT.Reset() ;
        return &pIEV->m_xEnumVARIANT ;
    }
    
    return NULL ;
}

void CItems::Remove(const VARIANT FAR& vt)
{
    CItem* pItem = NULL ;

    // Find by VT_DISPATCH, VT_I4, or VT_BSTR...
    //
    if (vt.vt == VT_DISPATCH || vt.vt == VT_UNKNOWN)
    {
        CItem* pNewItem ;
        pNewItem = (CItem*)CCmdTarget::FromIDispatch( vt.pdispVal ) ;
        if (pNewItem != NULL && pNewItem->IsKindOf(RUNTIME_CLASS(CItem)))
        {
            m_pDoc->RemoveDocItem( pNewItem ) ;
        }
    }
    else if (vt.vt == VT_BSTR)
    {
        m_pDoc->RemoveDocItem( CString( vt.bstrVal ) ) ;
    }
    else
    {
        // coerce to VT_I4
        VARIANT va ;
        VariantInit( &va );
        if (SUCCEEDED(VariantChangeType( &va, (VARIANT FAR*)&vt, 0, VT_I4 )))
        {
            m_pDoc->RemoveDocItem( (int)va.lVal ) ;
        }
    }
}

VARIANT CItems::Add(const VARIANT FAR& vtName, const VARIANT FAR& vtValue )
{
    VARIANT va;
    VariantInit( &va ) ;
    CItem* pItem ;
    
    va.vt = VT_EMPTY ;

    pItem = new CItem( this ) ;
    
    if (pItem != NULL)
    {
        VARIANT vTemp ;
        VariantInit( &vTemp ) ;
        if (SUCCEEDED(VariantChangeType( &vTemp, (LPVARIANT)&vtName, 0, VT_BSTR )))
        {
            pItem->m_szName = vTemp.bstrVal ;
            VariantClear( &vTemp ) ;
        }
        else
        {
            char sz[16] ; wsprintf( sz, "Item%d", m_pDoc->m_nUpperBound + 1 ) ;
            pItem->m_szName = sz ;
            m_pDoc->m_nUpperBound++ ;
        }
        
        VariantCopy( &pItem->m_vtValue, (LPVARIANT)&vtValue ) ;        
        
        m_pDoc->AddDocItem( pItem ) ;

        va.vt = VT_DISPATCH ;
        va.pdispVal = pItem->GetIDispatch(TRUE) ;
    }    
    
    return va;
}

long CItems::GetCount()
{
    //TRACE( "CItems::GetCount()\r\n" ) ;
    if (m_pDoc) 
        return m_pDoc->GetDocItemCount() ;
    else
        return 0 ;
}


//----------------------------------------------------------------------------
// Implementation of CEnumItems which implements the IEnumVARIANT interface
// used by _NewEnum
//
CEnumItems::CEnumItems()
{
    //TRACE("CEnumVARAINT::CEnumItems()\r\n" ) ;
    // This version of the constructor should never be called because
    // we are not OLE createable and we only get created within the
    // impelementation of _NewEnum, and we use the pDoc constructor
    // below...
    //
    m_pDoc = NULL ;
    
    AfxOleLockApp();
}

CEnumItems::CEnumItems( CColSampDoc* pDoc )
{
    //TRACE("CEnumVARAINT::CEnumItems( pDoc )\r\n" ) ;
    m_pDoc = pDoc ;
    
    AfxOleLockApp();
}

CEnumItems::~CEnumItems()
{
    //TRACE("CEnumVARAINT::~CEnumItems()\r\n" ) ;
    AfxOleUnlockApp();
}

void CEnumItems::OnFinalRelease()
{
    // When the last reference for an automation object is released
    //  OnFinalRelease is called.  This implementation deletes the 
    //  object.  Add additional cleanup required for your object before
    //  deleting it from memory.
    //TRACE("CEnumItems::OnFinalRelease\r\n" ) ;
    delete this;
}

//----------------------------------------------------------------------------
// Implementation of the nested class XEnumVARIANT (declared through the
// BEGIN/END_INTERFACE_PART macros in the declaration of CEnumItems)
//
// This class implements the IEnumVARIANT interface requried by the 
// _NewEnum property of the collection
//
CEnumItems::XEnumVARIANT::XEnumVARIANT()
{
    m_posCurrent = NULL ;
}

STDMETHODIMP_(ULONG) CEnumItems::XEnumVARIANT::AddRef()
{   
    METHOD_PROLOGUE(CEnumItems, EnumVARIANT)
    return pThis->ExternalAddRef() ;
}   

STDMETHODIMP_(ULONG) CEnumItems::XEnumVARIANT::Release()
{   
    METHOD_PROLOGUE(CEnumItems, EnumVARIANT)
    return pThis->ExternalRelease() ;
}   

STDMETHODIMP CEnumItems::XEnumVARIANT::QueryInterface( REFIID iid, void FAR* FAR* ppvObj )
{   
    METHOD_PROLOGUE(CEnumItems, EnumVARIANT)
    return (HRESULT)pThis->ExternalQueryInterface( (void FAR*)&iid, ppvObj) ;
}   

// IEnumVARIANT::Next
// 
STDMETHODIMP CEnumItems::XEnumVARIANT::Next( ULONG celt, VARIANT FAR* rgvar, ULONG FAR* pceltFetched)
{
    // This sets up the "pThis" pointer so that it points to our
    // containing CItems instance
    //
    METHOD_PROLOGUE(CEnumItems, EnumVARIANT)

    ASSERT( pThis->m_pDoc ) ;
    if (pThis->m_pDoc == NULL) return ResultFromScode( E_UNEXPECTED ) ;

    HRESULT hr;
    ULONG   l ;
    CItem*  pItem = NULL ;

    // pceltFetched can legally == 0
    //                                           
    if (pceltFetched != NULL)
        *pceltFetched = 0;
    else if (celt > 1)
    {   
        TRACE("XEnumVARIANT::Next() celt > 1 and pceltFetched == NULL!\r\n", celt ) ;
        return ResultFromScode( E_INVALIDARG ) ;   
    }

    for (l=0; l < celt; l++)
        VariantInit( &rgvar[l] ) ;

    // Retrieve the next celt elements.
    hr = NOERROR ;
    for (l = 0 ; m_posCurrent != NULL && celt != 0 ; l++)
    {   
        pItem = (CItem*)pThis->m_pDoc->m_lstDocItems.GetNext( m_posCurrent ) ;
        celt-- ;
        if (pItem)
        {
            //TRACE( "   Setting rgvar[%d]\r\n", (int)l ) ;
            rgvar[l].vt = VT_DISPATCH ;
            rgvar[l].pdispVal = pItem->GetIDispatch( TRUE ) ;
            if (pceltFetched != NULL)
                (*pceltFetched)++ ;
        }
        else 
        {
            TRACE("GetNext failed in IEnumVARIANT::Next\r\n" ) ;
            return ResultFromScode( E_UNEXPECTED ) ;
        }
    }
    
    if (celt != 0)
    {
        //TRACE( "   End of list...celt == %d\r\n", (int)celt ) ;
        hr = ResultFromScode( S_FALSE ) ;
    }
    
    return hr ;
}

// IEnumVARIANT::Skip
//
STDMETHODIMP CEnumItems::XEnumVARIANT::Skip(unsigned long celt) 
{
    METHOD_PROLOGUE(CEnumItems, EnumVARIANT)

    ASSERT( pThis->m_pDoc ) ;
    if (pThis->m_pDoc == NULL) return ResultFromScode( E_UNEXPECTED ) ;

    //TRACE("XEnumVARIANT::Skip( %l )\r\n", celt ) ;
    
    while (m_posCurrent != NULL && celt--)
        pThis->m_pDoc->m_lstDocItems.GetNext( m_posCurrent ) ;
    
    return (celt == 0 ? NOERROR : ResultFromScode( S_FALSE )) ;
}

STDMETHODIMP CEnumItems::XEnumVARIANT::Reset()
{
    METHOD_PROLOGUE(CEnumItems, EnumVARIANT)

    ASSERT( pThis->m_pDoc ) ;
    if (pThis->m_pDoc == NULL) return ResultFromScode( E_UNEXPECTED ) ;

    //TRACE("XEnumVARIANT::Reset()\r\n") ;
    
    m_posCurrent = pThis->m_pDoc->m_lstDocItems.GetHeadPosition() ;
    
    return NOERROR ;
}

STDMETHODIMP CEnumItems::XEnumVARIANT::Clone(IEnumVARIANT FAR* FAR* ppenum) 
{
    METHOD_PROLOGUE(CEnumItems, EnumVARIANT)   

    ASSERT( pThis->m_pDoc ) ;
    if (pThis->m_pDoc == NULL) return ResultFromScode( E_UNEXPECTED ) ;
    
    //TRACE("XEnumVARIANT::Clone()\r\n" ) ;

    CEnumItems* p = new CEnumItems( pThis->m_pDoc ) ;
    if (p)
    {
        p->m_xEnumVARIANT.m_posCurrent = m_posCurrent ;
        return NOERROR ;    
    }
    else
        return ResultFromScode( E_OUTOFMEMORY ) ;
}
 
 