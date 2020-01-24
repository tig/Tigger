/*************************************************************\
 ** FILE:  comm2.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **  BBS communications routines.
 **
 ** HISTORY:   
 **  Raleigh Bellas, initially created 12-89.
 **  Revised, Doug Kent and Raleigh Bellas May 7, 1990.
 **
\*************************************************************/

#include <nowin.h>
#include <windows.h>
#include "ws.h"
#include <dos.h>
#include <io.h>
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
#include "libmgr.h"
#include "download.h"

DCB     CommDCB;
COMSTAT ComStatInfo;
#define SHIPCOSTARRAYSIZE   sizeof(ShippingCosts)

int
SendByte(HANDLE hCom, BYTE ByteToSend)
/* issues error and hangs up upon error */
{
    if (NOT StillConnected())
    {
        IssueErr();
        return FAIL;
    }

    if (WriteComm(hCom,&ByteToSend,1) < 1)
    {
        CheckCommError(0xFF);
        HangUp();
        if (InqErr())
            IssueErr();
        else
            ErrMsg(IDS_ERR_COMM);
        return FAIL;
    }

    if (WaitForEmptyOutBuf(3000L) != OK)
    {
        HangUp();
        IssueErr();
        return FAIL;
    }

    return OK;
}

int
SendBuf(HANDLE hCom, char *buf, int iBufLen)
/* issues error and hangs up upon error */
{
    if (NOT StillConnected())
    {
        IssueErr();
        return FAIL;
    }

   if (iBufLen > OUTBUFSIZE)
   {
#ifdef INTERNAL
        AuxPrint("!!! SendBuf: output buffer too large !!!");
#endif
        HangUp();
        ErrMsg(IDS_ERR_COMM);
        return FAIL;
   }

   if (WriteComm(hCom,buf,iBufLen) < iBufLen)
    {
        CheckCommError(0xFF);
        HangUp();
        if (InqErr())
            IssueErr();
        else
            ErrMsg(IDS_ERR_COMM);
        return FAIL;
    }

    if (WaitForEmptyOutBuf(30000L) != OK)
    {
        HangUp();
        IssueErr();
        return FAIL;
    }

    return OK;
}

int
SendCRC(HANDLE hCom, void *buf, int iBufLen)
{
    CRC Crc;

    if (SendBuf(hCom,buf,iBufLen) != OK)
        return FAIL;

    /** calculate CRC **/
    Crc = CalcCRC(buf,iBufLen);

    if (SendBuf(hCom,(BYTE *)&Crc,sizeof(CRC)) != OK)
        return FAIL;

    return OK;
}

/*************************************************************\
 ** Send_Message
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Waits for ACK or RESEND for appropriate messages.
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **  Issues error message, hangs up if comm error.
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  May 8, 1990 (written)
\*************************************************************/


