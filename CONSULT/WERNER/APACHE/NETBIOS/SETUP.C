/************************************************************************
 *
 *    Copyright (C) 1991 Werner Associates.  All Rights Reserved.
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
 *     Remarks:  Handles connector setup dialog box
 *
 *   Revisions:
 *     01.00.000  6/24/91 baw   Wrote it.
 *
 ************************************************************************/

#include "netbios.h"

/************************************************************************
 *  BOOL FAR PASCAL DoConnectorSetupDlg( LOCALHANDLE hCCB, HWND hWnd,
 *                                       LPSTR lpConnector, LPSTR lpFile )
 *
 *  Description:
 *     This function calls up the setup dialog box, massages the data,
 *     sets the connector based on the new settings and and writes the
 *     settings (if requested) to the given .INI file.
 *
 *  Comments:
 *      6/24/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL FAR PASCAL DoConnectorSetupDlg( LOCALHANDLE hCCB, HWND hWnd,
                                     LPSTR lpConnector, LPSTR lpFile )
{
   BOOL  fRetVal ;

   fRetVal = (TRUE == DialogBoxParam( hInstLib,
                                      MAKEINTRESOURCE( SETTINGSDLGBOX ),
                                      hWnd, SettingsDlgProc, (DWORD) hCCB )) ;

   if (fRetVal)
   {
      if (IsConnected( hCCB ))
         fRetVal = SetupConnector( hCCB ) ;
      if (fRetVal && (lpConnector != NULL && lpFile != NULL))
         fRetVal = SaveConnectorSettings( hCCB, lpConnector, lpFile ) ;
   }

   return ( fRetVal ) ;
   
} /* end of DoConnectorSetupDlg() */

/************************************************************************
 * End of File: setup.c
 ************************************************************************/

