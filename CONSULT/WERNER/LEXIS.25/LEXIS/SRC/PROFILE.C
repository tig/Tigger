/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  profile.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This module contains functions that access the profile files
 *    related to lexis (i.e. LEXIS.INI).  The module provides
 *    helper functions that simplify the caller...
 *
 *    Ther is an Example LEXIS.INI file in the main directory
 *
 *   Revisions:  
 *     00.00.000  1/25/91 cek   First version
 *
 ************************************************************************/

#include <windows.h>

#include <cmndll.h>

#include "..\inc\lexis.h"
#include "..\inc\global.h"

#include "..\inc\profile.h"

/************************************************************************
 * Local Defines
 ************************************************************************/

/************************************************************************
 * Macros 
 ************************************************************************/

/************************************************************************
 * Local data structures
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/

/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * External Functions
 ************************************************************************/
/*************************************************************************
 *  LPSTR FAR PASCAL ProfileRead( LPSTR lpszSection,
 *                               LPSTR lpszKey,
 *                               LPSTR lpszDefault,
 *                               LPSTR lpszValue )
 *
 *  Description:
 *    This function reads a string from the lexis.ini file.
 *    lpszValue must point to a buffer MAX_STRLEN characters big.
 *
 *  Type/Parameter
 *          Description
 *
 *    LPSTR lpszSection
 *          Specifies the section name (i.e. [Modem]).  Note that
 *          the brackets are NOT to be specified.
 *
 *    LPSTR lpszKey
 *          Specifies the key (i.e. Baud =).
 *
 *    LPSTR lpszDefault
 *          Specifies the value to be used as the default.
 *
 *    LPSTR lpszValue
 *          Buffer that is to hold the resulting value
 *          
 * 
 *  Return Value
 *
 *    Returns a LPSTR to lpszValue.
 *
 *  Comments:
 *
 *************************************************************************/
LPSTR FAR PASCAL ProfileRead( LPSTR lpszSection,
                              LPSTR lpszKey,
                              LPSTR lpszDefault,
                              LPSTR lpszValue )
{
   char szFileName[13] ;

   LPLEXISSTATE lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals ) ;
   if (lpLS)
   {
      wsprintf( szFileName, "%s.INI", (LPSTR)lpLS->szAppName ) ;

      GetPrivateProfileString( lpszSection, lpszKey,
                               lpszDefault, lpszValue, MAX_STRLEN,
                               szFileName ) ;
   }
   else
      lstrcpy( lpszValue, lpszDefault ) ;

   GlobalUnlock( ghGlobals ) ;

   return lpszValue ;

}/* ProfileRead() */

/*************************************************************************
 *  BOOL FAR PASCAL ProfileReadTrueFalse( LPSTR lpszSection,
 *                                        LPSTR lpszKey,
 *                                        BOOL  fDefault )
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
 *  Type/Parameter
 *          Description
 * 
 *    LPSTR lpszSection
 *          Specifies the section name (i.e. [Modem]).  Note that
 *          the brackets are NOT to be specified.
 *
 *    LPSTR lpszKey
 *          Specifies the key (i.e. Baud =).
 *
 *    BOOL  fDefault
 *          Specifies the value to be used as the default (1 or 0).
 *
 *  Return Value
 *
 *    Returns TRUE if the key was true, FALSE if not.  
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL ProfileReadTrueFalse( LPSTR lpszSection,
                                      LPSTR lpszKey,
                                      BOOL  fDefault )
{
   char szBuf[32] ;
   char szBuf2[MAX_INILEN] ;

   wsprintf( szBuf, "%d", fDefault ) ;

   ProfileRead( lpszSection, lpszKey, szBuf,
                szBuf2 ) ;

   if (*szBuf2 == '1' ||
       !lstrcmpi( szBuf2, "true" ) ||
       !lstrcmpi( szBuf2, "on" ) ||
       !lstrcmpi( szBuf2, "yes" ))
      return TRUE ;
   else
      return FALSE ;    // this also covers fDefault

}/* ProfileReadTrueFalse() */
   
