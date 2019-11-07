/************************************************************************
 *
 *     Project:  XEDIT 2.0
 *
 *      Module:  subdlgs.c
 *
 *     Remarks:  Contains stubs for sub dialogs (fileopen, font)
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "..\inc\XEDIT.h"
#include <print.h>
#include <sys\types.h>
#include <sys\stat.h>         // time shit
#include <stdlib.h>
#include <cderr.h>      // commdlg errors
#include <string.h>
#include "..\inc\subdlgs.h"
#include "..\inc\dlgs.h"
#include "..\inc\dlghelp.h"
#include "..\inc\font.h"
#include "..\inc\config.h"


HFONT hfontHex ; // hack! for now

/****************************************************************
 *  BOOL WINAPI DoFileOpen( LPSTR lpszFileName, LPSTR lpszFileTitle ) ;
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI DoFileOpen( LPSTR lpszFileName, LPSTR lpszFileTitle ) 
{
   static OPENFILENAME  ofn ;
   static char          szDirName[256] ;  /* directory name array  */
   static char          szFile[256],
                        szFileTitle[256] ;/* file and title arrays */
   static char          szDefExt[MAX_EXT*2] ;
   static BOOL          fOnce = FALSE ;

   BOOL           fExisted ;

   SetWaitCursor() ;

   if (fOnce == FALSE)
   {
      /*
       * Nice sleazy hack to get the list of filters.  The resource
       * string is of the form
       * "Windows INI files (*.INI)|*.INI|Text files (*.TXT)|*.TXT|".
       * Note that there must be a '|' on the end to get the final
       * NULL terminator.
       */
      if (rglpsz[IDS_OPENFILENAMEFILTER])
      {
         LPSTR lpsz = _fstrtok( rglpsz[IDS_OPENFILENAMEFILTER], "|" ) ;

         while (lpsz)
            lpsz = _fstrtok( NULL, "|" ) ;
      }

      lstrcpy( szDefExt, rglpsz[IDS_DEFEXT] ) ;
   }


   /* Initialize the OPENFILENAME members */

   szFile[0] = '\0';
   szFileTitle[0] = '\0';
   ofn.lStructSize = sizeof(OPENFILENAME) ;
   ofn.hwndOwner = hwndApp ;
   ofn.hInstance = NULL ;
   ofn.lpstrFilter = rglpsz[IDS_OPENFILENAMEFILTER] ;
   ofn.lpstrCustomFilter = (LPSTR) NULL ;
   ofn.nMaxCustFilter = 0L ;
   ofn.lpstrFile= szFile ;
   ofn.nMaxFile = sizeof(szFile) ;
   ofn.lpstrFileTitle = szFileTitle ;
   ofn.nMaxFileTitle = sizeof(szFileTitle) ;


   if (fOnce == FALSE)
   {
      ofn.nFilterIndex = 1L ;
      /* Get the system directory name and store in szDirName */
      GetWindowsDirectory((LPSTR)szDirName, 255);
      ofn.lpstrInitialDir = szDirName ;
   }

   ofn.lpstrTitle = (LPSTR) NULL ;
   ofn.Flags = OFN_SHOWHELP |
               OFN_PATHMUSTEXIST |
               OFN_FILEMUSTEXIST |
               OFN_HIDEREADONLY ;
   ofn.nFileOffset = 0 ;
   ofn.nFileExtension = 0 ;
   ofn.lpstrDefExt = szDefExt ;

   /*
    * Say we've already setup so when we get called again it
    * uses the stuff from last time
    */
   fOnce = TRUE ;

   /*
    * Call the GetOpenFilename function
    */

   SetNormalCursor() ;

   if (GetOpenFileName(&ofn))
   {
   }
   else
   {
      /*
       * Some sort of Commdlg error
       */
      DP1( hWDB, "GetOpenFileName returned false.  CommDlgExtendedError = %08lX",
          CommDlgExtendedError() ) ;

      return FALSE ;
   }

   return TRUE ;

} /* DoFileOpen()  */


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

   GetObject( hfontHex, sizeof( LOGFONT ), (LPSTR)&lf ) ;

   chf.lStructSize = sizeof( CHOOSEFONT ) ;
   chf.hwndOwner = hwndApp ;
   chf.lpLogFont = &lf ;
   chf.Flags = CF_SCREENFONTS | CF_ANSIONLY |
               CF_INITTOLOGFONTSTRUCT | CF_SHOWHELP ;
   chf.rgbColors = 0L ;
   chf.lCustData = 0L ;
   chf.lpfnHook = (FARPROC) NULL ;
   chf.lpTemplateName = (LPSTR) NULL ;
   chf.hInstance = (HANDLE) NULL ;
   chf.iPointSize = 0 ;
   chf.lpszStyle = (LPSTR)NULL ;
   chf.nFontType = SCREEN_FONTTYPE ;
   chf.nSizeMin = 0 ;
   chf.nSizeMax = 0 ;

   SetNormalCursor() ;

   if (ChooseFont(&chf))
   {
      char szBuf[64] ;
      WORD w ;

      SetWaitCursor() ;

      DeleteObject( hfontHex ) ;

      hfontHex = CreateFontIndirect( &lf ) ;

      /*
       * Now write it to the INI file
       */
      w = RCF_NORMAL ;
      wsprintf( szBuf, rglpsz[IDS_INI_FONT_SPEC],
                chf.iPointSize / 10,
                w,
                (LPSTR)lf.lfFaceName ) ;

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

BOOL CALLBACK fnEditor( HWND hDlg, WORD wMsg, WPARAM wParam, LPARAM lParam) ;

/****************************************************************
 *  BOOL WINAPI DoEditor( void )
 *
 *  Description: 
 *
 *    Pops up the configue eternal editor dialog.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI DoEditor( void )
{
   DLGPROC  lpfnDlgProc ;
   BOOL     fOk ;

   lpfnDlgProc = (DLGPROC)MakeProcInstance((FARPROC)fnEditor, hinstApp ) ;

   fOk = DialogBox( hinstApp, MAKEINTRESOURCE( DLG_EDITOR ),
                    hwndApp, lpfnDlgProc ) ;

   FreeProcInstance( (FARPROC)lpfnDlgProc ) ;

   return fOk ;

} /* DoEditor()  */

BOOL CALLBACK fnEditor( HWND hDlg, WORD wMsg, WPARAM wParam, LPARAM lParam)
{
   char szEditor[MAX_PATH] ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
         SendDlgItemMessage( hDlg, IDD_EDITOR, EM_LIMITTEXT,
                             (WPARAM)(MAX_PATH - 1),
                             (LPARAM)0L ) ;

         GetPrivateProfileString( rglpsz[IDS_INI_MAIN],
                                  rglpsz[IDS_INI_EDITOR],
                                  "",
                                  szEditor,
                                  MAX_PATH,
                                  rglpsz[IDS_INI_FILENAME] ) ;

         SetDlgItemText( hDlg, IDD_EDITOR, szEditor ) ;
      return TRUE ;

      case WM_COMMAND:
         switch ((WORD)wParam)
         {
            case IDOK:

               GetDlgItemText( hDlg, IDD_EDITOR, szEditor, MAX_PATH ) ;

               WritePrivateProfileString( rglpsz[IDS_INI_MAIN],
                                          rglpsz[IDS_INI_EDITOR],
                                          szEditor,
                                          rglpsz[IDS_INI_FILENAME] ) ;

               EndDialog( hDlg, TRUE ) ;
            break ;

            case IDCANCEL:
               EndDialog( hDlg, FALSE ) ;
            break ;

            case IDD_BROWSE:
            break ;

            default:
               return FALSE ;
         }

      default:
         return FALSE ;
   }
   return TRUE ;
}

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

/************************************************************************
 * End of File: subdlgs.c
 ***********************************************************************/

