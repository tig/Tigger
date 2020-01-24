#include <windows.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <io.h>

#define WS_MAIN     /* to get globals declared */
#include "ws.h"
#include "error.h"
#include "file.h"
#include "misc.h"
#include "viewbox.h"
#include "settings.h"
#include "messages.h"  /* Status bar strings */
#include "help.h"
#include "order.h"
#include "database.h"
#include "comm.h"
#include "rc_symb.h"
#include "winundoc.h"
#include "custinfo.h"
#include "download.h"
#include "libmgr.h"
#include "info.h"
#include "ksearch.h"
#include "busy.h"

/* the official "version" of windows shopper */
WORD CurVersion = 38;   /* WS 1.2 */

LONG FAR PASCAL 
    ChildKeyProc (HWND hWnd, unsigned message, WORD wParam, LONG lParam);
void KeySubClass(int nID, BOOL bOnOff);
int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow);

/* Misc strings  & statics */
       char  szNull[]="";
static short nFocus;
static BOOL  bNSView, bNSOrderMAIN;
       char  szMasterFileName[_MAX_PATH];
       char  szExePath[_MAX_PATH];
       char  szDataPath[_MAX_PATH];
static char  szCurrListFile[_MAX_PATH];
static char  szCurrSetFile[_MAX_PATH];
static char  szCurrCategory[MAX_STR_LEN];
       char  szPublishers[]="Publishe&rs";
       char  szCategories[]="Catego&ries";
       char  szClipArt[]=" libr&aries";
static WORD  wLastLibCatModeSelection = LB_ERR, wLastLibPubModeSelection = LB_ERR;
static WORD  wLastPubSelection = LB_ERR, wLastCatSelection = LB_ERR;

#define CURCAT (szCurrCategory)

static void ClearListBox(HWND hWnd);
static int CreateChildren(HWND hWnd);


/** GLOBALS */
HANDLE  hInstanceWS;
HANDLE  hWndWS=NULL;
HANDLE  hWndStatusBar = NULL;
WORD    nCurViewMode = ID_LIB_PUBLISHER;
int     nMissingThumbContext = DEFAULT_MODE;
HANDLE  hAccel;
HANDLE  hAccelView=NULL;
FARPROC lpprocViewDlg;
FARPROC lpprocHelpTextDlg;
FARPROC lpprocAboutDlg, lpproc;
FARPROC lpprocSearchDlg;
FARPROC	lpprocStopSearchDlg;

