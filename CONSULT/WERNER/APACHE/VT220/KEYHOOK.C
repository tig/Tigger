-/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  keyhook.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  VT220 Terminal Emulation (keyhook routines)
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "vt220.h"

/************************************************************************
 *  DWORD FAR PASCAL EmulatorKeyboardHook( int nCode, WORD wParam,
 *                                         DWORD dwParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

DWORD FAR PASCAL EmulatorKeyboardHook( int nCode, WORD wParam, DWORD dwParam )
{
   BOOL    fUseNumLock ;
   BYTE    ControlSeq[MAXLEN_CONTROLSEQ] ;
   WORD    wRepeat, wCount, wKeypadType ;
   HECB    hECB ;
   NPECB   npECB ;

   static BOOL  fShift = FALSE,
                fMenu = FALSE,
                fControl = FALSE ;

   if ((hFocusWnd != NULL) && (nCode >= 0) && (nCode != HC_NOREMOVE))
   {
      DPF5( hWDB, "wParam = %04x, dwParam = %04x %04x\r\n", wParam,
            HIWORD( dwParam ), LOWORD( dwParam ) ) ;

      _fmemset( (LPSTR) ControlSeq, NULL, MAXLEN_CONTROLSEQ ) ;

      hECB = GETHECB( hFocusWnd ) ;
      if (NULL == (npECB = (NPECB) LocalLock( (HLOCAL) hECB )))
         return ( 1 ) ;

      fUseNumLock = USENUMLOCK( npECB ) ;
      wKeypadType = KEYPADTYPE( npECB ) ;

      LocalUnlock( hECB ) ;

      D(
         // If DEBUG force a NULL so a GP fault
         // will occur if macros are use
         npECB = NULL ;
      ) ;

      if (wKeypadType == KEYPAD_APPLICATION)
         ControlSeq[0] = ASCII_SS3 ;
      else 
         ControlSeq[0] = ASCII_CSI ;

      switch (wParam)
      {
         case VK_UP:
            ControlSeq[1] = 'A' ;
            break ;
      
         case VK_RIGHT:
            ControlSeq[1] = 'C' ;
            break ;
      
         case VK_DOWN:
            ControlSeq[1] = 'B' ;
            break ;
      
         case VK_LEFT:
            ControlSeq[1] = 'D' ;
            break ;
      
         case VK_NUMPAD0:
         case VK_NUMPAD1:
         case VK_NUMPAD2:
         case VK_NUMPAD3:
         case VK_NUMPAD4:
         case VK_NUMPAD5:
         case VK_NUMPAD6:
         case VK_NUMPAD7:
         case VK_NUMPAD8:
         case VK_NUMPAD9:
            if (wKeypadType == KEYPAD_APPLICATION)
               ControlSeq[1] = (BYTE) ('p' + LOBYTE( wParam - VK_NUMPAD0 )) ;
            else
               ControlSeq[0] = (BYTE) ('0' + LOBYTE( wParam - VK_NUMPAD0 )) ;
            break ;

         case VK_NUMLOCK:
         {
            BYTE  KeyboardState[ MAXLEN_KEYBOARDSTATE ] ;

            // for NumLock we need to check if we are mapping it
            // and if so, we need to force it off otherwise
            // let it go through

            if (!fUseNumLock)
               return ( DefHookProc( nCode, wParam, dwParam, &hOldHookProc ) ) ;

            // force NumLock key ON

            GetKeyboardState( KeyboardState ) ;
            KeyboardState[ VK_NUMLOCK ] |= 0x01 ;
            SetKeyboardState( KeyboardState ) ;

            ControlSeq[1] = 'P' ;
         }
         break ;

         case VK_DIVIDE:
            ControlSeq[1] = 'Q' ;
            break ;

         case VK_MULTIPLY:
            ControlSeq[1] = 'R' ;
            break ;

         case VK_SUBTRACT:
            ControlSeq[1] = 'S' ;
            break ;

         case VK_ADD:
            ControlSeq[1] = 'm' ;
            break ;

         case VK_DECIMAL:
            if (wKeypadType == KEYPAD_NUMERIC)
               return ( DefHookProc( nCode, wParam, dwParam, &hOldHookProc ) ) ;
            ControlSeq[1] = 'n' ;
            break ;

         case VK_RETURN:
            if (!(HIBYTE( HIWORD( dwParam ) ) & (BYTE) 0x01))
               return ( DefHookProc( nCode, wParam, dwParam, &hOldHookProc ) ) ;
            ControlSeq[1] = 'M' ;
            break ;
         
         case VK_BACK:
            if (fControl)
               ControlSeq[0] = ASCII_BS ;
            else
               ControlSeq[0] = ASCII_DEL ;
            break ;
         
         case VK_SHIFT:
            fShift = !(HIBYTE( HIWORD( dwParam ) ) & (BYTE) 0x80) ;
            return ( DefHookProc( nCode, wParam, dwParam, &hOldHookProc ) ) ;
   
         case VK_CONTROL:
            fControl = !(HIBYTE( HIWORD( dwParam ) ) & (BYTE) 0x80) ;
            return ( DefHookProc( nCode, wParam, dwParam, &hOldHookProc ) ) ;

         case VK_MENU:
            fMenu = !(HIBYTE( HIWORD( dwParam ) ) & (BYTE) 0x80) ;
            return ( DefHookProc( nCode, wParam, dwParam, &hOldHookProc ) ) ;
   
         default:
            return ( DefHookProc( nCode, wParam, dwParam, &hOldHookProc ) ) ;
      }
   
      // Check if key is being released, if so ignore the message.
      // Yeah, it seems strange to do this here, but we want to
      // trap both states of the keys we are trapping, otherwise
      // we pass the keys onto the next proc.
   
      if (HIBYTE( HIWORD( dwParam ) ) & (BYTE) 0x80)
         return ( 1 ) ;
   
      // perform repeat

      if (NULL == (npECB = (NPECB) LocalLock( hECB )))
         return ( 1 ) ;

      wRepeat = LOWORD( dwParam ) ;
      for (wCount = 0; wCount < wRepeat; wCount++)
      {
         // write keyboard translation code here

         if (0L == SendMessage( GetParent( hFocusWnd ), WM_CMD_WRITEBLK,
                                (WORD) lstrlen( (LPSTR) ControlSeq ),
                                (LONG) (LPSTR) ControlSeq ))
         {
            // failed for some reason
            LocalUnlock( hECB ) ;
            return ( 1 ) ;
         }

         // echo back to ourself (if half-duplex)

         if (LOCALECHO( npECB ))
            WriteEmulatorBlock( hFocusWnd, ControlSeq,
                                lstrlen( (LPSTR) ControlSeq ) ) ;
      }
      LocalUnlock( hECB ) ;
      return ( 1 ) ;
   }
   return ( DefHookProc( nCode, wParam, dwParam, &hOldHookProc ) ) ;

} /* end of ProcessEmulatorKeyDown() */

/************************************************************************
 * End of File: keyhook.c
 ************************************************************************/


