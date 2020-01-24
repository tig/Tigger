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
 *    The primary use for the functions in this file is for apps that
 *    need the relatively simplistic font model presented in the
 *    ChooseFont common dialog box.  ChooseFont deals with fonts using
 *    three primary variables (discount colors and effects for now...):
 *
 *    --------------------------------------------------
 *    Face Name               Style          Point Size
 *    --------------------------------------------------
 *
 *    The functions below allow you to create and interogate fonts
 *    in a manner that is in sync with the ChooseFont model.  For example
 *    you can create a font using "ReallyCreateFontEx()" by specifying
 *    the facename, style, and point size.  Also, functions are
 *    provided for retrieving information about a font like the
 *    style name ("GetTextStyle()").
 *
 *   Revisions:  
 *
 *     0.0  6/ 7/91 cek   Pulled together from various files.
 *     1.0  1/29/92 cek   Modified to use 3.1 features.
 *     1.1  4/29/92 cek   Fixed bugs
 *
 ***********************************************************************/

   #include "PRECOMP.H"
   #include "CONXLIBi.h"

#define SOME_SOPHISTICATED_APP

#if 0

#define STRICT
#define _WINDLL
#include <windows.h>
#include <windowsx.h>
#include <ctype.h>

#include "fontutil.h"
#define hmodApp hinstApp

#endif

/*
 * If you put this code in a DLL #define IN_A_DLL
 */
#define IN_A_DLL

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
 *  are not constant for other languages.  For example 'bold' is not
 *  'bold' in German, it's something like 'der bolderstang'.
 */
#ifdef SOME_SOPHISTICATED_APP

//#include "..\inc\isz.h"
extern LPSTR rglpsz[] ;

#define szBold       rglpsz[IDS_BOLD] 
#define szItalic     rglpsz[IDS_ITALIC]
#define szBoldItalic rglpsz[IDS_BOLDITALIC]
#define szRegular    rglpsz[IDS_REGULAR]

#else

char szBold[]        = "Bold" ;
char szItalic[]      = "Italic" ;
char szBoldItalic[]  = "Bold Italic" ;
char szRegular[]     = "Regular" ;

#endif

static LPABC lpABC = NULL ;

//#define WORK_IN_WIN30

#ifdef WORK_IN_WIN30
/* Wrapper functions we use so we can run in 3.0 and 3.1 since
 * GetOutlineTextMetrics() et al do not exist in 3.0 and we'll fail
 * loading if kernel 3.0 sees we're importing externals that don't
 * exist in GDI.
 */
UINT NEAR PASCAL MyGetOutlineTextMetrics(HDC hdc, UINT ui, OUTLINETEXTMETRIC FAR* lpOTM ) ;
BOOL NEAR PASCAL MyGetCharABCWidths(HDC hdc, UINT ui1, UINT ui2, ABC FAR* lpABC) ;
BOOL NEAR PASCAL MyGetTextExtentPoint(HDC, LPCSTR, int, SIZE FAR*);

#else

/*
 * If we are a 3.1 only app just call the 3.1 api's directly!
 */
#define MyGetOutlineTextMetrics GetOutlineTextMetrics
#define MyGetCharABCWidths      GetCharABCWidths
#define MyGetTextExtentPoint    GetTextExtentPoint

#endif

/*
 * NOTE: You *must* export this callback function in your DEF file!!
 */
int CALLBACK
fnEnumReallyEx( LPENUMLOGFONT lpLogFont,
                LPNEWTEXTMETRIC lpTextMetrics,
                short nFontType,
                LPENUMLOGFONT lpELF ) ;

