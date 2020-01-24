/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  apache.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Main module
 *
 *    This module contains the main (frame) window procedure and the
 *    message loop.  It also contains WinMain (since that's were
 *    the message loop is).
 *
 *   Revisions:  
 *     01.00.001  1/ 7/91 baw   Initial Version, first build
 *     01.00.002  1/17/91 baw   Now uses EVENT.DLL (STEM)
 *     01.00.003  1/18/91 baw   Various bug fixes. Using new CMNDLL.DLL.
 *     01.00.004  1/19/91 baw   Enlarged COMM buffers to 4K.  Fixed cursor
 *                              problems.
 *     01.00.005  2/20/91 baw   Implemented explict loading of DLLs.
 *                              STEM is now in the STDCOMM DLL.  STEM is not
 *                              necessary for NETBIOS as NETBIOS uses its
 *                              own interrupt mechanism.
 *     01.00.008  3/24/91 cek   Changed dir structure and removed unneeded
 *                              code.
 *     01.00.009  3/24/91 cek   Set up resource management stuff (strings).
 *                              See RES_DEF.H and STRING.RC.
 *                              Ripped out guts and re-organzied code.
 *     01.00.010  5/ 2/91 baw   Restructured window hierarchy.  The emulator
 *                              is a now a control.
 *     01.00.015  8/24/91 baw   Added task manager functionality.
 *
 ************************************************************************/

#include "precomp.h"   // precompiled headers (windows.h, etc.)

// setup for global declarations (main module only)
#define GLOBALDEFS

#include "apache.h"
#include "dialog.h"
#include "init.h"
#include "misc.h"
#include "mdi.h"
#include "session.h"
#include "size.h"

BOOL FAR PASCAL
GetKeyStates( BOOL *bCapsLock, BOOL *bNumLock, BOOL *bScrollLock ) ;

HWND  hwndBottom ;

#define INDEX_CAPSLOCK     0     // each stat box within the status
#define INDEX_NUMLOCK      1     // window has an index value
#define INDEX_SCROLLLOCK   2     // (from left to right).
#define INDEX_DATE         3
#define INDEX_TIME         4

#define NUM_BOTBOXES 5

/*
* rgszBot (bottom stat box items)
*/
struct
{
char *szStat[2] ;
COLORREF rgb ;
}
rgszBot [] =
{
"", "CAPS", RGBBLUE,
"", "NUM", RGBRED,
"", "SCRL", RGBBLACK,
} ;

/*
* the following are sample strings that we will SetWindowText()
* to the 'message' part of the bottom status window
*/

BOOL  bCapsLock ;
BOOL  bNumLock ;
BOOL  bScrollLock ;

LRESULT FAR PASCAL TaskProc( HWND, HTASKDB, UINT, WPARAM, LPARAM ) ;

