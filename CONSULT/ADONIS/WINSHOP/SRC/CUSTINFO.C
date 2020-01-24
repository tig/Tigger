/*************************************************************\
 ** FILE:  custinfo.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:    functions to read and write customer info
 **                 from the file where they are stored.
 **
 ** NOTE:           See the bottom of this file for the order
 **                 the info is stored in the file. 
 **
 ** HISTORY:   
 **                 Lauren Bricker  May 11, 1990 (created)
 **
\*************************************************************/


#include <windows.h>
#include <winundoc.h>
#include <string.h>
#include "ws.h"
#include "rc_symb.h"
#include "custinfo.h"
#include "file.h"
#include "billing.h"
#include "comm.h"
#include "messages.h"
#include "error.h"
#include "misc.h"
#include "ksearch.h"


/* GLOBALS */

CUSTINFO_STRUCT custinfo;

/* Bogus win.ini strings to confuse the user ... */

static char *szCardName[]  = {"top",                /* c0name */
                              "install",            /* c1name */
                              "mode",               /* c2name */
                              "blam",               /* c3name */
                              "conversion"};        /* c4name */

static char *szCardNumber[] = {"left",              /* c0number */
                               "installed",         /* c1number */
                               "where",             /* c2number */
                               "it",                /* c3number */
                               "comment"};          /* c4number */


static char *szMonth[] = {"right",                  /* c0month */
                          "getShopper",             /* c1month */
                          "number",                 /* c2month */
                          "lauren",                 /* c3month */
                          "dialog"};                /* c4month */

static char *szYear[] = {"bottom",                  /* c0year */
                         "application",             /* c1year */
                         "letter",                  /* c2year */
                         "shopper",                 /* c3year */
                         "single"};                 /* c4year */


/*-------------------------------------------------------------------------*/
/*                           L O C A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
static LPSTR NEAR PASCAL EncryptString (LPSTR unencrypted, LPSTR encrypted,
                                        unsigned length);
static LPSTR NEAR PASCAL UnencryptString (LPSTR encrypted, LPSTR unencrypted,
                                          unsigned length);
static int NEAR PASCAL EncryptNumber (int unencrypted);
static int NEAR PASCAL UnencryptNumber (int encrypted);
static BOOL SetAltCache(BOOL bReset);

/*************************************************************\
 ** ResetCustInfo
 **
 ** ARGUMENTS:      BOOL	bAll
 **
 ** DESCRIPTION:    blank out all the information in win.ini based on bAll flag
 **  				
 ** 				if bAll is true, blank out all of it
 **  				if bAll if false, blank out only billing info.
 **
 ** ASSUMES:        custinfo is a global
 **
 ** MODIFIES:       custinfo
 **
 ** RETURNS:        TRUE if successful, FALSE if not.
 **
 ** HISTORY:        Lauren Bricker  June 7, 1990 (written)
 **  				Lauren Bricker  Oct  2, 1990 (Only reset billing info...)
**             beng 4/29/91   use shopper/thumb as altcache if exists
\*************************************************************/

