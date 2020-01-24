// WOSAXRT.H
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
// July 15, 1993
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  August 18, 1993   cek     First implementation.
//
// Implementation Notes:
//
//

#ifndef _WOSAXRT_H_
#define _WOSAXRT_H_

#define XSM_SYMBOL 0x0001
#define XSM_HIGH   0x0002
#define XSM_LOW    0x0004
#define XSM_ASK    0x0008
#define XSM_VOLUME 0x0010
#define XSM_PE     0x0020  
#define XSM_ALL    0xFFFF

typedef struct tagXRTSTOCK
{
    UINT        uiID ;
    UINT        uiMembers ;     // XSM_ flags
    char        szSymbol[16] ;  // XSM_SYMBOL 0x0001
    ULONG       ulHigh ;        // XSM_HIGH   0x0002
    ULONG       ulLow ;         // XSM_LOW    0x0004
    ULONG       ulAsk ;         // XSM_ASK    0x0008
    ULONG       ulVolume ;      // XSM_VOLUME 0x0010
    ULONG       ulPE ;          // XSM_PE     0x0020
} XRTSTOCK, *PXRTSTOCK, FAR *LPXRTSTOCK ;

typedef struct tagXRTSTOCKS
{
    UINT        cStocks ;
    XRTSTOCK    rgStocks[1] ; // pointer access only
} XRTSTOCKS, *PXRTSTOCKS, FAR *LPXRTSTOCKS ;


#endif // _WOSAXRT_H_

 