/*************************************************************\
 ** FILE:  wsinit.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **
 ** HISTORY:   
 **   Doug Kent  ??? (created)
 **
\*************************************************************/
#include <string.h>
#include <io.h>
#include "nowin.h"
#include "windows.h"
#include "winundoc.h"
#include "ws.h"
#include "error.h"
#include "file.h"
#include "viewbox.h"
#include "help.h"
#include "rc_symb.h"
#include "custinfo.h"
#include "libmgr.h"
#include "misc.h"
#include "order.h"
#include "comm.h"
#include "settings.h"
#include "busy.h"
#include "ksearch.h"

DWORD FAR PASCAL KeyboardHook(int, WORD, LONG);
DWORD FAR PASCAL MessageHook(int, WORD, LONG);

HANDLE hTaskWS=NULL;
DWORD    dwCacheSize ;
GLOBALHANDLE hLRU = NULL;
char    szLRUFileName[]="00000004.001";
BOOL    bNewUser=TRUE;
BOOL    bMonochrome = FALSE;
char    szAppName[40];
char    szWinIniLabel[40];
char    szTitleAppName[40];
char    szBuf[40] ;
FARPROC lpfnKeyHook, lpfnOldHook;
//FARPROC lpfnMsgHook, lpfnOldMsgHook;
WORD    wExeType,wWinVer;
WORD    nOffsetToPass;
DWORD   lLastMsgTime = 0L;
int     nMainWinDisableCount = 0;   //to control enabling, disabling of hWndWS

#ifdef INTERNAL
   BOOL bCallRetail = FALSE;
#endif

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine,
                     int nCmdShow)
{
    MSG         msg;

    REFERENCE_FORMAL_PARAM(lpszCmdLine);

    hInstanceWS=hInstance;

    if (!WSInit(hInstance,hPrevInstance,nCmdShow))
         return (0);

    /** note that in handling hWndView, we are assuming only one
        such view exists.  Also note we are assuming no HelpTopics
        window is open.  (2.11.90) D. Kent **/
    while (GetMessage((LPMSG)&msg, NULL, 0, 0))
    {
        //if (msg.message == WM_ACTIVATE)
#if 0
        {
            hMommaWnd = msg.hwnd;
#ifdef INTERNAL
            //AuxPrint("(Get) MommaWnd: %x, msg: %x",hMommaWnd,msg.message);
#endif
        }
#endif

        if (!hWndView OR !TranslateAccelerator(hWndView, hAccelView, &msg))

         if(!TranslateAccelerator(hWndWS, hAccel, &msg))

          if (!hWndView OR !IsDialogMessage(hWndView,(LPMSG)&msg))

            if (!hWndSearch OR !IsDialogMessage(hWndSearch, (LPMSG)&msg))

                if (!hWndHelpText OR !IsDialogMessage(hWndHelpText,(LPMSG)&msg))
                {
                    TranslateMessage((LPMSG)&msg);
                    DispatchMessage((LPMSG)&msg);
                }
    }
    return (msg.wParam);
}

