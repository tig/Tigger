/************************************************************************
 *
 *    Copyright (c) 1991 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  profile.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *     Remarks:  Profile manipulation functions
 *
 *   Revisions:
 *     01.00.000  7/ 3/91 baw   Pulled from LEXIS source and modified
 *
 ************************************************************************/

#include "netbios.h"

/*************************************************************************
 *  BOOL GetPrivateProfileTrueFalse( LPSTR lpSection, LPSTR lpKey,
 *                                   LPSTR lpDefault, LPSTR lpFile )
 *
 *  Description:
 *    Determines if a profile string has is set to true or false.
 *    Possible ways of specifing true and/or false are:
 *
 *       TRUE     FALSE
 *       "true"   "false"
 *       "1"      "0"
 *       "on"     "off"
 *       "yes"    "no"
 *
 *  Comments:
 *      7/ 3/91  baw  Pulled from Lexis and modified for NetBIOS.
 *
 *************************************************************************/

BOOL GetPrivateProfileTrueFalse( LPSTR lpSection, LPSTR lpKey, LPSTR lpDefault,
                                 LPSTR lpFile )
{
   char  szValue[ MAXLEN_TEMPSTR ] ;

   GetPrivateProfileString( lpSection, lpKey, lpDefault, szValue,
                            MAXLEN_TEMPSTR, lpFile ) ;
   if (*szValue == '1' ||
       !lstrcmpi( szValue, "true" ) ||
       !lstrcmpi( szValue, "on" ) ||
       !lstrcmpi( szValue, "yes" ))
      return TRUE ;
   else
      return FALSE ;

} /* end of GetPrivateProfileTrueFalse() */
   
/*************************************************************************
 *  WORD GetPrivateProfileWord( LPSTR lpSection, LPSTR lpKey,
 *                              LPSTR lpDefault, LPSTR lpFile )
 *
 *  Description:
 *     Gets the specified profile string, returning an integer.
 *
 *  Comments:
 *      7/ 3/91  baw  Pulled from Lexis and modified.   
 *
 *************************************************************************/

WORD GetPrivateProfileWord( LPSTR lpSection, LPSTR lpKey, LPSTR lpDefault,
                           LPSTR lpFile )
{
   char  szValue[ MAXLEN_TEMPSTR ] ;

   
   GetPrivateProfileString( lpSection, lpKey, lpDefault,
                            szValue, MAXLEN_TEMPSTR, lpFile ) ;
   return ( AtoI( szValue ) ) ;

} /* end of GetPrivateProfileWord() */

/************************************************************************
 *  DWORD GetPrivateProfileDW( LPSTR lpSection, LPSTR lpKey,
 *                             LPSTR szDefault, LPSTR lpFile )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

DWORD GetPrivateProfileDW( LPSTR lpSection, LPSTR lpKey, LPSTR lpDefault,
                           LPSTR lpFile )
{
   char  szValue[ MAXLEN_TEMPSTR ] ;

   GetPrivateProfileString( lpSection, lpKey, lpDefault,
                            szValue, MAXLEN_TEMPSTR, lpFile ) ;
   return ( AtoL( szValue ) ) ;

} /* end of GetPrivateProfileDW() */

/************************************************************************
 *  BOOL WritePrivateProfileTrueFalse( LPSTR lpSection, LPSTR lpKey,
 *                                     BOOL fFlag, LPSTR lpFile )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL WritePrivateProfileTrueFalse( LPSTR lpSection, LPSTR lpKey, BOOL fFlag,
                                   LPSTR lpFile )
{
   char szValue[ MAXLEN_TEMPSTR ] ;

   wsprintf( szValue, "%s", (LPSTR)((fFlag) ? "True" : "False") ) ;
   return WritePrivateProfileString( lpSection, lpKey, szValue, lpFile ) ;

} /* end of WritePrivateProfileTrueFalse() */

/*************************************************************************
 *  BOOL WritePrivateProfileWord( LPSTR lpSection, LPSTR lpKey,
 *                                WORD wValue, LPSTR lpFile )
 *
 *  Description:
 *    Writes an integer to the specified profile section.
 *
 *  Comments:
 *
 *************************************************************************/

BOOL WritePrivateProfileWord( LPSTR lpSection, LPSTR lpKey, WORD wValue,
                              LPSTR lpFile )
{
   char szValue[ MAXLEN_TEMPSTR ] ;

   wsprintf( szValue, "%u", wValue ) ;
   return WritePrivateProfileString( lpSection, lpKey, szValue, lpFile ) ;

} /* end of WritePrivateProfileWord() */

/************************************************************************
 *  BOOL WritePrivateProfileDW( LPSTR lpSection, LPSTR lpKey,
 *                              DWORD dwValue, LPSTR lpFile )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL WritePrivateProfileDW( LPSTR lpSection, LPSTR lpKey, DWORD dwValue,
                            LPSTR lpFile )
{
   char szValue[ MAXLEN_TEMPSTR ] ;

   wsprintf( szValue, "%lu", dwValue ) ;
   return WritePrivateProfileString( lpSection, lpKey, szValue, lpFile ) ;

} /* end of WritePrivateProfileDW() */

/************************************************************************
 * End of File: profile.c
 ************************************************************************/