/****************************************************************
 *  HFONT WINAPI
 *    ReallyCreateFont( HDC hDC, LPSTR lpszFace,
 *                      short nPointSize,
 *                      WORD wFlags )
 *
 *  Description: 
 *
 *    Creates a font like CreateFont, but attempts to avoid the
 *    font randomizer.  The ReallyCreateFont() function is
 *    somewhat obsolete for Windows 3.1 since it does not
 *    corretly handle style strings like ReallyCreateFontEx() does.
 *    It is provided here for compatibility with apps that
 *    link with FONTUTIL.C.
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
 *          RCF_NODEFAULT  -  Will not return a default font if
 *                            lpszFace does not exist on the system.
 *
 *  Return Value:
 *
 *    The return value is a handle to the font (HFONT) if the function
 *    was successful, otherwise it is NULL.  Note that you *must*
 *    eventually call DeleteObject() to destroy the font (unless
 *    RCF_NODEFAULT is used).
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
   ENUMLOGFONT  elf ;
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
   lpfn = (FONTENUMPROC)fnEnumReallyEx ;
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
         lstrcpy( elf.elfStyle, lpszStyle ) ;
      else
         elf.elfStyle[0] = '\0' ;

      if (lpszFace)
         (*lpfnEnumFont)( hdcCur, lpszFace, lpfn, (LPARAM)(LPVOID)&elf ) ;


      if (!lpszFace || lstrcmpi( elf.elfLogFont.lfFaceName, lpszFace))
      {
         if ((wFlags & RCF_NODEFAULT) == RCF_NODEFAULT)
         {
            #ifndef IN_A_DLL
            FreeProcInstance( (FARPROC)lpfn ) ;
            #endif

            if (hdcCur != hDC)
               ReleaseDC( NULL, hdcCur ) ;

            return NULL ;

         }
         else
            GetObject( GetStockObject( ANSI_VAR_FONT ),
                       sizeof( LOGFONT ), (LPSTR)&elf.elfLogFont ) ;
      }

      //
      // See pages 4-48, 4-49, of Reference Vol. 1 for explaination
      // of why we set lfWidth to 0
      //
      elf.elfLogFont.lfWidth = 0 ;

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
      elf.elfLogFont.lfHeight = -MulDiv( nPointSize,
                             GetDeviceCaps( hdcCur, LOGPIXELSY),
                             72 )  ;

      elf.elfLogFont.lfStrikeOut = (BYTE)((wFlags & RCF_STRIKEOUT) == RCF_STRIKEOUT) ;
      elf.elfLogFont.lfUnderline = (BYTE)((wFlags & RCF_UNDERLINE) == RCF_UNDERLINE) ;

      if (!lpszStyle)
      {
         elf.elfLogFont.lfWeight    = (wFlags & RCF_BOLD) ? FW_BOLD : FW_NORMAL ;
         elf.elfLogFont.lfItalic    = (BYTE)((wFlags & RCF_ITALIC) == RCF_ITALIC) ;
      }

   }

   #ifndef IN_A_DLL
   FreeProcInstance( (FARPROC)lpfn ) ;
   #endif

   if (hdcCur != hDC)
      ReleaseDC( NULL, hdcCur ) ;

   return CreateFontIndirect( &elf.elfLogFont ) ;

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
fnEnumReallyEx( LPENUMLOGFONT lpELFIN,
                LPNEWTEXTMETRIC lpTextMetrics,
                short nFontType,
                LPENUMLOGFONT lpELF )
{
   /* If we are not true type, then we will only be called once,
    * in in this case we need to compare the weight and italic
    * members of the TM struct to some strings.
    */
   if (!(nFontType & TRUETYPE_FONTTYPE))
   {
      lpELF->elfLogFont = lpELFIN->elfLogFont ;

      /* Parse szStyle.  It may be something like "Bold Italic" or
       * "Demibold Italic".  Let's be somewhat smart about this.
       *
       * If "Demibold Italic" is passed in then he wants bold and
       * italic.
       *
       * We want to search szStyle for the word "bold".  Got a better
       * search strategy, go ahead and put it here (but tell me about it!).
       *
       * WARNING!!!
       *   The style string is language dependent.
       */
      if (lpELF->elfStyle)
      {
         /*
          * Assume "Regular"
          */
         lpELF->elfLogFont.lfWeight = FW_NORMAL ;
         lpELF->elfLogFont.lfItalic = FALSE ;

         if (!lstrcmpi( lpELF->elfStyle, szBold ))
         {
            lpELF->elfLogFont.lfWeight = FW_BOLD ;
            return 0 ;
         }

         if (!lstrcmpi( lpELF->elfStyle, szItalic))
         {
            lpELF->elfLogFont.lfItalic = TRUE ;
            return 0 ;
         }

         if (!lstrcmpi( lpELF->elfStyle, szBoldItalic ))
         {
            lpELF->elfLogFont.lfWeight = FW_BOLD ;
            lpELF->elfLogFont.lfItalic = TRUE ;
            return 0 ;
         }
      }

      return 0 ;  /* stop the enumeration */

   }

   /* We now know we have a TT font.  For each style in the
    * family passed in, we will get here. 
    */
    if (0==lstrcmpi( lpELF->elfStyle, lpELFIN->elfStyle ))
    {
        *lpELF = *lpELFIN;
        return 0;
    }
    return 1;

}/* fnEnumReallyEx() */

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
   if (lpTM)
      GetTextMetrics(hDC, lpTM);

   //
   // If it's not a variable pitch font GetTextMetrics was correct
   // so just return.
   //
   if (lpTM && !(lpTM->tmPitchAndFamily & FIXED_PITCH))  
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
   char        szStyle[LF_FACESIZE] ;

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

      cbOTMBuf = MyGetOutlineTextMetrics( hDC, NULL, NULL ) ;

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
         BOOL  fSim = FALSE ;

         potm->otmSize = cbOTMBuf ;
         
         /* 
          * Call GOTM() with the pointer to the buffer.  It will
          * fill in the buffer.
          */
         if (!MyGetOutlineTextMetrics( hDC, cbOTMBuf, potm ))
         {
             /* GetOutlineTextMetrics failed! */
             return 0 ;
         }

         szStyle[0] = '\0' ;


         /*
          * Is this a simulated bold font?
          */
         if ((tm.tmWeight > FW_NORMAL) &&
             !(potm->otmfsSelection & 0x0020)) 
         {
            lstrcpy( szStyle, szBold ) ;
            fSim = TRUE ;
         }

         /*
          * Is italic simulated, or was bold simulated and we have
          * italic?
          */
         if (tm.tmItalic && (fSim || !(potm->otmfsSelection & 0x0001)))
         {
            /*
             * Concat Italic
             */
            if (szStyle[0] != '\0')
               lstrcpy( szStyle, szBoldItalic ) ;
            else
               lstrcpy( szStyle, szItalic ) ;

            fSim = TRUE ;
         }

         if (fSim)
         {
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
         else
         {
         
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
         }

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

      /*
       * Fabricate a style string.  Important!:  The strings
       * "Italic", "Bold", and "Regular" are only valid
       * for English.  On versions of Windows localized for
       * other countries these strings will differ.
       */
      szStyle[0] = '\0' ;

      if (tm.tmWeight > FW_NORMAL)
         lstrcpy( szStyle, szBold ) ;

      /*
       * If we have "Bold Italic" concatenate.
       */
      if (tm.tmItalic)
      {
         if (szStyle[0])
            lstrcpy( szStyle, szBoldItalic ) ;
         else
            lstrcpy( szStyle, szItalic ) ;
      }

      if (!lstrlen( szStyle ))
         lstrcpy( szStyle, szRegular ) ;

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


#if 0

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


      /*
       * Call GOTM() with NULL to retrieve the size of the
       * buffer.
       */
      cbOTMBuf = MyGetOutlineTextMetrics( hDC, NULL, NULL ) ;

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
         if (!MyGetOutlineTextMetrics( hDC, cbOTMBuf, potm ))
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

#endif // #if 0

/****************************************************************
 *  BOOL NEAR PASCAL IsBoldSimulated( HDC hDC )
 *
 *  Description: 
 *
 *       This function allows us to get around a bug
 *    in 3.1 where simulated bold fonts have incorrect ABC data.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL IsBoldSimulated( HDC hDC )
{
   TEXTMETRIC                 tm ;
   OUTLINETEXTMETRIC          otm ;

   GetTextMetrics( hDC, &tm ) ;

   if (tm.tmWeight <= FW_NORMAL)
      return FALSE ;

   otm.otmSize = sizeof( OUTLINETEXTMETRIC ) ;
         
   /* 
    * Call GOTM() with the pointer to the buffer.  It will
    * fill in the buffer.
    */
   if (!MyGetOutlineTextMetrics( hDC, otm.otmSize, &otm ))
      return FALSE ;

   if (!(otm.otmfsSelection & 0x0020)) // bit 5 means bold
      return TRUE ;

   return FALSE ;

} /* IsBoldSimulated()  */
         