BOOL FAR PASCAL 
WSInit(HANDLE hInstance, HANDLE hPrevInstance, int nCmdShow) 
{
#if 0
    //see GetProfileString() below
    char szModemParam[MAX_STR_LEN];
#endif
    HDC hDC;
    char szLRU[_MAX_PATH];
    int x;

/* this condition should be defined on compile command line */
#if defined(BETA)
    wExeType = EXE_BETA;
#elif defined(INTERNAL)
    wExeType = EXE_INTERNAL;
#else
    wExeType = EXE_REGULAR;
#endif

    LoadString(hInstance, IDS_APPNAME,       szAppName,      sizeof(szAppName));
    LoadString(hInstance, IDS_WININILABEL,   szWinIniLabel,  sizeof(szWinIniLabel));
    LoadString(hInstance, IDS_TITLE_APPNAME, szTitleAppName, sizeof(szTitleAppName));

#if 0
    if (GetProfileString(szWinIniLabel, "modem", "",
                         szModemParam, sizeof(szModemParam)) == 0)
        {
        // insert modem code below this line
        strcpy(szModemParam, "testparam");
        // insert modem code above this line
        WriteProfileString(szWinIniLabel, "modem", szModemParam);
        }
    else
      ;  //do something with this param
#endif

	if (hPrevInstance)  
    {
		/*---------------------*/
		/* Not first instance  */
		/*---------------------*/
		ErrMsg(IDS_ERR_SECOND_INSTANCE);
		return(FALSE);
	}

    hInstanceWS = hInstance;
    hTaskWS = GetCurrentTask();

    //Load Cursors
    for (x = 0; x < 4; x++)
         hPhoneCursArr[x] = LoadCursor (hInstanceWS, MAKEINTRESOURCE(PHONE1_CURSOR + x));
    hSearchCurs = LoadCursor (hInstanceWS, MAKEINTRESOURCE(SEARCH_CURSOR));

    wWinVer = GetWindowsVersion();

    if (wWinVer < 0x210)
    {
        ErrMsg(IDS_ERR_OLDWINVER);
        return FALSE;
    }

#ifdef INTERNAL
    //AuxPrint("Windows Version %x",wWinVer);
#endif

    /** initialize error subsytem **/
    SetDefErr();

    /** Load Drive\path\executable name into global variable szExePath.
        Do before creating LIBRARY dialog.  **/
    if (NOT GetExePath())  return FALSE;   

    bNewUser = (GetProfileString(szWinIniLabel, PS_USERID,
                "", custinfo.userid, sizeof (custinfo.userid)) == 0);

   /* Load Accelerators */
   hAccel = LoadAccelerators(hInstanceWS, (LPSTR)szAppName);
	hAccelView = LoadAccelerators(hInstanceWS, (LPSTR)"WSView");

	lpprocSearchDlg     = MakeProcInstance((FARPROC)WSSearchDlgProc, hInstanceWS);
	lpprocStopSearchDlg = MakeProcInstance((FARPROC)WSStopSearchDlgProc, hInstanceWS);
	lpprocViewDlg       = MakeProcInstance((FARPROC)WSViewBoxDlgProc, hInstanceWS);
	lpprocHelpTextDlg   = MakeProcInstance((FARPROC)WSHelpTextDlgProc, hInstanceWS);
	lpprocAboutDlg      = MakeProcInstance((FARPROC)WSAboutDlgProc, hInstanceWS);

	if( (lpprocHelpTextDlg == NULL) || (lpprocViewDlg == NULL) ||
        (lpprocAboutDlg == NULL)  || (lpprocSearchDlg == NULL) )
    {
		ErrMsg(ES_LOWMEMMSG);
        return FALSE;
    }

    if (SuperClassEdits() != OK)
        return FALSE;

    /** do this before trying to read any database 
        files (5.28.90) D. Kent **/
    if ( !ReadCustInfo() )
        return FALSE;

    DialogBox(hInstanceWS, (LPSTR)"ABOUT", hWndWS, lpprocAboutDlg);
    /* Are we dealing with a monochrome display */
    hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
    bMonochrome = GetDeviceCaps(hDC, NUMCOLORS) <= 2;
    DeleteDC(hDC);

    lstrcpy(szLRU,szLRUFileName);
    AddFullPath(szLRU);

    /* cache time */
    if (access(szLRU,0))
    {  /* the file is missing */
       /* note: maybe because shopper isn't set up yet */
        if (CreateLRUFile()==FAIL)  /* start from scratch with a new file */            
           ErrMsg(IDS_FILE_IO_ERR,ID_ERR_MISSING_LRU,szLRUFileName);
        /* CreateLRU will set custinfo.cacheSize to a default value */
    }
    else if (ReadLRUFromDisk(&hLRU)==FAIL) 
        if (InqErr() == IDS_FILE_IO_ERR)
        {
            if (RemakeLRU() != OK)
                return FALSE;
        }
        else // out of memory
        {
            IssueErr();
            return FALSE;
        }

    dwCacheSize = GetCacheSize(custinfo.cacheSize) ;

    if (CreateLibWnd() != OK)
        return FALSE;

    // Used to invoke help screens from Modal D-Boxes
    lpfnKeyHook = MakeProcInstance((FARPROC)KeyboardHook, hInstance);
    lpfnOldHook = SetWindowsHook(WH_KEYBOARD, lpfnKeyHook);

    // Used to keep track of whose got the focus
    //lpfnMsgHook = MakeProcInstance((FARPROC)MessageHook, hInstance);
    //lpfnOldMsgHook = SetWindowsHook(WH_MSGFILTER, lpfnMsgHook);

    ShowWindow(hWndWS,nCmdShow);
    UpdateWindow(hWndWS);
    //if ( !bConnect )
        //GiveUpdateOption(hWndWS);

	return(TRUE);
}


