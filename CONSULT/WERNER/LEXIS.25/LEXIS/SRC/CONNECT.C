/************************************************************************
 *
 *     Project:  LEXIS 2000 2.5 for Windows
 *
 *      Module:  connect.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "..\inc\undef.h"
#include <windows.h>

#include <cmndll.h>

#include "..\inc\lexis.h"
#include "..\inc\lexdlgd.h"
#include "..\inc\connect.h"

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
fnConnect( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * External functions
 ************************************************************************/
/*************************************************************************
 *  BOOL FAR PASCAL ConnectDialog( HWND hWnd )
 *
 *  Description:
 *    This function displays the select connecter dialog box.
 *
 *  Type/Parameter
 *          Description
 * 
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL ConnectDialog( HWND hWnd )
{
   BOOL n ;
   FARPROC lpfnDlgProc ;

   //
   // Bring up the dialog box
   //

   if (lpfnDlgProc = MakeProcInstance( (FARPROC)fnConnect,
                                       GETHINST( hWnd ) ))
   {
      n = DialogBox( GETHINST( hWnd ), "CONNECT", hWnd, lpfnDlgProc ) ;
      FreeProcInstance( lpfnDlgProc ) ;
   }
   else
      n = DLG_ERROR ;

   return n ;

}/* ConnectDialog() */

/*************************************************************************
 *  int FAR PASCAL
 *    fnConnect( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description:
 *    Window procedure for the Connect dialog box.
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL
fnConnect( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
{
   static HANDLE hTimer ;
   static BOOL   fFlag ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;
         hTimer = SetTimer( hDlg, 2, 200, NULL );

         SendMessage( GetDlgItem( hDlg, IDD_LED0), LED_SETCOLOR,
                        0, RGBGRAY ) ;

         SendMessage( GetDlgItem( hDlg, IDD_LED1), LED_SETCOLOR,
                        0, RGBGRAY ) ;

         SendMessage( GetDlgItem( hDlg, IDD_CONNECTING), LED_SETCOLOR,
                        0, RGBLTGREEN ) ;

         SendMessage( GetDlgItem( hDlg, IDD_PRIMARY), LED_SETCOLOR,
                        0, RGBLTGREEN ) ;

         fFlag = FALSE ;
      }
      break ;

      case WM_COMMAND:
         switch( wParam )
         {
            case IDOK:
               if ( hTimer )
                  KillTimer( hDlg, 42 );
               EndDialog( hDlg, DLG_OK ) ;
            break ;

            case IDCANCEL:
               if ( hTimer )
                  KillTimer( hDlg, 42 );
               EndDialog( hDlg, DLG_CANCEL ) ;
            break ;
         }

      case WM_TIMER:
      {
         if (fFlag)
         {
            SendMessage( GetDlgItem( hDlg, IDD_LED0), LED_SETCOLOR,
                           0, RGBLTRED ) ;
            SendMessage( GetDlgItem( hDlg, IDD_LED1), LED_SETCOLOR,
                           0, RGBGRAY ) ;
            fFlag = FALSE ;
         }
         else
         {
            SendMessage( GetDlgItem( hDlg, IDD_LED0), LED_SETCOLOR,
                           0, RGBGRAY ) ;
            SendMessage( GetDlgItem( hDlg, IDD_LED1), LED_SETCOLOR,
                           0, RGBLTRED ) ;
            fFlag = TRUE ;
         }
      }
      break;


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

}/* fnConnect() */


/************************************************************************
 * Internal functions
 ************************************************************************/


/************************************************************************
 * End of File: connect.c
 ***********************************************************************/

