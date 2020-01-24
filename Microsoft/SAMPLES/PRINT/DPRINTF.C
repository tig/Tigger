#ifndef LPSTR
#include <windows.h>
#endif
#include <stdarg.h>
#include <string.h>


#include "dprintf.h"
// Formatted printf-style OutputDebugString

int dprintf( LPSTR lpszFormat, ... )
{
   char  szText [512] ;
   int   cbText ;
   va_list  marker ;
   
   
   va_start ( marker, lpszFormat ) ;
   cbText = wvsprintf( (LPSTR)szText, (LPSTR)lpszFormat,
                        (LPSTR) marker ) ;
   va_end( marker ) ;

   if (szText [cbText - 1] == '\n')
   {
      szText [cbText++] = '\r' ;
      szText [cbText] = '\0' ;
   }

   OutputDebugString( szText ) ;

   return cbText ;
}


/** dp.c -- code stolen from CharlKin 8-) **/


//#include <stdarg.h>
//#include <string.h>

/* Formatted printf-style OutputDebugString */
int dp( LPSTR lpszFormat, ... )
{
   char     szText[512];
   int      cbText;
   va_list  marker;
   
   
   va_start( marker, lpszFormat );

   cbText = wvsprintf( (LPSTR)szText, (LPSTR)lpszFormat, (LPSTR)marker );

   va_end( marker ) ;

//   if ( szText[ cbText - 1 ] == '\n' )
   {
      szText[ cbText++ ] = '\n';
      szText[ cbText++ ] = '\r';
      szText[ cbText ] = '\0';
   }

   OutputDebugString( szText );

   return ( cbText );
} /* dp() */

/** EOF: dp.c **/

