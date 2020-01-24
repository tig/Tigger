#include "nowin.h"
#include <windows.h>
#include "winundoc.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ws.h"
#include "misc.h"
#include "comm.h"
#include "order.h"
#include "file.h"
#include "help.h"
#include "rc_symb.h"
#include "custinfo.h"
#include "billing.h"
#include "error.h"
#include "info.h"           
#include "messages.h"
#include "busy.h"

static WORD QueryOrderInStock(void);
static int GetShippingCosts(void);
static WORD SubmitOrder(HWND hWndDlg);
static WORD SendOrderStruct(ORDER *Order);
static WORD QueryOrderCredit(ORDER *Order);
static int ComputePrices(HWND hWndDlg, int nMediaIndex, char *szVersion);

/* filled in by ComputerPrices(), used by SendOrderStruct() */
static int iQuantity;
static int prevQuantity;
static double Total;
static double WSPrice;
static double Taxes;
static double Shipping;
static double SurCharge;
static int nMediaType,nMIndex;
static char szVersion[MAX_STR_LEN+1];

/* g-g-g-g-globals  */
SHIPPING_COSTS_STRUCT ShippingCosts[MAX_NUM_SERVICES];


char *MediaStrings[4] = { "5¼\" 360K Floppies",
                          "5¼\" 1.2M Floppies" ,
                          "3½\" 720K Diskettes" ,
                          "3½\" 1.4M Diskettes" } ;

static void             FillDlgItem (HWND hWndGet, HWND hWndPut, char padChar);

static char szLibraryOrderTitle[]=" Library Order Form ";
static char szUsers[PW_SIZE + 1];
static int  UID;             
static BOOL bPass1;
BOOL UPS1, UPS2, USPS;

#define PADDING '*'


BOOL bKillViewbox = FALSE;

/*************************************************************\
 ** WSNewUserDlgProc
 **
 ** ARGUMENTS:  HWND    hDlg
 **             unsigned message
 **             WORD    wParam
 **             LONG    lParam
 **
 ** DESCRIPTION:    password dlg procedure
 **
 ** ASSUMES:
 **  User has userID, but is not yet in win.ini.  User must define a 
 **  new password.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   John Ano  April 20, 1990 (written)
\*************************************************************/

LONG FAR PASCAL
WSNewUserDlgProc(HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    REFERENCE_FORMAL_PARAM(lParam);

    switch(message)
    {
        case WM_INITDIALOG:
            CenterPopUp(hWndDlg); 
            SetDlgItemText(hWndDlg, IDS_PW_TEXT, "Define a new password:");

            ShowWindow(GetDlgItem(hWndDlg,IDS_BLANK_USER), SW_HIDE);
            SendDlgItemMessage (hWndDlg, IDS_USER_ID, EM_LIMITTEXT, PW_SIZE, 0L);
            SendDlgItemMessage (hWndDlg, ID_PASSWORD, EM_LIMITTEXT, PW_SIZE, 0L);
            SendDlgItemMessage (hWndDlg, ID_BLANK,    EM_LIMITTEXT, PW_SIZE, 0L);

            SetWindowWord (GetDlgItem (hWndDlg, ID_BLANK), nOffsetToPass,
                           GetDlgItem (hWndDlg, ID_PASSWORD));

            SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_NEW_USER);
            if ( ErrMsg (IDS_MSG_HAVE_ID) == IDYES )
            {
                SetFocus(GetDlgItem(hWndDlg, IDS_USER_ID)) ;
                EnableWindow(GetDlgItem(hWndDlg,IDS_PW_TEXT), FALSE);
                EnableWindow(GetDlgItem(hWndDlg,ID_PASSWORD), FALSE);
                EnableWindow(GetDlgItem(hWndDlg,ID_BLANK),    FALSE);
                bPass1=TRUE;
            }
            else
            {
                SetFocus(GetDlgItem(hWndDlg, ID_BLANK)) ;
                EnableWindow(GetDlgItem(hWndDlg,IDL_UID_LABEL), FALSE);
                EnableWindow(GetDlgItem(hWndDlg,IDS_USER_ID),   FALSE);
                bPass1 = FALSE;
                UID = 2;
            }
            return FALSE; // changed focus

        case WM_COMMAND:
            switch(wParam)
            {
                case IDCANCEL:
                    EndDialog(hWndDlg,IDCANCEL) ;
                    break;
                case ID_HELP:
                    GetHelp (GH_TEXT,IDH_NEWUSER);
                    break;
                case IDOK:
                    if (bPass1)  // getting user id
                        UID = 0;
                    if (GetDlgItemText(hWndDlg,IDS_USER_ID+UID,szUsers,sizeof (szUsers)) == 0)
                    {
                        WORD    focus = IDS_USER_ID + UID;
                        if ( UID )
                            focus++;
                        SetFocus (GetDlgItem (hWndDlg, focus));
                        break;
                    }
                    //otherwise, there was something in the password buff.
                    if (bPass1)
                       // if this is a new user process and they are entering the user id,
                       // write it out and go to the password!
                       {
                        strcpy(custinfo.userid, szUsers);
                        // getting password,
                        // Enable the pwStuff;
                        EnableWindow(GetDlgItem(hWndDlg,IDS_PW_TEXT), TRUE);
                        EnableWindow(GetDlgItem(hWndDlg,ID_PASSWORD), TRUE);
                        EnableWindow(GetDlgItem(hWndDlg,ID_BLANK),    TRUE);
                        EnableWindow(GetDlgItem(hWndDlg,IDS_USER_ID),   FALSE);
                        EnableWindow(GetDlgItem(hWndDlg,IDL_UID_LABEL), FALSE);
                        SetFocus(GetDlgItem(hWndDlg, ID_BLANK));
                        UID = 2;
                        bPass1 = FALSE;
                    }
                    else
                    {
                        int pwlen;
                        pwlen=strlen(szUsers);
                        if ((pwlen >= 5) && (pwlen <= 8))
                        {
                            strcpy(custinfo.password, szUsers);
                            EndDialog(hWndDlg, OK);
                        }
                        else
                        {
                            SetFocus(GetDlgItem(hWndDlg,  ID_BLANK));
                            HIGHLIGHT(GetDlgItem(hWndDlg, ID_BLANK));
                            ErrMsg(IDS_ERR_PASSWD_LEN);
                        }
                    }
                    break;
                default:
                    return (FALSE);
            }
        default:
            return (FALSE);
    }
    return (TRUE);
}


