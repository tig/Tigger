/************************************************************************
 *
 *     Project:  Windows Sample Applications
 *
 *      Module:  bmutil.c
 *
 *     Remarks:
 *
 *    This module contains a bunch of fun and useful bitmap manipulation
 *    functions.  Of interest are transparent bitmaps and the such.
 *
 *    As is typical with most Windows code in the world,
 *    some of this module was written by Todd Laney.
 *
 *   Revisions:  
 *
 *     00.00.000  6/10/91 cek   Pulled together from various places.
 *
 ***********************************************************************/

#define WINDLL
#define STRICT

#define OEMRESOURCE

#include <windows.h>
#include <ctype.h>
#include <wdb.h>

#ifdef SAMPLE
   #include "colrutil.h"

   /*
   * This header file contains function prototypes and API defines.
   */
   #include "bmutil.h"
#else
   #include "colrutil.h"

   /*
   * This header file contains function prototypes and API defines.
   */
   #include "bmutil.h"
#endif

/*
 * Local functions
 */
int NEAR PASCAL AtoI(const LPSTR lpszBuf) ;
HBITMAP NEAR PASCAL AutoLoadBitmap( HANDLE hInst, LPSTR lpszRes ) ;
HBITMAP NEAR PASCAL
   LoadIconAndReturnBitmap( HANDLE hInst, LPSTR lpszRes ) ;

#define MAX_SPECIFIER   128


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

#ifdef ICONHACK
static BYTE    XORbits[] =
{
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,

   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,

   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,

   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,

   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,

   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,

   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,

   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55
} ;

static BYTE    ANDbits[] =
{
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,

   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,

   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,

   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,

   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,

   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,

   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,

   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA,
   0x55, 0x55, 0x55, 0x55,
   0xAA, 0xAA, 0xAA, 0xAA
} ;

#endif /* ICONHACK */

extern HINSTANCE hinstApp ;

/****************************************************************
 *  BOOL FAR PASCAL bmMaskBlt( HDC hdcD, short xD, short yD
 *                             short dx, short dy, HDC hdcS,
 *                             HDC hdcMask, short xS, short yS )
 *
 *  Description:
 *
 *    This function does a transparent bitblt.  It requires that
 *    you supply an image in hdcS and a monochrome mask of that
 *    image that specifies the tranparent part in hdcMask.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL bmMaskBlt( HDC hdcD, short xD, short yD,
                           short dx, short dy, HDC hdcS,
                           HDC hdcMask, short xS, short yS )
{
   COLORREF rgbBG, rgbFG ;
   BOOL     f ;

   //
   // Be nice, and if there is no mask just do a normal blt.
   //
   if (!hdcMask)
      return BitBlt( hdcD, xD, yD, dx, dy, hdcS, xS, yS, SRCCOPY ) ;

   //
   // The ROP codes we use will use the fore and background colors
   //
   rgbFG = SetTextColor( hdcD, RGBWHITE ) ;
   rgbBG = SetBkColor( hdcD, RGBBLACK ) ;

       BitBlt( hdcD, xD, yD, dx, dy, hdcS   , xS, yS, DSx ) ;
       BitBlt( hdcD, xD, yD, dx, dy, hdcMask, xS, yS, DSa ) ;
   f = BitBlt( hdcD, xD, yD, dx, dy, hdcS   , xS, yS, DSx ) ;

   SetBkColor( hdcD, rgbBG ) ;
   SetTextColor( hdcD, rgbFG ) ;

   return f ;

} /* bmMaskBlt()  */

/****************************************************************
 *  BOOL FAR PASCAL bmTransBlt( HDC hdcD, short xD, short yD
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
BOOL FAR PASCAL bmTransBlt( HDC hdcD, short xD, short yD,
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
 *  HBITMAP FAR PASCAL
 *    bmCopy( HBITMAP hbmSrc )
 *
 *  Description: 
 *
 *    This function makes a copy of a bitmap.
 *
 *  Comments:
 *
 ****************************************************************/
