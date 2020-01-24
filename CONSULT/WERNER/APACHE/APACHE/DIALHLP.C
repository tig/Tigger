//---------------------------------------------------------------------------
//
//  Module: dialhlp.c
//
//  Purpose:
//     Provides dialing support for the "Dialing..." dialog.
//
//  Description of functions:
//     Contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              3/ 2/92   BryanW      Wrote it.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"
#include "dialog.h"
#include "session.h"

#include "dialhlp.h"

//------------------------------------------------------------------------
//  HSCB OpenPort( HWND hWnd, WORD wEntry )
//
//  Description:
//     Opens the port as specified by the entry in the .PHN
//     settings file.
//
//  Parameters:
//     HWND hWnd
//        handle to parent window
//
//     WORD wEntry
//        entry number in .PHN file
//
//  History:   Date       Author      Comment
//              3/ 2/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

HSCB OpenPort( HWND hWnd, WORD wEntry )
{
   char     szEntryName[ MAXLEN_TEMPSTR ] ; 
   HCURSOR  hOldCursor, hWaitCursor ;
   HSCB     hSCB ;

   // put up "wait" cursor

   hWaitCursor = LoadCursor( NULL, IDC_WAIT ) ;
   hOldCursor = SetCursor( hWaitCursor ) ;

   if (NULL == (hSCB = CreateSCB()))
      goto OP_Exit ;

   wsprintf( szEntryName, "Entry%04d", wEntry ) ;

   if (ERR_SUCCESS != CreateConnector_New( hWnd, hSCB, szEntryName ))
   {
      DestroySCB( hSCB ) ;
      hSCB = NULL ;
      goto OP_Exit ;
   }

OP_Exit:
   SetCursor( hOldCursor ) ;
   return ( hSCB ) ;

} // end of OpenPort()

//------------------------------------------------------------------------
//  VOID ClosePort( HSCB hSCB )
//
//  Description:
//     Closes port specified in the hSCB.
//
//  Parameters:
//     HSCB hSCB
//        handle to SCB
//
//  History:   Date       Author      Comment
//              3/ 2/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID ClosePort( HSCB hSCB )
{
   DestroyConnector_New( hSCB ) ;
   DestroySCB( hSCB ) ;

} // end of ClosePort()

//------------------------------------------------------------------------
//  BOOL CreateConnector_New( HWND hWnd, HSCB hSCB, LPSTR lpEntryName )
//
//  Description:
//     Attempts to create the connection specified in the
//     [lpEntryName] section of the .PHN file.
//
//  Parameters:
//     HWND hWnd
//        handle to associated window
//
//     HSCB hSCB
//        handle to SCB
//
//     LPSTR lpEntryName
//        pointer to entry name
//
//  History:   Date       Author      Comment
//              3/ 2/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL CreateConnector_New( HWND hWnd, HSCB hSCB, LPSTR lpEntryName )
{
   char    szConnector[ MAXLEN_STRLEN ], szSection[ MAXLEN_STRLEN ],
           szKey[ MAXLEN_STRLEN ] ;
   HANDLE  hInstance ;
   NPSCB   npSCB ;

   // Get and store instance handle locally

   hInstance = GETHINST( hWnd ) ;

   // Get connector from profile

   if (LoadString( hInstance, IDS_INI_ENTRYCONNECTOR, szKey, MAXLEN_STRLEN ))
   {
      // NEED: load connector default setting
//      LoadString( hInstance, IDS_CON_DEFAULT,
//                  szDefault, MAXLEN_STRLEN ) ;
      GetPrivateProfileString( lpEntryName, szKey, "STDCOMM",
                               szConnector, MAXLEN_STRLEN, PHONE_FILE ) ;
   }
   else
   {
      DP1( hWDB, "Fatal error - unable to load IDS_INI_ENTRYCONNECTOR" ) ;
      return ( ERR_LIBERR ) ;
   }

   // Lock SCB

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

   wsprintf( szSection, "%s,%s", lpEntryName, (LPSTR) szConnector ) ;
   if (!LoadConnectorSettings( SESSIONCCB, szSection, PHONE_FILE ))
   {
      DestroyCCB( SESSIONCCB ) ;
      CloseLibrary( CONNECTORLIB, (FARPROC) LPGETCONNECTORUSECOUNT ) ;
      CONNECTORLIB = NULL ;
      LocalUnlock( hSCB ) ;
      return ( ERR_PARAMS ) ;
   }

   // open connector

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

} // end of CreateConnector_New()

