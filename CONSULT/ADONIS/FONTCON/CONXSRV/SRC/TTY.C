/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CONXSRV
 *
 *      Module:  tty.c
 *
 *     Remarks:  
 *               It provides a simple TTY window for debugging.
 *
 *               This code originated in the 3.1 sample TTY app.
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "CONXSRV.h"

/*
 * Color stuff
 */
COLORREF rgColors[] =
{
   /* CLR_BLACK      */    RGB(0,0,0),
   /* CLR_WHITE      */    RGB(255,255,255),
   /* CLR_GRAY       */    RGB(128,128,128),
   /* CLR_LTGRAY     */    RGB(192,192,192),
   /* CLR_BLUE       */    RGB(0,0,128),
   /* CLR_LTBLUE     */    RGB(0,0,255),
   /* CLR_GREEN      */    RGB(0,128,0),
   /* CLR_CYAN       */    RGB(0,128,128),
   /* CLR_LTGREEN    */    RGB(0,255,0),
   /* CLR_LTCYAN     */    RGB(0,255,255),
   /* CLR_RED        */    RGB(128,0,0),
   /* CLR_MAGENTA    */    RGB(128,0,128),
   /* CLR_BROWN      */    RGB(128,128,0),
   /* CLR_LTRED      */    RGB(255,0,0),
   /* CLR_LTMAGENTA  */    RGB(255,0,255),
   /* CLR_YELLOW     */    RGB(255,255,0),
} ;

extern BOOL fCommConnected ;

BOOL fBinaryTTY ;

BOOL     NEAR ScrollTTYHorz( HWND, UINT, UINT ) ;
BOOL     NEAR ScrollTTYVert( HWND, UINT, UINT ) ;
BOOL     NEAR SizeTTY( HWND, UINT, UINT ) ;
BOOL     NEAR MoveTTYCursor( HWND ) ;
BOOL     NEAR PaintTTY( HWND ) ;

// Exported functions

LRESULT  CALLBACK fnTTYWnd( HWND, UINT, WPARAM, LPARAM ) ;

LRESULT CALLBACK fnTTYWnd( HWND hWnd, UINT wMsg,
                               WPARAM wParam, LPARAM lParam )
{
   switch (wMsg)
   {
      case WM_CREATE:
         return ( CreateTTYInfo( hWnd ) ) ;
         break ;

      case WM_COMMNOTIFY:
         ProcessCOMMNotification( (UINT) wParam, (LONG) lParam ) ;
         break ;

      case WM_PAINT:
         PaintTTY( hWnd ) ;
         break ;

      case WM_SIZE:
         SizeTTY( hWnd, HIWORD( lParam ), LOWORD( lParam ) ) ;
         break ;

      case WM_HSCROLL:
         ScrollTTYHorz( hWnd, (UINT) wParam, LOWORD( lParam ) ) ;
         break ;

      case WM_VSCROLL:
         ScrollTTYVert( hWnd, (UINT) wParam, LOWORD( lParam ) ) ;
         break ;

      case WM_CHAR:
         ProcessTTYCharacter( hWnd, LOBYTE( wParam ) ) ;
         break ;

      case WM_SETFOCUS:
         SetTTYFocus( hWnd ) ;
         break ;

      case WM_KILLFOCUS:
         KillTTYFocus( hWnd ) ;
         break ;

      case WM_DESTROY:
         DestroyTTYInfo( hWnd ) ;
         break ;

      default:
         return( DefWindowProc( hWnd, wMsg, wParam, lParam ) ) ;
   }
   return 0L ;

} // end of fnTTYWnd()

/****************************************************************
 *  VOID WINAPI SetTTYBinary( BOOL f )
 *
 *  Description: 
 *
 *    Sets the TTY terminal to binary mode (i.e all ctrl chars
 *    are displayed as <..>
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI SetTTYBinary( BOOL f )
{
   BOOL fPrev = fBinaryTTY ;

   fBinaryTTY = f ;

   return fPrev ;

} /* SetTTYBinary()  */

