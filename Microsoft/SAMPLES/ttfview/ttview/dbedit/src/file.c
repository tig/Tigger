/************************************************************************
 *
 *     Project:  DBEDIT
 *
 *      Module:  file.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "DBEDIT.h"
#include <memory.h>
#include <string.h>
#include <sys\types.h>
#include <sys\stat.h>         // time shit
#include <errno.h>
#include "listbox.h"
#include "cmd.h"
#include "config.h"
#include "fileio.h"
#include "file.h"


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
   LPFONTFILE   lpFONT ;
   HFILE       hFile ;

   hFile = _lopen( lpszFile, OF_READ ) ;

   /* Since we specified OFN_FILEMUSTEXIST hFile better not be -1 */
   if (hFile != (HFILE)-1)
   {
      /*
       * Verify that this is a real DB file.
       */
      if (0) //!ValidateFONTFILE( hFile ))       // file.c
      {
         _lclose( hFile ) ;

         /*
          * Pop up message box saying it's no good
          */
         DP2( hWDB, "The file '%s' is not a valid FontShop db file",
            (LPSTR)lpszFile ) ;

         return FALSE ;
      }

      _lclose( hFile ) ;

      /*
       * Ok, it's a good file.  Add it and make it active.
       *
       * If it's already there, just make it active.
       */
      DP5( hWDB, "Valid db file : %s", (LPSTR)lpszFile ) ;

      if (lpFONT = (LPFONTFILE)hlsqListAllocNode( hlstFiles, NULL, sizeof(FONTFILE) ))
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

         lpFONT->dwSize = sizeof( FONTFILE ) ;
         lpFONT->wMagic = FONTFILE_TAG ;
         lpFONT->wFlags = 0 ;

         lstrcpy( lpFONT->lpszFile, lpszFile ) ;
         
         lstrcpy( szFile, lpszFile ) ;
         stat( szFile, &Stat ) ;

         lpFONT->tTimeDate = Stat.st_atime ;

         lpFONT->dwFileSize = Stat.st_size ;
         lpFONT->wOffsetFile = wOffsetFile ;

         FONTFILE_lpszOutBuf(lpFONT) = NULL ;

         /*
          * The addfile2 adds the  node to both the list and
          * the listbox.  It returns it's index (or -1 if the
          * node could not be added).
          */
         w = AddFile( hlstFiles, lpFONT, lpf ) ;

         if (w == -1)
            hlsqListFreeNode( hlstFiles, lpFONT ) ;
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
 *  WORD WINAPI AddFile( HLIST hlst, LPFONTFILE lpFONTnew, BOOL FAR * lpf )
 *
 *  Description: 
 *
 *    The addfile adds the  node to both the list and
 *    the listbox.  It returns it's index (or -1 if the
 *    node could not be added).
 *
 ****************************************************************/
WORD WINAPI AddFile( HLIST hlst, LPFONTFILE lpFONTnew, BOOL FAR * lpf )
{
   LPFONTFILE      lpFONT ;
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
      lpFONT = (LPFONTFILE)SendMessage( hwndLB, LB_GETITEMDATA, (WPARAM)w, 0L ) ;
      if (lpFONT->wMagic == FONTFILE_TAG)
      {
         /*
          * We are sorted by filename, but we may have more than one
          * identical file name (path might be different).  
          */
         if (0 == lstrcmpi( lpFONT->lpszFile, lpFONTnew->lpszFile ))
         {
            /*
             * This guy is already there so hasta.
             *
             * But free up the node that was passed in since we can't
             * return some error value.  The caller will just have to
             * imagine everything worked groovy.
             *
             */
            hlsqListFreeNode( hlst, lpFONTnew ) ;

            *lpf = TRUE ;

            return w ;
         }

         /*
          * We know we can insert as soon as we find an existing
          * one that is "bigger" than us.
          */
         if (lstrcmpi( lpFONTnew->lpszFile + lpFONTnew->wOffsetFile,
                       lpFONT->lpszFile + lpFONT->wOffsetFile ) < 0)
         {
            if (!hlsqListInsertNode( hlst, lpFONTnew, lpFONT ))
            {
               DP1( hWDB, "Could not add node in GetDirectoryList" ) ;
               return -1 ;
            }
            SendMessage( hwndLB, LB_INSERTSTRING, (WPARAM)w, (LPARAM)lpFONTnew ) ;
            return w ;
         }

      } /* wMagic == FONTFILE_TAG */

   } /* for... */

   /*
    * We didn't find a place above, so add it to the end
    */
   if (!hlsqListInsertNode( hlst, lpFONTnew, LIST_INSERTTAIL ))
   {
      DP1( hWDB, "Could not add node in GetDirectoryList" ) ;
      return -1 ;
   }

   return (WORD)(DWORD)SendMessage( hwndLB, LB_ADDSTRING,
                                    0, (LPARAM)lpFONTnew ) ;

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
 *  int SORTPROC fnFileCompare( LPFONTFILE lp2, LPFONTFILE lp2 )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
int CALLBACK fnFileCompare( LPFONTFILE lp1, LPFONTFILE lp2, LONG l )
{
   DASSERT( hWDB, lp1 ) ;
   DASSERT( hWDB, lp2 ) ;

   return
      lstrcmpi( lp1->lpszFile + lp1->wOffsetFile,
               lp2->lpszFile + lp2->wOffsetFile ) ;


} /* fnFileCompare()  */


/****************************************************************
 *  WINAPI FileDoubleClick( WORD wID, LPFONTFILE lpFONT )
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
VOID WINAPI FileDoubleClick( WORD wID, LPFONTFILE lpFONT )
{
   #if 0
   /*
    * We'll just be extra careful here and validate that
    * it's really a file node...
    */
   if (FONTFILE_dwSize(lpFONT) != sizeof(FONTFILE))
   {
      DP1( hWDB, "FileDoubleClick called with a non FONTFILE node!" ) ;
      return ;
   }

   if (FONTFILE_fVisible(lpFONT) == TRUE)
   {
      /*
       * What kind of file is it?
       */
      switch( FONTFILE_wFlags( lpFONT ) )
      {
         case 0:
            ShowSections( lpFONT, wID, FALSE, TRUE ) ;
         break ;
      }
   }
   else
   {
      /*
       * What kind of file is it?
       */
      switch( FONTFILE_wFlags( lpFONT ) )
      {
         case 0:
            ShowSections( lpFONT, wID, TRUE, TRUE ) ;
         break ;
      }
   }
   #endif
} /* FileDoubleClick()  */


/************************************************************************
 * End of File: file.c
 ***********************************************************************/

