#define WM_MYINIT 0x0401

//#define USEADVANCED

#define BIT_2X       1
#define BIT_30       11
#define BIT_MEMORY   10
#define BIT_FONT     9

// bit (bit number, variable)
#define bit(p,x) (BOOL)((WORD)x >> p & (WORD)~((WORD)~(WORD)0 << 1) )

enum mark_err 
{
   ME_OK          = -1,
   ME_BADOPEN     = -2,
   ME_NOHEADER    = -3,
   ME_BADFORMAT   = -4,
   ME_SEEK        = -5,
   ME_READ        = -6,
   ME_WRITE       = -7,
   ME_WRITEFAIL   = -8,
   ME_30          = -9
} ;

long FAR PASCAL WndProc (HWND hDlg, unsigned iMessage, WORD wParam, LONG lParam) ;
void OkMessageBox (HWND hDlg, char *szString, char *szFileName) ;
WORD Check30File ( PSTR szFileName ) ;
int Mark30File( PSTR szFileName, WORD dwBits ) ;
BOOL DrawShadow (HDC hDC, HWND hWnd, HBRUSH hBrush, short ShadowWidth) ;
BOOL ColorText (HDC hDC, HWND hWnd, DWORD dwBrush, LPSTR lpszMsg) ;
BOOL FAR PASCAL AboutDlgProc (HWND hDlg, unsigned iMessage, WORD wParam, LONG lParam) ;
BOOL FAR PASCAL WarnDlgProc (HWND hDlg, unsigned iMessage, WORD wParam, LONG lParam) ;
BOOL FAR PASCAL AdvDlgProc (HWND hDlg, unsigned iMessage, WORD wParam, LONG lParam) ; 
BOOL InitWnd (HANDLE hInstance) ;
static char *astrrchr (char *str, int ch) ;
static char *astrchr (char *str, int ch) ;

