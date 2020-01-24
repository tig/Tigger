/************************************************************************
 *
 *    Copyright (c) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  NetBIOS DLL
 *
 *      Module:  libmain.c
 *
 *      Author:  Bryan A. Woodruff (baw)
 *
 *
 *     Remarks:  
 *
 *   Revisions:  
 *     01.00.001  2/ 7/91 baw   Initial version, initial build
 *
 *
 ************************************************************************/

#include "netbios.h"

/************************************************************************
 *  int FAR PASCAL LibMain( HANDLE hModule, WORD wDataSeg,
 *                          WORD cbHeapSize, LPSTR lpszCmdLine )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

int FAR PASCAL LibMain( HANDLE hModule, WORD wDataSeg, WORD cbHeapSize,
                        LPSTR lpszCmdLine )
{
   D(
      if (!(hWDB=wdbOpenSession( NULL,
                                 "NETBIOS",
                                 "WDB.INI",
                                 WDB_LIBVERSION )))
      {
         ODS( "wdb OpenSession Failed!\n\r" ) ;
      }
   ) ;

    // store instance handle globally
    hInstLib = hModule ;

    // return successful initialization
    return ( TRUE ) ;

} /* end of LibMain() */


/************************************************************************
 *  VOID FAR PASCAL WEP( short bSystemExit )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

VOID FAR PASCAL WEP( short bSystemExit )
{
   D( wdbCloseSession( hWDB ) ) ;

   return ;

} /* end of WEP() */

/************************************************************************
 * End of File: libmain.c
 ************************************************************************/