//---------------------------------------------------------------------------
//  LRESULT WINAPI CreateTTYInfo( HWND hWnd )
//
//  Description:
//     Creates the tty information structure and sets
//     menu option availability.  Returns -1 if unsuccessful.
//
//  Parameters:
//     HWND  hWnd
//        Handle to main window.
//
//  History:   Date       Author      Comment
//             10/18/91   BryanW      Pulled from tty window proc.
//              1/13/92   BryanW      Fixed bug with invalid handle
//                                    caused by WM_SIZE sent by
//                                    ResetTTYScreen().
//
//---------------------------------------------------------------------------

LRESULT WINAPI CreateTTYInfo( HWND hWnd )
{
#ifdef DISABLETTY         
   return 0L ;
#else

   PTTYINFO   pTTYInfo ;

   // set TTYInfo handle into HWND extra bytes

   SetWindowLong( hWnd, GWL_TTYINFO, (LPARAM)(DWORD)ALLOCTTYINFO() ) ;

   if (NULL == (pTTYInfo = GETTTYINFO( hWnd )))
      return ( (LRESULT) -1 ) ;

   // initialize TTY info structure

   CURSORSTATE( pTTYInfo )   = CS_HIDE ;
   LOCALECHO( pTTYInfo )     = TRUE ;
   AUTOWRAP( pTTYInfo )      = TRUE ;
   NEWLINE( pTTYInfo )       = FALSE ;
   XSIZE( pTTYInfo )         = 0 ;
   YSIZE( pTTYInfo )         = 0 ;
   XSCROLL( pTTYInfo )       = 0 ;
   YSCROLL( pTTYInfo )       = 0 ;
   XOFFSET( pTTYInfo )       = 0 ;
   YOFFSET( pTTYInfo )       = 0 ;
   COLUMN( pTTYInfo )        = 0 ;
   ROW( pTTYInfo )           = MAXROWS - 1 ;
   HTTYFONT( pTTYInfo )      = NULL ;

   // clear screen space

   _fmemset( SCREEN( pTTYInfo ), ' ', MAXROWS * MAXCOLS ) ;
   _fmemset( ATTRIB( pTTYInfo ), (BYTE)CLR_BLACK, MAXROWS * MAXCOLS ) ;

   //
   // If we're using global memory (#define USE_GLOBALMEM) then
   // the following macro is a no-op.
   //
   UNGETTTYINFO( pTTYInfo ) ;

   // reset the character information, etc.

   ResetTTYScreen( hWnd ) ;

   return ( (LRESULT) TRUE ) ;

#endif

} // end of CreateTTYInfo()

//---------------------------------------------------------------------------
//  BOOL WINAPI DestroyTTYInfo( HWND hWnd )
//
//  Description:
//     Destroys block associated with TTY window handle.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI DestroyTTYInfo( HWND hWnd )
{
#ifndef DISABLETTY         
   PTTYINFO pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   FREETTYINFO( hWnd ) ;

#endif
   return ( TRUE ) ;

} // end of DestroyTTYInfo()

