//---------------------------------------------------------------------------
//
//  Module: zmodem.h
//
//  Purpose:
//     Private ZMODEM stuff.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              3/ 6/92   BryanW      Wrote it.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "..\apache\asciidef.h"
#include "..\apache\facelift.h"

#include "version.h"
#include "resource.h"

#define ZPAD       '*'          /* 052 Padding character begins frames */
#define ZDLE       0x18         /* Ctrl-X Zmodem escape - `ala BISYNC DLE */
#define ZDLEE      (ZDLE^0x40)  /* Escaped ZDLE as transmitted */
#define ZBIN       'A'          /* Binary frame indicator */
#define ZHEX       'B'          /* HEX frame indicator */
#define ZBIN32     'C'          /* Binary frame with 32 bit FCS */

// frame types

#define ZRQINIT    0    /* Request receive init */
#define ZRINIT     1    /* Receive init */
#define ZSINIT     2    /* Send init sequence (optional) */
#define ZACK       3    /* ACK to above */
#define ZFILE      4    /* File name from sender */
#define ZSKIP      5    /* To sender: skip this file */
#define ZNAK       6    /* Last packet was garbled */
#define ZABORT     7    /* Abort batch transfers */
#define ZFIN       8    /* Finish session */
#define ZRPOS      9    /* Resume data trans at this position */
#define ZDATA      10   /* Data packet(s) follow */
#define ZEOF       11   /* End of file */
#define ZFERR      12   /* Fatal Read or Write error Detected */
#define ZCRC       13   /* Request for file CRC and response */
#define ZCHALLENGE 14   /* Receiver's Challenge */
#define ZCOMPL     15   /* Request is complete */
#define ZCAN       16   /* Other end canned session with CAN*5 */
#define ZFREECNT   17   /* Request for free bytes on filesystem */
#define ZCOMMAND   18   /* Command from sending program */
#define ZSTDERR    19   /* Output to standard error, data follows */

// ZDEL sequences

#define ZCRCE      'h'  /* CRC next, frame ends, header packet follows */
#define ZCRCG      'i'  /* CRC next, frame continues nonstop */
#define ZCRCQ      'j'  /* CRC next, frame continues, ZACK expected */
#define ZCRCW      'k'  /* CRC next, ZACK expected, end of frame */
#define ZRUB0      'l'  /* Translate to rubout 0x7F */
#define ZRUB1      'm'  /* Translate to rubout 0xFF */

/* zdlread return values (internal) */
/* -1 is general error, -2 is timeout */

#define GOTOR   0x100
#define GOTCRCE (ZCRCE|GOTOR)  /* ZDLE-ZCRCE received */
#define GOTCRCG (ZCRCG|GOTOR)  /* ZDLE-ZCRCG received */
#define GOTCRCQ (ZCRCQ|GOTOR)  /* ZDLE-ZCRCQ received */
#define GOTCRCW (ZCRCW|GOTOR)  /* ZDLE-ZCRCW received */
#define GOTCAN  (GOTOR|0x18)    /* CAN*5 seen */

/* Byte positions within header array */

#define ZF0   3                /* First flags byte */
#define ZF1   2
#define ZF2   1
#define ZF3   0
#define ZP0   0                /* Low order 8 bits of position */
#define ZP1   1
#define ZP2   2
#define ZP3   3                /* High order 8 bits of file position */

/* Bit Masks for ZRINIT flags byte ZF0 */

#define CANFDX   0x01          /* Rx can send and receive true FDX */
#define CANOVIO  0x02          /* Rx can receive data during disk I/O */
#define CANBRK   0x04          /* Rx can send a break signal */
#define CANCRY   0x08          /* Receiver can decrypt */
#define CANLZW   0x10          /* Receiver can uncompress */
#define CANFC32  0x20          /* Receiver can use 32 bit Frame Check */
#define ESCCTL   0x40          /* Receiver expects ctl chars to be escaped */
#define ESC8     0x80          /* Receiver expects 8th bit to be escaped */

/* Parameters for ZSINIT frame */

#define ZATTNLEN 32           /* Max length of attention string */

/* Bit Masks for ZSINIT flags byte ZF0 */

#define TESCCTL 0x40      /* Transmitter expects ctl chars to be escaped */
#define TESC8   0x80      /* Transmitter expects 8th bit to be escaped */

