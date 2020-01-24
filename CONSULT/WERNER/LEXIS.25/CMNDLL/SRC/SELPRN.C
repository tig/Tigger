/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CMNDLL, Common DLL for Werner Apps
 *
 *      Module:  selprn.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/


#define WINDLL
#include <windows.h>
#include <drivinit.h>
#include <string.h>

#include "..\inc\cmndll.h"
// Include the dialog defines
#include "..\inc\cmndlld.h"

//------------------------------------------------------------------
// Code defines
//------------------------------------------------------------------
#define LISTBOXFMT   "%s (%s) on %s"
#define HIDDENLISTBOXFMT "%s,%s,%s"
#define MAXDEVICELEN 64 // Device Name (i.e. PC/LaserJet II)
#define MAXDRIVERLEN 13 // Driver name (i.e. HPPCL (NULL included))
#define MAXPORTLEN   140 // DOS Port name. 127 for path, 13 for filename

// Maximum printer string
// Format:
// DEVICENAME (DRIVERNAME) on PORTNAME
// Example: "PCL/LaserJet II (HPPCL) on LPT1:"
#define MAXPRINTERLEN MAXDEVICELEN+MAXDRIVERLEN+MAXPORTLEN+6

// Maximum size of the [device] section
#define MAXALLDEVICELEN  20 * MAXPRINTERLEN 

// Property list strings
#define PRINTER_PROP "printer"
#define LIST_PROP    "list"
#define HIDDEN_PROP  "hidden"

//
// DeviceMode is exported from the printer drivers.  This set's up
// a function prototype for it.  The prototypes for ExtDeviceMode
// and DeviceCapabilities are similarly defined in 
// \windev\include\driveinit.h
//
typedef WORD FAR PASCAL FNOLDDEVMODE(HWND, HANDLE, LPSTR, LPSTR ) ;
typedef FNOLDDEVMODE FAR * LPFNOLDDEVMODE;

//------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------
extern HANDLE hInst ;

