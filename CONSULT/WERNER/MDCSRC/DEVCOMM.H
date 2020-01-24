/*****************************************************************************
* FILE NAME:  devcomm.h                                                      *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Comm Routines for Modem, Novell ACS2, NETBIOS ACS2, and INT 14 Devices   *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Dev_Init      - Initialize comm device                                   *
*   Dev_Open      - Open comm device                                         *
*   Dev_Close     - Close comm device                                        *
*   Dev_ReadCh    - Read a character from the comm device                    *
*   Dev_ReadStr   - read a string from the comm device                       *
*   Dev_WriteCh   - Write a character to the comm device                     *
*   Dev_WriteStr  - Write a string to the comm device                        *
*   Dev_Break     - Send a break signal                                      *
*   Dev_Flush     - Flush the input and ouput of the comm device             *
*   Dev_LineCheck - Check the phone line to see if still active              *
*   Dev_CharReady - Check to see if a character is ready to be read          *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
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
 *  $Log:   N:/src/devcomm.h_v  $
   
      Rev 8.0   18 Dec 1990 10:52:32   joeb
   TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
   
      Rev 7.0   06 Dec 1990 15:14:20   joeb
   revision 7.0 for iteration 5
   
      Rev 6.1   21 Nov 1990 14:16:52   joeb
   touch up where phone_flag is declared for the comm lib
   
      Rev 6.0   19 Nov 1990  9:18:46   joeb
   revision 6 for interation 4
   
      Rev 5.0   24 Oct 1990  9:02:42   andrews
   No change.
   
      Rev 4.0   01 Oct 1990 13:20:20   andrews
   ITERATION NUMBER 2
   
      Rev 3.0   17 Sep 1990 10:02:16   andrews
   Full Iteration 1
   
      Rev 2.0   27 Aug 1990  9:15:58   andrews
   iteration 1
   
      Rev 0.3   25 Aug 1990 18:44:38   deanm
   cleaning up novell and int14
   
      Rev 0.2   04 Aug 1990 12:07:32   deanm
   making devcomm a stronger lib
   
      Rev 0.1   25 Jul 1990 14:57:42   deanm
   general update
 *
*/


#ifndef DEVCOMM_H
#define DEVCOMM_H


/*  define the devices that are used (or will be used) in this library */

#define MAXDEVICES      8

#define DEV_PAD         0
#define DEV_MODEM       1
#define DEV_NULL        2
#define DEV_NOVELL      3
#define DEV_NETBIOS     4
#define DEV_INT14TICK   5
#define DEV_PCSA        6   /* currently not used in this library */
#define DEV_INT14POLL   7





/* define the port numbers */

#define DEV_COM1   0
#define DEV_COM2   1
#define DEV_COM3   2
#define DEV_COM4   3





/* define parity */

#define NOPARITY     0
#define ODDPARITY    1
#define EVENPARITY   2
#define MARKPARITY   3
#define SPACEPARITY  4





extern INT     Dev_Init       (INT, INT, INT, INT, INT, CHAR *, CHAR *, 
                               CHAR *, CHAR *);
extern INT     Dev_Open       (void);
extern INT     Dev_Close      (void);
extern INT     Dev_ReadCh     (CHAR *);
extern INT     Dev_ReadStr    (CHAR *, INT);
extern INT     Dev_WriteCh    (CHAR);
extern INT     Dev_WriteStr   (CHAR *);
extern INT     Dev_Break      (void);
extern INT     Dev_Flush      (void);
extern INT     Dev_LineCheck  (void);
extern BOOLEAN Dev_CharReady  (void);

extern BOOLEAN  Phone_Flag;
#endif
