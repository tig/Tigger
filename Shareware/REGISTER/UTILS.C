/************************************************************************
 *
 *     Project:  ShareReg
 *
 *      Module:  utils.c
 *
 *     Remarks:  fun utility functions!
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include <windows.h>

#include "sharereg.h"

extern  HANDLE hAppInst ;
extern  char szAppName[] ;

#define MAX_STRLEN   256

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/************************************************************************
 *
 *  REALLY SELECT A FONT STUFF
 *
 *  The following code selects a font based on a point setting (not
 *  pixels).  It is cool since it helps avoid the wrath of the font
 *  randomizer in GDI.
 *
 **********************************************************************/

/****************************************************************
 *  HFONT FAR PASCAL
 *    ReallyCreateFont( HDC hDC, LPSTR lpszFace, short nPointSize,
 *                      short nWeight, WORD wFlags )
 *
 *  Description: 
 *
 *    Creates a font just like CreateFont, but attempts to avoid the
 *    font randomizer.
 *
 *    wFlags can be RCF_ITALIC, RCF_UNDERLINE,
 *    RCF_BOLD and/or RCF_STRIKEOUT.
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

   if (!(lpfnEnumReally = MakeProcInstance( fnEnumReally, hAppInst )))
   {
      DP1( hWDB, "MakeProcInstance failed" ) ;
      return FALSE ;
   }

   //
   // if the DC wasn't specified then use the display...
   //
   if (!hDC)
   {
      if (!(hCurDC = GetDC( NULL )))
      {
         DP1( hWDB,"Could not GetDC" ) ;
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

      lf.lfHeight = -MulDiv( nPointSize,
                             GetDeviceCaps( hCurDC, LOGPIXELSX),
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
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/*************************************************************************
 *  short FAR _cdecl ErrorResBox( HWND hWnd, HANDLE hInst, WORD wFlags,
 *                                WORD idAppName, WORD idErrorStr,
 *                                LPSTR lpszParams, ... ) ;
 *
 *  Description:
 *
 *  This function displays a message box using program resource strings.
 *       
 *  Type/Parameter
 *          Description
 *
 *      HWND hWnd
 *            Specifies the message box parent window.
 *
 *      HANDLE hInst
 *            Specifies the handle to the module that contains the resource
 *            strings specified by idAppName and idErrorStr.  If this value
 *            is NULL, the instance handle is obtained from hWnd (in which
 *            case hWnd may not be NULL).
 *
 *      WORD wFlags
 *            Specifies message box types controlloing the message box
 *            appearance.  All message box types valid for MessageBox are valid.
 *      
 *      WORD idAPpName
 *            Specifies the resource ID of a string that
 *            is to be used as the message box caption.
 * 
 *      WORD idErrorStr
 *             Specifies the resource ID of a error message format string.
 *             This string is of the style passed to wsprintf(),
 *             containing the standard C argument formatting
 *             characters.  Any procedure parameters following
 *             idErrorStr will be taken as arguments for this format
 *             string.
 * 
 *      arguments [ arguments, ... ]
 *             Specifies additional arguments corresponding to the
 *             format specification given by idErrorStr.  All
 *             string arguments must be FAR pointers.
 * 
 * 
 *  Return Value
 *    Returns the result of the call to MessageBox().  If an
 *    error occurs, returns zero.
 *
 *  Comment:
 *       This is a variable arguments function, the parameters after
 *       idErrorStr being taken for arguments to the printf() format
 *       string specified by idErrorStr.  The string resources specified
 *       by idAppName and idErrorStr must be loadable using the
 *       instance handle hInst.  If the strings cannot be
 *       loaded, or hwnd is not valid, the function will fail and return
 *       zero.
 *
 *************************************************************************/
short FAR _cdecl ErrorResBox( HWND hwnd, HANDLE hInst, WORD flags,
                        		WORD idAppName, WORD idErrorStr, ...)
{
   PSTR	sz ;
   PSTR	szFmt ;
   WORD	w ;

   DP5( hWDB, "ErrorResBox( 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X )",
                       hwnd,   hInst,  flags,  idAppName, idErrorStr ) ;
   if (!hInst)
   {
      if (!hwnd)
      {
         MessageBeep( 0 ) ;
         return FALSE ;
      }
      hInst = GetWindowWord( hwnd, GWW_HINSTANCE ) ;
   }

   sz = (PSTR) LocalAlloc( LPTR, MAX_STRLEN ) ;
   szFmt = (PSTR) LocalAlloc( LPTR, MAX_STRLEN ) ;
   w = 0 ;

   if (!LoadString( hInst, idErrorStr, szFmt, MAX_STRLEN ))
      goto ExitError ;

   wvsprintf( sz, szFmt, (LPSTR)(((WORD FAR *)&idErrorStr) + 1) ) ;

   if (!LoadString( hInst, idAppName, szFmt, MAX_STRLEN ))
      goto ExitError ;
   w = MessageBox( hwnd, sz, szFmt, flags ) ;

   ExitError :
   LocalFree( (HANDLE) sz) ;
   LocalFree( (HANDLE) szFmt) ;
   return w ;

}/* ErrorResBox() */