/*************************************************************\
 ** WSChangePasswordDlgProc
 **
 ** ARGUMENTS:  HWND    hDlg
 **             unsigned message
 **             WORD    wParam
 **             LONG    lParam
 **
 ** DESCRIPTION:    change password dlg procedure
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   John Ano  April 20, 1990 (written)
 **  cloned by (5.25.90) D. Kent
\*************************************************************/
LONG FAR PASCAL
WSChangePasswordDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    char szTitle[MAX_MSG_LEN];

    REFERENCE_FORMAL_PARAM(lParam);
    switch(message)
    {
        case WM_INITDIALOG:
        {
            HWND    hUserWnd = GetDlgItem (hWndDlg, IDS_USER_ID);
            LONG    tmpStyle = GetWindowLong (hUserWnd, GWL_STYLE);

            UnencryptCards();
            CenterPopUp(hWndDlg); 
            LoadString (hInstanceWS, IDS_TITLE_CHANGE_PW, szTitle, sizeof (szTitle));
            SetWindowText(hWndDlg, szTitle);
            SetDlgItemText(hWndDlg, IDL_UID_LABEL, "Enter your current password:");
            SetDlgItemText(hWndDlg, IDS_PW_TEXT,   "Enter your new password:");

            SendDlgItemMessage (hWndDlg, IDS_USER_ID,    EM_LIMITTEXT, PW_SIZE, 0L);
            SendDlgItemMessage (hWndDlg, IDS_BLANK_USER, EM_LIMITTEXT, PW_SIZE, 0L);
            SendDlgItemMessage (hWndDlg, ID_PASSWORD,    EM_LIMITTEXT, PW_SIZE, 0L);
            SendDlgItemMessage (hWndDlg, ID_BLANK,       EM_LIMITTEXT, PW_SIZE, 0L);

            SetWindowWord (GetDlgItem (hWndDlg, ID_BLANK), nOffsetToPass, 
                           GetDlgItem (hWndDlg, ID_PASSWORD));
            SetWindowWord (GetDlgItem (hWndDlg, IDS_BLANK_USER), nOffsetToPass, 
                           hUserWnd);
            /* Make it so the IDS_USER_ID window cant be tabbed to */
            SetWindowLong (hUserWnd, GWL_STYLE, tmpStyle & ~WS_TABSTOP);
            

            SetFocus(GetDlgItem(hWndDlg, IDS_BLANK_USER));
            EnableWindow(GetDlgItem(hWndDlg, IDS_PW_TEXT), FALSE);
            EnableWindow(GetDlgItem(hWndDlg, ID_PASSWORD), FALSE);
            EnableWindow(GetDlgItem(hWndDlg, ID_BLANK),    FALSE);

            bPass1 = TRUE;
            return FALSE; // changed focus
        }

        case WM_COMMAND:
            switch(wParam)
            {
                case IDCANCEL:
                {
                    HWND    hUserWnd = GetDlgItem (hWndDlg, IDS_USER_ID);
                    LONG    tmpStyle = GetWindowLong (hUserWnd, GWL_STYLE);

                    EncryptCards();
                    /* Fix the IDS_USER_ID window style. */
                    SetWindowLong (hUserWnd, GWL_STYLE, tmpStyle | WS_TABSTOP);
                    EndDialog(hWndDlg,IDCANCEL);
                    break;
                }
                case ID_HELP:
                    GetHelp (GH_TEXT,IDH_CHANGEPW);
                    break ;
                case IDOK:
                    if (bPass1)  // getting current password
                        UID = 0;
                    if (GetDlgItemText(hWndDlg, IDS_USER_ID+UID, 
                                       szUsers, sizeof (szUsers))==0)
                    {
                        SetFocus(GetDlgItem(hWndDlg, IDS_USER_ID+UID+1));
                        break;
                    }

                    //otherwise, there was something in the password buff.
                    if (bPass1)
                    {
                        if (strcmpi(szUsers, custinfo.password))
                        /* bad password */
                        {
                            SetFocus(GetDlgItem(hWndDlg, IDS_BLANK_USER));
                            HIGHLIGHT(GetDlgItem(hWndDlg, IDS_BLANK_USER));
                            //SetDest (hWndDlg, NOBEEP, MB_ICONEXCLAMATION | MB_OK, IDS_TITLE_CHANGE_PW);
                            ErrMsg (IDS_ERR_INVALID_PASSWORD);
                        }
                        else // move on to new password 
                        {
                            EnableWindow(GetDlgItem(hWndDlg,IDL_UID_LABEL),  FALSE);
                            EnableWindow(GetDlgItem(hWndDlg,IDS_USER_ID),    FALSE);
                            EnableWindow(GetDlgItem(hWndDlg,IDS_BLANK_USER), FALSE);
                            EnableWindow(GetDlgItem(hWndDlg,IDS_PW_TEXT), TRUE);
                            EnableWindow(GetDlgItem(hWndDlg,ID_PASSWORD), TRUE);
                            EnableWindow(GetDlgItem(hWndDlg,ID_BLANK),    TRUE);
                            SetFocus(GetDlgItem(hWndDlg,ID_BLANK));
                            UID = 2;
                            bPass1 = FALSE;
                        }
                    }
                    else // bPass2
                    {
                        int pwlen;
                        pwlen=strlen(szUsers);
                        if ((pwlen >= 5) && (pwlen <= 8))
                        {
                            if (!strcmpi(custinfo.password, szUsers))
                            /* then no change */
                            {
                                SetFocus(GetDlgItem(hWndDlg,  ID_BLANK));
                                HIGHLIGHT(GetDlgItem(hWndDlg, ID_BLANK));
                            }
                            else /* change */
                            {
                                HWND    hUserWnd = GetDlgItem (hWndDlg, IDS_USER_ID);
                                LONG    tmpStyle = GetWindowLong (hUserWnd, GWL_STYLE);

                                if (Send_Message(MSG_SET_PASSWD,szUsers) != OK)
                                {
                                    ErrMsg(IDS_ERR_SETTING_PASSWD);
                                    /* set back to original password */
                                    if (bConnect)
                                        Send_Message(MSG_SET_PASSWD,custinfo.password);
                                    EndDialog(hWndDlg, FAIL);
                                }
                                if (Send_Message(MSG_COMMIT_PASSWD) != OK)
                                {
                                    ErrMsg(IDS_ERR_SETTING_PASSWD);
                                    /* set back to original password */
                                    if (bConnect)
                                        Send_Message(MSG_SET_PASSWD,custinfo.password);
                                    EndDialog(hWndDlg, FAIL);
                                }

                                strcpy(custinfo.password, szUsers);
                                // change the card encryption based on new password
                                EncryptCards();
                                WriteCustInfo(WRITE_CARDINFO);
                                /* Fix the IDS_USER_ID window style. */
                                SetWindowLong (hUserWnd, GWL_STYLE, tmpStyle | WS_TABSTOP);
                                EndDialog(hWndDlg, OK);
                            }
                        }
                        else
                        {
                            SetFocus(GetDlgItem(hWndDlg,  ID_BLANK));
                            HIGHLIGHT(GetDlgItem(hWndDlg, ID_BLANK));
                            ErrMsg(IDS_ERR_PASSWD_LEN);
                        }

                    }
                    break;
                default:
                    return (FALSE);
            }
        default:
            return (FALSE);
    }
    return (TRUE);
}

