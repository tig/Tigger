/*-------------------------------------------------------------------------*/
/*                         G L O B A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
#define WILDCARD "*.*"
extern HWND hMommaWnd;

extern void EnableAllModeless(BOOL bEnable);
extern BOOL FAR PASCAL EnableWindowProc(HWND hWnd, LONG OnOff);
extern void EnableAllWindows(BOOL bOnoff);
HCURSOR FAR PASCAL MySetCursor(HCURSOR hCursor);
extern int FAR PASCAL GetDecision(void);
LONG FAR PASCAL WSDecisionDlgProc (HWND hDlg, unsigned message, WORD wParam, LONG lParam);
extern int FAR PASCAL GetAltDrive(void);
LONG FAR PASCAL WSGetAltDriveDlgProc (HWND hDlg, unsigned message, WORD wParam, LONG lParam);
unsigned long FAR PASCAL AvailDiskSpace(int nWhichDisk);
void PASCAL HandleIfNewUser(void);
BOOL FAR PASCAL GetExePath(void);
BOOL FAR PASCAL DirExists(char* path);
extern void FAR PASCAL  NewUserConfig(HMENU hM, BOOL bFlag);
int SuperClassEdits(void);
BOOL FAR PASCAL CreateLibWnd(void);
WORD GetWindowsVersion(void);
char * BaseName(char *szFileName);
void get_cur_dir(char *dir);
int ExtractFromCache(char *szFileName, BOOL bDelete);
void FAR PASCAL FixSpec(char *str);
char* FAR PASCAL UnFixSpec(char *str);
char* FAR PASCAL Append(char *str1, char *str2);
BOOL FAR PASCAL IsFile(char *str);
int FAR PASCAL FillEditBox(int fp, HANDLE hEdit, char StopChar);
extern  char * FAR PASCAL AddFullPath(char *str);
char* FAR PASCAL StripPath(char *str);
#if 0
char* FAR PASCAL MakeParentDir(char *szPath);
#endif
VOID FAR PASCAL CenterPopUp(HWND);
VOID FAR PASCAL SetStatusBarText(char *);
VOID FAR PASCAL EnableMain(HWND, BOOL);
void FAR wprintf(NPSTR, ...);
int FAR PASCAL QueryListSel(HWND hWnd);
void FAR PASCAL ftoa(double f, char *str, int fract_digits);
void FAR PASCAL ultostr(unsigned long l, char *s, int padchars);
void FAR PASCAL ultostr10(unsigned long l, char *s, int padchars);
#ifdef INTERNAL
void FAR AuxPrint(char *smsg,...);
#endif
int PASCAL CopyFile(char *sSrcName,char *sDestName);
extern DWORD FAR PASCAL FileSize(char *szSrc) ;
void FAR PASCAL FixString (char *InString, char *FixedString, int size);
BOOL FixDirPath (HWND hWndText, char *OldText, char *NewText, int size);
BOOL isWashington (char *state);
char * FAR PASCAL stoupper(char *str);
double round_cents(double d);