//---------------------------------------------------------------------------
//  BOOL WINAPI ResetTTYScreen( HWND hWnd )
//
//  Description:
//     Resets the TTY character information and causes the
//     screen to resize to update the scroll information.
//
//  History:   Date       Author      Comment
//             10/20/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI ResetTTYScreen( HWND hWnd )
{
#ifndef DISABLETTY         
   HDC         hDC ;
   PTTYINFO    pTTYInfo ;
   TEXTMETRIC  tm ;
   RECT        rcWindow ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   HTTYFONT( pTTYInfo ) = hfontCur ;

   hDC = GetDC( hWnd ) ;
   SelectObject( hDC, HTTYFONT( pTTYInfo ) ) ;
   GetTextMetrics( hDC, &tm ) ;
   ReleaseDC( hWnd, hDC ) ;

   XCHAR( pTTYInfo ) = tm.tmAveCharWidth  ;
   YCHAR( pTTYInfo ) = tm.tmHeight + tm.tmExternalLeading ;

   // a slimy hack to force the scroll position, region to
   // be recalculated based on the new character sizes

   GetWindowRect( hWnd, &rcWindow ) ;
   SendMessage( hWnd, WM_SIZE, SIZENORMAL,
                (LPARAM) MAKELONG( rcWindow.right - rcWindow.left,
                                   rcWindow.bottom - rcWindow.top ) ) ;

   ScrollTTYVert( hWnd, SB_BOTTOM, 0 ) ;

   UNGETTTYINFO( pTTYInfo ) ;

#endif

   return ( TRUE ) ;
} // end of ResetTTYScreen()

//---------------------------------------------------------------------------
//  BOOL WINAPI SetTTYFocus( HWND hWnd )
//
//  Description:
//     Sets the focus to the TTY window also creates caret.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI SetTTYFocus( HWND hWnd )
{
#ifndef DISABLETTY         
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   if (fCommConnected && (CURSORSTATE( pTTYInfo ) != CS_SHOW))
   {
      CreateCaret( hWnd, NULL, XCHAR( pTTYInfo ), YCHAR( pTTYInfo ) ) ;
      ShowCaret( hWnd ) ;
      CURSORSTATE( pTTYInfo ) = CS_SHOW ;
   }
   MoveTTYCursor( hWnd ) ;

   UNGETTTYINFO( pTTYInfo ) ;

#endif
   return ( TRUE ) ;

} // end of SetTTYFocus()

//---------------------------------------------------------------------------
//  BOOL WINAPI KillTTYFocus( HWND hWnd )
//
//  Description:
//     Kills TTY focus and destroys the caret.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI KillTTYFocus( HWND hWnd )
{
#ifndef DISABLETTY         
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   if (fCommConnected && (CURSORSTATE( pTTYInfo ) != CS_HIDE))
   {
      HideCaret( hWnd ) ;
      DestroyCaret() ;
      CURSORSTATE( pTTYInfo ) = CS_HIDE ;
   }

   UNGETTTYINFO( pTTYInfo ) ;

#endif   
   return ( TRUE ) ;

} // end of KillTTYFocus()

//---------------------------------------------------------------------------
//  BOOL WINAPI IsTTYConnected( HWND hWnd )
//
//  Description:
//    Exported routine to check if we're connected or not.  The TTY 
//    sample app does not call this routine.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              2/15/92   ckindel     Wrote it.
//
//---------------------------------------------------------------------------
BOOL WINAPI IsTTYConnected( HWND hWnd )
{
   return fCommConnected ;
}

//---------------------------------------------------------------------------
//  BOOL WINAPI ProcessTTYCharacter( HWND hWnd, BYTE bOut )
//
//  Description:
//     This simply writes a character to the port and echos it
//     to the TTY screen if fLocalEcho is set.  Some minor
//     keyboard mapping could be performed here.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     BYTE bOut
//        byte from keyboard
//
//  History:   Date       Author      Comment
//              5/11/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI ProcessTTYCharacter( HWND hWnd, BYTE bOut )
{
#ifndef DISABLETTY         
   PTTYINFO  pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   if (!fCommConnected)
   {
      UNGETTTYINFO( pTTYInfo ) ;
      return ( FALSE ) ;
   }

   WriteCommByte( bOut ) ;
   if (LOCALECHO( pTTYInfo ))
   {
      BOOL b ;

      b = SetTTYBinary( FALSE ) ;
      WriteTTYBlock( hWnd, &bOut, 1, KEYBOARD_COLOR ) ;
      SetTTYBinary( b ) ;
      UpdateWindow( hWnd ) ;
   }

   UNGETTTYINFO( pTTYInfo ) ;

#endif
   return ( TRUE ) ;

} // end of ProcessTTYCharacter()


