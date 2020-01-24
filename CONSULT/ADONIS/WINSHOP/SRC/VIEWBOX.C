#include "nowin.h"
#include "windows.h"
#include "winundoc.h"
#include <string.h>
#include <stdio.h>
#include <io.h>
#include "ws.h"
#include "rc_symb.h"
#include "libids.h" /* thumbnail resource info */
#include "viewbox.h"
#include "error.h"
#include "order.h"
#include "help.h"
#include "file.h"
#include "misc.h"
#include "download.h"
#include "database.h"
#include "libmgr.h"
#include "comm.h"
#include "ksearch.h"

extern    BOOL    bNSOrderVIEW;
static    WORD    yOffset;
static    HWND    hWndSubLib=NULL;
static    char    szViewing[]="Viewing: ";
static    char    szEmptyString[]="";
static    char    szTitleName[MAX_STR_LEN];
static    BOOL    bReverseUpdate = FALSE;
static    WORD    wLastSublib = 0;
static    int     sublibNumber = 0;
static    WORD    imageNumber = 0;
static    long    lastThumbFile;

// array of flags indicating which of the currently visible images are free
// NOTE: size is hardwired!!!
#define AB_MAX 10
static    BOOL    abIsClipFree[AB_MAX];

BOOL bPromptedForBackup;
char newThumbFile[MAX_BASE_LEN + 1];

extern BOOL bKillViewbox;
LOCALHANDLE hClipIndexMemory = NULL;
static FILE*  fClipFileStream = NULL;

VBS     vbs;

static WORD wPrevSelect = 0;
static int nPrevScrollPos = -1;

typedef fpos_t  FILE_OFFSET;
typedef fpos_t *PFILE_OFFSET;  /* to store offsets into thumbinfo file */
typedef FILE*  FILE_HANDLE;

static int NEAR PASCAL InitNewLibrary(HWND hWndDlg, BOOL bReInit, WORD wImage);
void FAR PASCAL UnloadRCData(LPSTR lpData);
void NEAR PASCAL UnderlineThumbnails(WORD, VBS *, int, int, int, HWND);
void NEAR PASCAL MoveHighlight(HWND, WORD, LONG, VBS *);