HBITMAP FAR PASCAL bmCopy( HBITMAP hbmSrc )
{
   HBITMAP     hbmDest = NULL ;
   BITMAP      BM ;
   LPSTR       lpBits ;
   DWORD       dwNumBytes ;
   GLOBALHANDLE hMem ;

   #ifdef DEBUG
   if (!hbmSrc)
   {
      DP1( hWDB, "bmCopy(): hbmSrc is NULL!" ) ;
      return NULL ;
   }
   #endif

   GetObject( hbmSrc, sizeof(BITMAP), (LPSTR)&BM ) ;

   dwNumBytes = (DWORD)BM.bmWidth * BM.bmHeight * BM.bmWidthBytes ;

   if (hMem = GlobalAlloc( GHND, dwNumBytes ))
   {
      if (hbmDest = CreateBitmapIndirect( &BM ))
      {
         if (!(lpBits = GlobalLock( hMem )))
         {
            DP1( hWDB, "bmCopy(): Could not lock bits" ) ;
            DeleteObject( hbmDest ) ;
            return NULL ;
         }   

         GetBitmapBits( hbmSrc, dwNumBytes, lpBits ) ;

         SetBitmapBits( hbmDest, dwNumBytes, lpBits ) ;

         GlobalUnlock( hMem ) ;
         GlobalFree( hMem ) ;
      }
      else
      {
         DP1( hWDB, "bmCopy(): could not create bitmap" ) ;
         GlobalFree( hMem ) ;
         return NULL ;
      }
   }
   else
   {
      DP1( hWDB, "bmCopy(): could not allocate bits" ) ;
      return NULL ;
   }

   return hbmDest ;
   
} /* bmCopy()  */


/****************************************************************
 *  HBITMAP FAR PASCAL
 *    bmCopyTranslate( HBITMAP hbmSrc, COLORREF rgb,
 *                     short x, short y, WORD wStyle )
 *
 *  Description: 
 *
 *    This function makes a copy of a bitmap, translating
 *    colors if necessary.
 *
 *  Comments:
 *
 ****************************************************************/
HBITMAP FAR PASCAL
   bmCopyTranslate( HBITMAP hbmSrc, COLORREF rgb,
                    short x, short y, WORD wStyle )
{
   HBITMAP  hbmDest ;
   BITMAP   BM ;
   HDC      hdc ;
   HDC      hdcMem ;

   if (hbmDest = bmCopy( hbmSrc ))
   {
      GetObject( hbmDest, sizeof( BITMAP ), (LPSTR)&BM ) ;

      hdc = GetDC( NULL ) ;
      if (hdcMem = CreateCompatibleDC( hdc ))
      {
         hbmDest = SelectObject( hdcMem, hbmDest ) ;

         bmColorTranslateDC( hdcMem,
                             (LPBITMAP)&BM,
                             GetPixel( hdcMem, x, y ),
                             rgb,
                             wStyle ) ;

         hbmDest = SelectObject( hdcMem, hbmDest ) ;

         DeleteDC( hdcMem ) ;
      }

      ReleaseDC( NULL, hdc ) ;
   }

   return hbmDest ;

} /* bmCopyTranslate()  */