int  FAR // variable args must not be PASCAL
Send_Message(MSGTYPE msg, va_list va_alist)
/** gotta use XENIX method of varargs because stdarg has bug when arg is
    only a byte **/
{
    va_list argP;
    HANDLE hCom = CommDCB.Id;
    int nRetval = OK;
    DWORD timetowait = WAIT_FOR_BBS;
    BYTE ack;

    if (SendBuf(hCom,&msg,sizeof(msg)) != OK)
    {
        if (NOT InqErr())
            ErrMsg(IDS_ERR_COMM);
        else 
            IssueErr();

        return FAIL;
    }

    /** send again for redundancy to reduce errors **/
    if (SendBuf(hCom,&msg,sizeof(msg)) != OK)
    {
        if (NOT InqErr())
            ErrMsg(IDS_ERR_COMM);
        else 
            IssueErr();

        return FAIL;
    }

    va_start(argP);

    switch(msg)
    {
		case MSG_GET_NEW_USER_ID:
        {
            // first arg is pointer to userID to receive
            char *szUserID = va_arg(argP,char *);

#ifdef INTERNAL
            AuxPrint("Sending MSG_GET_NEW_USER_ID");
#endif

            if (ReceiveCRC(hCom,(BYTE *)szUserID,PW_SIZE) != OK)
                END(FAIL)

            szUserID[PW_SIZE] = '\0';

            END(OK)
        }
        break;

		case MSG_SET_PASSWD:
        {
            char szPasswordBuf[PW_SIZE];
            // first arg is pointer to password to send
            char *pstr = va_arg(argP,char *);
            WORD len = strlen(pstr);  // pstr is password to send

            /** single arg is file name.  It must be PW_SIZE,
                padded in back.  **/
            memcpy(szPasswordBuf,pstr,len);
            memset(szPasswordBuf + len,PADCHAR,PW_SIZE - len);

#ifdef INTERNAL
            AuxPrint("Sending MSG_SET_PASSWD %s",pstr);
#endif

            if (SendCRC(hCom,szPasswordBuf,PW_SIZE) != OK)
                END(FAIL)

            if (ReceiveBuf(hCom,(BYTE *)&ack,sizeof(ack),timetowait) != OK)
                END(FAIL)

            END(ack == ACK ? OK : FAIL)
        }
        break;

		case MSG_SET_USER_ID:
        {
            // first arg is pointer to password to send
            char *pstr = va_arg(argP,char *);

#ifdef INTERNAL
            AuxPrint("Sending MSG_SET_USER_ID %s",pstr);
#endif

            if (SendCRC(hCom,pstr,PW_SIZE) != OK)
                END(FAIL)

            if (ReceiveBuf(hCom,(BYTE *)&ack,sizeof(ack),timetowait) != OK)
                END(FAIL)

            {
                BYTE dfg;
                while(ReadComm(hCom,&dfg,1))
#ifdef INTERNAL
                    AuxPrint("Also received %d",dfg)
#endif
                   ;
            }

            END(ack == ACK ? OK : FAIL)
        }
        break;

        case MSG_SET_CUR_FILENAME:
        {
            char szFileNameBuf[STD_FILENAME_SIZE];
            // first arg is pointer to filename to send
            char *pstr = va_arg(argP,char *);
            WORD len = strlen(pstr);  // pstr is filename to send

            /** single arg is file name.  It must be STD_FILENAME_SIZE,
                padded in back.  **/
            memcpy(szFileNameBuf,pstr,len);
            memset(szFileNameBuf + len,PADCHAR,STD_FILENAME_SIZE - len);

#ifdef INTERNAL
            AuxPrint("Sending MSG_SET_CUR_FILENAME %x %s",msg,pstr);
#endif

            if (SendCRC(hCom,szFileNameBuf,STD_FILENAME_SIZE) != OK)
                END(FAIL)

            if (ReceiveBuf(hCom,(BYTE *)&ack,sizeof(ack),timetowait) != OK)
                END(FAIL)

            END(ack == ACK ? OK : FAIL)
        }
        break;

        case MSG_SET_CUST_INFO:
        {
            // first arg is pointer to order to send
            ADDRESS_STRUCT *pstr = va_arg(argP,ADDRESS_STRUCT *);

#ifdef INTERNAL
            AuxPrint("Sending MSG_SET_CUST_INFO %x",sizeof(ADDRESS_STRUCT));
#endif

            if (SendCRC(hCom,(char *)pstr,sizeof(ADDRESS_STRUCT)) != OK)
                END(FAIL)

            if (ReceiveBuf(hCom,(BYTE *)&ack,sizeof(ack),timetowait) != OK)
                END(FAIL)

            END(ack == ACK ? OK : FAIL)
        }
        break;

        case MSG_SET_NEW_USER_ORDER:
        case MSG_SET_CUR_LIB_ORDER:
        case MSG_SET_CUR_CLIP_DLD:
        {
            // first arg is pointer to order to send
            ORDER *pstr = va_arg(argP,ORDER *);

#ifdef INTERNAL
            AuxPrint("Sending MSG_SET_CUR_ORDER (%x)",sizeof(ORDER));
#endif

            //WriteBuf("ORDER.TXT", pstr, sizeof(ORDER));

            if (SendCRC(hCom,(char *)pstr,sizeof(ORDER)) != OK)
            {
#ifdef INTERNAL
                AuxPrint("Send CRC failed");
#endif
                END(FAIL)
            }

            if (ReceiveBuf(hCom,(BYTE *)&ack,sizeof(ack),timetowait) != OK)
                END(FAIL)

            END(ack == ACK ? OK : FAIL)
        }
        break;

        case MSG_QUERY_CUR_FILESIZE:
        case MSG_QUERY_USERID_OK:
		  case MSG_QUERY_NEW_USERID_OK:
        case MSG_QUERY_INSTOCK:
        case MSG_QUERY_SUBTYPE:
        {
            // first arg is pointer to WORD 
            WORD *wordP = va_arg(argP,WORD *);

#ifdef INTERNAL
            switch(msg)
            {
                case MSG_QUERY_CUR_FILESIZE:
                    AuxPrint("Sending MSG_QUERY_CUR_FILESIZE");
                break;
                case MSG_QUERY_USERID_OK:
                    AuxPrint("Sending MSG_QUERY_USERID_OK");
                break;
                case MSG_QUERY_NEW_USERID_OK:
                    AuxPrint("Sending MSG_QUERY_NEW_USERID_OK");
                break;
                case MSG_QUERY_INSTOCK:
                    AuxPrint("Sending MSG_QUERY_INSTOCK");
                break;
                case MSG_QUERY_SUBTYPE:
                    AuxPrint("Sending MSG_QUERY_SUBTYPE");
                break;
            }
#endif

            if (ReceiveBuf(hCom,(BYTE *)wordP,sizeof(*wordP),timetowait) != OK)
                END(FAIL)

            if (msg != MSG_QUERY_CUR_FILESIZE)
            {
                BYTE dfg;
#ifdef INTERNAL
                AuxPrint("returning %d",*wordP);
#endif
                while(ReadComm(hCom,&dfg,1))
#ifdef INTERNAL
                    AuxPrint("Also received %d",dfg)
#endif
                    ;
            }

            END(OK)
        }
        break;

        case MSG_QUERY_UNCOMPRESSED_FILESIZE:
        case MSG_QUERY_RENEWAL_PRICE:
        case MSG_QUERY_SUBSCRIPTION_PRICE:
        {
            // first arg is pointer to DWORD to receive size (in bYTES)
            DWORD *dwordP = va_arg(argP,DWORD *);

#ifdef INTERNAL
            switch(msg)
            {
                case MSG_QUERY_SUBSCRIPTION_PRICE:
                    AuxPrint("Sending MSG_QUERY_SUBSCRIPTION_PRICE");
                    break;
                case MSG_QUERY_RENEWAL_PRICE:
                    AuxPrint("Sending MSG_QUERY_RENEWAL_PRICE");
                    break;
                case MSG_QUERY_UNCOMPRESSED_FILESIZE:
                    AuxPrint("Sending MSG_QUERY_UNCOMPRESSED_FILESIZE");
                break;
            }
#endif

            if (ReceiveBuf(hCom,(BYTE *)dwordP,sizeof(*dwordP),timetowait) != OK)
                END(FAIL)
            END(OK)
        }
        break;

        case MSG_QUERY_SHIPPING_COSTS:
        {
            WORD *sm = va_arg(argP,WORD *);
#ifdef INTERNAL
            AuxPrint("Sending MSG_QUERY_SHIPPING_COSTS");
#endif

            if (ReceiveBuf(hCom,(BYTE *)&ack,sizeof(ack),timetowait) != OK)
                END(FAIL)

            if (ack != ACK)
                END(FAIL)

            if (ReceiveCRC(hCom,(BYTE *)sm,sizeof(ShippingCosts)) != OK)
                END(FAIL)

            END(OK)
        }
        break;

        case MSG_QUERY_DEST_FILENAME:
        {
            // first arg is pointer to filename to receive
            char *szFilename = va_arg(argP,char *),
                 *p;

#ifdef INTERNAL
            AuxPrint("Sending MSG_QUERY_DEST_FILENAME");
#endif

            if (ReceiveCRC(hCom,(BYTE *)szFilename,STD_FILENAME_SIZE) != OK)
                END(FAIL)

            /* remove padding */
            szFilename[STD_FILENAME_SIZE] = '\0';
            if (p = strchr(szFilename,PADCHAR))
                *p = '\0';

            END(OK)
        }
        break;

        case MSG_RECEIVE_FILE:
#ifdef INTERNAL
            AuxPrint("Sending MSG_RECEIVE_FILE");
#endif
        break;

        case MSG_DOWNLOAD_EXE:
#ifdef INTERNAL
            AuxPrint("Sending MSG_DOWNLOAD_EXE");
#endif
        break;

        case MSG_LATEST_VERSION:
        /* standard wait for ACK, return in BOOL arg messages */
        {
            // first arg is pointer to WORD to receive TRUE or FALSE
            WORD *boolP = va_arg(argP,WORD *);

            *boolP = FALSE;

            switch(msg)
            {
                case MSG_LATEST_VERSION:
#ifdef INTERNAL
                    AuxPrint("Sending MSG_LATEST_VERSION");
#endif
                break;
            }

            if (ReceiveBuf(hCom,(BYTE *)&ack,sizeof(ack),timetowait) != OK)
                END(FAIL)

            *boolP = (ack == ACK);

            END(OK)
        }
        break;

        case MSG_LATEST_EXE:
        {
            // first arg is pointer to WORD to receive TRUE or FALSE
            WORD *boolP = va_arg(argP,WORD *);

#ifdef INTERNAL
            AuxPrint("Sending MSG_LATEST_EXE %d", CurVersion);
#endif

            if (SendBuf(hCom, (char *)&CurVersion, sizeof(CurVersion)) != OK)
                END(FAIL)

            if (ReceiveBuf(hCom, (BYTE *)&ack, sizeof(ack),timetowait) != OK)
                END(FAIL)

            *boolP = (ack == ACK);

            END(OK)
        }
        break;

        case MSG_SEND_WINVER:
        case MSG_SEND_EXETYPE:
        {
            // first arg is WORD 
            WORD Version = va_arg(argP, WORD);

#ifdef INTERANL
            switch(msg)
            {
                case MSG_SEND_WINVER:
                    AuxPrint("Sending MSG_SEND_WINVER %x", Version);
                break;

                case MSG_SEND_EXETYPE:
                    AuxPrint("Sending MSG_SEND_EXETYPE %d", Version);
                break;
            }
#endif

            if (SendBuf(hCom,(char *)&Version,sizeof(Version)) != OK)
                END(FAIL)

            if (ReceiveBuf(hCom,(BYTE *)&ack,sizeof(ack),timetowait) != OK)
                END(FAIL)

            END(ack == ACK ? OK : FAIL)
        }
        break;

		case MSG_QUERY_PASSWD:
        {
            // first arg is pointer to password to receive
            char *szPassword = va_arg(argP,char *),
                 *p;

#ifdef INTERNAL
            AuxPrint("Sending MSG_QUERY_PASSWD");
#endif

            if (ReceiveCRC(hCom,(BYTE *)szPassword,PW_SIZE) != OK)
                END(FAIL)

            /* remove padding */
            szPassword[PW_SIZE] = '\0';
            if (p = strchr(szPassword,PADCHAR))
                *p = '\0';

            END(OK)
        }
        break;

        case MSG_BEGIN_SESSION:
        case MSG_COMMIT_CUST_INFO:
        case MSG_COMMIT_NEWUSER:
        case MSG_COMMIT_RENEWAL:
        case MSG_COMMIT_PASSWD:
        case MSG_SAY_HI:
            /* standard wait for ACK, return OK/FAIL messages */

#ifdef INTERNAL
            switch(msg)
            {
                case MSG_SAY_HI:
                    AuxPrint("Sending MSG_SAY_HI");
                break;
                case MSG_COMMIT_CUST_INFO:
                    AuxPrint("Sending MSG_COMMIT_CUST_INFO");
                break;
                case MSG_COMMIT_NEWUSER:
                    AuxPrint("Sending MSG_COMMIT_NEWUSER");
                break;
                case MSG_COMMIT_RENEWAL:
                    AuxPrint("Sending MSG_COMMIT_RENEWAL");
                break;
                case MSG_BEGIN_SESSION:
                    AuxPrint("Sending MSG_BEGIN_SESSION");
                break;
                case MSG_COMMIT_PASSWD:
                    AuxPrint("Sending MSG_COMMIT_PASSWD");
                break;
            }
#endif

            if (ReceiveBuf(hCom,(BYTE *)&ack,sizeof(ack),timetowait) != OK)
                END(FAIL)

            if (ack != ACK)
                END(FAIL)

            END(OK)
        break;

		  case MSG_QUERY_RENEWAL_OK:
		  case MSG_QUERY_NEWUSER_OK:
        case MSG_QUERY_LIB_ORDER_OK:
        case MSG_QUERY_CLIP_DLD_OK:

        /* standard return byte result in arg messages */
        {
            // first arg is pointer to WORD to receive ACK, NAK, etc
            WORD *boolP = va_arg(argP,WORD *);

            *boolP = 0; // to clear both bytes

#ifdef INTERNAL
            AuxPrint("Sending MSG_QUERY_OK");
#endif

            switch(msg)
            {
				   case MSG_QUERY_RENEWAL_OK:
               case MSG_QUERY_LIB_ORDER_OK:
               case MSG_QUERY_CLIP_DLD_OK:
		         case MSG_QUERY_NEWUSER_OK:
                    /* gotta check credit card... */
                    timetowait = 150000L; // 2.5 minutes
               break;
            }

            /* only receiving one byte here */
            if (ReceiveBuf(hCom,(BYTE *)boolP,1,timetowait) != OK)
                END(FAIL)
            END(OK)
        }

        case MSG_QUERY_SHIP_RENEWAL:
        /* standard return byte result in arg messages */
        {
            // first arg is pointer to WORD to receive ACK, NAK, etc
            WORD *boolP = va_arg(argP,WORD *);

            *boolP = 0; // to clear both bytes

            if (ReceiveBuf(hCom,(BYTE *)boolP,sizeof(BOOL),timetowait) != OK)
                END(FAIL)
#ifdef INTERNAL
            AuxPrint("Query Ship Renewal Pkg returns %s", (*boolP)?"TRUE":"FALSE");
#endif
            END(OK)
            break;
        }

        case MSG_QUERY_DATA_LRC:
        {
            // first arg is pointer to WORD to receive extension value
            DWORD* dwP = va_arg(argP, DWORD*);

            *dwP = 0;

            if (ReceiveBuf(hCom, (BYTE *)dwP, sizeof(DWORD), timetowait) != OK)
                END(FAIL)
#ifdef INTERNAL
            AuxPrint("Query BBS Data LRC returns %ld", *dwP);
#endif
            END(OK)
            break;
        }

        case MSG_COMMIT_LIB_ORDER:
        case MSG_COMMIT_CLIP_DLD:
        {
            // first arg is pointer to WORD to receive order number
            DWORD *longP = va_arg(argP,DWORD *);

#ifdef INTERNAL
            AuxPrint("Sending MSG_COMMIT_ORDER");
#endif

            if (ReceiveBuf(hCom,(BYTE *)&ack,sizeof(ack),60000L) != OK)
                END(FAIL)

            if (ack != ACK)
                END(FAIL)

            if (ReceiveBuf(hCom,(BYTE *)longP,sizeof(*longP),timetowait) != OK)
                END(FAIL)

            END(OK)
        }

        case MSG_HANGING_UP:
#ifdef INTERNAL
            AuxPrint("Sending MSG_HANGING_UP");
#endif
        break;

        default:
#ifdef INTERNAL
            AuxPrint("Bad Message!!!");
#endif
            END(FAIL)
        break;
    }

    end:
    va_end(argP);

    /* at this point BBS is waiting for us again and has reset its timer */
    lLastMsgTime = GetTickCount (); // so WM_TIMER don't count us out

    return nRetval;
}

