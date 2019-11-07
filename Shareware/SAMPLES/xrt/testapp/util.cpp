// util.cpp
//
// Implementation file for utility functions.
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
// August 7, 1993
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  August 7, 1993  cek     First implementation.
//
#include "precomp.h" 
#include "xrttest.h"
#include "util.h"

/////
// Utility functions
//

static TCHAR szBold[]       = TEXT("Bold") ;
static TCHAR szItalic[]      = TEXT("Italic") ;
static TCHAR szBoldItalic[]  = TEXT("Bold Italic") ;
static TCHAR szRegular[]     = TEXT("Regular") ;

extern "C"
int EXPORT CALLBACK
    fnEnumReallyEx( LPENUMLOGFONT lpLogFont,
                LPNEWTEXTMETRIC lpTextMetrics,
                int nFontType,
                LPENUMLOGFONT lpELF ) ;

 /****************************************************************
 *  HFONT WINAPI
 *    ReallyCreateFontEx( HDC hDC, LPSTR lpszFace,
 *                        LPSTR lpszStyle, int nPointSize, UINT uiFlags )
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
 *    int nPointSize  size in points (required)
 *
 *    UINT  uiFlags      Flags, same as for ReallyCreateFont() except
 *                      RCF_BOLD and RCF_ITALIC are ignored if lpszStyle
 *                      is not NULL.
 *
 *  Comments:
 *
 ****************************************************************/

HFONT WINAPI
   ReallyCreateFont( HDC hDC, LPTSTR lpszFace, LPTSTR lpszStyle, UINT nPointSize, UINT uiFlags )
{
   ENUMLOGFONT  elf ;
   FONTENUMPROC lpfn ;
   HDC          hdcCur ;
   
   lpfn = (FONTENUMPROC)fnEnumReallyEx ;

   // if the DC wasn't specified then use the display...
   //
   if (!hDC)
   {
      if (!(hdcCur = GetDC( NULL )))
         return FALSE ;
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
         lstrcpy( (LPTSTR)elf.elfStyle, lpszStyle ) ;
      else
        switch( uiFlags & ~(RCF_NODEFAULT | RCF_STRIKEOUT | RCF_UNDERLINE) )
        {
            case RCF_BOLD:
                lstrcpy( (LPTSTR)elf.elfStyle, szBold ) ;
            break ;

            case RCF_ITALIC:
                lstrcpy( (LPTSTR)elf.elfStyle, szItalic ) ;
            break ;

            case RCF_BOLD | RCF_ITALIC:
                lstrcpy( (LPTSTR)elf.elfStyle, szBold ) ;
                lstrcat( (LPTSTR)elf.elfStyle, TEXT(" ") ) ;
                lstrcat( (LPTSTR)elf.elfStyle, szItalic ) ;
            break ;

            default:
                lstrcpy( (LPTSTR)elf.elfStyle, szRegular ) ;
                lpszStyle = (LPTSTR)elf.elfStyle ;
            break ;
        }

      if (lpszFace)
#ifdef WIN32
         EnumFontFamilies( hdcCur, lpszFace, lpfn, (LONG)(LPVOID)&elf ) ;
#else
         EnumFontFamilies( hdcCur, lpszFace, lpfn, (LPTSTR)(LPVOID)&elf ) ;
#endif
      if (!lpszFace || lstrcmpi( elf.elfLogFont.lfFaceName, lpszFace))
      {
         if ((uiFlags & RCF_NODEFAULT) == RCF_NODEFAULT)
         {
            if (hdcCur != hDC)
               ReleaseDC( NULL, hdcCur ) ;

            return NULL ;
         }
         else
            GetObject( GetStockObject( ANSI_VAR_FONT ),
                       sizeof( LOGFONT ), (LPTSTR)&elf.elfLogFont ) ;
      }

      // See pages 4-48, 4-49, of Reference Vol. 1 for explaination
      // of why we set lfWidth to 0
      //
      elf.elfLogFont.lfWidth = 0 ;

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

      elf.elfLogFont.lfStrikeOut = (BYTE)((uiFlags & RCF_STRIKEOUT) == RCF_STRIKEOUT) ;
      elf.elfLogFont.lfUnderline = (BYTE)((uiFlags & RCF_UNDERLINE) == RCF_UNDERLINE) ;

      if (!lpszStyle)
      {
         elf.elfLogFont.lfWeight    = (uiFlags & RCF_BOLD) ? FW_BOLD : FW_NORMAL ;
         elf.elfLogFont.lfItalic    = (BYTE)((uiFlags & RCF_ITALIC) == RCF_ITALIC) ;
      }

   }

   if (hdcCur != hDC)
      ReleaseDC( NULL, hdcCur ) ;

   return CreateFontIndirect( &elf.elfLogFont ) ;

} /* ReallyCreateFontEx()  */