/************************************************************************
 *  int PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInstance,
 *                      LPSTR lpszCmdLine, int nCmdShow )
 *
 *  Description:
 *     Main window procedure - called by Windows
 *
 *  Comments:
 *      9/15/91  baw  Added this comment.
 *
 ************************************************************************/

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow )
{
   int   i ;
   MSG   msg, msgNotify ;
   BOOL  fDone, fDispatchNotify, fTranslated ;

   D(
      if (!(hWDB=wdbOpenSession( NULL,
                                 "Apache",
                                 "WDB.INI",
                                 WDB_LIBVERSION )))
      {
         ODS( "wdb OpenSession Failed!\n\r" ) ;
      }
   ) ;

   if (!hPrevInstance)
      if (!InitApplication( hInstance ))
         return ( FALSE ) ;

   if (!InitInstance( hInstance, nCmdShow ))
   {
      DP1( hWDB, "Malfunction! Couldn't initialize instance" ) ;
      return ( FALSE ) ;
   }

   // create task list

   hTaskList = CreateTaskList() ;

   fDone = FALSE ;
   fDispatchNotify = FALSE ;

   while ( TRUE )
   {
      while (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
      {
         if (WM_QUIT == msg.message)
            goto OutOfHere ;
         else if (WM_CONNECTOR_NOTIFY == msg.message)
         {
            // make NOTIFY messages a lower priority

            msgNotify = msg ;
            fDispatchNotify = TRUE ;
         }
         else
         {
            if (WM_MOUSEMOVE == msg.message || WM_SETFOCUS == msg.message ||
                WM_KEYDOWN == msg.message)
            {
               /*
                * use the GetKeyStates() function (see below) to get our
                * current key states.  Use the SetStatus() macro to
                * set the stat boxes to the approriate state.
                */

               GetKeyStates( &bCapsLock, &bNumLock, &bScrollLock ) ;
               StatusSetStatBox( hwndBottom, INDEX_CAPSLOCK,
                                 rgszBot[INDEX_CAPSLOCK].szStat[bCapsLock] ) ;
               StatusSetStatBox( hwndBottom, INDEX_NUMLOCK,
                                 rgszBot[INDEX_NUMLOCK].szStat[bNumLock] ) ;
               StatusSetStatBox( hwndBottom, INDEX_SCROLLLOCK,
                                 rgszBot[INDEX_SCROLLLOCK].szStat[bScrollLock] ) ;
            }
            fTranslated = FALSE ;
            for (i = 0; i < MAXLEN_MODELESSDLGS; i++)
               if (ahModelessDlgs[ i ] != NULL)
               {
                  if (IsWindow( ahModelessDlgs[ i ] ))
                     fTranslated = IsDialogMessage( ahModelessDlgs[ i ],
                                                    &msg ) ;
                  else
                     ahModelessDlgs[ i ] = NULL ;
                  break ;
               }

            if (!fTranslated && 
                !TranslateMDISysAccel( hClientWnd, &msg ) &&
                !TranslateAccelerator( hFrameWnd, hAccel, &msg ))
            {
               TranslateMessage( &msg ) ;
               DispatchMessage( &msg ) ;
            }
         }
      }
      if (fDispatchNotify)
      {
         DispatchMessage( &msgNotify ) ;
         fDispatchNotify = FALSE ;
      }
      TaskManagerSlice( hFrameWnd, hTaskList ) ;
   }

OutOfHere:

#ifdef OLD_MSG_PUMP
   while (GetMessage( &msg, NULL, 0, 0 ))
   {
      if (!TranslateMDISysAccel( hClientWnd, &msg ) &&
          !TranslateAccelerator( hFrameWnd, hAccel, &msg ))
      {
         TranslateMessage( &msg ) ;
         DispatchMessage( &msg ) ;
      }
   }
#endif

   // clean up task list

   DestroyTaskList( hTaskList ) ;

   D( wdbCloseSession( hWDB ) ) ;

   return ( msg.wParam ) ;

} /* end of WinMain() */

void BuildBottomBoxes( HWND hwnd )
{
   int   i ;
   char  szBuf[ MAXLEN_STRLEN ] ;

   for (i = 0 ; i < NUM_BOTBOXES - 2 ; i++)
   {
      /*
         * Get the width of the widest peice of text you might
         * put into a stat box and use that to set the stat
         * box width.  This way you are sure that the text
         * will fit.
         */
      StatusSetStatBoxSize( hwnd, i,
                            max( StatusGetWidth( hwnd,
                                                 rgszBot[i].szStat[0] ),
                                 StatusGetWidth( hwnd,
                                                 rgszBot[i].szStat[1] ) ) ) ;

      StatusSetStatBox( hwnd, i, rgszBot[i].szStat[0] ) ;

      /*
         * set the color of this guy
         */
      SendMessage( hwnd, ST_SETCOLOR, i, rgszBot[i].rgb ) ;

   }

   TimeGetCurDate( szBuf, 0 ) ;
   StatusSetStatBoxSize( hwnd, INDEX_DATE, StatusGetWidth( hwnd, szBuf ) ) ;
   StatusSetStatBox( hwnd, INDEX_DATE, szBuf ) ;

   TimeGetCurTime( szBuf, 0 ) ;
   StatusSetStatBoxSize( hwnd, INDEX_TIME, StatusGetWidth( hwnd, szBuf ) ) ;
   StatusSetStatBox( hwnd, INDEX_TIME, szBuf ) ;
}

//************************************************************************
//  HWND NEAR CreateStatus( HWND hWnd )
//
//  Description:
//
//
//  Parameters:
//
//  History:   Date       Author      Comment
//
//************************************************************************

HWND NEAR CreateStatus( HWND hWnd )
{
   HDC           hDC ;
   LONG          lNumStats ;

   hDC = GetDC( hWnd ) ;
   hStatFont = ReallyCreateFont( hDC, "Helv", 10, RCF_NORMAL ) ;
   ReleaseDC ( hWnd, hDC ) ;

   lNumStats = NUM_BOTBOXES ;
   if (!(hwndBottom = CreateWindow(
         "FL_STATUS",
         "Ready",
         WS_CHILD,
         0,
         0,
         1,
         1,
         hWnd,
         (HMENU) IDD_BOTTOM,
         GETHINST( hWnd ),
         (LPSTR)&lNumStats
      )))
   {
      /*DP( hWDB,  "MsgWnd create failed!" ) ;*/
      return NULL ;
   }

   SendMessage( hwndBottom, WM_SETFONT, (WPARAM) hStatFont, 0L ) ;

   BuildBottomBoxes( hwndBottom ) ;

   ShowWindow( hwndBottom, SW_NORMAL ) ;

   UpdateWindow( hwndBottom ) ;

   /*
      * use the GetKeyStates() function (see below) to get our
      * current key states.  Use the SetStatus() macro to
      * set the stat boxes to the approriate state.
      */
   GetKeyStates( &bCapsLock, &bNumLock, &bScrollLock ) ;
   StatusSetStatBox( hwndBottom, INDEX_CAPSLOCK,
                     rgszBot[INDEX_CAPSLOCK].szStat[bCapsLock] ) ;
   StatusSetStatBox( hwndBottom, INDEX_NUMLOCK,
                     rgszBot[INDEX_NUMLOCK].szStat[bNumLock] ) ;
   StatusSetStatBox( hwndBottom, INDEX_SCROLLLOCK,
                     rgszBot[INDEX_SCROLLLOCK].szStat[bScrollLock] ) ;
    
   return ( hwndBottom ) ;

} // end of CreateStatus()

//************************************************************************
//  LRESULT FAR PASCAL SplashWndProc( HWND hWnd, WORD wMsg,
//                                    WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This is the window procedure for the "splash" window.
//
//  Parameters:
//     Same as standard window procedures.
//
//  History:   Date       Author      Comment
//              1/19/92   BryanW      Wrote it.
//
//************************************************************************

LRESULT FAR PASCAL SplashWndProc( HWND hWnd, UINT uMsg,
                                  WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_CREATE:
      {
         int      cxSize, cySize ;
         HBITMAP  hbmCredits ;
         BITMAP   bmCredits ;

         DP3( hWDB, "SplashWndProc::WM_CREATE" ) ;

         hbmCredits = LoadBitmap( GETHINST( hWnd ),
                                  MAKEINTRESOURCE( CREDITSBMP ) ) ;
         if (NULL == hbmCredits)
         {
            DP1( hWDB, "SplashWndProc: Failed to load bitmap!" ) ;
            return ( -1 ) ;
         }

         SET_BMP_PROP( hWnd, (HANDLE) hbmCredits ) ;

         // set a timer for window destruction 

         SetTimer( hWnd, TIMERID_INIT, TIMERFREQ_INIT, NULL ) ;

         GetObject( hbmCredits, sizeof( BITMAP ), (LPSTR) &bmCredits ) ;
         cxSize = GetSystemMetrics( SM_CXSCREEN ) ;
         cySize = GetSystemMetrics( SM_CYSCREEN ) ;
         MoveWindow( hWnd, (cxSize - bmCredits.bmWidth) / 2,
                     (cySize - bmCredits.bmHeight) / 2,
                     bmCredits.bmWidth, bmCredits.bmHeight, FALSE ) ;
      }
      break ;

      case WM_TIMER:
         KillTimer( hWnd, TIMERID_INIT ) ;
         DestroyWindow( hWnd ) ;
         break ;

      case WM_PAINT:
      {
         HBITMAP      hbmOld ;
         HDC          hDC, hMemDC ;
         PAINTSTRUCT  ps ;

         hDC = BeginPaint( hWnd, &ps ) ;
         hMemDC = CreateCompatibleDC( hDC ) ;
         hbmOld = SelectObject( hMemDC, (HBITMAP) GET_BMP_PROP( hWnd ) ) ;
         BitBlt( hDC, ps.rcPaint.left, ps.rcPaint.top,
                 ps.rcPaint.right - ps.rcPaint.left,
                 ps.rcPaint.bottom - ps.rcPaint.top,
                 hMemDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY ) ;
         SelectObject( hMemDC, hbmOld ) ;
         DeleteDC( hMemDC ) ;
         EndPaint( hWnd, &ps ) ;
      }
      break ;

      case WM_DESTROY:
         DeleteObject( (HGDIOBJ) GET_BMP_PROP( hWnd ) ) ;
         REMOVE_BMP_PROP( hWnd ) ;
         break ;

      default:
         return( DefWindowProc( hWnd, uMsg, wParam, lParam ) ) ;
   }
   return ( NULL ) ;

} // end of SplashWndProc()

