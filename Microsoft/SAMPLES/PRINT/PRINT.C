// print.c 
//
// Printing demo demonstration
//

#include <windows.h>   
#include <drivinit.h>
#include <string.h>
#include <stdio.h>
#include "print.h"     
#include "printd.h" 
#include "selprn.h"
#include "extdev.h"

// Globals
char           szCurPrinterName[64] ;
char           szCurDriver[16] ;
char           szCurPort[256] ;
BININFO        biBins ;

// We need two things
HANDLE         hDevModeMem ;
HANDLE         hDriver ;

HANDLE         hInst;
char           szAppName [] = "PrintDemo" ;
HWND           hWndMain ;

BOOL Print( LPSTR ) ;
short DoEscSupport (HDC hdc, HDC hdcInfo, short cxChar, short cyChar, short y) ;

int PASCAL WinMain(  HANDLE hInstance,
                     HANDLE hPrevInstance,
                     LPSTR lpCmdLine,       
                     int nCmdShow )         
{
    MSG msg;
         
//   D(dp(" ********** %s *********** \n", (LPSTR)szAppName )) ;            
   if (!hPrevInstance)  
      if (!InitApplication(hInstance)) 
          return (FALSE);

   if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

   while (GetMessage(&msg, NULL, NULL, NULL))    
   {
      TranslateMessage(&msg) ;
      DispatchMessage(&msg) ;   
   }
//  D( dp( "----------- Normal Exit ------------\n") );
   return (msg.wParam) ;
}



BOOL InitApplication( HANDLE hInstance )
{
   WNDCLASS  wc;

   wc.style         = NULL ;      
   wc.lpfnWndProc   = MainWndProc;  
   wc.cbClsExtra    = 0;             
   wc.cbWndExtra    = 0;             
   wc.hInstance     = hInstance;      
   wc.hIcon         = LoadIcon( hInstance, szAppName );
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = COLOR_WINDOW + 1 ; 
   wc.lpszMenuName  = szAppName;
   wc.lpszClassName = szAppName;

   return RegisterClass(&wc) ;
}


BOOL InitInstance(   HANDLE hInstance,
                     int    nCmdShow )
{
   HWND            hWnd;
   int             x, y ;

   hInst = hInstance;

   x = GetSystemMetrics( SM_CXSCREEN ) ;
   y = GetSystemMetrics( SM_CYSCREEN ) ;

   hWnd = CreateWindow(   szAppName,
                        szAppName,   
                        WS_OVERLAPPEDWINDOW,  
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, y / 3,
                        NULL,                 
                        NULL,                 
                        hInstance,            
                        NULL                  
                     ) ;

   if (!hWnd)
      return (FALSE);


   ShowWindow(hWnd, nCmdShow) ;
   UpdateWindow(hWnd) ;         

   hWndMain = hWnd ;

   return (TRUE) ;
}

typedef WORD FAR PASCAL FNOLDDEVMODE( HWND, HANDLE, LPSTR, LPSTR );
typedef FNOLDDEVMODE FAR * LPFNOLDDEVMODE ;


