/*************************************************************\
 ** FILE:  subscr.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **  Subscription fee dialog box
 **
 ** HISTORY:   
 **  Bill King   9/19/90        Created
 **
\*************************************************************/

#include "nowin.h"
#include <windows.h>
#include <dos.h>
#include <io.h>
#include <stdio.h>
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

static int NEAR PASCAL FillDlg(HWND hWndDlg);
static int SubmitNewUserOrder(void);
static int GetShippingCosts(HWND hWndDlg);
static double Price;
static double Total;
static double Taxes;
static double Shipping;
static double SurCharge;
static int nMediaType,nMIndex;
static MSGTYPE msgPrice,msgQueryOK;
extern BOOL bRenewingUser; // from WSConnect()
extern BOOL bUpgradingUser; // from WSConnect()
extern BOOL bShipRenewal; // from WSConnect()

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSSubscriptionDlgProc                                  */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL 
WSSubscriptionDlgProc (hWndDlg, message, wParam, lParam) 
	HWND		hWndDlg;
	unsigned	message;
	WORD		wParam;
	LONG		lParam;
{
	FARPROC lpproc;
    BOOL    stat;
    static DWORD DPrice;
    HWND    hWndLB;
    static WORD ShipSave;

	switch(message) {
		case WM_INITDIALOG:
      {
			if (bRenewingUser AND (NOT bUpgradingUser))
				msgPrice = MSG_QUERY_RENEWAL_PRICE;
			else  // Demo users pay full subscription price
				msgPrice = MSG_QUERY_SUBSCRIPTION_PRICE;

/* Difference between these messages is whether to check local credit or not*/
			if (bRenewingUser)
				msgQueryOK = MSG_QUERY_RENEWAL_OK; //Check Renewers local credit
			else
				msgQueryOK = MSG_QUERY_NEWUSER_OK; //No local credit to check

         if (Send_Message(msgPrice,&DPrice) != OK)
         {
             ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
             ErrMsg (IDS_ERR_BAD_NEW_ACCOUNT);
             EndDialog(hWndDlg,FAIL);
			    return FALSE;
         }
         Price = ((double) DPrice) / 100.00;

         ShipSave = custinfo.whichShip;
         custinfo.whichShip = IDB_USPS - COURIER_ID_BASE;

         if (GetShippingCosts(hWndDlg) != OK)
			{
            EndDialog(hWndDlg,FAIL);
				return FALSE;
			}

         /* fill media list box */
         nMIndex = 0;
         hWndLB=GetDlgItem(hWndDlg, IDL_MEDIA);
         SendMessage(hWndLB, LB_ADDSTRING, 0, (LONG)(LPSTR)szMEDIA(2));
         SendMessage(hWndLB, LB_ADDSTRING, 0, (LONG)(LPSTR)szMEDIA(0));
	      SendMessage(hWndLB, LB_SETCURSEL, 0, 0L);
         /* convert to normalized mediatype (11.1.90) D. Kent */
         TranslateMType(hWndLB,nMIndex,&nMediaType);

			CenterPopUp(hWndDlg);
			return FALSE;
      }
		case WM_COMMAND:
			switch(wParam) 
         {
                case ID_HELP:
		            GetHelp(GH_TEXT,IDH_ORDERWS);
                    break;

            	case IDOK:
#if 0
                    if ( NOT ValidShippingInfo() )
                    {
	                    lpproc=MakeProcInstance( (FARPROC)WSShippingInfoDlgProc, hInstanceWS);
	                    bReturn = DialogBox(hInstanceWS, (LPSTR)"SHIPPINGINFO", hWndDlg, lpproc);
	                    FreeProcInstance(lpproc);
                        if ( !bReturn )
                        {
                            // ???? ENABLE(TRUE);
                            break;
                        }
                    }
#endif
                    // Are we shipping user a package and is the customer
                    // int'l? If so tell them shipping is not computed
                    if (((bRenewingUser AND bShipRenewal) OR
                       NOT bRenewingUser OR
                       bUpgradingUser) AND
                       (custinfo.mailing.whichCountry+COUNTRY_ID_BASE == IDB_OTHER))
                    {
                           SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_OKCANCEL, IDS_TITLE_APPNAME);
                           if (ErrMsg(IDS_MSG_NOSHIPPING) == IDCANCEL)
                               break;
                    }
#if 0
                    if ( !custinfo.card[custinfo.whichCard].valid )
                    {
                        SetDest (hWndDlg, NOBEEP, MB_YESNO| MB_ICONQUESTION, IDS_TITLE_APPNAME);
                        if ( ErrMsg (IDS_ERR_INVALID_BILLING) == IDNO )
                        {
                            ENABLE(TRUE);
                            break;
                        }

                        lpproc = MakeProcInstance ((FARPROC)WSVerifyPasswdDlgProc, hInstanceWS) ;
                        bReturn = DialogBox (hInstanceWS, "NEWUSER", hWndDlg, lpproc);
                        FreeProcInstance (lpproc);
                        if ( !bReturn )
                        {
                            ENABLE(TRUE);
                            break;
                        }

                        lpproc = MakeProcInstance ((FARPROC)WSBillingInfoDlgProc, hInstanceWS) ;
                        bReturn = DialogBox (hInstanceWS, "BILLINGINFO", hWndDlg, lpproc);
                        FreeProcInstance (lpproc);
                        if ( !bReturn )
                        {
                            ENABLE(TRUE);
                            break;
                        }
                    }

#endif
                    /* get money */
                    BusyBoxWithIcon(hInstanceWS,hWndDlg,IDS_MSG_VALIDATE_CREDIT, IDS_BUSY_CREDIT_ICON);
                    stat = SubmitNewUserOrder();
                    BusyClear(FALSE);

                    switch (stat)
                    {
                        case NAK:
                            // not accepted
                            ErrMsg (IDS_ERR_CREDIT_NOT_ACCEPT);
                            ErrMsg (IDS_ERR_BAD_NEW_ACCOUNT);
        					       EndDialog(hWndDlg, FAIL);
                            break;

                        case FAIL:
                        default:
                            ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
                            ErrMsg (IDS_ERR_BAD_NEW_ACCOUNT);
        				          EndDialog(hWndDlg, FAIL);
                            break;

                        case EOT:
                            // unable to check
                            SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_APPNAME);
			                   if (ErrMsg(IDS_ERR_CONDITIONAL_ORDER) == IDNO)
                            {
	                               EndDialog(hWndDlg,FAIL);
                                   break;
                            }
        					       EndDialog(hWndDlg, OK);
                            break;

                        case ACK:
        					       EndDialog(hWndDlg, OK);
                            break;
                    }

			       break;

                case IDB_SELECT_BILLING:
#if 1
                    GetInfo(hWndDlg,IDM_BILLING_INFO);
#else
	                lpproc=MakeProcInstance((FARPROC)WSSelectBillingDlgProc, hInstanceWS);
	                DialogBox(hInstanceWS, (LPSTR)"SELECTBILLING", hWndDlg, lpproc);
	                FreeProcInstance(lpproc);
#endif
                    if (GetShippingCosts(hWndDlg) != OK)
                        EndDialog(hWndDlg,FAIL);
                    return TRUE;

                case IDB_SHIPPING_INFO:
	                lpproc=MakeProcInstance((FARPROC)WSShippingInfoDlgProc, hInstanceWS);
	                DialogBox(hInstanceWS, (LPSTR)"SHIPPINGINFO", hWndDlg, lpproc);
	                FreeProcInstance(lpproc);
                    if (GetShippingCosts(hWndDlg) != OK)
                        EndDialog(hWndDlg,FAIL);
                    return TRUE;

                case IDL_MEDIA:
		            switch(HIWORD(lParam))
                    {
                        case LBN_SELCHANGE:
                            nMIndex = QueryListSel(GetDlgItem(hWndDlg,IDL_MEDIA));
                            TranslateMType(GetDlgItem(hWndDlg,IDL_MEDIA),nMIndex,&nMediaType);
                            break;
                        default:
                            return FALSE;
                    }
                    break;
				case IDCANCEL:
					EndDialog(hWndDlg, IDCANCEL);
			        break;
			   break;
			}

        case WM_DESTROY:
            custinfo.whichShip = ShipSave;
            return(FALSE);
		default:
 			return(FALSE);	
	} /* end switch message */
	return (TRUE);

}