//------------------------------------------------------------------------
//  int FAR PASCAL
//     PrintSelPrnDialog( HWND hwndParent,
//                        LPSTR lpszTitle,
//                        LPSTR lpszDriverName,
//                        LPSTR lpszDeviceName,
//                        LPSTR lpszPortName,                       
//                        WORD  wFlags )
//
// Displays a dialog box containing a listbox of all the currently
// installed printers, allowing the user to select one.  Returns
// the drivername, devicename, and port for the selected printer.
// 
// Type/Parameter
//          Description
// 
// HWND hwndParent
//          Specifies the window to be used as the parent of the dialog box.
//          This must be a valid window handle.
// 
// LPSTR lpszTitle
//          Specifies the title of the dialog box.  May be NULL if no title
//          is desired.  This string is not altered.
// 
// LPSTR lpszDriverName
//          Contains the current driver name (i.e. HPPCL).  This string
//          is used in conjunction with lpszDeviceName and lpszPortName to
//          select the current item in the list box.  Upon return, this string
//          contains the driver name of the selected printer.
// 
// LPSTR lpszDeviceName
//          Contains the current device name (i.e. PCL/HP LaserJet II).
//          This string is used in conjunction with the lpszDriverName and
//          lpszPortName ot select the current itme in the list box.  Upon
//          return, this string contains the device name of the selected
//          printer.
// 
// LPSTR lpszPortName
//          Contains the current printer port or output file name (i.e.
//          LPT1: or OUTPUT.PRN).  This string is used in conjunction
//          with the lpszDriverName and lpszDeviceName to select the
//          current item in the list box.  Upon return this string contians
//          the port or output file name of the selected printer.
// 
// WORD wFlags
//          Specifies flags, and may be any of the following.
// 
//          Value
//                Meaning
// 
//          SELPRN_STANDARD
//                The caller specifies lpszDriverName, lpszDeviceName, and
//                lpszPortName and the dialog box is displayed with the printer
//                corresponding to the parameters selected.
//                If the SELPRN_USEDEFAULT flag is set the parameters 
//                specified by the caller are ignored.
// 
//          SELPRN_GETDEFAULT
//                The default printer as specified on the "device=" line
//                of the [windows] section of the WIN.INI file is returned.
//                The dialog box is not displayed.  No other flags may be
//                specified when the SELPRN_GETDEFAULT flag is used.
// 
//          SELPRN_USEDEFAULT
//                the default printer as specified on the "device=" line
//                of the [windows] section of the WIN.INI file is used
//                as the current printer as though it were passed by the
//                caller.  The dialog box is displayed with the current
//                printer selected in the list box.
// 
//          SELPRN_ALLOWSETUP
//                Causes a "Setup" button to be displayed and enabled.  If the
//                user presses the Setup button the currently selected printer
//                drivers DeviceMode function will be called, allowing the
//                user to setup the printer's global settings.
// 
// Return Value
//          Returns one of the following values:
// 
//             Value
//                      Meaning
// 
//             DLG_CANCEL
//                      The user pressed the Cabcel button.  The
//                      contents of the lpszDriverName, lpszDeviceName,
//                      and lpszPortName remain unchanged.
// 
//             DLG_OK
//                      The user pressed the OK button.  The contents
//                      of the lpszDriverName, lpszDeviceName, and lpszPortName
//                      parameters contain the selected printer info.  DLG_OK
//                      is returned in the case of a successful call with
//                      the SLEPRN_GETDEFAULT flag specified.
// 
//             DLG_ERROR
//                      An error occured reading the WIN.INI file, calling
//                      DeviceMode, creating the dialog box, or other reasons.
// 
// Comments
//    This function does not currently support ExtDeviceMode.  There may
//    be a flag to support this in the future.
// 
// Modifications
//    CEK     10/25/90     First version.
//
//---------------------------------------------------------------------------
int FAR PASCAL
    PrintSelPrnDialog( HWND hwndParent,
                       LPSTR lpszTitle,
                       LPSTR lpszDriverName,
                       LPSTR lpszDeviceName,
                       LPSTR lpszPortName,                       
                       WORD  wFlags )
{
   FARPROC  lpfnDlgProc ;
   int      n ;
   LOCALHANDLE hPrinterMem ;
   LPSTR    lpszPrinter ;
   LPSTR    lpszDrv, lpszDev, lpszPort ;


   if (wFlags & SELPRN_GETDEFAULT)
      return PrintGetDefaultPrn( lpszDriverName, lpszDeviceName, lpszPortName ) ;

   if (wFlags & SELPRN_USEDEFAULT)
      if (DLG_ERROR == PrintGetDefaultPrn( lpszDriverName, lpszDeviceName,
                                           lpszPortName ))
         return DLG_ERROR ;

   if (wFlags & SELPRN_STANDARD)
   {
      hPrinterMem = LocalAlloc( LHND, MAXDRIVERLEN+MAXDEVICELEN+MAXPORTLEN+2 ) ;
      if (!hPrinterMem)
         return DLG_ERROR ;

      lpszPrinter = (LPSTR)LocalLock( hPrinterMem ) ;
      wsprintf( lpszPrinter, (LPSTR)HIDDENLISTBOXFMT,
                             (LPSTR)lpszDriverName,
                             (LPSTR)lpszDeviceName,
                             (LPSTR)lpszPortName ) ;

      // i.e. "HPPLC,PCL/LaserJet III,LPT1:"
      LocalUnlock( hPrinterMem ) ;

      if (lpfnDlgProc = MakeProcInstance( SelPrinterDlgProc, hInst ))
      {
         n = DialogBoxParam(  hInst,		
                              "SELECTPRINTER",
                              hwndParent,			 
                              lpfnDlgProc,
                              MAKELONG( hPrinterMem, wFlags ) ) ;
         FreeProcInstance( lpfnDlgProc ) ;
      }
      else
         n = DLG_ERROR ;

      switch (n)
      {
         case DLG_OK:
            lpszPrinter = (LPSTR)LocalLock( hPrinterMem ) ;
            lpszDrv = _fstrtok( lpszPrinter, "," ) ;
            lpszDev = _fstrtok( NULL, "," ) ;
            lpszPort = _fstrtok( NULL, "," ) ;

            lstrcpy( lpszDriverName, lpszDrv ) ;
            lstrcpy( lpszDeviceName, lpszDev ) ;
            lstrcpy( lpszPortName, lpszPort ) ;

            LocalUnlock( hPrinterMem ) ;
            // Fall through

         case DLG_CANCEL:
         default:
            LocalFree( hPrinterMem ) ;
      }
   }
   return n ;
}

