//***************************************************************************
//
//  Module: libmain.c
//
//  Purpose:
//     This is the LibMain() and WEP() for the STDCOMM library.
//
//  Description of functions:
//     Descriptions are contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              1/12/92   BryanW      Updated to STRICT and these new
//                                    comment blocks.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//***************************************************************************

#include "precomp.h"     // precompiled stuff

// global declarations in this module
#define GLOBALDEFS

#include "stdcomm.h"

//************************************************************************
//  int FAR PASCAL LibMain( HINSTANCE hInstance, WORD wDataSegment,
//                          WORD wHeapSize, LPSTR lpszCmdLine )
//
//  Description:
//     This is the LibMain() for the STDCOMM library.
//
//  Parameters:
//     HINSTANCE hInstance
//        handle to library instance (DS segment)
//
//     WORD wDataSegment
//        DS selector
//
//     WORD wHeapSize
//        heap size
//
//     LPSTR lpszCmdLine
//        pointer to command line
//
//  History:   Date       Author      Comment
//              1/12/92   BryanW      Updated this comment block.
//
//************************************************************************

int FAR PASCAL LibMain( HINSTANCE hInstance, WORD wDataSegment,
                        WORD wHeapSize, LPSTR lpszCmdLine )
{
   int  i ;

   D(
      WORD  wOutput ;

      if (!(hWDB=wdbOpenSession( NULL,
                                 "STDCOMM",
                                 "WDB.INI",
                                 WDB_LIBVERSION )))
      {
         ODS( "wdb OpenSession Failed!\n\r" ) ;
      }
      else
      {
         wOutput = wdbGetOutput( hWDB, NULL ) ;
         wdbSetOutput( hWDB, WDB_OUTPUT_AUX, NULL ) ;
         DPF1( hWDB, "STDCOMM V%02d.%02d.%03d\r\n", VER_MAJOR, VER_MINOR,
               VER_BUILD ) ;
         wdbSetOutput( hWDB, wOutput, NULL ) ;
      }
   ) ;

   // store instance handle

   hLibInst = hInstance ;

   // initialize event array

   for (i = 0; i < MAXEVENTS; i++)
   {
      Events[i].hWnd =       NULL ;
      Events[i].nCid =       0 ;
      Events[i].fPostFlag =  FALSE ;
      Events[i].wEvent =     0 ;
      Events[i].wEventMask = 0 ;
      Events[i].lpEventPtr = NULL ;
   }

   // initialize timer id 

   nIdTimer = 0 ;

   // initialize use count

   nUseCount = 0 ;

   return ( TRUE ) ;

} // end of LibMain()

/************************************************************************
 *  VOID FAR PASCAL WEP( int nSystemExit )
 *
 *  Description: 
 *     This is the WEP() for the STDCOMM.CON.
 *
 *  Comments:
 *      9/21/91  baw  Wrote this comment.
 *
 ************************************************************************/

VOID FAR PASCAL WEP( int nSystemExit )
{

   D( wdbCloseSession( hWDB ) ) ;

   if (nIdTimer)
      KillSystemTimer( nIdTimer ) ;

   return ;

} /* end of WEP() */

//***************************************************************************
//  End of File: libmain.c
//***************************************************************************

