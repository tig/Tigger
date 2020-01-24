// selprn.c
//
// Select Printer Dialog
//
#include <windows.h>
#include <drivinit.h>
#include <string.h>
#include "print.h"
#include "selprn.h"

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
   GLOBALHANDLE hPrinterMem ;
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
      hPrinterMem = GlobalAlloc( GHND, MAXDRIVERLEN+MAXDEVICELEN+MAXPORTLEN+2 ) ;
      if (!hPrinterMem)
         return DLG_ERROR ;

      lpszPrinter = (LPSTR)GlobalLock( hPrinterMem ) ;
      wsprintf( lpszPrinter, (LPSTR)HIDDENLISTBOXFMT,
                             (LPSTR)lpszDriverName,
                             (LPSTR)lpszDeviceName,
                             (LPSTR)lpszPortName ) ;
      // i.e. "HPPLC,PCL/LaserJet III,LPT1:"
      GlobalUnlock( hPrinterMem ) ;

      lpfnDlgProc = MakeProcInstance( SelPrinterDlgProc, hInst ) ;
      n = DialogBoxParam(  hInst,      
                           "SELECTPRINTER",
                           hwndParent,        
                           lpfnDlgProc,
                           MAKELONG( hPrinterMem, wFlags ) ) ;
      FreeProcInstance( lpfnDlgProc ) ;
      switch (n)
      {
         case DLG_OK:
            lpszPrinter = (LPSTR)GlobalLock( hPrinterMem ) ;
            lpszDrv = _fstrtok( lpszPrinter, "," ) ;
            lpszDev = _fstrtok( NULL, "," ) ;
            lpszPort = _fstrtok( NULL, "," ) ;

            lstrcpy( lpszDriverName, lpszDrv ) ;
            lstrcpy( lpszDeviceName, lpszDev ) ;
            lstrcpy( lpszPortName, lpszPort ) ;

            GlobalUnlock( hPrinterMem ) ;
            // Fall through

         case DLG_CANCEL:
         default:
            GlobalFree( hPrinterMem ) ;
      }
   }
   return n ;
}

   char szPrinter [MAXPRINTERLEN] ;
   LPSTR lpsDrv, lpsDev, lpsPort ;

int FAR PASCAL
    PrintGetDefaultPrn( LPSTR lpszDriverName,
                        LPSTR lpszDeviceName,
                        LPSTR lpszPortName )
{
   GetProfileString( "windows", "device", "",
                     (LPSTR)szPrinter, MAXPRINTERLEN ) ;
   if ((lpsDev  = strtok( szPrinter, "," )) &&
       (lpsDrv  = strtok( NULL,      "," )) &&
       (lpsPort = strtok( NULL,      "," )))
   {
      lstrcpy( lpszDriverName, lpsDrv ) ;
      lstrcpy( lpszDeviceName, lpsDev ) ;
      lstrcpy( lpszPortName, lpsPort ) ;
   }
   else
      return DLG_ERROR ;

   return DLG_OK ;
}

