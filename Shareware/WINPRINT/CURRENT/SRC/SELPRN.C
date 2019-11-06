/************************************************************************
 *
 *     Project:  WinPrint 1.4
 *
 *      Module:  selprn.c
 *
 *     Remarks:  Select Printer dialog box (uses COMMDLG)
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

BOOL FAR PASCAL DoPrinterSetup( VOID )
{
   PRINTDLG       pd ;
   DEVNAMES FAR * lpdn ;
   char           szDriver[_MAX_PATH] ;
   char           szDevice[32] ;
   char           szOutput[_MAX_PATH] ;
   DWORD dwErr ;

   if (ghDevNames)
   {
      lpdn = (DEVNAMES FAR *)GlobalLock( ghDevNames ) ;
      lstrcpy( szDriver, (LPSTR)lpdn+(lpdn->wDriverOffset) ) ;
      lstrcpy( szDevice, (LPSTR)lpdn+(lpdn->wDeviceOffset) ) ;
      lstrcpy( szOutput, (LPSTR)lpdn+(lpdn->wOutputOffset) ) ;
         DP1( hWDB, "Doing Print Setup: dnDriver = %s, dnDevice = %s, dnOutput = %s",
            (LPSTR)lpdn+(lpdn->wDriverOffset),
            (LPSTR)lpdn+(lpdn->wDeviceOffset),
            (LPSTR)lpdn+(lpdn->wOutputOffset) ) ;
      GlobalUnlock( ghDevNames ) ;
   }
   else
   {
      szDriver[0] = '\0' ;
      szDevice[0] = '\0' ;
      szOutput[0] = '\0' ;
   }

DoPrintDlg:

   /*
    * Use commdlgs to change the printer
    */
   memset( &pd, '\0', sizeof( PRINTDLG ) ) ;

   DP1( hWDB, "Before:  ghDevMode = 0x%04X, ghDevNames = 0x%04X",
               ghDevMode, ghDevNames ) ;

   pd.lStructSize = sizeof( PRINTDLG ) ;
   pd.hwndOwner = hwndMain ;
   pd.hDevMode = ghDevMode ;
   pd.hDevNames = ghDevNames ;
   pd.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNIC ;
//   pd.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_PRINTSETUP | PD_RETURNIC ;
   pd.nCopies = 0 ;
   pd.hInstance = NULL ;

   if(Options.bPrintToFile)
     pd.Flags |= PD_PRINTTOFILE ;
    
   if (PrintDlg( &pd ) != 0)
   {
      /*
       * Set our internal vars to match those returned...
       */

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
      {
         Options.lfCurFont.lfHeight = MulDiv( Options.lfCurFont.lfHeight,
                           GetDeviceCaps( pd.hDC, LOGPIXELSY ),
                           nLogicalPixelsY ) ;
         Options.lfHFFont.lfHeight = MulDiv( Options.lfHFFont.lfHeight,
                           GetDeviceCaps( pd.hDC, LOGPIXELSY ),
                           nLogicalPixelsY ) ;
         DeleteDC( pd.hDC ) ;
      }
      
      Options.bPrintToFile = (pd.Flags & PD_PRINTTOFILE ? TRUE : FALSE) ;

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
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
                         IDS_PDERR_PRINTERCODES ) ;
         break ;

         case PDERR_SETUPFAILURE:
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
                         IDS_PDERR_SETUPFAILURE ) ;
         break ;

         case PDERR_PARSEFAILURE:
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
                         IDS_PDERR_PARSEFAILURE ) ;
         break ;

         case PDERR_RETDEFFAILURE:
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
                         IDS_PDERR_RETDEFFAILURE ) ;
         break ;

         case PDERR_LOADDRVFAILURE:
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
                         IDS_PDERR_LOADDRVFAILURE ) ;
         break ;

         case PDERR_GETDEVMODEFAIL:
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
                         IDS_PDERR_GETDEVMODEFAIL ) ;
         break ;

         case PDERR_INITFAILURE:
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
                         IDS_PDERR_INITFAILURE ) ;
         break ;

         case PDERR_NODEVICES:
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
                         IDS_PDERR_NODEVICES ) ;
         break ;

         case PDERR_NODEFAULTPRN:
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
                         IDS_PDERR_NODEFAULTPRN ) ;
         break ;

         case PDERR_DNDMMISMATCH:
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
                         IDS_PDERR_DNDMMISMATCH ) ;
         break ;

         case PDERR_CREATEICFAILURE:
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
                         IDS_PDERR_CREATEICFAILURE ) ;
         break ;

         case PDERR_PRINTERNOTFOUND:
            DP1( hWDB, "ghDevnames = %d", ghDevNames ) ;
            DP1( hWDB, "ghDevMode = %d", ghDevMode ) ;
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
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
            ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION, IDS_APPNAME,
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

}


/************************************************************************
 * End of File: selprn.c
 ***********************************************************************/