BOOL FAR PASCAL
ResetCustInfo (BOOL bAll)
{
    HANDLE      hCursor;

    hCursor = SetCursor (LoadCursor (NULL, IDC_WAIT));

	if ( bAll ) 					 // This stuff resets everthing!!!!
	{
		
		char	phonePrefix[MAXLEN_PREFIX + 1], phoneSuffix[MAXLEN_SUFFIX + 1],
                modemInitStr[MAXLEN_MODEMINITSTR+1],userModemStr[MAXLEN_MODEMINITSTR+1];
		WORD	comm_port, baud, dial_type;

		// Save the comm port stuff to restore later !!!!!!!!!!!!!!!!!! 
		comm_port = custinfo.comm_port;
		baud      = custinfo.baud;
		dial_type = custinfo.dial_type;
		lstrcpy (phonePrefix, custinfo.phonePrefix);
		lstrcpy (phoneSuffix, custinfo.phoneSuffix);
      lstrcpy(modemInitStr,custinfo.modemInitString);
      lstrcpy(userModemStr,custinfo.userModemString);

	    /* reset the entire structure */
    	memset (&custinfo, 0, sizeof (custinfo));

    	/* set defaults */
    	custinfo.customer.whichCountry = IDB_US - COUNTRY_ID_BASE;
    	custinfo.mailing.whichCountry  = IDB_US - COUNTRY_ID_BASE;
    	custinfo.whichCard   = MASTERCARD;
    	custinfo.whichShip   = IDB_USPS - COURIER_ID_BASE;
    	custinfo.cacheSize   = 2;
    	custinfo.useAltCache = TRUE;
      custinfo.searchParam = ALL_TYPES;

      if (SetAltCache(TRUE) == FALSE)
         return FALSE;

	   strcpy(custinfo.downloadDir, szExePath);

		// Use the old comm settings!!!
		lstrcpy (custinfo.phonePrefix, phonePrefix);
		lstrcpy (custinfo.phoneSuffix, phoneSuffix);
    	custinfo.comm_port   = comm_port;
    	custinfo.baud        = baud;
    	custinfo.dial_type   = dial_type;
      lstrcpy(custinfo.modemInitString,modemInitStr);
      lstrcpy(custinfo.userModemString,userModemStr);

	}
	else
	    /* reset the billing info array */
    	memset (custinfo.card, 0, sizeof (custinfo.card));

    //ShowCursor (FALSE);
    SetCursor (hCursor);

    return TRUE;
}


/*************************************************************\
 ** ReadCustInfo  
 **
 ** ARGUMENTS:      none
 **
 ** DESCRIPTION:    Read the customer info from the file into 
 **                 custinfo structure
 **
 ** ASSUMES:        custinfo struct is a global
 **
 ** MODIFIES:       custinfo struct
 **
 ** RETURNS:        TRUE if successful, FALSE if not.
 **
 ** HISTORY:        Lauren Bricker  May 11, 1990 (written)
\*************************************************************/