//---------------------------------------------------------------------------
//  BOOL WINAPI WriteTTYBlock( HWND hWnd, LPSTR lpBlock, int nLength )
//
//  Description:
//     Writes block to TTY screen.  Nothing fancy - just
//     straight TTY.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     LPSTR lpBlock
//        far pointer to block of data
//
//     int  nLength
//        length of block
//
//     int  nColorID
//        Color ID to be used.
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//              5/20/91   BryanW      Modified... not character based,
//                                    block based now.  It was processing
//                                    per char.
//              3/27/92   Ckindel     Modified to accept uiFlags and use
//                                    attributes.
//
//---------------------------------------------------------------------------

BOOL WINAPI WriteTTYBlock( HWND hWnd, LPSTR lpBlock, int nLength, int nColorID )
{
#ifndef DISABLETTY         
   int        i ;
   PTTYINFO   pTTYInfo ;
   RECT       rect ;

   if (hWnd == 0)
      return FALSE ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   for (i = 0 ; i < nLength; i++)
   {
      if (fBinaryTTY)
         goto OutputChar ;

      switch (lpBlock[ i ])
      {
         case ASCII_CR:
            // Carriage return
            COLUMN( pTTYInfo ) = 0 ;
            MoveTTYCursor( hWnd ) ;
            if (!NEWLINE( pTTYInfo ))
               break;
   
            // fall through
   
         case ASCII_LF:
            // Line feed
            if (ROW( pTTYInfo )++ == MAXROWS - 1)
            {
               _fmemmove( (LPSTR) (SCREEN( pTTYInfo )),
                          (LPSTR) (SCREEN( pTTYInfo ) + MAXCOLS),
                          (MAXROWS - 1) * MAXCOLS ) ;
               _fmemset( (LPSTR) (SCREEN( pTTYInfo ) + (MAXROWS - 1) * MAXCOLS),
                         ' ', MAXCOLS ) ;

               _fmemmove( (LPSTR) (ATTRIB( pTTYInfo )),
                          (LPSTR) (ATTRIB( pTTYInfo ) + MAXCOLS),
                          (MAXROWS - 1) * MAXCOLS ) ;
               _fmemset( (LPSTR) (ATTRIB( pTTYInfo ) + (MAXROWS - 1) * MAXCOLS),
                         (BYTE)CLR_BLACK, MAXCOLS ) ;

               InvalidateRect( hWnd, NULL, FALSE ) ;
               ROW( pTTYInfo )-- ;
            }
            MoveTTYCursor( hWnd ) ;
            break ;

         case ASCII_TAB:
            if (EXPANDTABS)
            {
               int n ;

               // tab
               //
               n = TABSIZE - (COLUMN ( pTTYInfo ) % TABSIZE) ;
               WriteTTYBlock( hWnd, "         ", n, nColorID ) ;
            }
            else
               goto OutputChar ;
         break ;

         case ASCII_BEL:
            if (EXPANDBELLS)
            {
               // Bell
               MessageBeep( 0 ) ;
            }
            else
               goto OutputChar ;
         break ;

         case ASCII_BS:
            if (EXPANDBS)
            {
               // Backspace
               if (COLUMN( pTTYInfo ) > 0)
                  COLUMN( pTTYInfo ) -- ;
               MoveTTYCursor( hWnd ) ;
            }
            else
               goto OutputChar ;
         break ;

         default:
OutputChar:
            if (fBinaryTTY || (DISPLAYCODES && (lpBlock[i] < 0x20 || lpBlock[i] > 0x7e)))
            {
               char szBuf[16] ;

               if ((BYTE)lpBlock[i] < (BYTE)0x20)
                  wsprintf( szBuf, "<%s>", (LPSTR)rglpsz[IDS_ASCII_NUL + (UINT)lpBlock[i]] ) ;
               else
                  wsprintf( szBuf, "<%02X>", (UINT)(0x00FF & lpBlock[i]) ) ;

               if (fBinaryTTY)
               {
                  /*
                  * We call ourselves recursivly to put our translated
                  * codes in the tty window.  Set the global flag before
                  * we do this or else we'll go infinte!
                  */
                  fBinaryTTY = FALSE ;
                  WriteTTYBlock( hWnd, szBuf, lstrlen(szBuf), nColorID ) ;
                  fBinaryTTY = TRUE ;
               }
               else
                  WriteTTYBlock( hWnd, szBuf, lstrlen(szBuf), nColorID ) ;
               break ;
            }

            *(SCREEN( pTTYInfo ) + ROW( pTTYInfo ) * MAXCOLS +
                COLUMN( pTTYInfo )) = lpBlock[ i ] ;

            *(ATTRIB( pTTYInfo ) + ROW( pTTYInfo ) * MAXCOLS +
                COLUMN( pTTYInfo )) = (BYTE)nColorID ;

            rect.left = (COLUMN( pTTYInfo ) * XCHAR( pTTYInfo )) -
                        XOFFSET( pTTYInfo ) ;
            rect.right = rect.left + XCHAR( pTTYInfo ) ;
            rect.top = (ROW( pTTYInfo ) * YCHAR( pTTYInfo )) -
                       YOFFSET( pTTYInfo ) ;
            rect.bottom = rect.top + YCHAR( pTTYInfo ) ;
            InvalidateRect( hWnd, &rect, FALSE ) ;

            // Line wrap
            if (COLUMN( pTTYInfo ) < MAXCOLS - 1)
               COLUMN( pTTYInfo )++ ;
            else if (AUTOWRAP( pTTYInfo ))
               WriteTTYBlock( hWnd, "\r\n", 2, nColorID ) ;
            break;
      }
   }

   UNGETTTYINFO( pTTYInfo ) ;

#endif
   return ( TRUE ) ;

} // end of WriteTTYBlock()

