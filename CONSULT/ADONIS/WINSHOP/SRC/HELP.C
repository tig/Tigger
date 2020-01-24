#include <windows.h>
#include <string.h>
#include "ws.h"
#include "misc.h"
#include "file.h"
#include "error.h"


#include "help.h"
#include "rc_symb.h"

HWND    hWndHelpText=NULL;
char    szWhatsNew[] = "40000000";
char    szWholePath[_MAX_PATH];
BOOL    bWhatsNew = FALSE;

#if 0
static    HWND    hWndHelpTopics=NULL;
static HelpTopic aHelpTopic[]={ {"Billing information", IDH_BILLING},
                                          {"Change Settings", IDH_SETTINGS},
                                          {"Getting Info", IDH_INFO},
                                          {"Ordering", IDH_ORDERING}
                                        };
#define NUM_TOPICS    sizeof(aHelpTopic)/sizeof(HelpTopic)
#endif

LONG FAR PASCAL 
    WSMultiEditSubProc1(HWND hWndDlg, unsigned message, WORD wParam, LONG lParam);


/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  GetHelp                                                */
/*                                                                        */
/*   Description:  Displays the HelpTopics or HelpText dialog boxes.      */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/

/* JOHNSTUFF */
/* ? LOOKS LIKE THE HELPTOPICS DLG BOX IS NOT DEFINED */

VOID FAR PASCAL 
GetHelp(WORD nID, WORD nTopic) 
{

    if ( !hWndHelpText )				 /* if help text not already up  */
        hWndHelpText=CreateDialog(hInstanceWS, (LPSTR)"HELPTEXT", hWndWS, lpprocHelpTextDlg);

    if ( IsWindow (hWndHelpText) )
    {
        PostMessage(hWndHelpText, WM_COMMAND, IDM_HELP_TOPIC, MAKELONG(nTopic,nID));
    	SetFocus(hWndHelpText);
    }
    else
	    ErrMsg (IDS_ERR_NOHELP);

}


/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  ShowWhatsNew                                           */
/*                                                                        */
/*   Description:  Show the what's new file in the help dialog box        */
/*                                                                        */
/*       History:  4/5/91 LJB Created                                     */
/*                                                                        */
/*------------------------------------------------------------------------*/


