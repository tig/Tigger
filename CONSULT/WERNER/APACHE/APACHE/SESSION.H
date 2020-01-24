/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  session.h
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/

// DLL function definitions

#include "funcdefs.h"

// constant definitions

#define MAXLEN_INBLOCK        512
#define MAXLEN_STRLEN         81

#define PHONE_FILE            "APACHE.PHN"

#define CONNECTOR_EXT         ".CON"
#define EMULATOR_EXT          ".EMU"
#define PROTOCOL_EXT          ".XFR"

// notification message declarations

#define WM_CONNECTOR_NOTIFY 0x44
#ifndef CN_EVENT
#define CN_EVENT            0x04
#endif
#define CN_RXBLK            0x100
#define CN_TXEMPTY          0x101
#define CN_CONNECT          0x102
#define CN_DISCONNECT       0x103

#define WM_CMD_RXBLK        (WM_USER + 0x2000)
#define WM_CMD_TXBLK        (WM_USER + 0x2001)

#define DN_ENDDIALOG        0x100

// OpenSessionConnector / OpenSessionEmulator return values

#define ERR_SUCCESS    0x00
#define ERR_LIBERR     0x01
#define ERR_NOCB       0x02
#define ERR_PARAMS     0x03
#define ERR_CANTOPEN   0x04

// escape connector functions

#define ECF_CONNECTSTATUS   0x1000
#define ECF_SETNOTIFYWND    0x1001
#define ECF_GETNOTIFYWND    0x1002

// escape connector function return values

#define ECFERR_INVALIDCCB   -1
#define ECFERR_UNDEFFUNC    -2

// cursor states

#define ECS_HIDE       0x0001
#define ECS_SHOW       0x0002

// function ordinals

#define ORDINAL_GETCONNECTORUSECOUNT    0x02
#define ORDINAL_CREATECCB               0x03
#define ORDINAL_DESTROYCCB              0x04
#define ORDINAL_LOADCONNECTORSETTINGS   0x05
#define ORDINAL_SAVECONNECTORSETTINGS   0x06
#define ORDINAL_DOCONNECTORSETUPDLG     0x07
#define ORDINAL_OPENCONNECTOR           0x08
#define ORDINAL_CLOSECONNECTOR          0x09
#define ORDINAL_READCONNECTORBLOCK      0x0A
#define ORDINAL_WRITECONNECTORBYTE      0x0B
#define ORDINAL_WRITECONNECTORBLOCK     0x0C
#define ORDINAL_WRITECONNECTORFORMAT    0x0D
#define ORDINAL_ESCAPECONNECTORFUNCTION 0x0E

#define ORDINAL_GETEMULATORUSECOUNT     0x02
#define ORDINAL_LOADEMULATORSETTINGS    0x03
#define ORDINAL_SAVEEMULATORSETTINGS    0x04
#define ORDINAL_DOEMULATORSETUPDLG      0x05

#define ORDINAL_GETPROTOCOLUSECOUNT     0x02
#define ORDINAL_PROTOCOLRECEIVE         0x03
#define ORDINAL_PROTOCOLSEND            0x04

// macros

#define SESSIONCCB                (npSCB -> hCCB)
#define EMULATORWND               (npSCB -> hEmulatorWnd)
#define EMUTOPSTATWND             (npSCB -> hEmuTopStatWnd)
#define HPROTOCOLDLG              (npSCB -> hProtocolDlg)

#define CONNECTORLIB              (npSCB -> hConnectorLib)
#define EMULATORLIB               (npSCB -> hEmulatorLib)
#define PROTOCOLLIB               (npSCB -> hProtocolLib)

#define SESSIONENTRY              (npSCB -> wSessionEntry)
#define EMULATORNAME              (npSCB -> szEmulatorName)
#define CONNECTORNAME             (npSCB -> szConnectorName)
#define PROTOCOLNAME              (npSCB -> szProtocolName)

#define LPGETCONNECTORUSECOUNT    (npSCB -> lpGetConnectorUseCount)
#define LPCREATECCB               (npSCB -> lpCreateCCB)
#define LPDESTROYCCB              (npSCB -> lpDestroyCCB)
#define LPLOADCONNECTORSETTINGS   (npSCB -> lpLoadConnectorSettings)
#define LPSAVECONNECTORSETTINGS   (npSCB -> lpSaveConnectorSettings)
#define LPDOCONNECTORSETUPDLG     (npSCB -> lpDoConnectorSetupDlg)
#define LPOPENCONNECTOR           (npSCB -> lpOpenConnector)
#define LPCLOSECONNECTOR          (npSCB -> lpCloseConnector)
#define LPREADCONNECTORBLOCK      (npSCB -> lpReadConnectorBlock)
#define LPWRITECONNECTORBYTE      (npSCB -> lpWriteConnectorByte)
#define LPWRITECONNECTORBLOCK     (npSCB -> lpWriteConnectorBlock)
#define LPWRITECONNECTORFORMAT    (npSCB -> lpWriteConnectorFormat)
#define LPESCAPECONNECTORFUNCTION (npSCB -> lpEscapeConnectorFunction)

#define LPGETEMULATORUSECOUNT     (npSCB -> lpGetEmulatorUseCount)
#define LPLOADEMULATORSETTINGS    (npSCB -> lpLoadEmulatorSettings)
#define LPSAVEEMULATORSETTINGS    (npSCB -> lpSaveEmulatorSettings)
#define LPDOEMULATORSETUPDLG      (npSCB -> lpDoEmulatorSetupDlg)