BOOL FAR PASCAL 
ReadCustInfo ()
{

    PTR_ADDRESS      tmpAddress;
    PTR_CREDIT_CARD  tmpCredit;
    PTR_PHONE        tmpPhone;
    HANDLE           hCursor;
    int              ii;

    char sztmp[MAXLEN_SEARCHPARAM + 1];
    char szParam[MAXLEN_SEARCHPARAM + 1];

    hCursor = SetCursor (LoadCursor (NULL, IDC_WAIT));
    //ShowCursor (TRUE);

    /* initialize the structure */
    memset (&custinfo, 0, sizeof (custinfo));

    GetProfileString (szWinIniLabel, PS_MODEM_INIT_STRING,DEF_MODEM_INIT_STR, 
                      custinfo.modemInitString, MAXLEN_MODEMINITSTR);

    GetProfileString (szWinIniLabel, PS_USER_INIT_STRING,DEF_USER_INIT_STR, 
                      custinfo.userModemString, MAXLEN_MODEMINITSTR);

    GetProfileString (szWinIniLabel, PS_USERID, "", 
                      custinfo.userid, sizeof (custinfo.userid));

    GetProfileString (szWinIniLabel, PS_PREFIX, "", 
                      custinfo.phonePrefix, sizeof (custinfo.phonePrefix));

    GetProfileString (szWinIniLabel, PS_SUFFIX, "", 
                      custinfo.phoneSuffix, sizeof (custinfo.phoneSuffix));

    custinfo.whichCard   = GetProfileInt (szWinIniLabel, PS_WHICHCARD, MASTERCARD);
    custinfo.whichShip   = GetProfileInt (szWinIniLabel, PS_SHIPPING, IDB_USPS - COURIER_ID_BASE);
    custinfo.comm_port   = GetProfileInt (szWinIniLabel, PS_COMMPORT, ID_COM1 - PORT_ID_BASE);
    custinfo.baud        = GetProfileInt (szWinIniLabel, PS_BAUD, ID_2400 - BAUD_ID_BASE);
    custinfo.dial_type   = GetProfileInt (szWinIniLabel, PS_DIALTYPE, ID_TONE - DIAL_ID_BASE);
    custinfo.cacheSize   = GetProfileInt (szWinIniLabel, PS_CACHESIZE, 2);

    /* In version 1.2, we always use the Alternate Thumbnail Directory ..pt */
    /*custinfo.useAltCache = GetProfileInt (szWinIniLabel, PS_USEALTCACHE, TRUE);*/
    custinfo.useAltCache = TRUE;

    GetProfileString (szWinIniLabel, PS_SEARCHPARAM,
                     (LPSTR)ltoa(ALL_TYPES, sztmp, 16),
                     szParam, sizeof(szParam));
    custinfo.searchParam = strtol(szParam, NULL, 16);

    if (SetAltCache(TRUE) == FALSE)
       return FALSE;

    if ( GetProfileString (szWinIniLabel, PS_DOWNLOADDIR, "", 
                      custinfo.downloadDir, sizeof (custinfo.downloadDir)) == 0)
        strcpy(custinfo.downloadDir, szExePath);


    tmpAddress = &(custinfo.customer);
    GetProfileString (szWinIniLabel, PS_CNAME, "", 
                      tmpAddress->name, sizeof (tmpAddress->name));
    GetProfileString (szWinIniLabel, PS_CCOMPANY, "", 
                      tmpAddress->company, sizeof (tmpAddress->company));
    GetProfileString (szWinIniLabel, PS_CADDRESS, "", 
                      tmpAddress->street, sizeof (tmpAddress->street));
    GetProfileString (szWinIniLabel, PS_CCITY, "", 
                      tmpAddress->city, sizeof (tmpAddress->city));
    GetProfileString (szWinIniLabel, PS_CSTATE, "", 
                      tmpAddress->state, sizeof (tmpAddress->state));
    GetProfileString (szWinIniLabel, PS_CCOUNTRY, "", 
                      tmpAddress->country, sizeof (tmpAddress->country));
    GetProfileString (szWinIniLabel, PS_CZIP, "", 
                      tmpAddress->postal_code, 
                      sizeof (tmpAddress->postal_code));
    tmpAddress->whichCountry = GetProfileInt (szWinIniLabel, PS_CWHICHCOUNTRY, IDB_US - COUNTRY_ID_BASE);

    tmpPhone = &(custinfo.customer.phone);
    GetProfileString (szWinIniLabel, PS_CAREA, "", 
                      tmpPhone->area, sizeof (tmpPhone->area));
    GetProfileString (szWinIniLabel, PS_CNUMBER, "", 
                      tmpPhone->number, sizeof (tmpPhone->number));
    GetProfileString (szWinIniLabel, PS_CEXT, "", 
                      tmpPhone->ext, sizeof (tmpPhone->ext));

    tmpAddress = &(custinfo.mailing);
    GetProfileString (szWinIniLabel, PS_MNAME, "", 
                      tmpAddress->name, sizeof (tmpAddress->name));
    GetProfileString (szWinIniLabel, PS_MCOMPANY, "", 
                     tmpAddress->company, sizeof (tmpAddress->company));
    GetProfileString (szWinIniLabel, PS_MADDRESS, "", 
                      tmpAddress->street, sizeof (tmpAddress->street));
    GetProfileString (szWinIniLabel, PS_MCITY, "", 
                      tmpAddress->city, sizeof (tmpAddress->city));
    GetProfileString (szWinIniLabel, PS_MSTATE, "", 
                      tmpAddress->state, sizeof (tmpAddress->state));
    GetProfileString (szWinIniLabel, PS_MCOUNTRY, "", 
                      tmpAddress->country, sizeof (tmpAddress->country));
    GetProfileString (szWinIniLabel, PS_MZIP, "", 
                      tmpAddress->postal_code, sizeof (tmpAddress->postal_code));
    tmpAddress->whichCountry = GetProfileInt (szWinIniLabel, PS_MWHICHCOUNTRY, IDB_US - COUNTRY_ID_BASE);

    tmpPhone = &(custinfo.mailing.phone);
    GetProfileString (szWinIniLabel, PS_MAREA, "",
                      tmpPhone->area, sizeof (tmpPhone->area));
    GetProfileString (szWinIniLabel, PS_MNUMBER, "",
                      tmpPhone->number, sizeof (tmpPhone->number));
    GetProfileString (szWinIniLabel, PS_MEXT, "",
                      tmpPhone->ext, sizeof (tmpPhone->ext));


    /* get everything but the corporate acct info */
    for ( ii = 0; ii < NUMBER_OF_CARDS; ii++ )
    {
        tmpCredit = &(custinfo.card[ii]);

        GetProfileString (szWinIniLabel, szCardName[ii], "", tmpCredit->name, 
                          sizeof (tmpCredit->name));
        GetProfileString (szWinIniLabel, szCardNumber[ii], "", tmpCredit->number, 
                          sizeof (tmpCredit->number));
        tmpCredit->exp_month = GetProfileInt (szWinIniLabel, szMonth[ii], 0);
        tmpCredit->exp_year  = GetProfileInt (szWinIniLabel, szYear[ii], 0);
    }

    SetCursor (hCursor);

    return TRUE;
}


