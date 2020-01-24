/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*  Include: Communication Include (COMM.H)                                   */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/*  Remarks: This is were all the communciation with the real world is done.  */
/*           Only here are the functions defined by the Windows communication */
/*           driver used.  Also the data stream to and from the serial port   */
/*           is filtered by AI (more or less) routines to keep track on the   */
/*           users action.                                                    */
/*									      */
/******************************************************************************/


/******************************************************************************/
/*									      */
/*  Global Definitions							      */
/*									      */
/******************************************************************************/

#define COMM_EVEN   0		    /* Even Parity value for sCommParity    */
#define COMM_ODD    1		    /* Odd Parity value for sCommParity     */
#define COMM_NONE   2		    /* No Parity value for sCommParity	    */


/******************************************************************************/
/*									      */
/*  Input Global Variables                                                    */
/*									      */
/******************************************************************************/

  extern SHORT	sCommPort;	    /* Communication port		    */
  extern SHORT	sCommSpeed;	    /* Speed (or baud rate) of serial port  */
  extern SHORT	sCommParity;	    /* Parity, values defined above	    */
  extern BOOL	fCommHardware;	    /* Use hardware handshaking, CTS/RTS    */
  extern BOOL	fCommLocal;	    /* Local connection, hardwired	    */


/******************************************************************************/
/*									      */
/*  Output Global Variables                                                   */
/*									      */
/******************************************************************************/

  extern SHORT  sCommID;            /* Handle from communication driver     */
  extern BOOL   fCommConnected;     /* Connected to the communication port  */
  extern BOOL   fCommCarrier;       /* Carrier has been detected            */
  extern USHORT usCommErrorCode;    /* Error code from device on bad return */
  extern ULONG  ulCommLastRecv;     /* Time data was last received          */
  extern ULONG  ulCommLastSent;     /* Time data was last transmitted       */


/******************************************************************************/
/*									      */
/*  External Procedure's Prototypes                                           */
/*									      */
/******************************************************************************/

  extern BOOL   CommInit();
  extern void   CommTerm();
  extern BOOL   CommOpen();
  extern BOOL   CommUpdate();
  extern BOOL	CommUpdateDtr(BOOL bDtr);
  extern BOOL   CommClose();
  extern USHORT CommReadData(PBYTE, USHORT);
  extern USHORT CommWriteChar(BYTE);
  extern USHORT CommWriteData(PBYTE, USHORT);
  extern void	CommEnableBreak(BOOL);
  extern BOOL	CommQueryCarrierDetect();


  extern void   CmdBreak(SHORT sDuration);
