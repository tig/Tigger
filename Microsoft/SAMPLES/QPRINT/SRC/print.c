/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  QPRINT 
 *
 *      Module:  QPRINT.c
 *
 *   Revisions:  
 *       8/13/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include <print.h>

#include "version.h"

#include "QPRINT.h"

char     szDevice[144] ;
char     szDriver[144] ;
char     szPort[144] ; 

BOOL     bUserAbort ;
HWND     hDlgPrint ;

int      cPrev ;  // used by MyReadLine.
                  // Must be set to NULL for each new file

int         xChar ;
int         yChar ;
int         nLinesPerPage ;
int         nCharsPerLine ;


typedef enum errs 
   {  ERR_OK = 0,
      ERR_EOP,
      ERR_EOF,
      ERR_STARTDOC,
      ERR_NEWFRAME,
      ERR_USERABORT,
      ERR_SPOOLABORT,
      ERR_OPEN,
      ERR_READ,
      ERR_GEN,
      ERR_LAST 
   } errors ;

char *szErrorText [] = {"No Error",               // ERR_OK
                        "End of page",            // ERR_EOP
                        "End of file",            // ERR_EOF
                        "Error from STARTDOC",    // ERR_STARTDOC
                        "Error from NEWFRAME",    // ERR_NEWFRAME
                        "Canceled from Program",  // ERR_USERABORT
                        "Canceled from Spooler",  // ERR_SPOOLABORT
                        "Could not open file",    // ERR_OPEN
                        "Could not read file",    // ERR_READ
                        "General Error",          // ERR_GEN
                        "LastErr"                 // ERR_LAST
                       } ;

errors HandlePrintFile( HDC hDC, LPSTR szFileName ) ;
errors PrintPage (HDC hPrnDC, FILE *fFile, int nPage) ;
errors MyReadLine (PSTR szBuffer, int nLineLen, FILE *fFile) ;
BOOL FAR PASCAL fnPrintCancel( HWND hDlg,
                              unsigned iMessage,
                              WORD wParam,
                              LONG lParam) ;
BOOL FAR PASCAL fnAbort( HDC hPrnDC, short nCode );
char *GetErrorText( errors eErr ) ;


/**************************************************************************
 *    PrintGetDefaultPrn
 *
 *    Gets the default printer from the WIN.INI file.
 *
 *************************************************************************/
BOOL WINAPI
    PrintGetDefaultPrn( LPSTR lpszDriverName,
                        LPSTR lpszDeviceName,
                        LPSTR lpszPortName )
{
   char szPrinter [256] ;
   LPSTR lpsDrv, lpsDev, lpsPort ;

   GetProfileString( "windows", "device", "",
                     (LPSTR)szPrinter, 256 ) ;
   if ((lpsDev  = _fstrtok( szPrinter, "," )) &&
       (lpsDrv  = _fstrtok( NULL,      "," )) &&
       (lpsPort = _fstrtok( NULL,      "," )))
   {
      lstrcpy( lpszDriverName, lpsDrv ) ;
      lstrcpy( lpszDeviceName, lpsDev ) ;
      lstrcpy( lpszPortName, lpsPort ) ;
   }
   else
      return FALSE;

   return TRUE ;
}

