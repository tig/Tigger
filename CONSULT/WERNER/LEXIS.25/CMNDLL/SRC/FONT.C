/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  font.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *    This module has routines in it for handling fonts.  It provides
 *    facilities for selecting and displaying fonts.  The idea here
 *    is to provide a consistent way to get the font you want and to
 *    provide utility functions for font selection.
 *
 *    A SelectFont Dialog box may be added sometime...
 *
 *   Revisions:  
 *     00.00.000  2/ 9/91 cek   First Version
 *     00.00.001  4/ 4/91 cek   Modified to work with Win31
 *
 *
 ************************************************************************/

#define WINDLL
#include "..\inc\undef.h"
#include <windows.h>
#include <string.h>
#include <stdlib.h>

#include "..\inc\cmndll.h"
#include "..\inc\cmndlld.h"

/************************************************************************
 * Imported variables
 ************************************************************************/

extern HANDLE hInst ;      // DLL's instance

/************************************************************************
 * Local Defines
 ************************************************************************/
#define MAX_TITLELEN  64 
#define HLSS_PROP "hlSS"
#define SAMPLESTRING "AaBbYyZz 0123"

#define MIN_VECTORFONT_SIZE   4
#define VECTORFONT_MULT       2

/************************************************************************
 * Macros 
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/
int FAR PASCAL
   fnFontSelect( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;

BOOL NEAR PASCAL DrawSample( HWND hDlg, LOCALHANDLE lhSS ) ;

int FAR PASCAL
   fnEnumFaces( LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetrics,
                short nFontType, LPENUMFACE lpEF ) ;
BOOL NEAR PASCAL
OkFont( LPLOGFONT lplf, LPTEXTMETRIC lptm, WORD nFontType, WORD wFontType ) ;

int FAR PASCAL
   fnEnumSizes( LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetrics,
                short nFontType, LPENUMSIZE lpES ) ;

BOOL NEAR PASCAL SetItems( HWND hDlg, LOCALHANDLE lhSS ) ;
BOOL NEAR PASCAL GetFaceSize( LPSTR lpszFace, short FAR *pnH, short FAR *pnW );

typedef struct tagSELECTSTRUCT
{
   char     szTitle[MAX_TITLELEN] ;
   WORD     wFlags ;
   EXTLOGFONT  lf ;
   EXTLOGFONT  lfDefault ;
   WORD     wFontType ;
   HDC      hDC ;
   HDC      hPrnDC ;

} SELECTSTRUCT, *PSELECTSTRUCT, FAR *LPSELECTSTRUCT ;

/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * Exported Functions
 ************************************************************************/

/*** New functions 9/17/91 cek        ***/
/*
 * The font enumeration stuff needs the hInstance of the EXE (or DLL)
 * that contains the fnEnumReally() function for callback.  Thus
 * the following hInst var. needs to be exported from
 * the app's WinMain() or the DLL's LibMain().
 */

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
 *  HFONT FAR PASCAL
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
HFONT FAR PASCAL
ReallyCreateFont( HDC hDC, LPSTR lpszFace, short nPointSize, WORD wFlags )
{
   LOGFONT     lf ;
   FARPROC     lpfnEnumReally ;
   HDC         hCurDC ;

   //
   // First use font enumeration get the facename specified.
   // If that fails use a font based off of ANSI_VAR_FONT
   //

   if (!(lpfnEnumReally = MakeProcInstance( fnEnumReally, hInst )))
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
         FreeProcInstance( lpfnEnumReally ) ;
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
         EnumFonts( hCurDC, lpszFace, lpfnEnumReally, (LPSTR)&lf ) ;

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

   FreeProcInstance( lpfnEnumReally ) ;

   if (hCurDC != hDC)
      ReleaseDC( NULL, hCurDC ) ;


   return CreateFontIndirect( &lf ) ;

} /* ReallyCreateFont()  */


/****************************************************************
 *  int FAR PASCAL
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
   *lpLF = *lpLogFont ;

   //
   // Only need to be called once!
   //
   return FALSE ;

}/* fnEnumReally() */
/************************************************************************
 *
 *  end of REALLY SELECT A FONT STUFF
 *
 ************************************************************************/


/****************************************************************
 *  int FAR PASCAL
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
int FAR PASCAL
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


/*************************************************************************
 *  BOOL FAR PASCAL FontSelectIndirect( HDC hDC, LPLOGFONT lpLogFont )
 *
 *  Description:
 *
 *    Returns a pointer to a LOGFONT strcture that is completely filled out
 *    with a font available on the specified device context.  It takes a
 *    partially specified LOGFONT and looks at the lfFaceName and lfHeight
 *    items to find the corresponding physical font.  If lfFaceName does
 *    not exist on the device FALSE is returned.
 *
 *  Type/Parameter
 *          Description
 *
 *    HDC   HDC
 *          Handle to the device context that the font is to be taken from.
 *          if hDC is NULL then the display's device context will be used.
 *
 *    LPLOGFONT   lpLogFont
 *          Pointer to a LOGFONT structure that has at least the lfFaceName
 *          and lfHeight fields filled out.
 * 
 *  Return Value
 *    TRUE if a matching FaceName was found.  FALSE otherwise.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL FontSelectIndirect( HDC hDC, LPLOGFONT lpLogFont )
{
   FARPROC     lpfnEnumSizes ;
   ENUMSIZE    es ;
   HDC         hCurDC ;
   HFONT       hFont ;

   BOOL        fRet = TRUE ;

   //
   // Enumerate sizes.  If the EnumFont fails then the lfFaceName is 
   // invalid.
   //
   es.nNumSizes = 0 ;

   if (!(lpfnEnumSizes = MakeProcInstance( fnEnumSizes, hInst )))
   {
      DP1( hWDB, "MakeProcInstance failed (FontSelectIndirect)" ) ;
      return FALSE ;
   }

   //
   // if the DC wasn't specified then use the display...
   //
   if (!hDC)
   {
      if (!(hCurDC = GetDC( NULL )))
      {
         DP1( hWDB,"Could not GetDC (FontSelectIndirect)") ;
         FreeProcInstance( lpfnEnumSizes ) ;
         return FALSE ;
      }
   }
   else
      hCurDC = hDC ;

   if (hCurDC && *lpLogFont->lfFaceName)
   {
      /* HACK!  This should be totally re-written */
      if (lpLogFont->lfWidth)
      {
         if (!EnumFonts( hCurDC, lpLogFont->lfFaceName,
                        lpfnEnumSizes, (LPSTR)&es ))
         {
            DP1( hWDB,"EnumFonts failed (FontSelectIndirect)") ;
            fRet = FALSE ;
            goto END ;
         }
         if (!lpLogFont->lfWidth)
            lpLogFont->lfWidth = es.lf.lfWidth ;

         if (!lpLogFont->lfHeight)
            lpLogFont->lfHeight = es.lf.lfHeight ;

         if (!lpLogFont->lfWeight)
            lpLogFont->lfWeight = es.lf.lfWeight ;

         if (!lpLogFont->lfItalic)
            lpLogFont->lfItalic = es.lf.lfItalic ;

         if (!lpLogFont->lfUnderline)
            lpLogFont->lfUnderline = es.lf.lfUnderline ;

         if (!lpLogFont->lfStrikeOut)
            lpLogFont->lfStrikeOut = es.lf.lfStrikeOut ;

         lpLogFont->lfEscapement = es.lf.lfEscapement ;
         lpLogFont->lfOrientation = es.lf.lfOrientation ;
         lpLogFont->lfCharSet = es.lf.lfCharSet ;
         lpLogFont->lfOutPrecision = es.lf.lfOutPrecision ;
         lpLogFont->lfClipPrecision = es.lf.lfClipPrecision ;
         lpLogFont->lfQuality = es.lf.lfQuality ;
         lpLogFont->lfPitchAndFamily = es.lf.lfPitchAndFamily ;
      }
      else
      {
         //    pt = (height * 72) / number of pixels in the Y direction
         short nPoints = MulDiv( lpLogFont->lfHeight,
                                 72, GetDeviceCaps( hCurDC, LOGPIXELSY) ) ;

         if (hFont = ReallyCreateFont( hCurDC,
                     lpLogFont->lfFaceName,
                     nPoints,
                     (lpLogFont->lfWeight >= FW_BOLD ? RCF_BOLD : RCF_NORMAL) |
                     (lpLogFont->lfItalic ? RCF_ITALIC : RCF_NORMAL)          |
                     (lpLogFont->lfStrikeOut ? RCF_STRIKEOUT : RCF_NORMAL)    |
                     (lpLogFont->lfUnderline ? RCF_UNDERLINE : RCF_NORMAL) ))
         {
            GetObject( hFont, sizeof( LOGFONT ), lpLogFont ) ;
            DeleteObject( hFont ) ;
         }  
      }
   }

