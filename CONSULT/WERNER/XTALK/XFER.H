/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   File Transfer Include File (XFER.H)                             *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions:																   *
*  1.00 07/17/89 Initial Version.											   *
*  1.01 08/08/89 00007    Change Kermit logic to not use the CR char as an	   *
*                         event flag.  Added input storage.				   *
*  1.1  03/12/90 dca00045 PJL Added wFileDate & wFileTime fields to the	   *
*                         ZMODEM filexfer data structure (file recovery).     *
*                                                                             *
\*****************************************************************************/

#define XFER_XTALK	0
#define XFER_DART	1
#define XFER_MODEM7	2
#define XFER_XMODEM	3
#define XFER_XMODEM1K	4
#define XFER_YMODEM	5
#define XFER_KERMIT     6
#define XFER_CSERVEB    7
#define XFER_ZMODEM     8

#define XFER_WAIT_NONE	    0
#define XFER_WAIT_ECHO	    1
#define XFER_WAIT_PROMPT    2
#define XFER_WAIT_COUNT     3
#define XFER_WAIT_DELAY     4

#define XFER_TIMING_TIGHT   0
#define XFER_TIMING_NORMAL  1
#define XFER_TIMING_LOOSE   2
#define XFER_TIMING_SLOPPY  3

#define XFER_RECV_NORMAL    0
#define XFER_RECV_NEWER     1
#define XFER_RECV_ALL       2

#define XFER_ERROR_NONE     0
#define XFER_ERROR_CRC	    1
#define XFER_ERROR_TIMEOUT  2
#define XFER_ERROR_SEQ	    3
#define XFER_ERROR_NAK	    4
#define XFER_ERROR_DUP	    5
#define XFER_ERROR_LONG     6
#define XFER_ERROR_GENERAL  7

#define XFER_NUL_WAIT	    0
#define XFER_SND_WAIT	    1
#define XFER_RCV_WAIT	    2

struct stXfer
  {
  char cProtocol;
  char cLWait;
  char szLWaitPrompt[9];
  BYTE cLWaitCount;
  int  nLWaitDelay;
  char cCWait;
  int  nCWaitDelay;
  char szLineDelim[9];
  char szBlankEx[9];
  char cWordWrap;
  char fTabEx;
  char cLineTime;
  int  nPacketSize;
  int  nfiller;
  char fRecovery;
  char fDateMode;
  char fHostMode;
  char cMaxErrors;
  char fPmode;
  char cTiming;
  char cFileType;
  char cCheckType;
  char cHeader;
  char cControlQuote;
  char cBinaryQuote;
  char cRecvMode;
  };

extern struct stXfer Xfer;

struct SCRIPT
  {
  BYTE data[5120];
  };

struct ASCII
  {
  int  hFile;
  BOOL fEof;
  int  nState;
  long lTimeOut;
  int  lines;
  long lTime;
  char fEOF;
  char line[256];
  int  length;
  int  index;
  int  count;
  char prompt[16];
  int  plength;
  int  pindex;
  int  nEchoCh;
  int  fSkip;
  };

struct CROSS
  {
  int	    nProtocol;
  char	    cWait;
  char	    cState;
  BYTE	    cSeqNo;
  BYTE      sData[2620];
  BYTE	    sSend[1128];
  int	    nPacketSize;
  int	    nSendSize;
  int	    nChrCnt;
  char	    cCANCnt;
  int	    nTimeOut;
  long	    lTimeOut;
  int	    file;
  BOOL	    fEof;
  char	    szRequest[MAX_PATH];
  struct find_t sendfile;       /* DOS FindFirst/FindNext status buffer */
  char      bExtra[32];
  char      fDisplay;
  char      fServer;
  char      fCancel;
  };

