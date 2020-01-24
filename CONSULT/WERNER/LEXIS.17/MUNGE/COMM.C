/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Communication Module (COMM.C)				      */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/*  Remarks: This communication module was design to meet the communications  */
/*           needs of the WinLexis project, to be modular, and efficient. Any */
/*	     feature not needed to support the project was not included.  I   */
/*	     did not want or need to write a general purpose communication    */
/*	     module.  But I needed to put all communication relative logic in */
/*	     one place for future ports to other environments.	This is all   */
/*	     well and good, except that I added some of the AI filter logic   */
/*	     here.   Given the code is well documented, adding the AI filter  */
/*	     with the communication code does improve speed, and does not     */
/*	     make the module harder to maintain.			      */
/*									      */
/*	     The AI filter routines are rather simple.	All data going thru   */
/*	     the communication port is broken down into lines.	Lines are     */
/*	     terminated with CR and 'juck' is removed.	Then the completed    */
/*	     lines are scanned and flags are set if conditions are met.  If   */
/*	     filters detect something useful, the AI (other module) routines  */
/*	     are called.  Thus most of the 'grunt' work is done here which    */
/*	     make us efficient, and the real AI work is done else where which */
/*	     make us modular.  You can have your cake and eat it too.	      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <dos.h>
#include "lexis.h"
#include "comm.h"
#include "data.h"
#include "aiscan.h"
#include "diary.h"
#include "record.h"
#include "report.h"
#include "session.h"
#include "term.h"


/******************************************************************************/
/*									      */
/*  Input Global Variables                                                    */
/*									      */
/******************************************************************************/

  SHORT  sCommPort;		/* Communication port, 0 is COM1, 1 is COM2 */
  SHORT  sCommSpeed;		/* Speed (or baud rate) of serial port	    */
  SHORT  sCommParity;		/* Parity, 0 is EVEN, 1 is ODD, 2 is NONE   */
  BOOL	 fCommHardware; 	/* Use hardware handshaking, CTS/RTS	    */
  BOOL   fCommSoftware;         /* Use software handshaking, XON/XOFF       */
  BOOL	 fCommLocal;		/* Local connection, hardwired, no modem    */


/******************************************************************************/
/*									      */
/*  Output Global Variables                                                   */
/*									      */
/******************************************************************************/

  SHORT  sCommID;               /* Handle from communication driver         */
  BOOL   fCommConnected;        /* Connected to the communication port      */
  BOOL   fCommCarrier;          /* Carrier has been detected                */
  USHORT usCommErrorCode;	/* Error code from device on bad return     */
  ULONG  ulCommLastRecv;        /* Time data was last received              */
  ULONG  ulCommLastSent;        /* Time data was last transmitted           */


/******************************************************************************/
/*									      */
/*  Local Constants							      */
/*									      */
/******************************************************************************/

#define COMM_INPBUF     4096    /* Define the size of our input buffer      */
#define COMM_OUTBUF     2048    /* Define the size of our output buffer     */
#define COMM_LINE       256     /* Define the size of the line buffers      */


/******************************************************************************/
/*									      */
/*  Local Variables							      */
/*									      */
/******************************************************************************/

  static BYTE  abInpLine[COMM_LINE];    /* Input AI line buffer             */
  static SHORT ibInpLine;               /* Input AI line buffer index       */
  static BYTE  abOutLine[COMM_LINE];    /* Output AI line buffer index      */
  static SHORT ibOutLine;               /* Output AI line buffer index      */
  static SHORT sFilterEscape;           /* Flag used to skip escape seq     */
  static SHORT iColLine;


/******************************************************************************/
/*									      */
/*  Local Procedures Prototypes 					      */
/*									      */
/******************************************************************************/

  static BOOL near QueryCarrierDetect();


