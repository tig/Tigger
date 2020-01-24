/*
 * Declare.h	declarations of all procedures in dlgsave.c, dlgopen.c,
 *		print.c, sample.c, and tools.asm.
 *------------------------------------------------------------------------*/

/*------ dlgsave.c ------*/
extern int  DlgSaveAs (HANDLE, HWND, int, OFSTRUCT *, int *, char *, char *, char *);
extern BOOL DlgFnSaveAs (HWND, unsigned, WORD, LONG);
extern VOID DlgInitSaveAs (HWND, OFSTRUCT *);
extern char * DlgPFileInPath (char *);
extern DlgAddExt (char *);
extern BOOL DlgMergeStrings (char *, char *, char *);


/*----- print.c -----*/
extern int PrintFile(HANDLE, HWND, int, OFSTRUCT, char *, char *);
extern int AbortTest(HDC, int);
extern int DlgFnAbort(HWND, unsigned, WORD, LONG);
extern HDC GetPrinterDC();
extern PrintErr(HWND, int);
extern CenterPopup(HWND);
extern GetScreenCenter(HWND, LPPOINT, int, int);

/*------ sample.c -------*/
extern LONG SampleWndProc (HWND, unsigned, WORD, LONG);
extern SampleCommand (HWND, WORD);
extern BOOL SampleAboutDlg (HWND, unsigned, WORD, LONG);
extern int WinMain (HANDLE, HANDLE, LPSTR, int);
extern BOOL SampleInit (HANDLE);
extern SampleCheckSave (HWND);
extern SampleUpdateTitle (HWND);


/*------ tools.asm -------*/
extern LPSTR lstrcpy( LPSTR, LPSTR );
extern LPSTR lstrcat( LPSTR, LPSTR );
extern int   lstrlen( LPSTR );
extern VOID  _lclose( int );
