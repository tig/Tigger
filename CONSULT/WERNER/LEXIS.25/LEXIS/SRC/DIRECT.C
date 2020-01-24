/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  direct.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *    This module handles the "Default Directories" Dialog Box...
 *
 *   Revisions:  
 *     00.00.000  2/21/91 cek   first version
 *
 ************************************************************************/

#include <windows.h>
#include <stdlib.h>
#include <cmndll.h>
#include "..\inc\lexis.h"
#include "..\inc\lexdlgd.h"
#include "..\inc\global.h"
#include "..\inc\state.h"
#include "..\inc\filedlgs.h"
#include "..\inc\help.h"

#include "..\inc\direct.h"
/************************************************************************
 * Imported variables
 ************************************************************************/

/************************************************************************
 * Local Defines
 ************************************************************************/
/************************************************************************
 * Macros 
 ************************************************************************/

/************************************************************************
 * Local data structures
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/
int FAR PASCAL
fnDirectories( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/

/*************************************************************************
 *  BOOL FAR PASCAL DirectoriesDialog( HWND hWnd )
 *
 *  Description:
 *    This function is called by MainWnd.C and handles the default dirs
 *    dialog box.
 *
 *  Return Value
 *    TRUE if the user hit OK, FALSE otherwise.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL DirectoriesDialog( HWND hWnd )
{
   BOOL n ;
   FARPROC lpfnDlgProc ;

   //
   // Bring up the dialog box
   //

   if (lpfnDlgProc = MakeProcInstance( (FARPROC)fnDirectories,
                                       GETHINST( hWnd ) ))
   {
      n = DialogBox( GETHINST( hWnd ), "DIRECTORIES", hWnd, lpfnDlgProc ) ;
      FreeProcInstance( lpfnDlgProc ) ;
   }
   else
      n = DLG_ERROR ;

   //
   // If one of the "other dialog" buttons was hit to end the dialog
   // box, then go to that dialog box by simulating a Menu selection
   //
   if (n == IDD_HARDWARE_DLG)
      PostMessage( hWnd, WM_COMMAND, IDM_SETUP_HARDWARE, 0L ) ;

   if (n == IDD_USERPREF_DLG)
      PostMessage( hWnd, WM_COMMAND, IDM_SETUP_USERPREF, 0L ) ;


   DP5( hWDB, "DirectoriesDialog() returning %d", n ) ;
   return n ;

}/* DirectoriesDialog() */

/*************************************************************************
 *  int FAR PASCAL
 *    fnDirectories( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description:
 *    Window procedure for the Default directories dlg box.
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL
fnDirectories( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         LPLEXISSTATE lpLS ;

         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;

         //
         // set edit controls up...
         //
         DlgSetItemLength( hDlg, IDD_RECORDING_DIR, _MAX_PATH ) ;
         DlgSetItemLength( hDlg, IDD_NOTEPAD_DIR, _MAX_PATH ) ;
         DlgSetItemLength( hDlg, IDD_DIARY_DIR, _MAX_PATH ) ;
         DlgSetItemLength( hDlg, IDD_DOCUMENT_DIR, _MAX_PATH ) ;

         //
         // Get strings from Global memory and put in edit controls...
         //
         if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
         {
            EndDialog( hDlg, DLG_ERROR );
            return FALSE ;
         }
         SetDlgItemText( hDlg, IDD_RECORDING_DIR, lpLS->szRecord ) ;
         SetDlgItemText( hDlg, IDD_NOTEPAD_DIR, lpLS->szNotepad ) ;
         SetDlgItemText( hDlg, IDD_DIARY_DIR, lpLS->szDiary ) ;
         SetDlgItemText( hDlg, IDD_DOCUMENT_DIR, lpLS->szDocument ) ;
         GlobalUnlock( ghGlobals ) ;
      }
      break ;

      case WM_COMMAND:
         switch( wParam )
         {
            case IDD_HELP:
               Help( HELP_HELPONHELP, NULL, 0 );
            break ;

            case IDD_USERPREF_DLG:
            case IDD_HARDWARE_DLG:
            case IDOK:
            {
               LPLEXISSTATE lpLS ;
               HCURSOR      hCur ;

               if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
               {
                  EndDialog( hDlg, DLG_ERROR );
                  return FALSE ;
               }

               hCur = SetHourGlass( ) ;
               //
               // Get strings into globals...
               //

               GetDlgItemText( hDlg, IDD_RECORDING_DIR, lpLS->szRecord,
                               _MAX_PATH ) ;
               GetDlgItemText( hDlg, IDD_NOTEPAD_DIR, lpLS->szNotepad,
                               _MAX_PATH ) ;
               GetDlgItemText( hDlg, IDD_DIARY_DIR, lpLS->szDiary,
                               _MAX_PATH ) ;
               GetDlgItemText( hDlg, IDD_DOCUMENT_DIR, lpLS->szDocument,
                               _MAX_PATH ) ;

               //
               // Write dirs to profile....
               //
               WriteDirectories( GETHINST( lpLS->hwndMainWnd ),
                                       lpLS->szDiary, lpLS->szNotepad,
                                       lpLS->szRecord, lpLS->szDocument ) ;

               //
               // Make changes take effect for this session...
               //
               InitDirectories( lpLS->szDiary, lpLS->szNotepad,
                       lpLS->szRecord, lpLS->szDocument ) ;

               GlobalUnlock( ghGlobals ) ;
               ResetHourGlass( hCur ) ;

               EndDialog( hDlg, wParam ) ;
            }
            break ;

            case IDCANCEL:
               EndDialog( hDlg, DLG_CANCEL ) ;
            break ;
         }

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;
         TEXTMETRIC  tm ;
         HDC         hDC ;

         hDC = BeginPaint(hDlg, &ps ) ;

         GetTextMetrics( hDC, &tm ) ;
         DrawDropShadow( hDC, GetDlgItem( hDlg, IDD_BOX0 ),
                         tm.tmAveCharWidth / 2 ) ;

         EndPaint( hDlg, &ps ) ;
      }
      return FALSE ;

      default:
         return FALSE ;
   }

   return TRUE ;

}/* fnDirectories() */


/************************************************************************
 * End of File: direct.c
 ************************************************************************/


