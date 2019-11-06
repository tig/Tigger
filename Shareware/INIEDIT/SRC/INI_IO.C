/************************************************************************
 *
 *     Project:  INIEDIT Copyright 1991 Charles E. Kindel, Jr.
 *
 *      Module:  ini_io.c
 *
 *     Remarks:
 *
 *  File I/O routines
 *  These routines supplement the Get/WriteProfileString 
 *  API calls.
 *
 *  GetPrivateProfileSections 
 *     Gets _all_ section headers in a INI file
 *  DelPrivateProfileString
 *     Deletes a section/and or key line
 *  CleanUpPrivateProfile
 *     Optimizes an INI file by stripping all comments and unnecessary
 *     white space.
 *
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include <windows.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <io.h>
#include <errno.h>
#include <stdlib.h>
#include <ceklib.h>
#include "..\inc\iniedit.h"
#include "..\inc\ini_io.h"

extern   char     szSysDir [] ;

BOOL NEAR PASCAL ValidINIFile( int hFile ) ;

//---------------------------------------------------
// 
//   int GetPrivateProfileSections( LPSTR   lpszReturnedString,
//                                  LPSTR   lpszFileName )
//
// This function copies the names of all of the sections in the
// specified initilization file into the buffer pointed to by
// lpReturnedString.
//
// Written 8/20/90 by charlkin
// 
int GetPrivateProfileSections( LPSTR   lpszReturnedString,
                               LPSTR   lpszFileName )
{
   #define BUFFERSIZE 4096
   HANDLE   hMemory ;
   PSTR     pszMemory ;
   PSTR     psCur ;
   PSTR     psOpen, psClose ;
   LPSTR    lpszEnd ;
   char     szSect [INI_MAXSECTIONLEN * 2 + 2] ;
   WORD     wBytes ;
   int      hFile ;
   int      i ;
   char     szFileName[_MAX_PATH] ;

   //
   // Copy the filename in case we have to munge it
   //
   lstrcpy( szFileName, lpszFileName ) ;

   if (-1 == (hFile = _lopen( szFileName, OF_READ )))
   {
      wsprintf( szFileName, "%s\\%s", (LPSTR)szSysDir, (LPSTR)lpszFileName ) ;

      if (-1 == (hFile = _lopen( szFileName, OF_READ )))
      {
         wsprintf( lpszReturnedString,
                   (LPSTR)"Cannot open %s!", (LPSTR) szFileName ) ;
         return -1 ;
      }

   }

   if (!ValidINIFile( hFile ))
   {
      wsprintf( lpszReturnedString,
               "The file '%s' does not appear to be a valid Windows INI file.",
               (LPSTR)szFileName ) ;
      _lclose( hFile ) ;
      return -1 ;
   }

   if (!(hMemory = LocalAlloc( LHND, BUFFERSIZE )))
   {
      wsprintf( (LPSTR)lpszReturnedString,
                (LPSTR)"Not enough memory!(a)" ) ;
      _lclose( hFile ) ;
      return -1 ;
   }

   pszMemory = LocalLock ( hMemory ) ;

   *szSect = '\0' ;
   *lpszReturnedString = '\0' ;
   lpszEnd = lpszReturnedString ;

   while (-1 != (wBytes = _lread( hFile, (LPSTR)pszMemory, BUFFERSIZE - 1)))
   {
      D(dp("Read buffer, wBytes = %d", wBytes )) ;

      //
      // Empty file check
      //
      if (lpszEnd == lpszReturnedString && wBytes < 3)
      {
         wsprintf( lpszReturnedString, "The file '%s' is empty!", (LPSTR)szFileName ) ;
         _lclose( hFile ) ;
         LocalUnlock( hMemory ) ;
         LocalFree( hMemory ) ;
         return -1 ;
      }

      *lpszEnd = '\0' ;
      pszMemory [wBytes] = '\0' ;
      if (psCur = strtok( pszMemory, "\n" ))
         do
         {
            if (*szSect != '\0') // we have part of a previous line
               lstrcat( (LPSTR)szSect, (LPSTR)psCur ) ;
            else
               lstrcpy( (LPSTR)szSect, (LPSTR)psCur ) ;

            if (psOpen = IsSectionLine(szSect))
            {
               if (psClose = strchr( szSect, ']' ))
               {
                  psOpen++ ;
                  *psClose = '\0' ;

                  // add to list
                  lstrcpy( lpszEnd, psOpen ) ;
                  while (*(++lpszEnd) != NULL)
                     ;
                  ++lpszEnd ; 


                  *szSect = '\0' ;  // reset
               }
               else  // there was no closing "]"
               {
                  //D( dp( (LPSTR)"No closing bracket=%s", (LPSTR)psCur ) ) ;
                  i = lstrlen( (LPSTR)psCur ) ;          // length of half line
                  if ((LPSTR)(psCur + i) < (LPSTR)(pszMemory + wBytes))
                  {
                     wsprintf( (LPSTR)lpszReturnedString,
                              (LPSTR)"The file %s appears to be corrupted!\nOffending line: \"%s\"",
                              (LPSTR)szFileName,
                              (LPSTR)szSect ) ;
                     _lclose( hFile ) ;
                     LocalUnlock( hMemory ) ;
                     LocalFree( hMemory ) ;
                     return -1 ;
                  }
                  lstrcpy ( (LPSTR)szSect, (LPSTR)psCur ) ;                  
               }
            }
            else
               *szSect = '\0' ;

         } while (psCur = strtok( NULL, "\n" )) ;

      if (wBytes < BUFFERSIZE - 1)
         break ;
   }
   _lclose( hFile ) ;
   LocalUnlock( hMemory ) ;
   LocalFree( hMemory ) ;
   return TRUE ;
}


/****************************************************************
 *  BOOL NEAR PASCAL ValidINIFile( HFILE hFile )
 *
 *  Description: 
 *
 *    Simple minded thing to determine if it's a valid INI file
 *    or not.  Simply looks at the first 64 bytes of the file
 *    and the first '['.  If anything before the '[' is not white
 *    space then it's invalid!
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL ValidINIFile( int hFile )
{
   #define TEST_SIZE    64

   char  szBuf[TEST_SIZE] ;
   WORD  wBytes ;
   PSTR  pszBracket ;

   if (-1 != (wBytes = _lread( hFile, (LPSTR)szBuf, TEST_SIZE - 1)))
   {
      szBuf[wBytes] = '\0' ;

      // 
      // Find first '[' and then see if there's any thing before it
      //
      if (pszBracket = strchr( szBuf, '[' ))
      {
         *pszBracket = '\0' ;

         for (pszBracket = szBuf ; *pszBracket != '\0' ; pszBracket++)
         {
            if (!isspace(*pszBracket))
               return FALSE ;
         }
      }
      else
         return FALSE ;
   }
   else
      return FALSE ;

   // Go back to beginning of the file
   lseek( hFile, SEEK_SET, 0 ) ;

   return TRUE ;

} /* ValidINIFile()  */