/****************************************************************
 *  BOOL WINAPI PrintFile LPSTR szFileName )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI PrintFile( LPSTR szFileName )
{
   char        szMsg [80] ;
   ABORTPROC   lpfnAbort ;
   DLGPROC     lpfnPrintCancel ;
   HDC         hPrnDC ;
   TEXTMETRIC  tm ;
   errors      eErr ;
   

   if (!PrintGetDefaultPrn( szDriver, szDevice, szPort ))
   {
      MessageBox(hwndApp, "There is no default printer driver installed!","Print Error",
         MB_ICONEXCLAMATION ) ;
      return FALSE ;
   }

   if (NULL == (hPrnDC = CreateDC( szDriver, szDevice, szPort, NULL )))
   {
      MessageBox(hwndApp, "Could not access default printer!","Print Error",
         MB_ICONEXCLAMATION ) ;
      return FALSE ;
   }
   DP3( hWDB, "Printing... File = %s Printer = %s, %s Port = %s",
         (LPSTR)szFileName, (LPSTR)szDriver, (LPSTR)szDevice, (LPSTR)szPort ) ;

   GetTextMetrics( hPrnDC, &tm ) ;
   xChar = tm.tmAveCharWidth ;
   yChar = tm.tmHeight + tm.tmExternalLeading ;

   nCharsPerLine = GetDeviceCaps( hPrnDC, HORZRES ) / xChar ;
   nLinesPerPage = GetDeviceCaps( hPrnDC, VERTRES ) / yChar ;


   DP3( hWDB, "%s, xChar = %d, yChar = %d, nChars = %d, nLines = %d",
         (LPSTR)szFileName,
         xChar,
         yChar,
         nCharsPerLine,
         nLinesPerPage) ;

   EnableWindow( hwndApp, FALSE ) ;

   bUserAbort = FALSE ;

#ifdef ALLOW_CANCEL
   lpfnPrintCancel = (DLGPROC)MakeProcInstance( (FARPROC)fnPrintCancel,
                     hinstApp ) ;
   hDlgPrint = CreateDialog( hinstApp, MAKEINTRESOURCE( DLG_PRINTCANCEL ),
                              hwndApp, lpfnPrintCancel ) ;
#endif

   DP3( hWDB, "hDlgPrint = %#04X", hDlgPrint) ;

   lpfnAbort = (ABORTPROC)MakeProcInstance( (FARPROC)fnAbort, hinstApp ) ;
   Escape( hPrnDC, SETABORTPROC, NULL, (LPSTR)lpfnAbort, (LPSTR)NULL ) ;

#ifdef ALLOW_CANCEL
   SetDlgItemText( hDlgPrint, IDD_PRINTING, "(working...)" ) ;
#endif
   wsprintf( szMsg, (LPSTR)"%s: %s", (LPSTR)rglpsz[IDS_APPNAME], (LPSTR)szFileName ) ;
   if (Escape( hPrnDC, STARTDOC, lstrlen( szMsg ), (LPSTR)szMsg, NULL ) > 0)
   {
      DP3( hWDB, "STARTDOC ok") ;
      if (hDlgPrint)
      {
         wsprintf( szMsg, (LPSTR)"Printing %s on the", (LPSTR)szFileName ) ;
         DP3( hWDB, szMsg) ;
         SetDlgItemText( hDlgPrint, IDD_PRINTING, szMsg ) ;
         SetDlgItemText( hDlgPrint, IDD_DEVICE, szDevice ) ;
         SetDlgItemText( hDlgPrint, IDD_PORT, szPort ) ;
      }
      eErr = HandlePrintFile( hPrnDC, szFileName ) ;
   }
   else
   {
      eErr = ERR_STARTDOC ;
      goto Error1 ;
   }

   if (eErr != ERR_USERABORT)
   {
      DP3( hWDB,  "User did not abort") ;
      Escape (hPrnDC, ENDDOC, 0, 0L, 0L) ;
      EnableWindow( hwndApp, TRUE ) ;
      if (hDlgPrint) 
         DestroyWindow( hDlgPrint ) ;
   }
   else
      DP3( hWDB,  "User aborted") ;

   if (lpfnPrintCancel)
      FreeProcInstance( (FARPROC)lpfnPrintCancel ) ;
   FreeProcInstance( (FARPROC)lpfnAbort ) ;
   DeleteDC( hPrnDC ) ;

   DP3( hWDB,  "%s, eErr %i = %s",
          (LPSTR)szFileName,
          eErr,
          (LPSTR)GetErrorText(eErr) ) ;
   return TRUE ;

//-------------------------------------------------------------
Error1:
   wsprintf( (LPSTR)szMsg,
             (LPSTR)"Could not print\n%s\n eErr = %i\n%s",
             (LPSTR)szFileName,
             eErr,
             (LPSTR)GetErrorText(eErr) ) ;

   EnableWindow( hwndApp, TRUE ) ; 
   MessageBox( hwndApp, szMsg, rglpsz[IDS_APPNAME], MB_OK | MB_ICONEXCLAMATION ) ;

   if (lpfnPrintCancel)
      FreeProcInstance( (FARPROC)lpfnPrintCancel ) ;

   FreeProcInstance( (FARPROC)lpfnAbort ) ;
   DeleteDC( hPrnDC ) ;
   return FALSE ;
} /* PrintFile)  */


//-----------------------------------------------------
// HandlePrintFile
//
//
errors HandlePrintFile( HDC hDC, LPSTR szFileName ) 
{
int         nPage = 1 ;
errors      nPrintReturn ;
FILE        *fFile ;
HFILE       hFile ;
OFSTRUCT    of ;

   DP3( hWDB, "HandlePrintFile( %#04X, %s )", hDC, (LPSTR)szFileName ) ;
   if (HFILE_ERROR == (hFile = OpenFile( szFileName, &of,
                                 OF_READ | OF_SHARE_DENY_WRITE )))
   {
      DP1( hWDB, "Could not open with OpenFile()" ) ;
      return ERR_OPEN ;
   }

   if (NULL == (fFile = ( _fdopen( hFile, "rt" ))))
   {
      DP1( hWDB, "Could not open with _fdopen()" ) ;
      return ERR_OPEN ;
   }

   cPrev = NULL ;
   do
   {
      nPrintReturn = PrintPage( hDC, fFile, nPage ) ;
      if (nPrintReturn > ERR_EOF)
         break ;
      if (Escape( hDC, NEWFRAME, 0, 0L, 0L ) < 0)
      {
         DP3( hWDB, "ERR_NEWFRAME")  ;
         fclose( fFile ) ;
         return ERR_NEWFRAME ;
      }
      nPage++ ;
   }
   while (nPrintReturn < ERR_EOF) ;

   fclose( fFile ) ;
   return nPrintReturn ;
}


