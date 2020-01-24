#ifndef COMMH
#define COMMH

#include "stdcomm.h"
#include "rc_symb.h"

extern BOOL bConnect;	
extern DCB CommDCB;
extern COMSTAT ComStatInfo;

#define DEF_MODEM_INIT_STR "ATE0V0Q0X4"
#define DEF_USER_INIT_STR "ATS7=60"

#define WAIT_FOR_BBS 30000L // time in milliseconds to wait for response from bbs
#define COMM_OK             '0'
#define COMM_RING           '2'
#define COMM_NO_CARRIER     '3'
#define COMM_ERROR          '4'
#define COMM_NO_DIAL_TONE   '6'
#define COMM_BUSY           '7'
#define COMM_NO_ANSWER      '8'

#define EV_RLSDS 0x1000 // undocumented flag, bit is set when CD is up
#define nEVTMASK 0xFF //(EV_RLSDS|EV_RLSD) // for *CommEventMask()

#define INBUFSIZE  1024
#define OUTBUFSIZE 1024

//For the cancel download dialog meter control

/* for setting download type */
#define DLD_TYPE_CLIPART     CLIPART_BMP
#define DLD_TYPE_DB          DB_BMP
#define DLD_TYPE_EXE         WSBAG_BMP

// The Meter control doesn't have any class-specific window styles.

// Meter control's class-specific window messages.
#define MM_SET_PARTS_IN_JOB        (WM_USER + 1)
#define MM_GET_PARTS_IN_JOB        (WM_USER + 2)
#define MM_SET_PARTS_COMPLETE      (WM_USER + 3)
#define MM_GET_PARTS_COMPLETE      (WM_USER + 4)
#define MM_INC_PARTS_COMPLETE      (WM_USER + 5)
#define MM_SET_EST_TIME            (WM_USER + 6)
#define MM_SET_DOWNLOAD_TYPE       (WM_USER + 7)
#define MM_SET_FILE_NAME           (WM_USER + 8)
#define MM_SET_DEST_DIR            (WM_USER + 9)

// Meter control has no notification codes to send to parent.

#include "varargs.h"

extern BOOL PASCAL StillConnected(void);
extern void SetLocal(BOOL bOnOff);
short CheckCommError(short sFlagsToCheck);
int BBSsync(void);
CRC CalcCRC(BYTE *cCRCBuf,WORD dCRC_BUFFSIZE);
int FAR PASCAL Initialize(WORD, WORD);
int  InitialConnect(void);
int  FAR PASCAL WSConnect(BOOL bVerify);
int FAR PASCAL Connect(void);
int FAR PASCAL Order(char *, char *);
int  FAR PASCAL Receive_Message(MSGTYPE *Msg);
int ReceiveBuf(HANDLE hCom,BYTE *buf,WORD nNumBytes,long nTimeToWait);
int ReceiveCRC(HANDLE hCom, char *buf, int iBufLen);
int  FAR Send_Message(MSGTYPE msg, ...);
int FAR PASCAL Download(char *szFileName, char *szDldPath,
                        BOOL bUpdateMeter, BOOL bSetNameWnd,
                        BOOL bIsThumbFile, BOOL bNeedTimeEst,
                        BOOL bNeedJobSize, WORD* wpNeedNewExt);
#if 0
extern LONG FAR PASCAL
WSCancelDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam) ;
#endif

void FAR PASCAL HangUp(void);
BOOL PASCAL LatestVersion(char *szFileName, char *szLatestFileName);
BOOL PASCAL LatestExe(void);
int WaitForModemTextMsg(char *msg, DWORD mSec);
int WaitForModemMsg(unsigned char msg,DWORD mSec);
int WriteModemCommand(char *szCommand);
int SendBuf(HANDLE hCom, char *buf, int iBufLen);
int SendCRC(HANDLE hCom, void *buf, int iBufLen);
int WaitForInBuf(WORD nNumChars, long mSec);
int WaitForACK(void);
void Wait(DWORD mSec);
int WaitForEmptyOutBuf(DWORD mSec);
int WaitForChar(unsigned char value);
int WaitForConnection(void);
extern int SetPortState(void);
CRC CalcCRC(BYTE *cCRCBuf,WORD dCRC_BUFFSIZE);
int SendByte(HANDLE hCom, BYTE ByteToSend);

extern BOOL bBackupThumb;
extern BOOL bStillConnected;

/* from xmodem.h */
extern void    FAR *variables;
int  XMReceive(char *szDestFileName, char *szCompFileName,
               char *szDldPath, WORD NumBlocks, BOOL bUpdateMeter,
               BOOL bNeedTimeEst, DWORD lUncompSize);
void close_buffers(void);
int  init_buffers(void);
DWORD  expand_buf(BYTE *input_buffer, 
                WORD size,                        // of input
                int  output_handle,               // file to write to
                BOOL bLast);
#endif
