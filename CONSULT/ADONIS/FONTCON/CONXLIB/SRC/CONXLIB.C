/************************************************************************
 *
 *     Project:  CONXLIB
 *
 *      Module:  CONXLIB.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *     00.00.000  3/10/91 cek   First version
 *
 ************************************************************************/

#include "PRECOMP.H"
#include "CONXLIBI.h"
/*
 * Exported variables
 * These are "extern'd" in CONXLIB.h
 */
HMODULE     hmodDLL ;
BOOL        bMonochrome ;

LPSTR       rglpsz[LAST_IDS-FIRST_IDS+1] ;

LPVOID WINAPI GetStrings( HINSTANCE hInst ) ;

int WINAPI LibMain( HMODULE hModule, UINT wDataSeg, UINT cbHeapSize, LPSTR lpszCmdLine ) 
{
   HDC hDC ;

   #ifdef DEBUG
      WORD w ;
   #endif 

   if (cbHeapSize == 0)
      return FALSE ;

   hmodDLL = hModule ;

   D( hWDB = wdbOpenSession( NULL, "CONXLIB", "wdb.ini", WDB_LIBVERSION ) ) ;

   D( w = wdbGetOutput( hWDB, NULL ) ) ;

   D( wdbSetOutput( hWDB, WDB_OUTPUT_ODS, NULL ) ) ;

   #ifdef DEBUG
   {
      char szVersion[64] ;
      if ( VER_BUILD )
         wsprintf( szVersion, (LPSTR)"%d.%.2d.%.3d",
                  VER_MAJOR, VER_MINOR, VER_BUILD ) ;
      else
         wsprintf( szVersion, (LPSTR)"%d.%.2d", VER_MAJOR, VER_MINOR ) ;

      DP1( hWDB, "\n**** %s %s %s (Windows Version 0x%04X)",
               (LPSTR)"CONXLIB",
               (LPSTR)"Version",
               (LPSTR)szVersion,
               LOWORD(GetVersion()) ) ;

   }
   #endif

   if (GetStrings( hModule ) == NULL)
   {
      DP1( hWDB, "\n***** GetStrings() failed!!!!!\n" ) ;
      D( wdbSetOutput( hWDB, w, NULL ) ) ;

      return FALSE ;
   }

   StatusInit( NULL, hmodDLL, SZ_STATUSCLASSNAME ) ;
   LocalLEDInit( NULL, hmodDLL ) ;

   SliderInit( NULL, hmodDLL ) ;

   RegisterToolbarClass( hmodDLL ) ;

   TimeResetInternational() ;

   if (hDC = GetDC( NULL ))
   {
      if (GetDeviceCaps( hDC, NUMCOLORS ) <= 4)
         bMonochrome = TRUE ;
      else
         bMonochrome = FALSE ;
      ReleaseDC( NULL, hDC ) ;
   }

   D( wdbSetOutput( hWDB, w, NULL ) ) ;

   return TRUE ;
}

/*
 * Dummy routine that an app can call so CONXLIB gets implicitly
 * linked.
 */

VOID WINAPI ShopLibInit( VOID )
{
   return ;
}

/****************************************************************
 *  LPVOID WINAPI GetStrings( hInst )
 *
 *  Description: 
 *
 *    Fills rglpsz[] with resource strings.
 *
 *  Comments:
 *
 ****************************************************************/
LPVOID WINAPI GetStrings( HINSTANCE hInst )
{
   LPSTR          lpMem ;
   LPSTR          lpCur ;
   short          i ;
   short          n ;

   if (lpMem = GlobalAllocPtr( GHND, (LAST_IDS - FIRST_IDS) * 256 ))
   {
      /*
       * Load each string and keep it's pointer
       */
      lpCur = lpMem ;

      for (i = FIRST_IDS ; i <= LAST_IDS ; i++)
      {
         if (n = LoadString( hInst, i, lpCur, 256 ))
         {
            rglpsz[i] = lpCur ;
            lpCur += n + 1 ;
         }
         else
         {
            GlobalFreePtr( lpMem ) ;
            return NULL ;
         }
      }

      /*
       * Now reallocate the block so it is just big enough
       */
      GlobalReAllocPtr( lpMem, (lpCur - lpMem + 16), GHND ) ;

      return lpMem ;
   }

   return NULL ;

} /* GetStrings()  */


