int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
BOOL PrintCancel( HWND hWnd ) ;
BOOL PaintInfo( HDC hDC, int xChar, int yChar ) ;
HDC GetPrinterDC (void) ;
int _ExtDeviceMode( HWND       hWnd,
                    LPDEVMODE  lpDevModeOutput,
                    LPDEVMODE  lpDevModeInput,
                    WORD       wMode ) ;
void dumpDevMode(LPDEVMODE lpEnv) ;

typedef struct tagBININFO
{
   short BinNumber ;
   short NbrofBins ;
   short Reserved1 ;
   short Reserved2 ;
   short Reserved3 ;
   short Reserved4 ;
} BININFO, *PBININFO ;



#ifdef DEBUG
      #include "dprintf.h"
      int dp( LPSTR lpszFormat, ... );
      int dprintf( LPSTR lpszFormat, ... ) ;

      #define D(a)   {a;}
#else
      #define D(a)   ;
#endif