/****************************************************************
 *  int CALLBACK
 *    fnEnumReallyEx( LPLOGFONT lpLogFont,
 *                  LPTEXTMETRIC lpTextMetrics,
 *                  int nFontType, LPLOGFONT lpLF )
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
extern "C"
int EXPORT CALLBACK
fnEnumReallyEx( LPENUMLOGFONT lpELFin,
                LPNEWTEXTMETRIC lpTextMetrics,
                int nFontType,
                LPENUMLOGFONT lpELF )
{
   /* If we are not true type, then we will only be called once,
    * in in this case we need to compare the weight and italic
    * members of the TM struct to some strings.
    */
   if (!(nFontType & TRUETYPE_FONTTYPE))
   {
      lpELF->elfLogFont = lpELFin->elfLogFont ;

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

         if (!lstrcmpi( (LPTSTR)lpELF->elfStyle, szBold ))
         {
            lpELF->elfLogFont.lfWeight = FW_BOLD ;
            return 0 ;
         }

         if (!lstrcmpi( (LPTSTR)lpELF->elfStyle, szItalic))
         {
            lpELF->elfLogFont.lfItalic = TRUE ;
            return 0 ;
         }

         if (!lstrcmpi( (LPTSTR)lpELF->elfStyle, szBoldItalic ))
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
    *
    * If we get an exact match copy it into lpELF and return.
    * Otherwise get as close as possible.
    */
   if (0==lstrcmpi( (LPTSTR)lpELF->elfStyle, (LPTSTR)lpELFin->elfStyle ))
   {
      *lpELF = *lpELFin;
      return 0;
   }

   lpELF->elfLogFont = lpELFin->elfLogFont ;

   return 1 ;

}/* fnEnumReallyEx() */


/*************************************************************************
 *  void WINAPI
 *  DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient )
 *
 *  Description:
 *
 *    Centers a window within another window.
 *
 *  Type/Parameter
 *          Description
 *
 *    HWND  hwndCenter
 *          Window to center.  This does not have to be a child of
 *          hwndWithin.
 *    
 *    HWND  hwndWithin
 *          Window to center the above window within.  Can be NULL.
 *
 *    BOOL  fClient
 *          If TRUE the window is centered within the available client
 *          area.  Otherwise it's centered within the entire window area.
 * 
 *  Comments:
 *
 *************************************************************************/
void WINAPI
DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient )
{
   RECT rcWithin ;
   RECT rcCenter ;
   int   x, y ;
   int   dxCenter, dyCenter ;
   int   dxScreen, dyScreen ;

   dxScreen = GetSystemMetrics( SM_CXSCREEN ) ;
   dyScreen = GetSystemMetrics( SM_CYSCREEN ) ;

   if (!IsWindow(hwndCenter))
      return ;

   if (hwndWithin && !IsWindow(hwndWithin))
      return ;

   if (hwndWithin == NULL)
   {
      rcWithin.left = rcWithin.top = 0 ;
      rcWithin.right = dxScreen ;
      rcWithin.bottom = dyScreen ;
   }
   else
   {
      if (fClient)
      {
         /*
          * First get screen coords of rectangle we're going to be
          * centered within.
          */
         GetClientRect( hwndWithin, (LPRECT)&rcWithin ) ;
         ClientToScreen( hwndWithin, (LPPOINT)&rcWithin.left ) ;
         ClientToScreen( hwndWithin, (LPPOINT)&rcWithin.right ) ;
      }
      else
         GetWindowRect( hwndWithin, (LPRECT)&rcWithin ) ;
   }

   GetWindowRect( hwndCenter, (LPRECT)&rcCenter ) ;
   dxCenter = rcCenter.right - rcCenter.left ;
   dyCenter = rcCenter.bottom - rcCenter.top ;

   /*
    * Now we have both the within and center rects in screen coords.
    *
    * SetWindowPos behaves differently for Non child windows
    * than for child windows.  For popups it's coordinates
    * are relative to the upper left corner of the screen.  For
    * children it's coords are relative to the upper left corner
    * of the client area of the parent.
    */
   x = ((rcWithin.right - rcWithin.left) - dxCenter) / 2 ;
   y = ((rcWithin.bottom - rcWithin.top) - dyCenter) / 2 ;

   if (hwndWithin == GetParent( hwndCenter ) &&
       !(GetWindowLong( hwndCenter, GWL_STYLE ) & WS_CHILD ))
   {
      x += rcWithin.left ;
      y += rcWithin.top ;

      if (x + dxCenter > dxScreen )
         x = dxScreen - dxCenter ;

      if (y + dyCenter > dyScreen )
         y = dyScreen - dyCenter ;

      SetWindowPos( hwndCenter, NULL,
                    max(x,0),
                    max(y,0),
                    0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER ) ;

      return ;
   }

   SetWindowPos( hwndCenter, NULL,
                 max(x,0),
                 max(y,0),
                 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER ) ;

   return ;

}/* DlgCenter() */

