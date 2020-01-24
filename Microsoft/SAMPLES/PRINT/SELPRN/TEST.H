
/********************************************************************
 *    Function Prototypes
 ********************************************************************/
//
// Utility functions for this program
//
void DoSelPrn( HWND hWnd, WORD wFlags ) ;

BOOL PaintInfo( HDC hDC, int xChar, int yChar ) ;

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);

BOOL InitApplication(HANDLE);

BOOL InitInstance(HANDLE, int);

long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);

BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);


