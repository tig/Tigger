/*************************************************************\
 ** FILE:  comm3.c
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
#include "busy.h"
// for WriteBuf
#ifdef DEBUG
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#endif

BOOL bBackupThumb;
DCB     CommDCB;
COMSTAT ComStatInfo;
BOOL bStillConnected;
#define DCB_MSRSHADOW  35
#define ACE_RLSD       0x80
static BYTE FAR *lpMSRShadow;

int
ReceiveBuf(HANDLE hCom,BYTE *buf,WORD nNumBytes, long nTimeToWait)
/* if nTimeToWait < 0, don't hangup or issue error msg */
{
    int nNumRead;

    if (WaitForInBuf(nNumBytes, nTimeToWait) != OK)
    {
        if (nTimeToWait > 0)
        {
            HangUp();
            IssueErr();
        }
        else
            ClrErr();
        return FAIL;
    }

    nNumRead = ReadComm(hCom, buf, nNumBytes);
    if (nNumRead < 0)
    {
        CheckCommError(0xFF);
        nNumRead = -nNumRead;
    }

    if ((WORD)nNumRead == nNumBytes)
        return OK;
    else
    {
        if (CheckCommError(0xFF))
        {
            HangUp();
            IssueErr();
        }
        return FAIL;
    }
}

int
ReceiveCRC(HANDLE hCom, char *buf, int iBufLen)
{
    CRC Crc;

    if (ReceiveBuf(hCom,buf,iBufLen,WAIT_FOR_BBS) != OK)
        return FAIL;

    /** get CRC **/
    if (ReceiveBuf(hCom,(BYTE *)&Crc,sizeof(CRC),WAIT_FOR_BBS) != OK)
        return FAIL;

    /** calculate CRC **/
    if (Crc != CalcCRC(buf,iBufLen))
    {
#ifdef INTERNAL
        AuxPrint("Bad CRC");
#endif
        return FAIL;
    }

    return OK;
}


int
SetPortState(void)
{
    if (GetCommState(CommDCB.Id, (DCB FAR *) &CommDCB) != -1)
	{
        switch (custinfo.baud + BAUD_ID_BASE)
        {
            case ID_300:
	            CommDCB.BaudRate = 300;
            break;
            case ID_600:
	            CommDCB.BaudRate = 600;
            break;
            case ID_1200:
	            CommDCB.BaudRate = 1200;
            break;
            case ID_2400:
	            CommDCB.BaudRate = 2400;
            break;
            case ID_4800:
	            CommDCB.BaudRate = 4800;
            break;
            case ID_9600:
	            CommDCB.BaudRate = 9600;
            break;
        }
        CommDCB.RlsTimeout = 10000;
	    CommDCB.Parity = NOPARITY;
        CommDCB.fParity = FALSE;
	    CommDCB.ByteSize = 8;
	    CommDCB.StopBits = ONESTOPBIT;
        CommDCB.fBinary   = TRUE;
        CommDCB.XonChar   = 0x11;    /* Xon char                         */
        CommDCB.XoffChar  = 0x13;    /* Xoff char                        */
        CommDCB.fNull     = FALSE;   /* Do Not Strip Null Chars          */
        CommDCB.XonLim    = (INBUFSIZE >> 3);      /* Distance from Q empty to Xon     */
        CommDCB.XoffLim   = (INBUFSIZE >> 3); /* Q Full to Xoff      */
	    CommDCB.fOutX    = FALSE; /* output Xon/Xoff Flow Ctrl */
	    CommDCB.fInX     = FALSE; /* input Xon/Xoff Flow Ctrl   */
	    CommDCB.fOutxCtsFlow = TRUE;
	    CommDCB.fOutxDsrFlow = TRUE;
	    //CommDCB.fChEvt = FALSE;
	    CommDCB.fRtsflow = TRUE;
	    CommDCB.fDtrflow = TRUE;
	    //CommDCB.TxDelay = 30; 


	    SetCommState((DCB FAR *)&CommDCB);
        if (CheckCommError(0xFF))
            return FAIL;
    	//GetCommError(CommDCB.Id, (COMSTAT FAR *)&ComStatInfo);
        lpMSRShadow = ((BYTE FAR *)SetCommEventMask( CommDCB.Id, nEVTMASK )) + DCB_MSRSHADOW;
        //SetCommEventMask(CommDCB.Id,nEVTMASK); // to detect loss of carrier signal
        return OK;
	}
    else
        return FAIL;
}


