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

/***************************************************************************
 *                                                                         *
 *                       Modification Log                                  *
 *  $Log:   N:/src/online.h_v  $
   
      Rev 8.0   18 Dec 1990 11:00:52   joeb
   TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
   
      Rev 7.0   06 Dec 1990 15:22:16   joeb
   revision 7.0 for iteration 5
   
      Rev 6.0   19 Nov 1990  9:27:10   joeb
   revision 6 for interation 4
   
      Rev 5.0   24 Oct 1990  9:11:16   andrews
   No change.
   
      Rev 4.0   01 Oct 1990 13:29:00   andrews
   ITERATION NUMBER 2
   
      Rev 3.1   21 Sep 1990 17:35:26   deanm
   fixing stop, kwic, sign off, focus keys
   
      Rev 3.0   17 Sep 1990 10:11:00   andrews
   Full Iteration 1
   
      Rev 2.1   11 Sep 1990 20:23:42   deanm
   added tobasco
   
      Rev 2.0   27 Aug 1990  9:24:40   andrews
   iteration 1
   
      Rev 0.2   22 Aug 1990 11:58:26   joeb
   ADD SCREEN HIDING
   
      Rev 0.1   14 Aug 1990 17:07:32   deanm
   fixing up online
   
      Rev 0.0   13 Aug 1990 10:07:32   deanm
   Initial revision.
   
 **************************************************************************/

#ifndef ONLINE_H
#define ONLINE_H

#include "systool.h"

/* tokens received from online system */
#define UNAVAILABLE 0
#define LINEFEED    1
#define FORMFEED    2
#define RET         3
#define UNKNOWN     4 
#define CLEAR       5
#define LOCK        6
#define UNLOCK      7
#define PRINT       8
#define DISCONNECT  9  
#define HOME        10
#define REV_OFF     11
#define REV_ON      12
#define UND_ON      13
#define UND_OFF     14
#define STRING      15
#define SECT_SYM_TOK    16
#define ECHO_ON     17
#define ECHO_OFF    18

#define ATTR 'A'
#define CH_CLEAR  'C'
#define CH_LOCK   'L'
#define CH_UNLOCK 'U'
#define CH_PRINT  'P'
#define CH_DISCONNECT 'D'
#define CH_HOME   'H'
#define CH_REV_OFF '0'
#define CH_REV_ON  '1'
#define CH_UND_ON  '2'
#define CH_UND_OFF '3'
#define CH_ECHO_ON '4'
#define CH_ECHO_OFF '5'


#define STOP_SEQ "\033\075"
#define ACK_SEQ  "\033\053"
#define NAK_SEQ  "\033\055"
#define STX      '\02'
#define NULBYTE  '\0'
#define ESC      '\033'

/* status window dimensions */
#define STAT_ROW      24
#define STAT_COL      0
#define STAT_NUMROWS  1
#define STAT_NUMCOLS  80

#define FIRSTROW 0
#define FIRSTCOL 0
#define LASTROW  23
#define LASTCOL  79


typedef struct {
      BOOLEAN online;        
      BOOLEAN id_flag;  
      BOOLEAN kbd_locked;              /* keyboard status           */
      BOOLEAN disp_data;               /* display or print data     */
      BOOLEAN first_block;
      BOOLEAN stop_sent;
      BOOLEAN rec_on;
      BOOLEAN doc_disk_on;
      BOOLEAN prt_on;
      BOOLEAN client;
      BOOLEAN echo;
      INT     cur_row;                 /* cursor row                */
      INT     cur_col;                 /* cursor column             */
      INT     prnt_row;                /* printer row               */
      INT     prnt_col;                /* printer col               */
      INT     error_count;
      INT     status; 
      WINDOW  stat_win;
      FILE    *prnt_file;              /* temporary print file      */
      FILE    *rec_file;
      FILE    *doc_file;
      CHAR    *scrnBuffPtr;
   } TYPE_TS;



typedef struct {
   INT key;          /* KEY CODE/SCAN CODE */
   CHAR *send;       /* what to send */
   CHAR *display;    /* what to display in status bar */
   BOOLEAN HTC;      /* send home to cursor ? */
   } TRANS_TABLE_ENTRY;



#define TT_SIZE 25
extern TRANS_TABLE_ENTRY TTable[];  /* will be defined of size TT_SIZE */

#define TOBASCO_TT_SIZE 41
extern TRANS_TABLE_ENTRY TobascoTable[];  // defined of size TOBASCO_TT_SIZE


extern INT  Online (CHAR *, OLDATA *,BOOLEAN);
extern INT  GetToClientScreen (TYPE_TS *, OLDATA *);


#endif