/****************************************************************
 *  UINT WINAPI GetTextExtentABCPoint( HDC hDC, LPSTR lpsz, int cb, LPSIZE lpSize )
 *
 *  Description: 
 *
 *    Just like GetTextExtentPoint() but only works on TT fonts
 *    and uses GetCharABCWidths().  The only problem with
 *    this function is that it does not correctly account a negative
 *    A value for the first character or a negative C value
 *    for the last character.  Use GetTextBoundingSize for
 *    that.
 *
 *    Returns the absolute value for the A value of the first
 *    character if it is negative otherwise it returns 0.
 *
 *  Comments:
 *
 ****************************************************************/
UINT WINAPI GetTextExtentABCPoint( HDC hDC, LPSTR lpsz, int cb, LPSIZE lpSize )
{
   int   A, C ;
   UINT  nRet = 0 ;

   nRet = MyGetTextExtentPoint( hDC, lpsz, cb, lpSize );

   if (!fWin30 && (lpABC || (lpABC = (LPABC)GlobalAllocPtr( GHND, 256*sizeof(ABC) ))))
   {
      nRet = 0 ;

      if (MyGetCharABCWidths( hDC, 0, 255, lpABC ))
      {
         int         n ;

         /*
          * We now have an array of ABC structures
          */
         for ( lpSize->cx = 0, n = 0 ; n < cb ; n++ )
         {
            lpSize->cx += (
                           lpABC[(int)lpsz[n]].abcA + 
                           lpABC[(int)lpsz[n]].abcB +
                           lpABC[(int)lpsz[n]].abcC ) ;
         }

         if( cb )
         {

            A = lpABC[(int)lpsz[0]].abcA;

            if( A < 0 )
            {
               nRet = -A ;
               lpSize->cx -= A;
            }

            C = lpABC[(int)lpsz[cb-1]].abcC ;

            if ( C < 0 )
            {
               lpSize->cx -= C ;    // two minus' make a plus
            }

         }


         /*
          * Bug in Windows 3.1.  If the tt font is emboldend then
          * add one to the width for each char.
          */

         if ((BOOL)LOWORD( GetVersion() ) == 0x0A03 &&
             IsBoldSimulated( hDC ))
         {
            lpSize->cx += cb ;
         }

      }
      else
      {
         return MyGetTextExtentPoint( hDC, lpsz, cb, lpSize ) ;
      }

      return nRet  ;
   }

   return nRet ;

} /* GetTextExtentABCPoint()  */

