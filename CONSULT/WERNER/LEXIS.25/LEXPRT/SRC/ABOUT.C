/************************************************************************
 *
 *     Project:  WINPRINT 2.0
 *
 *      Module:  about.c
 *
 *     Remarks:  About box and shareware stuff
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "..\inc\WINPRINT.h"
#include "..\inc\dlgs.h"
#include "..\inc\dlghelp.h"
#include "..\inc\about.h"
#include "..\inc\version.h"

#include <string.h>

BOOL CALLBACK fnAbout( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) ;

BOOL WINAPI DoAboutBox( HWND hwndApp )
{
   DLGPROC  lpfnDlgProc ;

   (FARPROC)lpfnDlgProc = MakeProcInstance( (FARPROC)fnAbout, hmodDLL ) ;
   DialogBox( hmodDLL,
              MAKEINTRESOURCE( DLG_ABOUTBOX ),
              hwndApp,
              lpfnDlgProc ) ;
   FreeProcInstance( (FARPROC)lpfnDlgProc ) ;

   return TRUE ;
}

BOOL CALLBACK  fnAbout( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam ) 
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         char szBuf[64] ;
         char szBuf2[64] ;
         GetDlgItemText( hDlg, IDD_VERSION, szBuf, 64 ) ;
         wsprintf( szBuf2, szBuf, VER_MAJOR, VER_MINOR, VER_BUILD ) ;
         SetDlgItemText( hDlg, IDD_VERSION, szBuf2 ) ;
      }
      break ;

      case WM_COMMAND:
         switch ((UINT)wParam)
         {
            case IDOK:
            case IDCANCEL:
               EndDialog (hDlg, 0) ;
            break ;

            default :
               return FALSE ;
         }
         break ;

      default:
         return FALSE ;

   }
   return TRUE ;
}



/************************************************************************
 * End of File: about.c
 ***********************************************************************/

