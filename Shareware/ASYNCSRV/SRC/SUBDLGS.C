/************************************************************************
 *
 *     Copyright (c) 1992-93 C.E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  ASYNCSRV
 *
 *      Module:  subdlgs.c
 *
 *     Remarks:  Contains stubs for sub dialogs (fileopen, font)
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "ASYNCSRV.h"

#include "subdlgs.h"
#include "dlgs.h"
#include "config.h"

/****************************************************************
 *  BOOL WINAPI DoChooseFont( void )
 *
 *  Description: 
 *
 *    Pops up the choosefont dialog
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI DoChooseFont( void )
{
   HDC hdc;
   LOGFONT lf;
   CHOOSEFONT chf;

   SetWaitCursor() ;

   hdc = GetDC(hwndApp);
   chf.hDC = CreateCompatibleDC(hdc);
   ReleaseDC(hwndApp, hdc);

   GetObject( hfontCur, sizeof( LOGFONT ), (LPSTR)&lf ) ;

   chf.lStructSize = sizeof( CHOOSEFONT ) ;
   chf.hwndOwner = hwndApp ;
   chf.lpLogFont = &lf ;
   chf.Flags = CF_SCREENFONTS | CF_USESTYLE | CF_FIXEDPITCHONLY |
               CF_INITTOLOGFONTSTRUCT ;
   chf.rgbColors = 0L ;
   chf.lCustData = 0L ;
   chf.lpfnHook = NULL ;
   chf.lpTemplateName = (LPSTR) NULL ;
   chf.hInstance = (HANDLE) NULL ;
   chf.iPointSize = 0 ;
   chf.lpszStyle = (LPSTR)szFaceStyle ;
   chf.nFontType = SCREEN_FONTTYPE ;
   chf.nSizeMin = 0 ;
   chf.nSizeMax = 0 ;

   SetNormalCursor() ;

   if (ChooseFont(&chf))
   {
      char szBuf[64] ;

      SetWaitCursor() ;

      DeleteObject( hfontCur ) ;

      lstrcpy( szFaceName, lf.lfFaceName ) ;

      hfontCur = CreateFontIndirect( &lf ) ;

      /*
       * Now write it to the INI file
       */
      wsprintf( szBuf, rglpsz[IDS_INI_FONT_SPEC],
                chf.iPointSize / 10,
                (LPSTR)szFaceName,
                (LPSTR)szFaceStyle ) ;

      DP3( hWDB, "Writing profile : %s", (LPSTR)szBuf ) ;

      WritePrivateProfileString( rglpsz[IDS_INI_MAIN],
                                 rglpsz[IDS_INI_FONT],
                                 szBuf,
                                 rglpsz[IDS_INI_FILENAME] ) ;
      

      DeleteDC( chf.hDC ) ;

      ResetTTYScreen( hwndTTY ) ;

      SetNormalCursor() ;
      return TRUE ;
   }
   else
   {
      DeleteDC( chf.hDC ) ;
      return FALSE ;
   }

   return TRUE ;


} /* DoChooseFont()  */


BOOL CALLBACK fnAbout( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) ;

BOOL WINAPI DoAboutBox( VOID )
{
   DLGPROC  lpfnDlgProc ;

   (FARPROC)lpfnDlgProc = MakeProcInstance( (FARPROC)fnAbout, hinstApp ) ;
   DialogBox( hinstApp,
              MAKEINTRESOURCE( DLG_ABOUTBOX ),
              hwndApp,
              lpfnDlgProc ) ;
   FreeProcInstance( (FARPROC)lpfnDlgProc ) ;

   return TRUE ;
}

BOOL CALLBACK fnAbout( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam ) 
{
   char  szTemp [256] ;

   switch (wMsg)
   {
      case WM_INITDIALOG:

         SendDlgItemMessage( hDlg, IDD_LED1, LED_SETCOLOR, 0, RGBLTRED ) ;
         SendDlgItemMessage( hDlg, IDD_LED2, LED_SETCOLOR, 0, RGBLTGREEN ) ;

         wsprintf( szTemp, "%s %s - %s",
                   (LPSTR)rglpsz[IDS_VERSION], (LPSTR)szVerNum,
                   (LPSTR)__DATE__ ) ;
         SetDlgItemText (hDlg, IDD_VERSION, szTemp ) ;

         DlgCenter( hDlg, GetParent( hDlg ), 0 ) ;

         break ;

      case WM_COMMAND:
         switch ((UINT)wParam)
         {
            case IDOK:
            case IDCANCEL:
               EndDialog (hDlg, wParam) ;
            break ;

            default :
               return FALSE ;
         }
         break ;

   }
   return FALSE ;
}


/************************************************************************
 * End of File: subdlgs.c
 ***********************************************************************/

