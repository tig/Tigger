/************************************************************************
 *
 *    Copyright (C) 1990-91 Charles E. Kindel.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  WinPrint
 *
 *      Module:  font.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *       Font utils
 *
 *   Revisions:  
 *     00.00.000  2/ 9/91  cek   First Version
 *     00.00.001  10/ 4/91 cek   Minimzied for WInPrint
 *     02.00.000 11/ 3/91 cek    Moved to XEDIT 2.0
 *
 ************************************************************************/

#define STRICT
#define _WINDLL
#include <windows.h>
#ifdef WIN32
#include <strict.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <wdb.h>

#include "..\inc\font.h"

/************************************************************************
 * Imported variables
 ************************************************************************/

extern HANDLE hinstApp ;      // DLL's instance

/************************************************************************
 * Local Defines
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/

/************************************************************************
 * Exported Functions
 ************************************************************************/

/************************************************************************
 *
 *  REALLY SELECT A FONT STUFF
 *
 *  The following code selects a font based on a point setting (not
 *  pixels).  It is cool since it helps avoid the wrath of the font
 *  randomizer in GDI.
 *
 **********************************************************************/

/*
 * NOTE: You *must* export this callback function in your DEF file!!
 */
int FAR PASCAL
   fnEnumReally( LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetrics,
                short nFontType, LPLOGFONT lpLF ) ;

/****************************************************************
 *  HFONT WINAPI
 *    ReallyCreateFont( HDC hDC, LPSTR lpszFace,
 *                      short nPointSize,
 *                      WORD wFlags )
 *
 *  Description: 
 *
 *    Creates a font like CreateFont, but attempts to avoid the
 *    font randomizer.
 *
 *  Arguments:
 *
 *    HDC hDC
 *
 *       Handle to the device context that the font is to be
 *       create for.
 *
 *       If the hDC parameter is NULL, the screen DC will be used
 *       for font enumeration.  This means that you don't have to
 *       muck with getting a DC yourself if you don't want to (or
 *       don't have one available).
 *
 *    LPSTR lpszFace
 *
 *       The lpszFace parameter is a pointer to a zero terminated
 *       string that specifies the font face name you want (i.e. Helv).
 *       If the face you specify is not supported by the device
 *       (for instance if you specify "Helvetica" instead of "Helv")
 *       the face name of the stock object ANSI_VAR_FONT will be used.
 *       This means that the only time this function will fail to create
 *       a font is when GDI resources are *very* low.
 *
 *    short nPointSize
 *
 *       nPointSize specifies the size of the font in POINTS.  A point
 *       is 1/72 of an inch.  When you choose a font size in WinWord, you
 *       are choosing it in points.
 *
 *    WORD wFlags
 *
 *       wFlags can be any combination of:
 *
 *          RCF_NORMAL     -  Normal text (#defined as 0)
 *          RCF_ITALIC     -  Italic text
 *          RCF_UNDERLINE  -  Underlined text
 *          RCF_BOLD       -  Bold text
 *          RCF_STRIKEOUT  -  Strikeout text
 *
 *  Return Value:
 *
 *    The return value is a handle to the font (HFONT) if the function
 *    was successful, otherwise it is NULL.  Note that you *must*
 *    eventually call DeleteObject() to destroy the font.
 *
 *  Comments:
 *
 ****************************************************************/