int    NEAR PASCAL SetupClipIndexes(int nNumBitmaps);
FILE*  NEAR PASCAL OpenClipFile(char *szThumbInfoFile);
void   NEAR PASCAL CloseClipFile(void);
int    NEAR PASCAL GetClipDesc(int index, char *szClipDesc);
int    NEAR PASCAL IsClipFree(int index);
int    NEAR PASCAL HighlightSelection(WORD wImageOffset, HDC hDC, BOOL bDraw);

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  CreateViewBox    ÿ                                     */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
VOID FAR PASCAL 
CreateViewBox (int whichSublib, WORD whichImage)
{
    /*----------------------------------------------------------------*/
    /* Store the name of the selected library and its listbox index.  */
    /*----------------------------------------------------------------*/
    lstrcpy(szTitleName, CURLIB);
    sublibNumber = whichSublib;
    imageNumber  = whichImage - 1;      // 0 - based indexing

    if (!hWndView)
    {
        lastThumbFile = 0L;
        wLastSublib = whichSublib;
        FREEZE_HWND_WS
        CreateDialog(hInstanceWS, (LPSTR)"CLIPVIEW", hWndWS, lpprocViewDlg);
    }
    else 
    {
        // only load the library if its different from the last one...
        if ( lastThumbFile != GetFileNum(newThumbFile) )
        {
            if ( InitNewLibrary (hWndView, TRUE, imageNumber) == FAIL )  // was FALSE
            {
                SendMessage (hWndSubLib, LB_SETCURSEL, wLastSublib, 0L);
                /* Kludge to reset the focus in the list box */
                InvalidateRect (hWndSubLib, (LPRECT)NULL, TRUE);
                UpdateWindow (hWndSubLib);
            }
            wLastSublib = whichSublib;
        }
        else   // just change which picture in this sublib
        {
            vbs.nSelect = imageNumber;

            //scroll the window if the new selection is not currently visible
            if ((vbs.nSelect < (int)vbs.nScrollPos) OR
                (vbs.nSelect >= (int)(vbs.nScrollPos + vbs.nMaxVisible)))
            {
                  vbs.nScrollPos = min ((int)vbs.nSelect, vbs.nMaxScrollPos);
                  SetScrollPos(vbs.hWndScroll, SB_CTL, vbs.nSelect, TRUE);
            }
            InvalidateRect (vbs.hWndList, (LPRECT)NULL, TRUE);
            UpdateWindow (vbs.hWndList);
        }
    }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*      Function:  WSViewBoxDlgProc                                       */
/*                                                                        */
/*   Description:                                                         */
/*                                                                        */
/*       History:                                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/
LONG FAR PASCAL 
WSViewBoxDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam)
{
    HBITMAP    hBitmap;
    HDC        hDCMem;

    static    HWND    hWndClipartName;
    static    HWND    hWndLibrary;

    static    BOOL    bCompress=FALSE;
    static    HBRUSH hBrush;  /* Need this for processing WM_CTLCOLOR msgs */

    WORD        ii;
    WORD        nNewSelect;
    WORD        rHeight;
    HBITMAP     hOld;
    POINT       mpt;
    HCURSOR     hSaveCurs;
    int         abx;

    int nCount, nTarget, nInc;
    switch(message) 
    {
        case WM_INITDIALOG:

	         hSaveCurs = SetCursor(LoadCursor(NULL, IDC_WAIT));
            CenterPopUp(hWndDlg);
            hWndView = hWndDlg;
            hMommaWnd = hWndView;
            hClipIndexMemory = NULL;
            nPrevScrollPos = -1;

            //Make search dlg visible when Viewbox has focus
            if (IsWindow(hWndSearch))
               SetWindowWord(hWndSearch, GWW_HWNDPARENT, hWndDlg);

            hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));

            /* Map window to library, can't use SetWindowWord, because it is */
            /* a dialog box and they don't have any extra words.             */
            vbs.hWndList = GetDlgItem(hWndDlg, ID_LIST);
            GetWindowRect(vbs.hWndList, (LPRECT)&vbs.rList);

            /*---------------------------------------------------------------------*/
            /* Subtract out the frame from the client area, so updates don't cause */
            /* frame to be redrawn.                                                */
            /*---------------------------------------------------------------------*/
            vbs.rList.left += 1;
            vbs.rList.right -= 1;

            /*-----------------------------------------------*/
            /* If the bmp list is taller than the height of  */
            /* the bitmaps, need to center the bitmaps       */
            /* vertically.                                   */
            /*-----------------------------------------------*/
            if (HEIGHT_RECT(vbs.rList) > HEIGHT_BITMAP) 
                yOffset = (HEIGHT_RECT(vbs.rList)-HEIGHT_BITMAP)/2;
            else
            {
                /*-----------------------------------------*/
                /* HEIGHT_RECT(vbs.rList) <= HEIGHT_BITMAP */
                /*-----------------------------------------*/
                bCompress=TRUE;
                yOffset = 2;
            }

            vbs.hWndScroll = GetDlgItem(hWndDlg, ID_SCROLL);
            hWndClipartName = GetDlgItem(hWndDlg, IDS_CLIPART_NAME);
            hWndLibrary = GetDlgItem(hWndDlg, IDS_LIBRARY);
            hWndSubLib = GetDlgItem(hWndDlg, IDL_SUBLIBRARIES);

            if (InitNewLibrary(hWndDlg, TRUE, imageNumber) == FAIL)
            {
                SetCursor(hSaveCurs);
                PostMessage(hWndDlg, WM_CLOSE, 0, 0L);
            }
            else
            {
                /* if we got here, stick the file in the LRU */
                if (SearchLRUTree(CURLIBFILE, &hLRU, FALSE) == FAIL)
                {
                    SetCursor(hSaveCurs);
                    IssueErr() ;
                    if (RemakeLRU() != OK)
                        return FALSE ;
                }
                /* LRU table is updated */
                ShowWindow (hWndDlg, SW_SHOW);
                EnableWindow (hWndDlg, TRUE);
			       SetFocus (GetDlgItem (hWndDlg, ID_SCROLL));
            }
            SetCursor(hSaveCurs);
            return FALSE; // Init changes focus

        case WM_SETFOCUS:
            hMommaWnd = hWndView;
            return (FALSE);
        break;

        case WM_MOVE:
            GetWindowRect(GetDlgItem(hWndDlg, ID_LIST), (LPRECT)&vbs.rList);
            nPrevScrollPos = -1;
            break;

        case WM_HSCROLL: 
            UpdateVBScroll(wParam, lParam, &vbs);
            break;

        case WM_CTLCOLOR:
           if(LOWORD(lParam) != GetDlgItem(hWndDlg, ID_LIST))
           {
               //kludge for actual Change of color to get list to redraw
               if (LOWORD(lParam) == GetDlgItem(hWndDlg, IDS_PUBLISHER))
               {
#ifdef INTERNAL
           AuxPrint("Processing CTLCOLOR in Viewbox, for Child Window 0");
#endif
                  nPrevScrollPos = -1;
               }
               return(FALSE);
           }

           hDCMem  = CreateCompatibleDC(wParam);

           if (bReverseUpdate)
           {
               nCount = vbs.nScrollPos+vbs.nMaxVisible-1;
               nTarget = vbs.nScrollPos - 1;
               nInc = -1;
           }
           else
           {
               nCount = vbs.nScrollPos;
               nTarget = vbs.nScrollPos+vbs.nMaxVisible;
               nInc = 1;
           }

           SetBkColor(wParam, GetSysColor(COLOR_HIGHLIGHT));
           SetTextColor(wParam, GetSysColor(COLOR_HIGHLIGHTTEXT));

           //Unhighlight old selection if visible
           if (wPrevSelect)  //Non-zero value means its visible
           {
               HighlightSelection(wPrevSelect - 1, wParam, FALSE);
               wPrevSelect = 0;
           }

           for(ii = nCount;
               ii != (WORD)nTarget;
               ii += nInc)
           {
                //only draw/erase bitmaps and "FREE"'s if window is scrolled
                if (nPrevScrollPos != vbs.nScrollPos)
                {
                    // Determine which displayed image we are dealing with
                    // (relative to the zeroth position)
                    if (nInc == -1)
                        abx = max(0, min(AB_MAX-1, vbs.nMaxVisible-abs(ii-nCount)-1));
                    else
                        abx = max(0, min(AB_MAX-1, abs(ii - nCount)));

                    /* erase any previously drawn "FREE"'s */
                    if (abIsClipFree[abx])
                    {
                       DWORD dwPrevTextColor = SetTextColor(wParam, GetSysColor(COLOR_WINDOW));
                       DWORD dwPrevBkColor = SetBkColor(wParam, GetSysColor(COLOR_WINDOW));

                       TextOut(wParam,
                               ((ii-vbs.nScrollPos) * (WIDTH_BITMAP+DELTA_X)+X_START) + 16,
                               yOffset - 20,
                               (LPSTR)"FREE", 4);
                       abIsClipFree[abx] = FALSE;

                       SetTextColor(wParam, dwPrevTextColor);
                       SetBkColor(wParam, dwPrevBkColor);
                    }

                    hBitmap = LoadBitmap(vbs.hLib, lpBMP_ID(ii));
                    if (!hBitmap)
                    {
                        /* !!! THIS IS A BAD ERROR, WHAT TO DO */
                        ErrMsg(IDS_ERR_BAD_BITMAP);
                        goto EXIT_CTLCOLOR;
                    }

                    /* First time through, store the handle to the default bitmap */
                    if (ii == (WORD)nCount)
                        hOld = SelectObject(hDCMem, hBitmap);
                    else
                        SelectObject(hDCMem, hBitmap);

                    /* Now blt the bitmap */
                    if(bCompress)
                    {
                        /*---------------------------------------------*/
                        /* If the rectangle in which these bitmaps are */
                        /* being displayed is not at least the height  */
                        /* of the bitmaps (64 pixels), then we are     */
                        /* going to shrink the bitmap on the blt.      */
                        /* The dimensions of the shrunken bitmap will  */
                        /* be N x N where N is the height of the       */
                        /* actual rectangle.                           */
                        /*---------------------------------------------*/
                        rHeight = HEIGHT_RECT(vbs.rList)-6;
                        StretchBlt(wParam, ((ii-vbs.nScrollPos)*(rHeight+DELTA_X)+X_START),
                            yOffset, rHeight, rHeight, hDCMem, 0, 0, WIDTH_BITMAP, HEIGHT_BITMAP, NOTSRCCOPY);
                    }
                    else
                    {
                        BitBlt(wParam, ((ii-vbs.nScrollPos)*(WIDTH_BITMAP+DELTA_X)+X_START), yOffset,
                            WIDTH_BITMAP, HEIGHT_BITMAP, hDCMem, 0, 0, NOTSRCCOPY);
                    }

                    /* Following step is necessary so we can delete the bitmap */
                    SelectObject(hDCMem, hOld);
                    DeleteObject(hBitmap);

                    // Underline Thumbnails if Member of a Group
                    UnderlineThumbnails(wParam, &vbs, ii, nCount, nTarget-nInc, hWndDlg);

                    /* Get price of clipart and if its free, lets show them */
                    if (CURLIBDLDABLE)
                    {
                       switch ( IsClipFree(ii) )
                       {
                          case TRUE :
                          {
                               DWORD dwPrevBkColor = SetBkColor(wParam, GetSysColor(COLOR_WINDOW));
                               DWORD dwPrevTextColor = SetTextColor(wParam, GetSysColor(COLOR_WINDOWTEXT));

                               TextOut(wParam, ((ii - vbs.nScrollPos)*(WIDTH_BITMAP+DELTA_X)+X_START) + 16,
                                        yOffset - 20, (LPSTR)"FREE", 4);
                               abIsClipFree[abx] = TRUE;

                               SetBkColor(wParam, dwPrevBkColor);
                               SetTextColor(wParam, dwPrevTextColor);
                               break;
                          }
                          case FAIL :   //error parsing thumbinfo file
                          {
                               ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                                      (LPSTR)StripPath (CURTHUMBINFO));
                               DeleteDC(hDCMem);
                               PostMessage(hWndDlg, WM_CLOSE, 0, 0L);
                               return((DWORD)hBrush);
                               break;
                          }

                          case FALSE:
                          default:
                               break;
                       }    /* end switch */
                    }       /* end if CURLIBDLDABLE */
                }        // if window scrolled

                // Change clip description if drawing selcted
                if (ii == vbs.nSelect)
                {
                    char Buffer[MAX_STR_LEN];

                    if (GetClipDesc(ii, Buffer) == FAIL)
                    {
                         ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                                (LPSTR)StripPath (CURTHUMBINFO));
                         DeleteDC(hDCMem);
                         PostMessage(hWndDlg, WM_CLOSE, 0, 0L);
                         return((DWORD)hBrush);
                    }
                    FixString (Buffer, CURCLIPDESC, sizeof (CURCLIPDESC));
                    SetWindowText(hWndClipartName, (LPSTR)CURCLIPDESC);
                }
            }  //  End For Loop

            //set new nPrevScrollPos
            nPrevScrollPos = vbs.nScrollPos;

            //Add frame to selected bitmap if visible
            if ((vbs.nSelect >= vbs.nScrollPos) AND
                (vbs.nSelect < (vbs.nScrollPos + vbs.nMaxVisible)))
            {
                HighlightSelection(vbs.nSelect - vbs.nScrollPos, wParam, TRUE);
                wPrevSelect = (WORD)(vbs.nSelect - vbs.nScrollPos + 1);
            }

