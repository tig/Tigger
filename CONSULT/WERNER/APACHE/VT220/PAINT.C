/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  paint.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  VT220 Terminal Emulation (paint routines)
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.002  2/28/91 baw   Moved to self-contained DLL
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "vt220.h"

/************************************************************************
 *  BOOL FAR PASCAL PaintEmulator( HWND hWnd )
 *
 *  Description:
 *     This function paints the current invalid rect.  It handles
 *     all attribute processing and performs output by determining
 *     the run length of common attribute strings.
 *
 *     Discovery: rects are not inclusive - this code computes the
 *     characters to be displayed by using a modified equation
 *     (subtracts 1 from top and right).
 *
 *  Comments:
 *     6/18/91  baw  Wrote this comment block.
 *
 ************************************************************************/

BOOL FAR PASCAL PaintEmulator( HWND hWnd )
{
   int          nRow, nCol, nEndRow, nEndCol, nCount, nCurPos, nRun, nMaxCol ;
   BOOL         fFirstPass ;
   BYTE         bAttr ;
   HDC          hDC ;
   LOCALHANDLE  hECB ;
   NPECB        npECB ;
   PAINTSTRUCT  ps ;

   // Get Emulator Control Block

   hECB = GETECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;
   hDC = BeginPaint( hWnd, &ps ) ;

   nRow = max( 0, (ps.rcPaint.top + YOFFSET) / YCHAR ) ;
   nEndRow = min( MAXROWS - 1, (ps.rcPaint.bottom - 1 + YOFFSET) / YCHAR ) ;
   nCol = max( 0, (ps.rcPaint.left + XOFFSET) / XCHAR ) ;
   nEndCol = min( MAXCOLS - 1, (ps.rcPaint.right - 1 + XOFFSET) / XCHAR ) ;
   nCount = nEndCol - nCol + 1 ;

   fFirstPass = TRUE ;
   for (; nRow <= nEndRow; nRow++ )
   {
      nCurPos = nCol ;
      nMaxCol = nEndCol ;
      while (nCurPos <= nMaxCol)
      {
         bAttr = *(LPSTR) (ATTRBUF + nRow * MAXCOLS + nCurPos) ;
         if (bAttr & (BYTE) ATTRIBUTE_DBLSIZE)
            nMaxCol = nEndCol / 2 ;
         nRun = 1 ;
         while ((nCurPos + nRun <= nMaxCol) &&
                (bAttr == (BYTE) *(LPSTR) (ATTRBUF + nRow * MAXCOLS + nCurPos + nRun)))
            nRun++ ;

         PaintRun( npECB, hDC, bAttr, nRow, nCol, nRun, fFirstPass ) ;

         if (fFirstPass)
            fFirstPass = FALSE ;

         nCurPos += nRun ;
      }
   }
   EndPaint( hWnd, &ps ) ;

   LocalUnlock( hECB ) ;

   return ( TRUE ) ;

} /* end of PaintEmulator() */

/************************************************************************
 *  VOID NEAR PASCAL PaintRun( NPECB npECB, HDC hDC, BYTE bAttr,
 *                             int nRow, int nCol, int nRun, BOOL fReset )
 *
 *  Description:
 *     This function is provided so that a call can be made from
 *     the write function.
 *
 *  Comments:
 *      7/11/91  baw  Pulled from the paint function.
 *
 ************************************************************************/

