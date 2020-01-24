/*************************************************************\
 ** FILE:  info.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION: All the stuff to handle the info dialogs
 **
 ** HISTORY:   
 **   Lauren Bricker  May 28, 1990 (split from order.c)
 **
\*************************************************************/


#include <windows.h>
#include <memory.h>
#include "ws.h"
#include "misc.h"
#include "order.h"
#include "help.h"
#include "rc_symb.h"
#include "custinfo.h"
#include "info.h"
#include "settings.h"
#include "error.h"
#include "pref.h"
#include "comm.h"
#include "billing.h"

static BOOL GetCustInfo (HWND hDlg, WORD which);
static BOOL SaveCustInfo (HWND hDlg, WORD which);

static WORD tmpShip;          /* In shipping info, how the user wants to ship */
static WORD tmpCountry;       /* In customer and shipping info, where are they from */
static BOOL bChangePW;
extern UPS1, UPS2, USPS;

#define CUSTOMER    0x01
#define MAILING     0x02

/*************************************************************\
 ** GetInfo
 **
 ** ARGUMENTS:  HWND    hDlg
 **             WORD    wWhichDialog
 **
 **
 ** DESCRIPTION: new user dialog handling procedure
 **  Every case saves changed information to the win.ini.
 **
 ** ASSUMES:   if wWhichDialog == ALL, run thru every setup dialog
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  TRUE if successful, FALSE if not.
 **
 ** HISTORY:   John Ano  May 2, 1990 (written)
               beng - pounded out call to UpdateDatabase Dialog
\*************************************************************/
BOOL 
GetInfo(HWND hWndDlg, WORD wWhichDialog) 
{
    FARPROC lpproc;
    BOOL bRetVal=FALSE ;
    int nRetval;
    int iRet;

    //SetDest (hWndDlg, NOBEEP, MB_OK, IDS_TITLE_APPNAME);
    switch(wWhichDialog)
    {
        case ALL:  // if ALL is selected, we fall through each dialog in order.
        case IDM_PASSWORD:
       	    lpproc=MakeProcInstance((FARPROC)WSNewUserDlgProc, hInstanceWS);
            if (wWhichDialog > ALL_SAUF_PASSWORD)
                bChangePW = TRUE;
	        iRet=DialogBox(hInstanceWS, (LPSTR)"NEWUSER", hWndWS, lpproc);
            switch(iRet) 
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
            FreeProcInstance(lpproc);

            if (wWhichDialog > ALL_SAUF_PASSWORD)
                break;

        case ALL_SAUF_PASSWORD:  // if selected, we fall through each dialog in order.
        case IDM_CUSTOMER_INFO:
            lpproc=MakeProcInstance((FARPROC)WSCustomerInfoDlgProc,hInstanceWS) ;
            iRet=DialogBox(hInstanceWS,(LPSTR)"CUSTOMERINFO",hWndDlg,lpproc) ;
            FreeProcInstance(lpproc) ;
            if ( bNewUser && !iRet ) // Mandatory information for first time user
                return FALSE;
            if (wWhichDialog > ALL_SAUF_PASSWORD)
                break;

        case IDM_SHIPPING_INFO:
            lpproc=MakeProcInstance((FARPROC)WSShippingInfoDlgProc,hInstanceWS) ;
            iRet=DialogBox(hInstanceWS,(LPSTR)"SHIPPINGINFO",hWndDlg,lpproc) ;
            FreeProcInstance(lpproc) ;
            if ( bNewUser && !iRet ) // Mandatory information for first time user
                return FALSE;
            if (wWhichDialog > ALL_SAUF_PASSWORD)
                break;

        case IDM_BILLING_INFO:
            if (wWhichDialog > ALL_SAUF_PASSWORD) // if user wants to change, enter password first
            {

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
            }
            else  // this is a first-time user, give 'em the riot act
            {
				ErrMsg(IDS_ERR_CREDIT_SECURE);
            }
            // falling through means that the user either wanted to change info and passed verify
            // or this is a first time user who needs to go through this dialog and really wants to!
            lpproc=MakeProcInstance((FARPROC)WSBillingInfoDlgProc,hInstanceWS) ;
            iRet=DialogBox(hInstanceWS,(LPSTR)"BILLINGINFO",hWndDlg,lpproc) ;
            FreeProcInstance(lpproc) ;
            if ( bNewUser && !iRet )   // Mandatory information for first time user
                return FALSE;
            if (wWhichDialog > ALL_SAUF_PASSWORD)
                break;

        case IDM_COMM:
            lpproc=MakeProcInstance((FARPROC)WSCommInfoDlgProc,hInstanceWS) ;
            iRet=DialogBox(hInstanceWS, (LPSTR)"DLDSETTINGS",hWndDlg, lpproc) ;
            FreeProcInstance(lpproc);
            if ( bNewUser && !iRet ) // Mandatory information for first time user
                return FALSE;
            break;

        case IDM_LIB_MANAGEMENT:
            lpproc=MakeProcInstance((FARPROC)WSLibManagerDlgProc,hInstanceWS) ;
            iRet=DialogBox(hInstanceWS,(LPSTR)"LIBMAN",hWndDlg,lpproc) ;
            FreeProcInstance(lpproc) ;
            break;

#if 0
        case IDM_UPDATE_DB:
            lpproc = MakeProcInstance((FARPROC)WSUpdateDBDlgProc, hInstanceWS) ;
            iRet = DialogBox(hInstanceWS, (LPSTR)"UPDATEDB", hWndDlg, lpproc) ;
            FreeProcInstance(lpproc);
            break;
#endif

        default:
            return FALSE;
    }
  return(TRUE); 
}