HFONT WINAPI
ReallyCreateFont( HDC hDC, LPSTR lpszFace, short nPointSize, WORD wFlags )
{
   LOGFONT        lf ;
   FONTENUMPROC   lpfnEnumReally ;
   HDC            hCurDC ;

   //
   // First use font enumeration get the facename specified.
   // If that fails use a font based off of ANSI_VAR_FONT
   //

   if (!(lpfnEnumReally =
            (FONTENUMPROC)MakeProcInstance( (FARPROC)fnEnumReally,
                                            hinstApp )))
   {
      return FALSE ;
   }

   //
   // if the DC wasn't specified then use the display...
   //
   if (!hDC)
   {
      if (!(hCurDC = GetDC( NULL )))
      {
         FreeProcInstance( (FARPROC)lpfnEnumReally ) ;
         return FALSE ;
      }
   }
   else
      hCurDC = hDC ;

   if (hCurDC)
   {

      //
      // Enumerate.  If lfFaceName is not lpszFace then the font was
      // not found.
      //
      if (lpszFace)
         EnumFonts( hCurDC, lpszFace, lpfnEnumReally, (LPARAM)(LPVOID)&lf ) ;

      if (!lpszFace || lstrcmpi( lf.lfFaceName, lpszFace))
         GetObject( GetStockObject( ANSI_VAR_FONT ),
                    sizeof( LOGFONT ), (LPSTR)&lf ) ;

      //
      // See pages 4-48, 4-49, of Reference Vol. 1 for explaination
      // of why we set lfWidth to 0
      //
      lf.lfWidth = 0 ;

      //
      // The equation for calculating point size based on font
      // height (a point is 1/72 of an inch) is:
      //
      //
      //    pt = (height * 72) / number of pixels in the Y direction
      //
      // Using GetTextMetrics() you should calculate height as:
      //
      //    height = tm.tmHeight - tm.tmInternalLeading
      //
      // This is because Windows' interpretation of a font height
      // is different from everyone else's.
      //
      //
      lf.lfHeight = -MulDiv( nPointSize,
                             GetDeviceCaps( hCurDC, LOGPIXELSY),
                             72 )  ;

      lf.lfWeight    = (wFlags & RCF_BOLD) ? FW_BOLD : FW_NORMAL ;
      lf.lfItalic    = (BYTE)((wFlags & RCF_ITALIC) == RCF_ITALIC) ;
      lf.lfStrikeOut = (BYTE)((wFlags & RCF_STRIKEOUT) == RCF_STRIKEOUT) ;
      lf.lfUnderline = (BYTE)((wFlags & RCF_UNDERLINE) == RCF_UNDERLINE) ;
   }

   FreeProcInstance( (FARPROC)lpfnEnumReally ) ;

   if (hCurDC != hDC)
      ReleaseDC( NULL, hCurDC ) ;


   return CreateFontIndirect( &lf ) ;

} /* ReallyCreateFont()  */

/****************************************************************
 *  int CALLBACK
 *    fnEnumReally( LPLOGFONT lpLogFont,
 *                  LPTEXTMETRIC lpTextMetrics,
 *                  short nFontType, LPLOGFONT lpLF )
 *
 *  Description: 
 *
 *    Call back for EnumFonts and ReallySelectFont().
 *
 *    DO NOT FORGET TO EXPORT!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 *  Comments:
 *
 ****************************************************************/
int FAR PASCAL
fnEnumReally( LPLOGFONT lpLogFont,
              LPTEXTMETRIC lpTextMetrics, short nFontType, LPLOGFONT lpLF )
{
   #ifdef WIN32
   UNREFERENCED_PARAMETER( lpTextMetrics ) ;
   #endif

   *lpLF = *lpLogFont ;

   //
   // Only need to be called once!
   //
   return FALSE ;

}/* fnEnumReally() */

int FAR PASCAL
fnEnumFonts( LPLOGFONT lpLogFont,
              LPTEXTMETRIC lpTextMetrics, short nFontType, LPLOGFONT lpLF ) ;

HFONT WINAPI
GetDeviceDefaultFont( HDC hDC, short nPointSize, WORD wFlags )
{
   LOGFONT        lf ;
   FONTENUMPROC   lpfnEnumFonts ;
   HDC            hCurDC ;

   //
   // First use font enumeration get the facename specified.
   // If that fails use a font based off of ANSI_VAR_FONT
   //

   if (!(lpfnEnumFonts =
               (FONTENUMPROC)MakeProcInstance( (FARPROC)fnEnumFonts,
                                               hinstApp )))
   {
      return FALSE ;
   }

   //
   // if the DC wasn't specified then use the display...
   //
   if (!hDC)
   {
      if (!(hCurDC = GetDC( NULL )))
      {
         FreeProcInstance( (FARPROC)lpfnEnumFonts ) ;
         return FALSE ;
      }
   }
   else
      hCurDC = hDC ;

   if (hCurDC)
   {

      /*
       * Enumerate until we get the first fixed pitch font that is
       * a device font
       */
      lf.lfFaceName[0] = '\0' ;

      EnumFonts( hCurDC, NULL, lpfnEnumFonts, (LPARAM)(LPVOID)&lf ) ;

      if (!lstrlen( lf.lfFaceName ))
         GetObject( GetStockObject( DEVICE_DEFAULT_FONT ),
                    sizeof( LOGFONT ), (LPSTR)&lf ) ;

      //
      // See pages 4-48, 4-49, of Reference Vol. 1 for explaination
      // of why we set lfWidth to 0
      //
      lf.lfWidth = 0 ;

      //
      // The equation for calculating point size based on font
      // height (a point is 1/72 of an inch) is:
      //
      //
      //    pt = (height * 72) / number of pixels in the Y direction
      //
      // Using GetTextMetrics() you should calculate height as:
      //
      //    height = tm.tmHeight - tm.tmInternalLeading
      //
      // This is because Windows' interpretation of a font height
      // is different from everyone else's.
      //
      //
      lf.lfHeight = -MulDiv( nPointSize,
                             GetDeviceCaps( hCurDC, LOGPIXELSY),
                             72 )  ;

      lf.lfWeight    = (wFlags & RCF_BOLD) ? FW_BOLD : FW_NORMAL ;
      lf.lfItalic    = (BYTE)((wFlags & RCF_ITALIC) == RCF_ITALIC) ;
      lf.lfStrikeOut = (BYTE)((wFlags & RCF_STRIKEOUT) == RCF_STRIKEOUT) ;
      lf.lfUnderline = (BYTE)((wFlags & RCF_UNDERLINE) == RCF_UNDERLINE) ;
   }

   FreeProcInstance( (FARPROC)lpfnEnumFonts ) ;

   if (hCurDC != hDC)
      ReleaseDC( NULL, hCurDC ) ;


   return CreateFontIndirect( &lf ) ;

} /* GetDeviceDefaultFont()  */

