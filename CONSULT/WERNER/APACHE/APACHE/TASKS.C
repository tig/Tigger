/************************************************************************
 *
 *    Copyright (C) 1991 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  tasks.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Predefined task functions
 *
 *   Revisions: 
 *     01.00.000  9/ 2/91 baw   Wrote it.
 *
 ************************************************************************/

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"
#include "tasks.h"

/************************************************************************
 *  LONG FAR PASCAL SessionTaskProc( HWND hWnd, HANDLE hTaskDB,
 *                                   WORD wFunction, WORD wParam,
 *                                   LONG lParam )
 *
 *  Description:
 *     Session task handler.  This handles all functionality of the
 *     session manager.  The owner of this task MUST be the session
 *     window (TerminalWndClass).
 *
 *  Comments:
 *      9/ 2/91  baw  Wrote it.  Stolen from the old style window
 *                    based session manager.
 *
 ************************************************************************/

LONG FAR PASCAL SessionTaskProc( HWND hWnd, HANDLE hTaskDB, WORD wFunction,
                                 WORD wParam, LONG lParam )
{
   LONG lRetVal ;

   switch (wFunction)
   {
      case TF_INITTASK:
         lRetVal = TF_IDLESTATE ;
         break ;

      case TF_CREATECONNECTOR:
         lRetVal = (LONG) CreateConnector( hWnd, (LPSTR) lParam ) ;
         break ;

      case TF_OPENCONNECTOR:
         
      case TF_EXITTASK:
         lRetVal = 0L ;
         break ; 
   }
   DPF3( hWDB, "Session task executed function: %d, return value: %ld\r\n",
         wFunction, lRetVal ) ;
   return ( lRetVal ) ;

} /* end of SessionTaskProc() */

/************************************************************************
 * End of File: tasks.c
 ************************************************************************/

