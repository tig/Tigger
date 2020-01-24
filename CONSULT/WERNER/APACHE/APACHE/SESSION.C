/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  session.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Session control functions
 *
 *               These functions control opening and closing of
 *               sessions.  Sessions are defined in APACHE.INI.
 *
 *   Revisions:  
 *     01.00.001  2/25/91 baw   Initial version, initial build
 *     01.00.010  5/ 2/91 baw   Overhauled the code.  Simplified
 *                              this code by reducing the number
 *                              of exported functions from the
 *                              associated DLLs.
 *     01.00.011  6/25/91 baw   Implemented new INI structure.
 *
 ************************************************************************/

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"
#include "dialog.h"
#include "session.h"
#include "size.h"
#include "emutop.h"

/************************************************************************
 *  HSCB CreateSCB( VOID )
 *
 *  Description:
 *     Creates a Session Control Block
 *
 *  Comments:
 *      6/25/91  baw  Added this comment block.
 *
 ************************************************************************/

HSCB CreateSCB( VOID )
{
   return ( (HSCB) LocalAlloc( LMEM_MOVEABLE | LMEM_ZEROINIT,
                               sizeof( SCB ) ) ) ;

} /* end of CreateSCB() */

/************************************************************************
 *  BOOL DestroySCB( HSCB hSCB )
 *
 *  Description:
 *     Destroys the Session Control Block.
 *
 *  Comments:
 *      6/25/91  baw  Added this comment block.
 *
 ************************************************************************/

BOOL DestroySCB( HSCB hSCB )
{
   if (!hSCB)
      return ( FALSE ) ;

   return ( NULL == LocalFree( hSCB ) ) ;

} /* end of DestroySCB() */

/************************************************************************
 *  BOOL CreateSession( HWND hWnd, LPSTR lpSessionName )
 *
 *  Description:
 *     Attempts to open related DLLs given the session name.
 *     Returns TRUE on SUCCESS, otherwise FALSE.
 *
 *  Comments:
 *      6/25/91  baw  Added this comment block.
 *      6/25/91  baw  Implemented new INI file format
 *
 ************************************************************************/

BOOL CreateSession( HWND hWnd, LPSTR lpSessionName )
{
   HCURSOR  hOldCursor, hWaitCursor ;
   HSCB     hSCB ;
   NPSCB    npSCB ;

   // inform user it could be a wait

   hWaitCursor = LoadCursor( NULL, IDC_WAIT ) ;
   hOldCursor = SetCursor( hWaitCursor ) ;

   if (NULL == (hSCB = CreateSCB()))
   {
      SetCursor( hOldCursor ) ;
      return ( FALSE ) ;
   }

   // Store SCB in Window structure

   SetWindowWord( hWnd, GWW_SCB, (WPARAM) hSCB ) ;

   if (ERR_SUCCESS != CreateConnector( hWnd, lpSessionName ))
   {
      // NEED: to implement retry here

      MessageBox( hWnd, "Connector not available!", szAppName,
                  MB_ICONEXCLAMATION | IDOK ) ;
      DestroySCB( hSCB ) ;
      SetWindowWord( hWnd, GWW_SCB, NULL ) ;
      SetCursor( hOldCursor ) ;
      return ( FALSE ) ;
   }

   if (ERR_SUCCESS != CreateEmulator( hWnd, lpSessionName ))
   {
      // fatal error - can not continue
      DestroyConnector( hWnd ) ;
      DestroySCB( hSCB ) ;
      SetWindowWord( hWnd, GWW_SCB, NULL ) ;
      SetCursor( hOldCursor ) ;
      return ( FALSE ) ;
   }

#ifdef NOT_USED

   if (!CreateEmuTopStatus( hWnd ))
   {
      // fatal error - can not continue
      DestroyEmulator( hWnd ) ;
      DestroyConnector( hWnd ) ;
      DestroySCB( hSCB ) ;
      SetWindowWord( hWnd, GWW_SCB, NULL ) ;
      SetCursor( hOldCursor ) ;
      return ( FALSE ) ;
   }

#endif

   // Everything went A-OK... store session name in structure and
   // set window title.

   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
   {
      DestroyEmulator( hWnd ) ;
      DestroyConnector( hWnd ) ;
      DestroySCB( hSCB ) ;
      SetWindowWord( hWnd, GWW_SCB, NULL ) ;
      SetCursor( hOldCursor ) ;
      return ( FALSE ) ;
   }

   LocalUnlock( hSCB ) ;
   SetWindowText( hWnd, lpSessionName ) ;
   SetCursor( hOldCursor ) ;

   return ( TRUE ) ;

} /* end of CreateSession() */