struct KERMIT
  {
  char	    fExchangedParameters;
  char	    cMode;
  int	    nSndLen;
  BOOL	    bAbortFile;
  BOOL	    bAbortBatch;
  char	    fAbortKermit;
  char	    fTextFlag;
  char	    fBellOn;
  int	    nPause;
  char	    fEofCtrlZ;
  char	    fFileNamesConverted;
  char	    fFileSupersede;
  char	    fFileWarning;
  char	    fFileKeep;
  int	    nInitialErrors;
  int	    nPacketErrors;
  char	    fTakeEcho;
  char	    fTakeError;
  char	    fTimer;
  char	    cSndSeq;
  char	    cSndType;
  char	    sSndPacket[1100];
  char	    cSndChk[3];
  int	    nSndPacketSize;
  char	    nSndDataOffset;
  BOOL	    bSndPacketSizeOverride;
  int	    nSndTimeOut;
  BOOL	    bSndTimeOutOverride;
  char	    cSndStartChar;
  char	    cSndCtlq;
  char	    nSndPadNum;
  char	    cSndPadChar;
  char	    cSndEOL;
  char	    cSndPadCtlq;
  BOOL	    bSndRepeatPrefixFlag;
  char	    cSndRepeatPrefix;
  char      sInputPacket[1100];            // 00007
  short     iInputPacket;                  // 00007
  int	    nRcvLen;
  char	    cRcvSeq;
  char	    cRcvType;
  char	    sRcvPacket[1100];
  char	    cRcvChk[3];
  int	    nRcvPacketSize;
  char	    nRcvDataOffset;
  int	    nRcvTimeOut;
  char	    cRcvStartChar;
  char	    cRcvCtlq;
  char	    nRcvPadNum;
  char	    cRcvPadChar;
  char	    cRcvEOL;
  BOOL	    bRcvRepeatPrefixFlag;
  char	    cRcvRepeatPrefix;
  char	    cPrvSeq;
  char	    cCurSeq;
  char	    cBlockCheck;
  BOOL	    bEightBitPrefixFlag;
  char	    cEightBitPrefix;
  BOOL	    bExtendedHeaderFlag;
  int	    nDelay;
  int	    nCycleDelay;
  int	    nPktCnt;
  int	    nPktState;
  int	    nProtocol;
  int	    nPacketSize;
  long	    lTimeOut;
  HFILE     hFile;
  BOOL	    fWrite;
  BOOL	    fEof;
  BYTE	    abBuf[MAX_BUFFER];
  BYTE	    *pBuf;
  short     iBuf;
  long	    lEstStopTime;
  long	    lEstStartBytes;
  WORD	    wCurAvgBytes;
  long	    lStatFileChars;
  long	    lStatCommIn;
  long	    lStatCommOut;
  long	    lStatStartTime;
  long	    lStatStopTime;
  char	    fStatLastXfer;
  char	    fWildCardSend;	/* True if we are sending multible file */
  struct find_t sendfile;       /* DOS FindFirst/FindNext status buffer */
  char      bExtra[32];
  };

struct XMODEM
  {
  char	    cWait;
  char	    cState;
  BYTE	    cSeqNo;
  BYTE	    sData[1029];
  char	    cCRC;
  int	    nChrCnt;
  char	    cCANCnt;
  char	    cEOTCnt;
  int	    nTimeOut;
  long	    lTimeOut;
  int	    file;
  BOOL	    fEof;
  char      fFirstNak;
  };

struct YMODEM
  {
  int	    nProtocol;
  char	    cWait;
  char	    cState;
  BYTE	    cSeqNo;
  BYTE	    sData[2048];
  int	    nChrCnt;
  char	    cCANCnt;
  char	    cEOTCnt;
  int	    nTimeOut;
  long	    lTimeOut;
  int	    file;
  BOOL	    fEof;
  struct find_t sendfile;       /* DOS FindFirst/FindNext status buffer */
  char      bExtra[32];
  char      fFirstNak;
  };

struct DART
  {
  unsigned  wFileDate;
  unsigned  wFileTime;
  char	    fFileMode;
  char	    cWait;
  char	    cState;
  BYTE	    cSeqNo;
  BYTE	    sData[1128];
  int	    nPacketSize;
  BYTE	    sSend[1128];
  int	    nSendLen;
  int	    nChrCnt;
  char	    cCANCnt;
  int	    nTimeOut;
  long	    lTimeOut;
  int	    file;
  BOOL	    fEof;
  int	    filechk;
  char	    szRequest[MAX_PATH];
  struct find_t sendfile;       /* DOS FindFirst/FindNext status buffer */
  char      bExtra[32];
  int	    nSendPacketSize;
  char      fDisplay;
  char      fServer;
  int       nLastCh;
  };

