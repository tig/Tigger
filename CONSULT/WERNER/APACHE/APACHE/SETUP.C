/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  setup.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  This module handles the calls from setup dialog
 *               boxes for connector or emulator description creation
 *
 *   Revisions:  
 *     01.00.000  6/27/91 baw   Wrote it
 *
 ************************************************************************/

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"
#include "session.h"
#include "setup.h"

/************************************************************************
 *  BOOL DlgConnectorSetup( HWND hDlg, LPSTR lpEntryName, int nIDComboBox )
 *
 *  Description: 
 *     Gets the module name from the selected item.  Creates a
 *     temporary session block to perform the setup function of
 *     the selected connector.
 *
 *  Comments:
 *      6/27/91  Wrote it.
 *
 ************************************************************************/

BOOL DlgConnectorSetup( HWND hDlg, LPSTR lpEntryName, int nIDComboBox )
{
   char         szModuleName[ MAXLEN_MODULENAME ],
                szSectionName[ MAXLEN_STRLEN ] ;
   ATOM         atomModuleName ;
   BOOL         fRetVal ;
   HCURSOR      hOldCursor, hWaitCursor ;
   LOCALHANDLE  hSCB ;
   NPSCB        npSCB ;
   WORD         wItemSel ;

   // inform user it could be a wait

   hWaitCursor = LoadCursor( NULL, IDC_WAIT ) ;
   hOldCursor = SetCursor( hWaitCursor ) ;

   wItemSel =
      LOWORD( SendDlgItemMessage( hDlg, nIDComboBox, CB_GETCURSEL,
                                  NULL, NULL ) ) ;
   atomModuleName =
      LOWORD( SendDlgItemMessage( hDlg, nIDComboBox, CB_GETITEMDATA,
                                  wItemSel, NULL ) ) ;
   GetAtomName( atomModuleName, szModuleName, MAXLEN_MODULENAME ) ;

   if (NULL == (hSCB = CreateSCB()))
   {
      SetCursor( hOldCursor ) ;
      return ( FALSE ) ;
   }

   // Lock the SCB for later use
   
   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
   {
      DP1( hWDB, "Fatal error - could not create SCB" ) ;
      DestroySCB( hSCB ) ;
      SetCursor( hOldCursor ) ;
      return ( FALSE ) ;
   }

   // load library and get function pointers

   CONNECTORLIB = OpenLibrary( szModuleName, CONNECTOR_EXT ) ;
   if (NULL == CONNECTORLIB)
   {
      DP1( hWDB, "Fatal error - could not open library (DLL)" ) ;
      LocalUnlock( hSCB ) ;
      DestroySCB( hSCB ) ;
      SetCursor( hOldCursor ) ;
      return ( FALSE ) ;
   }

   GetConnectorFunctionPointers( npSCB ) ;

   // create control block

   if (NULL == (SESSIONCCB = CreateCCB( hDlg )))
   {
      CloseLibrary( CONNECTORLIB, (FARPROC) LPGETCONNECTORUSECOUNT ) ;
      CONNECTORLIB = NULL ;
      LocalUnlock( hSCB ) ;
      DestroySCB( hSCB ) ;
      SetCursor( hOldCursor ) ;
      return ( FALSE ) ;
   }

   // perform setup function

   wsprintf( szSectionName, "%s,%s", (LPSTR) lpEntryName,
             (LPSTR) szModuleName ) ;
   LoadConnectorSettings( SESSIONCCB, szSectionName, PHONE_FILE ) ;
   fRetVal = DoConnectorSetupDlg( SESSIONCCB, hDlg, szSectionName,
                                  PHONE_FILE ) ;

#ifdef NOT_USED
   // if successful return, add to the connector dialog box in the
   // session selection dialog ( one level up ) and select it as 
   // the current selection

   if (fRetVal)
   {
      wItemSel = LOWORD( SendDlgItemMessage( GetParent( hDlg ),
                                             IDD_CONNECTORCB,
                                             CB_ADDSTRING, NULL,
                                             (LONG) (LPSTR) szConnectorName ) ) ;
      SendDlgItemMessage( GetParent( hDlg ), IDD_CONNECTORCB, CB_SETITEMDATA,
                          wItemSel, atomModuleName ) ;
      SendDlgItemMessage( GetParent( hDlg ), IDD_CONNECTORCB, CB_SETCURSEL,
                          wItemSel, NULL ) ;
   }
#endif

   // clean up

   DestroyCCB( SESSIONCCB ) ;
   CloseLibrary( CONNECTORLIB, (FARPROC) LPGETCONNECTORUSECOUNT ) ;
   LocalUnlock( hSCB ) ;
   DestroySCB( hSCB ) ;
   SetCursor( hOldCursor ) ;

   return ( fRetVal ) ;

} /* end of DlgConnectorSetup() */

