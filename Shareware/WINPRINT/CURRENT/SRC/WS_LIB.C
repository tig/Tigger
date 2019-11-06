/*
   ws_lib.c - Assorted routines for WinPrint

   (c) Copyright 1990, Charles E. Kindel, Jr.

*/

/* INCLUDES */
#include "PRECOMP.H"

#include "winprint.h"
#include "isz.h"
#include "dlgs.h"
#include "ws_lib.h"
#include "wsscanf.h"
#include "fontutil.h"
#include "dlghelp.h"


/* IMPORTS */

HDC GetPrinterIC (void)
{
   HDC            hdc ;
   DEVNAMES FAR * lpdn = NULL ;
   DEVMODE  FAR * lpdm = NULL ;
   PRINTDLG       pd ;

   if (!ghDevNames && ghDevMode)
   {
      GlobalFree( ghDevMode ) ;
      ghDevMode = NULL ;
   }

   memset( &pd, '\0', sizeof( PRINTDLG ) ) ;
   pd.lStructSize = sizeof( PRINTDLG ) ;
   pd.hwndOwner = hwndMain ;
   pd.hDevMode = ghDevMode ;
   pd.hDevNames = ghDevNames ;

   if (!ghDevNames)
   {
      pd.Flags = PD_RETURNIC | PD_RETURNDEFAULT ;

      if (!PrintDlg( &pd ))
      {
         DP1( hWDB, "GetPrinterDC : PrintDlg() failed! (0x%04X)",
                    (WORD)CommDlgExtendedError() ) ;
         return FALSE ;
      }

      ghDevMode = pd.hDevMode ;
      ghDevNames = pd.hDevNames ;

      hdc = pd.hDC ;

   }
   else
   {
      lpdn = (DEVNAMES FAR *)GlobalLock( ghDevNames ) ;
      if (ghDevMode)
         lpdm = (DEVMODE FAR *)GlobalLock( ghDevMode ) ;

      if (lpdn && lpdm)
         hdc = CreateIC( (LPSTR)lpdn+(lpdn->wDriverOffset),
                            (LPSTR)lpdn+(lpdn->wDeviceOffset),
                            (LPSTR)lpdn+(lpdn->wOutputOffset),
                            (LPVOID)lpdm ) ;

      if (lpdm)
         GlobalUnlock( ghDevMode ) ;
      if (lpdn)
         GlobalUnlock( ghDevNames ) ;

   }

   return hdc ;

}


HDC GetPrinterDC (void)
{
   HDC            hdc ;
   DEVNAMES FAR * lpdn = NULL;
   DEVMODE  FAR * lpdm = NULL ;
   PRINTDLG       pd ;

   if (!ghDevNames && ghDevMode)
   {
      GlobalFree( ghDevMode ) ;
      ghDevMode = NULL ;
   }

   memset( &pd, '\0', sizeof( PRINTDLG ) ) ;
   pd.lStructSize = sizeof( PRINTDLG ) ;
   pd.hwndOwner = hwndMain ;
   pd.hDevMode = ghDevMode ;
   pd.hDevNames = ghDevNames ;

   DP3( hWDB, "GetPrinterDC() : ghDevNames = 0x%04X", ghDevNames ) ;
   DP3( hWDB, "GetPrinterDC() : ghDevMode = 0x%04X", ghDevMode ) ;

   if (!ghDevNames)
   {
      DP3( hWDB, "Getting Default Printer DC" ) ;
      pd.Flags = PD_RETURNIC | PD_RETURNDEFAULT ;

      if (!PrintDlg( &pd ))
      {
         DP1( hWDB, "GetPrinterDC : PrintDlg() failed! (0x%04X)",
                    (WORD)CommDlgExtendedError() ) ;
         return FALSE ;
      }

      ghDevMode = pd.hDevMode ;
      ghDevNames = pd.hDevNames ;

      hdc = pd.hDC ;
   }
   else
   {

      if (ghDevMode)
         lpdm = (DEVMODE FAR *)GlobalLock( ghDevMode ) ;

      lpdn = (DEVNAMES FAR *)GlobalLock( ghDevNames ) ;

      if (lpdn)
      {
         if (Options.bPrintToFile == TRUE)
         {
             DP3( hWDB, "GetPrinterDC : CreateDC( %s, %s, FILE: )",
                     (LPSTR)lpdn+(lpdn->wDriverOffset),
                     (LPSTR)lpdn+(lpdn->wDeviceOffset)) ;


             hdc = CreateDC( (LPSTR)lpdn+(lpdn->wDriverOffset),
                            (LPSTR)lpdn+(lpdn->wDeviceOffset),
                            "FILE:",
                            (LPVOID)lpdm ) ;
         }
         else
         {
             DP3( hWDB, "GetPrinterDC : CreateDC( %s, %s, %s )",
                     (LPSTR)lpdn+(lpdn->wDriverOffset),
                     (LPSTR)lpdn+(lpdn->wDeviceOffset),
                     (LPSTR)lpdn+(lpdn->wOutputOffset)) ;


             hdc = CreateDC( (LPSTR)lpdn+(lpdn->wDriverOffset),
                            (LPSTR)lpdn+(lpdn->wDeviceOffset),
                            (LPSTR)lpdn+(lpdn->wOutputOffset),
                            (LPVOID)lpdm ) ;
         }
      }

      if (lpdm)
         GlobalUnlock( ghDevMode ) ;
      if (lpdn)
         GlobalUnlock( ghDevNames ) ;

   }

   DP( hWDB, "GetPrinterDC(): hdc = %d", hdc ) ;
   return hdc ;
}