/************************************************************************
 *  BOOL DestroySession( HSCB hSCB, HWND hWnd )
 *
 *  Description:
 *     Destroys (closes) the session associated with hWnd and hSCB.
 *
 *  Comments:
 *      6/25/91  baw  Added this comment block.
 *
 ************************************************************************/

BOOL DestroySession( HWND hWnd )
{
   HCURSOR  hOldCursor, hWaitCursor ;
   HSCB     hSCB ;

   // Get SCB from Window structure
   hSCB = GETSCB( hWnd ) ;

   // inform user to wait a while
   hWaitCursor = LoadCursor( NULL, IDC_WAIT ) ;
   hOldCursor = SetCursor( hWaitCursor ) ;

#ifdef NOT_USED
   DestroyEmuTopStatus( hWnd ) ;
#endif

   DestroyEmulator( hWnd ) ;
   DestroyConnector( hWnd ) ;

   // restore old cursor

   SetCursor( hOldCursor ) ;

   DestroySCB( hSCB ) ;

   return ( TRUE ) ;

} /* end of DestroySession() */

/************************************************************************
 *  int CreateConnector( HWND hWnd, LPSTR lpSessionName )
 *
 *  Description:
 *     Attempts to open the given connector associated with the
 *     session name.
 *
 *  Comments:
 *
 ************************************************************************/

int CreateConnector( HWND hWnd, LPSTR lpSessionName )
{
   char    szConnector[ MAXLEN_STRLEN ], szSection[ MAXLEN_STRLEN ],
           szKey[ MAXLEN_STRLEN ] ;
   HANDLE  hInstance ;
   HSCB    hSCB ;
   NPSCB   npSCB ;

   // Get and store instance handle locally

   hInstance = GETHINST( hWnd ) ;

   // Get connector list from profile

   if (LoadString( hInstance, IDS_INI_ENTRYCONNECTOR, szKey, MAXLEN_STRLEN ))
   {
//      LoadString( hInstance, IDS_CON_DEFAULT,
//                  szDefault, MAXLEN_STRLEN ) ;
      GetPrivateProfileString( lpSessionName, szKey, "STDCOMM",
                               szConnector, MAXLEN_STRLEN, PHONE_FILE ) ;
   }
   else
   {
      DP1( hWDB, "Fatal error - unable to load IDS_INI_ENTRYCONNECTOR" ) ;
      return ( ERR_LIBERR ) ;
   }

   // Get SCB from Window structure and lock it

   hSCB = GETSCB( hWnd ) ;
   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
      return ( ERR_LIBERR ) ;

   // load library and get function pointers

   CONNECTORLIB = OpenLibrary( szConnector, CONNECTOR_EXT ) ;
   if (NULL == CONNECTORLIB)
   {
      LocalUnlock( hSCB ) ;
      return ( ERR_LIBERR ) ;
   }

   GetConnectorFunctionPointers( npSCB ) ;

   // create control block

   if (NULL == (SESSIONCCB = CreateCCB( hWnd )))
   {
      CloseLibrary( CONNECTORLIB, (FARPROC) LPGETCONNECTORUSECOUNT ) ;
      CONNECTORLIB = NULL ;
      LocalUnlock( hSCB ) ;
      return ( ERR_NOCB ) ;
   }

   // load settings

   wsprintf( szSection, "%s,%s", lpSessionName, (LPSTR) szConnector ) ;
   if (!LoadConnectorSettings( SESSIONCCB, szSection, PHONE_FILE ))
   {
      DestroyCCB( SESSIONCCB ) ;
      CloseLibrary( CONNECTORLIB, (FARPROC) LPGETCONNECTORUSECOUNT ) ;
      CONNECTORLIB = NULL ;
      LocalUnlock( hSCB ) ;
      return ( ERR_PARAMS ) ;
   }

   // open connector (notification when completed)

   if (!OpenConnector( SESSIONCCB ))
   {
      DestroyCCB( SESSIONCCB ) ;
      CloseLibrary( CONNECTORLIB, (FARPROC) LPGETCONNECTORUSECOUNT ) ;
      CONNECTORLIB = NULL ;
      LocalUnlock( hSCB ) ;
      return ( ERR_LIBERR ) ;
   }
   lstrcpy( CONNECTORNAME, szConnector ) ;
   LocalUnlock( hSCB ) ;

   return ( ERR_SUCCESS ) ;

} /* end of CreateConnector() */