/************************************************************************
 *  LRESULT FAR PASCAL FrameWndProc( HWND hWnd, UINT uMsg,
 *                                   WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    This is the window proc for the MDI frame.  It is also the "main"
 *    window!
 *
 *  Comments:
 *
 ************************************************************************/

LRESULT FAR PASCAL FrameWndProc( HWND hWnd, UINT uMsg,
                                 WPARAM wParam, LPARAM lParam )
{
   HWND           hChildWnd ;
   static HANDLE  hTaskDB = NULL ;
   LPFNTASKPROC   lpfnTaskProc = NULL ;

   switch (uMsg)
   {
      case WM_CREATE:
      {
         // set a timer for the time display update.

         SetTimer( hWnd, TIMERID_UPDATE, TIMERFREQ_UPDATE, NULL ) ;

         // create the children

         hClientWnd = CreateMDIClient( hWnd ) ;
         hStatusWnd = CreateStatus( hWnd ) ;

         return 0 ;
      }

      case WM_SIZE:
      {
         WORD  wStatHeight ;

         DP3( hWDB, "FrameWndProc::WM_SIZE" ) ;

         wStatHeight = LOWORD( SendMessage( hStatusWnd, ST_GETHEIGHT,
                                            0, 0L  ) ) ;
         MoveWindow( hClientWnd, 0, 0, LOWORD( lParam ),
                     HIWORD( lParam ) - wStatHeight + 1, TRUE ) ;
         MoveWindow( hStatusWnd, 0, HIWORD( lParam ) - wStatHeight + 1,
                     LOWORD( lParam ), wStatHeight, TRUE ) ;
         return 0 ;
      } 

      case WM_ENTERIDLE:
      {
         MSG  msg ;

         while (!PeekMessage( &msg, NULL, NULL, NULL, PM_NOREMOVE ))
            TaskManagerSlice( hWnd, hTaskList ) ;
      }
      break ;

      case WM_COMMAND :
         switch (wParam)
         {
            // DN_ENDDIALOG is sent by the modeless dialog procs
            // during termination - this allows us to clean-up here.

            case SESSIONDIRDLG:
            {
               switch (HIWORD( lParam ))
               {
                  case DN_ENDDIALOG:
                  {
                     FARPROC  lpfnDlgProc ;

                     UnlinkDialog( (HWND) LOWORD( lParam ) ) ;

                     lpfnDlgProc =
                        (FARPROC) MAKELONG( GET_PROP( hWnd,
                                                      ATOM_SESSIONDIRPROCL ),
                                            GET_PROP( hWnd,
                                                      ATOM_SESSIONDIRPROCH ) ) ;

                     FreeProcInstance( lpfnDlgProc ) ;

                     REMOVE_PROP( hWnd, ATOM_SESSIONDIRPROCH ) ;
                     REMOVE_PROP( hWnd, ATOM_SESSIONDIRPROCL ) ;
                  }
                  break ;
               }
            }
            break ;

            case IDM_MENUCHANGE:
               SendMessage( hClientWnd, WM_MDISETMENU, 0, lParam ) ;
               DrawMenuBar( hWnd ) ;
               return 0 ;

            case IDM_CREATETASK:
               if (hTaskDB == NULL)
               {
                  lpfnTaskProc =
                     (LPFNTASKPROC) MakeProcInstance( (FARPROC) TaskProc,
                                                      GETHINST( hWnd ) ) ;
                  hTaskDB = CreateTaskDatabase( 1024, 1024,
                                                sizeof( HANDLE ), NULL,
                                                lpfnTaskProc ) ;
               }
               else
                  MessageBox( hWnd, "Task already created.", "Apache",
                              MB_ICONEXCLAMATION ) ;
               break ; 

            case IDM_STARTTASK:
               if (hTaskDB != NULL)
                  AddTask( hTaskList, hTaskDB ) ;
               else
                  MessageBox( hWnd, "Create task first!", "Apache",
                              MB_ICONEXCLAMATION ) ;
               break ;

            case IDM_STOPTASK:
               if (hTaskDB != NULL)
                  RemoveTask( hTaskList, hTaskDB ) ;
               else
                  MessageBox( hWnd, "Task does not exist!", "Apache",
                              MB_ICONEXCLAMATION ) ;
               break ;

            case IDM_DESTROYTASK:
               if (hTaskDB != NULL)
               {
                  RemoveTask( hTaskList, hTaskDB ) ;
                  FreeProcInstance( (FARPROC) lpfnTaskProc ) ;
                  DestroyTaskDatabase( hTaskDB ) ;
                  hTaskDB = NULL ;
               }
               else
                  MessageBox( hWnd, "Task already destroyed.", "Apache",
                              MB_ICONEXCLAMATION ) ;
               break ;

            case IDM_SESSIONDIR:
            {
               FARPROC    lpfnDlgProc ;
               HINSTANCE  hInstance ;
               HWND       hDlg ;

               // avoid multiple session directories

               if (NULL != GET_PROP( hWnd, ATOM_SESSIONDIRPROCH ))
               {
                  MessageBeep( 1 ) ;
                  return ( FALSE ) ;
               }

               hInstance = GETHINST( hWnd ) ;
               lpfnDlgProc =
                  MakeProcInstance( (FARPROC) SessionDirDlgProc,  hInstance ) ;

               hDlg = CreateDialogParam( hInstance,
                                         MAKEINTRESOURCE( SESSIONDIRDLG ),
                                         hWnd, (DLGPROC) SessionDirDlgProc,
                                         NULL ) ;

               SET_PROP( hWnd, ATOM_SESSIONDIRPROCH,
                         (HANDLE) HIWORD( lpfnDlgProc ) ) ;
               SET_PROP( hWnd, ATOM_SESSIONDIRPROCL,
                         (HANDLE) LOWORD( lpfnDlgProc ) ) ;

               // NEED: Check for error linking dialog box

               LinkDialog( hDlg ) ;
            }
            break ;

            case IDM_ABOUT:
            {
               ABOUTCREATESTRUCT  AboutCreateStruct ;

               AboutCreateStruct.hAppIcon = NULL ;
               AboutCreateStruct.hAppBitmap =
                  LoadBitmap( GETHINST( hWnd ),
                              MAKEINTRESOURCE( APACHEBMP ) ) ;
               AboutCreateStruct.lpszAppName = (LPSTR) szAppName ;
               AboutCreateStruct.lpszVersion = "01.00.00" ;
               AboutCreateStruct.lpszByLine = "By Bryan A. Woodruff\nPortions written by Charles E. Kindel, Jr." ;

               AboutBox( hWnd, &AboutCreateStruct ) ;

               DeleteObject( AboutCreateStruct.hAppBitmap ) ;

               return 0 ;
            }
            break ;

            case IDM_EXIT:
               hChildWnd =
                  (HWND) LOWORD( SendMessage( hClientWnd, WM_MDIGETACTIVE,
                                              0, 0L ) ) ;
               if (SendMessage( hChildWnd, WM_QUERYENDSESSION, 0, 0L))
                  SendMessage( hClientWnd, WM_MDIDESTROY,
                               (WPARAM) hChildWnd, 0L ) ;
               return 0 ;

            case IDM_TILE:
               SendMessage( hClientWnd, WM_MDITILE, 0, 0L ) ;
               return 0 ;

            case IDM_CASCADE:
               SendMessage( hClientWnd, WM_MDICASCADE, 0, 0L ) ;
               return 0 ;

            case IDM_ARRANGE:
               SendMessage( hClientWnd, WM_MDIICONARRANGE, 0, 0L ) ;
               return 0 ;

            case IDM_CLOSEALL:
            {
               WNDENUMPROC  WndEnumProc ;

               WndEnumProc =
                  (WNDENUMPROC) MakeProcInstance( (FARPROC) CloseEnumProc,
                                                  GETHINST( hWnd ) ) ;
               EnumChildWindows( hClientWnd, WndEnumProc, 0L ) ;
               FreeProcInstance( (FARPROC) WndEnumProc ) ;
               return 0 ;
            }

            default:
               hChildWnd =
                  (HWND) LOWORD( SendMessage( hClientWnd, WM_MDIGETACTIVE,
                                              0, 0L ) ) ;
               if (IsWindow( hChildWnd ))
                  SendMessage( hChildWnd, WM_COMMAND, wParam, lParam ) ;
               break ;
         }
         break ;

      // used for notification of menu information

      case WM_MENUSELECT:
      {
         char  szMenuItem[ MAXLEN_MENUITEM ] ;

         if (LoadString( GETHINST( hWnd ), (WORD) wParam, szMenuItem,
                         MAXLEN_MENUITEM ))
            SetWindowText( hStatusWnd, szMenuItem ) ;
         else
            SetWindowText( hStatusWnd, "Ready" ) ;
      }
      // ... FALL THROUGH ...

      case WM_SETCURSOR:
         NotifyMenuState( hWnd, uMsg, wParam, lParam ) ;
         break ;

      case WM_WININICHANGE:
         TimeResetInternational() ;

         // ... FALL THROUGH ...

      case WM_TIMER:
      {
         char  szBuf[ MAXLEN_STRLEN ] ;

         // update date and time status

         TimeGetCurDate( szBuf, 0 ) ;
         StatusSetStatBox( hStatusWnd, INDEX_DATE, szBuf ) ;

         TimeGetCurTime( szBuf, 0 ) ;
         StatusSetStatBox( hStatusWnd, INDEX_TIME, szBuf ) ;
      }
      break ;

      case WM_QUERYENDSESSION:
      case WM_CLOSE:
         SendMessage( hWnd, WM_COMMAND, IDM_CLOSEALL, 0L ) ;
         if (NULL != GetWindow( hClientWnd, GW_CHILD ))
            return 0 ;
         break ;

      case WM_DESTROY:
      {
         HMENU  hCurrentMenu = GetMenu( hWnd ) ;

         if (hMenuInit != hCurrentMenu)
            DestroyMenu( hMenuInit ) ;

         if (hMenuTerminal != hCurrentMenu)
            DestroyMenu( hMenuTerminal ) ;

         // DestroyWindow( hMsgWnd ) ;

         // clean up font

         DeleteObject( (HGDIOBJ) hStatFont ) ;

         PostQuitMessage( 0 ) ;
         return 0 ;
      }
   }

   return ( DefFrameProc( hWnd, hClientWnd, uMsg, wParam, lParam ) ) ;

} /* end of FrameWndProc() */