int
WaitForEmptyOutBuf(DWORD mSec)
{
    long oldtime;

    if (mSec)
	    oldtime = GetCurrentTime();

    do
    {
	    if (CheckCommError(0xFF /*CE_RLSDTO*/))
            return FAIL;

	    if (!ComStatInfo.cbOutQue) // then out Q is empty
		    return OK;

        if (mSec)
	        if ((GetCurrentTime() - oldtime) > mSec)
            {
                SetErr(IDS_ERR_BBS_TIMED_OUT);
#ifdef INTERNAL
                AuxPrint("Error waiting for empty outbuf (%d seconds)",mSec/1000);
#endif
		        return FAIL;
            }
	}
	while(TRUE);
}


int
WaitForInBuf(WORD nNumChars, long mSec)
/* waits for mSec milli seconds if mSec is != 0, otherwise waits forever. */
/* sets error condition */
{
    long oldtime;
    MSG msg;
#if 0
	 HCURSOR	hCursor;
#endif
    int is_msg;

    if (mSec)
    {
        if (mSec < 0)
            mSec = -mSec;
	    oldtime = (long)GetCurrentTime();
    }

#if 0
    if (hDldWnd)
    {
        hCursor = SetCursor(LoadCursor(NULL,"PHONE4"));
        SetCursor(hCursor);
    }
#endif

    do
    {
        if (bConnect AND (NOT StillConnected())) // sets error
            return FAIL;

	    if (CheckCommError(0xFF /*CE_RLSDTO*/))
            return FAIL;  // ??? error msg, hangup (w/o infinite loop)?
            
	    if (ComStatInfo.cbInQue >= nNumChars)
		    return OK;

        /* when BBS is done sending stuff, it will reset its timer, so we'll
           keep ours up-to-date */
        lLastMsgTime = GetTickCount (); // so WM_TIMER don't count us out while we're waiting

        if (mSec)
	        if (((long)GetCurrentTime() - oldtime) > mSec)
            {
                SetErr(IDS_ERR_BBS_TIMED_OUT);
#ifdef INTERNAL
                AuxPrint("timed out waiting %d seconds for bbs",mSec / 1000);
#endif
		        return FAIL;
            }

#if 0
        while ( PeekMessage (&msg, NULL, NULL, NULL, PM_REMOVE) )
        {
            //if ( !((msg.hwnd == hDldWnd) || (msg.hwnd == hWndTimer)) )
                //continue;

            if (hDldWnd && IsWindow (hDldWnd))
                if (IsDialogMessage(hDldWnd,(LPMSG)&msg))
                    continue;

            if (hWndTimer && IsWindow (hWndTimer))
                if (IsDialogMessage(hWndTimer,(LPMSG)&msg))
                    continue;

            TranslateMessage((LPMSG)&msg);
            DispatchMessage((LPMSG)&msg);
        }
#else

        /** this is mainly for TIMER, but will also enable user to go
            to another app.  Also for cancel dialogs. **/
        while (hDldWnd)
        {
            is_msg = PeekMessage(&msg,hDldWnd,NULL,NULL,PM_REMOVE|PM_NOYIELD);

        	//SetCursor(hCursor);
            if (NOT is_msg) break;

            if (hDldWnd && IsWindow (hDldWnd))
                if (IsDialogMessage(hDldWnd,(LPMSG)&msg))
                    continue;

            //TranslateMessage((LPMSG)&msg);
            //DispatchMessage((LPMSG)&msg);
        }

        while(hWndTimer)
        {
            is_msg = PeekMessage(&msg,hWndTimer,NULL,NULL,PM_REMOVE|PM_NOYIELD);

            if (NOT is_msg) break;

            if (hWndTimer && IsWindow (hWndTimer))
                if (IsDialogMessage(hWndTimer,(LPMSG)&msg))
                    continue;

            //TranslateMessage((LPMSG)&msg);
            //DispatchMessage((LPMSG)&msg);
        }
#endif
	}
	while(TRUE);
}

void
Wait(DWORD mSec)
/** wait for mSec milliseconds **/
{
    long oldtime;

	oldtime = (long)GetCurrentTime();

	while((long)(GetCurrentTime() - oldtime) < (long)mSec)
        ;
}
