/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  topstat.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *    This module is responsible for the top status window.  It uses the
 *    "status" window class exported from CMNDLL.
 *
 *   Revisions:  
 *     00.00.000  1/20/91 cek   First Version
 *
 *
 ************************************************************************/
#include "..\inc\undef.h"
#include <windows.h>
#include <cmndll.h>

#include "..\inc\lexis.h"
#include "..\inc\topstat.h"

/************************************************************************
 * Local Defines
 ************************************************************************/
#define FONTNAME  "Helv"
#define FONTWEIGHT 400

/************************************************************************
 * Macros 
 ************************************************************************/

/************************************************************************
 * Local data structures
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/
static void NEAR PASCAL BuildTopBoxes( HWND hwndStat ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/
static FARPROC lpfnTopStat ;        // new status window proc
static FARPROC lpfnOldTopStat ;     // old status window proc

/************************************************************************
 * Functions
 ************************************************************************/

/*************************************************************************
 *  HWND FAR PASCAL CreateTopStatus( HWND hwndParent ) ;
 *
 *  Description:
 *    This function creates, and displays the the top status window using
 *    the "status" window class exported from cmndll.dll.
 *
 *  Type/Parameter
 *          Description
 *    HWND  hwndParent
 *          The parent window.
 * 
 *  Return Value
 *    Returns the handle to the top status window, 0 if it fails to
 *    create the window.
 *
 *  Comments:
 *
 *  The top status bar has four "stat boxes" and no "message box".
 *  The four stat boxes, in order of appearance are:
 *
 *     IDB_CLIENT_LBL    - the client label (no frame)
 *     IDB_CLIENT        - the actual client box (frame)
 *     IDB_LIBRARY_LBL   - the library/file label (no frame)
 *     IDB_LIBRARY       - the actual library box (frame)
 *     IDB_TIME          - Current time (frame)
 *       (NOTE: #define TIMEDISPLAY for time display!)
 *
 *  These boxes act very much like buttons, thus the IDB identifiers.
 *
 *  The strings for the labels are pulled out of the resource file.  They
 *  should never change during a run, so we won't bother storing them
 *  elsewhere.
 *
 *
 *************************************************************************/
HWND FAR PASCAL CreateTopStatus( HWND hwndParent ) 
{
   HWND   hwndStat ;

   HFONT   hFont ;
   LOGFONT lf ;
   HDC     hDC ;
   TEXTMETRIC tm ;
   
   #ifdef TIMEDISPLAY
   LONG  lNumStats = 6 ;
   #else
   LONG  lNumStats = 4 ;
   #endif

   DP5( hWDB, "CreateTopStatus" ) ;
   if (!(hwndStat = CreateWindow(
                                 "status",
                                 NULL,
                                 WS_CHILD,
                                 0,
                                 0,
                                 1,
                                 1,
                                 hwndParent,
                                 IDB_TOPSTAT,
                                 GETHINST( hwndParent ),
                                 (LPSTR)&lNumStats )))
   {
      DP1( hWDB, "MsgWnd create failed!" ) ;
      return 0 ;
   }

   //
   // Subclass the status window so we can send it timer messages
   //
   lpfnTopStat = MakeProcInstance((FARPROC)fnTopStat, GETHINST( hwndParent )) ;

   if (lpfnTopStat)
   {
      lpfnOldTopStat = (FARPROC) GetWindowLong( hwndStat, GWL_WNDPROC ) ;

      SetWindowLong( hwndStat, GWL_WNDPROC, (LONG)lpfnTopStat ) ;
   }

   FontInitLogFont( &lf ) ;

   if (hDC = GetDC( hwndStat ))
   {
      SelectObject( hDC,  GetStockObject( SYSTEM_FIXED_FONT ) ) ;
      GetTextMetrics( hDC, &tm ) ;

      lstrcpy( lf.lfFaceName, FONTNAME ) ;

      lf.lfHeight = tm.tmHeight - tm.tmExternalLeading ;

      lf.lfWeight = FONTWEIGHT ;
      if (!FontSelectIndirect( hDC, &lf ))
      {
         DP1( hWDB,"FontSelectIndirect failed!!")   ;
      }
      ReleaseDC( hwndStat, hDC ) ;
   }

   if (hFont = CreateFontIndirect( &lf ))
   {
      SendMessage( hwndStat, WM_SETFONT, hFont, TRUE ) ;
   }

   BuildTopBoxes( hwndStat ) ;
   
   return hwndStat ;

}/* CreateTopStatus() */