// column text out

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
   TCHAR  rgchAlphabet[52] ;
   int  i ;
   SIZE size ;

   // Get the TM of the current font.  Note that GetTextMetrics
   // gets the incorrect AveCharWidth value for proportional fonts.
   // This is the whole poshort in this exercise.
   //
   if (lpTM)
    GetTextMetrics(hDC, lpTM);

   // If it's not a variable pitch font GetTextMetrics was correct
   // so just return.
   //
   if (lpTM && !(lpTM->tmPitchAndFamily & FIXED_PITCH))  
      return lpTM->tmAveCharWidth ;
   else
   {
      for ( i = 0 ; i <= 25 ; i++)
         rgchAlphabet[i] = (TCHAR)(i+(int)'a') ;

      for ( i = 0 ; i<=25 ; i++)
         rgchAlphabet[i+25] = (TCHAR)(i+(int)'A') ;

      GetTextExtentPoint( hDC, (LPTSTR)rgchAlphabet, 52, &size ) ;
      nAveWidth = size.cx / 26 ;
      nAveWidth = (nAveWidth + 1) / 2 ;
   }

   // Return the calculated average char width
   //
   return nAveWidth ;

} /* GetTextMetricsCorrectly()  */
 
/****************************************************************
 *
 *  Description: 
 *
 *    The ColumntTextOut function writes a character string at
 *    the specified location, using tabs to identify column breaks.  Each
 *    column is aligned according to the array of COLUMNSTRUCTs.
 *
 *    A COLUMNSTRUCT looks like this:
 *
 *    {
 *       int   nLeft ;       // starting x position of the column
 *       int   nRight ;      // ending x position of the column
 *       UINT  uiFlags ;      // format flags
 *    }
 *
 *    If a column has another column to the left of it, it's nLeft member
 *    is ignored.
 *
 *    uiFlags can be any of the following:
 *
 *          #define DT_LEFT      0x0000
 *          #define DT_CENTER    0x0001
 *          #define DT_RIGHT     0x0002
 *
 *  Comments:
 *
 ****************************************************************/
void WINAPI ColumnTextOut( HDC hdc, int nX, int nY, LPTSTR lpszIN,
                            int cColumns, LPCOLUMNSTRUCT rgColumns )
{

#define ETOFLAGS ETO_CLIPPED

    COLUMNSTRUCT   CS ;              // local copy for speed 
    RECT           rc ;              // current cell rect 
    int            cIteration = 0 ;  // what column 
    LPTSTR          lpNext ;          // next string (current is lpsz) 
    int            cch ;             // count of chars in current string 
    SIZE           size ;            // extent of current string 
    int            dx ;              // column width 
    
    rc.left = nX ;
    rc.top = nY ;
    rc.right = 0 ;
    
    if (rgColumns == NULL || cColumns <= 1)
    {                                                                      
        int Tab = 15 ;
        TabbedTextOut( hdc, nX, nY, lpszIN, lstrlen(lpszIN), 1, &Tab, nX ) ;
        return ;
    }

    // For each sub string (bracketed by a tab)
    //
    LPTSTR lpsz = lpszIN ;
    while (*lpsz)
    {
        if (cIteration >= cColumns)
            return ;
    
        for (cch = 0, lpNext = lpsz ;
             *lpNext != TEXT('\t') && *lpNext ;
             lpNext++, cch++)
            ;
    
        CS = rgColumns[cIteration] ;
    
        // If it's the leftmost column use
        //
        if (cIteration == 0)
        {
            rc.left = nX + CS.nLeft ;
            rc.right = nX + CS.nRight ;
        }
        else
            rc.right = nX + CS.nRight ;
        
        GetTextExtentPoint( hdc, lpsz, cch, &size ) ;
        rc.bottom = rc.top + size.cy ;
    
        // If the width of the column is 0 do nothing
        //
        if ((dx = (rc.right - rc.left)) > 0)
        {
            switch(CS.uiFlags)
            {
                case DT_CENTER:
                    ExtTextOut( hdc, rc.left + ((dx - size.cx) / (int)2),
                    rc.top, ETOFLAGS, &rc, lpsz, cch, NULL ) ;
                break ;
                
                case DT_RIGHT:
                    // If it's right justified then make the left border 0
                    //
                    //rc.left = nX + rgColumns[0].nLeft ;
                    ExtTextOut( hdc, rc.left + (dx - size.cx),
                    rc.top, ETOFLAGS, &rc, lpsz, cch, NULL ) ;
                break ;
                
                case DT_LEFT:
                default:
                    ExtTextOut( hdc, rc.left, rc.top, ETOFLAGS, &rc, lpsz, cch, NULL ) ;
                break ;
            }
        }
        rc.left = rc.right ;
        cIteration++ ;
        lpsz = lpNext + 1 ;
    }
    
} // ColumnTextOut()  


