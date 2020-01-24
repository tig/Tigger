/************************************************************************
 *
 *     Project:  Windows Sample Applications
 *
 *      Module:  fontutil.c
 *
 *     Remarks:  
 *
 *    This file contains font handling routines that are useful when
 *    dealing with creating and manipulating fonts in Windows.
 *
 *  REALLY SELECT A FONT STUFF
 *
 *  The following code selects a font based on a point setting (not
 *  pixels).  It is cool since it helps avoid the wrath of the font
 *  randomizer in GDI.
 *
 *   Revisions:  
 *
 *     00.00.000  6/ 7/91 cek   Pulled together from various files.
 *     1.0  1/29/92 cek   Modified to use 3.1 features.
 *
 ***********************************************************************/

#define STRICT
#define _WINDLL
#include <windows.h>
#include <windowsx.h>
#include <ctype.h>
#include <wdb.h>

#ifndef TTVIEW
#include "fontutil.h"
#else
#include "..\inc\font.h"
#define hmodApp hinstApp
#endif

/*
 * If you put this code in a DLL #define IN_A_DLL
 */
//#define IN_A_DLL

/*
 * The font enumeration stuff needs the hInstance of the EXE (or DLL)
 * that contains the fnEnumReallyEx() function for callback.  Thus
 * the following hmodApp var. needs to be exported from
 * the app's WinMain().
 */
#ifndef IN_A_DLL
extern HMODULE hmodApp ;
#endif

/*
 * You probably want to make this a global in your app.  There is, after
 * all no reason to call GetVersion() all the time.
 */
#define fWin30 FALSE //((BOOL)(LOWORD( GetVersion() ) == 0x0003))

/*
 * HACK!!!!
 *  These strings should be resources and LoadStirng'd.  They
 *  are not constant for other languages.  For example 'bold' is
 *  'bold' in german, it's something like 'bolderstang'.
 */
#ifdef TTVIEW

#include "..\inc\isz.h"
extern LPSTR rglpsz[] ;

#define szBold       rglpsz[IDS_BOLD] 
#define szItalic     rglpsz[IDS_ITALIC]
#define szBoldItalic rglpsz[IDS_BOLDITALIC]
#define szRegular    rglpsz[IDS_REGULAR]

#else
char szBold[]        = "Bold" ;
char szItalic[]      = "Italic" ;
char szBoldItalic[]  = "BoldItalic" ;
char szRegular[]     = "Regular" ;
#endif

typedef struct tagMYENUMFONT
{
   char     szStyle[LF_FACESIZE] ; 
   LOGFONT  lf ;
   
   /* These are appended to LOGFONT in EnumFontFamilies callback */
   char     lfFullName[LF_FACESIZE*2] ;
   char     lfStyle[LF_FACESIZE] ;
} MYENUMFONT, *PMYENUMFONT, FAR *LPMYENUMFONT ;

/*
 * NOTE: You *must* export this callback function in your DEF file!!
 */

int CALLBACK
fnEnumReallyEx( LPLOGFONT lpLogFont,
                LPNEWTEXTMETRIC lpTextMetrics,
                short nFontType,
                LPMYENUMFONT lpMLF ) ;

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
   return ReallyCreateFontEx( hDC, lpszFace, NULL, nPointSize, wFlags ) ;
}

/****************************************************************
 *  HFONT WINAPI
 *    ReallyCreateFontEx( HDC hDC, LPSTR lpszFace,
 *                        LPSTR lpszStyle, short nPointSize, WORD wFlags )
 *
 *  Description: 
 *
 *    Creates a font based on facename, pointsize, and style.
 *
 *    Uses 3.1 API's to correctly select TT fonts.
 *
 *    HDC   hDC         the target DC (optional)
 *
 *    LPSTR lpszFace    pointer the facename (required)
 *
 *    LPSTR lpszStyle   pointer to the style (like "Demibold Italic")
 *                      (optional).
 *
 *    short nPointSize  size in points (required)
 *
 *    WORD  wFlags      Flags, same as for ReallyCreateFont() except
 *                      RCF_BOLD and RCF_ITALIC are ignored if lpszStyle
 *                      is not NULL.
 *
 *  Comments:
 *
 ****************************************************************/

