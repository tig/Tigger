/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  print.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This module contains the printing functions for Lexis.  Since
 *    most of the real work is done in the common DLL these functions
 *    are pretty much stubs.
 *
 *   Revisions:  
 *     00.00.000  2/22/91 cek   First version
 *
 *
 ************************************************************************/
#include "..\inc\undef.h"
#include <windows.h>

#include <cmndll.h>
#include "..\inc\lexprnt.h"

#include "..\inc\lexis.h"
#include "..\inc\lexdlgd.h"
#include "..\inc\state.h"
#include "..\inc\profile.h"
#include "..\inc\print.h"
#include "..\inc\help.h"
#include "..\inc\filedlgs.h"
#include "..\inc\global.h"

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
fnPageSetup( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/
static char szDriver[_MAX_FNAME+_MAX_EXT+1] ;
static char szDevice[64] ;
static char szPort[_MAX_PATH+1] ;

/************************************************************************
 * External functions
 ************************************************************************/
/*************************************************************************
 *  BOOL FAR PASCAL PrinterSetupDialog( HWND hWnd )
 *
 *  Description:
 *    This function displays the select printer dialog box.
 *
 *  Type/Parameter
 *          Description
 * 
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL PrinterSetupDialog( HWND hWnd )
{
   WORD wRet ;

   wRet = PrintSelPrnDialog( hWnd, NULL, szDriver, szDevice, szPort,
                             SELPRN_STANDARD | SELPRN_ALLOWSETUP ) ;

   if (wRet == DLG_OK)
      return TRUE ;

   return FALSE ;

}/* PrinterSetupDialog() */

/*************************************************************************
 *  BOOL FAR PASCAL PageSetupDialog( HWND hWnd )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL PageSetupDialog( HWND hWnd )
{

   BOOL n ;
   FARPROC lpfnDlgProc ;

   //
   // Bring up the dialog box
   //

   if (lpfnDlgProc = MakeProcInstance( (FARPROC)fnPageSetup,
                                       GETHINST( hWnd ) ))
   {
      n = DialogBox( GETHINST( hWnd ), "PAGE", hWnd, lpfnDlgProc ) ;
      FreeProcInstance( lpfnDlgProc ) ;
   }
   else
      n = DLG_ERROR ;

   //
   // If one of the "other dialog" buttons was hit to end the dialog
   // box, then go to that dialog box by simulating a Menu selection
   //
   if (n == IDD_PRINTER_DLG)
      PostMessage( hWnd, WM_COMMAND, IDM_PRINTSETUP, 0L ) ;

   return n ;

}/* PageSetupDialog() */

/*************************************************************************
 *  int FAR PASCAL
 *    fnPageSetup( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description:
 *    Window procedure for the Default directories dlg box.
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL
fnPageSetup( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         char szHeader[MAX_STRLEN] ;
         char szFooter[MAX_STRLEN] ;

         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;

         //
         // set edit controls up...
         //
         DlgSetItemLength( hDlg, IDD_HEADER, MAX_STRLEN ) ;
         DlgSetItemLength( hDlg, IDD_HEADER, MAX_STRLEN ) ;

         ReadHeaderFooter( GETHINST( hDlg ), szHeader, szFooter ) ;
         SetDlgItemText( hDlg, IDD_HEADER, szHeader ) ;
         SetDlgItemText( hDlg, IDD_FOOTER, szFooter ) ;
      }
      break ;

      case WM_COMMAND:
         switch( wParam )
         {
            case IDD_HELP:
               Help( HELP_HELPONHELP, NULL, 0 );
            break ;

            case IDD_PRINTER_DLG:
            case IDOK:
            {
               char szHeader[MAX_STRLEN] ;
               char szFooter[MAX_STRLEN] ;

               GetDlgItemText( hDlg, IDD_HEADER, szHeader,
                               _MAX_PATH ) ;
               GetDlgItemText( hDlg, IDD_FOOTER, szFooter,
                               _MAX_PATH ) ;

               //
               // Write strings to profile....
               //
               WriteHeaderFooter( GETHINST( hDlg ), szHeader, szFooter ) ;

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

}/* fnPageSetup() */



/*************************************************************************
 *  BOOL FAR PASCAL InitPrinter( HWND hWwnd )
 *
 *  Description: 
 *
 *    This function initializes the local printer strings and may do
 *    other stuff in the future...
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL InitPrinter( HWND hWnd )
{
   if (DLG_ERROR == PrintSelPrnDialog( hWnd, NULL, szDriver, szDevice, szPort,
                              SELPRN_GETDEFAULT ))
   {
      lstrcpy( szDriver, "" ) ;
      lstrcpy( szDevice, "" ) ;
      lstrcpy( szPort, "" ) ;
      return FALSE ;
   }
   return TRUE ;
}/* InitPrinter() */

/****************************************************************
 *  BOOL FAR PASCAL PrintFile( HWND hWnd, WORD wFileType )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL PrintFile( HWND hWnd, WORD wFileType )
{
   char szFileName[144] ;

   lstrcpy( szFileName, "" ) ;

   if (DLG_OK == PrintFileDialog( hWnd, szFileName, wFileType ))
   {
      WinPrtPrintFile( hWnd, szDriver, szDevice, szPort,
                       NULL, NULL, szFileName ) ;
   }

   return TRUE ;

} /* PrintFile()  */


/************************************************************************
 * Internal functions
 ************************************************************************/

/************************************************************************
 * End of File: print.c
 ************************************************************************/

