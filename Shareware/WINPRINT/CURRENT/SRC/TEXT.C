/************************************************************************
 *
 *     Project:  WinPrint 1.4
 *
 *      Module:  text.c
 *
 *     Remarks:  This module supports the "Text Options dialog box"
 *             The dialog box itself is a COMMDLG dialog (choose font)
 *             the function here (fnTextOption) is a hook function.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "PRECOMP.H"

#include "winprint.h"
#include "dlgs.h"
#include "ws_lib.h"
#include "isz.h"
#include "dlghelp.h"
#include "text.h"
#include "..\help\map.h"

static UINT uiIDD ;

DWORD CALLBACK PASCAL fnTextOptions( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;


BOOL FAR PASCAL DoTextOptions( VOID )
{
   CHOOSEFONT     cf ;
   BOOL           fModify = FALSE ;

   memset( &cf, '\0', sizeof( CHOOSEFONT ) ) ;

   cf.lStructSize = sizeof( CHOOSEFONT ) ;
   cf.hwndOwner = hwndMain ;
   cf.hDC = GetPrinterDC() ;
   cf.lpLogFont = &Options.lfCurFont ;
   cf.iPointSize = 0 ;
   cf.Flags = CF_INITTOLOGFONTSTRUCT |
               CF_ANSIONLY |
               CF_BOTH |
               CF_ENABLEHOOK |
               CF_ENABLETEMPLATE |
               CF_SHOWHELP |
               CF_FORCEFONTEXIST ;

   if (fFixedPitchOnly)
      cf.Flags |= CF_FIXEDPITCHONLY ;

   cf.lpfnHook = (LPFNHOOKPROC)MakeProcInstance( (FARPROC)fnTextOptions, hInst ) ;
   cf.lpTemplateName = MAKEINTRESOURCE( DLG_TEXTOPTIONS ) ;
   cf.hInstance = hInst ;

   if (cf.hDC)
      PtoS( cf.hDC, &Options.lfCurFont.lfHeight ) ;

   BackOptions() ;
   uiIDD = NULL ;
   if (ChooseFont( &cf ))
      fModify = !CmpOptions() ;

   if (cf.hDC)
   {
      StoP( cf.hDC, &Options.lfCurFont.lfHeight ) ;
      DeleteDC( cf.hDC ) ;
   }
   FreeProcInstance( (FARPROC)cf.lpfnHook ) ;

   if (uiIDD)
      PostMessage( hwndMain, WM_COMMAND, uiIDD, 0L ) ;

   DP( hWDB, "lfHeight = %d, lfWidth = %d", Options.lfCurFont.lfHeight,
      Options.lfCurFont.lfWidth ) ;

   return fModify ;
}


DWORD CALLBACK PASCAL fnTextOptions( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) 
{
   char           szNum [16] ;
   double         d ;
   int            i ;
   BOOL           bError ;
   static BOOL    bInches ;

   switch (uiMsg)
   {
      case WM_INITDIALOG:
         DP( hWDB, "WM_INITDIALOG" ) ;

         tdShadowControl( hDlg, uiMsg, wParam, RGBLTGRAY ) ;

         /* Tabs */
         CheckDlgButton( hDlg, IDD_TABS, Options.bExpandTabs) ;
         SetDlgItemInt( hDlg, IDD_TABSIZE, Options.nTabSize, FALSE) ;

         EnableWindow( GetDlgItem( hDlg, IDD_TABSIZE ),
                       IsDlgButtonChecked( hDlg, IDD_TABS ) ) ;
         /* Margins */
         if (bInches = Options.bInches)
         {
            CheckRadioButton( hDlg, IDD_INCH, IDD_CM, IDD_INCH ) ;
            SetDlgItemText( hDlg, IDD_LEFTMAR,
               DtoInch( szNum, (double)Options.nLeftMargin   / 1000)) ;
            SetDlgItemText( hDlg, IDD_RIGHTMAR,
               DtoInch( szNum, (double)Options.nRightMargin  / 1000)) ;
            SetDlgItemText( hDlg, IDD_BOTMAR,
               DtoInch( szNum, (double)Options.nBottomMargin / 1000)) ;
            SetDlgItemText( hDlg, IDD_TOPMAR,
               DtoInch( szNum, (double)Options.nTopMargin    / 1000)) ;
         }
         else
         {
            CheckRadioButton( hDlg, IDD_INCH, IDD_CM, IDD_CM ) ;
            SetDlgItemText( hDlg, IDD_LEFTMAR,
               DtoCM( szNum, (double)Options.nLeftMargin / 254 ) ) ;
            SetDlgItemText( hDlg, IDD_RIGHTMAR,
               DtoCM( szNum, (double)Options.nRightMargin / 254 ) ) ;
            SetDlgItemText( hDlg, IDD_BOTMAR,
               DtoCM( szNum, (double)Options.nBottomMargin/ 254 ) ) ;
            SetDlgItemText( hDlg, IDD_TOPMAR,
               DtoCM( szNum, (double)Options.nTopMargin / 254 ) ) ;
         }

         /*
          * Pass it on to the choose font func..
          */
      return FALSE ;

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
            /* Margins */
            case IDD_CM:
            case IDD_INCH:
               /*
                  * If one of the radio buttons was clicked and it is
                  * not what was clicked before then process
                  */
               if ((wNotifyCode == BN_CLICKED) &&
                     (bInches != (BOOL)IsDlgButtonChecked( hDlg, IDD_INCH )))
               {
                  char     szNum [16] ;
                  double   d ;

                  DP( hWDB, "Clicked" ) ;

                  for (i = IDD_LEFTMAR ; i <= IDD_BOTMAR ; i++)
                  {
                     GetDlgItemText (hDlg, i, szNum, sizeof szNum) ;
                     d = atof( szNum ) ;

                     /*
                        * If we were inches before, then convert to
                        * CM, otherwise do otherwise...
                        */
                     if (bInches)
                     {
                        d /= 0.254 ;
                        SetDlgItemText( hDlg, i, DtoCM( szNum, d ) ) ;
                     }
                     else
                     {
                        d *= 0.254 ;
                        SetDlgItemText( hDlg, i, DtoInch( szNum, d ) ) ;
                     }
                  }
                  bInches = IsDlgButtonChecked( hDlg, IDD_INCH ) ;
               }
            return TRUE ;

            case IDD_TABS:
               EnableWindow( GetDlgItem( hDlg, IDD_TABSIZE ),
                  IsDlgButtonChecked( hDlg, IDD_TABS ) ) ;
            return TRUE ;

            case IDM_HEADFOOT:
            case IDM_PAGELAYOUT:
               uiIDD = wID ;
               #ifdef WIN32
               SendMessage( hDlg, WM_COMMAND, MAKELPARAM( IDOK, BN_CLICKED ),
                            (LPARAM)hwndCtl ) ;
               #else
               SendMessage( hDlg, WM_COMMAND, IDOK, 
                  MAKELONG( BN_CLICKED, hwndCtl ) );
               #endif
            break ;


            /* Ok */
            case IDOK:
               bInches = IsDlgButtonChecked( hDlg, IDD_INCH ) ;
               /* Margin check */
               for( i = IDD_LEFTMAR ; i <= IDD_BOTMAR ; i++)
                  if( bError = (0.0 > (d = GetDlgValue( hDlg, i, bInches ))))
                  {
                     /*
                        * Can't have margins less than 0
                        */
                     MessageBeep( 0 ) ;
                     ErrorResBox( hDlg, hInst,
                                    MB_ICONEXCLAMATION,IDS_APPNAME, 
                                    IDS_ERR_BADMARGINS ) ;
                     SetFocus( GetDlgItem( hDlg, i ) ) ;
                     return TRUE ;
                  }


               if (IsDlgButtonChecked( hDlg, IDD_TABS ) &&
                     (short)GetDlgItemInt( hDlg, IDD_TABSIZE, NULL, FALSE ) < 0)
               {
                  MessageBeep( 0 ) ;
                  ErrorResBox( hDlg, hInst,
                                    MB_ICONEXCLAMATION,IDS_APPNAME, 
                                    IDS_ERR_BADMARGINS ) ;
                  SetFocus( GetDlgItem( hDlg, IDD_TABSIZE ) ) ;
                  return TRUE ;
               }

               /* Margins */
               Options.nTopMargin    = (int)(GetDlgValue( hDlg, IDD_TOPMAR, bInches )   * 1000) ;
               Options.nBottomMargin = (int)(GetDlgValue( hDlg, IDD_BOTMAR, bInches )   * 1000) ;
               Options.nLeftMargin   = (int)(GetDlgValue( hDlg, IDD_LEFTMAR, bInches )  * 1000) ;
               Options.nRightMargin  = (int)(GetDlgValue( hDlg, IDD_RIGHTMAR, bInches ) * 1000) ;

               /* Tabs */
               Options.bExpandTabs = IsDlgButtonChecked( hDlg, IDD_TABS ) ;
               Options.nTabSize    = GetDlgItemInt( hDlg, IDD_TABSIZE, NULL, FALSE ) ;

               Options.bInches = IsDlgButtonChecked( hDlg, IDD_INCH ) ;
            return FALSE ;

            case pshHelp:
               WinHelp( hwndMain, GRCS(IDS_HELPFILE), HELP_CONTEXT, HLP_TEXT ) ;
            break ;

         }
      }
      break;

      default:
         return tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;

   }

   return FALSE ;
}

