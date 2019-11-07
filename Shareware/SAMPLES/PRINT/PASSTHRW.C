/* Module for printing text using the PASSTHROUGH escape */

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
extern   char     *szErrorText[] ;

BOOL     bUserAbort ;
HWND     hDlgPrint ;

char     szCurPrinterName [] ;
char     szCurPort [] ;
char     szCurDriver [] ;

errors PrintPageWithPassthrough( HDC hDC, LPSTR lpszPageName ) ;

BOOL PrintPassThrough( ) 
{
char        szMsg [80] ;
FARPROC     lpfnAbortProc,
            lpfnPrintDlgProc ;
HDC         hPrnDC ;
errors      eErr ;
short rv ;
int i ;
   
   if (NULL == (hPrnDC = GetPrinterDC() ))
   {
      MessageBox(hWndMain, "Could not get Device Context","Error: PrintFile",
         MB_ICONEXCLAMATION ) ;
      return TRUE ;
   }
   D( dp("Printing using PASSTROUGH... Printer = %s Port = %s",
         (LPSTR)szCurPrinterName, (LPSTR)szCurPort) ) ;

   EnableWindow( hWndMain, FALSE ) ;

   bUserAbort = FALSE ;
   lpfnPrintDlgProc = MakeProcInstance( PrintDlgProc, hInst ) ;
   hDlgPrint = CreateDialog( hInst, "CancelBox", hWndMain, lpfnPrintDlgProc ) ;
   D( dp("hDlgPrint = %#04X", hDlgPrint) ) ;

   lpfnAbortProc = MakeProcInstance( AbortProc, hInst ) ;
   Escape( hPrnDC, SETABORTPROC, NULL, (LPSTR)lpfnAbortProc, (LPSTR)NULL ) ;

   SetDlgItemText( hDlgPrint, IDD_PRINTER_TXT, "(working...)" ) ;

   if ((rv = Escape( hPrnDC, STARTDOC, lstrlen( szMsg ), (LPSTR)szMsg, NULL )) > 0)
   {
      if (hDlgPrint)
      {           
         wsprintf( szMsg, (LPSTR)"Sending data to" ) ;
         SetDlgItemText( hDlgPrint, IDD_PRINTER_TXT, szMsg ) ;
      }

      //
      // Do passthrough Escapes...
      //
      eErr = PrintPageWithPassthrough( hPrnDC, NULL ) ;

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

   D(dp( "eErr %i = \"%s\"",
          eErr,
          (LPSTR)GetErrorText(eErr) )) ;
   return TRUE ;

//-------------------------------------------------------------
Error1:
   wsprintf( (LPSTR)szMsg,
             (LPSTR)"Could not print: eErr = %i %s",
             eErr,
             (LPSTR)GetErrorText(eErr) ) ;

   EnableWindow( hWndMain, TRUE ) ; 
   MessageBox( hWndMain, szMsg, szAppName, MB_OK | MB_ICONEXCLAMATION ) ;

   FreeProcInstance( lpfnPrintDlgProc ) ;
   FreeProcInstance( lpfnAbortProc ) ;
   DeleteDC( hPrnDC ) ;
   return FALSE ;
}

typedef struct  tagPTDATA
            {
               WORD wLen ;
               char szBuf[256] ;
            } PTDATA, FAR *LPPTDATA ;

errors PrintPageWithPassthrough( HDC hDC, LPSTR lpszPageName )
{
short       rv ;
PTDATA      ptData ;

   //
   // build the structure
   //
   lstrcpy( ptData.szBuf, "Hello World\n\r\fGoodbye World!\n\r" ) ;
   ptData.wLen = lstrlen( ptData.szBuf ) ;

   rv = Escape( hDC, PASSTHROUGH, sizeof( ptData ), &ptData, NULL ) ;
   D(dp( "PASSTHROUGH == %d", rv ) ) ;

   //
   // Second page
   //   
   lstrcpy( ptData.szBuf, "This is a test of the emergency broadcast system of America.  This is only a test.\n\r" ) ;
   ptData.wLen = lstrlen( ptData.szBuf ) ;

   rv = Escape( hDC, PASSTHROUGH, sizeof( ptData ), &ptData, NULL ) ;
   D(dp( "PASSTHROUGH == %d", rv ) ) ;

   //
   // Second page
   //   
   lstrcpy( ptData.szBuf, "If this were not a test, you would be dead.\n\r" ) ;
   ptData.wLen = lstrlen( ptData.szBuf ) ;

   rv = Escape( hDC, PASSTHROUGH, sizeof( ptData ), &ptData, NULL ) ;
   D(dp( "PASSTHROUGH == %d", rv ) ) ;

   if ((rv = Escape( hDC, NEWFRAME, 0, 0L, 0L )) < 0)
   {
      D( dp("ERR_NEWFRAME.  rv = %d",rv) ) ;
      return ERR_NEWFRAME ;
   }

   return ERR_OK ;
}