HWND    hWndView = NULL;
LIBRARY CurLib;
BOOL    bUsingDC;
HCURSOR hPhoneCursArr[4];
HCURSOR hSearchCurs;

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSMainDlgProc                                          */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL 
WSLibraryDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    HWND    hWndRadio, hWndList;
    int     nIndex;

    switch (message)    
    {
        case WM_SETFOCUS:
            //BringWindowToTop(hWndDlg);
            SetFocus(hChildren[nFocus].hWnd);
        break;

        case WM_SYSCOMMAND:
            return DefWindowProc(hWndDlg,message,wParam,lParam);

#if 0
        case WM_ACTIVATEAPP:
            if (wParam != 0) /* WS is regaining the focus */
            {
            if (ReadLRUFromDisk(&hLRU)==FAIL) /* get the LRU back in memory */
                    IssueErr();
                break;    /* kick out of the switch */
            }
            else /* WS is losing focus to another app */
                wParam = SIZEICONIC; /* emulate WM_SIZE message and free LRU table */
                /* fall through */

        case WM_SIZE: /* are we going ICONIC? */
            if (hLRU != NULL) /* if LRU table exists */
            {
                if (wParam == SIZEICONIC) /* and we're going iconic */
                {
                    if (CloseLRUTable(hLRU) == FAIL) /* write the LRU */
                        IssueErr();
                    GlobalFree(hLRU) ;
                    hLRU = NULL;
                }
                else /* can't maximize shopper, BUT if restoring... */
                {
                   if (wParam == SIZENORMAL) /* get the LRU back */
                        if (ReadLRUFromDisk(&hLRU)==FAIL) /* get the LRU back in memory */
                            IssueErr();  /* kick out of the switch */
                }
            }
            break;  
#endif
        case WM_CREATE:

            hWndWS = hWndDlg; // needed by WSConnect()
            hMommaWnd=hWndWS;

            CenterPopUp(hWndDlg);

            if (CreateChildren(hWndDlg) != OK)
            {
                FatalExit(1);
                return FALSE;
            }

            hWndStatusBar=hChildren[ID_LIB_STATUSBAR].hWnd;

            /** Set up file system.  Connect if not possible. **/
            GetMasterFileName(szMasterFileName);
            nCurViewMode=ID_LIB_PUBLISHER;

            /** if basic high level files don't exist, then need
                to logon.  **/
            if (InitFileSystem() == OK)
            /* else error msg has been issued */
            {
                SendMessage(hChildren[nCurViewMode].hWnd, BM_SETCHECK, 1, 0L);    
                lstrcpy(szCurrListFile,szPublishersFileName);
                FillListBoxes(hWndDlg,TRUE);
            }

#if 0
            if ( wExeType != EXE_LIMITED )
#endif
            {
                HMENU hWSMenu = GetMenu(hWndWS);
                ChangeMenu (hWSMenu, IDM_UPGRADE_SUBSCRIPTION, "", 0, MF_DELETE);
            }

#ifdef NONSTANDARD
            /*------------------------------*/
            /* Add About to System Menu.    */ 
            /*------------------------------*/
            hSysMenu = GetSystemMenu(hWndDlg, FALSE);
            hMenu = GetMenu(hWndDlg);
            ChangeMenu(hSysMenu, 0, NULL, 0, MF_APPEND);
            ChangeMenu(hSysMenu, 0, "&About", IDM_ABOUT, MF_APPEND);
#endif

            //these are update flags UPDATE LIBRARY msgbox
            bNSView=bNSOrderMAIN=TRUE;
            break;


        case WM_SHOWWINDOW:
            if (wParam) {
                SetStatusBarText (szLibSelections);
            }
            return DefWindowProc(hWndDlg,message,wParam,lParam);

        case WM_MINUTE_WARNING:
            // sent from WSTimerDlgProc()
            SetDest (ES_ACTIVEWND, NOBEEP, MB_YESNO | MB_ICONQUESTION, IDS_TITLE_APPNAME);
            if (ErrMsg(IDS_MSG_MINUTE_WARNING) == IDYES)
            {
                if (bConnect)
                    Send_Message(MSG_SAY_HI);  // isues error msg
                else
                    ErrMsg(IDS_ERR_TOO_LATE);
            }
            else // IT'S TIME TO LOG OFF
            {
                if (bConnect)
                {
                    SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_APPNAME);
                    if (ErrMsg(IDS_MSG_QUERY_HANGUP) == IDYES)
                    {
                        HangUp();
                        SetWindowText(hChildren[ID_LIB_LOGON].hWnd,"&LogOn");
                    }
                }
            }
            break;

        /** simulate dialog accelerator keys **/
        case WM_MENUCHAR:
            switch(wParam)
            {
				case 'b':
				case 'B':
            		SetFocus(hChildren[ID_LIB_DESCRIPTION].hWnd);
                    return(MAKELONG(ID_LIB_DESCRIPTION,1));
					break;
				case 'y':
				case 'Y':
            		SetFocus(hChildren[nCurViewMode].hWnd);
                    return(MAKELONG(nCurViewMode,1));
					break;
				case 'r':
				case 'R':
            		SetFocus(hChildren[ID_LIB_CAT_PUB_LIST].hWnd);
                    return(MAKELONG(ID_LIB_CAT_PUB_LIST,1));
					break;
				case 'a':
				case 'A':
            		SetFocus(hChildren[ID_LIB_LIST].hWnd);
                    return(MAKELONG(ID_LIB_LIST,1));
					break;
                case 'v':
                case 'V':
                    SendMessage(hWndDlg,WM_COMMAND,ID_LIB_VIEW,0L);
                    return(MAKELONG(ID_LIB_VIEW,1));
                break;
                case 'o':
                case 'O':
                    SendMessage(hWndDlg,WM_COMMAND,ID_LIB_ORDER,0L);
                    return(MAKELONG(ID_LIB_ORDER,1));
                break;
                case 'l':
                case 'L':
                    SendMessage(hWndDlg,WM_COMMAND,ID_LIB_LOGON,0L);
                    return(MAKELONG(ID_LIB_LOGON,1));
                break;
                case 'n':
                case 'N':
                    SendMessage(hWndDlg,WM_COMMAND,ID_LIB_NEWUSER,0L);
                    return(MAKELONG(ID_LIB_NEWUSER,1));
                break;
                case 'h':
                case 'H':
                    SendMessage(hWndDlg,WM_COMMAND,ID_LIB_HELP,0L);
                    return(MAKELONG(ID_LIB_HELP,1));
                break;
                case 'c':
                case 'C':
                    SendMessage(hWndDlg,WM_COMMAND,ID_LIB_CATEGORY,0L);
                    return(MAKELONG(ID_LIB_CATEGORY,1));
                break;
                case 'p':
                case 'P':
                    SendMessage(hWndDlg,WM_COMMAND,ID_LIB_PUBLISHER,0L);
                    return(MAKELONG(ID_LIB_PUBLISHER,1));
                break;
                case 'e':
                case 'E':
                    SendMessage(hWndDlg,WM_COMMAND,ID_LIB_SEARCH,0L);
                    return(MAKELONG(ID_LIB_SEARCH,1));
                break;
                default:
                    return DefWindowProc(hWndDlg,message,wParam,lParam);
                break;
            }
            break;

        case WM_COMMAND:
        {
            static int nLastPubSel = 0;
            static int nLastLibSel = 0;

            switch(wParam)
            {
                case IDM_UPDATE_THUMB:
                    GetAltDrive();
                break;

                case IDM_PASSWORD:
                    // password validation FIRST
                    if (NOT bConnect)
                    {
                       ErrMsg(IDS_ERR_MUST_LOGON);
                       break;
                    }
                    lpproc=MakeProcInstance((FARPROC) WSChangePasswordDlgProc, hInstanceWS) ;
                    DialogBox(hInstanceWS,(LPSTR) "NEWUSER",hWndDlg,lpproc);
                    FreeProcInstance(lpproc);
                break;

                case  IDM_ABOUT:
                    DialogBox(hInstanceWS, (LPSTR)"ABOUT", hWndDlg, lpprocAboutDlg);
                    break;

                case IDM_BILLING_INFO:
                    if (NOT bConnect)
                    {
                       ErrMsg(IDS_ERR_MUST_LOGON);
                       break;
                    }
                    // else fall through...
                case IDM_COMM:
                case IDM_SHIPPING_INFO:
                case IDM_CUSTOMER_INFO:
                case IDM_LIB_MANAGEMENT:
                    GetInfo(hWndDlg,wParam);
                    break;

                case ID_HELP:
                    GetHelp(GH_TEXT,IDH_MAIN);
                    break;

                case IDM_UPGRADE_SUBSCRIPTION:
					     ErrMsg(IDS_MSG_USERID_ASSIGNED);
                    break;

                case ID_LIB_SEARCH:
                    StartSearch (hWndDlg);
                    break;

                case ID_LIB_VIEW:
                case ID_LIB_ORDER:

                   nIndex = QueryListSel(hChildren[ID_LIB_LIST].hWnd);
                   if(nIndex >= 0) 
                       switch(wParam)
                       {
                           case ID_LIB_VIEW:
                               /** note this gal is modeless **/
                                if (strcmp(CURLIB,"None Available"))
                                {
                                    bFixAfterFind = FALSE;
                                    /* viewbox will call SW_SHOW */
                                    CreateViewBox(0, 1);
                                }
                                else
                                    ErrMsg(IDS_ERR_LIB_NOTAVAIL);
                                break;
                           case ID_LIB_ORDER:
                               if (strcmp(CURLIB,"None Available"))
                               {
                                    FREEZE_HWND_WS
                                    CreateOrder();
                                    THAW_HWND_WS
                               }
                               else
                                   ErrMsg(IDS_ERR_LIB_NOTAVAIL);
                               break;
                       }     // end of internal switch(wParam)
                   else 
                   {
                       /* Nothing in list box to view or get info about */
                       //ErrMsg(IDS_ERR_LIB_NOT_SELECTED);
                   }
                   break;

                case IDM_WHATS_NEW:
                    ShowWhatsNew (hWndWS);
                    break;

#if 0
                case IDM_SET_PHONENUM:
                {
                    SetDest (hWndDlg, NOBEEP, MB_YESNOCANCEL| MB_ICONQUESTION, IDS_TITLE_APPNAME);
                    bCallRetail = (ErrMsg (IDS_MSG_SELECT_PHONENUM) == IDYES);
                    break;
                }
#endif

                /*-------------------------*/
                /*  SWITCHING VIEW MODES   */
                /*-------------------------*/
                case ID_LIB_CATEGORY:
                case ID_LIB_PUBLISHER:
                    hWndRadio = hChildren[nCurViewMode].hWnd;
                    SendMessage(hWndRadio, BM_SETCHECK, 0, 0L);

                    /* Turn on new radio button */
                    hWndRadio = hChildren[wParam].hWnd;
                    SendMessage(hWndRadio, BM_SETCHECK, 1, 0L);
                    SetFocus(hWndRadio);
                    if (nCurViewMode != wParam)
                    {
                        nCurViewMode=wParam;
                        //set defaults for new view mode
                        nLastPubSel = (nCurViewMode == ID_LIB_CATEGORY) ?
                                       wLastCatSelection : wLastPubSelection;

                        nLastLibSel = (nCurViewMode == ID_LIB_CATEGORY) ?
                                       wLastLibCatModeSelection : wLastLibPubModeSelection;

                        //save current lib and Pub/Cat selections for when we return to this mode
                        if (nCurViewMode == ID_LIB_CATEGORY) //this is the new mode, we're saving old selections
                        {
                           wLastLibPubModeSelection = QueryListSel(hChildren[ID_LIB_LIST].hWnd);
                           wLastPubSelection = QueryListSel(hChildren[ID_LIB_CAT_PUB_LIST].hWnd);
                        }
                        else
                        {
                           wLastLibCatModeSelection = QueryListSel(hChildren[ID_LIB_LIST].hWnd);
                           wLastCatSelection = QueryListSel(hChildren[ID_LIB_CAT_PUB_LIST].hWnd);
                        }

                        lstrcpy(szCurrListFile, nCurViewMode == ID_LIB_CATEGORY ? szCategoryFileName : szPublishersFileName);
                        FREEZE_HWND_WS
                        FillListBoxes(hWndDlg,TRUE);
                        THAW_HWND_WS
                    }
                    break;

               case ID_LIB_CAT_PUB_LIST:
                    switch(HIWORD(lParam)) 
                    {
                        case LBN_SELCHANGE:
                            hWndList = hChildren[ID_LIB_CAT_PUB_LIST].hWnd;
                            nIndex = QueryListSel(hWndList); /* gotta do this for side effect */
                            if (nIndex != nLastPubSel)
                            {
                                 nLastPubSel = nIndex;
                                 nLastLibSel = 0;
                                 if (nCurViewMode == ID_LIB_CATEGORY)
                                 {
                                    wLastCatSelection = nIndex;
                                    wLastLibCatModeSelection = 0;
                                 }
                                 else
                                 {
                                    wLastPubSelection = nIndex;
                                    wLastLibPubModeSelection = 0;
                                 }
                                 FREEZE_HWND_WS
                                 FillListBoxes(hWndDlg, FALSE);
                                 THAW_HWND_WS
                            }
                            break;

                        default:
                            break;
                    }
                    break;

               case ID_LIB_LIST:
                    switch(HIWORD(lParam)) 
                    {
                        case LBN_SELCHANGE:
                        {
                            HCURSOR hSaveCurs;

                            hWndList=hChildren[ID_LIB_LIST].hWnd;

                            nIndex = QueryListSel(hWndList);
                            if (nLastLibSel != nIndex)
                            {
	                             hSaveCurs = SetCursor(LoadCursor(NULL, IDC_WAIT));
                                SetStatusBarText (szInitializing);
                                 /* in case have to download, user can't
                                    confuse things by inputing during download */
                                FREEZE_HWND_WS
                                QueryLibFileName(nIndex,szCurrSetFile,CURLIBFILE);
                                //QueryLibName(nIndex,szCurrSetFile,CURLIB);
                                FillDesc(hWndDlg,nIndex);
                                THAW_HWND_WS
                                SetCursor(hSaveCurs);
                                SetStatusBarText (szLibSelections);
                                nLastLibSel = nIndex;
                                if (nCurViewMode == ID_LIB_CATEGORY)
                                   wLastLibCatModeSelection = nIndex;
                                else
                                   wLastLibPubModeSelection = nIndex;
                            }
                        }
                        break;

                        case LBN_DBLCLK:
                            SendMessage(hWndDlg,WM_COMMAND,ID_LIB_VIEW,0L);
                        break;

                        default:
                            return DefWindowProc(hWndDlg,message,wParam,lParam);
                    }
                    break;

                case ID_LIB_HELP:
                    GetHelp(GH_TEXT,IDH_MAIN);
                    break;

                case ID_LIB_NEWUSER:
#ifdef NO_LOGON
                    SetDest (hWndDlg, NOBEEP, MB_ICONEXCLAMATION | MB_OK, IDS_TITLE_APPNAME);
                    ErrMsg(IDS_MSG_NO_LOGON);
                    break;
#else
                    SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_APPNAME);
                    if ( ErrMsg (IDS_MSG_NEW_USER_START) == IDYES )
                    {
                        FREEZE_HWND_WS
                        if ( !ResetCustInfo (FALSE) )
                            ErrMsg (IDS_ERR_INITIALIZING);
                        else
                        {
                            if (GetInfo(hWndDlg,ALL))
                            {
                                HMENU hWSMenu;

                                if (WSConnect(FALSE) == OK) // then connected
                                {
                                    hWSMenu = GetMenu(hWndWS);
                                    NewUserConfig(hWSMenu,OFF);
                                    /* write userid, etc to win.ini */
                                    WriteCustInfo (WRITE_USERID);
                                    bNewUser = FALSE;
                                }
                            }
                        }
                        THAW_HWND_WS
                    }
                    break;
#endif

                case ID_LIB_LOGON:
#ifdef NO_LOGON
                    SetDest (hWndDlg, NOBEEP, MB_ICONEXCLAMATION | MB_OK, IDS_TITLE_APPNAME);
                    ErrMsg(IDS_MSG_NO_LOGON);
                    break;
#else
                    if (bConnect)
                    {
                        // IT'S TIME TO LOG OFF
                        SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_APPNAME);
                        if (ErrMsg(IDS_MSG_QUERY_HANGUP) == IDYES)
                        {
                            HangUp();
                            SetWindowText(hChildren[ID_LIB_LOGON].hWnd,"&LogOn");
                        }
                        break;
                    }
                    else
                    {
                        WSConnect(TRUE);
                    }
                    SetFocus(hWndWS);
                    break;