// IsSectionLine( PSTR psLine )
//
//
PSTR IsSectionLine( PSTR psLine )
{
   PSTR   psCur = psLine ;

   if (psCur)
      do
      {
         if (*psCur != ' ' && *psCur != '\t')
         {
            if (*psCur == '[')
               return psCur ;
            else
               return NULL ;
         }
      } while (*(++psCur)) ;

   return NULL ;
}
         

//---------------------------------------------------
//
//    int DelPrivateProfileString( LPSTR lpszApplicationName,
//                                 LPSTR lpszKeyName,
//                                 LPSTR lpFilename )
//
// This function deletes an entry from the specified initialization
// file.  If lpszKeyName is NULL then the entire section specified
// by lpszApplicationName is deleted.  If lpszKeyName points to a
// null terminated string, then just that entry in the file is 
// deleted.
//
// Written 8/20/90 by charlkin
//

int DelPrivateProfileString( LPSTR lpszApplicationName,
                             LPSTR lpszKeyName,
                             LPSTR lpFilename )
{
   return TRUE ;
}

//-----------------------------------------------------
//
//    int CleanUpPrivateProfile( LPSTR lpszFileName )
//
// This function removes all comments and uneccessary whitespace
// from the specified initialization file.
//
// Written 8/20/90 by charlkin
//
int CleanUpPrivateProfile( LPSTR lpszFileName )
{
  return TRUE ;
}
/************************************************************************
 * End of File: ini_io.c
 ***********************************************************************/

