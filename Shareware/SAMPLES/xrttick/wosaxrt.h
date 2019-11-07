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
// WOSA/XRT Version 1.01
//
// WOSAXRT.H
//
// Revisions:
//  August 18, 1993     cek     First implementation.
//  Jan 4, 1994         cek     Updated to match 1.00.301 spec
//  Sept. 29 1994       cek     Updated to the 1.01 specifcation.
//
// Implementation Notes:
//
//  *   This file has been updated to match revsion 1.01 of the
//      WOSA/XRT Design Specification.  To support the 1.00 
//      release you must modify the #define XRT_VER below
//      to be 0x0100.
//

#ifndef __WOSAXRT_H__
#define __WOSAXRT_H__

// XRT_VER indicates the WOSA/XRT Design Specification revision level.
// Version 1.00 is 0x0100
// Version 1.01 is 0x0101
// Version 2.95 would be 0x0295
//
#define XRT_VER 0x0101

// WOSA/XRT Clipboard format strings
//
#if XRT_VER >= 0x0101
#define szCF_WOSAXRT101W    "{00023C00-0000-0000-C000-000000000046}"   // Unicode
#define szCF_WOSAXRT101A    "{00023C01-0000-0000-C000-000000000046}"   // Ansi
#else
#define szCF_WOSAXRT100     "Market Data (WOSA/XRT)"         // obsolete
#endif

// Stream header
//
// sizeof(MARKETDATA) == 48 bytes
//
typedef struct tagMARKETDATA
{
    DWORD   cbStream ;          // Count of bytes in entire structure
    CLSID   clsID ;             // Data Object CLSID
    DWORD   dwStatus ;          // Indicates the status of the Data Object
    DWORD   dwSubStatus ;       // Additional status information
    DWORD   dwVendorData1 ;     // 
    DWORD   dwVendorData2 ;     // 
    DWORD   dwPropSetOffset;    // Offset to first property set
    DWORD   dwItemOffset ;      // Offset to first data item

#if XRT_VER >= 0x0101
    DWORD   dwReserved ;        // Reserved
#endif
    
} MARKETDATA, *PMARKETDATA, FAR *LPMARKETDATA ;

// Update Item Header
//
// sizeof(UPDATEITEM) == 56 bytes  
//
typedef struct tagUPDATEITEM
{
#if XRT_VER >= 0x0101
    DWORD       dwDataItemID ;
#endif
    DWORD       dwRequestID ;       // ID of the Request Object
    DWORD       cProperties ;       // Number of properties
    DWORD       dwNext ;            // Offset from start of stream to next item
    DWORD       dwPropertySet ;     // Offset to associated property set

#if XRT_VER >= 0x0101
    DWORD       dwReserved ;        // Reserved
#endif

 // UPDATEITEMPROP  rgPropIDValue[0];   // Array of PropertyID/ValueOffset pairs
 // UPDATEITEMPROP  rgPropIDValue[1];   // Array of PropertyID/ValueOffset pairs
 // ...
 // UPDATEITEMPROP  rgPropIDValue[cProperties-1]; 
    
} UPDATEITEM, *PUPDATEITEM, FAR *LPUPDATEITEM ;

// PropertyID/ValueOffset pairs
//
typedef struct tagUPDATEITEMPROP
{
    DWORD       dwPropertyID;       // Property ID
    DWORD       dwValueOffset ;     // Offset to the VARIANT (from start of 
                                    // the Update Item Header)
} UPDATEITEMPROP, *PUPDATEITEMPROP, FAR* LPUPDATEITEMPROP ;

// Property sets               
//
// sizeof(PROPERTYSET) == 32 bytes 
//
typedef struct tagPROPERTYSET
{
    DWORD       dwPropertySetID ;
    DWORD       cProperties ;
    DWORD       dwNext ;            // Offset from start of stream to next item

#if XRT_VER >= 0x0101
    DWORD       dwReserved ;        // Reserved
#endif
    
 // DWORD       cbName ;            /* rgProperty[0] (property ID == 0)*/
 // char        szName[cbName] ;
 // DWORD       cbName ;            /* rgProperty[1] (property ID == 1)*/
 // char        szName[cbName] ;
 // ...
 // DWORD       cbName ;            /* rgProperty[cProperties-1] (property ID == cProperties-1) */
 // char        szName[cbName] ;
 
} PROPERTYSET, *PPROPERTYSET, FAR *LPPROPERTYSET ;
  
// WOSA/XRT status values
//
#define xrtStatusNormal         0x00
#define xrtStatusError          0x01

// Subvalues for xrtStatusError
//
#define xrtErrorGeneral         0x00
#define xrtErrorFeedDown        0x01
#define xrtErrorNoPrivledge     0x02

// Helper functions found in WOSAXRT.CPP
//
// NOTE:  These functions are provided as SAMPLE CODE.
//
LPMARKETDATA  WINAPI xrtCreateMarketData( const CLSID FAR* lpclsid, LPMARKETDATA FAR * ppXRT = NULL ) ;
void          WINAPI xrtDestroyMarketData( LPMARKETDATA pMD ) ;

#if XRT_VER >= 0x0101
// 1.01 added dwDataItemID
LPUPDATEITEM WINAPI xrtAddUpdateItem( LPMARKETDATA FAR* ppMD, DWORD dwDataItemID, DWORD dwRequestID, DWORD dwPropSetID, DWORD cMaxProps ) ;
#else
LPUPDATEITEM WINAPI xrtAddUpdateItem( LPMARKETDATA FAR* ppMD, DWORD dwRequestID, DWORD dwPropSetID, DWORD cMaxProps ) ;
#endif

BOOL          WINAPI xrtAddUpdateItemProperty( LPMARKETDATA FAR* ppMD, LPUPDATEITEM FAR* ppUI, LPVARIANT pVar, DWORD dwPropertyID ) ;
LPUPDATEITEM  WINAPI xrtFindUpdateItem( LPMARKETDATA pMD, DWORD dwRequestID ) ;

#if XRT_VER >= 0x0101
// 1.01 added to support finding by DataItemID
LPUPDATEITEM  WINAPI xrtFindUpdateItemByDataItem( LPMARKETDATA pMD, DWORD dwDataItemID ) ;
#endif

LPPROPERTYSET WINAPI xrtAddPropertySet( LPMARKETDATA FAR* ppMD, DWORD dwID ) ;
BOOL          WINAPI xrtAddProperty( LPMARKETDATA FAR* ppMD, LPPROPERTYSET FAR *pPS, LPCSTR pszName ) ;
LPPROPERTYSET WINAPI xrtFindPropertySet( LPMARKETDATA pMD, DWORD dwID ) ;

#endif // __WOSAXRT_H__

 