/*************************************************************\
 ** GetShippingCosts
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
 ** HISTORY:   Doug Kent  September 27, 1990 (written)
\*************************************************************/
static int
GetShippingCosts(HWND hWndDlg)
{
    BOOL nRetval;
    char szTmp[MAX_STR_LEN];
    ORDER Order;
    HCURSOR hCursor;

	 hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    memset(&Order,0,sizeof(ORDER));

    Order.prodType = WS;
    Order.vendorID = WS_VEND_ID;

    if (bUpgradingUser)
         Order.libID = WS_LIBID_UPGRADE;
    else if (bRenewingUser)
         Order.libID = WS_LIBID_RENEWAL;
    else
         Order.libID = WS_LIBID_NEW;

    Order.CourierType = (char)custinfo.whichShip;
    Order.ShipAddress = custinfo.mailing;
    lstrcpy(Order.Version,"EXE");
    Order.Quantity = 1;
    Order.mediaType = (char)nMediaType;

    if (Send_Message(MSG_SET_NEW_USER_ORDER,&Order) != OK)
    {
        ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
        END(FAIL)
    }

    /* shipping defaults */
    Shipping = 0;
    lstrcpy(szTmp,"N/A");

    if (Send_Message(MSG_QUERY_SHIPPING_COSTS,ShippingCosts) != OK)
    {
        ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
        END(FAIL)
    }

    /* fetch the shipping cost from the currently selected courier */
    if (ShippingCosts[custinfo.whichShip].valid)
    {
        Shipping = ShippingCosts[custinfo.whichShip].cost / 100;
        ftoa(Shipping,szTmp,2);
    }

    nRetval = FillDlg(hWndDlg);

    end:
	SetCursor(hCursor);
    return nRetval;
}

