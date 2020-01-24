//***************************************************************************
//
//  Module: libmain.c
//
//  Purpose:
//     LibMain() and WEP() for the TASK MANAGEMENT DLL.
//
//  Description of functions:
//     Contained in function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              8/24/92   BryanW      Wrote it.
//              1/11/92   BryanW      Added this comment block.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//***************************************************************************

#include "precomp.h"  // precompiled stuff

// global declarations in this module
#define GLOBALDEFS

#include "taskpriv.h"

/************************************************************************
 *  int FAR PASCAL LibMain( HINSTANCE hInstance, WORD wDataSegment,
 *                          WORD wHeapSize, LPSTR lpszCmdLine )
 *
 *  Description:
 *     LibMain() call from LibEntry().
 *
 *  Comments:
 *      8/24/91  baw  Wrote it.
 *
 ************************************************************************/

int FAR PASCAL LibMain( HINSTANCE hInstance, WORD wDataSegment,
                        WORD wHeapSize, LPSTR lpszCmdLine )
{
   int  i ;

   D(
      WORD  wOutput ;

      if (!(hWDB=wdbOpenSession( NULL,
                                 "TASKMGR",
                                 "WDB.INI",
                                 WDB_LIBVERSION )))
      {
         ODS( "wdb OpenSession Failed!\n\r" ) ;
      }
      else
      {
         wOutput = wdbGetOutput( hWDB, NULL ) ;
         wdbSetOutput( hWDB, WDB_OUTPUT_AUX, NULL ) ;
         DPF1( hWDB, "TASKMGR V%02d.%02d.%03d\r\n", VER_MAJOR, VER_MINOR,
               VER_BUILD ) ;
         wdbSetOutput( hWDB, wOutput, NULL ) ;
      }
   ) ;

   // store instance handle

   hInstLib = hInstance ;

   // initialize timer id 

   nIdTimer = 0 ;

   // clear all entries in the event array

   for (i = 0; i < MAXLEN_TASKEVENTS; i++)
   {
      TaskEvents[i].hNotifyWnd = NULL ;
      TaskEvents[i].fPostFlag = FALSE ;
   }

   return ( TRUE ) ;

} /* end of LibMain() */

/************************************************************************
 *  VOID FAR PASCAL WEP( int nSystemExit )
 *
 *  Description:
 *     The Windows Exit Procedure.
 *
 *  Comments:
 *      8/24/91  baw  Wrote it.
 *
 ************************************************************************/

VOID FAR PASCAL WEP( int nSystemExit )
{

//   D( wdbCloseSession( hWDB ) ) ;

   if (nIdTimer)
      KillSystemTimer( nIdTimer ) ;

   return ;

} /* end of WEP() */

//***************************************************************************
//  End of File: libmain.c
//***************************************************************************