/*************************************************************\
 ** WriteCustInfo
 **
 ** ARGUMENTS:     int flags -- use defines in custinfo.h
 **
 ** DESCRIPTION:   Writes the customer info to win.ini from
 **                the custinfo structure
 **
 ** ASSUMES:       custinfo struct is a global
 **
 ** MODIFIES:      custinfo
 **
 ** RETURNS:       TRUE if successful, FALSE if not.
 **
 ** HISTORY:       Lauren Bricker  May 11, 1990 (written)
\*************************************************************/

BOOL FAR PASCAL 
WriteCustInfo (int flags)
{

    PTR_ADDRESS      tmpAddress;
    PTR_CREDIT_CARD  tmpCredit;
    PTR_PHONE        tmpPhone;
    char             buffer[MAXLEN_CCNUMBER + 1];
    HANDLE           hCursor;
    int              ii;

    hCursor = SetCursor (LoadCursor (NULL, IDC_WAIT));
    //ShowCursor (TRUE);

    if ( flags & WRITE_WHICHCARD )
    {
        WriteProfileString (szWinIniLabel, PS_WHICHCARD, 
                            itoa (custinfo.whichCard, buffer, 10));
    }

    if ( flags & WRITE_USERID )
    {
        WriteProfileString (szWinIniLabel, PS_USERID, custinfo.userid);
    }

    if ( flags & WRITE_CUSTCOMM )
    {
        WriteProfileString (szWinIniLabel, PS_COMMPORT,
                            itoa (custinfo.comm_port, buffer, 10));
        WriteProfileString (szWinIniLabel, PS_BAUD, 
                            itoa (custinfo.baud, buffer, 10));
        WriteProfileString (szWinIniLabel, PS_DIALTYPE, 
                            itoa (custinfo.dial_type, buffer, 10));
        WriteProfileString (szWinIniLabel, PS_PREFIX, custinfo.phonePrefix);
        WriteProfileString (szWinIniLabel, PS_SUFFIX, custinfo.phoneSuffix);
        WriteProfileString (szWinIniLabel, PS_MODEM_INIT_STRING, custinfo.modemInitString);
        WriteProfileString (szWinIniLabel, PS_USER_INIT_STRING, custinfo.userModemString);
    }

    if ( flags & WRITE_USECACHE )
    {
        WriteProfileString (szWinIniLabel, PS_USEALTCACHE, 
                            itoa (custinfo.useAltCache, buffer, 10));
        WriteProfileString (szWinIniLabel, PS_ALTCACHE, 
                        custinfo.altcache);
    }

    if ( flags & WRITE_DOWNDIR )
        WriteProfileString (szWinIniLabel, PS_DOWNLOADDIR, 
                        custinfo.downloadDir);


    if ( flags & WRITE_CUSTCACHE )
        WriteProfileString (szWinIniLabel, PS_CACHESIZE, 
                            itoa (custinfo.cacheSize, buffer, 10));

    if ( flags & WRITE_SEARCHPARAM )
    {
        char szParam[MAXLEN_SEARCHPARAM + 1];
        WriteProfileString ((LPSTR)szWinIniLabel,
                            (LPSTR)PS_SEARCHPARAM,
                            (LPSTR)ltoa(custinfo.searchParam, szParam, 16));
    }

    if ( flags & WRITE_CUSTINFO )
    {
        tmpAddress = &(custinfo.customer);
        WriteProfileString (szWinIniLabel, PS_CNAME, tmpAddress->name);
        WriteProfileString (szWinIniLabel, PS_CCOMPANY, tmpAddress->company);
        WriteProfileString (szWinIniLabel, PS_CADDRESS, tmpAddress->street);
        WriteProfileString (szWinIniLabel, PS_CCITY, tmpAddress->city);
        WriteProfileString (szWinIniLabel, PS_CSTATE, tmpAddress->state);
        WriteProfileString (szWinIniLabel, PS_CCOUNTRY, tmpAddress->country);
        WriteProfileString (szWinIniLabel, PS_CZIP, tmpAddress->postal_code);
        WriteProfileString (szWinIniLabel, PS_CWHICHCOUNTRY, 
                            itoa (tmpAddress->whichCountry, buffer, 10));

        tmpPhone = &(custinfo.customer.phone);
        WriteProfileString (szWinIniLabel, PS_CAREA, tmpPhone->area);
        WriteProfileString (szWinIniLabel, PS_CNUMBER, tmpPhone->number);
        WriteProfileString (szWinIniLabel, PS_CEXT, tmpPhone->ext);
    }

    if ( flags & WRITE_MAILINFO )
    {
        tmpAddress = &(custinfo.mailing);
        WriteProfileString (szWinIniLabel, PS_MNAME, tmpAddress->name);
        WriteProfileString (szWinIniLabel, PS_MCOMPANY, tmpAddress->company);
        WriteProfileString (szWinIniLabel, PS_MADDRESS, tmpAddress->street);
        WriteProfileString (szWinIniLabel, PS_MCITY, tmpAddress->city);
        WriteProfileString (szWinIniLabel, PS_MSTATE, tmpAddress->state);
        WriteProfileString (szWinIniLabel, PS_MCOUNTRY, tmpAddress->country);
        WriteProfileString (szWinIniLabel, PS_MZIP, tmpAddress->postal_code);
        WriteProfileString (szWinIniLabel, PS_MWHICHCOUNTRY, 
                            itoa (tmpAddress->whichCountry, buffer, 10));

        tmpPhone = &(custinfo.mailing.phone);
        WriteProfileString (szWinIniLabel, PS_MAREA, tmpPhone->area);
        WriteProfileString (szWinIniLabel, PS_MNUMBER, tmpPhone->number);
        WriteProfileString (szWinIniLabel, PS_MEXT, tmpPhone->ext);

        WriteProfileString (szWinIniLabel, PS_SHIPPING, 
                            itoa (custinfo.whichShip, buffer, 10));
    }

        
    if ( flags & WRITE_CARDINFO )
    {

        /* save everything but the corporate acct info */
        for ( ii = 0; ii < NUMBER_OF_CARDS; ii++ )
        {
            tmpCredit = &(custinfo.card[ii]);

            WriteProfileString (szWinIniLabel, szCardName[ii], tmpCredit->name);
            WriteProfileString (szWinIniLabel, szCardNumber[ii], tmpCredit->number);
            WriteProfileString (szWinIniLabel, szMonth[ii],
                               itoa(tmpCredit->exp_month, buffer, 10));
            WriteProfileString (szWinIniLabel, szYear[ii],
                               itoa(tmpCredit->exp_year, buffer, 10));
        }
    }


    SetCursor (hCursor);
    return TRUE;
}


