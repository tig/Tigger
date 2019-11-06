/************************************************************************
 *                        DLGHELP.C FUNCTIONS
 ************************************************************************/

UINT WINAPI
DlgWhichRadioButton( HWND hDlg, UINT wFirstID, UINT wLastID, UINT wDefault ) ;

void WINAPI
DlgSetItemLength( HWND hDlg, UINT wID, UINT wLength ) ;

void WINAPI
DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient ) ;

UINT WINAPI
LBGetCurSel( HWND hLB ) ;

BOOL WINAPI
LBGetText( HWND hLB, UINT wItem, LPSTR lpszText, UINT wMax ) ;

BOOL WINAPI
LBAddString( HWND hLB, LPSTR lpszText ) ;

UINT WINAPI
LBFindString( HWND hLB, UINT wStart, LPSTR lpszPrefix ) ;

UINT WINAPI
LBSelectString( HWND hLB, UINT wStart, LPSTR lpszPrefix ) ;

short FAR _cdecl ErrorResBox( HWND hWnd, HANDLE hInst,
                              UINT wFlags, UINT idAppName,
                              UINT idErrorStr, ... ) ;


