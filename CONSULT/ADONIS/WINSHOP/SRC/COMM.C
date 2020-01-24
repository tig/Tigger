 /*************************************************************\
 ** FILE:  comm.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **  BBS communications routines.
 **
 ** HISTORY:   
 **  Raleigh Bellas, initially created 12-89.
 **  Revised, Doug Kent and Raleigh Bellas May 7, 1990.
 **
\*************************************************************/

#include "nowin.h"
#include <windows.h>
#include <dos.h>
#include <io.h>
#include "ws.h"
#include "string.h" // for strchr()
#include "error.h"
#include "rc_symb.h"
#include "misc.h"
#include "messages.h"
#include "winundoc.h"
#include "file.h"
#include "custinfo.h"
#include "comm.h"
#include "order.h"
#include "database.h"
#include "download.h"
#include "settings.h"
#include "info.h"
#include "busy.h"
#include "help.h"
#include "billing.h"
#include "subscr.h"

#if 1
#define LOCALNUM2400_RETAIL "4510083"
#define LOCALNUM9600_RETAIL "4510303"
#else
#define LOCALNUM2400_RETAIL "4517461"
#define LOCALNUM9600_RETAIL "4517461"
#endif

#define LOCALNUM2400_DEV "4517461"
#define LOCALNUM9600_DEV "4517461"

#if defined(BETA)

#define LONGNUM9600  "12064510303"
#define LONGNUMBER   "18008780108"
#define LOCALAREA    "206"

/***********************************************/
/* THIS ARRAY MUST BE IN INCREASING ORDER      */
/* since A Binary search will be done on it.   */
/***********************************************/

static  int  localPrefixes[] = {223, 224, 226, 228, 232, 234, 235, 236,
                                237, 241, 242, 243, 244, 246, 248, 251, 
                                255, 271, 277, 281, 282, 284, 285, 286, 
                                292, 296, 322, 323, 324, 325, 326, 328, 
                                329, 340, 343, 344, 345, 346, 358, 361, 
                                362, 363, 364, 365, 367, 368, 382, 386, 
                                391, 392, 421, 431, 433, 441, 442, 443, 
                                447, 448, 451, 453, 455, 462, 461, 464, 
                                467, 522, 523, 524, 525, 526, 527, 543, 
                                544, 545, 547, 548, 562, 575, 583, 587, 
                                621, 622, 623, 624, 625, 626, 628, 632, 
                                633, 634, 637, 641, 643, 644, 646, 655, 
                                656, 682, 684, 721, 722, 723, 725, 727, 
                                728, 746, 747, 762, 763, 764, 767, 768, 
                                772, 781, 782, 783, 784, 789, 820, 821, 
                                823, 822, 827, 828, 865, 867, 868, 881, 
                                882, 883, 885, 932, 935, 937, 938, 982};

BOOL PrefixIsLocal (int prefix);

#endif

static BOOL GetPhoneNumber (char *buffer, int size);
static int
QueryNewUserStatus(ORDER *Order);

BOOL    bConnect = FALSE;	
BOOL    bUpgradingUser = FALSE;
BOOL    bShipRenewal = FALSE;
BOOL    bRenewingUser;

