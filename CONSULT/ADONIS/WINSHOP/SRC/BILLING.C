/*************************************************************\
 ** FILE:  billing.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION: All the functions to handle the billing info dialogs
 **
 ** HISTORY:   
 **   Lauren Bricker May 28, 1990 (split from order.c)
 **
\*************************************************************/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "ws.h"
#include "help.h"
#include "rc_symb.h"
#include "custinfo.h"
#include "billing.h"
#include "misc.h"
#include "error.h"

static BOOL SaveOldBillingInfo (HWND hDlg, BOOL fromOK);
static BOOL UpdateNewBillingInfo (HWND hDlg, WORD wCardNum);
static BOOL GetCustCardInfo (HWND hDlg);
static BOOL SaveCustCardInfo (void);
static WORD DisableCards (HWND hDlg);
static BOOL Mod10Validation (LPSTR cardNumber, int count);

static CREDIT_CARD_STRUCT tmpCards[NUMBER_OF_CARDS];

/* used in SaveOldBillingInfo for checking card validity */
#define CARD_LEN        16
#define VISA_LEN        13
#define AMEX_LEN        15

#define TESTMASTER      "5499750000000007"
#define TESTVISA        "4005550000000019"

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSBillingInfoDlgProc                                   */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL 
WSBillingInfoDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    REFERENCE_FORMAL_PARAM(lParam);

	switch(message) {
		case WM_INITDIALOG:
            if ( !UnencryptCards() )
            {
                //SetDest (hWndDlg, NOBEEP, MB_OK, IDS_TITLE_BILLING);
                ErrMsg (IDS_ERR_UNENCRYPTING);
            }
			   CenterPopUp(hWndDlg);
            SendDlgItemMessage (hWndDlg, IDS_CARDOWNER, EM_LIMITTEXT, 
                                MAXLEN_NAME, 0L);
			   SendMessage(GetDlgItem(hWndDlg, custinfo.whichCard + CC_ID_BASE), BM_SETCHECK, 1, 0L);
            GetCustCardInfo (hWndDlg);
            SetFocus (GetDlgItem (hWndDlg, IDS_CARDOWNER));
            return FALSE;

		case WM_COMMAND:
			switch(wParam) {
                case IDB_MASTERCARD:
                case IDB_VISA:
                case IDB_AMEX:
                case IDB_DISCOVER:
                case IDB_CORP_ACCT:

                    if ( wParam != (custinfo.whichCard + CC_ID_BASE) )
                    {
                        HWND    hWndParam, hWndCard;

                        hWndParam = GetDlgItem (hWndDlg, wParam);
                        hWndCard  = GetDlgItem (hWndDlg, custinfo.whichCard + CC_ID_BASE);
                        SendMessage (hWndParam, BM_SETCHECK, 1, 0L);
                        SendMessage (hWndCard, BM_SETCHECK, 0, 0L);
                        UpdateNewBillingInfo (hWndDlg, wParam - CC_ID_BASE);
                        //SetFocus (GetDlgItem (hWndDlg, IDS_CARDOWNER));
                    }
                    break;

                case ID_HELP:
		            GetHelp(GH_TEXT,IDH_BILLINFO);
                    break;

                case ID_SAVE:
                    SaveOldBillingInfo (hWndDlg, FALSE);
                    break;

				case IDOK:
                    if ( SaveOldBillingInfo (hWndDlg, TRUE) )
                    {   
                        //SetDest (hWndDlg, NOBEEP, MB_OK, IDS_TITLE_BILLING);
                        if ( !tmpCards[custinfo.whichCard].valid )
                            ErrMsg (IDS_ERR_MUST_SELECT);
                        else
                        {
                            if ( SaveCustCardInfo() )
                            {
                                SetDefErr();
                                EndDialog(hWndDlg, TRUE);
                            }
                            else
                                ErrMsg (IDS_ERR_ENCRYPTING);
                        }
                    }
                    break;

				case IDCANCEL:
                    if ( !EncryptCards() )
                    {
                        //SetDest (hWndDlg, NOBEEP, MB_OK, IDS_TITLE_BILLING);
                        ErrMsg (IDS_ERR_ENCRYPTING);
                    }
					EndDialog(hWndDlg, FALSE);
					break;
			}
			break;
		
		default:
 			return(FALSE);	
	} /* end switch message */
	return (TRUE);

}

