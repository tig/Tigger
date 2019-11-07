// extdev.c
//
// ExtDeviceMode dialog box
//
//
#include "windows.h"   
#include <drivinit.h>
#include "print.h"     
#include "printd.h" 
#include "extdev.h"

extern char       szCurPrinterName[] ;
extern char       szCurDriver[] ;
extern char       szCurPort[] ;
extern HANDLE     hDevModeMem ;
extern HANDLE     hDriver ;

char *rgszPaperSize[] =
{
   "<Non Standard>",
   "DMPAPER_LETTER",    
   "DMPAPER_LETTERSMALL",
   "DMPAPER_TABLOID",    
   "DMPAPER_LEDGER",    
   "DMPAPER_LEGAL",     
   "DMPAPER_STATEMENT",  
   "DMPAPER_EXECUTIVE",  
   "DMPAPER_A3",
   "DMPAPER_A4",        
   "DMPAPER_A4SMALL",    
   "DMPAPER_A5",      
   "DMPAPER_B4",      
   "DMPAPER_B5",      
   "DMPAPER_FOLIO",     
   "DMPAPER_QUARTO",    
   "DMPAPER_10X14",     
   "DMPAPER_11X17",     
   "DMPAPER_NOTE",       
   "DMPAPER_ENV_9",     
   "DMPAPER_ENV_10",    
   "DMPAPER_ENV_11",    
   "DMPAPER_ENV_12",    
   "DMPAPER_ENV_14",    
   "DMPAPER_CSHEET",    
   "DMPAPER_DSHEET",    
   "DMPAPER_ESHEET"     
} ;

char *rgszPaperBin[] =
{
   "<Non Standard>",
   "DMBIN_UPPER",
   "DMBIN_LOWER",
   "DMBIN_MIDDLE",
   "DMBIN_MANUAL",
   "DMBIN_ENVELOPE",
   "DMBIN_ENVMANUAL",    
   "DMBIN_AUTO",
   "DMBIN_TRACTOR",
   "DMBIN_SMALLFMT",    
   "DMBIN_LARGEFMT",    
   "DMBIN_LARGECAPACITY",
   "<n/a>",
   "<n/a>",
   "DMBIN_CASSETTE"
} ;

char *rgszPrintQuality[] =
{
   "DMRES_HIGH",
   "DMRES_MEDIUM",
   "DMRES_LOW",
   "DMRES_DRAFT"
} ;


   

BOOL FAR PASCAL ExtDevModeDlgProc( HWND hDlg,
                                    unsigned iMessage,
                                    WORD wParam,
                                    LONG lParam)
{
   switch (iMessage)
   {
      case WM_INITDIALOG:
         CopyFromDriver( hDlg ) ;
         break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDD_MODIFY_BTN :
               CopyToDriver( hDlg ) ;
               break ;

            case IDD_COPY_BTN :
               CopyFromDriver( hDlg ) ;
               break ;

            case IDD_UPDATE_BTN :
               UpdateINI( hDlg ) ;
               break ;

            case IDD_PROMPT_BTN :
               DoPrompt( hDlg ) ;
               break ;

            case IDOK :
               CopyToDevMode( hDlg ) ;
               EndDialog( hDlg, TRUE ) ;
               break ;

            case IDCANCEL :
               EndDialog ( hDlg, FALSE ) ;
               break ;

         }

      default :
         return FALSE ;
    }

    return TRUE ;   
}