long FAR PASCAL MainWndProc( HWND hWnd,
                             unsigned message,
                             WORD wParam,
                             LONG lParam )
{
   static int  xChar, yChar ;
   int n ;
   FARPROC     lpProcAbout ;
   char        szBuf[256] ;
   HANDLE      hFont, hOldFont ;
   PAINTSTRUCT ps ;
   TEXTMETRIC  tm ;
   HDC         hDC ;
   LPDEVMODE   lpDevMode ;

   DWORD        Results ;
   LPFNDEVCAPS lpfnDevCaps ;
   LPFNDEVMODE lpfnExtDeviceMode ;
   LPFNOLDDEVMODE lpfnOldDeviceMode ;

   switch (message)
   {


      case WM_CREATE :
         lpDevMode = NULL ;

         hDC = GetDC (hWnd) ;
         hFont = CreateFont(  6, 6, 0, 0, 0, 0, 0, 0, ANSI_CHARSET,
                              OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS,
                              PROOF_QUALITY,
                              FIXED_PITCH,
                              "Courier" ) ;

         hOldFont = SelectObject( hDC, hFont ) ;

         GetTextMetrics( hDC, &tm ) ;
         xChar = tm.tmAveCharWidth ;
         yChar = tm.tmHeight + tm.tmExternalLeading ;

         SelectObject( hDC, hOldFont ) ;
         DeleteObject( hFont ) ;

         ReleaseDC (hWnd, hDC) ;

         GetProfileString( "windows", "device", "<none>,<none>,<none>", szBuf, 255 ) ;
         lstrcpy( szCurPrinterName, strtok( szBuf, "," )) ;
         lstrcpy( szCurDriver, strtok( NULL, "," )) ;
         lstrcpy( szCurPort, strtok( NULL, "," )) ;

         // Allocate memory for lpDevMode structure
         //    Load driver,
         wsprintf( szBuf, (LPSTR)"%s.DRV", (LPSTR)szCurDriver ) ;
         hDriver = LoadLibrary( szBuf ) ;
         if (hDriver >= 32)
         {
            lpfnExtDeviceMode = GetProcAddress( hDriver, PROC_EXTDEVICEMODE ) ;
            if (!lpfnExtDeviceMode)
            {
               D( dp( "Driver does not support ExtDeviceMode" )) ;
               FreeLibrary( hDriver ) ;
               hDriver = 0 ;
            }
            else
            {

               // Call ExtDeviceMode to get the size of the structure
               n = (*lpfnExtDeviceMode)( hWnd, hDriver,
                                       (LPDEVMODE)NULL,
                                       (LPSTR)szCurPrinterName,
                                          (LPSTR)szCurPort,
                                          (LPDEVMODE)NULL,
                                          (LPSTR)NULL,
                                          0 ) ;
               if (n = 0 || !(hDevModeMem = LocalAlloc( LHND, n )))
               {
                  D(dp( "Could not allocate local memory for DevMode" )) ;
                  FreeLibrary( hDriver ) ;
                  hDriver = 0 ;
               }
               else
               {
                  lpDevMode = (LPDEVMODE)LocalLock( hDevModeMem ) ;
                  // Now fill the structure
                  (*lpfnExtDeviceMode)( hWnd, hDriver,
                                          (LPDEVMODE)lpDevMode,
                                          (LPSTR)szCurPrinterName,
                                          (LPSTR)szCurPort,
                                          (LPDEVMODE)NULL,
                                          (LPSTR)NULL,
                                          DM_COPY ) ;
                  LocalUnlock( hDevModeMem ) ;
               }
            }
         }
         else
         {
            D(dp( "Could not load driver: %s", (LPSTR)szBuf )) ;
            hDriver = 0 ;
         }

         break ;

      case WM_COMMAND:    
         switch (wParam)
         {
            case IDM_INCBIN:
               biBins.BinNumber++ ;
               D(dp( "biBins.BinNumber == %d", biBins.BinNumber )) ;
               InvalidateRect( hWnd, NULL, TRUE ) ;
               break ;

            case IDM_DECBIN:
               biBins.BinNumber-- ;
               D(dp( "biBins.BinNumber == %d", biBins.BinNumber )) ;
               InvalidateRect( hWnd, NULL, TRUE ) ;
               break ;

            case IDM_DEVCAPS:
            {
               BOOL fLoaded ;

               if (hDriver == 0)
               {
                  fLoaded = FALSE ;
                  // if the driver is not loaded, extdevmode is not supported
                  // so we'll just load the driver for devcaps, then free it
                  // at the end of this stack frame.
                  //
                  wsprintf( szBuf, (LPSTR)"%s.DRV", (LPSTR)szCurDriver ) ;
                  hDriver = LoadLibrary( szBuf ) ;
               }
               else
                  fLoaded = TRUE ;

               if (hDriver >= 32)
               {
                  lpfnDevCaps = GetProcAddress( hDriver, "DEVICECAPABILITIES" ) ;
                  if (lpfnDevCaps)
                  {
                     Results = (*lpfnDevCaps)(szCurPrinterName,
                                              szCurPort,
                                              DC_VERSION,
                                              NULL,
                                              NULL ) ;
                     D(dp("DevCaps(DC_VERSION) = %d", Results)) ;
                  }
                  else
                  {
                     D(dp("Driver does not support DeviceCapabilites" )) ;
                     MessageBox( hWnd,
                        "This driver does not support DeviceCapabilites",
                        "Error",
                        MB_ICONEXCLAMATION ) ;
                  }
               }
               else
                  MessageBox( hWnd, "The driver is not loaded.", "Error",
                              MB_ICONEXCLAMATION ) ;
               if (!fLoaded)
               {
                  FreeLibrary( hDriver ) ;
                  hDriver = 0 ;
               }
            }
            InvalidateRect( hWnd, NULL, TRUE ) ;
            break ;


            case IDM_ABOUT:
               lpProcAbout = MakeProcInstance(About, hInst);
               DialogBox(  hInst,      
                           "AboutBox",  
                           hWnd,        
                           lpProcAbout );
               FreeProcInstance(lpProcAbout);
               break;

            case IDM_PRINTBANDING:
               MessageBox( hWnd, "Not implemented yet.", szAppName,
                           MB_ICONEXCLAMATION ) ;
               InvalidateRect( hWnd, NULL, TRUE ) ;
               break ;

            case IDM_PRINTNOBANDING :
               Print( "C:\\autoexec.bat" ) ;
               InvalidateRect( hWnd, NULL, TRUE ) ;
               break ;

            case IDM_PASSTHROUGH:
               PrintPassThrough( ) ;
               break ;

            case IDM_SELECTPRINTER:
               switch (PrintSelPrnDialog( hWnd,
                                          (LPSTR)szAppName,
                                          (LPSTR)szCurDriver,
                                          (LPSTR)szCurPrinterName,
                                          (LPSTR)szCurPort,
                                          SELPRN_STANDARD | SELPRN_ALLOWSETUP ))
               {
                  case DLG_CANCEL:
                     break ;

                  case DLG_OK:
                     //
                     // We first need to de-alloc the previous driver
                     // and DevMode
                     //
                     if (hDriver)
                     {
                        FreeLibrary( hDriver ) ;
                        hDriver = 0 ;

                        LocalFree( hDevModeMem ) ;
                        hDevModeMem = 0 ;
                     }

                     //
                     // Allocate memory for lpDevMode structure
                     //
                     wsprintf( szBuf, (LPSTR)"%s.DRV", (LPSTR)szCurDriver ) ;
                     hDriver = LoadLibrary( szBuf ) ;
                     if (hDriver >= 32)
                     {
                        lpfnExtDeviceMode = GetProcAddress( hDriver, PROC_EXTDEVICEMODE ) ;
                        if (!lpfnExtDeviceMode || !lstrcmpi( szBuf, "HPPLOT.DRV" ))
                        {
                           D( dp( "Driver does not support ExtDeviceMode" )) ;
                           FreeLibrary( hDriver ) ;
                           hDriver = 0 ;
                        }
                        else
                        {

                           // Call ExtDeviceMode to get the size of the structure
                           n = (*lpfnExtDeviceMode)( hWnd, hDriver,
                                                   (LPDEVMODE)NULL,
                                                   (LPSTR)szCurPrinterName,
                                                      (LPSTR)szCurPort,
                                                      (LPDEVMODE)NULL,
                                                      (LPSTR)NULL,
                                                      0 ) ;
                           if (n = 0 || !(hDevModeMem = LocalAlloc( LHND, n )))
                           {
                              D(dp( "Could not allocate local memory for DevMode" )) ;
                              FreeLibrary( hDriver ) ;
                              hDriver = 0 ;
                           }
                           else
                           {
                              lpDevMode = (LPDEVMODE)LocalLock( hDevModeMem ) ;
                              // Now fill the structure
                              (*lpfnExtDeviceMode)( hWnd, hDriver,
                                                    (LPDEVMODE)lpDevMode,
                                                    (LPSTR)szCurPrinterName,
                                                    (LPSTR)szCurPort,
                                                    (LPDEVMODE)NULL,
                                                    (LPSTR)NULL,
                                                    DM_COPY ) ;
                              LocalUnlock( hDevModeMem ) ;
                           }
                        }
                     }
                     else
                     {
                        D(dp( "Could not load driver: %s", (LPSTR)szBuf )) ;
                        hDriver = 0 ;
                     }

                     InvalidateRect( hWnd, NULL, TRUE ) ;
                     break ;

                  case DLG_ERROR:
                     MessageBox( hWnd, "Select Printer Failed.",
                                       "Error", MB_ICONEXCLAMATION ) ;
                     break ;

               }
               break ;


            case IDM_EXTDEVMODE :
               if (hDriver)
               {
                  lpProcAbout = MakeProcInstance(ExtDevModeDlgProc, hInst);
                  DialogBox(  hInst,      
                              "EXTDEVMODE",   
                              hWnd,        
                              lpProcAbout );
                  FreeProcInstance(lpProcAbout);
                  InvalidateRect( hWnd, NULL, TRUE ) ;
               }
               else
                  MessageBox( hWnd,
                        "This printer does not support ExtDeviceMode",
                        "Error",
                        MB_ICONEXCLAMATION ) ;
               InvalidateRect( hWnd, NULL, TRUE ) ;
               break ;

            case IDM_DEVICEMODE :
               D(dp( "IDM_DEVICEMODE, hDriver = %d", hDriver )) ;
               if (hDriver)
               {
                  lpfnOldDeviceMode = GetProcAddress( hDriver, "DEVICEMODE" ) ;
                  D(dp( "lpfnOldDeviceMode = %d", lpfnOldDeviceMode )) ;
                  if (lpfnOldDeviceMode)
                     (*lpfnOldDeviceMode)( hWnd, hDriver,
                                           szCurPrinterName, szCurPort ) ;
                  else
                     MessageBox( hWnd,
                       "Could not get the address of the Old DeviceMode func",
                       "Error",
                       MB_ICONEXCLAMATION ) ;
               }
               else
               {
                  wsprintf( szBuf, (LPSTR)"%s.DRV", (LPSTR)szCurDriver ) ;
                  D(dp( "szBuf" )) ;
                  hDriver = LoadLibrary( szBuf ) ;
                  if (hDriver >= 32)
                  {
                     lpfnOldDeviceMode = GetProcAddress( hDriver, 
                                                         PROC_OLDDEVICEMODE ) ;
                     (*lpfnOldDeviceMode)( hWnd, hDriver, 
                                           szCurPrinterName, szCurPort ) ;
                     FreeLibrary( hDriver ) ;
                  }
                  hDriver = 0 ;
               }
               InvalidateRect( hWnd, NULL, TRUE ) ;
            break ;


            case IDM_EXIT:
               SendMessage( hWnd, WM_CLOSE, 0, 0L ) ;
               break ;

            default:
               return DefWindowProc( hWnd, message, wParam, lParam ) ;
         }
         break ;

      case WM_PAINT :
            hDC = BeginPaint (hWnd, &ps) ;

            hFont = CreateFont(  6, 6, 0, 0, 0, 0, 0, 0, ANSI_CHARSET,
                                 OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS,
                                 PROOF_QUALITY,
                                 FIXED_PITCH,
                                 "Courier" ) ;

            hOldFont = SelectObject( hDC, hFont ) ;

            SetBkMode( hDC, OPAQUE ) ;
            SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;
         
            PaintInfo( hDC, xChar, yChar) ;

            SelectObject( hDC, hOldFont ) ;
            DeleteObject( hFont ) ;
            EndPaint (hWnd, &ps) ;
         break ;

      case WM_DESTROY :
         // De-Allocate memory for lpDevMode structure
         if (hDriver)
         {
            FreeLibrary( hDriver ) ;
            LocalFree( hDevModeMem ) ;
         }
         PostQuitMessage( 0 ) ;
         break ;

      default: 
         return DefWindowProc( hWnd, message, wParam, lParam ) ;
   }
   return 0L ;
}

