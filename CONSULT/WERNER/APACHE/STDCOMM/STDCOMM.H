/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  stdcomm.h
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  header file for stdcomm.dll (exported information)
 *
 *   Revisions:  
 *     01.00.000  1/25/91 baw   Initial build, initial version
 *     01.00.001  6/25/91 baw   Cleaned up.
 *     01.00.002  8/10/91 baw   Added TranslateAndDispatch callback
 *
 ************************************************************************/

#include "resource.h"
#include "version.h"

#include "..\apache\asciidef.h"
#include "..\apache\facelift.h"

// constant definitions

#define FC_DTRDSR         0x01
#define FC_RTSCTS         0x02
#define FC_XONXOFF        0x04

#define TXBUFSIZE         4096
#define RXBUFSIZE         4096

#define MAXLEN_PORT       10
#define MAXLEN_FMTSTR     255
#define MAXLEN_PARAMS     255
#define MAXLEN_TEMPSTR    81

// connector events

#define WM_CONNECTOR_NOTIFY 0x44
#ifndef CN_EVENT
#define CN_EVENT            0x04
#endif
#define CN_RXBLK            0x100
#define CN_TXEMPTY          0x101
#define CN_CONNECT          0x102
#define CN_DISCONNECT       0x103

#define MAXEVENTS           16

// escape connector functions

#define ECF_CONNECTSTATUS   0x1000
#define ECF_SETNOTIFYWND    0x1001
#define ECF_GETNOTIFYWND    0x1002

// escape connector function return values

#define ECFERR_INVALIDCCB   -1
#define ECFERR_UNDEFFUNC    -2

// atoms defined for use with properties

#define ATOM_HCCB           0x100

// data structures

typedef struct
{
   BYTE  bPort ;
   WORD  wBaudRate ;
   BYTE  bByteSize ;
   BYTE  bParity ;
   BYTE  bStopBits ;
   BYTE  bFlowControl ;
   WORD  wBreakLength ;

} COMSETTINGS ;

typedef struct tagCCB
{
   COMSETTINGS      csSettings ;   // comm settings
   BOOL             fConnected ;   // connected status
   HWND             hParentWnd ;         // parent window
   int              nCid ;         // Comm ID

} CCB ;

typedef CCB NEAR *NPCCB ;

#ifdef WIN31
DECLARE_HANDLE( HCCB ) ;
#else
typedef HANDLE HCCB ;
#endif

typedef struct tagEVENTID
{
   int         nCid ;          // nCid
   BOOL        fPostFlag ;     // notification posted (blocking)
   HANDLE      hWnd ;          // used in posting notification
   WORD        wEvent ;        // posted to window
   WORD        wEventMask ;    // used for CommEventMask()
   WORD FAR *  lpEventPtr ;    // event pointer

} EVENTID ;

// macros

#define SETTINGS( x )    (x -> csSettings)
#define CONNECTED( x )   (x -> fConnected)
#define PARENTWND( x )   (x -> hParentWnd)
#define CID( x )         (x -> nCid)

#define SETHCCB( x, y )  SetProp( x, MAKEINTATOM( ATOM_HCCB ), y )
#define GETHCCB( x )     GetProp( x, MAKEINTATOM( ATOM_HCCB ) )
#define REMOVEHCCB( x )  RemoveProp( x, MAKEINTATOM( ATOM_HCCB ) )

// global variables

#ifdef GLOBALDEFS
HANDLE   hLibInst ;
int      nIdTimer, nUseCount ;
EVENTID  Events[MAXEVENTS] ;
#else
extern HANDLE   hLibInst ;
extern int      nIdTimer, nUseCount ;
extern EVENTID  Events[MAXEVENTS] ;
#endif

// function prototypes (private)

/* event handling functions */

BOOL ClearEvent( HANDLE, WORD ) ;
BOOL KillEvent( HANDLE ) ;
BOOL SetEvent( HANDLE, WORD ) ;

/* dialog box functions */

BOOL NEAR SettingsDlgInit( HWND, HANDLE ) ;
BOOL NEAR SettingsDlgTerm( HWND, HANDLE ) ;

BOOL SetupConnector( HCCB ) ;

// function prototypes (public)

int FAR PASCAL LibMain( HINSTANCE, WORD, WORD, LPSTR ) ;
VOID FAR PASCAL WEP( int ) ;
int FAR PASCAL GetConnectorUseCount( VOID ) ;
HCCB FAR PASCAL CreateCCB( HWND ) ;
BOOL FAR PASCAL DestroyCCB( HCCB ) ;
BOOL FAR PASCAL LoadConnectorSettings( HCCB, LPSTR, LPSTR ) ;
BOOL FAR PASCAL SaveConnectorSettings( HCCB, LPSTR, LPSTR ) ;
BOOL FAR PASCAL DoConnectorSetupDlg( HCCB, HWND, LPSTR, LPSTR ) ;
BOOL FAR PASCAL OpenConnector( HCCB ) ;
BOOL FAR PASCAL CloseConnector( HCCB ) ;
int FAR PASCAL ReadConnectorBlock( HCCB, int, LPSTR ) ;
BOOL FAR PASCAL WriteConnectorByte( HCCB, BYTE ) ;
BOOL FAR PASCAL WriteConnectorBlock( HCCB, LPSTR, int ) ;
BOOL FAR _cdecl WriteConnectorFormat( HCCB, LPSTR, LPSTR ) ;
LONG FAR PASCAL EscapeConnectorFunction( HCCB, WORD, LONG, LONG ) ;
BOOL FAR PASCAL SettingsDlgProc( HWND, WORD, WORD, LONG ) ;
VOID FAR PASCAL TimerHandler( VOID ) ;

// function prototypes (imported or externally defined)

WORD FAR PASCAL CreateSystemTimer( int, FARPROC ) ;
WORD FAR PASCAL KillSystemTimer( WORD ) ;

/************************************************************************
 * End of File: stdcomm.h
 ************************************************************************/