/*************************************************************\
 ** WSConnect
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **  Issues error message.
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  May 14, 1990 (written)
\*************************************************************/
int FAR PASCAL 
WSConnect(BOOL bVerify)
{
    FARPROC lpproc;
	 HCURSOR	hCursor = NULL;
    int     nRetval = OK;
    BOOL    bHaveAUserId = custinfo.userid[0];
    BOOL    bActiveNewUser = FALSE;
    BOOL    bActiveExpiredNewUser = FALSE;
    int     nSubType;

    if (bConnect)
        return TRUE;

#ifdef NO_LOGON
    SetDest (hWndDlg, NOBEEP, MB_ICONEXCLAMATION | MB_OK, IDS_TITLE_APPNAME);
    ErrMsg(IDS_MSG_NO_LOGON);
    return IDCANCEL;
#endif

	if (bVerify)
   {
	    switch (VerifyLogon(custinfo.customer.name))
        {
            case IDNO:
            case IDCANCEL:
                return IDCANCEL;
                break;
        }
    }

    if (NOT bNewUser)
    {
        FARPROC lpproc;
        // password validation FIRST
        lpproc=MakeProcInstance((FARPROC) WSVerifyPasswdDlgProc, hInstanceWS) ;
        nRetval = DialogBox(hInstanceWS,(LPSTR) "NEWUSER",hWndWS,lpproc);
        FreeProcInstance(lpproc);
        switch(nRetval)
        {
            case OK:
                break;
            case FAIL:
                END(IDCANCEL) /* cruise out of WSConnect without logging on */
            default:
                END(FAIL)
        }
    }

    SetStatusBarText (szConnecting);
    BusyLoadNewText(NULL, IDS_MSG_CONNECTING, IDS_BUSY_CONNECT_ICON);

    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

    Connect();

    if (NOT bConnect)
    {
        IssueErr();
        END(FAIL)
    }

    /* Create the timer window */
    lpprocTimerDlg = MakeProcInstance ((FARPROC)WSTimerDlgProc, hInstanceWS);
    CreateDialog (hInstanceWS, (LPSTR)"TIMER", /*hWndWS*/NULL, lpprocTimerDlg);
    if ( NOT hWndTimer )
        ErrMsg(IDS_ERR_TOO_MANY_TIMERS);

    if ( hWndTimer )
    {
        ShowWindow (hWndTimer, SW_SHOW);
        SetFocus (IsWindow(hBusyBox) ? hBusyBox : hWndWS);
    }

	 bRenewingUser = FALSE;

    /** send exetype **/
    if (Send_Message(MSG_SEND_EXETYPE, wExeType) != OK)
    {
        ErrMsg(IDS_ERR_PLEASE_WAIT);
        END(FAIL)
    }

    /** send windows version **/
    if (Send_Message(MSG_SEND_WINVER, wWinVer) != OK)
    {
        ErrMsg(IDS_ERR_UNABLE_TO_CONNECT);
        END(FAIL)
    }

    /** see whether executable is up-to-date **/
    if (NOT LatestExe())
    {
        int nResult;
        SetDest (hWndWS, NOBEEP, MB_ICONEXCLAMATION | MB_OKCANCEL, IDS_TITLE_APPNAME);
        nResult = ErrMsg(IDS_MSG_UPDATE_SHOPPER_LONG);
        if (nResult == IDCANCEL)
        {
            /* tell em how to get the update by mail  */
            SetDest (ES_ACTIVEWND, NOBEEP, MB_OK | MB_ICONASTERISK, IDS_TITLE_APPNAME);
            ErrMsg(IDS_MSG_UPDATE_BY_MAIL);
            END(FAIL)
        }

        SetStatusBarText (szUpgrading);

        BusyClear(FALSE);  //remove INITIALIZING busy box
        lpproc=MakeProcInstance((FARPROC)WSDownloadSingleDlgProc, hInstanceWS);
        if (hDldWnd = CreateDialog(hInstanceWS, (LPSTR)"DOWNLOAD", hWndWS, lpproc))
        {
            SendMessage(hDldWnd, MM_SET_DOWNLOAD_TYPE, IDS_MSG_UPDATE_SHOPPER_SHORT, 0L);
            SendMessage(hDldWnd, MM_SET_DEST_DIR, 0, (LONG)((LPSTR)szExePath));
            SendMessage(hDldWnd, MM_SET_FILE_NAME, 0, (LONG)((LPSTR)"NEWSHOP.EXE"));

            nResult = Download(NULL, szExePath,
                              TRUE, FALSE, FALSE,
                              TRUE, TRUE, NULL);

            if (hDldWnd) // then dld not terminated by user
            {
                DestroyWindow(hDldWnd);
                hDldWnd = NULL;
                FreeProcInstance(lpproc);
            }
            else        // update terminated by user
            {
                FreeProcInstance(lpproc);
                IssueErr();
                /* tell em how to get the update by mail  */
                SetDest (ES_ACTIVEWND, NOBEEP, MB_OK | MB_ICONASTERISK, IDS_TITLE_APPNAME);
                ErrMsg(IDS_MSG_UPDATE_BY_MAIL);
                END(FAIL)
            }

            if ( nResult == OK )
            {
                char sztmpPath[_MAX_PATH];
                // tell 'em they should exit and run NEWSHOP.BAT
                FixDirPath(NULL, szExePath, sztmpPath, _MAX_PATH);
                SetDest (hWndWS, NOBEEP, MB_ICONEXCLAMATION | MB_OK, IDS_TITLE_APPNAME);
                ErrMsg(IDS_MSG_NEW_EXE, (LPSTR)sztmpPath);
				    PostMessage (hWndWS, WM_DESTROY, 0, 0L);
                END(FAIL)
            }
            else //if (NOT bConnect) // something went wrong
            {
                IssueErr();
                END(FAIL)
            }
        }
    }

    BusyLoadNewText(hWndWS, IDS_MSG_UPDATING_CUST_REC, IDS_BUSY_PENCIL_ICON);
    SetStatusBarText (szUpdateCust);

    if (Send_Message(MSG_SET_CUST_INFO,&custinfo.customer) != OK)
    {
        ErrMsg(IDS_ERR_UNABLE_TO_CONNECT);
        END(FAIL)
    }

    if (bNewUser)
    {
        if (bHaveAUserId) 
        {
            WORD nUserIDOK;
            if (Send_Message(MSG_SET_USER_ID,custinfo.userid) != OK)
            {
                ErrMsg (IDS_ERR_UNABLE_TO_CONNECT);
                END(FAIL)
            }

            if (Send_Message(MSG_BEGIN_SESSION) != OK)
            /* assumes userid is set. necessary for ingres operations */
            {
                ErrMsg (IDS_ERR_UNABLE_TO_CONNECT);
                END(FAIL)
            }

            if (Send_Message(MSG_QUERY_NEW_USERID_OK,&nUserIDOK) != OK)
            {
                ErrMsg (IDS_ERR_UNABLE_TO_CONNECT);
                END(FAIL)
            }

            switch (nUserIDOK)
            {
                case WS_AUTH: // paid for
                break;
                case WS_ACTIVE:   // Has already registered/logged in once
                {
                  bActiveNewUser = TRUE;
                  break;
                }
                case WS_EXPIRED:  // and is expired!
                {
                  bActiveExpiredNewUser = TRUE;
                  bActiveNewUser = TRUE;
                  break;
                }
                default: // allow chance to reenter?
                    ErrMsg(IDS_ERR_BAD_USERID);
                    END(FAIL)
                break;
            }
        }
        else // they need to get a userid
        {
            if (Send_Message(MSG_GET_NEW_USER_ID,custinfo.userid) != OK)
            {
                ErrMsg(IDS_ERR_BAD_NEW_ACCOUNT);
                END(FAIL)
            }

            if (Send_Message(MSG_BEGIN_SESSION) != OK)
            /* assumes userid is set. necessary for ingres operations */
            {
                ErrMsg (IDS_ERR_UNABLE_TO_CONNECT);
                END(FAIL)
            }

            // Put up subscription order form and get their money
            lpproc=MakeProcInstance((FARPROC) WSSubscriptionDlgProc, hInstanceWS) ;
            nRetval = DialogBox(hInstanceWS,(LPSTR) "SUBSCRIPTORDER",hWndWS,lpproc);
            FreeProcInstance(lpproc);

            if (nRetval != OK)
            {
                custinfo.userid[0] = '\0';
                END(FAIL)
            }
        }
        // Need to verify password rather then set it for already Active user
        // if its correct we will go ahead and reset custinfo rather than
        // COMMIT_NEWUSER
        if (bActiveNewUser)
        {
            char passTmp[PW_SIZE+1];
        		if (Send_Message(MSG_QUERY_PASSWD,passTmp) != OK)
        		{
            		ErrMsg(IDS_ERR_UNABLE_TO_CONNECT);
            		END(FAIL)
        		}
            if (stricmp(custinfo.password,passTmp) != 0)
            {
            	ErrMsg (IDS_ERR_INVALID_PASSWORD);
            	lstrcpy (custinfo.password, passTmp); // store it in the global for VerifyDlgProc

            	// try password validation again while still connected
            	lpproc=MakeProcInstance((FARPROC) WSVerifyPasswdDlgProc, hInstanceWS) ;
            	nRetval = DialogBox(hInstanceWS,(LPSTR) "NEWUSER",hWndWS,lpproc);
            	FreeProcInstance(lpproc);

            	if (nRetval != OK)
              		END(FAIL)
            }
        		if (Send_Message(MSG_COMMIT_CUST_INFO) != OK)
        		{
            		ErrMsg(IDS_ERR_UNABLE_TO_CONNECT);
            		END(FAIL)
        		}
            if (bActiveExpiredNewUser)
               goto ActiveExpiredNewUser; //Watch out!!
        }
        else
        {
           if (Send_Message(MSG_SET_PASSWD,custinfo.password) != OK)
           {
               ErrMsg(IDS_ERR_BAD_NEW_ACCOUNT);
               END(FAIL)
           }
        }
    }
    else // not a new user
    {
        char passTmp[PW_SIZE+1];
        WORD nUserIDOK;

        if (Send_Message(MSG_SET_USER_ID, custinfo.userid) != OK)
        {
            ErrMsg(IDS_ERR_UNABLE_TO_CONNECT);
            END(FAIL)
        }

        if (Send_Message(MSG_BEGIN_SESSION) != OK)
        /* assumes userid is set. necessary for ingres operations */
        {
            ErrMsg (IDS_ERR_UNABLE_TO_CONNECT);
            END(FAIL)
        }

        if (Send_Message(MSG_QUERY_USERID_OK, &nUserIDOK) != OK)
        {
            ErrMsg (IDS_ERR_UNABLE_TO_CONNECT);
            END(FAIL)
        }

        switch (nUserIDOK)
        {
            case WS_ACTIVE:
        		if (Send_Message(MSG_QUERY_PASSWD, passTmp) != OK)
        		{
            		ErrMsg(IDS_ERR_UNABLE_TO_CONNECT);
            		END(FAIL)
        		}
        		else
        		{
            		if (stricmp(custinfo.password,passTmp) != 0)
            		{
                		ErrMsg (IDS_ERR_INVALID_PASSWORD);
                		lstrcpy (custinfo.password, passTmp); // store it in the global for VerifyDlgProc

                		// try password validation again while still connected
                		lpproc=MakeProcInstance((FARPROC) WSVerifyPasswdDlgProc, hInstanceWS) ;
                		nRetval = DialogBox(hInstanceWS,(LPSTR) "NEWUSER",hWndWS,lpproc);
                		FreeProcInstance(lpproc);

                		if (nRetval != OK)
                    		END(FAIL)
            		}
        		}

        		/** commit the customer information (it may have changed since last
            		logged on) */
        		if (Send_Message(MSG_COMMIT_CUST_INFO) != OK)
        		{
            		ErrMsg(IDS_ERR_UNABLE_TO_CONNECT);
            		END(FAIL)
        		}
            break;
			case WS_EXPIRED:

        		if (Send_Message(MSG_QUERY_PASSWD,passTmp) != OK)
        		{
            		ErrMsg(IDS_ERR_UNABLE_TO_CONNECT);
            		END(FAIL)
        		}
        		else
        		{
            		if (stricmp(custinfo.password,passTmp) != 0)
            		{
                		ErrMsg (IDS_ERR_INVALID_PASSWORD);
                		lstrcpy (custinfo.password, passTmp); // store it in the global for VerifyDlgProc

                		// try password validation again while still connected
                		lpproc=MakeProcInstance((FARPROC) WSVerifyPasswdDlgProc, hInstanceWS) ;
                		nRetval = DialogBox(hInstanceWS,(LPSTR) "NEWUSER",hWndWS,lpproc);
                		FreeProcInstance(lpproc);

                		if (nRetval != OK)
                    		END(FAIL)
            		}
        		}

            ActiveExpiredNewUser:
            if (Send_Message(MSG_QUERY_SUBTYPE,&nSubType) != OK)
            {
               ErrMsg (IDS_ERR_UNABLE_TO_CONNECT);
               END(FAIL)
            }
            if (nSubType == WS_SUBTYPE_DEMO)
               bUpgradingUser = TRUE;
            else
               //Find out if we are shipping a package for renewer's
               if (Send_Message(MSG_QUERY_SHIP_RENEWAL,&bShipRenewal) != OK)
               {
                   ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
                   ErrMsg (IDS_ERR_BAD_NEW_ACCOUNT);
			          return FALSE;
               }

            SetDest (ES_ACTIVEWND, NOBEEP, MB_OKCANCEL, IDS_TITLE_APPNAME);
            if ( ErrMsg (IDS_ERR_EXPIRED_ID) != IDOK )
				   END(FAIL)

				if (NOT (bShipRenewal OR bUpgradingUser))
            {
               SetDest (ES_ACTIVEWND, NOBEEP, MB_OK, IDS_TITLE_APPNAME);
               if ( ErrMsg (IDS_MSG_NO_RENEWAL_PKG) != IDOK )
					   END(FAIL)
            }

				bRenewingUser = TRUE;
            lpproc=MakeProcInstance((FARPROC) WSSubscriptionDlgProc, hInstanceWS) ;
            nRetval = DialogBox(hInstanceWS,(LPSTR) "SUBSCRIPTORDER",hWndWS,lpproc);
            FreeProcInstance(lpproc);

				if ( nRetval != OK )
					END(FAIL)
			break;
         default:
             ErrMsg(IDS_ERR_BAD_USERID);
             END(FAIL)
         break;
        }
    }

    BusyLoadNewText(hWndWS, IDS_MSG_CHECKING_DATABASE, IDS_BUSY_DB_ICON);
    SetStatusBarText (szUpdateData);
    UpdateLocalDataBase();

    if (NOT bConnect)
        END(FAIL)

    /* fill main window list boxes */
    FillListBoxes(hWndWS, FALSE);

    if (NOT bConnect)
        END(FAIL)

    if (bNewUser AND NOT bActiveExpiredNewUser)
    {
        /* got their money, so hangup and freeze their machine... */
	     // Don't try to commit NewUser if already active
        if (NOT bActiveNewUser AND Send_Message(MSG_COMMIT_NEWUSER) != OK)
        {
            ErrMsg(IDS_ERR_BAD_NEW_ACCOUNT);
            END(FAIL)
        }
        else
        {
            if (bHaveAUserId)
            {
               /* tell them to save their password */
               if (bActiveNewUser) // give feedback that user is re-registering
	               ErrMsg(IDS_MSG_SAVE_ACTIVE_USERID);
               else
	               ErrMsg(IDS_MSG_SAVE_USERID);
            }
            else
               /* tell them their new userid */
	            ErrMsg(IDS_MSG_NEW_USERID,(LPSTR) custinfo.userid);
        }
   }
	else if (bRenewingUser)
	{
        /* got their money, so hangup and freeze their machine... */
        if (Send_Message(MSG_COMMIT_RENEWAL) != OK)
        {
            ErrMsg(IDS_ERR_BAD_NEW_ACCOUNT);
            END(FAIL)
        }
        else
            /* tell them they are renewed */
	        ErrMsg(IDS_MSG_COMPLETE_RENEWAL);
	}

    /* unencrypt, then reencrypt the cards just to get the valid flags. */
    if ( !UnencryptCards () )
        ErrMsg (IDS_ERR_UNENCRYPTING);
    else
    {
        if ( !EncryptCards () )
            ErrMsg (IDS_ERR_ENCRYPTING);
    }

   end:
	if (hCursor)
        SetCursor(hCursor);

    if (bConnect)
        SetWindowText(hChildren[ID_LIB_LOGON].hWnd,"&LogOff");

    if (nRetval == IDCANCEL)
        return (IDCANCEL);

    if ((nRetval != OK) AND bConnect)
        HangUp();

    BusyClear(FALSE);
    SetStatusBarText (szLibSelections);

    return (bConnect ? OK : FAIL);
}

