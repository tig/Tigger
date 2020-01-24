/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  hardware.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *       This module is responsible for the Hardware settings dialog
 *       box.  
 *
 *   Revisions:  
 *     00.00.000  2/23/91 cek   First version
 *
 ************************************************************************/
#include "..\inc\undef.h"
#include <windows.h>
#include <stdlib.h>
#include <direct.h>
#include <cmndll.h>

#include "..\inc\lexis.h"
#include "..\inc\lexdlgd.h"
#include "..\inc\hardware.h"
#include "..\inc\global.h"
#include "..\inc\help.h"

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
int FAR PASCAL fnHardware( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;
BOOL NEAR PASCAL ModemDialog( HWND hwndParent, LPSTR lpszModem ) ;
BOOL NEAR PASCAL NetworksDialog( HWND hwndParent ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * External/Exported functions
 ************************************************************************/
BOOL FAR PASCAL HardwareDialog( HWND hWnd )
{
   BOOL n ;
   FARPROC lpfnDlgProc ;
   LPLEXISSTATE lpLS ;

   if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
      return FALSE ;

   //
   // Bring up the dialog box
   //

   if (lpfnDlgProc = MakeProcInstance( (FARPROC)fnHardware,
                                       GETHINST( hWnd ) ))
   {
      n = DialogBox( GETHINST( hWnd ), "HARDWARE", hWnd, lpfnDlgProc ) ;
      FreeProcInstance( lpfnDlgProc ) ;
   }
   else
   {
      GlobalUnlock( ghGlobals ) ;
      return FALSE ;
   }

   if (n != DLG_CANCEL && n != DLG_ERROR)
   {
      HANDLE hInst = GETHINST( hWnd ) ;

      //
      // Update profile file...
      //

      //
      // Make changes immediate...
      //
   }

   //
   // If one of the "other dialog" buttons was hit to end the dialog
   // box, then go to that dialog box by simulating a Menu selection
   //
   if (n == IDD_DIRECTORIES_DLG)
      PostMessage( hWnd, WM_COMMAND, IDM_SETUP_DIRECTORIES, 0L ) ;

   if (n == IDD_USERPREF_DLG)
      PostMessage( hWnd, WM_COMMAND, IDM_SETUP_USERPREF, 0L ) ;

   GlobalUnlock( ghGlobals ) ;
   return TRUE ;

}/* HardwareDialog() */


/*************************************************************************
 *  int FAR PASCAL
 *    fnHardware( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description:
 *    Window procedure for the Hardware Setup dialog box.
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL fnHardware( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         LPLEXISSTATE lpLS ;

         if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
         {
            EndDialog( hDlg, DLG_ERROR ) ;
            return FALSE ;
         }

         //
         // LexisState contains all the stuff we need...
         //

         //
         // Center the dialog within the main window
         //
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;

         GlobalUnlock( ghGlobals ) ;
      }
      break ;

      case WM_COMMAND:
         switch( wParam )
         {
            case IDD_CONNECTOR:
               //
               // Popup the dialog box for the specified connector.
               // the connector dialog box is exported from the connector
               // driver (DLL)
               //
            break ;

            case IDD_MODEM:
               //
               // Popup the modem dialog box.  The second parameter is the
               // current modem.  Someday we may want to expand the modem
               // dialog box...
               //
               ModemDialog( hDlg, NULL ) ;
            break ;

            case IDD_NETWORKS:
               // 
               // Popup the "networks and phone nubmers" dialog box.
               //
               NetworksDialog( hDlg ) ;
            break ;

            case IDD_HELP:
               Help( HELP_HELPONHELP, NULL, 0 );
            break ;

            case IDD_USERPREF_DLG:
            case IDD_DIRECTORIES_DLG:
            case IDOK:
            {
               HANDLE hInst = GETHINST( GetParent( hDlg )) ;

               LPLEXISSTATE lpLS ;

               if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
               {
                  EndDialog( hDlg, DLG_ERROR ) ;
                  return FALSE ;
               }

               GlobalUnlock( ghGlobals ) ;

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

}/* fnHardware() */

/*************************************************************************
 *  int FAR PASCAL
 *    fnModem( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description:
 *    Window procedure for the Modem Setup dialog box.
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL fnModem( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         LPLEXISSTATE lpLS ;

         if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
         {
            EndDialog( hDlg, DLG_ERROR ) ;
            return FALSE ;
         }

         //
         // LexisState contains all the stuff we need...
         //

         //
         // Center the dialog within the main window
         //
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;

         GlobalUnlock( ghGlobals ) ;
      }
      break ;

      case WM_COMMAND:
         switch( wParam )
         {
            case IDD_HELP:
               Help( HELP_HELPONHELP, NULL, 0 );
            break ;

            case IDOK:
            {
               HANDLE hInst = GETHINST( GetParent( hDlg )) ;

               LPLEXISSTATE lpLS ;

               if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
               {
                  EndDialog( hDlg, DLG_ERROR ) ;
                  return FALSE ;
               }

               GlobalUnlock( ghGlobals ) ;

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

}/* fnModem() */


/*************************************************************************
 *  int FAR PASCAL
 *    fnNetworks( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description:
 *    Window procedure for the Networks Setup dialog box.
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL fnNetworks( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         LPLEXISSTATE lpLS ;

         if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
         {
            EndDialog( hDlg, DLG_ERROR ) ;
            return FALSE ;
         }

         //
         // LexisState contains all the stuff we need...
         //

         //
         // Center the dialog within the main window
         //
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;

         GlobalUnlock( ghGlobals ) ;
      }
      break ;

      case WM_COMMAND:
         switch( wParam )
         {
            case IDD_HELP:
               Help( HELP_HELPONHELP, NULL, 0 );
            break ;

            case IDOK:
            {
               HANDLE hInst = GETHINST( GetParent( hDlg )) ;

               LPLEXISSTATE lpLS ;

               if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
               {
                  EndDialog( hDlg, DLG_ERROR ) ;
                  return FALSE ;
               }

               GlobalUnlock( ghGlobals ) ;

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

}/* fnNetworks() */


/************************************************************************
 * Internal functions
 ************************************************************************/
/*************************************************************************
 *  BOOL NEAR PASCAL ModemDialog( HWND hwndParent, LPSTR lpszModem )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL ModemDialog( HWND hwndParent, LPSTR lpszModem )
{
   BOOL n ;
   FARPROC lpfnDlgProc ;
   LPLEXISSTATE lpLS ;

   if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
      return FALSE ;

   //
   // Bring up the dialog box
   //
   if (lpfnDlgProc = MakeProcInstance( (FARPROC)fnModem,
                                       GETHINST( hwndParent ) ))
   {
      n = DialogBox( GETHINST( hwndParent ), "MODEM", hwndParent, lpfnDlgProc ) ;
      FreeProcInstance( lpfnDlgProc ) ;
   }
   else
   {
      GlobalUnlock( ghGlobals ) ;
      return FALSE ;
   }

   if (n != DLG_CANCEL && n != DLG_ERROR)
   {
      HANDLE hInst = GETHINST( hwndParent ) ;

      //
      // Update profile file...
      //

      //
      // Make changes immediate...
      //
   }

   GlobalUnlock( ghGlobals ) ;

   return TRUE ;
}/* ModemDialog() */

/*************************************************************************
 *  BOOL NEAR PASCAL NetworksDialog( HWND hwndParent )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL NetworksDialog( HWND hwndParent )
{
   BOOL n ;
   FARPROC lpfnDlgProc ;
   LPLEXISSTATE lpLS ;

   if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
      return FALSE ;

   //
   // Bring up the dialog box
   //
   if (lpfnDlgProc = MakeProcInstance( (FARPROC)fnNetworks,
                                       GETHINST( hwndParent ) ))
   {
      n = DialogBox( GETHINST( hwndParent ), "PHONENUM", hwndParent, lpfnDlgProc ) ;
      FreeProcInstance( lpfnDlgProc ) ;
   }
   else
   {
      GlobalUnlock( ghGlobals ) ;
      return FALSE ;
   }

   if (n != DLG_CANCEL && n != DLG_ERROR)
   {
      HANDLE hInst = GETHINST( hwndParent ) ;

      //
      // Update profile file...
      //

      //
      // Make changes immediate...
      //
   }

   GlobalUnlock( ghGlobals ) ;

   return TRUE ;
}/* NetworksDialog() */



/************************************************************************
 * End of File: hardware.c
 ************************************************************************/