END:
   FreeProcInstance( lpfnEnumSizes ) ;
   if (hCurDC != hDC)
      ReleaseDC( NULL, hCurDC ) ;

   return fRet ;
}/* FontSelectIndirect() */


/*************************************************************************
 *  void FAR PASCAL FontInitLogFont( LPLOGFONT )
 *
 *  Description: 
 *
 *   This function initilizes a LOGFONT structure to default values.
 *
 *  Comments:
 *
 *************************************************************************/
void FAR PASCAL FontInitLogFont( LPLOGFONT lplf )
{
   lplf->lfHeight = 0 ;
   lplf->lfWidth = 0 ;
   lplf->lfEscapement = 0 ;
   lplf->lfOrientation = 0 ;
   lplf->lfWeight = 0 ;
   lplf->lfItalic = 0 ;
   lplf->lfUnderline = 0 ;
   lplf->lfStrikeOut = 0 ;
   lplf->lfCharSet = ANSI_CHARSET ;
   lplf->lfOutPrecision = OUT_DEFAULT_PRECIS ;
   lplf->lfClipPrecision = OUT_DEFAULT_PRECIS ;
   lplf->lfQuality = DEFAULT_QUALITY ;
   lplf->lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS ;
   *lplf->lfFaceName = '\0' ;

}/* FontInitLogFont() */

/*************************************************************************
 *  WORD FAR PASCAL
 *     FontSelectDialog( HWND hwndParent, LPSTR lpszTitle,
 *                       HDC hDC, WORD wFontType, WORD wFlags,
 *                       LPEXTLOGFONT lpLF, LPEXTLOGFONT lplfDefault ) 
 *
 *  Description:
 *
 *    This function brings up a standard Select Font dialog box.  You
 *    can specify the fonts you want listed in the WORD wFontType parameter.
 *
 *  Type/Parameter
 *          Description
 *    HWND  hwndParent
 *          Handle to the parent of the dialog box.
 *
 *    LPSTR lpszTitle
 *          Specifies the title of the dialog box (window title).
 *
 *    HDC   hDC
 *          Handle to the device context of the device that is to have
 *          the fonts enumerated.  This argument can be NULL if the
 *          display is to used.
 *
 *    WORD  wFontType
 *          Determines what type of fonts are listed in the dialog box.
 *          Can be any combination of the following:
 *
 *                RASTER_FONTTYPE    
 *                DEVICE_FONTTYPE    
 *                ANSI_FONTTYPE    
 *                SYMBOL_FONTTYPE  
 *                SHIFTJIS_FONTTYPE
 *                OEM_FONTTYPE          
 *                FIXED_FONTTYPE   
 *                VARIABLE_FONTTYPE
 *
 *    WORD  wFlags
 *          Flags that determine how the dialog box will look and act.
 *          Currently not implemented.
 *
 *    LPLOGFONT lpLF
 *          FAR pointer to a LOGFONT structer that will contain the selected
 *          font when the user hits OK.
 *
 *    LPLOGFONT lpLF
 *          FAR pointer to a logfont strcuture that defines the default font.
 * 
 *  Return Value
 *    DLG_OK if the user hit OK.
 *    DLG_CANCEL if the user hit cancel.
 *    DLG_ERROR if there was an error creating or displaying the dialog box.
 *
 *  Comments:
 *
 *************************************************************************/
WORD FAR PASCAL
FontSelectDialog( HWND hwndParent, LPSTR lpszTitle,
                  HDC  hDC,
                  WORD wFontType,
                  WORD wFlags, LPEXTLOGFONT lpLF, LPEXTLOGFONT lplfDefault ) 
{
   FARPROC           lpfnDlgProc ;
   PSELECTSTRUCT     pSS ;
   LOCALHANDLE       hlSS ;
   int               n = DLG_ERROR ;

   //
   // Setup data structure to pass to dialog box
   //
   if (!(hlSS = LocalAlloc( LHND, sizeof( SELECTSTRUCT ) )))
   {
      DP1( hWDB, "FontSelectDialog: LocalAlloc failed" ) ;
      return DLG_ERROR ;
   }

   pSS = (PSELECTSTRUCT)LocalLock( hlSS ) ;

   if (lpszTitle)
      lstrcpy( pSS->szTitle, lpszTitle ) ;

   pSS->wFontType = wFontType ;

   pSS->wFlags = wFlags ;

   DP( hWDB, "ENTER: height = %d, width = %d", lpLF->lfHeight, lpLF->lfWidth ) ;
   pSS->lf = *lpLF ;

   pSS->lfDefault = *lplfDefault ;

   pSS->hDC = hDC ;

   pSS->hPrnDC = NULL ;

   LocalUnlock( hlSS ) ;

   // Now bring up dlg box

   if (lpfnDlgProc = MakeProcInstance( fnFontSelect, hInst ))
   {
      n=  DialogBoxParam( hInst,
                          "FONTSELECT",
                          hwndParent,
                          lpfnDlgProc,
                         (LONG)hlSS ) ;
      FreeProcInstance( lpfnDlgProc ) ;

      if (n == DLG_OK)
      {
         pSS = (PSELECTSTRUCT)LocalLock( hlSS ) ;

         *lpLF = pSS->lf ;

         LocalUnlock( hlSS ) ;
      }
   }
   else
   {
      DP1( hWDB, "MakeProcInstance failed (FontSelectDialog)" ) ;
      return FALSE ;
   }

   DP( hWDB, "EXIT: height = %d, width = %d", lpLF->lfHeight, lpLF->lfWidth ) ;

   LocalFree( hlSS ) ;
   return n ;

}/* FontSelectDialog() */

