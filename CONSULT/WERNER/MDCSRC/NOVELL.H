/*****************************************************************************
* FILE NAME:  novell.h                                                       *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Comm Routines for Novell                                                 *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
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

/***************************************************************************
 *                                                                         *
 *                       Modification Log                                  *
 *  $Log:   N:/src/novell.h_v  $
   
      Rev 8.0   18 Dec 1990 10:56:38   joeb
   TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
   
      Rev 7.0   06 Dec 1990 15:18:28   joeb
   revision 7.0 for iteration 5
   
      Rev 6.0   19 Nov 1990  9:23:08   joeb
   revision 6 for interation 4
   
      Rev 5.0   24 Oct 1990  9:06:48   andrews
   No change.
   
      Rev 4.0   01 Oct 1990 13:24:38   andrews
   ITERATION NUMBER 2
   
      Rev 3.0   17 Sep 1990 10:07:02   andrews
   Full Iteration 1
   
      Rev 2.0   27 Aug 1990  9:21:02   andrews
   iteration 1
   
      Rev 0.0   25 Jul 1990 14:57:10   deanm
   Initial revision.
 *
 **************************************************************************/

#ifndef NOVELL_H
#define NOVELL_H


/*  vc.h  --  virtual circuit header file for NCSI network */


#define NCSI            0x6B            /*  interrupt vector number     */


/*  NCSI and virtual circuit interrupt functions */

#define VC_ALLOC        0x11            /*  allocate a virtual circuit  */
#define VC_STATUS       0x12            /*  check virtual circuit status*/
#define VC_SERV_NM      0x13            /*  set/retrieve service name   */
#define VC_CONFIG       0x15            /*  set or get vc configuration */
#define VC_INIT         0x16            /*  initialize a virtual circuit*/
#define VC_DISCON       0x17            /*  disconnect a virtual circuit*/
#define VC_WRITE        0x18            /*  write to a virtual circuit  */
#define VC_READ         0x19            /*  read from a virtual circuit */
#define VC_RECV_STAT    0x1A            /*  vc's receive buffer status  */
#define VC_XMIT_STAT    0x1B            /*  vc's transmit buffer status */
#define VC_RECV_CLR     0x1C            /*  resets and clears server's
                                            receive buffers             */
#define VC_XMIT_CLR     0x1D            /*  resets and clears server's
                                            transmit buffers            */
#define EXTERN_STAT     0x1F            /*  server external line status */
#define VC_SET_RECV     0x20            /*  relocate receive buffers    */
#define VC_QUERY        0x21            /*  name translation and group
                                            name identification service */

/*  virtual circuit states */

#define IDLE            0x00            /*  virtual circuit is idle     */
#define ACTIVE          0x01            /*  virtual circuit is active   */
#define VCONNECT        0x02            /*  virtual circuit is connected*/
#define HOLD            0x03            /*  virtual circuit is holding  */
#define BUSY            0x04            /*  virtual circuit is busy     */
#define SUPER           0x05            /*  vc is in supervisory process*/
#define SHUTDOWN        0x06            /*  virtual circuit is busted   */

/*  NCSI error codes */

#define NCSI_BUSY       0xFF            /*  NCSI is busy                */
#define NOT_RESPD       0xFE            /*  server not responding       */
#define NOT_OPENED      0xFD            /*  session not opened (usually
                                            port is already in use)     */
#define XMIT_FULL       0xFC            /*  XMIT buffer is full         */
#define XMIT_XOFF       0xFB            /*  XMIT is in XOFF state       */
#define NOT_ACTIVE      0xFA            /*  circuit is not active       */
#define BAD_FUNCTION    0xF9            /*  bad function call to NCSI   */
#define REFUSED         0xF8            /*  request refused (port or 
                                            circuit is not logged right)*/
#define NOT_ALLOCATED   0xF7            /*  vc is not allocated         */
#define BAD_SIZE        0xF6            /*  illegal buffer size         */
#define BAD_NUM         0xF5            /*  illegal circuit or port num */
#define BAD_RECALL      0xF4            /*  illegal re-entrant call     */

/*  vc init return codes */

#define PORT_UNAVAIL    0x15            /*  port not available          */

/*  virtual circuit receive status flags */

#define NO_DATA         0x00            /*  no data available to read   */
#define DATA_AVAIL      0x01            /*  data available to read      */

/*  virtual circuit transmit status flags */

#define XMIT_AVAIL      0x00            /*  transmission is available   */
#define XMIT_BUSY       0x01            /*  xmit not avail., port busy  */

/*  general definitions */

