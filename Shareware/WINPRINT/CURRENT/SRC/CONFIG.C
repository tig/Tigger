/************************************************************************
 *
 *     Project:  WinPrint 1.4
 *
 *      Module:  config.c
 *
 *     Remarks:  Handles reading/writing configuration data.
 *
 *    Data is stored in WINPRINT.INI:
 *
 *    [Main]
 *    CurrentConfig=sourcecode
 *    Name=Microsoft WinSDK
 *    Code=W9C92E6CCB2
 *    ScreenPos = x,y
 *
 *    [Configs]
 *    Default = Default
 *    Source Code = Source Code
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "precomp.h"

#include "winprint.h"
#include "isz.h"
#include "config.h"
#include "ws_lib.h"
#include "fontutil.h"
#include "wsscanf.h"
#include "dlghelp.h"

VOID NEAR PASCAL GetGoodDefaultFonts( HDC hDC ) ;

/****************************************************************
 *  BOOL FAR PASCAL GetDefaultConfig( VOID )
 *
 *  Description: 
 *
 *    Gets the default configuration.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL GetDefaultConfig( VOID )
{
   PRINTDLG       pd ;

   DP4( hWDB, "GetDefaultConfig..." ) ;

   if (ghDevMode)
   {
      GlobalFree( ghDevMode ) ;
      ghDevMode = NULL ;
   }

   if (ghDevNames)
   {
      GlobalFree( ghDevNames ) ;
      ghDevNames = NULL ;
   }

   memset( &pd, '\0', sizeof( PRINTDLG ) ) ;
   pd.lStructSize = sizeof( PRINTDLG ) ;
   pd.hwndOwner =  hwndMain ;

   /*
    * Use common dialogs to get the default printer.
    * We will get an hIC back.  If it is NULL, then ReallyCreateFont()
    * will use the screen, which is ok.
    */
   pd.Flags = PD_RETURNDEFAULT | PD_RETURNIC ;

   if (!PrintDlg( &pd ))
   {
      DP1( hWDB, "GetDefaultConfig() : PrintDlg failed %04X",
         LOWORD(CommDlgExtendedError()) ) ;
   }

   ghDevMode = pd.hDevMode ;
   ghDevNames = pd.hDevNames ;

   DP4( hWDB, "GetDefaultConfig() : ghDevMode = %d, gDevNames = %d",
         ghDevMode, ghDevNames ) ;

   /*
    * Now get a good default font.  Make the header footer bold.
    */
   if (pd.hDC)
   {
      GetGoodDefaultFonts( pd.hDC ) ;
      DeleteDC( pd.hDC ) ;
   }

   Options.bMinimize = FALSE ;
   Options.bClose = FALSE ;
   Options.wHeaderMask = H_DEFAULT ;
   Options.wFooterMask = F_DEFAULT ;
   Options.bHFMargins = TRUE ;
   Options.bExpandTabs = TRUE ;
   Options.nTabSize = 8 ;
   Options.bInches = TRUE ;
   Options.nTopMargin = 0 ;
   Options.nLeftMargin = 0 ;
   Options.nBottomMargin = 0 ;
   Options.nRightMargin = 0 ;
   Options.nColumns = 1 ;
   Options.nColSpace = 200 ;                 /* .25 inch */
   Options.wSeparator = SEP_GRAPHIC ;
   Options.bSeparateJobs = FALSE ;
   Options.bPrintToFile = FALSE ;
   
   return TRUE ;

} /* GetDefaultConfig()  */


VOID NEAR PASCAL GetGoodDefaultFonts( HDC hDC )
{
   HFONT hFont ;
   LOGFONT lf ;

   hFont = ReallyCreateFont( hDC, "Courier", 10, RCF_NORMAL ) ;
   GetObject( hFont, sizeof(LOGFONT), &lf ) ;
   DeleteObject( hFont ) ;

   Options.lfCurFont = lf ;
   Options.lfHFFont = lf ;

}

