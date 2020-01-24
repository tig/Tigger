/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Kermit Include File (KERMIT.H)                                  *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <dos.h>

#define KERMIT_MAXBUF	    1100

#define SP                  ' '

#define PACKET_SIZE	    90
#define LOCAL_TIMEOUT	    20
#define REMOTE_TIMEOUT	    24
#define PADDING_NUM	    0
#define CONTROL_QUOTE       '#'
#define EIGHT_BIT_PREFIX    '&'
#define REPEAT_PREFIX       '~'
#define PADDING_CHAR        '@'
#define EOL_CHAR            '\r'
#define START_CHAR	    1

#define WAIT_FOR_COMMAND    2
#define WAIT_FOR_REPLY	    3
#define WAIT_TO_END         4
#define WAIT_TO_ABORT      20

#define SEND_SEND_INIT	    5
#define SEND_SEND_START     6
#define SEND_FILE_HEADER    7
#define SEND_FILE_DATA	    8
#define SEND_END_FILE	    9
#define SEND_END_BATCH	   10
#define SEND_BREAK	   11
#define WAIT_SEND_INIT	   12
#define WAIT_FILE_HEADER   13
#define WAIT_FILE_DATA	   14
#define WAIT_COMMAND_SYNC  15
#define ABORT_SERVER	   16

#define KM_NULL 	    0
#define KM_IDLE 	    1
#define KM_BUSY 	    2
#define KM_SERVER	    3

#define KRMERR_PACKET_N     2
#define KRMERR_PACKET_Q     3
#define KRMERR_PACKET_U     4

#define KRMSRC_USER	    0
#define KRMSRC_SCRIPT	    1
#define KRMSRC_DDE	    2

#define TKN_ALL 	     1
#define TKN_BATCH	     2
#define TKN_BAUD	     3
#define TKN_BELL	     4
#define TKN_BLOCKCHECK	     5
#define TKN_BYE 	     6
#define TKN_CANCEL	     7
#define TKN_CLEAR	     8
#define TKN_CLOSE	     9
#define TKN_COMMENT	    10
#define TKN_CONNECT	    11
#define TKN_CONTROL         12
#define TKN_CONVERTED       13
#define TKN_COPY            14
#define TKN_CTRLZ           15
#define TKN_CWD             16
#define TKN_DEBUG           17
#define TKN_DEFINE          18
#define TKN_DELAY	    19
#define TKN_DELETE	    20
#define TKN_DIAL	    21
#define TKN_DIRECTORY	    22
#define TKN_DISCARD	    23
#define TKN_DISK	    24
#define TKN_ECHO	    25
#define TKN_ENDOFPACKET     26
#define TKN_EOF 	    27
#define TKN_ERASE	    28
#define TKN_ERROR	    29
#define TKN_EVEN	    30
#define TKN_EXIT	    31
#define TKN_FILE	    32
#define TKN_FINISH	    33
#define TKN_GET 	    34
#define TKN_HANGUP	    35
#define TKN_HELP	    36
#define TKN_HOST	    37
#define TKN_INCOMPLETE	    38
#define TKN_INITIAL	    39
#define TKN_INPUT	    40
#define TKN_KEEP	    41
#define TKN_KERMIT	    42
#define TKN_LITERAL	    43
#define TKN_LOCAL	    44
#define TKN_LOGIN	    45
#define TKN_MODE	    46
#define TKN_NAMES	    47
#define TKN_NOCTRLZ	    48
#define TKN_NONE	    49
#define TKN_ODD 	    50
#define TKN_OFF 	    51
#define TKN_ON		    52
#define TKN_OUTPUT	    53
#define TKN_PACKETLENGTH    54
#define TKN_PACKETS	    55
#define TKN_PADCHARACTER    56
#define TKN_PADDING	    57
#define TKN_PARITY	    58
#define TKN_PAUSE	    59
#define TKN_PRINT	    60
#define TKN_PROGRAM	    61
#define TKN_PUSH	    62
#define TKN_QUIT	    63
#define TKN_QUOTE	    64
#define TKN_RECEIVE	    65
#define TKN_REMOTE	    66
#define TKN_RENAME	    67
#define TKN_RETRY	    68
#define TKN_RUN 	    69
#define TKN_SCRIPT	    70
#define TKN_SEND	    71
#define TKN_SERVER	    72
#define TKN_SESSION	    73
#define TKN_SET 	    74
#define TKN_SHOW	    75
#define TKN_SPACE	    76
#define TKN_STARTOFPACKET   77
#define TKN_STATISTICS	    78
#define TKN_SUBMIT	    79
#define TKN_SUPERSEDE	    80
#define TKN_TAKE	    81
#define TKN_TIMEOUT	    82
#define TKN_TIMER	    83
#define TKN_TRANSMIT	    84
#define TKN_TRANSACTIONS    85
#define TKN_TYPE	    86
#define TKN_WARNING	    87
#define TKN_WHO 	    88


#define tochar(c)   ((c) + SP)
#define unchar(c)   ((c) - SP)
#define ctl(c)	    ((c) ^ 64)

extern void near SendStart();
extern void near SendAnother();
extern void near SendStartAck();
extern void near SendHeader();
extern void near SendAttr();
extern void near SendData();
extern void near SendEof();
extern void near SendEob();
extern void far  GetOurParams(PSTR);

extern void NEAR RcvStart();
extern void NEAR RcvInit();
extern void NEAR RcvFile();
extern void NEAR RcvText();
extern void NEAR RcvAttr();
extern void NEAR RcvData();
extern void NEAR RcvEOF();
extern void NEAR RcvPar(char *);
extern void NEAR RcvServerCommand();
extern void NEAR RequestFileCommand();

extern int  NEAR ReceivePacket();
extern int  NEAR PacketTimeOut();
extern void NEAR SendPacket(char, char, int, char *);
extern void	 SendPacketX(char, char, int, char *);
extern void NEAR AckPacket();
extern void NEAR AckSPacket(char *);
extern void near ReReceive();
extern void NEAR ReSend();
extern int  near DecodeBuffData(PSTR, PSTR, int);
extern int  near EncodeBuffData(PSTR, PSTR, int);


extern int  nKermitMode;
extern int  nKermitStatus;
extern int  KermitCommand(char *, int);
extern int  KermitMessage(PSTR);
extern USHORT KermitRemoteMessage(PSTR);
extern int  KermitGetFile(PSTR, PSTR);
extern int  KermitReceive(PSTR);
extern int  KermitSendFile(PSTR, PSTR);
extern BOOL KermitConnect();
extern BOOL KermitHangup();

extern void KermitFileName(PSTR);
extern void KermitConvert(PSTR);

extern void KermitUserAbort();

extern BOOL InitKermit(BOOL);
extern BOOL TermKermit();
extern BOOL CreateKermitChild(HWND);
extern void DestroyKermitChild();

extern HWND hKermitChild;

extern BOOL bKermit;
extern BOOL bKermitNonServer;

extern USHORT KermitOpen(char *pName, BOOL fWrite);
extern USHORT KermitClose(void);
extern SHORT  KermitFlush(short sChar);
extern SHORT  KermitFill(void);
extern SHORT  KermitUnget(short sChar);

#define KermitGetChar() (--filexfer.kermit.iBuf >= 0 ? \
			 0xFF & *filexfer.kermit.pBuf++ : KermitFill())
#define KermitPutChar(c) (--filexfer.kermit.iBuf >= 0 ? \
			 *filexfer.kermit.pBuf++ = (BYTE)(c) : \
			 (int)KermitFlush(c))