#endif

                default:
                    return DefWindowProc(hWndDlg,message,wParam,lParam);
            }  /* end switch */
            break;
        }  //end case WM_COMMAND


        case WM_DESTROY:
        case WM_ENDSESSION:
            if (bConnect)
                HangUp();

	        FreeProcInstance(lpprocViewDlg);
	        FreeProcInstance(lpprocHelpTextDlg);
	        FreeProcInstance(lpprocAboutDlg);
	        FreeProcInstance(lpprocSearchDlg);
	        FreeProcInstance(lpprocStopSearchDlg);
            SetWindowsHook(WH_KEYBOARD, lpfnOldHook);
            FreeProcInstance(lpfnKeyHook);
            if (hLRU != NULL)     
            {
                if (CloseLRUTable(hLRU) == FAIL)
                    IssueErr();
                GlobalFree(hLRU) ;
            }
            CloseFileCheckSystem( AND_DISABLE );
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWndDlg,message,wParam,lParam);
    }
    return (0L);
}   


static void
ClearListBox(HWND hWnd)
{
    SendMessage(hWnd, WM_SETREDRAW, 0, 0L);
    SendMessage(hWnd, LB_RESETCONTENT, 0, 0L);
    SendMessage(hWnd, WM_SETREDRAW, 1, 0L);
    InvalidateRect(hWnd, (LPRECT)NULL, TRUE);
    UpdateWindow(hWnd);
    SetWindowText (hChildren[ID_LIB_DESCRIPTION].hWnd, (LPSTR)"");
    SetWindowText (hChildren[ID_LIB_DESC_TITLE].hWnd, (LPSTR)"");
}


