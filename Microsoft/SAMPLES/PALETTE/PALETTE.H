int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
void DoPalette( HWND hWnd, HDC hDC ) ;
void DoAnimate( HWND hWnd, HDC hDC ) ;
void DoDiamond( HDC hDC ) ;

