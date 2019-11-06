/*
   ws_print.c - Print routines for WinPrint

   (c) Copyright 1989, Charles E. Kindel, Jr.

*/

#include "PRECOMP.H"

#include "winprint.h"
#include "dlghelp.h"
#include "isz.h"
#include "dlgs.h"
#include "ws_print.h"
#include "ws_lib.h"
#include "ws_files.h"
#include "ws_dlg.h"
#include "fontutil.h"
#include "wintime.h"
#include "dragdrop.h"

static BOOL    bUserAbort ;
static BOOL    bError  ;
static HWND    hDlgPrint ;
static char    szFileBeingPrinted [_MAX_DRIVE + _MAX_FNAME + _MAX_DIR + _MAX_EXT] ;
static char    szDateRevised [64] ;
static char    szDatePrinted [64] ;
static int     cPrev ;  /* used by MyReadLine.  Must be set to NULL for each new file */

static HFONT   hfontText ;
static HFONT   hfontHF ;
static PAGE    Page ;
static short   nEscapeReturn ; // last escape return value
static char    szBuf[_MAX_PATH] ;

static BOOL    fPrintUI = FALSE ;
static DLGPROC lpfnPrintCancel ;

int NEAR PASCAL   PrintOneFile( FILE *fFile, HDC hdcPrn) ; 
BOOL NEAR PASCAL  PrintPage (HDC hdcPrn, FILE *fFile, int nPage) ;
void NEAR PASCAL  PrintHF (HDC hdcPrn, int nPage, PAGEPTR pPage) ;
void NEAR PASCAL  HFOut (HDC hdcPrn, WORD wMask, PSTR szPage, int nY, int nLeft, int nRight, int xChar) ;
BOOL NEAR PASCAL  MyReadLine (char *, int, FILE *) ;
short NEAR PASCAL GetErrorID (short nEscapeReturn) ;