/****************************************************************
 *  BOOL FAR PASCAL GetConfig( LPSTR lpszCfg )
 *
 *  Description: 
 *
 *    Gets the configuration with the name pointed to by lpszCfg.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL GetConfig( LPSTR lpszCfg )
{
   DEVNAMES FAR * lpdn ;
   DEVMODE  FAR * lpdm ;
   char           szBuf[32] ;
   char  szDriver[_MAX_PATH] ;
   char  szDevice[32] ;
   char  szOutput[_MAX_PATH] ;
   char  szDev[_MAX_PATH * 2 + 32 + 3] ;
   UINT  w1, w2, w3 ;
   HDC      hDC ;
   short    nLogPixelsY = 0 ;

   if (lpszCfg)
   {
      /*
       * Make sure we really have a config by this name
       */
      if (!GetPrivateProfileString( GRCS(IDS_INI_CONFIGS),
                                    lpszCfg, "", szBuf, 32,
                                    GRCS(IDS_INI_FILENAME) ))
      {
         DP1( hWDB, "Config %s does not exist!", (LPSTR)lpszCfg ) ;
         return FALSE ;
      }

      /*
       * See if it's an old config
       */
      if (GetPrivateProfileString(  lpszCfg, GRCS(IDS_INI_LFHEIGHT),
                                    "", szBuf, 32,
                                    GRCS(IDS_INI_FILENAME) ))
      {
         ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION,
                      IDS_APPNAME, IDS_ERR_OLDCONFIG,
                      (LPSTR)lpszCfg ) ;

         WritePrivateProfileString( lpszCfg, NULL, NULL,
                                    GRCS(IDS_INI_FILENAME) ) ;
         WritePrivateProfileString( GRCS(IDS_INI_CONFIGS),
                                    lpszCfg, NULL,
                                    GRCS(IDS_INI_FILENAME) ) ;
      }

      if (GetPrivateProfileString( lpszCfg, GRCS(IDS_INI_DEVICE),
                                    "", szDev, sizeof( szDev ),
                                    GRCS(IDS_INI_FILENAME) ))
      {
         PSTR psz ;

         szDriver[0] = '\0' ;
         szDevice[0] = '\0' ;
         szOutput[0] = '\0' ;

         if (psz = strtok( szDev, "|" ))
         {
            DP( hWDB, psz ) ;
            lstrcpy( szDriver, psz ) ;
            if (psz = strtok( NULL, "|" ))
            {
            DP( hWDB, psz ) ;
               lstrcpy( szDevice, psz ) ;
               if (psz = strtok( NULL, "|" ))
               {
                  DP( hWDB, psz ) ;
                  lstrcpy( szOutput, psz ) ;
               }
            }
         }

         w1 = lstrlen( szDriver ) + 1;
         w2 = lstrlen( szDevice ) + 1;
         w3 = lstrlen( szOutput ) + 1;

         if (ghDevNames ||
            (ghDevNames = GlobalAlloc( GHND, sizeof( DEVNAMES ) +
                                          w1 + w2 + w3 )))
         {
            if (lpdn = (DEVNAMES FAR *)GlobalLock( ghDevNames ))
            {
               lpdn->wDriverOffset = sizeof( DEVNAMES ) ;
               _fmemcpy( (LPSTR)lpdn + lpdn->wDriverOffset, szDriver, w1 ) ;

               lpdn->wDeviceOffset = sizeof( DEVNAMES ) + w1 ;
               _fmemcpy( (LPSTR)lpdn + lpdn->wDeviceOffset, szDevice, w2 ) ;

               lpdn->wOutputOffset = sizeof( DEVNAMES ) + w1 + w2 ;
               _fmemcpy( (LPSTR)lpdn + lpdn->wOutputOffset, szOutput, w3 ) ;

               GlobalUnlock( ghDevNames ) ;
            }
         }

         /*
         * Load the DEVMODE structure
         */
         if (ghDevMode)
         {
            DP3( hWDB, "Previous DEVMODE existed, freeing" ) ;
            GlobalFree( ghDevMode ) ;
            ghDevMode = NULL ;
         }

         /*
         * If ExtDeviceMode() is available, use it to get the default devmode
         * structure for this printer.  If it is not available, we can't
         * save settings anyway.
         */
         ghDevMode = GetDevMode( ghDevNames ) ;

         if (ghDevMode && (lpdm = (DEVMODE FAR *)GlobalLock( ghDevMode )))
         {
            if (hDC = CreateDC( szDriver, szDevice, szOutput, lpdm ))
            {
               nLogPixelsY = GetDeviceCaps( hDC, LOGPIXELSY ) ;
               DeleteDC( hDC ) ;
            }

            GlobalUnlock( ghDevMode ) ;
         }


      }
      else
      {
         fSavePrinter = FALSE ;
         /*
          * get the default printer
          */
         DP1( hWDB, "Must use default printer because there is no saved one!" ) ;
         if (hDC = GetPrinterIC())
         {
            nLogPixelsY = GetDeviceCaps( hDC, LOGPIXELSY ) ;

            DeleteDC( hDC ) ;
         }
      }

      if (ghDevMode && (lpdm = (DEVMODE FAR *)GlobalLock( ghDevMode )))
      {
         DP3( hWDB, "Getting INI_DEVMODE" ) ;
         if (GetPrivateProfileString( lpszCfg, GRCS(IDS_INI_DEVMODE),
                                       "", szDev, sizeof( szDev ),
                                       GRCS(IDS_INI_FILENAME) ))
         {
            wsscanf( szDev, GRCS(IDS_INI_DEVMODE_SPEC),
                     (LPSTR)&lpdm->dmOrientation,
                     (LPSTR)&lpdm->dmPaperSize  ,   
                     (LPSTR)&lpdm->dmScale      ,   
                     (LPSTR)&lpdm->dmCopies     ,   
                     (LPSTR)&lpdm->dmDefaultSource, 
                     (LPSTR)&lpdm->dmPrintQuality,  
                     (LPSTR)&lpdm->dmDuplex ) ;
         }

         GlobalUnlock( ghDevMode ) ;
      }
      else
      {
         ghDevMode = NULL ;
         DP1( hWDB, "Could not lock DEVMODE" ) ;
         return FALSE ;
      }

      /*
       * Now get font (text)
       */
      memset( &Options.lfCurFont, 0, sizeof( LOGFONT ) ) ;
      if (GetPrivateProfileString( lpszCfg, GRCS(IDS_INI_FONT),
                                    "", szDev, sizeof( szDev ),
                                    GRCS(IDS_INI_FILENAME) ))
      {
         DP4( hWDB, "Read CurFont = %s", (LPSTR)szDev ) ;
         wsscanf( szDev, GRCS(IDS_INI_FONT_SPEC),
                  (LPSTR)&Options.lfCurFont.lfHeight,         
                  (LPSTR)&Options.lfCurFont.lfWeight,         
                  (LPSTR)&Options.lfCurFont.lfItalic,         
                  (LPSTR)&Options.lfCurFont.lfCharSet,        
                  (LPSTR)&Options.lfCurFont.lfPitchAndFamily,
                  (LPSTR)Options.lfCurFont.lfFaceName ) ;

         // HACK!  wsscanf stops at the first space
         lstrcpy( Options.lfCurFont.lfFaceName, strrchr( szDev, '|' ) + 1) ;
         DP( hWDB, "lfFaceName = %s", (LPSTR)Options.lfCurFont.lfFaceName ) ;
      }
      else
      {
         HFONT hFont ;
         /*
         * Get a good default font.
         */
         hFont = ReallyCreateFont( NULL, GRCS(IDS_DEFAULTFONT), 10, RCF_NORMAL ) ;
         GetObject( hFont, sizeof( LOGFONT ), &Options.lfCurFont ) ;
         DeleteObject( hFont ) ;

         Options.lfCurFont.lfWidth = 0 ;

         Options.lfCurFont.lfHeight = -MulDiv( 10, nLogPixelsY, 72 ) ;
      }

      /*
       * Now get font (HF)
       */
      memset( &Options.lfHFFont, 0, sizeof( LOGFONT ) ) ;
      if (GetPrivateProfileString( lpszCfg, GRCS(IDS_INI_HFFONT),
                                    "", szDev, sizeof( szDev ),
                                    GRCS(IDS_INI_FILENAME) ))
      {
         DP4( hWDB, "Read HFFont = %s", (LPSTR)szDev ) ;
         wsscanf( szDev, GRCS(IDS_INI_HFFONT_SPEC),
                  (LPSTR)&Options.lfHFFont.lfHeight,         
                  (LPSTR)&Options.lfHFFont.lfWeight,         
                  (LPSTR)&Options.lfHFFont.lfItalic,         
                  (LPSTR)&Options.lfHFFont.lfCharSet,        
                  (LPSTR)&Options.lfHFFont.lfPitchAndFamily,
                  (LPSTR)Options.lfHFFont.lfFaceName ) ;

         // HACK!  wsscanf stops at the first space
         lstrcpy( Options.lfHFFont.lfFaceName, strrchr( szDev, '|' ) + 1) ;
         DP( hWDB, "lfFaceName = %s", (LPSTR)Options.lfHFFont.lfFaceName ) ;
      }
      else
      {
         HFONT hFont ;
         /*
         * Get a good default font.
         */
         hFont = ReallyCreateFont( NULL, GRCS(IDS_DEFAULTFONT), 10, RCF_NORMAL ) ;
         GetObject( hFont, sizeof( LOGFONT ), &Options.lfHFFont ) ;
         DeleteObject( hFont ) ;

         Options.lfHFFont.lfWidth = 0 ;

         Options.lfHFFont.lfHeight = -MulDiv( 10, nLogPixelsY, 72 ) ;
      }

      if (GetPrivateProfileString( lpszCfg, GRCS(IDS_INI_OPTIONS),
                                    "", szDev, sizeof( szDev ),
                                    GRCS(IDS_INI_FILENAME) ))
      {
         wsscanf( szDev, GRCS(IDS_INI_OPTIONS_SPEC),
                  (LPSTR)&Options.bMinimize,
                  (LPSTR)&Options.bClose,
                  (LPSTR)&Options.wHeaderMask,
                  (LPSTR)&Options.wFooterMask,
                  (LPSTR)&Options.bHFMargins,
                  (LPSTR)&Options.bExpandTabs,
                  (LPSTR)&Options.nTabSize,
                  (LPSTR)&Options.bInches,
                  (LPSTR)&Options.nTopMargin,
                  (LPSTR)&Options.nLeftMargin,
                  (LPSTR)&Options.nBottomMargin,
                  (LPSTR)&Options.nRightMargin,
                  (LPSTR)&Options.nColumns,
                  (LPSTR)&Options.nColSpace,
                  (LPSTR)&Options.wSeparator,
                  (LPSTR)&Options.bSeparateJobs) ;
      }
      else
      {
         Options.bMinimize = FALSE ;
         Options.bClose = FALSE ;
         Options.wHeaderMask = H_DEFAULT ;
         Options.wFooterMask = F_DEFAULT ;
         Options.bHFMargins = TRUE ;
         Options.bExpandTabs = TRUE ;
         Options.nTabSize = 8 ;
         Options.bInches = TRUE ;
         Options.nTopMargin = 0 ;
         Options.nLeftMargin = 0 ;
         Options.nBottomMargin = 0 ;
         Options.nRightMargin = 0 ;
         Options.nColumns = 1 ;
         Options.nColSpace = 200 ;                 /* .25 inch */
         Options.wSeparator = SEP_GRAPHIC ;
         Options.bSeparateJobs = FALSE ;
      }

      Options.bPrintToFile = GetPrivateProfileInt( lpszCfg, GRCS(IDS_INI_TOFILE),
                                    FALSE, GRCS(IDS_INI_FILENAME) ) ;

      return TRUE ;
   }
   else
      return GetDefaultConfig() ;

   return FALSE ;
} /* GetConfig()  */

