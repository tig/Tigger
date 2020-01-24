#include <windows.h>
#include "winundoc.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "ws.h"
#include "settings.h"
#include "error.h"
#include "messages.h"  /* Status bar strings */
#include "help.h"

#include "file.h"
#include "misc.h"
#include "viewbox.h"
#include "rc_symb.h"
#include "custinfo.h"
#include "libmgr.h"
#include "download.h"

char szWindowShopper[]="Window Shopper";
char szBaud[]="Baud";
char szPort[]="Port";
char  szUser[]="User";

WORD CacheOpts[14] = {256,
                      512,
                      1024,
                      1536,
                      2048,
                      2560,
                      3072,
                      4196,
                      5120,
                      7168,
                      10240,
                      15360,
                      20480,
                      41960};

char *szCacheOptions[] = {"256 K",
                                 "512 K",
                                 "1.0 Mb",
                                 "1.5 Mb",
                                 "2.0 Mb",
                                 "2.5 Mb",
                                 "3.0 Mb",
                                 "4.0 Mb",
                                 "5.0 Mb",
                                 "7.0 Mb",
                                 "10.0 Mb",
                                 "15.0 Mb",
                                 "20.0 Mb", 
                                 "40.0 Mb+" } ;

static	WORD	nB, nP;
static	char	szUserName[80];
static	WORD	aBaudIDs[]={ID_300, ID_600, ID_1200, ID_2400, ID_4800, ID_9600};


/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  GetSettings                                            */
/*                                                                        */
/*   Description:   Gets and validates the port setting, baud rate        */
/*						  and user name.  If any of this information is         */
/*                  missing or invalid puts up the settings dialog box,   */
/*                  which forces user to enter valid data.                */
/*                                                                        */
/*       History:  Dec 20, 1989  Initially created.  [lds]                */
/*                                                                        */
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  VerifyLogon                                            */
/*                                                                        */
/*   Description:  Queries user if ready to connect to bulletin board.    */
/*                                                                        */
/*       History:  12-30-89  Initially created. [lds]                     */
/*                                                                        */
/*------------------------------------------------------------------------*/
WORD FAR PASCAL 
VerifyLogon(char *szUser) 
{
    SetDest (ES_ACTIVEWND, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_APPNAME);
	return(ErrMsg(IDS_MSG_CONNECT_TO_WS));
}

/*************************************************************\
 ** WSCommInfoDlgProc
 **
 ** ARGUMENTS:   HWND    hDlg
 **              unsigned message
 **              WORD    wParam
 **              LONG    lParam
 **
 ** DESCRIPTION: Communications dialog handling procedure
 **
 ** RETURNS:     TRUE if successful, FALSE if not.
 **
 ** HISTORY:     
\*************************************************************/

LONG FAR PASCAL 
WSCommInfoDlgProc (hDlg, message, wParam, lParam)
    HWND     hDlg;
    unsigned message;
    WORD     wParam;
    LONG     lParam;
{
    static WORD tmpBaud, tmpDial, tmpCom;

    REFERENCE_FORMAL_PARAM(lParam);
    switch ( message ) 
    {
        case WM_INITDIALOG:
		    CheckRadioButton (hDlg, ID_300, ID_9600, custinfo.baud + BAUD_ID_BASE);
			CheckRadioButton (hDlg, ID_TONE, ID_PULSE, custinfo.dial_type + DIAL_ID_BASE);
			CheckRadioButton (hDlg, ID_COM1, ID_COM4, custinfo.comm_port + PORT_ID_BASE);
			tmpBaud = custinfo.baud + BAUD_ID_BASE;
            tmpDial = custinfo.dial_type + DIAL_ID_BASE;
            tmpCom  = custinfo.comm_port + PORT_ID_BASE;
            SendDlgItemMessage (hDlg, ID_PPREFIX, EM_LIMITTEXT, MAXLEN_PREFIX, 0L);
            SetDlgItemText (hDlg, ID_PPREFIX, custinfo.phonePrefix);
            SendDlgItemMessage (hDlg, ID_PSUFFIX, EM_LIMITTEXT, MAXLEN_SUFFIX, 0L);
            SetDlgItemText (hDlg, ID_PSUFFIX, custinfo.phoneSuffix);
            SetFocus (GetDlgItem (hDlg, IDOK));
			return FALSE;
		    break;
        case WM_COMMAND:
		    switch ( wParam )
            {
                case ID_300:
				case ID_1200:
				case ID_2400:
				case ID_4800:
				case ID_9600:
                    tmpBaud = wParam;
				    CheckRadioButton (hDlg, ID_300, ID_9600, wParam);
					return TRUE;
					break;

				case ID_COM1:
				case ID_COM2:
				case ID_COM3:
				case ID_COM4:
                    tmpCom = wParam;
				    CheckRadioButton (hDlg, ID_COM1, ID_COM4, wParam);
					return TRUE;
				    break;

				case ID_TONE:
				case ID_PULSE:
                    tmpDial = wParam;
				    CheckRadioButton (hDlg, ID_TONE, ID_PULSE, wParam);
					return TRUE;
				    break;

                case ID_HELP:
		            GetHelp(GH_TEXT,IDH_COMM);
                    return TRUE;
                    break;

				case IDOK:
				    custinfo.comm_port = tmpCom - PORT_ID_BASE;
					custinfo.dial_type = tmpDial - DIAL_ID_BASE;
					custinfo.baud      = tmpBaud - BAUD_ID_BASE;
                    GetDlgItemText (hDlg, ID_PPREFIX, custinfo.phonePrefix,
                                    sizeof (custinfo.phonePrefix));
                    GetDlgItemText (hDlg, ID_PSUFFIX, custinfo.phoneSuffix,
                                    sizeof (custinfo.phoneSuffix));
                    WriteCustInfo (WRITE_CUSTCOMM);
				    EndDialog (hDlg, TRUE);
				    break;

				case IDCANCEL:
				    EndDialog (hDlg, FALSE);
				    break;

				default:
				    break;
			}
            break;

        default:
		    break;
    }
    return 0L;
}