/* Parameters for ZFILE frame */
/* Conversion options one of these in ZF0 */

#define ZCBIN   1         /* Binary transfer - inhibit conversion */
#define ZCNL    2         /* Convert NL to local end of line convention */
#define ZCRESUM 3         /* Resume interrupted file transfer */

/* Management include options, one of these ored in ZF1 */

#define ZMSKNOLOC   0x80 /* Skip file if not present at rx */

/* Management options, one of these ored in ZF1 */

#define ZMMASK   0x1F     /* Mask for the choices below */
#define ZMNEWL   0x01     /* Transfer if source newer or longer */
#define ZMCRC    0x02     /* Transfer if different file CRC or length */
#define ZMAPND   0x03     /* Append contents to existing file (if any) */
#define ZMCLOB   0x04     /* Replace existing file */
#define ZMNEW    0x05     /* Transfer if source newer */

/* Number 5 is alive ... */

#define ZMDIFF   0x06     /* Transfer if dates or lengths different */
#define ZMPROT   0x07     /* Protect destination file */

/* Transport options, one of these in ZF2 */

#define ZTLZW    0x01     /* Lempel-Ziv compression */
#define ZTCRYPT  0x02     /* Encryption */
#define ZTRLE    0x03     /* Run Length encoding */

/* Extended options for ZF3, bit encoded */

#define ZXSPARS  0x40    /* Encoding for sparse file operations */

/* Parameters for ZCOMMAND frame ZF0 (otherwise 0) */

#define ZCACK1   1        /* Acknowledge, then do command */

#define SENT_INIT    1
#define SENT_FILE    2
#define SENT_DATA    3
#define SENT_ZFIN    4

#define RECV_INIT    5
#define RECV_FILE    6
#define RECV_DATA    7
#define RECV_ZFIN    8

#define PHASE_SYNC   1
#define PHASE_PAD    2
#define PHASE_TYPE   3
#define PHASE_BINARY 4
#define PHASE_HEX    5
#define PHASE_ZFIN   6

// direction flag

#define DIR_SEND     0
#define DIR_RECEIVE  1

// error constants

#define ERROR_LOCALCANCEL     0xFFFF
#define ERROR_TOOMANYERRORS   0xFFFE
#define ERROR_TIMEOUT         0xFFFD
#define ERROR_GENERAL         0xFFFC
#define ERROR_HOSTCANCEL      0xFFFB
#define ERROR_DISKERROR       0xFFFA
#define ERROR_DISKFULL        0xFFF9
#define ERROR_NAK             0xFFF8

#define ZMODEM_TIMEOUT        1
#define ZMODEM_HDR            2
#define ZMODEM_CAN            3
#define ZMODEM_DATA           4
#define ZMODEM_ERROR          5
#define ZMODEM_EOF            6

// atom declarations
                      
#define ATOM_NPZCB            0x100

// max length constants

#define MAXLEN_ATTNSTR        33
#define MAXLEN_PATH           164
#define MAXLEN_FILENAME       32
#define MAXLEN_INBUFFER       2100
#define MAXLEN_OUTBUFFER      2100
#define MAXLEN_INCOMING       16384
#define MAXLEN_TEMPSTR        81

// timer definitions

#define TIMERID_ZMODEMFSM        0x100
#define TIMERINTERVAL_ZMODEMFSM  500

// notification / communication messages

#define WM_CMD_RXBLK          (WM_USER + 0x2000)
#define WM_CMD_TXBLK          (WM_USER + 0x2001)

// data structures

typedef struct tagFILESTATUS
{
  WORD   wDate ;
  WORD   wTime ;
  DWORD  dwLength ;

} FILESTATUS, FAR *LPFILESTATUS ;

