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
 *               for saving the emulator settings to
 *               a private .INI file.
 *
 *   Revisions:
 *     01.00.000  6/25/91 baw   Wrote it.
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "ibmpc.h"

/************************************************************************
 *  BOOL FAR PASCAL SaveEmulatorSettings( HECB hECB,
 *                                        LPSTR lpSection, LPSTR lpFile )
 *
 *  Description:
 *     Saves emulator information to private profile given in
 *     the file <lpFile> and the application area <lpSection>.
 *
 *  Comments:
 *      6/25/91  baw  Wrote this comment block.
 *      7/ 7/91  baw  Modified for IBMPC.
 *
 ************************************************************************/

BOOL FAR PASCAL SaveEmulatorSettings( HECB hECB, LPSTR lpSection,
                                      LPSTR lpFile )
{
   char         szKey[ MAXLEN_TEMPSTR ] ;
   NPECB        npECB ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   // save stuff

   if (LoadString( hInstLib, IDS_SETTINGS_LOCALECHO, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileTrueFalse( lpSection, szKey, LOCALECHO, lpFile ) ;
   if (LoadString( hInstLib, IDS_SETTINGS_NEWLINE, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileTrueFalse( lpSection, szKey, NEWLINE, lpFile ) ;
   if (LoadString( hInstLib, IDS_SETTINGS_LINEWRAP, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileTrueFalse( lpSection, szKey, AUTOWRAP, lpFile ) ;
   WriteFonts( lpSection, &NORMALFONT, &HIGHLIGHTFONT, lpFile ) ;

   LocalUnlock( hECB ) ;

   return ( TRUE ) ;

} /* end of SaveEmulatorSettings() */

/************************************************************************
 *  BOOL WriteFonts( LPSTR lpSection, LPEXTLOGFONT lplfN,
 *                   LPEXTLOGFONT lplfH, LPSTR lpFile )
 *
 *  Description:
 *     Writes the font settings <lplfN> and <lplfH> to the
 *     <lpSection> of <lpFile>.
 *
 *  Comments:
 *      7/ 7/91  baw  Borrowed from LEXIS (modified)
 *
 ************************************************************************/

BOOL WriteFonts( LPSTR lpSection, LPEXTLOGFONT lplfN, LPEXTLOGFONT lplfH,
                 LPSTR lpFile )
{
   char szKey[ MAXLEN_TEMPSTR ] ;

   if (LoadString( hInstLib, IDS_SETTINGS_NFACENAME, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileString( lpSection, szKey, lplfN -> lfFaceName,
                                 lpFile ) ;

   if (LoadString( hInstLib, IDS_SETTINGS_NFACESIZEX, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, lplfN -> lfWidth, lpFile ) ;

   if (LoadString( hInstLib, IDS_SETTINGS_NFACESIZEY, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, lplfN -> lfHeight, lpFile ) ;

   if (LoadString( hInstLib, IDS_SETTINGS_NFGCOLOR, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileDW( lpSection, szKey, lplfN -> lfFGcolor, lpFile ) ;

   if (LoadString( hInstLib, IDS_SETTINGS_NBGCOLOR, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileDW( lpSection, szKey, lplfN -> lfBGcolor, lpFile ) ;

   if (LoadString( hInstLib, IDS_SETTINGS_HFACENAME, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileString( lpSection, szKey, lplfH -> lfFaceName,
                                 lpFile ) ;

   if (LoadString( hInstLib, IDS_SETTINGS_HFACESIZEX, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, lplfH -> lfWidth, lpFile ) ;

   if (LoadString( hInstLib, IDS_SETTINGS_HFACESIZEY, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileWord( lpSection, szKey, lplfH -> lfHeight, lpFile ) ;

   if (LoadString( hInstLib, IDS_SETTINGS_HFGCOLOR, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileDW( lpSection, szKey, lplfH -> lfFGcolor, lpFile ) ;

   if (LoadString( hInstLib, IDS_SETTINGS_HBGCOLOR, szKey, MAXLEN_TEMPSTR ))
      WritePrivateProfileDW( lpSection, szKey, lplfH -> lfBGcolor, lpFile ) ;

   return ( TRUE ) ;

} /* end of WriteFont() */

/************************************************************************
 * End of File: save.c
 ************************************************************************/

