//---------------------------------------------------------------------------
//
//  Module: keyhook.c
//
//  Purpose:
//     This processes the key strokes and translates when
//     appropriate.
//
//  Description of functions:
//     Function descriptions are contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              2/22/92   BryanW      Added this comment block.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"    // precompiled header stuff

#include "ibmpc.h"

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
   BYTE   abControlSeq[ MAXLEN_CONTROLSEQ ] ;
   WORD   wRepeat, wCount ;
   HECB   hECB ;
   NPECB  npECB ;

   static BOOL  fShift = FALSE,
                fMenu = FALSE,
                fControl = FALSE ;

   if ((hFocusWnd != NULL) && (nCode >= 0) && (nCode != HC_NOREMOVE))
   {
      if (NULL == (hECB = GETECB( hFocusWnd )))
         return ( 1 ) ;

      _fmemset( (LPSTR) abControlSeq, NULL, MAXLEN_CONTROLSEQ ) ;

      abControlSeq[0] = ASCII_ESC ;
      abControlSeq[1] = '[' ;

      switch (wParam)
      {
         case VK_UP:
            abControlSeq[2] = 'A' ;
            break ;
      
         case VK_RIGHT:
            abControlSeq[2] = 'C' ;
            break ;
      
         case VK_DOWN:
            abControlSeq[2] = 'B' ;
            break ;
      
         case VK_LEFT:
            abControlSeq[2] = 'D' ;
            break ;

         case VK_HOME:
            abControlSeq[2] = 'H' ;
            break ;

         case VK_END:
            abControlSeq[2] = 'K' ;
            break ;

         case VK_RETURN:
            if (fControl)
            {
               abControlSeq[0] = ASCII_LF ;
               abControlSeq[1] = NULL ;
            }
            else
               return ( DefHookProc( nCode, wParam, dwParam, &hOldHookProc ) ) ;
            break ;

         case VK_BACK:
            // check for reverse
            if (fControl)
            {
               abControlSeq[0] = ASCII_DEL ;
               abControlSeq[1] = NULL ;
            }
            else
               return ( DefHookProc( nCode, wParam, dwParam, &hOldHookProc ) ) ;
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
                                (WORD) lstrlen( (LPSTR) abControlSeq ),
                                (LONG) (LPSTR) abControlSeq ))
         {
            // failed for some reason
            LocalUnlock( hECB ) ;
            return ( 1 ) ;
         }

         // echo back to ourself (if half-duplex)

         if (LOCALECHO)
            WriteEmulatorBlock( hFocusWnd, abControlSeq,
                                lstrlen( (LPSTR) abControlSeq ) ) ;
      }
      LocalUnlock( hECB ) ;
      return ( 1 ) ;
   }
   return ( DefHookProc( nCode, wParam, dwParam, &hOldHookProc ) ) ;

} /* end of ProcessEmulatorKeyDown() */

/************************************************************************
 * End of File: keyhook.c
 ************************************************************************/

//---------------------------------------------------------------------------
//  End of File: keyhook.c
//---------------------------------------------------------------------------