/****************************************************************
 *  GLOBALHANDLE FAR PASCAL GetDevMode( GLOBALHANDLE ghDevNames )
 *
 *  Description: 
 *
 *    Uses the ExtDeviceMode function to get the default DEVMODE
 *    structure for the printer in ghDevNames
 *
 *  Comments:
 *
 ****************************************************************/
GLOBALHANDLE FAR PASCAL GetDevMode( GLOBALHANDLE ghDN )
{
   LPDEVMODE      lpDevMode ;
   LPFNDEVMODE    lpfnExtDeviceMode ;
   DEVNAMES FAR * lpDN = NULL ;
   GLOBALHANDLE   ghDM = NULL;
   int n ;
   #ifndef WIN32
   char           szDriver[_MAX_PATH];
   HANDLE         hDriver ;
   #endif

   DASSERTMSG( hWDB, ghDN, "GetDevMode() : ghDN == NULL" ) ;

   if (!ghDN)
      return NULL ;

   if (lpDN = (DEVNAMES FAR *)GlobalLock( ghDevNames ))
   {
      #ifndef WIN32

      wsprintf( szDriver, "%s.DRV", (LPSTR)lpDN+(lpDN->wDriverOffset) ) ;

      if (32 >= (int)(hDriver = LoadLibrary( szDriver )))
      {
         DP1( hWDB, "GetDevMode : Could not load driver : %s", (LPSTR)szDriver ) ;
         GlobalUnlock( ghDN ) ;
         return NULL ;
      }

      lpfnExtDeviceMode = (LPFNDEVMODE)GetProcAddress( hDriver, PROC_EXTDEVICEMODE ) ;

      if (!lpfnExtDeviceMode)
      {
         DP3( hWDB, "GetDevMode : Driver does not support ExtDeviceMode") ;
         FreeLibrary( hDriver ) ;
         GlobalUnlock( ghDN ) ;
         return NULL ;
      }
      else
      {
         /*
          * Call ExtDeviceMode to get size of DEVMODE structure
          */
         n = (*lpfnExtDeviceMode)( hwndMain, hDriver,
                                   (LPDEVMODE)NULL,
                                   (LPSTR)lpDN+(lpDN->wDeviceOffset),
                                   (LPSTR)lpDN+(lpDN->wOutputOffset),
                                   (LPDEVMODE)NULL,
                                   (LPSTR)NULL,
                                   0 ) ;

         if ((n <= 0) || !(ghDM = GlobalAlloc( GHND, n )))
         {
            DP1( hWDB, "GetDevMode : First call to ExtDeviceMode bad! n = %d", n ) ;
            FreeLibrary( hDriver ) ;
            GlobalUnlock( ghDN ) ;
            return NULL ;
         }

         lpDevMode = (LPDEVMODE)GlobalLock( ghDM ) ;

         n = (*lpfnExtDeviceMode)( hwndMain, hDriver,
                                   (LPDEVMODE)lpDevMode,
                                   (LPSTR)lpDN+(lpDN->wDeviceOffset),
                                   (LPSTR)lpDN+(lpDN->wOutputOffset),
                                   (LPDEVMODE)lpDevMode,
                                   (LPSTR)NULL,
                                   DM_COPY ) ;

         if (n != IDOK)
         {
            DP1( hWDB, "GetDevMode : Second call to ExtDeviceMode bad! n = %d", n ) ;
            FreeLibrary( hDriver ) ;
            GlobalUnlock( ghDM ) ;
            GlobalFree( ghDM ) ;
            GlobalUnlock( ghDN ) ;
            return NULL ;
         }

         GlobalUnlock( ghDM ) ;
      }

      FreeLibrary( hDriver ) ;
      GlobalUnlock( ghDN ) ;

      #else
      /*
       * Call DeviceCapabilitiesEx to get size of DEVMODE structure
       */
      n = DeviceCapabilitiesEx( (LPSTR)lpDN+(lpDN->wDriverOffset),
                                (LPSTR)lpDN+(lpDN->wDeviceOffset),
                                (LPSTR)lpDN+(lpDN->wOutputOffset),
                                DC_SIZE,
                                (LPSTR)NULL,
                                (LPDEVMODE)NULL ) ;

      if ((n == GDI_ERROR) || !(ghDM = GlobalAlloc( GHND, n )))
      {
         DP1( hWDB, "GetDevMode : First call to ExtDeviceMode bad! n = %d", n ) ;
         GlobalUnlock( ghDN ) ;
         return NULL ;
      }

      lpDevMode = (LPDEVMODE)GlobalLock( ghDM ) ;

      n = DeviceCapabilitiesEx( (LPSTR)lpDN+(lpDN->wDriverOffset),
                                (LPSTR)lpDN+(lpDN->wDeviceOffset),
                                (LPSTR)lpDN+(lpDN->wOutputOffset),
                                DC_SIZE,
                                (LPSTR)lpDevMode,
                                (LPDEVMODE)lpDevMode ) ;

      if (n == GDI_ERROR)
      {
         DP1( hWDB, "GetDevMode : Second call to ExtDeviceMode bad! n = %d", n ) ;
         GlobalUnlock( ghDM ) ;
         GlobalFree( ghDM ) ;
         GlobalUnlock( ghDN ) ;
         return NULL ;
      }

      GlobalUnlock( ghDM ) ;
      GlobalUnlock( ghDN ) ;

      #endif
   }

   return ghDM ;

} /* GetDevMode()  */