/*************************************************************\
 ** WSLibManagerDlgProc
 **
 ** ARGUMENTS:  HWND    hDlg
 **             unsigned message
 **             WORD    wParam
 **             LONG    lParam
 **
 ** DESCRIPTION:  Window Shopper database/library management dlg procedure
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   John Ano  April 20, 1990 (written)
\*************************************************************/

LONG FAR PASCAL
WSLibManagerDlgProc(hWndDlg, message, wParam, lParam) 

    HWND        hWndDlg;
    unsigned    message;
    WORD        wParam;
    LONG        lParam;
{
    HWND    hWndLB;
    int     i;

    switch(message)
    {
        case WM_INITDIALOG:
            CenterPopUp(hWndDlg);

            // fill the cache list with values
            hWndLB = GetDlgItem(hWndDlg, ID_LB_CACHE);
            // turn off redraw
            SendMessage (hWndLB, WM_SETREDRAW, FALSE, 0L);
            for ( i=0; i < NUM_CACHE_OPTIONS; i++ )
                SendMessage(hWndLB, LB_ADDSTRING, 0, (LONG) (LPSTR) szCacheOptions[i]) ;
            // turn redraw back on
            SendMessage (hWndLB, WM_SETREDRAW, TRUE, 0L);
            //set default to what the user has saved
            SendMessage (hWndLB, LB_SETCURSEL, custinfo.cacheSize, 0L);

            SetFocus(hWndLB);
            return FALSE;
            break;

        case WM_COMMAND:
            switch(wParam)
            {
                case ID_HELP:
		            GetHelp(GH_TEXT,IDH_LIBMGR);
                    break;

                case ID_LB_CACHE:
                    if ( HIWORD (lParam) == LBN_DBLCLK )
                        SendMessage(hWndDlg,WM_COMMAND,IDOK,0L);
                    break;

                case IDCANCEL:
                    EndDialog (hWndDlg, FALSE);
                    break;

                case IDOK:
                    i = (WORD) SendDlgItemMessage (
                                                hWndDlg, 
                                                ID_LB_CACHE,
                                                LB_GETCURSEL, 0, 0L);

                    /* if what's requested will cause deletion of libs... */
                    if (GetCacheSize(i) < FileHead.CacheUsed)
                    {
                        SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_APPNAME);
                        if (ErrMsg(IDS_ERR_DELETE_LIBS) == IDYES)
                        {
                            custinfo.cacheSize = i;
                            dwCacheSize = GetCacheSize(custinfo.cacheSize) ;
                            MakeCacheSpace(-1L);
                        }
                        else
                            break;
                    }
                    else
                    {
                        /* if what we're adding is greater than avail... */
                        if (((long)GetCacheSize(i) - (long)GetCacheSize(custinfo.cacheSize)) >
                                                (long)AvailDiskSpace(-1))
                        {
                            ErrMsg(IDS_ERR_DISK_UNAVAIL);
                            break;
                        }
                        else
                        {
                            custinfo.cacheSize = i;
                            dwCacheSize = GetCacheSize(custinfo.cacheSize) ;
                        }
                    }

                    // write info to the config file
                    WriteCustInfo (WRITE_CUSTCACHE);
                    EndDialog (hWndDlg, TRUE);
                break;
            }
        break;
        default:
            return(FALSE);
    }
    return (TRUE);
}

WORD wValidBaudRates[] = {300, 600, 1200, 2400, 4800, 9600};
#define LEN_BAUD_LIST sizeof(wValidBaudRates) / sizeof(WORD)

VOID NEAR PASCAL ValidateBaudRate (WORD *nB)
{
    WORD    ii;
	 BOOL		bOK = FALSE;

	for (ii = 0; ii < LEN_BAUD_LIST; ii++)
   {
		if (*nB == wValidBaudRates[ii])
			bOK = TRUE;
	}
	if (!bOK)
		*nB = NULL;
}


VOID NEAR PASCAL ValidatePort (WORD *nP)  {

	WORD	ii;
	BOOL	bOK=FALSE;

    for (ii=1; ii<5; ii++) {
        if(*nP==ii)
            bOK=TRUE;
    }
	 if(!bOK)
	 	*nP=NULL;

}

DWORD FAR PASCAL
GetCacheSize(WORD cacheindex)

{
   /* convert index to memsize */
   return ((DWORD) CacheOpts[cacheindex] * 1024) ;
}
