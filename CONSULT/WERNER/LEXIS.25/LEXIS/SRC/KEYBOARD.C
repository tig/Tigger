/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  keyboard.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This module controls the keyboard handling for the terminal.
 *
 *   Revisions:  
 *     00.00.000  2/28/91 cek   first version
 *
 *
 ************************************************************************/
#include "..\inc\undef.h"
#include <windows.h>
#include <cmndll.h>
#include <memory.h>

#include "..\inc\lexis.h"
#include "..\inc\global.h"
#include "..\inc\term.h"
#include "..\inc\botstat.h"
#include "..\inc\keyboard.h"

short NEAR PASCAL LastCharOnLine( LPSTR lpsLine ) ;



/*************************************************************************
 *  BOOL FAR PASCAL KeyDownTerm( HWND hWnd, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *    this function handles the WM_KEYDOWN messages to the terminal window.
 *    The messages that are not processed here are sent on to the
 *    "command" module (command.c) (KeyDownCommand is called in term.c).
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL KeyDownTerm( HWND hWnd, WORD wParam, LONG lParam )
{
   PTERMBLOCK  pTB ;
   LOCALHANDLE lhTB ;
   BOOL        fProcess = TRUE ;
   short       OffsetY, OffsetX ;
   RECT        rc ;

   lhTB  = GETLHTB( hWnd ) ;

   if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
   {
      short i ;
      OffsetY = min( 0, (-pTB->nVscrollPos * pTB->nCharSizeY) ) ;
      OffsetX = min( 0, (-pTB->nHscrollPos * pTB->nCharSizeX) ) ;

      rc.top = pTB->nCurPosY + OffsetY ;
      rc.left = pTB->nCurPosX + OffsetX ;
      rc.bottom = rc.top + pTB->nCharSizeY ;
      rc.right = rc.left + pTB->nCharSizeX ;

      switch( wParam )
      {
         case VK_TAB:
         {
            HWND hwndButton = GETHWNDBUTTON( GetParent( hWnd ) )  ;
            pTB->fWantFocus = FALSE ;
            if (hwndButton)
               SetFocus( hwndButton ) ;
         }
         break ;

         case VK_HOME:
            pTB->nCurCol = 0 ;

         break ;

         case VK_END:
            i = LastCharOnLine( ROW_CHAR( pTB->nCurRow ) ) ;
            if (i == NUMCOLS)
               pTB->nCurCol = NUMCOLS - 1 ;
            else
               pTB->nCurCol = i ;
         break ;

         case VK_PRIOR:
            pTB->nCurRow = 0 ;
         break ;

         case VK_NEXT:
            pTB->nCurRow = NUMROWS - 1 ;
         break ;

         case VK_LEFT:
            if (pTB->nCurCol)
               pTB->nCurCol-- ;
         break ;

         case VK_RIGHT:
            if (pTB->nCurCol < NUMCOLS - 1)
               pTB->nCurCol++ ;
         break ;

         case VK_UP:
            if (pTB->nCurRow > 0)
               pTB->nCurRow-- ;
         break ;

         case VK_DOWN:
            if (pTB->nCurRow < NUMROWS - 1)
               pTB->nCurRow++ ;
         break ;

         case VK_BACK:
            if (pTB->nCurCol)
            {
               pTB->nCurCol-- ;
               PostMessage( hWnd, WM_KEYDOWN, VK_DELETE, 0L ) ;
            }
         break ;

         case VK_DELETE:
         {
            short x, y ;

            i = LastCharOnLine( ROW_CHAR( pTB->nCurRow ) ) ;
            x = pTB->nCurCol ;
            y = pTB->nCurRow ;

            _fmemcpy( ROW_CHAR( y ) + x, ROW_CHAR( y ) + x + 1, (NUMCOLS-1) - x ) ;
            CHARYX( y, NUMCOLS - 1 ) = CHAR_SPACE ;

            _fmemcpy( ROW_ATTR( y )+ x, ROW_ATTR( y )+ x + 1, (NUMCOLS-1) - x ) ;
            ATTRYX( y, NUMCOLS - 1 ) = ATTR_NORMAL ;

            rc.top = pTB->nCurPosY + OffsetY ;
            rc.bottom = rc.top + pTB->nCharSizeY ;
            rc.left = pTB->nCurPosX + OffsetX ;
            rc.right = rc.left + (i * pTB->nCharSizeX) ;
         }
         break ;

         default:
            CheckKeyStates( ) ;
            fProcess = FALSE ;

      }

      UNLOCKTERMBLOCK( lhTB ) ;
   }
   else
   {
      DP1( hWDB, "Could not local lock term block" ) ;
      fProcess = FALSE ;
   }

   if (fProcess)
   {
      pTB->nCurPosY = pTB->nCharSizeY * pTB->nCurRow ;
      pTB->nCurPosX = pTB->nCharSizeX * pTB->nCurCol ;

      InvalidateRect( hWnd, &rc, FALSE ) ;
      SetTermPos( hWnd ) ;
   }
   return fProcess ;

} /* KeyDownTerm() */