BOOL CALLBACK fnAbortProc( HDC hdcPrn, int nCode ) ;
BOOL CALLBACK fnPrintCancel( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;


/****************************************************************
 *  VOID WINAPI AbortPrintJob( VOID )
 *
 *  Description: 
 *
 *    Called when the user hits cancel.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI AbortPrintJob( VOID )
{
   DP3( hWDB, "AbortPrintJob() SETTING bUserAbort to TRUE!!!" ) ;
   bUserAbort = TRUE ;

} /* AbortPrintJob()  */

BOOL WINAPI PrintFiles( HWND hDlg, HWND hwndSel, HWND hwndHid, LPSTR lpszFileName )
{
   char        szFileName[_MAX_PATH] ;
   char        szDate[64], szTime[64] ;
   LPFNABORTPROC  lpfnAbortProc ;
   HDC         hdcPrn ;
   int         nPrintReturn ;
   FILE        *fFile ;
   HFILE       hFile ;
   OFSTRUCT    of ;
   struct stat FileInfo ;

   if (NULL == (hdcPrn = GetPrinterDC ()))
   {
      ErrorResBox( hwndMain, hInst, MB_ICONEXCLAMATION,
                   IDS_APPNAME, IDS_ERR_BADPRINTERDC ) ;
      return TRUE ;
   }

   if (!(hfontText = CreateFontIndirect( &Options.lfCurFont )))
      return TRUE ;

   if (!(hfontHF = CreateFontIndirect( &Options.lfHFFont )))
   {
      DeleteObject( hfontText ) ;
      return TRUE ;
   }

   hfontText = SelectObject( hdcPrn, hfontText ) ;

   if (!GetPageInfo (hdcPrn, &Page))
   {
      hfontText = SelectObject( hdcPrn, hfontText ) ;
      DeleteObject( hfontText ) ;
      DeleteObject( hfontHF ) ;
      return TRUE ;
   }


   bUserAbort = FALSE ;
   bError = FALSE ;
   lpfnAbortProc = (LPFNABORTPROC)MakeProcInstance( (FARPROC)fnAbortProc, hInst ) ;
   DP3( hWDB, "Escape( SETABORTPROC )..." ) ;
   Escape (hdcPrn, SETABORTPROC, NULL, (LPSTR) lpfnAbortProc, (LPSTR) NULL) ;

   hfontText = SelectObject( hdcPrn, hfontText ) ;

   /*
    * Global variable used by ws_dlg.c
    */
   fPrinting = TRUE ;

   /*
    * Get the current time, and use my time routines to put it into
    * a valid string format
    */
   TimeResetInternational() ;

   TimeGetCurTime( szTime, 0 ) ;
   TimeGetCurDate( szDate, 0 ) ;
   wsprintf( szDatePrinted,
             GRCS(IDS_FILEPRINTEDON),
             (LPSTR)szDate, (LPSTR)szTime ) ;

   lstrcpy( szFileBeingPrinted, "" ) ;

   DP3( hWDB, "Escape( STARTDOC )..." ) ;
   if (nEscapeReturn = Escape( hdcPrn, STARTDOC,
                               lstrlen (GRCS(IDS_APPNAME)),
                               (LPSTR)GRCS(IDS_APPNAME), NULL ) > 0)
   {

      /*
       * If a filename was passed in do it, otherwise get the
       * filenames from the list box.
       */
      while ((lpszFileName ||
              (BOOL)SendMessage( hwndHid, LB_GETCOUNT, 0, 0L) ) &&
             !bError &&
             !bUserAbort)
      {
         if (!lpszFileName)
         {
            if (hwndSel)
               SendMessage( hwndSel, LB_SETCURSEL, 0, 0L ) ;          
            SendMessage( hwndHid, LB_GETTEXT, 0,
                        (LONG)(LPSTR)szFileBeingPrinted ) ;

            if (hwndSel)
               SendMessage( hwndSel, LB_DELETESTRING, 0, 0L ) ;
            SendMessage( hwndHid, LB_DELETESTRING, 0, 0L ) ;
         }
         else
            lstrcpy( szFileBeingPrinted, lpszFileName ) ;

         if (!(fFile = (fdopen( hFile = OpenFile( szFileBeingPrinted,
                                                &of, OF_READ), "rt" ) )))
         {
            ErrorResBox( hwndMain, hInst, MB_OK | MB_ICONEXCLAMATION,
                     IDS_APPNAME, IDS_ERR_OPENFILE,
                     (LPSTR)szFileBeingPrinted ) ;

            if (lpszFileName)
               break ;
            else
               continue ;
         }
         DP3( hWDB, "Printing %s", (LPSTR)szFileBeingPrinted ) ;

         lstrcpy( szFileName, "" ) ;

         if (Options.bMinimize || IsIconic( hwndMain ))
         {
            ShortenPath( szFileName, szFileBeingPrinted, _MAX_EXT + _MAX_FNAME ) ;
            wsprintf( szBuf, GRCS(IDS_MSG_PRINTING), (LPSTR)szFileName ) ;
            SetWindowText( hwndMain, szBuf ) ;
         }
         else
         {
            ShortenPath( szFileName, szFileBeingPrinted, 30 ) ;
            if (hDlgPrint)
            {
               SetDlgItemText( hDlgPrint, IDD_FNAME, szFileName) ;
            }
         }

         /*
         * Get the file time and date info and use the international
         * time functions to format it.
         */
         fstat( hFile, &FileInfo) ;
         TimeFormatTime( szTime, localtime( &FileInfo.st_mtime ), 0 ) ;
         TimeFormatDate( szDate, localtime( &FileInfo.st_mtime ), 0 ) ;

         wsprintf( szDateRevised, GRCS(IDS_FILEREVISEDON),
                  (LPSTR)szDate,
                  (LPSTR)szTime ) ;

         if (EOF != (nPrintReturn = PrintOneFile( fFile, hdcPrn )))
         {
            DP1( hWDB, "PrintOneFile returned ! EOF, nPrintReturn = %d", nPrintReturn ) ;
            DP1( hWDB, "  bUserAbort = %d", bUserAbort ) ;
            fclose (fFile) ;
            break ;
         }

         fclose( fFile ) ;

         if (lpszFileName)
            break ;
      }
   }
   else
   {
      DP1( hWDB, "STARTDOC Failed, assumming error" ) ;
      bError = TRUE ;
   }

   if (bError && !bUserAbort)
   {
      EnableWindow( hwndMain, TRUE ) ;

      ErrorResBox( hwndMain, hInst, MB_OK | MB_ICONEXCLAMATION,
                   IDS_APPNAME, IDS_ERR_PRINTERR,
                   (LPSTR)GRCS(GetErrorID( nEscapeReturn )),
                   (LPSTR)szFileBeingPrinted ) ;

   }
   else
   {
      DP3( hWDB, "Escape( ENDDOC )..." ) ;
      Escape( hdcPrn, ENDDOC, 0, 0L, 0L ) ;
   }


   DeleteObject( hfontText ) ;
   DeleteObject( hfontHF ) ;

   DeleteDC( hdcPrn ) ;

   FreeProcInstance( (FARPROC)lpfnAbortProc ) ;

   DP3( hWDB, "Done printing" ) ;

   fPrinting = FALSE ;

   return( bUserAbort || bError ) ;
}

int NEAR PASCAL PrintOneFile (FILE *fFile, HDC hdcPrn) 
{
   int   nPage = 1, 
         nPrintReturn ;


   DP5( hWDB, "PrintOneFile:" ) ;
   DP5( hWDB, "  xChar = %d, yChar = %d", Page.xChar, Page.yChar ) ;
   DP5( hWDB, "  nTopMargin = %d, nLeftMargin = %d, nBotMargin = %d, nRightMargin = %d",
                 Page.nTopMargin, Page.nLeftMargin, Page.nBotMargin, Page.nRightMargin ) ;
   DP5( hWDB, "  nUnitsPerLine = %d, nUnitsPerPage = %d", Page.nUnitsPerLine, Page.nUnitsPerPage ) ;

   cPrev = NULL ;
   do
   {

      nPrintReturn = PrintPage( hdcPrn, fFile, nPage ) ;
      nPage++ ;
      DP3( hWDB, "Escape( NEWFRAME )... (Page %d, nPrintReturn = %d", nPage, nPrintReturn ) ;
      if ((nEscapeReturn = Escape( hdcPrn, NEWFRAME, 0, 0L, 0L )) < -1)
      {
         DP3( hWDB, "PrintOneFile() : Setting bError == TRUE (nEscapeReturn == %d)", nEscapeReturn ) ;
         bError = TRUE ;
         return (nEscapeReturn) ;
      }
   }
   while ((!nPrintReturn && !bError)) ;

   return (nPrintReturn) ;
}



BOOL NEAR PASCAL PrintPage( HDC hdcPrn, FILE *fFile, int nPage)
{
   int            nLine,
                  nReadReturn = 0 ;
   PSTR           pszBuf ;
   short          yPos ;
   int            rgiTabs[1] ;
   short          xCur ;
   short          nChars ;
   short          nCol ;
   short          nColWidth ;
   short          nColSpace ;
   HPEN           hpen ;
   short          i ;


   if (Options.nColumns > 1)
   {
      nColSpace = MulDiv( GetDeviceCaps( hdcPrn, LOGPIXELSX ),
                          Options.nColSpace, 1000 ) ;

      nChars = Page.nCharsPerLine / Options.nColumns -
                  ((nColSpace/2) / Page.xChar) ;

      nColWidth = ((Page.nUnitsPerLine - Page.nRightMargin) -
                  Page.nLeftMargin) / Options.nColumns ;
   }
   else
   {
      nColSpace = 0 ;
      nChars = Page.nCharsPerLine ;
      nColWidth = ((Page.nUnitsPerLine - Page.nRightMargin) -
                   Page.nLeftMargin) ;
   }

   pszBuf = (PSTR)LocalAlloc( LPTR,
                                nChars + 16 ) ;

   if (!pszBuf)
   {
      DP1( hWDB, "LocalAlloc in PrintPage Failed!!!" ) ;
      return 1 ;
   }

   hfontText = SelectObject( hdcPrn, hfontText ) ;

   SetBkMode( hdcPrn, TRANSPARENT ) ;

   /*
    * Print a page.  Each page has Options.nColumns columns.  So
    * for each column act like it's a page (huh?)
    */
   for (nCol = 1 ; nCol <= Options.nColumns ; nCol++)
   {
      nLine = 0 ;

      xCur = Page.nLeftMargin + ((nCol-1) * nColWidth) ;

      if (nCol > 1 && Options.wSeparator == SEP_TEXT)
         for (i = 0 ; i < Page.nLinesPerPage ; i++)
            TextOut( hdcPrn, xCur,
                     Page.yChar *
                        (i+(2 * (0 < Options.wHeaderMask))) +
                        Page.nTopMargin,
                     "|", 1 ) ;


      pszBuf[0] = '\0' ;

      while( !bError &&
             !nReadReturn &&
             (nLine < Page.nLinesPerPage) &&
             !strchr( pszBuf, '\f' ))
      {
         
         nReadReturn = MyReadLine( pszBuf, nChars, fFile ) ;

         if (nReadReturn && nReadReturn != EOF)
            break ;

         yPos = Page.yChar * (nLine+(2 * (0 < Options.wHeaderMask))) + Page.nTopMargin ;

         if (nCol > 1 && Options.wSeparator == SEP_TEXT)
            TextOut( hdcPrn, xCur, yPos, "|", 1 ) ;

         if (!Options.bExpandTabs)
         {
            TextOut( hdcPrn, xCur + (nCol > 1 ? nColSpace/2 : 0), yPos,
                     pszBuf, (strchr( pszBuf, '\f') ? lstrlen(pszBuf) - 1 :
                              lstrlen(pszBuf))) ;

         }
         else
         {
            /*
            * Use TabbedTextOut to output each line with tabs intact.
            */

            /*
            * Build a one element array containing SpacesPerTab * AveCharWidth
            */
            rgiTabs[0] = Options.nTabSize * Page.xChar ;

            TabbedTextOut( hdcPrn,
                           xCur + (nCol > 1 ? nColSpace/2 : 0),
                           yPos,
                           pszBuf,
                           (strchr (pszBuf, '\f') ?
                              lstrlen(pszBuf) - 1 : lstrlen(pszBuf)),
                           sizeof( rgiTabs ) / sizeof( int ),
                           rgiTabs,
                           xCur ) ;
         }

         if (bUserAbort)
         {
            DP3( hWDB, "bUserAbort in PrintPage()" ) ;
            hfontText = SelectObject( hdcPrn, hfontText ) ;
            LocalFree ((LOCALHANDLE) pszBuf) ;
            return EOF ;
         }
         nLine++ ;
      }
   }

   if (bError)
   {
      DP3( hWDB, "bError in PrintPage()" ) ;
      hfontText = SelectObject( hdcPrn, hfontText ) ;
      LocalFree ((LOCALHANDLE) pszBuf) ;

      return nReadReturn ;
   }

   /*
    * For each two columns we need a separator line
    */
   if (Options.wSeparator != SEP_NONE)
   {
      yPos = Page.yChar * (2 * (0 < Options.wHeaderMask)) + Page.nTopMargin ;

      if (Options.wSeparator == SEP_GRAPHIC)
         hpen = SelectObject( hdcPrn, GetStockObject( BLACK_PEN ) ) ;

      for (nCol = 2 ; nCol <= Options.nColumns ; nCol++)
      {
         xCur = Page.nLeftMargin + ((nCol-1) * nColWidth) ;

         if (Options.wSeparator == SEP_GRAPHIC)
         {
            MoveToEx( hdcPrn, xCur, yPos, NULL ) ;
            LineTo( hdcPrn, xCur, Page.nUnitsPerPage -
                     Page.nBotMargin -
                     (Options.wHeaderMask ? Page.yChar : 0 ) -
                     (Options.wFooterMask ? Page.yChar : 0 ) ) ;
         }

         /*
          * We do he SEP_TEXT while outputing lines
          */
      }
      if (Options.wSeparator == SEP_GRAPHIC)
         hpen = SelectObject( hdcPrn, hpen ) ;
   }

   hfontText = SelectObject( hdcPrn, hfontText ) ;
   PrintHF( hdcPrn, nPage, &Page ) ;
   LocalFree( (LOCALHANDLE)pszBuf ) ;

   return nReadReturn ;
}

void NEAR PASCAL PrintHF (HDC hdcPrn, int nPage, PAGEPTR pPage)
{
   char           szPage [80] ;
   int            xC ;
   TEXTMETRIC     tm ;

   if (!(Options.wHeaderMask || Options.wFooterMask))
      return ;

   hfontHF = SelectObject( hdcPrn, hfontHF ) ;

   xC = GetTextMetricsCorrectly( hdcPrn, &tm ) ;

   if (!(tm.tmPitchAndFamily & 0x01))  // !VARIABLE_PITCH
      xC = tm.tmAveCharWidth ;

   wsprintf( szPage, GRCS(IDS_PAGE), nPage) ;

   /* if header */
   if (Options.wHeaderMask)
      HFOut (hdcPrn, Options.wHeaderMask, szPage, pPage->nTopMargin,
             Options.bHFMargins ? pPage->nLeftMargin : 0,
             Options.bHFMargins ? pPage->nUnitsPerLine - pPage->nRightMargin : pPage->nUnitsPerLine, xC) ;

   if (Options.wFooterMask)
      HFOut( hdcPrn, Options.wFooterMask, szPage,
             pPage->nUnitsPerPage -
               pPage->nBotMargin -
               pPage->yChar * (Options.wHeaderMask ? 2 : 1),
             Options.bHFMargins ? pPage->nLeftMargin : 0,
             Options.bHFMargins ? pPage->nUnitsPerLine - pPage->nRightMargin : pPage->nUnitsPerLine, xC) ;

   hfontHF = SelectObject( hdcPrn, hfontHF ) ;
}


void NEAR PASCAL HFOut (HDC hdcPrn, WORD wMask, PSTR szPage, int nY, int nLeft, int nRight, int xChar)
{
   char  szFileName [_MAX_DIR + _MAX_DRIVE + _MAX_FNAME + _MAX_EXT] ;
   short nWidth = nRight - nLeft ;
   short nDateX ;
   short nOffset ;
   SIZE  size ;

   lstrcpy (szFileName, szFileBeingPrinted) ;

   nOffset = GetTextExtentABCPoint( hdcPrn, szPage,
                                       lstrlen( szPage ), &size ) ;

   if (wMask & HF_PAGENUM)
      TextOut( hdcPrn,
               nRight - size.cx,
               nY,
               szPage,
               lstrlen( szPage ) ) ;

   if (wMask & HF_RDATE)
   {
      nOffset = GetTextExtentABCPoint( hdcPrn, szDateRevised,
                                       lstrlen( szDateRevised ), &size ) ;

      nDateX = (nWidth - size.cx) / 2 ;
         
      ShortenPath( szFileName,
                   szFileBeingPrinted,
                   (nDateX - nLeft) / xChar - 4 ) ;

      if (wMask & HF_FNAME)
         TextOut(hdcPrn, nLeft+nOffset, nY, szFileName, lstrlen( szFileName ) ) ;
      else
         TextOut( hdcPrn, nLeft+nOffset, nY, " ", 1 ) ; /* what's this HACK?!? */

      TextOut( hdcPrn,
               nLeft + nDateX,
               nY, szDateRevised, lstrlen(szDateRevised)) ;

      return ;
   }

   if (wMask & HF_PDATE)
   {
      nOffset = GetTextExtentABCPoint( hdcPrn, szDatePrinted,
                                       lstrlen( szDatePrinted ), &size ) ;

      nDateX = (nWidth - size.cx) / 2 ;

      ShortenPath( szFileName,
                   szFileBeingPrinted,
                   (nDateX - nLeft) / xChar - 4 ) ;

      if (wMask & HF_FNAME)
         TextOut(hdcPrn, nLeft, nY, szFileName, lstrlen( szFileName ) ) ;
      else
         TextOut( hdcPrn, nLeft, nY, " ", 1 ) ; /* what's this HACK?!? */

      TextOut( hdcPrn,
               nLeft + nDateX,
               nY, szDatePrinted, lstrlen(szDatePrinted ) ) ;

      return ;
   }

   if (wMask & HF_FNAME)
      TextOut (hdcPrn, nLeft, nY, szFileName, lstrlen (szFileName)) ;
}

BOOL NEAR PASCAL MyReadLine (PSTR szBuffer, int nLineLen, FILE *fFile)
{
   int      c,
            i ;
   int      nCharCount = 0 ;

   szBuffer[0] = '\0' ;

   while ((nCharCount < nLineLen) &&
         EOF != (c = (cPrev ? cPrev : fgetc( fFile ))))
   {
      if (cPrev == '\n' ||
          cPrev == '\r' ||
          cPrev == '\f')   /* if last char was CR/LF then see if next one is too */
         c = fgetc (fFile) ;

      cPrev = NULL ;

      switch (c)
      {
         case '\n':
               cPrev = fgetc (fFile) ;
               return FALSE ;

         case '\r':
               cPrev = fgetc (fFile) ;
               return FALSE ;

         case '\f':
               cPrev = fgetc (fFile) ;
               lstrcat (szBuffer, "\f") ;
               return (cPrev == EOF || cPrev == '\x1A' ? EOF : FALSE) ;
               /* if next char (after \f) was a EOF or ^z then we're EOF */

         case '\x1A':
         case EOF:
               return EOF ;
               break ;

         case '\t':
            if (Options.bExpandTabs)
            {
               int n ;

               n = Options.nTabSize - (nCharCount % Options.nTabSize);

               nCharCount += n - 1 ;
            }

            /* Fall through!!! */

         default:
            nCharCount++ ;
            szBuffer [i = lstrlen(szBuffer)] = (char) c ;
            szBuffer [i+1] = '\0' ;
      }
   }

   if (feof (fFile))
      return EOF ;
   else
   {
      if (i = ferror(fFile))
      {
         DP1( hWDB, "bError set to true in MyReadLine" ) ;
         bError = TRUE ;
         return i ;
      }
      else
         if (lstrlen( szBuffer ) == nLineLen)
         {
            cPrev = fgetc( fFile ) ;
            if (cPrev == '\n' ||
                cPrev == '\r')
               cPrev = fgetc( fFile ) ;
         }
   }


   return FALSE ;
}


short NEAR PASCAL GetErrorID(short nEscapeReturn)
{
   static short nErr[] =
      {
         IDS_ERR_PRINTERR0,
         IDS_ERR_PRINTERR0,
         IDS_ERR_PRINTERR1,
         IDS_ERR_PRINTERR2,
         IDS_ERR_PRINTERR3,
         IDS_ERR_PRINTERR4
      } ;

   DP1( hWDB, "GetErrorTxt( %d )", nEscapeReturn ) ;

   nEscapeReturn = abs( nEscapeReturn ) ;

   if (nEscapeReturn > 5)
   {
      DP1( hWDB, "  Unknown Printer Error!" ) ;
      DP1( hWDB, "  %s", (LPSTR)GRCS(nErr[0]) ) ;
      return nErr[0] ;
   }

   DP1( hWDB, "  %s", (LPSTR)GRCS(nErr[nEscapeReturn]) ) ;
   return nErr[nEscapeReturn] ;
}

/****************************************************************
 *  BOOL CALLBACK
 *   fnPrintCancel( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;
 *
 *  Description: 
 *
 *    Hanldes the print cancel dialog box.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL CALLBACK fnPrintCancel( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
   switch (uiMsg)
   {
      case WM_INITDIALOG:
      {
         DEVNAMES FAR * lpdn ;
         
         SetDlgItemText( hDlg, IDD_FNAME, GRCS(IDS_MSG_WORKING) ) ;
         if (lpdn = (DEVNAMES FAR *)GlobalLock( ghDevNames ))
         {
            PSTR pszMsg ;
            
            pszMsg = (PSTR)LocalAlloc( LPTR,
                           lstrlen( (LPSTR)lpdn + lpdn->wDeviceOffset ) +
                           lstrlen( (LPSTR)lpdn + lpdn->wOutputOffset ) +
                           lstrlen( GRCS(IDS_MSG_PRINTERONPORT) ) ) ;

            if (pszMsg)
            {
               wsprintf( pszMsg, GRCS(IDS_MSG_PRINTERONPORT),
                         (LPSTR)lpdn+lpdn->wDeviceOffset,
                         (LPSTR)lpdn+lpdn->wOutputOffset ) ;

               DP4( hWDB, "Printing...%s", (LPSTR)pszMsg ) ;
               SetDlgItemText( hDlg, IDD_PRINTER, pszMsg ) ;

               LocalFree( (HANDLE)pszMsg ) ;
            }

            GlobalUnlock( ghDevNames ) ;
         }
         EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE, MF_GRAYED) ;
         MyDragAcceptFiles( hDlg, TRUE ) ;

         tdShadowControl( hDlg, uiMsg, wParam, RGBLTGRAY ) ;
      }
      break ;

      case WM_COMMAND:
            AbortPrintJob() ;
            EnableWindow (hwndMain, TRUE) ;
            MyDragAcceptFiles( hDlg, FALSE ) ;
            DestroyWindow (hDlg) ;
            hDlgPrint = 0 ;
            break ;

      case WM_DROPFILES:
         SendMessage( hwndMain, uiMsg, wParam, lParam ) ;
      break ;

      default:
         return tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;
   }
   return TRUE ;
}

/****************************************************************
 *  BOOL CALLBACK fnAbortProc( HDC hdcPrn, short nCode )
 *
 *  Description: 
 *
 *    Printing abort proc.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL CALLBACK fnAbortProc( HDC hdcPrn, int nCode )
{
   MSG   msg ;

   while (!bUserAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
   {
      if (!hDlgPrint || !IsDialogMessage( hDlgPrint, &msg ))
      {
         TranslateMessage( &msg ) ;
         DispatchMessage( &msg ) ;
      }
   }
   return !bUserAbort ;

} /* fnAbortProc()  */


/****************************************************************
 *  VOID WINAPI StartPrintUI( VOID )
 *
 *  Description: 
 *
 *    Either pops up a cancel print dialog box or
 *    handles setting the icon text and changing the
 *    menu.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI StartPrintUI( VOID )
{
   if (fPrintUI)
      return ;

   fPrintUI = TRUE ;

   if (Options.bMinimize || IsIconic( hwndMain ))
   {
      DP2( hWDB, "Printing while Iconic" ) ;
      hDlgPrint = NULL ;
      lpfnPrintCancel = NULL ;

      /*
       * Put a "Cancel" option on the system menu
       */
      AppendMenu( GetSystemMenu( hwndMain, FALSE ),
                  MF_SEPARATOR, 0, (LPSTR)NULL ) ;
      AppendMenu( GetSystemMenu( hwndMain, FALSE ),
                  MF_STRING, IDM_CANCEL, (LPSTR)GRCS(IDS_MENU_CANCEL) ) ;
              
      SetWindowText( hwndMain, (LPSTR)GRCS(IDS_MSG_WORKING) ) ;

   }
   else
   {
      DP2( hWDB, "Printing while not Iconic" ) ;
      EnableWindow( hwndMain, FALSE ) ;
      lpfnPrintCancel = (DLGPROC)MakeProcInstance( (FARPROC)fnPrintCancel, hInst) ;
      hDlgPrint = CreateDialog( hInst, MAKEINTRESOURCE( DLG_PRINTING ),
                                hwndMain, lpfnPrintCancel) ;
   }

} /* StartPrintUI()  */


/****************************************************************
 *  VOID WINAPI EndPrintUI( VOID )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI EndPrintUI( VOID )
{
   char szText[_MAX_PATH] ;

   if (!fPrintUI)
      return ;

   fPrintUI = FALSE ;

   EnableWindow (hwndMain, TRUE) ;
   if (hDlgPrint)
   {
      DP4( hWDB, "hDlgPrint is not NULL" ) ;
      DestroyWindow (hDlgPrint) ;
      hDlgPrint = NULL ;
   }

   if (Options.bMinimize || IsIconic( hwndMain ))
   {
      wsprintf( (LPSTR)szText, (LPSTR)"%s %c [%s]",
                (LPSTR)GRCS(IDS_APPNAME),
                bModify ? '*' : '-', (LPSTR)szCurrentConfig) ;

      SetWindowText( hwndMain, szText ) ;

      /* reset the system menu */
      GetSystemMenu( hwndMain, TRUE ) ;
   }

   if (lpfnPrintCancel)
      FreeProcInstance( (FARPROC)lpfnPrintCancel ) ;

   lpfnPrintCancel = NULL ;

} /* EndPrintUI()  */
