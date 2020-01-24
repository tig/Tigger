#ifndef LPSTR
#include <windows.h>
#endif
#include <stdarg.h>
#include <string.h>

#include "dprintf.h"

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

/** EOF: dp.c **/