/*************************************************************\
 ** EncryptCards
 **
 ** ARGUMENTS:   
 **
 ** DESCRIPTION: Encrypt the unencrypted card information.
 **
 ** ASSUMES:     custinfo.cards is a global
 **
 ** MODIFIES:    custinfo.cards
 **
 ** RETURNS:     TRUE if all went well, FALSE if it didn't
 **
 ** HISTORY:     Lauren Bricker  June 7, 1990 (written)
\*************************************************************/

BOOL FAR PASCAL 
EncryptCards()

{
    int              ii;
    char             buffer[MAXLEN_NAME + 1];    
    PTR_CREDIT_CARD  tmpCredit;

    if ( !bConnect && !bNewUser )
        return FALSE;

    /* encrypt everything but the corporate card. */
    for ( ii = 0; ii < NUMBER_OF_CARDS; ii++ )
    {
        tmpCredit = &(custinfo.card[ii]);

        EncryptString (tmpCredit->number,    buffer, sizeof (buffer));
        strcpy (tmpCredit->number, buffer);

        EncryptString (tmpCredit->name,      buffer, sizeof (buffer));
        strcpy (tmpCredit->name, buffer);

        tmpCredit->exp_month = EncryptNumber(tmpCredit->exp_month);
        tmpCredit->exp_year  = EncryptNumber(tmpCredit->exp_year);

    }

    return TRUE;
}


