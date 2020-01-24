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

#include "precomp.h"     // precompiled stuff

#include "stdcomm.h"

/************************************************************************
 *  BOOL FAR PASCAL SaveConnectorSettings( HCCB hCCB, LPSTR lpSection,
 *                                         LPSTR lpFile )
 *
 *  Description:
 *     Saves connector information to private profile given in
 *     the file <lpFile> and the application area <lpSection>.
 *
 *  Comments:
 *      6/25/91  baw  Wrote this comment block.
 *
 ************************************************************************/

BOOL FAR PASCAL SaveConnectorSettings( HCCB hCCB, LPSTR lpSection,
                                       LPSTR lpFile )
{
   char   szKey[ MAXLEN_TEMPSTR ] ;
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   // save stuff

   if (LoadString( hLibInst, IDS_SETTINGS_PORT, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, SETTINGS( npCCB ).bPort,
                               lpFile ) ;
   if (LoadString( hLibInst, IDS_SETTINGS_BAUDRATE, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, SETTINGS( npCCB ).wBaudRate,
                               lpFile ) ;
   if (LoadString( hLibInst, IDS_SETTINGS_DATABITS, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, SETTINGS( npCCB ).bByteSize,
                               lpFile ) ;
   if (LoadString( hLibInst, IDS_SETTINGS_PARITY, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, SETTINGS( npCCB ).bParity,
                               lpFile ) ;
   if (LoadString( hLibInst, IDS_SETTINGS_STOPBITS, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, SETTINGS( npCCB ).bStopBits,
                               lpFile ) ;
   if (LoadString( hLibInst, IDS_SETTINGS_BREAKLENGTH, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, SETTINGS( npCCB ).wBreakLength,
                               lpFile ) ;
   if (LoadString( hLibInst, IDS_SETTINGS_FLOWCONTROL, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, SETTINGS( npCCB ).bFlowControl,
                               lpFile ) ;
   LocalUnlock( hCCB ) ;

   return ( TRUE ) ;

} /* end of SaveConnectorSettings() */

/************************************************************************
 * End of File: save.c
 ************************************************************************/

