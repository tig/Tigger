/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  load.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  This module contains the functions used
 *               for loading the connector settings from
 *               a private .INI file.
 *
 *   Revisions:
 *     01.00.000  6/25/91 baw   Wrote it.
 *
 ************************************************************************/

#include "netbios.h"

/************************************************************************
 *  BOOL FAR PASCAL LoadConnectorSettings( LOCALHANDLE hCCB, 
 *                                         LPSTR lpFile, LPSTR lpSection )
 *
 *  Description:
 *     Loads connector information from private profile given in
 *     the file <lpFile> and the application area <lpSection>.
 *
 *  Comments:
 *     6/25/91  baw  Wrote this comment block.
 *
 ************************************************************************/

BOOL FAR PASCAL LoadConnectorSettings( LOCALHANDLE hCCB, LPSTR lpSection,
                                       LPSTR lpFile )
{
   char   szKey[ MAXLEN_TEMPSTR ], szDefault[ MAXLEN_TEMPSTR ] ;
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (LoadString( hInstLib, IDS_SETTINGS_LOCALNAME, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_LOCALNAME, szDefault,
                  MAXLEN_NCBNAMESIZE ) ;
      GetPrivateProfileString( lpSection, szKey, szDefault,
                               npCCB -> achLocal, MAXLEN_NCBNAMESIZE,
                               lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_REMOTENAME, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_REMOTENAME, szDefault,
                  MAXLEN_NCBNAMESIZE ) ;
      GetPrivateProfileString( lpSection, szKey, szDefault,
                               npCCB -> achServer, MAXLEN_NCBNAMESIZE,
                               lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_SERVERMODE, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_SERVERMODE, szDefault, MAXLEN_TEMPSTR ) ;
      npCCB -> fServerMode = GetPrivateProfileTrueFalse( lpSection, szKey,
                                                         szDefault, lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_BLANKPAD, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_BLANKPAD, szDefault, MAXLEN_TEMPSTR ) ;
      npCCB -> fBlankPadding = GetPrivateProfileTrueFalse( lpSection, szKey,
                                                           szDefault, lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_RXTIMEOUT, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_RXTIMEOUT, szDefault, MAXLEN_TEMPSTR ) ;
      npCCB -> wRxTimeout = (WORD) GetPrivateProfileWord( lpSection, szKey,
                                                          szDefault, lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_TXTIMEOUT, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_TXTIMEOUT, szDefault, MAXLEN_TEMPSTR ) ;
      npCCB -> wTxTimeout = (WORD) GetPrivateProfileWord( lpSection, szKey,
                                                          szDefault, lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_EXTENSION, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_EXTENSION, szDefault, MAXLEN_TEMPSTR ) ;
      npCCB -> wExtension = (WORD) GetPrivateProfileWord( lpSection, szKey,
                                                          szDefault, lpFile ) ;
   }

   LocalUnlock( hCCB ) ;

   return ( TRUE ) ;

} /* end of LoadConnectorSettings() */

/************************************************************************
 * End of File: load.c
 ************************************************************************/
