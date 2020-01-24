//***************************************************************************
//
//  Module: libmain.c
//
//  Purpose:
//     This is the LibMain() and WEP() of the ZMODEM DLL.
//
//  Description of functions:
//     Descriptions are contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              3/10/92   BryanW      Wrote it.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//***************************************************************************

#include "precomp.h"

// global declarations in this module
#define GLOBALDEFS

#include "zmodem.h"

/************************************************************************
 *  int FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSegment,
 *                          WORD wHeapSize, LPSTR lpszCmdLine )
 *
 *  Description:
 *     This is the LibMain() for the ZMODEM library.
 *
 *  Comments:
 *      8/ 2/91  baw  Wrote this comment
 *
 ************************************************************************/

int FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSegment,
                        WORD wHeapSize, LPSTR lpszCmdLine )
{
   D(
      WORD  wOutput ;

      if (!(hWDB=wdbOpenSession( NULL,
                                 "ZMODEM",
                                 "WDB.INI",
                                 WDB_LIBVERSION )))
      {
         ODS( "wdb OpenSession Failed!\n\r" ) ;
      }
      else
      {
         wOutput = wdbGetOutput( hWDB, NULL ) ;
         wdbSetOutput( hWDB, WDB_OUTPUT_AUX, NULL ) ;
         DPF1( hWDB, "ZMODEM V%02d.%02d.%03d\r\n", VER_MAJOR, VER_MINOR,
               VER_BUILD ) ;
         wdbSetOutput( hWDB, wOutput, NULL ) ;
      }
   ) ;

   // use count starts at zero

   nUseCount = 0 ;

   // store lib instance

   hInstLib = hInstance ;

   return ( TRUE ) ;

} /* end of LibMain() */

/************************************************************************
 *  int FAR PASCAL WEP( int nSystemExit )
 *
 *  Description:
 *     The WEP()... although it is useless if we GP fault.
 *
 *  Comments:
 *      8/ 2/91  baw  Wrote this comment
 *
 ************************************************************************/

int FAR PASCAL WEP( int nSystemExit )
{
//   D( wdbCloseSession( hWDB ) ) ;

   return (0) ;

} /* end of WEP() */

//***************************************************************************
//  End of File: libmain.c
//***************************************************************************