BOOL PaintInfo( HDC hDC, int xChar, int yChar )
{
   char szBuffer[256] ;
   int x, y ;
   HDC   hPrnDC ;
   
   x = 0 ;
   y = 1 ;  

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Current Printer: %s (%s) on %s",
                      (LPSTR)szCurPrinterName,
                      (LPSTR)szCurDriver,
                      (LPSTR)szCurPort ) ) ;

   TextOut( hDC, xChar * x, yChar * ++y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"hDriver = %d, hDevModeMem = %d",
                      hDriver, hDevModeMem ) ) ;

   TextOut( hDC, xChar * x, yChar * ++y, szBuffer,
            wsprintf( szBuffer, "biBins.BinNumber = %d",
            biBins.BinNumber )) ;


   {

      hPrnDC = GetPrinterDC() ;

      if (hPrnDC)
      {
         short s ;
         short sEscNum ;
         POINT p ;
      
         s = Escape( hPrnDC, GETSCALINGFACTOR, NULL, NULL, (LPSTR)&p ) ;
         if (s < 0)
            TextOut( hDC, xChar * x, yChar * ++y, szBuffer,
                     wsprintf( szBuffer,
                              (LPSTR)"Escape( GETSCALINGFACTOR ) returned %d",
                              s ) ) ;
         else
            TextOut( hDC, xChar * x, yChar * ++y, szBuffer,
                     wsprintf( szBuffer,
                        (LPSTR)"Escape( GETSCALINGFACTOR ): x = %d, y = %d",
                        p.x, p.y ) ) ;

         y = DoEscSupport( hDC, hPrnDC, xChar, yChar, ++y ) ;
         DeleteDC( hPrnDC ) ;
      }
   }


   return TRUE ;
}