LPTSTR HRtoString( HRESULT hr )
{   
    static  TCHAR sz[256] ; 
    LPTSTR  szErrName ;
    SCODE   sc ;
#define CASE_SCODE(sc)  case sc: szErrName = (LPTSTR)TEXT(#sc) ; break ;
    
    sc = GetScode( hr ) ;
    
    switch (sc) {
        /* SCODE's defined in SCODE.H */
        CASE_SCODE(S_OK)
 // same value as S_OK      CASE_SCODE(S_TRUE)
        CASE_SCODE(S_FALSE)
        CASE_SCODE(E_UNEXPECTED)
        CASE_SCODE(E_NOTIMPL)
        CASE_SCODE(E_OUTOFMEMORY)
        CASE_SCODE(E_INVALIDARG)
        CASE_SCODE(E_NOINTERFACE)
        CASE_SCODE(E_POINTER)
        CASE_SCODE(E_HANDLE)
        CASE_SCODE(E_ABORT)
        CASE_SCODE(E_FAIL)
        CASE_SCODE(E_ACCESSDENIED)
            
        /* SCODE's defined in DVOBJ.H */
        CASE_SCODE(DATA_E_FORMATETC)
// same as DATA_E_FORMATETC     CASE_SCODE(DV_E_FORMATETC)
        CASE_SCODE(DATA_S_SAMEFORMATETC)
        CASE_SCODE(VIEW_E_DRAW)     
//  same as VIEW_E_DRAW         CASE_SCODE(E_DRAW)
        CASE_SCODE(VIEW_S_ALREADY_FROZEN)
        CASE_SCODE(CACHE_E_NOCACHE_UPDATED)
        CASE_SCODE(CACHE_S_FORMATETC_NOTSUPPORTED)
        CASE_SCODE(CACHE_S_SAMECACHE)
        CASE_SCODE(CACHE_S_SOMECACHES_NOTUPDATED)

        /* SCODE's defined in OLE2.H */
        CASE_SCODE(OLE_E_OLEVERB)
        CASE_SCODE(OLE_E_ADVF)
        CASE_SCODE(OLE_E_ENUM_NOMORE)
        CASE_SCODE(OLE_E_ADVISENOTSUPPORTED)
        CASE_SCODE(OLE_E_NOCONNECTION)
        CASE_SCODE(OLE_E_NOTRUNNING)
        CASE_SCODE(OLE_E_NOCACHE)
        CASE_SCODE(OLE_E_BLANK)
        CASE_SCODE(OLE_E_CLASSDIFF)
        CASE_SCODE(OLE_E_CANT_GETMONIKER)
        CASE_SCODE(OLE_E_CANT_BINDTOSOURCE)
        CASE_SCODE(OLE_E_STATIC)
        CASE_SCODE(OLE_E_PROMPTSAVECANCELLED)
        CASE_SCODE(OLE_E_INVALIDRECT)
        CASE_SCODE(OLE_E_WRONGCOMPOBJ)
        CASE_SCODE(OLE_E_INVALIDHWND)
        CASE_SCODE(DV_E_DVTARGETDEVICE)
        CASE_SCODE(DV_E_STGMEDIUM)
        CASE_SCODE(DV_E_STATDATA)
        CASE_SCODE(DV_E_LINDEX)
        CASE_SCODE(DV_E_TYMED)
        CASE_SCODE(DV_E_CLIPFORMAT)
        CASE_SCODE(DV_E_DVASPECT)
        CASE_SCODE(DV_E_DVTARGETDEVICE_SIZE)
        CASE_SCODE(DV_E_NOIVIEWOBJECT)
        CASE_SCODE(OLE_S_USEREG)
        CASE_SCODE(OLE_S_STATIC)
        CASE_SCODE(OLE_S_MAC_CLIPFORMAT)
        CASE_SCODE(CONVERT10_E_OLESTREAM_GET)
        CASE_SCODE(CONVERT10_E_OLESTREAM_PUT)
        CASE_SCODE(CONVERT10_E_OLESTREAM_FMT)
        CASE_SCODE(CONVERT10_E_OLESTREAM_BITMAP_TO_DIB)
        CASE_SCODE(CONVERT10_E_STG_FMT)
        CASE_SCODE(CONVERT10_E_STG_NO_STD_STREAM)
        CASE_SCODE(CONVERT10_E_STG_DIB_TO_BITMAP)
        CASE_SCODE(CONVERT10_S_NO_PRESENTATION)
        CASE_SCODE(CLIPBRD_E_CANT_OPEN)
        CASE_SCODE(CLIPBRD_E_CANT_EMPTY)
        CASE_SCODE(CLIPBRD_E_CANT_SET)
        CASE_SCODE(CLIPBRD_E_BAD_DATA)
        CASE_SCODE(CLIPBRD_E_CANT_CLOSE)
        CASE_SCODE(DRAGDROP_E_NOTREGISTERED)
        CASE_SCODE(DRAGDROP_E_ALREADYREGISTERED)
        CASE_SCODE(DRAGDROP_E_INVALIDHWND)
        CASE_SCODE(DRAGDROP_S_DROP)
        CASE_SCODE(DRAGDROP_S_CANCEL)
        CASE_SCODE(DRAGDROP_S_USEDEFAULTCURSORS)
        CASE_SCODE(OLEOBJ_E_NOVERBS)
        CASE_SCODE(OLEOBJ_S_INVALIDVERB)
        CASE_SCODE(OLEOBJ_S_CANNOT_DOVERB_NOW)
        CASE_SCODE(OLEOBJ_S_INVALIDHWND)
        CASE_SCODE(INPLACE_E_NOTUNDOABLE)
        CASE_SCODE(INPLACE_E_NOTOOLSPACE)
        CASE_SCODE(INPLACE_S_TRUNCATED)

        /* SCODE's defined in STORAGE.H */
        CASE_SCODE(STG_E_INVALIDFUNCTION)
        CASE_SCODE(STG_E_FILENOTFOUND)
        CASE_SCODE(STG_E_PATHNOTFOUND)
        CASE_SCODE(STG_E_TOOMANYOPENFILES)
        CASE_SCODE(STG_E_ACCESSDENIED)
        CASE_SCODE(STG_E_INVALIDHANDLE)
        CASE_SCODE(STG_E_INSUFFICIENTMEMORY)
        CASE_SCODE(STG_E_INVALIDPOINTER)
        CASE_SCODE(STG_E_NOMOREFILES)
        CASE_SCODE(STG_E_DISKISWRITEPROTECTED)
        CASE_SCODE(STG_E_SEEKERROR)
        CASE_SCODE(STG_E_WRITEFAULT)
        CASE_SCODE(STG_E_READFAULT)
        CASE_SCODE(STG_E_LOCKVIOLATION)
        CASE_SCODE(STG_E_FILEALREADYEXISTS)
        CASE_SCODE(STG_E_INVALIDPARAMETER)
        CASE_SCODE(STG_E_MEDIUMFULL)
        CASE_SCODE(STG_E_ABNORMALAPIEXIT)
        CASE_SCODE(STG_E_INVALIDHEADER)
        CASE_SCODE(STG_E_INVALIDNAME)
        CASE_SCODE(STG_E_UNKNOWN)
        CASE_SCODE(STG_E_UNIMPLEMENTEDFUNCTION)
        CASE_SCODE(STG_E_INVALIDFLAG)
        CASE_SCODE(STG_E_INUSE)
        CASE_SCODE(STG_E_NOTCURRENT)
        CASE_SCODE(STG_E_REVERTED)
        CASE_SCODE(STG_E_CANTSAVE)
        CASE_SCODE(STG_E_OLDFORMAT)
        CASE_SCODE(STG_E_OLDDLL)
        CASE_SCODE(STG_E_SHAREREQUIRED)
        CASE_SCODE(STG_S_CONVERTED)
//        CASE_SCODE(STG_S_BUFFEROVERFLOW)
//        CASE_SCODE(STG_S_TRYOVERWRITE)

        /* SCODE's defined in COMPOBJ.H */
        CASE_SCODE(CO_E_NOTINITIALIZED)
        CASE_SCODE(CO_E_ALREADYINITIALIZED)
        CASE_SCODE(CO_E_CANTDETERMINECLASS)
        CASE_SCODE(CO_E_CLASSSTRING)
        CASE_SCODE(CO_E_IIDSTRING)
        CASE_SCODE(CO_E_APPNOTFOUND)
        CASE_SCODE(CO_E_APPSINGLEUSE)
        CASE_SCODE(CO_E_ERRORINAPP)
        CASE_SCODE(CO_E_DLLNOTFOUND)
        CASE_SCODE(CO_E_ERRORINDLL)
        CASE_SCODE(CO_E_WRONGOSFORAPP)
        CASE_SCODE(CO_E_OBJNOTREG)
        CASE_SCODE(CO_E_OBJISREG)
        CASE_SCODE(CO_E_OBJNOTCONNECTED)
        CASE_SCODE(CO_E_APPDIDNTREG)
        CASE_SCODE(CLASS_E_NOAGGREGATION)
        CASE_SCODE(REGDB_E_READREGDB)
        CASE_SCODE(REGDB_E_WRITEREGDB)
        CASE_SCODE(REGDB_E_KEYMISSING)
        CASE_SCODE(REGDB_E_INVALIDVALUE)
        CASE_SCODE(REGDB_E_CLASSNOTREG)
        CASE_SCODE(REGDB_E_IIDNOTREG)
        CASE_SCODE(RPC_E_FIRST)
        CASE_SCODE(RPC_E_CALL_REJECTED)
        CASE_SCODE(RPC_E_CALL_CANCELED)
        CASE_SCODE(RPC_E_CANTPOST_INSENDCALL)
        CASE_SCODE(RPC_E_CANTCALLOUT_INASYNCCALL)
        CASE_SCODE(RPC_E_CANTCALLOUT_INEXTERNALCALL)
        CASE_SCODE(RPC_E_CONNECTION_TERMINATED)
        CASE_SCODE(RPC_E_SERVER_DIED)
        CASE_SCODE(RPC_E_CLIENT_DIED)
        CASE_SCODE(RPC_E_INVALID_DATAPACKET)
        CASE_SCODE(RPC_E_CANTTRANSMIT_CALL)
        CASE_SCODE(RPC_E_CLIENT_CANTMARSHAL_DATA)
        CASE_SCODE(RPC_E_CLIENT_CANTUNMARSHAL_DATA)
        CASE_SCODE(RPC_E_SERVER_CANTMARSHAL_DATA)
        CASE_SCODE(RPC_E_SERVER_CANTUNMARSHAL_DATA)
        CASE_SCODE(RPC_E_INVALID_DATA)
        CASE_SCODE(RPC_E_INVALID_PARAMETER)
        CASE_SCODE(RPC_E_UNEXPECTED)

        /* SCODE's defined in MONIKER.H */          
        CASE_SCODE(MK_E_CONNECTMANUALLY)
        CASE_SCODE(MK_E_EXCEEDEDDEADLINE)
        CASE_SCODE(MK_E_NEEDGENERIC)
        CASE_SCODE(MK_E_UNAVAILABLE)
        CASE_SCODE(MK_E_SYNTAX)
        CASE_SCODE(MK_E_NOOBJECT)
        CASE_SCODE(MK_E_INVALIDEXTENSION)
        CASE_SCODE(MK_E_INTERMEDIATEINTERFACENOTSUPPORTED)
        CASE_SCODE(MK_E_NOTBINDABLE)
        CASE_SCODE(MK_E_NOTBOUND)
        CASE_SCODE(MK_E_CANTOPENFILE)
        CASE_SCODE(MK_E_MUSTBOTHERUSER)
        CASE_SCODE(MK_E_NOINVERSE)
        CASE_SCODE(MK_E_NOSTORAGE)
        CASE_SCODE(MK_S_REDUCED_TO_SELF)
        CASE_SCODE(MK_S_ME)
        CASE_SCODE(MK_S_HIM)
        CASE_SCODE(MK_S_US)
        CASE_SCODE(MK_S_MONIKERALREADYREGISTERED)
        
/* SCODE's defined in DISPATCH.H */
//        CASE_SCODE(DISP_E_UNKNOWNINTERFACE)
//        CASE_SCODE(DISP_E_MEMBERNOTFOUND)
//        CASE_SCODE(DISP_E_PARAMNOTFOUND)
//        CASE_SCODE(DISP_E_TYPEMISMATCH)
//        CASE_SCODE(DISP_E_UNKNOWNNAME)
//        CASE_SCODE(DISP_E_NONAMEDARGS)
//        CASE_SCODE(DISP_E_BADVARTYPE)
//        CASE_SCODE(DISP_E_EXCEPTION)
//        CASE_SCODE(DISP_E_OVERFLOW)
//        CASE_SCODE(DISP_E_BADINDEX)
//        CASE_SCODE(DISP_E_UNKNOWNLCID)
//        CASE_SCODE(DISP_E_ARRAYISLOCKED)
//        CASE_SCODE(DISP_E_BADPARAMCOUNT)
//        CASE_SCODE(DISP_E_PARAMNOTOPTIONAL)
//        CASE_SCODE(DISP_E_BADCALLEE)
//        CASE_SCODE(DISP_E_NOTACOLLECTION)
//        CASE_SCODE(TYPE_E_IOERROR)
//        CASE_SCODE(TYPE_E_COMPILEERROR)
//        CASE_SCODE(TYPE_E_CANTCREATETMPFILE)
//        CASE_SCODE(TYPE_E_ILLEGALINDEX)
//        CASE_SCODE(TYPE_E_IDNOTFOUND)
//        CASE_SCODE(TYPE_E_BUFFERTOOSMALL)
//        CASE_SCODE(TYPE_E_READONLY)
//        CASE_SCODE(TYPE_E_INVDATAREAD)
//        CASE_SCODE(TYPE_E_UNSUPFORMAT)
//        CASE_SCODE(TYPE_E_ALREADYCONTAINSNAME)
//        CASE_SCODE(TYPE_E_NOMATCHINGARITY)
//        CASE_SCODE(TYPE_E_REGISTRYACCESS)
//        CASE_SCODE(TYPE_E_LIBNOTREGISTERED)
//        CASE_SCODE(TYPE_E_DUPLICATEDEFINITION)
//        CASE_SCODE(TYPE_E_USAGE)
//        CASE_SCODE(TYPE_E_DESTNOTKNOWN)
//        CASE_SCODE(TYPE_E_UNDEFINEDTYPE)
//        CASE_SCODE(TYPE_E_QUALIFIEDNAMEDISALLOWED)
//        CASE_SCODE(TYPE_E_INVALIDSTATE)
//        CASE_SCODE(TYPE_E_WRONGTYPEKIND)
//        CASE_SCODE(TYPE_E_ELEMENTNOTFOUND)
//        CASE_SCODE(TYPE_E_AMBIGUOUSNAME)
//        CASE_SCODE(TYPE_E_INVOKEFUNCTIONMISMATCH)
//        CASE_SCODE(TYPE_E_DLLFUNCTIONNOTFOUND)
//        CASE_SCODE(TYPE_E_BADMODULEKIND)
//        CASE_SCODE(TYPE_E_WRONGPLATFORM)
//        CASE_SCODE(TYPE_E_ALREADYBEINGLAIDOUT)
//        CASE_SCODE(TYPE_E_CANTLOADLIBRARY)
                                   
        default:
            szErrName = TEXT("UNKNOWN SCODE") ;
    }
    
    wsprintf( sz, TEXT( "%s (0x%lx)" ), (LPTSTR)szErrName, sc);

    return (LPTSTR)sz ;
}   

