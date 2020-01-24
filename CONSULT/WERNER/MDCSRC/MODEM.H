/*****************************************************************************
* FILE NAME:  modem.h                                                        *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Comm Routines for Blaise Asynch Manager                                  *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   no functions described                                                   *
*                                                                            *
*                                                                            *
* DATA STRUCTURES                                                            *
*                                                                            *
*   no structures defined                                                    *
*                                                                            *
*                                                                            *
* =========================================================================  *
*           M E A D   D A T A   C E N T R A L ,   I N C .                    *
*                          CONFIDENTIAL                                      *
*                                                                            *
* This document is property of Mead Data Central, Inc. (MDC) and its         *
* contents are proprietary to MDC.  Reproduction in any form by anyone       *
* of the materials combined herein without the permission of MDC is          *
* prohibited.  Finders are asked to return this document to Mead Data        *
* Central, Inc. at P.O. BOX 933, Dayton, OH 45401.                           *
*                                                                            *
*****************************************************************************/



/*
 *  $Log:   N:/src/modem.h_v  $
   
      Rev 8.0   18 Dec 1990 10:56:52   joeb
   TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
   
      Rev 7.0   06 Dec 1990 15:18:44   joeb
   revision 7.0 for iteration 5
   
      Rev 6.0   19 Nov 1990  9:23:22   joeb
   revision 6 for interation 4
   
      Rev 5.0   24 Oct 1990  9:07:06   andrews
   No change.
   
      Rev 4.0   01 Oct 1990 13:24:56   andrews
   ITERATION NUMBER 2
   
      Rev 3.0   17 Sep 1990 10:07:18   andrews
   Full Iteration 1
   
      Rev 2.0   27 Aug 1990  9:21:28   andrews
   iteration 1
   
      Rev 0.0   25 Jul 1990 14:59:22   deanm
   Initial revision.
 *
*/



#ifndef MODEM_H
#define MODEM_H


/* define parity for Asynch Manager */

#define MODEM_NOPAR              0
#define MODEM_ODDPAR             1
#define MODEM_EVENPAR            2


/* define data bits for Asynch Manager */

#define MODEM_8_BITS             3
#define MODEM_7_BITS             2


/* define stop bits for Asynch Manager */

#define MODEM_STOP_BITS          0


/* define baud rates for Asynch Manager */

#define MODEM_300_BAUD           2
#define MODEM_1200_BAUD          4
#define MODEM_2400_BAUD          5
#define MODEM_4800_BAUD          6
#define MODEM_9600_BAUD          7


/* define comm ports for Asynch Manager */

#define MODEM_COM1               1
#define MODEM_COM2               2
#define MODEM_COM3               3
#define MODEM_COM4               4


/* define default IRQ and port address for Asynch Manager */

#define MODEM_DEFAULT_INT        0
#define MODEM_DEFAULT_PORT_ADS   0


/* other defines */

#define MODEM_REMOTE_FLOW        5
#define MODEM_LOCAL_FLOW         6
#define MODEM_CTS_REQUIRE        9
#define MODEM_BIT7_FORCE         8
#define MODEM_BIT7_TRIM          7


/* define CD check mask for Asynch Manager */

#define MODEM_CD_MASK      0x0080



/* define function prototypes */

extern INT     ModemInit      (COMMDATA *);
extern INT     ModemOpen      (COMMDATA *);
extern INT     ModemClose     (COMMDATA *);
extern INT     ModemReadCh    (COMMDATA *, CHAR *);
extern INT     ModemReadStr   (COMMDATA *, CHAR *, INT);
extern INT     ModemWriteCh   (COMMDATA *, CHAR);
extern INT     ModemWriteStr  (COMMDATA *, CHAR *);
extern INT     ModemBreak     (COMMDATA *);
extern INT     ModemFlush     (COMMDATA *);
extern INT     ModemLineCheck (COMMDATA *);
extern BOOLEAN ModemCharReady (COMMDATA *);

#endif
