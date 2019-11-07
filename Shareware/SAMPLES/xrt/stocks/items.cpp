// items.cpp : implementation file
//

#include "stdafx.h"
#include "stocks.h" 
#include "item.h"
#include "items.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStockItems

IMPLEMENT_DYNCREATE(CStockItems, CCmdTarget)

CStockItems::CStockItems()
{
    EnableAutomation();
    
    // To keep the application running as long as an OLE automation 
    //  object is active, the constructor calls AfxOleLockApp.
    

    
    AfxOleLockApp();
}

CStockItems::~CStockItems()
{
    // To terminate the application when all objects created with
    //  with OLE automation, the destructor calls AfxOleUnlockApp.
    
    AfxOleUnlockApp();
}

void CStockItems::OnFinalRelease()
{
    // When the last reference for an automation object is released
    //  OnFinalRelease is called.  This implementation deletes the 
    //  object.  Add additional cleanup required for your object before
    //  deleting it from memory.
    CStockItem* pItem ;
    for (int i = 0 ; i < m_rgItems.GetSize() ; i++)
        if (pItem = (CStockItem*)m_rgItems.GetAt(i))
        {
            // When we added it, we did an AddRef.  So Release now.
            //
            LPDISPATCH lpDisp = pItem->GetIDispatch(FALSE) ;
            if (lpDisp) lpDisp->Release() ;
        }

    m_rgItems.RemoveAll() ;

    delete this;
}

int CStockItems::Find( const VARIANT FAR &vt )
{
    CStockItem* pItem ;                
    int i ;

    if (vt.vt == VT_I4)
    {
        return (int)vt.lVal ;
    }        
    else if (vt.vt == VT_I2)
    {
        return vt.iVal ;
    }
        
    // Coerce to string
    //
    VARIANT va ;
    VariantInit( &va ) ;
    if (SUCCEEDED(VariantChangeType( &va, (LPVARIANT)&vt, 0, VT_BSTR )))
    {
        for (i = 0 ; i <= m_rgItems.GetUpperBound() ; i++)
            if (NULL != (pItem = (CStockItem*)m_rgItems.GetAt( i )))
            {
                if (lstrcmpi( pItem->m_szName, va.bstrVal ) == 0)
                    return i ;
            }
    }

    return -1 ;
}

BEGIN_MESSAGE_MAP(CStockItems, CCmdTarget)
    //{{AFX_MSG_MAP(CStockItems)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CStockItems, CCmdTarget)
    //{{AFX_DISPATCH_MAP(CStockItems)
    DISP_FUNCTION(CStockItems, "Remove", Remove, VT_EMPTY, VTS_VARIANT)
    DISP_FUNCTION(CStockItems, "Add", Add, VT_VARIANT, VTS_VARIANT)
    //}}AFX_DISPATCH_MAP
    DISP_PROPERTY_PARAM(CStockItems, "Item", GetItem, SetItem, VT_DISPATCH, VTS_VARIANT )
//    DISP_DEFVALUE(CStockItems,"Item")
    DISP_FUNCTION_ID(CStockItems, "_NewEnum", DISPID_NEWENUM, _NewEnum, VT_UNKNOWN, VTS_NONE)
    DISP_PROPERTY_EX(CStockItems, "Count", GetCount, NULL, VT_I4)
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStockItems message handlers


LPDISPATCH CStockItems::GetItem( const VARIANT FAR& vt )
{   
    CStockItem* pItem = NULL ;  
    LPDISPATCH lpDisp = NULL ;
    
    if (vt.vt == VT_BSTR)
    {
        int i = Find( vt ) ;
        if (-1 != i && (pItem = (CStockItem*)m_rgItems.GetAt( i )))
            lpDisp = pItem->GetIDispatch(TRUE) ;        // AddRef
    }
    else
    {
        // coerce to VT_I4
        VARIANT va ;
        VariantInit( &va );
        if (SUCCEEDED(VariantChangeType( &va, (VARIANT FAR*)&vt, 0, VT_I4 )))
        {
            if ((m_rgItems.GetUpperBound() >= va.lVal) &&
                (pItem = (CStockItem*)m_rgItems.GetAt( (int)va.lVal )))
                lpDisp = pItem->GetIDispatch(TRUE) ;         // AddRef
        }
    }                 

    // BUGBUG:  Implement dispatch exception if lpDisp == NULL
    return lpDisp ;
}