/*************************************************************\
 ** Download
 **
 ** ARGUMENTS:
 **  char   *szFileName
 **  char   *szDldPath
 **  HANDLE hProgressBar
 **  HANDLE hNameWnd
 **  BOOL bIsThumbFile
 **  WORD* wpNeedNewExt
 **
 ** DESCRIPTION:
 **  Download (in xmodem format) a file and uncompress it into 
 **  the file that the bbs say szFileName should decompress into.
 **  
 **  If bIsThumbFile, then temporarily save the compressed info as well and
 **  handle backing it up to an alternate drive.  Checks against
 **  cache to ensure enuf space is available.
 **
 **  Prepend szDldPath to both output file names.
 **  
 **  If wpNeedNewExt is not null the extension of the downloaded file is
 **  assigned to it
 **
 ** ASSUMES:
 **  CommDCB.Id is the opened and initialized comm port.
 **  sz*FileName may or may not have full paths.
 **  szFileName is in standard WSdatabase format.
 **
 ** MODIFIES:
 **  Sets error condition,
 **  wpNeedNewExt
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **  Sets error condition if failed.
 **
 ** HISTORY:   Doug Kent  April 28, 1990 (written)
 **  revised from R. Bellas Galactacom version of order().
\*************************************************************/
int FAR PASCAL
Download(char *szFileName, char *szDldPath,
         BOOL bUpdateMeter, BOOL bSetNameWnd,
         BOOL bIsThumbFile, BOOL bNeedTimeEst,
         BOOL bNeedJobSize, WORD* wpNeedNewExt)
{
     DWORD  dwFileSize, dwCompress;
     int    nRetval=OK;
     char   szDestFileName[_MAX_PATH];
     char   szUncomprFileName[_MAX_PATH];
     char   szBaseName[STD_FILENAME_SIZE + 1]; /* name of file at bbs 
        end, in the standard WS database naming format */
     HCURSOR hCursor;
     WORD wNumBlocks;
     BOOL bGetExe=FALSE;

	hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
     if (NOT bConnect)
        return FAIL;

    if (NOT bUpdateMeter)  //Not using DOWNLOAD dlg, throw up a BusyBox
        BusyLoadNewText (hWndView,
               (bIsThumbFile) ? IDS_MSG_DOWNLOADING : IDS_MSG_DOWNLOADING_DB,
               (bIsThumbFile) ? IDS_BUSY_THUMB_ICON : IDS_BUSY_DB_ICON);

    if (szFileName)
        strcpy(szBaseName, StripPath(szFileName));
    else
    {
        lstrcpy(szBaseName,"NEWSHOP.EXE");
        bGetExe = TRUE;
    }
    /* first check that there is enough free disk space for file and tmp file **/

    if (Send_Message(MSG_SET_CUR_FILENAME,szBaseName) != OK)
        END(FAIL_W_MSG)

    if (Send_Message(MSG_QUERY_CUR_FILESIZE,(char *)&wNumBlocks) != OK)
        END(FAIL_W_MSG)

    /* Arbitrary file size limit here!! Sometimes if a file is makexed twice or
       otherwise corrupted the file size in the header can be way out of whack
       (i.e. huge). If this happens with a thumbnail file WS wil try to clear
       out the cache */
    if ((NOT wNumBlocks)  OR //file size is zero, doesn't exist on server
        (wNumBlocks > MAX_DATA_FILE_SIZE_BLOCKS))
       END(FAIL_W_MSG)

    if ((IsWindow(hDldWnd)) AND (bNeedJobSize))
       SendMessage(hDldWnd, MM_SET_PARTS_IN_JOB, wNumBlocks, 0L);

    /** get size of file in bytes **/
    if (Send_Message(MSG_QUERY_UNCOMPRESSED_FILESIZE,(char *)&dwFileSize) != OK)
        END(FAIL_W_MSG)

    /* Arbitrary file size limit here!! Sometimes if a file is makexed twice or
       otherwise corrupted the file size in the header can be way out of whack (huge).
       If this happens with a thumbnail file WS wil try to clear out the cache */
    if (dwFileSize > MAX_DATA_FILE_SIZE_BYTES)
       END(FAIL_W_MSG)

    dwCompress = dwFileSize;
    if ( bIsThumbFile ) // then need room for compressed file as well
        dwCompress += (wNumBlocks * sizeof (DATABLOCKSIZE));

#ifdef INTERNAL
    AuxPrint("wNumBlocks = %d",wNumBlocks);
    AuxPrint("FileSize = %ld",dwCompress);
#endif

    /* Do we have enough room to put the stuff we get from the BBS */
    if ( ConfirmCopyOK(NULL, szDldPath, dwCompress, bIsThumbFile) != OK ) 
        END(FAIL)

    /** get name to give file after decompressing **/
    if (Send_Message(MSG_QUERY_DEST_FILENAME,szDestFileName) != OK)
        END(FAIL_W_MSG)

    if (wpNeedNewExt)
       *wpNeedNewExt = GetExtNum(szDestFileName);

   if ((IsWindow(hDldWnd)) AND (bSetNameWnd))
	{
        char   Buffer[_MAX_PATH];

	    // Fix the file name so that it has no &'s
	    FixString (szDestFileName, Buffer, sizeof (Buffer));
       SendMessage(hDldWnd, MM_SET_FILE_NAME, 0, (LONG)((LPSTR)Buffer));
	}

    if (bGetExe)
    {
        if (Send_Message(MSG_DOWNLOAD_EXE) != OK)
            END(FAIL_W_MSG)
    }
    else
        if (Send_Message(MSG_RECEIVE_FILE) != OK)
            END(FAIL_W_MSG)

    /** we wanna put the uncompressed info into a temporary file
        and the compressed info into the file owning the standard
        WSdatabase monicker.  Then it will be easy to backup the
        file containing the compressed info. **/
    if (bIsThumbFile)
        lstrcpy(szUncomprFileName,szTMP1);

#ifdef INTERNAL
    AuxPrint("FileName = %s", szDestFileName);
#endif

    if(XMReceive(bIsThumbFile ? szUncomprFileName : szDestFileName, 
                 bIsThumbFile ? szDestFileName : NULL,
                 szDldPath,
                 wNumBlocks, bUpdateMeter,
                 bNeedTimeEst, dwFileSize) != OK)
        END(FAIL_W_MSG)

    if ( bIsThumbFile )
    {
        char   szFullDestName[_MAX_PATH];
        char   szFullUncomprName[_MAX_PATH];

        lstrcpy (szFullDestName, szDldPath);
        Append (szFullDestName, szDestFileName);

        lstrcpy (szFullUncomprName, szDldPath);
        Append (szFullUncomprName, szUncomprFileName);

        if (bBackupThumb)
            ExtractFromCache(szFullDestName, FALSE); /* copies to szPath,
                deletes existing old versions if there */

        // copy the Uncompressed info to the compressed file name 
        unlink (szFullDestName);
        if (rename(szFullUncomprName,szFullDestName))
        {
            ErrMsg(IDS_FILE_IO_ERR,ID_ERR_MOVING_FILE);
            END(FAIL_W_MSG)
        }
    }

    end:
    if (NOT bUpdateMeter)  //Not using DOWNLOAD dlg, close up BusyBox
        BusyClear(TRUE);

    if (nRetval == FAIL_W_MSG)
    {
        SetErr(IDS_ERR_UNABLE_TO_DOWNLOAD,(LPSTR) szBaseName);
        nRetval = FAIL;
    }
    SetCursor(hCursor);

    return(nRetval);
}