static TCHAR * g_rgszVT[] =
{
    TEXT( "Void" ),             //VT_EMPTY           = 0,   /* [V]   [P]  nothing                     */
    TEXT( "Null" ),             //VT_NULL            = 1,   /* [V]        SQL style Null              */
    TEXT( "Integer" ),          //VT_I2              = 2,   /* [V][T][P]  2 byte signed int           */
    TEXT( "Long" ),             //VT_I4              = 3,   /* [V][T][P]  4 byte signed int           */
    TEXT( "Single" ),           //VT_R4              = 4,   /* [V][T][P]  4 byte real                 */
    TEXT( "Double" ),           //VT_R8              = 5,   /* [V][T][P]  8 byte real                 */
    TEXT( "Currency" ),         //VT_CY              = 6,   /* [V][T][P]  currency                    */
    TEXT( "Date" ),             //VT_DATE            = 7,   /* [V][T][P]  date                        */
    TEXT( "String" ),           //VT_BSTR            = 8,   /* [V][T][P]  binary string               */
    TEXT( "Object" ),           //VT_DISPATCH        = 9,   /* [V][T]     IDispatch FAR*              */
    TEXT( "SCODE" ),            //VT_ERROR           = 10,  /* [V][T]     SCODE                       */
    TEXT( "Boolean" ),          //VT_BOOL            = 11,  /* [V][T][P]  True=-1, False=0            */
    TEXT( "Variant" ),          //VT_VARIANT         = 12,  /* [V][T][P]  VARIANT FAR*                */
    TEXT( "pIUnknown" ),        //VT_UNKNOWN         = 13,  /* [V][T]     IUnknown FAR*               */
    TEXT( "Unicode" ),          //VT_WBSTR           = 14,  /* [V][T]     wide binary string          */
    TEXT( "" ),                 //                   = 15,
    TEXT( "BYTE" ),             //VT_I1              = 16,  /*    [T]     signed char                 */
    TEXT( "char" ),             //VT_UI1             = 17,  /*    [T]     unsigned char               */
    TEXT( "USHORT" ),           //VT_UI2             = 18,  /*    [T]     unsigned short              */
    TEXT( "ULONG" ),            //VT_UI4             = 19,  /*    [T]     unsigned short              */
    TEXT( "int64" ),            //VT_I8              = 20,  /*    [T][P]  signed 64-bit int           */
    TEXT( "uint64" ),           //VT_UI8             = 21,  /*    [T]     unsigned 64-bit int         */
    TEXT( "int" ),              //VT_INT             = 22,  /*    [T]     signed machine int          */
    TEXT( "UINT" ),             //VT_UINT            = 23,  /*    [T]     unsigned machine int        */
    TEXT( "VOID" ),             //VT_VOID            = 24,  /*    [T]     C style void                */
    TEXT( "HRESULT" ),          //VT_HRESULT         = 25,  /*    [T]                                 */
    TEXT( "PTR" ),              //VT_PTR             = 26,  /*    [T]     pointer type                */
    TEXT( "SAFEARRAY" ),        //VT_SAFEARRAY       = 27,  /*    [T]     (use VT_ARRAY in VARIANT)   */
    TEXT( "CARRAY" ),           //VT_CARRAY          = 28,  /*    [T]     C style array               */
    TEXT( "USERDEFINED" ),      //VT_USERDEFINED     = 29,  /*    [T]     user defined type         */
    TEXT( "LPSTR" ),            //VT_LPSTR           = 30,  /*    [T][P]  null terminated string      */
    TEXT( "LPWSTR" ),           //VT_LPWSTR          = 31,  /*    [T][P]  wide null terminated string */
    TEXT( "" ),                 //                   = 32,
    TEXT( "" ),                 //                   = 33,
    TEXT( "" ),                 //                   = 34,
    TEXT( "" ),                 //                   = 35,
    TEXT( "" ),                 //                   = 36,
    TEXT( "" ),                 //                   = 37,
    TEXT( "" ),                 //                   = 38,
    TEXT( "" ),                 //                   = 39,
    TEXT( "" ),                 //                   = 40,
    TEXT( "" ),                 //                   = 41,
    TEXT( "" ),                 //                   = 42,
    TEXT( "" ),                 //                   = 43,
    TEXT( "" ),                 //                   = 44,
    TEXT( "" ),                 //                   = 45,
    TEXT( "" ),                 //                   = 46,
    TEXT( "" ),                 //                   = 47,
    TEXT( "" ),                 //                   = 48,
    TEXT( "" ),                 //                   = 49,
    TEXT( "" ),                 //                   = 50,
    TEXT( "" ),                 //                   = 51,
    TEXT( "" ),                 //                   = 52,
    TEXT( "" ),                 //                   = 53,
    TEXT( "" ),                 //                   = 54,
    TEXT( "" ),                 //                   = 55,
    TEXT( "" ),                 //                   = 56,
    TEXT( "" ),                 //                   = 57,
    TEXT( "" ),                 //                   = 58,
    TEXT( "" ),                 //                   = 59,
    TEXT( "" ),                 //                   = 60,
    TEXT( "" ),                 //                   = 61,
    TEXT( "" ),                 //                   = 62,
    TEXT( "" ),                 //                   = 63,
    TEXT( "FILETIME" ),         //VT_FILETIME        = 64,  /*       [P]  FILETIME                    */
    TEXT( "BLOB" ),             //VT_BLOB            = 65,  /*       [P]  Length prefixed bytes       */
    TEXT( "STREAM" ),           //VT_STREAM          = 66,  /*       [P]  Name of the stream follows  */
    TEXT( "STORAGE" ),          //VT_STORAGE         = 67,  /*       [P]  Name of the storage follows */
    TEXT( "STREAMED_OBJECT" ),  //VT_STREAMED_OBJECT = 68,  /*       [P]  Stream contains an object   */
    TEXT( "STORED_OBJECT" ),    //VT_STORED_OBJECT   = 69,  /*       [P]  Storage contains an object  */
    TEXT( "BLOB_OBJECT" ),      //VT_BLOB_OBJECT     = 70,  /*       [P]  Blob contains an object     */
    TEXT( "CF" ),               //VT_CF              = 71,  /*       [P]  Clipboard format            */
    TEXT( "CLSID" ),            //VT_CLSID           = 72   /*       [P]  A Class ID                  */
};

