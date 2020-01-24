/***************************************************************************
 *             M E A D   D A T A   C E N T R A L,   I N C.                 *
 *                             CONFIDENTIAL                                *
 *                                                                         *
 *  This document is the property of Mead Data Central, Inc. (MDC)         *
 *  and its contents are proprietary to MDC.  Reproduction in any form     *
 *  by anyone of the materials combined herein without the permission      *
 *  of MDC is prohibited.  Finders are asked to return this document to    *
 *  Mead Data Central, Inc. at 3445 Newmark Dr.,  Miamisburg, OH  45342    *
 **************************************************************************/

/*
 * $Log:   N:/src/ols.h_v  $
   
      Rev 8.0   18 Dec 1990 11:00:36   joeb
   TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
   
      Rev 7.0   06 Dec 1990 15:22:04   joeb
   revision 7.0 for iteration 5
   
      Rev 6.0   19 Nov 1990  9:26:56   joeb
   revision 6 for interation 4
   
      Rev 5.0   24 Oct 1990  9:11:04   andrews
   No change.
   
      Rev 4.1   03 Oct 1990 15:00:54   deanm
   adding retry and cancel options to the two disk fulls
   
      Rev 4.0   01 Oct 1990 13:28:48   andrews
   ITERATION NUMBER 2
   
      Rev 3.0   17 Sep 1990 10:10:52   andrews
   Full Iteration 1
   
      Rev 2.0   27 Aug 1990  9:24:32   andrews
   iteration 1
   
      Rev 0.3   23 Aug 1990 22:00:40   deanm
   fixing up document download and status bar
   
      Rev 0.2   22 Aug 1990  9:02:26   deanm
   fixing up session recording
   
      Rev 0.1   14 Aug 1990 17:07:24   deanm
   fixing up online
   
      Rev 0.0   13 Aug 1990 10:07:26   deanm
   Initial revision.
 *
 */

#ifndef OLS_H
#define OLS_H



extern INT OLS_EditSendRcv (TYPE_TS *, OLDATA *);
extern INT OLS_Edit        (TYPE_TS *, OLDATA *, CHAR *, TRANS_TABLE_ENTRY **);
extern INT OLS_Send        (TYPE_TS *, TRANS_TABLE_ENTRY *);
extern INT OLS_Receive     (TYPE_TS *, OLDATA *);
extern INT OLS_Write       (TYPE_TS *, CHAR *, OLDATA *);
extern INT OLS_SendStr     (TYPE_TS *, CHAR *, CHAR *);
extern INT OLS_Record      (TYPE_TS *, OLDATA *);
extern INT OLS_UpdateBar   (TYPE_TS *, CHAR *, BOOLEAN);
extern INT OLS_DocDiskOpen (TYPE_TS *, OLDATA *);

#endif