/*************************************************************************
 *  BOOL FAR PASCAL
 *    FontFillFaceLB( HWND hLB, HDC hDC, WORD wFontType, WORD wFlags )
 *
 *  Description:
 *    Fills a listbox with the fonts availble for the device specified
 *    by the hDC parameter.  
 *
 *  Type/Parameter
 *          Description
 *
 *    HWND  hLB
 *          Handle to the listbox to be filled.  Must be a valid handle.
 * 
 *    HDC   hDC
 *          Handle to the device context of the device that is to have
 *          the fonts enumerated.
 *
 *    WORD  wFontType
 *          Specifies what fonts are to be included in the list box.
 *          Can be any combination of the following:
 *
 *                RASTER_FONTTYPE    
 *                DEVICE_FONTTYPE    
 *                ANSI_FONTTYPE    
 *                SYMBOL_FONTTYPE  
 *                SHIFTJIS_FONTTYPE
 *                OEM_FONTTYPE          
 *                FIXED_FONTTYPE   
 *                VARIABLE_FONTTYPE
 *
 *    WORD  wFlags
 *          Flags that determine how the dialog box will look and act.
 *          Currently not implemented.
 *
 *  Return Value
 *    TRUE if the list box was filled correctly.
 *    FALSE otherwise.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL
   FontFillFaceLB( HWND hLB, HDC hDC, WORD wFontType, WORD wFlags )
{
   HDC      hCurDC ;
   ENUMFACE ef ;
   LPSTR    lpFaceNames ;
   short    i ;
   BOOL     fRet = TRUE ;

   FARPROC  lpfnEnumFaces ;

   if ((DWORD)LB_ERR == SendMessage( hLB, LB_RESETCONTENT, 0, 0L ))
      return FALSE ;

   if (!(ef.ghFaceNames = GlobalAlloc( GHND, 1L )))
   {
      DP1( hWDB, "GlobalAlloc Failed (FontFillFaceLB)" ) ;
      return FALSE ;
   }

   ef.nNumFonts = 0 ;
   ef.wFontType = wFontType ;

   if (!(lpfnEnumFaces = MakeProcInstance( fnEnumFaces, hInst )))
   {
      DP1( hWDB, "MakeProcInstance failed (FontFillFaceLB)" ) ;
      GlobalFree( ef.ghFaceNames ) ;
      return FALSE ;
   }

   //
   // if the DC wasn't specified then use the display...
   //
   if (!hDC)
   {
      if (!(hCurDC = GetDC( NULL )))
      {
         DP1( hWDB,"Could not CreateDC (FontFillFaceLB)") ;
         FreeProcInstance( lpfnEnumFaces ) ;
         return FALSE ;
      }
   }
   else
      hCurDC = hDC ;

   if (hCurDC)
   {
      if (!EnumFonts( hCurDC, NULL, lpfnEnumFaces, (LPSTR)&ef ))
      {
         DP1( hWDB,"EnumFonts failed (FontFillFaceLB)") ;
         fRet = FALSE ;
         goto END ;
      }

      lpFaceNames = GlobalLock( ef.ghFaceNames ) ;

      for (i = 0 ; i < ef.nNumFonts ; i++)
      {
         if (LB_ERR >= LBAddString( hLB, lpFaceNames + (i * LF_FACESIZE) ))
         {
            DP1( hWDB,"LBAddString failed (FontFillFaceLB)" ) ;
            GlobalUnlock( ef.ghFaceNames ) ;
            fRet = FALSE ;
            goto END ;
         }
      }
      GlobalUnlock( ef.ghFaceNames ) ;
   }


END:
   FreeProcInstance( lpfnEnumFaces ) ;
   if (hCurDC != hDC)
      ReleaseDC( NULL, hCurDC ) ;

   GlobalFree( ef.ghFaceNames ) ;

   return fRet ;

}/* FontFillFaceLB() */

/*************************************************************************
 *  BOOL FAR PASCAL
 *    FontFillSizeLB( HWND hLB, HDC hDC, LPSTR lpszFaceName, WORD wFlags )
 *
 *  Description:
 *    Fills a listbox with the fonts availble for the device specified
 *    by the hDC parameter.  
 *
 *  Type/Parameter
 *          Description
 *
 *    HWND  hLB
 *          Handle to the listbox to be filled.  Must be a valid handle.
 * 
 *    HDC   hDC
 *          Handle to the device context of the device that is to have
 *          the fonts enumerated.
 *
 *    LPSTR lpszFaceName
 *          Name of the font that is to be enumerated.
 *
 *    WORD  wFlags
 *          Flags that determine how the dialog box will look and act.
 *          Currently not implemented.
 *
 *  Return Value
 *    TRUE if the list box was filled correctly.
 *    FALSE otherwise.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL
   FontFillSizeLB( HWND hLB, HDC hDC, LPSTR lpszFaceName, WORD wFlags )
{
   HDC      hCurDC ;
   ENUMSIZE es ;
   char     szSize[16] ;
   short    i ;
   BOOL     fRet = TRUE ;

   FARPROC  lpfnEnumSizes ;

   if (LB_ERR == (int)SendMessage( hLB, LB_RESETCONTENT, 0, 0L ))
      return FALSE ;

   es.nNumSizes = 0 ;

   if (!(lpfnEnumSizes = MakeProcInstance( fnEnumSizes, hInst )))
   {
      DP1( hWDB, "MakeProcInstance failed (FontFillSizeLB)" ) ;
      return FALSE ;
   }

   //
   // if the DC wasn't specified then use the display...
   //
   if (!hDC)
   {
      if (!(hCurDC = GetDC( NULL )))
      {
         DP1( hWDB,"Could not CreateDC (FontFillSizeLB)") ;
         FreeProcInstance( lpfnEnumSizes ) ;
         return FALSE ;
      }
   }
   else
      hCurDC = hDC ;

   if (hCurDC && lpszFaceName)
   {
      if (!EnumFonts( hCurDC, lpszFaceName, lpfnEnumSizes, (LPSTR)&es ))
      {
         DP1( hWDB,"EnumFonts failed (FontFillSizeLB)") ;
         fRet = FALSE ;
         goto END ;
      }

      for (i = 0 ; i < es.nNumSizes ; i++)
      {
         if (es.wFontType & RASTER_FONTTYPE &&
             !(es.tm.tmPitchAndFamily & 0x0001))
            wsprintf( szSize, "%dx%d", es.rgnSize[i], es.rgnWidth[i] ) ;
         else
            wsprintf( szSize, "%d", MulDiv( es.rgnSize[i],
                                    72,
                                    GetDeviceCaps( hCurDC, LOGPIXELSY ) ) ) ;

         if (LB_ERR == LBFindString( hLB, -1, szSize ))
            if (LB_ERR >= LBAddString( hLB, szSize ))
            {
               DP1( hWDB,"LBAddString failed (FontFillSizeLB)") ;
               fRet = FALSE ;
               goto END ;
            }
      }
   }


END:
   FreeProcInstance( lpfnEnumSizes ) ;
   if (hCurDC != hDC)
      ReleaseDC( NULL, hCurDC ) ;

   return fRet ;

}/* FontFillSizeLB() */


