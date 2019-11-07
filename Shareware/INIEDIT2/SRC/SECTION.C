/************************************************************************
 *
 *     Project:  
 *
 *      Module:  section.c
 *
 *     Remarks:  
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
#include "section.h"
#include "keyvalue.h"
#include "listbox.h"

#define MAX_SECTIONBUF     (WORD)(1024L * 60L)  /* 62k? */

static   char           szFile[MAX_PATH] ;
static   struct stat    Stat ;


/****************************************************************
 *  BOOL WINAPI UpdateSection( HWND hwnd )
 *
 *  Description: 
 *
 *    This is called when the user does something that would
 *    change the value of the current section.
 *
 *    If this would be a change, then the new section is
 *    created, the old one copied to it, then the old one
 *    is deleted.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI UpdateSection( HWND hwnd )
{
   WORD           cbTextLen ;
   LPINIFILE      lpINI ;
   WORD           wID ;
   LPNODEHEADER   lpCurNode ;

   wID = (WORD)(DWORD)SendMessage( hwndLB, LB_GETCURSEL, 0, 0L ) ;
   if (wID == -1)
      return FALSE ;

   lpCurNode = (LPNODEHEADER)SendMessage( hwndLB, LB_GETITEMDATA,
                                          (WPARAM)wID, 0L ) ;

   /*
    * Find out how long the text in the edit is
    */
   cbTextLen = (WORD)(DWORD)SendMessage( hwnd, WM_GETTEXTLENGTH,
                                    0, 0L ) ;


   /*
    * If the length of the text is less than 1 or we detect
    * something wrong with the text just beep and return
    * !!! Maybe we want to pop up a dlg...
    */
   if (cbTextLen < 1)
   {
      DP( hWDB, "Text len is less than 1 char long." ) ;
      MessageBeep( 0 ) ;
      return FALSE ;
   }

   /*
    * Nuke the memory previously allocated for this item's string
    * then allocate it anew... (or use Realloc)
    */
   lpINI = SECTION_lpParent( lpCurNode ) ;

   FREEFROMHEAP( hhStrings, (LPSTR)SECTION_lpszSec( lpCurNode ) ) ;

   SECTION_lpszSec( lpCurNode ) = ALLOCFROMHEAP( hhStrings, cbTextLen+2 ) ;
                                    
   if (SECTION_lpszSec( lpCurNode ))
      GetWindowText( hwnd, SECTION_lpszSec( lpCurNode ),
                     cbTextLen+1) ;
   else
   {
      DP1( hWDB, "Could not realloc text!" ) ;
      return FALSE ;
   }

   return TRUE ;

} /* UpdateSection()  */


/****************************************************************
 *  VOID WINAPI SortSections( HLIST hlst )
 *
 *  Description: 
 *
 *    Sorts the file list
 *
 *  Comments:
 *
 ****************************************************************/
int CALLBACK fnSectionCompare( LPSECTION lp1, LPSECTION lp2, LONG l ) ;

VOID WINAPI SortSections( HLIST hlst )
{
   // hlsqListSort( hlst, lpfnSectionCompare, LIST_SORT_BUBBLE, 0L ) ;
} /* SortSections()  */

/****************************************************************
 *  int SORTPROC fnSectionCompare( LPSECTION lp2, LPSECTION lp2 )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
int CALLBACK fnSectionCompare( LPSECTION lp1, LPSECTION lp2, LONG l )
{
   DASSERT( hWDB, lp1 ) ;
   DASSERT( hWDB, lp2 ) ;

   return
      lstrcmpi( lp1->lpszSec, lp2->lpszSec ) ;


} /* fnSectionCompare()  */


/****************************************************************
 *  WINAPI SectionDoubleClick( WORD wID, LPINIFILE lpSection )
 *
 *  Description: 
 *
 *    This function is called from the listbox manager when the
 *    user double clicks on a section item (or hits return).  We
 *    need to determine if the section is "open" or "closed" and
 *    do the reverse.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI SectionDoubleClick( WORD wID, LPSECTION lpSection )
{
   /*
    * We'll just be extra careful here and validate that
    * it's really a file node...
    */
   if (SECTION_dwSize(lpSection) != sizeof(SECTION))
   {
      DP1( hWDB, "SectionDoubleClick called with a non SECTION node!" ) ;
      return ;
   }

   if (SECTION_fVisible(lpSection) == TRUE)
   {
      /*
       * What kind of section is it?  Does it matter?
       */

      switch( SECTION_wFlags( lpSection ) )
      {
         case 0:
            ShowKeyValues( lpSection, wID, FALSE, FALSE ) ;
         break ;
      }
   }
   else
   {
      switch( SECTION_wFlags( lpSection ) )
      {
         case 0:
            ShowKeyValues( lpSection, wID, TRUE, TRUE ) ;
         break ;
      }
   }

   return ;


} /* SectionDoubleClick()  */