/************************************************************************
 *  LRESULT FAR PASCAL TaskProc( HWND hWnd, HTASKDB hTaskDB,
 *                               UINT uFunction,
 *                               WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *     Temporary task for testing.
 *
 *  Comments:
 *      8/24/91  baw  Wrote it.
 *
 ************************************************************************/

LRESULT FAR PASCAL TaskProc( HWND hWnd, HTASKDB hTaskDB, UINT uFunction,
                             WPARAM wParam, LPARAM lParam )
{
   LRESULT lRetVal ;

   switch (uFunction)
   {
      case TF_INITTASK:
      {
         HANDLE  hTaskMem ;
         LPSTR   lpMsg ;

         MessageBox( hWnd, "Task starting!", "Test Task",
                     MB_ICONEXCLAMATION | MB_OK ) ;
         hTaskMem = TaskLocalAlloc( hTaskDB, LMEM_MOVEABLE, 80 ) ;
         lpMsg = (LPSTR) TaskLocalLock( hTaskDB, hTaskMem ) ;
         lstrcpy( lpMsg, "This is a message" ) ;
         DPF3( hWDB, "hTaskMem = %04x, lpMsg = %04x:%04x\r\n", hTaskMem,
               HIWORD( (DWORD) lpMsg ), LOWORD( (DWORD) lpMsg ) ) ;
         TaskLocalUnlock( hTaskDB, hTaskMem ) ;
         SetTaskWord( hTaskDB, 0, (WPARAM) hTaskMem ) ;
         lRetVal = 0x100 ;
      }
      break ;

      case 0x100:
      {
         HANDLE  hTaskMem ;
         LPSTR   lpMsg ;

         hTaskMem = (HANDLE) GetTaskWord( hTaskDB, 0 ) ;
         lpMsg = (LPSTR) TaskLocalLock( hTaskDB, hTaskMem ) ;
         DPF3( hWDB, "hTaskMem = %04x, lpMsg = %04x:%04x\r\n", hTaskMem,
               HIWORD( (DWORD) lpMsg ), LOWORD( (DWORD) lpMsg ) ) ;
//         MessageBox( hWnd, lpMsg, "Test Task", MB_OK ) ;
         TaskLocalUnlock( hTaskDB, hTaskMem ) ;
         lRetVal = 0x100 ;
      }
      break ;

      case 0x101:
      {
         HANDLE  hTaskMem ;

         hTaskMem = (HANDLE) GetTaskWord( hTaskDB, 0 ) ;
         TaskLocalFree( hTaskDB, hTaskMem ) ;
         lRetVal = TF_EXITTASK ;
      }
      break ;

      case TF_EXITTASK:
         MessageBox( hWnd, "Task exiting!", "Test Task",
                     MB_ICONEXCLAMATION | MB_OK ) ;
         break ; 
   }
   DPF3( hWDB, "Task executed function: %u, return value: %ld\r\n",
         uFunction, lRetVal ) ;
   return ( lRetVal ) ;

} /* end of TaskProc() */