/*************************************************************************
 *  BOOL FAR PASCAL DestroyTopStatus( HWND hwndStatus ) ;
 *
 *  Description:
 *    This function destroys the top status window.
 *
 *  Type/Parameter
 *          Description
 *    HWND  hwndStatus
 *          Handle to the status window obtained from CreateTopStatus...
 * 
 *  Return Value
 *    TRUE if it succeeded, FALSE otherwise.
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL DestroyTopStatus( HWND hwndStatus ) 
{
   if (hwndStatus)
   {
      BOOL b ;
      HFONT hFont ;

      if (hFont = (HFONT)SendMessage( hwndStatus, WM_GETFONT, 0, 0L )) 
         DeleteObject( hFont ) ;

      SetWindowLong( hwndStatus, GWL_WNDPROC, (LONG) lpfnOldTopStat ) ;

      if (lpfnTopStat)
         FreeProcInstance( lpfnTopStat ) ;

      b = DestroyWindow( hwndStatus ) ;

      return b ;
   }
   else
      return FALSE ;

}/* DestroyTopStatus() */


/************************************************************************
 * Local/Private functions
 ************************************************************************/
/*************************************************************************
 *  long FAR PASCAL fnTopStat( HWND hWnd, WORD iMessage,
 *                             WORD wParam, LONG lParam )
 *
 *  Description:
 *    New window procedure for the status control.  We are subclassing
 *    the status window, mainly so we can send it timer messages.
 *
 *  Comments:
 *
 *************************************************************************/
LONG FAR PASCAL
fnTopStat( HWND hwndStat, WORD iMessage, WORD wParam, LONG lParam )
{
   #ifdef TIMEDISPLAY
   static char szPrevTime[MAX_STRLEN] ;
   #endif

   switch (iMessage)
   {
      #ifdef TIMEDISPLAY
      case WM_TIMER:
      {
         char szBuf[MAX_STRLEN] ;

         TimeGetCurTime( szBuf, 0 ) ;
         if (lstrcmpi( szBuf, szPrevTime ))
         {
            StatusSetStatBox( hwndStat, IDB_TIME - IDB_TOPSTAT, szBuf ) ;
            lstrcpy( szPrevTime, szBuf ) ;
         }
      }
      break ;

      case WM_WININICHANGE:
      {
         char szBuf[MAX_STRLEN] ;

         TimeResetInternational() ;
         TimeGetCurTime( szBuf, TIME_NOSECONDS ) ;
         StatusSetStatBoxSize(  hwndStat, IDB_TIME - IDB_TOPSTAT,
                        StatusGetWidth( hwndStat, szBuf ) ) ;
         StatusSetStatBox( hwndStat, IDB_TIME - IDB_TOPSTAT, szBuf ) ;
      }
      break ;
      #endif
   }

   return CallWindowProc( lpfnOldTopStat, hwndStat, iMessage, wParam, lParam ) ;

}/* fnTopStat() */

/*************************************************************************
 *  void NEAR PASCAL BuildTopBoxes( hwndStat )
 *
 *  Description:
 *    This function figures out the top status boxes and set's them up.
 *
 *  Comments:
 *
 *************************************************************************/
