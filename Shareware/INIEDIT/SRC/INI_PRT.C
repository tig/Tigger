// ini_prt.c
//
// printing routines
//
//

#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ceklib.h>
#include "..\inc\iniedit.h"
#include "..\inc\inieditd.h"
#include "..\inc\ini_prt.h"

extern   HWND     hMainWnd ;
extern   char     szAppName [] ;
extern   char     szFileName [] ;
extern   char     szSysDir [] ;
extern   char     szDevice[] ;
extern   char     szDriver[] ;
extern   char     szPort[] ; 
extern   HANDLE   hInst ;

BOOL     bUserAbort ;
HWND     hDlgPrint ;

int      cPrev ;  // used by MyReadLine.
                  // Must be set to NULL for each new file

int         xChar ;
int         yChar ;
int         nLinesPerPage ;
int         nCharsPerLine ;

BOOL Print( void )
{
char        szMsg [80],
            szFileToPrint [_MAX_DRIVE+ _MAX_FNAME + _MAX_DIR + _MAX_EXT] ;
FARPROC     lpfnAbortProc,
            lpfnPrintDlgProc ;
HDC         hPrnDC ;
TEXTMETRIC  tm ;
errors      eErr ;
   
   if (NULL == (hPrnDC = CreateDC( szDriver, szDevice, szPort, NULL )))
   {
      MessageBox(hMainWnd, "Could not get Device Context","Error: PrintFile",
         MB_ICONEXCLAMATION ) ;
      return TRUE ;
   }
   D( dp("Printing... File = %s Printer = %s, %s Port = %s",
         (LPSTR)szFileName, (LPSTR)szDriver, (LPSTR)szDevice, (LPSTR)szPort) ) ;

   GetTextMetrics( hPrnDC, &tm ) ;
   xChar = tm.tmAveCharWidth ;
   yChar = tm.tmHeight + tm.tmExternalLeading ;

   nCharsPerLine = GetDeviceCaps( hPrnDC, HORZRES ) / xChar ;
   nLinesPerPage = GetDeviceCaps( hPrnDC, VERTRES ) / yChar ;


   wsprintf( szFileToPrint, (LPSTR)"%s%s",
                            (LPSTR)szSysDir,
                            (LPSTR)szFileName ) ;

   D( dp("%s, xChar = %d, yChar = %d, nChars = %d, nLines = %d",
         (LPSTR)szFileToPrint,
         xChar,
         yChar,
         nCharsPerLine,
         nLinesPerPage) ) ;

   EnableWindow( hMainWnd, FALSE ) ;

   bUserAbort = FALSE ;
   lpfnPrintDlgProc = MakeProcInstance( PrintDlgProc, hInst ) ;
   hDlgPrint = CreateDialog( hInst, "PrintDlgBox", hMainWnd, lpfnPrintDlgProc ) ;
   D( dp("hDlgPrint = %#04X", hDlgPrint) ) ;

   lpfnAbortProc = MakeProcInstance( AbortProc, hInst ) ;
   Escape( hPrnDC, SETABORTPROC, NULL, (LPSTR)lpfnAbortProc, (LPSTR)NULL ) ;

   SetDlgItemText( hDlgPrint, IDD_NAME_TXT, "(working...)" ) ;
   wsprintf( szMsg, (LPSTR)"%s: %s", (LPSTR)szAppName, (LPSTR)szFileName ) ;
   if (Escape( hPrnDC, STARTDOC, lstrlen( szMsg ), (LPSTR)szMsg, NULL ) > 0)
   {
      D( dp("STARTDOC ok") ) ;
      if (hDlgPrint)
      {
         wsprintf( szMsg, (LPSTR)"Sending %s to the", (LPSTR)szFileName ) ;
         D( dp(szMsg) ) ;
         SetDlgItemText( hDlgPrint, IDD_NAME_TXT, szMsg ) ;
         SetDlgItemText( hDlgPrint, IDD_DEVICE_TXT, szDevice ) ;
         wsprintf( szMsg, (LPSTR)"on %s", (LPSTR)szPort ) ;
         SetDlgItemText( hDlgPrint, IDD_PORT_TXT, szMsg ) ;
      }
      eErr = PrintFile( hPrnDC, szFileToPrint ) ;
   }
   else
   {
      eErr = ERR_STARTDOC ;
      goto Error1 ;
   }

   if (eErr != ERR_USERABORT)
   {
      D (dp( "User did not abort") ) ;
      Escape (hPrnDC, ENDDOC, 0, 0L, 0L) ;
      EnableWindow( hMainWnd, TRUE ) ;
      DestroyWindow( hDlgPrint ) ;
   }
   else
      D (dp( "User aborted") ) ;

   FreeProcInstance( lpfnPrintDlgProc ) ;
   FreeProcInstance( lpfnAbortProc ) ;
   DeleteDC( hPrnDC ) ;

   D(dp( "%s, eErr %i = %s",
          (LPSTR)szFileToPrint,
          eErr,
          (LPSTR)GetErrorText(eErr) )) ;
   return TRUE ;

//-------------------------------------------------------------
Error1:
   wsprintf( (LPSTR)szMsg,
             (LPSTR)"Could not print\n%s\n eErr = %i\n%s",
             (LPSTR)szFileToPrint,
             eErr,
             (LPSTR)GetErrorText(eErr) ) ;

   EnableWindow( hMainWnd, TRUE ) ; 
   MessageBox( hMainWnd, szMsg, szAppName, MB_OK | MB_ICONEXCLAMATION ) ;

   FreeProcInstance( lpfnPrintDlgProc ) ;
   FreeProcInstance( lpfnAbortProc ) ;
   DeleteDC( hPrnDC ) ;
   return FALSE ;
}


//-----------------------------------------------------
// PrintFile
//
//
errors PrintFile( HDC hDC, char szFileName[] ) 
{
int         nPage = 1 ;
errors      nPrintReturn ;
FILE        *fFile ;
HANDLE      hFile ;
OFSTRUCT    of ;

   D( dp("PrintFile( %#04X, %s )", hDC, (LPSTR)szFileName )) ;
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
         D( dp("ERR_NEWFRAME") ) ;
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

   D( dp("PrintPage( %#04X, %#04X, %d )", hPrnDC, fFile, nPage )) ;
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
BOOL FAR PASCAL PrintDlgProc( HWND hDlg,
                              unsigned iMessage,
                              WORD wParam,
                              LONG lParam)
{
   switch (iMessage)
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
BOOL FAR PASCAL AbortProc( HDC hPrnDC, short nCode )
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