/************************************************************************
 * Internal functions
 ************************************************************************/
/*************************************************************************
 *  int FAR PASCAL
 *    fnFontSelect( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL
   fnFontSelect( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
{
   char           szFace[LF_FACESIZE] ;
   LOCALHANDLE    lhSS ;
   PSELECTSTRUCT  pSS ;
   WORD           w ;


   switch (wMsg)
   {
      case WM_INITDIALOG:
         DP5( hWDB, "fnFontSelect: WM_INITDIALOG") ;

         lhSS = (LOCALHANDLE)LOWORD( lParam ) ;

         if (!SetProp( hDlg, HLSS_PROP, lhSS ))
         {
            DP1( hWDB," Could not setprop") ;
            EndDialog( hDlg, DLG_ERROR ) ;
            return TRUE ;
         }

         pSS = (PSELECTSTRUCT)LocalLock( lhSS ) ;

         if (pSS->szTitle)
            SetWindowText( hDlg, pSS->szTitle ) ;

         if (pSS->wFlags & FONTSEL_USEPRINTER)
         {
            if (pSS->hPrnDC = GetDefaultPrnDC( ))
            {
               if (!FontFillFaceLB( GetDlgItem( hDlg, IDD_FONT_LB ),
                                    pSS->hPrnDC, pSS->wFontType, 0 ))
               {
                  DP1( hWDB, " FontFillFaceLB returned FALSE (INIT)" ) ;
                  LocalUnlock( lhSS ) ;
                  EndDialog( hDlg, DLG_ERROR ) ;
                  return FALSE ;
               }
            }
            else
            {
               DP1( hWDB,"GetDefaultPrnDC failed (fnFontSelect)(INIT)") ;
            }
         }
         else
         {
            pSS->hPrnDC = NULL ;
            if (!FontFillFaceLB( GetDlgItem( hDlg, IDD_FONT_LB ),
                                 pSS->hDC, pSS->wFontType, 0 ))
            {
               DP1( hWDB, " FontFillFaceLB returned FALSE (INIT)" ) ;
               LocalUnlock( lhSS ) ;
               EndDialog( hDlg, DLG_ERROR ) ;
               return FALSE ;
            }
         }

         if (pSS->hPrnDC)
            CheckDlgButton( hDlg, IDD_PRINTERFONTS, TRUE ) ;

         if (pSS->wFlags & FONTSEL_NOPRINTER)
         {
            EnableWindow( GetDlgItem( hDlg, IDD_PRINTERFONTS ), FALSE ) ;
            ShowWindow( GetDlgItem( hDlg, IDD_PRINTERFONTS ), SW_HIDE ) ;
         }

         if (pSS->wFlags & FONTSEL_NOFGCOLOR)
         {
            EnableWindow( GetDlgItem( hDlg, IDD_COLORFG_CB ), FALSE ) ;
            ShowWindow( GetDlgItem( hDlg, IDD_COLORFG_CB ), SW_HIDE ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_COLORFG_TXT ), FALSE ) ;
            ShowWindow( GetDlgItem( hDlg, IDD_COLORFG_TXT ), SW_HIDE ) ;
         }

         if (pSS->wFlags & FONTSEL_NOBGCOLOR)
         {
            EnableWindow( GetDlgItem( hDlg, IDD_COLORBG_CB ), FALSE ) ;
            ShowWindow( GetDlgItem( hDlg, IDD_COLORBG_CB ), SW_HIDE ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_COLORBG_TXT ), FALSE ) ;
            ShowWindow( GetDlgItem( hDlg, IDD_COLORBG_TXT ), SW_HIDE ) ;
         }

         if (pSS->wFlags & FONTSEL_NOFGCOLOR &&
             pSS->wFlags & FONTSEL_NOBGCOLOR )
         {
            EnableWindow( GetDlgItem( hDlg, IDD_COLOR_GRP ), FALSE ) ;
            ShowWindow( GetDlgItem( hDlg, IDD_COLOR_GRP ), SW_HIDE ) ;
         }

         if (pSS->wFlags & FONTSEL_NODEFAULT)
         {
            EnableWindow( GetDlgItem( hDlg, IDD_NORMALFONT ), FALSE ) ;
            ShowWindow( GetDlgItem( hDlg, IDD_NORMALFONT ), SW_HIDE ) ;
         }

         if (pSS->wFlags & FONTSEL_NOBOLD)
         {
            RECT rc ;
            RECT rc2 ;

            GetWindowRect( GetDlgItem( hDlg, IDD_BOLD ), &rc ) ;
            ScreenToClient( hDlg, (LPPOINT)&rc.left ) ;

            EnableWindow( GetDlgItem( hDlg, IDD_BOLD ), FALSE ) ;
            ShowWindow( GetDlgItem( hDlg, IDD_BOLD ), SW_HIDE ) ;

            GetWindowRect( GetDlgItem( hDlg, IDD_ITALIC ), &rc2 ) ;
            ScreenToClient( hDlg, (LPPOINT)&rc2.left ) ;

            SetWindowPos( GetDlgItem( hDlg, IDD_ITALIC ), NULL,
                          rc.left, rc.top, 0, 0, SWP_NOSIZE ) ;
                        
            GetWindowRect( GetDlgItem( hDlg, IDD_UNDERLINE ), &rc ) ;
            ScreenToClient( hDlg, (LPPOINT)&rc.left ) ;

            SetWindowPos( GetDlgItem( hDlg, IDD_UNDERLINE ), NULL,
                          rc2.left, rc2.top, 0, 0, SWP_NOSIZE ) ;
                        
            SetWindowPos( GetDlgItem( hDlg, IDD_STRIKEOUT ), NULL,
                          rc.left, rc.top, 0, 0, SWP_NOSIZE ) ;
         }

         if (pSS->wFlags & FONTSEL_NOITALIC)
         {
            RECT rc ;
            RECT rc2 ;

            GetWindowRect( GetDlgItem( hDlg, IDD_ITALIC ), &rc ) ;
            ScreenToClient( hDlg, (LPPOINT)&rc.left ) ;

            EnableWindow( GetDlgItem( hDlg, IDD_ITALIC ), FALSE ) ;
            ShowWindow( GetDlgItem( hDlg, IDD_ITALIC ), SW_HIDE ) ;

            GetWindowRect( GetDlgItem( hDlg, IDD_UNDERLINE ), &rc2 ) ;
            ScreenToClient( hDlg, (LPPOINT)&rc2.left ) ;
            SetWindowPos( GetDlgItem( hDlg, IDD_UNDERLINE ), NULL,
                          rc.left, rc.top, 0, 0, SWP_NOSIZE ) ;
                        
            SetWindowPos( GetDlgItem( hDlg, IDD_STRIKEOUT ), NULL,
                          rc2.left, rc2.top, 0, 0, SWP_NOSIZE ) ;
         }

         if (pSS->wFlags & FONTSEL_NOUNDERLINE)
         {
            RECT rc ;

            GetWindowRect( GetDlgItem( hDlg, IDD_UNDERLINE ), &rc ) ;
            ScreenToClient( hDlg, (LPPOINT)&rc.left ) ;

            EnableWindow( GetDlgItem( hDlg, IDD_UNDERLINE ), FALSE ) ;
            ShowWindow( GetDlgItem( hDlg, IDD_UNDERLINE ), SW_HIDE ) ;

            SetWindowPos( GetDlgItem( hDlg, IDD_STRIKEOUT ), NULL,
                          rc.left, rc.top, 0, 0, SWP_NOSIZE ) ;
         }

         if (pSS->wFlags & FONTSEL_NOSTRIKEOUT)
         {
            EnableWindow( GetDlgItem( hDlg, IDD_STRIKEOUT ), FALSE ) ;
            ShowWindow( GetDlgItem( hDlg, IDD_STRIKEOUT ), SW_HIDE ) ;
         }

         if (pSS->wFlags & FONTSEL_NOBOLD &&
             pSS->wFlags & FONTSEL_NOITALIC &&
             pSS->wFlags & FONTSEL_NOUNDERLINE &&
             pSS->wFlags & FONTSEL_NOSTRIKEOUT )
            ShowWindow( GetDlgItem( hDlg, IDD_STYLE_GRP ), SW_HIDE ) ;

         LocalUnlock( lhSS ) ;

         //
         // Initilaize the controls
         //
         DlgSetItemLength( hDlg, IDD_FONT_EC, LF_FACESIZE ) ;
         DlgSetItemLength( hDlg, IDD_SIZE_EC, 8 ) ;

         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;

         SetFocus( GetDlgItem( hDlg, IDD_FONT_LB )) ;

         return SetItems( hDlg, lhSS ) ;
      break ;

      case WM_COMMAND:
         switch( wParam )
         {
            case IDD_FONT_LB:
               lhSS = GetProp( hDlg, HLSS_PROP ) ;

               switch(HIWORD(lParam))
               {
                  case LBN_SELCHANGE:

                     w = LBGetCurSel( LOWORD( lParam ) ) ;

                     if (w != -1)
                     {
                        LPSTR lpsCur ;

                        pSS = (PSELECTSTRUCT)LocalLock( lhSS ) ;

                        if (LB_ERR == LBGetText( LOWORD( lParam ), w,
                                                 szFace, LF_FACESIZE ))
                        {
                           LocalUnlock( lhSS ) ;
                           break ;
                        }

                        SetDlgItemText( hDlg, IDD_FONT_EC, szFace ) ;

                        //
                        // Fill the size list box
                        //
                        if (pSS->hPrnDC)
                        {
                           if (!FontFillSizeLB( GetDlgItem( hDlg, IDD_SIZE_LB ),
                                                pSS->hPrnDC, szFace,
                                                pSS->wFontType ))
                           {
                              DP1( hWDB, " FontFillSizeLB(hPrnDC) returned FALSE" ) ;
                           }
                        }
                        else
                           if (!FontFillSizeLB( GetDlgItem( hDlg, IDD_SIZE_LB ),
                                                pSS->hDC, szFace,
                                                pSS->wFontType ))
                           {
                              DP1( hWDB, " FontFillSizeLB returned FALSE" ) ;
                           }

                        GetDlgItemText( hDlg, IDD_SIZE_EC, szFace, 5 ) ;

                        if (lpsCur = _fstrchr( szFace, 'x' ))
                           *lpsCur = '\0' ;

                        LBSelectString( GetDlgItem( hDlg, IDD_SIZE_LB ), -1,
                                        szFace ) ;

                        LocalUnlock( lhSS ) ;
                     }
                     DrawSample( hDlg, lhSS ) ;
                  break ;

                  case LBN_DBLCLK:
                     goto OKAY ;
               }
            break ;

            case IDD_SIZE_LB:
               lhSS = GetProp( hDlg, HLSS_PROP ) ;

               switch(HIWORD(lParam))
               {
                  case LBN_DBLCLK :
                  case LBN_SELCHANGE:
                     if (LB_ERR != LBGetText( LOWORD(lParam), 0xFFFF,
                                              szFace, LF_FACESIZE ))
                     {
                        SetDlgItemText( hDlg, IDD_SIZE_EC, szFace ) ;
                        DrawSample( hDlg, lhSS ) ;
                     }
                  break ;
               }
            break ;

            case IDD_FONT_EC:
               switch(HIWORD( lParam ) )
               {
                  case EN_KILLFOCUS:
                     lhSS = GetProp( hDlg, HLSS_PROP ) ;
                     DrawSample( hDlg, lhSS ) ;
                  break ;

                  case EN_CHANGE:
                  {
                     char szText[LF_FACESIZE] ;

                     GetWindowText( LOWORD( lParam ), szText, LF_FACESIZE ) ;
               
                     LBSelectString( GetDlgItem( hDlg, IDD_FONT_LB ), -1,
                                     szText ) ;
                  }
               }
            break ;

            case IDD_SIZE_EC:
               switch(HIWORD( lParam ) )
               {
                  case EN_KILLFOCUS:
                     lhSS = GetProp( hDlg, HLSS_PROP ) ;
                     DrawSample( hDlg, lhSS ) ;
                  break ;

                  case EN_CHANGE:
                  {
                     char szText[LF_FACESIZE] ;

                     GetWindowText( LOWORD( lParam ), szText, LF_FACESIZE ) ;
               
                     LBSelectString( GetDlgItem( hDlg, IDD_SIZE_LB ), -1,
                                     szText ) ;
                  }
                  break ;
               }
            break ;

            case IDD_BOLD:
            case IDD_ITALIC:
            case IDD_UNDERLINE:
            case IDD_STRIKEOUT:
               lhSS = GetProp( hDlg, HLSS_PROP ) ;
               DrawSample( hDlg, lhSS ) ;
            break ;

            case IDD_NORMALFONT:
               lhSS = GetProp( hDlg, HLSS_PROP ) ;
               pSS = (PSELECTSTRUCT)LocalLock( lhSS ) ;

               pSS->lf = pSS->lfDefault ;
               LocalUnlock( lhSS ) ;

               return SetItems( hDlg, lhSS ) ;
            break ;
               
            case IDD_PRINTERFONTS:
               lhSS = GetProp( hDlg, HLSS_PROP ) ;
               pSS = (PSELECTSTRUCT)LocalLock( lhSS ) ;

               if (pSS->hPrnDC)
               {
                  if (!DeleteDC( pSS->hPrnDC ))
                  {
                     DP1( hWDB,"Error deleting hPrnDC" ) ;
                  }
               }


               if (IsDlgButtonChecked( hDlg, IDD_PRINTERFONTS ))
               {
                  if (pSS->hPrnDC = GetDefaultPrnDC())
                  {
                     if (!FontFillFaceLB( GetDlgItem( hDlg, IDD_FONT_LB ),
                                          pSS->hPrnDC, pSS->wFontType, 0 ))
                     {
                        DP1( hWDB, " FontFillFaceLB returned FALSE" ) ;
                        LocalUnlock( lhSS ) ;
                        EndDialog( hDlg, DLG_ERROR ) ;
                        return FALSE ;
                     }
                  }
                  else
                  {
                     DP1( hWDB,"GetDefaultPrnDC failed! (PRINTERFONTS)") ;
                  }
               }
               else
               {
                  pSS->hPrnDC = NULL ;

                  if (!FontFillFaceLB( GetDlgItem( hDlg, IDD_FONT_LB ),
                                       pSS->hDC, pSS->wFontType, 0 ))
                  {
                     DP1( hWDB, " FontFillFaceLB returned FALSE" ) ;
                     LocalUnlock( lhSS ) ;
                     EndDialog( hDlg, DLG_ERROR ) ;
                     return FALSE ;
                  }
               }
               LocalUnlock( lhSS ) ;

               return SetItems( hDlg, lhSS ) ;
            break ;

            case IDD_COLORFG_CB:
            case IDD_COLORBG_CB:
               if (HIWORD( lParam ) == CBN_SELCHANGE )
               {
                  lhSS = GetProp( hDlg, HLSS_PROP ) ;
                  DrawSample( hDlg, lhSS ) ;
               }
            break ;


            case IDOK:
OKAY:
            {
               HDC      hdcCur ;
               LOGFONT  lf ;
               HFONT    hFont ;
               BOOL     fMyDC = FALSE ;

               lhSS = GetProp( hDlg, HLSS_PROP ) ;
               pSS = (PSELECTSTRUCT)LocalLock( lhSS ) ;

               if (!(hdcCur = pSS->hPrnDC))
                  hdcCur = pSS->hDC ;

               GetDlgItemText( hDlg, IDD_SIZE_EC, szFace, LF_FACESIZE ) ;

               GetDlgItemText( hDlg, IDD_FONT_EC,
                               lf.lfFaceName, LF_FACESIZE ) ;

               if (!GetFaceSize( szFace, &lf.lfHeight, &lf.lfWidth))
               {
                  MessageBeep( 0 ) ;
                  LocalUnlock( lhSS ) ;
                  break ;
               }

               if (!hdcCur)
               {
                  fMyDC = TRUE ;
                  hdcCur = GetDC( hDlg ) ;
               }

               if (!lf.lfWidth)
               {
                  hFont = ReallyCreateFont( hdcCur, lf.lfFaceName,
                           lf.lfHeight,
                           (IsDlgButtonChecked( hDlg, IDD_BOLD ) ?
                              RCF_BOLD : RCF_NORMAL) |
                           (IsDlgButtonChecked( hDlg, IDD_ITALIC ) ?
                              RCF_ITALIC : RCF_NORMAL) |
                           (IsDlgButtonChecked( hDlg, IDD_UNDERLINE ) ?
                              RCF_UNDERLINE : RCF_NORMAL ) |
                           (IsDlgButtonChecked( hDlg, IDD_STRIKEOUT ) ?
                              RCF_STRIKEOUT : RCF_NORMAL ) ) ;

               }
               else
               {
                  lf.lfWeight = (short)(IsDlgButtonChecked( hDlg, IDD_BOLD ) ? 700 : 0) ;

                  lf.lfItalic =
                     (BYTE)(IsDlgButtonChecked( hDlg, IDD_ITALIC ) ? TRUE : FALSE) ;

                  lf.lfUnderline =
                     (BYTE)(IsDlgButtonChecked( hDlg, IDD_UNDERLINE ) ? TRUE : FALSE) ;

                  lf.lfStrikeOut =
                     (BYTE)(IsDlgButtonChecked( hDlg, IDD_STRIKEOUT ) ? TRUE : FALSE) ;

                  FontSelectIndirect( hdcCur, &lf ) ;

                  hFont = CreateFontIndirect( &lf ) ;
               }

               ReleaseDC( hDlg, hdcCur ) ;

               GetObject( hFont, sizeof( LOGFONT ), &pSS->lf ) ;
               DeleteObject( hFont ) ;

               if (!(pSS->wFlags & FONTSEL_NOFGCOLOR))
                  pSS->lf.lfFGcolor = ColorCBGet( GetDlgItem( hDlg, IDD_COLORFG_CB ) ) ;

               if (!(pSS->wFlags & FONTSEL_NOBGCOLOR))
                  pSS->lf.lfBGcolor = ColorCBGet( GetDlgItem( hDlg, IDD_COLORBG_CB ) ) ;

               LocalUnlock( lhSS ) ;

               EndDialog( hDlg, DLG_OK ) ;
            }
            break ;

            case IDCANCEL:
               EndDialog( hDlg, DLG_CANCEL ) ;
            break ;

            default:
               return FALSE ;
         }
      break ;

      case WM_DRAWITEM:
      case WM_MEASUREITEM:
      case WM_DELETEITEM:
         return (int)ColorCBOwnerDraw( hDlg, wMsg, wParam, lParam ) ;
      break ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;
         TEXTMETRIC  tm ;
         HDC         hDC ;

         lhSS = GetProp( hDlg, HLSS_PROP ) ;
         hDC = BeginPaint(hDlg, &ps ) ;

         GetTextMetrics( hDC, &tm ) ;
         DrawDropShadow( hDC, GetDlgItem( hDlg, IDD_BOX_0 ),
                         tm.tmAveCharWidth / 2 ) ;

         EndPaint( hDlg, &ps ) ;

         DrawSample( hDlg, lhSS ) ;

      }
      return FALSE ;

      case WM_DESTROY:
         RemoveProp( hDlg, HLSS_PROP ) ;
      break ;

      default:
         return FALSE ;
   }


   return TRUE ;

}/* fnFontSelect() */

