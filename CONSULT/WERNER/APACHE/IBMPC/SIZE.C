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
 *     Remarks:  IBMPC Terminal Emulation (size routines)
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.002  2/28/91 baw   Moved to self-contained DLL
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "ibmpc.h"

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
   int    nScrollAmt ;
   HECB   hECB ;
   NPECB  npECB ;

   // Get Emulator Control Block

   hECB = GETECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   YSIZE = (int) wVertSize ;
   YSCROLL = max( 0, (MAXROWS * YCHAR) - YSIZE ) ;
   nScrollAmt = min( YSCROLL, YOFFSET ) - YOFFSET ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;
   YOFFSET = YOFFSET + nScrollAmt ;
   SetScrollPos( hWnd, SB_VERT, YOFFSET, FALSE ) ;
   SetScrollRange( hWnd, SB_VERT, 0, YSCROLL, TRUE ) ;

   XSIZE = (int) wHorzSize ;
   XSCROLL = max( 0, (MAXCOLS * XCHAR) - XSIZE ) ;
   nScrollAmt = min( XSCROLL, XOFFSET ) - XOFFSET ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;
   XOFFSET = XOFFSET + nScrollAmt ;
   SetScrollPos( hWnd, SB_HORZ, XOFFSET, FALSE ) ;
   SetScrollRange( hWnd, SB_HORZ, 0, XSCROLL, TRUE ) ;

   // InvalidateRect( hWnd, NULL, TRUE ) ;

   LocalUnlock( hECB ) ;
   return( TRUE ) ;

} /* end of SizeEmulator() */

/************************************************************************
 * End of File: size.c
 ************************************************************************/


