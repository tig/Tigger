/************************************************************************
 *
 *     Project:  WinPrint 1.4
 *
 *      Module:  page.c
 *
 *     Remarks:  Page Layout dialog box
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "PRECOMP.H"

#include "winprint.h"
#include "ws_dlg.h"
#include "dlgs.h"
#include "isz.h"
#include "page.h"
#include "text.h"
#include "dlghelp.h"
#include "..\help\map.h"


static UINT uiIDD ;

static BOOL fOrientChanged = FALSE ;

DWORD CALLBACK
   fnPageLayout( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

/****************************************************************
 *  BOOL WINAPI DoPageLayout( VOID )
 *
 *  Description: 
 *
 *    Hanldes the page layout dialog box.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI DoPageLayout( VOID )
{
   DLGPROC  lpfnPage ;
   BOOL     f ;

   lpfnPage = (DLGPROC)MakeProcInstance( (FARPROC)fnPageLayout, hInst ) ;

   uiIDD = NULL ;

   BackOptions() ;
   f = DialogBox( hInst, MAKEINTRESOURCE( DLG_LAYOUT ), hwndMain, lpfnPage) ;

   if (f)
   {
      f = !CmpOptions() ;

      if (fOrientChanged)
         f = TRUE ;
   }

   FreeProcInstance( (FARPROC)lpfnPage ) ;

   if (uiIDD)
      PostMessage( hwndMain, WM_COMMAND, uiIDD, 0L ) ;

   return f ;

} /* DoPageLayout()  */

