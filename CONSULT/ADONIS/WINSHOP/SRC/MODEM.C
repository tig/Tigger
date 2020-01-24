/*************************************************************\
 ** FILE:  modem.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **  Modem access routines.
 **
 ** HISTORY:   
 **   Doug Kent  May 7, 1990 (created)
 **
\*************************************************************/
#include <windows.h>
#include <string.h>
#include "ws.h"
#include "comm.h"
#include "rc_symb.h"
#include "misc.h"
#include "messages.h"
#include "file.h"

int
WriteModemCommand(char *szCommand)
{
    int len = strlen(szCommand);
    long oldtime;

#ifdef INTERNAL
    //AuxPrint("Writing %s to modem...",szCommand);
#endif

    SetLocal(ON);

    /* flush input Q so is cleared for response to this command **/
    CheckCommError(0xFF);
	FlushComm(CommDCB.Id, 1);

    for (;*szCommand; ++szCommand)
    {
        if (*szCommand == '~')
            Wait(500L); // wait for half a second 
        else
        {
            oldtime = GetCurrentTime();
            while (WriteComm(CommDCB.Id,szCommand,1) <= 0)
	        {
                CheckCommError(0xFF);
	            FlushComm(CommDCB.Id, 0);
                CheckCommError(0xFF);
	            FlushComm(CommDCB.Id, 1);
	            if ((GetCurrentTime() - oldtime) > 1000L)
		            return FAIL;
	        }

            /* wait maximum of 3 seconds for buffer to clear **/
            if (WaitForEmptyOutBuf(3000L) != OK)
                return FAIL;

            Wait(5L); // wait between characters
        }
    }
    return OK;
}

int
WaitForConnection(void)
/* set error condition */
{
    unsigned char c[3];

	while(TRUE)
    {
        if (WaitForInBuf(2, 40000L) != OK)
            return FAIL;

        /* gotta clear event mask */
        GetCommEventMask(CommDCB.Id,nEVTMASK);

        ReadComm(CommDCB.Id,c,2);

        switch(c[0])
        {
            case COMM_OK:
            break;

            case COMM_BUSY:
                return IDS_ERR_BUSY;
            break;

            case COMM_RING: // (huh?)
            case COMM_NO_CARRIER:
            case COMM_ERROR:
            case COMM_NO_DIAL_TONE:
            case COMM_NO_ANSWER:
                return FAIL;
            break;

            default: // CONNECT
                /** get rid of extra character(s?) received after connect. */
#if 0
                if (c[0] == '1') // received a high baud connect
                {
                    CheckCommError(0xFF);
                    ReadComm(CommDCB.Id,c,1);
                }
#else
                Wait(250L);
	            FlushComm(CommDCB.Id, 1);
#endif
                /* gotta clear event mask */
                GetCommEventMask(CommDCB.Id,nEVTMASK);
                bStillConnected = TRUE; // see StillConnected()
                return OK;
            break;
        }
    }
}


void FAR PASCAL
HangUp(void)
{
    HCURSOR hCursor=NULL;

    if (bConnect == FALSE)
        return;

    hCursor=SetCursor(hPhoneCursArr[0]);

    SetStatusBarText (szHangingUp);

    /* do this first cause if disconnected and Send_Message() issues
       msgbox, timer continues. */
    if ( hWndTimer )
    {
        KillTimer(hWndTimer, 1);
    }

    if (StillConnected())
        Send_Message(MSG_HANGING_UP);

    SetLocal(ON);

    if (WriteModemCommand("+~+~+~~~+~+~+~") == OK)
    {
        SetCursor(hPhoneCursArr[1]);
        WriteModemCommand("ATZ\r"); // hangup and reset to modem's default parms
        SetCursor(hPhoneCursArr[2]);
    }

    CloseComm(CommDCB.Id);

    bConnect = FALSE;
    CloseFileCheckSystem(AND_RESET);   // free memory for Uptodatelist

    if ( hWndTimer )
    {
        DestroyWindow (hWndTimer);
        FreeProcInstance (lpprocTimerDlg);
    }

    if (hChildren[ID_LIB_LOGON].hWnd)
        SetWindowText(hChildren[ID_LIB_LOGON].hWnd,"&Logon");
    if (hCursor)
        SetCursor(hCursor);
    SetStatusBarText (szLibSelections);
}
