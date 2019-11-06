#include "PRECOMP.H"

#include "isz.h"
#include "dlgs.h"
#include "ws_dlg.h"
#include "winprint.h"
#include "ws_init.h"
#include "ws_lib.h"
#include "dlghelp.h"
#include "ws_print.h"
#include "ws_reg.h"
#include "..\help\map.h"

BOOL NEAR PASCAL PrintRegForm( VOID ) ;
BOOL CALLBACK fnAbout( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) ;
BOOL CALLBACK fnShareware( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) ;

BOOL WINAPI DoAboutBox( VOID )
{
   DLGPROC  lpfnDlgProc ;

   lpfnDlgProc = (DLGPROC)MakeProcInstance( (FARPROC)fnAbout, hInst) ;
   DialogBox (hInst, MAKEINTRESOURCE( DLG_ABOUTBOX ), hwndMain, lpfnDlgProc) ;
   FreeProcInstance( (FARPROC)lpfnDlgProc ) ;

   return TRUE ;
}

BOOL WINAPI DoSharewareBox( VOID )
{
   DLGPROC  lpfnDlgProc ;

   lpfnDlgProc = (DLGPROC)MakeProcInstance( (FARPROC)fnShareware, hInst) ;
   DialogBox (hInst, MAKEINTRESOURCE( DLG_SHAREWARE ), hwndMain, lpfnDlgProc) ;
   FreeProcInstance( (FARPROC)lpfnDlgProc ) ;

   return TRUE ;
}