/*************************************************************************
 *  short NEAR PASCAL LastCharOnLine( LPSTR lpsLine )
 *
 *  Description: 
 *
 *    Sleazy little guy to figure out where the last non space character
 *    on the line is.
 *
 *  Comments:
 *
 *************************************************************************/
short NEAR PASCAL LastCharOnLine( LPSTR lpsLine )
{
   short i ;

   //
   // Scan from end of line to beginning of line, looking for
   // a non space char...  Return character to right of last...
   //
   for (i = NUMCOLS-1 ; i >= 0 ; i--)
      if (lpsLine[i] != CHAR_SPACE)
         break ;

   if ((i == -1 || i == 0) && lpsLine[0] == CHAR_SPACE)
      i = 0 ;
   else
      i++ ;

   return i ;
}/* LastCharOnLine() */


/*************************************************************************
 *  BOOL FAR PASCAL CharTerm( HWND hWnd, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *    This function handles WM_CHAR messages to the terminal window.
 *    All characters are processed here.  They go no further.  Except
 *    of course things like CHAR_CR which should cause a "transmit"
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL CharTerm( HWND hWnd, WORD wParam, LONG lParam )
{
   PTERMBLOCK  pTB ;
   LOCALHANDLE lhTB ;
   BOOL        fProcess = TRUE ;
   short       OffsetY, OffsetX ;
   RECT        rc ;
   short       nCurCol, nCurRow ;

   lhTB  = GETLHTB( hWnd ) ;

   if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
   {
      OffsetY = min( 0, (-pTB->nVscrollPos * pTB->nCharSizeY) ) ;
      OffsetX = min( 0, (-pTB->nHscrollPos * pTB->nCharSizeX) ) ;
      nCurCol = pTB->nCurCol ;
      nCurRow = pTB->nCurRow ;

      switch( wParam )
      {
         case CHAR_BS:
            // 
            // Ignore since it's handled in KeyDownTerm
            //
         break ;

         case CHAR_TAB:
            // 
            // Ignore since it's handled in KeyDownTerm
            //
         break ;

         case CHAR_LF:
            //
            // Move down one line
            //
         break ;

         case CHAR_CR:
            //
            // Go to beggining of current line.  Send transmit?
            //
         break ;

         case CHAR_ESC:
            //
            // Hmmm?  What to do with escape?
            // Ignore for now.
         break ;

         default:
            fProcess = FALSE ;
            if (pTB->fInsert)
               InsertChar( pTB, (char)wParam ) ;
            else
               AddChar( pTB, (char)wParam ) ;

      }

      UNLOCKTERMBLOCK( lhTB ) ;
   }
   else
   {
      DP1( hWDB, "Could not local lock term block" ) ;
      fProcess = FALSE ;
   }

   // 
   // Calculate changed rectangle.
   //
   rc.top = pTB->nCurPosY + OffsetY ;
   rc.bottom = rc.top + pTB->nCharSizeY * (pTB->nCurRow - nCurRow + 1) ;
   if (nCurCol >= pTB->nCurCol)
   {
      rc.left = 0 ;
      rc.right = pTB->nCharSizeX * NUMCOLS ;
   }
   else
   {
      rc.left = pTB->nCurPosX + OffsetX ;
      rc.right = rc.left + pTB->nCharSizeX * (pTB->nCurCol - nCurCol + 1) ;
   }

   pTB->nCurPosY = pTB->nCharSizeY * pTB->nCurRow ;
   pTB->nCurPosX = pTB->nCharSizeX * pTB->nCurCol ;

   InvalidateRect( hWnd, &rc, FALSE ) ;
   SetTermPos( hWnd ) ;

   return fProcess ;

} /* CharTerm() */