VOID FAR PASCAL 
CreateOrder(void) 
{
	FARPROC	lpproc;

   EnableAllModeless(FALSE);
	lpproc = MakeProcInstance((FARPROC)WSOrderFormDlgProc, hInstanceWS);
   /* don't use hWndView as 'parent' because it is modeless */
	DialogBox(hInstanceWS, (LPSTR)"LIBORDER", hWndWS, lpproc);
	FreeProcInstance(lpproc);
   EnableAllModeless(TRUE);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSOrderFormDlgProc                                     */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
static BOOL bEditHasFocus=FALSE;
LONG FAR PASCAL 
WSOrderFormDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
	FARPROC	lpproc;
    char    szTitle[80];
    HWND    hWndLB;
    BOOL    bReturn;
    char szQuantity[3];
    static BOOL bReturnFocusToEdit=FALSE;
    /* turn WSPrice red stuff: */
    static  HBRUSH   hBrush;
    /* ====================== */

	switch(message) {
		case WM_INITDIALOG:
			   CenterPopUp(hWndDlg);
            iQuantity = 1;
            szQuantity[2]='\0';
            wsprintf((LPSTR)szTitle, (LPSTR)"%s - %s",
                     (LPSTR)szLibraryOrderTitle, (LPSTR)CURLIB);
            SetWindowText(hWndDlg,(LPSTR) szTitle);
            EnableWindow(GetDlgItem(hWndDlg,IDB_SUBMIT),FALSE);

            itoa(iQuantity,szQuantity,10);
            SetDlgItemText(hWndDlg,IDS_LIB_QUANTITY, (LPSTR) szQuantity);

            SetDlgItemText(hWndDlg, IDS_PUBLISHER, (LPSTR)CURPUB);
            //nNumClips = SumSubLibSizes(CURLIBFILE);
            //itoa(nNumClips,CURLIBNUMIMAGES,10);
            SetDlgItemText(hWndDlg, IDS_NUMBER_CLIPART,(LPSTR)CURLIBNUMIMAGES);

            /* fill media list box */
            nMIndex = 0;
            hWndLB=GetDlgItem(hWndDlg, IDL_MEDIA);
            if (EnumMedia(AddStringToListBox,hWndLB,CURLIBFILE) <= 0)
            {
                ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                       (LPSTR)StripPath (CURLIBFILE));
                EndDialog(hWndDlg,FALSE);
            }
	         SendMessage(hWndLB, LB_SETCURSEL, 0, 0L);
            /* convert to normalized mediatype (11.1.90) D. Kent */
            TranslateMType(hWndLB,nMIndex,&nMediaType);

            /* fill version list box */
            szVersion[0] = '\0';
            hWndLB=GetDlgItem(hWndDlg, IDL_VERSION);
            if (EnumLibVersions(AddStringToListBox,hWndLB,CURLIBFILE) <= 0)
            {
                ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                       (LPSTR)StripPath (CURLIBFILE));
                EndDialog(hWndDlg,FALSE);
            }
	         SendMessage(hWndLB, LB_SETCURSEL, 0, 0L);
            SendMessage(hWndLB,LB_GETTEXT,0,(LONG)(LPSTR)szVersion);

            hBrush=CreateSolidBrush(GetSysColor(COLOR_WINDOW));

            // to compute prices: (in clean way)
            bEditHasFocus = TRUE;
            prevQuantity = -1;
            PostMessage(hWndDlg,WM_USER,0,0L); 

			return FALSE;

        case WM_CTLCOLOR:
            if ((LOWORD(lParam) != GetDlgItem(hWndDlg, IDS_DISCOUNT_LABEL)) AND
                (LOWORD(lParam) != GetDlgItem(hWndDlg, IDS_DISCOUNT_AMT)))

                return(FALSE);

            /************************************************ KRG ***
            *
            *   This assumes a set number of colors for windows bkgrnd
            *   and Text which may not  hold true for WIN 3.0
            *
            *********************************************************/
            SetBkColor(wParam, GetSysColor(COLOR_WINDOW));

            if ((GetSysColor(COLOR_WINDOWTEXT) != RGB(255, 0, 0)) &&
                  (GetSysColor(COLOR_WINDOW) != RGB(255,0,0) ) )
            {
              SetTextColor(wParam, RGB(255,0,0));
            }
            else
            {
              SetTextColor(wParam, RGB(255,255,0));
            }
            return((DWORD)hBrush);

        /* when edit box loses focus or user presses ENTER key in edit window */
        case WM_USER:
        /* frequently when focus moves from one button to another? */
            {
                BOOL bSaveEditFocus = bEditHasFocus;
                if (GetDlgItemText(hWndDlg,IDS_LIB_QUANTITY,szQuantity,3) == 0)
                {
                    SetDlgItemText(hWndDlg,IDS_LIB_QUANTITY,(LPSTR)"1");
                    iQuantity = 1;
                }
                else
                {
                    iQuantity=atoi(szQuantity);
                }

                if (iQuantity != prevQuantity)
                    if (ComputePrices(hWndDlg,nMIndex,szVersion) != OK)
                        EndDialog(hWndDlg,FALSE);

                if (bSaveEditFocus)
                {
                    SetFocus(GetDlgItem(hWndDlg,IDS_LIB_QUANTITY));
                    HIGHLIGHT(GetDlgItem(hWndDlg, IDS_LIB_QUANTITY));
                }
            }
            return TRUE;
        break;

        case WM_DESTROY:
            bEditHasFocus = FALSE; // or else fatal exit
            return FALSE;
        break;

		case WM_COMMAND:
			switch(wParam) 
            {
                case ID_HELP:
                    SendMessage(hWndDlg,WM_USER,0,0L);  // only compute if coming from edit
		              GetHelp(GH_TEXT,IDH_ORDERLIB);
                break;

                case IDB_SELECT_BILLING:
                {
                    int nRetval;

                    if (NOT bConnect)
                    {
                        ErrMsg(IDS_ERR_MUST_LOGON);
                        break;
                    }

                    lpproc=MakeProcInstance((FARPROC)WSVerifyPasswdDlgProc,hInstanceWS) ;
                    nRetval=DialogBox(hInstanceWS,(LPSTR)"NEWUSER",hWndDlg,lpproc) ;
                    FreeProcInstance(lpproc) ;

                    switch(nRetval)
                    {
                        case OK:
                            break;
                        case IDCANCEL:
                        case FAIL:
                            return FALSE;
                            break;
                        default:
                            return FALSE;
                    }

	                 lpproc = MakeProcInstance((FARPROC)WSBillingInfoDlgProc, hInstanceWS);
	                 nRetval = DialogBox(hInstanceWS, (LPSTR)"BILLINGINFO", hWndDlg, lpproc);
	                 FreeProcInstance(lpproc);

                    if (nRetval)  //else user CANCELED dialog
                    {
                       if (ComputePrices(hWndDlg, nMIndex, szVersion) != OK)
                            EndDialog(hWndDlg, FALSE);
                    }

                    return ( TRUE );
                }

                case IDB_SHIPPING_INFO:
                {
                   int nRetval;

	                lpproc=MakeProcInstance((FARPROC)WSShippingInfoDlgProc, hInstanceWS);
	                nRetval = DialogBox(hInstanceWS, (LPSTR)"SHIPPINGINFO", hWndDlg, lpproc);
	                FreeProcInstance(lpproc);

                   if (nRetval)  //else user CANCELED dialog
                   {
                      if (ComputePrices(hWndDlg, nMIndex, szVersion) != OK)
                         EndDialog(hWndDlg,FALSE);
                   }

                   return( TRUE );
                }

                case IDL_MEDIA:
		            switch(HIWORD(lParam))
                    {
                        case LBN_SELCHANGE:
                        {
                            int nOldMediaType = nMIndex;
                            nMIndex = QueryListSel(GetDlgItem(hWndDlg,IDL_MEDIA));
                            if (nMIndex != nOldMediaType)
                            {
                                /* convert to normalized mediatype (11.1.90) D. Kent */
                                TranslateMType(GetDlgItem(hWndDlg,IDL_MEDIA),nMIndex,&nMediaType);
                                if (ComputePrices(hWndDlg,nMIndex,szVersion) != OK)
                                    EndDialog(hWndDlg,FALSE);
                             }
                        }
                        break;
                        default:
                            return FALSE;
                    }
                    break;
                case IDL_VERSION:
		            switch(HIWORD(lParam))
                    {
                        case LBN_SELCHANGE:
                        {
                            char szOldVersion[MAX_STR_LEN];
                            strcpy(szOldVersion,szVersion);
                            SendMessage(GetDlgItem(hWndDlg, IDL_VERSION),
                                LB_GETTEXT,
                                QueryListSel(GetDlgItem(hWndDlg, IDL_VERSION)),
                                (LONG)(LPSTR)szVersion);
                            if (strcmp(szOldVersion,szVersion))
                                if (ComputePrices(hWndDlg,nMIndex,szVersion) != OK)
                                    EndDialog(hWndDlg,FALSE);
                        }
                        break;
                        default:
                            return FALSE;
                    }
                    break;
                case IDS_LIB_QUANTITY:
                    switch (HIWORD(lParam))
                    {
                        case EN_CHANGE:
                        {
                            char Quantity[5];
                            // if EDIT contents exists, Enable the SUBMIT button;
                            GetDlgItemText (hWndDlg, IDS_LIB_QUANTITY, Quantity, sizeof (Quantity));
                            EnableWindow(GetDlgItem(hWndDlg,IDB_SUBMIT), Quantity[0]);
                            return FALSE;
                        }

                        case EN_KILLFOCUS:
                                bEditHasFocus = FALSE;
                            return FALSE;
                        break;

                        case EN_SETFOCUS:
                            bEditHasFocus = TRUE;
                        break;
                    }

                    break;

            	case IDB_SUBMIT:
#if 0
#define ENABLE(onoff) if (wWinVer < 0x300) \
            		    EnableWindow(GetDlgItem (hWndDlg, IDB_SUBMIT), onoff); \
				      EnableWindow (hWndDlg, onoff)
#else
#define ENABLE(onoff) ;
#endif

#if 0
                    // work around for Win 2.11 bug!!
                    ENABLE(FALSE);
#endif
                    SendMessage(hWndDlg,WM_USER,0,0L); // only compute if coming from edit

                    if (NOT bConnect)
                    {
                        //SetDest (hWndDlg, NOBEEP, MB_OK, IDS_TITLE_APPNAME);
                        ErrMsg(IDS_ERR_MUST_LOGON);
                        ENABLE(TRUE)
                        break;
                    }

                    if ( NOT ValidShippingInfo() )
                    {
	                    lpproc=MakeProcInstance( (FARPROC)WSShippingInfoDlgProc, hInstanceWS);
	                    bReturn = DialogBox(hInstanceWS, (LPSTR)"SHIPPINGINFO", hWndDlg, lpproc);
	                    FreeProcInstance(lpproc);
                        if ( !bReturn )
                        {
                            ENABLE(TRUE)
                            break;
                        }
                    }

                    if ( !custinfo.card[custinfo.whichCard].valid )
                    {
                        SetDest (hWndDlg, NOBEEP, MB_YESNO| MB_ICONQUESTION, IDS_TITLE_APPNAME);
                        if ( ErrMsg (IDS_ERR_INVALID_BILLING) == IDNO )
                        {
                            ENABLE(TRUE)
                            break;
                        }

                        lpproc = MakeProcInstance ((FARPROC)WSVerifyPasswdDlgProc, hInstanceWS) ;
                        bReturn = DialogBox (hInstanceWS, "NEWUSER", hWndDlg, lpproc);
                        FreeProcInstance (lpproc);
                        if ( bReturn != OK)
                        {
                            ENABLE(TRUE)
                            break;
                        }

                        lpproc = MakeProcInstance ((FARPROC)WSBillingInfoDlgProc, hInstanceWS) ;
                        bReturn = DialogBox (hInstanceWS, "BILLINGINFO", hWndDlg, lpproc);
                        FreeProcInstance (lpproc);
                        if ( !bReturn )
                        {
                            ENABLE(TRUE)
                            break;
                        }
                    }

                    /* if int'l, then shipping is not computed */
                    if ((custinfo.mailing.whichCountry+COUNTRY_ID_BASE) == IDB_OTHER)
                    {
                        SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_OKCANCEL, IDS_TITLE_APPNAME);
                        if (ErrMsg(IDS_MSG_NOSHIPPING) == IDCANCEL)
                            break;
                    }

                    /** at long last, send the order to the bbs and see if it 
                        flies.  If so, then we're off. **/
                    if (SubmitOrder(hWndDlg) == OK)
                    {
                        bKillViewbox = TRUE;     //bounce back to main window for quick logoff
					         EndDialog(hWndDlg, TRUE);
                    }
                    else
                    {
                        ENABLE(TRUE)
                    }
                    break;

				case IDCANCEL:
					EndDialog(hWndDlg, TRUE);
			        break;
			}
			break;

		default:
 			return(FALSE);	
	} /* end switch message */
	return (TRUE);

}