BOOL CALLBACK fnAbout (HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) 
{
   char  szTemp [64] ;

   switch (uiMsg)
   {
      case WM_INITDIALOG:

         SendDlgItemMessage( hDlg, IDD_USERNAME, WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;
#ifdef SHOWREGNUM
         SendDlgItemMessage( hDlg, IDD_REGNUMBER, WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;
#endif
         wsprintf( szTemp, "%s %s - %s",
                   (LPSTR)GRCS(IDS_VERSION), (LPSTR)szVerNum,
                   (LPSTR)__DATE__ ) ;

         SetDlgItemText( hDlg, IDD_VERSION, szTemp ) ;

         wsprintf( szTemp, " %s", (LPSTR)szRegisteredName ) ;
         SetDlgItemText (hDlg, IDD_USERNAME, szTemp ) ;

#ifdef SHOWREGNUM
         if (lstrcmpi( szRegisteredName, GRCS(IDS_PLEASEPAY) ))
         {
            GetPrivateProfileString( GRCS(IDS_INI_MAIN),
                                     GRCS(IDS_INI_MAIN_CODE),
                                     "", szNum, REG_NUM_LEN,
                                     GRCS(IDS_INI_FILENAME)) ;
            wsprintf( szTemp, " %s", (LPSTR)szNum ) ;
            SetDlgItemText (hDlg, IDD_REGNUMBER, szTemp) ;
         }
#endif
         return tdShadowControl( hDlg, uiMsg, wParam, 0L ) ;
         break ;

      case WM_COMMAND:
         switch (LOWORD( wParam ))
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
         return tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;
   }
   return TRUE ;
}

BOOL CALLBACK fnShareware (HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) 
{
   char  szName [REG_NAME_LEN+1] ;
   char  szNum [REG_NAME_LEN+1] ;

   switch (uiMsg)
   {
      case WM_INITDIALOG:
            SendMessage (GetDlgItem (hDlg, IDD_NAME), EM_LIMITTEXT, REG_NAME_LEN, 0L) ;
            SetDlgItemText (hDlg, IDD_NAME, szRegisteredName) ;
            SendMessage (GetDlgItem (hDlg, IDD_REGNUMBER), EM_LIMITTEXT, REG_NUM_LEN, 0L) ;
      return tdShadowControl( hDlg, uiMsg, wParam, 0L ) ;

      case WM_COMMAND:
         switch (LOWORD( wParam ))
         {
            case IDD_CLOSE:
               MessageBeep( MB_ICONHAND ) ;
               MessageBeep( MB_ICONQUESTION ) ;
               EndDialog (hDlg, 0) ;
               break ;

            case IDOK :
               GetDlgItemText (hDlg, IDD_NAME, szName, REG_NAME_LEN+1) ;
               GetDlgItemText (hDlg, IDD_REGNUMBER, szNum, REG_NUM_LEN+1) ;
               if (!VerifyRegistration (szName, szNum))
               {
                  ErrorResBox( hDlg, hInst, MB_ICONEXCLAMATION,
                               IDS_APPNAME, IDS_ERR_BADREGISTRATION ) ;
                  SetFocus (GetDlgItem (hDlg, IDD_NAME)) ;
               }
               else
               {
                  WritePrivateProfileString(
                                       GRCS(IDS_INI_MAIN), 
                                       GRCS(IDS_INI_MAIN_NAME),
                                       szName,
                                       GRCS(IDS_INI_FILENAME)) ;

                  WritePrivateProfileString(
                                       GRCS(IDS_INI_MAIN),
                                       GRCS(IDS_INI_MAIN_CODE),
                                       AnsiUpper( szNum ),
                                       GRCS(IDS_INI_FILENAME)) ;

                  lstrcpy (szRegisteredName, szName) ;
                  EndDialog (hDlg, 1) ;
               }
               break ;

            case IDD_PRINTREGFORM:
               PrintRegForm() ;
            break ;

            case pshHelp:
               WinHelp( hwndMain, GRCS(IDS_HELPFILE), HELP_CONTEXT,
                        HLP_SHAREWARE ) ;
            break ;


            default :
               return FALSE ;
         }
         break ;

      default:
         return tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;
   }
   return TRUE ;
}


BOOL NEAR PASCAL PrintRegForm( VOID )
{
   LPSTR          lpText ;
   LPSTR          lpCur ;
   char           szFile[144] ;
   HANDLE         hTemp ;
   HANDLE         hRes ;
   int            hfTempFile ;
   int            n ;

   StartPrintUI() ;

   if (!(hTemp = FindResource( hInst,
         MAKEINTRESOURCE(ID_RCDATA_ORDER_FRM), "TEXTFILE" )))
   {
      DASSERTMSG( hWDB, 0, "Cannot read resource to print order form!" ) ;
      EndPrintUI() ;
      return FALSE ;
   }

   if ((hRes = LoadResource( hInst, hTemp )) &&
       (lpText = LockResource( hRes )))
   {
      #ifdef WIN32
      char szTemp[256] ;
      GetTempPath( 256, szTemp ) ;
      GetTempFileName( szTemp, "wpr", 0, (LPSTR)szFile ) ;
      #else
      GetTempFileName( GetTempDrive( 0 ), "wpr", 0, (LPSTR)szFile ) ;
      #endif

      DP3( hWDB, "Temp file == %s", (LPSTR)szFile ) ;

      hfTempFile = _lcreat((LPSTR)szFile, 0 ) ;

      if (hfTempFile == -1)
      {
         DP1( hWDB, "Temp file == %s", (LPSTR)szFile ) ;
         DASSERTMSG( hWDB, 0, "Could not create temp file!" ) ;

         UnlockResource( hRes ) ;
         FreeResource( hRes ) ;
         EndPrintUI() ;
         return FALSE ;
      }

      /*
       * find out how big it is by searching for the ^Z
       */
      lpCur = lpText ;
      n = 0 ;
      while( *lpCur++ != '\x1A' )
         n++ ;

      _lwrite( hfTempFile, (LPSTR)lpText, n ) ;

      _lclose( hfTempFile ) ;


      UnlockResource( hRes ) ;
      FreeResource( hRes ) ;

      /*
       * now print
       */
      PrintFiles( hwndMain, NULL, NULL, szFile ) ;
   }

   /*
    * Delete the file
    */

   EndPrintUI() ;
   return TRUE ;
}


