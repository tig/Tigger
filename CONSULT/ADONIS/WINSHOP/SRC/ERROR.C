#include <windows.h>
#include "winundoc.h"
#include <string.h>
#include <stdio.h>
#include "error.h"
#include "ws.h"
#include "rc_symb.h"
#include "help.h"
#include "misc.h"
#include <stdarg.h>


/* Globals */
#define MSGBUFSIZE 512
static BOOL bDestSet=FALSE;
static char szErrMsg[MSGBUFSIZE];    /*  error msg constructed from string table and
                                         variable arguments */

static char szCaption[120];   /*  msg box caption constructed from string
                                  table and variable arguments  */

static int nCurrErr;                      /* Current Error ID for lookup into string table*/
static int nCaption = IDS_TITLE_APPNAME;  /* Current Caption ID for lookup into string table*/
static int nFlags = MB_OK;                /* Flags for msg box (buttons, icons and modal
                                             restrictions) */
static HANDLE hParentWindow;              /* parent window of msg box */
static BOOL BeepBox = NOBEEP;             /* Make beep when message box is created? */

/* Caption and message to be used for low memory condition; located here
instead of string table to avoid loading resource file when memory is low */

static char lowmemmsg[] = " Insufficient Memory ";
static char defcpt[]    = " Window Shopper ";

/*
 Procedure:  ClrErr
 Purpose:    Resets the error condition to zero and returns the previous error
             condition
*/

int FAR PASCAL
ClrErr()
{
   int nTemp = nCurrErr;
   nCurrErr = 0;
   return  nTemp;
}

/*
 Procedure:  InqErr
 Purpose:    Returns the current error condition
*/

int FAR PASCAL
InqErr()
{
  return nCurrErr;
}

/*
 Procedure:  InqDest
 Purpose:    InqDest passes the current Error Destination (parent window),
             Msg Box caption ID, flags, and optionally, if *szCptn is non-null,
             the current caption string, which would include any variable
             arguments to the caption ID.  bBeep passes the value for BeepBox,
             which if true, enables MessageBeep with each MessageBox.
*/

void FAR PASCAL
InqDest(HANDLE *hParent, BOOL *bBeep, int *nCptn, int *nFlg, char *szCptn)

{
    *hParent = hParentWindow;
    *nCptn   = nCaption;
    *nFlg    = nFlags;
    *bBeep   = BeepBox;

    if (szCptn)
    {
       lstrcpy(szCptn,szCaption);
    }
}                        /*  END  InqDest  */

/*
 Procedure:  SetErr
 Purpose:    SetErr sets the current error condition and formats the message
             string with any variable arguments
*/

int FAR
SetErr(int nCtrlStr,...)
{
    char szCtrlStr[MSGBUFSIZE];
    va_list argList;

    if (nCtrlStr == ES_LOWMEMMSG)
    {
       lstrcpy(szErrMsg, lowmemmsg);
    }
    else
    {
       LoadString(hInstanceWS, nCtrlStr, szCtrlStr, sizeof(szCtrlStr));
       va_start(argList, nCtrlStr);
       WSvsprintf((LPSTR)szErrMsg,(LPSTR)szCtrlStr,argList);
       va_end(argList);
    }

    nCurrErr = nCtrlStr;

    return(nCtrlStr);
}                        /*  END  SetErr  */

/*
 Procedure:  SetDest
 Purpose:    Sets the destination of the error message box, the parent window
             (use ES_ACTIVEWND for the active window), the flags (buttons and
             icons for the message box), and the caption (caption is formatted
             here with any variable arguments). For low memory conditions use
             ES_LOWMEMCPT and LOWMEMFLAGS to avoid loading the resource file.
             Beep, if true, will enable MessageBeep with each call to IssueErr.
*/

void FAR
SetDest(HANDLE hParent, BOOL bBeep, int nFlg, int nCptn,...)
{
    if ((nCptn == ES_LOWMEMCPT) || (nCptn == ES_DEFCPT))
    {
       lstrcpy(szCaption, defcpt);
    }
    else
    {
       va_list argList;
       char szCptn[120];

       nCaption = nCptn;
       LoadString(hInstanceWS, nCaption, szCptn, 120);
       va_start(argList, nCptn);
       WSvsprintf((LPSTR)szCaption,(LPSTR) szCptn, argList);
       va_end(argList);
    }

    hParentWindow = hParent;

    nFlags = nFlg;
    BeepBox = bBeep;
    bDestSet = TRUE;
}                        /*  END  SetDest  */

