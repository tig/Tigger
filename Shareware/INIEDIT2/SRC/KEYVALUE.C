/************************************************************************
 *
 *     Project:  
 *
 *      Module:  keyvalue.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "iniedit.h"

#include <string.h>
#include <ctype.h>
#include <sys\types.h>
#include <sys\stat.h>         // time shit
#include <errno.h>
#include "config.h"
#include "fileio.h"
#include "keyvalue.h"
#include "listbox.h"

#define MAX_KEYBUF         (WORD)(1024 * 60L)
#define MAX_VALUEBUF       (WORD)(1024 * 60L)

VOID WINAPI StripWhiteSpace( LPSTR lpsz ) ;

/****************************************************************
 *  BOOL WINAPI UpdateKeyValue( HWND hwnd )
 *
 *  Description: 
 *
 *    This is called when the user does something that would
 *    change the value of the current key/value.
 *
 *    If the contents of the edit buffer consititue a change,
 *    WPPS will be called to make the change.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI UpdateKeyValue( HWND hwnd )
{
   WORD           cbTextLen ;
   LPSECTION      lpParentSec ;
   LPSTR          lpszKey ;
   LPSTR          lpszValue ;
   WORD           wID ;
   LPKEYVALUE     lpKeyValue ;

   wID = (WORD)(DWORD)SendMessage( hwndLB, LB_GETCURSEL, 0, 0L ) ;
   if (wID == -1)
      return FALSE ;

   lpKeyValue = (LPKEYVALUE)SendMessage( hwndLB, LB_GETITEMDATA,
                                          (WPARAM)wID, 0L ) ;

   if (KEYVALUE_dwSize(lpKeyValue) != sizeof(KEYVALUE))
      return FALSE ;

   /*
    * Find out how long the text in the edit is
    */
   cbTextLen = (WORD)(DWORD)SendMessage( hwnd, WM_GETTEXTLENGTH,
                                    0, 0L ) ;

   if (!(lpszKey = ALLOCFROMHEAP( hhScratch, cbTextLen + 2 )))
   {
      DP1( hWDB, "Could not alloc string in UpdateKeyvalue" ) ;
      return FALSE ;
   }

   GetWindowText( hwnd, lpszKey, cbTextLen + 1) ;

   /*
    * Get pointers to key and value parts
    */
   if (lpszKey = _fstrtok( lpszKey, "=" ))
   {
      if (lpszValue = _fstrtok( NULL, "" ))
         /*
          * Strip any leading or trailing blanks
          */
         StripWhiteSpace( lpszValue ) ;

      StripWhiteSpace( lpszKey ) ;
   }

   /*
    * Nuke the memory previously allocated for this item's string
    * then allocate it anew... (or use Realloc)
    */
   lpParentSec = KEYVALUE_lpParent( lpKeyValue ) ;
   if (KEYVALUE_lpszValue( lpKeyValue ))
   {
      FREEFROMHEAP( hhStrings, KEYVALUE_lpszValue( lpKeyValue ) ) ;
      KEYVALUE_lpszValue( lpKeyValue ) = NULL ;
   }

   if (KEYVALUE_lpszKey( lpKeyValue ))
   {   
      FREEFROMHEAP( hhStrings, KEYVALUE_lpszKey( lpKeyValue ) ) ;
      KEYVALUE_lpszKey( lpKeyValue ) = NULL ;
   }


   /*
    * KEYVALUE structures have pointers to the key AND the value.
    * need to get both.
    */

   if (lpszKey && lstrlen( lpszKey ))
   {
      KEYVALUE_lpszKey( lpKeyValue ) =
            ALLOCFROMHEAP( hhStrings, lstrlen( lpszKey ) + 1 ) ;
      lstrcpy( KEYVALUE_lpszKey( lpKeyValue ), lpszKey ) ;
   }

   if (lpszValue && lstrlen( lpszValue ))
   {
      KEYVALUE_lpszValue( lpKeyValue ) =
            ALLOCFROMHEAP( hhStrings, lstrlen( lpszValue ) + 1 ) ;
      lstrcpy( KEYVALUE_lpszValue( lpKeyValue ), lpszValue ) ;
   }

   /*
    * Nuke output buffer
    */
   if (KEYVALUE_lpszOutBuf( lpKeyValue ))
   {
      FREEFROMHEAP( hhOutBuf, KEYVALUE_lpszOutBuf( lpKeyValue ) ) ;
      KEYVALUE_lpszOutBuf( lpKeyValue ) = NULL ;
   }

   FREEFROMHEAP( hhScratch, lpszKey ) ;

   /*
    * Now WRITE THE FUCKING THING!
    */
   WritePrivateProfileString( SECTION_lpszSec( lpParentSec ),
                              lpszKey,
                              lpszValue,
                    INIFILE_lpszFile( SECTION_lpParent( lpParentSec ) ) ) ;


   return TRUE ;

} /* UpdateKeyValue()  */