void CopyFromDevMode( HWND hDlg )
{
   LPDEVMODE   lpDevMode ;
   char        szBuf[80] ;
   HANDLE      hMem ;
   PSTR        pszExtra ;
   int         i ;
   WORD       n ;
   HWND        hLB ;

   lpDevMode = (LPDEVMODE)LocalLock( hDevModeMem ) ;



   //dumpDevMode( lpDevMode) ;

   SetDlgItemText( hDlg, IDD_DEVICENAME_TXT, lpDevMode->dmDeviceName ) ;

   wsprintf( szBuf, (LPSTR)"%#04X", lpDevMode->dmSpecVersion ) ;
   SetDlgItemText( hDlg, IDD_SPECVERSION_TXT, szBuf ) ;

   wsprintf( szBuf, (LPSTR)"%#04X", lpDevMode->dmDriverVersion ) ;
   SetDlgItemText( hDlg, IDD_DRIVERVERSION_TXT, szBuf ) ;

   wsprintf( szBuf, (LPSTR)"%#04X", lpDevMode->dmSize ) ;
   SetDlgItemText( hDlg, IDD_SIZE_TXT, szBuf ) ;

   wsprintf( szBuf, (LPSTR)"%#04X", lpDevMode->dmDriverExtra ) ;
   SetDlgItemText( hDlg, IDD_DRIVEREXTRA_TXT, szBuf ) ;

   wsprintf( szBuf, (LPSTR)"%#04X", lpDevMode->dmFields ) ;
   SetDlgItemText( hDlg, IDD_FIELDS_TXT, szBuf ) ;

   // Orientation radio buttons
   CheckRadioButton( hDlg, IDD_PORTRAIT_RB, IDD_LANDSCAPE_RB, 
                     lpDevMode->dmOrientation == DMORIENT_PORTRAIT ?
                     IDD_PORTRAIT_RB : IDD_LANDSCAPE_RB ) ;
   
//  EnableWindow( GetDlgItem( hDlg, IDD_PORTRAIT_RB ),
//                 (BOOL)(DM_ORIENTATION & lpDevMode->dmFields) ) ;
//   EnableWindow( GetDlgItem( hDlg, IDD_LANDSCAPE_RB ),
//                 (BOOL)(DM_ORIENTATION & lpDevMode->dmFields) ) ;

   // Papersize listbox
   hLB = GetDlgItem( hDlg, IDD_PAPERSIZES_LB ) ;
   SendMessage( hLB, LB_RESETCONTENT, 0, 0L ) ;
   for (i = DMPAPER_FIRST - 1 ; i <= DMPAPER_LAST ; i++)
      SendMessage( hLB, LB_INSERTSTRING,
                     i, (LONG)(LPSTR)rgszPaperSize[i] ) ;
   if (lpDevMode->dmPaperSize > DMPAPER_LAST)
      SendMessage( hLB, LB_SETCURSEL, 0, 0L ) ;
   else
      SendMessage( hLB, LB_SETCURSEL, lpDevMode->dmPaperSize, 0L ) ;

//   EnableWindow( hLB, (BOOL)(DM_PAPERSIZE & lpDevMode->dmFields) ) ;

   // PaperLength Edit
   SetDlgItemInt( hDlg, IDD_PAPERLENGTH_EC,
                  lpDevMode->dmPaperLength, FALSE ) ;

//   EnableWindow( GetDlgItem( hDlg, IDD_PAPERLENGTH_EC ),
//                 (BOOL)(DM_PAPERLENGTH & lpDevMode->dmFields) ) ;

   // PaperWidth Edit
   SetDlgItemInt( hDlg, IDD_PAPERWIDTH_EC,
                  lpDevMode->dmPaperWidth, FALSE ) ;

//   EnableWindow( GetDlgItem( hDlg, IDD_PAPERWIDTH_EC ),
//                 (BOOL)(DM_PAPERWIDTH & lpDevMode->dmFields) ) ;

   // Scale
   SetDlgItemInt( hDlg, IDD_SCALE_EC,
                  lpDevMode->dmScale, FALSE ) ;

//   EnableWindow( GetDlgItem( hDlg, IDD_SCALE_EC),
//                 (BOOL)(DM_SCALE & lpDevMode->dmFields) ) ;

   // #copies
   SetDlgItemInt( hDlg, IDD_COPIES_EC,
                  lpDevMode->dmCopies, FALSE ) ;

//   EnableWindow( GetDlgItem( hDlg, IDD_COPIES_EC),
//                 (BOOL)(DM_COPIES & lpDevMode->dmFields) ) ;

   // Standard paper bins (more through GETSETPAPERBINS escape)
   hLB = GetDlgItem( hDlg, IDD_DEFAULTSOURCE_LB ) ;
   SendMessage( hLB, LB_RESETCONTENT, 0, 0L ) ;
   for (i = DMBIN_FIRST - 1 ; i <= DMBIN_LAST ; i++)
      SendMessage( hLB, LB_INSERTSTRING,
                     i, (LONG)(LPSTR)rgszPaperBin[i] ) ;
   if (lpDevMode->dmDefaultSource > DMBIN_LAST)
      SendMessage( hLB, LB_SETCURSEL, 0, 0L ) ;
   else
      SendMessage( hLB, LB_SETCURSEL, lpDevMode->dmDefaultSource ?
                                      lpDevMode->dmDefaultSource :
                                      -1, 0L ) ;

//   EnableWindow( hLB, (BOOL)(DM_DEFAULTSOURCE & lpDevMode->dmFields) ) ;

   //  Print quality
   hLB = GetDlgItem( hDlg, IDD_PRINTQUALITY_LB ) ;
   SendMessage( hLB, LB_RESETCONTENT, 0, 0L ) ;
   for (i = 0 ; i < 4  ; i++)
      SendMessage( hLB, LB_INSERTSTRING,
                     i, (LONG)(LPSTR)rgszPrintQuality[i] ) ;
   if (lpDevMode->dmPrintQuality)
      SendMessage( hLB, LB_SETCURSEL, lpDevMode->dmPrintQuality + 4, 0L ) ;
   else
      SendMessage( hLB, LB_SETCURSEL, -1, 0L ) ;
//   EnableWindow( hLB, (BOOL)(DM_PRINTQUALITY & lpDevMode->dmFields) ) ;

   // Color Radio buttons
   CheckRadioButton( hDlg, IDD_MONOCHROME_RB, IDD_COLOR_RB,
                     lpDevMode->dmColor == DMCOLOR_COLOR ?
                     IDD_COLOR_RB : IDD_MONOCHROME_RB ) ;

//   EnableWindow( GetDlgItem( hDlg, IDD_MONOCHROME_RB),
//                 (BOOL)(DM_COLOR & lpDevMode->dmFields) ) ;
//   EnableWindow( GetDlgItem( hDlg, IDD_COLOR_RB),
//                 (BOOL)(DM_COLOR & lpDevMode->dmFields) ) ;

   // Duplex radios
   CheckRadioButton( hDlg, IDD_SIMPLEX_RB, IDD_VERTICAL_RB,
                     lpDevMode->dmDuplex + IDD_SIMPLEX_RB - 1) ;

//   EnableWindow( GetDlgItem( hDlg, IDD_SIMPLEX_RB),
//                 (BOOL)(DM_DUPLEX & lpDevMode->dmFields) ) ;
//   EnableWindow( GetDlgItem( hDlg, IDD_HORIZONTAL_RB),
//                 (BOOL)(DM_DUPLEX & lpDevMode->dmFields) ) ;
//   EnableWindow( GetDlgItem( hDlg, IDD_VERTICAL_RB),
//                 (BOOL)(DM_DUPLEX & lpDevMode->dmFields) ) ;

   if (lpDevMode->dmDriverExtra)
   {
      hMem = LocalAlloc( LMEM_MOVEABLE, lpDevMode->dmDriverExtra * 4) ;
      pszExtra = (PSTR)LocalLock( hMem ) ;
      SendDlgItemMessage( hDlg, IDD_DRIVEREXTRA_EC,
                          EM_LIMITTEXT, lpDevMode->dmDriverExtra * 4, 0L ) ;
      wsprintf( pszExtra, (LPSTR)"" ) ;
      SetDlgItemText( hDlg, IDD_DRIVEREXTRA_EC, pszExtra ) ;
      for (n = 1; n <= lpDevMode->dmDriverExtra ; n++)
      {
         wsprintf( pszExtra, (LPSTR)"%s%02X ", (LPSTR)pszExtra,
                  *((BYTE*)lpDevMode + sizeof(LPDEVMODE) + n)  ) ;
         // D(dp( "%02X", *((BYTE*)lpDevMode + sizeof(LPDEVMODE) + n) )) ;
      }
//      D(dp(pszExtra)) ;
      SendDlgItemMessage( hDlg, IDD_DRIVEREXTRA_EC,
                         WM_SETTEXT, 0, (LONG)(LPSTR)pszExtra ) ;
      //SetDlgItemText( hDlg, IDD_DRIVEREXTRA_EC, pszExtra ) ;
      LocalUnlock( hMem ) ;
      LocalFree( hMem ) ;
   }
   else
      SetDlgItemText( hDlg, IDD_DRIVEREXTRA_EC, "" ) ;
//   EnableWindow( GetDlgItem( hDlg, IDD_DRIVEREXTRA_EC),
//                 (BOOL)lpDevMode->dmDriverExtra ) ;

   LocalUnlock( hDevModeMem ) ;
}

