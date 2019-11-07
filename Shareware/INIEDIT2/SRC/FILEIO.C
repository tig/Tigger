/************************************************************************
 *
 *     Project:  INIedit 2.0
 *
 *      Module:  fileio.c
 *
 *     Remarks:  File i/o functions. 
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "iniedit.h"

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
 *    INIFILE:
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

   LPSTR          lpszFile ;
   LPSTR          lpszBuf ;

   struct find_t  FT ;
   WORD           wOffsetFile ;

   if (hlst || (hlst = hlsqListCreate( hhLists )))
   {
      if (!lpszSpec)
         lpszSpec = rglpsz[IDS_DEFEXT] ;

      if (!lpszDir)
      {
         GetWindowsDirectory( szDir, MAX_PATH ) ;
         lpszDir = szDir ;
      }
      else
         lstrcpy( szDir, lpszDir ) ;

      if (szDir[lstrlen(szDir) - 1] != '\\')
          lstrcat( szDir, "\\" ) ;

      wOffsetFile = lstrlen( lpszDir ) ;

      wsprintf( szSpec, "%s%s", (LPSTR)lpszDir, (LPSTR)lpszSpec ) ;

      /*
       * First get the WINDOWS directory
       */
      if (!_dos_findfirst( szSpec, _A_NORMAL, &FT ))
         do
         {
            wsprintf( szFile, "%s%s", (LPSTR)szDir, (LPSTR)FT.name ) ;

            PutFileInList( hlst, szFile ) ;

         } while (!_dos_findnext( &FT )) ;

      /*
       * Then add the files in the INIEDIT.INI [Other Files] section
       *
       */
      if (lpszBuf = ALLOCFROMHEAP( hhScratch, 1024 * 2 ))
      {
         if (GetPrivateProfileString( rglpsz[IDS_INI_OTHERFILES],
                                      NULL, "",
                                      lpszBuf, 1024 * 2 - 1,
                                      rglpsz[IDS_INI_FILENAME] ))
         {
            OFSTRUCT OF ;

            for (lpszFile = lpszBuf ;
                 *lpszFile != '\0' ;
                lpszFile += lstrlen(lpszFile) + 1)
            {
               if ((HFILE)-1 != OpenFile( lpszFile, &OF, OF_EXIST ))
                  PutFileInList( hlst, lpszFile ) ;
               else
                  KillOtherFile( lpszFile ) ;
            }
         }

         FREEFROMHEAP( hhScratch, lpszBuf ) ;
      }
      else
      {
         DP1( hWDB, "ALLOCFROMHEAP failed" ) ;
      }
      

   }
   else
   {
      DP1( hWDB, "Could not create INIFILE list" ) ;
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
   LPINIFILE      lpINI ;
   WORD           wOffsetFile ;
   WORD           wOffsetExt ;

   if (lpINI = (LPINIFILE)hlsqListAllocNode( hlst, NULL, sizeof(INIFILE) ))
   {
      wOffsetFile = (WORD)(_fstrrchr( lpszFile, '\\' ) - lpszFile) ;
      if (wOffsetFile)
         wOffsetFile++ ;

      wOffsetExt = (WORD)(_fstrrchr( lpszFile, '.' )
         - lpszFile) ;
      if (wOffsetExt)
         wOffsetExt++ ;


      lpINI->dwSize = sizeof( INIFILE ) ;
      lpINI->wMagic = INIFILE_TAG ;
      lpINI->wFlags = 0 ;

      if (!lstrcmp( lpszFile + wOffsetExt, rglpsz[IDS_DEFEXT]+2 ))
         lpINI->wIcon = ICON_INIFILE ;
      else
         lpINI->wIcon = ICON_DOSFILE ;

      lpINI->fVisible = FALSE ;

      lstrcpy( lpINI->lpszFile, lpszFile ) ;

      lstrcpy( szBuf, lpINI->lpszFile ) ;
      stat( szBuf, &Stat ) ;

      lpINI->tTimeDate = Stat.st_atime ;

      lpINI->dwFileSize = Stat.st_size ;

      wOffsetFile = (WORD)(_fstrrchr( lpszFile, '\\' ) - lpszFile) ;
      if (wOffsetFile)
         wOffsetFile++ ;

      lpINI->wOffsetFile = wOffsetFile ;

      INIFILE_lpszOutBuf(lpINI) = NULL ;
      INIFILE_hlstSections(lpINI) = NULL ;

      if (!hlsqListInsertSorted( hlst, lpINI, lpfnFileCompare, 0L))
      {
         DP1( hWDB, "Could not add node in GetDirectoryList" ) ;
         hlsqListFreeNode( hlst, lpINI ) ;
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


/****************************************************************
 *  BOOL WINAPI GetPrivateProfileSections( LPSTR lpOutBuf, WORD cbMax, LPSTR lpszFile )
 *
 *  Description: 
 *
 *    This function enumerates all of the profile sections in
 *    the given profile.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI GetPrivateProfileSections( LPSTR lpOutBuf, WORD cbMax, LPSTR lpszFile, WORD wFileSize )
{
   HFILE          hFile ;
   OFSTRUCT       OF ;

   GLOBALHANDLE   gh ;
   WORD           wBytes ;       /* number of bytes read from file */
   LPSTR          lpFileBuf ;
   LPSTR          lpCur ;        /* Pointers into the file buffer */
   LPSTR          lpEOL ;
   LPSTR          lpEOF ;
   LPSTR          lpLine ;
   BOOL           fInSec ;

   LPSTR          lpOutBufCur ;  /* pointers into the output buffer */
   WORD           cbCurSec ;
   LPSTR          lpTemp ;

   if ((HFILE)-1 == (hFile = OpenFile( lpszFile, &OF, OF_READ | OF_SHARE_DENY_NONE )))
   {
      DP1( hWDB, "Could not open ini file %s", (LPSTR)lpszFile ) ;
      return 0 ;
   }

   /*
    * Read the entire file into memory.
    */
   if (!(gh = GlobalAlloc( GHND, wFileSize+1 )))
   {
      DP1( hWDB, "Could not allocate global mem for sections (%d bytes!)",
            wFileSize + 1 ) ;
      return 0 ;
   }
   
   if (!(lpFileBuf = GlobalLock( gh )))
   {
      DP1( hWDB, "Could not lock global mem for sections" ) ;
      GlobalFree( gh ) ;
      return 0 ;
   }

   if (-1 != (wBytes = _lread( hFile, lpFileBuf, wFileSize )))
   {
      DP3( hWDB, "Read %d bytes into buffer (%d bytes in size)", wBytes,wFileSize+1 ) ;

      //
      // Empty file check
      //
      if (wBytes < 3)
      {
         _lclose( hFile ) ;
         GlobalUnlock( gh ) ;
         GlobalFree( gh ) ;
         return 0 ;
      }

      /*
       * Go through file with lpLine always pointing to the
       * beginning of a line and lpEOL always pointing to the
       * last character of a line.
       *
       * lpCur is our current pointer within the line.
       */
      lpLine = lpFileBuf ;
      lpEOF = lpFileBuf + wBytes ;


      lpTemp = lpOutBufCur = lpOutBuf ;
      lpOutBuf[0] = '\0' ;
      lpOutBuf[1] = '\0' ;

      while (lpEOL = FindEndOfLine( lpLine, lpEOF ))
      {

         /*
          * Scan the current line.  If we find a ';' the line is
          * a comment.  If we find a '[' then we are beginning
          * a section.
          */
         for (lpCur = lpLine, fInSec = FALSE, cbCurSec = 0 ;
              lpCur < lpEOL ;
              lpCur++)
         {
            if (fInSec)
            {
               /*
                * We are in a section
                */
               if (*lpCur == ']')
                  break ;


               *lpOutBufCur++ = *lpCur ;
               cbCurSec++ ;
            }
            else
            {
               if (*lpCur == '[')
                  fInSec = TRUE ;

               if (*lpCur == ';')
                  /*
                   * This line is a comment.  Break out of the for loop,
                   * and go on to the next line.
                   */
                  break ;

               if (*lpCur == '=')
                  /*
                   * This line is key value line.
                   */
                  break ;

            }
         }

         if (cbCurSec)
            *lpOutBufCur++ = '\0' ;

         lpTemp = lpOutBufCur ;

         lpLine = lpEOL ;

      }

      /*
       * Put the second NULL on
       */
      *lpOutBufCur++ = '\0' ;
   }

   _lclose( hFile ) ;

   GlobalUnlock( gh ) ;
   GlobalFree( gh ) ;

   return TRUE ;

} /* GetPrivateProfileSections()  */


/****************************************************************
 *  LPSTR NEAR PASCAL FindEndOfLine( LPSTR lpLine, LPSTR lpEOF )
 *
 *  Description: 
 *
 *    This function scans the line lpLine returning a pointer to
 *    the last character on the line.
 *
 *    It returns NULL when lpLine == lpEOF.
 *
 *  Comments:
 *
 *
 ****************************************************************/
LPSTR NEAR PASCAL FindEndOfLine( LPSTR lpLine, LPSTR lpEOF )
{
   register char  c ;

   while (lpLine <= lpEOF)
   {
      c = *lpLine ;

      if (c == '\n' || c == '\r' || c == '\f' || c == '\x1A')
      {
         /*
          * Scan until you get to a non '\n', '\r', '\f', or '\x1A'
          * character and return that pointer.
          */
         while( lpLine <= lpEOF )
         {
            if (c == '\n' || c == '\r' || c == '\f' || c == '\x1A')
               lpLine++ ;
            else
               return lpLine ;

            c = *lpLine ;
             
         }

       }
      lpLine++ ;
   }

   if (lpLine >= lpEOF)
      return NULL ;

   return lpLine ;

} /* FindEndOfLine()  */

/************************************************************************
 * End of File: fileio.c
 ***********************************************************************/

