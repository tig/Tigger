/* ws_init.c */
#define MAX_INI_LEN     128 

GLOBALHANDLE WINAPI GetStrings( HANDLE hInst ) ;
BOOL WINAPI WinPrintInit (HANDLE hInstance) ;
BOOL WINAPI DoCmdLine (HWND hDlg, char *szResult) ;
VOID WINAPI MergeCmdLine( VOID ) ;

