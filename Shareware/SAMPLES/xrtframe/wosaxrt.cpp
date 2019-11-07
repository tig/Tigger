// WOSA/XRT XRTFrame Sample Application Version 1.01
//
// Copyright (c) 1993-94 Microsoft Corporation, All Rights Reserved.
//
//      This is a part of the Microsoft Source Code Samples. 
//      This source code is only intended as a supplement to 
//      Microsoft Development Tools and/or WinHelp documentation.
//      See these sources for detailed information regarding the 
//      Microsoft samples programs.
//
// WOSAXRT.CPP
// 
// Helper routines for allocating and maintaining WOSA/XRT related
// data structures.
//
// Revisions:
//  August 18, 1993     cek     First implementation.
//  Jan 4, 1994         cek     Updated to match 1.00.301 spec
//  September 29, 1994  cek     Updated to 1.01
//
// Implementation Notes:
//
// 

#include "stdafx.h"
#include "windowsx.h"
#include "wosaxrt.h"

DWORD xrtReAlloc( LPMARKETDATA FAR* ppMD, DWORD cb ) ;
DWORD xrtAddValue( LPMARKETDATA FAR* ppMD, LPVARIANT pVar ) ;

#define GMEM_XRT  (GMEM_MOVEABLE | GMEM_ZEROINIT)

//-------------------------------------------------------------------------
// MarketData Helpers
//                                                   
// Create a new "Market Data (WOSA/XRT)" data stream in
// global memory.
//
LPMARKETDATA WINAPI xrtCreateMarketData( const CLSID FAR* lpclsid, LPMARKETDATA FAR * ppXRT )
{
    ASSERT(lpclsid) ;
    LPMARKETDATA p ;
    
    if (ppXRT == NULL || *ppXRT == NULL)
    {
        //TRACE( "Allocating %d using GlobalAllocPtr\r\n", sizeof(MARKETDATA) ) ;
        p = (LPMARKETDATA)GlobalAllocPtr(  GMEM_XRT, sizeof(MARKETDATA) ) ;
        //TRACE( "   GlobalSize = %lu\r\n", GlobalSize(GlobalPtrHandle(p)) ) ; 
    }
    else
    {
        p = (LPMARKETDATA)GlobalReAllocPtr(*ppXRT, sizeof(MARKETDATA), GMEM_XRT ) ;
    }
    
    if (p != NULL)
    {
        p->cbStream = sizeof(MARKETDATA) ;
        p->clsID = *lpclsid ;
        p->dwStatus = xrtStatusNormal ;
        p->dwSubStatus = xrtStatusNormal ;
        p->dwVendorData1 = 0xCDCDCDCD ;
        p->dwVendorData2 = 0xCDCDCDCD ;
        p->dwPropSetOffset = 0 ;
        p->dwItemOffset = 0 ;

        if (ppXRT != NULL)
            *ppXRT = p ;
    }
    
    return p ;
}

void WINAPI xrtDestroyMarketData( LPMARKETDATA pMD ) 
{                         
    ASSERT(pMD) ;
    //TRACE( "GlobalFreePtr...\r\n") ;
    GlobalFreePtr( pMD ) ;
}

//-------------------------------------------------------------------------
// UpdateItem Helpers
//                                                   
LPUPDATEITEM WINAPI xrtFindUpdateItem( LPMARKETDATA FAR* ppMD, DWORD dwRequestID )
{                        
    ASSERT(ppMD) ;
    LPMARKETDATA pMD = *ppMD ;
    ASSERT(pMD) ;

    LPUPDATEITEM  pUD = NULL ;
    DWORD dwOffset = pMD->dwItemOffset ;
    
    while (dwOffset != NULL) 
    {
        pUD = (LPUPDATEITEM)((BYTE FAR*)pMD + dwOffset) ;
        if (pUD->dwRequestID == dwRequestID)
            return pUD ;
        dwOffset = pUD->dwNext ;
    }
    
    return NULL ;
}