// Used to trap F1-HELP
DWORD FAR PASCAL KeyboardHook(int  iCode,
                              WORD wParam,
                              LONG lParam)
{
   HWND hWnd;

   hWnd = GetActiveWindow();

   if(iCode == HC_ACTION && wParam == VK_F1)
   {
      PostMessage(hWnd, WM_COMMAND, ID_HELP, 0L);
      return TRUE;
   }
   else
   {
      return DefHookProc(iCode, wParam, lParam, &lpfnOldHook);
   }
}

#if 0
DWORD FAR PASCAL 
MessageHook(int  iCode, WORD wParam, LONG lParam)
{
   switch (iCode)
   {
        case MSGF_DIALOGBOX:
        case MSGF_MESSAGEBOX:
        {
            MSG *lpMsg = (MSG *)lParam;
            //if (lpMsg->message == WM_ACTIVATE)
            {
                hMommaWnd = lpMsg -> hwnd;
#ifdef INTERNAL
                //AuxPrint("MommaWnd: %x, msg: %x",hMommaWnd,lpMsg -> message);
#endif
            }
        }
        break;
    }
    return DefHookProc(iCode, wParam, lParam, &lpfnOldMsgHook);
}
#endif


/*************************************************************\
 ** WSTimerDlgProc
 **
 ** ARGUMENTS:   HWND    hDlg
 **              unsigned message
 **              WORD    wParam
 **              LONG    lParam
 **
 ** DESCRIPTION: Timer dialog handling procedure
 **
 ** RETURNS:     TRUE if successful, FALSE if not.
 **
 ** HISTORY:   Lauren Bricker  June 18, 1990 (written)
\*************************************************************/

#define WINBORDER    5

