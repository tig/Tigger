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
 *               for loading the emulator settings from
 *               a private .INI file.
 *
 *   Revisions:
 *     01.00.000  6/25/91 baw   Wrote it.
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "ibmpc.h"

/************************************************************************
 *  BOOL FAR PASCAL LoadEmulatorSettings( HWND hEmulatorWnd,
 *                                        LPSTR lpSection, LPSTR lpFile )
 *
 *  Description:
 *     Loads emulator information from private profile given in
 *     the file <lpFile> and the application area <lpSection>.
 *
 *  Comments:
 *      6/25/91  baw  Wrote this comment block.
 *      7/ 7/91  baw  Modified for IBMPC.
 *
 ************************************************************************/

BOOL FAR PASCAL LoadEmulatorSettings( HWND hEmulatorWnd, LPSTR lpSection,
                                      LPSTR lpFile )
{
   char         szKey[ MAXLEN_TEMPSTR ], szDefault[ MAXLEN_TEMPSTR ] ;
   LOCALHANDLE  hECB ;
   NPECB        npECB ;
   
   // get ECB associated with emulator window

   hECB = GETECB( hEmulatorWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   if (LoadString( hInstLib, IDS_SETTINGS_LOCALECHO, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_LOCALECHO, szDefault, MAXLEN_TEMPSTR ) ;
      LOCALECHO =
         GetPrivateProfileTrueFalse( lpSection, szKey, szDefault, lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_NEWLINE, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_NEWLINE, szDefault, MAXLEN_TEMPSTR ) ;
      NEWLINE =
         GetPrivateProfileTrueFalse( lpSection, szKey, szDefault, lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_LINEWRAP, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_LINEWRAP, szDefault, MAXLEN_TEMPSTR ) ;
      AUTOWRAP =
         GetPrivateProfileTrueFalse( lpSection, szKey, szDefault, lpFile) ;
   }
   GetFonts( lpSection, &NORMALFONT, &HIGHLIGHTFONT, lpFile ) ;

   LocalUnlock( hECB ) ;

   // update fonts

   DestroyEmulatorFonts( hEmulatorWnd, hECB ) ;
   CreateEmulatorFonts( hEmulatorWnd, hECB ) ;

   return ( TRUE ) ;

} /* end of LoadEmulatorSettings() */

/************************************************************************
 *  BOOL GetFonts( LPSTR lpSection, LPEXTLOGFONT lplfN,
 *                 LPEXTLOGFONT lplfH, LPSTR lpFile )
 *
 *  Description:
 *     Gets the font settings <lplfN> and <lplfH> from the
 *     <lpSection> of <lpFile>.
 *
 *  Comments:
 *      7/ 7/91  baw  Borrowed from LEXIS (modified)
 *
 ************************************************************************/

BOOL GetFonts( LPSTR lpSection, LPEXTLOGFONT lplfN,
               LPEXTLOGFONT lplfH, LPSTR lpFile )
{
   char szKey[ MAXLEN_TEMPSTR ], szDefault[ MAXLEN_TEMPSTR ] ;

   if (LoadString( hInstLib, IDS_SETTINGS_NFACENAME, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_NFACENAME, szDefault, MAXLEN_TEMPSTR ) ;
      GetPrivateProfileString( lpSection, szKey, szDefault,
                               lplfN -> lfFaceName, MAXLEN_TEMPSTR, lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_NFACESIZEX, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_NFACESIZEX, szDefault, MAXLEN_TEMPSTR ) ;
      lplfN -> lfWidth = GetPrivateProfileWord( lpSection, szKey, szDefault,
                                                lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_NFACESIZEY, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_NFACESIZEY, szDefault, MAXLEN_TEMPSTR ) ;
      lplfN -> lfHeight = GetPrivateProfileWord( lpSection, szKey, szDefault,
                                                 lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_NFGCOLOR, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_NFGCOLOR, szDefault, MAXLEN_TEMPSTR ) ;
      lplfN -> lfFGcolor = GetPrivateProfileDW( lpSection, szKey, szDefault,
                                                lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_NBGCOLOR, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_NBGCOLOR, szDefault, MAXLEN_TEMPSTR ) ;
      lplfN -> lfBGcolor = GetPrivateProfileDW( lpSection, szKey, szDefault,
                                                lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_HFACENAME, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_HFACENAME, szDefault, MAXLEN_TEMPSTR ) ;
      GetPrivateProfileString( lpSection, szKey, szDefault,
                               lplfH -> lfFaceName, MAXLEN_TEMPSTR, lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_HFACESIZEX, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_HFACESIZEX, szDefault, MAXLEN_TEMPSTR ) ;
      lplfH -> lfWidth = GetPrivateProfileWord( lpSection, szKey, szDefault,
                                                lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_HFACESIZEY, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_HFACESIZEY, szDefault, MAXLEN_TEMPSTR ) ;
      lplfH -> lfHeight = GetPrivateProfileWord( lpSection, szKey, szDefault,
                                                 lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_HFGCOLOR, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_HFGCOLOR, szDefault, MAXLEN_TEMPSTR ) ;
      lplfH -> lfFGcolor = GetPrivateProfileDW( lpSection, szKey, szDefault,
                                                lpFile ) ;
   }
   if (LoadString( hInstLib, IDS_SETTINGS_HBGCOLOR, szKey, MAXLEN_TEMPSTR ))
   {
      LoadString( hInstLib, IDS_DEFAULT_HBGCOLOR, szDefault, MAXLEN_TEMPSTR ) ;
      lplfH -> lfBGcolor = GetPrivateProfileDW( lpSection, szKey, szDefault,
                                                lpFile ) ;
   }

   return ( TRUE ) ;

} /* end of GetFonts() */

/************************************************************************
 * End of File: load.c
 ************************************************************************/
