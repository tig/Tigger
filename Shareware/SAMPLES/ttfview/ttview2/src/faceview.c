/************************************************************************
 *
 *     Project:  TTView
 *
 *      Module:  faceview.c
 *
 *     Remarks:  Handles the painting of the face view
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "global.h"

typedef struct tagMYENUM
{
   HDC      hDC ;
   int      x ;
   int      dx ;
   int      y ;

}  MYENUM, FAR *LPMYENUM ;
int CALLBACK fnPaintEnum( LPLOGFONT lpLogFont,
      LPNEWTEXTMETRIC lpTextMetrics, short nFontType, LPMYENUM lpMy ) ;


/****************************************************************
 *  VOID WINAPI PaintFaceView( HDC hdc, HFONT hfont )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI PaintFaceView( HDC hdc, HFONT hfont )
{
   RECT              rcClient, rc ;

   UINT              x = CX_DEFAULT ;
   UINT              y = CY_DEFAULT ;

   char              szBuf[256] ;
   char              szFace[LF_FACESIZE] ;
   char              szFullname[3*LF_FACESIZE] ;
   TEXTMETRIC        tm ;
   MYENUM            myenum ;
   FONTENUMPROC      lpfn ;

   GetClientRect( hWnd, &rcClient ) ;

   BeginPaint( hWnd, &ps ) ;

   hfontCur = SelectObject( ps.hdc, hfontCur ) ;
   GetTextMetrics( ps.hdc, &tm ) ;
   GetTextFullName( ps.hdc, 256, szFullname ) ;
   GetTextFace( ps.hdc, 256, szFace ) ;

   wsprintf( szBuf, "%s (%s)", (LPSTR)szFace, (LPSTR)szFullname ) ;

   /*
    * Use Times New Roman to draw our header if it's a symbol font or
    * if it looks like the font is a symbol font. 
    */
   DP( hWDB, "face = %s, charset = %d", (LPSTR)szFaceName, tm.tmCharSet ) ;
   if (TRUE) //tm.tmCharSet != ANSI_CHARSET && tm.tmCharSet != DEFAULT_CHARSET)
   {
      hfont = ReallyCreateFontEx( ps.hdc, "Times New Roman", "Bold", 12, 0 )  ;
      hfont = SelectObject( ps.hdc, hfont ) ;
      GetTextMetrics( ps.hdc, &tm ) ;
   }

   TextOut( ps.hdc, x, y, szBuf, lstrlen(szBuf) ) ;

   y += tm.tmHeight + tm.tmExternalLeading + CY_DEFAULT ;

   if (TRUE)//tm.tmCharSet != ANSI_CHARSET && tm.tmCharSet != DEFAULT_CHARSET)
   {
      hfont = SelectObject( ps.hdc, hfont ) ;
      DeleteObject( hfont ) ;
   }

   MoveToEx( ps.hdc, x, y, NULL ) ;
   LineTo( ps.hdc, rcClient.right-(x*2), y ) ;

   y += CY_DEFAULT ;

   GetTextMetrics( ps.hdc, &tm ) ;

   /*
    * The header is now there and x,y is the starting point
    * for our display.
    */
   if (hfont = ReallyCreateFontEx( ps.hdc, szFaceName, szFaceStyle, 32, 0 ))
   {
      /*
       * Draw the main sample
       */
      hfont = SelectObject( ps.hdc, hfont ) ;

      rc.top = y ;
      rc.top += DrawCaption( ps.hdc, hfontSmall, x, rc.top) ;
      /*
       * If it's a symbol font then we can party on the whole
       * area.
       */
      if (tm.tmCharSet == SYMBOL_CHARSET)
      {
         char szBuf[256-32] ;
         UINT i ;

         for (i = 32 ; i < 256 ; i++)
            szBuf[i-32] = (char)i ;

         rc.left = x ;
         rc.right = rcClient.right - 2*x ;
         DrawSample( ps.hdc, &rc, szBuf ) ;
      }
      else
      {
         rc.left = x ;
         rc.right = (rcClient.right-2*x) / 2 - CX_DEFAULT ;
         DrawSample( ps.hdc, &rc, rglpsz[IDS_SAMPLE_ALPHA] ) ;
      
         rc.top = rc.bottom ;
         DrawSample( ps.hdc, &rc, rglpsz[IDS_SAMPLE_DIGIT] ) ;
      }

      hfont = SelectObject( ps.hdc, hfont ) ;
      DeleteObject( hfont ) ;
   }

   /*
    * Enumerate the face.  For each style output it.  The
    * first style will be considered the 'main' and will
    * be big (24 point).  the others will be smaller (16 pt?).
    */
   myenum.hDC = ps.hdc ;
   myenum.x = x ;
   myenum.dx = rcClient.right - (2*x) ;
   myenum.y = y ;

   lpfn = (FONTENUMPROC)MakeProcInstance( (FARPROC)fnPaintEnum, hinstApp ) ;
   EnumFontFamilies( ps.hdc, szFaceName, lpfn, (LPARAM)(LPVOID)&myenum ) ;
   FreeProcInstance( (FARPROC)lpfn ) ;

   /*
    * Now output the font info table:


      +------------------------------------------------------+
      | Full Name: Arial Bold Microsoft 1992                 |
      |------------------------------------------------------|
      | Font File Size(s):                                   |
      |   ....


    */

   y = max( myenum.y, rc.bottom ) ;

   hfontCur = SelectObject( ps.hdc, hfontCur ) ;
   
} /* PaintFaceView()  */

