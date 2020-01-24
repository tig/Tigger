/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  window.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Window controls
 *
 *   Revisions:
 *     01.00.000  4/ 3/91 baw   Wrote it
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "vt220.h"

/************************************************************************
 *  BOOL RegisterClasses( HINSTANCE hInstance )
 *
 *  Description: 
 *
 *     This function registers the window class(es) (emulator)
 *     specific to this library.
 *
 *  Comments:
 *
 *     4/ 7/91 - BAW - Wrote It (Only 1 Class)
 *
 ************************************************************************/

BOOL RegisterClasses( HINSTANCE hInstance )
{
   WNDCLASS  wndclass ;

   wndclass.style =         CS_OWNDC ;
   wndclass.lpfnWndProc =   EmulatorWndProc ;
   wndclass.cbClsExtra =    0 ;
   wndclass.cbWndExtra =    GWW_EXTRABYTES ;
   wndclass.hInstance =     hInstance ;
   wndclass.hIcon =         LoadIcon( NULL, IDI_APPLICATION ) ;
   wndclass.hCursor =       LoadCursor( NULL, IDC_ARROW ) ;
   wndclass.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1) ;
   wndclass.lpszMenuName =  NULL ;
   wndclass.lpszClassName = szEmulatorClass ;

   RegisterClass( &wndclass ) ;

   return ( TRUE ) ;

} /* end of RegisterClasses() */

/************************************************************************
 *  BOOL UnregisterClasses( HINSTANCE hInstance )
 *
 *  Description:
 *
 *     This function unregisters the window class(es) (emulator)
 *     specific to this library.
 *
 *  Comments:
 *
 *     4/ 7/91 - BAW - Wrote It (Only 1 Class)
 *        
 ************************************************************************/

BOOL UnregisterClasses( HINSTANCE hInstance )
{
   UnregisterClass( szEmulatorClass, hInstance ) ;
   
   return ( TRUE ) ;

} /* end of UnregisterClasses() */

/************************************************************************
 *  LONG FAR PASCAL EmulatorWndProc( HWND hWnd, UINT uMsg,
 *                                   WPARAM wParam, LPARAM lParam )
 *
 *  Description:
 *     
 *     This is the Emulator Window Procedure.  It handles all messages
 *     for the emulator.
 *
 *  Comments:
 *
 *     04/17/91 - BAW - "Wrote this nifty comment block."
 *                      Quote borrowed from David Weise.
 *
 ************************************************************************/

LONG FAR PASCAL EmulatorWndProc( HWND hWnd, UINT uMsg,
                                 WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_CREATE:
      {
         HECB  hECB ;

         if (NULL == (hECB = CreateEmulator( hWnd )))
            return (-1) ;   // fail create
         SETHECB( hWnd, hECB ) ;
      }
      break ;

      case WM_TIMER:
         BlinkEmulator( hWnd ) ;
         break ;

      case WM_ERASEBKGND:
      {
         HBRUSH  hbrBG ;
         HECB    hECB ;
         LONG    lRetVal ;
         NPECB   npECB ;

         // set up background color for areas
         // not painted by paint routine

         hECB = GETHECB( hWnd ) ;
         if (NULL == (npECB = (NPECB) LocalLock( hECB )))
            return ( 0 ) ;

         hbrBG = CreateSolidBrush( NORMALFONT( npECB ).lfBGcolor) ;
         hbrBG =
            (HBRUSH) SetClassWord( hWnd, GCW_HBRBACKGROUND, (WPARAM) hbrBG ) ;
         lRetVal = DefWindowProc( hWnd, uMsg, wParam, lParam ) ;
         hbrBG =
            (HBRUSH) SetClassWord( hWnd, GCW_HBRBACKGROUND, (WPARAM) hbrBG ) ;
         DeleteObject( hbrBG ) ;
         LocalUnlock( hECB ) ;

         return ( lRetVal ) ;
      }

      case WM_DESTROY:
         DestroyEmulator( hWnd ) ;
         break ;

      case WM_CHAR:
      {
         char   chKey ;
         HECB   hECB ;
         NPECB  npECB ;

         // NOTES:
         //    This should probably call the emulator to find any
         //    key mapping changes... although the WM_KEYDOWN hook
         //    will already trap mappings.  This message will most
         //    likely result in a SendMessage() call to the parent
         //    to allow the Session Manager to send a string to
         //    the connector.

         chKey = LOBYTE( wParam ) ;
         if (0L == SendMessage( GetParent( hWnd ), WM_CMD_WRITEBLK,
                                1, (LONG) (LPSTR) &chKey ))
            return ( 0 ) ;

         hECB = GETHECB( hWnd ) ;
         if (NULL == (npECB = (NPECB) LocalLock( hECB )))
            return ( 0 ) ;

         if (LOCALECHO( npECB ))
            WriteEmulatorBlock( hWnd, (LPSTR) &chKey, 1 ) ;

         LocalUnlock( hECB ) ;
      }
      break ;

      case WM_CMD_WRITEBLK:
         WriteEmulatorBlock( hWnd, (LPSTR) lParam, (int) wParam ) ;
         if ( GetActiveWindow() == hWnd )
            UpdateWindow( hWnd ) ;
         break ;

      case WM_SIZE:
         SizeEmulator( hWnd, HIWORD( lParam ), LOWORD( lParam ) ) ;
         break ;

      case WM_VSCROLL:
         ScrollEmulatorVert( hWnd, wParam, LOWORD( lParam ) ) ;
         break ;

      case WM_HSCROLL:
         ScrollEmulatorHorz( hWnd, wParam, LOWORD( lParam ) ) ;
         break ;

      case WM_PAINT:
         // This needs to be changed.  Cursor position is
         // updated in the paint, the cursor is positioned
         // after the paint.
         // ?'s:  Should the paint be forced ( e.g.
         //       UpdateWindow() ) on character input
         //       and then move cursor position or
         //       should the move cursor occur when
         //       the character is written to the
         //       buffer?
         if (!IsIconic( hWnd ))
            return ( PaintEmulator( hWnd ) ) ;
         else
            return ( DefWindowProc( hWnd, uMsg, wParam, lParam ) ) ;

      case WM_SETFOCUS:
         // Send a message back to parent to query (escape)
         // the connector for a connect flag
         // SendMessage( GetParent( hWnd ), WM_QUERYCONNECTOR, ..., ... )

         SetEmulatorFocus( hWnd ) ;
         MoveEmulatorCursor( hWnd ) ;
         break ;

      case WM_KILLFOCUS:
         // Send a message back to parent to query (escape)
         // the connector for a connect flag
         // SendMessage( GetParent( hWnd ), WM_QUERYCONNECTOR, ..., ... )

         KillEmulatorFocus( hWnd ) ;
         break ;

      default:
         return ( DefWindowProc( hWnd, uMsg, wParam, lParam ) ) ;
   }
   return ( 0 ) ;

} /* end of EmulatorWndProc() */

/************************************************************************
 * End of File: window.c
 ************************************************************************/