#define LPGETPROTOCOLUSECOUNT     (npSCB -> lpGetProtocolUseCount)
#define LPPROTOCOLRECEIVE         (npSCB -> lpProtocolReceive)
#define LPPROTOCOLSEND            (npSCB -> lpProtocolSend)

// pseudo-functions

#define GetConnectorUseCount      (*(npSCB -> lpGetConnectorUseCount))
#define CreateCCB                 (*(npSCB -> lpCreateCCB))
#define DestroyCCB                (*(npSCB -> lpDestroyCCB))
#define LoadConnectorSettings     (*(npSCB -> lpLoadConnectorSettings))
#define SaveConnectorSettings     (*(npSCB -> lpSaveConnectorSettings))
#define DoConnectorSetupDlg       (*(npSCB -> lpDoConnectorSetupDlg))
#define OpenConnector             (*(npSCB -> lpOpenConnector))
#define CloseConnector            (*(npSCB -> lpCloseConnector))
#define ReadConnectorBlock        (*(npSCB -> lpReadConnectorBlock))
#define WriteConnectorByte        (*(npSCB -> lpWriteConnectorByte))
#define WriteConnectorBlock       (*(npSCB -> lpWriteConnectorBlock))
#define WriteConnectorFormat      (*(npSCB -> lpWriteConnectorFormat))
#define EscapeConnectorFunction   (*(npSCB -> lpEscapeConnectorFunction))
#define IsConnected               (*(npSCB -> lpIsConnected))

#define GetEmulatorUseCount       (*(npSCB -> lpGetEmulatorUseCount))
#define LoadEmulatorSettings      (*(npSCB -> lpLoadEmulatorSettings))
#define SaveEmulatorSettings      (*(npSCB -> lpSaveEmulatorSettings))
#define DoEmulatorSetupDlg        (*(npSCB -> lpDoEmulatorSetupDlg))

#define ProtocolReceive           (*(npSCB -> lpProtocolReceive))
#define ProtocolSend              (*(npSCB -> lpProtocolSend))

// data structures

typedef struct tagSCB
{
   char         szEmulatorName[ MAXLEN_STRLEN ], 
                szConnectorName[ MAXLEN_STRLEN ],
                szProtocolName[ MAXLEN_STRLEN ] ;
   BOOL         fConnected ;
   HINSTANCE    hEmulatorLib, hConnectorLib, hProtocolLib ;
   HWND         hEmulatorWnd, hEmuTopStatWnd, hProtocolDlg ;
   HANDLE       hCCB ;
   WORD         wSessionEntry ;

   // connector function pointers

   LPFNGETUSECOUNT              lpGetConnectorUseCount ;
   LPFNCREATECCB                lpCreateCCB ;
   LPFNDESTROYCCB               lpDestroyCCB ;
   LPFNLOADCONNECTORSETTINGS    lpLoadConnectorSettings ;
   LPFNSAVECONNECTORSETTINGS    lpSaveConnectorSettings ;
   LPFNDOCONNECTORSETUPDLG      lpDoConnectorSetupDlg ;
   LPFNOPENCONNECTOR            lpOpenConnector ;
   LPFNCLOSECONNECTOR           lpCloseConnector ;
   LPFNREADCONNECTORBLOCK       lpReadConnectorBlock ;
   LPFNWRITECONNECTORBYTE       lpWriteConnectorByte ;
   LPFNWRITECONNECTORBLOCK      lpWriteConnectorBlock ;
   LPFNWRITECONNECTORFORMAT     lpWriteConnectorFormat ;
   LPFNESCAPECONNECTORFUNCTION  lpEscapeConnectorFunction ;

   // emulator function pointers

   LPFNGETUSECOUNT              lpGetEmulatorUseCount ;
   LPFNLOADEMULATORSETTINGS     lpLoadEmulatorSettings ;
   LPFNSAVEEMULATORSETTINGS     lpSaveEmulatorSettings ;
   LPFNDOEMULATORSETUPDLG       lpDoEmulatorSetupDlg ;

   // protocol function pointers

   LPFNGETUSECOUNT              lpGetProtocolUseCount ;
   LPFNPROTOCOLRECEIVE          lpProtocolReceive ;
   LPFNPROTOCOLSEND             lpProtocolSend ;

} SCB, NEAR *NPSCB ;

#if (WINVER < 0x30a)
typedef HANDLE HSCB ;
#else
DECLARE_HANDLE( HSCB ) ;
#endif

// function prototypes (public)

BOOL CreateSession( HWND, LPSTR ) ;
BOOL DestroySession( HWND ) ;
BOOL TranslateNotification( HWND, WORD, WORD, LONG ) ;

// function prototypes (private)

HSCB CreateSCB( VOID ) ;
BOOL DestroySCB( HSCB ) ;
int CreateConnector( HWND, LPSTR ) ;
BOOL DestroyConnector( HWND ) ;
VOID GetConnectorFunctionPointers( NPSCB ) ;
int CreateEmulator( HWND, LPSTR ) ;
BOOL DestroyEmulator( HWND ) ;
VOID GetEmulatorFunctionPointers( NPSCB ) ;
int OpenProtocol( HINSTANCE, HSCB ) ;
BOOL CloseProtocol( HSCB ) ;
VOID GetProtocolFunctionPointers( NPSCB ) ;

BOOL TranslateEvent( HWND, WORD, WORD, LONG ) ;
BOOL ProcessTerminalMessage( HWND, WORD, WORD, LONG ) ;

// function prototypes (imported or external)

HINSTANCE OpenLibrary( LPSTR, LPSTR ) ;
VOID CloseLibrary( HINSTANCE, FARPROC ) ;

/************************************************************************
 * End of File: session.h
 ************************************************************************/