void CopyToDevMode( HWND hDlg )
{
   LPDEVMODE   lpDevMode ;
   HWND        hLB ;
   short       nIndex ;

   lpDevMode = (LPDEVMODE)LocalLock( hDevModeMem ) ;

   //dumpDevMode( lpDevMode) ;
   
   // Orientation radio buttons
   lpDevMode->dmOrientation = IsDlgButtonChecked( hDlg, IDD_PORTRAIT_RB ) ?
                              DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE ;
   
   // Papersize listbox
   hLB = GetDlgItem( hDlg, IDD_PAPERSIZES_LB ) ;
   nIndex = (short)SendMessage( hLB, LB_GETCURSEL, 0, 0L ) ;
   if (nIndex == LB_ERR)
      lpDevMode->dmPaperSize = 0 ;
   else
      lpDevMode->dmPaperSize = nIndex ;

   // PaperLength Edit
   lpDevMode->dmPaperLength = GetDlgItemInt( hDlg, IDD_PAPERLENGTH_EC,
                                              NULL, FALSE ) ;

   // PaperWidth Edit
   lpDevMode->dmPaperWidth = GetDlgItemInt( hDlg, IDD_PAPERWIDTH_EC,
                                             NULL, FALSE ) ;

   // Scale
   lpDevMode->dmScale = GetDlgItemInt( hDlg, IDD_SCALE_EC,
                                       NULL, FALSE ) ;

   // #copies
   lpDevMode->dmCopies = GetDlgItemInt( hDlg, IDD_COPIES_EC,
                                       NULL, FALSE ) ;

   // Standard paper bins (more through GETSETPAPERBINS escape)
   hLB = GetDlgItem( hDlg, IDD_DEFAULTSOURCE_LB ) ;
   nIndex = (short)SendMessage( hLB, LB_GETCURSEL, 0, 0L ) ;
   if (nIndex == LB_ERR)
      lpDevMode->dmDefaultSource = 0 ;
   else
      lpDevMode->dmDefaultSource = nIndex ;

   //  Print quality
   hLB = GetDlgItem( hDlg, IDD_PRINTQUALITY_LB ) ;
   nIndex = (short) SendMessage( hLB, LB_GETCURSEL, 0, 0L ) ;
   if (nIndex == LB_ERR)
      lpDevMode->dmPrintQuality = 0 ;
   else
      lpDevMode->dmPrintQuality = nIndex - 4 ;

   // Color Radio buttons
   lpDevMode->dmColor = IsDlgButtonChecked( hDlg, IDD_COLOR_RB ) ?
                              DMCOLOR_COLOR : DMCOLOR_MONOCHROME ;

   // Duplex radios
   if (IsDlgButtonChecked( hDlg, IDD_SIMPLEX_RB ))
      lpDevMode->dmDuplex = DMDUP_SIMPLEX ;
   else
      if (IsDlgButtonChecked( hDlg, IDD_HORIZONTAL_RB ))
         lpDevMode->dmDuplex = DMDUP_HORIZONTAL ;
      else
         lpDevMode->dmDuplex = DMDUP_VERTICAL ;

   LocalUnlock( hDevModeMem ) ;
}

