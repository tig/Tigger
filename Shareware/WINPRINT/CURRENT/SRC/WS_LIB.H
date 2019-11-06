/* ws_lib.c */

HDC               GetPrinterDC (void) ;
HDC               GetPrinterIC (void) ;

VOID FAR PASCAL StoP( HDC hdcPrn, int FAR *lpnHeight ) ;
VOID FAR PASCAL PtoS( HDC hdcPrn, int FAR *lpnHeight ) ;

GLOBALHANDLE FAR PASCAL GetDevMode( GLOBALHANDLE ghDN ) ;

BOOL              GetPageInfo (HDC hDC, PAGEPTR pPage) ;
void              WriteLF (LPLOGFONT lplf) ;

void FAR PASCAL SaveWindowPos( HWND hWnd ) ;
void FAR PASCAL WritePrivateProfileInt(LPSTR szSection, LPSTR szKey, int i, LPSTR szIniFile) ;
void FAR PASCAL PlaceWindow( HWND hWnd ) ;
DWORD FAR PASCAL GetProfileDWORD( LPSTR lpszSec, LPSTR lpszKey, DWORD dwDef ) ;
WORD FAR PASCAL GetProfileWORD( LPSTR lpszSec, LPSTR lpszKey, WORD wDef ) ;
VOID FAR PASCAL WriteProfileDWORD( LPSTR lpszSec, LPSTR lpszKey, DWORD dw ) ;
VOID FAR PASCAL WriteProfileWORD( LPSTR lpszSec, LPSTR lpszKey, WORD w ) ;