/*************************************************************************
 *   int FAR PASCAL ProfileReadInt( LPSTR lpszSection,
 *                                  LPSTR lpszKey,
 *                                  int   nDefault )
 *
 *  Description:
 *    Reads the specified profile string, returning an integer.
 *
 *  Type/Parameter
 *          Description
 * 
 *    LPSTR lpszSection
 *          Specifies the section name (i.e. [Modem]).  Note that
 *          the brackets are NOT to be specified.
 *
 *    LPSTR lpszKey
 *          Specifies the key (i.e. Baud =).
 *
 *    int   nDefault
 *          Specifies the value to be used as the default.
 *
 *  Return Value
 *
 *    Returns an the value of the profile string converted to an integer.  
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL ProfileReadInt( LPSTR lpszSection,
                               LPSTR lpszKey,
                               int   nDefault )
{
   char szBuf1[MAX_INILEN] ;
   char szBuf2[32] ;

   wsprintf( szBuf2, "%d", nDefault ) ;

   return AtoI( ProfileRead( lpszSection, lpszKey,
                             szBuf2, 
                             szBuf1 ) ) ;

}/* ProfileReadInt() */

DWORD FAR PASCAL ProfileReadDW( LPSTR  lpszSection,
                                LPSTR  lpszKey,
                                DWORD dwDefault )
{
   char szBuf1[MAX_INILEN] ;
   char szBuf2[32] ;

   wsprintf( szBuf2, "%lu", dwDefault ) ;

   return AtoL( ProfileRead( lpszSection, lpszKey,
                             szBuf2, 
                             szBuf1 ) ) ;

}/* ProfileReadDW() */

/*************************************************************************
 *   BOOL FAR PASCAL ProfileWrite( LPSTR lpszSection,
 *                                 LPSTR lpszKey,
 *                                 LPSTR lpszValue )
 *
 *  Description:
 *    This function writes a string to the lexis.ini file.
 *
 *  Type/Parameter
 *          Description
 *
 *    LPSTR lpszSection
 *          Specifies the section name (i.e. [Modem]).  Note that
 *          the brackets are NOT to be specified.
 *
 *    LPSTR lpszKey
 *          Specifies the key (i.e. Baud =).
 *
 *    LPSTR lpszValue
 *          String to write.
 *          
 *  Return Value
 *
 *    Returns a TRUE if successful, FALSE otherwise.
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL ProfileWrite( LPSTR lpszSection,
                             LPSTR lpszKey,
                             LPSTR lpszValue )
{
   char szFileName[13] ;

   LPLEXISSTATE lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals ) ;
   if (lpLS)
   {
      wsprintf( szFileName, "%s.INI", (LPSTR)lpLS->szAppName ) ;

      GlobalUnlock( ghGlobals ) ;
      return WritePrivateProfileString( lpszSection, lpszKey,
                                        lpszValue,
                                        szFileName ) ;
   }
   GlobalUnlock( ghGlobals ) ;

   return FALSE ;

}/* ProfileWrite() */


/*************************************************************************
 *  int FAR PASCAL ProfileWriteInt( LPSTR lpszSection,
 *                                  LPSTR lpszKey,
 *                                  int   nValue )
 *
 *  Description:
 *    Writes an integer to the specified profile section.
 *
 *  Type/Parameter
 *          Description
 *
 *    LPSTR lpszSection
 *          Specifies the section name (i.e. [Modem]).  Note that
 *          the brackets are NOT to be specified.
 *
 *    LPSTR lpszKey
 *          Specifies the key (i.e. Baud =).
 *
 *    LPSTR lpszValue
 *          String to write.
 *          
 *  Return Value
 *
 *    Returns a TRUE if successful, FALSE otherwise.
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL ProfileWriteInt( LPSTR lpszSection,
                                LPSTR lpszKey,
                                int   nValue )
{
   char szValue[32] ;

   wsprintf( szValue, "%d", nValue ) ;

   return ProfileWrite( lpszSection, lpszKey, szValue ) ;

}/* ProfileWriteInt() */

DWORD FAR PASCAL ProfileWriteDW( LPSTR lpszSection,
                                 LPSTR lpszKey,
                                 DWORD dwValue )
{
   char szValue[32] ;

   wsprintf( szValue, "%lu", dwValue ) ;

   return ProfileWrite( lpszSection, lpszKey, szValue ) ;

}/* ProfileWriteDW() */

/************************************************************************
 * Internal Functions
 ************************************************************************/


/************************************************************************
 * End of File: profile.c
 ************************************************************************/

