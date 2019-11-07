/************************************************************************
 *                        DLGHELP.C FUNCTIONS
 ************************************************************************/

void WINAPI
DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient ) ;

UINT WINAPI
DlgWhichRadioButton( HWND hDlg, UINT wFirstID, UINT wLastID, UINT wDefault )  ;

short FAR _cdecl ErrorResBox( HWND hWnd, HINSTANCE hInst,
                              UINT wFlags, UINT idAppName,
                              UINT idErrorStr, ... ) ;


