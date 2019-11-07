// WOSA/XRT XRTFrame Sample Application Version 1.00.007
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// request.cpp : implementation file
//
// This file implements the CRequest class which is the WOSA/XRT "Request"
// object.
//
// Each CDataObject (i.e. the "DataObject" object) has a m_listRequests
// member, which is a list of these CRequest objects.   
//
#include "stdafx.h"
#include <afxpriv.h>    // for CSharedFile

#include "xrtframe.h"
#include "request.h"
#include "properti.h"
#include "property.h"

#include "dataitem.h"

#include "doc.h"
#include "view.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRequest
CRequest::CRequest( CDataObject* pDataObject )
{
    m_pDataObject = pDataObject ;

    if (pDataObject)
    {
        m_pProperties = pDataObject->GetNamePropertySet() ;
        
        // Each Request object has a unique ID (per DataObject instance).
        m_dwID = pDataObject->GenerateRequestID() ;
    }
    else
    {
        m_pProperties = NULL ; // Serialize should set.        
        m_dwID = 0 ;
    }

    EnableAutomation();
}

CRequest::~CRequest()
{
    if (m_pProperties != NULL)
        m_pProperties->ExternalRelease() ;
}

void CRequest::OnFinalRelease()
{
    // When the last reference for an automation object is released
    //  OnFinalRelease is called.  This implementation deletes the 
    //  object.  Add additional cleanup required for your object before
    //  deleting it from memory.

    delete this;
}

// A Request object in this implementation is capable of
// deciding whether an incomming tick 'matches' itself.
//
// In this sample implementation only simple request
// strings are supported:  
//
//   Either a instrument name (case sensitive)
// or
//   A string followed by a "*", which will result in
//   a cheapo wildcard match.  (e.g. "MS*" matches "MSFT"
//   and "MSAB").
//
// It is expected that vendors will have much more advanced
// matching capabilities, thus this function will probably be 
// replaced with much more advanced parsing...
//
BOOL CRequest::Match( const CString& rstr )
{
    if (strcmp(m_strRequest, rstr) == 0)
        return TRUE ;

    const char* pstr = m_strRequest ;
    char* p = strchr( pstr, '*' ) ;
    
    if (p) // there's a wildcard
    {
        if (strncmp( pstr, rstr, (p - pstr) ) == 0)
            return TRUE ;
    }
    
    return FALSE ;
}

// Each CRequest 'knows' what DataItem's it caused to be instantiated.
// 
CDataItem* CRequest::FindDataItem( const CString& rstr )
{
    CDataItem* pDataItem = NULL ;
    
    POSITION pos ;
    for (pos = m_lstDataItems.GetHeadPosition() ; pos != NULL;)
    {
        pDataItem = (CDataItem*)m_lstDataItems.GetNext(pos) ;
        CProperty*  pProp = (CProperty*)pDataItem->m_pValueProps->m_lstProps.GetHead() ;
        if (pProp->m_varValue.vt == VT_BSTR)
        {
            if (pProp->m_varValue.bstrVal == rstr)
                return pDataItem ;
        }
    }
    
    return NULL ;
}

BEGIN_MESSAGE_MAP(CRequest, CCmdTarget)
    //{{AFX_MSG_MAP(CRequest)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CRequest, CCmdTarget)
    //{{AFX_DISPATCH_MAP(CRequest)
    DISP_PROPERTY_EX(CRequest, "Request", GetRequest, SetRequest, VT_BSTR)
    DISP_PROPERTY_EX(CRequest, "Properties", GetProperties, SetProperties, VT_DISPATCH)
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

IMPLEMENT_SERIAL(CRequest, CCmdTarget,1)
#define new DEBUG_NEW

// CRequest can be serialized as part of the CDataObject serialization.
//
void CRequest::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        ar << m_strRequest ;
        // Save the index of our Properties colleciton
        ar << (LONG)m_pDataObject->GetPropertySetIndex( m_pProperties ) ;
    }
    else
    {
        m_pDataObject = (CDataObject*)ar.m_pDocument ;
        m_dwID = m_pDataObject->GenerateRequestID() ;
        ar >> m_strRequest ;   
        LONG    lIndex ;
        ar >> lIndex ;
        m_pProperties = m_pDataObject->FindPropertySet( (int)lIndex ) ;
    }
}    

