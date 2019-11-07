/************************************************************************
 *
 *     Project:  INIedit 2.0
 *
 *      Module:  file.c
 *
 *     Remarks:  File i/o functions. 
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "iniedit.h"

#include <string.h>
#include <sys\types.h>
#include <sys\stat.h>         // time shit
#include <errno.h>
#include "config.h"
#include "fileio.h"
#include "file.h"
#include "section.h"
#include "listbox.h"

/****************************************************************
 *  BOOL WINAPI AddSelectFile( LPSTR lpszFile )
 *
 *  Description: 
 *
 *    This function validates, adds, and selects the given file.
 *    If the file already exists in the list, then it simply
 *    selects it.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI AddSelectFile( LPSTR lpszFile, BOOL FAR *lpf )
{
   LPINIFILE   lpINI ;
   HFILE       hFile ;

   hFile = _lopen( lpszFile, OF_READ ) ;

   /* Since we specified OFN_FILEMUSTEXIST hFile better not be -1 */
   if (hFile != (HFILE)-1)
   {
      /*
       * Verify that this is a real INI file.
       */
      if (0) //!ValidateINIFile( hFile ))       // file.c
      {
         _lclose( hFile ) ;

         /*
          * Pop up message box saying it's no good
          */
         DP2( hWDB, "The file '%s' is not a valid Windows INI file",
            (LPSTR)lpszFile ) ;

         return FALSE ;
      }

      _lclose( hFile ) ;

      /*
       * Ok, it's a good file.  Add it and make it active.
       *
       * If it's already there, just make it active.
       */
      DP5( hWDB, "Valid INI file : %s", (LPSTR)lpszFile ) ;

      if (lpINI = (LPINIFILE)hlsqListAllocNode( hlstFiles, NULL, sizeof(INIFILE) ))
      {
         WORD           w ;
         char           szFile[MAX_PATH] ;
         struct stat    Stat ;
         WORD           wOffsetFile ;
         WORD           wOffsetExt ;

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
         
         lstrcpy( szFile, lpszFile ) ;
         stat( szFile, &Stat ) ;

         lpINI->tTimeDate = Stat.st_atime ;

         lpINI->dwFileSize = Stat.st_size ;
         lpINI->wOffsetFile = wOffsetFile ;

         INIFILE_lpszOutBuf(lpINI) = NULL ;
         INIFILE_hlstSections(lpINI) = NULL ;
         /*
          * The addfile2 adds the  node to both the list and
          * the listbox.  It returns it's index (or -1 if the
          * node could not be added).
          */
         w = AddFile( hlstFiles, lpINI, lpf ) ;

         if (w == -1)
            hlsqListFreeNode( hlstFiles, lpINI ) ;
         else
            SendMessage( hwndLB, LB_SETCURSEL, (WPARAM)w, 0L ) ;

      }
      else
      {
         DP1( hWDB, "Could not add file" ) ;
         return FALSE ;
      }
   }
   else
   {
      DP1( hWDB, "hFile = -1" ) ;
      return FALSE ;
   }

   return TRUE ;

} /* AddSelectFile()  */

/****************************************************************
 *  WORD WINAPI AddFile( HLIST hlst, LPINIFILE lpINInew, BOOL FAR * lpf )
 *
 *  Description: 
 *
 *    The addfile adds the  node to both the list and
 *    the listbox.  It returns it's index (or -1 if the
 *    node could not be added).
 *
 ****************************************************************/