#if XRT_VER >= 0x0101
// 1.01 Helper to find an updateitem via dwDataItemID
//
LPUPDATEITEM WINAPI xrtFindUpdateItemByDataItem( LPMARKETDATA FAR* ppMD, DWORD dwDataItemID )
{                        
    ASSERT(ppMD) ;
    LPMARKETDATA pMD = *ppMD ;
    ASSERT(pMD) ;

    LPUPDATEITEM  pUD = NULL ;
    DWORD dwOffset = pMD->dwItemOffset ;
    
    while (dwOffset != NULL) 
    {
        pUD = (LPUPDATEITEM)((BYTE FAR*)pMD + dwOffset) ;
        if (pUD->dwDataItemID == dwDataItemID)
            return pUD ;
        dwOffset = pUD->dwNext ;
    }
    
    return NULL ;
}
#endif

#if XRT_VER >= 0x0101
// 1.01 added dwDataItemID
LPUPDATEITEM WINAPI xrtAddUpdateItem( LPMARKETDATA FAR* ppMD, DWORD dwDataItemID, DWORD dwRequestID, DWORD dwPropSetID, DWORD cMaxProps ) 
#else
LPUPDATEITEM WINAPI xrtAddUpdateItem( LPMARKETDATA FAR* ppMD, DWORD dwRequestID, DWORD dwPropSetID, DWORD cMaxProps ) 
#endif
{
    ASSERT(ppMD) ;
    LPMARKETDATA pMD = *ppMD ;
    ASSERT(pMD) ;
    
    // First find the last UpdateItem
    //
    LPUPDATEITEM pUpdateItem = NULL ;
    for (DWORD dwOffset = pMD->dwItemOffset ; dwOffset != NULL ; dwOffset = pUpdateItem->dwNext)
    {
        pUpdateItem = (LPUPDATEITEM)((LPBYTE)pMD + dwOffset) ;
    }

    DWORD   dwOld = 0 ;
    if (pUpdateItem)
        dwOld = (LPBYTE)pUpdateItem - (LPBYTE)*ppMD ;
     
    // Allocate enuf for the UPDATE item including cMaxProps PropID/Value pairs
    //
    dwOffset = xrtReAlloc( ppMD, sizeof(UPDATEITEM) + (cMaxProps * sizeof(UPDATEITEMPROP)) ) ;
    if (dwOffset == 0)
        return NULL ;
    
    pMD = *ppMD ;   // one less pointer indirection

    if (pUpdateItem != NULL)    
    {
        // we found a dataitem set above
        // The realloc may have changed the location of the dataitem
        pUpdateItem = (LPUPDATEITEM)((LPBYTE)*ppMD + dwOld) ;
        pUpdateItem->dwNext = dwOffset ;
    }
    else
        pMD->dwItemOffset = dwOffset ;
    
    LPPROPERTYSET pPS = xrtFindPropertySet( *ppMD, dwPropSetID ) ;
    ASSERT(pPS ) ;
    DWORD dwPropSetOffset = (LPBYTE)pPS - (LPBYTE)*ppMD ; 
    // Now point to the new one
    pUpdateItem = (LPUPDATEITEM)((LPBYTE)pMD + dwOffset) ;
    if (pUpdateItem != NULL)
    {
#if XRT_VER >= 0x0101
        pUpdateItem->dwDataItemID = dwDataItemID ;  // 1.01
#endif
        pUpdateItem->dwRequestID = dwRequestID ;
        pUpdateItem->cProperties = 0 ;
        pUpdateItem->dwNext = 0 ;
        pUpdateItem->dwPropertySet = dwPropSetOffset ;
        
        LPUPDATEITEMPROP pUIP = (LPUPDATEITEMPROP)((LPBYTE)pUpdateItem + sizeof(UPDATEITEM)) ;
        for (DWORD c = 0 ; c < pUpdateItem->cProperties ; c++)
        {
            pUIP->dwPropertyID = 0 ;
            pUIP->dwValueOffset = 0 ;
            pUIP += sizeof(UPDATEITEMPROP) ;
        }        
        
    }
    return pUpdateItem ;
}