/////////////////////////////////////////////////////////////////////////////
// CRequest message handlers

// CRequests know how to draw themselves.
int CRequest::Draw( CDC* pDC, CPoint ptStart, int n ) 
{
    TEXTMETRIC tm ;
    pDC->GetTextMetrics( &tm ) ;

    RECT rc ;                             
    rc.left = ptStart.x ;
    rc.top = ptStart.y ;
    rc.bottom = ptStart.y + tm.tmHeight ;
    rc.right = rc.left ;
    
    if (m_pDataObject)
    {
        COLINFO* rgColInfo ;
        int nCols ;
        CString strTitle ;
        char szName[256] ;
        
        rgColInfo = m_pDataObject->GetRequestsColInfo( &nCols, strTitle ) ;
             
        rc.right += rgColInfo[0].uiWidth ;
        wsprintf( szName, "%lu", m_dwID ) ;
            
        CColumnListBox::JustifiedTextOut( pDC, rc.left+2, rc.top, 
                           rc, szName, 0, rgColInfo[0].uiFormatFlags ) ;
        rc.left += rgColInfo[0].uiWidth ;

        rc.right += rgColInfo[1].uiWidth ;
        CColumnListBox::JustifiedTextOut( pDC, rc.left+2, rc.top,  
                           rc, (LPSTR)(LPCSTR)m_strRequest, 0, rgColInfo[1].uiFormatFlags ) ;
        rc.left += rgColInfo[1].uiWidth ;
                           
        POSITION pos ;
        CProperty* pProp ;
        for (n = 2, pos = m_pProperties->m_lstProps.GetHeadPosition() ; pos != NULL ; n++ )
        {
            pProp = (CProperty*)m_pProperties->m_lstProps.GetNext( pos ) ;
            if (pProp)
            {
                rc.right += rgColInfo[n].uiWidth ;
                CColumnListBox::JustifiedTextOut( pDC, rc.left+2, rc.top,  
                                   rc, (LPSTR)(LPCSTR)pProp->m_strName, 
                                   0, rgColInfo[n].uiFormatFlags ) ;
                rc.left += rgColInfo[n].uiWidth ;
            }
        }                        
    }
    return tm.tmHeight ;
}

void CRequest::CalcDisplaySize( CDC* pDC, CSize& sizeItem ) 
{  
    TEXTMETRIC tm ;
    pDC->GetTextMetrics( &tm ) ;
    sizeItem.cy = tm.tmHeight ;
    if (m_pDataObject) sizeItem.cx = m_pDataObject->GetWidth( pDC ) ;
    else sizeItem.cx = 10 ;
}

////////////////////////////////////////////////////////////
// OLE Automation methods/properties
//

// str = req.Request
//
BSTR CRequest::GetRequest()
{
    return m_strRequest.AllocSysString();
}

// req.Request = "MSFT" or req = "MSFT"
//
void CRequest::SetRequest(LPCSTR lpszNewValue)
{
    m_strRequest = lpszNewValue ;
    if (m_pDataObject)
    {
      
        m_pDataObject->UpdateAllViews( NULL, UPDATE_CHANGEREQUEST, this ) ;
        m_pDataObject->UpdateAllItems( NULL, UPDATE_CHANGEREQUEST, this ) ;
    }
}

// Set props = req.Properties
//
LPDISPATCH CRequest::GetProperties()
{
    if (m_pProperties == NULL || !m_pProperties->IsKindOf(RUNTIME_CLASS(CProperties)))
    {
        // TODO:  raise exception
        return NULL ;
    }    

    return m_pProperties->GetIDispatch(TRUE) ;
}

// Set req.Properties = props
//
void CRequest::SetProperties(LPDISPATCH newValue)
{
    CProperties*    pProps = (CProperties*)CCmdTarget::FromIDispatch( newValue ) ;

    if (pProps == NULL || !pProps->IsKindOf(RUNTIME_CLASS(CProperties)))
    {
        TRACE("CRequest::SetProperties() got a non-CProperties object\r\n") ;
        // TODO:  Implement exception
        return ;
    }

    if (m_pProperties != NULL)
        m_pProperties->ExternalRelease() ;

    m_pProperties = pProps ;
    m_pProperties->ExternalAddRef() ;  // We're keeping a pointer
}
 