//-------------------------------------------------
// 
// PrintPage
//
//
errors PrintPage (HDC hPrnDC, FILE *fFile, int nPage)
{
int            nLine ;
errors         eErr ;
char           *szBuffer ;

   DP3( hWDB, "PrintPage( %#04X, %#04X, %d )", hPrnDC, fFile, nPage ) ;
   szBuffer = (char NEAR *) LocalAlloc( LPTR, nCharsPerLine ) ;

   nLine = 0 ;
   while (nLine < nLinesPerPage)
   {
      eErr = MyReadLine( szBuffer, nCharsPerLine, fFile ) ;
      switch (eErr)
      {
         case ERR_OK :
            TextOut( hPrnDC, 0,
                           yChar * nLine,
                           szBuffer,
                           lstrlen( szBuffer ) ) ;
            break ;

         case ERR_EOP :
            TextOut( hPrnDC, 0,
                           yChar * nLine,
                           szBuffer,
                           lstrlen( szBuffer ) ) ;
            nLine = nLinesPerPage ;
            break ;


         case ERR_EOF :
            TextOut( hPrnDC, 0,
                           yChar * nLine,
                           szBuffer,
                           lstrlen( szBuffer ) ) ;

         default :
            LocalFree( (LOCALHANDLE)szBuffer ) ;
            return eErr ;
      }

      if (bUserAbort)
      {
         LocalFree( (LOCALHANDLE)szBuffer ) ;
         return ERR_USERABORT ;
      }
      
      nLine++ ;
   }
   LocalFree ((LOCALHANDLE) szBuffer) ;
   return ERR_OK ;
}

//-------------------------------------------------
// MyReadLine
//
errors MyReadLine (PSTR szBuffer, int nLineLen, FILE *fFile)
{
int         c,
            i ;

   szBuffer[0] = '\0' ;
   while ((lstrlen (szBuffer) < nLineLen) && EOF != (c = (cPrev ? cPrev : fgetc (fFile))))
   {
      if (cPrev == '\n' || cPrev == '\r' || cPrev == '\f')   /* if last char was CR/LF then see if next one is too */
         c = fgetc (fFile) ;

      cPrev = NULL ;

      switch (c)
      {
         case '\n':
               cPrev = fgetc (fFile) ;
               return ERR_OK ;

         case '\r':
               cPrev = fgetc (fFile) ;
               return ERR_OK ;

         case '\f':
               DP3( hWDB, "case '\\f'")  ;
               cPrev = fgetc (fFile) ;
               //lstrcat (szBuffer, "\f") ;
               return (cPrev == EOF || cPrev == '\x1A' ? ERR_EOF : ERR_EOP) ;

         case '\x1A':
         case EOF:
               return ERR_EOF ;
               break ;

         default:
               szBuffer [i = lstrlen(szBuffer)] = (char) c ;
               szBuffer [i+1] = '\0' ;
      }
   }
   if (feof (fFile))
      return ERR_EOF ;
   else
      if (i = ferror(fFile))
         return ERR_READ ;

   return ERR_OK ;
}

//--------------------------------------------------
// fnPrintCancel
//
//
BOOL FAR PASCAL fnPrintCancel( HWND hDlg,
                              unsigned iMessage,
                              WORD wParam,
                              LONG lParam)
{
   switch (iMessage)
   {
      case WM_INITDIALOG:
            SetDlgItemText( hDlg, IDD_PRINTING, "Working..." ) ;
            SetDlgItemText( hDlg, IDD_DEVICE, "" ) ;
            SetDlgItemText( hDlg, IDD_PORT, "" ) ;
            EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE, MF_GRAYED) ;
            break ;

      case WM_COMMAND:
            bUserAbort = TRUE ;
            EnableWindow( hwndApp, TRUE ) ;
            DestroyWindow( hDlg ) ;
            hDlgPrint = 0 ;
            break ;

      default:
            return FALSE ;
   }
   return TRUE ;
}

//-----------------------------------------------
// fnAbort
//
BOOL FAR PASCAL fnAbort( HDC hPrnDC, short nCode )
{
   MSG   msg ;

   while (!bUserAbort && PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
   {
      if (!hDlgPrint || !IsDialogMessage( hDlgPrint, &msg ))
      {
         TranslateMessage( &msg ) ;
         DispatchMessage( &msg ) ;
      }
   }
   return !bUserAbort ;
}

//---------------------------------------------
// GetErrorText
//
//
char *GetErrorText( errors eErr)
{
   if (eErr > ERR_LAST)
      return NULL ;

   return szErrorText [eErr] ;
}

//----------------------------------------------------
// Printer setup stuff...
//
BOOL PrintSetup( void )
{
   return TRUE ;
}