/****************************************************************
 * WORD WINAPI ShowSections( LPINIFILE lpINI, WORD wID, BOOL fVis, BOOL fOpen )
 *
 *  Description: 
 *
 *    Shows or hides the sections associated with the specified
 *    file.  Uses wID as the listbox index of the INIFILE entry.
 *
 *    fVis     specifies whether the contents are to be
 *             visible or not.  If fVis is TRUE the contents
 *             are loaded (if not already) and made visible.  If
 *             fVis is FALSE the contents are hiden, but not
 *             freed.
 *
 *    fOpen    If fVis is TRUE:
 *                if fOpen is TRUE the file will be displayed otherwise
 *                it won't.
 *             If fVis is FALSE:
 *                if fOpen is TRUE fOpen is set otherwise it is cleared.
 *
 *   This function returns the listbox index of the last section
 *   displayed if the sections were shown.  Otherwise it
 *   returns the listbox index of the lpINI file entry.
 *
 *  Comments:
 *
 ****************************************************************/
WORD WINAPI ShowSections( LPINIFILE lpINI, WORD wID, BOOL fVis, BOOL fOpen )
{
   HLIST          hlst ;
   LPSECTION      lpSec ;

   if (fVis == FALSE)
   {
      if (!(hlst = INIFILE_hlstSections(lpINI)))
         return -1 ;


      if (!INIFILE_fVisible( lpINI ))
         return wID ;

      SetWaitCursor() ;

      /*
       * Remove them from the list box.
       */
      SetLBRedraw( FALSE ) ;
      lpSec = hlsqListFirstNode( hlst ) ;
      wID++ ;
      while (lpSec)
      {
         /*
          * Does this guy have any children?  If so is the list
          * of children (sections) open?
          *
          * Make the list of keys hidden.
          */
         ShowKeyValues( lpSec, wID, FALSE, fOpen ) ;

         SendMessage( hwndLB, LB_DELETESTRING, (WPARAM)wID, 0L ) ;

         lpSec = hlsqListNextNode( hlst, lpSec ) ;
      }
      SetLBRedraw( TRUE ) ;

      if (!fWin31)
         InvalidateRect( hwndLB, NULL, TRUE ) ;

      INIFILE_fVisible(lpINI) = FALSE ;
      INIFILE_fOpen(lpINI) = fOpen ;

      SetNormalCursor() ;
      return --wID ;
   }
   else
   {
      /*
       * We want to make the contents visible.  Call ReadSections()
       * To get the latest list.
       */
      if (!ReadSections( lpINI, fOpen ))
         return wID ;

      if (INIFILE_fVisible( lpINI ))
         return wID + (WORD)hlsqListItems( INIFILE_hlstSections( lpINI ) ) ;

      SetWaitCursor() ;

      /*
       * Put em in the list box.
       */
      if (hlst = INIFILE_hlstSections( lpINI ))
      {
         INIFILE_fVisible(lpINI) = TRUE ;
         INIFILE_fOpen(lpINI) = TRUE ;

         SetLBRedraw( FALSE ) ;
         lpSec = hlsqListFirstNode( hlst ) ;
         while (lpSec)
         {

            wID++ ;
            SendMessage( hwndLB, LB_INSERTSTRING, (WPARAM)wID,
                           (LPARAM)lpSec ) ;
       
            /*
            * Show children if they have their fVisible flag
            * set.
            */
            wID = ShowKeyValues( lpSec, wID, TRUE, FALSE ) ;

            lpSec = hlsqListNextNode( hlst, lpSec ) ;
         }

         SetLBRedraw( TRUE ) ;

         if (!fWin31)
            InvalidateRect( hwndLB, NULL, TRUE ) ;

         /*
         * Update date/time
         */
         lstrcpy( szFile, INIFILE_lpszFile(lpINI) ) ;
         stat( szFile, &Stat ) ;
         if ((DWORD)Stat.st_size != INIFILE_dwFileSize(lpINI) ||
               Stat.st_atime != INIFILE_tTimeDate(lpINI))
         {
            INIFILE_dwFileSize(lpINI) = (DWORD)Stat.st_size ;
            INIFILE_tTimeDate(lpINI) = Stat.st_atime ;
         }

      }
      SetNormalCursor() ;

      return wID ;
   }

   return -1 ;

} /* ShowSections()  */