/****************************************************************
 *  VOID FAR PASCAL PrnHeightToScrnHeight( HDC hdcPrn, (int FAR *)lpnHeight )
 *
 *  Description: 
 *
 *    Converts a height in printer pixels to a height in screen pixels.
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL PtoS( HDC hdcPrn, int FAR *lpnHeight )
{
   HDC      hDC ;

   if (hDC = GetDC( NULL ))
   {
      *lpnHeight = MulDiv( *lpnHeight,
                           GetDeviceCaps( hDC, LOGPIXELSY ),
                           GetDeviceCaps( hdcPrn, LOGPIXELSY ) ) ;

      ReleaseDC( NULL, hDC ) ;
   }

   return ;

} /* PrnHeightToScrnHeight()  */


/****************************************************************
 *  VOID FAR PASCAL ScrnHeightToPrnHeight( HDC hdcPrn, (int FAR *)lpnHeight )
 *
 *  Description: 
 *
 *    Converts a height in printer pixels to a height in screen pixels.
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL StoP( HDC hdcPrn, int FAR * lpnHeight )
{
   HDC      hDC ;

   if (hDC = GetDC( NULL ))
   {
      *lpnHeight = MulDiv( *lpnHeight,
                           GetDeviceCaps( hdcPrn, LOGPIXELSY ),
                           GetDeviceCaps( hDC, LOGPIXELSY ) ) ;

      ReleaseDC( NULL, hDC ) ;
   }

   return ;

} /* ScrnHeightToPrnHeight()  */