/*************************************************************************
 *  BOOL NEAR PASCAL GetFaceSize( LPSTR lpszFace, short *pnH, short *pnW )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL GetFaceSize( LPSTR lpszFace, short FAR *pnH, short FAR *pnW )
{
   LPSTR lpsCur ;

   *pnH = AtoI( lpszFace ) ;

   if (*pnH)
   {
      lpsCur = _fstrchr( lpszFace, 'x' ) ;
      if (lpsCur)
         *pnW = AtoI( lpsCur + 1) ;
      else
         *pnW = 0 ;

      return TRUE ;
   }
   else
      return FALSE ;

}/* GetFaceSize() */


/*************************************************************************
 *  BOOL NEAR PASCAL SetItems( HWND hDlg, LOCALHANDLE lhSS )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL SetItems( HWND hDlg, LOCALHANDLE lhSS )
{
   PSELECTSTRUCT  pSS ;
   char           szFace[LF_FACESIZE] ;
   HDC            hdcCur ;
   BOOL           fMyDC = FALSE ;

   pSS = (PSELECTSTRUCT)LocalLock( lhSS ) ;

   if (!(hdcCur = pSS->hPrnDC))
      hdcCur = pSS->hDC ;

   if (!hdcCur)
   {
      fMyDC = TRUE ;
      hdcCur = GetDC( hDlg ) ;
   }

   //
   // highlight the default font.  If there isn't a default
   // use the first in the list...
   //
   LBSelectString( GetDlgItem( hDlg, IDD_FONT_LB ),
                                       -1, pSS->lf.lfFaceName ) ;
   SetDlgItemText( hDlg, IDD_FONT_EC, pSS->lf.lfFaceName ) ;

   //
   // Fill the size list box
   //
   if (!FontFillSizeLB( GetDlgItem( hDlg, IDD_SIZE_LB ),
                        hdcCur, pSS->lf.lfFaceName, 0 ))
   {
       DP1( hWDB, " FontFillSizeLB returned FALSE (SetItems)" ) ;
   }

   if (pSS->lf.lfWidth && pSS->lf.lfPitchAndFamily & FIXED_PITCH)
      wsprintf( szFace, "%dx%d", pSS->lf.lfHeight, pSS->lf.lfWidth ) ;
   else
      wsprintf( szFace, "%d", MulDiv( abs(pSS->lf.lfHeight),
                                 72,
                                 GetDeviceCaps( hdcCur, LOGPIXELSY ) ) ) ;


   LBSelectString( GetDlgItem( hDlg, IDD_SIZE_LB), -1, szFace ) ;

   SetDlgItemText( hDlg, IDD_SIZE_EC, szFace ) ;

   if (pSS->lf.lfWeight >= 700)
      CheckDlgButton( hDlg, IDD_BOLD, TRUE ) ;
   else
      CheckDlgButton( hDlg, IDD_BOLD, FALSE ) ;

   CheckDlgButton( hDlg, IDD_ITALIC, pSS->lf.lfItalic ) ;

   CheckDlgButton( hDlg, IDD_UNDERLINE, pSS->lf.lfUnderline ) ;

   CheckDlgButton( hDlg, IDD_STRIKEOUT, pSS->lf.lfStrikeOut ) ;

   CheckDlgButton( hDlg, IDD_PRINTERFONTS, pSS->hPrnDC ) ;

   if (!(pSS->wFlags & FONTSEL_NOFGCOLOR))
   {
      ColorCBSetup( GetDlgItem( hDlg, IDD_COLORFG_CB ) ) ;
      ColorCBSetCurSel( GetDlgItem( hDlg, IDD_COLORFG_CB ), pSS->lf.lfFGcolor ) ;
   }


   if (!(pSS->wFlags & FONTSEL_NOBGCOLOR))
   {
      ColorCBSetup( GetDlgItem( hDlg, IDD_COLORBG_CB ) ) ;
      ColorCBSetCurSel( GetDlgItem( hDlg, IDD_COLORBG_CB ), pSS->lf.lfBGcolor ) ;
   }

   LocalUnlock( lhSS ) ;

   if (fMyDC)
      ReleaseDC( hDlg, hdcCur ) ;

   DrawSample( hDlg, lhSS ) ;

   return TRUE ;
}/* SetItems() */


