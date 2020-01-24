/************************************************************************
 *
 *     Project:  TTVIEW
 *
 *      Module:  subdlgs.c
 *
 *     Remarks:  Contains stubs for sub dialogs (fileopen, font)
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "..\inc\TTVIEW.h"
#include <print.h>
//#include <sys\types.h>
//#include <sys\stat.h>         // time shit
#include <stdlib.h>
#include <cderr.h>      // commdlg errors
#include <string.h>
#include "..\inc\subdlgs.h"
#include "..\inc\dlgs.h"
#include "..\inc\dlghelp.h"
#include "..\inc\font.h"
#include "..\inc\config.h"

/****************************************************************
 *  BOOL WINAPI DoPrinterSetup( void )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI DoPrinterSetup( void )
{

   PRINTDLG       pd ;
   DEVNAMES FAR * lpdn ;
   char           szDriver[MAX_PATH] ;
   char           szDevice[32] ;
   char           szOutput[MAX_PATH] ;
   DWORD dwErr ;

   GetDevNamesInfo( ghDevNames, szDriver, szDevice, szOutput ) ;

DoPrintDlg:

   /*
    * Use commdlgs to change the printer
    */
   memset( &pd, '\0', sizeof( PRINTDLG ) ) ;

   pd.lStructSize = sizeof( PRINTDLG ) ;
   pd.hwndOwner = hwndApp ;
   pd.hDevMode = ghDevMode ;
   pd.hDevNames = ghDevNames ;
   pd.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_PRINTSETUP ;
   pd.nCopies = 0 ;
   pd.hInstance = NULL ;

   if (PrintDlg( &pd ) != 0)
   {
      /*
       * Set our internal vars to match those returned...
       */

      ghDevMode = pd.hDevMode ;
      ghDevNames = pd.hDevNames ;

      if (pd.hDC)
      {
         DeleteDC( pd.hDC ) ;
         pd.hDC = NULL ;
      }

      return TRUE ;
   }
   else
   {
      /*
       * We got an error.
       */
      switch( dwErr = CommDlgExtendedError() )
      {
         case PDERR_PRINTERCODES:
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_PRINTERCODES ) ;
         break ;

         case PDERR_SETUPFAILURE:
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_SETUPFAILURE ) ;
         break ;

         case PDERR_PARSEFAILURE:
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_PARSEFAILURE ) ;
         break ;

         case PDERR_RETDEFFAILURE:
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_RETDEFFAILURE ) ;
         break ;

         case PDERR_LOADDRVFAILURE:
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_LOADDRVFAILURE ) ;
         break ;

         case PDERR_GETDEVMODEFAIL:
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_GETDEVMODEFAIL ) ;
         break ;

         case PDERR_INITFAILURE:
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_INITFAILURE ) ;
         break ;

         case PDERR_NODEVICES:
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_NODEVICES ) ;
         break ;

         case PDERR_NODEFAULTPRN:
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_NODEFAULTPRN ) ;
         break ;

         case PDERR_DNDMMISMATCH:
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_DNDMMISMATCH ) ;
         break ;

         case PDERR_CREATEICFAILURE:
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_CREATEICFAILURE ) ;
         break ;

         case PDERR_PRINTERNOTFOUND:
            DP1( hWDB, "ghDevnames = %d", ghDevNames ) ;
            DP1( hWDB, "ghDevMode = %d", ghDevMode ) ;
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_PRINTERNOTFOUND,
                         (LPSTR)szDevice,
                         (LPSTR)szDriver,
                         (LPSTR)szOutput
                         ) ;
            GlobalFree( ghDevNames ) ;
            ghDevNames = NULL ;
            if (pd.hDC)
            {
               DeleteDC( pd.hDC ) ;
               pd.hDC = NULL ;
            }
            goto DoPrintDlg ;
         break ;

         case CDERR_GENERALCODES:
            ghDevMode = pd.hDevMode ;
            ghDevNames = pd.hDevNames ;

            if (ghDevNames)
            {
               lpdn = (DEVNAMES FAR *)GlobalLock( ghDevNames ) ;
               DP1( hWDB, "  dnDriver = %s, dnDevice = %s, dnOuptut = %s",
                  (LPSTR)lpdn+(lpdn->wDriverOffset),
                  (LPSTR)lpdn+(lpdn->wDeviceOffset),
                  (LPSTR)lpdn+(lpdn->wOutputOffset) ) ;
               GlobalUnlock( ghDevNames ) ;
            }
            if (pd.hDC)
               DeleteDC( pd.hDC ) ;

         return FALSE ;

         default:
            ErrorResBox( hwndApp, hinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_CDERR_GENERALCODES,
                         LOWORD( dwErr ) ) ;
         break ;
      }

      ghDevMode = pd.hDevMode ;
      ghDevNames = pd.hDevNames ;

      if (ghDevNames)
      {
         lpdn = (DEVNAMES FAR *)GlobalLock( ghDevNames ) ;
         DP1( hWDB, "  dnDriver = %s, dnDevice = %s, dnOuptut = %s",
            (LPSTR)lpdn+(lpdn->wDriverOffset),
            (LPSTR)lpdn+(lpdn->wDeviceOffset),
            (LPSTR)lpdn+(lpdn->wOutputOffset) ) ;
         GlobalUnlock( ghDevNames ) ;
      }

      if (pd.hDC)
         DeleteDC( pd.hDC ) ;
      return FALSE ;
   }

} /* DoPrinterSetup()  */

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
   chf.Flags = CF_SCREENFONTS | CF_USESTYLE |CF_TTONLY |
               CF_INITTOLOGFONTSTRUCT | CF_SHOWHELP ;
   chf.rgbColors = 0L ;
   chf.lCustData = 0L ;
   chf.lpfnHook = (FARPROC) NULL ;
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
      WORD w ;

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

