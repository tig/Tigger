#include <windows.h>
#include "database.h"
#include <string.h>
#include <stdio.h>
#include <io.h>
#include "winundoc.h"
#include "ws.h"
#include "rc_symb.h"
#include "libids.h" /* thumbnail resource info */
#include "misc.h"
#include "order.h"
#include "file.h"
#include "help.h"
#include "download.h"
#include "viewbox.h"
#include "error.h"
#include "custinfo.h"
#include "comm.h"
#include "billing.h"
#include "messages.h"
#include "busy.h"
#include "info.h"

LONG FAR PASCAL 
WSCopyrightDlgProc(HWND hWndDlg, unsigned message, WORD wParam, LONG lParam);
static BOOL ShowCopyright(void);
static WORD SubmitDld(void);
static WORD QueryDldStatus(HWND hWndDlg,ORDER *Order);
static void NEAR PASCAL ComputePrices(HWND hWndDlg, int nClipIndex);
static char* GetTimeStr(long lElapsedSeconds, char* str);

HANDLE hDldWnd=NULL;
static double  Total, WSPrice;
static double  Taxes, SurCharge;

/* To determine initialization of Select Directory dialog box */
int nSetDldInfoMode;
BOOL bShowUseAltCacheBox;

extern VBS      vbs;

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  DownloadSingle                                         */
/*                                                                        */
/*   Description:  Displays the Download dialog box.                      */
/*                                                                        */
/*       History:  1-1-90  Originally created.  [lds]                     */
/*                                                                        */
/*------------------------------------------------------------------------*/
VOID FAR PASCAL 
DownloadSingle() 
{
    FARPROC    lpproc;
    int nRetval;

    EnableAllModeless(FALSE);

    lpproc = MakeProcInstance((FARPROC)WSDownloadOrderDlgProc, hInstanceWS);
    nRetval = DialogBox(hInstanceWS, (LPSTR)"CLIPORDER", hWndWS, lpproc);
    FreeProcInstance(lpproc);
    EnableAllModeless(TRUE);
    SetFocus(hWndView);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSDownloadOrderDlgProc                                */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:  1-1-90  Originally created.  [lds]                     */
/*                                                                        */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL 
WSDownloadOrderDlgProc (HWND hWndDlg, unsigned message,
                        WORD wParam, LONG lParam)
{
    FARPROC    lpproc;
    int stat;

    switch(message) {

        case WM_INITDIALOG:
                CenterPopUp(hWndDlg);
                SetDlgItemText(hWndDlg, IDS_CLIPART_NAME, CURCLIPDESC);
                SetFocus(GetDlgItem(hWndDlg, IDOK));
                if (EnumClipVersions(vbs.nSelect, AddStringToListBox,
                    GetDlgItem(hWndDlg, IDL_VERSION), CURTHUMBINFO) == 0)
                {
                    ErrMsg (IDS_ERR_NOT_DOWNLOADABLE);
                    EndDialog(hWndDlg, FALSE);
                    break;
                }
                SendDlgItemMessage(hWndDlg, IDL_VERSION, LB_SETCURSEL, 0, 0L);    
                ComputePrices(hWndDlg,vbs.nSelect);

                return FALSE; // changed focus

        case WM_COMMAND:
            switch (wParam)
            {
                ORDER Order;
                case ID_HELP:
                    GetHelp(GH_TEXT,IDH_DLOADCLIP);
                    break;

                case IDL_VERSION:
                    switch(HIWORD(lParam)) 
                    {
                        case LBN_SELCHANGE:
                            ComputePrices(hWndDlg,vbs.nSelect);
                            break;
                    }
                    break;

                case IDB_DLDINFO:
                    nSetDldInfoMode = DLD_INFO_MODE_CLIPART;
                    lpproc=MakeProcInstance((FARPROC)WSDldInfoDlgProc, hInstanceWS);
                    DialogBox(hInstanceWS, (LPSTR)"DLDINFO", hWndDlg, lpproc);   /* not sure about the parent proc here*/
                    FreeProcInstance(lpproc);
                    return TRUE;

                case IDB_SELECT_BILLING:
                    if (NOT bConnect)
                    {
                        ErrMsg(IDS_ERR_MUST_LOGON);
                        EnableAllModeless(FALSE); //just in case
                        SetFocus(GetDlgItem(hWndDlg,IDOK));
                        break;
                    }

#if 1
                    GetInfo(hWndDlg,IDM_BILLING_INFO);
#else
                    lpproc=MakeProcInstance((FARPROC)WSSelectBillingDlgProc, hInstanceWS);
                    DialogBox(hInstanceWS, (LPSTR)"SELECTBILLING", hWndDlg, lpproc);
                    FreeProcInstance(lpproc);
#endif
                    ComputePrices(hWndDlg,vbs.nSelect);
                    return TRUE;

#if 0
#define ENABLE(onoff) if (wWinVer < 0x300) \
            		    EnableWindow(GetDlgItem (hWndDlg, IDOK), onoff); \

#else
// don't want to disable unless necessary (busy box will handle it)
#define ENABLE(onoff) EnableWindow (hWndDlg, onoff);
#endif

                case IDOK:
                    // work around for Win 2.11 bug!!
                    ENABLE(FALSE);

                    if (NOT bConnect)
                    {
                        ErrMsg(IDS_ERR_MUST_LOGON);
                        EnableAllModeless(FALSE);  //just in case
                        SetFocus(GetDlgItem(hWndDlg,IDCANCEL));
                        ENABLE(TRUE);
                        break;
                    }

                    if (bNULLFILE(CURCLIPFILENAME))
                    {
                        ErrMsg (IDS_ERR_NOT_DOWNLOADABLE);
                        SetFocus(GetDlgItem(hWndDlg,IDCANCEL));
                        ENABLE(TRUE);
                        break;
                    }

                    if ( !custinfo.card[custinfo.whichCard].valid )
                    {
                        FARPROC lpproc;
                        BOOL bReturn;
                        int    nReturn;

                        SetDest (hWndDlg, NOBEEP, MB_YESNO | MB_ICONQUESTION, IDS_TITLE_APPNAME);
                        if ( ErrMsg (IDS_ERR_INVALID_BILLING) == IDNO )
                        {
                            ENABLE(TRUE);
                            EnableAllModeless(FALSE); //just in case
                            break;
                        }
                        EnableAllModeless(FALSE); //just in case

                        lpproc = MakeProcInstance ((FARPROC)WSVerifyPasswdDlgProc, hInstanceWS) ;
                        nReturn = DialogBox (hInstanceWS, "NEWUSER", hWndDlg, lpproc);
                        FreeProcInstance (lpproc);
                        switch(nReturn) 
                        {
                           case OK:
                               break;
                           case IDCANCEL:
                           case FAIL:
                           default:
                               ENABLE(TRUE);
                               goto done;
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

                    Append(custinfo.downloadDir, ".");  // trailing "\." for access
					     if ( access (custinfo.downloadDir, 0) )
					     {
                        char szTmpPath[_MAX_PATH];
                        SetDest (hWndDlg, NOBEEP, MB_OK | MB_ICONEXCLAMATION, IDS_TITLE_APPNAME);
                        ErrMsg (IDS_ERR_DIR_DOESNT_EXIST, (LPSTR) custinfo.downloadDir);
                        EnableAllModeless(FALSE); //just in case
                        lstrcpy (szTmpPath, custinfo.downloadDir);      //Save it incase CANCEL
					     	   lstrcpy (custinfo.downloadDir, szExePath);
                        nSetDldInfoMode = DLD_INFO_MODE_CLIPART;
                        lpproc = MakeProcInstance((FARPROC)WSDldInfoDlgProc, hInstanceWS);
                        stat = DialogBox(hInstanceWS, (LPSTR)"DLDINFO", hWndDlg, lpproc);
                        FreeProcInstance(lpproc);
                        if (stat == IDCANCEL)
                        {
                            lstrcpy(custinfo.downloadDir, szTmpPath);
                            ENABLE(TRUE);
                            break;
                        }
					     }

                    if ( !stricmp (custinfo.downloadDir, szDataPath) )
                    {
                        SetDest (hWndDlg, NOBEEP, MB_OK | MB_ICONEXCLAMATION, IDS_TITLE_APPNAME);
                        ErrMsg (IDS_ERR_BACKUP_TO_DIFF);
                        EnableAllModeless(FALSE); //just in case
                        nSetDldInfoMode = DLD_INFO_MODE_CLIPART;
                        lpproc = MakeProcInstance((FARPROC)WSDldInfoDlgProc, hInstanceWS);
                        stat = DialogBox(hInstanceWS, (LPSTR)"DLDINFO", hWndDlg, lpproc);
                        FreeProcInstance(lpproc);
                        if (stat == IDCANCEL)
                        {
                            ENABLE(TRUE);
                            break;
                        }
                    }

                    /** first display copyright notice **/
                    if (NOT ShowCopyright())
                    { //User canceled/rejected copyright, we stop download process
                        ENABLE(TRUE);
                        //EndDialog(hWndDlg, FALSE);
                        break;
                    }

                    switch (QueryDldStatus(hWndDlg,&Order)) 
                    {
                        case NAK:
                            // not accepted
                            ErrMsg (IDS_ERR_CREDIT_NOT_ACCEPT);
                            EnableAllModeless(FALSE); //just in case
                            break;

                        case FAIL:
                        case EOT:
			                   ErrMsg(IDS_ERR_UNABLE_TO_ORDER);
                            EnableAllModeless(FALSE); //just in case
                            break;

                        case ACK:
                        {   //start to download
                            lpproc = MakeProcInstance((FARPROC)WSDownloadSingleDlgProc, hInstanceWS);
                            if (CreateDialog(hInstanceWS, (LPSTR)"DOWNLOAD", NULL, lpproc))
                            {
                                int status;

                                SendMessage(hDldWnd, MM_SET_DOWNLOAD_TYPE, IDS_MSG_GETTING_CLIP, 0L);

                                //set download dest text
                                if ( !custinfo.downloadDir[0] )
                                {
                                    get_cur_dir (custinfo.downloadDir);
                                    /* get rid of . on end */
                                    custinfo.downloadDir[strlen(custinfo.downloadDir) - 1] = '\0';
                                }

                                SendMessage(hDldWnd, MM_SET_DEST_DIR, 0, (LONG)((LPSTR)custinfo.downloadDir));

                                status = Download(CURCLIPFILENAME, custinfo.downloadDir,
                                            TRUE, TRUE,
                                            FALSE, TRUE,
                                            TRUE, NULL);

                                if (hDldWnd) // then dld not terminated by user
                                {
                                    DestroyWindow(hDldWnd);
                                    hDldWnd = NULL;
                                }
                                FreeProcInstance(lpproc);

                                if (status == OK) // dld status that is.
                                {
                                    if (SubmitDld() != OK)
                                        ; // they may have just got a free clipart

                                    ErrMsg (IDS_MSG_CLIPART_DOWN); // they did get a clipart
                                    EnableAllModeless(FALSE); //just in case
                                    EndDialog(hWndDlg, TRUE);
                                    break;
                                }
                                else
                                {
                                    IssueErr();
                                    ErrMsg (IDS_MSG_CLIPART_NOT_DOWN);
                                    EnableAllModeless(FALSE);
                                    ENABLE(TRUE);
                                }
                            }
                            else
                            {
                                ErrMsg (IDS_ERR_UNABLE_TO_ORDER);
                                EnableAllModeless(FALSE); //just in case
                                ENABLE(TRUE);
                            }
                            break;
                        }
                    }
                    done:
                    break;

                case IDCANCEL:
                    EndDialog(hWndDlg, FALSE);
                    break;
            }
            break;
        default:
            return(FALSE);
    } /* end switch message */
    return(TRUE);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSDownloadSingleDlgProc                                */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:  1-1-90  Originally created.  [lds]                     */
/*                                                                        */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL
WSDownloadSingleDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    static  HBRUSH   hBrush;
    HWND    hThumbnail;
    HDC     hDCMem;
    HBITMAP hBitmap, hOld;
    RECT    bitRect;
    int     bitLeft, bitTop;
    static WORD wPartsInJob = 0;
    static WORD wPartsComplete = 0;
    static WORD wBitMapType = 0;
    char szPercentage[10];
    LONG lResult = 0;
    static WORD wHeight;
    static int nOldPercent = -1;

    switch(message)
    {
         case WM_INITDIALOG:
		   {
            wPartsInJob = 1;
            wPartsComplete = 0;
            wBitMapType = 0;
            wHeight = 0;
            nOldPercent = -1;
			   EnableWindow (GetDlgItem (hWndDlg, IDCANCEL), FALSE);
            CenterPopUp(hWndDlg);
            hDldWnd = hWndDlg;
            break;
        }

        case WM_CTLCOLOR:
            hThumbnail = GetDlgItem(hWndDlg, ID_THUMBNAIL);
            hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
            if((LOWORD(lParam)!= hThumbnail) OR (NOT wBitMapType))
                return(FALSE);

            SetBkColor(wParam, GetSysColor(COLOR_WINDOW));
            SetTextColor(wParam,GetSysColor(COLOR_WINDOWTEXT));

            /* load thumbnail image into device context */
            hDCMem = CreateCompatibleDC(wParam);
            if (wBitMapType == CLIPART_BMP)   //use Cur thumbnail
               hBitmap = LoadBitmap(vbs.hLib, lpBMP_ID(vbs.nSelect));
            else                                  //load resource
               hBitmap = LoadBitmap(hInstanceWS, MAKEINTRESOURCE(wBitMapType));

            hOld = SelectObject(hDCMem, hBitmap);

            /* might want to center bitmap in control */
            GetClientRect (GetDlgItem (hWndDlg, ID_THUMBNAIL), &bitRect);
            bitLeft = (bitRect.right - WIDTH_BITMAP) / 2;
            bitTop  = (bitRect.bottom - HEIGHT_BITMAP) / 2;
            BitBlt(wParam, bitLeft, bitTop, WIDTH_BITMAP, HEIGHT_BITMAP,hDCMem,0,0,SRCCOPY);

            SelectObject(hDCMem, hOld);
            DeleteObject(hBitmap);
            ReleaseDC(hWndDlg, hDCMem);
            return((DWORD)hBrush);
            break;

        case WM_PAINT:
        {
           PAINTSTRUCT ps;
           /* HBRUSH hBrush; */
           DWORD dwColor;
           RECT rcClient, rcPrcnt;
           int nlength;

           if (wPartsInJob == 0)
           {
              wPartsInJob = 1;
              wPartsComplete = 0;
           }

           nlength = wsprintf(szPercentage, "%d%%", (100 * (DWORD)wPartsComplete) / wPartsInJob);

           BeginPaint(hWndDlg, (LPPAINTSTRUCT)&ps);

           //set this once, it shouldn't change between PAINT msg's
           if (NOT wPartsComplete)
               wHeight = HIWORD(GetTextExtent(ps.hdc, "X", 1));

           // Set-up default foreground and background text colors.
           SetBkColor(ps.hdc, GetSysColor(COLOR_WINDOW));
           SetTextColor(ps.hdc, GetSysColor(COLOR_WINDOWTEXT));

           // Send WM_CTLCOLOR message to parent in case parent want to
           // use a different color in the Meter control.
           //hBrush = (HBRUSH) SendMessage(hWndDlg, WM_CTLCOLOR, ps.hdc,
              //MAKELONG(hWndMeter, CTLCOLOR_METER));

           // Always use brush returned by parent.
           /* hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOWTEXT)); */
           //SelectObject(ps.hdc, hBrush);

           SetTextAlign(ps.hdc, TA_CENTER | TA_TOP);

           // Invert the foreground and background colors.
           dwColor = GetBkColor(ps.hdc);
           SetBkColor(ps.hdc, SetTextColor(ps.hdc, dwColor));

           //Get co-ords of meter window in terms of hWndDlg client
           GetWindowRect(GetDlgItem(hWndDlg, ID_PERCENT_COMPLETE), (LPRECT)&rcClient);
           ScreenToClient(hWndDlg, (LPPOINT) &rcClient.left);
           ScreenToClient(hWndDlg, (LPPOINT) &rcClient.right);

           // Set rectangle coordinates to include only left
           // percentage of the window.
           SetRect(&rcPrcnt,
              rcClient.left,
              rcClient.top,
              rcClient.left + (int)(((DWORD)(rcClient.right - rcClient.left) * (DWORD)wPartsComplete) / wPartsInJob),
              rcClient.bottom);

           // Output the percentage value in the window.
           // Function also paints left part of window.
           ExtTextOut(ps.hdc,
              rcClient.left + ((rcClient.right - rcClient.left) / 2),
              (rcClient.top + ((rcClient.bottom - rcClient.top) - wHeight) / 2),
              ETO_OPAQUE | ETO_CLIPPED, &rcPrcnt,
              (LPSTR)szPercentage, nlength, NULL);

           // Adjust rectangle so that it includes the remaining
           // percentage of the window.
           rcPrcnt.left = rcPrcnt.right;
           rcPrcnt.right = rcClient.right;

           // Invert the foreground and background colors.
           dwColor = GetBkColor(ps.hdc);
           SetBkColor(ps.hdc, SetTextColor(ps.hdc, dwColor));

           // Output the percentage value a second time in the window.
           // Function also paints right part of window.
           ExtTextOut(ps.hdc,
              rcClient.left + ((rcClient.right - rcClient.left) / 2),
              (rcClient.top + ((rcClient.bottom - rcClient.top) - wHeight) / 2),
              ETO_OPAQUE | ETO_CLIPPED, &rcPrcnt,
              szPercentage, nlength, NULL);

           EndPaint(hWndDlg, &ps);
           break;
        }

        case MM_SET_PARTS_IN_JOB:
        case MM_SET_PARTS_COMPLETE:
        case MM_INC_PARTS_COMPLETE:
        {
           RECT rcClient;
           switch (message)
           {
              case MM_SET_PARTS_IN_JOB : wPartsInJob = wParam;
#ifdef INTERNAL
                 AuxPrint("Initializing, PARTS_IN_JOB = %d", wPartsInJob);
#endif
                 break;
              case MM_SET_PARTS_COMPLETE : wPartsComplete = wParam;
#ifdef INTERNAL
                 AuxPrint("Initializing, PARTS_COMPLETE = %d", wPartsComplete);
#endif
                 break;
              case MM_INC_PARTS_COMPLETE : wPartsComplete++;
#ifdef INTERNAL
                 //AuxPrint("Initializing, Incrementing PARTS_COMPLETE = %d", wPartsComplete);
#endif
                 break;
           }
           wPartsInJob = max(1, wPartsInJob);
           if ((nOldPercent != (int)((100 * (DWORD)wPartsComplete) / wPartsInJob)) AND
               (nOldPercent <= 100))   //greater than 100% would look bad
           {   //don't update unless percentage actually changes, prevents flashing
              nOldPercent = (int)((100 * (DWORD)wPartsComplete) / wPartsInJob);
              GetWindowRect(GetDlgItem(hWndDlg, ID_PERCENT_COMPLETE), (LPRECT)&rcClient);
              ScreenToClient(hWndDlg, (LPPOINT) &rcClient.left);
              ScreenToClient(hWndDlg, (LPPOINT) &rcClient.right);
              InvalidateRect(hWndDlg, (LPRECT) &rcClient, FALSE);
              UpdateWindow(hWndDlg);
           }
           break;
        }

        case MM_SET_DOWNLOAD_TYPE:
        {
           char szMessage[MAX_MSG_LEN];
           HDC hDC;

#ifdef INTERNAL
           AuxPrint("Initializing, Download type = %d", wParam);
#endif
           //set bitmap type for drawing bitmap (during processing of CTL_COLOR msg)
           wBitMapType = wParam;
           hDC = GetDC(GetDlgItem(hWndDlg, ID_THUMBNAIL));
           SendMessage(hWndDlg, WM_CTLCOLOR, hDC,
                        MAKELONG(GetDlgItem(hWndDlg, ID_THUMBNAIL), CTLCOLOR_STATIC));
           ReleaseDC(GetDlgItem(hWndDlg, ID_THUMBNAIL), hDC);

           //Set descriptive text
           LoadString(hInstanceWS, wParam, (LPSTR)szMessage, MAX_MSG_LEN);
           SetWindowText(GetDlgItem(hWndDlg, IDS_DESCRIPTION), (LPSTR)szMessage);

           //set file format
           switch (wParam)
           {
               case DLD_TYPE_CLIPART :
                  strcpy(szMessage, CURVERSION);
                  break;
               case DLD_TYPE_DB :
                  SetWindowText(GetDlgItem(hWndDlg, ID_DLDTIME_LABEL), (LPSTR)"Update Time:");
                  LoadString(hInstanceWS, IDS_MSG_WS_FRMT, (LPSTR)szMessage, MAX_MSG_LEN);
                  break;
               case DLD_TYPE_EXE :
                  LoadString(hInstanceWS, IDS_MSG_EXE, (LPSTR)szMessage, MAX_MSG_LEN);
                  break;
           }
           SetWindowText (GetDlgItem(hWndDlg, IDS_VERSION), (LPSTR)szMessage);

           lResult = OK;
           break;
        }

        case MM_SET_DEST_DIR:
        {
           char sztmpPath[_MAX_PATH];
#ifdef INTERNAL
           AuxPrint("Initializing, Download dir = %s", (PSTR)((LPSTR)lParam));
#endif
           FixDirPath(GetDlgItem(hWndDlg, IDS_CLIP_DIR), (PSTR)((LPSTR)lParam),
			              sztmpPath, sizeof (sztmpPath));
           SetWindowText(GetDlgItem(hWndDlg, IDS_CLIP_DIR), (LPSTR)sztmpPath);
           lResult = OK;
           break;
        }

        case MM_SET_FILE_NAME:
        {
#ifdef INTERNAL
           AuxPrint("Initializing, Filename = %d", (PSTR)((LPSTR)lParam));
#endif
           SetWindowText(GetDlgItem(hWndDlg, IDS_CLIP_FILENAME), (LPSTR)lParam);
           lResult = OK;
           break;
        }

        case MM_GET_PARTS_IN_JOB:
           lResult = (LONG) wPartsInJob;
           break;

        case MM_GET_PARTS_COMPLETE:
           lResult = (LONG) wPartsComplete;
           break;

        case MM_SET_EST_TIME:
        {
           char str[10];
#ifdef INTERNAL
           AuxPrint("Initializing, time to %ld secs", lParam);
#endif
           SetWindowText(GetDlgItem(hWndDlg, ID_DLDTIME),
                          (LPSTR)GetTimeStr(lParam, str));
           lResult = OK;
           break;
        }

        case WM_DESTROY:
            DeleteObject(hBrush);
            hDldWnd = NULL;
            return(FALSE);

        case WM_COMMAND:
            switch (wParam)
            {
                case IDCANCEL:
                    SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_APPNAME);
                    if (ErrMsg(IDS_MSG_QUERY_INTERRUPT) == IDYES)
                    {
                        hDldWnd = NULL;
                        DestroyWindow(hWndDlg);
                    }
                    break;
            }
            break;

        default:
            return(FALSE);
            break;
    } /* end switch message */
    return(TRUE);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSDldInfoDlgProc                                       */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:  1-1-90  Originally created.  [lds]                     */
/*                 5-2-91  Modiified to support selection of altcache and */
/*                         thumbnail backup directories as well - beng    */
/*                                                                        */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL 
WSDldInfoDlgProc(HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{

    char cur_dir[_MAX_PATH];

    switch(message) 
    {
        case WM_INITDIALOG:
        {
            char szDirection[MAX_STR_LEN+1];

            CenterPopUp(hWndDlg);

            switch (nSetDldInfoMode)
            {
               case DLD_INFO_MODE_CLIPART :
               {
                  /* set dir list to whatever it previously was */
                  strcpy(cur_dir, custinfo.downloadDir);

#ifdef NEVER
                  ShowWindow(GetDlgItem(hWndDlg, ID_USEALTDIR), SW_HIDE);
#endif

                  LoadString(hInstanceWS, IDS_SELECT_DIR_CLIPART,
                              (LPSTR)szDirection, sizeof(szDirection)-1);

                  SetDlgItemText(hWndDlg, IDS_DESCRIPTION, CURCLIPDESC);
                  break;
               }
#ifdef NEVER
               case DLD_INFO_MODE_ALTCACHE :
               {
                  char  szCaption[MAX_STR_LEN+1];

                  if (bShowUseAltCacheBox)
                      CheckDlgButton(hWndDlg, ID_USEALTDIR, custinfo.useAltCache);
                  else
                     ShowWindow(GetDlgItem(hWndDlg,ID_USEALTDIR), SW_HIDE);

                  LoadString(hInstanceWS, IDS_SELECT_DIR_ALT_THUMBNAIL,
                              (LPSTR)szDirection, sizeof(szDirection)-1);

                  LoadString(hInstanceWS, IDS_TITLE_ALT_THUMBNAIL,
                              (LPSTR)szCaption, sizeof(szCaption)-1);
                  SetWindowText(hWndDlg, szCaption);

                  strcpy(cur_dir, custinfo.altcache);

                  break;
               }
#endif
               default :
                  return FALSE;
               break;
            }

            SetDlgItemText(hWndDlg, ID_SELECT_DIR, (LPSTR)szDirection);

            FixSpec(cur_dir);
            SetErrorMode(1); // disable int24 message boxes
            DlgDirList(hWndDlg, cur_dir, ID_DIR, ID_CURDIR, 0xC010);
            get_cur_dir(cur_dir);
            SetErrorMode(0); // enable int24 message boxes

            EnableWindow (GetDlgItem (hWndDlg, ID_SELECT), FALSE);
            break;
        }
        case WM_COMMAND:
            switch (wParam)
            {
                case ID_HELP:
                    if (nSetDldInfoMode == DLD_INFO_MODE_CLIPART)
                        GetHelp(GH_TEXT,IDH_DLDIR);
#ifdef NEVER
                    else
                        GetHelp(GH_TEXT,IDH_LIBDRIVE);
#endif
                    break;

                case ID_DIR:
                    switch(HIWORD(lParam))
                    {
                        case LBN_DBLCLK:
                            // does the same as Select button
                            SendMessage(hWndDlg,WM_COMMAND,ID_SELECT,0L);
                            break;

                        case LBN_SELCHANGE:
                            EnableWindow (GetDlgItem (hWndDlg, ID_SELECT), TRUE);
                            break;

                        default:
                            return FALSE;
                    }
                    break;

                case ID_SELECT:
                    //  get the dir
                    DlgDirSelect(hWndDlg, cur_dir, ID_DIR);

                    // reset list box to new directory listing
                    FixSpec(cur_dir);
                    SetErrorMode(1); // disable int24 message boxes
                    DlgDirList(hWndDlg, cur_dir, ID_DIR, ID_CURDIR, 0xC010);
                    get_cur_dir(cur_dir);
                    SetErrorMode(0); // disable int24 message boxes
                    EnableWindow (GetDlgItem (hWndDlg, ID_SELECT), FALSE);
                    break;

                case IDOK:
                    /* add trailing \ */
                    get_cur_dir(cur_dir);
                    Append(cur_dir,NULL);

                    if ( !stricmp (cur_dir, szDataPath) )
                    {
                        SetDest (hWndDlg, NOBEEP, MB_OK | MB_ICONEXCLAMATION, IDS_TITLE_APPNAME);
                        ErrMsg (IDS_ERR_BACKUP_TO_DIFF);
                        EnableAllModeless(FALSE); //just in case
                        break;
                    }

                    if (nSetDldInfoMode == DLD_INFO_MODE_CLIPART)
                    {
                        lstrcpy(custinfo.downloadDir, cur_dir);
                        WriteCustInfo (WRITE_DOWNDIR);
                    }
#ifdef NEVER
                    else
                    {
                        UnFixSpec(cur_dir); // Remove trailing backslash
#ifdef INTERNAL
                        AuxPrint("New altcache (cur_dir) = %s", cur_dir);
#endif
                        strcpy(custinfo.altcache, cur_dir);
                        if (bShowUseAltCacheBox)
                           custinfo.useAltCache = IsDlgButtonChecked(hWndDlg, ID_USEALTDIR);
                        WriteCustInfo(WRITE_USECACHE);
                    }
#endif
                    EndDialog(hWndDlg, IDOK);
                    break;

                case IDCANCEL:
                    EndDialog(hWndDlg, IDCANCEL);
                    break;

                default:
                    return FALSE;
                    break;
            }
            break;

        default:
            return(FALSE);
            break;

    } /* end switch message */

    return(TRUE);
}


static void NEAR PASCAL
ComputePrices(HWND hWndDlg, int nClipIndex)
{
    char szTotal[MAX_PRICE_LEN],
         szSurCharge[MAX_PRICE_LEN],
         szTaxes[MAX_PRICE_LEN];
    int  nVerIndex;

    nVerIndex = QueryListSel(GetDlgItem(hWndDlg,IDL_VERSION));

    if (nVerIndex == -1)
        return; // empty list

    /* fill prices */
    QueryClipVerInfo(nClipIndex, nVerIndex, CURTHUMBINFO, 
        CURVERSION,
        CURWSPRICE,
        CURCLIPSIZE,
        CURCLIPFILENAME);

    /* note side effect: */
    SetDlgItemText(hWndDlg, IDS_CLIPSIZE, (LPSTR)CURCLIPSIZE);

    /* fill various prices */
    WSPrice = atof(CURWSPRICE);

    if (custinfo.whichCard == AMERICAN_EXPRESS)
        SurCharge = SURCHARGE(WSPrice);
    else
        SurCharge = 0;

    if (isWashington(custinfo.mailing.state))
        Taxes = TAXES(WSPrice+SurCharge);
    else
        Taxes = 0.0;

    Total = WSPrice + SurCharge + Taxes;

    SetDlgItemText(hWndDlg, IDS_PRICE, (LPSTR)CURWSPRICE);

    ftoa(SurCharge,szSurCharge,2);
    SetDlgItemText(hWndDlg, IDS_SURCHARGE, (LPSTR)szSurCharge);

    ftoa(Taxes,szTaxes,2);
    SetDlgItemText(hWndDlg, IDS_TAX, (LPSTR)szTaxes);

    ftoa(Total,szTotal,2);
    SetDlgItemText(hWndDlg, IDS_TOTAL, (LPSTR)szTotal);
}


#if 0
/*************************************************************\
 ** WSWhereFromDlgProc
 **
 ** ARGUMENTS:  HWND    hWndDlg
 **             unsigned message
 **             WORD    wParam
 **             LONG    lParam
 **
 ** DESCRIPTION: Where from dialog handling procedure
 **
 ** ASSUMES:     custinfo is global
 **
 ** MODIFIES:    custinfo
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Lauren Bricker  May 17, 1990 (written)
\*************************************************************/
LONG FAR PASCAL 
WSWhereFromDlgProc(HWND hWndDlg, unsigned message, WORD wParam, LONG lParam) 
{

    static WORD tmpSource;
           char szErrMsg[MAX_MSG_LEN];

    REFERENCE_FORMAL_PARAM (lParam);
    switch(message) 
    {
        case WM_INITDIALOG:
            CenterPopUp(hWndDlg);
            tmpSource = custinfo.updateFrom;
            if ( tmpSource )
                CheckRadioButton (hWndDlg, IDB_BBS, IDB_NOUPDATE, tmpSource);
            SetFocus (GetDlgItem (hWndDlg, IDOK));
            return FALSE;
            break;

        case WM_COMMAND:
            switch (wParam)
            {
                case IDB_FLOPPY:
                case IDB_BBS:
                case IDB_NOUPDATE:
                    tmpSource = wParam;
                    CheckRadioButton (hWndDlg, IDB_BBS, IDB_NOUPDATE, tmpSource);
                    break;

                case IDOK:
                    if ( tmpSource )
                    {
                        custinfo.updateFrom = tmpSource;
                        WriteCustInfo (WRITE_CUSTFROM);
                        EndDialog (hWndDlg, OK);
                    }
                    else
                        ErrMsg(IDS_ERR_MUST_SELECT);
                    return TRUE;
                    break;

                case IDCANCEL:
                    EndDialog(hWndDlg,OK);
                    return TRUE;
                    break;

                case ID_HELP:
                    GetHelp(GH_TEXT, IDH_UPDATEDB);
                    break;

                default:
                    return(FALSE);
                    break;
            }
            break;
        default:
            return(FALSE);
            break;
    } /* end switch message */
    return(TRUE);
}
#endif

static WORD
QueryDldStatus(HWND hWndDlg,ORDER *Order)
{
    CREDIT_CARD_STRUCT cc;
    BOOL nRetval;
    BOOL stat;
    HCURSOR hSaveCurs;

	hSaveCurs = SetCursor(LoadCursor(NULL, IDC_WAIT));

    if (Total > 0L)
        BusyBoxWithIcon(hInstanceWS, hWndDlg, IDS_MSG_VALIDATE_CREDIT, IDS_BUSY_CREDIT_ICON);

    memset(Order,0,sizeof(ORDER));

    Order->prodType = CLIP;

    //lstrcpy(Order->libName,CURLIB);
    Order->libID = GetFileNum(CURLIBFILE);

    //lstrcpy(Order->clipDesc,CURCLIPDESC);
    //lstrcpy(Order->clipFileName,CURCLIPFILENAME);
    lstrcpy(Order->Version,CURVERSION);
    Order->clipID = GetFileNum(CURCLIPFILENAME);

    //lstrcpy(Order->vendorName,CURPUB);
    Order->pubID = GetFileNum(CURPUBFILE);

    Order->ShipAddress = custinfo.mailing;

    /* credit card stuff */
    UnencryptCardNumber(custinfo.whichCard,&cc);
    lstrcpy(Order->ccName,cc.name);
    lstrcpy(Order->ccNum,cc.number);
    wsprintf((LPSTR)Order->ccExpDate, (LPSTR)"%02d%02d",
               cc.exp_month, cc.exp_year);
    Order->ccType = custinfo.whichCard;

    Order->Quantity = 1;
    Order->priceTotal = (long)(Total*100);
    Order->priceEach = (long)(WSPrice*100);
    Order->taxamount = (long)(Taxes*100);
    Order->shipping = 0;
    Order->surcharge = (long)(SurCharge*100);

    if (Send_Message(MSG_SET_CUR_CLIP_DLD,Order) != OK)
        END(FAIL)

    stat = Send_Message(MSG_QUERY_CLIP_DLD_OK,&nRetval);

    if (stat != OK)
        END(FAIL)

    end:

    if (Total > 0L)
        BusyClear(FALSE); 

	SetCursor(hSaveCurs);
    return nRetval;
}

static WORD
SubmitDld(void)
{
    DWORD OrderNum;
    WORD nRetval=OK;
    HCURSOR hSaveCurs;

	hSaveCurs = SetCursor(LoadCursor(NULL, IDC_WAIT));

    if (Send_Message(MSG_COMMIT_CLIP_DLD,&OrderNum) != OK)
    {
        ErrMsg (IDS_ERR_UNABLE_TO_ORDER_CLIP);
        END(FAIL)
    }

    // fix to logoff soon
    lLastMsgTime = GetTickCount() - (1000L * ((( TIMER_DISCONNECT - 1 ) * 60 ) - 30)); // 1:30

	ErrMsg(IDS_MSG_NEW_PURCHASE_ORDER,OrderNum);
   EnableAllModeless(FALSE); //just in case

    if (bConnect)
        Send_Message(MSG_SAY_HI); // to reset timer

    end:
	SetCursor(hSaveCurs);
    return nRetval;
}


static BOOL
ShowCopyright(void)
{
    FARPROC    lpproc;
    BOOL       bRetval;

    lpproc = MakeProcInstance((FARPROC)WSCopyrightDlgProc, hInstanceWS);
    bRetval = DialogBox(hInstanceWS, (LPSTR)"COPYRIGHT", hWndWS, lpproc);
    FreeProcInstance(lpproc);
    return bRetval;
}

LONG FAR PASCAL 
WSCopyrightDlgProc(HWND hWndDlg, unsigned message, WORD wParam, LONG lParam) 
{
    REFERENCE_FORMAL_PARAM(lParam);
    switch(message) {

        case WM_INITDIALOG:
            CenterPopUp(hWndDlg);
            QueryCopyright(CURPUBFILE,GetDlgItem(hWndDlg,ID_EDIT));
            SetFocus (GetDlgItem (hWndDlg, IDOK));
            return FALSE;

        case WM_COMMAND:
            switch (wParam)
            {
                case IDOK:
                    EndDialog(hWndDlg, TRUE);
                break;

                case IDCANCEL:
                    EndDialog(hWndDlg, FALSE);
                break;
            }
            break;
        default:
            return(FALSE);
    } /* end switch message */
    return(TRUE);
}

static char*
GetTimeStr(long lElapsedSeconds, char* str) // seconds
{
    char    szSeconds[6];
    char    szMinutes[6];
    char    szHours[6];
    char    szTemp[3];
    WORD    nTmp;

   lstrcpy (szTemp,"00");

   nTmp = (int) (lElapsedSeconds / SEC_PER_HR);
	itoa(nTmp, szHours, 10);
	if ( nTmp < 10 )
   {
		szTemp[1] = szHours[0];
		strcpy(szHours, szTemp);
	}

   lElapsedSeconds %= SEC_PER_HR;
	nTmp = (int) (lElapsedSeconds / SEC_PER_MIN);

	itoa(nTmp, szMinutes, 10);
	if ( nTmp < 10 )
   {
		szTemp[1]=szMinutes[0];
		strcpy(szMinutes, szTemp);
	}

	nTmp = (int) (lElapsedSeconds % SEC_PER_MIN);
	itoa(nTmp, szSeconds, 10);
	if (nTmp < 10)
   {
		szTemp[1]=szSeconds[0];
		strcpy(szSeconds, szTemp);
	}

   lstrcpy (str,szHours);
   lstrcat (str,":");
   lstrcat (str,szMinutes);
   lstrcat (str,":");
   lstrcat (str,szSeconds);

   return (str);
}
