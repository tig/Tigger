/*            auxwrite.c - routines for writing debugging info
                           to the AUX port in Windows.

*/

#include <io.h>
#include <stdio.h>
#include <stdarg.h>

#define AUX_HANDLE 3

void auxcls (void)
{
   int i ;
   
   for (i = 1 ; i <= 25 ; i++)
      write (AUX_HANDLE, "\n\r", 2) ;
}

void auxwrite (char *fmt, ...)
{
   int ival ;
   char *p, *sval ;
   double dval ;
   char cval ;
   char szText[132] ;
   va_list args ;

   va_start (args, fmt) ;
   szText [0] = '\0' ;
   for (p = fmt; *p ; p++)
   {
      if (*p != '%')
      {
         if (*p == '\n')
            sprintf (szText, "%s%c\r", szText, *p) ;
         else
            sprintf (szText, "%s%c", szText, *p) ;
         continue ;
      }
      switch (*++p)
      {
          case 'd' :
               ival = va_arg (args, int) ;
               sprintf (szText, "%s%d", szText, ival) ;
               break ;
     
          case 'x' :
                ival = va_arg (args, int) ;
                sprintf (szText, "%s%x", szText, ival) ;
                break ;

           case 'X' :
                ival = va_arg (args, int) ;
                sprintf (szText, "%s%X", szText, ival) ;
                break ;

          case 'c' :
               cval = va_arg (args, char) ;
               sprintf (szText, "%s%c", szText, cval) ;
               break ;

          case 'f' :
               dval = va_arg (args, double) ;
               sprintf (szText, "%s%f", szText, dval) ;
               break ;

          case 's' :
               sval = va_arg(args, char *) ; 
               sprintf (szText, "%s%s", szText, sval) ;
               break ;

          default:
               sprintf (szText, "%s%c", szText, *p) ;
               break ;
       }
   }
   va_end (args) ;
   write (AUX_HANDLE, szText, strlen (szText)) ;
}

/* end of file */