/*************************************************************\
 ** Connect
 **
 ** ARGUMENTS:
 **  int baud;
 **  int com_number;
 **
 ** DESCRIPTION:
 **  Open and initialize com port.  Dial Adonis bbs.
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **  Sets error condition if error.
 **
 ** RETURNS:
 **  OK if successful, error condition if not.
 **
 ** HISTORY:   Doug Kent  April 19, 1990 (written)
\*************************************************************/
int FAR PASCAL 
Connect(void)
{
    static char portstr[5] = "COM";
	HCURSOR	    hCursor = NULL;
    int         nRetval = OK;
    BOOL        bOpened = FALSE;
    char        phoneNumber[12];

    portstr[3] =  (char)(custinfo.comm_port + '1');
    portstr[4] = '\0';

	hCursor = SetCursor(hPhoneCursArr[0]);

    if((nRetval = OpenComm(portstr,INBUFSIZE,OUTBUFSIZE)) < 0)
        END1(IDS_ERR_CHECK_MODEM)
    else
    {
        CommDCB.Id = (BYTE)nRetval;
        nRetval = OK;
    }
    bOpened = TRUE;

	SetCursor(hPhoneCursArr[1]);

	if (SetPortState() != OK)
        END1(IDS_ERR_CHECK_MODEM)

    SetLocal(ON);

	SetCursor(hPhoneCursArr[2]);

    if (WriteModemCommand(custinfo.userModemString) != OK)
        END1(IDS_ERR_CHECK_MODEM)

	SetCursor(hPhoneCursArr[3]);

    if (WriteModemCommand("\r~~~") != OK)
        END1(IDS_ERR_CHECK_MODEM)

    if (WriteModemCommand(custinfo.modemInitString) != OK)
        END1(IDS_ERR_CHECK_MODEM)

	SetCursor(hPhoneCursArr[0]);
    if (WriteModemCommand("\r~~~ATD") != OK)
        END1(IDS_ERR_CHECK_MODEM)

    if ((custinfo.dial_type + DIAL_ID_BASE) == ID_PULSE)
    {
        if (WriteModemCommand("P") != OK)
            END1(IDS_ERR_CHECK_MODEM)
    }
    else
    {
        if (WriteModemCommand("T") != OK)
            END1(IDS_ERR_CHECK_MODEM)
    }

    if (custinfo.phonePrefix[0])
        if (WriteModemCommand(custinfo.phonePrefix) != OK)
            END1(IDS_ERR_CHECK_MODEM)

    if ( GetPhoneNumber(phoneNumber, sizeof (phoneNumber)) != OK )
            END1(IDS_ERR_UNABLE_TO_CONNECT)

    if (WriteModemCommand(phoneNumber) != OK)
        END1(IDS_ERR_CHECK_MODEM)

    if ( custinfo.phoneSuffix[0] )
        if (WriteModemCommand(custinfo.phoneSuffix) != OK)
            END1(IDS_ERR_CHECK_MODEM)

    if (WriteModemCommand("\r") != OK)
        END1(IDS_ERR_CHECK_MODEM)

	SetCursor(hPhoneCursArr[0]);

#ifdef INTERNAL
    AuxPrint("Dialing %s...",(LPSTR) phoneNumber);
#endif
    do
    {
        switch (nRetval = WaitForConnection())
        {
            case OK:
                SetLocal(OFF);
#ifdef INTERNAL
                AuxPrint("Connected");
#endif
                bConnect = TRUE;
            break;
            case IDS_ERR_BUSY:
                SetDest (ES_ACTIVEWND, NOBEEP, MB_YESNO | MB_ICONQUESTION, IDS_TITLE_APPNAME);
                if ( ErrMsg (IDS_ERR_BUSY) == IDYES )
                {
                    if (WriteModemCommand("A/") != OK)
                        END1(IDS_ERR_CHECK_MODEM)
                    // continue loop...
                }
                else
                    END1(IDS_ERR_UNABLE_TO_CONNECT)
            break;
            default:
#ifdef INTERNAL
                AuxPrint("Unable to Connect");
#endif
                END1(IDS_ERR_UNABLE_TO_CONNECT)
            break;
        }
    }
    while (nRetval != OK);

    Wait(500L); // for communications link to settle down?
    if (BBSsync() != OK)
        END1(IDS_ERR_UNABLE_TO_CONNECT)

    end:
	if (hCursor)
        SetCursor(hCursor);

    if ((nRetval != OK) AND bOpened)
    {
        if (NOT bConnect)
            CloseComm(CommDCB.Id);
        else
            HangUp();
        bOpened=FALSE;
    }

    return nRetval;
}



