#ifndef WSH
#define WSH

#include "stdlib.h"
#include "stdinc.h"

/* WM_USER wParam values */
#define WM_MINUTE_WARNING   (WM_USER+100)

/* Debugging Defines */

#if defined(DEBUG) || defined(INTERNAL)
#define  Assert(stmt)   stmt
#else
#define  Assert(stmt)   
#endif

extern WORD CurVersion;
extern HANDLE hTaskWS;

#define END(x) {nRetval = x; goto end;}
#define END1(x) {nRetval = SetErr(x); goto end;}
#define END2(x) {SetErr(x,szFileName);nRetval=x;goto end;}
#define END3(x,y) {SetErr(x,szLRUFileName);nRetval=y;goto end;}
#define REFERENCE_FORMAL_PARAM(x) (unsigned int) x;

#define SURCHARGE(x)    (round_cents((x) * 0.05))
#define TAXES(x)        (round_cents((x) * 0.082))

#define DEBUG_CACHE		
/** STR_LEN is only for stuff coming in from the WSdatabase */
#define MAX_STR_LEN         120
/** MSG_LEN is for stuff coming in from the resource file **/
#define MAX_MSG_LEN         256
#define MAX_PRICE_LEN	     12
#define MAX_BASE_LEN          8
#define MAX_CLIP_LEN	         8
#define PW_SIZE               8
#define ALL                   0
#define ALL_SAUF_PASSWORD     1
#define NUM_CACHE_OPTIONS     14

#define SEC_PER_MIN  60
#define SEC_PER_HR   3600
#define TIMER_INTERVAL  5000     // msecs
#ifdef BETA
    #define TIMER_DISCONNECT      2      // minutes
#else
    #define TIMER_DISCONNECT      10      // minutes
#endif
#define TIMER_WARNING         1

typedef int	SWORD;
typedef long near *PLONG;
//typedef unsigned char BYTE;

typedef struct
{
    char szLibName[MAX_STR_LEN];
    char szLibFile[_MAX_PATH];
    char szPubName[MAX_STR_LEN];
    char szPubFile[MAX_STR_LEN];
    char szSubLibName[MAX_STR_LEN];
    char szThumbNailFileName[_MAX_PATH];
    char szThumbInfoFileName[_MAX_PATH];
    char szVersion[MAX_STR_LEN];
    char szMedia[MAX_STR_LEN];
    char szSRPrice[MAX_PRICE_LEN];
    char szWSPrice[MAX_PRICE_LEN];
    char szClipFileName[_MAX_PATH];
    char szClipDesc[MAX_STR_LEN];
    char szClipSize[MAX_CLIP_LEN];
    BOOL bDownloadable;
    char szNumImages[6];
}
LIBRARY;

#define CURPUB (CurLib.szPubName)
#define CURLIB (CurLib.szLibName)
#define CURLIBFILE (CurLib.szLibFile)
#define CURPUBFILE (CurLib.szPubFile)
#define CURSUBLIB (CurLib.szSubLibName)
#define CURTHUMBNAIL (CurLib.szThumbNailFileName)
#define CURTHUMBINFO (CurLib.szThumbInfoFileName)
#define CURVERSION (CurLib.szVersion)
#define CURMEDIA (CurLib.szMedia)
#define CURSRPRICE (CurLib.szSRPrice)
#define CURWSPRICE (CurLib.szWSPrice)
#define CURCLIPSIZE (CurLib.szClipSize)
#define CURCLIPFILENAME (CurLib.szClipFileName)
#define CURCLIPDESC (CurLib.szClipDesc)
#define CURLIBDLDABLE (CurLib.bDownloadable)
#define CURLIBNUMIMAGES (CurLib.szNumImages)
#define CURLIBMSGNUM (CurLib.nMsgNumber)

#define WIDTH_RECT(rct) (rct.right-rct.left) +  1
#define HEIGHT_RECT(rct) (rct.bottom-rct.top) + 1 

#define WITH_DC(hDC, hWnd)	\
    for(bUsingDC=TRUE, hDC = GetDC(hWnd); \
	(bUsingDC);    \
	bUsingDC=FALSE,ReleaseDC(hWnd, hDC))

#define HIGHLIGHT(hWnd); SendMessage(hWnd, EM_SETSEL,\
        NULL, MAKELONG(0, 0x7fff));

#define DLG_TO_SCR_X(X_CRD)   (((X_CRD) * (LOWORD(GetDialogBaseUnits()))) / 4)
#define DLG_TO_SCR_Y(Y_CRD)   (((Y_CRD) * (HIWORD(GetDialogBaseUnits()))) / 8)

/*-------------------------------------------------------------------------*/
/*                        G L O B A L    D A T A                           */
/*-------------------------------------------------------------------------*/

extern DWORD lLastMsgTime;  // defined in WSINIT.C

//defined in messages.c:
extern char szEnterSettings[];
extern char szWelcome[];
extern char szConnecting[];
extern char szUpgrading[];
extern char szNewUser[];
extern char szInitializing[];
extern char szInitComplete[];
extern char	szLibSelections[];
extern char	szHangingUp[];
extern char szWindowShopper[];
extern char szUpdateCust[];
extern char szUpdateData[];

