/********************************************************************         
 *
 *  SELPRN.C
 *
 *    Select Printer Dialog box routines.
 *
 *    These routines are general purpose routines for allowing the
 *    user to choose a printer, and set up print options on that
 *    printer.
 *
 *    These routines are fully DLLable (i.e. this module can
 *    be placed in a DLL without changing it).
 *
 *    See the comment blocks before each function for documentation
 *    on the use of each function.
 *
 * Revisions:
 *    2/26/91 - First version for Software Libarary (cek).
 *
 ********************************************************************/

#define WINDLL

#include <windows.h>
#include <drivinit.h>
#include <string.h>
#include "selprn.h"

/*******************************************************************
 * Code defines
 ******************************************************************/
#define LISTBOXFMT         "%s (%s) on %s"
#define HIDDENLISTBOXFMT   "%s,%s,%s"
#define MAXDEVICELEN       64   // Device Name (i.e. PC/LaserJet II)
#define MAXDRIVERLEN       13   // Driver name (i.e. HPPCL (NULL included))
#define MAXPORTLEN         140  // DOS Port name. 127 for path, 13 for filenam

// Maximum printer string
// Format:
// DEVICENAME (DRIVERNAME) on PORTNAME
// Example: "PCL/LaserJet II (HPPCL) on LPT1:"
#define MAXPRINTERLEN MAXDEVICELEN+MAXDRIVERLEN+MAXPORTLEN+6

// Maximum size of the [device] section
#define MAXALLDEVICELEN  20 * MAXPRINTERLEN 

//
// Property list strings.  Used so multiple instances can use this
// if in a DLL.
//
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

/******************************************************************
 * Global Variables
 *****************************************************************/
//
// This is the handle of the module that contains the dialog box
// resource.  It should be the hModule of the DLL if this is implemented
// in a DLL or it should be the hInstance of the applciation if it is 
// not.
//
extern HANDLE hInst ;

/************************************************************************
 *
 *  int FAR PASCAL
 *     PrintSelPrnDialog( HWND hwndParent,
 *                        LPSTR lpszTitle,
 *                        LPSTR lpszDriverName,
 *                        LPSTR lpszDeviceName,
 *                        LPSTR lpszPortName,                       
 *                        WORD  wFlags )
 *
 * Displays a dialog box containing a listbox of all the currently
 * installed printers, allowing the user to select one.  Returns
 * the drivername, devicename, and port for the selected printer.
 * 
 * Type/Parameter
 *          Description
 * 
 * HWND hwndParent
 *          Specifies the window to be used as the parent of the dialog box.
 *          This must be a valid window handle.
 * 
 * LPSTR lpszTitle
 *          Specifies the title of the dialog box.  May be NULL if no title
 *          is desired.  This string is not altered.
 * 
 * LPSTR lpszDriverName
 *          Contains the current driver name (i.e. HPPCL).  This string
 *          is used in conjunction with lpszDeviceName and lpszPortName to
 *          select the current item in the list box.  Upon return, this string
 *          contains the driver name of the selected printer.
 * 
 * LPSTR lpszDeviceName
 *          Contains the current device name (i.e. PCL/HP LaserJet II).
 *          This string is used in conjunction with the lpszDriverName and
 *          lpszPortName ot select the current itme in the list box.  Upon
 *          return, this string contains the device name of the selected
 *          printer.
 * 
 * LPSTR lpszPortName
 *          Contains the current printer port or output file name (i.e.
 *          LPT1: or OUTPUT.PRN).  This string is used in conjunction
 *          with the lpszDriverName and lpszDeviceName to select the
 *          current item in the list box.  Upon return this string contians
 *          the port or output file name of the selected printer.
 * 
 * WORD wFlags
 *          Specifies flags, and may be any of the following.
 * 
 *          Value
 *                Meaning
 * 
 *          SELPRN_STANDARD
 *                The caller specifies lpszDriverName, lpszDeviceName, and
 *                lpszPortName and the dialog box is displayed with the printe
 *                corresponding to the parameters selected.
 *                If the SELPRN_USEDEFAULT flag is set the parameters 
 *                specified by the caller are ignored.
 * 
 *          SELPRN_GETDEFAULT
 *                The default printer as specified on the "device=" line
 *                of the [windows] section of the WIN.INI file is returned.
 *                The dialog box is not displayed.  No other flags may be
 *                specified when the SELPRN_GETDEFAULT flag is used.
 * 
 *          SELPRN_USEDEFAULT
 *                the default printer as specified on the "device=" line
 *                of the [windows] section of the WIN.INI file is used
 *                as the current printer as though it were passed by the
 *                caller.  The dialog box is displayed with the current
 *                printer selected in the list box.
 * 
 *          SELPRN_ALLOWSETUP
 *                Causes a "Setup" button to be displayed and enabled.  If the
 *                user presses the Setup button the currently selected printer
 *                drivers DeviceMode function will be called, allowing the
 *                user to setup the printer's global settings.
 * 
 * Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 * 
 *             DLG_CANCEL
 *                      The user pressed the Cabcel button.  The
 *                      contents of the lpszDriverName, lpszDeviceName,
 *                      and lpszPortName remain unchanged.
 * 
 *             DLG_OK
 *                      The user pressed the OK button.  The contents
 *                      of the lpszDriverName, lpszDeviceName, and lpszPortNam
 *                      parameters contain the selected printer info.  DLG_OK
 *                      is returned in the case of a successful call with
 *                      the SLEPRN_GETDEFAULT flag specified.
 * 
 *             DLG_ERROR
 *                      An error occured reading the WIN.INI file, calling
 *                      DeviceMode, creating the dialog box, or other reasons.
 * 
 * Modifications
 *    CEK     10/25/90     First version.
 *    CEK     2/28/91      Added use of properties to make DLLable
 *
 ***************************************************************************/
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