/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  FillListBoxes                                          */
/*                                                                        */
/*   Description:  Based on the current ViewMode (either cateogory or     */
/*                 publisher), this function fills the upper list box     */
/*                 with the contents of the appropriate .lst file and     */
/*                 then calls the function that fills the bottom list     */
/*                 box.                                                   */
/*                                                                        */
/*                 created  11/20/89  lori                                */
/*                 rewritten           dug                                */
/*                                                                        */
/*------------------------------------------------------------------------*/
BOOL FAR PASCAL 
FillListBoxes (HWND hWndDlg, BOOL bNewCategory) 
{
    HWND    hWndList, hWndLib;
    WORD    nSelect,count;
    int nRetval=OK;
    HCURSOR hSaveCurs;

    SetStatusBarText (szInitializing);

	hSaveCurs = SetCursor(LoadCursor(NULL, IDC_WAIT));

    hWndList = hChildren[ID_LIB_CAT_PUB_LIST].hWnd;
    hWndLib =  hChildren[ID_LIB_LIST].hWnd;

    if (bNewCategory)
    /* then gotta fill box and set selection */
    {
        int nMin, nMax;

        /* clear redraw flag */
        SendMessage(hWndList, WM_SETREDRAW, 0, 0L);

        SendMessage(hWndList, LB_RESETCONTENT, 0, 0L);

        count = EnumListFile(
            (nCurViewMode == ID_LIB_CATEGORY) ? AddStringToListBox : AddIndirectionToListBox,
                hWndList, szCurrListFile);

        if (count == 0)
        {
            /* missing list file? */
            ClearListBox(hWndList);
            ClearListBox(hWndLib);
            END (FAIL)
        }

        nSelect = (nCurViewMode == ID_LIB_CATEGORY) ?
                   wLastCatSelection : wLastPubSelection;

        if (nSelect == LB_ERR) /* no previously selected item in the group */
            nSelect = 0;

        SendMessage(hWndList, LB_SETCURSEL, nSelect, 0L);

        GetScrollRange(hWndList,SB_VERT,(LPINT)&nMin,(LPINT)&nMax);
        if (count > 0)
        {
            nMax = (nMax - nMin + 1) * nSelect / count;
            SetScrollPos(hWndList,SB_VERT,nMax,TRUE);
        }

        /*---------------------------*/
        /* Turn redraw on and update */    
        /*---------------------------*/
        SendMessage(hWndList, WM_SETREDRAW, 1, 0L);
        InvalidateRect(hWndList, (LPRECT)NULL, TRUE);
        UpdateWindow(hWndList);


        /*-------------*/
        /* Set Title   */
        /*-------------*/
        SetWindowText(hChildren[ID_LIB_CAT_PUB_TITLE].hWnd,
            (nCurViewMode==ID_LIB_CATEGORY) ? (LPSTR)szCategories: (LPSTR)szPublishers);
    }
    else
        nSelect = (WORD) SendMessage(hWndList, LB_GETCURSEL, 0, 0L);


    SendMessage(hWndList,LB_GETTEXT,nSelect,
        (LONG)(LPSTR)((nCurViewMode==ID_LIB_CATEGORY) ? CURCAT:CURPUB));

    end:

    /*----------------------------------------------------------------------------*/
    /* We just switched our display mode so certainly must refresh secondary list */
    /*----------------------------------------------------------------------------*/
    if (nRetval == OK)
        nRetval =  FillLibList(hWndDlg);

    SetCursor(hSaveCurs);

    //THAW_HWND_WS

    SetStatusBarText (szLibSelections);
    return nRetval;
}



