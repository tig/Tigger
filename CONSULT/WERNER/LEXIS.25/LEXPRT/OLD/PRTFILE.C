/************************************************************************
 *
 *     Project:  WinPrint 2.0
 *
 *      Module:  prtfile.c
 *
 *     Remarks:
 *
 *       This module prints a file.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "..\inc\WINPRINT.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "..\inc\prtfile.h"

/************************************************************************
 * Local defines
 ***********************************************************************/
typedef enum tagERR
{  ERR_OK = 0,
   ERR_GEN,
   ERR_EOP,
   ERR_EOF,
   ERR_OPEN,
   ERR_READ,
   ERR_LAST 
} ERR ;

//
// This data structure is local to this module and is used so the
// data can be instanced between more than one app using the 
// module.
//
typedef struct tagPRINTFILE
{
   HFONT    hfontText ;
   HFONT    hfontHF ;

   char     cPrev ;

   FILE     *fFile ;

   short    nPage  ;

   short    nCharsPerLine ;
   short    nLinesPerPage ;

   short    xChar, yChar ;

} PRINTFILE, *PPRINTFILE, FAR *LPPRINTFILE ;

/************************************************************************
 * Local Functions
 ***********************************************************************/
PRTERR NEAR PASCAL
 DoPrintFile( HPRTJOB hPJ, LPSTR lpszFile, LPPAGEDESC lpPD ) ;

ERR PrintPage( HDC hPrnDC, LPPRINTFILE lpPF ) ;

ERR MyReadLine( LPSTR lpszBuffer, int nLineLen, FILE *fFile, LPSTR lpcPrev ) ;


/****************************************************************
 *  BOOL FAR PASCAL
 *    WinPrtPrintFile( HWND hwndparent,
 *                LPSTR lpszDriver, LPSTR lpszDevice, LPSTR lpszPort,
 *                LPDEVMODE lpDM, LPPAGEDESC lpPageDesc, LPSTR lpszFile )
 *
 *  Description: 
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL
WinPrtPrintFile(  HWND hwndParent,
                  LPSTR lpszDriver, LPSTR lpszDevice, LPSTR lpszPort,
                  LPDEVMODE lpDM, LPPAGEDESC lpPageDesc, LPSTR lpszFile )
{
#if 0
   PRTJOB   pj ;
   PAGEDESC pd ;
   HPRTJOB  hPJ ;
   BOOL     fRetVal = TRUE ;

   //
   // if there was no file then return an error
   //
   if (!lpszFile)
      return FALSE ;

   //
   // if lpszDriver is null then use default printer
   //
   _fmemset( &pj, 0, sizeof( PRTJOB ) ) ;
   if (!lpszDriver)
      PrintGetDefaultPrn( pj.szDriver, pj.szDevice, pj.szPort ) ;
   else
   {
      lstrcpy( pj.szDriver, lpszDriver ) ;
      lstrcpy( pj.szDevice, lpszDevice ) ;
      lstrcpy( pj.szPort, lpszPort ) ;
   }

   // 
   // if no page desc was specified use defaults.
   //
   if (!lpPageDesc)
   {
      *pd.szHeader = '\0' ;
      *pd.szFooter = '\0' ;

      _fmemset( &pd.lfText, 0, sizeof( LOGFONT ) ) ;
      lstrcpy( pd.lfText.lfFaceName, "Courier" ) ;
      FontSelectIndirect( NULL, &pd.lfText ) ;
      SetRectEmpty( &pd.rcMargins ) ;
   }
   else
   {
      lstrcpy( pd.szHeader, lpPageDesc->szHeader ) ;
      lstrcpy( pd.szFooter, lpPageDesc->szFooter ) ;
      _fmemcpy( &pd.lfText, &lpPageDesc->lfText, sizeof( LOGFONT ) ) ;
      _fmemcpy( &pd.lfHF, &lpPageDesc->lfHF, sizeof( LOGFONT ) ) ;
      CopyRect( &pd.rcMargins, &lpPageDesc->rcMargins ) ;
   }

   if (hPJ = CreatePrintJob( hwndParent, &pj, 0 ))
   {
      PRTERR   PrtErr ;

      if (!StartPrintJob( hPJ, lpszFile ))
      {
         //
         // Print the file
         //
         PrtErr = DoPrintFile( hPJ, lpszFile, &pd ) ;

         //
         // End the job
         //
         switch (PrtErr)
         {
            case PRTERR_OK:
               EndPrintJob( hPJ ) ;
            break ;

            case PRTERR_USERABORT:
               DP( hWDB, "USERABORT") ;
               fRetVal = FALSE ;
            break ;

            default:
               DP( hWDB, "DoPrintFile returned an error") ;
         }
      }

      //
      // Destroy the job
      //
      DestroyPrintJob( hPJ ) ;
      
   } /* hPJ = Create */

   return fRetVal ;