/*************************************************************\
 ** GetCustCardInfo
 **
 ** ARGUMENTS:   none
 **
 ** DESCRIPTION: Put the customer account information into the 
 **              into the tmpCards array 
 **
 ** ASSUMES:     tmpCards is a global
 **
 ** MODIFIES:    tmpCards
 **
 ** RETURNS:     TRUE if successful, FALSE if not.
 **
 ** HISTORY:     Lauren Bricker  May 11, 1990 (written)
\*************************************************************/

static BOOL
GetCustCardInfo (HWND hDlg)
{
    WORD ii;

    /* read the new information to the custinfo struct and ... */
    for ( ii = 0; ii < NUMBER_OF_CARDS; ii++ )
    {
	     tmpCards[ii].valid = custinfo.card[ii].valid;
        strcpy (tmpCards[ii].name,      custinfo.card[ii].name);
        strcpy (tmpCards[ii].number,    custinfo.card[ii].number);
        tmpCards[ii].exp_month = custinfo.card[ii].exp_month;
        tmpCards[ii].exp_year  = custinfo.card[ii].exp_year;
    }

    /* Kludge!!!, UpdateNewBillingInfo info needs something to set to 
       custinfo.whichCard, so the first time through, just set 
       the second param to custinfo.whichCard... */
    UpdateNewBillingInfo (hDlg, custinfo.whichCard);
    return TRUE;
}


/*************************************************************\
 ** StripSpaces
 **
 ** ARGUMENTS:   LPSTR   oldNumber, newNumber
 **              int     size   
 **
 ** DESCRIPTION: Strips the spaces out of oldNumber and puts
 **              the resulting string in newNumber.  Returns
 **              the number of characters copied to newNumber
 **  
 ** ASSUMES:     newNumber is a buffer big enough to hold the 
 **              new string
 **
 ** RETURNS:     TRUE if successful, FALSE if not.
 **
 ** HISTORY:     Lauren Bricker  May 21, 1990 (written)
\*************************************************************/


int StripSpaces (LPSTR oldNumber, LPSTR newNumber, int size)
{
    int     count;
    LPSTR   tmpOld, tmpNew;

    count  = 0;
    tmpOld = oldNumber;
    tmpNew = newNumber;

    while (count < size)
    {
        if (*tmpOld)
        {
            if ( (*tmpOld != ' ') && (*tmpOld != '-') )
            {
                *tmpNew++ = *tmpOld++;
                count++;
            }
            else
                tmpOld++;
        }
        else 
            break;
    }
    *tmpNew = '\0';

    return count;
}

/*************************************************************\
 ** StripCardDashes
 **
 ** ARGUMENTS:   LPSTR   ccNumber
 **
 ** DESCRIPTION: Strips the dashes out of credit card number
 **
 ** RETURNS:     nothing
 **
 ** HISTORY:     Lauren Bricker  May 16, 1991 (written)
\*************************************************************/


void StripCardDashes (LPSTR ccNumber)
{
    LPSTR   ptr = ccNumber;

    while ( *ptr )
    {
        if ( *ptr == '-' )
            *ptr = ' ';
        ptr++;
    }
}

/*************************************************************\
 ** Mod10Validation
 **
 ** ARGUMENTS:   LPSTR   cardNumber
 **              int     count;
 **
 ** DESCRIPTION: Returns TRUE if the card passes the Mod 10
 **              formula for validation.  Returns FALSE 
 **              otherwise.
 **
 ** HISTORY:     Lauren Bricker  May 21, 1990 (written)
\*************************************************************/
static BOOL
Mod10Validation (LPSTR cardNumber, int count)
{
    int     ii, total, doubleDigit;
    char    tmpDouble[MAXLEN_CCNUMBER];
    LPSTR   doublePtr;

    /* first add up every other digit starting with the rightmost */
    ii = count - 1;
    total = 0;
    while ( ii >= 0 )
    {
        total += (cardNumber[ii] - '0');
        ii -= 2;
    }

    /* now double every other digit starting with the second from 
       the rightmost, and add the digits of the resulting number */

    ii = count - 2;

    while ( ii >= 0 )
    {
        doubleDigit = (cardNumber[ii] - '0') * 2;
        itoa (doubleDigit, tmpDouble, 10);
        doublePtr = tmpDouble;
        while ( *doublePtr )
        {
            total += (*doublePtr - '0');
            doublePtr++;
        }
        ii -= 2;
    }

    if ( total % 10 )
        return FALSE;
    else
        return TRUE;
}

