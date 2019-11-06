/************************************************************************
 *
 *     Project:  WinPrint 1.4
 *
 *      Module:  headfoot.c
 *
 *     Remarks:  This module supports the "Header/Footer Options dialog box"
 *             The dialog box itself is a COMMDLG dialog (choose font)
 *             the function here (fnHFOption) is a hook function.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "PRECOMP.H"

#include "winprint.h"
#include "isz.h"
#include "dlgs.h"
#include "ws_lib.h"
#include "..\help\map.h"

static UINT uiIDD ;

LRESULT CALLBACK fnHFOptions( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

BOOL FAR PASCAL DoHeadFootOptions( VOID )
{
   CHOOSEFONT     cf ;
   BOOL           fModify = FALSE ;

   memset( &cf, '\0', sizeof( CHOOSEFONT ) ) ;

   cf.lStructSize = sizeof( CHOOSEFONT ) ;
   cf.hwndOwner = hwndMain ;
   cf.hDC = GetPrinterDC() ;
   cf.lpLogFont = &Options.lfHFFont ;
   cf.iPointSize = 0 ;
   cf.Flags = CF_INITTOLOGFONTSTRUCT |
               CF_ANSIONLY |
               CF_BOTH |
               CF_ENABLEHOOK |
               CF_ENABLETEMPLATE |
               CF_SHOWHELP |
               CF_FORCEFONTEXIST ;

   cf.lpfnHook = (LPFNHOOKPROC)MakeProcInstance( (FARPROC)fnHFOptions, hInst ) ;
   cf.lpTemplateName = MAKEINTRESOURCE( DLG_HFOPTIONS ) ;
   cf.hInstance = hInst ;

   if (cf.hDC)
      PtoS( cf.hDC, &Options.lfHFFont.lfHeight ) ;

   uiIDD = 0 ;
   
   BackOptions() ;
   if (ChooseFont( &cf ))
      fModify = !CmpOptions() ;

   if (cf.hDC)
   {
      StoP( cf.hDC, &Options.lfHFFont.lfHeight ) ;
      DeleteDC( cf.hDC ) ;
   }

   FreeProcInstance( (FARPROC)cf.lpfnHook ) ;

   if (uiIDD)
      PostMessage( hwndMain, WM_COMMAND, uiIDD, 0L ) ;

   return fModify ;
}


LRESULT CALLBACK fnHFOptions( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
   switch (uiMsg)
   {
      case WM_INITDIALOG:
         /* Headers/Footers */

         CheckDlgButton( hDlg, IDD_HFMARGINS, Options.bHFMargins ) ;

         CheckDlgButton( hDlg, IDD_HFNAME,
                         (BOOL)(Options.wHeaderMask & HF_FNAME)) ;
         CheckDlgButton( hDlg, IDD_HPAGENUM,
                         (BOOL)(Options.wHeaderMask & HF_PAGENUM)) ;
         CheckDlgButton( hDlg, IDD_HDATE,
                         (BOOL)(Options.wHeaderMask & HF_PDATE) ||
                         (BOOL)(Options.wHeaderMask & HF_RDATE)) ;

         CheckRadioButton( hDlg, IDD_HRDATE, IDD_HPDATE,
                           (BOOL)(Options.wHeaderMask & HF_RDATE) ?
                                  IDD_HRDATE : IDD_HPDATE) ;

         EnableWindow( GetDlgItem( hDlg, IDD_HPDATE ),
                       IsDlgButtonChecked( hDlg, IDD_HDATE ) ) ;
         EnableWindow( GetDlgItem( hDlg, IDD_HRDATE ),
                       IsDlgButtonChecked( hDlg, IDD_HDATE ) ) ;

         CheckDlgButton( hDlg, IDD_FFNAME,
                         (BOOL)(Options.wFooterMask & HF_FNAME) ) ;
         CheckDlgButton( hDlg, IDD_FPAGENUM,
                         (BOOL)(Options.wFooterMask & HF_PAGENUM) ) ;
         CheckDlgButton( hDlg, IDD_FDATE,
                         (BOOL)(Options.wFooterMask & HF_PDATE) ||
                         (BOOL) (Options.wFooterMask & HF_RDATE)) ;

         CheckRadioButton( hDlg, IDD_FRDATE, IDD_FPDATE,
                           (BOOL)(Options.wFooterMask & HF_RDATE) ?
                                 IDD_FRDATE : IDD_FPDATE) ;

         EnableWindow( GetDlgItem( hDlg, IDD_FPDATE ),
                       IsDlgButtonChecked( hDlg, IDD_FDATE ) ) ;
         EnableWindow( GetDlgItem( hDlg, IDD_FRDATE ),
                       IsDlgButtonChecked( hDlg, IDD_FDATE ) ) ;

         uiIDD = 0L ;

         tdShadowControl( hDlg, uiMsg, wParam, RGBLTGRAY ) ;
      break ;

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
            /* Headers/ Footers */
            case IDD_HDATE:
               EnableWindow( GetDlgItem( hDlg, IDD_HRDATE ),
                             IsDlgButtonChecked( hDlg, wParam ) ) ;
               EnableWindow( GetDlgItem( hDlg, IDD_HPDATE ),
                             IsDlgButtonChecked( hDlg, wParam ) ) ;

            break ;

            case IDD_FDATE:
               EnableWindow( GetDlgItem( hDlg, IDD_FRDATE ),
                             IsDlgButtonChecked( hDlg, wParam ) ) ;
               EnableWindow( GetDlgItem( hDlg, IDD_FPDATE ),
                             IsDlgButtonChecked( hDlg, wParam ) ) ;
               break ;


            case IDOK:
               /* HF */
               Options.wHeaderMask   =
                  (IsDlgButtonChecked( hDlg, IDD_HFNAME)   ? HF_FNAME   : 0) |  
                  (IsDlgButtonChecked( hDlg, IDD_HPAGENUM) ? HF_PAGENUM : 0) |
                  (IsDlgButtonChecked( hDlg, IDD_HDATE)    ?
                  (IsDlgButtonChecked( hDlg, IDD_HRDATE)   ? HF_RDATE : HF_PDATE) : 0) ;

               Options.wFooterMask   =
                  (IsDlgButtonChecked( hDlg, IDD_FFNAME)   ? HF_FNAME   : 0) |  
                  (IsDlgButtonChecked( hDlg, IDD_FPAGENUM) ? HF_PAGENUM : 0) |
                  (IsDlgButtonChecked( hDlg, IDD_FDATE)    ?
                  (IsDlgButtonChecked( hDlg, IDD_FRDATE)   ? HF_RDATE : HF_PDATE) : 0) ;

               Options.bHFMargins    = IsDlgButtonChecked( hDlg, IDD_HFMARGINS ) ;
               return FALSE ;

            case IDM_OPTIONS:
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

            case pshHelp:
               WinHelp( hwndMain, GRCS(IDS_HELPFILE), HELP_CONTEXT,
                        HLP_HEADERS ) ;
            break ;


         }
      }
      break;

      default:
         return tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;

   }
   /*
    * return false to pass message on to default
    */
   return FALSE ;

}


/************************************************************************
 * End of File: headfoot.c
 ***********************************************************************/

