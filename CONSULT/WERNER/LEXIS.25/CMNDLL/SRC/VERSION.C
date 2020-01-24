/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CMNDLL, Common DLL for Werner Apps
 *
 *      Module:  version.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/

#define WINDLL
#include <windows.h>
#include "..\inc\cmndll.h"
#include "..\cmndll.ver"

//------------------------------------------------------------------
// LPSTR FAR PASCAL
// VerGetCMNDLLVersion( LPSTR lpszVersion ) 
//
// Returns the current version number in the form 00.00.00 in the
// string passed as lpszVersion.  A pointer to this string is
// also returned.
//
// NOTE:  The address passed must point to a string that is at
//        least 22 bytes long.
//
// Modifications
// 10/31/90    CEK   First version
//------------------------------------------------------------------
LPSTR FAR PASCAL
VerGetCMNDLLVersion( LPSTR lpszVersion ) 
{
   VerMakeVersionNumber( lpszVersion, VER_MAJOR, VER_MINOR, VER_BUILD ) ;

   DP( hWDB, "CMNDLL.DLL Version %s", (LPSTR)lpszVersion )  ;

   return lpszVersion ;
}


//------------------------------------------------------------------
//
// LPSTR FAR PASCAL
// VerMakeVersionNumber( LPSTR   lpszOut,
//                       WORD    wMajor,
//                       WORD    wMinor,
//                       WORD    wBeta )
//
// Returns a string in the form XX.YY.ZZZ where XX is the major version
// number.  YY is the minor, and ZZZ is the pre-release part.  
// If wBeta is zero the value returned is of the form 00.00
//
// Modifications
// 11/20/90 CEK   Build now has 3 digits.
//------------------------------------------------------------------
LPSTR FAR PASCAL
VerMakeVersionNumber( LPSTR   lpszOut,
                      WORD    wMajor,
                      WORD    wMinor,
                      WORD    wBeta )
{
   if ( wBeta )
      wsprintf( lpszOut, (LPSTR)"%d.%.2d.%.3d",
                wMajor, wMinor, wBeta ) ;
   else
      wsprintf( lpszOut, (LPSTR)"%d.%.2d", wMajor, wMinor ) ;

   return lpszOut ;
}


/************************************************************************
 * End of File: version.c
 ************************************************************************/