#ifdef BETA
/*************************************************************\
 ** PrefixIsLocal
 **
 ** ARGUMENTS:      int aPrefix
 **                 
 ** DESCRIPTION:    do a binary search on the global data 
 **
 ** RETURNS:        OK if successful, FAIL if not.
 **
 ** HISTORY:   Lauren Bricker  June 14, 1990 (written)
 **  (9.26.90) D. Kent -- added -1 to 'end' initialization
\*************************************************************/

BOOL
PrefixIsLocal (prefix)
    int prefix;
{

    int start = 0,
        end   = (sizeof (localPrefixes) / sizeof(int)) - 1,
        x;

    while ( end >= start )
    {
        x = (start + end) / 2;
        if ( prefix < localPrefixes[x] )
            end = x - 1;
        else
            start = x + 1;
        if ( prefix == localPrefixes[x] )
            return TRUE;

    }

    return FALSE;

}
#endif


/*************************************************************\
 ** GetPhoneNumber
 **
 ** ARGUMENTS:      char *buffer - to receive the phone number
 **                 int  size    - of buffer to hold number
 **
 **
 ** DESCRIPTION:    Get the Adonis Clipart Bulletin board phone
 **                 number
 **
 ** RETURNS:        OK if successful, FAIL if not.
 **
 ** HISTORY:        Lauren Bricker  June 14, 1990 (written)
\*************************************************************/

