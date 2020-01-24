/************************************************************************
 *
 *    Copyright (C) 1991 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  ini.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Miscellaneous INI file stuff
 *
 *   Revisions:
 *     01.00.000  6/25/91 baw   Wrote this comment block.
 *
 ************************************************************************/

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"
#include "ini.h"

/************************************************************************
 *  VOID FlushINIFile( LPSTR lpFile )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ************************************************************************/

VOID FlushINIFile( LPSTR lpFile )
{
   // Force Windows to release the cached .INI file.
   // This is a feature of Windows!

   WritePrivateProfileString( NULL, NULL, NULL, lpFile ) ;

} /* end of FlushINIFile() */

/************************************************************************
 * End of File: ini.c
 ************************************************************************/