#define NCSI_ERR        -1              /*  error in interrupt call     */
#define NCSI_OK          0              /*  interrupt call is alright   */
#define VCMAX            9              /*  maximum number of vc's      */
#define VC_IBUFFSIZ      2049           /*  input buffer size           */
#define VC_OBUFFSIZ      129            /*  output buffer size          */
#define NAME_LEN         8              /*  name length                 */
#define SPEC_NMLEN       14             /*  special name length         */
#define SET              1
#define RESET            0
#define DCD_BIT          0x08           /*  CD bit in EXTERN_STAT ret.  */

/* these defines are scattered throughout lantool.c */

#define REQUEST_SIZE     12             /*  request packet size  VCINIT */
#define USE_DEF          0xFF           /*  use default port num VCINIT */
#define GET_SERVICE      1              /*  get the service name  VCINQ */
#define GET_SERVER       2              /*  get the server name   VCINQ */
#define GET_GENERAL      3              /*  get the general name  VCINQ */
#define GET_SPECIFIC     4              /*  get the specific name VCINQ */
#define SEARCH_FIRST     0              /*  get 1st match in bindery    */
#define SEARCH_NEXT      1              /*  get next match in bindery   */
#define SERV_DOWN        0              /*  server is down code OKTOSEND*/
#define SERV_BUSY        1              /*  server is busy code OKTOSEND*/
#define BIN_RETRIEVE     1              /*  retrieve name from bindery  */



/* define novell communication parameters */
#define NOVELL_NOPAR     0
#define NOVELL_ODDPAR    1
#define NOVELL_EVENPAR   2
#define NOVELL_MARKPAR   3
#define NOVELL_SPACEPAR  4

#define NOVELL_7_BITS    2
#define NOVELL_8_BITS    3

#define NOVELL_300_BAUD  5
#define NOVELL_1200_BAUD 7
#define NOVELL_2400_BAUD 10
#define NOVELL_4800_BAUD 12
#define NOVELL_9600_BAUD 14

#define NOVELL_STOP_BITS 2

#define NOVELL_ASYNCH_PROTOCOL   0
#define NOVELL_X25_PROTOCOL      1


/*  virtual circuit table */

typedef struct
   {
   BYTE vcid;                   /*  virtual circuit id           */
   INT  vcstate;                /*  virtual circuit state        */
   INT  portnum;                /*  port number                  */
   INT  srvportid;              /*  server channel port id       */
   CHAR srvc_name[COLS + 1];    /*  service name                 */
   CHAR srvr_name[NAME_LEN+1];  /*  server name                  */
   CHAR gen_name[NAME_LEN+1];   /*  general service name         */
   CHAR spec_name[SPEC_NMLEN+1];/*  special name                 */
   } VC_TYPE;


typedef struct {
   INT packet_len;                  /*  packet length            */
   CHAR server_name[NAME_LEN];      /*  server name              */
   CHAR general_name[NAME_LEN];     /*  general name             */
   CHAR specific_name[SPEC_NMLEN];  /*  specific_name            */
   BYTE protocol;                   /*  NEW  2.00 NCSI protocol  */
   BYTE status;                     /*  NEW  service's status    */
   } QUE_BUF_TYPE;

typedef struct
   {
   INT  reqpktlen;
   BYTE srvportid;
   BYTE rcvbaud;
   BYTE rcvbytelen;
   BYTE rcvstop;
   BYTE rcvprty;
   BYTE xmtbaud;
   BYTE xmtbytelen;
   BYTE xmtstop;
   BYTE xmtprty;
   BYTE dtr_ctrl;
   BYTE rts_ctrl;
   BYTE xon_xoff;
   BYTE reserved;
   } INIT_BUF_TYPE;


/* private functions */

extern INT      getSrvID          (BYTE, INIT_BUF_TYPE *);
extern INT      okToSend          (void);
extern INT      vcAlloc           (void);
extern INT      ncsiLdCk          (void);
extern INT      query             (QUE_BUF_TYPE *);
extern INT      vcOpen            (COMMDATA *);


/* public functions */
extern INT      NovellInit        (COMMDATA *);
extern INT      NovellOpen        (COMMDATA *);
extern INT      NovellClose       (COMMDATA *);
extern INT      NovellReadCh      (COMMDATA *, CHAR *);
extern INT      NovellReadStr     (COMMDATA *, CHAR *, INT);
extern INT      NovellWriteCh     (COMMDATA *, CHAR);
extern INT      NovellWriteStr    (COMMDATA *, CHAR *);
extern INT      NovellFlush       (COMMDATA *);
extern INT      NovellLineCheck   (COMMDATA *);
extern BOOLEAN  NovellCharReady   (COMMDATA *);

#endif