BOOL
GetPhoneNumber (char* buffer, int size)
{
    BOOL HiSpeedModem;

    switch (custinfo.baud + BAUD_ID_BASE)
    {
        case ID_4800:
        case ID_9600:
            HiSpeedModem = TRUE;
            break;

        default:
            HiSpeedModem = FALSE;
            break;
    }
    
#if defined(BETA)
   {
    PTR_PHONE   ptrPhone = &(custinfo.customer.phone);
    char        prefix[MAXLEN_AREA + 1];
    int         theirPrefix, retVal;
    FARPROC     lpproc;

    if (custinfo.customer.whichCountry != (IDB_US - COUNTRY_ID_BASE))
    {
        /* then they gotta type their own long distance prefix */
        if (HiSpeedModem)  strncpy (buffer, LOCALNUM9600, size);    
        else strncpy (buffer, LOCALNUM2400, size);    
        return OK;
    }

    if ( !ptrPhone->area[0] || !ptrPhone->number[0] )
    {
        SetDest (ES_ACTIVEWND, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_APPNAME);
        if ( ErrMsg (IDS_ERR_MISSING_NUMBER) == IDYES )
        {
            lpproc = MakeProcInstance ((FARPROC)WSCustomerInfoDlgProc, hInstanceWS);
            retVal = DialogBox (hInstanceWS, "CUSTOMERINFO", hWndWS, lpproc);
            FreeProcInstance(lpproc);
            if ( !retVal )
                return FAIL;
        }
        else
            return FAIL;
    }

    if ( !strcmp (ptrPhone->area, LOCALAREA) )
    {
        strncpy (prefix, ptrPhone->number, MAXLEN_AREA);   
        prefix[MAXLEN_AREA + 1] = '\0';
        theirPrefix = atoi (prefix);

        /* Binary search on the localPrefixes table to see if the prefix
           is in there */
        if ( PrefixIsLocal (theirPrefix) )
        {   
            if (HiSpeedModem)  strncpy (buffer, LOCALNUM9600, size);    
            else strncpy (buffer, LOCALNUM2400, size);
            return OK;
        }
    }

    if (HiSpeedModem)  strncpy (buffer, LONGNUM9600, size);    
    else strncpy (buffer, LONGNUMBER, size);
  }

#elif defined(INTERNAL)
    if (bCallRetail)
       if (HiSpeedModem)  strncpy (buffer, LOCALNUM9600_RETAIL, size);
       else strncpy (buffer, LOCALNUM2400_RETAIL, size);
    else
       if (HiSpeedModem)  strncpy (buffer, LOCALNUM9600_DEV, size);
       else strncpy (buffer, LOCALNUM2400_DEV, size);
#else
    if (HiSpeedModem)  strncpy (buffer, LOCALNUM9600_RETAIL, size);
    else strncpy (buffer, LOCALNUM2400_RETAIL, size);
#endif

    return OK;
}

