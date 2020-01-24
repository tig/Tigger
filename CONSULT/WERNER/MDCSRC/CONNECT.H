/*****************************************************************************
* FILE NAME:  connect.h                                                      *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Connect to the OLS.                                                      *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Connect       - Connects to the OLS                                      *
*                                                                            *
*                                                                            *
* DATA STRUCTURES                                                            *
*                                                                            *
*   CONNECTDATA   - General info about connection to OLS                     *
*      INT     commPort                                                      *
*      INT     device                                                        *
*      INT     maxSpeed                                                      *
*      INT     parity                                                        *
*      INT     dialMethod                                                    *
*      CHAR    servName[]                                                    *
*      CHAR    targetName[]                                                  *
*      BOOLEAN checkCD                                                       *
*      BOOLEAN debug                                                         *
*      INT     speed                                                         *
*      CHAR    dialPrefix[]                                                  *
*      CHAR    phoneNumber[]                                                 *
*      CHAR    WANname[]                                                     *
*      CHAR    WANnode[]                                                     *
*      CHAR    WANnodeAddress[]                                              *
*      CHAR    WANpassword[]                                                 *
*      CHAR    ADF[]                                                         *
*      CHAR    ALF[]                                                         *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS if successful and FAILURE if failed to connect.                  *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/05/90                                                            *
*        08/08/90 moved Conn_Except prototype to exptions.h                  *
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
 *  $Log:   N:/src/connect.h_v  $
   
      Rev 8.0   18 Dec 1990 10:56:42   joeb
   TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
   
      Rev 7.0   06 Dec 1990 15:18:32   joeb
   revision 7.0 for iteration 5
   
      Rev 6.0   19 Nov 1990  9:23:12   joeb
   revision 6 for interation 4
   
      Rev 5.0   24 Oct 1990  9:06:52   andrews
   No change.
   
      Rev 4.0   01 Oct 1990 13:24:42   andrews
   ITERATION NUMBER 2
   
      Rev 3.0   17 Sep 1990 10:07:06   andrews
   Full Iteration 1
   
      Rev 2.0   27 Aug 1990  9:21:08   andrews
   iteration 1
   
      Rev 0.2   08 Aug 1990 15:02:42   deanm
   removed Conn_Except function prototype
   
      Rev 0.1   30 Jul 1990  9:12:00   deanm
   finishing up comm device drivers and comm scripts
   
      Rev 0.0   25 Jul 1990 14:57:14   deanm
   Initial revision.
 *
*/


#ifndef CONNECT_H
#define CONNECT_H


#define LAN_NAME_LEN       16
#define PREFIX_LEN         30
#define NUMBER_LEN         30
#define WAN_NAME_LEN       30
#define WAN_NODE_LEN       30
#define WAN_ADDRESS_LEN    30
#define WAN_PASSWORD_LEN   30
#define ADF_ALF_LEN        42
#define MODCHARS_LEN       20



typedef struct
   {
   INT     commPort;        
   INT     device;          
   INT     maxSpeed;        
   INT     parity;          
   INT     dialMethod;      
   CHAR    servName [LAN_NAME_LEN+1];      
   CHAR    targetName [LAN_NAME_LEN+1];    
   BOOLEAN checkCD;         
   BOOLEAN debug;
   INT     speed;           
   CHAR    dialPrefix [PREFIX_LEN+1];    
   CHAR    phoneNumber [NUMBER_LEN+1];   
   CHAR    WANname [WAN_NAME_LEN+1];       
   CHAR    WANnode [WAN_NODE_LEN+1];       
   CHAR    WANnodeAddress [WAN_ADDRESS_LEN+1];
   CHAR    WANpassword [WAN_PASSWORD_LEN+1];   
   CHAR    ADF [ADF_ALF_LEN+1];           
   CHAR    ALF [ADF_ALF_LEN+1];           
   } CONNECTDATA;



extern INT  Connect        (CHAR *, OLDATA *, INT (*) (void), INT (*) (void), 
                            BOOLEAN);
extern INT  Conn_SetupInit (CHAR *, CONNECTDATA *, OLDATA *);

#endif
