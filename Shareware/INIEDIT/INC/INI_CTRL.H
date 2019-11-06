int ProcessFileNameCB (LONG lParam) ;
int FillSectionLB (void) ;
int ProcessSectionLB (LONG lParam) ;

int FillNameLB (void) ;
int ProcessNameLB (LONG lParam) ;
BOOL ProfileToListBox( HWND hDlgItem, PSTR lpsBuffer ) ;
BOOL FillValueEC( void ) ;
BOOL ProcessValueEC( LONG lParam ) ;

BOOL FAR PASCAL ValueDlgProc( HWND hDlg,
                           unsigned iMessage,
                           WORD  wParam,
                           LONG lParam ) ;

void ParseKeyName( LPSTR lpszBuf )  ;

