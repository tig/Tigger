/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  dde.h
 *
 ***********************************************************************/

#ifndef _DDE_H_

#define _DDE_H_

#include <ddeml.h>

#include "..\..\inc\messages.h"

#define MAX_ITEM        64
#define MAX_TOPIC       64
#define MAX_COMMENT     64
#define MAX_EXEC        128


/****** Structrues ******/

// this structure depends on the fact that EXPENTRY is defined as
// pascal calling convention.  This allows us to merely point to
// the last parameter of DdeCallback and cast the pointer to this
// structure to save stack space. (a common hack)

typedef struct _XFERINFO
{  
    DWORD lData2;
    DWORD lData1;
    HDDEDATA hData;
    HSZ hszItem;
    HSZ hszTopic;
    HCONV hConv;
    UINT wFmt;
    UINT wType;                            
} XFERINFO, *PXFERINFO, FAR *LPXFERINFO ;

typedef struct _ITEMLIST
{
    HSZ        hszItem ;
    HDDEDATA   (CALLBACK *lpfnCallback)(LPXFERINFO, UINT);
    UINT       wIDS ;   /* string table entry */
} ITEMLIST, *PITEMLIST, FAR *LPITEMLIST ;

typedef struct _CMDLIST
{
    HSZ hszCmd;
    HDDEDATA (CALLBACK *lpfnCallback)(HSZ, LPSTR, UINT);
    UINT       wIDS ;   /* string table entry */
} CMDLIST, *PCMDLIST, FAR *LPCMDLIST ;


typedef struct _TOPICLIST
{
    HSZ           hszTopic ;
    LPITEMLIST    lpItemList ;
    UINT          cItems ;
    LPCMDLIST     lpCmdList ;
    UINT          cCmds ;
    UINT          wIDS ;
} TOPICLIST, *PTOPICLIST, FAR *LPTOPICLIST;

typedef struct {
    ATOM atom;
    PSTR sz;
} FORMATINFO;

// Exported vars
extern GENERICMSG  gMsg ;
extern DWORD    dwDDEID ;
extern HSZ      hszAppName ;
extern BOOL     fConnected ;
extern BOOL     fLoggedIn ;
extern char     szLastDdeErr[] ;

extern FORMATINFO aFormats[] ;
extern TOPICLIST  Topics[] ;


//DDE.C - DDE stuff.  Prefixed by 'dde' are exported from this
// module.
HDDEDATA EXPENTRY fnDdeCallback( UINT     wType,
                                 UINT     wFmt,
                                 HCONV    hConv,
                                 HSZ      hsz1,
                                 HSZ      hsz2,
                                 HDDEDATA hData,
                                 DWORD    dwData1,
                                 DWORD    dwData2 ) ;


BOOL FAR PASCAL ddeInitDDEServer( void ) ;
BOOL FAR PASCAL ddeUnInitDDEServer( void ) ;

         
#endif

/************************************************************************
 * End of File: dde.h
 ***********************************************************************/