/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSVerifyPasswdDlgProc                                  */
/*                                                                        */
/*   Description:  For military use only                                  */
/*                                                                        */
/*       History:  John Ano   May 8, 1990 (written)                       */
/*                                                                        */
/*------------------------------------------------------------------------*/
BOOL FAR PASCAL 
WSVerifyPasswdDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{

    char szTitle[MAX_MSG_LEN];

    REFERENCE_FORMAL_PARAM(lParam);
	switch(message) {
		case WM_INITDIALOG:
			CenterPopUp(hWndDlg);
            LoadString (hInstanceWS, IDS_TITLE_PASSWORD_VERIFY, szTitle, sizeof (szTitle));
            SetWindowText(hWndDlg, szTitle);
            SetDlgItemText(hWndDlg, IDS_PW_TEXT, "Enter your password:");
            SetDlgItemText(hWndDlg, IDL_UID_LABEL, "Your User ID:");

            ShowWindow(GetDlgItem(hWndDlg,IDS_BLANK_USER), SW_HIDE);
            SendDlgItemMessage (hWndDlg, IDS_USER_ID, EM_LIMITTEXT, PW_SIZE, 0L);
            SendDlgItemMessage (hWndDlg, ID_PASSWORD, EM_LIMITTEXT, PW_SIZE, 0L);
            SendDlgItemMessage (hWndDlg, ID_BLANK,    EM_LIMITTEXT, PW_SIZE, 0L);

            SetWindowWord (GetDlgItem (hWndDlg, ID_BLANK), nOffsetToPass, 
                           GetDlgItem (hWndDlg, ID_PASSWORD));

            //force user to enter UID first
            SetDlgItemText(hWndDlg, IDS_USER_ID, custinfo.userid);
			   EnableWindow(GetDlgItem(hWndDlg, IDS_USER_ID), FALSE);
            EnableWindow(GetDlgItem(hWndDlg,IDL_UID_LABEL), FALSE);
            SetFocus(GetDlgItem(hWndDlg, ID_BLANK));
            HIGHLIGHT(GetDlgItem(hWndDlg, ID_BLANK));
            bPass1 = FALSE;
            return FALSE;
            break;

        case WM_COMMAND:
			switch(wParam) {
                case ID_HELP:
		            GetHelp(GH_TEXT,IDH_VERIFYPW);
                    break;

				case IDOK:
                    if (bPass1)  // getting user id
                        UID = 0;
					else		 // getting password
						UID = 2;

                    if (GetDlgItemText(hWndDlg, IDS_USER_ID+UID, szUsers, sizeof (szUsers)) == 0)
                    {
                        WORD    focus = IDS_USER_ID + UID;
                        if ( UID )
                            focus++;
                        SetFocus(GetDlgItem(hWndDlg,focus));
                        break;
                    }
                    //otherwise, there was something in szUsers buffer.
                    if ( UID ) // password check (5.25.90) D. Kent
                    {
                        if (bConnect)
                        {
                            BOOL status = !strcmpi(custinfo.password, szUsers);

                            if ( status )
                                EndDialog(hWndDlg, OK);
                            else        // password didn't confirm
                            {
                                //SetDest (hWndDlg, NOBEEP, MB_ICONEXCLAMATION | MB_OK, IDS_TITLE_PASSWORD_VERIFY);
                                ErrMsg (IDS_ERR_INVALID_PASSWORD);
                                SetFocus (GetDlgItem(hWndDlg, ID_BLANK));
                                HIGHLIGHT(GetDlgItem(hWndDlg, ID_BLANK));
                                break;
                            }
                        }
                        else /* can't confirm cause don't have yet */
                        {    /* confirmed in WSConnect */
                            strcpy (custinfo.password, szUsers);
                            EndDialog(hWndDlg,OK);
                        }
                    }
                    else // user id (5.25.90) D. Kent
                    /* check against win.ini contents.  */
                    {
                        if (strcmpi(custinfo.userid, szUsers))
                        { 
                            //SetDest (hWndDlg, NOBEEP, MB_ICONEXCLAMATION | MB_OK, IDS_TITLE_PASSWORD_VERIFY);
                            ErrMsg (IDS_ERR_INVALID_USERID);
                            /* Highlite the bad uid */
                            SetFocus(GetDlgItem(hWndDlg,  IDS_USER_ID));
                            HIGHLIGHT(GetDlgItem(hWndDlg, IDS_USER_ID));
                            break;
                        }

                        // if uid passed, reset flag and do it again for pw
                        // getting password,
                        // Enable the pwStuff;
                        EnableWindow(GetDlgItem(hWndDlg,IDS_USER_ID),   FALSE);
                        EnableWindow(GetDlgItem(hWndDlg,IDL_UID_LABEL), FALSE);
                        EnableWindow(GetDlgItem(hWndDlg,IDS_PW_TEXT), TRUE);
                        EnableWindow(GetDlgItem(hWndDlg,ID_PASSWORD), TRUE);
                        EnableWindow(GetDlgItem(hWndDlg,ID_BLANK),    TRUE);
                        SetFocus(GetDlgItem(hWndDlg, ID_BLANK));
                        UID = 2;
                        bPass1 = FALSE;
                    }
                    break;

              	case IDCANCEL:
                    if ( bConnect )
					    EndDialog(hWndDlg, IDCANCEL);
                    else
                    {
                        //HandleIfNewUser();
					    EndDialog(hWndDlg, FAIL);
                    }
					break;
			}
            break;
		
		default:
 			return(FALSE);	
	} /* end switch message */
	return (TRUE);
}


