// item.cpp : implementation file
//
// CItem implementation  
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
// An "Item" in this application is an object that has a 
// name and a value.  The name is stored as a CString and
// the value is a VARIANT.   The CItem class implements
// the "Item" objects.
//
// This class implements an automation interface.  The following
// methods and properties are defined on Item
// (Note:  those marked with a '*' are not implemented yet).
// (Note:  '!' indicates "Value" property)
//
//  Item Object
//      Methods:
//          Delete*
//          Delete( shift )*
//          Select*
//
//      Properties:
//          Application [out]
//          Document [out]
//          Name [in/out]
//          Value [in/out] !
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
// CItem

IMPLEMENT_SERIAL(CItem, CCmdTarget,1)
#define new DEBUG_NEW

IMPLEMENT_OLECREATE(CItem, "ColSamp.Item.1", 0xd3104580, 0xce0e, 0x101a, 0x9a, 0x7c, 0x0, 0xaa, 0x0, 0x33, 0x97, 0x10)

CItem::CItem()
{
    EnableAutomation();

    VariantInit( &m_vtValue ) ;
    m_vtValue.vt = VT_ERROR ;
    m_vtValue.scode = S_OK ;
    m_pParent = NULL ;
    m_pDoc = NULL ;    
    // To keep the application running as long as an OLE automation 
    //  object is active, the constructor calls AfxOleLockApp.
    
    AfxOleLockApp();
}

CItem::CItem( CColSampDoc* pDoc )
{
    EnableAutomation();

    VariantInit( &m_vtValue ) ;
    m_vtValue.vt = VT_ERROR ;
    m_vtValue.scode = S_OK ;
    m_pParent = NULL ;
    
    m_pDoc = pDoc ;
        
    // To keep the application running as long as an OLE automation 
    //  object is active, the constructor calls AfxOleLockApp.
    
    AfxOleLockApp();
}

CItem::CItem( CItems* pParent )
{
    EnableAutomation();

    VariantInit( &m_vtValue ) ;
    m_vtValue.vt = VT_ERROR ;
    m_vtValue.scode = S_OK ;
    m_pParent = pParent ;
    
    if (m_pParent)
        m_pDoc = m_pParent->GetDoc() ;
    else
        m_pDoc = NULL ;
        
    // To keep the application running as long as an OLE automation 
    //  object is active, the constructor calls AfxOleLockApp.
    
    AfxOleLockApp();
}

CItem::~CItem()
{
    // To terminate the application when all objects created with
    //  with OLE automation, the destructor calls AfxOleUnlockApp.
    
    AfxOleUnlockApp();
}

int CItem::Draw( CDC* pDC, CPoint ptStart, int n ) 
{
    TEXTMETRIC tm ;
    pDC->GetTextMetrics( &tm ) ;

    RECT rc ;                             
    rc.left = ptStart.x ;
    rc.top = ptStart.y ;
    rc.bottom = ptStart.y + tm.tmHeight ;
    rc.right = rc.left ;
    
    if (m_pDoc)
    {
        COLINFO* rgColInfo ;
        char szName[256] ;
        rgColInfo = m_pDoc->GetColInfo() ;
             
        rc.right += rgColInfo[0].uiWidth ;
        if (n != -1)
            wsprintf( szName, "%d", n ) ;
        else
            *szName = '\0' ;
            
        CColumnListBox::JustifiedTextOut( pDC, rc.left+2, rc.top, 
                           rc, szName, 0, rgColInfo[0].uiFormatFlags ) ;
        rc.left += rgColInfo[0].uiWidth ;

        rc.right += rgColInfo[1].uiWidth ;
        lstrcpy( szName, m_szName ) ;
        CColumnListBox::JustifiedTextOut( pDC, rc.left+2, rc.top,  
                           rc, szName, 0, rgColInfo[1].uiFormatFlags ) ;
        rc.left += rgColInfo[1].uiWidth ;
        
        rc.right += rgColInfo[2].uiWidth ;
        CColumnListBox::JustifiedTextOut( pDC, rc.left+2, rc.top,  
                           rc, VTtoString(m_vtValue.vt), 0, 
                           rgColInfo[2].uiFormatFlags ) ;
        rc.left += rgColInfo[2].uiWidth ;
    
        rc.right += rgColInfo[3].uiWidth ;
        CColumnListBox::JustifiedTextOut( pDC, rc.left+2, rc.top,  
                           rc, VariantToString(m_vtValue), 0, 
                           rgColInfo[3].uiFormatFlags ) ;
    }
    return tm.tmHeight ;
}

void CItem::CalcDisplaySize( CDC* pDC, CSize& sizeItem ) 
{  
    TEXTMETRIC tm ;
    pDC->GetTextMetrics( &tm ) ;
    sizeItem.cy = tm.tmHeight ;
    if (m_pDoc) sizeItem.cx = m_pDoc->GetWidth( pDC ) ;
    else sizeItem.cx = 10 ;
}

void CItem::OnFinalRelease()
{
    // When the last reference for an automation object is released
    //  OnFinalRelease is called.  This implementation deletes the 
    //  object.  Add additional cleanup required for your object before
    //  deleting it from memory.

    VariantClear( &m_vtValue ) ;
    
    delete this;
}

