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
 *  $Log:   N:/src/netbios.h_v  $
   
      Rev 8.0   18 Dec 1990 10:56:56   joeb
   TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
   
      Rev 7.0   06 Dec 1990 15:18:48   joeb
   revision 7.0 for iteration 5
   
      Rev 6.0   19 Nov 1990  9:23:24   joeb
   revision 6 for interation 4
   
      Rev 5.0   24 Oct 1990  9:07:10   andrews
   No change.
   
      Rev 4.0   01 Oct 1990 13:25:00   andrews
   ITERATION NUMBER 2
   
      Rev 3.0   17 Sep 1990 10:07:22   andrews
   Full Iteration 1
   
      Rev 2.0   27 Aug 1990  9:21:32   andrews
   iteration 1
   
      Rev 0.2   04 Aug 1990 12:08:40   deanm
   making devcomm a stronger lib
   
      Rev 0.1   30 Jul 1990  9:13:30   deanm
   finishing up comm device drivers and comm scripts
   
      Rev 0.0   25 Jul 1990 14:59:38   deanm
   Initial revision.
   
 **************************************************************************/

#ifndef NETBIOS_H
#define NETBIOS_H

/* ncb.h  --  network control block definitions for NETBIOS network */

#define NETBIOS         0x5C

#define CONNECT_REQ        0 
#define CONNECT_ID_RESP    1
#define CONNECT_RESP       2
#define CONNECT_PARM       3
#define DATA               4
#define CONNECT_ID         5

/* NCB commands */
#define NCB_CALL        0x10
#define NCB_HANG_UP     0x12
#define NCB_SEND        0x14
#define NCB_SEND_DG     0x20
#define NCB_ADD_NAME    0x30
#define NCB_DEL_NAME    0x31
#define NCB_RESET       0x32
#define NCB_STATUS      0x33
#define NCB_SESS_STAT   0x34
#define NCB_LISTEN      0x91
#define NCB_RECEIVE     0x95
#define NCB_RCV_DG      0xA1

/* postflag indexes */
#define LISTEN_POST        0
#define RCV_DG_POST        1
#define RECEIVE_POST       2
#define SEND_POST          3

/* NETBIOS return codes (some are translated into mdc errors in nb_error) */
#define NCB_OK          0x00
#define BAD_BUFF_LEN    0x01
#define BAD_COMMAND     0x03
#define TIMED_OUT       0x05
#define BROKEN_PACKET   0x06
#define BAD_LSN         0x08
#define NO_RESOURCE     0x09
#define SESS_CLOSED     0x0A
#define COMMAND_CANCEL  0x0B
#define DUPLICATE_NAME  0x0D
#define NAME_TABLE_FULL 0x0E
#define NAME_DEREG      0x0F
#define SESS_TABLE_FULL 0x11
#define BAD_OPEN        0x12
#define BAD_NAME_NUM    0x13
#define NO_ANSWER       0x14
#define BAD_NAME        0x15
#define NAME_IN_USE     0x16
#define NAME_DELETED    0x17
#define NB_ABEND        0x18
#define NAME_CONFLICT   0x19
#define BAD_PROTOCOL    0x1A
#define NB_BUSY         0x21
#define NB_BACKED_UP    0x22
#define WRONG_ADAPTER   0x23
#define BAD_CANCEL      0x24
#define CANNOT_CANCEL   0x26
#define BUSTED_ADAPTER  0x40
#define HOT_CD_FOUND    0x41
#define HOT_CD_SENT     0x42
#define NO_CARRIER      0x43
#define UNUSUAL_NETWORK 0x44
#define BAD_STAT_BIT    0x4E
#define EXTRA_STAT_BIT  0x4F
#define NETBIOS_ABSENT  0xFB
#define NCB_WAIT        0xFF

/* miscellaneous */
#define PRIM_ADAPT         0
#define ALT_ADAPT          1
#define RCV_BUFF_LEN     100
#define STAT_BUFF_LEN    256
#define NO_TIMEOUT         0
#define MAX_SESSION       10
#define MAX_NCB           30
#define NCB_NAME_LEN      16