static void NEAR PASCAL BuildTopBoxes( HWND hwndStat ) 
{
   char szBuf[MAX_STRLEN] ;

   DP5( hWDB, "BuildTopBoxes" ) ;

   if (LoadString( GETHINST( hwndStat ), IDS_STATBOX_CLIENT_LBL, szBuf,
                   MAX_STRLEN ))
   {
      StatusSetStatBoxSize(  hwndStat, IDB_CLIENT_LBL - IDB_TOPSTAT,
                             StatusGetWidth( hwndStat, szBuf ) ) ;

      StatusSetStatBox( hwndStat, IDB_CLIENT_LBL - IDB_TOPSTAT, szBuf ) ;
   }

   if (LoadString( GETHINST( hwndStat ), IDS_STATBOX_CLIENT, szBuf,
                   MAX_STRLEN ))
      StatusSetStatBoxSize(  hwndStat, IDB_CLIENT - IDB_TOPSTAT,
                     StatusGetWidth( hwndStat, szBuf ) ) ;
                  
   if (LoadString( GETHINST( hwndStat ), IDS_STATBOX_LIBRARY_LBL, szBuf,
                   MAX_STRLEN ))
   {
      StatusSetStatBoxSize(  hwndStat, IDB_LIBRARY_LBL - IDB_TOPSTAT,
                     StatusGetWidth( hwndStat, szBuf ) ) ;
      StatusSetStatBox( hwndStat, IDB_LIBRARY_LBL - IDB_TOPSTAT, szBuf ) ;
   }

   if (LoadString( GETHINST( hwndStat ), IDS_STATBOX_LIBRARY, szBuf,
                   MAX_STRLEN ))
      StatusSetStatBoxSize(  hwndStat, IDB_LIBRARY - IDB_TOPSTAT,
                     StatusGetWidth( hwndStat, szBuf ) ) ;

   #ifdef TIMEDISPLAY
   if (LoadString( GETHINST( hwndStat ), IDS_STATBOX_TIME_LBL, szBuf,
               MAX_STRLEN ))
      StatusSetStatBoxSize(  hwndStat, IDB_TIME_LBL - IDB_TOPSTAT,
                     StatusGetWidth( hwndStat, szBuf ) ) ;

   TimeGetCurTime( szBuf, 0 ) ;
   StatusSetStatBoxSize(  hwndStat, IDB_TIME - IDB_TOPSTAT,
                  StatusGetWidth( hwndStat, szBuf ) ) ;
   StatusSetStatBox( hwndStat, IDB_TIME - IDB_TOPSTAT, szBuf ) ;
   #endif

   //
   // Set justification and borders
   //
   SendMessage( hwndStat, ST_SETBORDER,  IDB_CLIENT_LBL - IDB_TOPSTAT,
                (LONG)FALSE ) ;
   SendMessage( hwndStat, ST_SETJUSTIFY, IDB_CLIENT_LBL - IDB_TOPSTAT,
                (LONG)DT_RIGHT) ;
   SendMessage( hwndStat, ST_SETJUSTIFY, IDB_CLIENT     - IDB_TOPSTAT,
                (LONG)DT_LEFT) ;

   SendMessage( hwndStat, ST_SETBORDER,  IDB_LIBRARY_LBL - IDB_TOPSTAT,
                (LONG)FALSE ) ;
   SendMessage( hwndStat, ST_SETJUSTIFY, IDB_LIBRARY_LBL - IDB_TOPSTAT,
                (LONG)DT_RIGHT ) ;
   SendMessage( hwndStat, ST_SETJUSTIFY, IDB_LIBRARY     - IDB_TOPSTAT,
                (LONG)DT_LEFT ) ;

   #ifdef TIMEDISPLAY
   SendMessage( hwndStat, ST_SETBORDER,  IDB_TIME_LBL - IDB_TOPSTAT,
                (LONG)FALSE ) ;
   #endif
}/* BuildTopBoxes() */

/************************************************************************
 * End of File: topstat.c
 ************************************************************************/