typedef struct 
{
    FARPROC lpfn;
    HWND hWnd;
} CHILDREC;

extern  BOOL     bNewUser;
extern  FARPROC  lpfnKeyHook, lpfnOldHook;
extern  FARPROC  lpprocEditWnd;
extern  CHILDREC hChildren[];
extern  char     szAppName[];
extern  char     szWinIniLabel[];
extern  char     szTitleAppName[];
extern  LIBRARY  CurLib;
extern	HANDLE	 hInstanceWS;
extern	HANDLE	 hWndWS;
extern	HANDLE	 hWndStatusBar;
extern  HANDLE   hWndTimer;
extern	WORD	 wsGlobalError;  /* !!! DON'T THINK THIS IS USED */
extern	WORD	 nCurViewMode;
extern  int     nMissingThumbContext;
extern  DWORD    dwCacheSize;    /* user defined cache value -- multiply this value by 1000 to get bytes */
extern	HANDLE	 hAccel;
extern	HANDLE	 hAccelView;
extern	FARPROC	 lpprocViewDlg;
extern	FARPROC	 lpprocHelpTextDlg;
extern	FARPROC	 lpprocAboutDlg;
extern	FARPROC	 lpprocSearchDlg;
extern	FARPROC	 lpprocStopSearchDlg;
extern	FARPROC	 lpprocTimerDlg;
extern	FARPROC	 lpprocBusyDlg;
extern	HWND	 hWndView;
extern	HWND	 hWndSearch;
extern  char     szMasterFileName[];
extern  char     szExePath[];
extern  char     szDataPath[];
extern  BOOL     bUsingDC;
extern  BOOL     bMonochrome;
extern  HCURSOR  hPhoneCursArr[];
extern  HCURSOR  hSearchCurs;
extern  WORD     wExeType;
extern  WORD     wWinVer;
extern  WORD     nOffsetToPass;

/****************************bg ********************************************
   To control the enabling and disabling of the main WS window since the with
   the implementation of the search feature we've made the viewbox modeless so
   the user can switch back and forth between the viewbox and the search dialog
   but we also don't want them switching to the main window from either the search
   or viewbox we must disable the main window whenever the either the viewbox
   or the serach is active or both
*********************************6/18/91*************************************/
extern  int      nMainWinDisableCount;


#ifdef INTERNAL
#define  FREEZE_HWND_WS    ++nMainWinDisableCount; \
                           EnableWindow (hWndWS, FALSE); \
                           AuxPrint("Freezing HWndWS, freeze count is %d", nMainWinDisableCount);

#define  THAW_HWND_WS      nMainWinDisableCount = max(0, nMainWinDisableCount - 1); \
                           if (NOT nMainWinDisableCount) EnableWindow (hWndWS, TRUE); \
                           AuxPrint("Thawing HWndWS, freeze count is %d", nMainWinDisableCount);
#else
#define  FREEZE_HWND_WS    ++nMainWinDisableCount; \
                           EnableWindow (hWndWS, FALSE);

#define  THAW_HWND_WS      nMainWinDisableCount = max(0, nMainWinDisableCount - 1); \
                           if (NOT nMainWinDisableCount) EnableWindow (hWndWS, TRUE);
#endif

#ifdef INTERNAL
   extern BOOL bCallRetail;
#endif

/* defines for wExeType */
#define EXE_BETA        0
#define EXE_REGULAR     1
#define EXE_INTERNAL    2


extern  char     szCategoryFileName[_MAX_PATH];
extern  char     szPublishersFileName[_MAX_PATH];

/*-------------------------------------------------------------------------*/
/*                           L O C A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
BOOL	FAR PASCAL FillListBoxes(HWND,BOOL);
static BOOL NEAR PASCAL FillLibList(HWND);
static int FAR  PASCAL FillDesc(HWND,WORD);
static void NEAR PASCAL Iconize(BOOL bOnOff);
extern BOOL             GiveUpdateOption (HWND hDlg);

/*-------------------------------------------------------------------------*/
/*                         G L O B A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
extern LONG FAR PASCAL	WSMainDlgProc(HWND, unsigned, WORD, LONG);
extern BOOL FAR PASCAL	WSInit(HANDLE, HANDLE, int);      /* wsinit.c */
extern LONG FAR PASCAL  WSLibraryDlgProc (HWND, unsigned, WORD, LONG);
extern BOOL FAR PASCAL  AddStringToListBox (char *, HWND);
extern BOOL FAR PASCAL  AddIndirectionToListBox (char *szFileName, HWND hWndListBox);
extern BOOL FAR PASCAL  AddIndirectionToLibListBox (char *szFileName, HWND hWndListBox);

extern LONG FAR PASCAL	WSAboutDlgProc(HWND, unsigned, WORD, LONG);
extern LONG FAR PASCAL  WSTimerDlgProc (HWND hDlg, unsigned message, WORD wParam, LONG lParam);
#endif