//--------------------------------------------------------------------------
// SelPrinterDlgProc
//
// By CEK 10/25/90
//--------------------------------------------------------------------------                   
BOOL FAR PASCAL SelPrinterDlgProc( HWND hDlg,
                                   unsigned iMessage,
                                   WORD wParam,
                                   LONG lParam )
{
static HWND hList,
            hHidden ;

static GLOBALHANDLE hDeviceMem ;
static GLOBALHANDLE hPrinterMem ;

FARPROC     lpfnDM ;
LPSTR       lpsAllDevices ;
LPSTR       lpsCurrent, lpsDevice, lpsDriver, lpsPort ;
HANDLE      hLibrary ;

char        szBuf [MAXPRINTERLEN] ;
char        szTemp [MAXPRINTERLEN] ;
int         n ;

   switch (iMessage)
   {
      case WM_INITDIALOG:
         hPrinterMem = (HANDLE)LOWORD( lParam ) ;

         D(dp( "SelPrinterDlgProc( %d, %d, %d, %d )",
               hDlg, iMessage, wParam, lParam )) ;

         hList = GetDlgItem( hDlg, IDD_LIST ) ;
         hHidden = GetDlgItem( hDlg, IDD_HIDDENLIST ) ;

         if (!(HIWORD( lParam ) & SELPRN_ALLOWSETUP))
            ShowWindow( GetDlgItem( hDlg, IDD_SETUP ), SW_HIDE ) ;

         ShowWindow( hHidden, SW_HIDE ) ;

      case WM_WININICHANGE:
         if (!LOWORD( lParam ) == (WORD)hPrinterMem)
            if (lParam != NULL && lstrcmp( (LPSTR) lParam, "devices" ) != 0)
               break;
         SendMessage( hList, LB_RESETCONTENT, 0, 0L ) ;
         SendMessage( hHidden, LB_RESETCONTENT, 0, 0L ) ;

         hDeviceMem = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT,
                                   MAXALLDEVICELEN ) ;
         if (!hDeviceMem)
         {
            D(dp( "Could not alloc global memory for all device list" )) ;
            EndDialog( hDlg, DLG_ERROR ) ;
            break ;
         }
         lpsAllDevices = (LPSTR)GlobalLock( hDeviceMem ) ;

         GetProfileString( "devices", NULL, "ERROR!", lpsAllDevices,
                           MAXALLDEVICELEN ) ;
         if (*lpsAllDevices)
            for (lpsDevice = lpsAllDevices ;
                 *lpsDevice != '\0' ;
                 lpsDevice += lstrlen (lpsDevice)+1)
            {
               GetProfileString( "devices", lpsDevice, "ERROR",
                                 szTemp, 255 ) ;

               lpsDriver = _fstrtok( szTemp, "," ) ;
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
                  }
               }
            }

         else
         {
            MessageBox( hDlg, "There are no printers listed in WIN.INI",
                        "SelectPrinter", MB_OK | MB_ICONEXCLAMATION ) ;
            GlobalUnlock( hDeviceMem ) ;
            GlobalFree( hDeviceMem ) ;
            EndDialog( hDlg, DLG_ERROR ) ;
            break ;
         }
         GlobalUnlock( hDeviceMem ) ;
         GlobalFree( hDeviceMem ) ;

         lpsCurrent = (LPSTR)GlobalLock( hPrinterMem ) ;

         lpsDriver = _fstrtok( lpsCurrent, ",") ;
         lpsDevice = _fstrtok( NULL, ",") ;
         lpsPort = _fstrtok( NULL, ",") ;

         wsprintf( (LPSTR)szBuf, (LPSTR)LISTBOXFMT,
                                 (LPSTR)lpsDevice,
                                 (LPSTR)lpsDriver,
                                 (LPSTR)lpsPort ) ;
         GlobalUnlock( hPrinterMem ) ;
         
         if (LB_ERR == SendMessage( hList, LB_SELECTSTRING, 0,
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

               lpsCurrent = (LPSTR)GlobalLock( hPrinterMem ) ;
               lstrcpy( lpsCurrent, szBuf ) ;
               GlobalUnlock( hPrinterMem ) ;

               EndDialog( hDlg, DLG_OK ) ;
               break ;

            case IDD_SETUP:
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

               wsprintf( szBuf, (LPSTR)"%s.DRV", (LPSTR)lpsDriver ) ;
               hLibrary = LoadLibrary( szBuf ) ;
               if (hLibrary >= 32)
               {
                  lpfnDM = GetProcAddress( hLibrary, "DEVICEMODE" ) ;
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
               break ;

            case IDCANCEL:
               EndDialog( hDlg, DLG_CANCEL ) ;
               break ;

            default:
               return FALSE ;
         }
      default:
         return FALSE ;
   }
   return TRUE ;

}

