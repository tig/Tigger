// ini_prt.c
//
// printing routines
//
//

#include "PRECOMP.H"
#include "CONXLIBI.h"

#include "print.h"

char     szDevice[144] ;
char     szDriver[144] ;
char     szPort[144] ;

extern   HANDLE   hmodDLL ;

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

BOOL NEAR PASCAL PrintSetup( void );
LPSTR NEAR PASCAL GetErrorText( errors eErr );
BOOL CALLBACK fnAbortProc( HDC hPrnDC, short nCode );
BOOL CALLBACK fnPrintDlg(  HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
errors NEAR PASCAL MyReadLine (LPSTR szBuffer, int nLineLen, FILE *fFile);
errors NEAR PASCAL PrintPage( HDC hPrnDC, FILE *fFile, int nPage);
errors NEAR PASCAL PrintFile( HDC hDC, LPSTR szFileName ) ;

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


BOOL WINAPI Print( HWND hwndParent, LPSTR szAppName, LPSTR szFileName,
                       DWORD dw1, DWORD dw2 )
{
   char        szMsg [144+80],
               szFileToPrint [_MAX_DRIVE+ _MAX_FNAME + _MAX_DIR + _MAX_EXT] ;

   HDC         hPrnDC ;
   TEXTMETRIC  tm ;
   errors      eErr ;
   
   if (NULL == (hPrnDC = CreateDC( szDriver, szDevice, szPort, NULL )))
   {
      MessageBox(hMainWnd, "Could not get Device Context","Error: PrintFile",
         MB_ICONEXCLAMATION ) ;
      return FALSE ;
   }

   DP2( hWDB, "Printing... File = %s Printer = %s, %s Port = %s",
         (LPSTR)szFileName, (LPSTR)szDriver, (LPSTR)szDevice, (LPSTR)szPort)  ;

   GetTextMetrics( hPrnDC, &tm ) ;
   xChar = tm.tmAveCharWidth ;
   yChar = tm.tmHeight + tm.tmExternalLeading ;

   nCharsPerLine = GetDeviceCaps( hPrnDC, HORZRES ) / xChar ;
   nLinesPerPage = GetDeviceCaps( hPrnDC, VERTRES ) / yChar ;

   DP2( hWDB, "xChar = %d, yChar = %d, nChars = %d, nLines = %d",
         xChar,
         yChar,
         nCharsPerLine,
         nLinesPerPage)  ;

   EnableWindow( hMainWnd, FALSE ) ;

   bUserAbort = FALSE ;

   hDlgPrint = CreateDialog( hmodDLL, "PrintDlgBox", hMainWnd, fnPrintDlg ) ;

   Escape( hPrnDC, SETABORTPROC, NULL, (LPSTR)fnAbortProc, (LPSTR)NULL ) ;

   SetDlgItemText( hDlgPrint, IDD_NAME_TXT, "(working...)" ) ;

   wsprintf( szMsg, (LPSTR)"%s: %s", (LPSTR)szAppName, (LPSTR)szFileName ) ;
   if (Escape( hPrnDC, STARTDOC, lstrlen( szMsg ), (LPSTR)szMsg, NULL ) > 0)
   {
      if (hDlgPrint)
      {
         wsprintf( szMsg, (LPSTR)"Sending %s to the", (LPSTR)szFileName ) ;

         SetDlgItemText( hDlgPrint, IDD_NAME_TXT, szMsg ) ;
         SetDlgItemText( hDlgPrint, IDD_DEVICE_TXT, szDevice ) ;
         wsprintf( szMsg, (LPSTR)"on %s", (LPSTR)szPort ) ;
         SetDlgItemText( hDlgPrint, IDD_PORT_TXT, szMsg ) ;
      }
      eErr = PrintFile( hPrnDC, szFileName ) ;
   }
   else
   {
      eErr = ERR_STARTDOC ;
      goto Error1 ;
   }

   if (eErr != ERR_USERABORT)
   {
      Escape (hPrnDC, ENDDOC, 0, 0L, 0L) ;
      EnableWindow( hMainWnd, TRUE ) ;
      DestroyWindow( hDlgPrint ) ;
   }
   else
   {
      DP1( hWDB, "User aborted" )  ;
   }

   DeleteDC( hPrnDC ) ;

   DP2( hWDB, "%s, eErr %i = %s",
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

   EnableWindow( hMainWnd, TRUE ) ; 
   MessageBox( hMainWnd, szMsg, szAppName, MB_OK | MB_ICONEXCLAMATION ) ;

   DeleteDC( hPrnDC ) ;
   return FALSE ;
}


//-----------------------------------------------------
// PrintFile
//
//
errors NEAR PASCAL PrintFile( HDC hDC, LPSTR szFileName ) 
{
   int         nPage = 1 ;
   errors      nPrintReturn ;
   FILE        *fFile ;
   HANDLE      hFile ;
   OFSTRUCT    of ;

   if (NULL == (fFile = ( fdopen( hFile = OpenFile( szFileName,
                                                    &of,
                                                    OF_READ ), "rt" ))))
         return ERR_OPEN ;

   cPrev = NULL ;
   do
   {
      nPrintReturn = PrintPage( hDC, fFile, nPage ) ;
      if (nPrintReturn > ERR_EOF)
         break ;

      if (Escape( hDC, NEWFRAME, 0, 0L, 0L ) < 0)
      {
         DP1( hWDB, "ERR_NEWFRAME" ) ;
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
errors NEAR PASCAL PrintPage( HDC hPrnDC, FILE *fFile, int nPage)
{
   int            nLine ;
   errors         eErr ;
   LPSTR          szBuffer ;

   szBuffer = GlobalAllocPtr( nCharsPerLine ) ;

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
            GlobalFreePtr( szBuffer ) ;
            return eErr ;
      }

      if (bUserAbort)
      {
         GlobalFreePtr( szBuffer ) ;
         return ERR_USERABORT ;
      }
      
      nLine++ ;
   }
   GlobalFreePtr( szBuffer ) ;
   return ERR_OK ;
}

//-------------------------------------------------
// MyReadLine
//
errors NEAR PASCAL MyReadLine (LPSTR szBuffer, int nLineLen, FILE *fFile)
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
               D( dp("case '\\f'") ) ;
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
// PrintDlgProc
//
//
BOOL CALLBACK fnPrintDlg(  HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uiMsg)
   {
      case WM_INITDIALOG:
            EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE, MF_GRAYED) ;
            break ;

      case WM_COMMAND:
            bUserAbort = TRUE ;
            EnableWindow( hMainWnd, TRUE ) ;
            DestroyWindow( hDlg ) ;
            hDlgPrint = 0 ;
            break ;

      default:
            return FALSE ;
   }
   return TRUE ;
}

//-----------------------------------------------
// AbortProc
//
BOOL CALLBACK fnAbortProc( HDC hPrnDC, short nCode )
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
LPSTR NEAR PASCAL GetErrorText( errors eErr )
{
   if (eErr > ERR_LAST)
      return NULL ;

   return szErrorText [eErr] ;
}

//----------------------------------------------------
// Printer setup stuff...
//
BOOL NEAR PASCAL PrintSetup( void )
{
   return TRUE ;
}
