/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  netbios.h
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Header file for NetBIOS DLL
 *
 *   Revisions:  
 *     01.00.000  3/ 6/91 baw   Initial version, initial build
 *     01.00.001  8/10/91 baw   Added TranslateAndDispatch callback
 *     01.00.002  9/15/91 baw   Removed TranslateAndDispatch callback
 *
 ************************************************************************/

#include <windows.h>

#include <cmndll.h>
#include <wdb.h>

#include <dos.h>
#include <memory.h>
#include <string.h>

#include "resource.h"
#include "version.h"

/************************************************************************
 *
 * Module definitions
 *    netbios.c
 *    
 ************************************************************************/

// constant definitions

/* NetBIOS timeout (30 seconds) */

#define NETBIOS_TIMEOUT     30000

/* post message */

#define WM_EVENT_RXCHAR     (WM_USER + 0x1000)
#define WM_EVENT_TXCHAR     (WM_USER + 0x1001)
#define WM_EVENT_DISCONNECT (WM_USER + 0x1002)

#define WM_CMD_RXBLK        (WM_USER + 0x2000)
#define WM_CMD_TXBLK        (WM_USER + 0x2001)

/* string lengths */

#define MAXLEN_FMTSTR     255
#define MAXLEN_PARAMS     255
#define MAXLEN_TEMPSTR    81
#define MAXLEN_TIMEOUT    5

#define MAXLEN_NCBNAMESIZE  16
#define MAXLEN_XMTBLOCK     4096
#define MAXLEN_RECBLOCK     4096

/* LAN adapter numbers */

#define LAN_ADAPTER_PRIMARY    0
#define LAN_ADAPTER_SECONDARY  1

/* definitions for NetBIOS commands (NBC's) */

#define NBC_ASYNC               0x80
#define NBC_NO_WAIT             0x80

#define NBC_RESET               0x32
#define NBC_CANCEL              0x35
#define NBC_ADAPTER_STATUS      0x33
#define NBC_TRACE               0x79
#define NBC_UNLINK              0x70
#define NBC_ADD_NAME            0x30
#define NBC_ADD_GROUP_NAME      0x36
#define NBC_DELETE_NAME         0x31
#define NBC_CALL                0x10
#define NBC_LISTEN              0x11
#define NBC_HANG_UP             0x12
#define NBC_SEND                0x14
#define NBC_SEND_NO_ACK         0x71
#define NBC_CHAIN_SEND          0x17
#define NBC_CHAIN_SEND_NO_ACK   0x72
#define NBC_RECEIVE             0x15
#define NBC_RECEIVE_ANY         0x16
#define NBC_SESSION_STATUS      0x34
#define NBC_SEND_DATAGRAM       0x20
#define NBC_SEND_BCST_DATAGRAM  0x22
#define NBC_RCV_DATAGRAM        0x21
#define NBC_RCV_BCST_DATAGRAM   0x23

#define NBC_CALL_NIU            0x74    /* Ungerman-Bass only       */
#define NBC_SEND_PKTU           0x77    /* Ungerman-Bass only       */
#define NBC_RECEIVE_PKTU        0x78    /* Ungerman-Bass only       */

#define NBC_INVALID_COMMAND     0x7F    /* for presence test        */


/* definitions for NetBIOS errors */

#define NBE_NO_ERROR            0x00    /* no error                 */
#define NBE_BUFFER_LENGTH       0x01    /* illegal buffer length    */
#define NBE_INVALID_COMMAND     0x03    /* invalid command          */
#define NBE_TIMED_OUT           0x05    /* timed out                */
#define NBE_MSG_INCOMPLETE      0x06    /* message incomplete       */
#define NBE_SEND_NO_ACK_FAIL    0x07    /* send no ack data !rcvd   */
#define NBE_INVALID_LSN         0x08    /* invalid local session #  */
#define NBE_NO_RESOURCE         0x09    /* no resource avail; temp. */
#define NBE_SESSION_CLOSED      0x0A    /* session closed           */
#define NBE_COMMAND_CANCELLED   0x0B    /* command cancelled        */
#define NBE_DUPLICATE_LNAME     0x0D    /* dup name in local table  */
#define NBE_LNAME_TABLE_FULL    0x0E    /* local name table is full */
#define NBE_ACTIVE_SESSIONS     0x0F    /* name has active sessions */
#define NBE_LSESSION_TABLE_FULL 0x11    /* local session table full */
#define NBE_UNSPECIFIED_ERROR   0xF0    /* some other error         */
#define NBE_PENDING             0xFF    /* command is pending       */

/* NetBIOS Extension definitions */

#define NETEXT_STANDARD         0x00
#define NETEXT_UNGERMANBASS     0x01

/* PerformNetBIOS wait states */

