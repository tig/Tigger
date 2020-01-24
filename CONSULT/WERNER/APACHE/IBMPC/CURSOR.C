/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  cursor.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  IBMPC Terminal Emulation (cursor routines)
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.002  2/28/91 baw   Moved to self-contained DLL
 *
 ************************************************************************/

#include "precomp.h"

#include "ibmpc.h"

/************************************************************************
 *  BOOL MoveEmulatorCursor( HWND hWnd )
 *
 *  Description: 
 *     Moves the caret to the position determined by XPOS, YPOS in
 *     the Emulator control block.
 *
 *  Comments:
 *      6/18/91  Wrote this comment block.
 *
 ************************************************************************/

BOOL MoveEmulatorCursor( HWND hWnd )
{
   BYTE   bAttr ;
   HECB   hECB ;
   NPECB  npECB ;

   // Get Emulator Control Block
   hECB = GETECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   bAttr = *(LPSTR) (ATTRBUF + CURROW * MAXCOLS + CURCOL) ;

   if (CURSORSTATE & ECS_SHOW)
   {
      // set caret size
      SetCaretPos( (CURCOL * XCHAR) - XOFFSET,
                   (CURROW * YCHAR) - YOFFSET ) ;
   }
   CheckCursorPosition( hWnd, npECB, bAttr ) ;
   
   LocalUnlock( hECB ) ;
   return ( TRUE ) ;

} /* end of MoveEmulatorCursor() */

/************************************************************************
 *  BOOL HideEmulatorCursor( HWND hWnd )
 *
 *  Description: 
 *     Hides the emulator cursor for the specific window
 *     Returns TRUE if cursor was previously showing and available,
 *     otherwise returns FALSE. 
 *
 *  Comments:
 *      5/27/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL HideEmulatorCursor( HWND hWnd )
{
   BOOL   fRetVal ;
   HECB   hECB ;
   NPECB  npECB ;

   // The pesimistic approach
   fRetVal = FALSE ;

   // Get Emulator Control Block
   hECB = GETECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( fRetVal ) ;

   if (CURSORSTATE & ECS_SHOW)
   {
      HideCaret( hWnd ) ;
      CURSORSTATE &= (BYTE) ~ECS_SHOW ;
      fRetVal = TRUE ;
   }
   LocalUnlock( hECB ) ;
   return ( fRetVal ) ;

} /* end of HideEmulatorCursor() */

/************************************************************************
 *  BOOL ShowEmulatorCursor( HWND hWnd )
 *
 *  Description: 
 *     Shows the emulator cursor for the specific window.
 *     Returns TRUE if cursor was previously hidden and available,
 *     otherwise returns FALSE. 
 *
 *  Comments:
 *      5/27/91  baw  Wrote it.
 *      6/20/91  baw  Fix to put cursor in the correct spot.
 *
 ************************************************************************/

BOOL ShowEmulatorCursor( HWND hWnd )
{
   BOOL   fRetVal ;
   HECB   hECB ;
   NPECB  npECB ;

   // The pesimistic approach
   fRetVal = FALSE ;

   // Get Emulator Control Block
   hECB = GETECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( fRetVal ) ;

   if (ECS_AVAIL == CURSORSTATE & (ECS_SHOW | ECS_AVAIL))
   {
      ShowCaret( hWnd ) ;
      CURSORSTATE |= (BYTE) ECS_SHOW ;

      // put it in the right spot
      MoveEmulatorCursor( hWnd ) ;
      fRetVal = TRUE ;
   }

   LocalUnlock( hECB ) ;
   return ( fRetVal ) ;

} /* end of ShowEmulatorCursor() */

/************************************************************************
 *  VOID CheckCursorPosition( HWND hWnd, NPECB npECB, BYTE bAttr )
 *
 *  Description: 
 *    Checks the current cursor position.  If it is not visible, this
 *    routine scrolls the window.
 *
 *  Comments:
 *     6/18/91  baw  Wrote it.
 *
 ************************************************************************/

VOID CheckCursorPosition( HWND hWnd, NPECB npECB, BYTE bAttr )
{
   RECT  rcCursor ;

   GetStdCharRect( npECB, &rcCursor, CURCOL, CURROW ) ;

   if (rcCursor.top < 0) 
   {
      // scroll vertically  (top)
      UpdateWindow( hWnd ) ;
      SendMessage( hWnd, WM_VSCROLL, SB_THUMBPOSITION,
                   (LONG) (rcCursor.top + YOFFSET) ) ;
   }
   else if (rcCursor.bottom > YSIZE)
   {
      // scroll vertically  (bottom)
      UpdateWindow( hWnd ) ;
      SendMessage( hWnd, WM_VSCROLL, SB_THUMBPOSITION,
                   (LONG) (rcCursor.bottom - YSIZE + YOFFSET) ) ;

   }
   if (rcCursor.left < 0)
   {
      // scroll horizontally (left)
      UpdateWindow( hWnd ) ;
      SendMessage( hWnd, WM_HSCROLL, SB_THUMBPOSITION,
                   (LONG) (rcCursor.left + XOFFSET) ) ;
   }
   else if (rcCursor.right > XSIZE)
   {
      // scroll horizontally (right)
      UpdateWindow( hWnd ) ;
      SendMessage( hWnd, WM_HSCROLL, SB_THUMBPOSITION,
                   (LONG) (rcCursor.right - XSIZE + XOFFSET) ) ;
   }

} /* end of CheckCursorPosition() */

/************************************************************************
 * End of File: cursor.c
 ************************************************************************/