static int
ComputePrices(HWND hWndDlg, int nMediaIndex, char *szVersion)
{
    char szQuantity[3];
    char szTmp[MAX_PRICE_LEN];
    char szTmp2[MAX_PRICE_LEN];
    double SRPrice;
    double Discount;
    HCURSOR hCursor;
    int nRetval=OK;

    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

    if (GetDlgItemText(hWndDlg,IDS_LIB_QUANTITY,szQuantity,3) == 0)
    {
        SetDlgItemText(hWndDlg,IDS_LIB_QUANTITY,(LPSTR)"1");
        iQuantity = 1;
    }
    else
    {
        iQuantity=atoi(szQuantity);
    }

    BusyBoxWithIcon(hInstanceWS, hWndDlg, IDS_MSG_TOTALING, IDS_BUSY_CALC_ICON);

    /* get prices */
    QueryMediaPrices(nMediaIndex, CURLIBFILE, szTmp, szTmp2);
    SRPrice = atof(szTmp) ;
    WSPrice = atof(szTmp2);

    ftoa(SRPrice,szTmp,2);
    SetDlgItemText(hWndDlg, IDS_SR_PRICE, (LPSTR)szTmp);
    ftoa(WSPrice,szTmp,2);
    SetDlgItemText(hWndDlg, IDS_WS_PRICE, (LPSTR)szTmp);

    /* do discount and tax computations */
    Discount = SRPrice - WSPrice;
    ftoa(Discount,szTmp,2);
    SetDlgItemText(hWndDlg, IDS_DISCOUNT_AMT, (LPSTR) szTmp);

    /* shipping defaults */
    if (GetShippingCosts() != OK)
        END(FAIL)

    if (Shipping == 0)
        lstrcpy(szTmp,"N/A");
    else
        ftoa(Shipping,szTmp,2);
    SetDlgItemText(hWndDlg, IDS_SHIPPING_CHARGE, (LPSTR)szTmp);

    if (custinfo.whichCard == AMERICAN_EXPRESS)
        SurCharge = SURCHARGE((WSPrice * iQuantity) + Shipping);
    else
        SurCharge = 0.0;
    ftoa(SurCharge,szTmp,2);
    SetDlgItemText(hWndDlg, IDS_SURCHARGE, (LPSTR)szTmp);

    if (isWashington(custinfo.mailing.state))
        Taxes = TAXES((WSPrice * iQuantity) + Shipping + SurCharge);
    else
        Taxes = 0.0;
    ftoa(Taxes,szTmp,2);
    SetDlgItemText(hWndDlg, IDS_TAX, (LPSTR)szTmp);

    Total = (WSPrice * iQuantity) + SurCharge + Shipping + Taxes;
    ftoa(Total,szTmp,2);
    SetDlgItemText(hWndDlg, IDS_TOTAL, (LPSTR)szTmp);

    END(OK)

    end:
    prevQuantity = iQuantity;
    BusyClear(FALSE);
    EnableWindow(hWndDlg,TRUE); // just in case
    SetCursor(hCursor);
    return nRetval;
}