struct QUICKB
  {
  BOOL      bWait;
  char	    cState;
  BYTE	    cSeqNo;
  BYTE      sData[2560];
  int       cData;
  int       iData;
  char	    cCANCnt;
  char	    cEOTCnt;
  int	    nTimeOut;
  long	    lTimeOut;
  int	    file;
  BOOL	    fEof;
  char      ws;
  char      wr;
  char      bs;
  char      cm;
  char      dq;
  char      esc;
  char      end;
  BYTE      byLastAck;
  BYTE      aTextToQuote[256];
  BYTE      aQuoteToText[256];
  };

struct ZMODEM
  {
  char	    cState;
  char	    cPhase;
  char     sData[2100];
  int	    nChrCnt;
  int	    nLength;
  int	    nCanCnt;
  long	    lTimeOut;
  int	    nTimeOut;
  BOOL	    faborting;
  int	    errorcode;
  int	    delay;
  int	    file;
  BOOL	    fEof;
  char	    path[128];
  int	    nBlocks;
  int	    nLastCh;
  char	    fEscSeq;
  char	    fCtlEsc;
  char      fHeader;
  char      fStream;
  char      fStreaming;         // True if we are currently out streaming.
  char      szAttn[36];
  int       nRcvBuf;
  int	    nZnulls;
  long	    lPos;
  char	    fNextIsHeader;
  char      sReSendData[2100];
  int	    nReSendCnt;
  struct find_t sendfile;       /* DOS FindFirst/FindNext status buffer */
  char      bExtra[32];
  unsigned  wFileDate;		// dca0045
  unsigned  wFileTime;		// PJL
  };

union FILEXFER
  {
  struct SCRIPT stack;
  struct ASCII	ascii;
  struct CROSS	cross;
  struct KERMIT kermit;
  struct XMODEM xmodem;
  struct YMODEM ym;
  struct DART	dart;
  struct QUICKB qb;
  struct ZMODEM zmodem;
  };

extern union FILEXFER filexfer;


extern HWND	hXferDlg;
extern FARPROC lpXferDlg;
extern char    szXferFilePath[MAX_PATH];
extern WORD	wXferTotalErrors;
extern WORD	wXferConsecErrors;
extern WORD	wXferLastError;

extern WORD	wXferTotalFiles;	/* Total number of files to xfer    */
extern DWORD   dwXferTotalBytes;	/* Total number of bytes to xfer    */
extern WORD	wXferCurrFiles; 	/* Current file number being xfer   */
extern DWORD   dwXferCurrBytes; 	/* Total bytes xfer so far	    */
extern long	lXferFileBytes; 	/* Bytes xfer in active file xfer   */
extern long	lXferFileSize;		/* File size of active file xfer    */
extern int	nXferBlockSize; 	/* Size of last block xfer (info)   */
extern WORD	wXferBlockCount;	/* Number of blocks in active xfer  */
extern char	fXferDirection; 	/* True for sending, else receiving */
extern char	fXferChangedComm;	/* The comm params changed for xfer */
extern BOOL     fHideXferWindow;        // True if the xfer window is hidden

extern int	nXferRcvWait;		/* Wait for idle on line then abort */

extern WORD     crc_table[];

extern void InitFileTransfer();
extern void HideFileTransfer();
extern void TermFileTransfer();

extern void InitXferIndicators();
extern void ResetXferIndicators(char);
extern void UpdateXferIndicators();
extern void PaintXferGraphics(HWND);
extern BOOL InitProtocol();
extern void NewProtocolData();
extern void UpdateProtocol();
extern int  XferUploadBatch(char *, char *);
extern int  XferUploadFile(char *, OFSTRUCT *);
extern int  XferDownloadFile(char *, OFSTRUCT *);
extern int  XferFileRequest();

extern int  XferCrossSendName(char *);
extern int  XferCrossRecvName(char *);
extern int  XferCrossServer();
extern int  XferDartSendName(char *);
extern int  XferDartRecvName(char *);
extern int  XferDartServer(char);
extern int  XferZmodemSendName(char *);
extern int  XferZmodemRecvName(char *);
extern int  XferKermitSendName(char *);
extern int  XferKermitRecvName(char *);
extern int  XferKermitRequestName(char *);

extern int  XferQuickBSendName(char *);
extern int  XferQuickBRecvName(char *);
extern int  XferQuickBStart();

extern WORD   ComputeFilesToSend(char *, struct find_t *);
extern USHORT CreateXferFile(char *, int *);
extern int    OpenXferFile(char *);
