/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  misc.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Main module
 *
 *   Revisions:  
 *     01.00.001  1/ 8/91 baw   Initial Version, first build
 *
 ************************************************************************/

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"

/************************************************************************
 *  WORD NEAR PASCAL StatMax( WORD w1, WORD w2)
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

WORD FAR PASCAL StatMax( WORD w1, WORD w2)
{
   return ((w1 > w2) ? w1 : w2) ;

} /* end of StatMax() */

/************************************************************************
 *  BOOL GetKeyStates( BOOL *pfCapsLock, BOOL *pfNumLock,
 *                     BOOL *pfScrollLock )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL FAR PASCAL GetKeyStates( BOOL *pfCapsLock, BOOL *pfNumLock,
                              BOOL *pfScrollLock )
{
   BOOL  fChanged = TRUE ;

   if (*pfCapsLock != (GetKeyState( VK_CAPITAL ) & 0x01 ))
   {
      *pfCapsLock = !*pfCapsLock ;
      fChanged = TRUE ;
   }
   if (*pfNumLock != (GetKeyState( VK_NUMLOCK ) & 0x01 ))
   {
      *pfNumLock = !*pfNumLock ;
      fChanged = TRUE ;
   }
   if (*pfScrollLock != (GetKeyState( 0x91 ) & 0x01 ))
   {
      *pfScrollLock = !*pfScrollLock ;
      fChanged = TRUE ;
   }
   return ( fChanged ) ;

} /* end of GetKeyStates() */

/************************************************************************
 *  VOID SetKeyStatus( HWND hwndMsg )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

VOID FAR PASCAL SetKeyStatus( HWND hwndMsg )
{
#ifdef NOT_USED
   BOOL  fCaps = fCapsLock,
         fNum = fNumLock,
         fScroll = fScrollLock ;

   if (GetKeyStates( &fCapsLock, &fNumLock, &fScrollLock ))
   {
      if (fCapsLock != fCaps)
         SendMessage( hwndMsg, ST_SETSTATBOX, INDEX_CAPSLOCK,
                      (LONG) (LPSTR) StatusToggles[INDEX_CAPSLOCK].szStat[fCapsLock] ) ;
      if (fNumLock != fNum)
         SendMessage( hwndMsg, ST_SETSTATBOX, INDEX_NUMLOCK,
                      (LONG) (LPSTR) StatusToggles[INDEX_NUMLOCK].szStat[fNumLock] ) ;
      if (fScrollLock != fScroll)
         SendMessage( hwndMsg, ST_SETSTATBOX, INDEX_SCROLLLOCK,
                      (LONG) (LPSTR) StatusToggles[INDEX_SCROLLLOCK].szStat[fScrollLock] ) ;
   }

#endif

} /* end of SetKeyStatus() */

/************************************************************************
 * End of File: misc.c
 ************************************************************************/