/**************************************************************************
 *    PrintGetDefaultPrn
 *
 *    Gets the default printer from the WIN.INI file.
 *
 *************************************************************************/
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
 *    Gets the DC of the default printer.  Not really needed by SelPrn,
 *    but useful.
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


/**************************************************************************
 *
 * SelPrinterDlgProc
 *
 *    This dialog box imploys a neat trick of using a hidden list box
 *    to manage the list of printers.
 *
 *    Since we want to display the
 *    printer strings in a nice format (i.e. "PostScript Printer on LPT1:")
 *    instead of "PostScriptPrinter,PSCRIPT,LPT1:" we place the unformatted
 *    strings in the hidden listbox and the formatted ones in the visible
 *    list box.  We insert and select from these two list boxes
 *    in parallel.
 *
 *************************************************************************/
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
         hPrinterMem = (HANDLE)LOWORD( lParam ) ;

         hList = GetDlgItem( hDlg, IDD_LIST ) ;
         hHidden = GetDlgItem( hDlg, IDD_HIDDENLIST ) ;

         if (!(HIWORD( lParam ) & SELPRN_ALLOWSETUP))
            ShowWindow( GetDlgItem( hDlg, IDD_SETUP ), SW_HIDE ) ;

         //
         // Hide the extra List box
         //
         ShowWindow( hHidden, SW_HIDE ) ;

         // Place hPrinterMem in the windows property list
         if (!SetProp( hDlg, PRINTER_PROP, hPrinterMem ))
         {
            return FALSE ;
         }

         // Place hList in the windows property list
         if (!SetProp( hDlg, LIST_PROP, hList ))
         {
            return FALSE ;
         }

         // Place hHidden in the windows property list
         if (!SetProp( hDlg, HIDDEN_PROP, hHidden ))
         {
            return FALSE ;
         }

         // Fall through!

      case WM_WININICHANGE:
         //
         // If the [devices] section of the WIN.INI changes, we need to
         // know about it so we can update our list.
         //
         if (!(LOWORD( lParam ) == (WORD)hPrinterMem))
            if (lParam != NULL && lstrcmp( (LPSTR) lParam, "devices" ) != 0)
               break;

         SendMessage( hList, LB_RESETCONTENT, 0, 0L ) ;
         SendMessage( hHidden, LB_RESETCONTENT, 0, 0L ) ;

         hDeviceMem = LocalAlloc( LMEM_MOVEABLE | LMEM_ZEROINIT,
                                   MAXALLDEVICELEN ) ;
         if (!hDeviceMem)
         {
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

               //
               // Only add active printers that are assigned to ports
               //
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
               LPFNOLDDEVMODE lpfnDM ;

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
                     PostMessage( hDlg, WM_COMMAND, IDOK, 0L ) ;
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
         return FALSE ;
   }
   return TRUE ;

}