/*
 Procedure: IssueErr
 Purpose:   Actually displays the Error Message Box, with the caption and
            message strings using the global variables hParentWindow, szCaption,
            szErrMsg, and nFlags. The result from the MessageBox() call is
            returned.
*/

int FAR PASCAL
IssueErr()
{
    int ErrResult;
    HWND hParentWnd,hChildWnd;

    if (NOT InqErr())
        return 0;

    if (NOT bDestSet)
        SetDefErr();

    if (BeepBox)
       MessageBeep(0);

    bDestSet = FALSE;

    {
        hParentWnd = GetActiveWindow();
        if (hParentWnd)
            if (GetWindowTask(hParentWnd) != hTaskWS)
                hParentWnd = NULL;
            else
                hChildWnd = GetTopWindow(hParentWnd);
    }

#if 0
    EnableAllWindows(FALSE);
#else
    //EnableAllModeless(FALSE);
#endif

    ErrResult = MessageBox(NULL, szErrMsg, szCaption, nFlags | MB_TASKMODAL);

    if (hParentWnd)
    {
#ifdef INTERNAL
        //AuxPrint("Enabling %x, TRUE",hParentWnd);
#endif
        BringWindowToTop(hParentWnd);
#if 0
        if (hParentWnd == hWndWS)
        {
            THAW_HWND_WS
        }
        else
            EnableWindow(hParentWnd,TRUE);
#endif
        if (IsWindow(hChildWnd))
        {
            EnableWindow(hChildWnd,TRUE);
            SetFocus(hChildWnd);
        }
    }

    //EnableAllModeless(TRUE);

    SetDefErr();    /* Reset MsgBox parameters */
    ClrErr();
    return ErrResult;
}                        /*  END  IssueErr  */

void FAR PASCAL
SetDefErr()
{
  SetDest(ES_ACTIVEWND, NOBEEP, MB_OK, IDS_TITLE_APPNAME);
}

int FAR
ErrMsg(int nCtrlStr,...)
/* just like SetErr except calls IssueErr() (3.6.90) D. Kent */
{
    if (nCtrlStr == ES_LOWMEMMSG)
    {
       lstrcpy(szErrMsg, lowmemmsg);
    }
    else
    {
       va_list argList;
       char szCtrlStr[MSGBUFSIZE];

       LoadString(hInstanceWS, nCtrlStr, szCtrlStr, sizeof(szCtrlStr));
       va_start(argList, nCtrlStr);
       WSvsprintf((LPSTR)szErrMsg,(LPSTR)szCtrlStr,argList);
       va_end(argList);
    }

    nCurrErr = nCtrlStr;
    return IssueErr();
}

/******************************************************* KRG *********

    TITLE: WSvsprintf

   INTRODUCTION: This routine is a limited rewrite of the vsprintf
                 C runtime routine.

   LIMITATIONS: The following formats are supported.
                %d,, %i, %c, %s


 *********************************************************************/

int FAR _cdecl
WSvsprintf(char FAR * lpDest,
           const char FAR * lpTemplate,
           va_list argList)
{
    register char ch;
    LPSTR lpTemp, lpStart;
    PSTR  pTemp;

    lpStart = lpDest;
    while ( ch = *lpTemplate++ )
    {
        switch ( ch )
        {
          case '%':
            ch = *lpTemplate++;

            switch (ch)
            {
              case 'd':
              case 'i':
                itoa( va_arg(argList,int), pTemp, 10);
                while (*pTemp != '\0')
                   *lpDest++ = *pTemp++;
                break;

              case 'c':
                *lpDest++ = va_arg(argList, char);
                break;

              case 's':
			       lpTemp = va_arg(argList, LPSTR);
                while (*lpTemp) *lpDest++ = *lpTemp++;
                break;

              case '\0':
                continue;

              default:
                break;
            }
            break;

          case '\\':
            ch = *lpTemplate++;
            switch (ch)
            {
              case 'n':
                ch = '\n';
                break;
              case 't':
                ch = '\t';
                break;
              case 'r':
                ch = '\r';
                break;
              default:
                break;
            }
            /* Fall through here. */

          case '\0':
            continue;

          default:
            *lpDest++ = ch;
            break;
        }
    }
    *lpDest = '\0';
    return( lpDest - lpStart );
}