/************************************************************************
 *  BOOL DestroyConnector( HWND hWnd )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL DestroyConnector( HWND hWnd )
{
   HSCB   hSCB ;
   NPSCB  npSCB ;

   // Get SCB from Window structure
   hSCB = GETSCB( hWnd ) ;

   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
      return ( FALSE ) ;

   CloseConnector( SESSIONCCB ) ;
   DestroyCCB( SESSIONCCB ) ;
   CloseLibrary( CONNECTORLIB, (FARPROC) LPGETCONNECTORUSECOUNT ) ;
   CONNECTORLIB = NULL ;

   LocalUnlock( hSCB ) ;
   return ( TRUE ) ;

} /* end of DestroyConnector() */

/************************************************************************
 *  VOID GetConnectorFunctionPointers( NPSCB npSCB )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

VOID GetConnectorFunctionPointers( NPSCB npSCB )
{
   LPGETCONNECTORUSECOUNT =
      (LPFNGETUSECOUNT)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_GETCONNECTORUSECOUNT ) ) ;
   LPCREATECCB =
      (LPFNCREATECCB)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_CREATECCB ) ) ;
   LPDESTROYCCB =
      (LPFNDESTROYCCB)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_DESTROYCCB ) ) ;

   LPLOADCONNECTORSETTINGS =
      (LPFNLOADCONNECTORSETTINGS)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_LOADCONNECTORSETTINGS ) ) ;

   LPSAVECONNECTORSETTINGS =
      (LPFNSAVECONNECTORSETTINGS)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_SAVECONNECTORSETTINGS ) ) ;

   LPDOCONNECTORSETUPDLG =
      (LPFNDOCONNECTORSETUPDLG)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_DOCONNECTORSETUPDLG ) ) ;

   LPOPENCONNECTOR =
      (LPFNOPENCONNECTOR)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_OPENCONNECTOR ) ) ;
   LPCLOSECONNECTOR =
      (LPFNCLOSECONNECTOR)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_CLOSECONNECTOR ) ) ;
   LPREADCONNECTORBLOCK =
      (LPFNREADCONNECTORBLOCK)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_READCONNECTORBLOCK ) ) ;
   LPWRITECONNECTORBYTE =
      (LPFNWRITECONNECTORBYTE)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_WRITECONNECTORBYTE ) ) ;
   LPWRITECONNECTORBLOCK =
      (LPFNWRITECONNECTORBLOCK)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_WRITECONNECTORBLOCK ) ) ;
   LPWRITECONNECTORFORMAT =
      (LPFNWRITECONNECTORFORMAT)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_WRITECONNECTORFORMAT ) ) ;
   LPESCAPECONNECTORFUNCTION =
      (LPFNESCAPECONNECTORFUNCTION)
         GetProcAddress( CONNECTORLIB,
                         MAKEINTRESOURCE( ORDINAL_ESCAPECONNECTORFUNCTION ) ) ;

} /* end of GetConnectorFunctionPointers() */

/************************************************************************
 *  int CreateEmulator( HWND hWnd, LPSTR lpSessionName )
 *
 *  Description:
 *     Opens the emulator associated with the given session.
 *
 *  Comments:
 *      6/25/91  baw  Wrote this comment block.
 *      6/25/91  baw  Implemented new INI format.
 *
 ************************************************************************/