EXIT_CTLCOLOR:
            DeleteDC(hDCMem);
            return((DWORD)hBrush);
            break;

        case WM_LBUTTONDOWN:
            mpt.x = LOWORD(lParam);
            mpt.y = HIWORD(lParam);
            rHeight = HEIGHT_RECT(vbs.rList)-6;
            ClientToScreen(hWndDlg, &mpt);
            if(mpt.x >= vbs.rList.left && mpt.x <=vbs.rList.right &&
                mpt.y >= vbs.rList.top && mpt.y <= vbs.rList.bottom) {
                    /*--------------------------------------------*/
                    /* Calculation of which one is being selected */
                    /*--------------------------------------------*/

                    /* Improve mapping of mouseclick to a thumbnail..pt 6/91 */
                    if (!bCompress)
                        //nNewSelect=vbs.nScrollPos + ((mpt.x-vbs.rList.left)/(DELTA_X/2+WIDTH_BITMAP+X_START));
                        nNewSelect=vbs.nScrollPos + (mpt.x-vbs.rList.left-X_START)/(DELTA_X+WIDTH_BITMAP);
                    else
                        nNewSelect=vbs.nScrollPos + (mpt.x-vbs.rList.left)/(DELTA_X+HEIGHT_RECT(vbs.rList)-2);

                    /* nNewSelect is 0-based, MaxVis is not */
                    if (nNewSelect >= vbs.nNumBitmaps)
                        break;

                    if(nNewSelect < vbs.nSelect)
                        bReverseUpdate = TRUE;
                    else
                        bReverseUpdate = FALSE;

                    if(nNewSelect!=vbs.nSelect) {
                        vbs.nSelect=nNewSelect;

                        /*--------------------------------------------------*/
                        /* Force an update of everything that needs to be. */
                        /*-------------------------------------------------*/
                        InvalidateRect(vbs.hWndList, (LPRECT)NULL, TRUE);
                        UpdateWindow(vbs.hWndList);
			               SetFocus (GetDlgItem (hWndDlg, ID_SCROLL));
                    }
            }  /* if mouse in rList rect */
            break;