int FAR PASCAL
fnEnumFonts( LPLOGFONT lpLogFont,
              LPTEXTMETRIC lpTextMetrics, short nFontType, LPLOGFONT lpLF )
{
   #ifdef WIN32
   UNREFERENCED_PARAMETER( lpTextMetrics ) ;
   #endif

   if (nFontType & DEVICE_FONTTYPE)
   {
      if ((lpLogFont->lfPitchAndFamily & 0x03) == FIXED_PITCH)
      {
         *lpLF = *lpLogFont ;
         return 0 ;
      }
   }
   else
      return 1 ;

   return 0 ; /* when done */

}/* fnEnumFonts() */


/************************************************************************
 *
 *  end of REALLY SELECT A FONT STUFF
 *
 ************************************************************************/


/****************************************************************
 *  int WINAPI
 *    GetTextMetricsCorrectly( HDC hDC, LPTEXTMETRIC lpTextMetrics )
 *
 *  Description: 
 *    
 *    This function gets the textmetrics of the font currently
 *    selected into the hDC.  It returns the average char width as
 *    the return value.
 *
 *    This function computes the average character width correctly
 *    by using GetTextExtent() on the string "abc...xzyABC...XYZ"
 *    which works out much better for proportional fonts.
 *
 *    Note that this function returns the same TEXTMETRIC
 *    values that GetTextMetrics() does, it simply has a different
 *    return value.
 *
 *  Comments:
 *
 ****************************************************************/
int WINAPI
   GetTextMetricsCorrectly( HDC hDC, LPTEXTMETRIC lpTM )
{
   int    nAveWidth ;
   char   rgchAlphabet[52] ;
   short  i ;
   #ifdef WIN32
   SIZE size ;
   #endif

   //
   // Get the TM of the current font.  Note that GetTextMetrics
   // gets the incorrect AveCharWidth value for proportional fonts.
   // This is the whole poshort in this exercise.
   //
   GetTextMetrics(hDC, lpTM);

   //
   // If it's not a variable pitch font GetTextMetrics was correct
   // so just return.
   //
   if (!(lpTM->tmPitchAndFamily & FIXED_PITCH))  
      return lpTM->tmAveCharWidth ;
   else
   {
      for ( i = 0 ; i <= 25 ; i++)
         rgchAlphabet[i] = (char)(i+(short)'a') ;

      for ( i = 0 ; i<=25 ; i++)
         rgchAlphabet[i+25] = (char)(i+(short)'A') ;

      #ifndef WIN32
      nAveWidth = LOWORD( GetTextExtent( hDC,
                             (LPSTR)rgchAlphabet, 52 ) ) / 52 ;
      #else
      GetTextExtentPoint( hDC, (LPSTR)rgchAlphabet, 52, &size ) ;
      nAveWidth = size.cx / 52 ;

      #endif                         
   }

   //
   // Return the calculated average char width
   //
   return nAveWidth ;

} /* GetTextMetricsCorrectly()  */


/************************************************************************
 * End of File: font.c
 ************************************************************************/

