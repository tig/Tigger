/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  post.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  NetBIOS Communications DLL, message posting function
 *
 *   Revisions:  
 *     01.00.000  3/ 6/91 baw   Initial version, initial build
 *
 ************************************************************************/

#include "netbios.h"

/************************************************************************
 *  VOID FAR _cdecl interrupt PostNetBIOS( WORD es, WORD ds, WORD di,
 *                                         WORD si, WORD bp, WORD sp,
 *                                         WORD bx, WORD dx, WORD cx,
 *                                         WORD ax, WORD ip, WORD cs,
 *                                         WORD flags )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

VOID FAR _cdecl interrupt PostNetBIOS( WORD es, WORD ds, WORD di, WORD si,
                                       WORD bp, WORD sp, WORD bx, WORD dx,
                                       WORD cx, WORD ax, WORD ip, WORD cs,
                                       WORD flags )
{
   LPXNCB  lpXNCB ;

   lpXNCB = (LPXNCB) MAKELONG( bx, es ) ;

   PostMessage( lpXNCB -> hWnd, WM_EVENT_RXCHAR, lpXNCB -> hNCB,
                (long) lpXNCB ) ;

} /* end of PostNetBIOS() */

/************************************************************************
 * End of File: post.c
 ************************************************************************/

