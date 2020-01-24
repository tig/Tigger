/****************************************************************************
 * FILE NAME:  deverror.h                                                   *
 *                                                                          *
 * DESCRIPTIVE TITLE:                                                       *
 *                                                                          *
 *   header file for exception handling of device communications            *
 *                                                                          *
 * DESCRIPTION:                                                             *
 *                                                                          *
 *                                                                          *
 * DATA STRUCTURES:                                                         *
 *                                                                          *
 *                                                                          *
 * AUTHOR:  Dean Myers                                                      *
 *                                                                          *
 * DATE:   7/9/1990                                                         *
 *                                                                          *
 * ============================================================             *
 *        M E A D  D A T A  C E N T R A L,  I N C.                          *
 *                    CONFIDENTIAL                                          *
 *                                                                          *
 * This document is property of Mead Data Central, Inc. (MDC)               *
 * and its contents are proprietary to MDC.  Reproduction in                *
 * any form by anyone of the materials combined herein without              *
 * the permission of MDC is prohibited.  Finders are asked to               *
 * return this document to Mead Data Central, Inc. at 3445                  *
 * Newmark Dr., Miamisburg, OH 45342.                                       *
 *                                                                          *
 ***************************************************************************/

/***************************************************************************
 * $Log:   N:/src/deverror.h_v  $
   
      Rev 8.0   18 Dec 1990 10:58:38   joeb
   TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
   
      Rev 7.0   06 Dec 1990 15:20:24   joeb
   revision 7.0 for iteration 5
   
      Rev 6.0   19 Nov 1990  9:25:14   joeb
   revision 6 for interation 4
   
      Rev 5.0   24 Oct 1990  9:09:12   andrews
   No change.
   
      Rev 4.0   01 Oct 1990 13:27:12   andrews
   ITERATION NUMBER 2
   
      Rev 3.0   17 Sep 1990 10:09:28   andrews
   Full Iteration 1
   
      Rev 2.0   27 Aug 1990  9:23:06   andrews
   iteration 1
   
      Rev 0.2   21 Aug 1990 14:09:54   deanm
   cleaning up modem exceptions
   
      Rev 0.1   03 Aug 1990 16:59:14   andrews
   put #ifndef around deverror.h
   
      Rev 0.0   02 Aug 1990 15:00:44   deanm
   Initial revision.

 *
 **************************************************************************/   



#ifndef DEVERROR_H
#define DEVERROR_H


#define FIRST_COMM_ERROR        42
#define     DEVICE_OUT_OF_RANGE 42
#define     MODEM_FAIL_ERROR    43

/* Novell error codes */

#define     NO_NCSI             44      /*  NCSI not loaded (arb. num.) */
#define     NO_NAME             45      /*  name not found  (arb. num.) */
#define     NO_INIT             46      /*  could not init. (arb. num.) */
#define     CD_DOWN             47      /*  accidental CD drop          */
#define     NO_MODEMS           48      /*  no modems in lan modem pool */
#define     NOVELL_ERR          49      /*  a general novell error      */
#define     NO_COMM_SERVER      50      /*  Novell comm server not resp.*/

/* J&L error codes */

#define     JL_INIT_ERR         51      /* could not init 1C ISR        */
#define     JL_WRITE_ERR        52      /* could not write to port      */

/* NETBIOS error codes */

#define     NB_CONN_ERR         53
#define     NB_SERVER_ERR       54
#define     NB_LINE_BUSY        55
#define     NB_TERMINATED       56
#define     NB_NAME_IN_USE      57
#define     NB_NO_ANSWER        58
#define     NB_UNUSUAL_COND     59
#define     NB_ADAPTER_MALFUNC  60
#define     NB_NOT_LOADED       61
#define LAST_COMM_ERROR         61


#endif