//---------------------------------------------------------------------------
//  BOOL WINAPI SelectTTYFont( HWND hDlg )
//
//  Description:
//     Selects the current font for the TTY screen.
//     Uses the Common Dialog ChooseFont() API.
//
//  Parameters:
//     HWND hDlg
//        handle to settings dialog
//
//  History:   Date       Author      Comment
//             10/20/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI SelectTTYFont( HWND hDlg )
{
#ifndef DISABLETTY         

   CHOOSEFONT  cfTTYFont ;
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( GET_HWNDTTY_PROP( hDlg ) )))
      return FALSE ;

   cfTTYFont.lStructSize    = sizeof( CHOOSEFONT ) ;
   cfTTYFont.hwndOwner      = hDlg ;
   cfTTYFont.hDC            = NULL ;
   //cfTTYFont.lpLogFont      = &LFTTYFONT( pTTYInfo ) ;
   cfTTYFont.Flags          = CF_SCREENFONTS | CF_FIXEDPITCHONLY |
                              CF_EFFECTS | CF_INITTOLOGFONTSTRUCT ;
   cfTTYFont.lCustData      = NULL ;
   cfTTYFont.lpfnHook       = NULL ;
   cfTTYFont.lpTemplateName = NULL ;
   cfTTYFont.hInstance      = GETHINST( hDlg ) ;

   if (ChooseFont( &cfTTYFont ))
   {
     ResetTTYScreen( GET_HWNDTTY_PROP( hDlg ) ) ;
   }

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

#endif
} // end of SelectTTYFont()

//---------------------------------------------------------------------------
//  BOOL NEAR PaintTTY( HWND hWnd )
//
//  Description:
//     Paints the rectangle determined by the paint struct of
//     the DC.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window (as always)
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//             10/22/91   BryanW      Problem with background color
//                                    and "off by one" fixed.
//
//---------------------------------------------------------------------------

