/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  connect.h
 *
 ***********************************************************************/
#include "fsm.h"

#define MAX_MODEMCMD    256
#define MAX_MODEMRESP   256

/*
 * Connect dialog FSM
 */
typedef struct tagCONNECTFSM
{
   FSM_HEADER ;

   COMMSETTINGS   CommSettings ;

   /*
    * Stuff pulled out of MODEM.INI file
    */
   char     szInit[MAX_MODEMCMD+1] ;         // init modem string
   LPSTR    lpszInitCur ;                    // current position in init string
   char     szDial[MAX_MODEMCMD+1] ;        // dial modem string (incl. number)
   LPSTR    lpszDialCur ;                   // current position in dial string
   char     szHangUp[MAX_MODEMCMD+1] ;      // hang up modem string
   LPSTR    lpszHangUpCur ;                 // current position in hangup string
   char     szConnect[MAX_MODEMRESP+1] ;    // connect result for no specific baud
   char     szConnect300[MAX_MODEMRESP+1] ; // connect result for 300 baud
   char     szConnect1200[MAX_MODEMRESP+1] ;// connect result for 1200 baud
   char     szConnect2400[MAX_MODEMRESP+1] ;// connect result for 2400 baud
   char     szConnect4800[MAX_MODEMRESP+1] ;// connect result for 4800 baud
   char     szConnect9600[MAX_MODEMRESP+1] ;// connect result for 9600 baud
   char     szBusy[MAX_MODEMRESP+1] ;       // BUSY response
   char     szNoDialtone[MAX_MODEMRESP+1] ; // NO DIALTONE response
   char     szNoCarrier[MAX_MODEMRESP+1] ;  // NO CARRIER response
   char     szOk[MAX_MODEMRESP+1] ;         // OK response
   char     szError[MAX_MODEMRESP+1] ;      // ERROR Response
   BOOL     fIgnoreCD ;
} CONNECTFSM, *PCONNECTFSM, FAR *LPCONNECTFSM ;

/*
 * Forced functions for the FSM.
 */
#define FF_PROCESS          0x0000
#define FF_INITFSM          0x0001
#define FF_EXITFSM          0x0002
#define FF_USERABORT        0x0003


BOOL WINAPI DoConnect( VOID ) ;
BOOL WINAPI DoDisConnect( VOID ) ;