BOOL NEAR PASCAL DrawSample( HWND hDlg, LOCALHANDLE lhSS )
{
   RECT        rc ;
   HFONT       hFont ;
   HFONT       hOldFont ;
   HDC         hDC ;
   char        szFace[LF_FACESIZE] ;
   PSELECTSTRUCT pSS ;
   DWORD       rgbFG, rgbBG ;
   LOGFONT     lf ;

   pSS = (PSELECTSTRUCT)LocalLock( lhSS ) ;

   GetDlgItemText( hDlg, IDD_SIZE_EC, szFace, LF_FACESIZE ) ;

   GetFaceSize( szFace, &lf.lfHeight, &lf.lfWidth );

   GetDlgItemText( hDlg, IDD_FONT_EC, lf.lfFaceName, LF_FACESIZE ) ;

   if (!(pSS->wFlags & FONTSEL_NOFGCOLOR))
      rgbFG = ColorCBGet( GetDlgItem( hDlg, IDD_COLORFG_CB ) ) ;
   else
      rgbFG = GetSysColor( COLOR_WINDOWTEXT ) ;

   if (!(pSS->wFlags & FONTSEL_NOBGCOLOR))
      rgbBG = ColorCBGet( GetDlgItem( hDlg, IDD_COLORBG_CB ) ) ;
   else
      rgbBG = GetSysColor( COLOR_WINDOW ) ;

   LocalUnlock( lhSS ) ;

   if (hDC = GetDC( GetDlgItem( hDlg, IDD_SAMPLE) ))
   {
      if (!lf.lfWidth)
      {
         hFont = ReallyCreateFont( hDC, lf.lfFaceName, lf.lfHeight,
                  (IsDlgButtonChecked( hDlg, IDD_BOLD ) ?
                     RCF_BOLD : RCF_NORMAL) |
                  (IsDlgButtonChecked( hDlg, IDD_ITALIC ) ?
                     RCF_ITALIC : RCF_NORMAL) |
                  (IsDlgButtonChecked( hDlg, IDD_UNDERLINE ) ?
                     RCF_UNDERLINE : RCF_NORMAL ) |
                  (IsDlgButtonChecked( hDlg, IDD_STRIKEOUT ) ?
                     RCF_STRIKEOUT : RCF_NORMAL ) ) ;

      }
      else
      {
         lf.lfWeight = (short)(IsDlgButtonChecked( hDlg, IDD_BOLD ) ? 700 : 0) ;

         lf.lfItalic =
            (BYTE)(IsDlgButtonChecked( hDlg, IDD_ITALIC ) ? TRUE : FALSE) ;

         lf.lfUnderline =
            (BYTE)(IsDlgButtonChecked( hDlg, IDD_UNDERLINE ) ? TRUE : FALSE) ;

         lf.lfStrikeOut =
            (BYTE)(IsDlgButtonChecked( hDlg, IDD_STRIKEOUT ) ? TRUE : FALSE) ;

         if (pSS->hPrnDC)
            FontSelectIndirect( pSS->hPrnDC, &lf ) ;
         else
            FontSelectIndirect( hDC, &lf ) ;

         hFont = CreateFontIndirect( &lf ) ;
      }

      if (hFont)
      {
         SetBkColor( hDC, rgbBG ) ;
         SetTextColor( hDC, rgbFG ) ;

         hOldFont = SelectObject( hDC, hFont ) ;

         GetClientRect( GetDlgItem( hDlg, IDD_SAMPLE ), &rc ) ;

         ExtTextOut( hDC, rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc,
                     NULL, 0, NULL ) ;

         DrawText( hDC, SAMPLESTRING, -1, &rc,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE ) ;

         if (hOldFont)
            SelectObject( hDC, hOldFont ) ;
         DeleteObject( hFont ) ;
      }

      ReleaseDC( GetDlgItem( hDlg, IDD_SAMPLE ), hDC ) ;
   }

   return TRUE ;
}