/*************************************************************\
 ** GetCustInfo()
 **
 ** ARGUMENTS:      HWND    hDlg;
 **                 WORD    which;
 **
 ** DESCRIPTION:    Get the customer information if which = CUSTOMER
 **                 Get the mailing infomation if which = MAILING
 **
 ** ASSUMES:        custinfo is a global
 **
 ** RETURNS:        TRUE if successful, FALSE if not.
 **
 ** HISTORY:        Lauren Bricker  May 14, 1990 (written)
\*************************************************************/

static BOOL
GetCustInfo (HWND hDlg, WORD which)
{
    PTR_ADDRESS tmpAddress;
    PTR_PHONE   tmpPhone;

    SendDlgItemMessage (hDlg, ID_NAME, EM_LIMITTEXT, MAXLEN_NAME, 0L);
    SendDlgItemMessage (hDlg, ID_COMPANY, EM_LIMITTEXT, MAXLEN_NAME, 0L);
    SendDlgItemMessage (hDlg, ID_ADDRESS, EM_LIMITTEXT, MAXLEN_ADDRESS, 0L);
    SendDlgItemMessage (hDlg, IDS_CITY, EM_LIMITTEXT, MAXLEN_CITY, 0L);
    SendDlgItemMessage (hDlg, ID_STATE, EM_LIMITTEXT, MAXLEN_STATE, 0L);
    SendDlgItemMessage (hDlg, ID_COUNTRY, EM_LIMITTEXT, MAXLEN_COUNTRY, 0L);
    SendDlgItemMessage (hDlg, ID_POSTAL, EM_LIMITTEXT, MAXLEN_POSTAL, 0L);

    switch (which)
    {
        case CUSTOMER:
            tmpAddress = &(custinfo.customer);
            SetDlgItemText (hDlg, ID_NAME,      tmpAddress->name);
            SetDlgItemText (hDlg, ID_COMPANY,   tmpAddress->company);
            SetDlgItemText (hDlg, ID_ADDRESS,   tmpAddress->street);
            SetDlgItemText (hDlg, IDS_CITY,     tmpAddress->city);
            SetDlgItemText (hDlg, ID_STATE,     tmpAddress->state);
            SetDlgItemText (hDlg, ID_POSTAL,    tmpAddress->postal_code);
            SetDlgItemText (hDlg, ID_COUNTRY,   tmpAddress->country);
            tmpCountry = tmpAddress->whichCountry + COUNTRY_ID_BASE;
			CheckRadioButton (hDlg, IDB_US, IDB_OTHER, tmpCountry);
            tmpPhone = &(custinfo.customer.phone);
            SetDlgItemText (hDlg, ID_AREA,      tmpPhone->area);
            SetDlgItemText (hDlg, ID_PHONE,     tmpPhone->number);
            SetDlgItemText (hDlg, ID_EXTENSION, tmpPhone->ext);
            break;

        case MAILING:
            if ( bNewUser && !ValidShippingInfo () )
                                   /* use customer as default shipping info */
                tmpAddress = &(custinfo.customer);
            else
                tmpAddress = &(custinfo.mailing);
            SetDlgItemText (hDlg, ID_NAME,      tmpAddress->name);
            SetDlgItemText (hDlg, ID_COMPANY,   tmpAddress->company);
            SetDlgItemText (hDlg, ID_ADDRESS,   tmpAddress->street);
            SetDlgItemText (hDlg, IDS_CITY,     tmpAddress->city);
            SetDlgItemText (hDlg, ID_STATE,     tmpAddress->state);
            SetDlgItemText (hDlg, ID_POSTAL,    tmpAddress->postal_code);
            SetDlgItemText (hDlg, ID_COUNTRY,   tmpAddress->country);
            tmpCountry = tmpAddress->whichCountry + COUNTRY_ID_BASE;
			CheckRadioButton (hDlg, IDB_US, IDB_OTHER, tmpCountry);

            if ( bNewUser )         /* use customer info a default shipping info */
                tmpPhone = &(custinfo.customer.phone);
            else
                tmpPhone = &(custinfo.mailing.phone);
            SetDlgItemText (hDlg, ID_AREA,      tmpPhone->area);
            SetDlgItemText (hDlg, ID_PHONE,     tmpPhone->number);
            SetDlgItemText (hDlg, ID_EXTENSION, tmpPhone->ext);
            tmpShip = custinfo.whichShip + COURIER_ID_BASE;
			CheckRadioButton (hDlg, IDB_1_2DAY, IDB_USPS, tmpShip);
            break;

        default:
            return FALSE;
            break;
    }

    if ( tmpCountry != IDB_OTHER )
        EnableWindow (GetDlgItem (hDlg, ID_COUNTRY), FALSE);

    return TRUE;
}


