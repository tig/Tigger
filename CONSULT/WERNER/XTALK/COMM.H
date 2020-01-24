/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Communication Include File (COMM.H)                             *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions:																   *
*	1.00 07/17/89 Initial Version.											   *
*	1.1  04/07/90 dca00072 PJL Change CommStrOutput decl. (REPLY '~'problem).  *
* 	1.1  04/12/90 dca00082 PJL Couldn't set Port to COM3/COM4 from a script.   *
*                                                                             *
\*****************************************************************************/

#define MODEM_NONE	0
#define MODEM_XON	1
#define MODEM_RTS	2
#define MODEM_BOTH	3

// dca00082 PJL Use MAX_PORT constant for highest port #.
#define MAX_PORT   4

/*---------------------------------------------------------------------------*/
/* Define communication variable used.					     */
/*---------------------------------------------------------------------------*/
extern WORD    wCommDevices;
extern char    cCommPort;	/* Communication Port */
extern int     nCommID; 	/* Communication ID, returned from OpenComm */
extern DCB     ComDCB;		/* Current DCB information on the Comm Port */
extern COMSTAT ComStat; 	/* Current status of the Communication Port */
extern WORD far *lpCommEvent;
extern int     nCommEvent;
extern int     nCommCount;

extern char    fCommStsRLSD;
extern char    fCommCharEvent;
extern char    fCommStsXON;

extern int     nCommFramingErrors;
extern int     nCommOverrunErrors;
extern int     nCommParityErrors;

extern long    lTimeInitSent;
extern long    lTimeConnected;
extern long    lTimeLastRecv;

extern int     hDemoFile;
extern char    fDemoPause;
extern char    fDemoStep;

extern char    fCommPause;

extern  void  CommDataStatus();
extern  int   CommDataRead(char *, int);
extern  void  CommDataInput();
extern  void  CommDataOutput(char *, int);
extern  void  CommCharOutput(char);
// dca00072 PJL Change CommStrOutput decl. (REPLY '~'problem).
extern  void  CommStrOutput(char *, BOOL);
extern  void  CommStartTimer();
extern  void  CommStopTimer();
extern  int   CommPortOpen();
extern  int   CommPortUpdate();
extern  void  CommPortDump();
extern  short CommFlushBuffers();
extern  int   CommPortClose();
extern  void  CommPortBreak(int);

extern  void CommConfig();

extern  BOOL InitCommunications();
extern  void TermCommunications();
extern  void ReadModemParameters(int);
extern  void LoadModemParameters();
extern  void SaveModemParameters();
extern  void LoadCustomParameters();
extern  void SaveCustomParameters();


#define CommGet()   (--nCommBufCnt >= 0 ? (int)(*pCommBufPtr++) : CommBufFil())
#define CommReset() (nCommBufCnt = fCommBufErr = 0)
#define CommSize()  (nCommBufCnt)
#define CommUnGet() (--pCommBufPtr, nCommBufCnt++)
#define CommError() (fCommBufErr ? (!(fCommBufErr = FALSE)) : FALSE)

extern	BYTE *pCommBufPtr;
extern	int  nCommBufCnt;
extern	char fCommBufErr;

extern  int  CommBufFil();

struct stComm
  {
  char	cPort;
  int	nSpeed;
  char	cDataBits;
  char	cParity;
  char	cStopBits;
  int	nBreakLen;
  char	cLocal;
  char	cFlow;
  char  cXonChar;
  char  cXoffChar;
  short sCommMode;
  char  filler[256];
  };

struct stCommOld
  {
  char	cPort;
  int	nSpeed;
  char	cDataBits;
  char	cParity;
  char	cStopBits;
  int	nBreakLen;
  char	cLocal;
  char	cFlow;
  char  cXonChar;
  char  cXoffChar;
  };

struct stModem
  {
  char  szName[41];
  char  szDialPrefix[41];
  char  szDialSuffix[21];
  char  szAnswerSetup[41];
  char  szModemInit[61];
  char  szModemHangup[25];
  char  cFiller;
  char	cChange;
  char  szCommandOk[25];
  char  szCommandError[25];
  char  szConnect[25];
  char  szConnectSpeed[25];
  char  szConnectARQ[25];
  char  szDialTone[25];
  char  szRingDetect[25];
  char  szNoDialTone[25];
  char  szBusy[25];
  char  szNoCarrier[25];
  char	cFiller1;
  };

extern struct stComm  Comm;
extern struct stModem Modem;