/****************************************************************
 *  void WINAPI GetDevNamesInfo( GLOBALHANDLE ghDevNames, LPSTR szDriver, LPSTR szDevice, LPSTR szOutput )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
void WINAPI GetDevNamesInfo( GLOBALHANDLE ghDevNames, LPSTR szDriver, LPSTR szDevice, LPSTR szOutput )
{
   LPDEVNAMES lpdn ;

   if (ghDevNames)
   {
      lpdn = (DEVNAMES FAR *)GlobalLock( ghDevNames ) ;
      lstrcpy( szDriver, (LPSTR)lpdn+(lpdn->wDriverOffset) ) ;
      lstrcpy( szDevice, (LPSTR)lpdn+(lpdn->wDeviceOffset) ) ;
      lstrcpy( szOutput, (LPSTR)lpdn+(lpdn->wOutputOffset) ) ;
      GlobalUnlock( ghDevNames ) ;
   }
   else
   {
      szDriver[0] = '\0' ;
      szDevice[0] = '\0' ;
      szOutput[0] = '\0' ;
   }

} /* GetDevNamesInfo()  */


BOOL CALLBACK fnAbout( HWND hDlg, WORD wMsg, WPARAM wParam, LPARAM lParam) ;

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

BOOL CALLBACK fnAbout( HWND hDlg, WORD wMsg, WPARAM wParam, LPARAM lParam ) 
{
   char  szTemp [256] ;

   switch (wMsg)
   {
      case WM_INITDIALOG:

         wsprintf( szTemp, "%s %s - %s",
                   (LPSTR)rglpsz[IDS_VERSION], (LPSTR)szVerNum,
                   (LPSTR)__DATE__ ) ;
         SetDlgItemText (hDlg, IDD_VERSION, szTemp ) ;
         break ;

      case WM_COMMAND:
         switch ((WORD)wParam)
         {
            case IDOK:
            case IDCANCEL:
               EndDialog (hDlg, 0) ;
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