BOOL FAR PASCAL 
ShowWhatsNew (HWND hWnd)
{

    bWhatsNew = TRUE;
    /* Check that the what's new file exists */
    lstrcpy (szWholePath, szWhatsNew);
    AddFullPath(szWholePath);
    if ( GetExt(szWholePath) == FAIL )
    {
	    ErrMsg (IDS_MSG_MISSING_WHATS_NEW);
        return FAIL;
    }


    if ( !hWndHelpText )				 /* if help text not already up  */
        hWndHelpText = CreateDialog(hInstanceWS, (LPSTR)"HELPTEXT", hWndWS,
                                    lpprocHelpTextDlg);

    if ( IsWindow (hWndHelpText) )
    {
        PostMessage (hWndHelpText, WM_COMMAND, IDM_WHATS_NEW, 0L);
    	SetFocus (hWndHelpText);
    }
    else
    {
	    ErrMsg (IDS_ERR_NOHELP);
        return FAIL;
    }

    return OK;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSHelpTopicsDlgProc                                    */
/*                                                                        */
/*   Description:  Dialog box function for help topics dialog box.        */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
#if 0
LONG FAR PASCAL WSHelpTopicsDlgProc (hWndDlg, message, wParam, lParam) 

    HWND        hWndDlg;
    unsigned    message;
    WORD        wParam;
    LONG        lParam;

{

    WORD    ii;
    HWND    hWndList;

    REFERENCE_FORMAL_PARAM(lParam);

    switch(message) {
        
        case WM_INITDIALOG:
            CenterPopUp(hWndDlg);
            hWndList=GetDlgItem(hWndDlg, ID_HELP_LIST);
            for (ii=0; ii<NUM_TOPICS; ii++) {
                SendMessage(hWndList, LB_INSERTSTRING, -1, (LONG)(LPSTR)aHelpTopic[ii].szName);
                }
            break;

        case WM_COMMAND:

            switch(wParam) {
                case IDCANCEL:
                    hWndHelpTopics=NULL;
                    DestroyWindow(hWndDlg);
                    break;

                case IDOK:
                    if(!hWndHelpText) {
                        CreateDialog(hInstanceWS, (LPSTR)"HELPTEXT", hWndWS, lpprocHelpTextDlg);
                    }
                    ii = (WORD) SendMessage(GetDlgItem(hWndDlg, ID_HELP_LIST), LB_GETCURSEL, 0, 0L);
                    PostMessage(hWndHelpText, WM_COMMAND, IDM_HELP_TOPIC, MAKELONG(aHelpTopic[ii].rID,0));
                    SetFocus(hWndHelpText);

                    hWndHelpTopics=NULL;
                    DestroyWindow(hWndDlg);
                    break;

                default:
                    break;
            } /* end switch */
            break;

        default:
            return(FALSE);
    }

    return(TRUE);
}
#endif

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSHelpTextDlgProc                                      */
/*                                                                        */
/*   Description:  Dialog box function for help text dialog box.          */
/*                 Responsible for getting help text and displaying it.   */
/*                     ü  !  !  d_                                     */
/*       History:  12/31/89  Originally created. [lds]                    */
/*                                                                        */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL 
WSHelpTextDlgProc (hWndDlg, message, wParam, lParam) 

    HWND        hWndDlg;
    unsigned    message;
    WORD        wParam;
    LONG        lParam;

{
    static    int    nLastRCId;
    static    HWND        hWndEdit;
    static bMax = FALSE;
    int        nRCId;
    HANDLE    hResource;
    HANDLE    hResData;
    LPSTR        lpData;
    LPSTR        lpT;
    static    BOOL    bVisible=FALSE;
    RECT        rRect;


        
    switch(message) {
        case WM_INITDIALOG:
            if ( bWhatsNew )
            {
                bWhatsNew = FALSE;
                SetWindowText (hWndDlg, (LPSTR) "What's New");
            }
            CenterPopUp(hWndDlg);

            hWndEdit=GetDlgItem(hWndDlg, ID_EDIT);

            GetClientRect(hWndDlg, &rRect);
            MoveWindow(hWndEdit, 
                0, 0, WIDTH_RECT(rRect), HEIGHT_RECT(rRect), TRUE);

			InflateRect (&rRect, -1, -1);
            //rRect.left=rRect.top=1;
			rRect.right -= 2;
            SendMessage(hWndEdit, EM_SETRECT, 0, (LONG)((LPRECT)&rRect));

            return TRUE;

        case WM_NCLBUTTONDOWN:
            switch (wParam)
            {
                case HTZOOM:
                {
#if 0
                    int nPosShouldBe,nPosB,nPosA,nMin,nMax,nLineCountB,nLineCountA;
                    if (!bMax)
                    {
                        nLineCountB = (WORD)SendMessage(hWndEdit,EM_GETLINECOUNT,0,0L);
                        nPosB = GetScrollPos(hWndEdit,SB_VERT);
                    }
#endif
				    ShowWindow(hWndDlg, bMax ? SW_SHOWNORMAL : SW_SHOWMAXIMIZED);
                    if (!bMax)
                    {
                        int nPos = GetScrollPos(hWndEdit,SB_VERT);
                        /*SetScrollPos(hWndEdit,SB_VERT,0,TRUE);*/
                        SendMessage(hWndEdit,EM_LINESCROLL,0,MAKELONG(-nPos,0));
                    }
#if 0
                    if (!bMax)
                    {
                        nLineCountA = (WORD)SendMessage(hWndEdit,EM_GETLINECOUNT,0,0L);
                        nPosShouldBe = nPosB * nLineCountA / nLineCountB;
                        SendMessage(hWndEdit,EM_LINESCROLL,0,MAKELONG(nPosShouldBe-nPosB,0));
                    }
#endif
                    bMax = !bMax;
                }
                break;
            }
            return FALSE;
        break;

        case WM_SIZE: 
            hWndEdit = GetDlgItem (hWndDlg, ID_EDIT);
            MoveWindow (hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            UpdateWindow (hWndEdit);
            break;

        case WM_CLOSE:
            hWndEdit=NULL;
            DestroyWindow(hWndDlg);
            hWndHelpText=NULL;
            nLastRCId=NULL;
            bVisible=FALSE;
            break;

        case WM_COMMAND:

            switch(wParam) {
                    
                case IDOK:
                    PostMessage(hWndDlg,WM_CLOSE,0,0L);
                break;

                case IDM_WHATS_NEW:
                {
	                int     fp = -1;
                    HWND    hNew;
	                if ( (fp = _lopen(szWholePath, READ)) == -1 )
                    {
	                    ErrMsg (IDS_MSG_MISSING_WHATS_NEW);
                        break;
                    }
                    
                    /* load the what's new file into the help dialog box */
                    hNew = GetDlgItem (hWndDlg, ID_EDIT);
	                if ( hNew )
		                FillEditBox (fp, hNew, '$');

                    if(!bVisible)
                    {
                        bVisible = TRUE;
                        ShowWindow (hWndDlg, SW_SHOWNORMAL);
                    }
                    if (fp != -1)
                        _lclose (fp);
                    break;
                }

                case IDM_HELP_TOPIC:
                    nRCId=LOWORD(lParam);
                    if(nRCId > 0)  {
                        if (nRCId!=nLastRCId) {
                            hResource=FindResource(hInstanceWS, MAKEINTRESOURCE(nRCId), 
                                (LPSTR)MAKELONG(HELP,0));
                            if(hResource) {
                                hResData=LoadResource(hInstanceWS, hResource);
                                if(hResData) {
                                    lpData=LockResource(hResData);

                                    /* $ marks end of help, replace with NULL */
                                    for (lpT = lpData; *lpT != '$'; lpT++)
                                        ;
                                    *lpT = NULL;
                                    SetDlgItemText (hWndDlg, ID_EDIT, (LPSTR) lpData);
                                    *lpT = '$';
                                    UnlockResource(hResData);
                                    nLastRCId=nRCId;
                                }
                                FreeResource(hResData);
                                if(!bVisible) {
                                    bVisible=TRUE;
                                    ShowWindow(hWndDlg, SW_SHOWNORMAL);
                                }
                            } 
                        }
                    } 
                    else { /* not greater than 0, special case */
#if 0 
                         if (LOWORD(lParam)==IDH_SETTINGS) {
                             wprintf("Settings");
                        }
#endif
                    }
                    break;

                default:
                    break;
            } /* end switch */
            break;

        default:
            return(FALSE);
    }

    return(TRUE);

}

FARPROC    lpprocEditWnd;
LONG FAR PASCAL 
WSMultiEditSubProc(HWND hWndDlg, unsigned message, WORD wParam, LONG lParam) 
{
    static  HCURSOR hCursorArrow=NULL;
	static	BOOL	bFocus;

    switch (message)
    {
        case EM_SETSEL:
        	break;

        case WM_CHAR:

            if (wParam == VK_TAB)
			 	return 0L;
            //    break;
            else if (wParam == VK_UP)
            {
                wParam = VK_PRIOR;
                return(CallWindowProc(lpprocEditWnd, hWndDlg, message, wParam,lParam));
            }
            else if (wParam == VK_DOWN)
            {
                wParam = VK_NEXT;
                return(CallWindowProc(lpprocEditWnd, hWndDlg, message, wParam,lParam));
            }
            else
                break;

#if 0
        // Hilight the description box title to tell whether or not it has focus
        case WM_PAINT:          
        {

           PAINTSTRUCT ps;
           HDC     hDC;
           RECT    r;
           HBRUSH  hBr;
			long	retVal;
           retVal = CallWindowProc(lpprocEditWnd, hWndDlg, message, wParam, lParam);
           hDC = BeginPaint (hWndDlg, &ps);
           GetClientRect (hWndDlg, &r);
			InflateRect (&r, -3, -3);
			//InvertRect (hWndDlg, &r, TRUE);
           if (bFocus)
               hBr = GetStockObject (BLACK_BRUSH);
           else
               hBr = GetStockObject (WHITE_BRUSH);
           FrameRect (hDC, &r, hBr);
           EndPaint (hWndDlg, &ps);
			return retVal;
           break;
        }
#endif

        case WM_SETFOCUS:
            bFocus = TRUE;      
        case WM_MOUSEACTIVATE:
        case WM_MOUSEMOVE:
        case WM_SETCURSOR:
            SetCursor (LoadCursor (NULL, IDC_ARROW));
            break;

        case WM_KILLFOCUS:
            bFocus = FALSE;
            return(CallWindowProc(lpprocEditWnd, hWndDlg, message, wParam,lParam));
        
        default:
            return(CallWindowProc(lpprocEditWnd, hWndDlg, message, wParam,lParam));
    }

	return(TRUE);

}

