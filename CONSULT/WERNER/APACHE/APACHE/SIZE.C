/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  size.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Sizing routines for the session window
 *
 *   Revisions:  
 *     01.00.000  6/19/91 baw   Wrote it.
 *
 ************************************************************************/

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"
#include "session.h"
#include "size.h"

/************************************************************************
 *  BOOL SizeWindows( HWND hWnd, WORD wVertSize, WORD wHorzSize )
 *
 *  Description: 
 *     Arranges child windows (status, emulator, etc.) based on
 *     heights, widths and what we have available.
 *
 *  Comments:
 *      6/18/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL SizeWindows( HWND hWnd, WORD wVertSize, WORD wHorzSize )
{
   WORD  wHeightTop ;

   LOCALHANDLE  hSCB ;
   NPSCB        npSCB ;

   // Get SCB from Window structure
   if (NULL == (hSCB = GETSCB( hWnd )))
      return ( FALSE ) ;

   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
      return ( FALSE ) ;

#ifdef NOT_USED
   DP5( hWDB, "SizeWindows()" ) ;

   wHeightTop = (WORD) SendMessage( EMUTOPSTATWND, ST_GETHEIGHT, 0, 0L ) ;
   MoveWindow( EMUTOPSTATWND, 0, -1, wHorzSize, wHeightTop, TRUE ) ;
#else
   wHeightTop = 0 ;
#endif
   MoveWindow( EMULATORWND, 0, wHeightTop - 1, wHorzSize,
               wVertSize - wHeightTop + 1, TRUE ) ;

   LocalUnlock( hSCB ) ;
   return ( TRUE ) ;

} /* end of SizeWindows() */

/************************************************************************
 * End of File: size.c
 ************************************************************************/