BOOL WINAPI xrtAddUpdateItemProperty( LPMARKETDATA FAR* ppMD, LPUPDATEITEM FAR* ppUI, LPVARIANT pVar, DWORD dwPropertyID ) 
{
    ASSERT(ppMD && *ppMD) ;
    ASSERT(ppUI && *ppUI) ;
    ASSERT(pVar) ;
    
    // Allocate and copy the variant, getting it's offset.
    DWORD dwValueOffset ;
    DWORD dwOld ;
    
    dwOld = (LPBYTE)*ppUI - (LPBYTE)*ppMD ;

    dwValueOffset = xrtAddValue( ppMD, pVar ) ;
    ASSERT(dwValueOffset) ;
    
    *ppUI = (LPUPDATEITEM)((LPBYTE)*ppMD + dwOld) ;

//afxDump << "cProps : " << (*ppUI)->cProperties << "\n" ;
    // dwPropertyID[n]
    LPUPDATEITEMPROP lpUIP = (LPUPDATEITEMPROP)((LPBYTE)*ppUI + sizeof(UPDATEITEM) + (sizeof(UPDATEITEMPROP) * (*ppUI)->cProperties)) ;
    lpUIP->dwPropertyID = dwPropertyID ;
//xrtDebugDumpItems( afxDump, *ppMD ) ;
    
    // dwValueOffset[n]
    lpUIP->dwValueOffset = dwValueOffset - ((LPBYTE)*ppUI - (LPBYTE)*ppMD) ;

    (*ppUI)->cProperties++ ;      
    
    return TRUE ;
}
 
//-------------------------------------------------------------------------
// Property Set Helpers
//                                                   
LPPROPERTYSET WINAPI xrtAddPropertySet( LPMARKETDATA FAR* ppMD, DWORD dwID ) 
{
    ASSERT(ppMD) ;
    LPMARKETDATA pMD = *ppMD ;
    ASSERT(pMD) ;
    
    // First find the last Property Set
    //
    LPPROPERTYSET  pPS = NULL ;
    for (DWORD dwOffset = pMD->dwPropSetOffset ; dwOffset != NULL ; dwOffset = pPS->dwNext)
    {
        pPS = (LPPROPERTYSET)((BYTE FAR*)pMD + dwOffset) ;
    }

    DWORD   dwOldPS = (LPBYTE)pPS - (LPBYTE)*ppMD ;
     
    dwOffset = xrtReAlloc( ppMD, sizeof(PROPERTYSET) ) ;
    if (dwOffset == 0)
        return NULL ;
    
    pMD = *ppMD ;   // one less pointer indirection

    if (pPS != NULL)    
    {
        // we found a property set above
        // The realloc may have changed the location of the propset
        pPS = (LPPROPERTYSET)((LPBYTE)*ppMD + dwOldPS) ;
        pPS->dwNext = dwOffset ;
    }
    else
        pMD->dwPropSetOffset = dwOffset ;
    
    // Now point to the new one
    pPS = (LPPROPERTYSET)((LPBYTE)pMD + dwOffset) ;
    if (pPS != NULL)
    {
        pPS->dwPropertySetID = dwID ;
        pPS->cProperties = 0 ;
        pPS->dwNext = 0 ;
    }
    return pPS ;
}

// Note! It is only possible to add properties to a property set that has just been
// created.   In other words, the property set specified by *ppPS must have just
// been created with xrtAddPropertySet.
//
BOOL WINAPI xrtAddProperty( LPMARKETDATA FAR* ppMD, LPPROPERTYSET FAR* ppPS, LPCSTR pszName ) 
{
    ASSERT(ppMD && *ppMD) ;
    ASSERT(ppPS && *ppPS) ;
    ASSERT(pszName) ;
    
    DWORD   dwOldPS = (LPBYTE)*ppPS - (LPBYTE)*ppMD ;
    
    DWORD dwLen = lstrlen( pszName ) + 1 ; // incl. '\0'
    DWORD dwOffset = xrtReAlloc( ppMD, sizeof(DWORD) + dwLen) ; 
    if (dwOffset == 0)
        return FALSE ;
    
    *ppPS = (LPPROPERTYSET)((LPBYTE)*ppMD + dwOldPS) ;
    
    // cbName
    *(LPDWORD)((LPBYTE)*ppMD + dwOffset) = dwLen ;
    
    dwOffset +=  + sizeof(DWORD) ;
    // szName
    lstrcpy( (LPSTR)((LPBYTE)*ppMD + dwOffset), pszName ) ;
    
    char *p = (LPSTR)((LPBYTE)*ppMD + dwOffset) ;
    
    
    p = NULL ;
    
    (*ppPS)->cProperties++ ;      
    
    return TRUE ;
}