/*************************************************************************
 *  void FAR PASCAL AddChar( PTERMBLOCK pTB, char c )
 *
 *  Description: 
 *
 *    This function "adds" a character to the terminal.  It is used in
 *    "overstrike" mode (as opposed to "insert" mode).  It handles
 *    word wrap and screen wrap (sort of).
 *
 *  Comments:
 *    The InsertChar and AddChar functions are both called by the
 *    clipboard functions to "paste" data into the term.
 *
 *************************************************************************/
void FAR PASCAL AddChar( PTERMBLOCK pTB, char c )
{
   //
   // If current pos is last in line the check for a space.
   // if it's a space then put it in and go to next line, 0 column
   // if it's not a space then the current word needs to be
   //   word wrapped.
   // 
   if (pTB->nCurCol == NUMCOLS - 1 && c != CHAR_SPACE)
   {
      short i ;

      CHARYX( pTB->nCurRow, pTB->nCurCol ) = c ;
      ATTRYX( pTB->nCurRow, pTB->nCurCol ) = ATTR_NORMAL ;

      // word wrap
      // go back to last space on line.
      for (i = NUMCOLS - 2 ; i >= 0 ; i--)
         if (CHARYX( pTB->nCurRow, i ) == CHAR_SPACE)
            break ;

      //
      // i now points to last space or first char
      // 
      if (i >= 0 && (pTB->nCurRow < NUMROWS - 1))
      {
         _fmemcpy( ROW_CHAR( pTB->nCurRow + 1 ), ROW_CHAR( pTB->nCurRow ) + i + 1,
                   (NUMCOLS - 1 - i) ) ;
  
         _fmemset( ROW_CHAR( pTB->nCurRow ) + i + 1, CHAR_SPACE,
                   (NUMCOLS - 1 - i) ) ;

         _fmemcpy( ROW_ATTR( pTB->nCurRow + 1 ), ROW_ATTR( pTB->nCurRow ) + i + 1,
                   (NUMCOLS - 1 - i) ) ;
  
         _fmemset( ROW_ATTR( pTB->nCurRow ) + i + 1, CHAR_SPACE,
                   (NUMCOLS - 1 - i) ) ;

         pTB->nCurRow++ ;
         pTB->nCurCol = NUMCOLS - 1 - i ;
      }
      else
      {
         //
         // no spaces on line.
         //
         if (pTB->nCurRow < NUMROWS - 1)
            pTB->nCurRow++ ;
         else
            pTB->nCurRow = 0 ;
         pTB->nCurCol = 0 ;
      }
   }
   else
   {
      // no word wrap
      //
      CHARYX( pTB->nCurRow, pTB->nCurCol ) = c ;
      ATTRYX( pTB->nCurRow, pTB->nCurCol ) = ATTR_NORMAL ;
      if (pTB->nCurCol < NUMCOLS - 1) 
         pTB->nCurCol++ ;
      else
      {
         if (pTB->nCurRow < NUMROWS - 1)
            pTB->nCurRow++ ;
         else
            pTB->nCurRow = 0 ;
         pTB->nCurCol = 0 ;
      }
   }
}/* AddChar() */

/*************************************************************************
 *  void FAR PASCAL InsertChar( PTERMBLOCK pTB, char c )
 *
 *  Description: 
 *
 *    This function handles the inserting of characters in "insert"
 *    mode.
 *
 *  Comments:
 *    The InsertChar and AddChar functions are both called by the
 *    clipboard functions to "paste" data into the term.
 *
 *************************************************************************/
void FAR PASCAL InsertChar( PTERMBLOCK pTB, char c )
{
}/* InsertChar() */


/************************************************************************
 * End of File: keyboard.c
 ************************************************************************/