END_DIALOG:
        case WM_CLOSE:
            wPrevSelect = 0;

            if (vbs.hLib)
            {
                FreeLibrary(vbs.hLib);
                vbs.hLib=NULL;
            }

            if (hClipIndexMemory != NULL)
            {
#ifdef INTERNAL
               AuxPrint("About to free, Size of hClipIndexMemory is %d bytes",
                           LocalSize(hClipIndexMemory));
#endif
                hClipIndexMemory = LocalFree(hClipIndexMemory);
#ifdef INTERNAL
                if (hClipIndexMemory != NULL)
                     AuxPrint("LocalFree failed");
#endif
            }

            CloseClipFile();
            DeleteObject(hBrush);

            THAW_HWND_WS

           // Fix up the front screen stuff if we started a search
           if ( bFixAfterFind )
           {
                /* fill main window list boxes */
                bFixAfterFind = FALSE;
                FillListBoxes (hWndWS, TRUE);
           }

           if (IsWindow(hWndSearch))
              // If Search dlg is up it is now a child of viewbox and will be
              // destroyed here
              SendMessage(hWndSearch, WM_CLOSE, 0, 0L);

            hWndView=NULL;
            DestroyWindow(hWndDlg);
        break;

        case WM_COMMAND:
            if( HIWORD(lParam) == 1)  //WSView Accelerator
            {
               if(wParam == ID_PLUS || wParam == ID_MINUS)
               {
                  MoveHighlight(hWndDlg, wParam, lParam, &vbs);
	               SetFocus (GetDlgItem (hWndDlg, ID_SCROLL));
                  return(TRUE);
               }
            }

            switch(wParam)
            {
                case ID_HELP:
		            GetHelp(GH_TEXT,IDH_VIEW);
                break;

                case IDCANCEL:
                    goto END_DIALOG;

                case ID_SEARCH:
                    StartSearch (hWndDlg);
                break;

                case IDL_SUBLIBRARIES:
                    switch(HIWORD(lParam)) 
                    {
                        case LBN_SELCHANGE:
                        {
                           char szSubLib[MAX_STR_LEN];
                           HCURSOR hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
                           int nIndex = QueryListSel(hWndSubLib); /* (NOTE: QueryListSel has side effect we want for SELCHANGE) */
                           if (nIndex != (int)wLastSublib)
                           {
	                           SendMessage(hWndSubLib, LB_GETTEXT, nIndex, (LONG)(LPSTR)szSubLib);
                              if (strcmp(szSubLib, "None"))
                              {
                                  if (InitNewLibrary(hWndDlg, FALSE, 0) == FAIL)
                                  {
                                      SendMessage(hWndSubLib, LB_SETCURSEL, wLastSublib, 0L);
                                      /* Kludge to reset the focus in the list box */
                                      InvalidateRect(hWndSubLib, (LPRECT)NULL, TRUE);
                                      UpdateWindow(hWndSubLib);
			                             SetFocus (GetDlgItem (hWndDlg, ID_SCROLL));
                                  }
                                  else
                                      wLastSublib = nIndex;
                              }
                           }
                           SetCursor(hCursor);
                           break;
                        }
                    }
                break;

                case ID_DOWNLOAD:
                    if ( !CURLIBDLDABLE )
                    {
                        ErrMsg (IDS_ERR_NOT_DOWNLOADABLE);
                        SetFocus(GetDlgItem(hWndDlg, ID_ORDER));
                        break;
                    }

                    //ShowWindow(hWndDlg,SW_HIDE);
                    DownloadSingle();
                    //ShowWindow(hWndDlg,SW_SHOW);
                    break;

                case ID_ORDER:
                    CreateOrder();

                    if(bKillViewbox)
                    {
                       bKillViewbox = FALSE;
                       PostMessage(hWndDlg,WM_CLOSE,0,0L);
                    }
                    else
                    {
   			           SetFocus (GetDlgItem (hWndDlg, ID_DOWNLOAD));
                    }
                    break;

                case IDM_NEW:
                    if (InitNewLibrary(hWndDlg,TRUE, imageNumber) == FAIL)
                    {
                        PostMessage(hWndDlg,WM_CLOSE,0,0L);
                    }
                break;

                default:
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
/*      Function:  UpdateVBScroll                                         */
/*                                                                        */
/*   Description:  Determines whether or not the bitmap list scroll bar   */
/*                 needs to be updated and if so calculates the new       */
/*                 scroll position.                                       */
/*                                                                        */
/*       History:  Created [lori]                                         */
/*                 12/16/89                                               */
/*------------------------------------------------------------------------*/
BOOL NEAR PASCAL 
UpdateVBScroll(WORD wParam, LONG lParam, VBS *vbs)
{
    SWORD    nTemp;

    switch(wParam) {
        case SB_LINEDOWN:
            nTemp=vbs->nScrollPos+1;
            break;
        case SB_LINEUP:
            nTemp=vbs->nScrollPos-1;
            break;
        case SB_PAGEDOWN:
            nTemp=vbs->nScrollPos+vbs->nPageInc;
            break;
        case SB_PAGEUP:
            nTemp=vbs->nScrollPos-vbs->nPageInc;
            break;
        case SB_TOP:
            nTemp=0;
            break;
        case SB_BOTTOM:
            nTemp=vbs->nNumBitmaps-(vbs->nMaxVisible+1);
            break;
        case SB_THUMBPOSITION:
            nTemp=LOWORD(lParam);
            break;
        default:
            goto EXIT;
    } /* end switch */


    if(nTemp > vbs->nMaxScrollPos)
        nTemp=vbs->nMaxScrollPos;
    else if(nTemp < 0)
        nTemp=0;

    if (nTemp != vbs->nScrollPos) {
        vbs->nScrollPos=nTemp;
        SetScrollPos(HIWORD(lParam), SB_CTL, nTemp, TRUE);
        InvalidateRect(vbs->hWndList, (LPRECT)NULL, TRUE);
        UpdateWindow(vbs->hWndList);
    }
EXIT:
    return TRUE;
}


/*************************************************************\
 ** LoadRCData
 **
 ** ARGUMENTS:
 **  hLib;  // handle to library
 **  nID;   // ID of item in library's resource.
 **
 ** DESCRIPTION:
 **  Gets the handle to the resource item nID found in the hLib 
 **  library module.  Locks the resource and returns the pointer
 **  to the data.
 **  
 **  Caller must unlock and free the resource by calling UnloadRCData().
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  Pointer to data or NULL.
 **
 ** HISTORY:    Lori Sargent Created 12/16/89
 **  Doug Kent  March 29, 1990 changed documentation
\*************************************************************/
LPSTR    FAR PASCAL 
LoadRCData(HANDLE hLib, WORD nID) 
{
    HANDLE    hResource;
    HANDLE    hResData;
    LPSTR     lpData=NULL;

    SetErrorMode(1); // disable int24 message boxes
    hResource=FindResource(hLib, szBITMAP_GINFO, szBITMAP_GINFO);
    if(hResource) {
        hResData=LoadResource(hLib, hResource);
        if(hResData)
            lpData=LockResource(hResData);
    }
    SetErrorMode(0);
    return(lpData);
}

void FAR PASCAL 
UnloadRCData(LPSTR lpData)
{
    HANDLE hResource = LOWORD(GlobalHandle(HIWORD((LONG)lpData)));
    UnlockResource(hResource);
    FreeResource(hResource);
}

/*************************************************************\
 ** InitNewLibrary
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
 ** HISTORY:   Doug Kent  May 17, 1990 (written)
\*************************************************************/
static int NEAR PASCAL
InitNewLibrary(HWND hWndDlg, BOOL bNewLib, WORD wImage)
{
    char        szTitle[MAX_STR_LEN+10];
    char        szNumImages[5];
    LPSTR       lpData;
    int nRetval=OK;

    //this will cause re-drawing of bitmaps
    nPrevScrollPos = -1;

    EnableWindow(GetDlgItem(hWndDlg, ID_DOWNLOAD), TRUE);

    lstrcpy(szTitle, szViewing);
    lstrcat(szTitle, szTitleName);
    SetDlgItemText(hWndDlg, IDS_LIBRARY, szTitleName);
    SetDlgItemText(hWndDlg, IDS_PUBLISHER, (LPSTR)CURPUB);
    SetWindowText(hWndDlg, (LPSTR)szTitle);
	 SetDlgItemText(hWndDlg, IDS_COPYRIGHTER, (LPSTR)CURPUB);

    if (bNewLib)
    {
        SendMessage(hWndSubLib,LB_RESETCONTENT,0,0L);
        if (EnumSubLibs(AddStringToListBox, 
                    hWndSubLib,
                    CURLIBFILE) == 0)
        {
            ErrMsg(IDS_ERR_LIB_NOTAVAIL);
            END(FAIL)
        }
        SendMessage(hWndSubLib,LB_SETCURSEL,sublibNumber,0L);
        SetFocus(GetDlgItem(hWndDlg,ID_ORDER));
    }

    if (QueryThumbnailFileName((WORD)SendMessage(hWndSubLib,LB_GETCURSEL,0,0L),
                    CURLIBFILE, 
                    CURTHUMBNAIL) != OK)
    {
        ErrMsg(IDS_ERR_LIB_NOTAVAIL);
        END(FAIL)
    }
    if (QueryThumbInfoFileName((WORD)SendMessage(hWndSubLib,LB_GETCURSEL,0,0L),
                    CURLIBFILE, 
                    CURTHUMBINFO) != OK)
    {
        ErrMsg(IDS_ERR_LIB_NOTAVAIL);
        END(FAIL)
    }

    if (bNULLFILE(CURTHUMBNAIL) OR bNULLFILE(CURTHUMBINFO))
    {
        ErrMsg(IDS_ERR_LIB_NOTAVAIL);
        END(FAIL)
    }

    /*--------------------------------------------*/
    /* Download files from the BBS (if necessary) */ 
    /*--------------------------------------------*/
    /* it is important to the backup process in LoadFile() that we are
       doing the thumbres file before the thumbinfo file. (9.26.90) D. Kent */

    bPromptedForBackup = FALSE;

    if (LoadFile(CURTHUMBNAIL) != OK)
        END(FAIL)
    if (LoadFile(CURTHUMBINFO) != OK)
        END(FAIL)

    if (vbs.hLib)
    {
        FreeLibrary(vbs.hLib);
        vbs.hLib=NULL;
    }

    if ((vbs.hLib=LoadLibrary((LPSTR)CURTHUMBNAIL)) < 32)     
    {
        ErrMsg(IDS_FILE_IO_ERR, ID_ERR_LOADING_RES_LIB,
               (LPSTR)StripPath (CURTHUMBNAIL)) ;
        vbs.hLib=NULL;
        END(FAIL)
    }

    lpData=LoadRCData(vbs.hLib, BITMAP_INFO);
    if (!lpData) 
    {
        ErrMsg(IDS_FILE_IO_ERR,ID_ERR_INVALID_LIBRARY,
               (LPSTR)StripPath (CURTHUMBNAIL)) ;
        END(FAIL)
        //SendMessage(hWndDlg, WM_CLOSE, 0, 0L);
    }
    else
    {
        vbs.nNumBitmaps= *(WORD FAR *)lpData;
        //vbs.nNumClips = *(((WORD FAR *)lpData)+1);

        /* Free the resource */
        UnloadRCData(lpData);
    }

    vbs.nMaxVisible = (WIDTH_RECT(vbs.rList))/(WIDTH_BITMAP+DELTA_X);
    vbs.nMaxVisible = min(vbs.nMaxVisible, (int)vbs.nNumBitmaps);
    vbs.nPageInc = vbs.nMaxVisible-1;
    vbs.nMaxScrollPos = vbs.nNumBitmaps-vbs.nMaxVisible;
    vbs.nSelect = wImage;
    vbs.nScrollPos = min((int)wImage, vbs.nMaxScrollPos);
    SetScrollRange(vbs.hWndScroll, SB_CTL, 0, vbs.nMaxScrollPos, TRUE);
    SetScrollPos(vbs.hWndScroll, SB_CTL, vbs.nSelect, TRUE);

    /* We used to quote here the number of DOWNLOADABLE FILES IN LIB which
       of course counted one underlined group of 7 images as one image 
       (vbs.nNumClips instead of vbs.nNumBitmaps).  We more appropriately 
       inform them now how many images there are total ..pt 6/91 */
    itoa(vbs.nNumBitmaps, szNumImages, 10);
    SetDlgItemText(hWndDlg, IDS_NUM_IMAGES, (LPSTR)szNumImages);

    /****************************************** KRG *
    *
    *  Open Thumbnail File and setup indexes into it..
    *
    ************************************************/
    CloseClipFile();
    if ((fClipFileStream = OpenClipFile(CURTHUMBINFO)) == NULL)
    {
        ErrMsg(IDS_ERR_LIB_NOTAVAIL);
        END(FAIL)
    }
    if (SetupClipIndexes(vbs.nNumBitmaps) != OK)
    {
        IssueErr();
        END(FAIL)
    }

    if (!bNewLib) /* erase listbox */
    {
        HBRUSH    hBrushBkg,hOldBrush;
        HDC hDC;
        RECT tmpRect;
        tmpRect = vbs.rList;
        tmpRect.top +=2;
        tmpRect.bottom -=2;
        hDC = GetDC(vbs.hWndList);
        hBrushBkg=CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        hOldBrush = SelectObject(hDC,hBrushBkg);
        ScreenToClient(vbs.hWndList,(LPPOINT)&tmpRect);
        ScreenToClient(vbs.hWndList,((LPPOINT)&tmpRect)+1);
        FillRect(hDC, (LPRECT)&tmpRect, hBrushBkg);
        SelectObject(hDC,hOldBrush);
        DeleteObject(hBrushBkg);
        ReleaseDC(vbs.hWndList,hDC);

        //Since the listbox is being erased, frame is no longer visible
        wPrevSelect  = 0;
    }

    InvalidateRect(vbs.hWndList, (LPRECT)NULL, TRUE);
    UpdateWindow(vbs.hWndList);

    #if 0
    if(bNewLib) {
        /* Force the selected bitmap to change to zero */
        SendMessage(hWndDlg, WM_LBUTTONDOWN, 0, MAKELONG(1,1));
    }
    #endif

    end:

    lastThumbFile = GetFileNum(CURTHUMBNAIL);
    EnableWindow(hWndView, TRUE);
	 if ( IsWindow (hWndDlg) && IsWindowVisible (hWndDlg) )
	 	SetFocus (hWndDlg);
    return nRetval;
}

/********************************************************* KRG **
*
*   This routine underlines each thumbnail according to where
*   it may fall within a group, if it does at all.  It also 
*   attempts to minimize the redrawing of bits and therefore
*   flashing.
*
*****************************************************************/

void NEAR PASCAL 
UnderlineThumbnails(WORD hDC,
                    VBS  *vbs,
                    int  ii,
                    int  start,
                    int  end,
                    HWND hWndDlg)
{
    static HPEN        hColorPen, hOldPen;
    short       nDrawMode;
    DWORD       dwBkColor;
    int         xCur;
    static      LPSTR     lpRCData;
    static      LPSTR     lpRCDataSave;
    int         nYOffset = yOffset + Y_SPACING;

    if (ii == start)
    {
       /************************************** KRG ***
       *
       *    First time through, create pens
       *    Lock Resource
       *
       **********************************************/
       lpRCDataSave=LoadRCData(vbs->hLib, BITMAP_INFO);
       if (!lpRCDataSave)
       {
         ErrMsg(IDS_FILE_IO_ERR,ID_ERR_INVALID_LIBRARY,
                  (LPSTR)StripPath(CURLIBFILE));
         return;
       }

       nDrawMode = GetROP2(hDC);
       dwBkColor = GetSysColor(COLOR_WINDOW);
       hColorPen = CreatePen(0, LINE_SIZE, dwBkColor);
       hOldPen   = SelectObject(hDC, hColorPen);

    }

    if(ii==vbs->nScrollPos || (bReverseUpdate && ii!=vbs->nScrollPos))
    {
        // Skip over the 4 byte containing No of BitMaps     
        lpRCData = lpRCDataSave + sizeof(WORD) * 2;
        // Line up with correct Group Code in resource
        lpRCData += ii;
    }

    xCur = (ii - vbs->nScrollPos) * (WIDTH_BITMAP + DELTA_X) + X_START;

    if (bIS_INGROUP(*lpRCData))
    {
        if (*lpRCData == FIRST_IN_GROUP) 
        {
            /********************************************
            *
            *  Draw Opening Bracket, Underline Bitmap,
            *  delete any closing bracket
            *
            *********************************************/
            SetROP2(hDC, R2_NOTCOPYPEN);
            MoveTo(hDC, xCur - BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP - INTER_BRACKET);
            LineTo(hDC, xCur - BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
            LineTo(hDC, xCur + WIDTH_BITMAP + (DELTA_X/2),
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
    
            SetROP2(hDC, R2_COPYPEN);
            MoveTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET - LINE_SIZE);
            LineTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP - INTER_BRACKET - 1);
    
            MoveTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET + (DELTA_X/2),
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET - LINE_SIZE);
            LineTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET + (DELTA_X/2),
                    nYOffset + HEIGHT_BITMAP - INTER_BRACKET - 1);

            if (ii==vbs->nScrollPos && (LINE_SIZE == 1))
            {
                MoveTo(hDC, xCur - BRACKET_OFFSET - 1,
                    nYOffset + HEIGHT_BITMAP - INTER_BRACKET);
                LineTo(hDC, xCur - BRACKET_OFFSET - 1,
                        nYOffset + HEIGHT_BITMAP + EXTER_BRACKET + LINE_SIZE);
            }
            else
            {
                MoveTo(hDC, xCur - BRACKET_OFFSET - 2,
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
                LineTo(hDC, xCur - BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
            }
        }
        else if(*lpRCData == LAST_IN_GROUP )
        {
            /********************************************
            *
            *  Underline Bitmap,
            *  Draw Closing bracket
            *
            *********************************************/
            SetROP2(hDC, R2_NOTCOPYPEN);
            MoveTo(hDC, xCur - (DELTA_X/2),
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
            LineTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
            LineTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP - INTER_BRACKET - 1);
    
            SetROP2(hDC, R2_COPYPEN);
            MoveTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET + LINE_SIZE,
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
            LineTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET + LINE_SIZE + 1,
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
    
            // This is to clean up a little ugly that shows up
            // before an opening bracket & 1st position
            if (ii==vbs->nScrollPos)
            {
                MoveTo(hDC, xCur - BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP - INTER_BRACKET);
                LineTo(hDC, xCur - BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
            }
        }
        else if(*lpRCData == IN_GROUP)
        {
            /********************************************
            *
            *  Underline Bitmap,
            *  Delete Closing bracket
            *
            *********************************************/
    
            SetROP2(hDC, R2_NOTCOPYPEN);
            MoveTo(hDC, xCur - (DELTA_X/2),
                nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
            LineTo(hDC, xCur + WIDTH_BITMAP + (DELTA_X/2),
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
    
            SetROP2(hDC, R2_COPYPEN);
            MoveTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET - LINE_SIZE);
            LineTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP - INTER_BRACKET - 1);
    
            MoveTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET + (DELTA_X/2),
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET - LINE_SIZE);
            LineTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET + (DELTA_X/2),
                    nYOffset + HEIGHT_BITMAP - INTER_BRACKET - 1);
    
            // This is to clean up a little ugly that shows up
            // before an opening bracket & 1st position
            if (ii==vbs->nScrollPos)
            {
                MoveTo(hDC, xCur - BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP - INTER_BRACKET);
                LineTo(hDC, xCur - BRACKET_OFFSET,
                    nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
            }
        }
    }
    else  // Delete Old Lines that might be there
    {
        SetROP2(hDC, R2_COPYPEN);
        MoveTo(hDC, xCur - BRACKET_OFFSET,
            nYOffset + HEIGHT_BITMAP - INTER_BRACKET);
        LineTo(hDC, xCur - BRACKET_OFFSET,
            nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);

        if (ii==vbs->nScrollPos)
        {
            MoveTo(hDC, xCur - (DELTA_X/2) - BRACKET_OFFSET + LINE_SIZE + 2,
                nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
        }
        else
        {
            MoveTo(hDC, xCur - (DELTA_X/2) - BRACKET_OFFSET + LINE_SIZE,
                nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);
        }
        LineTo(hDC, xCur + WIDTH_BITMAP + (DELTA_X/2) + BRACKET_OFFSET + 1,
            nYOffset + HEIGHT_BITMAP + EXTER_BRACKET);


        MoveTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET,
            nYOffset + HEIGHT_BITMAP + EXTER_BRACKET - LINE_SIZE);
        LineTo(hDC, xCur + WIDTH_BITMAP + BRACKET_OFFSET,
            nYOffset + HEIGHT_BITMAP - INTER_BRACKET - 1);
    }

    // Group Codes found in sequence, every two bytes
    ++lpRCData;

    SetROP2(hDC, nDrawMode);

    if (ii == end)
    {
      /******************************* KRG *************
      *
      *    Looks like were done, so lets free the
      *    Resource and delete any objects created
      *
      *************************************************/
      UnloadRCData(lpRCDataSave);
      hColorPen = SelectObject(hDC, hOldPen);
      DeleteObject(hColorPen);
    }
}

void NEAR PASCAL 
MoveHighlight(HWND hWndDlg, WORD wParam, LONG lParam, VBS  *vbs)
{
   REFERENCE_FORMAL_PARAM(hWndDlg);
   REFERENCE_FORMAL_PARAM(lParam);

   switch(wParam)
   {
      case ID_MINUS:
            if ((int)vbs->nSelect <  vbs->nScrollPos OR
               (int)vbs->nSelect >= vbs->nScrollPos + vbs->nMaxVisible)
            {
               vbs->nSelect = vbs->nScrollPos + vbs->nMaxVisible - 1;
            }
            else if ((int)vbs->nSelect < vbs->nScrollPos + vbs->nMaxVisible AND
                     (int)vbs->nSelect > vbs->nScrollPos)
            {
               vbs->nSelect--;
               bReverseUpdate = TRUE;
            }
            else
            {
               if (vbs->nSelect > 0)
                  vbs->nSelect--;
               vbs->nScrollPos = max(vbs->nScrollPos - 1, 0);
               bReverseUpdate = TRUE;
            }
            break;

      case ID_PLUS:
            if ((int)vbs->nSelect <  vbs->nScrollPos OR
               (int)vbs->nSelect >= vbs->nScrollPos + vbs->nMaxVisible)
            {
               vbs->nSelect = vbs->nScrollPos;
            }
            else if ((int)vbs->nSelect >= vbs->nScrollPos AND
                     (int)vbs->nSelect < vbs->nScrollPos + vbs->nMaxVisible - 1)
            {
               vbs->nSelect++;
               bReverseUpdate = FALSE;
            }
            else
            {
               if ((int)vbs->nSelect < vbs->nMaxScrollPos + vbs->nMaxVisible - 1 )
                  vbs->nSelect++;
               vbs->nScrollPos = min(vbs->nScrollPos + 1, vbs->nMaxScrollPos);
               bReverseUpdate = FALSE;
            }
            break;
      default:
         break;
   }

   SetScrollPos(vbs->hWndScroll, SB_CTL, vbs->nScrollPos, TRUE);

   InvalidateRect(vbs->hWndList, (LPRECT)NULL, TRUE);
   UpdateWindow(vbs->hWndList);
}

int NEAR PASCAL 
SetupClipIndexes(int nNumBitmaps)
{
    PFILE_OFFSET pOffset;
    int   n = 0;
    int nRetval = OK;

    // Allocate Memory
    if(hClipIndexMemory == NULL)
    {
        // MOVEABLE & Zero Init
        hClipIndexMemory = LocalAlloc(LHND, sizeof(FILE_OFFSET) * nNumBitmaps );
        if(hClipIndexMemory == NULL)
        {
            SetErr(IDS_ERR_LMEM_FULL);
            END(FAIL)
        }
    }
    else
    {
#ifdef INTERNAL
        AuxPrint("Re-Allocing Clipfile index of offsets!");
#endif
        hClipIndexMemory = LocalReAlloc(hClipIndexMemory,
                              sizeof(FILE_OFFSET) * nNumBitmaps, LHND);
        if(hClipIndexMemory == NULL)
        {
            SetErr(IDS_ERR_LMEM_FULL);
            END(FAIL)
        }
    }

    pOffset  = (PFILE_OFFSET)LocalLock(hClipIndexMemory);

    while( (n < nNumBitmaps) AND !feof(fClipFileStream) )
    {
       if ( fgetpos(fClipFileStream, pOffset) != 0)
       {
           ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                    (LPSTR) StripPath(CURTHUMBINFO));
           END ( FAIL )
       }
       pOffset++;

	    /* read up to next record */
       if (fscanf(fClipFileStream, "%*[^]\r\n") != 0)
       {
          ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                   (LPSTR) StripPath(CURTHUMBINFO));
          END(FAIL)
       }
       n++;
    }

    end:
