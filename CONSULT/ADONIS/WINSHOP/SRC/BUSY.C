/*************************************************************\
 ** FILE:  busy.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION: "Busy..." message box, hourglass icon
 **              replacement for extremely long operations
 ** HISTORY:   
 **              Bill King  8/20/90   Created
\*************************************************************/

#include "windows.h"
#include "winundoc.h"
#include <string.h>
#include <stdio.h>
#include <io.h>
#include "ws.h"
#include "rc_symb.h"
#include "misc.h"
#include "comm.h"
#include "help.h"
#include "busy.h"

/*
 * Local Statics
 */
static FARPROC lpfnBusyDlgProc;
static WORD    BusyCalls = 0; //track how many times BusyBox(WinthIcon)
                             //is called before a BusyClear call since we want
                             // to Freeze and Thaw the main window only once

HWND   hBusyBox = NULL;
static ICON_ID gIcon;

LONG FAR PASCAL BusyBoxDlgProc (HWND, unsigned, WORD, LONG);
LONG FAR PASCAL BusyBoxIconDlgProc (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL BusyText(HANDLE hInstance, HWND hWnd, char* msg);
BOOL FAR PASCAL BusyTextWithIcon(HANDLE hInstance, HWND hWnd, char* msg, ICON_ID Icon);
void FAR PASCAL BusyLoadNewIcon (HWND hWndDlg, ICON_ID NewIcon);

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  BusyBox          ÿ                                     */
/*                                                                        */
/*   Description:  Put Up A "Busy..." message box.                        */
/*                 Returns immediately so you can continue                */
/*                 processing.                                            */
/*                 Call BusyBoxClear() to remove the box.                 */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
BOOL FAR PASCAL BusyBox(HANDLE hInstance,HWND hWnd, WORD msg)
{
    char msg_buf[MAX_MSG_LEN];

    ++BusyCalls;
    LoadString(hInstance,msg,(LPSTR) msg_buf,sizeof(msg_buf));
    return(BusyText(hInstance, hWnd, msg_buf));
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  BusyBoxWithIcon          ÿ                             */
/*                                                                        */
/*   Description:  Put Up A "Busy..." message box with an icon.           */
/*                 Returns immediately so you can continue                */
/*                 processing.                                            */
/*                 Call BusyBoxClear() to remove the box.                 */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
BOOL FAR PASCAL BusyBoxWithIcon (HANDLE hInstance, HWND hWnd, 
                                 WORD msg, ICON_ID Icon)
{
    char msg_buf[MAX_MSG_LEN];

    ++BusyCalls;
    LoadString(hInstance, msg, (LPSTR) msg_buf, sizeof (msg_buf));
    return (BusyTextWithIcon (hInstance, hWnd, msg_buf, Icon));
}

/*************************************************************\
 ** BusySetNewText
 **
 ** ARGUMENTS:      HWND hWnd - parent window
 **                 LPSTR   newText
 **                 ICON_ID   Icon - new icon, if zero don't use BusyWithIcon()
 **
 ** DESCRIPTION:    change the the text of the busy box to be 
 **                 newText, load new icon
 **
 ** ASSUMES:        hBusyBox is already up
 **
 ** MODIFIES:
 **
 ** RETURNS:   TRUE if successful, FALSE if not.
 **
 ** HISTORY:   Lauren Bricker  April 29, 1991 (written)
\*************************************************************/
BOOL
BusySetNewText (HWND hWnd, char* newText, ICON_ID Icon)
{
    if ( !hBusyBox )
    {
        ++BusyCalls;
        return (BusyText (hInstanceWS, hWnd, newText));
    }

    SetDlgItemText(hBusyBox, IDS_BUSY_MSG, (LPSTR)newText);
    if (Icon)
    {
         gIcon = Icon;
         BusyLoadNewIcon(hBusyBox, Icon);
    }
    return TRUE;
}


/*************************************************************\
 ** BusyLoadNewText
 **
 ** ARGUMENTS:  HWND hWnd - parent window
 **             WORD msg  - new message to load
 **             ICON_ID Icon - New icon to load, zero if not BusyWithIcon()
 **
 ** DESCRIPTION:    change the the text of the busy box to be 
 **                 newText
 **
 ** ASSUMES:        hBusyBox is already up
 **
 ** MODIFIES:       
 **
 ** RETURNS:   TRUE if successful, FALSE if not.
 **
 ** HISTORY:   Lauren Bricker  April 29, 1991 (written)
\*************************************************************/
BOOL
BusyLoadNewText (HWND hWnd, WORD msg, ICON_ID Icon)
{
    char msg_buf[MAX_MSG_LEN];

    LoadString (hInstanceWS, msg, (LPSTR) msg_buf, sizeof (msg_buf));

    if ( !hBusyBox )
    {
        ++BusyCalls;
        if (Icon)
            return (BusyTextWithIcon (hInstanceWS, hWnd, msg_buf, Icon));
        else
            return (BusyText (hInstanceWS, hWnd, msg_buf));
    }

    SetDlgItemText(hBusyBox, IDS_BUSY_MSG, (LPSTR)msg_buf);
    if (Icon)
    {
         gIcon = Icon;
         BusyLoadNewIcon(hBusyBox, Icon);
    }
    return TRUE;
}


static HWND hParentWnd, hChildWnd;
BOOL FAR PASCAL BusyText(HANDLE hInstance, HWND hWnd, char* msg)
{
    if (hBusyBox)  return(FALSE);

    if (hWnd)
    {
        hParentWnd = hWnd;
        hChildWnd = GetTopWindow(hParentWnd);
    }
    else
    {
        hParentWnd = GetActiveWindow();
        if (hParentWnd)
            if (GetWindowTask(hParentWnd) != hTaskWS)
                hParentWnd = NULL;
            else
                hChildWnd = GetTopWindow(hParentWnd);
    }

    lpfnBusyDlgProc = MakeProcInstance((FARPROC) BusyBoxDlgProc, hInstance);

    EnableAllModeless(FALSE);
    if (hParentWnd)
    {
       if (hParentWnd != hWndWS)
            EnableWindow(hParentWnd, FALSE);
       else if (BusyCalls == 1)
       {
            FREEZE_HWND_WS
       }
    }

    hBusyBox = CreateDialog(hInstance,"BUSYBOX",hParentWnd,lpfnBusyDlgProc);
    //hBusyBox = CreateDialog(hInstance,"BUSYBOX",NULL,lpfnBusyDlgProc);

    if (hBusyBox == NULL)  return(FALSE);

    SetDlgItemText(hBusyBox, IDS_BUSY_MSG, (LPSTR)msg);

    return(TRUE);
}


/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  BusyBoxDlgProc                                         */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                      */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL 
BusyBoxDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    switch(message) 
    {
        case WM_INITDIALOG:
            CenterPopUp(hWndDlg);
            ShowWindow(hWndDlg,SW_SHOW | SW_SHOWNORMAL);
            break;
        case WM_COMMAND:
            switch (wParam)
            {
                case IDOK:
                    EndDialog(hWndDlg,0);
                    break;
                default:
                    return(FALSE);
            }
        default:
            return(FALSE);
    }
    return(TRUE);
}

BOOL FAR PASCAL BusyTextWithIcon(HANDLE hInstance, HWND hWnd, char* msg,
                                 ICON_ID Icon)
{
    if (hBusyBox)
      return BusySetNewText(hWnd, msg, Icon);

    if (hWnd)
    {
        hParentWnd = hWnd;
        hChildWnd = GetTopWindow(hParentWnd);
    }
    else
    {
        hParentWnd = GetActiveWindow();
        if (hParentWnd)
            if (GetWindowTask(hParentWnd) != hTaskWS)
                hParentWnd = NULL;
            else
                hChildWnd = GetTopWindow(hParentWnd);
    }

    lpfnBusyDlgProc = MakeProcInstance((FARPROC) BusyBoxIconDlgProc, hInstance);

    EnableAllModeless(FALSE);
    if (hParentWnd)
    {
       if (hParentWnd != hWndWS)
            EnableWindow(hParentWnd, FALSE);
       else if (BusyCalls == 1)
       {
            FREEZE_HWND_WS
       }
    }

    gIcon = Icon;
    hBusyBox = CreateDialog(hInstance, "BUSYWICON", hParentWnd,
                lpfnBusyDlgProc);

    if (hBusyBox == NULL)  return(FALSE);

    SetDlgItemText(hBusyBox, IDS_BUSY_MSG, msg);

    return(TRUE);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  BusyBoxIconDlgProc                                     */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL 
BusyBoxIconDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    switch ( message ) 
    {
        case WM_INITDIALOG:
        {
            CenterPopUp (hWndDlg);
            BusyLoadNewIcon(hWndDlg, gIcon);
            break;
        }

        default:
            return (FALSE);
    }
    return(TRUE);
}


/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  BusyClear                                              */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
VOID FAR PASCAL 
BusyClear (BOOL bReEnable)
{
    /* do this before destroy so window bops down to the right parent */
    if (hParentWnd AND IsWindow(hParentWnd))
    {
       if (hParentWnd != hWndWS)
            EnableWindow(hParentWnd, TRUE);
       else
       {
            THAW_HWND_WS
            BusyCalls = 0;
       }

       if (hChildWnd AND IsWindow(hChildWnd))
       {
          EnableWindow(hChildWnd,TRUE);
          if ( IsWindow (hBusyBox) )
              ShowWindow(hBusyBox,SW_HIDE); // to make this clean
          SetFocus(hChildWnd);
          hParentWnd = hChildWnd = NULL;
       }
    }
    if ( IsWindow (hBusyBox) )
        DestroyWindow (hBusyBox);

    hBusyBox = NULL;

    if (bReEnable)
        EnableAllModeless(TRUE);
    else if ((hParentWnd) AND (hParentWnd != hWndWS))  //don't get it - beng
         EnableWindow(hParentWnd, FALSE); // disable again

    if ( lpfnBusyDlgProc )
    {
    	FreeProcInstance (lpfnBusyDlgProc);
		lpfnBusyDlgProc = NULL;
    }
}

void FAR PASCAL
BusyLoadNewIcon (HWND hWndDlg, ICON_ID NewIcon)
{
   ICON_ID IconID;
   for (IconID = IDS_BUSY_FIRST_ICON; IconID <= IDS_BUSY_LAST_ICON; IconID++)
         ShowWindow(GetDlgItem(hWndDlg, IconID),
               (IconID == NewIcon) ? SW_SHOW : SW_HIDE);
   UpdateWindow(hWndDlg);
}

#if 0
LONG FAR PASCAL
BusyLoadNewIcon (HWND hWndDlg, ICON_ID NewIcon)
{
     LONG lIconStyle;
     RECT rcClient;
     HWND hWndIcon;

     GetWindowRect(GetDlgItem(hWndDlg, IDS_ICON), (LPRECT)&rcClient);
     ScreenToClient(hWndDlg, (LPPOINT) &rcClient.left);
     ScreenToClient(hWndDlg, (LPPOINT) &rcClient.right);
     lIconStyle = GetWindowLong(GetDlgItem(hWndDlg, IDS_ICON), GWL_STYLE);
     DestroyWindow(GetDlgItem(hWndDlg, IDS_ICON));

     hWndIcon = (CreateWindow("static",
                    //(LPSTR)LoadIcon(hInstanceWS, MAKEINTRESOURCE(wNewIcon)),
                    (LPSTR)NewIcon,
                    lIconStyle,
                    rcClient.left,
                    rcClient.top,
                    NULL,
                    NULL,
                    hWndDlg,
                    IDS_ICON,
                    hInstanceWS,
                    (LPSTR)NULL));
     if (hWndIcon)
         ShowWindow(hWndIcon, SW_SHOW);
     return (hWndIcon);
}
#endif
