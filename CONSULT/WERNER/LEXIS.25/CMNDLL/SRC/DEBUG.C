/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CMNDLL, Common DLL for Werner Apps
 *
 *      Module:  debug.c
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
#ifndef LPSTR
#include <windows.h>
#endif
#include <stdarg.h>
#include <string.h>

// Formatted printf-style OutputDebugString
int far cdecl dprintf( LPSTR lpszFormat, LPSTR lpszParams, ... )
{
   char  szText [512] ;
   int   cbText ;
   
   cbText = wvsprintf( (LPSTR)szText, (LPSTR)lpszFormat,
                        (LPSTR)&lpszParams ) ;

   if (szText [cbText - 1] == '\n')
   {
      szText [cbText++] = '\r' ;
      szText [cbText] = '\0' ;
   }

   OutputDebugString( szText ) ;

   return cbText ;
}

//---------------------------------------------------------------
// Formatted printf-style OutputDebugString
// With \n\r
//
int far cdecl dp( LPSTR lpszFormat, LPSTR lpszParams,... )
{
   char     szText[512];
   int      cbText;

   cbText = wvsprintf( (LPSTR)szText, (LPSTR)lpszFormat, (LPSTR)&lpszParams );

   szText[ cbText++ ] = '\n';
   szText[ cbText++ ] = '\r';
   szText[ cbText ] = '\0';

   OutputDebugString( szText );

   return ( cbText );
} /* dp() */

/************************************************************************
 * End of File: debug.c
 ************************************************************************/