#ifdef INTERNAL
    AuxPrint("Done setting up offsets, Size of hClipIndexMemory is %d bytes",
               LocalSize(hClipIndexMemory));
#endif
    LocalUnlock(hClipIndexMemory);

    return nRetval;
}

void NEAR PASCAL 
CloseClipFile(void)
{
#ifdef INTERNAL
    AuxPrint("Closing clipfile, Size of hClipIndexMemory is %d bytes",
               LocalSize(hClipIndexMemory));
#endif
   if (fClipFileStream != NULL)
   {
      fclose(fClipFileStream);
      fClipFileStream = NULL;
   }
}

FILE_HANDLE NEAR PASCAL
OpenClipFile(char *szThumbInfoFile)
{
    int fHandle = -1;
    OFSTRUCT ofStruct;
    FILE_HANDLE fTmp;

    if ( (fHandle = OpenFile ((LPSTR)szThumbInfoFile,
                                  (LPOFSTRUCT)&ofStruct,
                                  OF_READ)) == -1 )
    {
        ErrMsg(IDS_FILE_IO_ERR,ID_ERR_OPENING_FILE,
                  (LPSTR)StripPath(szThumbInfoFile));
        return ( NULL );
    }

    //setup for buffered input
    if ((fTmp = fdopen(fHandle, "rt")) == NULL)
    {
        ErrMsg(IDS_FILE_IO_ERR,ID_ERR_OPENING_FILE,
                  (LPSTR)StripPath(szThumbInfoFile));
    }

    return(fTmp);
}