/*************************************************************\
 ** UnencryptCard
 **
 ** ARGUMENTS:  WORD    whichCard  
 **             PTR_CREDIT_CARD aCard
 **
 ** DESCRIPTION:
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Lauren Bricker  June 20, 1990 (written)
\*************************************************************/

BOOL FAR PASCAL 
UnencryptCardNumber (whichCard, aCard)
    WORD whichCard;
    PTR_CREDIT_CARD aCard;
{
    PTR_CREDIT_CARD tmpCredit  = &(custinfo.card[whichCard]);
    char            buffer[MAXLEN_CARDNUM + 1];

    UnencryptString (tmpCredit->number, buffer, sizeof (buffer));
    StripSpaces (buffer, aCard->number, sizeof (aCard->number));

    UnencryptString (tmpCredit->name, aCard->name, sizeof (aCard->name));
    aCard->exp_month = UnencryptNumber(tmpCredit->exp_month);
    aCard->exp_year  = UnencryptNumber(tmpCredit->exp_year);

    return TRUE;

}

/*************************************************************\
 ** UnencryptCards
 **
 ** ARGUMENTS:   
 **
 ** DESCRIPTION: Unencrypt the encrypted cards.
 **
 ** ASSUMES:     custinfo.cards is a global
 **
 ** MODIFIES:    custinfo.cards
 **
 ** RETURNS:     Pointer to the unencrypted string.
 **
 ** HISTORY:     Lauren Bricker  May 11, 1990 (written)
\*************************************************************/

BOOL FAR PASCAL 
UnencryptCards ()