HFONT WINAPI
   ReallyCreateFontEx( HDC hDC, LPSTR lpszFace, LPSTR lpszStyle, short nPointSize, WORD wFlags )
{
   MYENUMFONT   mlf ;
   FONTENUMPROC lpfn ;
   HDC          hdcCur ;
   int          (WINAPI *lpfnEnumFont)(HDC,LPSTR,FONTENUMPROC,LPARAM) ;

   /*
    * On 3.0 call EnumFonts, on 3.1 call EnumFontFamilies.
    *
    * EnumFonts() is exported from GDI at ordinal #70
    * EnumFontFamilies() is exported from GDI at ordinal #330 (3.1 and above)
    */
   (FARPROC)lpfnEnumFont = GetProcAddress( GetModuleHandle( "GDI" ),
      (LPCSTR)(fWin30 ? MAKEINTRESOURCE( 70 ) : MAKEINTRESOURCE( 330 )) ) ;

   #ifdef IN_A_DLL
   lpfn = fnEnumReallyEx ;
   #else
   if (!(lpfn =
         (FONTENUMPROC)MakeProcInstance( (FARPROC)fnEnumReallyEx, hmodApp )))
   {
      return NULL ;
   }
   #endif

   //
   // if the DC wasn't specified then use the display...
   //
   if (!hDC)
   {
      if (!(hdcCur = GetDC( NULL )))
      {
         #ifndef IN_A_DLL
         FreeProcInstance( (FARPROC)lpfn ) ;
         #endif
         return FALSE ;
      }
   }
   else
      hdcCur = hDC ;

   if (hdcCur)
   {
      /*
       * EnumFontFamilies takes the family name as the second param.
       * For example the family name might be "Lucida Sans" and the
       * style might be "Demibold Roman".
       *
       * The last parameter is app. defined.  In our case we pass a
       * structure that has a LOGFONT and a char array as it's members.
       * We put the style in the char array, and when this function
       * returns the LOGFONT has the logfont for the font we want to create.
       */

      if (lpszStyle)
         lstrcpy( mlf.szStyle, lpszStyle ) ;
      else
         mlf.szStyle[0] = '\0' ;

      if (lpszFace)
         (*lpfnEnumFont)( hdcCur, lpszFace, lpfn, (LPARAM)(LPVOID)&mlf ) ;

      if (!lpszFace || lstrcmpi( mlf.lf.lfFaceName, lpszFace))
         GetObject( GetStockObject( ANSI_VAR_FONT ),
                    sizeof( LOGFONT ), (LPSTR)&mlf.lf ) ;

      //
      // See pages 4-48, 4-49, of Reference Vol. 1 for explaination
      // of why we set lfWidth to 0
      //
      mlf.lf.lfWidth = 0 ;

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
      mlf.lf.lfHeight = -MulDiv( nPointSize,
                             GetDeviceCaps( hdcCur, LOGPIXELSY),
                             72 )  ;

      mlf.lf.lfStrikeOut = (BYTE)((wFlags & RCF_STRIKEOUT) == RCF_STRIKEOUT) ;
      mlf.lf.lfUnderline = (BYTE)((wFlags & RCF_UNDERLINE) == RCF_UNDERLINE) ;

      if (!lpszStyle)
      {
         mlf.lf.lfWeight    = (wFlags & RCF_BOLD) ? FW_BOLD : FW_NORMAL ;
         mlf.lf.lfItalic    = (BYTE)((wFlags & RCF_ITALIC) == RCF_ITALIC) ;
      }

   }

   #ifndef IN_A_DLL
   FreeProcInstance( (FARPROC)lpfn ) ;
   #endif

   if (hdcCur != hDC)
      ReleaseDC( NULL, hdcCur ) ;

   return CreateFontIndirect( &mlf.lf ) ;

} /* ReallyCreateFontEx()  */