HDC GetPrinterDC( void )
{
   LPDEVMODE   lpDevMode ;

   lpDevMode = (LPDEVMODE)LocalLock( hDevModeMem ) ;

   return CreateDC( szCurDriver, szCurPrinterName,
                    szCurPort, NULL ) ;//(LPSTR)lpDevMode ) ;

   LocalUnlock( hDevModeMem ) ;
}



BOOL FAR PASCAL About( HWND hDlg, unsigned iMessage, WORD wParam, LONG lParam)
{
   switch (iMessage)
   {
      case WM_INITDIALOG:
         return (TRUE);

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK :
            case IDCANCEL :
               EndDialog(hDlg, TRUE) ;
               return (TRUE);

         }
       break;
    }

    return (FALSE);    
}


short DoEscSupport (HDC hdc, HDC hdcInfo, short cxChar, short cyChar, short y)
     {
     static struct
          {
          char  *szEscCode ;
          short nEscCode ;
          } 
          esc [] =
          {
          "NEWFRAME",          NEWFRAME,
          "ABORTDOC",          ABORTDOC,
          "NEXTBAND",          NEXTBAND,
          "PASSTHROUGH",       PASSTHROUGH,
          "SETCOLORTABLE",     SETCOLORTABLE,
          "GETCOLORTABLE",     GETCOLORTABLE,
          "FLUSHOUTPUT",       FLUSHOUTPUT,
          "DRAFTMODE",         DRAFTMODE,
          "QUERYESCSUPPORT",   QUERYESCSUPPORT,
          "SETABORTPROC",      SETABORTPROC,
          "STARTDOC",          STARTDOC,
          "ENDDOC",            ENDDOC,
          "GETPHYSPAGESIZE",   GETPHYSPAGESIZE,
          "GETPRINTINGOFFSET", GETPRINTINGOFFSET,
          "GETSCALINGFACTOR",  GETSCALINGFACTOR,
          "GETSETPAPERORIENT", GETSETPRINTORIENT,
          "GETSETPAPERMETRICS", GETSETPAPERMETRICS,
          "ENUMPAPERMETRIC", ENUMPAPERMETRICS,
          "GETSETPAPERBINS",   GETSETPAPERBINS,
          "ENUMPAPERBINS", ENUMPAPERBINS } ;

     static char *szYesNo [] = { "Yes", "No" } ;
     char        szBuffer [32] ;
     POINT       pt ;
     short       n, nReturn ;

     TextOut (hdc, cxChar, cyChar * (y++), "Escape Support", 14) ;

     for (n = 0 ; n < sizeof esc / sizeof esc [0] ; n++)
          {
          nReturn = Escape (hdcInfo, QUERYESCSUPPORT, 1,
                                   (LPSTR) & esc[n].nEscCode, NULL) ;
          TextOut (hdc, 6 * cxChar, (n +y+ 3) * cyChar, szBuffer,
               wsprintf (szBuffer, "%-24s %3s", (LPSTR) esc[n].szEscCode,
                         (LPSTR) szYesNo [nReturn > 0 ? 0 : 1])) ;

          if (nReturn > 0 && esc[n].nEscCode >= GETPHYSPAGESIZE
                          && esc[n].nEscCode <= GETSCALINGFACTOR)
               {
               Escape (hdcInfo, esc[n].nEscCode, 0, NULL, (LPSTR) &pt) ;
               TextOut (hdc, 36 * cxChar, (n+y + 3) * cyChar, szBuffer,
                        wsprintf (szBuffer, "(%u,%u)", pt.x, pt.y)) ;
               }
          }
         return y + n ;
     }