/******************************************************************************/
/*									      */
/*  Global Procedures							      */
/*									      */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL CommInit();                                                          */
/*									      */
/*  Called in the application's initialize routine so we can get everything   */
/*  in order.  Returns true is successful (for future possiblities).          */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL CommInit()
{

  /*
  **  Initialize all the Input Global Variables to their defaults.
  */
  sCommPort	  = 0;
  sCommSpeed	  = 1200;
  sCommParity	  = COMM_EVEN;
  fCommHardware   = TRUE;
  fCommLocal	  = FALSE;

  /*
  **  Initialize all the Output Global Variables to their proper values.
  */
  fCommConnected  = FALSE;
  fCommCarrier	  = FALSE;
  usCommErrorCode = NULL;

  /*
  **  Initialize our all internal values.
  */
  sCommID   = -1;

  return (TRUE);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  void CommTerm();                                                          */
/*									      */
/*  Called during the application's wrapup routine so we can make sure that   */
/*  the communication port is closed and returned to the system.  Since the   */
/*  system is dieing we don't bother with a return code here.                 */
/*									      */
/*----------------------------------------------------------------------------*/
void CommTerm()
{
  if (sCommID >= 0)
    (void)CommClose();
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL CommOpen();                                                          */
/*									      */
/*  After filling in the Input Global Variables to their correct values, call */
/*  this routine to open the communiation port.  Returns true if successful.  */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL CommOpen()
{
  char szPort[8];


  if (sCommID >= 0)
    return (FALSE);

  /*
  **  Lets format the name of the device and open up some communications.
  */
  szPort[0] = 'C';
  szPort[1] = 'O';
  szPort[2] = 'M';
  szPort[3] = (char)'1' + sCommPort;
  szPort[4] = NULL;

  if ((sCommID = OpenComm(szPort, COMM_INPBUF, COMM_OUTBUF)) < 0)
    {
    usCommErrorCode = sCommID;
    return (FALSE);
    }

  /*
  **  Raise the DTR & RTS just in case we need it.
  */
  EscapeCommFunction(sCommID, SETRTS);
  EscapeCommFunction(sCommID, SETDTR);

  /*
  **  Change the serial port to the user specified parameters.
  */
  CommUpdate();

  fCommConnected = TRUE;

  ibInpLine      = 0;
  ibOutLine      = 0;
  sFilterEscape  = 0;
  iColLine       = 0;

  if (fDataPBX)
    nWorkCDLostCount = 1;
  else
    nWorkCDLostCount = 0;

  ulCommLastRecv = GetCurrentTime();
  ulCommLastSent = GetCurrentTime();

  return (TRUE);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL CommUpdate();                                                        */
/*									      */
/*  Used to update the communication port setting after we are connected.  We */
/*  can't change the communication port.  Returns true if successful.         */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL CommUpdate()
{
  DCB dcbComm;
  register i;


  if (sCommID < 0)
    {
    usCommErrorCode = 0;
    return (FALSE);
    }

  /*
  **  Load the current communication parameters.
  */
  if ((i = GetCommState(sCommID, (DCB FAR *)&dcbComm)) < 0)
    {
    usCommErrorCode = i;
    return (FALSE);
    }

  /*
  **  Update the new communication parameters.
  */
  dcbComm.BaudRate       = sCommSpeed;

  if (sCommParity == COMM_EVEN)
    {
    dcbComm.ByteSize = 7;
    dcbComm.Parity   = EVENPARITY;
    }
  else if (sCommParity == COMM_ODD)
    {
    dcbComm.ByteSize = 7;
    dcbComm.Parity   = ODDPARITY;
    }
  else
    {
    dcbComm.ByteSize = 8;
    dcbComm.Parity   = NOPARITY;
    }

//if (fCommHardware)
//  {
//  dcbComm.fOutxCtsFlow = TRUE;
//  dcbComm.fOutxDsrFlow = TRUE;
//  dcbComm.fRtsflow     = TRUE;
//  }
//else
//  {
//  dcbComm.fOutxCtsFlow = FALSE;
//  dcbComm.fOutxDsrFlow = FALSE;
//  dcbComm.fRtsflow     = FALSE;
//  }

  dcbComm.fBinary = TRUE;
  dcbComm.fParity = FALSE;

  /*
  **  Save the new communication parameters.
  */
  if ((i = SetCommState((DCB FAR *)&dcbComm)) < 0)
    {
    usCommErrorCode = i;
    return (FALSE);
    }

  /*
  **  While will are here, lets check the state of the carrier.
  */
  fCommCarrier = QueryCarrierDetect();

  return (TRUE);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL CommUpdateDtr(BOOL bDtr);					      */
/*									      */
/*  Used to update the communication port dtr setting from the user script.   */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL CommUpdateDtr(bDir)
  {
  if (sCommID < 0)
    {
    usCommErrorCode = 0;
    return (FALSE);
    }

  if (bDir)
    EscapeCommFunction(sCommID, SETDTR);
  else
    EscapeCommFunction(sCommID, CLRDTR);

  return (TRUE);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL CommClose();                                                         */
/*									      */
/*  Close the communication port.  Returns true if successful.  We will not   */
/*  return an error if the communication port is already is closed.           */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL CommClose()
{
  register i;


  if (sCommID < 0)
    {
    usCommErrorCode = 0;
    return (FALSE);
    }

  fCommConnected = FALSE;
  fCommCarrier	 = FALSE;

  EscapeCommFunction(sCommID, CLRRTS);
  EscapeCommFunction(sCommID, CLRDTR);

  if ((i = CloseComm(sCommID)) < 0)
    {
    sCommID = -1;
    usCommErrorCode = i;
    return (FALSE);
    }

  sCommID = -1;

  return (TRUE);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  USHORT CommReadData(pbBuffer, cbBuffer);				      */
/*    PBYTE  pbBuffer;	    pointer to Input Buffer			      */
/*    USHORT cbBuffer;	    length of Input Buffer			      */
/*									      */
/*  The CommReadData function reads data from the current communications port */
/*  and returns the number of bytes copied to pbBuffer. 		      */
/*									      */
/*----------------------------------------------------------------------------*/
USHORT CommReadData(PBYTE pbBuffer, USHORT cbBuffer)
  {
  COMSTAT  statComm;
  int      sBytes;
  register i;


  fCommCarrier	  = QueryCarrierDetect();
  usCommErrorCode = 0;

  if (sCommID < 0)
    return (0);

  if ((i = ReadComm(sCommID, pbBuffer, cbBuffer)) <= 0)
    {
    i = -i;

    usCommErrorCode = GetCommError(sCommID, (COMSTAT FAR *)&statComm);
    }

  if (i)
    {
    register j;

    if (sCommParity != COMM_NONE)
      {
      for (j = 0; j < i; j++)
        pbBuffer[j] &= 0x7F;
      }

    for (j = 0; j < i; j++)
      {
      if (sFilterEscape)
        {
        if (sFilterEscape == 1 && pbBuffer[j] == 'A')
          sFilterEscape = 2;
        else
          sFilterEscape = 0;
        continue;
        }
      else if ((isprint(pbBuffer[j]) && pbBuffer[j] != ';'))
        {
        if (ibInpLine < sizeof(abInpLine))
          abInpLine[ibInpLine++] = pbBuffer[j];

        iColLine++;

	if (iColLine < 80 && ibInpLine < 80)
          continue;
        }

      if ((bWorkPrintDoc && fSessProcess == TRUE) &&
          sAiState != STATE_LOGON)
        {
        if (ibInpLine)
          PrintWriteData(abInpLine, ibInpLine);
        if (pbBuffer[j] == ASC_LF)
          PrintWriteNewline();
        else if (pbBuffer[j] == ASC_FF)
          {
          PrintWriteNewline();
          FlushPrinter();
          }
        }
      if (bWorkStoreDoc && fSessProcess == TRUE)
        {
	if (ibInpLine && !fTermCancel)
          {
          if (_dos_write(hWorkDocument, abInpLine, ibInpLine, &sBytes))
            ReportError();
          else if (sBytes != ibInpLine)
            ReportError();
          }
	if (bWorkStoreDoc && pbBuffer[j] == ASC_LF && !fTermCancel)
          {
          if (_dos_write(hWorkDocument, "\r\n", 2, &sBytes))
            ReportError();
          else if (sBytes != 2)
            ReportError();
          }
	if (bWorkStoreDoc && pbBuffer[j] == ASC_FF && !fTermCancel)
          {
          if (_dos_write(hWorkDocument, "\r\n\014", 3, &sBytes))
            ReportError();
          else if (sBytes != 3)
            ReportError();
          }
        }
   // else if (bRecording && fSessProcess == FALSE)
   //   {
   //   if (ibInpLine)
   //     RecordWriteData(abInpLine, ibInpLine);
   //   if (pbBuffer[j] == ASC_LF || iColLine >= 80)
   //     RecordWriteNewline();
   //   }

      if (pbBuffer[j] == ASC_CR || iColLine >= 80)
	iColLine = 0;

      while (ibInpLine && abInpLine[ibInpLine-1] == ' ')
        abInpLine[--ibInpLine] = NULL;

      if (ibInpLine)
        {
        abInpLine[ibInpLine] = '\0';
        AIscanInpLine(abInpLine, ibInpLine);
        ibInpLine = 0;
        }
      if (pbBuffer[j] == ASC_ESC)
        sFilterEscape = 1;
      }

    ulCommLastRecv = GetCurrentTime();
    }
  else
    {
    if (fSessOnline)
      {
      if (GetCurrentTime() > ulCommLastSent + (ULONG)usWorkTimeout * 60000L)
        SessTellDisconnect(FALSE);
      }
    }

  return (i);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  USHORT CommWriteChar(bChar);                                              */
/*    BYTE  bChar;          pointer to Output Buffer                          */
/*									      */
/*  The CommWriteChar function write the specified byte to the communication  */
/*  port.  Returns true is the byte put on the output queue.                  */
/*									      */
/*----------------------------------------------------------------------------*/
USHORT CommWriteChar(bChar)
  BYTE bChar;
{
  BYTE bBuffer[1];

  bBuffer[0] = bChar;
  return (CommWriteData(bBuffer, 1));
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  USHORT CommWriteData(pbBuffer, cbBuffer);				      */
/*    PBYTE  pbBuffer;	    pointer to Output Buffer			      */
/*    USHORT cbBuffer;	    length of Output Buffer			      */
/*									      */
/*  The CommWriteData function write the data in the given buffer to the      */
/*  communication port.  Returns the number of bytes copied.		      */
/*									      */
/*----------------------------------------------------------------------------*/
USHORT CommWriteData(PBYTE pbBuffer, USHORT cbBuffer)
  {
  COMSTAT  statComm;
  register i;


  fCommCarrier	  = QueryCarrierDetect();
  usCommErrorCode = 0;

  if (sCommID < 0)
    return (0);

  if (cbBuffer == 0xFFFF)
    cbBuffer = strlen(pbBuffer);

  for (i = 0; i < cbBuffer; i++)
    {
    if (isprint(pbBuffer[i]) && pbBuffer[i] != ';')
      {
      if (ibOutLine < sizeof(abOutLine))
        abOutLine[ibOutLine++] = pbBuffer[i];
      }
    else
      {
      if (ibOutLine)
        {
        abOutLine[ibOutLine] = NULL;
        AIscanOutLine(abOutLine, ibOutLine);
        ibOutLine = 0;
        }
      }
    }

  if ((i = WriteComm(sCommID, pbBuffer, cbBuffer)) <= 0)
    {
    i = -i;
    usCommErrorCode = GetCommError(sCommID, (COMSTAT FAR *)&statComm);
    }

  ulCommLastSent = GetCurrentTime();

  return (i);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void CommEnableBreak(fEnable);					      */
/*    BOOL   fEnable;	    enable switch for break			      */
/*									      */
/*  The CommEnableBreak function is use to turn the communication break on or */
/*  off.  Normally the break is turn on by the caller and after a given unit  */
/*  time has passed, the caller turns the break back off.		      */
/*									      */
/*----------------------------------------------------------------------------*/
void CommEnableBreak(fEnable)
  BOOL	 fEnable;
{

  if (sCommID != -1)
    {
    if (fEnable)
      SetCommBreak(sCommID);
    else
      ClearCommBreak(sCommID);
    }
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL CommQueryCarrierDetect();					      */
/*									      */
/*  The CommQueryCarrierDetect function is use to check for the carrier       */
/*  detect (CD) state.	The output variable "fCommCarrier" is set to the      */
/*  current value of the carrier each time will do a call in this module.     */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL CommQueryCarrierDetect()
{

  fCommCarrier = QueryCarrierDetect();

  return (fCommCarrier);
}


/******************************************************************************/
/*									      */
/*  Local Procedures							      */
/*									      */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/*									      */
/*  static BOOL QueryCarrierDetect();					      */
/*									      */
/*  The QueryCarrierDetect function is use to check for the carrier detect    */
/*  (CD) state are various points in this module's logic.  Due to the windows */
/*  communication driver no allowing us to read the CD state, we will go to   */
/*  the hardware to peek at the modem status.  This is not a problem under    */
/*  OS2.								      */
/*									      */
/*----------------------------------------------------------------------------*/
static BOOL near QueryCarrierDetect()
{
  register i;


  if (sCommID < 0)
    return (FALSE);

  i = fCommCarrier;

  if (sCommID == 0)
    fCommCarrier = inp(0x03fe) & 0x80;
  else
    fCommCarrier = inp(0x02fe) & 0x80;

  if (hProcessDlg)
    return (fCommCarrier);

  if (fSessOnline && i && !fCommCarrier)
    {
    if (nWorkCDLostCount)
      nWorkCDLostCount--;
    else
      SessTellDisconnect(FALSE);
    }
  else if (fSessOnline && !i && fCommCarrier)
    SessTellCarrierHigh();

  return (fCommCarrier);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  void CmdBreak()                                                           */
/*									      */
/*  The CmdBreak function is to send a break.                                 */
/*									      */
/*----------------------------------------------------------------------------*/
void CmdBreak(SHORT sDuration)
  {
  LONG lTime;


  if (sDuration > 5000)
    sDuration = 5000;
  else if (sDuration < 50)
    sDuration = 250;

  if (sCommID != -1)
    {
    SetCommBreak(sCommID);

    lTime = GetCurrentTime() + (LONG)sDuration;
    while (GetCurrentTime() < lTime);

    ClearCommBreak(sCommID);
    }
  }
