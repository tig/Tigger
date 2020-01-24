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
 *     Remarks:  IBMPC Terminal Emulation (scroll routines)
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.002  2/28/91 baw   Moved to self-contained DLL
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "ibmpc.h"

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
   hECB = GETECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   switch (wScrollCmd)
   {
      case SB_TOP:
         nScrollAmt = -YOFFSET ;
         break ;

      case SB_BOTTOM:
         nScrollAmt = YSCROLL - YOFFSET ;
         break ;

      case SB_PAGEUP:
         nScrollAmt = -YSIZE ;
         break ;

      case SB_PAGEDOWN:
         nScrollAmt = YSIZE ;
         break ;

      case SB_LINEUP:
         nScrollAmt = -YCHAR ;
         break ;

      case SB_LINEDOWN:
         nScrollAmt = YCHAR ;
         break ;

      case SB_THUMBTRACK:
         nScrollAmt = wScrollPos - YOFFSET ;
         break ;

      case SB_THUMBPOSITION:
         nScrollAmt = wScrollPos - YOFFSET ;
         break ;

      default :
         LocalUnlock( hECB ) ;
         return ( FALSE ) ;
   }

   // Hide cursor if showing

   fCursorWasShowing = HideEmulatorCursor( hWnd ) ;

   if ((YOFFSET + nScrollAmt) > YSCROLL)
      nScrollAmt = YSCROLL - YOFFSET ;
   if ((YOFFSET + nScrollAmt) < 0)
      nScrollAmt = -YOFFSET ;

   // Force update of window

   UpdateWindow( hWnd ) ;

   // Scroll the device context

   GetClientRect( hWnd, &rcScroll ) ;
   rcClip = rcScroll ;
   ScrollDC( SCREENDC, 0, -nScrollAmt, &rcScroll, &rcClip, NULL, &rcUpdate ) ;
   InvalidateRect( hWnd, &rcUpdate, FALSE ) ;

   // Update scroll position 

   YOFFSET = YOFFSET + nScrollAmt ;
   SetScrollPos( hWnd, SB_VERT, YOFFSET, TRUE ) ;

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

   hECB = GETECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   switch (wScrollCmd)
   {
      case SB_TOP:
         nScrollAmt = -XOFFSET ;
         break ;

      case SB_BOTTOM:
         nScrollAmt = XSCROLL - XOFFSET ;
         break ;

      case SB_PAGEUP:
         nScrollAmt = -XSIZE ;
         break ;

      case SB_PAGEDOWN:
         nScrollAmt = XSIZE ;
         break ;

      case SB_LINEUP:
         nScrollAmt = -XCHAR ;
         break ;

      case SB_LINEDOWN:
         nScrollAmt = XCHAR ;
         break ;

      case SB_THUMBTRACK:
         nScrollAmt = wScrollPos - XOFFSET ;
         break ;

      case SB_THUMBPOSITION:
         nScrollAmt = wScrollPos - XOFFSET ;
         break ;

      default :
         LocalUnlock( hECB ) ;
         return ( FALSE ) ;
   }

   // Hide cursor if showing

   fCursorWasShowing = HideEmulatorCursor( hWnd ) ;

   if ((XOFFSET + nScrollAmt) > XSCROLL)
      nScrollAmt = XSCROLL - XOFFSET ;
   if ((XOFFSET + nScrollAmt) < 0)
      nScrollAmt = -XOFFSET ;

   // Force update of window

   UpdateWindow( hWnd ) ;

   // Scroll the device context

   GetClientRect( hWnd, &rcScroll ) ;
   rcClip = rcScroll ;
   ScrollDC( SCREENDC, -nScrollAmt, 0, &rcScroll, &rcClip, NULL, &rcUpdate ) ;
   InvalidateRect( hWnd, &rcUpdate, FALSE ) ;

   // Update scroll position

   XOFFSET = XOFFSET + nScrollAmt ;
   SetScrollPos( hWnd, SB_HORZ, XOFFSET, TRUE ) ;

   // Restore cursor if previously showing

   if (fCursorWasShowing)
      ShowEmulatorCursor( hWnd ) ;

   LocalUnlock( hECB ) ;
   return ( TRUE ) ;

} /* end of ScrollEmulatorHorz() */

/************************************************************************
 * End of File: scroll.c
 ************************************************************************/