/****************************************************************
 *  VOID FAR PASCAL
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
VOID FAR PASCAL
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
 *  VOID FAR PASCAL bmColorTranslateDC( HDC hdcBM, LPBITMAP lpBM,
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
VOID FAR PASCAL bmColorTranslateDC( HDC hdcBM, LPBITMAP lpBM,
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

#ifdef ICONHACK
         //
         // Below is a fairly sleazy peace of code.  To do a nice
         // disabled effect we use a 2 color icon.
         //
         // Ok, we need to be smarter about this since this only
         // works for 32x32 bitmaps!!!!
         //
         if (wStyle & BM_DISABLED)
         {
            HICON    hIcon ;

            DP5( hWDB, "Creating stupid icon: hinstApp = %04X, bmWidth = %d, bmHeight = %d",
               hinstApp, lpBM->bmWidth, lpBM->bmHeight ) ;

            if (hIcon = CreateIcon( hinstApp,
                              32, 32, //lpBM->bmWidth, lpBM->bmHeight,
                              1,
                              1,
                              (LPSTR)&XORbits,
                              (LPSTR)&ANDbits ))
            {
               SetBkColor( hdcBM, rgbNew ) ;
               DrawIcon( hdcBM, 0, 0, hIcon ) ;

               DestroyIcon( hIcon ) ;
            }
            else
            {
               DP1( hWDB, "bmColorTranslateDC(): Could not create stupid disabled icon thingy" ) ;
            }   
         }
#endif

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

/****************************************************************
 *  HBITMAP FAR PASCAL
 *    bmLoadBitmapTranslate( HANDLE hInst, LPSTR lpszResName,
 *                           DWORD rgbNew, short x, short y,
 *                                         WORD  wStyle )
 *
 *  Description: 
 *
 *    This function loads a bitmap from an application or DLL's
 *    resources.  It finds the bitmap using the lpszResName string
 *    and sets all pixels that are the color of the (x,y) pixel
 *    to the color specified in rgbNew.
 *
 *    To load a bitmap without translation use the bmLoadBitmap()
 *    function.
 *
 *    If the HIWORD of lpszResName is 0x0000 then the LOWORD contains
 *    the resource ID of the bitmap to load.
 *
 *    Otherwise lpszResName points to a "specifier" string that specifies the
 *    bitmap resource string.  This string can be of the
 *    following format:
 *
 *    General Syntax:  [#|^][%]<RESOURCE NAME>
 *
 *       # means the <RESOURCE NAME> string contains an integer that
 *         specifies a stringtable ID that contains the actual
 *         bitmap name (which is also in of the above format).
 *         The % specifier cannot be used with the # specifier.
 *
 *       ^ means that <RESOURCE NAME> contains a fully qualified
 *         pathname to a BMP file that contains the bitmap.
 *
 *       % If this modifier is used the resources will be searched for
 *         a bitmap that corresponds the the current device resolution
 *         (using the LOGPIXELS values as a key) in the following order:
 *
 *           fooWxHxC
 *           fooWxH
 *           foo
 *
 *             W is the horizontal dpi for the current display
 *             H is the vertical dpi value of the current display
 *             C is the number of colors of the current display
 *
 *          For example a VGA specific resource would be named
 *          "foo96x96x16" and the specifier string would be
 *          "%foo96x96x16".
 *
 *    Examples:
 *       If you wanted to use a bitmap named "bar" and automatically
 *       select the correct resolution you would specify
 *       the following as the window text:
 *
 *          "%bar"
 *
 *       If you have a string resource at id IDS_FOOBAR and you want to
 *       have the bitmap associated with that string loaded, use the
 *       following code to set up the window text string:
 *
 *          wsprintf( szResName, "#%d", IDS_FOOBAR ) ;
 *
 *          In the resource file you should have a string at id IDS_FOOBAR
 *          that looks something like this:
 *
 *             "%foobar"
 *
 *       If you want to use an external file for the bitmap use the
 *       following:
 *
 *          "^C:\WIN\FOO.BMP"
 *       
 *  Comments:
 *
 *    Currently the only supported specifier syntax is:
 *
 *       [#][%]<RESOURCE NAME>
 *       
 ****************************************************************/