//------------------------------------------------------------------------
//  BOOL DestroyConnector_New( HSCB hSCB )
//
//  Description:
//     Destroys the connection specified in the hSCB.
//
//  Parameters:
//     HSCB hSCB
//        handle to SCB
//
//  History:   Date       Author      Comment
//              3/ 2/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL DestroyConnector_New( HSCB hSCB )
{
   NPSCB  npSCB ;

   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
      return ( FALSE ) ;

   CloseConnector( SESSIONCCB ) ;
   DestroyCCB( SESSIONCCB ) ;
   CloseLibrary( CONNECTORLIB, (FARPROC) LPGETCONNECTORUSECOUNT ) ;
   CONNECTORLIB = NULL ;

   LocalUnlock( hSCB ) ;
   return ( TRUE ) ;

} // end of DestroyConnector_New()

//------------------------------------------------------------------------
//  BOOL CreateEmulator_New( HWND hWnd, HSCB hSCB, WORD wEntry )
//
//  Description:
//     Creates / loads the emulator.
//
//  Parameters:
//     HWND hWnd
//        handle to associated window
//
//     HSCB hSCB
//        handle to SCB
//
//     WORD wEntry
//        entry slot in .PHN file
//
//  History:   Date       Author      Comment
//              3/ 4/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL CreateEmulator_New( HWND hWnd, HSCB hSCB, WORD wEntry )
{
   char             szEmulator[ MAXLEN_STRLEN ], szSection[ MAXLEN_STRLEN ],
                    szTemp[ MAXLEN_STRLEN ];
   HINSTANCE        hInstance ;
   HWND             hMDIChildWnd ;
   MDICREATESTRUCT  MDIcs ;
   NPSCB            npSCB ;
   RECT             rcClient ;

   // Get and store instance handle locally

   hInstance = GETHINST( hWnd ) ;

   // get session name

   GetPhoneINIString( hInstance, wEntry, IDS_INI_ENTRYNAME, szTemp,
                      sizeof( szTemp ) ) ;

   // create the MDI window

   MDIcs.szClass = szTerminalClass ;
   MDIcs.szTitle = szTemp ;
   MDIcs.hOwner =  hInstance ;
   MDIcs.x =       CW_USEDEFAULT ;
   MDIcs.y =       CW_USEDEFAULT ;
   MDIcs.cx =      CW_USEDEFAULT ;
   MDIcs.cy =      CW_USEDEFAULT ;
   MDIcs.style =   0 ;
   MDIcs.lParam =  NULL ;

   hMDIChildWnd =
      (HWND) LOWORD( SendMessage( hClientWnd, WM_MDICREATE, 0,
                                  (LPARAM) (LPMDICREATESTRUCT) &MDIcs ) ) ;
   if (!hMDIChildWnd)
      return ( FALSE ) ;

   // save SCB

   SetWindowWord( hMDIChildWnd, GWW_SCB, (WORD) hSCB ) ;

   // Get emulator from profile

   GetPhoneINIString( hInstance, wEntry, IDS_INI_ENTRYEMULATOR, szEmulator,
                      sizeof( szEmulator ) ) ;

   // Get SCB from Window structure and lock it

   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
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

   GetClientRect( hMDIChildWnd, &rcClient ) ;
   if (NULL == (EMULATORWND = CreateWindow( szEmulatorClass, NULL,
                                            WS_HSCROLL | WS_VSCROLL |
                                            WS_CHILD,
                                            CW_USEDEFAULT, CW_USEDEFAULT,
                                            CW_USEDEFAULT, CW_USEDEFAULT, 
                                            hMDIChildWnd,
                                            NULL, EMULATORLIB, NULL)))
   {
      CloseLibrary( EMULATORLIB, (FARPROC) LPGETEMULATORUSECOUNT ) ;
      EMULATORLIB = NULL ;
      LocalUnlock( hSCB ) ;
      return ( ERR_CANTOPEN ) ;
   }

   // load settings

   wsprintf( szSection, "Entry%04d,%s",  wEntry, (LPSTR) szEmulator ) ;
   LoadEmulatorSettings( EMULATORWND, szSection, PHONE_FILE ) ;

   // size window to force character size calculations, etc.

   MoveWindow( EMULATORWND, 0, 0, rcClient.right, rcClient.bottom, FALSE );

   // clean up

   SESSIONENTRY = wEntry ;
   lstrcpy( EMULATORNAME, szEmulator ) ;
   ShowWindow( EMULATORWND, SW_SHOWNORMAL ) ;
   UpdateWindow( EMULATORWND ) ;
   LocalUnlock( hSCB ) ;
   return ( ERR_SUCCESS ) ;

} // end of CreateEmulator_New()