void CItem::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        ar << m_szName ;
        ar.Write( &m_vtValue, sizeof(VARIANT) ) ;
        if (!(m_vtValue.vt & VT_BYREF) && !(m_vtValue.vt & VT_ARRAY) &&
             m_vtValue.vt != VT_DISPATCH  &&
             m_vtValue.vt != VT_VARIANT  &&
             m_vtValue.vt != VT_UNKNOWN)
        {
            if (m_vtValue.vt == VT_BSTR)
            {
                ar << CString( m_vtValue.bstrVal ) ;
            }
        }
    }
    else
    {
        m_pDoc = (CColSampDoc*)ar.m_pDocument;
        ar >> m_szName ;   
        
        ar.Read( &m_vtValue, sizeof(VARIANT) ) ;
        if (!(m_vtValue.vt & VT_BYREF) && !(m_vtValue.vt & VT_ARRAY) &&
             m_vtValue.vt != VT_DISPATCH  &&
             m_vtValue.vt != VT_VARIANT  &&
             m_vtValue.vt != VT_UNKNOWN)
        {
            if (m_vtValue.vt == VT_BSTR)
            {
                CString s ;
                ar >> s ;
                m_vtValue.bstrVal = s.AllocSysString() ;
            }
        }
        else
            VariantClear( &m_vtValue ) ;
    }
}

BEGIN_MESSAGE_MAP(CItem, CCmdTarget)
    //{{AFX_MSG_MAP(CItem)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CItem, CCmdTarget)
    //{{AFX_DISPATCH_MAP(CItem)
    DISP_PROPERTY_EX(CItem, "Value", GetValue, SetValue, VT_VARIANT)
    DISP_PROPERTY_EX(CItem, "Name", GetName, SetName, VT_BSTR)
    DISP_PROPERTY_EX(CItem, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
    DISP_PROPERTY_EX(CItem, "Document", GetDocument, SetNotSupported, VT_DISPATCH)
    DISP_PROPERTY_EX(CItem, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
    //}}AFX_DISPATCH_MAP
    DISP_DEFVALUE(CItem, "Value")   
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// CItem message handlers


VARIANT CItem::GetValue()
{
    VARIANT va ;
    VariantInit( &va ) ;
    VariantCopy( &va, &m_vtValue ) ;
    return va ;
}

void CItem::SetValue(const VARIANT FAR& newValue)
{
    VariantClear( &m_vtValue ) ;
    VariantCopy( &m_vtValue, (VARIANT FAR*)&newValue ) ;
    
    if (m_pDoc)
       m_pDoc->UpdateAllViews( NULL, 1, this ) ;
}

BSTR CItem::GetName()
{
    return m_szName.AllocSysString();
}

void CItem::SetName(LPCSTR lpszNewValue)
{
    m_szName = lpszNewValue ;
    if (m_pDoc)
       m_pDoc->UpdateAllViews( NULL, 1, this ) ;
}

LPDISPATCH CItem::GetApplication()
{
    LPDISPATCH  lpDisp = NULL ;
    CApplication* pApp = new CApplication ;
    
    if (pApp)
        lpDisp = pApp->GetIDispatch(FALSE) ;
    
    return lpDisp ;
}

LPDISPATCH CItem::GetDocument()
{
    LPDISPATCH lpDisp = NULL ;
    
    if (m_pDoc)
        lpDisp = m_pDoc->GetIDispatch(TRUE) ;
    
    return lpDisp ;
}

// BUGBUG:  This is bogus.  An item may have multiple CItems pointing to it.
//
LPDISPATCH CItem::GetParent()
{
    LPDISPATCH lpDisp = NULL ;
    if (m_pParent)
        lpDisp = m_pParent->GetIDispatch(TRUE) ;
        
    return lpDisp ;
}

static char * g_rgszVT[] =
{
    "VT_EMPTY",      
    "VT_NULL",       
    "VT_I2",         
    "VT_I4",         
    "VT_R4",         
    "VT_R8",         
    "VT_CY",         
    "VT_DATE",       
    "VT_BSTR",       
    "VT_DISPATCH",   
    "VT_ERROR",      
    "VT_BOOL",       
    "VT_VARIANT",    
    "VT_UNKNOWN",    
    #ifdef UNICODE
    "VT_WBSTR",      
    #endif
} ;

LPSTR VTtoString( const VARTYPE vt )
{
    static char szBuf[64];

    *szBuf = '\0' ;
    
    if (vt & VT_BYREF)
        lstrcat( szBuf, "VT_BYREF | " ) ;
    
    if (vt & VT_ARRAY)
        lstrcat( szBuf, "VT_ARRAY | " ) ;

    #ifdef UNICODE
    if (vt <= VT_WBSTR)
    #else
    if (vt <= VT_UNKNOWN)
    #endif
    {
        lstrcat( szBuf, g_rgszVT[vt] ) ;
        return (LPSTR)szBuf ;
    }
    
    wsprintf( szBuf, "<Unknown %08lX>", vt ) ;
            
    return (LPSTR)szBuf ;
}

VARTYPE StringToVT( const CString& rstr ) 
{
    VARTYPE vt  ;
    
    #ifdef UNICODE
    for (vt = VT_EMPTY ; vt <= VT_WBSTR ; vt++)
    #else
    for (vt = VT_EMPTY ; vt <= VT_UNKNOWN ; vt++)
    #endif
    {
        if (lstrcmpi( g_rgszVT[vt], rstr ) == 0)
            return vt ;
    }
    return -1 ;
}

LPSTR VariantToString( const VARIANT &v )
{
    static char szBuf[256] ;
    
    VARIANT va ;
    VariantInit( &va ) ;
    if (SUCCEEDED(VariantChangeType( &va, (LPVARIANT)&v, 0, VT_BSTR )))
    {
        lstrcpy( szBuf, va.bstrVal ) ;
        VariantClear( &va ) ;
        return szBuf ;
    }
    
    wsprintf( szBuf, "%#08lx", v.lVal ) ;
    return szBuf ;
}