LPPROPERTYSET WINAPI xrtFindPropertySet( LPMARKETDATA pMD, DWORD dwID )
{                        
    ASSERT(pMD) ;

    LPPROPERTYSET  pPS = NULL ;
    DWORD dwOffset = pMD->dwPropSetOffset ;
    
    while (dwOffset != NULL) 
    {
        pPS = (LPPROPERTYSET)((BYTE FAR*)pMD + dwOffset) ;
        if (pPS->dwPropertySetID == dwID)
            return pPS ;
        dwOffset = pPS->dwNext ;
    }
    
    return NULL ;
}


//-------------------------------------------------------------------------
// Helper helpers
//                                                   
DWORD xrtReAlloc( LPMARKETDATA FAR* ppMD, DWORD cb )
{
    ASSERT(ppMD) ;
    ASSERT(*ppMD) ;

    DWORD   dwOffsetEnd = (*ppMD)->cbStream ;
    (*ppMD)->cbStream += cb ;

    // GlobalRealloc does not destroy the original if it fails.
    //TRACE( "ReAllocating to %d using GlobalReAllocPtr\r\n", (UINT)(*ppMD)->cbStream ) ;
    LPMARKETDATA  pMD2 = (LPMARKETDATA)GlobalReAllocPtr(*ppMD, (*ppMD)->cbStream, GMEM_XRT ) ;
    //TRACE( "   GlobalSize = %lu\r\n", GlobalSize(GlobalPtrHandle(pMD2)) ) ; 

    ASSERT(pMD2) ;
    if (pMD2 == 0)
    {
        TRACE("GlobalReAlloc failed in xrtReAlloc!\r\n" ) ;
        return 0 ;
    }
        
    *ppMD = pMD2 ;
    
    return dwOffsetEnd ;
}

DWORD xrtAddValue( LPMARKETDATA FAR* ppMD, LPVARIANT pVar ) 
{
    ASSERT(ppMD) ;
    ASSERT(pVar) ;
    USES_CONVERSION;
    
    // Figure out how long it'll be
    DWORD cb = sizeof(VARIANT) ;     
    DWORD cbStr = 0 ;
    if (pVar->vt == VT_BSTR)
    {
        // If it's a BSTR allocate enuf on the end for the DWORD 
        // count of bytes and the NULL terminated string
        //
        cbStr = SysStringLen( pVar->bstrVal )+ 1 ;
        cb +=  cbStr + sizeof(DWORD) ;
    }        
    else if (pVar->vt == (VT_VARIANT | VT_BYREF))
    {
        ASSERT(0) ;
        TRACE("Writing properties of properties not yet implemented!\r\n") ;
        return 0 ;
    }
    else if (pVar->vt > VT_BOOL)
    {
        // Out of our range
        TRACE("xrtAddValue called with a bad VARAINT type\r\n") ;
        return 0 ;
    }
    
    
    DWORD  dwOffset = (*ppMD)->cbStream ;
    (*ppMD)->cbStream += cb ;

    // GlobalRealloc does not destroy the original if it fails.
    //TRACE( "ReAllocating to %d using GlobalReAllocPtr\r\n", (UINT)(*ppMD)->cbStream ) ;
    
    LPMARKETDATA  pMD2 = (LPMARKETDATA)GlobalReAllocPtr(*ppMD, (*ppMD)->cbStream, GMEM_XRT ) ;
    
    //TRACE( "   GlobalSize = %lu\r\n", GlobalSize(GlobalPtrHandle(pMD2)) ) ; 

    ASSERT(pMD2) ;
    if (pMD2 == 0)
    {
        TRACE("GlobalReAlloc failed in xrtReAlloc!\r\n" ) ;
        return 0 ;
    }
        
    *ppMD = pMD2 ;
    
    memcpy( (LPBYTE)pMD2 + dwOffset, pVar, sizeof(VARIANT) ) ;
    if (pVar->vt == VT_BSTR)  
    {
        memcpy( (LPBYTE)pMD2 + dwOffset + sizeof(VARIANT), &cbStr, sizeof(cbStr) ) ;
        lstrcpy( (LPSTR)(LPBYTE)pMD2 +  dwOffset + sizeof(VARIANT) + sizeof(cbStr), OLE2T(pVar->bstrVal)) ;
    }
    else if (pVar->vt == VT_VARIANT | VT_BYREF)
    {
        // TODO: Implement
    }
    
    return dwOffset ;
}


