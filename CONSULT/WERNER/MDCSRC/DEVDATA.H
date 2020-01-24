/*****************************************************************************
* FILE NAME:  devdata.h                                                      *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Comm Routines for Modem, Novell ACS2, NETBIOS ACS2, and INT 14 Devices   *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   no functions described                                                   *
*                                                                            *
*                                                                            *
* DATA STRUCTURES                                                            *
*                                                                            *
*   COMMTABLE    - A look-up table containing function pointers.  Each       *
*                  device class (defined in devcomm.h) has an array entry    *
*                  into the look-up table.  From this array entry, all of    *
*                  the functions for opening, closing, reading, writing, etc *
*                  can be referenced.                                        *
*                                                                            *
*                  Functions:                                                *
*                                                                            *
*                      init                                                  *
*                      open                                                  *
*                      close                                                 *
*                      readch                                                *
*                      readstr                                               *
*                      writech                                               *
*                      writestr                                              *
*                      break                                                 *
*                      flush                                                 *
*                      linecheck                                             *
*                      charready                                             *
*                                                                            *
*                                                                            *
*                  Example:                                                  *
*                                                                            *
*                      status = (CommTable [DEV_NOVELL].open) ();            *
*                                                                            *
*                                                                            *
*                                                                            *
*   COMMDATA     - General information about the communications              *
*                                                                            *
*                  Elements:                                                 *
*                                                                            *
*                    INT      portnum;                                       *
*                    INT      dev_type;                                      *
*                    BYTE     data_bits;                                     *
*                    BYTE     parity;                                        *
*                    BYTE     stop_bits;                                     *
*                    BOOLEAN  cd_check;                                      *
*                    INT      speed;                                         *
*                    CHAR     serv_name [];                                  *
*                    CHAR     target_name [];                                *
*                    CHAR     buffer [4K];                                   *
*                    CHAR     *buff_head;                                    *
*                    CHAR     *buff_tail;                                    *
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
 *  $Log:   N:/src/devdata.h_v  $
   
      Rev 8.0   18 Dec 1990 10:52:28   joeb
   TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
   
      Rev 7.0   06 Dec 1990 15:14:16   joeb
   revision 7.0 for iteration 5
   
      Rev 6.0   19 Nov 1990  9:18:40   joeb
   revision 6 for interation 4
   
      Rev 5.0   24 Oct 1990  9:02:10   andrews
   No change.
   
      Rev 4.0   01 Oct 1990 13:20:14   andrews
   ITERATION NUMBER 2
   
      Rev 3.0   17 Sep 1990 10:02:12   andrews
   Full Iteration 1
   
      Rev 2.0   27 Aug 1990  9:15:54   andrews
   iteration 1
   
      Rev 0.2   04 Aug 1990 12:07:28   deanm
   making devcomm a stronger lib
   
      Rev 0.1   25 Jul 1990 14:57:44   deanm
   general update
 *
*/



#ifndef DEVDATA_H
#define DEVDATA_H


/* define some stuff for the COMMDATA data structure */

#define INPUT_SIZE         4000
#define OUTPUT_SIZE        400
#define OVERFLOW_SIZE      20
#define COMM_BUFFER_SIZE  OUTPUT_SIZE + INPUT_SIZE + OVERFLOW_SIZE

#define LAN_NAME_LEN       16


/* needed for netbios */
#define PREFIX_LEN         30
#define NUMBER_LEN         30


/* the communication stuff data structure */

typedef struct
   {
   INT      portnum;
   INT      dev_type;
   BYTE     data_bits;
   BYTE     parity;
   BYTE     stop_bits;
   BOOLEAN  cd_check;
   INT      speed;
   CHAR     servName [LAN_NAME_LEN+1];      /* NETBIOS 16, NOVELL 8 chars */
   CHAR     targetName [LAN_NAME_LEN+1];    /* NETBIOS 16, NOVELL 8 chars */
   CHAR     dialPrefix [PREFIX_LEN+1];      /* needed for netbios */
   CHAR     phoneNumber [NUMBER_LEN+1];     /* needed for netbios */
   CHAR     buffer [COMM_BUFFER_SIZE];
   CHAR     *buff_head;
   CHAR     *buff_tail;
   INT      buff_cnt;
   }  COMMDATA;




/* the communication table data structure */

typedef struct
   {
   INT      (*init     )  (COMMDATA *) ;  
   INT      (*open     )  (COMMDATA *) ;  
   INT      (*close    )  (COMMDATA *) ;  
   INT      (*readch   )  (COMMDATA *, CHAR *) ;  
   INT      (*readstr  )  (COMMDATA *, CHAR *, INT) ;
   INT      (*writech  )  (COMMDATA *, CHAR) ; 
   INT      (*writestr )  (COMMDATA *, CHAR *) ;
   INT      (*sendbreak)  (COMMDATA *) ;
   INT      (*flush    )  (COMMDATA *) ;
   INT      (*linecheck)  (COMMDATA *) ;
   BOOLEAN  (*charready)  (COMMDATA *) ;
   }  COMMTABLE;


extern INT     stub      (COMMDATA *);
extern BOOLEAN bool_stub (COMMDATA *);
extern INT     rdch_stub (COMMDATA *, CHAR *);
extern INT     rdst_stub (COMMDATA *, CHAR *, INT);
extern INT     wtch_stub (COMMDATA *, CHAR);
extern INT     wtst_stub (COMMDATA *, CHAR *);
   
#endif
/*

                          (COMMDATA *) ;  
                          (COMMDATA *) ;  
                          (COMMDATA *) ;  
                          (COMMDATA *, CHAR *) ;  
                          (COMMDATA *, CHAR *, INT) ; 
                          (COMMDATA *, CHAR) ; 
                          (COMMDATA *, CHAR *) ;
                          (COMMDATA *) ;
                          (COMMDATA *) ;
                          (COMMDATA *) ;
                          (COMMDATA *) ;
*/