/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  FillLibList                                      */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                11/20/89  lori                                                */
/*                    Initially created.                                  */
/*                                                                        */
/*------------------------------------------------------------------------*/
BOOL NEAR PASCAL 
FillLibList (HWND hWndDlg)  
{
    HWND    hWndLB1,  /* CAT/PUB List */
            hWndLB2;  /* LIB  List */
    char    szSetFile[_MAX_PATH];
    WORD    nSelect, nSelectLib;
    static int  nLastPubIndex = -1;
    char buf[MAX_STR_LEN];

    /* Update Title Text */
    lstrcpy (buf,nCurViewMode==ID_LIB_CATEGORY?CURCAT:CURPUB);
    lstrcat (buf, szClipArt);
    SetWindowText(hChildren[ID_LIB_LIST_TITLE].hWnd, (LPSTR)buf);

    hWndLB1=hChildren[ID_LIB_CAT_PUB_LIST].hWnd;
    hWndLB2=hChildren[ID_LIB_LIST].hWnd;

    /*-----------------------------------------------------*/
    /* Fill the library list box                           */
    /*-----------------------------------------------------*/
    /* clear the redraw flag */
    SendMessage(hWndLB2, WM_SETREDRAW, 0, 0L);
    SendMessage(hWndLB2, LB_RESETCONTENT, 0, 0L);

    /* get index of currently-selected cat or pub */
    nSelect = (WORD)SendMessage(hWndLB1, LB_GETCURSEL, 0, 0L);

    /* get name of corresponding set file */
    if (QuerySetFileName(nSelect,szCurrListFile,szSetFile) == FAIL) 
    {
        ClearListBox(hWndLB2);
        return FAIL;
    }

    if (EnumSetFile(AddIndirectionToLibListBox, hWndLB2, szSetFile) == 0)
    {
        ClearListBox(hWndLB2);
        return FAIL;
    }

    /* Turn the redraw on and update */
    SendMessage(hWndLB2, WM_SETREDRAW, 1, 0L);
    InvalidateRect(hWndLB2, (LPRECT)NULL, TRUE);
    UpdateWindow(hWndLB2);

    /*----------------------------*/
    /* Save name of set file     */
    /*----------------------------*/
    lstrcpy(szCurrSetFile,szSetFile);
        
    /*-----------------------------------*/
    /* Determine which lib to highlight  */
    /*   if the pub or cat is different  */
    /*   than previous then select the   */
    /*   first entry                     */
    /*-----------------------------------*/
    nSelectLib = (nCurViewMode == ID_LIB_CATEGORY) ?
                 wLastLibCatModeSelection : wLastLibPubModeSelection;

    if (nSelectLib == LB_ERR) /* no previously selected item in the group */
        nSelectLib = 0;

    SendMessage(hWndLB2, LB_SETCURSEL, nSelectLib, 0L);
    if (QueryLibFileName(nSelectLib,szCurrSetFile,CURLIBFILE) != FAIL)
        return FillDesc(hWndDlg,nSelectLib);
    else
    {
        SetWindowText (hChildren[ID_LIB_DESCRIPTION].hWnd, (LPSTR)"");
        return FAIL;
    }

}

static int FAR PASCAL
FillDesc(HWND hWnd, WORD whichlib)
/** whichlib is which library is highlighted.  Note this has the sideeffect
    of setting CURPUB. */
{
    REFERENCE_FORMAL_PARAM(hWnd);

    if (whichlib == -1)
    {
        SetWindowText (hChildren[ID_LIB_DESCRIPTION].hWnd, (LPSTR)"");
        SetWindowText (hChildren[ID_LIB_DESC_TITLE].hWnd, (LPSTR)"Description:");
    }
    else if (QueryLibInfo(CURLIBFILE,
                          CURLIB, 
                          CURPUBFILE, 
                          CURPUB, 
                          &CURLIBDLDABLE,
                          hChildren[ID_LIB_DESCRIPTION].hWnd,
                          NULL,
                          CURLIBNUMIMAGES) != OK) 
    {
        //ErrMsg(IDS_ERR_LIB_NOTAVAIL);
        return FAIL;
    }
            
    else if (nCurViewMode == ID_LIB_CATEGORY)
    {
        SetWindowText (hChildren[ID_LIB_PUB_NAME].hWnd, (LPSTR)CURPUB);
        ShowWindow(hChildren[ID_LIB_PUB_TITLE].hWnd,SW_SHOW);
        ShowWindow(hChildren[ID_LIB_PUB_NAME].hWnd,SW_SHOW);
    }
    else
        //SetWindowText (hChildren[ID_LIB_DESC_TITLE].hWnd, (LPSTR)"Description:");
    {
        ShowWindow(hChildren[ID_LIB_PUB_TITLE].hWnd,SW_HIDE);
        ShowWindow(hChildren[ID_LIB_PUB_NAME].hWnd,SW_HIDE);
    }

    return OK;
}