void CopyToDriver( HWND hDlg )
{
   LPDEVMODE   lpDevMode ;
   LPFNDEVMODE lpfnExtDeviceMode ;
   int n ;

   CopyToDevMode( hDlg ) ;

   lpDevMode = (LPDEVMODE) LocalLock( hDevModeMem ) ;

   lpfnExtDeviceMode = GetProcAddress( hDriver, PROC_EXTDEVICEMODE ) ;
   if (!lpfnExtDeviceMode)
   {
      D( dp( "Driver does not support ExtDeviceMode" )) ;
      FreeLibrary( hDriver ) ;
      hDriver = 0 ;
   }
   else
   {
      n = (*lpfnExtDeviceMode)( hDlg, hDriver,
                                (LPDEVMODE)lpDevMode,
                                (LPSTR)szCurPrinterName,
                                (LPSTR)szCurPort,
                                (LPDEVMODE)lpDevMode,
                                (LPSTR)NULL,
                                DM_MODIFY ) ;
   }

   LocalUnlock( hDevModeMem ) ;
}

void CopyFromDriver( HWND hDlg )
{
   LPDEVMODE   lpDevMode ;
   LPFNDEVMODE lpfnExtDeviceMode ;
   int n ;


   lpDevMode = (LPDEVMODE) LocalLock( hDevModeMem ) ;

   lpfnExtDeviceMode = GetProcAddress( hDriver, PROC_EXTDEVICEMODE ) ;
   if (!lpfnExtDeviceMode)
   {
      D( dp( "Driver does not support ExtDeviceMode" )) ;
      FreeLibrary( hDriver ) ;
      hDriver = 0 ;
   }
   else
   {
      n = (*lpfnExtDeviceMode)( hDlg, hDriver,
                                (LPDEVMODE)lpDevMode,
                                (LPSTR)szCurPrinterName,
                                (LPSTR)szCurPort,
                                (LPDEVMODE)lpDevMode,
                                (LPSTR)NULL,
                                DM_COPY ) ;
   }

   LocalUnlock( hDevModeMem ) ;
   CopyFromDevMode( hDlg ) ;
}