#define PERFORM_NOWAIT          0x00
#define PERFORM_WAIT            0x01

/* GlobalAlloc flags for NCB block types */

#define NCB_GMEMFLAGS  (GMEM_FIXED | GMEM_NOT_BANKED | GMEM_ZEROINIT)

/* GlobalAlloc flags for DATA BLOCK types */

#define DATA_GMEMFLAGS (GMEM_MOVEABLE | GMEM_NOT_BANKED | GMEM_ZEROINIT)

// data structures

typedef LOCALHANDLE  HNCB ;
typedef LOCALHANDLE  HNBAS ;
typedef LOCALHANDLE  HNBSS ;

/* NetBIOS Control Block (NCB) */

typedef struct tagNCB
{
   BYTE      bCommand ;
   BYTE      bReturn ;
   BYTE      bLSN ;
   BYTE      bNumber ;
   void FAR  *lpBuffer ;
   WORD      wLength ;
   BYTE      abCallName[MAXLEN_NCBNAMESIZE] ;
   BYTE      abName[MAXLEN_NCBNAMESIZE] ;
   BYTE      bRTO ;
   BYTE      bSTO ;
   void FAR  *lpPostFunc ;
   BYTE      bLanaNum ;
   BYTE      bCmdCmplt ;
   BYTE      abReserved[14] ;

} NCB ;

typedef NCB FAR  *LPNCB ;

/* NetBIOS Extended Control Block (XNCB) */

typedef struct tagXNCB
{

   NCB                       ncb ;
   HWND                      hWnd ;
   HNCB                      hNCB ;
   void FAR                  *lpPostFunc ;
   BYTE                      bLana, bAsync ;

} XNCB ;

typedef XNCB FAR  *LPXNCB ;

/* NetBIOS Adapter Status (NBAS) */

typedef struct tagNBAS
{
   BYTE   abCardId[6], bReleaseLevel, bReserved1, bTypeOfAdapter,
          bOldOrNewParams ;
   WORD   wReportingPeriodMins, wFrameRejectRecvdCount, wFrameRejectSentCount,
          wRecvdDataFrameErrors, wUnsuccessfulTransmissions ;
   DWORD  dwGoodTransmissions, dwGoodReceptions ;
   WORD   wRetransmissions, wExhaustedResourceCount, wT1TimerExpiredCount,
          wTiTimerExpiredCount ;
   BYTE   abReserved[4] ;
   WORD   wAvailableNCBs, wMaxNCBsConfigured, wMaxNCBsPossible, 
          wBufferOrStationBusyCount, wMaxDatagramSize, wPendingSessions,
          wMaxSessionsConfigured, wMaxSessionsPossible, wMaxFrameSize ;
   int    nNameCount ;

   struct
   {
      char  achName[MAXLEN_NCBNAMESIZE] ;
      BYTE  bNumber, bStatus ;
   } aNameTable[16] ;

} NBAS ;

typedef NBAS FAR  *LPNMAS ;

/* NetBIOS Session Info (NBSINFO) */

typedef struct tagNBSINFO
{
   BYTE  bLSN, bState ;
   char  achLocalName[MAXLEN_NCBNAMESIZE], achRemoteName[MAXLEN_NCBNAMESIZE] ;
   BYTE  bRxCount, bTxCount ;

} NBSINFO ;

typedef NBSINFO FAR  *LPNBSINFO ;

/* NetBIOS Session Status (NBSS) */

typedef struct tagNBSS
{
   BYTE     bNameNum, bSessionCount, bDatagramCount, bRxAnyCount ;
   NBSINFO  aSession[16] ;

} NBSS ;

typedef NBSS FAR  *LPNBSS ;

typedef struct tagCCB
{
   char                      achServer[MAXLEN_NCBNAMESIZE] ;
   char                      achLocal[MAXLEN_NCBNAMESIZE] ;
   BOOL                      fBlankPadding, fServerMode, fConnected ;
   BYTE                      bLSN ;
   GLOBALHANDLE              hRecBlock, hXmtBlock ;
   WORD                      wTxTimeout, wRxTimeout, wExtension ;
   HNCB                      hConnectNCB, hReadNCB, hWriteNCB ;
   HWND                      hParentWnd ;

} CCB ;

typedef CCB NEAR *NPCCB ;

// macros

