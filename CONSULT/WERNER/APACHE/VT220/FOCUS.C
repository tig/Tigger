//***************************************************************************
//
//  Module: focus.c
//
//  Purpose:
//     This handles the focus notifications for the emulator.
//
//  Description of functions:
//     Function decscriptions are contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              1/12/92   BryanW      Added this comment block.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//***************************************************************************

#include "precomp.h"    // precompiled header stuff

#include "vt220.h"

/************************************************************************
 *  BOOL SetEmulatorFocus( HWND hWnd )
 *
 *  Description:
 *     Sets the emulator focus.  This function saves the keyboard
 *     state and forces the NUMLOCK key off if we are using it as
 *     PF1.  Also, we hook the keyboard hook at this time.
 *
 *  Comments:
 *      6/ 8/91  baw  Wrote this comment block.
 *
 ************************************************************************/

BOOL SetEmulatorFocus( HWND hWnd )
{
   HECB   hECB ;
   NPECB  npECB ;
   BYTE   KeyboardStateCopy[ MAXLEN_KEYBOARDSTATE ] ;

   // Get Emulator Control Block
   hECB = GETHECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( (HLOCAL) hECB )))
      return ( FALSE ) ;

   GetKeyboardState( KEYBOARDSTATE( npECB ) ) ;

   if (USENUMLOCK( npECB ))
   {
      // copy keyboard state to a temporary
      // we are going to force the NUMLOCK key ON

      _fmemcpy( KeyboardStateCopy, KEYBOARDSTATE( npECB ),
                MAXLEN_KEYBOARDSTATE ) ;
      KeyboardStateCopy[ VK_NUMLOCK ] |= 0x01  ;
      SetKeyboardState( KeyboardStateCopy ) ;
   }

   // tell keyboard hook who has focus

   hFocusWnd = hWnd ;

   // Hook the keyboard.  This is performed at WM_SETFOCUS time.
   // This eliminates the unnecessary chain of keyboard hooks possible
   // from multiple emulator DLLs.

   if ((HHOOK) NULL == hOldHookProc)
      hOldHookProc = SetWindowsHook( WH_KEYBOARD,
                                     (HOOKPROC) EmulatorKeyboardHook ) ;

   if (0 == CURSORSTATE( npECB ) & ECS_AVAIL)
   {
      CreateCaret( hWnd, NULL, XCHAR( npECB ), YCHAR( npECB ) ) ;
      ShowCaret( hWnd ) ;
      CURSORSTATE( npECB ) = (BYTE) (ECS_AVAIL | ECS_SHOW) ;
   }

   LocalUnlock( (HLOCAL) hECB ) ;
   return ( TRUE ) ;

} /* end of SetEmulatorFocus() */

/************************************************************************
 *  BOOL KillEmulatorFocus( HWND hWnd )
 *
 *  Description:
 *     This function "kills" the emulator focus.  We restore the
 *     keyboard state as we found it and unhook the keyboard hook.
 *
 *  Comments:
 *      6/ 8/91  baw  Wrote this comment block.
 *
 ************************************************************************/

BOOL KillEmulatorFocus( HWND hWnd )
{
   HECB   hECB ;
   NPECB  npECB ;

   // Get Emulator Control Block

   hECB = GETHECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( (HLOCAL) hECB )))
      return ( FALSE ) ;

   // restore the state of the keyboard before we got to it

   SetKeyboardState( KEYBOARDSTATE( npECB ) ) ;

   // tell keyboard hook we don't know who has focus

   hFocusWnd = NULL ;

   // Unhook the keyboard.  This is done to eliminate the chain of hooks
   // possible from hooking the keyboard everytime an emulator DLL is 
   // started.  With this implementation, only one hook from our _DLLs_ 
   // (plural) is possible - as it should be (only one window has the focus
   // at any one time!  The only drawback...  Let's say that two windows
   // are using the same emulator, the focus is killed on one, the next
   // one receives the focus and we will unhook and rehook the same
   // keyboard hook function.  A small price to pay!

   if ((HHOOK) NULL != hOldHookProc)
   {
      UnhookWindowsHook( WH_KEYBOARD, (HOOKPROC) EmulatorKeyboardHook ) ;
      hOldHookProc = NULL ;
   }
   
   if (CURSORSTATE( npECB ) & (ECS_AVAIL | ECS_SHOW) == (ECS_AVAIL | ECS_SHOW))
   {
      HideCaret( hWnd ) ;
      DestroyCaret() ;
      CURSORSTATE( npECB ) = 0 ;
   }

   LocalUnlock( (HLOCAL) hECB ) ;
   return ( TRUE ) ;

} /* end of KillEmulatorFocus() */

//***************************************************************************
//  End of File: focus.c
//***************************************************************************