/*************************************************************\
 ** SaveOldBillingInfo
 **
 ** ARGUMENTS:   HWND hDlg;
 **
 ** DESCRIPTION: First checks the format of the infomation in
 **              the edit fields of the dialog box based on
 **              which account is currently selected, then
 **              saves the information in the structure based on
 **
 ** ASSUMES:     custinfo.whichCard, tmpCards are globals
 **
 ** MODIFIES:    tmpCards
 **
 ** RETURNS:     TRUE if successful, FALSE if not.
 **
 ** HISTORY:     Lauren Bricker  May 11, 1990 (written)
\*************************************************************/

#define CONDITIONAL_ERRMSG(from, message) (from ? IDNO : ErrMsg (message))

static BOOL
SaveOldBillingInfo (HWND hDlg, BOOL fromOK)
{
    WORD    cardNumber;
    int     count;
    BOOL    status, transMonth, transYear;
    CREDIT_CARD_STRUCT  aCard;
    char    tmpNumber[MAXLEN_CARDNUM + 1];
    BOOL    nRetval = TRUE;

    /* Set up for an error */
    //SetDest(hDlg, NOBEEP, MB_OK, IDS_TITLE_BILLING);

    cardNumber = custinfo.whichCard;

    GetDlgItemText (hDlg, IDS_CARDOWNER, aCard.name, sizeof (aCard.name));
    aCard.exp_month = GetDlgItemInt (hDlg, IDS_EXP_DATE_MO, &transMonth, FALSE);
    aCard.exp_year  = GetDlgItemInt (hDlg, IDS_EXP_DATE_YR, &transYear, FALSE);
    GetDlgItemText (hDlg, IDB_CARD_NUMBER, aCard.number, sizeof (aCard.number));

    /* If nothing is filled in, just go on... */
    if ( (strlen(aCard.name) == 0) AND
         (strlen(aCard.number) == 0) AND
         !transMonth AND !transYear )
    {
        tmpCards[cardNumber].valid     = FALSE;
        tmpCards[cardNumber].name[0]   = '\0';
        tmpCards[cardNumber].number[0] = '\0';
        tmpCards[cardNumber].exp_month = 0;
        tmpCards[cardNumber].exp_year  = 0;
        END(TRUE)
    }


    if ( cardNumber != CORPORATE_CARD ) // Corp acct doesn't need these fields
    {
        /* Make sure there is name text */
        if ( strlen (aCard.name) == 0 )
        {
            if ( CONDITIONAL_ERRMSG (fromOK, IDS_ERR_MISSING_NAME) == IDYES )
                END(FALSE)
            else
            {
                tmpCards[cardNumber].valid = FALSE;
                END(TRUE)
            }
        }

        /* Check the month and year for valid numbers */
        if ( !transMonth OR
            ((aCard.exp_month < 1) OR (aCard.exp_month > 12)) )
        {
            if ( CONDITIONAL_ERRMSG (fromOK, IDS_ERR_INVALID_EXPR) == IDYES )
                END(FALSE)
            else
            {
                tmpCards[cardNumber].valid = FALSE;
                END(TRUE)
            }
        }

        if ( !transYear OR (aCard.exp_year < 90) )
        {
            if ( CONDITIONAL_ERRMSG (fromOK, IDS_ERR_INVALID_EXPR) == IDYES )
                END(FALSE)
            else
            {
                tmpCards[cardNumber].valid = FALSE;
                END(TRUE)
            }
        }
    }

    /* Check the card number for validity */
    count = StripSpaces (aCard.number, tmpNumber, sizeof (tmpNumber));

    /* set ourselves up for failure */
    status = FALSE;

    /* Check the length of the card numbers, and the prefixes to make sure
       they are valid */
    switch ( custinfo.whichCard )
    {
        case MASTERCARD:
            if ( (count == CARD_LEN) AND (tmpNumber[0] == '5') AND 
                 (tmpNumber[1] >= '1') AND (tmpNumber[1] <= '5'))
                status = TRUE;
            break;

        case VISA:
            if ( ((count == CARD_LEN) OR (count == VISA_LEN)) AND
                 (tmpNumber[0] == '4') )
                status = TRUE;
            break;

        case AMERICAN_EXPRESS:
            if (  (count == AMEX_LEN) AND (tmpNumber[0] == '3') AND
                 ((tmpNumber[1] == '4') OR (tmpNumber[1] == '7')) )
                status = TRUE;
            break;

        case DISCOVER:
            if (  (count == CARD_LEN) AND ( !strncmp(tmpNumber, "6011", 4) ) )
                status = TRUE;
            break;

        case CORPORATE_CARD:
            if ( (count == CARD_LEN) AND (tmpNumber[0] == '9') )
                status = TRUE;
            break;
    }

    /* Do the MOD 10 formula for validiation */
    if ( status )
        status = Mod10Validation (tmpNumber, count);

#if !defined(INTERNAL)  /* commented out until we ship */

    /* These two card numbers will always give approval, they are used 
       for tests.  If the user types them in, make it look like an 
       invalid number */
    if ( status )
        if ( !strcmp (tmpNumber, TESTMASTER) OR !strcmp (tmpNumber, TESTVISA) )
            status = FALSE;
#endif

    if ( status )
    {
        tmpCards[cardNumber].valid = TRUE;
        strcpy (tmpCards[cardNumber].name,      aCard.name);
        strcpy (tmpCards[cardNumber].number,    aCard.number);
        tmpCards[cardNumber].exp_month = aCard.exp_month;
        tmpCards[cardNumber].exp_year  = aCard.exp_year;
    }
    else
    {
        if ( CONDITIONAL_ERRMSG (fromOK, IDS_ERR_INVALID_CARD_INFO) == IDYES )
            END(FALSE)
        else
        {
            tmpCards[cardNumber].valid = FALSE;
            END(TRUE)
        }
    }

end:
    SetDefErr();
    return nRetval;
}


