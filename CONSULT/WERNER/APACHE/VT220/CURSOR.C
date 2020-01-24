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
 *     Remarks:  VT220 Terminal Emulation (cursor routines)
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.002  2/28/91 baw   Moved to self-contained DLL
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "vt220.h"

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
   hECB = (HECB) GETHECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( (HLOCAL) hECB )))
      return ( FALSE ) ;

   bAttr = *(LPSTR) (ATTRBUF( npECB ) + CURROW( npECB ) * MAXCOLS( npECB ) +
                     CURCOL( npECB )) ;

   if (CURSORSTATE( npECB ) & ECS_SHOW)
   {
      if (bAttr & (ATTRIBUTE_DBLSIZE))
         // set caret size
         SetCaretPos( (CURCOL( npECB ) * XCHAR( npECB ) * 2) - XOFFSET( npECB ),
                      (CURROW( npECB ) * YCHAR( npECB )) - YOFFSET( npECB ) ) ;
      else
         // set caret size
         SetCaretPos( (CURCOL( npECB ) * XCHAR( npECB )) - XOFFSET( npECB ),
                      (CURROW( npECB ) * YCHAR( npECB )) - YOFFSET( npECB ) ) ;
   }
   CheckCursorPosition( hWnd, npECB, bAttr ) ;
   
   LocalUnlock( (HLOCAL) hECB ) ;
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
   hECB = (LOCALHANDLE) GETHECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( (HLOCAL) hECB )))
      return ( fRetVal ) ;

   if (CURSORSTATE( npECB ) & ECS_SHOW)
   {
      HideCaret( hWnd ) ;
      CURSORSTATE( npECB ) &= (BYTE) ~ECS_SHOW ;
      fRetVal = TRUE ;
   }
   LocalUnlock( (HLOCAL) hECB ) ;
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
   hECB = (HECB) GETHECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( (HLOCAL) hECB )))
      return ( fRetVal ) ;

   if (ECS_AVAIL == CURSORSTATE( npECB ) & (ECS_SHOW | ECS_AVAIL))
   {
      ShowCaret( hWnd ) ;
      CURSORSTATE( npECB ) |= (BYTE) ECS_SHOW ;

      // put it in the right spot
      MoveEmulatorCursor( hWnd ) ;
      fRetVal = TRUE ;
   }

   LocalUnlock( (HLOCAL) hECB ) ;
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

   if (bAttr & (BYTE) (ATTRIBUTE_DBLSIZE))
      GetDblCharRect( npECB, &rcCursor, CURCOL( npECB ), CURROW( npECB ) ) ;
   else 
      GetStdCharRect( npECB, &rcCursor, CURCOL( npECB ), CURROW( npECB ) ) ;

   if (rcCursor.top < 0) 
   {
      // scroll vertically  (top)
      UpdateWindow( hWnd ) ;
      SendMessage( hWnd, WM_VSCROLL, SB_THUMBPOSITION,
                   (LONG) (rcCursor.top + YOFFSET( npECB )) ) ;
   }
   else if (rcCursor.bottom > YSIZE( npECB ))
   {
      // scroll vertically  (bottom)
      UpdateWindow( hWnd ) ;
      SendMessage( hWnd, WM_VSCROLL, SB_THUMBPOSITION,
                   (LONG) (rcCursor.bottom - YSIZE( npECB ) +
                           YOFFSET( npECB )) ) ;

   }
   if (rcCursor.left < 0)
   {
      // scroll horizontally (left)
      UpdateWindow( hWnd ) ;
      SendMessage( hWnd, WM_HSCROLL, SB_THUMBPOSITION,
                   (LONG) (rcCursor.left + XOFFSET( npECB )) ) ;
   }
   else if (rcCursor.right > XSIZE( npECB ))
   {
      // scroll horizontally (right)
      UpdateWindow( hWnd ) ;
      SendMessage( hWnd, WM_HSCROLL, SB_THUMBPOSITION,
                   (LONG) (rcCursor.right - XSIZE( npECB ) +
                           XOFFSET( npECB )) ) ;
   }

} /* end of CheckCursorPosition() */

/************************************************************************
 * End of File: cursor.c
 ************************************************************************/
