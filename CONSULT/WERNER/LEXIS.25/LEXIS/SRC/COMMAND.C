/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  keyboard.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This module handles all menu, button, and command commands 
 *
 *   Revisions:  
 *     00.00.000  2/28/91 cek   first version
 *
 *
 ************************************************************************/
#include "..\inc\undef.h"
#include <windows.h>
#include <cmndll.h>
#include <memory.h>

#include "..\inc\lexis.h"
#include "..\inc\timer.h"
#include "..\inc\menu.h"
#include "..\inc\topstat.h"
#include "..\inc\botstat.h"
#include "..\inc\button.h"
#include "..\inc\state.h"
#include "..\inc\userpref.h"
#include "..\inc\profile.h"
#include "..\inc\print.h"
#include "..\inc\filedlgs.h"
#include "..\inc\hardware.h"
#include "..\inc\help.h"
#include "..\inc\print.h"
#include "..\inc\term.h"

#include "..\inc\mainwnd.h"
#include "..\inc\direct.h"

#include "..\inc\global.h"
#include "..\inc\command.h"

/************************************************************************
 * Imported variables
 ************************************************************************/
/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/

BOOL NEAR PASCAL AutoSize( HWND hWnd ) ;
/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * External functions
 ************************************************************************/

/*************************************************************************
 *  LONG FAR PASCAL ProcessMainCmd( HWND hWnd, WORD wParam, LONG lParam )
 *
 *  Description:
 *    This function orchestrates the handling of WM_COMMAND message to the
 *    main window proc.  
 *
 *  Return Value
 *          Returns one of the following values:
 *
 *    TRUE  Message was handled and should not be passed on to DefWindowProc
 *    FALSE Message was NOT handled and should be processed.
 *
 *
 *  Comments:
 *
 *************************************************************************/