#endif
} /* PrintFile()  */


/****************************************************************
 *  PRTERR NEAR PASCAL
 *       DoPrintFile( HPRTJOB hPJ, LPSTR lpszFile, LPPAGEDESC lpPD )
 *
 *  Description: 
 *
 *    This function prints the file.  Local to this module.
 *    It prints it without banding, and is pretty boring.
 *
 *  Comments:
 *
 ****************************************************************/
PRTERR NEAR PASCAL
DoPrintFile( HPRTJOB hPJ, LPSTR lpszFile, LPPAGEDESC lpPD )
{
#if 0
   PRTERR      prterr;
   OFSTRUCT    of ;
   LPPRINTFILE lpPF ;
   GLOBALHANDLE ghPF ;
   HDC         hDC ;
   ERR         n ;
   TEXTMETRIC  tm ;
   char        szMsg[80] ;


   if (!(ghPF = GlobalAlloc( GHND, sizeof( PRINTFILE ) )))
      return PRTERR_GEN ;

   //
   // Get DC from Print Core
   //
   if (!(hDC = GetPrintJobDC( hPJ )))
   {
      GlobalFree( ghPF ) ;
      return PRTERR_GEN ;
   }

   lpPF = (LPPRINTFILE)GlobalLock( ghPF ) ;
   //
   // Open the file
   //
   if (NULL == (lpPF->fFile = ( fdopen( OpenFile( lpszFile,
                                               &of, OF_READ ), "rt" ))))
   {
      GlobalUnlock( ghPF ) ;
      GlobalFree( ghPF ) ;
      return ERR_OPEN ;
   }

   lpPF->cPrev = NULL ;

   //
   // Create fonts
   //
   if (!(lpPF->hfontText = CreateFontIndirect( &lpPD->lfText )))
   {
      DP( hWDB,  "Could not create font") ;
      GlobalUnlock( ghPF ) ;
      GlobalFree( ghPF ) ;
      return ERR_OPEN ;
   }

   if (lpPD->szHeader)
      lpPF->hfontHF = CreateFontIndirect( &lpPD->lfHF ) ;
   else
      lpPF->hfontHF = NULL ;

   //
   // Figure out page size
   //
   SelectObject( hDC, lpPF->hfontText ) ;

   GetTextMetrics( hDC, &tm ) ;

   lpPF->xChar = tm.tmAveCharWidth ;
   lpPF->yChar = tm.tmHeight + tm.tmExternalLeading ;

   if (lpPF->xChar)
      lpPF->nCharsPerLine = GetDeviceCaps( hDC, HORZRES ) / lpPF->xChar ;
   else
   {
      DP( hWDB,  "xChar == 0") ;
      GlobalUnlock( ghPF ) ;
      GlobalFree( ghPF ) ;
      return ERR_OPEN ;
   }

   if (lpPF->yChar)
      lpPF->nLinesPerPage = (GetDeviceCaps( hDC, VERTRES ) -
                            (lpPF->yChar / 2)) / lpPF->yChar ;
   else
   {
      DP( hWDB,  "yChar == 0") ;
      GlobalUnlock( ghPF ) ;
      GlobalFree( ghPF ) ;
      return ERR_OPEN ;
   }

   lpPF->nPage = 1 ;

   n = ERR_OK ;
   prterr = PRTERR_OK ;

   DP( hWDB, "Printing: nLines = %d, nChars = %d", lpPF->nLinesPerPage, lpPF->nCharsPerLine ) ;

   while( n != ERR_EOF && prterr == PRTERR_OK )
   {
      wsprintf( szMsg, "Page %d", lpPF->nPage ) ;
      SetPrintJobMsg( hPJ, szMsg ) ;

      n = PrintPage( hDC, lpPF ) ;

      prterr = NewFrame( hPJ ) ;
      lpPF->nPage++ ;
   }

   DeleteObject( lpPF->hfontText ) ;
   DeleteObject( lpPF->hfontHF ) ;

   fclose( lpPF->fFile ) ;

   GlobalUnlock( ghPF ) ;
   GlobalFree( ghPF ) ;
   return prterr ;
#endif
} /* DoPrintFile()  */