BOOL NEAR PaintTTY( HWND hWnd )
{
#ifndef DISABLETTY         
   int          nRow, nCol, nEndRow, nEndCol, nCount, nHorzPos, nVertPos ;
   int          nCurCol, nRowIndex ;
   int          nCurIndex ; /* lots of locals for speed */
   HDC          hDC ;
   HFONT        hOldFont ;
   PTTYINFO     pTTYInfo ;
   PAINTSTRUCT  ps ;
   RECT         rect ;
   BYTE         bAttrib ;
   COLORREF     rgbColorWindow, rgbColorHighlight ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   hDC = BeginPaint( hWnd, &ps ) ;
   hOldFont = SelectObject( hDC, HTTYFONT( pTTYInfo ) ) ;
   rgbColorWindow = GetSysColor( COLOR_WINDOW ) ;
   rgbColorHighlight = GetSysColor( COLOR_HIGHLIGHT ) ;
   SetBkColor( hDC, rgbColorWindow ) ;

   rect = ps.rcPaint ;
   nRow =
      min( MAXROWS - 1,
           max( 0, (rect.top + YOFFSET( pTTYInfo )) / YCHAR( pTTYInfo ) ) ) ;
   nEndRow =
      min( MAXROWS - 1,
           ((rect.bottom + YOFFSET( pTTYInfo ) - 1) / YCHAR( pTTYInfo ) ) ) ;
   nCol =
      min( MAXCOLS - 1,
           max( 0, (rect.left + XOFFSET( pTTYInfo )) / XCHAR( pTTYInfo ) ) ) ;
   nEndCol =
      min( MAXCOLS - 1,
           ((rect.right + XOFFSET( pTTYInfo ) - 1) / XCHAR( pTTYInfo ) ) ) ;
   nCount = nEndCol - nCol + 1 ;

   for (; nRow <= nEndRow; nRow++)
   {
      nVertPos = (nRow * YCHAR( pTTYInfo )) - YOFFSET( pTTYInfo ) ;

      nRowIndex = nRow * MAXCOLS ;

      rect.top = nVertPos ;
      rect.bottom = nVertPos + YCHAR( pTTYInfo ) ;

      /*
       * This is a little 'hacky'.  By adding attributes to the tty code
       * it slowed down a bunch.  The main reason for the slow down was the
       * call to SetTextColor() for each character painted.  The code
       * below rectifies this by looking for runs with the same attribute
       * and only calling SetTextColor() once for each run.
       */
      for (nCount = 1, nCurCol = nCol ; nCurCol <= nEndCol ; nCurCol += nCount)
      {
         nHorzPos = (nCurCol * XCHAR( pTTYInfo )) - XOFFSET( pTTYInfo ) ;
         rect.left = nHorzPos ;

         nCurIndex = nRowIndex + nCurCol ;             // reduce number of ops
         bAttrib = *(ATTRIB( pTTYInfo ) + nCurIndex) ; // by putting these in locals

         for (nCount = 1 ;
              (nCurCol + nCount <= nEndCol) &&
              (bAttrib == *(ATTRIB( pTTYInfo ) + nCurIndex + nCount)) ;
              nCount++ )
               ;

         rect.right = nHorzPos + (XCHAR( pTTYInfo ) * nCount) ;

         /*
          * This is cool.  By setting the high bit of the attribute
          * byte we set the background color to highlight.  Neato.
          */
         if (bAttrib & 0x80)
            SetBkColor( hDC, rgbColorHighlight ) ;
         else
            SetBkColor( hDC, rgbColorWindow ) ;

         SetTextColor( hDC, rgColors[bAttrib & 0x7F] ) ;
         ExtTextOut( hDC,
                     nHorzPos, nVertPos,
                     ETO_OPAQUE,
                     &rect,
                     (LPSTR)( SCREEN( pTTYInfo ) + nCurIndex ),
                     nCount, NULL ) ;
      }
   }
   SelectObject( hDC, hOldFont ) ;
   EndPaint( hWnd, &ps ) ;
   MoveTTYCursor( hWnd ) ;

   UNGETTTYINFO( pTTYInfo ) ;

#endif
   return ( TRUE ) ;

} // end of PaintTTY()

