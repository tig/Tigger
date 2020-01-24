/************************************************************************
 *                        DLGHELP.C FUNCTIONS
 ************************************************************************/

UINT WINAPI
DlgWhichRadioButton( HWND hDlg, UINT wFirstID, UINT wLastID, UINT wDefault ) ;

void WINAPI
DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient ) ;

short FAR _cdecl ErrorResBox( HWND hWnd, HINSTANCE hInst,
                              UINT wFlags, UINT idAppName,
                              UINT idErrorStr, ... ) ;