/*************************************************************\
 ** SaveCustInfo()
 **
 ** ARGUMENTS:      HWND    hDlg;
 **                 WORD    which;
 **
 ** DESCRIPTION:    Save the customer information if which = CUSTOMER
 **                 Save the mailing infomation if which = MAILING
 **                 then save the custinfo struct to wherever
 **
 ** ASSUMES:        custinfo is a global
 **
 ** MODIFIES:       custinfo
 **
 ** RETURNS:        TRUE if successful, FALSE if not.
 **
 ** HISTORY:        Lauren Bricker  May 14, 1990 (written)
\*************************************************************/

static BOOL
SaveCustInfo (HWND hDlg, WORD which)
{
    ADDRESS 	tmpAddress;
    PTR_PHONE   tmpPhone = &(tmpAddress.phone);
    BOOL        status;
    int         flag, title;

    memset(&tmpAddress,0,sizeof(ADDRESS));

    switch (which)
    {
        case CUSTOMER:
            GetDlgItemText (hDlg, ID_NAME, tmpAddress.name, sizeof (tmpAddress.name));
            GetDlgItemText (hDlg, ID_COMPANY, tmpAddress.company, sizeof (tmpAddress.company));
            GetDlgItemText (hDlg, ID_ADDRESS, tmpAddress.street, sizeof (tmpAddress.street));
            GetDlgItemText (hDlg, IDS_CITY, tmpAddress.city, sizeof (tmpAddress.city));
            GetDlgItemText (hDlg, ID_STATE, tmpAddress.state, sizeof (tmpAddress.state));
            GetDlgItemText (hDlg, ID_POSTAL, tmpAddress.postal_code, sizeof (tmpAddress.postal_code));
            GetDlgItemText (hDlg, ID_COUNTRY, tmpAddress.country, sizeof (tmpAddress.country));
            tmpAddress.whichCountry = tmpCountry - COUNTRY_ID_BASE;
            GetDlgItemText (hDlg, ID_AREA, tmpPhone->area, sizeof (tmpPhone->area));
            GetDlgItemText (hDlg, ID_PHONE, tmpPhone->number, sizeof (tmpPhone->number));
            GetDlgItemText (hDlg, ID_EXTENSION, tmpPhone->ext, sizeof (tmpPhone->ext));
            flag = WRITE_CUSTINFO;
            title = IDS_TITLE_CUSTOMER;
            break;

        case MAILING:
            GetDlgItemText (hDlg, ID_NAME, tmpAddress.name, sizeof (tmpAddress.name));
            GetDlgItemText (hDlg, ID_COMPANY, tmpAddress.company, sizeof (tmpAddress.company));
            GetDlgItemText (hDlg, ID_ADDRESS, tmpAddress.street, sizeof (tmpAddress.street));
            GetDlgItemText (hDlg, IDS_CITY, tmpAddress.city, sizeof (tmpAddress.city));
            GetDlgItemText (hDlg, ID_STATE, tmpAddress.state, sizeof (tmpAddress.state));
            GetDlgItemText (hDlg, ID_POSTAL, tmpAddress.postal_code, sizeof (tmpAddress.postal_code));
            GetDlgItemText (hDlg, ID_COUNTRY, tmpAddress.country, sizeof (tmpAddress.country));
            tmpAddress.whichCountry = tmpCountry - COUNTRY_ID_BASE;
            GetDlgItemText (hDlg, ID_AREA, tmpPhone->area, sizeof (tmpPhone->area));
            GetDlgItemText (hDlg, ID_PHONE, tmpPhone->number, sizeof (tmpPhone->number));
            GetDlgItemText (hDlg, ID_EXTENSION, tmpPhone->ext, sizeof (tmpPhone->ext));
            flag = WRITE_MAILINFO;
            title = IDS_TITLE_SHIPPING;
            break;
        default:
            return FALSE;
            break;
    }
    
    /* If any of this stuff is missing, its an error */
    if ( (tmpAddress.name[0]    == '\0') ||
         (tmpAddress.street[0]     == '\0') ||
         (tmpAddress.city[0]        == '\0') ||
         (tmpAddress.state[0]       == '\0') ||
         (tmpAddress.postal_code[0] == '\0') ||
         (tmpPhone->area[0]          == '\0') ||
         (tmpPhone->number[0]        == '\0') ||
         ((tmpAddress.whichCountry  == (IDB_OTHER - COUNTRY_ID_BASE)) &&
          (tmpAddress.country[0]    == '\0')) )
    {
        //SetDest (hDlg, NOBEEP, MB_OK, title);
        ErrMsg (IDS_ERR_NEED_INFO);
        return FALSE;
    }

	// Everythings cool, copy it into custinfo struct for saving
	switch (which)
	{
		case CUSTOMER:
			lmemcpy ((LPSTR)&custinfo.customer, (LPSTR)&tmpAddress, sizeof (ADDRESS));
			break;
		case MAILING:
			lmemcpy ((LPSTR)&custinfo.mailing, (LPSTR)&tmpAddress, sizeof (ADDRESS));
            custinfo.whichShip = tmpShip - COURIER_ID_BASE;
			break;
	}
    status = WriteCustInfo(flag);

    return status;
}

