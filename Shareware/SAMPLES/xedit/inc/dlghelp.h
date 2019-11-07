/************************************************************************
 *                        DLGHELP.C FUNCTIONS
 ************************************************************************/

WORD WINAPI
DlgWhichRadioButton( HWND hDlg, WORD wFirstID, WORD wLastID, WORD wDefault ) ;

void WINAPI
DlgSetItemLength( HWND hDlg, WORD wID, WORD wLength ) ;

void WINAPI
DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient ) ;

WORD WINAPI
LBGetCurSel( HWND hLB ) ;

BOOL WINAPI
LBGetText( HWND hLB, WORD wItem, LPSTR lpszText, WORD wMax ) ;

BOOL WINAPI
LBAddString( HWND hLB, LPSTR lpszText ) ;

WORD WINAPI
LBFindString( HWND hLB, WORD wStart, LPSTR lpszPrefix ) ;

WORD WINAPI
LBSelectString( HWND hLB, WORD wStart, LPSTR lpszPrefix ) ;

short FAR _cdecl ErrorResBox( HWND hWnd, HINSTANCE hInst,
                              WORD wFlags, WORD idAppName,
                              WORD idErrorStr, ... ) ;