/****************************************************************
 *  HLIST WINAPI ReadSections( LPINIFILE lpINI, BOOL fForce )
 *
 *  Description: 
 *
 *    Returns a list of sections for the given file.
 *
 *    If fForce is TRUE and the list does not exist the
 *    list be read.  If fForce is FALSE the current list
 *    will be returned.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI ReadSections( LPINIFILE lpINI, BOOL fForce )
{
   HLIST          hlst ;
   HLIST          hlstOld = NULL ;
   LPSTR          lpszSec ;
   LPSTR          lpSectionBuf ;
   LPSECTION      lpSec ;
   LPSECTION      lpSecTemp ;
   WORD           wFileSize ;
   BOOL           f ;

   SetWaitCursor() ;
   /*
    * Allocate memory for the section strings.
    */
   wFileSize =
      min( LOWORD( INIFILE_dwFileSize( lpINI ) ), MAX_SECTIONBUF ) ;
   wFileSize = max( wFileSize, 1024 ) ;

   /*
    * If the file is already open (hlst is non zero) and
    * has changed, re-read it.
    *
    * Or if fForce is TRUE ;
    */
   if (hlst = INIFILE_hlstSections( lpINI ))
   {
      lstrcpy( szFile, INIFILE_lpszFile(lpINI) ) ;
      stat( szFile, &Stat ) ;
      if ((DWORD)Stat.st_size != INIFILE_dwFileSize(lpINI) ||
            Stat.st_atime != INIFILE_tTimeDate(lpINI))
      {
         DP( hWDB, "File has changed.  Re-reading Sections." ) ;

         /*
          * Only destroy the original if a section name
          * has changed.
          */
         hlstOld = hlst ;
         hlst = NULL ;
      }
   }
         
   /*
    * Create and build list if it is not already there.
    */
   if (hlst == NULL && fForce == TRUE)
   {
      DP( hWDB, "hlst == NULL " ) ;
      if (!(hlst = hlsqListCreate( hhLists )))
      {
         DP1( hWDB, "Could not create list of sections" ) ;
         SetNormalCursor() ;
         return FALSE ;
      }

      /*
       * Allocate a chunk that *should* be big enough.
       * We have the file size, so just use it!
       */
      if (!(lpSectionBuf = (LPSTR)GALLOC( wFileSize )))
      {
         DP1( hWDB, "Could not allocate %d bytes for section buffer" ) ;
         hlsqListDestroy( hlst ) ;
         SetNormalCursor() ;
         return FALSE ;
      }
   
      if (GetPrivateProfileSections( lpSectionBuf,
                                    wFileSize,
                                    INIFILE_lpszFile(lpINI),
                                    wFileSize ))
      {
         for (lpszSec = lpSectionBuf ;
            *lpszSec != '\0' ;
            lpszSec += lstrlen( lpszSec ) + 1)
         {
            /*
             * Create a node.  Add it to the list.
             */
            if (lpSec = (LPSECTION)hlsqListAllocNode( hlst, NULL, sizeof(SECTION) ))
            {
               SECTION_dwSize(lpSec) = sizeof( SECTION ) ;
               SECTION_wMagic(lpSec) = SECTION_TAG ;
               SECTION_wFlags(lpSec) = 0 ;
               SECTION_wIcon(lpSec)  = ICON_SECTION ;

               /*
                * Alloc mem out of the hhStrings buffer
                */
               if (!(SECTION_lpszSec(lpSec) =
                     ALLOCFROMHEAP( hhStrings, lstrlen( lpszSec ) + 1 ) ))
               {
                  hlsqListFreeNode( hlst, lpSec ) ;
                  continue ;
               }

               lstrcpy( SECTION_lpszSec(lpSec), lpszSec ) ;
      
               SECTION_hlstKeys(lpSec)     = NULL ;

               SECTION_lpParent(lpSec) = lpINI ;
            }
            else
            {
               /*
                * Whoa!  Could not alloc a node! Majorly fatal.
                */
               hlsqListDestroy( hlst ) ;
               GFREE( lpSectionBuf ) ;
               SetNormalCursor() ;
               return FALSE ;
            }

            /*
             * We do not allow duplicate section names.  Depending
             * on if we are sorted or not, finding another occurance
             * can be fast.
             */
            if (fSortSections)
            {
               f = hlsqListInsertSorted(hlst, lpSec,
                        lpfnSectionCompare, 0L ) ;
               /*
                * We've inserted the node.  Check to see if the
                * previous node is the same.  If so...hasta!
                */
               if (f &&
                   (lpSecTemp = (LPSECTION)hlsqListNextNode( hlst, lpSec )) &&
                   !lstrcmpi( SECTION_lpszSec(lpSec), SECTION_lpszSec(lpSecTemp) ))
               {
                  //DP( hWDB, "Duplicate! '%s'", (LPSTR)SECTION_lpszSec(lpSec) ) ;
                  hlsqListDeleteNode( hlst, lpSec ) ;
               }
            }
            else
            {
               /*
                * Since we're not sorted we must scan the list ourselves
                */
               lpSecTemp = hlsqListFirstNode( hlst ) ;
               while (lpSecTemp)
               {
                  if (!lstrcmpi( SECTION_lpszSec(lpSec),
                                SECTION_lpszSec(lpSecTemp) ))
                  {
                     /* Duplicate! */
                     //DP( hWDB, "Duplicate! '%s'", (LPSTR)SECTION_lpszSec(lpSec) ) ;
                     hlsqListFreeNode( hlst, lpSec ) ;
                     lpSec = NULL ;
                     f = TRUE ;
                     break ;
                  }
                  lpSecTemp = hlsqListNextNode( hlst, lpSecTemp ) ;
               }

               if (lpSec)
                  f = hlsqListInsertNode(hlst, lpSec, LIST_INSERTTAIL ) ;
            }

            if (f == FALSE)
            {
               DP1( hWDB, "Could not add node in DoubleClickFile : hlst = %08lX, lpSec = %08lX",
                  (DWORD)hlst, (DWORD)lpSec ) ;
               hlsqListFreeNode( hlst, lpSec ) ;

               continue ;
            }
         }
      }
      else
      {
         /*
          * It doesn't look like this is a INI file (or the buffer
          * wasn't big enough (I doubt it).
          */
         DP1( hWDB, "GetPrivateprofileSections failed" ) ;

         hlsqListDestroy( hlst ) ;
         GFREE( lpSectionBuf ) ;
         SetNormalCursor() ;
         return FALSE ;
      }

      /*
       * If hlstOld is non zero, then the file has changed, and hlst has
       * the new file.  Go through each section in hlst and compare it
       * with hlstOld.
       */
      if (hlstOld && hlst)
      {
         LPKEYVALUE     lpKeyValue ;

         for ( lpSec = hlsqListFirstNode( hlst ) ; lpSec ;
               lpSec = hlsqListNextNode( hlst, lpSec ) )
         {
            /*
             * Go through old list comparing.  If we find a match,
             * copy it and nuke it in the old list.
             */

            for ( lpSecTemp = hlsqListFirstNode( hlstOld ) ; lpSecTemp ;
                  lpSecTemp = hlsqListNextNode( hlstOld, lpSecTemp ) )
            {
               if (!lstrcmpi( SECTION_lpszSec(lpSec),
                              SECTION_lpszSec(lpSecTemp) ))
               {
                  /*
                   * Copy it!
                   */
                  SECTION_fVisible( lpSec ) = SECTION_fVisible( lpSecTemp ) ;
                  SECTION_fOpen( lpSec ) = SECTION_fOpen( lpSecTemp ) ;
                  SECTION_hlstKeys( lpSec ) = SECTION_hlstKeys( lpSecTemp ) ;

                  hlsqListRemoveNode( hlstOld, lpSecTemp ) ;

                  /*
                   * Go through all children and make sure their parent
                   * is fixed up.
                   */
                  for ( lpKeyValue = hlsqListFirstNode( SECTION_hlstKeys( lpSec ) ) ;
                        lpKeyValue ;
                        lpKeyValue = hlsqListNextNode( SECTION_hlstKeys( lpSec ),
                                                       lpKeyValue ) )
                  {
                     KEYVALUE_lpParent( lpKeyValue ) = lpSec ;
                  }

                  /*
                   * We can break out of here 'cause there are no dupes.
                   */
                  break ;
               }
            }
         }

         /*
          * Now we can nuke the old list
          */
         for ( lpSecTemp = hlsqListFirstNode( hlstOld ) ; lpSecTemp ;
               lpSecTemp = hlsqListNextNode( hlstOld, lpSecTemp ) )
         {
            DestroySection( lpSecTemp ) ;
         }
         hlsqListDestroy( hlstOld ) ;

      }

      GFREE( lpSectionBuf ) ;
   }

   SetNormalCursor() ;

   INIFILE_hlstSections( lpINI ) = hlst ;

   return TRUE  ;

} /* ReadSections()  */


/****************************************************************
 *  BOOL WINAPI DestroySection( LPSECTION lpSec )
 *
 *  Description: 
 *
 *    Destroys a section, freeing all memory allocated for it.
 *    Note, this does not destroy the node.  Only memory allocated
 *    below it (like keys).
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI DestroySection( LPSECTION lpSec )
{
   if (SECTION_hlstKeys( lpSec ))
   {
      hlsqListDestroy( SECTION_hlstKeys( lpSec ) ) ;
      SECTION_hlstKeys( lpSec ) = NULL ;
   }

   if (SECTION_lpszSec( lpSec ))
   {
      FREEFROMHEAP( hhStrings, SECTION_lpszSec( lpSec ) ) ;
      SECTION_lpszSec( lpSec ) = NULL ;
   }

   SECTION_fOpen( lpSec ) = FALSE ;
   SECTION_fVisible( lpSec ) = FALSE ;

   return TRUE ;

} /* DestroySection()  */


/************************************************************************
 * End of File: section.c
 ***********************************************************************/