int CreateEmulator( HWND hWnd, LPSTR lpSessionName )
{
   char    szEmulator[ MAXLEN_STRLEN ],
           szKey[ MAXLEN_STRLEN ], szSection[ MAXLEN_STRLEN ] ;
   HANDLE  hInstance ;
   HSCB    hSCB ;
   NPSCB   npSCB ;

   // Get and store instance handle locally

   hInstance = GETHINST( hWnd ) ;

   // Get connector list from profile

   if (LoadString( hInstance, IDS_INI_ENTRYEMULATOR, szKey, MAXLEN_STRLEN ))
   {
//      LoadString( hInstance, IDS_EMU_DEFAULT,
//                  szDefault, MAXLEN_STRLEN ) ;
      GetPrivateProfileString( lpSessionName, szKey, "DECVT220",
                               szEmulator, MAXLEN_STRLEN, PHONE_FILE ) ;
   }
   else
   {
      DP1( hWDB, "Fatal error - unable to load IDS_INIT_ENTRYEMULATOR" ) ;
      return ( ERR_LIBERR ) ;
   }

   // Get SCB from Window structure and lock it

   hSCB = GETSCB( hWnd ) ;
   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
      // should define a different error
      return ( ERR_LIBERR ) ;

   // open library

   EMULATORLIB = OpenLibrary( szEmulator, EMULATOR_EXT ) ;
   if (NULL == EMULATORLIB)
   {
      LocalUnlock( hSCB ) ;
      return ( ERR_LIBERR ) ;
   }

   GetEmulatorFunctionPointers( npSCB ) ;

   // create emulator window

   if (NULL == (EMULATORWND = CreateWindow( szEmulatorClass, NULL,
                                            WS_HSCROLL | WS_VSCROLL |
                                            WS_CHILD,
                                            0, 0, 0, 0, hWnd,
                                            NULL, EMULATORLIB, NULL)))
   {
      CloseLibrary( EMULATORLIB, (FARPROC) LPGETEMULATORUSECOUNT ) ;
      EMULATORLIB = NULL ;
      LocalUnlock( hSCB ) ;
      return ( ERR_CANTOPEN ) ;
   }

   // load settings

   wsprintf( szSection, "%s,%s", (LPSTR) lpSessionName, (LPSTR) szEmulator ) ;
   LoadEmulatorSettings( EMULATORWND, szSection, PHONE_FILE ) ;

   // clean up

   lstrcpy( EMULATORNAME, szEmulator ) ;
   ShowWindow( EMULATORWND, SW_SHOWNORMAL ) ;
   UpdateWindow( EMULATORWND ) ;
   LocalUnlock( hSCB ) ;
   return ( ERR_SUCCESS ) ;

} /* end of CreateEmulator() */

/************************************************************************
 *  BOOL DestroyEmulator( HWND hWnd, HSCB hSCB )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL DestroyEmulator( HWND hWnd )
{
   HSCB   hSCB ;
   NPSCB  npSCB ;

   // Get SCB from Window structure
   hSCB = GETSCB( hWnd ) ;

   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
      return ( FALSE ) ;

   // destroy emulator window
   DestroyWindow( EMULATORWND ) ;

   CloseLibrary( EMULATORLIB, (FARPROC) LPGETEMULATORUSECOUNT ) ;
   EMULATORLIB = NULL ;

   LocalUnlock( hSCB ) ;
   return ( TRUE ) ;

} /* end of DestroyEmulator() */

/************************************************************************
 *  VOID GetEmulatorFunctionPointers( NPSCB npSCB )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

VOID GetEmulatorFunctionPointers( NPSCB npSCB )
{
   LPGETEMULATORUSECOUNT =
      (LPFNGETUSECOUNT)
         GetProcAddress( EMULATORLIB,
                         MAKEINTRESOURCE( ORDINAL_GETEMULATORUSECOUNT ) ) ;

   LPLOADEMULATORSETTINGS =
      (LPFNLOADEMULATORSETTINGS)
         GetProcAddress( EMULATORLIB,
                         MAKEINTRESOURCE( ORDINAL_LOADEMULATORSETTINGS ) ) ;

   LPSAVEEMULATORSETTINGS =
      (LPFNSAVEEMULATORSETTINGS)
         GetProcAddress( EMULATORLIB,
                         MAKEINTRESOURCE( ORDINAL_SAVEEMULATORSETTINGS ) ) ;

   LPDOEMULATORSETUPDLG =
      (LPFNDOEMULATORSETUPDLG)
         GetProcAddress( EMULATORLIB,
                         MAKEINTRESOURCE( ORDINAL_DOEMULATORSETUPDLG ) ) ;

} /* end of GetEmulatorFunctionPointers() */

