// ini_prt.c
//
// printing routines
//
//

#include <windows.h>
#include <drivinit.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "print.h"
#include "printd.h"
#include "prt.h"

extern   HWND     hWndMain ;
extern   char     szAppName [] ;
extern   HANDLE   hInst ;
extern   BININFO  biBins ;
extern   char     *rgszPaperBin[] ;

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

BOOL     bUserAbort ;
HWND     hDlgPrint ;
LOGFONT  lf ;
HFONT    hFont ;

extern char     szCurPrinterName [] ;
extern char     szCurPort [] ;
extern char     szCurDriver [] ;

int      cPrev ;  // used by MyReadLine.
                  // Must be set to NULL for each new file

int         xChar ;
int         yChar ;
int         nLinesPerPage ;
int         nCharsPerLine ;

#define FONTNAME "Courier" 
#define SIZE     10

#define MAXNUMBINS  3

typedef struct tagBINSTUFF
{
   short BinList[MAXNUMBINS] ;
   char  PaperNames[MAXNUMBINS][24] ;
} BINSTUFF, *PBINSTUFF, FAR *LPBINSTUFF ;

BINSTUFF bsBinStuff ;


BOOL Print( LPSTR lpszFileName )
{
char        szMsg [80],
            szFileToPrint [_MAX_DRIVE+ _MAX_FNAME + _MAX_DIR + _MAX_EXT] ;
FARPROC     lpfnAbortProc,
            lpfnPrintDlgProc ;
HDC         hPrnDC ;
TEXTMETRIC  tm ;
errors      eErr ;
short rv ;
short NumCopies ;
short ActualCopies ;
int i ;
   
   wsprintf( szFileToPrint, (LPSTR)"%s",
                            (LPSTR)lpszFileName ) ;

   lstrcpy( szCurPort, "C:\\WINDOWS\\SYSTEM\\TEST\\OTHER\\BECKY123.TXT" ) ;

   if (NULL == (hPrnDC = GetPrinterDC() ))
   {
      MessageBox(hWndMain, "Could not get Device Context","Error: PrintFile",
         MB_ICONEXCLAMATION ) ;
      return TRUE ;
   }
   D( dp("Printing... File = %s Printer = %s Port = %s",
         (LPSTR)szFileToPrint, (LPSTR)szCurPrinterName, (LPSTR)szCurPort) ) ;

   lstrcpy (lf.lfFaceName, FONTNAME) ;
   lf.lfHeight = 0 ;
   lf.lfWeight = 400 ;

   hFont = CreateFontIndirect( &lf ) ;
   D( dp("hFont = %d") ) ;
   SelectObject( hPrnDC, hFont ) ;

   GetTextMetrics( hPrnDC, &tm ) ;
   D( dp("tm.tmHeight = %d, tm.tmExternalLeading = %d",
          tm.tmHeight,
          tm.tmExternalLeading) );
   DeleteObject (SelectObject (hPrnDC, hFont)) ;

   xChar = tm.tmAveCharWidth ;
   yChar = tm.tmHeight + tm.tmExternalLeading ;

   nCharsPerLine = GetDeviceCaps( hPrnDC, HORZRES ) / xChar ;
   nLinesPerPage = GetDeviceCaps( hPrnDC, VERTRES ) / yChar ;

   D( dp("%s, xChar = %d, yChar = %d, nChars = %d, nLines = %d",
         (LPSTR)szFileToPrint,
         xChar,
         yChar,
         nCharsPerLine,
         nLinesPerPage) ) ;

   EnableWindow( hWndMain, FALSE ) ;

   bUserAbort = FALSE ;
   lpfnPrintDlgProc = MakeProcInstance( PrintDlgProc, hInst ) ;
   hDlgPrint = CreateDialog( hInst, "CancelBox", hWndMain, lpfnPrintDlgProc ) ;
   D( dp("hDlgPrint = %#04X", hDlgPrint) ) ;

   lpfnAbortProc = MakeProcInstance( AbortProc, hInst ) ;
   Escape( hPrnDC, SETABORTPROC, NULL, (LPSTR)lpfnAbortProc, (LPSTR)NULL ) ;

   SetDlgItemText( hDlgPrint, IDD_PRINTER_TXT, "(working...)" ) ;
   wsprintf( szMsg, (LPSTR)"%s: %s", (LPSTR)szAppName, (LPSTR)szFileToPrint ) ;

#if 0
   // get number of bins
   Escape( hPrnDC, GETSETPAPERBINS, 0, NULL, (LPSTR)(BININFO FAR *)&biBins ) ;
   D(dp( "Printer has %d bins", biBins.NbrofBins )) ;

   D(dp( "ENUMPAPERBINS == %d",
      Escape( hPrnDC, ENUMPAPERBINS, sizeof(int),
              (LPSTR)(short FAR *)&biBins.NbrofBins,
              (LPSTR)(LPBINSTUFF)&bsBinStuff ) )) ;

   for (i = 0 ; i < biBins.NbrofBins ; i++)
      D(dp( "  Driver says: \"%s\", DMcode = \"%s\"",
          (LPSTR)bsBinStuff.PaperNames[i] ,
          (LPSTR)rgszPaperBin[bsBinStuff.BinList[i]] )) ;
#endif

   if ((rv = Escape( hPrnDC, STARTDOC, lstrlen( szMsg ), (LPSTR)szMsg, NULL )) > 0)
   {
      if (hDlgPrint)
      {           
         wsprintf( szMsg, (LPSTR)"Sending %s to", (LPSTR)szFileToPrint ) ;
         SetDlgItemText( hDlgPrint, IDD_PRINTER_TXT, szMsg ) ;
      }
      eErr = PrintFile( hPrnDC, szFileToPrint ) ;

#if 0
      for (i = 0 ; i < biBins.NbrofBins ; i++)
      {
         char szMsg[80] ;

         biBins.BinNumber = i ;
         D(dp( "Bin = \"%s\" (BinNumber = %d), (Translated = %d)",
               (LPSTR)bsBinStuff.PaperNames[biBins.BinNumber],
               biBins.BinNumber,
               bsBinStuff.BinList[biBins.BinNumber] )) ;

         wsprintf( szMsg, (LPSTR)"%d", i+1 ) ;
         biBins.BinNumber += 0x8000 ;
         Escape( hPrnDC, GETSETPAPERBINS, sizeof(BININFO),
                  (LPSTR)(BININFO FAR *)&biBins,NULL ) ;
         biBins.BinNumber -= 0x8000 ;
         eErr = PrintPageofJunk( hPrnDC, szMsg ) ;
      }
#endif

   }
   else
   {
      eErr = ERR_STARTDOC ;
      goto Error1 ;
   }

   if (eErr != ERR_USERABORT)
   {
      D (dp( "User did not abort") ) ;
      rv = Escape (hPrnDC, ENDDOC, 0, 0L, 0L) ;
      D(dp( "ENDDOC.  rv = %d", rv )) ;
      EnableWindow( hWndMain, TRUE ) ;
      DestroyWindow( hDlgPrint ) ;
   }
   else
      D (dp( "User aborted") ) ;

   FreeProcInstance( lpfnPrintDlgProc ) ;
   FreeProcInstance( lpfnAbortProc ) ;
   DeleteDC( hPrnDC ) ;

   D(dp( "\"%s\", eErr %i = \"%s\"",
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

   EnableWindow( hWndMain, TRUE ) ; 
   MessageBox( hWndMain, szMsg, szAppName, MB_OK | MB_ICONEXCLAMATION ) ;

   FreeProcInstance( lpfnPrintDlgProc ) ;
   FreeProcInstance( lpfnAbortProc ) ;
   DeleteDC( hPrnDC ) ;
   return FALSE ;
}


errors PrintPageofJunk( HDC hDC, LPSTR lpszPageName )
{
short       rv ;
char        szBuffer[256] ;
int         nLine ;

   Escape( hDC, GETSETPAPERBINS, sizeof(BININFO),
              NULL, (LPSTR)(BININFO FAR *)&biBins )  ;

   D(dp( "   PrintPageofJunk:  Bin = \"%s\"",
             (LPSTR)bsBinStuff.PaperNames[biBins.BinNumber]
      )) ;

   hFont = CreateFontIndirect( &lf ) ;
   SelectObject( hDC, hFont ) ;


   wsprintf( szBuffer, "Page \"%s\", Bin = \"%s\"(%d)",
            (LPSTR)lpszPageName,
             (LPSTR)bsBinStuff.PaperNames[biBins.BinNumber],
            biBins.BinNumber ) ;
   TextOut( hDC, xChar, yChar * 1, szBuffer, lstrlen( szBuffer ) ) ;
   for (nLine = 3 ; nLine <= 10 ; nLine++ ) ;
   {
      wsprintf( szBuffer, "This is line %d", nLine ) ;
      TextOut( hDC, xChar, (yChar+2) * nLine, szBuffer, lstrlen( szBuffer ) ) ;
   }

            
   if ((rv = Escape( hDC, NEWFRAME, 0, 0L, 0L )) < 0)
   {
      D( dp("ERR_NEWFRAME.  rv = %d",rv) ) ;
      DeleteObject (SelectObject (hDC, hFont)) ;
      return ERR_NEWFRAME ;
   }

   DeleteObject (SelectObject (hDC, hFont)) ;
   return ERR_OK ;
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
short       rv ;

   D( dp("PrintFile( %#04X, \"%s\" )", hDC, (LPSTR)szFileName )) ;
   if (NULL == (fFile = ( fdopen( hFile = OpenFile( szFileName,
                                                    &of,
                                                    OF_READ ), "rt" ))))
         return ERR_OPEN ;

   cPrev = NULL ;
   do
   {
      hFont = CreateFontIndirect( &lf ) ;
      SelectObject( hDC, hFont ) ;

      nPrintReturn = PrintPage( hDC, fFile, nPage ) ;
      if (nPrintReturn > ERR_EOF)
         break ;
                 
      if ((rv = Escape( hDC, NEWFRAME, 0, 0L, 0L )) < 0)
      {
         D( dp("ERR_NEWFRAME.  rv = %d",rv) ) ;
         DeleteObject (SelectObject (hDC, hFont)) ;
         fclose( fFile ) ;
         return ERR_NEWFRAME ;
      }
      D(dp( "NEWFRAME, rv = %d", rv)) ;
      nPage++ ;
   }
   while (nPrintReturn < ERR_EOF) ;

   DeleteObject (SelectObject (hDC, hFont)) ;
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
   char szTemp [_MAX_PATH] ;

   switch (iMessage)
   {
      case WM_INITDIALOG:
            EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE, MF_GRAYED) ;

            wsprintf( (LPSTR)szTemp, (LPSTR)"\"%s\" on \"%s\"", (LPSTR)szCurPrinterName,
                      (LPSTR)szCurPort ) ;
            SetDlgItemText( hDlg, IDD_PRINTER_TXT, szTemp ) ;
            break ;

      case WM_COMMAND:
            bUserAbort = TRUE ;
            EnableWindow( hWndMain, TRUE ) ;
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