void CStockItems::SetItem( const VARIANT FAR& vt, LPDISPATCH newValue)
{  
    CStockItem* pItem = (CStockItem*)CCmdTarget::FromIDispatch(newValue) ;

    if (pItem != NULL && pItem->IsKindOf(RUNTIME_CLASS(CStockItem)))
    {   
        VARIANT va ;
        VariantInit( &va );
        int index = -1 ;
        
        if (vt.vt == VT_BSTR)
        {
            index = Find( vt ) ;
        }
        else
        {
            // coerce to VT_I4
            if (SUCCEEDED(VariantChangeType( &va, (VARIANT FAR*)&vt, 0, VT_I4 )))
            {
                index = (int)va.lVal ;
            }
            else
            {
                // BUGBUG: Catch CMemoryException and trow to dispatch
            }
        }                 

        if (index != -1)    
        {
            // if there's already an item at index then delete it
            // first
            //
            CStockItem* pOldItem ;
            if (m_rgItems.GetUpperBound() >= index && 
                (pOldItem = (CStockItem*)m_rgItems.GetAt( (int)index )))
            {
                // BUGBUG: Should we actually delete an object?  ever?
                //
                //delete pOldItem ;
            }
            
            m_rgItems.SetAtGrow( (int)index, pItem ) ;
        }
        else
        {
            
        }
    }
}

LPUNKNOWN CStockItems::_NewEnum()
{
    // TODO: Add your dispatch handler code here
    return NULL;
}

void CStockItems::Remove(const VARIANT FAR& vt)
{
    CStockItem* pItem ;
    int index = -1 ;

    // Find by VT_DISPATCH, VT_I4, or VT_BSTR...
    //
    if (vt.vt == VT_DISPATCH || vt.vt == VT_UNKNOWN)
    {
        pItem = (CStockItem*)CCmdTarget::FromIDispatch( vt.pdispVal ) ;
        if (pItem != NULL && pItem->IsKindOf(RUNTIME_CLASS(CStockItem)))
        {
            // Find it in the array
            //
            for (int i = 0 ; i < m_rgItems.GetSize() ; i++)
                if (m_rgItems[i] == pItem)
                {
                    index = i ;
                    break ;
                }
        }
    }
    else if (vt.vt == VT_BSTR)
    {
        index = Find( vt ) ;
    }
    else
    {
        // coerce to VT_I4
        VARIANT va ;
        VariantInit( &va );
        if (SUCCEEDED(VariantChangeType( &va, (VARIANT FAR*)&vt, 0, VT_I4 )))
        {
            index = (int)va.lVal ;
        }
    }
    
    if (index != -1)
    {   
        if (pItem = (CStockItem*)m_rgItems.GetAt(index))
        {
            // When we added it, we did an AddRef.  So Release now.
            //
            LPDISPATCH lpDisp = pItem->GetIDispatch(FALSE) ;
            if (lpDisp) lpDisp->Release() ;
        }
        m_rgItems.RemoveAt(index) ;
    }
}

VARIANT CStockItems::Add(const VARIANT FAR& vt)
{
    VARIANT va;
    VariantInit( &va ) ;
    CStockItem* pItem ;
    
    va.vt = VT_EMPTY ;

    if (vt.vt == VT_DISPATCH || vt.vt == VT_UNKNOWN)
    {
        pItem = (CStockItem*)CCmdTarget::FromIDispatch( vt.pdispVal ) ;
        if (pItem != NULL && pItem->IsKindOf(RUNTIME_CLASS(CStockItem)))
        {
            // When an object is added to a collection, it needs
            // to be AddRef'd.  
            //
            vt.pdispVal->AddRef() ;   
            m_rgItems.Add( pItem ) ;

            // BUGBUG:  Catch memory exception and pass on to dispatch
        }
    }
    else
    {
        pItem = new CStockItem ;
    
        if (pItem != NULL)
        {
            VARIANT vTemp ;
            VariantInit( &vTemp ) ;
            if (SUCCEEDED(VariantChangeType( &vTemp, (LPVARIANT)&vt, 0, VT_BSTR )))
            {
                pItem->m_szName = vTemp.bstrVal ;
                VariantClear( &vTemp ) ;
            }
            else
                pItem->m_szName = "" ;

            m_rgItems.Add( pItem ) ;
            va.vt = VT_DISPATCH ;
            va.pdispVal = pItem->GetIDispatch(TRUE) ;
        }    
    }
    
    return va;
}

long CStockItems::GetCount()
{
    return m_rgItems.GetSize() ;
}

