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
#include "PRECOMP.H"
#include "GLOBAL.H"

#include "dlgs.h"

typedef UINT (CALLBACK *LPFNHOOKPROC)(HWND, UINT, WPARAM, LPARAM); 

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
   char           szDriver[_MAX_PATH] ;
   char           szDevice[32] ;
   char           szOutput[_MAX_PATH] ;
   DWORD dwErr ;

   GetDevNamesInfo( ghDevNames, szDriver, szDevice, szOutput ) ;

DoPrintDlg:

   /*
    * Use commdlgs to change the printer
    */
   memset( &pd, '\0', sizeof( PRINTDLG ) ) ;

   pd.lStructSize = sizeof( PRINTDLG ) ;
   pd.hwndOwner = ghwndApp ;
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
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_PRINTERCODES ) ;
         break ;

         case PDERR_SETUPFAILURE:
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_SETUPFAILURE ) ;
         break ;

         case PDERR_PARSEFAILURE:
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_PARSEFAILURE ) ;
         break ;

         case PDERR_RETDEFFAILURE:
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_RETDEFFAILURE ) ;
         break ;

         case PDERR_LOADDRVFAILURE:
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_LOADDRVFAILURE ) ;
         break ;

         case PDERR_GETDEVMODEFAIL:
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_GETDEVMODEFAIL ) ;
         break ;

         case PDERR_INITFAILURE:
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_INITFAILURE ) ;
         break ;

         case PDERR_NODEVICES:
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_NODEVICES ) ;
         break ;

         case PDERR_NODEFAULTPRN:
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_NODEFAULTPRN ) ;
         break ;

         case PDERR_DNDMMISMATCH:
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_DNDMMISMATCH ) ;
         break ;

         case PDERR_CREATEICFAILURE:
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
                         IDS_PDERR_CREATEICFAILURE ) ;
         break ;

         case PDERR_PRINTERNOTFOUND:
            DP1( hWDB, "ghDevnames = %d", ghDevNames ) ;
            DP1( hWDB, "ghDevMode = %d", ghDevMode ) ;
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
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
            ErrorResBox( ghwndApp, ghinstApp, MB_ICONEXCLAMATION, IDS_APPTITLE,
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

   hdc = GetDC(ghwndApp);
   chf.hDC = CreateCompatibleDC(hdc);
   ReleaseDC(ghwndApp, hdc);

   GetObject( ghfontCur, sizeof( LOGFONT ), (LPSTR)&lf ) ;

   chf.lStructSize = sizeof( CHOOSEFONT ) ;
   chf.hwndOwner = ghwndApp ;
   chf.lpLogFont = &lf ;
   chf.Flags = CF_SCREENFONTS |CF_TTONLY |
               CF_INITTOLOGFONTSTRUCT | CF_SHOWHELP ;
   chf.rgbColors = 0L ;
   chf.lCustData = 0L ;
   chf.lpfnHook = (LPFNHOOKPROC)NULL ;
   chf.lpTemplateName = (LPSTR) NULL ;
   chf.hInstance = (HANDLE) NULL ;
   chf.iPointSize = 0 ;
   chf.lpszStyle = NULL ;
   chf.nFontType = SCREEN_FONTTYPE ;
   chf.nSizeMin = 0 ;
   chf.nSizeMax = 0 ;

   SetNormalCursor() ;

   if (ChooseFont(&chf))
   {
      char szBuf[64] ;

      SetWaitCursor() ;

      DeleteObject( ghfontCur ) ;

      ghfontCur = CreateFontIndirect( &lf ) ;

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

   (FARPROC)lpfnDlgProc = MakeProcInstance( (FARPROC)fnAbout, ghinstApp ) ;
   DialogBox( ghinstApp,
              MAKEINTRESOURCE( DLG_ABOUTBOX ),
              ghwndApp,
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

         wsprintf( szTemp, "%s %d.%02d.%03d - %s",
                   (LPSTR)rglpsz[IDS_VERSION], VER_MAJOR, VER_MINOR, VER_BUILD,
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

