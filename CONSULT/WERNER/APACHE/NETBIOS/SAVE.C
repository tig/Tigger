/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  save.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  This module contains the functions used
 *               for saving the connector settings to
 *               a private .INI file.
 *
 *   Revisions:
 *     01.00.000  6/25/91 baw   Wrote it.
 *
 ************************************************************************/

#include "netbios.h"

/************************************************************************
 *  BOOL FAR PASCAL SaveConnectorSettings( LOCALHANDLE hCCB,
 *                                         LPSTR lpSection, LPSTR lpFile )
 *
 *  Description:
 *     Saves connector information to private profile given in
 *     the file <lpFile> and the application area <lpSection>.
 *
 *  Comments:
 *      6/25/91  baw  Wrote this comment block.
 *
 ************************************************************************/

BOOL FAR PASCAL SaveConnectorSettings( LOCALHANDLE hCCB, LPSTR lpSection,
                                       LPSTR lpFile )
{
   char  szKey[ MAXLEN_TEMPSTR ] ;
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   // save stuff

   if (LoadString( hInstLib, IDS_SETTINGS_LOCALNAME, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileString( lpSection, szKey, npCCB -> achLocal, lpFile ) ;
   if (LoadString( hInstLib, IDS_SETTINGS_REMOTENAME, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileString( lpSection, szKey, npCCB -> achServer, lpFile ) ;
   if (LoadString( hInstLib, IDS_SETTINGS_SERVERMODE, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileTrueFalse( lpSection, szKey, npCCB -> fServerMode,
                                    lpFile ) ;
   if (LoadString( hInstLib, IDS_SETTINGS_BLANKPAD, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileTrueFalse( lpSection, szKey, npCCB -> fBlankPadding,
                                    lpFile ) ;
   if (LoadString( hInstLib, IDS_SETTINGS_RXTIMEOUT, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, npCCB -> wRxTimeout, lpFile ) ;
   if (LoadString( hInstLib, IDS_SETTINGS_TXTIMEOUT, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, npCCB -> wTxTimeout, lpFile ) ;
   if (LoadString( hInstLib, IDS_SETTINGS_EXTENSION, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, npCCB -> wExtension, lpFile ) ;

   LocalUnlock( hCCB ) ;

   return ( TRUE ) ;

} /* end of SaveConnectorSettings() */

/************************************************************************
 * End of File: save.c
 ************************************************************************/

