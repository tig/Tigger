/************************************************************************
 *
 *     Copyright (c) 1992-93 C.E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  fsm.h
 *
 *    This include file defines all data structures needed for
 *    the finite state machine implementation used througout
 *    ASYNCSRV.  This file is included by modules such as
 *    connect.c and bbs.c to define the basic FSM constructs.
 *
 ***********************************************************************/
#ifndef _FSM_H_
#define _FSM_H_

/*
 * Finite state machines
 */
#define ATOM_FSM_H       0x1     /* HIWORD */
#define ATOM_FSM_L       0x2     /* LOWORD */

#define CREATE_FSM         SET_FSM
#define SET_FSM(hwnd,lp) {\
                         SetProp( hwnd,\
                                  MAKEINTATOM( ATOM_FSM_H ),\
                                  (HANDLE)HIWORD((DWORD)lp) );\
                         SetProp( hwnd,\
                                  MAKEINTATOM( ATOM_FSM_L ),\
                                  (HANDLE)LOWORD((DWORD)lp) );\
                         }

#define GET_FSM(hwnd)    (LPFSM)(MAKELONG( GetProp( hwnd,\
                         MAKEINTATOM( ATOM_FSM_L ) ),\
                         GetProp( hwnd,\
                         MAKEINTATOM( ATOM_FSM_H ) ) ))\

#define REMOVE_FSM(hwnd) {\
                         RemoveProp( hwnd,\
                                     MAKEINTATOM( ATOM_FSM_H ) ) ;\
                         RemoveProp( hwnd,\
                                     MAKEINTATOM( ATOM_FSM_L ) ) ;\
                         }

typedef struct tagPAUSE
{
   DWORD    dwPause ;         // Pause length in milliseconds
   DWORD    dwStart ;         // Time started in milliseconds
} PAUSE, FAR *LPPAUSE ;

#define InPause(Pause)   (Pause.dwPause != 0)

typedef struct tagTIMEOUT
{
   UINT     uiTimeout;    // Timeout value in seconds
   UINT     uiStart ;      // time started in seconds
} TIMEOUT, FAR *LPTIMEOUT ;

#define InTimeout(Timeout) (Timeout.uiTimeout != 0)

/*                                                     
 * Generic FSM structure.  All FSMs have this as a header.
 *
 * This define is kinda neat, but is also kinda a hack.  It
 * gives us union type functionality without having to dereference
 * the structure more than once (i.e. we can go lpFSM->uiState
 * instead of lpFSM->Connect.uiState).
 *
 * !!! The last member of the header MUST NOT have a
 *     semi-colon or the compiler will puke.
 */

#define FSM_HEADER   UINT     uiState ;\
                     UINT     uiLastState ;\
                     UINT     uiNextState ;\
                     UINT     uiExitCode ;\
                     PAUSE    Pause ;\
                     TIMEOUT  Timeout
                        
                        

typedef struct tagFSM
{
   FSM_HEADER ;

} FSM, *PFSM, FAR *LPFSM ;

#endif

/*
 * FSM related functions found in FSM.C
 */
VOID  WINAPI StartPause( LPPAUSE lpPause, DWORD msecs ) ;
BOOL  WINAPI IsPauseDone( LPPAUSE lpPause ) ;
VOID  WINAPI ResetPause( LPPAUSE lpPause ) ;
VOID  WINAPI StartTimeout( LPTIMEOUT lpTimeout, UINT secs ) ;
BOOL  WINAPI IsTimeoutOver( LPTIMEOUT lpTimeout ) ;
VOID  WINAPI ResetTimeout( LPTIMEOUT lpTimeout ) ;

/************************************************************************
 * End of File: fsm.h
 ***********************************************************************/