typedef struct tagZCB
{
   char   szAttn[ MAXLEN_ATTNSTR ],
          szFileName[ MAXLEN_PATH + MAXLEN_FILENAME ],
          szInBuffer[ MAXLEN_INBUFFER ], szIncoming[ MAXLEN_INCOMING ],
          szOutBuffer[ MAXLEN_OUTBUFFER ] ;
   int    nCanCount, nCharCount, nIncomingCount, nIncomingPos, 
          nLastChar, nLength, nNulls, nOutCount, nTimeoutInterval ;
   BOOL   fAborting, fCtlEsc, fEscSeq, fHeaderNext, fReadError, fSendHeader,
          fStreaming, fTxFCS32, fUseStream ;
   DWORD  dwFileBytes, dwFileSize, dwPosition, dwReceived,
          dwTimeout, dwTransmitted, dwTimeRead, dwTimeStarted ;
   HWND   hDlg, hParentWnd ;
   HFILE  hFile ;
   WORD   wBlockCount, wBlockSize, wConsecErrors, wCurFile, wDirection,
          wFileDate, wFileTime, wLastError, wMaxErrors, wPhase, wState,
          wTotalBlocks, wTotalErrors, wTotalFiles ;

} ZCB, NEAR *NPZCB ;

// macros to access elements in ZCB

#define ABORTING( x )        (x -> fAborting)
#define ATTN( x )            (x -> szAttn)
#define BLOCKCOUNT( x )      (x -> wBlockCount)
#define BLOCKSIZE( x )       (x -> wBlockSize)
#define CANCOUNT( x )        (x -> nCanCount)
#define CHARCOUNT( x )       (x -> nCharCount)
#define CONSECERRORS( x )    (x -> wConsecErrors)
#define CTLESC( x )          (x -> fCtlEsc)
#define CURFILE( x )         (x -> wCurFile)
#define DIRECTION( x )       (x -> wDirection)
#define ESCSEQ( x )          (x -> fEscSeq)
#define FILEBYTES( x )       (x -> dwFileBytes)
#define FILEDATE( x )        (x -> wFileDate)
#define FILENAME( x )        (x -> szFileName)
#define FILESIZE( x )        (x -> dwFileSize)
#define FILETIME( x )        (x -> wFileTime)
#define HDLG( x )            (x -> hDlg)
#define HEADERNEXT( x )      (x -> fHeaderNext)
#define HFILE( x )           (x -> hFile)
#define HPARENTWND( x )      (x -> hParentWnd)
#define INCOMING( x )        (x -> szIncoming)
#define INCOMINGCOUNT( x )   (x -> nIncomingCount)
#define INCOMINGPOS( x )     (x -> nIncomingPos)
#define INDATA( x )          (x -> szInBuffer)
#define LASTCHAR( x )        (x -> nLastChar)
#define LASTERROR( x )       (x -> wLastError)
#define LENGTH( x )          (x -> nLength)
#define MAXERRORS( x )       (x -> wMaxErrors)
#define NULLS( x )           (x -> nNulls)
#define OUTCOUNT( x )        (x -> nOutCount)
#define OUTDATA( x )         (x -> szOutBuffer)
#define PHASE( x )           (x -> wPhase)
#define POSITION( x )        (x -> dwPosition)
#define READERROR( x )       (x -> fReadError)
#define RECEIVED( x )        (x -> dwReceived)
#define SENDHEADER( x )      (x -> fSendHeader)
#define STATE( x )           (x -> wState)
#define STREAMING( x )       (x -> fStreaming)
#define TIMEOUT( x )         (x -> dwTimeout)
#define TIMEOUTINTERVAL( x ) (x -> nTimeoutInterval) 
#define TIMEREAD( x )        (x -> dwTimeRead)
#define TIMESTARTED( x )     (x -> dwTimeStarted)
#define TOTALBLOCKS( x )     (x -> wTotalBlocks)
#define TOTALERRORS( x )     (x -> wTotalErrors) 
#define TOTALFILES( x )      (x -> wTotalFiles)
#define TRANSMITTED( x )     (x -> dwTransmitted)
#define USESTREAM( x )       (x -> fUseStream)
#define TXFCS32( x )         (x -> fTxFCS32)

#define UPDCRC16(cp, crc) (CRC16Table[((crc >> 8) & 255)] ^ (crc << 8) ^ cp)

// property helper macros

#define SET_PROP( x, atom,  y )  SetProp( x, MAKEINTATOM( atom ), y )
#define GET_PROP( x, atom )      GetProp( x, MAKEINTATOM( atom ) )
#define REMOVE_PROP( x, atom )   RemoveProp( x, MAKEINTATOM( atom ) )

// global variables

#ifdef GLOBALDEFS
int      nUseCount ;
HANDLE   hInstLib ;
#else
extern int      nUseCount ;
extern HANDLE   hInstLib ;
#endif