/****************************************************************
 *  WORD WINAPI ShowKeyValues( LPSECTION lpSect, WORD wID, BOOL fVis, BOOL fOpen )
 *
 *  Description: 
 *
 *    Shows or hides the key values associated with the specified
 *    section.  Uses wID as the listbox index of the SECTION entry.
 *
 *    fVis     specifies whether the contents are to be
 *             visible or not.  If fVis is TRUE the contents
 *             are loaded (if not already) and made visible.  If
 *             fVis is FALSE the contents are hiden, but not
 *             freed.
 *
 *    fOpen    If fVis is TRUE:
 *                if fOpen is TRUE the section will be displayed otherwise
 *                it won't.
 *             If fVis is FALSE:
 *                if fOpen is TRUE fOpen is set otherwise it is cleared.
 *
 *   This function returns the listbox index of the last keyvalue
 *   displayed if the keys were shown.  Otherwise it
 *   returns the listbox index of the lpSect file entry.
 *
 *  Comments:
 *
 ****************************************************************/
WORD WINAPI ShowKeyValues( LPSECTION lpSect, WORD wID, BOOL fVis, BOOL fOpen )
{
   HLIST          hlst ;
   LPKEYVALUE     lpKeyVal ;


   if (fVis == FALSE)
   {
      if (!(hlst = SECTION_hlstKeys(lpSect)))
         return -1 ;

      if (!SECTION_fVisible( lpSect ))
         return wID ;

      SetWaitCursor() ;

      /*
       * Remove them from the list box.
       */
      SetLBRedraw( FALSE ) ;
      lpKeyVal = hlsqListFirstNode( hlst ) ;
      wID++ ;
      while (lpKeyVal)
      {
         SendMessage( hwndLB, LB_DELETESTRING, (WPARAM)wID, 0L ) ;

         lpKeyVal = hlsqListNextNode( hlst, lpKeyVal ) ;
      }

      SetLBRedraw( TRUE ) ;

      if (!fWin31)
         InvalidateRect( hwndLB, NULL, TRUE ) ;

      SECTION_fVisible(lpSect) =  FALSE ;
      SECTION_fOpen(lpSect) =  fOpen ;

      SetNormalCursor() ;

      return --wID ;
   }
   else
   {
      /*
       * We want to make the contents visible.  Call ReadKeyValues()
       * To get the latest list.
       */
      if (fOpen == FALSE && !SECTION_fOpen( lpSect ))
         return wID ;

      if (!ReadKeyValues( lpSect, fOpen ))
         return wID ;


      if (hlst = SECTION_hlstKeys( lpSect ))
      {
         if (hlst && SECTION_fVisible( lpSect ))
            return wID + (WORD)hlsqListItems( hlst ) ;

         SetWaitCursor() ;

         /*
          * Put em in the list box.
          */
         if (hlst)
         {
            SECTION_fVisible(lpSect) = TRUE ;
            SECTION_fOpen(lpSect) = TRUE ;
         }

         SetLBRedraw( FALSE ) ;
         lpKeyVal = hlsqListFirstNode( hlst ) ;
         while (lpKeyVal)
         {
            wID++ ;
            SendMessage( hwndLB, LB_INSERTSTRING, (WPARAM)wID,
                           (LPARAM)lpKeyVal ) ;
       
            lpKeyVal = hlsqListNextNode( hlst, lpKeyVal ) ;
         }

         SetLBRedraw( TRUE ) ;

         if (!fWin31)
            InvalidateRect( hwndLB, NULL, TRUE ) ;

         SetNormalCursor() ;
      }

      return wID ;
   }

   return -1 ;

} /* ShowKeyValues()  */