DWORD CALLBACK fnPageLayout( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
   static DEVMODE FAR *  lpdm ;
   static BOOL    fSupportsOrient ;

   switch (uiMsg)
   {
      case WM_INITDIALOG:
      {
         char  szNum[32] ;

         tdShadowControl( hDlg, uiMsg, wParam, RGBLTGRAY ) ;

         fOrientChanged = FALSE ;

         /*
          * Fill edit controls
          */
         SetDlgItemInt( hDlg, IDD_NUMCOLS, Options.nColumns, FALSE ) ;

         if (Options.bInches)
            SetDlgItemText( hDlg, IDD_COLSPACE,
               DtoInch( szNum, (double)Options.nColSpace   / 1000)) ;
         else
            SetDlgItemText( hDlg, IDD_COLSPACE,
               DtoCM( szNum, (double)Options.nColSpace/ 254 ) ) ;

         /*
          * Check Radio buttons
          */
         if (Options.wSeparator == SEP_NONE)
            CheckDlgButton( hDlg, IDD_NOSEPARATOR, TRUE ) ;
         else
            if (Options.wSeparator == SEP_GRAPHIC)
               CheckDlgButton( hDlg, IDD_GRAPHSEPARATOR, TRUE ) ;
            else
               CheckDlgButton( hDlg, IDD_TEXTSEPARATOR, TRUE ) ;

         if (ghDevMode)
         {
            lpdm = (DEVMODE FAR *)GlobalLock( ghDevMode ) ;

            if (lpdm->dmFields & DM_ORIENTATION)
            {
               fSupportsOrient = TRUE ;

               if (lpdm->dmOrientation == DMORIENT_PORTRAIT)
               {
                  CheckDlgButton( hDlg, IDD_PORTRAIT, TRUE ) ;
                  if (fWin31)
                     SendDlgItemMessage( hDlg, IDD_ORIENT, STM_SETICON,
                                         (WPARAM)hicoPortrait, 0L ) ;
                  else
                     SetDlgItemText( hDlg, IDD_ORIENT,
                                  (LPSTR)MAKELONG( hicoPortrait, 0 ) ) ;
               }
               else
               {
                  CheckDlgButton( hDlg, IDD_LANDSCAPE, TRUE ) ;
                  if (fWin31)
                     SendDlgItemMessage( hDlg, IDD_ORIENT, STM_SETICON,
                                         (WPARAM)hicoLandscape, 0L ) ;
                  else
                     SetDlgItemText( hDlg, IDD_ORIENT,
                                  (LPSTR)MAKELONG( hicoLandscape, 0 ) ) ;
               }
            }
            else
               fSupportsOrient = FALSE ;
         }
         else
         {
            fSupportsOrient = FALSE ;
            lpdm = NULL ;
         }

         if (!fSupportsOrient)
         {
            EnableWindow( GetDlgItem( hDlg, IDD_PORTRAIT ), FALSE ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_LANDSCAPE ), FALSE ) ;

            if (fWin31)
               SendDlgItemMessage( hDlg, IDD_ORIENT, STM_SETICON,
                                   (WPARAM)hicoNoOrient, 0L ) ;
            else
               SetDlgItemText( hDlg, IDD_ORIENT,
                               (LPSTR)MAKELONG( hicoNoOrient, 0 ) ) ;
         }

         if (GetDlgItemInt( hDlg, IDD_NUMCOLS, NULL, FALSE ) < 2)
         {
            EnableWindow( GetDlgItem( hDlg, IDD_NOSEPARATOR ), FALSE ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_GRAPHSEPARATOR ), FALSE ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_TEXTSEPARATOR ), FALSE ) ;
         }
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

         switch( wID )
         {
            case IDD_PORTRAIT:
            case IDD_LANDSCAPE:
            {
               if (IsDlgButtonChecked( hDlg, IDD_PORTRAIT ))
               {
                  if (fWin31)
                     SendDlgItemMessage( hDlg, IDD_ORIENT, STM_SETICON,
                                       (WPARAM)hicoPortrait, 0L ) ;
                  else
                     SetDlgItemText( hDlg, IDD_ORIENT,
                                    (LPSTR)MAKELONG( hicoPortrait, 0 ) ) ;
               }
               else
               {
                  if (fWin31)
                     SendDlgItemMessage( hDlg, IDD_ORIENT, STM_SETICON,
                                       (WPARAM)hicoLandscape, 0L ) ;
                  else
                     SetDlgItemText( hDlg, IDD_ORIENT,
                                    (LPSTR)MAKELONG( hicoLandscape, 0 ) ) ;
               }

               fOrientChanged = TRUE ;

            }
            break ;

            case IDD_NUMCOLS:
               if (wNotifyCode == EN_CHANGE)
               {
                  if (GetDlgItemInt( hDlg, IDD_NUMCOLS, NULL, FALSE ) < 2)
                  {
                     EnableWindow( GetDlgItem( hDlg, IDD_NOSEPARATOR ),
                                   FALSE ) ;
                     EnableWindow( GetDlgItem( hDlg, IDD_GRAPHSEPARATOR ),
                                   FALSE ) ;
                     EnableWindow( GetDlgItem( hDlg, IDD_TEXTSEPARATOR ),
                                   FALSE ) ;
                     EnableWindow( GetDlgItem( hDlg, IDD_COLSPACE ),
                                   FALSE ) ;

                  }
                  else
                  {
                     EnableWindow( GetDlgItem( hDlg, IDD_NOSEPARATOR ),
                                   TRUE ) ;
                     EnableWindow( GetDlgItem( hDlg, IDD_GRAPHSEPARATOR ),
                                   TRUE ) ;
                     EnableWindow( GetDlgItem( hDlg, IDD_TEXTSEPARATOR ),
                                   TRUE  ) ;
                     EnableWindow( GetDlgItem( hDlg, IDD_COLSPACE ),
                                   TRUE ) ;
                  }
               }
            break ;

            case IDM_HEADFOOT:
            case IDM_OPTIONS:
               uiIDD = wID ;

               /* Fall through */

            case IDOK:
            {
               short    nCol ;
               short    nColSpace ;
               /*
                * Get and validate number of columns
                */
               if ((nCol = GetDlgItemInt( hDlg,
                                          IDD_NUMCOLS, NULL, FALSE )) < 0)
               {
                  MessageBeep( 0 ) ;
                  ErrorResBox( hDlg, hInst,
                                    MB_ICONEXCLAMATION,IDS_APPNAME, 
                                    IDS_ERR_BADNUMCOLS ) ;
                  SetFocus( GetDlgItem( hDlg, IDD_NUMCOLS ) ) ;
                  return TRUE ;
               }

               if ((nColSpace = GetDlgItemInt( hDlg,
                                          IDD_COLSPACE, NULL, FALSE )) < 0)
               {
                  MessageBeep( 0 ) ;
                  ErrorResBox( hDlg, hInst,
                                    MB_ICONEXCLAMATION,IDS_APPNAME, 
                                    IDS_ERR_BADCOLSPACE ) ;
                  if (nCol)
                     SetFocus( GetDlgItem( hDlg, IDD_COLSPACE ) ) ;
                  else
                     SetFocus( GetDlgItem( hDlg, IDD_NUMCOLS ) ) ;

                  return TRUE ;
               }

               Options.nColumns = nCol ;

               Options.nColSpace = (int)(GetDlgValue( hDlg,
                                                     IDD_COLSPACE,
                                                     Options.bInches ) * 1000) ;

               if (IsDlgButtonChecked( hDlg, IDD_NOSEPARATOR ))
                  Options.wSeparator = SEP_NONE ;
               else
                  if (IsDlgButtonChecked( hDlg, IDD_GRAPHSEPARATOR ))
                     Options.wSeparator = SEP_GRAPHIC ;
                  else
                     Options.wSeparator = SEP_TEXT ;


               if (lpdm && fSupportsOrient)
               {
                  if (IsDlgButtonChecked( hDlg, IDD_PORTRAIT ))
                     lpdm->dmOrientation = DMORIENT_PORTRAIT  ;
                  else
                     lpdm->dmOrientation = DMORIENT_LANDSCAPE  ;
               }
               
               EndDialog( hDlg, TRUE ) ;
            }
            return TRUE ;

            case IDCANCEL:
               EndDialog( hDlg, FALSE ) ;
            return TRUE ;

            case pshHelp:
               WinHelp( hwndMain, GRCS(IDS_HELPFILE), HELP_CONTEXT,
                        HLP_PAGELAYOUT ) ;
            break ;
         }
      }
      break ;

      case WM_DESTROY:
         tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;
         if (lpdm)
            GlobalUnlock( ghDevMode ) ;

      break ;
  
      default:
         return tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;

   }

   return FALSE ;
}


/************************************************************************
 * End of File: page.c
 ***********************************************************************/