//------------------------------------------------------------------------
//  int OpenProtocol( HINSTANCE hInstance, HSCB hSCB )
//
//  Description:
//
//
//  Parameters:
//
//  History:   Date       Author      Comment
//              3/10/92   BryanW      Hacked together.
//
//------------------------------------------------------------------------

int OpenProtocol( HINSTANCE hInstance, HSCB hSCB )
{
   char    szProtocol[ MAXLEN_STRLEN ], szSection[ MAXLEN_STRLEN ],
           szKey[ MAXLEN_STRLEN ] ;
   NPSCB   npSCB ;

   // Get protocol setting from profile

   wsprintf( szSection, "Entry%04d", SESSIONENTRY ) ;
   if (LoadString( hInstance, IDS_INI_ENTRYPROTOCOL, szKey, MAXLEN_STRLEN ))
   {
//      LoadString( hInstance, IDS_CON_DEFAULT,
//                  szDefault, MAXLEN_STRLEN ) ;
      GetPrivateProfileString( szSection, szKey, "ZMODEM",
                               szProtocol, MAXLEN_STRLEN, PHONE_FILE ) ;
   }
   else
   {
      DP1( hWDB, "Fatal error - unable to load IDS_INI_ENTRYPROTOCOL" ) ;
      return ( ERR_LIBERR ) ;
   }

   // Get SCB from Window structure and lock it

   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
      return ( ERR_LIBERR ) ;

   // load library and get function pointers

   PROTOCOLLIB = OpenLibrary( szProtocol, PROTOCOL_EXT ) ;
   if (NULL == PROTOCOLLIB)
   {
      LocalUnlock( hSCB ) ;
      return ( ERR_LIBERR ) ;
   }

   GetProtocolFunctionPointers( npSCB ) ;

   LocalUnlock( hSCB ) ;
   return ( ERR_SUCCESS ) ;

} // end of OpenProtocol()

//------------------------------------------------------------------------
//  BOOL CloseProtocol( HSCB hSCB )
//
//  Description:
//
//
//  Parameters:
//
//  History:   Date       Author      Comment
//              3/10/92   BryanW      Hacked together.
//
//------------------------------------------------------------------------

BOOL CloseProtocol( HSCB hSCB )
{
   NPSCB  npSCB ;

   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
      return ( FALSE ) ;

   CloseLibrary( PROTOCOLLIB, (FARPROC) LPGETPROTOCOLUSECOUNT ) ;
   PROTOCOLLIB = NULL ;

   LocalUnlock( hSCB ) ;
   return ( TRUE ) ;

} // end of CloseProtocol()

//------------------------------------------------------------------------
//  VOID GetProtocolFunctionPointers( NPSCB npSCB )
//
//  Description:
//
//
//  Parameters:
//
//  History:   Date       Author      Comment
//
//------------------------------------------------------------------------

VOID GetProtocolFunctionPointers( NPSCB npSCB )
{
   LPGETPROTOCOLUSECOUNT =
      (LPFNGETUSECOUNT)
         GetProcAddress( PROTOCOLLIB,
                         MAKEINTRESOURCE( ORDINAL_GETPROTOCOLUSECOUNT ) ) ;

   LPPROTOCOLRECEIVE =
      (LPFNPROTOCOLRECEIVE)
         GetProcAddress( PROTOCOLLIB,
                         MAKEINTRESOURCE( ORDINAL_PROTOCOLRECEIVE ) ) ;

   LPPROTOCOLSEND =
      (LPFNPROTOCOLSEND)
         GetProcAddress( PROTOCOLLIB,
                         MAKEINTRESOURCE( ORDINAL_PROTOCOLSEND ) ) ;

} // end of GetProtocolFunctionPointers()

