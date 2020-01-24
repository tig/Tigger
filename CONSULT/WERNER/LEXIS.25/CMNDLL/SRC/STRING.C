/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CMNDLL.DLL, Common DLL for Werner Apps
 *
 *      Module:  string.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:
 *
 *    This module contains various string handling functions.  Some of
 *    them are "new and neato" and some of them are simply not found
 *    in the windows libaries.
 *
 *    Related to strings are the memory functions.  This module also
 *    contains several "l" memory copy, cmm, and set functions.
 *
 *   Revisions:  
 *     00.00.000  1/14/91 cek   First version.
 *
 *
 ************************************************************************/

#define WINDLL
#include <windows.h>
#include <ctype.h>
#include "..\inc\cmndll.h"

/************************************************************************
 * Imported variables
 ************************************************************************/

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
 * window class names
 ************************************************************************/


/*************************************************************************
 *  int FAR PASCAL AtoI(const LPSTR lpszBuf)
 *
 *  Description:
 *    Converts a zero terminated character string to an integer value.
 *    The function behaves exactly like the C runtime function AtoI,
 *    the only difference being the far pointer.
 *
 *  Type/Parameter
 *          Description
 *
 *    LPSTR lpszBuf
 *
 *          String to be converted
 * 
 *  Return Value
 *    Returns an int value produced by interpreting the input chars as
 *    a number.
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL AtoI(const LPSTR lpszBuf)
{
   int n = 0;
   LPSTR lpszCur = lpszBuf ;
   BOOL  fNeg ;

   while (isspace(*lpszCur))
      lpszCur++ ;

   if (!(isdigit(*lpszCur) || *lpszCur == '-' || *lpszCur == '+'))
      return 0 ;

   fNeg = FALSE ;

   if (*lpszCur == '-')
   {
      lpszCur++ ;
      fNeg = TRUE ;
   }

   if (*lpszCur == '+')
      lpszCur++ ;

   while (isdigit(*lpszCur))
   {
       n *= 10 ;
       n += *lpszCur - '0' ;
       lpszCur++ ;
   }
   return n * (fNeg ? -1 : 1) ;
}/* AtoI() */

/*************************************************************************
 *  DWORD FAR PASCAL AtoL(const LPSTR lpszBuf)
 *
 *  Description:
 *    Converts a zero terminated character string to an integer value.
 *    The function behaves exactly like the C runtime function AtoL,
 *    the only difference being the far pointer.
 *
 *  Type/Parameter
 *          Description
 *
 *    LPSTR lpszBuf
 *
 *          String to be converted
 * 
 *  Return Value
 *    Returns an DWORD value produced by interpreting the input chars as
 *    a number.
 *
 *  Comments:
 *
 *************************************************************************/
DWORD FAR PASCAL AtoL(const LPSTR lpszBuf)
{
   DWORD n = 0;
   LPSTR lpszCur = lpszBuf ;
   BOOL  fNeg ;

   while (isspace(*lpszCur))
      lpszCur++ ;

   if (!(isdigit(*lpszCur) || *lpszCur == '-' || *lpszCur == '+'))
      return 0 ;

   fNeg = FALSE ;

   if (*lpszCur == '-')
   {
      lpszCur++ ;
      fNeg = TRUE ;
   }

   if (*lpszCur == '+')
      lpszCur++ ;

   while (isdigit(*lpszCur))
   {
       n *= 10 ;
       n += *lpszCur - '0' ;
       lpszCur++ ;
   }
   return n * (fNeg ? -1 : 1) ;
}/* AtoL() */





/************************************************************************
 * End of File: string.c
 ************************************************************************/

