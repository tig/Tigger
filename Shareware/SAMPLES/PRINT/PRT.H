//prt.h

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

BOOL Print( LPSTR lpszFileName ) ;
BOOL PrintPassThrough( void ) ;

errors PrintPageofJunk( HDC hDC, LPSTR lpszPageName ) ;
errors PrintFile( HDC hDC, char szFileName[] ) ;
errors PrintPage (HDC hPrnDC, FILE *fFile, int nPage) ;
errors MyReadLine (PSTR szBuffer, int nLineLen, FILE *fFile) ;
BOOL FAR PASCAL PrintDlgProc( HWND hDlg,
                              unsigned iMessage,
                              WORD wParam,
                              LONG lParam) ;
BOOL FAR PASCAL AbortProc( HDC hPrnDC, short nCode );
char *GetErrorText( errors eErr ) ;
BOOL PrintSetup( void ) ;