/****************************************************************
 *  ERR PrintPage( HDC hPrnDC, LPPRINTFILE lpPF )
 *
 *  Description: 
 *
 *    Prints a page.
 *
 *  Comments:
 *
 ****************************************************************/
ERR PrintPage( HDC hPrnDC, LPPRINTFILE lpPF )
{
#if 0
   int            nLine ;
   ERR            eErr ;
   char           *szBuffer ;

   HFONT hfontTextTemp ;

   if (lpPF->hfontText)
      hfontTextTemp = SelectObject( hPrnDC, lpPF->hfontText ) ;

   szBuffer = (PSTR)LocalAlloc( LPTR, lpPF->nCharsPerLine+1 ) ;

   nLine = 0 ;
   while (nLine < lpPF->nLinesPerPage)
   {
      eErr = MyReadLine( szBuffer,
                         lpPF->nCharsPerLine,
                         lpPF->fFile,
                         &lpPF->cPrev ) ;
      switch (eErr)
      {
         case ERR_OK :
            TextOut( hPrnDC, 0,
                           lpPF->yChar * nLine,
                           szBuffer,
                           lstrlen( szBuffer ) ) ;
            break ;

         case ERR_EOP :
            TextOut( hPrnDC, 0,
                           lpPF->yChar * nLine,
                           szBuffer,
                           lstrlen( szBuffer ) ) ;
            nLine = lpPF->nLinesPerPage ;
            break ;


         case ERR_EOF :
            TextOut( hPrnDC, 0,
                           lpPF->yChar * nLine,
                           szBuffer,
                           lstrlen( szBuffer ) ) ;

         default :
            LocalFree( (LOCALHANDLE)szBuffer ) ;
            SelectObject( hPrnDC, hfontTextTemp ) ;
            return eErr ;
      }

      nLine++ ;
   }

   LocalFree((LOCALHANDLE) szBuffer ) ;
   SelectObject( hPrnDC, hfontTextTemp ) ;
   return ERR_OK ;
#endif
} /* PrintPage()  */


/****************************************************************
 *  ERR MyReadLine( LPSTR lpszBuffer, int nLineLen, FILE *fFile, LPSTR lpcPrev )
 *
 *  Description: 
 *
 *  Comments:
 *
 ****************************************************************/
ERR MyReadLine( LPSTR lpszBuffer, int nLineLen, FILE *fFile, LPSTR lpcPrev )
{
#if 0
   short i ;
   char  c ;

   lpszBuffer[0] = '\0' ;

   while ((lstrlen (lpszBuffer) < nLineLen) &&
          EOF != (c = (*lpcPrev ? *lpcPrev : (char)fgetc( fFile ))))
   {
      /* if last char was CR/LF then see if next one is too */
      if (*lpcPrev == '\n' || *lpcPrev == '\r' || *lpcPrev == '\f') 
         c = (char)fgetc (fFile) ;

      *lpcPrev = NULL ;

      switch (c)
      {
         case '\n':
               *lpcPrev = (char)fgetc( fFile ) ;
               return ERR_OK ;

         case '\r':
               *lpcPrev = (char)fgetc( fFile ) ;
               return ERR_OK ;

         case '\f':
               *lpcPrev = (char)fgetc( fFile ) ;
               //lstrcat (lpszBuffer, "\f") ;
               return (*lpcPrev == EOF || *lpcPrev == '\x1A' ? ERR_EOF : ERR_EOP) ;

         case '\x1A':
         case EOF:
               return ERR_EOF ;
               break ;

         default:
               lpszBuffer [i = lstrlen(lpszBuffer)] = c ;
               lpszBuffer [i+1] = '\0' ;
      }
   }
   if (feof( fFile ))
      return ERR_EOF ;
   else
      if (i = ferror( fFile ))
         return ERR_READ ;

   return ERR_OK ;
#endif
} /* MyReadLine()  */


/************************************************************************
 * End of File: prtfile.c
 ***********************************************************************/