/************************************************************************
 *  BOOL DlgEmulatorSetup( HWND hDlg, LPSTR lpEntryName,
 *                         int nIDComboBox )
 *
 *  Description: 
 *     Gets the module name from the selected item.  Creates a
 *     temporary session block to perform the setup function of
 *     the selected emulator.
 *
 *  Comments:
 *      6/27/91  Wrote it.
 *
 ************************************************************************/

BOOL DlgEmulatorSetup( HWND hDlg, LPSTR lpEntryName, int nIDComboBox )
{
   char         szModuleName[ MAXLEN_MODULENAME ], szSection[ MAXLEN_STRLEN ] ;
   ATOM         atomModuleName ;
   BOOL         fRetVal ;
   HCURSOR      hOldCursor, hWaitCursor ;
   LOCALHANDLE  hSCB ;
   NPSCB        npSCB ;
   WORD         wItemSel ;

   // inform user it could be a wait

   hWaitCursor = LoadCursor( NULL, IDC_WAIT ) ;
   hOldCursor = SetCursor( hWaitCursor ) ;

   wItemSel =
      LOWORD( SendDlgItemMessage( hDlg, nIDComboBox, CB_GETCURSEL,
                                  NULL, NULL ) ) ;
   atomModuleName =
      LOWORD( SendDlgItemMessage( hDlg, nIDComboBox, CB_GETITEMDATA,
                                  wItemSel, NULL ) ) ;
   GetAtomName( atomModuleName, szModuleName, MAXLEN_MODULENAME ) ;

   if (NULL == (hSCB = CreateSCB()))
   {
      SetCursor( hOldCursor ) ;
      return ( FALSE ) ;
   }

   // Lock the SCB for later use
   
   if (NULL == (npSCB = (NPSCB) LocalLock( hSCB )))
   {
      DP1( hWDB, "Fatal error - could not SCB" ) ;
      DestroySCB( hSCB ) ;
      SetCursor( hOldCursor ) ;
      return ( FALSE ) ;
   }

   // load library and get function pointers

   EMULATORLIB = OpenLibrary( szModuleName, EMULATOR_EXT ) ;
   if (NULL == EMULATORLIB)
   {
      DP1( hWDB, "Fatal error - could not open library (DLL)" ) ;
      LocalUnlock( hSCB ) ;
      DestroySCB( hSCB ) ;
      SetCursor( hOldCursor ) ;
      return ( FALSE ) ;
   }

   GetEmulatorFunctionPointers( npSCB ) ;

   // perform setup function

   wsprintf( szSection, "%s,%s", (LPSTR) lpEntryName,
             (LPSTR) szModuleName ) ;
   fRetVal = DoEmulatorSetupDlg( hDlg, NULL, szSection, PHONE_FILE ) ;

   // if successful return, add to the emulator dialog box in the
   // session selection dialog ( one level up ) and select it as 
   // the current selection

   if (fRetVal)
   {
      wItemSel = LOWORD( SendDlgItemMessage( GetParent( hDlg ),
                                             IDD_EMULATORCB,
                                             CB_ADDSTRING, NULL,
                                             (LONG) (LPSTR) szModuleName ) ) ;
      SendDlgItemMessage( GetParent( hDlg ), IDD_EMULATORCB, CB_SETITEMDATA,
                          wItemSel, atomModuleName ) ;
      SendDlgItemMessage( GetParent( hDlg ), IDD_EMULATORCB, CB_SETCURSEL,
                          wItemSel, NULL ) ;
   }

   // clean up

   CloseLibrary( EMULATORLIB, (FARPROC) LPGETEMULATORUSECOUNT ) ;
   LocalUnlock( hSCB ) ;
   DestroySCB( hSCB ) ;
   SetCursor( hOldCursor ) ;

   return ( fRetVal ) ;

} /* end of DlgEmulatorSetup() */

/************************************************************************
 * End of File: setup.c
 ************************************************************************/