/*************************************************************************
 *  int WINAPI AtoI(const LPSTR lpszBuf)
 *
 *  Description:
 *    Converts a zero terminated character string to an integer value.
 *    The function behaves exactly like the C runtime function AtoI,
 *    the only difference being the far pointer.
 *
 *  Type/Parameter
 *          Description
 *
 *    LPSTR lpszBuf
 *
 *          String to be converted
 * 
 *  Return Value
 *    Returns an int value produced by interpreting the input chars as
 *    a number.
 *
 *  Comments:
 *
 *************************************************************************/
int WINAPI AtoI(const LPSTR lpszBuf)
{
   int n = 0;
   LPSTR lpszCur = lpszBuf ;
   BOOL  fNeg ;

   while (isspace(*lpszCur))
      lpszCur++ ;

   if (!(isdigit(*lpszCur) || *lpszCur == '-' || *lpszCur == '+'))
      return 0 ;

   fNeg = FALSE ;

   if (*lpszCur == '-')
   {
      lpszCur++ ;
      fNeg = TRUE ;
   }

   if (*lpszCur == '+')
      lpszCur++ ;

   while (isdigit(*lpszCur))
   {
       n *= 10 ;
       n += *lpszCur - '0' ;
       lpszCur++ ;
   }
   return n * (fNeg ? -1 : 1) ;
}/* AtoI() */

/*************************************************************************
 *  DWORD WINAPI AtoL(const LPSTR lpszBuf)
 *
 *  Description:
 *    Converts a zero terminated character string to an integer value.
 *    The function behaves exactly like the C runtime function AtoL,
 *    the only difference being the far pointer.
 *
 *  Type/Parameter
 *          Description
 *
 *    LPSTR lpszBuf
 *
 *          String to be converted
 * 
 *  Return Value
 *    Returns an DWORD value produced by interpreting the input chars as
 *    a number.
 *
 *  Comments:
 *
 *************************************************************************/
DWORD WINAPI AtoL(const LPSTR lpszBuf)
{
   LONG n = 0;
   LPSTR lpszCur = lpszBuf ;
   BOOL  fNeg ;

   while (isspace(*lpszCur))
      lpszCur++ ;

   if (!(isdigit(*lpszCur) || *lpszCur == '-' || *lpszCur == '+'))
      return 0 ;

   fNeg = FALSE ;

   if (*lpszCur == '-')
   {
      lpszCur++ ;
      fNeg = TRUE ;
   }

   if (*lpszCur == '+')
      lpszCur++ ;

   while (isdigit(*lpszCur))
   {
       n *= 10L ;
       n += (LONG)(*lpszCur - '0') ;
       lpszCur++ ;
   }
   return n * (fNeg ? -1 : 1) ;
}/* AtoL() */

//
// The following mnmemonics represent ROP codes.  The syntax is 
// reverse polish notation.  For example DSa means that source and
// desitination are AND'd (&) together.
//
#define DSna      0x00220326L
#define DSa       0x008800C6L
#define DSo       0x00EE0086L
#define DSx       0x00660045L
#define DSPDxax   0x00E20746L