/************************************************************************
 *  VOID NotifyMenuState( HWND hWnd, UINT uMsg,
 *                         WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *     Watches the WM_SETCURSOR and WM_MENUSELECT message for
 *     entry / exit of a menu.
 *
 *  Comments:
 *      7/11/91  baw  Wrote it.
 *
 ************************************************************************/

VOID NotifyMenuState( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      // do processing for menu selection (e.g. tell the window
      // with focus (if an emulator) that we are entering or
      // exiting a menu)

      case WM_SETCURSOR:
      {
         HWND  hWndFocus ;

         // watch if we are entering a menu

         if ((HIWORD( lParam ) == 0x0000) &&
             (NULL != (hWndFocus = GetFocus())) &&
             IsChild( hWnd, hWndFocus ))
         {
            DP5( hWDB, "Sending WM_KILLFOCUS to child window" ) ;
            SendMessage( hWndFocus, WM_KILLFOCUS, NULL, NULL ) ;
         }
      }
      break ;

      case WM_MENUSELECT:
      {
         HWND  hWndFocus ;

         // watch if we are exiting a menu

         if ((wParam == 0x0000 && LOWORD( lParam ) == 0xFFFF) &&
             (NULL != (hWndFocus = GetFocus())) &&
             IsChild( hWnd, hWndFocus ))
         {
            DP5( hWDB, "Sending WM_SETFOCUS to child window" ) ;
            SendMessage( hWndFocus, WM_SETFOCUS, NULL, NULL ) ;
         }
         break ;
      }
   }

} /* end of NotifyMenuState() */