static int
SubmitNewUserOrder()
{
    CREDIT_CARD_STRUCT cc;
    BOOL stat;
    ORDER Order;
    HCURSOR hCursor;
    int nRetval=OK;

	hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    memset(&Order,0,sizeof(ORDER));

    Order.prodType = WS;
    Order.vendorID = WS_VEND_ID;
    if (bUpgradingUser)
      Order.libID = WS_LIBID_UPGRADE;
    else if (bRenewingUser)
      Order.libID = WS_LIBID_RENEWAL;
    else
      Order.libID = WS_LIBID_NEW;
    Order.CourierType = (char)custinfo.whichShip;
    Order.ShipAddress = custinfo.mailing;
    lstrcpy(Order.Version,"EXE");

    /* credit card stuff */
    UnencryptCardNumber(custinfo.whichCard,&cc);
    lstrcpy(Order.ccName,cc.name);
    lstrcpy(Order.ccNum,cc.number);

    wsprintf((LPSTR)Order.ccExpDate, (LPSTR)"%02d%02d",
               cc.exp_month, cc.exp_year);
    Order.ccType = custinfo.whichCard;

    Order.Quantity = 1;
    Order.mediaType = (char) nMediaType;
    Order.priceEach = (long) (Price * 100);
    Order.priceTotal = (long) (Total * 100);
    Order.taxamount = (long) (Taxes * 100);
    Order.shipping = (long) (Shipping * 100);
    Order.surcharge = (long) (SurCharge * 100);

    if (Send_Message(MSG_SET_NEW_USER_ORDER,&Order) != OK)
    {
        ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
        END(FAIL)
    }

    if ((stat = Send_Message(msgQueryOK,&nRetval)) != OK)
    {
        ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
        END(FAIL)
    }

    end:

	SetCursor(hCursor);
    return nRetval;
}

static int NEAR PASCAL
FillDlg(HWND hWndDlg)
{
    char szTmp[MAX_PRICE_LEN];

    /* fill in price */
    ftoa(Price,szTmp,2);
    SetDlgItemText(hWndDlg, IDS_PRICE, (LPSTR)szTmp);

    /* fetch the shipping cost from the currently selected courier */
    if (ShippingCosts[custinfo.whichShip].valid)
    {
        Shipping = ShippingCosts[custinfo.whichShip].cost / 100;
        ftoa(Shipping,szTmp,2);
    }
    SetDlgItemText(hWndDlg, IDS_SHIPPING_CHARGE, (LPSTR)szTmp);

    /* do AMEX computations */
    if (custinfo.whichCard == AMERICAN_EXPRESS)
        SurCharge = SURCHARGE(Price + Shipping);
    else
        SurCharge = 0;
    ftoa(SurCharge,szTmp,2);
    SetDlgItemText(hWndDlg, IDS_SURCHARGE, (LPSTR)szTmp);

    /* do tax computations */
    if (isWashington(custinfo.mailing.state))
        Taxes = TAXES(Price + SurCharge + Shipping);
    else
        Taxes = 0.0;
    ftoa(Taxes,szTmp,2);
    SetDlgItemText(hWndDlg, IDS_TAX, (LPSTR)szTmp);


    Total = Price + SurCharge + Shipping + Taxes;
    ftoa(Total,szTmp,2);
    SetDlgItemText(hWndDlg, IDS_TOTAL, (LPSTR)szTmp);

    return OK;
}