/****************************************************************
 *  int CALLBACK
 *    fnEnumReallyEx( LPLOGFONT lpLogFont,
 *                  LPTEXTMETRIC lpTextMetrics,
 *                  short nFontType, LPLOGFONT lpLF )
 *
 *  Description: 
 *
 *    Call back for EnumFonts and ReallySelectFontEx().
 *
 *    DO NOT FORGET TO EXPORT!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 *  Comments:
 *
 ****************************************************************/
int CALLBACK
fnEnumReallyEx( LPLOGFONT lpLogFont,
                LPNEWTEXTMETRIC lpTextMetrics,
                short nFontType,
                LPMYENUMFONT lpMLF )
{
   /*
    * If we are not true type, then we will only be called once,
    * in in this case we need to compare the weight and italic
    * members of the TM struct to some strings.
    */
   if (!(nFontType & TRUETYPE_FONTTYPE))
   {
      lpMLF->lf = *lpLogFont ;

      /*
       * Parse szStyle.  It may be something like "Bold Italic" or
       * "Demibold Italic".  Let's be somewhat smart about this.
       *
       * If "Demibold Italic" is passed in then he wants bold and
       * italic.
       *
       * We want to search szStyle for the word "bold".  Got a better
       * search strategy, go ahead and put it here (but tell me about it!).
       *
       * WARNING!!!
       *   The style string is language dependent.  The code below
       *   will not work international!
       */
      if (lpMLF->szStyle)
      {
         /*
          * Assume "Regular"
          */
         lpMLF->lf.lfWeight = FW_NORMAL ;
         lpMLF->lf.lfItalic = FALSE ;

         if (!lstrcmp( lpMLF->szStyle, szBold ))
         {
            lpMLF->lf.lfWeight = FW_BOLD ;
            return 0 ;
         }

         if (!lstrcmp( lpMLF->szStyle, szItalic))
         {
            lpMLF->lf.lfItalic = TRUE ;
            return 0 ;
         }

         if (!lstrcmp( lpMLF->szStyle, szBoldItalic ))
         {
            lpMLF->lf.lfWeight = FW_BOLD ;
            lpMLF->lf.lfItalic = TRUE ;
            return 0 ;
         }
      }

      return 0 ;  /* stop the enumeration */

   }

   /*
    * We now know we have a TT font.  For each style in the
    * family passed in, we will get here.  Check the
    * style with szStyle.
    *
    * If we find the style end the enumeration.
    *
    * In Win3.1 for TT fonts, there are two additional members in
    * the LOGFONT structure:
    *
    *    lf.lfFullName[2*LF_FACESIZE] ;
    *    lf.lfStyle[LF_FACESIZE] ;
    *
    * The macro's below help us get at this info.
    */
   #define LFFULLNAME(lf) ((LPSTR)lf.lfFaceName+LF_FACESIZE)
   #define LFSTYLE(lf)    ((LPSTR)lf.lfFaceName+3*LF_FACESIZE)

   lpMLF->lf = *lpLogFont ;

   /*
    * The compiler will just copy the standard LOGFONT members, so we
    * need to force to copy the additional ones!
    */
   lstrcpy( LFSTYLE(lpMLF->lf), LFSTYLE((*lpLogFont)) ) ;

   if (!lstrcmp( (LPSTR)LFSTYLE( lpMLF->lf ), lpMLF->szStyle ))
      return 0 ;

   return 1 ;

}/* fnEnumReallyEx() */
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
   short  nAveWidth ;
   char   rgchAlphabet[52] ;
   short  i ;

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

      nAveWidth = LOWORD( GetTextExtent( hDC,
                             (LPSTR)rgchAlphabet, 52 ) ) / 52 ;
   }

   //
   // Return the calculated average char width
   //
   return nAveWidth ;

} /* GetTextMetricsCorrectly()  */