//---------------------------------------------------------------------------
//  BOOL NEAR SizeTTY( HWND hWnd, UINT wVertSize, UINT wHorzSize )
//
//  Description:
//     Sizes TTY and sets up scrolling regions.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     UINT wVertSize
//        new vertical size
//
//     UINT wHorzSize
//        new horizontal size
//
//  History:   Date       Author      Comment
//              5/ 8/ 91  BryanW      Wrote it
//
//---------------------------------------------------------------------------

BOOL NEAR SizeTTY( HWND hWnd, UINT wVertSize, UINT wHorzSize )
{
#ifndef DISABLETTY         

   int        nScrollAmt ;
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   YSIZE( pTTYInfo ) = (int) wVertSize ;
   YSCROLL( pTTYInfo ) = max( 0, (MAXROWS * YCHAR( pTTYInfo )) -
                               YSIZE( pTTYInfo ) ) ;
   nScrollAmt = min( YSCROLL( pTTYInfo ), YOFFSET( pTTYInfo ) ) -
                     YOFFSET( pTTYInfo ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;

   YOFFSET( pTTYInfo ) = YOFFSET( pTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_VERT, YOFFSET( pTTYInfo ), FALSE ) ;
   SetScrollRange( hWnd, SB_VERT, 0, YSCROLL( pTTYInfo ), TRUE ) ;

   XSIZE( pTTYInfo ) = (int) wHorzSize ;
   XSCROLL( pTTYInfo ) = max( 0, (MAXCOLS * XCHAR( pTTYInfo )) -
                                XSIZE( pTTYInfo ) ) ;
   nScrollAmt = min( XSCROLL( pTTYInfo ), XOFFSET( pTTYInfo )) -
                     XOFFSET( pTTYInfo ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;
   XOFFSET( pTTYInfo ) = XOFFSET( pTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_HORZ, XOFFSET( pTTYInfo ), FALSE ) ;
   SetScrollRange( hWnd, SB_HORZ, 0, XSCROLL( pTTYInfo ), TRUE ) ;

   InvalidateRect( hWnd, NULL, TRUE ) ;

   UNGETTTYINFO( pTTYInfo ) ;

#endif
   return ( TRUE ) ;

} // end of SizeTTY()

//---------------------------------------------------------------------------
//  BOOL NEAR ScrollTTYVert( HWND hWnd, UINT wScrollCmd, UINT wScrollPos )
//
//  Description:
//     Scrolls TTY window vertically.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     UINT wScrollCmd
//        type of scrolling we're doing
//
//     UINT wScrollPos
//        scroll position
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR ScrollTTYVert( HWND hWnd, UINT wScrollCmd, UINT wScrollPos )
{
#ifndef DISABLETTY         

   int        nScrollAmt ;
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   switch (wScrollCmd)
   {
      case SB_TOP:
         nScrollAmt = -YOFFSET( pTTYInfo ) ;
         break ;

      case SB_BOTTOM:
         nScrollAmt = YSCROLL( pTTYInfo ) - YOFFSET( pTTYInfo ) ;
         break ;

      case SB_PAGEUP:
         nScrollAmt = -YSIZE( pTTYInfo ) ;
         break ;

      case SB_PAGEDOWN:
         nScrollAmt = YSIZE( pTTYInfo ) ;
         break ;

      case SB_LINEUP:
         nScrollAmt = -YCHAR( pTTYInfo ) ;
         break ;

      case SB_LINEDOWN:
         nScrollAmt = YCHAR( pTTYInfo ) ;
         break ;

      case SB_THUMBPOSITION:
         nScrollAmt = wScrollPos - YOFFSET( pTTYInfo ) ;
         break ;

      default:
         UNGETTTYINFO( pTTYInfo ) ;
         return ( FALSE ) ;
   }
   if ((YOFFSET( pTTYInfo ) + nScrollAmt) > YSCROLL( pTTYInfo ))
      nScrollAmt = YSCROLL( pTTYInfo ) - YOFFSET( pTTYInfo ) ;
   if ((YOFFSET( pTTYInfo ) + nScrollAmt) < 0)
      nScrollAmt = -YOFFSET( pTTYInfo ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;
   YOFFSET( pTTYInfo ) = YOFFSET( pTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_VERT, YOFFSET( pTTYInfo ), TRUE ) ;

   UNGETTTYINFO( pTTYInfo ) ;

#endif
   return ( TRUE ) ;

} // end of ScrollTTYVert()

//---------------------------------------------------------------------------
//  BOOL NEAR ScrollTTYHorz( HWND hWnd, UINT wScrollCmd, UINT wScrollPos )
//
//  Description:
//     Scrolls TTY window horizontally.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     UINT wScrollCmd
//        type of scrolling we're doing
//
//     UINT wScrollPos
//        scroll position
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR ScrollTTYHorz( HWND hWnd, UINT wScrollCmd, UINT wScrollPos )
{
#ifndef DISABLETTY         
   int        nScrollAmt ;
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   switch (wScrollCmd)
   {
      case SB_TOP:
         nScrollAmt = -XOFFSET( pTTYInfo ) ;
         break ;

      case SB_BOTTOM:
         nScrollAmt = XSCROLL( pTTYInfo ) - XOFFSET( pTTYInfo ) ;
         break ;

      case SB_PAGEUP:
         nScrollAmt = -XSIZE( pTTYInfo ) ;
         break ;

      case SB_PAGEDOWN:
         nScrollAmt = XSIZE( pTTYInfo ) ;
         break ;

      case SB_LINEUP:
         nScrollAmt = -XCHAR( pTTYInfo ) ;
         break ;

      case SB_LINEDOWN:
         nScrollAmt = XCHAR( pTTYInfo ) ;
         break ;

      case SB_THUMBPOSITION:
         nScrollAmt = wScrollPos - XOFFSET( pTTYInfo ) ;
         break ;

      default:
         UNGETTTYINFO( pTTYInfo ) ;
         return ( FALSE ) ;
   }
   if ((XOFFSET( pTTYInfo ) + nScrollAmt) > XSCROLL( pTTYInfo ))
      nScrollAmt = XSCROLL( pTTYInfo ) - XOFFSET( pTTYInfo ) ;
   if ((XOFFSET( pTTYInfo ) + nScrollAmt) < 0)
      nScrollAmt = -XOFFSET( pTTYInfo ) ;
   ScrollWindow( hWnd, -nScrollAmt, 0, NULL, NULL ) ;
   XOFFSET( pTTYInfo ) = XOFFSET( pTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_HORZ, XOFFSET( pTTYInfo ), TRUE ) ;

   UNGETTTYINFO( pTTYInfo ) ;

#endif
   return ( TRUE ) ;

} // end of ScrollTTYHorz()

//---------------------------------------------------------------------------
//  BOOL NEAR MoveTTYCursor( HWND hWnd )
//
//  Description:
//     Moves caret to current position.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR MoveTTYCursor( HWND hWnd )
{
#ifndef DISABLETTY         
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   if (fCommConnected && (CURSORSTATE( pTTYInfo ) & CS_SHOW))
      SetCaretPos( (COLUMN( pTTYInfo ) * XCHAR( pTTYInfo )) -
                   XOFFSET( pTTYInfo ),
                   (ROW( pTTYInfo ) * YCHAR( pTTYInfo )) -
                   YOFFSET( pTTYInfo ) ) ;
   
   UNGETTTYINFO( pTTYInfo ) ;

#endif
   return ( TRUE ) ;

} // end of MoveTTYCursor()


/************************************************************************
 * End of File: tty.c
 ***********************************************************************/