int NEAR PASCAL
IsClipFree(int index)
{
   PFILE_OFFSET pOffset;
   char  szTemp[MAX_STR_LEN];
   char  szPrice[6];

   pOffset = (PFILE_OFFSET)LocalLock(hClipIndexMemory);
   if ( fsetpos(fClipFileStream, pOffset + index) != 0)
   {
       ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                (LPSTR) StripPath(CURTHUMBINFO));
       return ( FAIL );
   }
   LocalUnlock(hClipIndexMemory);

   /* Eat description */
   if (fgets(szTemp, MAX_STR_LEN, fClipFileStream) == NULL)
   {
       ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                (LPSTR) StripPath(CURTHUMBINFO));
       return ( FAIL );
   }

   /* Get (a) price (if price for one format is free, all should be free */
   if (fscanf(fClipFileStream, "%*[^\t]\t%[^\t]", szPrice) != 1)
   {
       ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                (LPSTR) StripPath(CURTHUMBINFO));
       return ( FAIL );
   }

   // Does price string consist of zeros and "." ?, then must be free
   return ((strspn(szPrice, "0.") == strlen(szPrice)));
}

int NEAR PASCAL 
GetClipDesc(int index, char *szClipDesc)
{
   PFILE_OFFSET pOffset;

   pOffset = (PFILE_OFFSET)LocalLock(hClipIndexMemory);
   if ( fsetpos(fClipFileStream, pOffset + index) != 0)
   {
       ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                (LPSTR) StripPath(CURTHUMBINFO));
       return ( FAIL );
   }
   LocalUnlock(hClipIndexMemory);

   /* Get description */
   if (fgets(szClipDesc, MAX_STR_LEN, fClipFileStream) == NULL)
   {
       ErrMsg(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                (LPSTR) StripPath(CURTHUMBINFO));
       return ( FAIL );
   }

   //Trim off newline
   szClipDesc[strlen(szClipDesc)-1] = '\0';

   return OK;
}