/*************************************************************************
 *  int FAR PASCAL
 *    fnEnumFaces( LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetrics,
 *                 short nFontType, LPENUMFACE lpEF )
 *
 *  Description: 
 *
 *    Call back function for EnumFonts.  Called once for each font.
 *
 *    This function fills the EF structure with the fonts that the device
 *    supports.  It uses the wFontType element of the EF strcture to
 *    determine which fonts to return.
 *
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL
   fnEnumFaces( LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetrics,
                short nFontType, LPENUMFACE lpEF )
{
   LPSTR lpFaces ;

//   DP1( hWDB, "fnEnumFaces: \"%s\"", (LPSTR)lpLogFont->lfFaceName ) ;

   if (!OkFont( lpLogFont, lpTextMetrics, nFontType, lpEF->wFontType ))
      return 1 ;

   if (NULL == GlobalReAlloc( lpEF->ghFaceNames,
                              (DWORD)LF_FACESIZE * (1 + lpEF->nNumFonts),
                              GMEM_MOVEABLE ))
   {
      DP1( hWDB, "Could not GlobalReAlloc lpEF!!!" ) ;
      return 0 ;
   }

   lpFaces = GlobalLock( lpEF->ghFaceNames ) ;
   lstrcpy( lpFaces + lpEF->nNumFonts * LF_FACESIZE, lpLogFont->lfFaceName ) ;
   
   GlobalUnlock( lpEF->ghFaceNames ) ;

   lpEF->nNumFonts++ ;

   return 1 ;

}/* fnEnumFaces() */