{
    int              ii;
    char             buffer[MAXLEN_NAME + 1];    
    PTR_CREDIT_CARD  tmpCredit;

    if ( !bConnect && !bNewUser )
        return FALSE;

    /* unencrypt everything but the corporate card. */
    for ( ii = 0; ii < NUMBER_OF_CARDS; ii++ )
    {
        tmpCredit = &(custinfo.card[ii]);

        UnencryptString (tmpCredit->number,    buffer, sizeof (buffer));
        strcpy (tmpCredit->number, buffer);

        UnencryptString (tmpCredit->name,      buffer, sizeof (buffer));
        strcpy (tmpCredit->name, buffer);

        tmpCredit->exp_month = UnencryptNumber(tmpCredit->exp_month);
        tmpCredit->exp_year  = UnencryptNumber(tmpCredit->exp_year);

    }

	custinfo.card[MASTERCARD].valid       = ValidBillingInfo(MASTERCARD);
	custinfo.card[VISA].valid             = ValidBillingInfo(VISA);
	custinfo.card[AMERICAN_EXPRESS].valid = ValidBillingInfo(AMERICAN_EXPRESS);
	custinfo.card[DISCOVER].valid         = ValidBillingInfo(DISCOVER);
	custinfo.card[CORPORATE_CARD].valid   = ValidBillingInfo(CORPORATE_CARD);

    return TRUE;
}


/*************************************************************\
 ** EncryptString
 **
 ** ARGUMENTS:   LPSTR unencrypted, encrypted;
 **              int   length;
 **
 ** DESCRIPTION: Encrypt the unencrypted string.
 **
 ** ASSUMES:     encrypted is a buffer large enough to hold 
 **              the newly encrypted word
 **
 ** MODIFIES:    encrypted
 **
 ** RETURNS:     Pointer to the encrypted string.
 **
 ** HISTORY:     Lauren Bricker  May 11, 1990 (written)
\*************************************************************/

static LPSTR NEAR PASCAL 
EncryptString(unencrypted, encrypted, length)
    LPSTR       unencrypted, encrypted;
    unsigned    length;
{
    int unLen, passLen, ii, ij;

    unLen = lstrlen (unencrypted);
    passLen = lstrlen (custinfo.password);
    ij = 0;

    for ( ii = 0; (ii < unLen) && (ii < (int)length); ii++ )
    {
        encrypted[ii] = (char)(unencrypted[ii] + tolower(custinfo.password[ij]));
        ++ij;
        ij %= passLen;
    }

    encrypted[ii] = '\0';

    return encrypted;
}




/*************************************************************\
 ** UnencryptString
 **
 ** ARGUMENTS:   LPSTR encrypted, unencrypted;
 **              int   length;
 **
 ** DESCRIPTION: Unencrypt the encrypted string.
 **
 ** ASSUMES:     unencrypted is a buffer large enough to hold 
 **              the newly unencrypted word
 **
 ** MODIFIES:    unencrypted
 **
 ** RETURNS:     Pointer to the unencrypted string.
 **
 ** HISTORY:     Lauren Bricker  May 11, 1990 (written)
\*************************************************************/

static LPSTR NEAR PASCAL 
UnencryptString (encrypted, unencrypted, length)
    LPSTR       encrypted, unencrypted;
    unsigned    length;
{
    int enLen, passLen, ii, ij;

    enLen = lstrlen (encrypted);
    passLen = strlen (custinfo.password);
    ij = 0;

    for ( ii = 0; (ii < enLen) && (ii < (int)length); ii++ )
    {
        unencrypted[ii] = (char)(encrypted[ii] - tolower(custinfo.password[ij]));
        ++ij;
        ij %= passLen;
    }

    unencrypted[ii] = '\0';

    return unencrypted;
}


/*************************************************************\
 ** EncryptNumber
 **
 ** ARGUMENTS:   int unencrypted
 **
 ** DESCRIPTION: Encrypt the unencrypted number.
 **
 ** NOTE:        Will not encrypt 0
 **
 ** RETURNS:     the encrypted number.
 **
 ** HISTORY:     Lauren Bricker  June 7, 1990 (written)
\*************************************************************/

static int NEAR PASCAL 
EncryptNumber(unencrypted)
    int unencrypted;
{
    if ( unencrypted )
        return (unencrypted + tolower (custinfo.password[0]));
    else 
        return unencrypted;
}