/****************************************************************
 *  BOOL WINAPI bmTransBlt( HDC hdcD, short xD, short yD
 *                              short dx, short dy, HDC hdcS,
 *                              short xS, short yS )
 *
 *  Description:
 *
 *    This function does a transparent bitblt.
 *
 *    It requires only a source and Destination DC, it builds the
 *    mask for you.  This has a performance penalty over bmMaskBlt().
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI bmTransBlt( HDC hdcD, short xD, short yD,
                            short dx, short dy, HDC hdcS,
                            short xS, short yS )
{
   COLORREF rgbBG, rgbFG, rgbSourceBG ;
   HBITMAP  hbmMask ;
   HDC      hdcMask ;
   HBITMAP  hbmTemp ;
   BOOL     f = FALSE ;

   //
   // Save off the current DC's attributes
   //
   rgbBG = GetBkColor( hdcD ) ;
   rgbFG = GetTextColor( hdcD ) ;
   rgbSourceBG = GetBkColor( hdcS ) ;

   SetTextColor( hdcD, RGBBLACK ) ;

   //
   // if the source is a monochrome DC then just do a single ROP
   //
   if (GetDeviceCaps( hdcS, NUMCOLORS ) == 2)
      return BitBlt( hdcD, xD, yD, dx, dy, hdcS, xS, yS, DSo ) ;

   //
   // Make a memory DC to use in color conversion.
   //
   if (!(hdcMask = CreateCompatibleDC( hdcS )))
   {
      DASSERT( hWDB, hdcMask ) ;
      return FALSE ;
   }

   //
   // Create mask bitmap and associated DC 
   //
   if (!(hbmMask = CreateBitmap( dx, dy, 1, 1, NULL )))
   {
      DASSERT( hWDB, hbmMask ) ;
      goto errorDC ;
   }

   //
   // Select the mask into the mono DC
   //
   hbmTemp = SelectObject( hdcMask, hbmMask ) ;

   //
   // Do a color to mono bitblt to build the mask
   // generate 1's where the source is equal to the background, else
   // 0's
   //
   SetBkColor( hdcS, rgbBG ) ;
   BitBlt( hdcMask, 0, 0, dx, dy, hdcS, xS, yS, SRCCOPY ) ;

   /************************************************************/
   /* There are two solutions.  One involves 4 blts and does   */
   /* not flicker, but is slow.  The second flickers but is    */
   /* faster.                                                  */
   /************************************************************/

#ifdef NO_FLICKER_BUT_SLOW

   //
   // Make all background pixels in the source black
   //
   SetBkColor( hdcS, rgbBG ) ;
   BitBlt( hdcS, xS, yS, dx, dy, hdcMask, 0, 0, DSx ) ;

   //
   // Do two bitblts to draw the image w/transparent mask
   //
   SetBkColor( hdcD, RGBWHITE ) ;
   BitBlt( hdcD, xD, yD, hdcMask,  0,  0, DSa ) ;
   BitBlt( hdcD, xD, yD, hdcS   , xS, yS, DSx ) ;

   //
   // Restore all background pixels in the source
   //
   SetBkColor( hdcS, rgbBG ) ;
   f = BitBlt( hdcS, xS, yS, dx, dy, hdcMask, 0, 0, DSx ) ;

#else

   //
   // Do a MaskBlt to copy the bitmap to the dest
   //
   SetBkColor( hdcD, RGBWHITE ) ;

       BitBlt( hdcD, xD, yD, dx, dy, hdcS   , xS, yS, DSx ) ;
       BitBlt( hdcD, xD, yD, dx, dy, hdcMask, xS, yS, DSa ) ;
   f = BitBlt( hdcD, xD, yD, dx, dy, hdcS   , xS, yS, DSx ) ;

#endif

   SelectObject( hdcMask, hbmTemp ) ;
   DeleteObject( hbmMask ) ;

errorDC:

   //
   // Restore DC colors
   //
   SetBkColor( hdcS, rgbSourceBG ) ;
   SetBkColor( hdcD, rgbBG ) ;
   SetTextColor( hdcD, rgbFG ) ;

   DeleteDC( hdcMask ) ;
   
   return f ;

} /* bmTransBlt()  */