int NEAR PASCAL
HighlightSelection(WORD wImageOffset, HDC hDC, BOOL bDraw)
/************************************************ pt 5/91 ***
* We now highlight by putting box around thumbnail
************************************************************/
{
   int xOffset;
   //DWORD cr;
   int ropPrev;
   HPEN hPen, hPenPrev;
   HBRUSH hBrushPrev;
   int wLine = HIGHLIGHT_WIDTH-1;

   /* I am having a problem here in that I can't get the pen
      color to show up as expected!  So as a kludge we'll just
      stick with black and white ..pt 6/91 */

#if 0
   if (bAddHighLight)
   {
       //ropPrev = SetROP2(hDC, R2_BLACK);
       //cr = GetSysColor(COLOR_HIGHLIGHTTEXT);
   }
   else    /* remove it */
   {
       //ropPrev = SetROP2(hDC, R2_WHITE);
       //cr = GetSysColor(COLOR_WINDOW);
   }
#endif

   ropPrev = SetROP2(hDC, R2_COPYPEN);

   hPen = CreatePen(PS_SOLID,
                     HIGHLIGHT_WIDTH,
                     GetSysColor((bDraw) ? COLOR_WINDOWTEXT : COLOR_WINDOW));
   hPenPrev = SelectObject(hDC, hPen);

   hBrushPrev = SelectObject(hDC, GetStockObject(NULL_BRUSH));

   xOffset = (wImageOffset * (WIDTH_BITMAP + DELTA_X) + X_START);
   Rectangle(hDC,
             xOffset - wLine,
             yOffset - wLine,
             xOffset + WIDTH_BITMAP + (wLine*2),
             yOffset + HEIGHT_BITMAP + (wLine*2));

   //MoveTo(hDC, xOffset - wLine, yOffset - wLine);
   //LineTo(hDC, xOffset + WIDTH_BITMAP + wLine, yOffset - wLine);
   //LineTo(hDC, xOffset + WIDTH_BITMAP + wLine, yOffset + HEIGHT_BITMAP + wLine);
   //LineTo(hDC, xOffset - wLine, yOffset + HEIGHT_BITMAP + wLine);
   //LineTo(hDC, xOffset - wLine, yOffset - wLine);

   SetROP2(hDC, ropPrev);
   SelectObject(hDC, hPenPrev);
   SelectObject(hDC, hBrushPrev);
   DeleteObject(hPen);
   return OK;
}
