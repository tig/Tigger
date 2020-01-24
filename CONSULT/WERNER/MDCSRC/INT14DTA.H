/*****************************************************************************
* FILE NAME:  INT14DTA.H                                                     *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Private header file for int14 devices                                    *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14Tick        - Timer tick routine                                    *
*   INITPT14         - Initialize and open the INT14 port                    *
*   SETFLOW14        - Flow control                                          *
*   SNDCHR14         - Send a character                                      *
*   RCVCHR14RDY      - Character ready check                                 *
*   RCVCHR14         - Receive a character                                   *
*   CHAIN            - Chain to the previous timer tick routine              *
*   LINKUP           - Form a link to a previous timer tick routine          *
*   DELINK           - Break the link to a previous timer tick routine       *
*                                                                            *
*                                                                            *
* DATA STRUCTURES:  none                                                     *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/09/90                                                            *
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



/***************************************************************************
 *                                                                         *
 *  $Log:   N:/src/int14dta.h_v  $
   
      Rev 8.0   18 Dec 1990 10:56:48   joeb
   TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
   
      Rev 7.0   06 Dec 1990 15:18:40   joeb
   revision 7.0 for iteration 5
   
      Rev 6.0   19 Nov 1990  9:23:18   joeb
   revision 6 for interation 4
   
      Rev 5.0   24 Oct 1990  9:07:02   andrews
   No change.
   
      Rev 4.0   01 Oct 1990 13:24:52   andrews
   ITERATION NUMBER 2
   
      Rev 3.0   17 Sep 1990 10:07:14   andrews
   Full Iteration 1
   
      Rev 2.0   27 Aug 1990  9:21:24   andrews
   iteration 1
   
      Rev 0.1   26 Aug 1990 21:26:00   deanm
   fixing up chaining for int14
   
      Rev 0.0   25 Jul 1990 14:57:20   deanm
   Initial revision.
 *
*/   
   

#ifndef INT14DTA_H
#define INT14DTA_H


#define INT14_STACKSIZ      1024
#define INT14_FLOW_CTRL     0x13
#define INT14_DISCONNECT       0
#define INT14_TIMEOUT       0xFF


#define INT14_NOPAR            0
#define INT14_ODDPAR           1
#define INT14_EVENPAR          3



#define INT14_STOP_BITS        0


void  CHAIN  (DWORD *);
void  LINKUP (DWORD *);
void  DELINK (DWORD *);


extern void cdecl INT14Tick   (ALLREG *, ISRCTRL *, ISRMSG *);
extern INT        INITPT14    (INT, INT);
extern INT        SETFLOW14   (INT, INT);
extern INT        SNDCHR14    (INT, CHAR, INT);
extern INT        RCVCHR14RDY (INT);
extern INT        RCVCHR14    (INT, INT *, INT *);
extern INT        CHKSTAT14   (INT, INT *, INT *);

#endif
