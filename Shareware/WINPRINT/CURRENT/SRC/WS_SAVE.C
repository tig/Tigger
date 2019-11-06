/*
   ws_save.c - Save config routines for WinPrint

   (c) Copyright 1990, Charles E. Kindel, Jr.

*/
#include "PRECOMP.H"

#include "winprint.h"
#include "isz.h"
#include "dlgs.h"
#include "ws_save.h"
#include "config.h"
#include "dlghelp.h"
#include "..\help\map.h"

BOOL CALLBACK fnSave(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

BOOL FAR PASCAL DoSaveConfig( VOID )
{
   DLGPROC  lpfnDlgProc ;
   BOOL     fOk ;

   lpfnDlgProc = (DLGPROC)MakeProcInstance ((FARPROC) fnSave, hInst) ;
   fOk = DialogBox( hInst, MAKEINTRESOURCE( DLG_SAVECONFIG ),
              hwndMain, lpfnDlgProc ) ;
   FreeProcInstance( (FARPROC)lpfnDlgProc ) ;

   return fOk ;
}

BOOL CALLBACK fnSave(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   PSTR        szText ;
   char        szName [CFG_NAME_LEN+1] ;
   char        szTemp [CFG_NAME_LEN+1] ;
   char        szConfigList [CFG_LIST_LEN+1] ;
   static HWND hList ;
   UINT        w ;

   switch (uiMsg)
   {
      case WM_INITDIALOG:
         tdShadowControl( hDlg, uiMsg, wParam, 0L ) ;
         hList = GetDlgItem (hDlg, IDD_CHOOSELIST) ;
         SendDlgItemMessage (hDlg, IDD_FNAME, EM_LIMITTEXT, CFG_NAME_LEN, 0L) ;

         if (lstrcmpi( szCurrentConfig, GRCS(IDS_FACTORYDEFAULTS)))
            SetDlgItemText( hDlg, IDD_FNAME, szCurrentConfig ) ;

         GetPrivateProfileString( GRCS(IDS_INI_CONFIGS), NULL, "",
                                  szConfigList, CFG_LIST_LEN + 1,
                                  GRCS(IDS_INI_FILENAME) ) ;

         for (szText = (PSTR) szConfigList ; *szText != '\0' ; szText += lstrlen(szText) + 1)
            SendMessage (hList, LB_ADDSTRING, 0, (LONG)(LPSTR) szText) ;

         SendMessage( hList, LB_SELECTSTRING, 0,
                      (LONG)(LPSTR) szCurrentConfig) ;

         if ((SendMessage( hList, LB_GETCOUNT, 0, 0L ) == 0))
            EnableWindow( GetDlgItem( hDlg, IDD_REMOVE ), FALSE ) ;

         if (!GetDlgItemText( hDlg, IDD_FNAME, szName, CFG_NAME_LEN+1 ))
            EnableWindow( GetDlgItem( hDlg, IDOK ), FALSE ) ;

         CheckDlgButton( hDlg, IDD_SAVEPRINTER, fSavePrinter ) ;
         return TRUE ;

      case WM_COMMAND:
      {
         #ifdef WIN32
         WORD wNotifyCode = HIWORD(wParam);  
         WORD wID = LOWORD(wParam);         
         HWND hwndCtl = (HWND) lParam;      
         #else
         WORD wNotifyCode = HIWORD(lParam) ;
         WORD wID = wParam ;
         HWND hwndCtl = (HWND)LOWORD(lParam) ;
         #endif

         switch (wID)
         {
            case IDD_CHOOSELIST:
               switch (wNotifyCode)
               {
                  case LBN_SELCHANGE:
                     w = (UINT) SendMessage( hList, LB_GETCURSEL, 0, 0L) ;

                     if (w != LB_ERR)
                     {   
                        SendMessage( hList, LB_GETTEXT, w,
                                     (LONG)(LPSTR) szName) ;

                        SetDlgItemText (hDlg, IDD_FNAME, szName) ;
                        EnableWindow (GetDlgItem (hDlg, IDOK), TRUE) ;
                        EnableWindow( GetDlgItem( hDlg, IDD_REMOVE), TRUE ) ;
                     }
                     else
                     {
                        EnableWindow( GetDlgItem( hDlg, IDD_REMOVE), FALSE ) ;
                     }

                     if (!GetDlgItemText( hDlg, IDD_FNAME, szName, CFG_NAME_LEN+1 ))
                        EnableWindow( GetDlgItem (hDlg, IDOK), FALSE ) ;
                     else
                        EnableWindow( GetDlgItem (hDlg, IDOK), TRUE ) ;

                     break ;

                  case LBN_DBLCLK:
                     goto save ;
                     break ;
               }
               break ;

            case IDD_FNAME:
               if (wNotifyCode == EN_CHANGE)
               {
                  GetDlgItemText( hDlg, IDD_FNAME, szTemp, CFG_NAME_LEN ) ;

                  w = (UINT)SendMessage( hList, LB_FINDSTRING, (WPARAM)(UINT)-1,
                                         (LONG)(LPSTR)szTemp ) ;

                  SendMessage (hList, LB_SETCURSEL, w, 0L) ;

                  if ((BOOL)lstrlen( szTemp ) &&
                      (BOOL)!isspace( szTemp[0] ))
                     EnableWindow( GetDlgItem( hDlg, IDOK ), TRUE ) ;
                  else
                     EnableWindow( GetDlgItem (hDlg, IDOK), FALSE ) ;
               }
               break ;

            case IDOK:
save:          GetDlgItemText (hDlg, IDD_FNAME, szName, CFG_NAME_LEN+1) ;
               if (!lstrcmpi (szName, GRCS(IDS_FACTORYDEFAULTS)))
               {
                  ErrorResBox( hDlg, hInst, MB_ICONQUESTION | MB_YESNOCANCEL,
                               IDS_APPNAME, IDS_ERR_SAVEOVEROLD ) ;
                  break ;
               }

               lstrcpy( szCurrentConfig, szName ) ;

               fSavePrinter = IsDlgButtonChecked( hDlg, IDD_SAVEPRINTER ) ;
               WriteDefaults( szCurrentConfig ) ;

               WritePrivateProfileString( GRCS(IDS_INI_MAIN),
                                          GRCS(IDS_INI_MAIN_CURCFG),
                                          szCurrentConfig,
                                          GRCS(IDS_INI_FILENAME) ) ;
               EndDialog (hDlg, TRUE) ;
            break ;

            case IDD_REMOVE:
               w = (UINT)SendMessage( hList, LB_GETCURSEL, 0, 0L ) ;

               if (LB_ERR == (short)SendMessage( hList, LB_GETTEXT, w,
                                                (LONG)(LPSTR)szName ))
               {
                  MessageBeep (0) ;
                  break ;
               }

               if (IDYES == ErrorResBox( hDlg, hInst,
                                    MB_YESNO | MB_ICONQUESTION,
                                    IDS_APPNAME, IDS_ERR_CONFIRMDELETE,
                                    (LPSTR)szName ))
               {
                  /*
                     * Get rid of the tag in the [Configs] section
                     */
                  WritePrivateProfileString( GRCS(IDS_INI_CONFIGS),
                                             szName, NULL,
                                             GRCS(IDS_INI_FILENAME) ) ;
                  /*
                     * Get rid of the [szName] section.
                     */
                  WritePrivateProfileString( szName,NULL, NULL,
                                             GRCS(IDS_INI_FILENAME) ) ;

                  SendMessage( hList, LB_DELETESTRING, w, 0L ) ;
                  SendMessage( hList, LB_SETCURSEL, w-1, 0L ) ;

                  w = (UINT) SendMessage( hList, LB_GETCURSEL, 0, 0L) ;
                  if (w == LB_ERR)
                     EnableWindow( GetDlgItem( hDlg, IDD_REMOVE), FALSE ) ;

               }
            break ;

            case IDCANCEL:
               EndDialog (hDlg, FALSE) ;
               break ;

            case pshHelp:
               WinHelp( hwndMain, GRCS(IDS_HELPFILE), HELP_CONTEXT,
                        HLP_SAVE ) ;
            break ;

            default:
               return FALSE ;
         }
      }
      break ;

      default:
         return tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;
   }
   return TRUE ;
}


BOOL WriteDefaults (PSTR szConfigName)
{
   return SaveConfig( szConfigName ) ;
}

#if 0
BOOL WriteDefaults (PSTR szConfigName)
{
char  szString [CFG_LIST_LEN + 1] ;
char  szTemp [32],
      szName [32] ;

   WriteProfileString (GRCS(IDS_APPNAME), GRCS(IDS_OLDINI_CURCFG), szCurrentConfig) ;

   wsprintf( (LPSTR)szName, (LPSTR)"%s-Configurations", (LPSTR)GRCS(IDS_APPNAME)) ;

   wsprintf ((LPSTR)szString, (LPSTR)"%i|", Options.bMinimize) ;

   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.bClose) ;
   lstrcat (szString, szTemp) ;

   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.bDraftMode) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.wHeaderMask) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.wFooterMask) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.wHFStyle) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.bHFMargins) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%s|", (LPSTR)Options.szHFFont) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.nHFPtSize) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.bDuplex) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.bExpandTabs) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.nTabSize) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%s|", (LPSTR)(Options.bInches ? "inches" : "cm")) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.nTopMargin) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.nLeftMargin) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.nBottomMargin) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.nRightMargin) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%s|", (LPSTR)Options.szFont) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.nPtSize) ;
   lstrcat (szString, szTemp) ;
   
   wsprintf ((LPSTR)szTemp, (LPSTR)"%i|", Options.wStyle) ;
   lstrcat (szString, szTemp) ;

//   lstrcat (szString, szCurrentPrinter) ;
   
   WriteProfileString (szName, szConfigName, szString) ;

   return TRUE ;
}

#endif