LONG FAR PASCAL
WSTimerDlgProc (HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    static  BOOL    bTimerOK = FALSE;
    static  char    szTemp[] = "00";
    static DWORD lOriginalTime = 0L;
    static BOOL  warning_sent = FALSE;

    REFERENCE_FORMAL_PARAM (wParam);
    REFERENCE_FORMAL_PARAM (lParam);

    switch ( message )
    {
        case WM_INITDIALOG:
        {
	        RECT	rWindow;
	        int     xScreen = 0;
	        int	    height, width;
            POINT   start;

            // Move the window to the upper right hand corner
	        GetWindowRect(hDlg, &rWindow);

	        height  = rWindow.bottom - rWindow.top;
	        width   = rWindow.right - rWindow.left;
		    xScreen = GetSystemMetrics(SM_CXSCREEN);
            start.x = xScreen - (width + WINBORDER);

	        MoveWindow(hDlg, start.x, WINBORDER, width, height, FALSE);
        
            /* Create a one second timer */
            if( !SetTimer (hDlg, 1, TIMER_INTERVAL, NULL) ) 
            {
                DestroyWindow (hDlg);
                hWndTimer = NULL;
                return TRUE;
            }
            else
            {
                bTimerOK = TRUE;
                hWndTimer = hDlg;
            }

	        lOriginalTime = lLastMsgTime = GetTickCount ();

            SetDlgItemText (hDlg, IDS_HOURS,   szTemp);
            SetDlgItemText (hDlg, IDS_MINUTES, szTemp);
            SetDlgItemText (hDlg, IDS_SECONDS, szTemp);
            break;
        }

        case WM_TIMER:
        {
            char    szSeconds[6];
            char    szMinutes[6];
            char    szHours[6];
	        long	lElapsedSeconds = 0L;
	        DWORD	lNewTime = 0L;
            WORD    nTmp;
            WORD    d_minutes;

            /* first confirm whether we're still connected */
            if (NOT (StillConnected()))
            {
#ifdef INTERNAL
                AuxPrint("Timer finds no connection");
#endif
                HangUp();
                IssueErr();
                break;
            }

			lNewTime = GetTickCount ();     // in milliseconds

            /*
             * First do connect time
             */
			lElapsedSeconds = (lNewTime - lOriginalTime) / 1000;
            nTmp = (int) (lElapsedSeconds / SEC_PER_HR);
			itoa(nTmp, szHours, 10);
			if ( nTmp < 10 ) {
				szTemp[1] = szHours[0];
				strcpy(szHours, szTemp);
			}
            SetDlgItemText (hDlg, IDS_HOURS, szHours);

            lElapsedSeconds %= SEC_PER_HR;
			nTmp = (int) (lElapsedSeconds / SEC_PER_MIN);

			itoa(nTmp, szMinutes, 10);
			if ( nTmp < 10 ) {
				szTemp[1]=szMinutes[0];
				strcpy(szMinutes, szTemp);
			}
            SetDlgItemText (hDlg, IDS_MINUTES, szMinutes);

			nTmp = (int) (lElapsedSeconds % SEC_PER_MIN);
			itoa(nTmp, szSeconds, 10);
			if (nTmp < 10){
				szTemp[1]=szSeconds[0];
				strcpy(szSeconds, szTemp);
			}
            SetDlgItemText (hDlg, IDS_SECONDS, szSeconds);

            /*
             * Then countdown timer
             */
			lElapsedSeconds = (TIMER_DISCONNECT*60) - ((lNewTime - lLastMsgTime) / 1000);
            if (lElapsedSeconds < 0)
            {
                HangUp();
                ErrMsg(IDS_MSG_DISCONNECTED);
                break;
            }
            nTmp = (int) (lElapsedSeconds / SEC_PER_HR);
			itoa(nTmp, szHours, 10);
			if ( nTmp < 10 ) {
				szTemp[1] = szHours[0];
				strcpy(szHours, szTemp);
			}
            SetDlgItemText (hDlg, IDS_DHOURS, szHours);

            lElapsedSeconds %= SEC_PER_HR;
			nTmp = (int) (lElapsedSeconds / SEC_PER_MIN);

			itoa(nTmp, szMinutes, 10);
            d_minutes = nTmp;
			if ( nTmp < 10 ) {
				szTemp[1]=szMinutes[0];
				strcpy(szMinutes, szTemp);
			}
            SetDlgItemText (hDlg, IDS_DMINUTES, szMinutes);

			nTmp = (int) (lElapsedSeconds % SEC_PER_MIN);
			itoa(nTmp, szSeconds, 10);
			if (nTmp < 10){
				szTemp[1]=szSeconds[0];
				strcpy(szSeconds, szTemp);
			}
            SetDlgItemText (hDlg, IDS_DSECONDS, szSeconds);

            // Send message to have WinMain put disconnect warning
            // (Do last because SendMessage may not return immediately.)
            if (d_minutes >= TIMER_WARNING) warning_sent = FALSE;
            else
            {
                if (NOT warning_sent) {
                    warning_sent = TRUE;
                    SendMessage(hWndWS,WM_MINUTE_WARNING,0,0L);
                }
            }
            
			break;
        }

        case WM_DESTROY:
            if ( bTimerOK )
                KillTimer(hDlg, 1);
            hWndTimer = NULL;
            return FALSE;
            break;

        default:
            return FALSE;
    }

    return TRUE;
}