/*------------------------------------------------------------------------*/
/*                                                                        */
/*     Function:  AddStringToListBox
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
BOOL FAR PASCAL 
AddStringToListBox (char *sz, HWND hWndListBox)
{
    SendMessage(hWndListBox, LB_ADDSTRING, 0, (LONG)(LPSTR)sz);
    return (TRUE);
}

BOOL FAR PASCAL 
AddIndirectionToListBox (char *szFileName, HWND hWndListBox)
{
    int fp = -1;
    char szName[MAX_STR_LEN];
    int nRetval=TRUE;

    AddFullPath(szFileName);

    if (bNULLFILE(szFileName))
    {
        SET_NA(szName);
        SendMessage(hWndListBox, LB_ADDSTRING, 0, (LONG)(LPSTR)szName);
        return TRUE;
    }

    if (LoadFile(szFileName) != OK)
        return FALSE;

    /* open it */
    if ((fp = _lopen(szFileName,READ)) == -1)
        return FALSE;

    /* read name (note skipping header) */
    //if (ReadToChar(fp,'\n') == FAIL)
        //goto end;
    if (ReadStringToChar(fp, szName, '\t', sizeof(szName)) == FAIL)
        END(FALSE)

    SendMessage(hWndListBox, LB_ADDSTRING, 0, (LONG)(LPSTR)szName);

    end:
    if (fp != -1)
        close(fp);

    return (TRUE);
}

BOOL FAR PASCAL 
AddIndirectionToLibListBox (char *szFileName, HWND hWndListBox)
{
    int fp = -1;
    char szName[MAX_STR_LEN];
    char szTemp[MAX_STR_LEN];
    BOOL bDldable;
    int nRetval=TRUE;

    AddFullPath(szFileName);

    if (bNULLFILE(szFileName))
    {
        SET_NA(szName);
        SendMessage(hWndListBox, LB_ADDSTRING, 0, (LONG)(LPSTR)szName);
        return TRUE;
    }

    if (LoadFile(szFileName) != OK)
        return FALSE;

    /* open it */
    if ((fp = _lopen(szFileName,READ)) == -1)
        return FALSE;

    /* read name (note skipping header) */
    //if (ReadToChar(fp,'\n') == FAIL)
        //goto end;
    if (ReadStringToChar(fp, szName, '\t', sizeof(szName)) == FAIL)
        END(FALSE)

    if (QueryLibInfo(szFileName, szName, NULL, NULL, 
                 &bDldable, NULL, NULL, NULL) != OK)
        END(FALSE)

    if(bDldable)
    {
      strcpy(szTemp, "* ");
      strcat(szTemp, szName);
    }
    else
    {
      strcpy(szTemp, "  ");
      strcat(szTemp, szName);
    }
    strcpy(szName, szTemp);

    SendMessage(hWndListBox, LB_ADDSTRING, 0, (LONG)(LPSTR)szName);

    end:
    if (fp != -1)
        close(fp);

    return (TRUE);
}

CHILDREC hChildren[NUM_LIB_CHILDREN];

