/* ws_print.c */

typedef BOOL (CALLBACK* LPFNABORTPROC)(HDC, int);

VOID FAR PASCAL AbortPrintJob( VOID ) ;
BOOL FAR PASCAL PrintFiles( HWND hDlg, HWND hwndSel, HWND hwndHid, LPSTR lpszFile ) ;
VOID FAR PASCAL StartPrintUI( VOID );
VOID FAR PASCAL EndPrintUI( VOID );