BOOL PASCAL
LatestVersion(char *szFileName, char *szLatestFileName)
/** assumes we are connected **/
{
    BOOL bResult;

    REFERENCE_FORMAL_PARAM (szLatestFileName);

    if (Send_Message(MSG_SET_CUR_FILENAME, StripPath(szFileName)) != OK)
        return TRUE;

    if (Send_Message(MSG_LATEST_VERSION,&bResult) != OK)
        return TRUE;

#if 0
    if ((NOT bResult) AND szLatestFileName)
        Send_Message(MSG_GET_LATEST_VERSION,szLatestFileName);
#endif

    /* in case bResult isn't exactly == TRUE or FALSE... */
    return bResult ? TRUE : FALSE;
}

BOOL PASCAL
LatestExe(void)
/** assumes we are connected **/
{
    BOOL bResult;

    if (Send_Message(MSG_LATEST_EXE,&bResult) != OK)
        return FALSE;

    return bResult;
}

/*************************************************************\
 ** BBSsync
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  If we've had some kind of communications error with the BBS, 
 **  try to get back in sync.
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  June 22, 1990 (written)
\*************************************************************/
int
BBSsync(void)
{
    int count,subcount;
    MSGTYPE msg=MSG_SAY_HI;
    BYTE ack;
    HANDLE hCom = CommDCB.Id;
    int nRetval=OK;
    HCURSOR hCursor;

	hCursor = SetCursor(NULL);

    /* in case in the middle of a download */
#if 0
    if (SendByte(CommDCB.Id,CTRLX) != OK)
        return FAIL;
#endif

	FlushComm(hCom, 1);

    /** get ACK from bbs **/
    for (count=0;count<6;++count)
    {
	    SetCursor(hPhoneCursArr[count % 4]);

        if (SendBuf(hCom,&msg,sizeof(msg)) != OK)
        {
            if (NOT InqErr())
                ErrMsg(IDS_ERR_COMM);
            else 
                IssueErr();

            END(FAIL)
        }

        /** send again for redundancy to reduce errors **/
        if (SendBuf(hCom,&msg,sizeof(msg)) != OK)
        {
            if (NOT InqErr())
                ErrMsg(IDS_ERR_COMM);
            else 
                IssueErr();

            END(FAIL)
        }

        subcount = 2;
        while(subcount--)
        {
            if (ReceiveBuf(hCom,(BYTE *)&ack,sizeof(ack),-5000L) == OK)
                if (ack == ACK)
                {
                    lLastMsgTime = GetTickCount (); // so WM_TIMER don't count us out
	                FlushComm(hCom, 1);
                    END(OK)
                }
                else
#ifdef INTERNAL
                    AuxPrint("Received %x instead of ACK",ack)
#endif
                    ;
        }
    }

#ifdef INTERNAL
    AuxPrint("Error syncing");
#endif
    END(FAIL)

    end:
	SetCursor(hCursor);
    return nRetval;
}