WORD WINAPI AddFile( HLIST hlst, LPINIFILE lpINInew, BOOL FAR * lpf )
{
   LPINIFILE      lpINI ;
   WORD           w ;
   WORD           wCount ;

   *lpf = FALSE ;

   /*
    * Go through list box looking for the item.  If it's there
    * then never mind (wCur will be it's index).
    *
    */
   wCount = (WORD)(DWORD)SendMessage( hwndLB, LB_GETCOUNT, 0, 0L ) - 1 ;

   for (w = 0 ; w < wCount ; w++)
   {
      lpINI = (LPINIFILE)SendMessage( hwndLB, LB_GETITEMDATA, (WPARAM)w, 0L ) ;
      if (lpINI->wMagic == INIFILE_TAG)
      {
         /*
          * We are sorted by filename, but we may have more than one
          * identical file name (path might be different).  
          */
         if (0 == lstrcmpi( lpINI->lpszFile, lpINInew->lpszFile ))
         {
            /*
             * This guy is already there so hasta.
             *
             * But free up the node that was passed in since we can't
             * return some error value.  The caller will just have to
             * imagine everything worked groovy.
             *
             */
            hlsqListFreeNode( hlst, lpINInew ) ;

            *lpf = TRUE ;

            return w ;
         }

         /*
          * We know we can insert as soon as we find an existing
          * one that is "bigger" than us.
          */
         if (lstrcmpi( lpINInew->lpszFile + lpINInew->wOffsetFile,
                       lpINI->lpszFile + lpINI->wOffsetFile ) < 0)
         {
            if (!hlsqListInsertNode( hlst, lpINInew, lpINI ))
            {
               DP1( hWDB, "Could not add node in GetDirectoryList" ) ;
               return -1 ;
            }
            SendMessage( hwndLB, LB_INSERTSTRING, (WPARAM)w, (LPARAM)lpINInew ) ;
            return w ;
         }

      } /* wMagic == INIFILE_TAG */

   } /* for... */

   /*
    * We didn't find a place above, so add it to the end
    */
   if (!hlsqListInsertNode( hlst, lpINInew, LIST_INSERTTAIL ))
   {
      DP1( hWDB, "Could not add node in GetDirectoryList" ) ;
      return -1 ;
   }

   return (WORD)(DWORD)SendMessage( hwndLB, LB_ADDSTRING,
                                    0, (LPARAM)lpINInew ) ;

} /* AddFile()  */

/****************************************************************
 *  VOID WINAPI SortFiles( HLIST hlst )
 *
 *  Description: 
 *
 *    Sorts the file list
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI SortFiles( HLIST hlst )
{
   //   hlsqListSort( hlst, lpfnFileCompare, LIST_SORT_BUBBLE, 0L ) ;

} /* SortFiles()  */

/****************************************************************
 *  int SORTPROC fnFileCompare( LPINIFILE lp2, LPINIFILE lp2 )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
int CALLBACK fnFileCompare( LPINIFILE lp1, LPINIFILE lp2, LONG l )
{
   DASSERT( hWDB, lp1 ) ;
   DASSERT( hWDB, lp2 ) ;

   return
      lstrcmpi( lp1->lpszFile + lp1->wOffsetFile,
               lp2->lpszFile + lp2->wOffsetFile ) ;


} /* fnFileCompare()  */


/****************************************************************
 *  WINAPI FileDoubleClick( WORD wID, LPINIFILE lpINI )
 *
 *  Description: 
 *
 *    This function is called from the listbox manager when the
 *    user double clicks on a file item (or hits return).  We
 *    need to determine if the file is "open" or "closed" and
 *    do the reverse.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI FileDoubleClick( WORD wID, LPINIFILE lpINI )
{
   /*
    * We'll just be extra careful here and validate that
    * it's really a file node...
    */
   if (INIFILE_dwSize(lpINI) != sizeof(INIFILE))
   {
      DP1( hWDB, "FileDoubleClick called with a non INIFILE node!" ) ;
      return ;
   }

   if (INIFILE_fVisible(lpINI) == TRUE)
   {
      /*
       * What kind of file is it?
       */
      switch( INIFILE_wFlags( lpINI ) )
      {
         case 0:
            ShowSections( lpINI, wID, FALSE, TRUE ) ;
         break ;
      }
   }
   else
   {
      /*
       * What kind of file is it?
       */
      switch( INIFILE_wFlags( lpINI ) )
      {
         case 0:
            ShowSections( lpINI, wID, TRUE, TRUE ) ;
         break ;
      }
   }
} /* FileDoubleClick()  */
/************************************************************************
 * End of File: file.c
 ***********************************************************************/