/****************************************************************
 *  UINT WINAPI GetTextStyle( HDC hDC, UINT cbBuffer, LPSTR lpszStyle )
 *
 *  Description: 
 *
 *
 *    This function copies the style name of the current font into
 *    the buffer.  The style name is copied as a null-terminated
 *    string.
 *
 *    Parameter   Description
 *
 *    hDC         Identifies the device context. 
 *
 *    cbBuffer    Specifies the buffer size, in bytes. If the 
 *                typeface name is longer than the number of bytes 
 *                specified by this parameter, the name is truncated. 
 *
 *    lpszStyle   Points to the buffer for the typeface name. 
 *
 *    Returns
 *
 *       The return value specifies the number of bytes copied 
 *       to the buffer, not including the terminating null 
 *       character, if the function is successful. Otherwise, it 
 *       is zero. 
 *
 *
 *  Comments:
 *
 ****************************************************************/
UINT WINAPI GetTextStyle( HDC hDC, UINT cbBuffer, LPSTR lpszStyle )
{
   TEXTMETRIC  tm ;
   UINT        n ;
   LPSTR       lp ;

   /*
    * Call GetTextMetrics() to determine whether the font is a
    * TrueType font or not.
    */
   GetTextMetrics( hDC, &tm ) ;


   /*
    * For TrueType fonts we get the style name by using
    * GetOutlineTextMetrics().
    *
    * GetOutlineTextMetrics() is a 3.1 and above API.  Assume fWin30
    * was determined by calling GetVersion().
    *
    */
   if (!fWin30 && tm.tmPitchAndFamily & TMPF_TRUETYPE)
   {
      UINT                 cbOTMBuf ;
      LPOUTLINETEXTMETRIC  potm ;

      UINT  (WINAPI *lpfnGOTM)(HDC, UINT, LPOUTLINETEXTMETRIC);

      /*
       * GOTM() is exported from GDI.EXE at ordinal # 308
       */
      (FARPROC)lpfnGOTM = GetProcAddress( GetModuleHandle( "GDI" ),
                                 (LPCSTR)MAKEINTRESOURCE( 308 ) ) ;

      /*
       * Call GOTM() with NULL to retrieve the size of the
       * buffer.
       */
      cbOTMBuf = (*lpfnGOTM)( hDC, NULL, NULL ) ;

      if (cbOTMBuf == 0)
      {
         /* GetOutlineTextMetrics() failed!  */
         return 0 ;
      }

      /*
       * Allocate the memory for the OUTLINETEXTMETRIC structure plus
       * the strings.
       */
      potm = (LPOUTLINETEXTMETRIC)GlobalAllocPtr( GHND, cbOTMBuf ) ;

      if (potm)
      {
         potm->otmSize = cbOTMBuf ;
         
         /* 
          * Call GOTM() with the pointer to the buffer.  It will
          * fill in the buffer.
          */
         if (!(*lpfnGOTM)( hDC, cbOTMBuf, potm ))
         {
             /* GetOutlineTextMetrics failed! */
             return 0 ;
         }
         
         /*
          * Do something useful with the string buffers.  Note that to 
          * access the string buffers, the otmp???Name members are used
          * as OFFSETS into the buffer.  They *ARE NOT* pointers them-
          * selves.
          */
         for( n = 0, lp = (LPSTR)potm+(UINT)potm->otmpStyleName ;
              lp[n] != '\0' ;
              n++ )
         {
            if (n == (cbBuffer - 1))
               break ;

            lpszStyle[n] = lp[n] ;
         }

         lpszStyle[n] = '\0' ;
         
         /* Don't forget to free the memory! */
         GlobalFreePtr( potm ) ;
      }
      else
      {
         /* GlobalAllocPtr failed */
         return 0 ;
      }
   }
   else
   {
      /* 
       * It was not a truetype font, or we're in Windows 3.0.  
       */
      char        szStyle[LF_FACESIZE] ;

      /*
       * Fabricate a style string.  Important!:  The strings
       * "Italic", "Bold", and "Regular" are only valid
       * for English.  On versions of Windows localized for
       * other countries these strings will differ.
       */
      szStyle[0] = '\0' ;

      if (tm.tmWeight >= FW_BOLD)
         lstrcpy( szStyle, "Bold " ) ;

      /*
       * If we have "Bold Italic" concatenate.
       */
      lp = szStyle + lstrlen( szStyle ) ;

      if (tm.tmItalic)
         lstrcpy( lp, "Italic" ) ;

      if (!lstrlen( szStyle ))
         lstrcpy( szStyle, "Regular" ) ;

      /* 
       * szStyle now holds what is equivalent to the otmpStyleName
       * member.
       */
      for( n = 0 ; szStyle[n] != '\0' ; n++ )
      {
         if (n == (cbBuffer - 1))
            break ;

         lpszStyle[n] = szStyle[n] ;
      }

      lpszStyle[n] = '\0' ;
   }

   return lstrlen( lpszStyle ) ;

} /* GetTextStyle()  */