void UpdateINI (HWND hDlg )
{
   LPDEVMODE   lpDevMode ;
   LPFNDEVMODE lpfnExtDeviceMode ;
   int n ;

   CopyToDevMode( hDlg ) ;

   lpDevMode = (LPDEVMODE) LocalLock( hDevModeMem ) ;

   lpfnExtDeviceMode = GetProcAddress( hDriver, PROC_EXTDEVICEMODE ) ;
   if (!lpfnExtDeviceMode)
   {
      D( dp( "Driver does not support ExtDeviceMode" )) ;
      FreeLibrary( hDriver ) ;
      hDriver = 0 ;
   }
   else                                    
   {
      n = (*lpfnExtDeviceMode)( hDlg, hDriver,
                                (LPDEVMODE)NULL,
                                (LPSTR)szCurPrinterName,
                                (LPSTR)szCurPort,
                                (LPDEVMODE)lpDevMode,
                                (LPSTR)NULL,
                                DM_UPDATE | DM_MODIFY ) ;
   }

   LocalUnlock( hDevModeMem ) ;
}

void DoPrompt (HWND hDlg )
{
   LPDEVMODE   lpDevMode ;
   LPFNDEVMODE lpfnExtDeviceMode ;
   int n ;

   CopyToDevMode( hDlg ) ;

   lpDevMode = (LPDEVMODE) LocalLock( hDevModeMem ) ;

   lpfnExtDeviceMode = GetProcAddress( hDriver, PROC_EXTDEVICEMODE ) ;
   if (!lpfnExtDeviceMode)
   {
      D( dp( "Driver does not support ExtDeviceMode" )) ;
      FreeLibrary( hDriver ) ;
      hDriver = 0 ;
   }
   else                                    
   {
      n = (*lpfnExtDeviceMode)( hDlg, hDriver,
                                (LPDEVMODE)lpDevMode,
                                (LPSTR)szCurPrinterName,
                                (LPSTR)szCurPort,
                                (LPDEVMODE)lpDevMode,
                                (LPSTR)NULL,
                                DM_PROMPT | DM_MODIFY | DM_COPY ) ;
   }

   LocalUnlock( hDevModeMem ) ;

   CopyFromDevMode( hDlg ) ;
}