int FAR PASCAL
    PrintGetDefaultPrn( LPSTR lpszDriverName,
                        LPSTR lpszDeviceName,
                        LPSTR lpszPortName )
{
   char szPrinter [MAXPRINTERLEN] ;
   LPSTR lpsDrv, lpsDev, lpsPort ;

   GetProfileString( "windows", "device", "",
                     (LPSTR)szPrinter, MAXPRINTERLEN ) ;
   if ((lpsDev  = _fstrtok( szPrinter, "," )) &&
       (lpsDrv  = _fstrtok( NULL,      "," )) &&
       (lpsPort = _fstrtok( NULL,      "," )))
   {
      lstrcpy( lpszDriverName, lpsDrv ) ;
      lstrcpy( lpszDeviceName, lpsDev ) ;
      lstrcpy( lpszPortName, lpsPort ) ;
   }
   else
      return DLG_ERROR ;

   return DLG_OK ;
}

/*************************************************************************
 *  HDC FAR PASCAL GetDefaultPrnDC( void )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
HDC FAR PASCAL GetDefaultPrnDC( void )
{
   char szDriver[MAXDRIVERLEN] ;
   char szDevice[MAXDEVICELEN] ;
   char szPort[MAXPORTLEN] ;

   if (DLG_OK != PrintGetDefaultPrn( (LPSTR)szDriver,
                                    (LPSTR)szDevice, (LPSTR)szPort ))
      return NULL ;

   return CreateDC( szDriver, szDevice, szPort, NULL ) ;

}/* GetDefaultPrnDC() */