/****************************************************************
 *  HLIST WINAPI ReadKeyValues( LPSECTION lpSect, BOOL fForce )
 *
 *  Description: 
 *
 *    Returns a list of keyvalues for the given section.
 *
 *    If fForce is TRUE and the list does not exist the
 *    list be read.  If fForce is FALSE the current list
 *    will be returned.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI ReadKeyValues( LPSECTION lpSect, BOOL fForce )
{
   HLIST          hlst ;
   LPINIFILE      lpParentINI ;
   LPKEYVALUE     lpKeyValue ;

   LPSTR          lpKeyBuf ;
   LPSTR          lpValueBuf ;
   LPSTR          lpszKey ;
   BOOL           f ;
                      
   struct stat    Stat ;
   char           szFile[MAX_PATH] ;

   SetWaitCursor() ;

   lpParentINI = SECTION_lpParent( lpSect ) ;

   /*
    * Allocated memory for the keys and values.
    */

   if (hlst = SECTION_hlstKeys( lpSect ))
   {
      /*
       * The list already exists.  Make sure the file has not
       * changed since last read/write.
       */

      lstrcpy( szFile, INIFILE_lpszFile( lpParentINI ) ) ;
      stat( szFile, &Stat ) ;

      if ((DWORD)Stat.st_size != INIFILE_dwFileSize( lpParentINI ) ||
            Stat.st_atime != INIFILE_tTimeDate( lpParentINI ))
      {
         DP( hWDB, "File has changed.  Re-reading keys.  hlst = %04X", hlst ) ;
         hlsqListDestroy( hlst )  ;
         hlst = NULL ;

         /*
          * !!! Need to do something here in regards to re reading
          * entire file...
          */
      }
   }
      
   /*
    * Create and build list if it is not already there.
    */
   if (hlst == NULL)
   {
      if (!(hlst = hlsqListCreate( hhLists )))
      {
         DP1( hWDB, "Could not create list of sections" ) ;

         SetNormalCursor() ;
         return FALSE ;
      }

      /*
       * Allocate a chunk that *should* be big enough.
       */
      if (!(lpKeyBuf = GALLOC( MAX_KEYBUF  )))
      {
         DP1( hWDB, "Could not allocate %u bytes for key buffer", MAX_KEYBUF ) ;
         
         hlsqListDestroy( hlst ) ;
         SetNormalCursor() ;
         return FALSE ;
      }

      /*
       * Allocate a chunk that *should* be big enough.
       */
      if (!(lpValueBuf = GALLOC( MAX_VALUEBUF  )))
      {
         DP1( hWDB, "Could not allocate %u bytes for value buffer", MAX_VALUEBUF ) ;
         
         hlsqListDestroy( hlst ) ;
         GFREE( lpKeyBuf ) ;
         SetNormalCursor() ;
         return FALSE ;
      }

      if (GetPrivateProfileString( SECTION_lpszSec( lpSect ),
                                    NULL,
                                    "",
                                    lpKeyBuf,
                                    MAX_KEYBUF,
                                    INIFILE_lpszFile( lpParentINI ) ))
      {
         for (lpszKey = lpKeyBuf ;
            *lpszKey != '\0' ;
            lpszKey += lstrlen( lpszKey ) + 1)
         {
            /*
             * Create a node.  Add it to the list.
             */
            if (lpKeyValue = (LPKEYVALUE)hlsqListAllocNode( hlst, NULL, sizeof(SECTION) ))
            {
               KEYVALUE_dwSize(lpKeyValue) = sizeof( KEYVALUE ) ;
               KEYVALUE_wMagic(lpKeyValue) = KEYVALUE_TAG ;
               KEYVALUE_wFlags(lpKeyValue) = 0 ;
               KEYVALUE_wIcon(lpKeyValue)  = ICON_KEY ;

               /*
                * Alloc mem out of the hhStrings heap
                */
               if (!(KEYVALUE_lpszKey( lpKeyValue ) =
                     ALLOCFROMHEAP( hhStrings, lstrlen( lpszKey ) + 1 ) ))
               {
                  DP1( hWDB, "Could not alloc string for key" ) ;
                  hlsqListFreeNode( hlst, lpKeyValue ) ;
                  continue ;
               }

               lstrcpy( KEYVALUE_lpszKey(lpKeyValue), lpszKey ) ;

               if (GetPrivateProfileString(
                           SECTION_lpszSec( lpSect ),
                           lpszKey,
                           "",
                           lpValueBuf,
                           MAX_VALUEBUF,
                           INIFILE_lpszFile( lpParentINI ) ))
               {
                  if (!(KEYVALUE_lpszValue( lpKeyValue ) =
                        ALLOCFROMHEAP( hhStrings, lstrlen( lpValueBuf ) + 1 ) ))
                  {
                     DP1( hWDB, "Could not alloc string for value" ) ;
                     hlsqListFreeNode( hlst, lpKeyValue ) ;
                     continue ;
                  }

                  lstrcpy( KEYVALUE_lpszValue(lpKeyValue), lpValueBuf ) ;
               }
               else
                  KEYVALUE_lpszValue(lpKeyValue) = NULL ;
   
               KEYVALUE_lpParent(lpKeyValue) = lpSect ;
            }
            else
            {
               /*
                * Whoa!  Could not alloc a node! Majorly fatal.
                */
               
               hlsqListDestroy( hlst ) ;
               GFREE( lpKeyBuf ) ;
               GFREE( lpValueBuf ) ;
               SetNormalCursor() ;
               return FALSE ;
            }

            if (!fSortKeys)
               f = hlsqListInsertSorted(hlst, lpKeyValue,
                           lpfnKeyValueCompare, 0L ) ;
            else
               f = hlsqListInsertNode(hlst, lpKeyValue, LIST_INSERTTAIL ) ;

            if (f == FALSE)
            {
               DP1( hWDB, "Could not add node in SectionDoubleClick : hlst = %08lX, lpKeyValue = %08lX",
                     (DWORD)hlst, (DWORD)lpKeyValue ) ;
               hlsqListFreeNode( hlst, lpKeyValue ) ;

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
         DP1( hWDB, "GetPrivateprofileString (keys) failed" ) ;

         hlsqListDestroy( hlst ) ;
         GFREE( lpKeyBuf ) ;
         GFREE( lpValueBuf ) ;
         SetNormalCursor() ;
         return FALSE ;
      }

      GFREE( lpValueBuf ) ;
      GFREE( lpKeyBuf ) ;
   }

   SetNormalCursor() ;

   SECTION_hlstKeys( lpSect ) = hlst ;

   return TRUE  ;

} /* ReadKeyValues()  */


/****************************************************************
 *  VOID WINAPI StripWhiteSpace( LPSTR lpsz )
 *
 *  Description: 
 *
 *    This function strips the given string of all leading
 *    and trailing whtiespace.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI StripWhiteSpace( LPSTR lpsz )
{
   LPSTR lpszCur ;
   LPSTR lpszCur2 ;
   short n ;

   /*
    * Left to right.  Go to first non whitespace.
    */
   for (lpszCur = lpsz ; *lpszCur && isspace(*lpszCur) ; lpszCur++)
      ;

   if (!*lpszCur) // all whitespace!
   {
      lpsz[0] = '\0' ;
      return ;
   }

   /*
    * Now lpszCur points to the first non white guy.
    */
   if (lpszCur != lpsz)
   {
      n = lstrlen( lpszCur ) ;

      for ( lpszCur2 = lpsz ; *lpszCur ; lpszCur++, lpszCur2++)
          *lpszCur2 = *lpszCur ;

      lpsz[n] = '\0' ;
   }

   /*
    * Right to left.  Go back to first non-whitey.
    */
   n = lstrlen( lpsz ) - 1 ;
   for (lpszCur = lpsz + n ;
        lpszCur >= lpsz && isspace(*lpszCur) ; lpszCur--)
   {
      *lpszCur = '\0' ;
   }

} /* StripWhiteSpace()  */

/****************************************************************
 *  VOID WINAPI SortKeyValues( HLIST hlst )
 *
 *  Description: 
 *
 *    Sorts the file list
 *
 *  Comments:
 *
 ****************************************************************/
int CALLBACK fnKeyValueCompare( LPKEYVALUE lp1, LPKEYVALUE lp2, LONG l ) ;

VOID WINAPI SortKeyValues( HLIST hlst )
{
  // hlsqListSort( hlst, lpfnKeyValueCompare, LIST_SORT_BUBBLE, 0L ) ;
} /* SortKeyValues()  */

/****************************************************************
 *  int SORTPROC fnKeyValueCompare( LPKEYVALUE lp2, LPKEYVALUE lp2 )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
int CALLBACK fnKeyValueCompare( LPKEYVALUE lp1, LPKEYVALUE lp2, LONG l )
{
   return
      lstrcmpi( lp1->lpszKey, lp2->lpszKey ) ;


} /* fnKeyValueCompare()  */


/************************************************************************
 * End of File: keyvalue.c
 ***********************************************************************/