/************************************************************************
 *  BOOL ProcessTerminalMessage( HWND hWnd, WORD wMsg,
 *                               WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL ProcessTerminalMessage( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam )
{
   BOOL   fRetVal = TRUE ;
   HSCB   hSCB ;
   NPSCB  npSCB ;

   // Get SCB from Window structure

   if (NULL == (hSCB = GETSCB( hWnd )))
      return ( FALSE ) ;

   npSCB = (NPSCB) LocalLock( hSCB ) ;

   switch (wMsg)
   {
      case WM_SETFOCUS:
         SetFocus( EMULATORWND ) ;
         break ;

      case WM_CMD_RXBLK:
         break ;

      case WM_CMD_TXBLK:
         fRetVal = WriteConnectorBlock( SESSIONCCB,
                                        (LPSTR) lParam, (int) wParam ) ;
         break ;

      case WM_DROPFILES:
      {
         char   szFileName[ MAXLEN_PATH + MAXLEN_FILENAME ] ;
         HDROP  hDrop ;
         UINT   uPos, uFiles ;

         // NEED: Support multiple file drops

         hDrop = (HDROP) wParam ;
         if (ERR_SUCCESS != OpenProtocol( GETHINST( hWnd), hSCB ))
            MessageBox( hWnd, "Unable to load protocol.",
                        "Fatal Error!", MB_ICONEXCLAMATION | MB_OK ) ;
         else
         {
            uFiles = DragQueryFile( hDrop, 0xFFFF, NULL, NULL ) ;
            for (uPos = 0; uPos < uFiles; uPos++)
            {
               DragQueryFile( hDrop, uPos, szFileName, sizeof( szFileName ) ) ;
               HPROTOCOLDLG = ProtocolSend( hWnd, szFileName ) ;
               if (HPROTOCOLDLG)
                   LinkDialog( HPROTOCOLDLG ) ;

            }
         }
         DragFinish( hDrop ) ;
      }
      break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDM_SETUPCONNECTOR:
            {
               char   szSection[ MAXLEN_STRLEN ] ;

               wsprintf( szSection, "Entry%04d,%s", SESSIONENTRY,
                         (LPSTR) CONNECTORNAME ) ;
               fRetVal = DoConnectorSetupDlg( SESSIONCCB, hWnd,
                                              szSection, PHONE_FILE ) ;
            }
            break ;

            case IDM_SETUPEMULATOR:
            {
               char  szSection[ MAXLEN_STRLEN ] ;

               wsprintf( szSection, "Entry%04d,%s", SESSIONENTRY,
                         (LPSTR) EMULATORNAME ) ;
               fRetVal = DoEmulatorSetupDlg( hWnd, EMULATORWND, szSection,
                                             PHONE_FILE ) ;
            }
            break ;

            case IDM_DOWNLOAD:
            {
               if (HPROTOCOLDLG)
               {
                  MessageBeep( 0 ) ;
                  return ( FALSE ) ;
               }

               if (ERR_SUCCESS != OpenProtocol( GETHINST( hWnd), hSCB ))
                  MessageBox( hWnd, "Unable to load protocol.",
                              "Fatal Error!", MB_ICONEXCLAMATION | MB_OK ) ;
               else
               {
                  HPROTOCOLDLG = ProtocolReceive( hWnd, NULL ) ;
                  if (HPROTOCOLDLG)
                     LinkDialog( HPROTOCOLDLG ) ;
               }
               break ;
            }
            break ;

            case IDM_UPLOAD:
            {
               OPENFILENAME  ofn ;
               char          szFile[ MAXLEN_PATH + MAXLEN_FILENAME ] ;

               if (HPROTOCOLDLG)
               {
                  MessageBeep( 0 ) ;
                  return ( FALSE ) ;
               }

               if (ERR_SUCCESS != OpenProtocol( GETHINST( hWnd), hSCB ))
                  MessageBox( hWnd, "Unable to load protocol.",
                              "Fatal Error!", MB_ICONEXCLAMATION | MB_OK ) ;
               else
               {
                  // Set all structure members to zero.

                  _fmemset( &ofn, 0, sizeof( OPENFILENAME ) ) ;

                  *szFile = NULL ;
                  ofn.lStructSize = sizeof( OPENFILENAME ) ;
                  ofn.hwndOwner =   hWnd ;
                  ofn.lpstrFile=    szFile ;
                  ofn.nMaxFile =    sizeof( szFile ) ;
                  ofn.lpstrFilter = "All Files\0*.*\0ZIP files\0*.zip\0\0" ;
                  ofn.nFilterIndex = 2 ;
                  ofn.lpstrTitle =  "Select file to upload..." ;
                  ofn.Flags =       OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
                                    OFN_HIDEREADONLY ;

                  if (GetOpenFileName( &ofn ))
                  {
                     HPROTOCOLDLG = ProtocolSend( hWnd, ofn.lpstrFile ) ;
                     if (HPROTOCOLDLG)
                        LinkDialog( HPROTOCOLDLG ) ;
                  }
               }
            }
            break ;
         }
   }
   LocalUnlock( hSCB ) ;

   return ( fRetVal ) ;

} /* end of ProcessTerminalMessage() */