/****************************************************************
 *  VOID WINAPI
 *     bmColorTranslate( HBITMAP hbmSrc, COLORREF rgbOld,
 *                       COLORREF rgbNew, WORD wStyle )
 *
 *  Description: 
 *
 *    This function translates the colors in a bitmap from one
 *    to another.  It also will make a disabled bitmap.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI
      bmColorTranslate( HBITMAP hbmSrc, COLORREF rgbOld,
                        COLORREF rgbNew, WORD wStyle )
{
   HDC      hDC ;
   HDC      hdcMem ;
   BITMAP   bmBits ;


   if (hDC = GetDC( NULL ))
   {
      if (hdcMem = CreateCompatibleDC( hDC ))
      {
         //
         // Get the bitmap struct needed by bmColorTranslateDC()
         //
         GetObject( hbmSrc, sizeof( BITMAP ), (LPSTR)&bmBits ) ;

         //
         // Select our bitmap into the memory DC
         //
         hbmSrc = SelectObject( hdcMem, hbmSrc ) ;

         //
         // Translate the sucker
         //
         bmColorTranslateDC( hdcMem, &bmBits, rgbOld, rgbNew, wStyle ) ;

         //
         // Unselect our bitmap before deleting the DC
         //
         hbmSrc = SelectObject( hdcMem, hbmSrc ) ;

         DeleteDC( hdcMem ) ;
      }
      DASSERT( hWDB, hdcMem ) ;

      ReleaseDC( NULL, hDC ) ;
   }
   DASSERT( hWDB, hDC ) ;
} /* bmColorTranslate()  */


/****************************************************************
 *  VOID WINAPI bmColorTranslateDC( HDC hdcBM, LPBITMAP lpBM,
 *                                    COLORREF rgbOld, COLORREF rgbNew,
 *                                    WORD  wStyle )
 *
 *  Description: 
 *
 *    This function makes all pixels in the given DC that have the
 *    color rgbOld have the color rgbNew.
 *
 *    wSytle can be BM_NORMAL or BM_DISABLED.  If it is BM_DISABLED
 *    the bitmap will be translated so that it looks disabled.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI bmColorTranslateDC( HDC hdcBM, LPBITMAP lpBM,
                                  COLORREF rgbOld, COLORREF rgbNew,
                                  WORD  wStyle )
{
   HDC      hdcMask ;
   HBITMAP  hbmMask, hbmT ;
   HBRUSH   hbrT ;

   #ifdef DEBUG
   if (!lpBM)
   {
      DP1( hWDB, "bmColorTranslateDC(): lpBM passed in is null!" );
      return ;
   }
   #endif 

   //
   // if the bitmap is mono we have nothing to do
   //
   if (lpBM->bmPlanes == 1 && lpBM->bmBitsPixel == 1)
      return ;

   //
   //  Create the mask bitmap and associated DC
   //
   if (hbmMask = CreateBitmap( lpBM->bmWidth, lpBM->bmHeight, 1, 1, NULL ))
   {
      if (hdcMask = CreateCompatibleDC( hdcBM ))
      {
         //
         // Select th mask bitmap into the mono DC
         //
         hbmT = SelectObject( hdcMask, hbmMask ) ;

         // 
         // Create the brush and select it into the bits DC
         //
         hbrT = SelectObject( hdcBM, CreateSolidBrush( rgbNew ) ) ;

         // 
         // Do a color to mono bitblt to build the mask.  Generate
         // 1's where the source is equal to the background.
         //
         SetBkColor( hdcBM, rgbOld ) ;
         BitBlt( hdcMask, 0, 0, lpBM->bmWidth, lpBM->bmHeight,
                 hdcBM, 0, 0, SRCCOPY ) ;

         //
         // Where the mask is 1, lay down the brush, where it is 0, leave
         // the destination.
         //
         SetBkColor( hdcBM, RGBWHITE ) ;
         SetTextColor( hdcBM, RGBBLACK ) ;
         BitBlt( hdcBM, 0, 0, lpBM->bmWidth, lpBM->bmHeight,
                 hdcMask, 0, 0, DSPDxax ) ;

         SelectObject( hdcMask, hbmT ) ;

         hbrT = SelectObject( hdcBM, hbrT ) ;
         DeleteObject( hbrT ) ;

         DeleteDC( hdcMask ) ;
      }
      else
      {
         DP1( hWDB, "bmColorTranslateDC(): could not create hdcMask" ) ;
         return ;
      }

      DeleteObject( hbmMask ) ;
   }
   else
   {
      DP1( hWDB, "bmColorTranslateDC(): could not create hbmMask" ) ;
      return ;
   }

} /* bmColorTranslateDC()  */


/************************************************************************
 * End of File: CONXLIB.c
 ************************************************************************/