#define SERVER( x )        (x -> achServer)
#define LOCAL( x )         (x -> achLocal)
#define BLANKPADDING( x )  (x -> fBlankPadding)
#define SERVERMODE( x )    (x -> fServerMode)
#define CONNECTED( x )     (x -> fConnected)
#define LSN( x )           (x -> bLSN)
#define RXBLOCK( x )       (x -> hRecBlock)
#define TXBLOCK( x )       (x -> hXmtBlock)
#define TXTIMEOUT( x )     (x -> wTxTimeout)
#define RXTIMEOUT( x )     (x -> wRxTimeout)
#define EXTENSION( x )     (x -> wExtension)
#define CONNECTNCB( x )    (x -> hConnectNCB)
#define READNCB( x )       (x -> hReadNCB)
#define WRITENCB( x )      (x -> hWriteNCB)
#define PARENTWND( x )     (x -> hParentWnd)

// global variables

#ifdef GLOBALDEFS
int     nUseCount ;
HANDLE  hInstLib ;
#else
extern int     nUseCount ;
extern HANDLE  hInstLib ;
#endif

// function prototypes (public)

int FAR PASCAL GetConnectorUseCount( VOID ) ;
LOCALHANDLE FAR PASCAL CreateCCB( VOID ) ;
BOOL FAR PASCAL DestroyCCB( LOCALHANDLE ) ;
BOOL FAR PASCAL InitCCB( LOCALHANDLE ) ;
BOOL FAR PASCAL OpenConnector( LOCALHANDLE, HWND ) ;
BOOL FAR PASCAL CloseConnector( LOCALHANDLE, HWND ) ;
int FAR PASCAL ReadConnectorBlock( LOCALHANDLE, HWND, int, LPSTR ) ;
BOOL FAR PASCAL WriteConnectorByte( LOCALHANDLE, BYTE ) ;
BOOL FAR PASCAL WriteConnectorBlock( LOCALHANDLE, LPSTR, int ) ;
BOOL FAR _cdecl WriteConnectorFormat( LOCALHANDLE, LPSTR, ... ) ;
BOOL FAR PASCAL IsConnected( LOCALHANDLE ) ;
VOID FAR _cdecl interrupt PostNetBIOS( WORD, WORD, WORD, WORD, WORD, WORD,
                                       WORD, WORD, WORD, WORD, WORD, WORD,
                                       WORD ) ;
WORD FAR PASCAL InvokeNetBIOS( LPNCB ) ;
WORD FAR PASCAL GetMachineName( LPSTR ) ;
BOOL FAR PASCAL DoConnectorSetupDlg( LOCALHANDLE, HWND, LPSTR, LPSTR ) ;
BOOL FAR PASCAL SettingsDlgProc( HWND, WORD, WORD, LONG ) ;
BOOL FAR PASCAL LoadConnectorSettings( LOCALHANDLE, LPSTR, LPSTR ) ;
BOOL FAR PASCAL SaveConnectorSettings( LOCALHANDLE, LPSTR, LPSTR ) ;

// function prototypes (private)

BOOL SetupConnector( LOCALHANDLE ) ;
HNCB NBCreateNCB( HWND, BYTE, BOOL, LPVOID ) ;
BOOL NBDestroyNCB( HNCB ) ;
BOOL NBCheckAvail( VOID ) ;
WORD NBPerformCommand( LPNCB, BYTE, BOOL ) ;
LPSTR NBCreatePaddedName( LPSTR, LPSTR ) ;
BOOL NBClearNCB( HNCB ) ;
WORD NBAddLocalName( HNCB, LPSTR ) ;
WORD NBDeleteLocalName( HNCB, LPSTR ) ;
WORD NBCreateSession( HNCB, LPSTR, LPSTR, WORD, WORD, WORD, BOOL ) ;
WORD NBDestroySession( HNCB, LPSTR, BYTE ) ;
BYTE NBGetLSN( HNCB ) ;
BYTE NBGetNameLen( LPSTR ) ;
WORD NBRequestReceive( HNCB, BYTE, GLOBALHANDLE, WORD ) ;
WORD NBPostSend( HNCB, BYTE, GLOBALHANDLE, LPSTR, WORD, WORD ) ;
int GetRadioButton( HWND, int, int ) ;
BOOL SettingsDlgInit( HWND, LOCALHANDLE ) ;
BOOL SettingsDlgTerm( HWND, LOCALHANDLE ) ;

BOOL GetPrivateProfileTrueFalse( LPSTR, LPSTR, LPSTR, LPSTR ) ;
WORD GetPrivateProfileWord( LPSTR, LPSTR, LPSTR, LPSTR ) ;
DWORD GetPrivateProfileDW( LPSTR, LPSTR, LPSTR, LPSTR ) ;

BOOL WritePrivateProfileTrueFalse( LPSTR, LPSTR, BOOL, LPSTR ) ;
BOOL WritePrivateProfileWord( LPSTR, LPSTR, WORD, LPSTR ) ;
BOOL WritePrivateProfileDW( LPSTR, LPSTR, DWORD, LPSTR ) ;

/************************************************************************
 * End of File: netbios.h
 ************************************************************************/