short
CheckCommError(short sFlagsToCheck)
/* side effect is to set CommStatInfo */
{
    short sErr;
    sErr = GetCommError(CommDCB.Id, (COMSTAT FAR *)&ComStatInfo);

    sErr &= sFlagsToCheck;

    if (sErr & CE_RXOVER  )
#ifdef INTERNAL
        AuxPrint("CheckCommError:  Receive Queue overflow")
#endif
      ;

    if (sErr & CE_OVERRUN )
#ifdef INTERNAL
        AuxPrint("CheckCommError: Receive Overrun Error")
#endif
      ;
    

    if (sErr & CE_RXPARITY)
#ifdef INTERNAL
        AuxPrint("CheckCommError: Receive Parity Error")
#endif
      ;
    

    if (sErr & CE_FRAME   )
#ifdef INTERNAL
        AuxPrint("CheckCommErr: Receive Framing error")
#endif
      ;
    

    if (sErr & CE_BREAK   )
#ifdef INTERNAL
        AuxPrint("CheckCommErr: Break Detected")
#endif
      ;
    

    if (sErr & CE_CTSTO   )
#ifdef INTERNAL
        AuxPrint("CheckCommErr: CTS Timeout")
#endif
      ;
    

    if (sErr & CE_DSRTO   )
#ifdef INTERNAL
        AuxPrint("CheckCommErr: DSR Timeout")
#endif
      ;
    

    if (sErr & CE_RLSDTO  )
    {
#ifdef INTERNAL
        AuxPrint("CheckCommErr: RLSD Timeout")
#endif
        ;
        SetErr(IDS_MSG_DISCONNECTED);
    }

    if (sErr & CE_TXFULL  )
#ifdef INTERNAL
        AuxPrint("CheckCommErr: TX Queue is full")
#endif
      ;
    

    if (sErr & CE_PTO    )
#ifdef INTERNAL
        AuxPrint("CheckCommErr: LPTx Timeout")
#endif
      ;
    

    if (sErr & CE_IOE    )
#ifdef INTERNAL
        AuxPrint("CheckCommErr: LPTx IO Error")
#endif
      ;
    

    if (sErr & CE_DNS    )
#ifdef INTERNAL
        AuxPrint("CheckCommErr: LPTx Device not selected")
#endif
      ;
    

    if (sErr & CE_OOP    )
#ifdef INTERNAL
        AuxPrint("CheckCommErr: LPTx Out-Of-Paper")
#endif
      ;
    

    if (sErr & CE_MODE    )
#ifdef INTERNAL
        AuxPrint("CheckCommErr: Requested mode unsupported")
#endif
      ;
    

    if (sErr AND NOT InqErr())
        SetErr(IDS_ERR_COMM);

    return sErr;
}