static int
CreateChildren(HWND hWnd)
{
    RECT ClientRect;
    int width, height, tmpheight;
    float height_factor, width_factor;

    GetClientRect(hWnd,&ClientRect);
    width = ClientRect.right - ClientRect.left + 1;
    height = ClientRect.bottom - ClientRect.top + 1;

    height_factor = (float)(height / 386.0); // 386 is vga height
    width_factor = (float)(width / 459.0); // 460 is vga width

/* macros to compute positions & dimensions relative to vga values */
#define HEIGHT_REL(y) ((int)((y) * height_factor + 0.5))
#define WIDTH_REL(x) ((int)((x) * width_factor + 0.5))

#ifdef INTERNAL
    //AuxPrint("LibWnd: %d x %d",width,height);
#endif

    if ((hChildren[ID_LIB_LOGON].hWnd = CreateWindow("button","&Logon", 
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_GROUP | WS_TABSTOP,
        WIDTH_REL(332), HEIGHT_REL(110), WIDTH_REL(75), HEIGHT_REL(28) ,
        hWnd, ID_LIB_LOGON, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_LOGON,TRUE);

    if ((hChildren[ID_LIB_NEWUSER].hWnd = CreateWindow("button","&New User", 
        WS_CHILD | WS_DISABLED | BS_PUSHBUTTON | WS_GROUP | WS_TABSTOP,
        WIDTH_REL(332), HEIGHT_REL(110), WIDTH_REL(75), HEIGHT_REL(28),
        hWnd, ID_LIB_NEWUSER, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_NEWUSER,TRUE);

    if ((hChildren[ID_LIB_SEARCH].hWnd = CreateWindow("button","S&earch", 
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        WIDTH_REL(332), HEIGHT_REL(140), WIDTH_REL(75), HEIGHT_REL(28),
        hWnd, ID_LIB_SEARCH, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_SEARCH,TRUE);

    if ((hChildren[ID_LIB_VIEW].hWnd = CreateWindow("button","&View", 
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
        WIDTH_REL(332), HEIGHT_REL(170), WIDTH_REL(75), HEIGHT_REL(28),
        hWnd, ID_LIB_VIEW, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_VIEW,TRUE);

    if ((hChildren[ID_LIB_ORDER].hWnd = CreateWindow("button","&Order", 
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        WIDTH_REL(332), HEIGHT_REL(200), WIDTH_REL(75), HEIGHT_REL(28),
        hWnd, ID_LIB_ORDER, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_ORDER,TRUE);

    if ((hChildren[ID_LIB_HELP].hWnd = CreateWindow("button","F1=&Help", 
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        WIDTH_REL(332), HEIGHT_REL(230), WIDTH_REL(75), HEIGHT_REL(28),
        hWnd, ID_LIB_HELP, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_HELP,TRUE);

    if (CreateWindow("button","Select B&y", 
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_GROUP,
        WIDTH_REL(315), HEIGHT_REL(18), WIDTH_REL(107), HEIGHT_REL(85),
        hWnd, -1, hInstanceWS, NULL) == NULL)
        return FAIL;

    if ((hChildren[ID_LIB_CATEGORY].hWnd = CreateWindow("button","&Category", 
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP ,
        WIDTH_REL(324), HEIGHT_REL(44), WIDTH_REL(85), HEIGHT_REL(18),
        hWnd, ID_LIB_CATEGORY, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_CATEGORY,TRUE);

    if ((hChildren[ID_LIB_PUBLISHER].hWnd = CreateWindow("button","&Publisher", 
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        WIDTH_REL(324), HEIGHT_REL(72), WIDTH_REL(90), HEIGHT_REL(18),
        hWnd, ID_LIB_PUBLISHER, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_PUBLISHER,TRUE);

    if ((hChildren[ID_LIB_CAT_PUB_TITLE].hWnd = CreateWindow("static","", 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        WIDTH_REL(12), HEIGHT_REL(7), WIDTH_REL(180), HEIGHT_REL(18),
        hWnd, ID_LIB_CAT_PUB_TITLE, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_CAT_PUB_TITLE,FALSE);

    if ((hChildren[ID_LIB_CAT_PUB_LIST].hWnd = CreateWindow("listbox","", 
        WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER | WS_VSCROLL | WS_TABSTOP | WS_GROUP,
        WIDTH_REL(12), HEIGHT_REL(27), WIDTH_REL(252), HEIGHT_REL(80),
        //WIDTH_REL(12), HEIGHT_REL(35), WIDTH_REL(252), HEIGHT_REL(48),
        //WIDTH_REL(12), HEIGHT_REL(35), WIDTH_REL(252), HEIGHT_REL(51),
        hWnd, ID_LIB_CAT_PUB_LIST, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_CAT_PUB_LIST,TRUE);

#if 0
    GetWindowRect(hChildren[ID_LIB_CAT_PUB_LIST].hWnd,&ClientRect);
#ifdef INTERNAL
    AuxPrint("Cat Pub Wnd: %d x %d",ClientRect.right - ClientRect.left + 1,
                ClientRect.bottom - ClientRect.top + 1);
#endif
    tmpheight = ClientRect.bottom - ClientRect.top + 1;
#endif

    if ((hChildren[ID_LIB_LIST_TITLE].hWnd = CreateWindow("static","", 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        WIDTH_REL(12), HEIGHT_REL(116), WIDTH_REL(290), HEIGHT_REL(18),
        //WIDTH_REL(12), HEIGHT_REL(86), WIDTH_REL(290), HEIGHT_REL(18),
        //WIDTH_REL(12), tmpheight + HEIGHT_REL(20), WIDTH_REL(290), HEIGHT_REL(18),
        hWnd, ID_LIB_LIST_TITLE, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_LIST_TITLE,FALSE);

    if ((hChildren[ID_LIB_LIST].hWnd = CreateWindow("listbox","", 
        WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER | WS_VSCROLL | WS_GROUP | WS_TABSTOP,
        WIDTH_REL(12), HEIGHT_REL(137), WIDTH_REL(252), HEIGHT_REL(85),
        //WIDTH_REL(12), HEIGHT_REL(107), WIDTH_REL(252), HEIGHT_REL(118),
        //WIDTH_REL(12), tmpheight + HEIGHT_REL(40), WIDTH_REL(252), HEIGHT_REL(115),
        hWnd, ID_LIB_LIST, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_LIST,TRUE);

    if ((hChildren[ID_LIB_ASTERISK_DESC].hWnd = CreateWindow("static",
        "* Indicates Library is Downloadable",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        WIDTH_REL(14), HEIGHT_REL(223), WIDTH_REL(310), HEIGHT_REL(18),
        hWnd, ID_LIB_ASTERISK_DESC, hInstanceWS, NULL)) == NULL)
        return FAIL;

    if ((hChildren[ID_LIB_PUB_TITLE].hWnd = CreateWindow("static","Publisher:", 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        WIDTH_REL(12), HEIGHT_REL(241), WIDTH_REL(80), HEIGHT_REL(18),
        hWnd, ID_LIB_PUB_TITLE, hInstanceWS, NULL)) == NULL)
        return FAIL;

    if ((hChildren[ID_LIB_PUB_NAME].hWnd = CreateWindow("static","",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        WIDTH_REL(104), HEIGHT_REL(241), WIDTH_REL(225), HEIGHT_REL(18),
        hWnd, ID_LIB_PUB_NAME, hInstanceWS, NULL)) == NULL)
        return FAIL;

#if 0
    GetWindowRect(hChildren[ID_LIB_LIST].hWnd,&ClientRect);
#ifdef INTERNAL
    AuxPrint("Lib Wnd: %d x %d",ClientRect.right - ClientRect.left + 1,
                ClientRect.bottom - ClientRect.top + 1);
#endif
#endif
    //tweaked dimensions so border aligns with main window border - beng
    if ((hChildren[ID_LIB_STATUSBAR].hWnd = CreateWindow("static","",
        WS_CHILD | WS_VISIBLE | SS_LEFT | WS_BORDER,
        -1, height-HEIGHT_REL(18), width+1, HEIGHT_REL(18),
        hWnd, ID_LIB_STATUSBAR, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_STATUSBAR,FALSE);

    if ((hChildren[ID_LIB_DESC_TITLE].hWnd = CreateWindow("static","Li&brary description:", 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        WIDTH_REL(12), HEIGHT_REL(268), WIDTH_REL(165), HEIGHT_REL(18),
        //WIDTH_REL(12), HEIGHT_REL(252), WIDTH_REL(165), HEIGHT_REL(18),
        hWnd, ID_LIB_DESC_TITLE, hInstanceWS, NULL)) == NULL)
        return FAIL;

    //08-Aug-1991 removed tabstop to libdesc - beng
    if ((hChildren[ID_LIB_DESCRIPTION].hWnd = CreateWindow("SuperEdit","", 
        WS_GROUP | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER | WS_VSCROLL ,
        WIDTH_REL(12), HEIGHT_REL(288), WIDTH_REL(400), HEIGHT_REL(72),
        //WIDTH_REL(12), HEIGHT_REL(272), WIDTH_REL(400), HEIGHT_REL(85),
        hWnd, ID_LIB_DESCRIPTION, hInstanceWS, NULL)) == NULL)
        return FAIL;

    KeySubClass(ID_LIB_DESCRIPTION,TRUE);

    return OK;
}

LONG FAR PASCAL 
ChildKeyProc (HWND hWnd, unsigned message, WORD wParam, LONG lParam)
{
    short nID = GetWindowWord(hWnd,GWW_ID);

    switch(message)
    {
        case WM_KEYDOWN:
        switch (wParam)
        {
            case VK_TAB:
            {
                short nIndex=nID;
                LONG lStyle;

                if (GetKeyState(VK_SHIFT) & 0x8000) // SHIFT
                    do
                    {
                        nIndex = (nIndex == 0) ? (NUM_LIB_CHILDREN-1) : (nIndex - 1);
                        if (NOT IsWindowEnabled(hChildren[nIndex].hWnd))
						 {
						 	lStyle = 0;
                           continue;
						 }
                        lStyle = GetWindowLong(hChildren[nIndex].hWnd,GWL_STYLE);
                    }
                    while (NOT (lStyle & WS_TABSTOP));
                else
                    do
                    {
                        nIndex = (nIndex == (NUM_LIB_CHILDREN-1)) ? 0 : (nIndex + 1);
                        if (NOT IsWindowEnabled(hChildren[nIndex].hWnd))
						 {
						 	lStyle = 0;
                            continue;
						 }
                        lStyle = GetWindowLong(hChildren[nIndex].hWnd,GWL_STYLE);
                    }
                    while (NOT (lStyle & WS_TABSTOP));

                SetFocus(hChildren[nIndex].hWnd);
            }
            break;

            case VK_DOWN:
            {
                short nIndex=nID;
                LONG lStyle;

                do
                {
                    nIndex = (nIndex == (NUM_LIB_CHILDREN-1)) ? 0 : (nIndex + 1);
                }
                while (NOT IsWindowEnabled(hChildren[nIndex].hWnd));

                lStyle = GetWindowLong(hChildren[nIndex].hWnd,GWL_STYLE);
                if (NOT (lStyle & WS_GROUP))
                    SetFocus(hChildren[nIndex].hWnd);
            }
            break;

            case VK_UP:
            {
                short nIndex=nID;
                LONG lStyle;

                do
                {
                    nIndex = (nIndex == 0) ? (NUM_LIB_CHILDREN-1) : (nIndex - 1);
                }
                while (NOT IsWindowEnabled(hChildren[nIndex].hWnd));

                lStyle = GetWindowLong(hChildren[nID].hWnd,GWL_STYLE);
                if (NOT (lStyle & WS_GROUP))
                    SetFocus(hChildren[nIndex].hWnd);
            }
            break;

            default:
                return(CallWindowProc(hChildren[nID].lpfn, hWnd, message, wParam,lParam));
        }
        break;

        case WM_SETFOCUS:
            nFocus = GetWindowWord(hWnd,GWW_ID);
            hMommaWnd=hWndWS;
        break;

        default:
            return(CallWindowProc(hChildren[nID].lpfn, hWnd, message, wParam,lParam));
    }

    return(CallWindowProc(hChildren[nID].lpfn, hWnd, message, wParam,lParam));
}

void
KeySubClass(int nID, BOOL bOnOff)
{

    if (bOnOff)
    {
        FARPROC lpKeyProc = MakeProcInstance((FARPROC)ChildKeyProc,hInstanceWS);
        hChildren[nID].lpfn = 
            (FARPROC) GetWindowLong(hChildren[nID].hWnd,GWL_WNDPROC);
        SetWindowLong(hChildren[nID].hWnd, GWL_WNDPROC, (LONG) lpKeyProc);
    }
    else
        hChildren[nID].lpfn = NULL;
}


#if 0
BOOL GiveUpdateOption (HWND hDlg)
{

    SetDest (hDlg, NOBEEP, MB_ICONASTERISK | MB_OK, IDS_TITLE_APPNAME);
    if ( ErrMsg (IDS_MSG_REMEMBER) == IDOK )
	    return FALSE;

    return TRUE;

}
#endif