static int
GetShippingCosts()
{
    BOOL nRetval=OK;
    HCURSOR hCursor;
    ORDER Order;

    /* shipping defaults */
    Shipping = 0;

	hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

    if (NOT bConnect)
        END(OK)

    if (SendOrderStruct(&Order) != OK)
    {
        ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
        END(FAIL)
    }

    if (Send_Message(MSG_QUERY_SHIPPING_COSTS,ShippingCosts) != OK)
    {
        ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
        END(FAIL)
    }

    /* fetch the shipping cost from the currently selected courier */
    if (ShippingCosts[custinfo.whichShip].valid)
        Shipping = ShippingCosts[custinfo.whichShip].cost / 100;

    end:
	SetCursor(hCursor);
    return nRetval;
}

/*************************************************************\
 ** FillDlgItem (hWnd, nIDget, nIDput, padChar)
 **
 ** ARGUMENTS:      HWND    hWndGet;
 **                 WORD    hWndPut;
 **                 char    padChar;
 **
 ** DESCRIPTION:    fill the hWndPut window with the padChar
 **                 for as long as the hWndGet text is.
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **
 ** RETURNS:        none
 **
 ** HISTORY:        Lauren Bricker  June 12, 1990 (written)
\*************************************************************/

static void
FillDlgItem (HWND hWndGet, HWND hWndPut, char padChar)
{
    char    buffer [MAX_MSG_LEN];
    int     count;
    LONG    position;

    count = GetWindowText (hWndGet, buffer, sizeof (buffer));
    position = SendMessage (hWndGet, EM_GETSEL, 0, 0L);

    strnset (buffer, padChar, count);

    SetWindowText (hWndPut, buffer);
    SendMessage (hWndPut, EM_SETSEL, 0, position);
}