//--------------------------------------------------------------------------
// SelPrinterDlgProc
//
// By CEK 10/25/90
// 11/19/90 - Implemented Properties
//            Changed global allocs to local allocs
//--------------------------------------------------------------------------                   
BOOL FAR PASCAL SelPrinterDlgProc( HWND hDlg,
                                   WORD wMsg,
                                   WORD wParam,
                                   LONG lParam )
{
   HWND        hList,           // Stored as a property
               hHidden ;

   LOCALHANDLE hDeviceMem ;     // Stored in each instances Props
   LOCALHANDLE hPrinterMem ;   

   LPSTR       lpsAllDevices ;
   LPSTR       lpsCurrent, lpsDevice, lpsDriver, lpsPort ;
   HANDLE      hLibrary ;

   char        szBuf [MAXPRINTERLEN] ;
   char        szTemp [MAXPRINTERLEN] ;
   int         n ;


   // Get handles from the property lists
   hPrinterMem = GetProp( hDlg, PRINTER_PROP ) ;
   hList = GetProp( hDlg, LIST_PROP ) ;
   hHidden = GetProp( hDlg, HIDDEN_PROP ) ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
         DP5( hWDB, "SelPrinterDlgProc( %d, %d, %d, %d )",
               hDlg, wMsg, wParam, lParam ) ;

         hPrinterMem = (HANDLE)LOWORD( lParam ) ;

         hList = GetDlgItem( hDlg, IDD_LIST ) ;
         hHidden = GetDlgItem( hDlg, IDD_HIDDENLIST ) ;

         if (!(HIWORD( lParam ) & SELPRN_ALLOWSETUP))
            ShowWindow( GetDlgItem( hDlg, IDD_SETUP ), SW_HIDE ) ;

         ShowWindow( hHidden, SW_HIDE ) ;

         // Place hPrinterMem in the windows property list
         if (!SetProp( hDlg, PRINTER_PROP, hPrinterMem ))
         {
            DP1( hWDB,  "SelPrinterDlgProc: Could not SetProp(hPrinterMem)" ) ;
            return FALSE ;
         }

         // Place hList in the windows property list
         if (!SetProp( hDlg, LIST_PROP, hList ))
         {
            DP1( hWDB,  "SelPrinterDlgProc: Could not SetProp(hList)" ) ;
            return FALSE ;
         }

         // Place hHidden in the windows property list
         if (!SetProp( hDlg, HIDDEN_PROP, hHidden ))
         {
            DP1( hWDB, "SelPrinterDlgProc: Could not SetProp(hHidden)" ) ;
            return FALSE ;
         }

         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;

         ShadowControl( hDlg, wMsg, wParam, RGBCYAN ) ;
         // Fall through!

      case WM_WININICHANGE:
         if (!(LOWORD( lParam ) == (WORD)hPrinterMem))
            if (lParam != NULL && lstrcmp( (LPSTR) lParam, "devices" ) != 0)
               break;
         SendMessage( hList, LB_RESETCONTENT, 0, 0L ) ;
         SendMessage( hHidden, LB_RESETCONTENT, 0, 0L ) ;

         hDeviceMem = LocalAlloc( LMEM_MOVEABLE | LMEM_ZEROINIT,
                                   MAXALLDEVICELEN ) ;
         if (!hDeviceMem)
         {
            DP1( hWDB, "Could not alloc global memory for all device list" ) ;
            EndDialog( hDlg, DLG_ERROR ) ;
            break ;
         }
         lpsAllDevices = (LPSTR)LocalLock( hDeviceMem ) ;

         GetProfileString( "devices", NULL, "ERROR!", lpsAllDevices,
                           MAXALLDEVICELEN ) ;
         if (*lpsAllDevices)
            for (lpsDevice = lpsAllDevices ;
                 *lpsDevice != '\0' ;
                 lpsDevice += lstrlen (lpsDevice)+1)
            {
               GetProfileString( "devices", lpsDevice, "ERROR",
                                 szTemp, 255 ) ;

               if (!(lpsDriver = _fstrtok( szTemp, "," )))
                  break ;

               while (lpsPort = _fstrtok( NULL, "," ))
               {
                  if (lstrcmpi(lpsPort, "None"))
                  {   
                     wsprintf( (LPSTR)szBuf,
                              (LPSTR)LISTBOXFMT,
                              (LPSTR)lpsDevice,
                              (LPSTR)lpsDriver,
                              (LPSTR)lpsPort ) ;
                     SendMessage( hList, LB_ADDSTRING, 0, (LONG)(LPSTR) szBuf ) ;
                     wsprintf( szBuf, (LPSTR)HIDDENLISTBOXFMT,
                                    (LPSTR)lpsDriver,
                                    (LPSTR)lpsDevice,
                                    (LPSTR)lpsPort ) ;
                     SendMessage( hHidden, LB_ADDSTRING, 0, (LONG)(LPSTR) szBuf ) ;
                     DP5( hWDB, szBuf ) ;
                  }
               }
            }

         else
         {
            MessageBox( hDlg, "There are no printers listed in WIN.INI",
                        "SelectPrinter", MB_OK | MB_ICONEXCLAMATION ) ;
            LocalUnlock( hDeviceMem ) ;
            LocalFree( hDeviceMem ) ;
            EndDialog( hDlg, DLG_ERROR ) ;
            break ;
         }
         LocalUnlock( hDeviceMem ) ;
         LocalFree( hDeviceMem ) ;

         lpsCurrent = (LPSTR)LocalLock( hPrinterMem ) ;

         lpsDriver = _fstrtok( lpsCurrent, ",") ;
         lpsDevice = _fstrtok( NULL, ",") ;
         lpsPort = _fstrtok( NULL, ",") ;

         if (lpsDriver && lpsDevice && lpsPort)
            wsprintf( (LPSTR)szBuf, (LPSTR)LISTBOXFMT,
                                    (LPSTR)lpsDevice,
                                    (LPSTR)lpsDriver,
                                    (LPSTR)lpsPort ) ;

         LocalUnlock( hPrinterMem ) ;
         
         if (LB_ERR == (int)SendMessage( hList, LB_SELECTSTRING, 0,
                                    (LONG)(LPSTR) szBuf ))
         {
            EnableWindow( GetDlgItem( hDlg, IDOK ), FALSE ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_SETUP ), FALSE ) ;
         }
         else
         {
            EnableWindow( GetDlgItem( hDlg, IDOK ), TRUE ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_SETUP ), TRUE ) ;
         }

         break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDD_LIST:
               EnableWindow( GetDlgItem( hDlg, IDOK ), TRUE ) ;
               EnableWindow( GetDlgItem( hDlg, IDD_SETUP ), TRUE ) ;
               if (HIWORD (lParam) != LBN_DBLCLK)
                  break ;
               /* fall through */

            case IDOK:
do_ok:
               if (LB_ERR == (WORD)SendMessage( hHidden, LB_GETTEXT,
                             (WORD)SendMessage( hList, LB_GETCURSEL, 0, 0L ),
                             (LONG)(LPSTR)szBuf ))
               {
                  MessageBeep( MB_ICONEXCLAMATION ) ;
                  break ;
               }

               // Remove hPrinterMem from the windows property list
               RemoveProp( hDlg, PRINTER_PROP ) ;

               // Remove hList from the windows property list
               RemoveProp( hDlg, LIST_PROP ) ;

               // Remove hHidden from the windows property list
               RemoveProp( hDlg, HIDDEN_PROP ) ;

               lpsCurrent = (LPSTR)LocalLock( hPrinterMem ) ;
               lstrcpy( lpsCurrent, szBuf ) ;
               LocalUnlock( hPrinterMem ) ;

               EndDialog( hDlg, DLG_OK ) ;
               break ;

            case IDD_SETUP:
            {
               LPFNOLDDEVMODE  lpfnDM ;

               if (LB_ERR == (WORD)SendMessage( hHidden, LB_GETTEXT,
                            (WORD)SendMessage( hList, LB_GETCURSEL, 0, 0L ),
                            (LONG)(LPSTR)szBuf ))
               {
                  MessageBeep( MB_ICONEXCLAMATION ) ;
                  break ;
               }
               lpsDriver = _fstrtok( szBuf, ",") ;
               lpsDevice = _fstrtok( NULL, ",") ;
               lpsPort = _fstrtok( NULL, ",") ;

               if (lpsDriver && lpsDevice && lpsPort)
               {
                  wsprintf( szTemp, (LPSTR)"%s.DRV", (LPSTR)lpsDriver ) ;
                  hLibrary = LoadLibrary( szTemp ) ;
                  if (hLibrary >= 32)
                  {
                     //
                     // PROC_OLDDEVICEMODE is in \windev\include\driveinit.h
                     //
                     if (lpfnDM = GetProcAddress( hLibrary, PROC_OLDDEVICEMODE ))
                        n = (*lpfnDM)( hDlg, hLibrary, (LPSTR)lpsDevice,
                                                      (LPSTR)lpsPort ) ;

                     FreeLibrary( hLibrary ) ;
                  }
                  else
                  {
                     MessageBox( hDlg, "Could not load Device Driver",
                                 "SelectPrinter", MB_OK | MB_ICONEXCLAMATION ) ;
                     break ;
                  }
                  if (n == IDOK)
                     goto do_ok ;
               }
            }
            break ;

            case IDCANCEL:
               // Remove hPrinterMem from the windows property list
               RemoveProp( hDlg, PRINTER_PROP ) ;

               // Remove hList from the windows property list
               RemoveProp( hDlg, LIST_PROP ) ;

               // Remove hHidden from the windows property list
               RemoveProp( hDlg, HIDDEN_PROP ) ;

               EndDialog( hDlg, DLG_CANCEL ) ;
               break ;

            default:
               return FALSE ;
         }

      default:
         return ShadowControl( hDlg, wMsg, wParam, lParam ) ;
   }
   return TRUE ;

}
/************************************************************************
 * End of File: selprn.c
 ************************************************************************/