double FAR PASCAL GetDlgValue( HWND hDlg, UINT iID, BOOL bInches )
{
   char szNum [16] ;

   GetDlgItemText( hDlg, iID, szNum, sizeof szNum ) ;
   return ((double)atof( szNum ) * (bInches ? 1.0 : 0.254)) ;
}

#define PRECISION 5
PSTR FAR PASCAL DtoInch( PSTR szInch, double dValue )
{
   char szNum[32] ;
   short n ;

   gcvt( dValue, PRECISION, szNum ) ;

   n = lstrlen( szNum ) ;
   if (szNum[n-1] == '.')
   {
      szNum[n] = '0' ;
      szNum[n+1] = '\0' ;
   }
   wsprintf( szInch, "%s%s", (LPSTR)szNum, (LPSTR)GRCS(IDS_DOUBLEQUOTE) ) ;
   return szInch ;
}

PSTR FAR PASCAL DtoCM( PSTR szCM, double dValue )
{                                                       \
   char szNum[32] ;
   short n ;

   gcvt( dValue, PRECISION, szNum ) ;

   n = lstrlen( szNum ) ;
   if (szNum[n-1] == '.')
   {
      szNum[n] = '0' ;
      szNum[n+1] = '\0' ;
   }
   wsprintf( szCM, "%s%s", (LPSTR)szNum, (LPSTR)GRCS(IDS_CENTIMETERS) ) ;
   return szCM ;
}


/************************************************************************
 * End of File: text.c
 ***********************************************************************/