#if 0
#ifdef WORK_IN_WIN30

/* The functions below are wrappers for the 3.1 functions that do
 * not exist in 3.0.
 */

/****************************************************************
 *  UINT NEAR PASCAL
 *    MyGetOutlineTextMetrics(HDC hdc, UINT ui, OUTLINETEXTMETRIC FAR* lpOTM )
 *
 *  Description: 
 *
 *    This function provides a wrapper around the GetOutLineTextMetrics()
 *    function that let's us stay compatible with 3.0
 *
 ****************************************************************/
UINT NEAR PASCAL
   MyGetOutlineTextMetrics(HDC hdc, UINT ui, OUTLINETEXTMETRIC FAR* lpOTM )
{
   static UINT  (WINAPI *lpfnGOTM)(HDC, UINT, LPOUTLINETEXTMETRIC) = NULL ;

   /*
    * GOTM() is exported from GDI.EXE at ordinal # 308
    */
   if (lpfnGOTM == NULL)
      (FARPROC)lpfnGOTM = GetProcAddress( GetModuleHandle( "GDI" ),
                                 (LPCSTR)MAKEINTRESOURCE( 308 ) ) ;

   if (lpfnGOTM != NULL)
      return (*lpfnGOTM)( hdc, ui, lpOTM ) ;

} /* MyGetOutlineTextMetrics()  */


/****************************************************************
 * BOOL NEAR PASCAL MyGetCharABCWidths(HDC hdc, UINT ui1, UINT ui2, ABC FAR* lpABC)
 *
 *  Description: 
 *
 *    This function provides a wrapper around the GetCharABCWidths()
 *    function that let's us stay compatible with 3.0
 *
 ****************************************************************/
BOOL NEAR PASCAL MyGetCharABCWidths(HDC hdc, UINT ui1, UINT ui2, ABC FAR* lpABC)
{
   static BOOL (WINAPI *lpfnGABCW)(HDC, UINT, UINT, LPABC) = NULL ;

   /*
    * GABCW() is exported from GDI.EXE at ordinal # 307
    */
   if (lpfnGABCW == NULL)
      (FARPROC)lpfnGABCW = GetProcAddress( GetModuleHandle( "GDI" ),
                                 (LPCSTR)MAKEINTRESOURCE( 307 ) ) ;

   if (lpfnGABCW != NULL)
      return (*lpfnGABCW)( hdc, ui1, ui2, lpABC ) ;

} /* MyGetCharABCWidths()  */

/****************************************************************
 *  BOOL MyGetTextExtentPoint(HDC, LPCSTR, int, SIZE FAR*)
 *
 *  Description: 
 *
 *    This function provides a wrapper around the GetTextExtentPoint()
 *    API which is not in 3.0.  I like GetTextExtentPoint() 'cause
 *    it is Win32 compatible and GetTextExtent() is not.
 *
 *
 ****************************************************************/
BOOL NEAR PASCAL MyGetTextExtentPoint(HDC hdc, LPCSTR lpcsz, int i, SIZE FAR* lpSize)
{
   static BOOL (WINAPI *lpfnGTEP)(HDC, LPCSTR, int, SIZE FAR*) = NULL ;

   /*
    * GTEP() is exported from GDI.EXE at ordinal # 471
    */
   if (lpfnGTEP == NULL && !fWin30)
      (FARPROC)lpfnGTEP = GetProcAddress( GetModuleHandle( "GDI" ),
                                 (LPCSTR)MAKEINTRESOURCE( 471 ) ) ;

   if (lpfnGTEP != NULL)
      return (*lpfnGTEP)( hdc, lpcsz, i, lpSize ) ;
   else
   {
      DWORD dw ;
      SIZE Size ;

      dw = GetTextExtent( hdc, lpcsz, i ) ;

      Size.cx = LOWORD( dw ) ;
      Size.cy = HIWORD( dw ) ;

      return  TRUE ;
   }

} /* MyGetTextExtentPoint()  */

#endif // IF WORK_IN_30

#endif // #if 0

/************************************************************************
 * End of File: fontutil.c
 ***********************************************************************/