//------------------------------------------------------------------------
//  BOOL GetPhoneINIString( HINSTANCE hInstance, WORD wEntry,
//                          int nIDString, LPSTR lpString, int nLen )
//
//  Description:
//     Retrieves the given string from the entry in the .PHN file.
//
//  Parameters:
//     HINSTANCE hInstance
//        handle to instance
//
//     WORD wEntry
//        entry number in .PHN file
//
//     int nIDString
//        resource ID for INI entry
//
//     LPSTR lpString
//        pointer to string buffer
//
//     int nLen
//        size of string buffer
//
//  History:   Date       Author      Comment
//              3/ 2/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL GetPhoneINIString( HINSTANCE hInstance, WORD wEntry, int nIDString,
                        LPSTR lpString, int nLen )
{
   char  szEntryName[ MAXLEN_TEMPSTR ], szKey[ MAXLEN_TEMPSTR ],
         szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ] ;

   GetPhoneINIPath( szINIFile ) ;
   wsprintf( szEntryName, "Entry%04d", wEntry ) ;

   LoadString( hInstance, nIDString, szKey, sizeof( szKey ) ) ;
   GetPrivateProfileString( szEntryName, szKey, "", lpString,
                            nLen, szINIFile ) ;

   return( TRUE ) ;

} // end of GetPhoneINIString()

//------------------------------------------------------------------------
//  VOID ConvertModemString( LPSTR lpOut, LPSTR lpIn )
//
//  Description:
//     Translates the given string by embedding the
//     control characters.
//
//  Parameters:
//     LPSTR lpOut
//        pointer to resultant string buffer
//
//     LPSTR lpIn
//        pointer to string buffer to convert
//
//  History:   Date       Author      Comment
//              3/ 2/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID ConvertModemString( LPSTR lpOut, LPSTR lpIn )
{
   int    i, j ;

   j = 0 ;
   for (i = 0; i < lstrlen( lpIn ); i++)
   {
      switch (lpIn[ i ])
      {
         case '^':
            // control code prefix
            lpOut[ j++ ] = lpIn[ ++i ] - 'A' + 1 ;
            break ;

         default:
            lpOut[ j++ ] = lpIn[ i ] ;
      }
   }
   lpOut[ j ] = NULL ;

} // end of ConvertModemString()

//------------------------------------------------------------------------
//  int WriteModemString( HSCB hSCB, LPSTR lpString )
//
//  Description:
//     Converts the string sequence into string with embedded
//     control sequences, etc and writes to the port specified
//     in the hSCB.
//
//  Parameters:
//     HSCB hSCB
//        handle to SCB
//
//     LPSTR lpString
//        pointer to string
//
//  History:   Date       Author      Comment
//              3/ 2/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

