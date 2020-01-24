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
 *     01.00.001  7/ 4/91 baw   Stolen from STDCOMM.
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "ibmpc.h"

/************************************************************************
 *  BOOL FAR PASCAL DoEmulatorSetupDlg( HWND hParentWnd, HWND hEmulatorWnd,
 *                                      LPSTR lpSection, LPSTR lpFile )
 *
 *  Description:
 *     This function calls up the setup dialog box, massages the data,
 *     sets the emulator based on the new settings and and writes the
 *     settings (if requested) to the given .INI file.
 *
 *  Comments:
 *      6/24/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL FAR PASCAL DoEmulatorSetupDlg( HWND hParentWnd, HWND hEmulatorWnd,
                                    LPSTR lpSection, LPSTR lpFile )
{
   BOOL  fRetVal ;
   HECB  hECB ;

   // get ECB if it exists, otherwise
   // create a temporary one

   if (NULL == hEmulatorWnd)
   {
      if (NULL == (hECB = CreateECB()))
      {
         DP1( hWDB, "Fatal error - could not create ECB!" ) ;
         return ( FALSE ) ;
      }
      InitECB( hECB ) ;
   }
   else
      hECB = GETECB( hEmulatorWnd ) ;

   fRetVal = (TRUE == DialogBoxParam( hInstLib,
                                      MAKEINTRESOURCE( SETTINGSDLGBOX ),
                                      hParentWnd, SettingsDlgProc,
                                      (DWORD) MAKELONG( hECB, NULL ) )) ;
   if (fRetVal)
   {
      if (fRetVal && (lpSection != NULL && lpFile != NULL))
         fRetVal = SaveEmulatorSettings( hECB, lpSection, lpFile ) ;
   }

   if (NULL != hEmulatorWnd)
   {
      // install new fonts, clear and force a repaint 

      DestroyEmulatorFonts( hEmulatorWnd, hECB ) ;
      CreateEmulatorFonts( hEmulatorWnd, hECB ) ;
      InvalidateRect( hEmulatorWnd, NULL, FALSE ) ;
      UpdateWindow( hEmulatorWnd ) ;
   }
   else
   {
      // if we created a temporary ECB, destroy it
      DestroyECB( hECB ) ;
   }

   return ( fRetVal ) ;
   
} /* end of DoEmulatorSetupDlg() */

/************************************************************************
 * End of File: setup.c
 ************************************************************************/