/*************************************************************************
 *  BOOL NEAR PASCAL OkFont( LPLOGFONT lplf, LPTEXTMETRIC lptm, WORD nFontType, WORD wFontType )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL
OkFont( LPLOGFONT lplf, LPTEXTMETRIC lptm, WORD nFontType, WORD wFontType )
{
   BOOL fOk = TRUE ;

   if (wFontType == DEFAULT_FONTTYPE)
      return TRUE ;

   if (nFontType & RASTER_FONTTYPE && !(wFontType & RASTER_FONTTYPE))
      return FALSE ;

   if (nFontType & DEVICE_FONTTYPE && !(wFontType & DEVICE_FONTTYPE))
      return FALSE ;

   if (lplf->lfPitchAndFamily & FIXED_PITCH && !(wFontType & FIXED_FONTTYPE))
      return FALSE ;

   if (lplf->lfPitchAndFamily & VARIABLE_PITCH && !(wFontType & VARIABLE_FONTTYPE ))
      return FALSE ;

   if (lplf->lfCharSet == ANSI_CHARSET && !(wFontType & ANSI_FONTTYPE))
      return FALSE ;

   if (lplf->lfCharSet == OEM_CHARSET && !(wFontType & OEM_FONTTYPE))
      return FALSE ;

   if (lplf->lfCharSet == SYMBOL_CHARSET && !(wFontType & SYMBOL_FONTTYPE))
      return FALSE ;

   if (lplf->lfCharSet == SHIFTJIS_CHARSET && !(wFontType & SHIFTJIS_FONTTYPE))
      return FALSE ;

   return TRUE ;

}/* OkFont() */
   
/*************************************************************************
 *  int FAR PASCAL
 *    fnEnumSizes( LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetrics,
 *                 short nFontType, LPENUMSIZE lpES )
 *
 *  Description: 
 *
 *  Call back function for EnumFonts.  Called once for each size.
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL
   fnEnumSizes( LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetrics,
                short nFontType, LPENUMSIZE lpES )
{
   WORD w ;

   /*   
   DP5( hWDB, "%s: (%d) [%d %d %d %d %d %d %d %d %d] \t[%d %d]",
         (LPSTR)lpLogFont->lfFaceName,
         nFontType,
         lpTextMetrics->tmHeight,
         lpTextMetrics->tmAscent,
         lpTextMetrics->tmDescent,
         lpTextMetrics->tmInternalLeading,
         lpTextMetrics->tmExternalLeading,
         lpTextMetrics->tmAveCharWidth,
         lpTextMetrics->tmMaxCharWidth,
         lpTextMetrics->tmDigitizedAspectX,
         lpTextMetrics->tmDigitizedAspectY,
         lpLogFont->lfHeight,
         lpLogFont->lfWidth ) ;
   */

   DP( hWDB, "%s: (%d) \t[%d %d]",
         (LPSTR)lpLogFont->lfFaceName,
         nFontType,
         lpLogFont->lfHeight,
         lpLogFont->lfWidth ) ;

   if (lpES->nNumSizes == FONT_MAX_SIZES)
      return 1 ;

   //
   // Only copy for first font
   //
   if (!lpES->nNumSizes)
   {
      lpES->lf = *lpLogFont ;
      lpES->tm = *lpTextMetrics ;
   }

   if (nFontType & RASTER_FONTTYPE)
   {
      lpES->rgnSize[lpES->nNumSizes] = lpLogFont->lfHeight ;
      lpES->rgnWidth[lpES->nNumSizes] = lpLogFont->lfWidth ;
      lpES->wFontType = nFontType ;

      lpES->nNumSizes++ ;

      return 1 ;
   }

   //
   // It's a vector or true type, so make up sizes...
   //
   lpES->rgnSize  [0] = 8 ; 
   lpES->rgnWidth [0] = 0 ;

   lpES->rgnWidth [0] = 0 ;

   lpES->rgnSize  [1] = 9 ; 
   lpES->rgnWidth [1] = 0 ;

   lpES->rgnSize  [2] = 10 ;
   lpES->rgnWidth [2] = 0 ;

   lpES->rgnSize  [3] = 11 ;
   lpES->rgnWidth [3] = 0 ;

   lpES->rgnSize  [4] = 12 ;
   lpES->rgnWidth [4] = 0 ;

   lpES->rgnSize  [5] = 14 ;
   lpES->rgnWidth [5] = 0 ;

   lpES->rgnSize  [6] = 16 ;
   lpES->rgnWidth [6] = 0 ;

   lpES->rgnSize  [7] = 18 ;
   lpES->rgnWidth [7] = 0 ;

   lpES->rgnSize  [8] = 20 ;
   lpES->rgnWidth [8] = 0 ;

   lpES->rgnSize  [9] = 22 ;
   lpES->rgnWidth [9] = 0 ;

   lpES->rgnSize  [10]= 24 ;
   lpES->rgnWidth [10]= 0 ;

   lpES->rgnSize  [11]= 26 ;
   lpES->rgnWidth [11]= 0 ;

   lpES->rgnSize  [12]= 28 ;
   lpES->rgnWidth [12]= 0 ;

   lpES->rgnSize  [13]= 36 ;
   lpES->rgnWidth [13]= 0 ;

   lpES->rgnSize  [14]= 48 ;
   lpES->rgnWidth [14]= 0 ;

   lpES->rgnSize  [15]= 72 ;
   lpES->rgnWidth [15]= 0 ;

   lpES->nNumSizes = 15 ;

   return 1 ;

}/* fnEnumSizes() */


/************************************************************************
 * End of File: font.c
 ************************************************************************/


