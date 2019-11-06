//ini_prt.h

//

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

BOOL Print( void ) ;
errors PrintFile( HDC hDC, char szFileName[] ) ;
errors PrintPage (HDC hPrnDC, FILE *fFile, int nPage) ;
errors MyReadLine (PSTR szBuffer, int nLineLen, FILE *fFile) ;
BOOL FAR PASCAL PrintDlgProc( HWND hDlg,
                              unsigned iMessage,
                              WORD wParam,
                              LONG lParam) ;
BOOL FAR PASCAL AbortProc( HDC hPrnDC, short nCode );
char *GetErrorText( errors eErr ) ;