#if 0
LPTSTR VTtoString( VARTYPE vt )
{
    static TCHAR szBuf[64];

    if (vt <= VT_CLSID)
        return (LPTSTR)g_rgszVT[vt] ;

    if (vt & VT_VECTOR)
    {
        vt &= ~VT_VECTOR ;
        if (vt <= VT_CLSID)
            wsprintf( szBuf, TEXT( "VECTOR of %s" ), (LPTSTR)g_rgszVT[vt] ) ;
        else
            wsprintf( szBuf, TEXT( "<Unknown %08lX>" ), vt & VT_VECTOR ) ;
        return (LPTSTR)szBuf ;
    }

    if (vt & VT_ARRAY)
    {
        vt &= ~VT_ARRAY ;
        if (vt <= VT_CLSID)
            wsprintf( szBuf, TEXT( "Array of %s" ), (LPTSTR)g_rgszVT[vt] ) ;
        else
            wsprintf( szBuf, TEXT( "<Unknown %08lX>" ), vt & VT_ARRAY ) ;
        return (LPTSTR)szBuf ;
    }

    if (vt & VT_BYREF)
    {
        vt &= ~VT_BYREF ;
        if (vt <= VT_CLSID)
            wsprintf( szBuf, TEXT( "%s BYREF " ), (LPTSTR)g_rgszVT[vt] ) ;
        else
            wsprintf( szBuf, TEXT( "<Unknown %08lX>" ), vt & VT_BYREF ) ;
        return (LPTSTR)szBuf ;
    }

    if (vt & VT_RESERVED)
    {
        vt &= ~VT_RESERVED ;
        if (vt <= VT_CLSID)
            wsprintf( szBuf, TEXT( "RESERVED (%s)" ), (LPTSTR)g_rgszVT[vt] ) ;
        else
            wsprintf( szBuf, TEXT( "<Unknown %08lX>" ), vt & VT_RESERVED ) ;
        return (LPTSTR)szBuf ;
    }

    wsprintf( szBuf, TEXT( "<Unknown %08lX>" ), vt ) ;
    return (LPTSTR)szBuf ;
}
#endif
