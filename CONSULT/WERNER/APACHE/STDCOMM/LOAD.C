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

#include "precomp.h"     // precompiled stuff

#include "stdcomm.h"

/************************************************************************
 *  BOOL FAR PASCAL LoadConnectorSettings( HCCB hCCB, LPSTR lpSection,
 *                                         LPSTR lpFile )
 *
 *  Description:
 *     Loads connector information from private profile given in
 *     the file <lpFile> and the application area <lpSection>.
 *
 *  Comments:
 *     6/25/91  baw  Wrote this comment block.
 *
 ************************************************************************/

BOOL FAR PASCAL LoadConnectorSettings( HCCB hCCB, LPSTR lpSection,
                                       LPSTR lpFile )
{
   char   szKey[ MAXLEN_TEMPSTR ], szDefault[ MAXLEN_TEMPSTR ] ;
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (LoadString( hLibInst, IDS_SETTINGS_PORT, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hLibInst, IDS_DEFAULT_PORT, szDefault, MAXLEN_TEMPSTR ) ;
      SETTINGS( npCCB ).bPort =
         LOBYTE( GetPrivateProfileWord( lpSection, szKey, szDefault, lpFile ) ) ;
   }
   if (LoadString( hLibInst, IDS_SETTINGS_BAUDRATE, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hLibInst, IDS_DEFAULT_BAUDRATE, szDefault, MAXLEN_TEMPSTR ) ;
      SETTINGS( npCCB ).wBaudRate =
         GetPrivateProfileWord( lpSection, szKey, szDefault, lpFile ) ;
   }
   if (LoadString( hLibInst, IDS_SETTINGS_DATABITS, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hLibInst, IDS_DEFAULT_DATABITS, szDefault, MAXLEN_TEMPSTR ) ;
      SETTINGS( npCCB ).bByteSize =
         LOBYTE( GetPrivateProfileWord( lpSection, szKey, szDefault, lpFile ) ) ;
   }
   if (LoadString( hLibInst, IDS_SETTINGS_PARITY, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hLibInst, IDS_DEFAULT_PARITY, szDefault, MAXLEN_TEMPSTR ) ;
      SETTINGS( npCCB ).bParity =
         LOBYTE( GetPrivateProfileWord( lpSection, szKey, szDefault, lpFile ) ) ;
   }
   if (LoadString( hLibInst, IDS_SETTINGS_BREAKLENGTH, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hLibInst, IDS_DEFAULT_BREAKLENGTH, szDefault, MAXLEN_TEMPSTR ) ;
      SETTINGS( npCCB ).wBreakLength =
         GetPrivateProfileWord( lpSection, szKey, szDefault, lpFile ) ;
   }
   if (LoadString( hLibInst, IDS_SETTINGS_FLOWCONTROL, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hLibInst, IDS_DEFAULT_FLOWCONTROL, szDefault, MAXLEN_TEMPSTR ) ;
      SETTINGS( npCCB ).bFlowControl =
         LOBYTE( GetPrivateProfileWord( lpSection, szKey, szDefault, lpFile ) ) ;
   }
   
   LocalUnlock( hCCB ) ;

   return ( TRUE ) ;

} /* end of LoadConnectorSettings() */

/************************************************************************
 * End of File: load.c
 ************************************************************************/