HBITMAP FAR PASCAL bmLoadBitmapTranslate( HANDLE hInst, LPSTR lpszResName,
                                          DWORD rgbNew, short x, short y,
                                          WORD  wStyle )
{
   HBITMAP  hBM = 0 ;

   BITMAP   bmBits ;

   HDC      hDC ;
   HDC      hdcMem ;
   DWORD    rgbOld ;

   if (wStyle & BM_HBITMAP)
      hBM = (HBITMAP)LOWORD( (DWORD)lpszResName ) ;

   if (hBM || (hBM = bmLoadBitmap( hInst, lpszResName )))
   {
      //
      // Now tranlate the colors if specified
      //
      if (hDC = GetDC( NULL ))
      {
         if (hdcMem = CreateCompatibleDC( hDC ))
         {
            //
            // Get the bitmap struct needed by bmColorTranslateDC()
            //
            GetObject( hBM, sizeof( BITMAP ), (LPSTR)&bmBits ) ;

            //
            // Select our bitmap into the memory DC
            //
            hBM = SelectObject( hdcMem, hBM ) ;

            rgbOld = GetPixel( hdcMem, 0, 0 ) ;

            //
            // Translate the sucker
            //
            bmColorTranslateDC( hdcMem, &bmBits, rgbOld, rgbNew, wStyle ) ;

            //
            // Unselect our bitmap before deleting the DC
            //
            hBM = SelectObject( hdcMem, hBM ) ;

            DeleteDC( hdcMem ) ;
         }
         DASSERT( hWDB, hdcMem ) ;

         ReleaseDC( NULL, hDC ) ;
      }
      DASSERT( hWDB, hDC ) ;
   }
   else
   {
      DP4( hWDB, "bmLoadBitmap failed" ) ;
   }

   return hBM ;
} /* bmLoadBitmapTranslate()  */

/****************************************************************
 *  HBITMAP FAR PASCAL bmLoadBitmap( HANDLE hInst, LPSTR lpszResName )
 *
 *  Description: 
 *
 *    Loads a bitmap just like bmLoadBitmapTranslate() but does
 *    not translate the bitmap colors.
 *
 *  Comments:
 *
 ****************************************************************/
HBITMAP FAR PASCAL bmLoadBitmap( HANDLE hInst, LPSTR lpszResName )
{
   char     szRes[MAX_SPECIFIER] ;
   HBITMAP  hBM ;

   //
   // If the hiword of the resource name string ptr is 0 then
   // the loword is a resource ID so our job is easy.
   //

   if (!lpszResName)
   {
      DP4( hWDB, "NULL lpszResName passed to bmLoadBitmap" ) ;
      return NULL ;
   }

   if (HIWORD( lpszResName ) == 0)
      return  LoadBitmap( hInst, lpszResName );
   else
   {
      DP4( hWDB, "lpszResName = %s", (LPSTR)lpszResName ) ;
      //
      // If the load from file flag is set
      //
      if (*lpszResName == FILEFLAG)
      {
         return bmLoadBitmapFromFile( lpszResName + 1 ) ;
      }

      //
      // If we are to find the actual specifier string in a string
      // table entry.
      //
      if (*lpszResName == IDFLAG)
      {
         short nID = AtoI( lpszResName + 1 ) ;

         if (!LoadString( hInst, nID, szRes, MAX_SPECIFIER ))
            return NULL ;
      }
      else
         lstrcpy( szRes, lpszResName ) ;

      //
      // Now szRes contains the resource string.
      //
      switch (*szRes)
      {
         case AUTOFLAG:
            //
            // It looks like we have one of the form WxHxC or WxH...
            //
            return AutoLoadBitmap( hInst, szRes+1 ) ;
         break ;

         case ICONFLAG:
         {
            //
            // this is an icon
            //
            return LoadIconAndReturnBitmap( hInst, szRes + 1 ) ;
         }
         break ;

         default:
            DP5( hWDB, "  Loading bitmap (LoadBitmap( %d, \"%s\" )",
               hInst, (LPSTR)szRes ) ;
            return LoadBitmap( hInst, szRes ) ;
      }
   }

   return hBM ;

} /* bmLoadBitmap()  */

/****************************************************************
 *  HBITMAP FAR PASCAL bmLoadBitmapFromFile( LPSTR lpszFile )
 *
 *  Description: 
 *
 *    This function loads a bitmap from a file.  lpszFile
 *    must point to a filename that is either fully qualified
 *    or in the path.
 *
 *  Comments:
 *
 ****************************************************************/
