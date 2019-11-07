// WOSA/XRT XRTFrame Sample Application Version 1.00.007
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// WOSAXRT.H
//
// Revisions:
//  August 18, 1993     cek     First implementation.
//  Jan 4, 1994         cek     Updated to match 1.00.301 spec
//
// Implementation Notes:
//

#ifndef _WOSAXRT_H_
#define _WOSAXRT_H_
                            
// "Market Data (WOSA/XRT)" clipboard format structures
//
#define szCF_WOSAXRT100    "Market Data (WOSA/XRT)"

// Stream header
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
    
} MARKETDATA, *PMARKETDATA, FAR *LPMARKETDATA ;

// Update Item Header
//
typedef struct tagUPDATEITEM
{
    DWORD       dwRequestID ;       // ID of the Request Object
    DWORD       cProperties ;       // Number of properties
    DWORD       dwNext ;            // Offset from start of stream to next item
    DWORD       dwPropertySet ;     // Offset to associated property set
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
typedef struct tagPROPERTYSET
{
    DWORD       dwPropertySetID ;
    DWORD       cProperties ;
    DWORD       dwNext ;            // Offset from start of stream to next item
    
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

// Helper functions in WOSAXRT.CPP
//
LPMARKETDATA  WINAPI xrtCreateMarketData( const CLSID FAR* lpclsid, LPMARKETDATA FAR * ppXRT = NULL ) ;
void          WINAPI xrtDestroyMarketData( LPMARKETDATA pMD ) ;

LPUPDATEITEM  WINAPI xrtAddUpdateItem( LPMARKETDATA FAR* ppMD, DWORD dwID, DWORD dwPropSetID, DWORD cMaxProps ) ;
BOOL          WINAPI xrtAddUpdateItemProperty( LPMARKETDATA FAR* ppMD, LPUPDATEITEM FAR* ppUI, LPVARIANT pVar, DWORD dwPropertyID ) ;
LPUPDATEITEM  WINAPI xrtFindUpdateItem( LPMARKETDATA pMD, DWORD dwID ) ;

LPPROPERTYSET WINAPI xrtAddPropertySet( LPMARKETDATA FAR* ppMD, DWORD dwID ) ;
BOOL          WINAPI xrtAddProperty( LPMARKETDATA FAR* ppMD, LPPROPERTYSET FAR *pPS, LPCSTR pszName ) ;
LPPROPERTYSET WINAPI xrtFindPropertySet( LPMARKETDATA pMD, DWORD dwID ) ;

#endif // _WOSAXRT_H_

 