LONG FAR PASCAL ProcessMainCmd( HWND hWnd, WORD wParam, LONG lParam )
{
   static BOOL fConnected = FALSE ;


   //
   // ignore the buttons for now...
   //
   if (wParam >= IDB_START && wParam <= IDB_END)
      return TRUE ;

   switch (wParam)
   {
      // -------------- FILE MENU -----------------
      //
      // File.New
      //
      case IDM_NEW_RECORDING:
      {
         char szFileName[144] ;
         lstrcpy( szFileName, "" ) ;

         NewFileDialog( hWnd, szFileName, NEW_RECORD ) ;
      }
      break ;

      case IDM_NEW_DIARY:
      {
         char szFileName[144] ;
         lstrcpy( szFileName, "" ) ;
                                    
         NewFileDialog( hWnd, szFileName, NEW_DIARY ) ;
      }
      break ;

      case IDM_NEW_NOTEPAD:
      {
         char szFileName[144] ;
         lstrcpy( szFileName, "" ) ;
                         
         NewFileDialog( hWnd, szFileName, NEW_NOTEPAD ) ;
      }
      break ;

      //
      // File.Open
      //
      case IDM_OPEN_RECORDING:
      {
         char szFileName[144] ;
         lstrcpy( szFileName, "" ) ;
                         
         OpenFileDialog( hWnd, szFileName, OPEN_RECORD ) ;
      }
      break ;

      case IDM_OPEN_DIARY:
      {
         char szFileName[144] ;
         lstrcpy( szFileName, "" ) ;
                         
         OpenFileDialog( hWnd, szFileName, OPEN_DIARY ) ;
      }
      break ;

      case IDM_OPEN_NOTEPAD:
      {
         char szFileName[144] ;
         lstrcpy( szFileName, "" ) ;

         OpenFileDialog( hWnd, szFileName, OPEN_NOTEPAD ) ;
      }
      break ;


      //
      // File.Close
      //
      case IDM_CLOSE_RECORDING:
      case IDM_CLOSE_DIARY:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      //
      // File.Print
      //
      case IDM_PRINT_RECORDING:
         PrintFile( hWnd, PRINT_RECORD ) ;
      break ;

      case IDM_PRINT_DIARY:
         PrintFile( hWnd, PRINT_DIARY ) ;
      break ;

      case IDM_PRINT_NOTEPAD:
         PrintFile( hWnd, PRINT_NOTEPAD ) ;
      break ;

      case IDM_PRINT_DOCUMENT:
         PrintFile( hWnd, PRINT_DOCUMENT ) ;
      break ;

      case IDM_PRINT_SCREEN:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      //
      // File.Recording...
      //
      case IDM_RECORDING:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      //
      // File.Session Diary......
      //
      case IDM_DIARY:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      //
      // File.Printer...
      //
      case IDM_PRINTER:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      //
      // File.Setup
      //
      case IDM_SETUP_USERPREF:
         UserPrefsDialog( hWnd ) ;
      break ;

      case IDM_SETUP_DIRECTORIES:
         DirectoriesDialog( hWnd ) ;
      break ;

      case IDM_SETUP_HARDWARE:
         HardwareDialog( hWnd ) ;
      break ;

      case IDM_PAGESETUP:
         PageSetupDialog( hWnd ) ;
      break ;

      case IDM_PRINTSETUP:
         PrinterSetupDialog( hWnd ) ;
      break ;

      //
      // File.Signon
      //
      case IDM_SIGNON:
         if (fConnected)
         {
            ConnectTerm( GETHWNDTERM( hWnd ), FALSE ) ;
         }
         else
         {
            ConnectTerm( GETHWNDTERM( hWnd ), TRUE ) ;
         }
         fConnected = !fConnected ;

         //Signon() ;
      break ;

      //
      // File.Exit
      //
      case IDM_EXIT:
         SendMessage( hWnd, WM_CLOSE, 0, 0L ) ;
      break ;

      // -------------- EDIT MENU -----------------
      // 
      // Edit.Copy
      //
      case IDM_COPY:
      {
         HWND hwndBotStat = GETHWNDBOTSTAT( hWnd ) ;

//         NotImplemented( hWnd, wParam, lParam ) ;
         WriteTerm( GETHWNDTERM( hWnd), "X" ) ;
         UpdateWindow( GETHWNDTERM( hWnd )) ;

         //
         // This is a HACK! to get rid of the "Cut,Copy and Paste operations"
         // message that menu.c cannot get rid of
         //
         if (hwndBotStat)
            SetWindowText( hwndBotStat, "" ) ;
      }
      break ;

      // 
      // Edit.Paste
      //
      case IDM_PASTE:
      {
         HWND hwndBotStat = GETHWNDBOTSTAT( hWnd ) ;

         //NotImplemented( hWnd, wParam, lParam ) ;
         WriteTerm( GETHWNDTERM( hWnd ), "This line ends with a CR/LF\r\n" ) ;
         UpdateWindow( GETHWNDTERM( hWnd )) ;

         //
         // This is a HACK! to get rid of the "Cut,Copy and Paste operations"
         // message that menu.c cannot get rid of
         //
         if (hwndBotStat)
            SetWindowText( hwndBotStat, "" ) ;
      }
      break ;

      // 
      // Edit.CopyDiary
      //
      case IDM_COPYDIARY:
         //NotImplemented( hWnd, wParam, lParam ) ;
         WriteTerm( GETHWNDTERM( hWnd ), "This line ends with a LF/CR\n\r" ) ;
         UpdateWindow( GETHWNDTERM( hWnd )) ;

      break ;

      // -------------- SEARCH MENU ---------------
      //
      // Search.New Search
      //
      case IDM_NEWSEARCH:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      //
      // Search.Modify
      //
      case IDM_SEARCHMODIFY:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      case IDM_SEARCHLEVEL:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      case IDM_LIBRARY:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      case IDM_FILE:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      case IDM_CLIENT:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      case IDM_ECLIPSE:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      case IDM_ECLIPSERECALL:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      case IDM_SEARCHDISPLAY:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;

      case IDM_STOP:
         NotImplemented( hWnd, wParam, lParam ) ;
      break ;


      // -------------- VIEW MENU -----------------

      // -------------- FORMATS MENU --------------

      // -------------- SERVICES MENU -------------

      // -------------- DELIVERY MENU -------------

      // -------------- INFO MENU -----------------

      // -------------- HELP MENU -----------------
      case IDM_HELP_INDEX:
      case IDM_HELP_KEYBOARD:
      case IDM_HELP_BASIC:
      case IDM_HELP_COMMANDS:
      case IDM_HELP_PROCEDURES:
      case IDM_HELP_HELPONHELP:
         Help( HELP_HELPONHELP, NULL, 0 ) ;
      break ;
      // 
      // Help.About
      //
      case IDM_ABOUT:
         Help( 0, NULL, IDM_ABOUT ) ;
      break ;

      case IDB_TOPSTAT:
      {
         //
         // if it's the bottom status bar (double click).
         //
         if (LOWORD( lParam ) & MK_LBUTTON)     // only left button counts
         {
            switch (HIWORD( lParam ))  // which box was clicked on?
            {

               default:
                  NotImplemented( hWnd, HIWORD(lParam) + wParam, lParam ) ;

            }
            break ;
         }
      }
      break ;
                      
      case IDB_BOTSTAT:
      {
         //
         // if it's the bottom status bar (double click).
         //
         if (LOWORD( lParam ) & MK_LBUTTON)     // only left button counts
         {
            switch (HIWORD( lParam ))  // which box was clicked on?
            {

               case 0xFFFF:
                  Help( HELP_HELPONHELP, NULL, 0 ) ;
               break ;

               case IDB_CD_STATUS - IDB_BOTSTAT:
                  SetCDIndicator( GETHWNDBOTSTAT( hWnd ), TRUE ) ;
               break ;

               case IDB_RECORD_STATUS - IDB_BOTSTAT:
                  SetRecordIndicator( GETHWNDBOTSTAT( hWnd ), TRUE ) ;
               break ;

               case IDB_PRINT_STATUS - IDB_BOTSTAT:
                  SetPrintIndicator( GETHWNDBOTSTAT( hWnd ), TRUE ) ;
               break ;

               case IDB_DIARY_STATUS - IDB_BOTSTAT:
                  SetDiaryIndicator( GETHWNDBOTSTAT( hWnd ), TRUE ) ;
               break ;

               default:
                  NotImplemented( hWnd, HIWORD(lParam) + wParam, lParam ) ;

            }
            break ;
         }
         if (LOWORD( lParam ) & MK_RBUTTON)     // only left button counts
         {
            switch (HIWORD( lParam ))  // which box was clicked on?
            {
               case IDB_CD_STATUS - IDB_BOTSTAT:
                  SetCDIndicator( GETHWNDBOTSTAT( hWnd ), FALSE ) ;
               break ;

               case IDB_RECORD_STATUS - IDB_BOTSTAT:
                  SetRecordIndicator( GETHWNDBOTSTAT( hWnd ), FALSE ) ;
               break ;

               case IDB_PRINT_STATUS - IDB_BOTSTAT:
                  SetPrintIndicator( GETHWNDBOTSTAT( hWnd ), FALSE ) ;
               break ;

               case IDB_DIARY_STATUS - IDB_BOTSTAT:
                  SetDiaryIndicator( GETHWNDBOTSTAT( hWnd ), FALSE ) ;
               break ;

               default:
                  NotImplemented( hWnd, HIWORD(lParam) + wParam, lParam ) ;

            }
            break ;
         }
      }
      break ;
                      
      default :
         NotImplemented( hWnd, wParam, lParam ) ;
         // 
         // If we didn't handle the message return FALSE
         //
         return FALSE ;
   }

   //
   // if we handled the message then return TRUE.
   //
   return TRUE ;

}/* ProcessMainCmd() */