HBITMAP FAR PASCAL bmLoadBitmapFromFile( LPSTR lpszFile )
{
   DP1( hWDB, "Filenames not supported yet!" ) ;

   return NULL ;

} /* bmLoadBitmapFromFile()  */


/****************************************************************
 *  HBITMAP NEAR PASCAL AutoLoadBitmap( HANDLE hInst, LPSTR lpszRes )
 *
 *  Description: 
 *
 *    Loads a bitmap, automatically choosing the correct size and color
 *    for the display.
 *
 *  Comments:
 *
 ****************************************************************/
HBITMAP NEAR PASCAL AutoLoadBitmap( HANDLE hInst, LPSTR lpszRes )
{
   HDC      hDC ;
   short    nColors ;
   short    nLogPelX, nLogPelY ;
   char     szName[MAX_SPECIFIER] ;
   HBITMAP  hBM ;

   hDC = GetDC( NULL ) ;
   DASSERT( hWDB, hDC ) ;

   nColors  = GetDeviceCaps( hDC, NUMCOLORS ) ;
   nLogPelX = GetDeviceCaps( hDC, LOGPIXELSX ) ;
   nLogPelY = GetDeviceCaps( hDC, LOGPIXELSY ) ;

   ReleaseDC( NULL, hDC ) ;

   //
   // look for a resource of the form WxHxC
   //

   wsprintf( szName, "%s%dx%dx%d",
            (LPSTR)lpszRes+1, nLogPelX, nLogPelY, nColors ) ;

   DP5( hWDB, "  Trying to load bitmap %s", (LPSTR)szName ) ;
   hBM = LoadBitmap( hInst, szName ) ;


   if (!hBM)
   {
      //
      // look for a resource of the form WxHxC
      //

      wsprintf( szName, "%s%dx%d",
               (LPSTR)lpszRes+1, nLogPelX, nLogPelY ) ;

      DP5( hWDB, "  Trying to load bitmap %s", (LPSTR)szName ) ;
      hBM = LoadBitmap( hInst, szName ) ;
   }

   if (!hBM)
   {
      //
      // Since we didn't find one of the form WxHxC or WxH then
      // it might still be there in the form of <name> so try that.
      //
      DP5( hWDB, "  Trying to load bitmap %s", (LPSTR)lpszRes+1 ) ;
      return LoadBitmap( hInst, lpszRes + 1 ) ;
   }
   else
      return hBM ;

} /* AutoLoadBitmap()  */


/****************************************************************
 *  HBITMAP NEAR PASCAL
 *    LoadIconAndReturnBitmap( HANDLE hInst, LPSTR lpszRes )
 *
 *  Description: 
 *
 *    Loads a bitmap, automatically choosing the correct size and color
 *    for the display.
 *
 *  Comments:
 *
 ****************************************************************/