/************************************************************************
 *  BOOL FAR PASCAL CloseEnumProc( HWND hWnd, LONG lParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL FAR PASCAL CloseEnumProc( HWND hWnd, LONG lParam )
{
   if (!IsWindow( hWnd ))
      return 1 ;
   if (GetWindow( hWnd, GW_OWNER ))
      return 1 ;
   SendMessage( GetParent( hWnd ), WM_MDIRESTORE, (WPARAM) hWnd, 0L ) ;
   if (!SendMessage( hWnd, WM_QUERYENDSESSION, 0, 0L ))
      return 1 ;

   SendMessage( GetParent( hWnd ), WM_MDIDESTROY, (WPARAM) hWnd, 0L ) ;
   return 1 ;

} /* end of CloseEnumProc() */

/************************************************************************
 *  LRESULT FAR PASCAL TerminalWndProc( HWND hWnd, UINT uMsg,
 *                                      WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

LRESULT FAR PASCAL TerminalWndProc( HWND hWnd, UINT uMsg,
                                    WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_CREATE:
      {
         char szSessionName[ 33 ] ;
         LPMDICREATESTRUCT  lpMDICreateStruct ;
         LPCREATESTRUCT     lpCreateStruct ;

         // get session name and store into title bar

         lpCreateStruct =
            (LPCREATESTRUCT) lParam ;
         lpMDICreateStruct =
            (LPMDICREATESTRUCT) lpCreateStruct -> lpCreateParams ;
         if (lpMDICreateStruct -> lParam)
         {
            lstrcpy( szSessionName, (LPSTR) lpMDICreateStruct -> lParam ) ;
            if (!CreateSession( hWnd, szSessionName ))
               return ( -1 ) ;
         }

         // tell SHELL that we accept file drag & drop

         DragAcceptFiles( hWnd, TRUE ) ;
      }
      break ;

      case WM_MDIACTIVATE:
         if (TRUE == wParam)
         {
            PostMessage( hFrameWnd, WM_COMMAND, IDM_MENUCHANGE,
                         MAKELONG( hMenuTerminal, hMenuTerminalWindow ) ) ;
            if (!IsIconic( hWnd ))
               SetFocus( hWnd ) ;
         }
         if (FALSE == wParam)
            PostMessage( hFrameWnd, WM_COMMAND, IDM_MENUCHANGE,
                         MAKELONG( hMenuInit, hMenuInitWindow ) ) ;
         DrawMenuBar( hFrameWnd ) ;
         return 0 ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDM_SETUPEMULATOR:
            case IDM_SETUPCONNECTOR:
            case IDM_DOWNLOAD:
            case IDM_UPLOAD:
               return ( ProcessTerminalMessage( hWnd, uMsg, wParam, lParam ) ) ;
         }
         break ;

      case WM_DROPFILES:
         return ( ProcessTerminalMessage( hWnd, uMsg, wParam, lParam ) ) ;

      case WM_CONNECTOR_NOTIFY:
         TranslateNotification( hWnd, uMsg, wParam, lParam ) ;
         return 0 ;

      case WM_SIZE:
      {
         LONG  lRetVal ;

         lRetVal = DefMDIChildProc( hWnd, uMsg, wParam, lParam ) ;
         SizeWindows( hWnd, HIWORD( lParam ), LOWORD( lParam ) ) ;
         return ( lRetVal ) ;
      }

      case WM_SETFOCUS:
      case WM_CMD_RXBLK:
      case WM_CMD_TXBLK:
         return ( ProcessTerminalMessage( hWnd, uMsg, wParam, lParam ) ) ;

      case WM_KEYDOWN :
         // SetKeyStatus( hMsgWnd ) ;
         return 0 ;

      case WM_QUERYENDSESSION:
      case WM_CLOSE:
      {
         char  szSessionName[ MAXLEN_STRLEN ] ;

         GetWindowText( hWnd, szSessionName, MAXLEN_STRLEN ) ;
         if (IDOK != MessageBox( hWnd, "OK to close window?",
                                 szSessionName,
                                 MB_ICONQUESTION | MB_OKCANCEL ))
            return 0 ;
         SendMessage( GetParent( hWnd ), WM_MDIRESTORE, (WPARAM) hWnd, 0L ) ;
      }
      break ;

      case WM_DESTROY:
         if (NULL != GetWindowWord( hWnd, GWW_SCB ))
            DestroySession( hWnd ) ;
         break ;
   }
   return ( DefMDIChildProc( hWnd, uMsg, wParam, lParam ) ) ;
                                        
} /* end of TerminalWndProc() */

/************************************************************************
 * End of File: apache.c
 ************************************************************************/
