/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  scroll.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  VT220 Terminal Emulation (scroll routines)
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.002  2/28/91 baw   Moved to self-contained DLL
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "vt220.h"

/************************************************************************
 *  BOOL ScrollEmulatorVert( HWND hWnd, WORD wScrollCmd, WORD wScrollPos )
 *
 *  Description:
 *     Scrolls the emulator window vertically.  The code to track
 *     the thumb position (SB_THUMBTRACK) was commented out.
 *     The ScrollDC() function is too slow to keep up when scrolled
 *     horizontally.
 *
 *  Comments:
 *      6/30/91  baw  Wrote this comment block.
 *
 ************************************************************************/

BOOL ScrollEmulatorVert( HWND hWnd, WORD wScrollCmd, WORD wScrollPos )
{
   int    nScrollAmt ;
   BOOL   fCursorWasShowing ;
   HECB   hECB ;
   NPECB  npECB ;
   RECT   rcScroll, rcClip, rcUpdate ;

   // Get Emulator Control Block
   hECB = GETHECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   switch (wScrollCmd)
   {
      case SB_TOP:
         nScrollAmt = -YOFFSET( npECB ) ;
         break ;

      case SB_BOTTOM:
         nScrollAmt = YSCROLL( npECB ) - YOFFSET( npECB ) ;
         break ;

      case SB_PAGEUP:
         nScrollAmt = -YSIZE( npECB ) ;
         break ;

      case SB_PAGEDOWN:
         nScrollAmt = YSIZE( npECB ) ;
         break ;

      case SB_LINEUP:
         nScrollAmt = -YCHAR( npECB ) ;
         break ;

      case SB_LINEDOWN:
         nScrollAmt = YCHAR( npECB ) ;
         break ;

      case SB_THUMBTRACK:
         nScrollAmt = wScrollPos - YOFFSET( npECB ) ;
         break ;

      case SB_THUMBPOSITION:
         nScrollAmt = wScrollPos - YOFFSET( npECB ) ;
         break ;

      default :
         LocalUnlock( hECB ) ;
         return ( FALSE ) ;
   }

   // Hide cursor if showing

   fCursorWasShowing = HideEmulatorCursor( hWnd ) ;

   if ((YOFFSET( npECB ) + nScrollAmt) > YSCROLL( npECB ))
      nScrollAmt = YSCROLL( npECB ) - YOFFSET( npECB ) ;
   if ((YOFFSET( npECB ) + nScrollAmt) < 0)
      nScrollAmt = -YOFFSET( npECB ) ;

   // Force update of window

   UpdateWindow( hWnd ) ;

   // Scroll the device context

   GetClientRect( hWnd, &rcScroll ) ;
   rcClip = rcScroll ;
   ScrollDC( SCREENDC( npECB ), 0, -nScrollAmt, &rcScroll, &rcClip,
             NULL, &rcUpdate ) ;
   InvalidateRect( hWnd, &rcUpdate, FALSE ) ;

   // Update scroll position 

   YOFFSET( npECB ) = YOFFSET( npECB ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_VERT, YOFFSET( npECB ), TRUE ) ;

   // Restore cursor if previously showing

   if (fCursorWasShowing)
      ShowEmulatorCursor( hWnd ) ;

   LocalUnlock( hECB ) ;
   return ( TRUE ) ;

} /* end of ScrollEmulatorVert() */

/************************************************************************
 *  BOOL ScrollEmulatorHorz( HWND hWnd, WORD wScrollCmd, WORD wScrollPos )
 *
 *  Description: 
 *     Scrolls the emulator window horizontally.  The code to track
 *     the thumb position (SB_THUMBTRACK) was commented out.
 *     The ScrollDC() function is too slow to keep up when scrolled
 *     horizontally.
 *
 *  Comments:
 *      6/30/91  baw  Wrote this comment.
 *
 ************************************************************************/

BOOL ScrollEmulatorHorz( HWND hWnd, WORD wScrollCmd, WORD wScrollPos )
{
   int    nScrollAmt ;
   BOOL   fCursorWasShowing ;
   HECB   hECB ;
   NPECB  npECB ;
   RECT   rcScroll, rcClip, rcUpdate ;

   // Get Emulator Control Block

   hECB = GETHECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   switch (wScrollCmd)
   {
      case SB_TOP:
         nScrollAmt = -XOFFSET( npECB ) ;
         break ;

      case SB_BOTTOM:
         nScrollAmt = XSCROLL( npECB ) - XOFFSET( npECB ) ;
         break ;

      case SB_PAGEUP:
         nScrollAmt = -XSIZE( npECB ) ;
         break ;

      case SB_PAGEDOWN:
         nScrollAmt = XSIZE( npECB ) ;
         break ;

      case SB_LINEUP:
         nScrollAmt = -XCHAR( npECB ) ;
         break ;

      case SB_LINEDOWN:
         nScrollAmt = XCHAR( npECB ) ;
         break ;

      case SB_THUMBTRACK:
         nScrollAmt = wScrollPos - XOFFSET( npECB ) ;
         break ;

      case SB_THUMBPOSITION:
         nScrollAmt = wScrollPos - XOFFSET( npECB ) ;
         break ;

      default :
         LocalUnlock( hECB ) ;
         return ( FALSE ) ;
   }

   // Hide cursor if showing

   fCursorWasShowing = HideEmulatorCursor( hWnd ) ;

   if ((XOFFSET( npECB ) + nScrollAmt) > XSCROLL( npECB ))
      nScrollAmt = XSCROLL( npECB ) - XOFFSET( npECB ) ;
   if ((XOFFSET( npECB ) + nScrollAmt) < 0)
      nScrollAmt = -XOFFSET( npECB ) ;

   // Force update of window

   UpdateWindow( hWnd ) ;

   // Scroll the device context

   GetClientRect( hWnd, &rcScroll ) ;
   rcClip = rcScroll ;
   ScrollDC( SCREENDC( npECB ), -nScrollAmt, 0, &rcScroll, &rcClip,
             NULL, &rcUpdate ) ;
   InvalidateRect( hWnd, &rcUpdate, FALSE ) ;

   // Update scroll position

   XOFFSET( npECB ) = XOFFSET( npECB ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_HORZ, XOFFSET( npECB ), TRUE ) ;

   // Restore cursor if previously showing

   if (fCursorWasShowing)
      ShowEmulatorCursor( hWnd ) ;

   LocalUnlock( hECB ) ;
   return ( TRUE ) ;

} /* end of ScrollEmulatorHorz() */

/************************************************************************
 * End of File: scroll.c
 ************************************************************************/