/*************************************************************\
 ** WSPasswordEditProc
 **
 ** ARGUMENTS:  HWND     hWnd
 **             unsigned message
 **             WORD     wParam
 **             LONG     lParam
 **
 ** DESCRIPTION:    super class of the edit proc for password 
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **
 ** RETURNS:        TRUE if message is handled, FALSE if not
 **
 ** HISTORY:        Lauren Bricker  June 12, 1990 (written)
\*************************************************************/

long FAR PASCAL WSPasswordEditProc (HWND hWnd, unsigned message, WORD wParam, LONG lParam)
{
    HWND PassWnd = GetWindowWord (hWnd, nOffsetToPass);

    if (  (message != WM_SETTEXT) &&
          (message != WM_SETFOCUS) &&
         ((message != WM_CHAR) || (wParam  != VK_BACK) || (lParam != 0xFFFFFFFF)) )
    {
        if ( PassWnd && IsWindow (PassWnd) ) // Note this assumes that the
                                             // hWndExtra stuff is initialized 
											// to 0 
            SendMessage (PassWnd, message, wParam, lParam);
    }

    switch (message) 
    {
        case WM_CHAR:
            if ( isprint (wParam) || (wParam == VK_DELETE) || (wParam == VK_BACK) ) 
            {
                FillDlgItem (PassWnd, hWnd, PADDING); 
                return TRUE;
            }
            else
                return CallWindowProc(lpprocEditWnd, hWnd, message, wParam, lParam);
            break;

        default:
            return CallWindowProc(lpprocEditWnd, hWnd, message, wParam, lParam);

    } /* end switch */
}