/*************************************************************\
 ** ValidShippingInfo
 **
 ** ARGUMENTS:      none
 **
 ** ASSUMES:        custinfo is a global
 **
 ** RETURNS:        TRUE if there is valid shipping information, 
 **                 FALSE if not.
 **
 ** HISTORY:        Lauren Bricker  May 30, 1990 (written)
\*************************************************************/
BOOL
ValidShippingInfo ()
{
    PTR_ADDRESS tmpAddress;
    PTR_PHONE   tmpPhone;

    tmpPhone = &(custinfo.mailing.phone);
    tmpAddress = &(custinfo.mailing);

    return ( !((tmpAddress->name[0]    == '\0') ||
               (tmpAddress->street[0]     == '\0') ||
               (tmpAddress->city[0]        == '\0') ||
               (tmpAddress->state[0]       == '\0') ||
               (tmpAddress->postal_code[0] == '\0') ||
               (tmpPhone->area[0]          == '\0') ||
               (tmpPhone->number[0]        == '\0') ||
               ((tmpAddress->whichCountry  == (IDB_OTHER - COUNTRY_ID_BASE)) &&
                (tmpAddress->country[0]    == '\0'))) );
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSCustomerInfoDlgProc                                     */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL 
WSCustomerInfoDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    BOOL        status;

    REFERENCE_FORMAL_PARAM (lParam);
	switch ( message )
    {
		case WM_INITDIALOG:
			CenterPopUp (hWndDlg);
            GetCustInfo (hWndDlg, CUSTOMER);
            return TRUE;

		case WM_COMMAND:
			switch ( wParam )
            {
	            case IDB_US:
	            case IDB_CANADA:
	            case IDB_OTHER:
                    tmpCountry = wParam;
                    CheckRadioButton (hWndDlg, IDB_US, IDB_OTHER,
                                      wParam);
                    if ( tmpCountry != IDB_OTHER )
                    {
                        EnableWindow (GetDlgItem (hWndDlg, ID_COUNTRY), FALSE);
                        custinfo.customer.country[0] = '\0';
                        SetDlgItemText (hWndDlg, ID_COUNTRY, custinfo.customer.country);
                    }
                    else
                        EnableWindow (GetDlgItem (hWndDlg, ID_COUNTRY), TRUE);
                    return TRUE;
                    break;

                case ID_HELP:
		            GetHelp(GH_TEXT,IDH_CUSTINFO);
                    break;

				case IDOK:
                    status = SaveCustInfo(hWndDlg, CUSTOMER);

                    if (status AND bConnect)
                        if (BBSChangeCustInfo() != OK)
                            /** error msg is sent **/
					        EndDialog(hWndDlg, FALSE);
                        
                    if ( status )
                        EndDialog (hWndDlg, status);
                    break;

				case IDCANCEL:
					EndDialog(hWndDlg, FALSE);
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
/*      Function:  WSShippingInfoDlgProc                                     */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL 
WSShippingInfoDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    BOOL        status;
    int i;
    int fastest;

#define USA     (IDB_US - COUNTRY_ID_BASE)

    REFERENCE_FORMAL_PARAM(lParam);

   	switch ( message )
    {
		case WM_INITDIALOG:

            /*
             * Disable shipping options that are not
             * available for this product
             */
            for (i = 0; i < MAX_NUM_SERVICES; i++)
                EnableWindow (GetDlgItem (hWndDlg, IDB_1_2DAY + i), TRUE);
            fastest = 0;

            if (bConnect)
            {
                /* 1-2 day shipping valid for USA only */
                if (custinfo.mailing.whichCountry != USA)
                    ShippingCosts[0].valid = FALSE;

                // Ship by fastest courier valid
                for (i = MAX_NUM_SERVICES - 1; i >= 0; i--)
                {
                    if (NOT ShippingCosts[i].valid)
                        EnableWindow (GetDlgItem (hWndDlg, IDB_1_2DAY + i), FALSE);
                    else 
                        fastest = i;
                }
            }
                
            CheckRadioButton (hWndDlg, IDB_1_2DAY, IDB_USPS,
                fastest + COURIER_ID_BASE);
			CenterPopUp (hWndDlg);
            GetCustInfo(hWndDlg, MAILING);
            return TRUE;

		case WM_COMMAND:
			switch ( wParam )
            {
	            case IDB_US:
	            case IDB_CANADA:
	            case IDB_OTHER:
                    tmpCountry = wParam;
                    CheckRadioButton (hWndDlg, IDB_US, IDB_OTHER, wParam);
                    if ( tmpCountry != IDB_OTHER )
                    {   
                        EnableWindow (GetDlgItem (hWndDlg, ID_COUNTRY), FALSE);
                        custinfo.mailing.country[0] = '\0';
                        SetDlgItemText (hWndDlg, ID_COUNTRY, custinfo.mailing.country);
                    }
                    else
                        EnableWindow (GetDlgItem (hWndDlg, ID_COUNTRY), TRUE);
                    return TRUE;
                    break;

	            case IDB_1_2DAY:
	            case IDB_2_3DAY:
	            case IDB_USPS:
                    tmpShip = wParam;
                    CheckRadioButton (hWndDlg, IDB_1_2DAY, IDB_USPS, wParam);
                    return TRUE;
                    break;

                case ID_HELP:
		            GetHelp(GH_TEXT,IDH_SHIPINFO);
                    break;

				case IDOK:
                    status = SaveCustInfo (hWndDlg, MAILING);
                    if ( status )
                        EndDialog (hWndDlg, status);
                    break;

				case IDCANCEL:
					EndDialog(hWndDlg, FALSE);
					break;

                default:
                   return(FALSE);
			}
			break;
		
		default:
 			return(FALSE);	
	} /* end switch message */
	return (TRUE);

}
