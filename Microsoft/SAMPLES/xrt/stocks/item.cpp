// item.cpp : implementation file
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
// CStockItem

IMPLEMENT_DYNCREATE(CStockItem, CCmdTarget)

CStockItem::CStockItem()
{
    EnableAutomation();

    VariantInit( &m_vtValue ) ;
    m_vtValue.vt = VT_I4 ;
    m_vtValue.lVal = 0 ;
    
    // To keep the application running as long as an OLE automation 
    //  object is active, the constructor calls AfxOleLockApp.
    
    AfxOleLockApp();
}

CStockItem::~CStockItem()
{
    // To terminate the application when all objects created with
    //  with OLE automation, the destructor calls AfxOleUnlockApp.
    
    AfxOleUnlockApp();
}

void CStockItem::OnFinalRelease()
{
    // When the last reference for an automation object is released
    //  OnFinalRelease is called.  This implementation deletes the 
    //  object.  Add additional cleanup required for your object before
    //  deleting it from memory.

    VariantClear( &m_vtValue ) ;
    
    delete this;
}

BEGIN_MESSAGE_MAP(CStockItem, CCmdTarget)
    //{{AFX_MSG_MAP(CStockItem)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CStockItem, CCmdTarget)
    //{{AFX_DISPATCH_MAP(CStockItem)
    DISP_PROPERTY_EX(CStockItem, "Value", GetValue, SetValue, VT_VARIANT)
    DISP_PROPERTY(CStockItem, "Name", m_szName, VT_BSTR)
    //}}AFX_DISPATCH_MAP
//    DISP_DEFVALUE(CStockItem, "Value")   
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// CStockItem message handlers


VARIANT CStockItem::GetValue()
{
    VARIANT va ;
    VariantInit( &va ) ;
    VariantCopy( &va, &m_vtValue ) ;
    return va ;
}

void CStockItem::SetValue(const VARIANT FAR& newValue)
{
    VariantClear( &m_vtValue ) ;
    VariantCopy( &m_vtValue, (VARIANT FAR*)&newValue ) ;
}
