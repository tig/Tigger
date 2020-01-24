/*-------------------------------------------------------------------------*/
/*                         G L O B A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
extern VOID FAR PASCAL	DownloadSingle(void);
extern LONG FAR PASCAL	WSDownloadSingleDlgProc(HWND, unsigned, WORD, LONG);
extern LONG FAR PASCAL	WSDownloadOrderDlgProc(HWND, unsigned, WORD, LONG);
extern LONG FAR PASCAL WSDldInfoDlgProc(HWND, unsigned, WORD, LONG);
extern LONG FAR PASCAL 
WSWhereFromDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam) ;

/* source of up-to-date files: */
extern int DldSource;
extern HWND hDldWnd;

/*
   set purpose of DldInfo dialog box to:
   set download clipart directory or
   set backup thumbnail directory or
   set alternate cache directory
*/
extern int nSetDldInfoMode;
extern BOOL bShowUseAltCacheBox;

#define DLD_INFO_MODE_CLIPART 0
#define DLD_INFO_MODE_ALTCACHE 1
#define DLD_INFO_MODE_BACKUP 2