// function prototypes (private)

BOOL NEAR UpdateDialogBox( NPZCB ) ;
NPZCB NEAR CreateZCB( VOID ) ;
VOID NEAR DestroyZCB( NPZCB ) ;
HFILE NEAR OpenXferFile( NPZCB ) ;
VOID NEAR CloseXferFile( NPZCB ) ;
BOOL NEAR UpdateErrorCount( NPZCB ) ;
VOID NEAR InitZmodem( NPZCB ) ;
VOID NEAR CompleteZmodem( NPZCB ) ;
VOID NEAR TerminateZmodem( NPZCB, int ) ;
BOOL NEAR ZmodemFSM( HWND ) ;
BOOL NEAR ProcessZmodemReceiveState( NPZCB ) ;
BOOL NEAR ProcessZmodemSendState( NPZCB ) ;

int NEAR ReadBufferChar( NPZCB ) ;
VOID NEAR ReadAnyHdrInit( NPZCB ) ;
int NEAR ReadAnyHdr( NPZCB ) ;
VOID NEAR ReadBinDataInit( NPZCB ) ;
int NEAR ReadBinData( NPZCB ) ;
VOID NEAR SendBinHdr( NPZCB, int, LPSTR ) ;
VOID NEAR SendHexHdr( NPZCB, int, LPSTR ) ;
VOID NEAR SendBinData( NPZCB, int, LPSTR, int ) ;
VOID NEAR ResetWriteBuffer( NPZCB ) ;
VOID NEAR WriteBufferChar( NPZCB, int ) ;
VOID NEAR WriteBufferData( NPZCB, int ) ;
VOID NEAR WriteBufferHexC( NPZCB, int ) ;
VOID NEAR SendWriteBuffer( NPZCB ) ;

VOID NEAR XmitZRINITHdr( NPZCB ) ;
VOID NEAR ProcessZSINITHdr( NPZCB ) ;
VOID NEAR ProcessZSINITData( NPZCB ) ;
VOID NEAR ProcessZFILEHdr( NPZCB ) ;
BOOL NEAR ProcessZFILEData( NPZCB ) ;
VOID NEAR ProcessZDATAHdr( NPZCB ) ;
BOOL NEAR ProcessZDATAData( NPZCB ) ;
VOID NEAR XmitZACK( NPZCB ) ;
VOID NEAR XmitZNAK( NPZCB ) ;
VOID NEAR XmitZRPOS( NPZCB ) ;
BOOL NEAR ProcessZNAK( NPZCB ) ;
VOID NEAR XmitZFIN( NPZCB ) ;
VOID NEAR XmitZRQINIT( NPZCB ) ;
VOID NEAR ProcessZRINITHdr( NPZCB ) ;
VOID NEAR ProcessZRPOS( NPZCB ) ;
BOOL NEAR XmitZFILE( NPZCB ) ;
BOOL NEAR XmitZDATA( NPZCB ) ;
VOID NEAR XmitZEOF( NPZCB ) ;

VOID NEAR DateTimeToBuf( NPZCB, LPSTR ) ;
VOID NEAR DWORDToBuf( LPSTR, DWORD ) ;
VOID NEAR DWORDToDateTime( NPZCB, DWORD ) ;
BOOL NEAR VerifyBlock( LPSTR, int ) ;
VOID NEAR SendAbortString( HWND ) ;
BOOL NEAR GetFileStatus( HFILE, LPFILESTATUS ) ;
BOOL NEAR SetFileStatus( HFILE, LPFILESTATUS ) ;
LPSTR NEAR StripPath( LPSTR ) ;

BOOL NEAR DlgDrawItem( HWND, LPDRAWITEMSTRUCT ) ;
BOOL NEAR DlgMeasureItem( HWND, LPMEASUREITEMSTRUCT ) ;

// function prototypes (exported)

int FAR PASCAL GetZmodemUseCount( VOID ) ;
HWND FAR PASCAL ZmodemReceive( HWND, LPSTR ) ;
BOOL FAR PASCAL ZmodemDlgProc( HWND, UINT, WPARAM, LPARAM ) ;

//---------------------------------------------------------------------------
//  End of File: zmodem.h
//---------------------------------------------------------------------------