/****************************************************************
 *  UINT WINAPI GetTextFullName( HDC hDC, UINT cbBuffer, LPSTR lpszName )
 *
 *  Description: 
 *
 *
 *    This function copies the "full name" of the current font into
 *    the buffer.  The name is copied as a null-terminated
 *    string.
 *
 *    Parameter   Description
 *
 *    hDC         Identifies the device context. 
 *
 *    cbBuffer    Specifies the buffer size, in bytes. If the 
 *                typeface name is longer than the number of bytes 
 *                specified by this parameter, the name is truncated. 
 *
 *    lpszName   Points to the buffer for the typeface fullname. 
 *
 *    Returns
 *
 *       The return value specifies the number of bytes copied 
 *       to the buffer, not including the terminating null 
 *       character, if the function is successful. Otherwise, it 
 *       is zero. 
 *
 *
 *  Comments:
 *
 ****************************************************************/
UINT WINAPI GetTextFullName( HDC hDC, UINT cbBuffer, LPSTR lpszName )
{
   TEXTMETRIC  tm ;
   UINT        n ;
   LPSTR       lp ;

   /*
    * Call GetTextMetrics() to determine whether the font is a
    * TrueType font or not.
    */
   GetTextMetrics( hDC, &tm ) ;


   /*
    * For TrueType fonts we get the full name by using
    * GetOutlineTextMetrics().
    *
    * GetOutlineTextMetrics() is a 3.1 and above API.  Assume fWin30
    * was determined by calling GetVersion().
    *
    */
   if (!fWin30 && tm.tmPitchAndFamily & TMPF_TRUETYPE)
   {
      UINT                 cbOTMBuf ;
      LPOUTLINETEXTMETRIC  potm ;

      UINT  (WINAPI *lpfnGOTM)(HDC, UINT, LPOUTLINETEXTMETRIC);

      /*
       * GOTM() is exported from GDI.EXE at ordinal # 308
       */
      (FARPROC)lpfnGOTM = GetProcAddress( GetModuleHandle( "GDI" ),
                                 (LPCSTR)MAKEINTRESOURCE( 308 ) ) ;

      /*
       * Call GOTM() with NULL to retrieve the size of the
       * buffer.
       */
      cbOTMBuf = (*lpfnGOTM)( hDC, NULL, NULL ) ;

      if (cbOTMBuf == 0)
      {
         /* GetOutlineTextMetrics() failed!  */
         return 0 ;
      }

      /*
       * Allocate the memory for the OUTLINETEXTMETRIC structure plus
       * the strings.
       */
      potm = (LPOUTLINETEXTMETRIC)GlobalAllocPtr( GHND, cbOTMBuf ) ;

      if (potm)
      {
         potm->otmSize = cbOTMBuf ;
         
         /* 
          * Call GOTM() with the pointer to the buffer.  It will
          * fill in the buffer.
          */
         if (!(*lpfnGOTM)( hDC, cbOTMBuf, potm ))
         {
             /* GetOutlineTextMetrics failed! */
             return 0 ;
         }
         
         /*
          * Do something useful with the string buffers.  Note that to 
          * access the string buffers, the otmp???Name members are used
          * as OFFSETS into the buffer.  They *ARE NOT* pointers them-
          * selves.
          */
         for( n = 0, lp = (LPSTR)potm+(UINT)potm->otmpFullName ;
              lp[n] != '\0' ;
              n++ )
         {
            if (n == (cbBuffer - 1))
               break ;

            lpszName[n] = lp[n] ;
         }

         lpszName[n] = '\0' ;
         
         /* Don't forget to free the memory! */
         GlobalFreePtr( potm ) ;
      }
      else
      {
         /* GlobalAllocPtr failed */
         return 0 ;
      }
   }
   else
   {
      *lpszName = '\0' ;
      return 0 ;
   }

   return lstrlen( lpszName ) ;

} /* GetTextFullName()  */