/*************************************************************\
 ** SendOrderStruct
 **
 ** ARGUMENTS:
 **  ORDER *Order
 **
 ** DESCRIPTION:
 **  Check whether Order can be placed.
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **  Order structure; fills it up with relevant infol.
 **
 ** RETURNS:
 **  FAIL if not successful, otherwise returns 
 **    ACK : if all is OK.
 **    NAK : if not approved
 **    EOT : if unable to check
 **    ??? : if out-of-stock
 **
 ** HISTORY:   Doug Kent  June 21, 1990 (written)
\*************************************************************/
static WORD
SendOrderStruct(ORDER *Order)
{
    CREDIT_CARD_STRUCT cc;

    memset(Order,0,sizeof(ORDER));

    Order->prodType = LIB;

    //lstrcpy(Order->libName,CURLIB);
    Order->libID = GetFileNum(CURLIBFILE);

    //lstrcpy(Order->vendorName,CURPUB);
    Order->pubID = GetFileNum(CURPUBFILE);

    Order->mediaType = (char)nMediaType;
    lstrcpy(Order->Version,szVersion);
    Order->CourierType = (char)custinfo.whichShip;

    Order->ShipAddress = custinfo.mailing;

    /* credit card stuff */
    UnencryptCardNumber(custinfo.whichCard,&cc);
    lstrcpy(Order->ccName,cc.name);
    lstrcpy(Order->ccNum,cc.number);
    wsprintf((LPSTR)Order->ccExpDate, (LPSTR)"%02d%02d",
               cc.exp_month, cc.exp_year);
    Order->ccType = custinfo.whichCard;

    Order->Quantity = iQuantity;
    Order->priceTotal = (long)(Total*100);
    Order->priceEach = (long)(WSPrice*100);
    Order->taxamount = (long)(Taxes*100);
    Order->shipping = (long)(Shipping*100);
    Order->surcharge = (long)(SurCharge*100);

    if (Send_Message(MSG_SET_CUR_LIB_ORDER,Order) != OK)
        return FAIL;

    return OK;
}


static WORD
QueryOrderCredit(ORDER *Order)
{
    BOOL bRetval;
    BOOL stat;

    stat = Send_Message(MSG_QUERY_LIB_ORDER_OK,&bRetval);

    if (stat != OK)
        return FAIL;

    return bRetval;
}

static WORD
QueryOrderInStock(void)
{
    BOOL bRetval;
    int stat;

    stat = Send_Message(MSG_QUERY_INSTOCK,&bRetval);

    if (stat != OK)
        return FAIL;

    return bRetval;
}

static WORD
SubmitOrder(HWND hWndDlg)
{
    ORDER Order;
    DWORD OrderNum;
    WORD nRetval=OK;
    HCURSOR hSaveCurs;

	hSaveCurs = SetCursor(LoadCursor(NULL, IDC_WAIT));

    //bEditHasFocus = FALSE; // to disable EN_KILLFOCUS msg during busybox
    BusyBoxWithIcon(hInstanceWS,hWndDlg,IDS_MSG_VALIDATE_CREDIT, IDS_BUSY_CREDIT_ICON);

    if (SendOrderStruct(&Order) != OK)
    {
        ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
        END(FAIL)
    }

    switch(QueryOrderInStock())
    {
        case ACK:
        break;

        case CTRLX:
            SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_APPNAME);
	   		if (ErrMsg(IDS_MSG_ORDER_OUTOFSTOCK) == IDNO)
	            END(FAIL)
        break;

        case SOH: // No longer available
			ErrMsg(IDS_ERR_ORDER_UNAVAILABLE);
	        END(FAIL)

        case FAIL:
            ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
            END(FAIL)
    }

    switch(QueryOrderCredit(&Order))
    {
        default:
        case FAIL:
            ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
            END(FAIL)

        case ACK:
            // all is groovy, submit the order...
            break;

        case NAK:
            // not accepted
			ErrMsg(IDS_ERR_CREDIT_NOT_ACCEPT);
            END(FAIL)

        case EOT:
            // unable to check
            SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_APPNAME);
			if (ErrMsg(IDS_ERR_CONDITIONAL_ORDER) == IDNO)
	            END(FAIL)
        break;
    }

    if (Send_Message(MSG_COMMIT_LIB_ORDER,&OrderNum) != OK)
    {
        ErrMsg(IDS_ERR_ORDER_UNAVAILABLE);
        END(FAIL)
    }

	 ErrMsg(IDS_MSG_ORDER_PLACED,OrderNum);

    end:
    BusyClear(TRUE);
    EnableWindow(hWndDlg,TRUE); // just in case
	 SetCursor(hSaveCurs);
    return nRetval;
}

int
TranslateMType(HWND hList, int Index, int *MType)
{
    int i;
    char szMStr[MAX_STR_LEN+1];

    SendMessage(hList, LB_GETTEXT, Index, (LONG)(LPSTR)szMStr);

    for (i = 4; i-- ;)
    {
        if (!lstrcmp(MediaStrings[i],szMStr))
        {
            *MType = i;
            return OK;
        }
    }
    return FAIL;
}


