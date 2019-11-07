/************************************************************************
 *
 *     Project:  DBEDIT
 *
 *      Module:  fileio.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "dbedit.h"
#include <string.h>
#include <dos.h>              // dos_findfirst/dos_findnext
#include <sys\types.h>
#include <sys\stat.h>         // time shit
#include <errno.h>
#include <direct.h>           // getcwd,chdir,_chdrive
#include "config.h"
#include "fileio.h"
#include "listbox.h"

BOOL NEAR PASCAL PutFileInList( HLIST hlstFiles, LPSTR lpszFile ) ;

LPSTR NEAR PASCAL FindEndOfLine( LPSTR lpLine, LPSTR lpEOF ) ;

/****************************************************************
 *  HLIST WINAPI GetDirectoryList( HLIST hlstFiles, LPSTR lpszDir, LPSTR lpszSpec )
 *
 *  Description: 
 *
 *    This function adds the files found in the lpszDir that match
 *    the lpszSpec filespec to the list provided in hlstFiles.
 *
 *    If lpszDir is NULL, then the default WINDOWS directory is
 *    used.
 *
 *    If lpszSpec is NULL, then "*.INI" is used as the extension.
 *
 *    If the function was sucessful HLIST hlstFiles is returned.
 *    Otherwise it's NULL
 *
 *    The list that is possed in contains records of type
 *    FONTFILE:
 *
 *       DWORD    dwSize ;                // struct size
 *       LPSTR    lpszFile[MAX_PATH] ;   // fully qualified pathname
 *       WORD     wOffsetFile ;           // offset to filename.ext
 *       WORD     wTimeWrite ;            // Time of last write to file
 *       WORD     wDateWrite ;             // Date of last write to file
 *       DWORD    dwFileSize ;            // Length of file in bytes
 *
 *    Heck, while we're at it, if hlstFiles is NULL, this function
 *    will create the list.
 *
 *  Comments:
 *
 ****************************************************************/
HLIST WINAPI GetDirectoryList( HLIST hlst, LPSTR lpszDir, LPSTR lpszSpec )
{
   char           szDir[MAX_DIR] ;
   char           szSpec[MAX_PATH] ;
   char           szFile[MAX_PATH] ;

   struct find_t  FT ;

   if (hlst || (hlst = hlsqListCreate( hhLists )))
   {
      if (!lpszSpec)
         lpszSpec = rglpsz[IDS_DEF_SPEC] ;

      if (!lpszDir)
      {
         getcwd( szDir, MAX_PATH ) ;
         lpszDir = szDir ;
      }
      else
         lstrcpy( szDir, lpszDir ) ;

      if (szDir[lstrlen(szDir) - 1] != '\\')
          lstrcat( szDir, "\\" ) ;

      wsprintf( szSpec, "%s%s", (LPSTR)lpszDir, (LPSTR)lpszSpec ) ;

      /*
       * First get the current directory
       */
      if (!_dos_findfirst( szSpec, _A_NORMAL, &FT ))
         do
         {
            wsprintf( szFile, "%s%s", (LPSTR)szDir, (LPSTR)FT.name ) ;

            PutFileInList( hlst, szFile ) ;

         } while (!_dos_findnext( &FT )) ;

   }
   else
   {
      DP1( hWDB, "Could not create FONTFILE list" ) ;
      return NULL ;
   }

   return hlst ;

} /* GetDirectoryList()  */

/****************************************************************
 *  BOOL NEAR PASCAL PutFileInList( HLIST hlst, LPSTR lpszFile )
 *
 *  Description: 
 *
 *    Puts the file in the list (not the listbox).
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL PutFileInList( HLIST hlst, LPSTR lpszFile )
{
   char           szBuf[MAX_PATH] ;
   struct stat    Stat ;
   LPFONTFILE      lpFONT ;
   WORD           wOffsetFile ;
   WORD           wOffsetExt ;

   if (lpFONT = (LPFONTFILE)hlsqListAllocNode( hlst, NULL, sizeof(FONTFILE) ))
   {
      wOffsetFile = (WORD)(_fstrrchr( lpszFile, '\\' ) - lpszFile) ;
      if (wOffsetFile)
         wOffsetFile++ ;

      wOffsetExt = (WORD)(_fstrrchr( lpszFile, '.' )
         - lpszFile) ;
      if (wOffsetExt)
         wOffsetExt++ ;


      lpFONT->dwSize = sizeof( FONTFILE ) ;
      lpFONT->wMagic = FONTFILE_TAG ;
      lpFONT->wFlags = 0 ;

      lstrcpy( lpFONT->lpszFile, lpszFile ) ;

      lstrcpy( szBuf, lpFONT->lpszFile ) ;
      stat( szBuf, &Stat ) ;

      lpFONT->tTimeDate = Stat.st_atime ;

      lpFONT->dwFileSize = Stat.st_size ;

      wOffsetFile = (WORD)(_fstrrchr( lpszFile, '\\' ) - lpszFile) ;
      if (wOffsetFile)
         wOffsetFile++ ;

      lpFONT->wOffsetFile = wOffsetFile ;

      FONTFILE_lpszOutBuf(lpFONT) = NULL ;

      if (!hlsqListInsertSorted( hlst, lpFONT, lpfnFileCompare, 0L))
      {
         DP1( hWDB, "Could not add node in GetDirectoryList" ) ;
         hlsqListFreeNode( hlst, lpFONT ) ;
         return NULL ;
      }
   }
   else
   {
      DP1( hWDB, "Could not allocate node in GetDirectoryList" ) ;
      return NULL ;
   }

} /* PutFileInList()  */



/****************************************************************
 *  BOOL WINAPI ChangeDriveAndDir( LPSTR lpszNew, LPSTR lpszOld )
 *
 *  Description: 
 *
 *    This function changes the current drive and directory to
 *    the drive and directory specified in the string pointed to
 *    by lpszNew.  It returns the previous drive and directory
 *    in the string pointed to by lpszOld.
 *
 *    In both cases (lpszNew,lpszOld) the string is of the format:
 *
 *    d:\path\
 *
 *    The whack on the end of path is optional.  If just a drive is
 *    specified ("d:") just the current drive will be changed (but
 *    the entire previous path will be returned).
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI ChangeDriveAndDir( LPSTR lpszNew, LPSTR lpszOld )
{
   char szBuf[MAX_PATH] ;

   /*
    * get previous current dir
    */
   szBuf[0] = '\0' ;
   getcwd( szBuf, MAX_PATH ) ;
   lstrcpy( lpszOld, szBuf ) ;

   /*
    * Set drive if needed
    */
   if (lpszNew[1] == ':')
   {
      _chdrive( lpszNew[0] - 'A' ) ;
      lpszNew += 2 ;
   }

   /*
    * Change dir
    */
   lstrcpy( szBuf, lpszNew ) ;
   if (0 == chdir( szBuf ))
      return TRUE ;

   return FALSE ;

} /* ChangeDriveAndDir()  */



/************************************************************************
 * End of File: fileio.c
 ***********************************************************************/