void
SetLocal(BOOL bOnOff)
/* carrier detect is only on when connected */
{
    if (GetCommState(CommDCB.Id, (DCB FAR *) &CommDCB) != -1)
	{
        CommDCB.RlsTimeout = bOnOff ? 0 : 1000;
	    SetCommState((DCB FAR *)&CommDCB);
        CheckCommError(0xFF);
	}
}


BOOL PASCAL
StillConnected(void)
{
#if 0
    WORD nRetval;
#endif

    if (NOT bStillConnected) // initialized in WaitForConnection()
        return FALSE;

    /** ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    As you probably know, the 8250 chip has a Modem Status Register (MSR)
    that contains among other things the status of the Data Carrier
    Detect (DCD  aka CD and RLSD).  If you read this register directly
    (which you can NOT because it would clear valuable information that
    the comm driver needs), then you could simply check bit 7 of this
    register to find if you have lost the carrier.

    Now for the good part: the comm driver reads this register and stores
    it in a field that it calls the MSRShadow.  What this means for you
    is that you can _peek_ at this location to get the information you
    require.  All you need to know is the offset to this byte -- this is
    done with the following code snip:
    ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/

    bStillConnected = (*lpMSRShadow & ACE_RLSD);
    if (NOT bStillConnected)
        SetErr(IDS_MSG_DISCONNECTED);
    return bStillConnected;

#if 0
    else
        nRetval = GetCommEventMask(CommDCB.Id,nEVTMASK);
    if (wWinVer < 0x300)
    {
        return (bStillConnected = !(nRetval & EV_RLSD));
    }
    else
    {
#ifdef INTERNAL
        //AuxPrint("GetCommEventMask: %x",nRetval);
#endif
        //return (bStillConnected = !nRetval);
        return (bStillConnected = !(nRetval & EV_RLSDS));
    }
#endif

    
}

#ifdef DEBUG
int
WriteBuf(char *szFileName, char *buf, int NumBytes)
{
    int fp= -1;
    int nRetval=OK;

    if ((fp = open(szFileName,O_WRONLY|O_BINARY|O_CREAT|O_APPEND,S_IREAD|S_IWRITE)) == -1)
        END(FAIL)

    lseek(fp,0L,2); // to end of file for append

    if (write(fp,buf,NumBytes) != NumBytes)
        END(FAIL)

    end:
    if (fp != -1)
        close(fp);
    if (nRetval != OK)
#ifdef INTERNAL
        AuxPrint("WriteBuf (%s) failed",szFileName);
#endif
    return nRetval;
}
#endif

