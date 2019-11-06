/*
   ws_open.c - Save config routines for WinPrint

   (c) Copyright 1990, Charles E. Kindel, Jr.

*/
#include "PRECOMP.H"

#include "winprint.h"
#include "isz.h"
#include "dlgs.h"
#include "ws_open.h"
#include "ws_lib.h"
#include "config.h"
#include "dlghelp.h"
#include "..\help\map.h"

BOOL CALLBACK fnOpen(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) ;

BOOL WINAPI DoOpenConfig( VOID )
{
   DLGPROC lpfnDlgProc ;
   BOOL     fModify ;

   lpfnDlgProc = (DLGPROC)MakeProcInstance ((FARPROC) fnOpen, hInst) ;
   fModify = DialogBox( hInst, MAKEINTRESOURCE( DLG_OPENCONFIG ),
               hwndMain, lpfnDlgProc) ;
   FreeProcInstance( (FARPROC)lpfnDlgProc ) ;

   return fModify ;
}

BOOL CALLBACK fnOpen(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   PSTR        szText ;
   char        szName [CFG_NAME_LEN+1] ;
   char        szConfigList [CFG_LIST_LEN+1] ;
   static HWND hList ;
   UINT  w ;

   switch (uiMsg)
   {
      case WM_INITDIALOG:
         tdShadowControl( hDlg, uiMsg, wParam, 0L ) ;

         hList = GetDlgItem (hDlg, IDD_CHOOSELIST) ;

         GetPrivateProfileString( GRCS(IDS_INI_CONFIGS), NULL,
                                  "", szConfigList, CFG_LIST_LEN+1,
                                  GRCS(IDS_INI_FILENAME) ) ;

         for (szText = (PSTR)szConfigList ; *szText != '\0' ; szText += lstrlen(szText) + 1)
            SendMessage (hList, LB_ADDSTRING, 0, (LONG)(LPSTR) szText) ;

         /* put default one in top */
         SendMessage( hList, LB_INSERTSTRING,
                      0, (LONG)(LPSTR)GRCS(IDS_FACTORYDEFAULTS) ) ;
         SendMessage( hList, LB_SELECTSTRING,
                      0, (LONG)(LPSTR)szCurrentConfig) ;

         if ((SendMessage( hList, LB_GETCOUNT, 0, 0L ) < 2)  ||
             (SendMessage( hList, LB_GETCURSEL, 0, 0L ) < 1))
         {
            EnableWindow( GetDlgItem( hDlg, IDD_REMOVE), FALSE ) ;
         }
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
               if (wNotifyCode == LBN_SELCHANGE)
               {
                  w = (UINT)SendMessage( hList, LB_GETCURSEL, 0, 0L ) ;
                  EnableWindow( GetDlgItem( hDlg, IDD_REMOVE ),
                                (short)w >= 1 ) ;
               }

               if (wNotifyCode != LBN_DBLCLK)
                  break ;
               /* fall through */

            case IDOK:         
               if (LB_ERR == (UINT) SendMessage (hList, LB_GETTEXT,
                            (UINT) SendMessage (hList, LB_GETCURSEL, 0, 0L),
                            (LONG) (LPSTR) szName))
               {
                  MessageBeep (0) ;
                  break ;
               }
               ReadDefaults (szName) ;
               lstrcpy (szCurrentConfig, szName) ;
               WritePrivateProfileString( GRCS(IDS_INI_MAIN),
                                          GRCS(IDS_INI_MAIN_CURCFG),
                                          szCurrentConfig,
                                          GRCS(IDS_INI_FILENAME) ) ;
               EndDialog (hDlg, TRUE) ;
               break ;

            case IDCANCEL:
               EndDialog (hDlg, FALSE) ;
               break ;

            case IDD_REMOVE:
               
               w = (UINT)SendMessage( hList, LB_GETCURSEL, 0, 0L ) ;

               if (LB_ERR == (UINT)SendMessage( hList, LB_GETTEXT, w,
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

                  w = (UINT)SendMessage( hList, LB_GETCURSEL, 0, 0L ) ;
                  if (w < 1)
                     EnableWindow( GetDlgItem( hDlg, IDD_REMOVE), FALSE ) ;

               }
            break ;

            case pshHelp:
               WinHelp( hwndMain, GRCS(IDS_HELPFILE), HELP_CONTEXT,
                        HLP_OPEN ) ;
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

BOOL ReadDefaults (PSTR szConfigName)
{
   if (!lstrcmpi (szConfigName, GRCS(IDS_FACTORYDEFAULTS)))
      return GetDefaultConfig( ) ;
   else
      return GetConfig( szConfigName ) ;
}

#if 0
BOOL ReadDefaults (PSTR szConfigName)
{
   char  szName [32],
         szConfigList [CFG_LIST_LEN+1] ;
   PSTR  szString ;

   bModify = FALSE ;

   if (!lstrcmpi (szConfigName, GRCS(IDS_FACTORYDEFAULTS)))
   {
      DEVNAMES FAR * lpdn ;
      PRINTDLG       pd ;

      memset( &pd, '\0', sizeof( PRINTDLG ) ) ;
      pd.lStructSize = sizeof( PRINTDLG ) ;
      pd.hwndOwner =  hwndMain ;
      pd.hDevMode = ghDevMode ;
      pd.hDevNames = ghDevNames ;

      /*
       * Use common dialogs to get the default printer
       */
      pd.Flags = PD_RETURNDEFAULT ;

      if (PrintDlg( &pd ) != 0)
      {
         /*
          * Set our internal vars to match those returned...
          */
         ghDevMode = pd.hDevMode ;
         ghDevNames = pd.hDevNames ;
      }
      else
         return FALSE ;

      Options.bMinimize = FALSE ;
      Options.bClose = FALSE ;
      Options.bDraftMode = TRUE ;
      Options.wHeaderMask = HF_NONE ;
      Options.wFooterMask = HF_NONE ;
      Options.wHFStyle = HF_NONE ;
      Options.bHFMargins = TRUE ;
      lstrcpy (Options.szHFFont, DEFFONT) ;
      Options.nHFPtSize = 10 ;
      Options.bDuplex = FALSE ;
      Options.bExpandTabs = TRUE ;
      Options.nTabSize = 5 ;
      Options.bInches = TRUE ;
      Options.nTopMargin = 0 ;
      Options.nLeftMargin = 0 ;
      Options.nBottomMargin = 0 ;
      Options.nRightMargin = 0 ;
      lstrcpy (Options.szFont, DEFFONT) ;
      Options.nPtSize = 10 ;
      Options.wStyle = HF_NONE ;
      return TRUE ;
   }

   GetProfileString( GRCS(IDS_OLDINI_CONFIGURATIONS),
                     szConfigName, "",
                     szConfigList, CFG_LIST_LEN + 1) ;

   if (szConfigList[0] == '\0')
      return FALSE ;

   szString = strtok (szConfigList, "|") ;
   Options.bMinimize = (BOOL) atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.bClose = (BOOL) atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.bDraftMode = (BOOL) atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.wHeaderMask = atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.wFooterMask = atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.wHFStyle = atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.bHFMargins = (BOOL) atoi (szString) ;

   szString = strtok (NULL, "|") ;
   lstrcpy (Options.szHFFont, szString) ;

   szString = strtok (NULL, "|") ;
   Options.nHFPtSize = atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.bDuplex = (BOOL) atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.bExpandTabs = (BOOL) atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.nTabSize = atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.bInches = (BOOL) (lstrcmpi (szString, GRCS(IDS_CENTIMETERS))) ;  /* stricmp will return 0 if szString == "cm" */
   
   szString = strtok (NULL, "|") ;
   Options.nTopMargin = atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.nLeftMargin = atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.nBottomMargin = atoi (szString) ;
   
   szString = strtok (NULL, "|") ;
   Options.nRightMargin = atoi (szString) ;

   szString = strtok (NULL, "|") ;
   lstrcpy (Options.szFont, szString) ;

   szString = strtok (NULL, "|") ;
   Options.nPtSize = atoi (szString) ;

   szString = strtok (NULL, "|") ;
   Options.wStyle = atoi (szString) ;

   szString = strtok (NULL, "|") ;
//   lstrcpy (szCurrentPrinter, szString) ;

   return TRUE ;
}


BOOL CheckConfigList (PSTR szList, PSTR szName)
{
   PSTR szText ;

   for (szText = (PSTR) szList ; *szText != '\0' && lstrcmpi (szText, szName) ;  szText += lstrlen (szText) + 1)
      ;
   return (BOOL) *szText ;        
}

#endif