HBITMAP NEAR PASCAL
   LoadIconAndReturnBitmap( HANDLE hInst, LPSTR lpszRes )
{
   HDC      hDC    = NULL ;
   HDC      hdcMem = NULL ;
   HBITMAP  hBM    = NULL ;
   HBITMAP  hbmOld ;
   HICON    hIcon  = NULL ;
   RECT     rc ;

   if (!(hDC = GetDC( NULL )))
   {
      DASSERT( hWDB, hDC ) ;
      return NULL ;
   }

   if (!(hdcMem = CreateCompatibleDC( hDC )))
   {
      DASSERT( hWDB, hdcMem ) ;
      goto error1 ;
   }

   if (!(hBM = CreateCompatibleBitmap( hDC, GetSystemMetrics( SM_CXICON ),
                                            GetSystemMetrics( SM_CYICON ) )))
   {
      DASSERT( hWDB, hBM ) ;
      goto error1 ;
   }

   ReleaseDC( NULL, hDC ) ;

   DP5( hWDB, "  Trying to load icon %s", (LPSTR)lpszRes ) ;
   hIcon = LoadIcon( hInst, lpszRes ) ;

   if (!hIcon)
   {
      DASSERT( hWDB, hIcon ) ;
      goto error1 ;
   }

   //
   // Select the bitmap in 
   //
   hbmOld = SelectObject( hdcMem, hBM ) ;

   SetBkColor( hdcMem, RGBWHITE ) ;
   SetRect( &rc, 0, 0, GetSystemMetrics( SM_CXICON ),
                       GetSystemMetrics( SM_CYICON ) ) ;
   ExtTextOut( hdcMem, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;

   //
   // Draw the icon on to the dc
   //
   DrawIcon( hdcMem, 0, 0, hIcon ) ;

   hBM = SelectObject( hdcMem, hBM ) ;

   DeleteDC( hdcMem ) ;
   DestroyIcon( hIcon ) ;

   return hBM ;

error1:
   if (hdcMem)
      DeleteDC( hdcMem ) ;

   if (hBM)
      DeleteObject( hBM ) ;

   return NULL ;

} /* LoadIconAndReturnBitmap()  */

/*************************************************************************
 *  int NEAR PASCAL AtoI(const LPSTR lpszBuf)
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
int NEAR PASCAL AtoI(const LPSTR lpszBuf)
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

/*******************

Ideas for functions:

   - Some way to bit blt transparent bitmaps

      - To blt transparent bitmaps we need a mask.  The problem
        is that this introduces a performance hit.  How do we handle
        it?  Do we generate the mask on the fly?

            One idea is to have a "new" structure that is passed
            around for one of these puppies.  This structure might
            look like this:

               typedef struct tagTRANSBITMAP
               {
                  HBITMAP  hbmImage ;
                  HBITMAP  hbmMask ;
               } TRANSBITMAP, *PTRANSBITMAP, FAR *LPTRANSBITMAP ;

            Then have some APIs to create, blt, and destroy these:

               bmCreateTransBitmap( hDC, hbmImage, lpTransBitmap,
                                    rgbTransColor, wFlags )

                  where hDC is the device context to be used for
                  the blts, hbmImage is the original bitmap, and
                  lpTransBitmap is a pointer to a TRANSBITMAP
                  structure that will be filled in.

                  rgbTransColor would be the RGB in the image that
                  is to be transparent.  If wFlags is CTB_FROMPIXEL
                  then LOWORD(rgbTransColor) is the x of the pixel
                  to be use for the color and HIWORD(rgbTransColor)
                  is the y of the color.  Thus you could set
                  rgbTransColor to 0 and wFlags to CTB_FROMPIXEL and
                  the transparent color would come from the top
                  left pixel!

               bmBltTransBitmap( hdcDest, x, y, lpTransBitmap )

                  This would blt the given bitmap to the DC.  No
                  need for size.  We may, however, want a
                  bmExtBltTransBitmap that allows all that stuff.

               bmDestroyTransBitmap( lpTransBitmap ) ;                  

   - Quick function for blitting a small bmp out of a bigger one
     (ala roadrunner).  This should work by allowing the application
     to specify an index from 0 to n - 1 where n is the number of sub
     bitmaps in the source.

         bmSubBitBlt( hdcDest, x, y, cx, cy, hdcBig, wIndex )

            The code for this function is *really* easy since it
            just calls BitBlt after some multiplies.  The whole
            point for even including this function is to demonstrate
            how easy it is to just keep one big bitmap around
            for all your little ones.

   - Load a bitmap from a resource file given a resource
     name string (for bmp buttons) in the bmpbtm format.

         bmLoadBitmap( hInst, LPSTR lpszResName, DWORD rgbNew, DWORD rgbOld )

            This function loads a bitmap using a sbutton or bmpbtn
            style specifier string.  If HIWORD of lpszResName is NULL
            then the LOWORD is a resource ID for the bitmap.
            
            It optionally will set a given color to another.

********************/

/************************************************************************
 * End of File: bmutil.c
 ***********************************************************************/

