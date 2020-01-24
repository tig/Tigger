
/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CMNDLL, Common DLL for Werner Apps
 *
 *      Module:  cmndll.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/

#define WINDLL
#include <windows.h>
#include <wdb.h>
#include "blnkdll.h"

//------------------------------------------------------------
// Exported variables
//------------------------------------------------------------
HANDLE   hInst ;

short    nCurTime ;
FARPROC  lpfnOldKbdHook ;
BOOL     fHooked = FALSE ;

int FAR PASCAL LibMain( 
                        HANDLE   hModule,
                        WORD     wDataSeg,
                        WORD     cbHeapSize,
                        LPSTR    lpszCmdLine
                      ) 
{
   nCurTime = 0 ;

   return TRUE ;
}

void FAR PASCAL blnkSetHook( void )
{
   if (lpfnOldKbdHook || fHooked)
   {
      D(ODS("Hook already set!\n\r" )) ;
      return ;
   }

   D(ODS( "Setting windows hook...\n\r" )) ;
   lpfnOldKbdHook = SetWindowsHook( WH_KEYBOARD, (FARPROC)fnKbdFilter ) ;

   fHooked = TRUE ;

   if (!lpfnOldKbdHook)
   {
      D(ODS( "   lpnfOldKbdHook == 0\n\r" )) ;
   }
}

void FAR PASCAL blnkUnhookHook( void )
{
   BOOL b = 0;

   D(ODS( "Unhooking hook\n\r" ));
   if (lpfnOldKbdHook && fHooked)
   {
      b = UnhookWindowsHook( WH_KEYBOARD, (FARPROC)fnKbdFilter ) ;
//      lpfnOldKbdHook = NULL ;
      fHooked = FALSE ;
   }
   else
   {
      D(ODS( "  attempted to unhook the hook but it wasn't hooked!\n\r" ));
   }

}

short FAR PASCAL blnkIncTime( short n )
{
   short cur ;

   cur = nCurTime ;

   nCurTime += n ;

   return cur ;
}

short FAR PASCAL blnkResetTime( void )
{
   nCurTime = 0 ;

   return TRUE ;
}

int FAR PASCAL fnKbdFilter( int nCode, WORD wParam, DWORD lParam )
{
   D(ODS( "fnKbdfilter\n\r" )) ;
   nCurTime = 0 ;

   if (lpfnOldKbdHook)
      return DefHookProc( nCode, wParam, lParam, (FARPROC FAR *)&lpfnOldKbdHook ) ;
   else
   {
      D(ODS( "  lpfnOldKbdHook was NULL!!!\n\r" )) ;
      return FALSE ;
   }
}

int FAR PASCAL WEP( int bSystemExit )
{
   D(ODS( "WEP\n\r" )) ;

   if (fHooked)
      blnkUnhookHook() ;

   fHooked = FALSE ;

   return 1 ;
}

/************************************************************************
 * End of File: cmndll.c
 ************************************************************************/