BOOL GetPageInfo( HDC hDC, PAGEPTR pPage )
{
   TEXTMETRIC  tm ;
   short       nAveCharWidth ;
   short       nLogPixelsX, nLogPixelsY ;

   nAveCharWidth = GetTextMetricsCorrectly( hDC, &tm ) ;

   nLogPixelsX = GetDeviceCaps( hDC, LOGPIXELSX ) ;
   nLogPixelsY = GetDeviceCaps( hDC, LOGPIXELSY ) ;

   pPage->nUnitsPerLine = (int)(GetDeviceCaps( hDC, HORZRES ) ) ;
   pPage->nUnitsPerPage = (int)(GetDeviceCaps( hDC, VERTRES ) ) ;

   DP4( hWDB, "tm.tmPitchAndFamily = 0x%04X", tm.tmPitchAndFamily ) ;
   if (tm.tmPitchAndFamily & 0x01)  // VARIABLE_PITCH
   {
      pPage->xChar = nAveCharWidth  ;
   }
   else
   {
      pPage->xChar = tm.tmAveCharWidth   ;
   }

   DP5( hWDB, " tmHeight = %d", tm.tmHeight ) ;
   DP5( hWDB, " tmAscent = %d", tm.tmAscent ) ;
   DP5( hWDB, " tmDescent = %d", tm.tmDescent ) ;
   DP5( hWDB, " tmExternalLeading = %d", tm.tmExternalLeading ) ;
   DP5( hWDB, " tmInternalLeading = %d", tm.tmInternalLeading ) ;

   pPage->yChar = (tm.tmHeight + tm.tmExternalLeading) ;

   pPage->nTopMargin   = MulDiv( nLogPixelsY, Options.nTopMargin, 1000 ) ;
   pPage->nBotMargin   = MulDiv( nLogPixelsY, Options.nBottomMargin, 1000 ) ;
   pPage->nLeftMargin  = MulDiv( nLogPixelsX, Options.nLeftMargin, 1000 ) ;
   pPage->nRightMargin = MulDiv( nLogPixelsX, Options.nRightMargin, 1000 ) ;

   DASSERTMSG( hWDB, pPage->xChar,
               "Divide By Zero Error (pPage->xChar)" ) ;
   if (pPage->xChar != 0)
      pPage->nCharsPerLine = (pPage->nUnitsPerLine -
                           (pPage->nLeftMargin + pPage->nRightMargin)) / pPage->xChar ;
   else
      return FALSE ;


   DASSERTMSG( hWDB, pPage->yChar,
               "Divide By Zero Error (pPage->xChar)" ) ;
   if (pPage->yChar != 0)
      pPage->nLinesPerPage = ((pPage->nUnitsPerPage -
                             (pPage->nTopMargin + pPage->nBotMargin)) / pPage->yChar) -
                             (((Options.wHeaderMask > 0) * 2) + ((Options.wFooterMask > 0) * 2));
   else
      return FALSE ;
   
   return TRUE ;
}



/****************************************************************
 *  void FAR PASCAL PlaceWindow( HWND hwndMain )
 *
 *  Description: 
 *
 *    Places the window based on the private INI file.
 *
 *  Comments:
 *
 ****************************************************************/
void FAR PASCAL PlaceWindow( HWND hwndMain )
{
   short x, y ;

   x = GetPrivateProfileInt( GRCS(IDS_INI_MAIN), GRCS(IDS_INI_MAIN_X),
                             0, GRCS(IDS_INI_FILENAME) ) ;

   //
   // return if there is nothing saved.
   //
   if (!x)
      return ;

   y = GetPrivateProfileInt( GRCS(IDS_INI_MAIN), GRCS(IDS_INI_MAIN_Y),
                             0, GRCS(IDS_INI_FILENAME) ) ;
            
   SetWindowPos( hwndMain, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER ) ;

} /* PlaceWindow()  */

/****************************************************************
 *  void FAR PASCAL WritePrivateProfileInt()
 *
 *  Description: 
 *
 *    write a integer to a INI file
 *
 *  Comments:
 *
 ****************************************************************/

void FAR PASCAL WritePrivateProfileInt(LPSTR szSection, LPSTR szKey, int i, LPSTR szIniFile)
{
    char     ach[32] ;

    if (i != (int)GetPrivateProfileInt(szSection, szKey, ~i, szIniFile))
    {
        wsprintf(ach, "%d", i);
        WritePrivateProfileString(szSection, szKey, ach, szIniFile);
    }
}

