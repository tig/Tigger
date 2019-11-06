void DelName( void ) ;
void AddName( void ) ;
void DelSection( void ) ;
void AddSection( void ) ;

BOOL FAR PASCAL AddSectionDlgProc( HWND hDlg,
                              unsigned iMessage,
                              WORD  wParam,
                              LONG lParam ) ;

BOOL FAR PASCAL AddNameDlgProc( HWND hDlg,
                              unsigned iMessage,
                              WORD  wParam,
                              LONG lParam ) ;