/*************************************************************\
 ** UpdateNewBillingInfo
 **
 ** ARGUMENTS:   HWND hDlg;
 **              WORD wParam;
 **
 ** DESCRIPTION: Put the newly selected account information 
 **              in the dialog
 **
 ** ASSUMES:     custinfo.whichCard, tmpCards are globals
 **
 ** MODIFIES:    custinfo.whichCard
 **
 ** RETURNS:     TRUE if successful, FALSE if not.
 **
 ** HISTORY:     Lauren Bricker  May 11, 1990 (written)
\*************************************************************/

static BOOL
UpdateNewBillingInfo (HWND hDlg, WORD wCardNum)
{
    custinfo.whichCard = wCardNum;

    wCardNum = custinfo.whichCard;

    /* only give new information if there is some there */
    if ( ((wCardNum == CORPORATE_CARD) &&
          (tmpCards[wCardNum].number[0] != '\0')) ||  
         ((tmpCards[wCardNum].name[0]   != '\0') &&
          (tmpCards[wCardNum].number[0] != '\0') &&
          (tmpCards[wCardNum].exp_month)         &&
          (tmpCards[wCardNum].exp_year)) )
    {
        SetDlgItemText (hDlg, IDS_CARDOWNER,   tmpCards[wCardNum].name);
        SetDlgItemText (hDlg, IDB_CARD_NUMBER, tmpCards[wCardNum].number);
        if ( !tmpCards[wCardNum].exp_month AND !tmpCards[wCardNum].exp_year )
        {
            SetDlgItemText (hDlg, IDS_EXP_DATE_MO, "");
            SetDlgItemText (hDlg, IDS_EXP_DATE_YR, "");
        }
        else
        {
            //single digit months or years should be have leading zero
            char szTmp[3];

            wsprintf((LPSTR)szTmp, (LPSTR)"%02d", tmpCards[wCardNum].exp_month);
            SetDlgItemText (hDlg, IDS_EXP_DATE_MO, (LPSTR)szTmp);

            wsprintf((LPSTR)szTmp, (LPSTR)"%02d", tmpCards[wCardNum].exp_year);
            SetDlgItemText (hDlg, IDS_EXP_DATE_YR, (LPSTR)szTmp);
        }
    }
    else
    {
        SetDlgItemText (hDlg, IDB_CARD_NUMBER, "");
        SetDlgItemText (hDlg, IDS_EXP_DATE_MO, "");
        SetDlgItemText (hDlg, IDS_EXP_DATE_YR, "");
    }


    return TRUE;
}

