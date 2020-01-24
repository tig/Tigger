/*****************************************************************************
* FILE NAME:  int14.h                                                        *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Comm Routines for int14 devices                                          *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14TickInit    - Initialize    (timer tick only)                       *
*   INT14PollInit    - Initialize                                            *
*   INT14Open        - Open                                                  *
*   INT14TickClose   - Close         (timer tick only)                       *
*   INT14PollClose   - Close                                                 *
*   INT14TickReadCh  - Read a character      (timer tick only)               *
*   INT14PollReadCh  - Read a character                                      *
*   INT14TickReadStr - read a string         (timer tick only)               *
*   INT14PollReadStr - read a string                                         *
*   INT14WriteCh     - Write a character                                     *
*   INT14WriteStr    - Write a string                                        *
*   INT14TickFlush   - Flush the input and ouput buffers   (timer tick only) *
*   INT14PollFlush   - Flush the input and ouput buffers                     *
*   INT14LineCheck   - Check the phone line to see if still active           *
*   INT14TickCharReady - Check for a character is ready to be read (t t only)*
*   INT14PollCharReady - Check for a character is ready to be read           *
*                                                                            *
*                                                                            *
*                                                                            *
* PRIVATE FUNCTIONS:                                                         *
*                                                                            *
*   INT14Tick        - timer tick routine                                    *
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
 *  $Log:   N:/src/int14.h_v  $
   
      Rev 8.0   18 Dec 1990 10:56:46   joeb
   TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
   
      Rev 7.0   06 Dec 1990 15:18:36   joeb
   revision 7.0 for iteration 5
   
      Rev 6.0   19 Nov 1990  9:23:16   joeb
   revision 6 for interation 4
   
      Rev 5.0   24 Oct 1990  9:06:58   andrews
   No change.
   
      Rev 4.0   01 Oct 1990 13:24:48   andrews
   ITERATION NUMBER 2
   
      Rev 3.0   17 Sep 1990 10:07:10   andrews
   Full Iteration 1
   
      Rev 2.0   27 Aug 1990  9:21:20   andrews
   iteration 1
   
      Rev 0.0   25 Jul 1990 14:57:16   deanm
   Initial revision.
 *
*/   
   

#ifndef INT14_H
#define INT14_H



extern INT      INT14TickInit      (COMMDATA *);
extern INT      INT14PollInit      (COMMDATA *);
extern INT      INT14Open          (COMMDATA *);
extern INT      INT14TickClose     (COMMDATA *);
extern INT      INT14PollClose     (COMMDATA *);
extern INT      INT14TickReadCh    (COMMDATA *, CHAR *);
extern INT      INT14PollReadCh    (COMMDATA *, CHAR *);
extern INT      INT14TickReadStr   (COMMDATA *, CHAR *, INT);
extern INT      INT14PollReadStr   (COMMDATA *, CHAR *, INT);
extern INT      INT14WriteCh       (COMMDATA *, CHAR);
extern INT      INT14WriteStr      (COMMDATA *, CHAR *);
extern INT      INT14TickFlush     (COMMDATA *);
extern INT      INT14PollFlush     (COMMDATA *);
extern INT      INT14LineCheck     (COMMDATA *);
extern BOOLEAN  INT14TickCharReady (COMMDATA *);
extern BOOLEAN  INT14PollCharReady (COMMDATA *);


#endif
