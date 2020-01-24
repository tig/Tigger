#define NUM_BAUD_BUTTONS	6
#define NUM_PORT_BUTTONS	4

/*-------------------------------------------------------------------------*/
/*                         G L O B A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
BOOL FAR PASCAL	GetSettings(WORD *, WORD *, char *);
LONG FAR PASCAL	WSSettingsDlgProc (HWND, unsigned, WORD, LONG);
WORD FAR PASCAL	VerifyLogon(char *);
extern DWORD FAR PASCAL GetCacheSize(WORD cacheindex);
extern LONG FAR PASCAL WSLibManagerDlgProc (HWND, unsigned, WORD, LONG);
extern LONG FAR PASCAL WSCommInfoDlgProc (HWND, unsigned, WORD, LONG);


/*-------------------------------------------------------------------------*/
/*                           L O C A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
VOID NEAR PASCAL ValidateBaudRate(WORD *);
VOID NEAR PASCAL ValidatePort(WORD *);