void dumpDevMode(LPDEVMODE lpEnv) 
{
   WORD i ;

   D(dp("        dmDeviceName: %ls", (LPSTR)lpEnv->dmDeviceName));
   D(dp("       dmSpecVersion: %4xh", lpEnv->dmSpecVersion));
   D(dp("     dmDriverVersion: %4xh", lpEnv->dmDriverVersion));
   D(dp("              dmSize: %4xh", lpEnv->dmSize));
   D(dp("       dmDriverExtra: %4xh", lpEnv->dmDriverExtra));
   D(dp("            dmFields: %8lxh", lpEnv->dmFields));
   D(dp("       dmOrientation: %d", lpEnv->dmOrientation));
   D(dp("         dmPaperSize: %d", lpEnv->dmPaperSize));
   D(dp("       dmPaperLength: %d", lpEnv->dmPaperLength)) ;
   D(dp("        dmPaperWidth: %d", lpEnv->dmPaperWidth)) ;
   D(dp("             dmScale: %d", lpEnv->dmScale));
   D(dp("            dmCopies: %d", lpEnv->dmCopies));
   D(dp("     dmDefaultSource: %d", lpEnv->dmDefaultSource));
   D(dp("      dmPrintQuality: %d", lpEnv->dmPrintQuality));
   D(dp("             dmColor: %d", lpEnv->dmColor));
   D(dp("            dmDuplex: %d", lpEnv->dmDuplex));

   D(dp("       dmDriverExtra:") ) ;
   for (i = 1; i <= lpEnv->dmDriverExtra ; i++)
      D( dprintf( " %02X", *((BYTE*)lpEnv + sizeof(LPDEVMODE) + i) ) ) ;

   D(dp(""));
}