#if 0
static int
QueryNewUserStatus(ORDER *Order)
{
    CREDIT_CARD_STRUCT cc;
    BOOL nRetval;
    BOOL stat;

    memset(Order,0,sizeof(ORDER));

    Order->prodType = WS;

    Order->CourierType = custinfo.whichShip;
    Order->ShipAddress = custinfo.mailing;

    /* credit card stuff */
    UnencryptCardNumber(custinfo.whichCard,&cc);
    lstrcpy(Order->ccName,cc.name);
    lstrcpy(Order->ccNum,cc.number);

    wsprintf((LPSTR)Order->ccExpDate, (LPSTR)"%02d%02d",
               cc.exp_month, cc.exp_year);
    Order->ccType = custinfo.whichCard;

    Order->Quantity = 1;
    Order->priceTotal = 3500;
    Order->priceEach = 3500;

    if (Send_Message(MSG_SET_NEW_USER_ORDER,Order) != OK)
        return FAIL;

    /* gets money */
    BusyBoxWithIcon(hInstanceWS, NULL, IDS_MSG_VALIDATE_CREDIT, IDS_BUSY_CREDIT_ICON);
    stat = Send_Message(MSG_QUERY_NEWUSER_OK,&nRetval);
    BusyClear(FALSE);

    if (stat != OK)
        return FAIL;

    end:

    return nRetval;
}

#endif