/*************************************************************\
 ** SaveCustCardInfo
 **
 ** ARGUMENTS:   none
 **
 ** DESCRIPTION: Put the new customer account information 
 **              into the custinfo structure
 **
 ** ASSUMES:     tmpCards is a global and is filled in
 **
 ** RETURNS:     TRUE if successful, FALSE if not.
 **
 ** HISTORY:     Lauren Bricker  May 11, 1990 (written)
\*************************************************************/

static BOOL
SaveCustCardInfo (void)
{

    WORD ii;
    BOOL status;

    /* write the new information to the custinfo struct and ... */
    for ( ii = 0; ii < NUMBER_OF_CARDS; ii++ )
    {
	    custinfo.card[ii].valid = tmpCards[ii].valid;
        strcpy (custinfo.card[ii].name,   tmpCards[ii].name);
        strcpy (custinfo.card[ii].number, tmpCards[ii].number);
        custinfo.card[ii].exp_month = tmpCards[ii].exp_month;
        custinfo.card[ii].exp_year  = tmpCards[ii].exp_year;
    }


    if ( !EncryptCards() )
        return FALSE;

    /* write it to disk */
    status = WriteCustInfo(WRITE_CARDINFO | WRITE_WHICHCARD);

    return status;
}

/*************************************************************\
 ** DisableCards
 **
 ** ARGUMENTS:   HWND hDlg
 **
 ** DESCRIPTION: Disable the card buttons in the select billing
 **              dialog box
 **
 ** ASSUMES:     custinfo is a global and is filled in
 **
 ** RETURNS:     the number of cards that are enabled
 **
 ** HISTORY:     Lauren Bricker  May 15, 1990 (written)
\*************************************************************/

WORD DisableCards (HWND hDlg)
{
    WORD count = NUMBER_OF_CARDS;

    if ( !custinfo.card[MASTERCARD].valid )
    {
        EnableWindow (GetDlgItem (hDlg, IDB_MASTERCARD), FALSE);
        count--;
    }
    if ( !custinfo.card[VISA].valid )
    {
	    EnableWindow (GetDlgItem (hDlg, IDB_VISA), FALSE);
        count--;
    }
    if ( !custinfo.card[AMERICAN_EXPRESS].valid )
    {
	    EnableWindow (GetDlgItem (hDlg, IDB_AMEX), FALSE);
        count--;
    }
    if ( !custinfo.card[DISCOVER].valid )
    {
	    EnableWindow (GetDlgItem (hDlg, IDB_DISCOVER), FALSE);
        count--;
    }
    if ( !custinfo.card[CORPORATE_CARD].valid )
    {
	    EnableWindow (GetDlgItem (hDlg, IDB_CORP_ACCT), FALSE);
        count--;
    }

    return count;
}

#if 0
/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSSelectBillingDlgProc                                     */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL 
WSSelectBillingDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    int     cardNumber;
    HWND    hCardWnd;
    static  WORD    tmpCard;

    REFERENCE_FORMAL_PARAM(lParam);

	switch(message) {
		case WM_INITDIALOG:
                if ( !UnencryptCards() )
                {
                    //SetDest (hWndDlg, NOBEEP, MB_OK, IDS_TITLE_BILLING);
                    ErrMsg (IDS_ERR_UNENCRYPTING);
                }
				CenterPopUp(hWndDlg);
                tmpCard = custinfo.whichCard + CC_ID_BASE;
                hCardWnd = GetDlgItem(hWndDlg, tmpCard);

                if ( !DisableCards (hWndDlg) OR 
                     !IsWindowEnabled (hCardWnd) )
                     
                {
	                EnableWindow (GetDlgItem (hWndDlg, IDOK), FALSE);
                    SetFocus (GetDlgItem (hWndDlg, IDCANCEL));
                }
                else
                {
			        SendMessage(hCardWnd, BM_SETCHECK, 1, 0L);	
                    cardNumber = tmpCard - CC_ID_BASE;
                    SetDlgItemText (hWndDlg, IDS_CARDOWNER, 
                                    custinfo.card[cardNumber].name);
                    SetFocus (GetDlgItem (hWndDlg, IDOK));
                }
                return FALSE;

        case WM_COMMAND:
			switch(wParam) {
                case IDB_MASTERCARD:
                case IDB_VISA:
                case IDB_AMEX:
                case IDB_DISCOVER:
                case IDB_CORP_ACCT:
                    if ( wParam != tmpCard )
                    {
	                    EnableWindow (GetDlgItem (hWndDlg, IDOK), TRUE);
                        SendMessage (GetDlgItem (hWndDlg, wParam), BM_SETCHECK, 1, 0L);
                        SendMessage (GetDlgItem (hWndDlg, tmpCard), 
                                     BM_SETCHECK, 0, 0L);
                        cardNumber = wParam - CC_ID_BASE;
                        SetDlgItemText (hWndDlg, IDS_CARDOWNER, 
                                        custinfo.card[cardNumber].name);
                        tmpCard = wParam;
                    }
                    return TRUE;
                    break;
                case ID_HELP:
		            GetHelp(GH_TEXT,IDH_SELECTBILL);
                    break;
				case IDOK:
                    custinfo.whichCard = tmpCard - CC_ID_BASE;
                    if ( !EncryptCards() )
                    {
                        //SetDest (hWndDlg, NOBEEP, MB_OK, IDS_TITLE_BILLING);
                        ErrMsg (IDS_ERR_ENCRYPTING);
                    }
                    WriteCustInfo (WRITE_WHICHCARD);
					EndDialog(hWndDlg, TRUE);
                    return TRUE;
					break;

				case IDCANCEL:
                    if ( !EncryptCards() )
                    {
                        //SetDest (hWndDlg, NOBEEP, MB_OK, IDS_TITLE_BILLING);
                        ErrMsg (IDS_ERR_ENCRYPTING);
                    }
					EndDialog(hWndDlg, TRUE);
                    return TRUE;
					break;
			}
            break;
		
		default:
 			return(FALSE);	
	} /* end switch message */
	return (TRUE);
}
#endif


