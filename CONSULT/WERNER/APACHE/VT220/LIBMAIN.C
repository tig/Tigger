/************************************************************************
 *
 *    Copyright (c) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache for Windows
 *
 *      Module:  libmain.c
 *
 *      Author:  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  LibMain for DECVT220.EMU
 *
 *   Revisions:  
 *     01.00.000  6/20/91 baw   Wrote this comment
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "vt220.h"

/************************************************************************
 *  int FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSegment,
 *                          WORD wHeapSize, LPSTR lpszCmdLine )
 *
 *  Description:
 *     This is the LibMain() for the DECVT220 library.
 *
 *  Comments:
 *      6/20/91  baw  Wrote this comment
 *
 ************************************************************************/

int FAR PASCAL LibMain( HINSTANCE hInstance, WORD wDataSegment,
                        WORD wHeapSize, LPSTR lpszCmdLine )
{
   D(
      WORD  wOutput ;

      if (!(hWDB=wdbOpenSession( NULL,
                                 "DECVT220",
                                 "WDB.INI",
                                 WDB_LIBVERSION )))
      {
         ODS( "wdb OpenSession Failed!\n\r" ) ;
      }
      else
      {
         wOutput = wdbGetOutput( hWDB, NULL ) ;
         wdbSetOutput( hWDB, WDB_OUTPUT_AUX, NULL ) ;
         DPF1( hWDB, "DECVT220 V%02d.%02d.%03d\r\n", VER_MAJOR, VER_MINOR,
               VER_BUILD ) ;
         wdbSetOutput( hWDB, wOutput, NULL ) ;
      }
   ) ;

   // store lib instance
   hInstLib = hInstance ;

   // register classes associated with this library
   RegisterClasses( hInstLib ) ;

   // initialize use count
   nUseCount = 0 ;

   // set ECB focus to NULL
   hFocusWnd = NULL ;

   // set windows hook proc to NULL
   hOldHookProc = NULL ;

   return ( TRUE ) ;

} /* end of LibMain() */

/************************************************************************
 *  int FAR PASCAL WEP( int nSystemExit )
 *
 *  Description:
 *     The WEP()... although it is useless if we GP fault.
 *
 *  Comments:
 *      6/20/91  baw  Wrote this comment
 *
 ************************************************************************/

int FAR PASCAL WEP( int nSystemExit )
{
   UnregisterClasses( hInstLib ) ;

   if ((HHOOK) NULL != hOldHookProc)
   {
      UnhookWindowsHook( WH_KEYBOARD, (HOOKPROC) EmulatorKeyboardHook ) ;
      hOldHookProc = NULL ;
   }

   D( wdbCloseSession( hWDB ) ) ;

   return (0) ;

} /* end of WEP() */

/************************************************************************
 * End of File: libmain.c
 ************************************************************************/