/****************************************************************
 *  UINT WINAPI GetTextPointSize( HDC hDC )
 *
 *  Description:
 *
 *     This function retreives the point size of the current font.
 *     The point size is calculated using the following formula:
 *
 *     nPtSize = MulDiv (lf.lfHeight, 72, GetDeviceCaps(hDC, LOGPIXELSY);
 *
 *     A point is defined as 1/72 of an inch. 
 *
 *  Comments:
 *
 ****************************************************************/
UINT WINAPI GetTextPointSize( HDC hDC )
{
   TEXTMETRIC tm ;

   GetTextMetrics( hDC, &tm ) ;

   return MulDiv( tm.tmHeight - tm.tmInternalLeading,
                  72,
                  GetDeviceCaps( hDC, LOGPIXELSY ) );

} /* GetTextPointSize()  */

/****************************************************************
 *  BOOL WINAPI GetTextExtentABCPoint( HDC hDC, LPSTR lpsz, int cb, LPSIZE lpSize )
 *
 *  Description: 
 *
 *    Just like GetTextExtentPoint() but only works on TT fonts
 *    and uses GetCharABCWidths().
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI GetTextExtentABCPoint( HDC hDC, LPSTR lpsz, int cb, LPSIZE lpSize )
{
   LPABC lpABC ;

   if (lpABC = GlobalAllocPtr( GHND, 256*sizeof(ABC) ))
   {
      if (GetCharABCWidths( hDC, 0, 255, lpABC ))
      {
         int         n ;
         TEXTMETRIC  tm ;

         GetTextMetrics( hDC, &tm ) ;

         /*
          * I'm not so sure about this height thing.  I'm not 100%
          * positive that GetTextExtent() uses the TM values to
          * calculate the height.  It may actually return the
          * *actual* bitmap height as calculated by the
          * driver.
          *
          * If that's the case then this function behaves
          * differently than GetTextExtent().  Sorry.
          */

         lpSize->cy = tm.tmHeight + tm.tmInternalLeading ;

         /*
          * We now have an array of ABC structures
          */
         for ( lpSize->cx = 0, n = 0 ; n <= cb ; n++ )
         {
            lpSize->cx += (
                           lpABC[(int)lpsz[n]].abcA + 
                           lpABC[(int)lpsz[n]].abcB +
                           lpABC[(int)lpsz[n]].abcC ) ;
         }

      }
      else
      {
         DP1( hWDB, "Could not GetCharABCWidths!" ) ;
         GlobalFreePtr( lpABC ) ;
         return GetTextExtentPoint( hDC, lpsz, cb, lpSize ) ;
      }

      GlobalFreePtr( lpABC ) ;
      return TRUE ;
   }

   return FALSE ;

} /* GetTextExtentABCPoint()  */



/************************************************************************
 * End of File: fontutil.c
 ***********************************************************************/