/****************************************************************
 *  void NEAR PASCAL SaveWindowPos( HWND hwndMain )
 *
 *  Description: 
 *
 *    Saves the window position.
 *
 *  Comments:
 *
 ****************************************************************/
void FAR PASCAL SaveWindowPos( HWND hwndMain )
{
   RECT     rc ;

   if (IsIconic( hwndMain ))
      return ;

   GetWindowRect( hwndMain, &rc ) ;

   WritePrivateProfileInt( GRCS(IDS_INI_MAIN), GRCS(IDS_INI_MAIN_X),
                           rc.left, GRCS(IDS_INI_FILENAME) );
   WritePrivateProfileInt( GRCS(IDS_INI_MAIN), GRCS(IDS_INI_MAIN_Y),
                           rc.top, GRCS(IDS_INI_FILENAME) );

} /* SaveWindowPos()  */

/****************************************************************
 *  VOID FAR PASCAL WriteProfileWORD( LPSTR lpszSec, LPSTR lpszKey, WORD w )
 *
 *  Description: 
 *
 *    Writes a profile entry in the form of Key=0x0000.
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL WriteProfileWORD( LPSTR lpszSec, LPSTR lpszKey, WORD w )
{
   char     ach[32] ;
   WORD     w2 ;

    if (w2 != GetProfileWORD( lpszSec, lpszKey, w ))
    {
        wsprintf( ach, GRCS(IDS_HEXWORD), w ) ;
        WritePrivateProfileString( lpszSec, lpszKey,
                                    ach, GRCS(IDS_INI_FILENAME) ) ;
    }

} /* WriteProfileWORD()  */

/****************************************************************
 *  VOID FAR PASCAL
 *    WriteProfileDWORD( LPSTR lpszSec, LPSTR lpszKey, DWORD dw )
 *
 *  Description: 
 *
 *    Writes a profile entry in the form of Key=0x00000000.
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL WriteProfileDWORD( LPSTR lpszSec, LPSTR lpszKey, DWORD dw )
{
   char     ach[32] ;
   DWORD    dw2 ;

    if (dw2 != GetProfileDWORD( lpszSec, lpszKey, dw ))
    {
        wsprintf( ach, GRCS(IDS_HEXDWORD), dw ) ;
        WritePrivateProfileString( lpszSec, lpszKey,
                                   ach, GRCS(IDS_INI_FILENAME) ) ;
    }

} /* WriteProfileDWORD()  */

/****************************************************************
 *  WORD FAR PASCAL GetProfileWORD( LPSTR lpszSec, LPSTR lpszKey, WORD wDef )
 *
 *  Description: 
 *
 *    Gets a profile entry in the form of key=0x0000
 *
 *  Comments:
 *
 ****************************************************************/
WORD FAR PASCAL GetProfileWORD( LPSTR lpszSec, LPSTR lpszKey, WORD wDef )
{
   char     ach[32] ;
   WORD     w ;

   if (GetPrivateProfileString( lpszSec, lpszKey, "",
                                ach, 32, GRCS(IDS_INI_FILENAME) ) &&
       wsscanf( ach, GRCS(IDS_HEXWORD), (LPWORD)&w ))
      return w ;
   else
      return wDef ;

} /* GetProfileWORD()  */

/****************************************************************
 *  DWORD FAR PASCAL GetProfileDWORD( LPSTR lpszSec, LPSTR lpszKey, DWORD wDef )
 *
 *  Description: 
 *
 *    Gets a profile entry in the form of key=0x0000
 *
 *  Comments:
 *
 ****************************************************************/
DWORD FAR PASCAL GetProfileDWORD( LPSTR lpszSec, LPSTR lpszKey, DWORD dwDef )
{
   char     ach[32] ;
   DWORD    dw ;

   if (GetPrivateProfileString( lpszSec, lpszKey, "",
                                ach, 32, GRCS(IDS_INI_FILENAME) ) &&
       wsscanf( ach, GRCS(IDS_HEXDWORD), (LPDWORD)&dw ))
      return dw ;
   else
      return dwDef ;

} /* GetProfileDWORD()  */