void FAR PASCAL NotImplemented( HWND hWnd, WORD wParam, LONG lParam )
{
   char szMsg[MAX_STRLEN+1] ;

   LoadString( GETHINST( hWnd ), wParam, szMsg, MAX_STRLEN ) ;
   ErrorResBox( hWnd, NULL, MB_ICONINFORMATION,
                  IDS_APPTITLE_COMBINED, IDS_NOTIMPLEMENTED,
                  (LPSTR)szMsg ) ;
}

/************************************************************************
 * Internal functions
 ************************************************************************/
/*************************************************************************
 *  BOOL NEAR PASCAL AutoSize( HWND hWnd )
 *
 *  Description: 
 *
 *    This function "auto" sizes the window so that the terminal is
 *    exactly big enough for 80x24 characters.  It centers the window
 *    on the display if it would not fit.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL AutoSize( HWND hWnd )
{
   short xSize, ySize ;
   HWND  hwndTemp ;

   //
   // First ask the terminal how big wants to be
   //
   GetTermAutoSize( GETHWNDTERM( hWnd ), &xSize, &ySize ) ;

   //
   // Then add in the border
   //
   xSize += (GetSystemMetrics( SM_CXFRAME ) * 2) ;
   ySize += (GetSystemMetrics( SM_CYFRAME )  * 2) ;

   //
   // X is now complete.  But Y still needs the menu, caption, stat bars,
   // and button bar...
   //
   ySize += GetSystemMetrics( SM_CYCAPTION ) ;
   ySize += GetSystemMetrics( SM_CYMENU ) ;

   if (hwndTemp = GETHWNDTOPSTAT( hWnd ))
      ySize += (WORD)SendMessage( hwndTemp, ST_GETHEIGHT, 0, 0L ) - 1 ;

   if (hwndTemp = GETHWNDBUTTON( hWnd ))
      ySize += (WORD)SendMessage( hwndTemp, BU_GETHEIGHT, 0, 0L ) ;

   if (hwndTemp = GETHWNDBOTSTAT( hWnd ))
      ySize += (WORD)SendMessage( hwndTemp, ST_GETHEIGHT, 0, 0L ) - 1 ;

   SetWindowPos( hWnd, NULL, 0, 0,
                    xSize, ySize, SWP_NOMOVE | SWP_NOZORDER ) ;


   return TRUE ;

}/* AutoSize() */

/************************************************************************
 * End of File: commmand.c
 ************************************************************************/