VOID NEAR PASCAL PaintRun( NPECB npECB, HDC hDC, BYTE bAttr,
                      int nRow, int nCol, int nRun, BOOL fReset )
{
   int     nHorzPos, nVertPos, nFont ;
   RECT    rcTemp, rcPaint ;

   static int   nLastFont ;
   static BYTE  bLastAttr ;

   nFont = GetFontNumber( bAttr ) ;
   if ((nLastFont != nFont) || fReset)
   {
      // select font into DC

      SelectObject( hDC, EMULATORFONT( nFont ) ) ;
      nLastFont = nFont ;
   }

   if (bLastAttr != bAttr || fReset)
   {
      // set colors
 
      if ((bAttr & (BYTE) ATTRIBUTE_BLINKING) && BLINKSTATE)
      {
         // set color for blank

         SetTextColor( hDC, NORMALFONT.lfFGcolor ) ;
         SetBkColor( hDC, NORMALFONT.lfBGcolor ) ;
      }
      else if (bAttr & (BYTE) ATTRIBUTE_INVERSE)
      {
         if (bAttr & (BYTE) ATTRIBUTE_BOLD)
         {
            SetTextColor( hDC, HIGHLIGHTFONT.lfBGcolor ) ;
            SetBkColor( hDC, HIGHLIGHTFONT.lfFGcolor ) ;
         }
         else
         {
            SetTextColor( hDC, NORMALFONT.lfBGcolor ) ;
            SetBkColor( hDC, NORMALFONT.lfFGcolor ) ;
         }
      }
      else
      {
         if (bAttr & (BYTE) ATTRIBUTE_BOLD)
         {
            SetTextColor( hDC, HIGHLIGHTFONT.lfFGcolor ) ;
            SetBkColor( hDC, HIGHLIGHTFONT.lfBGcolor ) ;
         }
         else
         {
            SetTextColor( hDC, NORMALFONT.lfFGcolor ) ;
            SetBkColor( hDC, NORMALFONT.lfBGcolor ) ;
         }
      }
      bLastAttr = bAttr ;
   }

   // paint it

   if (bAttr & (BYTE) ATTRIBUTE_DBLSIZE)
   {
      GetDblCharRect( npECB, &rcTemp, nCol, nRow ) ;
      nVertPos = rcTemp.top ;
      nHorzPos = rcTemp.left ;
      GetDblCharRect( npECB, &rcPaint, nCol + nRun, nRow ) ;
      UnionRect( &rcPaint, &rcPaint, &rcTemp ) ;

      // if double height bottom half, subtract height of standard
      // character to get bottom half of double height character

      if (bAttr & (BYTE) ATTRIBUTE_DBLSIZE == ATTRIBUTE_DBLHEIGHTBTM)
         nVertPos -= YCHAR ;
      if ((bAttr & (BYTE) ATTRIBUTE_BLINKING) && BLINKSTATE)
         ExtTextOut( hDC, nHorzPos, nVertPos, ETO_OPAQUE | ETO_CLIPPED,
                     &rcPaint, NULL, 0, NULL ) ;
      else
         ExtTextOut( hDC, nHorzPos, nVertPos, ETO_OPAQUE | ETO_CLIPPED,
                     &rcPaint, CHARBUF + (nRow * MAXCOLS + nCol), nRun,
                     NULL ) ;
   }
   else
   {
      GetStdCharRect( npECB, &rcTemp, nCol, nRow ) ;
      nVertPos = rcTemp.top ;
      nHorzPos = rcTemp.left ;
      GetStdCharRect( npECB, &rcPaint, nCol + nRun, nRow ) ;
      UnionRect( &rcPaint, &rcPaint, &rcTemp ) ;

      if ((bAttr & (BYTE) ATTRIBUTE_BLINKING) > 0 && BLINKSTATE)
         ExtTextOut( hDC, nHorzPos, nVertPos, ETO_OPAQUE,
                     &rcPaint, NULL, 0, NULL ) ;
      else
         ExtTextOut( hDC, nHorzPos, nVertPos, ETO_OPAQUE,
                     &rcPaint, CHARBUF + (nRow * MAXCOLS + nCol), nRun,
                     NULL ) ;
   }

} /* end of PaintRun() */

/************************************************************************
 *  int NEAR GetFontNumber( BYTE bAttr )
 *
 *  Description: 
 *     Returns the font number associated with the character set
 *     and the attribute.
 *
 *  Comments:
 *      8/ 7/91  baw  Pulled from PAINT.C.
 *
 ************************************************************************/

int NEAR GetFontNumber( BYTE bAttr )
{
   int  nFont ;

   // select font based on attribute and character set

   if (bAttr & (BYTE) ATTRIBUTE_OEMCHARSET)
   {
      if (bAttr & (BYTE) ATTRIBUTE_UNDERLINE)
      {
         if (bAttr & (BYTE) ATTRIBUTE_DBLSIZE)
         {
            if (bAttr & (BYTE) ATTRIBUTE_DBLSIZE == ATTRIBUTE_DBLWIDTH)
               nFont = FONT_DBLWIDTHULOEM ;
            else
               nFont = FONT_DBLHEIGHTULOEM ;
         }
         else
            nFont = FONT_UNDERLINEOEM ;
      }
      else
      {
         if (bAttr & (BYTE) ATTRIBUTE_DBLSIZE)
         {
            if (bAttr & (BYTE) ATTRIBUTE_DBLSIZE == ATTRIBUTE_DBLWIDTH)
               nFont = FONT_DBLWIDTHOEM ;
            else
               nFont = FONT_DBLHEIGHTOEM ;
         }
         else
            nFont = FONT_STANDARDOEM ;
      }
   }
   else
   {
      if (bAttr & (BYTE) ATTRIBUTE_UNDERLINE)
      {
         if (bAttr & (BYTE) ATTRIBUTE_DBLSIZE)
         {
            if (bAttr & (BYTE) ATTRIBUTE_DBLSIZE == ATTRIBUTE_DBLWIDTH)
               nFont = FONT_DBLWIDTHUL ;
            else
               nFont = FONT_DBLHEIGHTUL ;
         }
         else
            nFont = FONT_UNDERLINE ;
      }
      else
      {
         if (bAttr & (BYTE) ATTRIBUTE_DBLSIZE)
         {
            if (bAttr & (BYTE) ATTRIBUTE_DBLSIZE == ATTRIBUTE_DBLWIDTH)
               nFont = FONT_DBLWIDTH ;
            else
               nFont = FONT_DBLHEIGHT ;
         }
         else
            nFont = FONT_STANDARD ;
      }
   }
   return ( nFont ) ;

} /* end of GetFontNumber() */

/************************************************************************
 * End of File: paint.c
 ************************************************************************/

