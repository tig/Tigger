#define GH_TEXT	0
#define GH_TOPICS	1
#define HELPTEXTWIDTH   50

#if 0
typedef struct	tHelpTopic {
	char	szName[32];		/* Name that appears in help topic list box */
	int	rID;				/* Resource ID which contains the help text */
} HelpTopic;
#endif

extern char szWhatsNew[];

/*-------------------------------------------------------------------------*/
/*                         G L O B A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
extern VOID FAR PASCAL	GetHelp(WORD, WORD);
extern BOOL FAR PASCAL  ShowWhatsNew (HWND);
extern LONG FAR PASCAL	WSHelpTextDlgProc(HWND, unsigned, WORD, LONG);
LONG FAR PASCAL 
WSMultiEditSubProc(HWND hWndDlg, unsigned message, WORD wParam, LONG lParam);
extern HWND	hWndHelpText;


