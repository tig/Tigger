#include "selprnd.h"

//----------------------------------------------------------------
// SelPrn
//----------------------------------------------------------------
// Exported defines

#define SELPRN_STANDARD    0x0001
#define SELPRN_USEDEFAULT  0x0002
#define SELPRN_GETDEFAULT  0x0004
#define SELPRN_ALLOWSETUP  0x0008

#define DLG_CANCEL     IDCANCEL
#define DLG_OK         IDOK
#define DLG_ERROR      -1

int FAR PASCAL
PrintSelPrnDialog( HWND hwndParent,
                       LPSTR lpszTitle,
                       LPSTR lpszDriverName,
                       LPSTR lpszDeviceName,
                       LPSTR lpszPort,                       
                       WORD  wFlags ) ;

int FAR PASCAL
PrintGetDefaultPrn( LPSTR lpszDriverName,
                        LPSTR lpszDeviceName,
                        LPSTR lpszPortName ) ;

BOOL FAR PASCAL 
SelPrinterDlgProc( HWND hDlg,
                   unsigned iMessage,
                   WORD wParam,
                   LONG lParam ) ;



                                   