/*************************************************************************
 *  void FAR PASCAL
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
void FAR PASCAL
DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient )
{
   RECT rcWithin ;
   RECT rcCenter ;
   int   x, y ;

//   DP5( hWDB, "DlgCenter: hwndCenter = %d, hwndWithin = %d, fClient = %d",
//         hwndCenter, hwndWithin, fClient ) ;

   if (!IsWindow(hwndCenter))
   {
      DP1( hWDB,"DlgCenter: hwndCenter is not a window") ;
      return ;
   }

   if (hwndWithin && !IsWindow(hwndWithin))
   {
      DP1( hWDB,"DlgCenter: hwndWithin is not a window or NULL") ;
      return ;
   }

   if (hwndWithin == NULL)
   {
      rcWithin.left = rcWithin.top = 0 ;
      rcWithin.right = GetSystemMetrics( SM_CXSCREEN ) ;
      rcWithin.bottom = GetSystemMetrics( SM_CYSCREEN ) ;
   }
   else
   {
      HWND hwndParent = GetParent( hwndCenter ) ;

      if (fClient)
      {
         GetClientRect( hwndWithin, (LPRECT)&rcWithin ) ;

         //
         // Convert to screen coords
         //
         if (hwndWithin)
         {
            ClientToScreen( hwndWithin, (LPPOINT)&rcWithin.left ) ;
            ClientToScreen( hwndWithin, (LPPOINT)&rcWithin.right ) ;
         }

         //
         // Convert parent coords
         //
         if (hwndParent)
         {
            ScreenToClient( hwndParent, (LPPOINT)&rcWithin.left ) ;
            ScreenToClient( hwndParent, (LPPOINT)&rcWithin.right ) ;
         }
      }
      else
      {
            GetWindowRect( hwndWithin, (LPRECT) &rcWithin ) ;
            //
            // Convert to parent coords
            //
            if (hwndParent)

            {
               ScreenToClient( hwndParent, (LPPOINT)&rcWithin.left ) ;
               ScreenToClient( hwndParent, (LPPOINT)&rcWithin.right ) ;
            }

      }
   }

   GetWindowRect( hwndCenter, (LPRECT)&rcCenter ) ;
   if (hwndWithin)
   {
      ScreenToClient( hwndWithin, (LPPOINT)&rcCenter.left ) ;
      ScreenToClient( hwndWithin, (LPPOINT)&rcCenter.right ) ;
   }

   x = (rcWithin.left + rcWithin.right  + rcCenter.left - rcCenter.right)  / 2 ;
   y = (rcWithin.top  + rcWithin.bottom + rcCenter.top  - rcCenter.bottom) / 2 ;

   //
   // if it's a popup
   //
   if (hwndWithin == GetParent( hwndCenter ) &&
       !(GetWindowLong( hwndCenter, GWL_STYLE ) & WS_CHILD ))
   {
      GetWindowRect( hwndWithin, (LPRECT) &rcWithin ) ;
      x += rcWithin.left ;
      y += rcWithin.top ;
   }

   SetWindowPos( hwndCenter, NULL, max(x,0), max(y,0),
                  0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER ) ;

   return ;

}/* DlgCenter() */



#define POS_STR      "Window Pos"
#define HEIGHT_STR   "Height"
#define WIDTH_STR    "Width"
#define X_STR        "X"
#define Y_STR        "Y"

/****************************************************************
 *  void FAR PASCAL PlaceWindow( HWND hWnd )
 *
 *  Description: 
 *
 *    Places the window based on the private INI file.
 *
 *  Comments:
 *
 ****************************************************************/
void FAR PASCAL PlaceWindow( HWND hWnd )
{
   short x, y ;
   short w, h ;
   char  szFileName[14] ;

   wsprintf( szFileName, "%s.INI", (LPSTR)szAppName) ;

   w = GetPrivateProfileInt( POS_STR, WIDTH_STR, 0, szFileName ) ;

   //
   // return if there is nothing saved.
   //
   if (!w)
      return ;

   h = GetPrivateProfileInt( POS_STR, HEIGHT_STR, 0, szFileName ) ;

   x = GetPrivateProfileInt( POS_STR, X_STR, 0, szFileName ) ;
   y = GetPrivateProfileInt( POS_STR, Y_STR, 0, szFileName ) ;

   SetWindowPos( hWnd, NULL, x, y, w, h, SWP_NOZORDER ) ;

} /* PlaceWindow()  */

/****************************************************************
 *  void FAR PASCAL WritePrivateProfileInt()
 *
 *  Description: 
 *
 *    write a integer to a INI file
 *
 *  Comments:
 *
 ****************************************************************/

void FAR PASCAL WritePrivateProfileInt(LPSTR szSection, LPSTR szKey, int i, LPSTR szIniFile)
{
    char     ach[32] ;

    if (i != (int)GetPrivateProfileInt(szSection, szKey, ~i, szIniFile))
    {
        wsprintf(ach, "%d", i);
        WritePrivateProfileString(szSection, szKey, ach, szIniFile);
    }
}

/****************************************************************
 *  void FAR PASCAL SaveWindowPos( HWND hWnd )
 *
 *  Description: 
 *
 *    Saves the window position.
 *
 *  Comments:
 *
 ****************************************************************/
void FAR PASCAL SaveWindowPos( HWND hWnd )
{
   RECT     rc ;

   char  szFileName[14] ;

   wsprintf( szFileName, "%s.INI", (LPSTR)szAppName) ;

   if (IsIconic( hWnd ))
      return ;

   GetWindowRect( hWnd, &rc ) ;

   WritePrivateProfileInt(POS_STR, X_STR, rc.left, szFileName);
   WritePrivateProfileInt(POS_STR, Y_STR, rc.top, szFileName);
   WritePrivateProfileInt(POS_STR, WIDTH_STR,  rc.right-rc.left, szFileName);
   WritePrivateProfileInt(POS_STR, HEIGHT_STR, rc.bottom-rc.top, szFileName);

} /* SaveWindowPos()  */



/************************************************************************
 * End of File: utils.c
 ***********************************************************************/