/************************************************************************
 *  BOOL TranslateNotification( HWND hWnd, WORD wMsg,
 *                              WORD wParam, LONG lParam )
 *
 *  Description:
 *
 *     This function will translate all external events for the
 *     session.
 *
 *     Examples:
 *        Connector events - Disconnect, RXChar, etc.
 *        Script events
 *        Protocol events
 *
 *  Comments:
 *
 ************************************************************************/

BOOL TranslateNotification( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam )
{
   HSCB   hSCB ;
   NPSCB  npSCB ;
   BOOL   fRetVal ;

   // Get SCB from Window structure

   hSCB = GETSCB( hWnd ) ;

   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
      return ( FALSE ) ;

   fRetVal = TRUE ;  // optimistic approach

   switch (wMsg)
   {
      case WM_CONNECTOR_NOTIFY:
         switch (LOWORD( lParam ))
         {
            case CN_RECEIVE:
            case CN_EVENT:
            {
               int   nBlkLen ;
               BYTE  abTemp[ MAXLEN_INBLOCK + 1 ] ;
               MSG   msg ;

               do
               {
                  if (nBlkLen = ReadConnectorBlock( SESSIONCCB,
                                                    MAXLEN_INBLOCK,
                                                    abTemp))
                  {
                     abTemp[ nBlkLen ] = NULL ;

                     // If protocol is active, send notification
                     // to protocol, otherwise send it to the
                     // emulator

                     if (HPROTOCOLDLG)
                     {
                        if (IsWindow( HPROTOCOLDLG ))
                           SendMessage( HPROTOCOLDLG, WM_CMD_RXBLK,
                                        (WORD) nBlkLen,
                                        (LONG) (LPSTR) abTemp ) ;
                        else
                           HPROTOCOLDLG = NULL ;
                     }
                     else
                        SendMessage( EMULATORWND, WM_CMD_TXBLK,
                                     (WORD) nBlkLen,
                                     (LONG) (LPSTR) abTemp ) ;
                  }
               }
               while (!PeekMessage( &msg, NULL, NULL, NULL, PM_NOREMOVE ) ) ;

#ifdef NOT_USED
               // There's an important message in the queue... so get
               // out of here. Oh yeah, if there is still incoming
               // data, make sure we get back here.

               if (nBlkLen != 0)
               {
                  if (!PeekMessage( &msg, NULL,
                                    WM_CONNECTOR_NOTIFY,
                                    WM_CONNECTOR_NOTIFY,
                                    PM_NOREMOVE | PM_NOYIELD ))
                     PostMessage( hWnd, WM_CONNECTOR_NOTIFY,
                                  wParam, lParam ) ;
               }
#endif

               fRetVal = TRUE ;
            }
            break ;
         }
         break ;
   }

   LocalUnlock( hSCB ) ;
   return ( fRetVal ) ;

} /* end of TranslateEvent() */

/************************************************************************
 * End of File: session.c
 ************************************************************************/
