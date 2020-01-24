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
 *     Remarks:  VT220 Terminal Emulation (size routines)
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.002  2/28/91 baw   Moved to self-contained DLL
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "vt220.h"

/************************************************************************
 *  BOOL SizeEmulator( HWND hWnd, WORD wVertSize, WORD wHorzSize )
 *
 *  Description:
 *     Sizes the emulator window, sets up scrolling range and
 *     scroll position.
 *
 *  Comments:
 *     6/19/91  baw  Added this comment block.
 *
 ************************************************************************/

BOOL SizeEmulator( HWND hWnd, WORD wVertSize, WORD wHorzSize )
{
   int          nScrollAmt ;
   LOCALHANDLE  hECB ;
   NPECB        npECB ;

   // Get Emulator Control Block
   hECB = GETHECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   YSIZE( npECB ) = (int) wVertSize ;
   YSCROLL( npECB ) =
      max( 0, (MAXROWS( npECB ) * YCHAR( npECB )) - YSIZE( npECB ) ) ;
   nScrollAmt = min( YSCROLL( npECB ), YOFFSET( npECB ) ) - YOFFSET( npECB ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;
   YOFFSET( npECB ) = YOFFSET( npECB ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_VERT, YOFFSET( npECB ), FALSE ) ;
   SetScrollRange( hWnd, SB_VERT, 0, YSCROLL( npECB ), TRUE ) ;

   XSIZE( npECB ) = (int) wHorzSize ;
   XSCROLL( npECB ) =
      max( 0, (MAXCOLS( npECB ) * XCHAR( npECB )) - XSIZE( npECB ) ) ;
   nScrollAmt = min( XSCROLL( npECB ), XOFFSET( npECB ) ) - XOFFSET( npECB ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;
   XOFFSET( npECB ) = XOFFSET( npECB ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_HORZ, XOFFSET( npECB ), FALSE ) ;
   SetScrollRange( hWnd, SB_HORZ, 0, XSCROLL( npECB ), TRUE ) ;

   // InvalidateRect( hWnd, NULL, TRUE ) ;

   LocalUnlock( hECB ) ;
   return( TRUE ) ;

} /* end of SizeEmulator() */

/************************************************************************
 * End of File: size.c
 ************************************************************************/