int WriteModemString( HSCB hSCB, LPSTR lpString )
{
   int    nRetVal ;
   char   szOut[ MAXLEN_TEMPSTR ] ;
   NPSCB  npSCB ;

   npSCB = (NPSCB) LocalLock( hSCB ) ;
   ConvertModemString( szOut, lpString ) ;
   nRetVal = WriteConnectorBlock( SESSIONCCB, szOut, lstrlen( szOut ) ) ;
   LocalUnlock( hSCB ) ;
   return ( nRetVal ) ;

} // end of WriteModemString()

//------------------------------------------------------------------------
//  int CheckModemResult( HWND hWnd, WORD wEntry, NPSTR npModemResult,
//                        PWORD pwResultPos )
//
//  Description:
//     Checks the result string with the defined modem result
//     strings for the selected modem.
//
//  Parameters:
//     HWND hWnd
//        handle to window
//
//     WORD wEntry
//        dialing entry
//
//     NPSTR npModemResult
//        actual result string read from modem
//
//     WORD wResultPos
//        result position
//
//     NPSTR npResult
//        returned result string
//
//  History:   Date       Author      Comment
//              3/ 2/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

int CheckModemResult( HWND hWnd, WORD wEntry, NPSTR npModemResult,
                      PWORD pwResultPos )
{
   BOOL   fMatch ;
   int    i, j, k, l ;
   char   szBuf[ MAXLEN_TEMPSTR ], szModemResult[ MAXLEN_TEMPSTR ] ;

   if (pwResultPos)
      *pwResultPos = 0 ;

   if (NULL == npModemResult || *npModemResult == NULL)
      return ( 0 ) ;

   DPF5( hWDB, "Result: %s\r\n", (LPSTR) npModemResult ) ;
   for (i = IDS_INI_COMMANDOK; i <= IDS_INI_BUSY; i++)
   {
      GetPhoneINIString( GETHINST( hWnd ), wEntry, i, szModemResult,
                         MAXLEN_TEMPSTR ) ;

      if (NULL != *szModemResult)
      {
         ConvertModemString( szBuf, szModemResult ) ;
         DPF5( hWDB, "ModemString: %s\r\n", (LPSTR) szModemResult ) ;
         for (k = 0; k < strlen( npModemResult ); k++)
         {
            if (*szBuf == npModemResult[ k ])
            {
               j = 0 ;
               l = k ;
               fMatch = TRUE ;
               while (fMatch &&
                      (l < strlen( npModemResult ) && j < strlen( szBuf )))
               {
                  l++ ;
                  j++ ;
                  if (szBuf[ j ] == '%')
                  {
                     DP3( hWDB, "Checking for digit or space\r\n" ) ;
                     if (isdigit( npModemResult[ l ] ) ||
                         npModemResult[ l ] == ' ')
                     {
                        j-- ;
                        fMatch = TRUE ;
                     }
                     else
                        fMatch = (szBuf[ ++j ] == npModemResult[ l ]) ;
                     DPF3( hWDB, "Result: %d\r\n", fMatch ) ;
                  }
                  else
                     fMatch = (szBuf[ j ] == npModemResult[ l ]) ;
               }
               if (j == strlen( szBuf ) ||
                   l == strlen( npModemResult ))
               {
                  // we got a full find or a partial find!
                  if (pwResultPos)
                     *pwResultPos = l ;
                  return ( (j == strlen( szBuf )) ? i : 0 ) ;
               }
            }
         }
         if (pwResultPos)
            *pwResultPos = k ;
      }
   }
   return ( 0 ) ;

} // end of CheckModemResult()

//---------------------------------------------------------------------------
//  End of File: dialhlp.c
//---------------------------------------------------------------------------