/****************************************************************
 *  int CALLBACK fnPaintEnum( LPLOGFONT lpLogFont,
 *       LPNEWTEXTMETRIC lpTextMetrics, short nFontType, LPMYENUM lpMy )
 *
 *  Description: 
 *
 *    EnumFontFamilies callback for the paint routine.
 *
 *  Comments:
 *
 ****************************************************************/
int CALLBACK fnPaintEnum( LPLOGFONT lpLogFont,
      LPNEWTEXTMETRIC lpTextMetrics, short nFontType, LPMYENUM lpMy )
{
   HFONT hfont ;
   RECT  rc ;

   if (!(nFontType & TRUETYPE_FONTTYPE))
      return 1 ;

   if (lpTextMetrics->tmCharSet == SYMBOL_CHARSET)
      return 1 ;

   /*
      * On the right, draw the other samples.
      */
   lpLogFont->lfHeight = -MulDiv( 18,
                           GetDeviceCaps( lpMy->hDC, LOGPIXELSY),
                           72 )  ;
   lpLogFont->lfWidth = 0 ;

   if (hfont = CreateFontIndirect( lpLogFont ))
   {
      hfont = SelectObject( lpMy->hDC, hfont ) ;

      rc.top = lpMy->y ;
      rc.left = lpMy->x + (lpMy->dx / 2) + 2*CX_DEFAULT ;
      rc.right = lpMy->dx ;

      rc.top += DrawCaption( lpMy->hDC, hfontSmall, rc.left, rc.top ) ;

      /*
         * Draw the main sample
         */
      DrawSample( lpMy->hDC, &rc, rglpsz[IDS_SAMPLE_PANOSE] ) ;
         
      /*
         * draw a caption
         */
      lpMy->y = rc.bottom ;

      hfont = SelectObject( lpMy->hDC, hfont ) ;
      DeleteObject( hfont ) ;
   }

   return 1 ;

} /* fnPaintEnum()  */

/****************************************************************
 *  VOID WINAPI DrawSample( HDC hDC, LPRECT lprc, LPSTR lpszSample )
 *
 *  Description: 
 *
 *    Draws the given sample within the rect.  Upon return
 *    lprc points to the actual rect that it fit in.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI DrawSample( HDC hDC, LPRECT lprc, LPSTR lpszSample )
{
   LPSTR             lps, lps2 ;
   UINT              n ;
   SIZE              size ;
   int               dx ;
   int               cxOffset ;
   int               x, y;
   UINT              cyChar ;
   TEXTMETRIC        tm ;

   x = lprc->left ;
   y = lprc->top ;
   dx = lprc->right - x ;

   GetTextMetrics( hDC, &tm ) ;
   cyChar = tm.tmHeight + tm.tmExternalLeading ;

   for (lps = lps2 = lpszSample, n = 0 ; *lps2 ; lps2++)
   {
      GetTextExtentABCPoint( hDC, lps, n+2, &size ) ;
      if (size.cx > dx)
      {
         cxOffset = GetTextExtentABCPoint( hDC, lps, n+1, &size ) ;
         TextOut( hDC, cxOffset + x+(dx-size.cx)/2, y, lps, n+1 ) ;
         y += cyChar ;

         n = 0 ;

         lps = lps2 + 1 ;
      }
      else
         n++ ;

   }

   if (*lps)
   {
      cxOffset = GetTextExtentABCPoint( hDC, lps, n, &size ) ;
      TextOut( hDC,cxOffset+ x+(dx-size.cx)/2, y, lps, n ) ;
      y += cyChar ;
   }

   lprc->bottom = y ;

} /* DrawSample()  */

/****************************************************************
 *  VOID WINAPI DrawCaption( HDC hDC, HFONT hfont, int x, int y )
 *
 *  Description: 
 *
 *    Draws a little caption using hfont indicating
 *    what the current font is.
 *
 *    Returns the hight of the caption
 *
 *  Comments:
 *
 ****************************************************************/
UINT WINAPI DrawCaption( HDC hDC, HFONT hfont, int x, int y )
{
   char        szStyle[LF_FACESIZE] ;
   char        szFace[LF_FACESIZE] ;
   char        szBuf[LF_FACESIZE * 4] ;
   TEXTMETRIC  tm ;

   GetTextFace( hDC, LF_FACESIZE, szFace ) ;
   GetTextStyle( hDC, LF_FACESIZE, szStyle ) ;

   wsprintf( szBuf, "%s %s %d point",
      (LPSTR)szFace,
      (LPSTR)szStyle,
      GetTextPointSize( hDC ) ) ;

   hfont = SelectObject( hDC, hfont ) ;
   GetTextMetrics( hDC, &tm ) ;
   TextOut( hDC, x, y, szBuf, lstrlen(szBuf) ) ;
   hfont = SelectObject( hDC, hfont ) ;

   return tm.tmHeight + tm.tmExternalLeading ;

} /* DrawCaption()  */


/************************************************************************
 * End of File: faceview.c
 ***********************************************************************/