/*************************************************************\
 ** ValidBillingInfo
 **
 ** ARGUMENTS:      none
 **
 ** ASSUMES:        custinfo is a global
 **
 ** RETURNS:        TRUE if the currently selected card has valid info
 **                 FALSE if not.
 **
 ** HISTORY:        Lauren Bricker  May 31, 1990 (written)
\*************************************************************/

BOOL
ValidBillingInfo (WORD aCard)
{
    WORD    cardNumber;
    int     count;
    BOOL    status;
    char    tmpNumber[MAXLEN_CARDNUM + 1];

    cardNumber = aCard;

    if ( aCard != CORPORATE_CARD )
    {
        /* Make sure there is name text */
        if ( strlen (custinfo.card[cardNumber].name) == 0 )
            return FALSE;

        /* Check the month and year for valid numbers */
        if ( (custinfo.card[cardNumber].exp_month < 1) OR 
             (custinfo.card[cardNumber].exp_month > 12) )
                return FALSE;

        if ( custinfo.card[cardNumber].exp_year <= 90 )
                return FALSE;
    }

    /* Check the card number for validity */
    count = StripSpaces (custinfo.card[cardNumber].number, tmpNumber, 
                         sizeof (tmpNumber));

    /* set ourselves up for failure */
    status = FALSE;

    /* Check the length of the card numbers, and the prefixes to make sure
       they are valid */
    switch ( aCard )
    {
        case MASTERCARD:
            if ( (count == CARD_LEN) AND (tmpNumber[0] == '5') AND 
                 (tmpNumber[1] >= '1') AND (tmpNumber[1] <= '5'))
                status = TRUE;
            break;

        case VISA:
            if ( ((count == CARD_LEN) OR (count == VISA_LEN)) AND
                 (tmpNumber[0] == '4') )
                status = TRUE;
            break;

        case AMERICAN_EXPRESS:
            if (  (count == AMEX_LEN) AND (tmpNumber[0] == '3') AND
                 ((tmpNumber[1] == '4') OR (tmpNumber[1] == '7')) )
                status = TRUE;
            break;

        case DISCOVER:
            if (  (count == CARD_LEN) AND ( !strncmp(tmpNumber, "6011", 4) ) )
                status = TRUE;
            break;

        case CORPORATE_CARD:
            if ( (count == CARD_LEN) AND (tmpNumber[0] == '9') )
                status = TRUE;
            break;
    }

#if !defined(INTERNAL)  /* commented out until we ship */

    /* These two card numbers will always give approval, they are used 
       for tests.  If the user types them in, make it look like an 
       invalid number */
    if ( status )
        if ( !strcmp (tmpNumber, TESTMASTER) OR !strcmp (tmpNumber, TESTVISA) )
            status = FALSE;
#endif

    /* Do the MOD 10 formula for validiation */
    if ( status )
        status = Mod10Validation (tmpNumber, count);

    return status;
}