typedef struct 
   {
   BYTE ncb_command;
   BYTE ncb_retcode;
   BYTE ncb_lsn;
   BYTE ncb_num;
   BYTE _far *ncb_buffer;
   INT  ncb_length;
   BYTE ncb_callname [NCB_NAME_LEN];
   BYTE ncb_name [NCB_NAME_LEN];
   BYTE ncb_rto;
   BYTE ncb_sto;
   void (far *ncb_post)();
   BYTE ncb_lana_num;
   BYTE ncb_cmd_cplt;
   BYTE ncb_reserve[14];
   }  TYPE_NCB;

typedef struct 
   {
   INT           index;
   TYPE_NCB _far *ncbs[4];
   } TYPE_POST;

typedef struct
   {
   TYPE_POST   postflag;                     /* offset    0 */
   TYPE_NCB    ncb_receive;                  /* offset   18 */
   CHAR        rcv_buff[STAT_BUFF_LEN];      /* offset   84 */
   CHAR        another_buff[STAT_BUFF_LEN];  /* offset  340 */
   BYTE        broken_packet;                /* offset  594 */
   BYTE        lsnum;                        /* offset  595 */
   INT         oldss;                        /* offset  596 */
   INT         oldsp;                        /* offset  598 */
   INT         oldbp;                        /* offset  600 */
   } TYPE_NB_ISR_GLOB;

extern TYPE_NB_ISR_GLOB isr_glob;
extern TYPE_NCB    ncb_send;
extern TYPE_NCB    ncb_receive_dg;
extern TYPE_NCB    ncb_listen; 
extern TYPE_NCB    ncb_send_dg;
extern TYPE_NCB    general_ncb_rec;
extern CHAR        rcv_dg_buff[STAT_BUFF_LEN];
extern CHAR        status_buff[STAT_BUFF_LEN];
extern CHAR        ncbname[NCB_NAME_LEN + 1];
extern INT         net_num;
extern INT         connected;



extern INT      NetbiosInit        (COMMDATA *);
extern INT      NetbiosOpen        (COMMDATA *);
extern INT      NetbiosClose       (COMMDATA *);
extern INT      NetbiosReadCh      (COMMDATA *, CHAR *);
extern INT      NetbiosReadStr     (COMMDATA *, CHAR *, INT);
extern INT      NetbiosWriteCh     (COMMDATA *, CHAR);
extern INT      NetbiosWriteStr    (COMMDATA *, CHAR *);
extern INT      NetbiosFlush       (COMMDATA *);
extern INT      NetbiosLineCheck   (COMMDATA *);
extern BOOLEAN  NetbiosCharReady   (COMMDATA *);



extern TYPE_NCB _far *nb_isr    (TYPE_NCB _far *, INT);
extern void _far post      (void);
extern void _far post_rcv  (void);
extern void printncb       (TYPE_NCB *);
extern INT  reset          (TYPE_NCB *);
extern INT  add_name       (TYPE_NCB *);
extern INT  listen         (void);
extern INT  rcv_data_gram  (void);
extern INT  send_data_gram (CHAR *, CHAR *);
extern INT  send           (CHAR *);
extern INT  nb_send        (CHAR *);
extern INT  nb_sendch      (CHAR *);
extern INT  nb_receive     (void);
extern INT  delete_name    (void);
extern INT  hang_up        (void);
extern INT  call           (CHAR *);
extern INT  nb_status      (TYPE_NCB *);
extern void command_vector (CHAR *, INT, COMMDATA *);
extern INT  post_handler   (COMMDATA *, INT *, CHAR *, BOOLEAN *);
extern void init_ncbname   (CHAR *);
extern void get_ncbname    (CHAR *);
extern BOOLEAN nb_rcv_ready(void);
extern INT  nb_rcv_str     (TYPE_NCB _far *, INT);
extern INT  nb_error       (INT);
extern INT  nb_netbios     (TYPE_NCB *);
extern INT  sess_stat      (void);

#endif