/****************************************************************
 *  BOOL FAR PASCAL SaveConfig( LPSTR lpszCfg )
 *
 *  Description: 
 *
 *    Saves the current configuration under the name pointed to
 *    by lpszCfg.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL SaveConfig( LPSTR lpszCfg )
{
   DEVNAMES FAR * lpdn ;
   DEVMODE  FAR * lpdm ;
   PSTR psz = (PSTR)LocalAlloc( LPTR, _MAX_PATH * 2 + LF_FACESIZE * 2 ) ;

   if (!psz)
      return FALSE ;

   /*
    * Need an item in the [Configs] section
    */
   WritePrivateProfileString( GRCS(IDS_INI_CONFIGS), lpszCfg, lpszCfg,
                              GRCS(IDS_INI_FILENAME) ) ;

   if (ghDevMode && (lpdm = (DEVMODE FAR *)GlobalLock( ghDevMode )))
   {
      wsprintf( psz, GRCS(IDS_INI_DEVMODE_SPEC),
               lpdm->dmOrientation,
               lpdm->dmPaperSize,
               lpdm->dmScale,
               lpdm->dmCopies,
               lpdm->dmDefaultSource,
               lpdm->dmPrintQuality,
               lpdm->dmDuplex ) ;

      DP4( hWDB, "Writing DM = %s", (LPSTR)psz ) ;
      WritePrivateProfileString( lpszCfg, GRCS(IDS_INI_DEVMODE),
         psz, GRCS(IDS_INI_FILENAME) ) ;

      GlobalUnlock( ghDevMode ) ;
   }

   DASSERT( hWDB, ghDevNames ) ;
   if (fSavePrinter && (lpdn = (DEVNAMES FAR *)GlobalLock( ghDevNames )))
   {
      wsprintf( psz, GRCS(IDS_INI_DEVICE_SPEC),
         (LPSTR)lpdn+lpdn->wDriverOffset,
         (LPSTR)lpdn+lpdn->wDeviceOffset,
         (LPSTR)lpdn+lpdn->wOutputOffset ) ;

      DP4( hWDB, "Writing Printer = \"%s\"", (LPSTR)psz ) ;
      WritePrivateProfileString( lpszCfg, GRCS(IDS_INI_DEVICE),
         psz, GRCS(IDS_INI_FILENAME) ) ;

      DP4( hWDB, "Wrote!" ) ;

      GlobalUnlock( ghDevNames ) ;
      DP4( hWDB, "GlobalUnlocked!" ) ;
   }
   else
   {
      DP4( hWDB, "Clearing Printer = NULL", (LPSTR)psz ) ;
      WritePrivateProfileString( lpszCfg, GRCS(IDS_INI_DEVICE),
                                 NULL, GRCS(IDS_INI_FILENAME) ) ;
   }

   wsprintf( psz, GRCS(IDS_INI_FONT_SPEC),
         Options.lfCurFont.lfHeight,
         Options.lfCurFont.lfWeight,
         Options.lfCurFont.lfItalic,
         Options.lfCurFont.lfCharSet,
         Options.lfCurFont.lfPitchAndFamily,
         (LPSTR)Options.lfCurFont.lfFaceName ) ;

   DP4( hWDB, "Writing CurFont = %s", (LPSTR)psz ) ;
   WritePrivateProfileString( lpszCfg, GRCS(IDS_INI_FONT),
                              psz, GRCS(IDS_INI_FILENAME) ) ;

   wsprintf( psz, GRCS(IDS_INI_HFFONT_SPEC),
         Options.lfHFFont.lfHeight,
         Options.lfHFFont.lfWeight,
         Options.lfHFFont.lfItalic,
         Options.lfHFFont.lfCharSet,
         Options.lfHFFont.lfPitchAndFamily,
         (LPSTR)Options.lfHFFont.lfFaceName ) ;

   DP4( hWDB, "Writing HFFont = %s", (LPSTR)psz ) ;
   WritePrivateProfileString( lpszCfg, GRCS(IDS_INI_HFFONT),
                              psz, GRCS(IDS_INI_FILENAME) ) ;

   wsprintf( psz, GRCS(IDS_INI_OPTIONS_SPEC),
               Options.bMinimize,
               Options.bClose,
               Options.wHeaderMask,
               Options.wFooterMask,
               Options.bHFMargins,
               Options.bExpandTabs,
               Options.nTabSize,
               Options.bInches,
               Options.nTopMargin,
               Options.nLeftMargin,
               Options.nBottomMargin,
               Options.nRightMargin,
               Options.nColumns,
               Options.nColSpace,
               Options.wSeparator,
               Options.bSeparateJobs) ;

   WritePrivateProfileString( lpszCfg, GRCS(IDS_INI_OPTIONS),
                              psz, GRCS(IDS_INI_FILENAME) ) ;

   wsprintf( psz, "%d", Options.bPrintToFile ) ;
   WritePrivateProfileString( lpszCfg, GRCS(IDS_INI_TOFILE),
                            psz, GRCS(IDS_INI_FILENAME) ) ;

   LocalFree( (HANDLE)psz ) ;

   return TRUE ;

} /* SaveConfig()  */


/************************************************************************
 * End of File: config.c
 ***********************************************************************/