/*************************************************************\
 ** UnencryptNumber
 **
 ** ARGUMENTS:   int encrypted;
 **
 ** DESCRIPTION: Unencrypt the encrypted number.
 **
 ** NOTE:        Will not unencrypt 0
 **
 ** RETURNS:     the unencrypted number.
 **
 ** HISTORY:     Lauren Bricker  June 7, 1990 (written)
\*************************************************************/

static int NEAR PASCAL 
UnencryptNumber (encrypted)

    int encrypted;
{
    if ( encrypted )
        return (encrypted - tolower (custinfo.password[0])); 
    else 
        return encrypted;
}


int
BBSChangeCustInfo(void)
/* assume logged on */
{
    if (Send_Message(MSG_SET_CUST_INFO,&custinfo.customer) != OK)
    {
        ErrMsg(IDS_ERR_SET_CUST_INFO);
        return FAIL;
    }

    if (Send_Message(MSG_COMMIT_CUST_INFO) != OK)
    {
        ErrMsg(IDS_ERR_SET_CUST_INFO);
        return FAIL;
    }
    return OK;
}

#ifdef EDOUT            /* use if we go back  to shopper.ini */

void GetIniFileName (LPSTR buffer, int size);
static  int     szWinIniLabel;
#define CR      '\n'
/*************************************************************\
 ** GetIniFileName
 **
 ** ARGUMENTS:  LPSTR   buffer;
 **             int     size;
 **
 ** DESCRIPTION: get the full path name of the shopper.ini file
 **
 ** ASSUMES:     there is enough room in the buffer to hold the
 **              the entire path name of shopper.ini
 **
 ** MODIFIES:    buffer
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Lauren Bricker  May 17, 1990 (written)
\*************************************************************/

void GetIniFileName (buffer, size)
    LPSTR   buffer;
    int     size;
{
    
    HANDLE  hModule;
    char    moduleName[MAX_FILE_NAME + 1];
    LPSTR   tmpBuffer;
    int     count;

    LoadString (hInstanceWS, IDS_MODULENAME, moduleName);
    hModule = GetModuleHandle (moduleName);
    GetModuleFileName (hModule, buffer, size);

    count = strlen (buffer);
    tmpBuffer = buffer + count - 1;       /* point to the end of the string */

    /* strip off the .exe part of the module name and make it .ini */
    while ( *tmpBuffer != '.' && tmpBuffer != buffer )
        tmpBuffer--;
     tmpBuffer != buffer )
        tmpBuffer ++;
    *tmpBuffer = '\0';
    strcat (tmpBuffer, "ini");

}

/* Then use the following in ReadCustInfo                                   */
/*    GetIniFileName (moduleName);                     */
/*                                                                          */
/*    szWinIniLabel = _lopen (moduleName, READ_WRITE);                      */
/*                                                                          */
/*    status =  ReadStringToChar (IniFileHandle, custinfo.userid, CR);      */

#endif

BOOL
SetAltCache(BOOL bReset)
{
   if (bReset OR
       (GetProfileString (szWinIniLabel, PS_ALTCACHE, "",
                      custinfo.altcache, sizeof (custinfo.altcache)) == 0))
   {
      // Construct a shopper/thumb directory
      strcpy(custinfo.altcache, szExePath);
    	if (NOT LoadString(hInstanceWS, IDS_THUMB_DIR, (LPSTR)(custinfo.altcache + strlen(custinfo.altcache)),
                  sizeof(custinfo.altcache)-strlen(szExePath)))
         return FALSE;

      //If SHOPPER/Thumb does not exist point altcache to A:drive

      if (NOT DirExists(custinfo.altcache))
    	   if (NOT LoadString(hInstanceWS, IDS_FLOPPY_THUMB, (LPSTR)(custinfo.altcache),
        	            sizeof(custinfo.altcache)))
            return FALSE;
      else
         custinfo.useAltCache = TRUE;  //set to true initially only if alt dir
                                       // is on harddrive
      WriteCustInfo(WRITE_USECACHE);
    }
    //else ->already set
    return TRUE;
}

