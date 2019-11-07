//*************************************************************
//  File name: slider.c
//
//  Description: 
//      Code for the slider control
//
//  History:    Date       Author     Comment
//               2/25/92   MSM        Updated for FontShopper
//               2/26/92   MSM        Removed LocalLock / LocalUnlock
//               2/26/92   MSM        Changed notification to SLN_????
//                                    and use parent if no hNotifyWnd!!
//               5/6/92    CEK        Moved to CEK.DLL
//
//*************************************************************

#include "PRECOMP.H"

#include "ICEK.h"

#ifndef WIN32

extern HINSTANCE ghmodDLL ;
extern BOOL      bMonochrome ;

#define MAXLEN_TEMPSTR      81
#define MAXLEN_FLAGSTR      81

#define SHADOWWIDTH     1 
#define GWW_HSLIDERINFO 0

// data structures

typedef struct tagSLIDERINFO
{
   int      nNumCols ;      // number of columns
   int      nDragCol ;      // column in drag
   int      nFontHeight ;   // computed font height
   BOOL     fDragging ;     // dragging status
   BOOL     fShifted ;      // shift key was down when we started dragging
   BOOL     f3DOut ;        // if TRUE "3D Out" look is used, else "3D In"
   HANDLE   hColumnInfo ;   // handle to column info structure
   HCURSOR  hSplitCursor ;  // handle to split cursor
   HDC      hScreenDC ;     // handle to screen DC
   HFONT    hFont ;         // handle to selected font
   HFONT    hOldFont ;      // handle to old font
   HWND     hNotifyWnd ;    // handle to notification window
   DWORD    rgbFG ;         // foreground color
   DWORD    rgbBG ;         // background color

} SLIDERINFO, NEAR *NPSLIDERINFO ;

DECLARE_HANDLE( HSLIDERINFO ) ;

typedef struct tagCOLUMNINFO
{
   int      nColPos ;         // in percentage (0 - 100)
   BOOL     bDragging ;       // in dragging mode
   WORD     wAlignment ;      // alignment flag
   HANDLE   hColHeader ;      // handle to column header
   
} COLUMNINFO, NEAR *NPCOLUMNINFO ;

DECLARE_HANDLE( HCOLUMNINFO ) ;

// macros

#ifdef WIN32
#define GETHSLIDERINFO( hWnd ) ((HSLIDERINFO) GetWindowLong( hWnd, GWW_HSLIDERINFO ))
#else
#define GETHSLIDERINFO( hWnd ) ((HSLIDERINFO) GetWindowWord( hWnd, GWW_HSLIDERINFO ))
#endif

// function prototypes (private)

HSLIDERINFO NEAR CreateSliderInfo( HWND, DWORD ) ;
LRESULT NEAR DestroySliderInfo( HWND ) ;
HCOLUMNINFO NEAR CreateColumnInfo( int ) ;
LRESULT NEAR DestroyColumnInfo( HCOLUMNINFO, int ) ;
LRESULT NEAR SetNumberColumns( HWND, UINT ) ;
LRESULT NEAR SetColumnHeader( HWND, UINT, LPSTR ) ;
LRESULT NEAR SetColumnAlign( HWND, UINT, UINT ) ;
LRESULT NEAR SetFGColor( HWND, DWORD ) ;
LRESULT NEAR SetBGColor( HWND, DWORD ) ;
LRESULT NEAR SetColumnPosition( HWND, UINT, UINT ) ;
LRESULT NEAR SetNotifyWnd( HWND, HWND ) ;
LRESULT NEAR GetNumberColumns( HWND ) ;
LRESULT NEAR GetColumnPosition( HWND, UINT ) ;
LRESULT NEAR GetColumnAlign( HWND, UINT ) ;
HWND NEAR GetNotifyWnd( HWND ) ;
LRESULT NEAR EraseBackground( HWND, HDC ) ;
LRESULT NEAR PaintSlider( HWND ) ;
LRESULT NEAR SetFont( HWND, HFONT, LONG ) ;
LRESULT NEAR SizeSlider( HWND, UINT, LONG ) ;
LRESULT NEAR MouseMove( HWND, UINT, LONG ) ;
LRESULT NEAR MouseLButtonDown( HWND, UINT, LONG ) ;
LRESULT NEAR MouseLButtonUp( HWND, UINT, LONG ) ;
VOID NEAR UpdateSliderPos( HDC, LPRECT, int, int ) ;

VOID NEAR AddStyleString( LPSTR, int ) ;

// function prototypes (exported)
LRESULT FAR PASCAL fnSlider( HWND, UINT, WPARAM, LPARAM ) ;


/*************************************************************************
 *  BOOL NEAR PASCAL SliderInit( HINSTANCE hPrev, HINSTANCE hInstance,
 *       LPSTR szClassName )
 *
 *  Description: 
 *
 *    Registers the window class.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL WINAPI SliderInit( HINSTANCE hPrev, HINSTANCE hInstance, LPSTR lpsz )
                     
{
   WNDCLASS wc ;

   if (!hPrev)
   {
      wc.style =         CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS |
                        CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW |
                        CS_OWNDC | CS_GLOBALCLASS ;
      wc.lpfnWndProc =   (WNDPROC)fnSlider;
      wc.cbClsExtra =    0 ;
      wc.cbWndExtra =    sizeof( HANDLE ) ;
      wc.hInstance =     hInstance ;
      wc.hIcon =         NULL ;
      wc.hCursor =       NULL ;
      wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1) ;
      wc.lpszMenuName =  NULL ;

      if (lpsz)
         wc.lpszClassName = lpsz;
      else
         wc.lpszClassName = SZ_SLIDERCLASSNAME;

      if (RegisterClass( &wc ))
      /*
      * always return true.  Register class will return failure if
      * the class was already registerd, but that's no big deal because
      * we have used a very unique class name (haven't we?).
      */
      return TRUE ;
   }
   return TRUE ;
}/* SliderInit() */

//************************************************************************
//  fnSlider
//
//  Description:
//     Slider control for list box multi-column management.
//
//  Parameters:
//     As defined for standard Window procedures with the following
//     exceptions:
//
//     UINT wMsg
//        Additional control messages supported:
//
//        NOTE:
//           All references to column position are ZERO based.
//           For example:  SendMessage( hSlider, SL_SETCOLHEADER, 0, 
//                                      (LPARAM) (LPSTR) "Column 1" ) ;
//           sets the column heading for the leftmost column.
//
//        SL_SETNUMCOLS:
//           wParam contains the number of slider columns
//           returns TRUE if everything was successful, otherwise
//           FALSE.
//
//        SL_SETCOLHEADER:
//           Sets the column header for the column selected in
//           wParam.  lParam is a LPSTR to the header string.
//            
//        SL_SETCOLALIGN:
//           Sets the column alignment for the column selected
//           in wParam.  The LOWORD( lParam ) contains the
//           alignment style, one of SA_LEFT, SA_RIGHT, or
//           SA_CENTER (left, right or center alignment
//           respectively).
//
//        SL_SETFGCOLOR:
//           Sets the foreground color of the column header text
//           to the RGB value in lParam.  Default is the system
//           button text color.
//
//        SL_SETBGCOLOR;
//           Sets the background color of the slider control to
//           the RGB value in lParam. Default is the system button
//           face color.
//
//        SL_SETCOLPOS:
//           Sets the position of column selected in wParam to
//           the percentage in the LOWORD of lParam.
//
//        SL_SETNOTIFYWND:
//           Sets the window to receive the notifications from
//           the slider control.
//
//        SL_GETNUMCOLS:
//           Returns the number of columns in the slider control.
//
//        SL_GETCOLPOS:
//           Returns the column position of the column selected
//           in wParam.
//
//        SL_GETCOLALIGN:
//           Returns the column alignment for the column selected
//           in wParam.
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT FAR PASCAL fnSlider( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   switch (wMsg)
   {
      case WM_CREATE:
      {
         int          cxVertScroll ;
         HSLIDERINFO  hSliderInfo ;

         if (NULL ==
            (hSliderInfo =
               CreateSliderInfo( hWnd,
                                 ((LPCREATESTRUCT) lParam) -> style )))
            return ( (LRESULT) -1 ) ;  // fail window creation

         // store the slider information

         SetWindowWord( hWnd, GWW_HSLIDERINFO, (WPARAM) hSliderInfo ) ;

         // check if SLS_ADJUSTSIZE... if so, munge the size to account
         // for a scroll bar in the list box 

         cxVertScroll = 0 ;
         if (((LPCREATESTRUCT) lParam) -> style & (DWORD) SLS_ADJUSTSIZE)
            cxVertScroll = GetSystemMetrics( SM_CXVSCROLL ) - 1 ;

         // resize the window (assumes that we are aligned
         // with a listbox)

         // NOTE: horizontal size gets fudged if we are using
         // the SLS_ADJUSTSIZE style

         MoveWindow( hWnd,
                     ((LPCREATESTRUCT) lParam) -> x - GetSystemMetrics( SM_CXBORDER ),
                     ((LPCREATESTRUCT) lParam) -> y - GetSystemMetrics( SM_CYBORDER ),
                     ((LPCREATESTRUCT) lParam) -> cx +
                        (GetSystemMetrics( SM_CXBORDER ) << 1) - cxVertScroll,
                     ((LPCREATESTRUCT) lParam) -> cy + (GetSystemMetrics( SM_CYBORDER ) << 1),
                     FALSE ) ;
      }
      return ( 0 ) ;

      case WM_DESTROY:
         DestroySliderInfo( hWnd ) ;
         break ;

      case WM_MOUSEMOVE:
         return ( MouseMove( hWnd, (UINT) wParam, (LONG) lParam ) ) ;

      case WM_ERASEBKGND:
         return ( EraseBackground( hWnd, (HDC) wParam ) ) ;

      case WM_PAINT:
         return ( PaintSlider( hWnd ) ) ;

      case WM_SETFONT:
         return ( SetFont( hWnd, (HFONT) wParam, (LONG) lParam ) ) ;

      case WM_LBUTTONDOWN:
         return ( MouseLButtonDown( hWnd, (UINT) wParam, (LONG) lParam ) ) ;

      case WM_LBUTTONUP:
         return ( MouseLButtonUp( hWnd, (UINT) wParam, (LONG) lParam ) ) ;

      case SL_SETNUMCOLS:
         return ( SetNumberColumns( hWnd, (UINT) wParam ) ) ;

      case SL_SETCOLHEADER:
         return ( SetColumnHeader( hWnd, (UINT) wParam, (LPSTR) lParam ) ) ;

      case SL_SETCOLALIGN:
         return ( SetColumnAlign( hWnd, (UINT) wParam, LOWORD( lParam ) ) ) ;

      case SL_SETFGCOLOR:
         return ( SetFGColor( hWnd, (LONG) lParam ) ) ;

      case SL_SETBGCOLOR:
         return ( SetBGColor( hWnd, (LONG) lParam ) ) ;

      case SL_SETCOLPOS:
         return ( SetColumnPosition( hWnd, (UINT) wParam,
                                     LOWORD( lParam ) ) ) ;

      case SL_SETNOTIFYWND:
         return ( SetNotifyWnd( hWnd, (HWND) wParam ) ) ;

      case SL_GETNUMCOLS:
         return ( GetNumberColumns( hWnd ) ) ;

      case SL_GETCOLPOS:
         return ( GetColumnPosition( hWnd, (UINT) wParam ) ) ;

      case SL_GETCOLALIGN:
         return ( GetColumnAlign( hWnd, (UINT) wParam ) ) ;

      default:
         return ( DefWindowProc( hWnd, wMsg, wParam, lParam ) ) ;
   }

} //*** fnSlider

//************************************************************************
//  CreateSliderInfo
//
//  Description:
//     Creates the slider information structure.  Returns
//     a handle if successful or NULL if failure.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     DWORD dwStyle
//        extended style information
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

HSLIDERINFO NEAR CreateSliderInfo( HWND hWnd, DWORD dwStyle )
{
   HSLIDERINFO   hSliderInfo ;
   NPSLIDERINFO  npSliderInfo ;

   if (NULL == (hSliderInfo = LocalAlloc( LPTR,
                                          sizeof ( SLIDERINFO ) )))
      return ( NULL ) ;

   npSliderInfo = (NPSLIDERINFO) hSliderInfo ;

   // load "split" cursor

   npSliderInfo -> hSplitCursor =
      LoadCursor( ghmodDLL, MAKEINTRESOURCE( SPLITCURSOR ) ) ;

   // default attributes 

   npSliderInfo -> rgbFG = GetSysColor( COLOR_BTNTEXT ) ;
   npSliderInfo -> rgbBG = GetSysColor( COLOR_BTNFACE ) ;

   // get extended style information

   if (dwStyle & SLS_3DLOOK_IN)
      npSliderInfo -> f3DOut = FALSE ;

   if (dwStyle & SLS_3DLOOK_OUT)
      npSliderInfo -> f3DOut = TRUE ;

   // get screen DC

   npSliderInfo -> hScreenDC = GetDC( hWnd ) ;

   // set colors

   SetBkColor( npSliderInfo -> hScreenDC, npSliderInfo -> rgbBG ) ;
   SetTextColor( npSliderInfo -> hScreenDC, npSliderInfo -> rgbFG ) ;

   // set text mode, alignment

   SetTextAlign( npSliderInfo -> hScreenDC, TA_LEFT ) ;
   SetBkMode( npSliderInfo -> hScreenDC, TRANSPARENT ) ;

   return ( hSliderInfo ) ;

} //*** CreateSliderInfo

//************************************************************************
//  DestroySliderInfo
//
//  Description:
//     Destroys (frees) the slider information structure.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR DestroySliderInfo( HWND hWnd )
{
   HSLIDERINFO   hSliderInfo ;
   NPSLIDERINFO  npSliderInfo ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) hSliderInfo;
                     
   if (hSliderInfo == NULL)
      return ( (LRESULT) TRUE ) ;  // no work to be done!

   npSliderInfo = (NPSLIDERINFO)hSliderInfo ;
   ReleaseDC( hWnd, npSliderInfo -> hScreenDC ) ;
   if (npSliderInfo -> hColumnInfo != NULL)
      DestroyColumnInfo( npSliderInfo -> hColumnInfo,
                         npSliderInfo -> nNumCols ) ;
   LocalFree( hSliderInfo ) ;
   return ( (LRESULT) TRUE ) ;

} //*** DestroySliderInfo

//************************************************************************
//  CreateColumnInfo
//
//  Description:
//     Creates the column information structure.  Returns NULL
//     if unsuccessful.
//
//     Note: This routine sets up default alignments and positions
//     for the columns.
//
//  Parameters:
//     int nNumCols
//        number of columns in the new column info structure
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

HCOLUMNINFO NEAR CreateColumnInfo( int nNumCols )
{
   int           i ;
   HCOLUMNINFO   hColumnInfo ;
   NPCOLUMNINFO  npColumnInfo ;

   if (0 == nNumCols)
      return ( NULL ) ;

   if (NULL == (hColumnInfo = LocalAlloc( LPTR,
                                          nNumCols * sizeof( COLUMNINFO ) )))
      return ( NULL ) ;

   npColumnInfo = (NPCOLUMNINFO)hColumnInfo;
   for (i = 0; i < nNumCols; i++)
   {
      (npColumnInfo + i) -> nColPos = 100 * i / nNumCols ; // a percentage
      (npColumnInfo + i) -> wAlignment = SA_LEFT ;
   }
   return ( hColumnInfo ) ;

} //*** CreateColumnInfo

//************************************************************************
//  DestroyColumnInfo
//
//  Description:
//     Destroys (frees) the column information structure.
//
//  Parameters:
//     HCOLUMNINFO hColumnInfo
//        handle to column information structure
//
//     int nNumCols
//        number of columns in the structure
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR DestroyColumnInfo( HCOLUMNINFO hColumnInfo, int nNumCols )
{
   int           i ;
   NPCOLUMNINFO  npColumnInfo ;

   if (hColumnInfo == NULL)
      return ( (LRESULT) TRUE ) ;  // no work to be done!

   npColumnInfo = (NPCOLUMNINFO)hColumnInfo;

   for (i = 0; i < nNumCols; i++)
      if (NULL != (npColumnInfo + i) -> hColHeader)
         LocalFree( (npColumnInfo + i) -> hColHeader ) ;
   LocalFree( hColumnInfo ) ;
   return ( (LRESULT) TRUE ) ;

} //*** DestroyColumnInfo

//************************************************************************
//  SetNumberColumns
//
//  Description:
//     Sets the number of columns for the slider control.  This
//     function creates a new column information structure.  If
//     previous structure already exists it is destroyed.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     UINT wNumCols
//        number of columns in the slider
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR SetNumberColumns( HWND hWnd, UINT wNumCols )
{
   HSLIDERINFO   hSliderInfo ;
   HCOLUMNINFO   hColumnInfo ;
   NPSLIDERINFO  npSliderInfo ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO)hSliderInfo;

   // destroy the current column information and clear
   // the settings in the SliderInfo structure

   DestroyColumnInfo( npSliderInfo -> hColumnInfo, npSliderInfo -> nNumCols ) ;
   npSliderInfo -> hColumnInfo = NULL ;
   npSliderInfo -> nNumCols = 0 ;

   if (wNumCols > 0)
   {
      if (NULL == (hColumnInfo = CreateColumnInfo( wNumCols )))
      {

         // cause repaint (we've deleted whatever was there)
         DP1( hWDB, "SetNumberColumns1" ) ;
         InvalidateRect( hWnd, NULL, TRUE ) ;

         // force an immediate paint

         UpdateWindow( hWnd ) ;

         return ( (LRESULT) FALSE ) ;
      }
      npSliderInfo -> hColumnInfo = hColumnInfo ;
      npSliderInfo -> nNumCols = wNumCols ;
   }

   // cause repaint

   DP1( hWDB, "SetNumberColumns2" ) ;
   InvalidateRect( hWnd, NULL, TRUE ) ;

   // force an immediate paint

   UpdateWindow( hWnd ) ;

   return ( (LRESULT) TRUE ) ;

} //*** SetNumberColumns

//************************************************************************
//  SetColumnHeader
//
//  Description:
//     Sets the column header text.  If a previous string was already
//     used it is deleted and a new allocation is made.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     UINT wColumn
//        column referenced
//
//     LPSTR lpText
//        column text
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR SetColumnHeader( HWND hWnd, UINT wColumn, LPSTR lpText )
{
   HSLIDERINFO   hSliderInfo ;
   HLOCAL        hColHeader ;
   NPCOLUMNINFO  npColumnInfo ;
   NPSLIDERINFO  npSliderInfo ;
   NPSTR         npColHeader ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO)hSliderInfo;
   if ((NULL == npSliderInfo -> hColumnInfo) ||
       (wColumn >= (UINT) npSliderInfo -> nNumCols))
   {
      return ( (LRESULT) FALSE ) ;
   }
   npColumnInfo = (NPCOLUMNINFO)(npSliderInfo -> hColumnInfo);
   if (NULL != (npColumnInfo + wColumn) -> hColHeader)
   {
      LocalFree( (npColumnInfo + wColumn) -> hColHeader ) ;
      (npColumnInfo + wColumn) -> hColHeader = NULL ;
   }
   if (NULL == (hColHeader = LocalAlloc( LPTR,
                                         lstrlen( lpText ) + 1 )))
   {
      return ( (LRESULT) FALSE ) ;
   }
   npColHeader = (NPSTR) ( hColHeader ) ;
   lstrcpy( (LPSTR) npColHeader, lpText ) ;
   (npColumnInfo + wColumn) -> hColHeader = hColHeader ;

   // cause repaint

   InvalidateRect( hWnd, NULL, FALSE ) ;

   return ( (LRESULT) TRUE ) ;

} //*** SetColumnHeader

//************************************************************************
//  SetColumnAlign
//
//  Description:
//     Sets the column alignment.  Returns FALSE if failure.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     UINT wColumn
//        referenced column
//
//     UINT wAlignment
//        column alignment, one of:
//           SA_LEFT, SA_CENTER, or SA_RIGHT
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR SetColumnAlign( HWND hWnd, UINT wColumn, UINT wAlignment )
{
   HSLIDERINFO   hSliderInfo ;
   NPCOLUMNINFO  npColumnInfo ;
   NPSLIDERINFO  npSliderInfo ;

   if (NULL == (hSliderInfo = GETHSLIDERINFO( hWnd )))
      return ( (LRESULT) FALSE ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;
   if ((NULL == npSliderInfo -> hColumnInfo) ||
       (wColumn >= (UINT) npSliderInfo -> nNumCols))
   {
      return ( (LRESULT) FALSE ) ;
   }
   npColumnInfo = (NPCOLUMNINFO) ( npSliderInfo -> hColumnInfo ) ;
   (npColumnInfo + wColumn) -> wAlignment = wAlignment ;

   // cause repaint

   InvalidateRect( hWnd, NULL, FALSE ) ;

   return ( (LRESULT) TRUE ) ;

} //*** SetColumnAlign

//************************************************************************
//  SetFGColor
//
//  Description:
//     Sets the foreground color of this control.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     DWORD rgbFG
//        RGB representation of the color
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR SetFGColor( HWND hWnd, DWORD rgbFG )
{
   HSLIDERINFO   hSliderInfo ;
   NPSLIDERINFO  npSliderInfo ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;
   npSliderInfo -> rgbFG = rgbFG ;
   SetTextColor( npSliderInfo -> hScreenDC, npSliderInfo -> rgbFG ) ;

   // cause repaint

   InvalidateRect( hWnd, NULL, FALSE ) ;

   return ( (LRESULT) TRUE ) ;

} //*** SetFGColor

//************************************************************************
//  SetBGColor
//
//  Description:
//     Sets background color of this control.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     DWORD rgbBG
//        RGB representation of the background color
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR SetBGColor( HWND hWnd, DWORD rgbBG )
{
   HSLIDERINFO   hSliderInfo ;
   NPSLIDERINFO  npSliderInfo ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;
   npSliderInfo -> rgbBG = rgbBG ;
   SetBkColor( npSliderInfo -> hScreenDC, npSliderInfo -> rgbBG ) ;

   // cause repaint

   DP1( hWDB, "SetBGCOlor" ) ;
   InvalidateRect( hWnd, NULL, TRUE ) ;

   return ( (LRESULT) TRUE ) ;

} // *** SetBGColor

//************************************************************************
//  SetColumnPosition
//
//  Description:
//     Sets the position of the column specified in wColumn.  wPosition
//     must be in the range (0, 100) (percentage).
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     UINT wColumn
//        referenced column
//
//     UINT wPosition
//        new column position
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR SetColumnPosition( HWND hWnd, UINT wColumn, UINT wPosition )
{
   HSLIDERINFO   hSliderInfo ;
   NPCOLUMNINFO  npColumnInfo ;
   NPSLIDERINFO  npSliderInfo ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;
   if ((NULL == npSliderInfo -> hColumnInfo) ||
       (wColumn >= (UINT) npSliderInfo -> nNumCols))
   {
      return ( (LRESULT) FALSE ) ;
   }
   npColumnInfo = (NPCOLUMNINFO) ( npSliderInfo -> hColumnInfo ) ;
   (npColumnInfo + wColumn) -> nColPos = wPosition ;

   // cause repaint

   InvalidateRect( hWnd, NULL, FALSE ) ;

   return ( (LRESULT) TRUE ) ;

} //*** SetColumnPosition

//************************************************************************
//  SetNotifyWnd
//
//  Description:
//     Sets the handle for the window that will receive drag
//     notifications.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     HWND hNotifyWnd
//        window to receive drag notifications
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR SetNotifyWnd( HWND hWnd, HWND hNotifyWnd )
{
   HSLIDERINFO   hSliderInfo ;
   NPSLIDERINFO  npSliderInfo ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;
   npSliderInfo -> hNotifyWnd = hNotifyWnd ;

   return ( (LRESULT) TRUE ) ;

} //*** SetNotifyWnd

//************************************************************************
//  GetNumberColumns
//
//  Description:
//     Returns the number of columns set using SetNumberColumns().
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR GetNumberColumns( HWND hWnd )
{
   HSLIDERINFO   hSliderInfo ;
   NPSLIDERINFO  npSliderInfo ;
   UINT          wNumCols ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;
   wNumCols = npSliderInfo -> nNumCols ;
   return ( (LRESULT) (LONG) wNumCols ) ;

} //*** GetNumberColumns

//************************************************************************
//  GetColumnPosition
//
//  Description:
//     Returns the current position of the column specified by
//     wColumn.  Returns -1 if not successful.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     UINT wColumn
//        referenced column
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR GetColumnPosition( HWND hWnd, UINT wColumn )
{
   HSLIDERINFO   hSliderInfo ;
   NPCOLUMNINFO  npColumnInfo ;
   NPSLIDERINFO  npSliderInfo ;
   UINT          wPosition ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;
   if ((NULL == npSliderInfo -> hColumnInfo) ||
       (wColumn >= (UINT) npSliderInfo -> nNumCols))
   {
      return ( (LRESULT) -1 ) ;
   }
   npColumnInfo = (NPCOLUMNINFO) ( npSliderInfo -> hColumnInfo ) ;
   wPosition = (npColumnInfo + wColumn) -> nColPos ;
   return ( (LRESULT) (LONG) wPosition ) ;

} //*** GetColumnPosition

//************************************************************************
//  GetColumnAlign
//
//  Description:
//     Returns the current alignment for the column specified by
//     wColumn.  Returns -1 if not successful.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     UINT wColumn
//        referenced column 
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR GetColumnAlign( HWND hWnd, UINT wColumn )
{
   HSLIDERINFO   hSliderInfo ;
   NPCOLUMNINFO  npColumnInfo ;
   NPSLIDERINFO  npSliderInfo ;
   UINT          wAlignment ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;
   if ((NULL == npSliderInfo -> hColumnInfo) ||
       (wColumn >= (UINT) npSliderInfo -> nNumCols))
   {
      return ( (LRESULT) -1 ) ;
   }
   npColumnInfo = (NPCOLUMNINFO) ( npSliderInfo -> hColumnInfo ) ;
   wAlignment = (npColumnInfo + wColumn) -> wAlignment ;
   return ( (LRESULT) (LONG) wAlignment ) ;

} //*** GetColumnAlign

//************************************************************************
//  EraseBackground
//
//  Description:
//     Erases the background (prepares the region for painting.)
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     HDC hDC
//        handle to screen DC
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR EraseBackground( HWND hWnd, HDC hDC )
{
   HBRUSH        hbrBG, hbrOld ;
   HSLIDERINFO   hSliderInfo ;
   NPSLIDERINFO  npSliderInfo ;
   RECT          rcClient ;
   LRESULT       lResult ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;

   // get client rect

   GetClientRect( hWnd, &rcClient ) ;

   // create the brush

   hbrBG = CreateSolidBrush( npSliderInfo -> rgbBG ) ;

   // set class word and allow DefWindowProc() to process

   hbrOld = (HBRUSH) SetClassWord( hWnd, GCW_HBRBACKGROUND, (UINT) hbrBG ) ;
   lResult = DefWindowProc( hWnd, WM_ERASEBKGND, (WPARAM) hDC, NULL ) ;

   // restore old brush handle and delete the one we created

   SetClassWord( hWnd, GCW_HBRBACKGROUND, (UINT) hbrOld ) ;
   DeleteObject( hbrBG ) ;

   // draw 3D frame

   tdDraw3DRect( hDC, &rcClient, SHADOWWIDTH,
               (npSliderInfo -> f3DOut) ? DRAW3D_OUT : DRAW3D_IN ) ;


   return ( lResult ) ;

} //*** EraseBackground

//************************************************************************
//  SetFont
//
//  Description:
//     Handles the WM_SETFONT message from the parent.
//
//  Parameters:
//     HWND hWnd
//        handle to slider control
//
//     HFONT hFont
//        handle to selected font
//
//     LONG lParam
//       TRUE if force immediate redraw
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR SetFont( HWND hWnd, HFONT hFont, LONG lParam )
{
   HSLIDERINFO   hSliderInfo ;
   NPSLIDERINFO  npSliderInfo ;
   TEXTMETRIC    tm ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   if (NULL == (npSliderInfo = (NPSLIDERINFO) ( hSliderInfo )))
      return ( FALSE ) ;             

   npSliderInfo -> hFont = hFont ;
   SelectObject( npSliderInfo -> hScreenDC, hFont ) ;
   GetTextMetrics( npSliderInfo -> hScreenDC, &tm ) ;
   npSliderInfo -> nFontHeight = tm.tmHeight + tm.tmExternalLeading ;


   // check for redraw and make it so

   if (lParam > 0)
   {
      DP1( hWDB, "SetFont" ) ;
      InvalidateRect( hWnd, NULL, TRUE ) ;
      UpdateWindow( hWnd ) ;
   }

   return ( TRUE ) ;

} //*** SetFont

//************************************************************************
//  PaintSlider
//
//  Description:
//     Handles the painting of the slider control.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR PaintSlider( HWND hWnd )
{
   int           i, nAlignHorz, nAlignVert, nColumnWidth,
                 nClientWidth, nClientHeight ;
   HDC           hDC ;
   HSLIDERINFO   hSliderInfo ;
   NPCOLUMNINFO  npColumnInfo ;
   NPSLIDERINFO  npSliderInfo ;
   NPSTR         npText ;
   PAINTSTRUCT   ps ;
   RECT          rcClient, rcColumn ;
   UINT          wTextExtent ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;

   // get DC handle

   hDC = npSliderInfo -> hScreenDC ;

   // initiate painting sequence

   GetClientRect( hWnd, &rcClient ) ;
   BeginPaint( hWnd, &ps ) ;

   if (0 < npSliderInfo -> nNumCols)
   {
      // draw 3D frame
   
      tdDraw3DRect( hDC, &rcClient, SHADOWWIDTH,
                  (npSliderInfo -> f3DOut) ? DRAW3D_OUT : DRAW3D_IN ) ;

      // get column info pointer
   
      npColumnInfo = (NPCOLUMNINFO) ( npSliderInfo -> hColumnInfo ) ;
      rcColumn.top = rcClient.top + SHADOWWIDTH ;
      rcColumn.bottom = rcClient.bottom - SHADOWWIDTH ;
      nClientWidth = rcClient.right - rcClient.left ;
      nClientHeight = rcClient.bottom - rcClient.top ;
      nAlignVert = (nClientHeight - npSliderInfo -> nFontHeight) / 2 ;
      nAlignVert = max( 0, nAlignVert - 1 ) ;
      for (i = 0; i < npSliderInfo -> nNumCols; i++)
      {
         if ((NULL != (npColumnInfo + i) -> hColHeader) &&
             ((npColumnInfo + i) -> nColPos < 100))
         {
            rcColumn.left = rcClient.left +
                            LOWORD( ((LONG) nClientWidth *
                                     (LONG) (npColumnInfo + i) -> nColPos /
                                     100L) ) + SHADOWWIDTH ;
            if (i == npSliderInfo -> nNumCols - 1)
               rcColumn.right = rcClient.right - SHADOWWIDTH ;
            else
               rcColumn.right = rcClient.left +
                                LOWORD( (LONG) nClientWidth *
                                        (LONG) (npColumnInfo + i + 1) -> nColPos /
                                        100L ) - SHADOWWIDTH + 1 ;
            nColumnWidth = rcColumn.right - rcColumn.left ;
            npText = (NPSTR) ( (npColumnInfo + i) -> hColHeader ) ;

            wTextExtent =
               LOWORD( GetTextExtent( hDC, npText, lstrlen( npText ) ) ) ;
            switch ((npColumnInfo + i) -> wAlignment)
            {
               case SA_RIGHT:
                  nAlignHorz = (nColumnWidth - wTextExtent) ;
                  break ;

               case SA_CENTER:
                  nAlignHorz = nColumnWidth - wTextExtent ;
                  if (SA_CENTER == (npColumnInfo + i) -> wAlignment)
                     nAlignHorz /= (int) 2 ;
                  break ;
   
               default:
                  nAlignHorz = SHADOWWIDTH + 3;
            }
            nAlignHorz = max( 0, nAlignHorz - 1 ) ;

            // blast text out

            ExtTextOut( hDC, rcColumn.left + nAlignHorz,
                        rcColumn.top + nAlignVert, ETO_OPAQUE | ETO_CLIPPED,
                        &rcColumn, npText, lstrlen( npText ), NULL ) ;


            // draw column seperator

            if (i != npSliderInfo -> nNumCols - 1)
            {
               tdDraw3DLine( hDC, rcColumn.right + SHADOWWIDTH - 1, rcClient.top,
                           nClientHeight, SHADOWWIDTH,
                           DRAW3D_RIGHTLINE |
                           ((npSliderInfo -> f3DOut) ? DRAW3D_OUT : DRAW3D_IN) ) ;
               tdDraw3DLine( hDC, rcColumn.right + SHADOWWIDTH - 1, rcClient.top,
                           nClientHeight, SHADOWWIDTH,
                           DRAW3D_LEFTLINE |
                           ((npSliderInfo -> f3DOut) ? DRAW3D_OUT : DRAW3D_IN) ) ;
            }
         }
      }
   }


   EndPaint( hWnd, &ps ) ;

   return ( (LRESULT) TRUE ) ;

} //*** PaintSlider

//************************************************************************
//  MouseMove
//
//  Description:
//     This handles the mouse movements in the slider control.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     UINT wFlags
//        flags associated with mouse move
//
//     LONG lPosition
//        position information
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR MouseMove( HWND hWnd, UINT wFlags, LONG lPosition )
{
   int           i, nClientWidth, nColPos, nMaxCol, nMoveDelta,
                 nOldSliderPos, nXPos ;
   BOOL          fCursorSet ;
   HSLIDERINFO   hSliderInfo ;
   NPCOLUMNINFO  npColumnInfo ;
   NPSLIDERINFO  npSliderInfo ;
   RECT          rcClient ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;
   if ((NULL == npSliderInfo -> hColumnInfo) ||
       (0 == npSliderInfo -> nNumCols))
   {
      return ( DefWindowProc( hWnd, WM_MOUSEMOVE, (WPARAM) wFlags,
                              (LPARAM) lPosition ) ) ;
   }
   if (npSliderInfo -> fDragging)
   {
      nXPos = LOWORD( lPosition ) + 1 ;
      GetClientRect( hWnd, &rcClient ) ;
      nClientWidth = rcClient.right - rcClient.left ;
      npColumnInfo = (NPCOLUMNINFO) ( npSliderInfo -> hColumnInfo ) ;
      nMoveDelta = LOWORD( ((LONG) nXPos * 100L / (LONG) nClientWidth) ) -
                   (npColumnInfo + npSliderInfo -> nDragCol) -> nColPos ;

      // check to see if we're bumping against the previous column

      if (nMoveDelta < 0 &&
          (npSliderInfo -> nDragCol > 0))
      {
         if ((npColumnInfo + npSliderInfo -> nDragCol) -> nColPos + nMoveDelta <=
             (npColumnInfo + npSliderInfo -> nDragCol - 1) -> nColPos)
            nMoveDelta = (npColumnInfo + npSliderInfo -> nDragCol - 1) -> nColPos -
                         (npColumnInfo + npSliderInfo -> nDragCol) -> nColPos ;
      }

      // check to see if we're bumping against the next column if shifted

      if (nMoveDelta > 0 &&
          (npSliderInfo -> nDragCol < npSliderInfo -> nNumCols - 1) &&
          (npSliderInfo -> fShifted))
      {
         if ((npColumnInfo + npSliderInfo -> nDragCol) -> nColPos + nMoveDelta >=
             (npColumnInfo + npSliderInfo -> nDragCol + 1) -> nColPos)
            nMoveDelta = (npColumnInfo + npSliderInfo -> nDragCol + 1) -> nColPos -
                         (npColumnInfo + npSliderInfo -> nDragCol) -> nColPos ;
      }

      // update column positions

      if (nMoveDelta != 0)
      {
         nOldSliderPos = (npColumnInfo + npSliderInfo -> nDragCol) -> nColPos ;
         nMaxCol = (npSliderInfo -> fShifted) ? npSliderInfo -> nDragCol + 1 :
                                                npSliderInfo -> nNumCols ;
         for (i = npSliderInfo -> nDragCol; i < nMaxCol; i++)
            (npColumnInfo + i) -> nColPos += nMoveDelta ;
         UpdateSliderPos( npSliderInfo -> hScreenDC, &rcClient, nOldSliderPos,
                          (npColumnInfo + npSliderInfo -> nDragCol) -> nColPos ) ;
      }
      if (npSliderInfo -> hNotifyWnd)
          SendMessage( npSliderInfo -> hNotifyWnd, WM_COMMAND, 
                    GetWindowWord(hWnd,GWW_ID), MAKELONG(hWnd,SLN_INDRAG));
      else
          SendMessage( GetParent(hWnd), WM_COMMAND, 
                    GetWindowWord(hWnd,GWW_ID), MAKELONG(hWnd,SLN_INDRAG));
   }
   else
   {
      nXPos = LOWORD( lPosition ) + 1 ;
      GetClientRect( hWnd, &rcClient ) ;
      nClientWidth = rcClient.right - rcClient.left ;
      npColumnInfo = (NPCOLUMNINFO) ( npSliderInfo -> hColumnInfo ) ;
      fCursorSet = FALSE ;

      // If the cursor is placed on top of a column break, change
      // the cursor to reflect that we can handle column position
      // changes.

      // NOTE:  We start looking at column 2 since we want
      // the left most column to always remain at position 0.

      for (i = 1; i < npSliderInfo -> nNumCols; i++)
      {
         nColPos = LOWORD( (LONG) (npColumnInfo + i) -> nColPos *
                            (LONG) nClientWidth / 100L ) ;
         if ((nXPos >= nColPos - SHADOWWIDTH ) &&
             (nXPos <= nColPos + SHADOWWIDTH ))
         {
            SetCursor( npSliderInfo -> hSplitCursor ) ;
            fCursorSet = TRUE ;
            break ;
         }
      }
      if (!fCursorSet)
         SetCursor( LoadCursor( NULL, IDC_ARROW ) ) ;
   }
   return ( (LRESULT) TRUE ) ;

} //*** MouseMove

//************************************************************************
//  MouseLButtonDown
//
//  Description:
//     Processes the WM_LBUTTONDOWN message.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     UINT wFlags
//        mouse info flags
//
//     LONG lPosition
//        position information
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR MouseLButtonDown( HWND hWnd, UINT wFlags, LONG lPosition )
{
   int           i, nColPos, nClientWidth, nXPos ;
   HSLIDERINFO   hSliderInfo ;
   NPCOLUMNINFO  npColumnInfo ;
   NPSLIDERINFO  npSliderInfo ;
   POINT         pt ;
   RECT          rcClient, rcClipCursor ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;
   if ((NULL == npSliderInfo -> hColumnInfo) ||
       (0 == npSliderInfo -> nNumCols))
   {
      return ( DefWindowProc( hWnd, WM_LBUTTONDOWN,
                              (WPARAM) wFlags, (LPARAM) lPosition ) ) ;
   }
   nXPos = LOWORD( lPosition ) + 1 ;
   GetClientRect( hWnd, &rcClient ) ;
   nClientWidth = rcClient.right - rcClient.left ;
   npColumnInfo = (NPCOLUMNINFO) ( npSliderInfo -> hColumnInfo ) ;

   // Search for the column break to be dragged.

   // NOTE:  We start looking at the end column and move left
   // to column 2.  This helps with moving stacked columns
   // and ensures that the left most column will always remain
   // at position 0.

   for (i = npSliderInfo -> nNumCols - 1; i > 0; i--)
   {
      nColPos = LOWORD( (LONG) (npColumnInfo + i) -> nColPos *
                        (LONG) nClientWidth / 100L ) ;
      if ((nXPos >= nColPos - SHADOWWIDTH ) &&
          (nXPos <= nColPos + SHADOWWIDTH ))
         break ;
   }
   if (i != 0)
   {
      npSliderInfo -> fDragging = TRUE ;
      npSliderInfo -> fShifted = (wFlags & MK_SHIFT) > 0 ;
      npSliderInfo -> nDragCol = i ;
      SetCursor( npSliderInfo -> hSplitCursor ) ;
      pt.x = rcClient.left ;
      pt.y = rcClient.top ;
      ClientToScreen( hWnd, &pt ) ;
      rcClipCursor.left = pt.x ;
      rcClipCursor.top = pt.y ;
      pt.x = rcClient.right ;
      pt.y = rcClient.bottom ;
      ClientToScreen( hWnd, &pt ) ;
      rcClipCursor.right = pt.x ;
      rcClipCursor.bottom = pt.y ;
      ClipCursor( &rcClipCursor ) ;

      // show "slider"

      UpdateSliderPos( npSliderInfo -> hScreenDC, &rcClient, -1,
                       (npColumnInfo + npSliderInfo -> nDragCol) -> nColPos ) ;

      // send a notification to associated control
      if (npSliderInfo -> hNotifyWnd)
          SendMessage( npSliderInfo -> hNotifyWnd, WM_COMMAND, 
                    GetWindowWord(hWnd,GWW_ID), MAKELONG(hWnd,SLN_STARTDRAG));
      else
          SendMessage( GetParent(hWnd), WM_COMMAND, 
                    GetWindowWord(hWnd,GWW_ID), MAKELONG(hWnd,SLN_STARTDRAG));
   }
   else
      npSliderInfo -> fDragging = FALSE ;
   LocalUnlock( hSliderInfo ) ;
   return ( (LRESULT) TRUE ) ;

} //*** MouseLButtonDown

//************************************************************************
//  MouseLButtonUP
//
//  Description:
//     Handles the WM_LBUTTONUP message.
//
//  Parameters:
//     HWND hWnd
//        handle to slider window
//
//     UINT wFlags
//        mouse info flags
//
//     LONG lPosition
//        position information
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

LRESULT NEAR MouseLButtonUP( HWND hWnd, UINT wFlags, LONG lPosition )
{
   HSLIDERINFO   hSliderInfo ;
   NPSLIDERINFO  npSliderInfo ;
   NPCOLUMNINFO  npColumnInfo ;
   RECT          rcClient ;

   hSliderInfo = GETHSLIDERINFO( hWnd ) ;
   npSliderInfo = (NPSLIDERINFO) ( hSliderInfo ) ;
   if ((NULL == npSliderInfo -> hColumnInfo) ||
       (0 == npSliderInfo -> nNumCols))
   {
      return ( DefWindowProc( hWnd, WM_LBUTTONDOWN,
                              (WPARAM) wFlags, (LPARAM) lPosition ) ) ;
   }
   if (npSliderInfo -> fDragging)
   {
      SetCursor( LoadCursor( NULL, IDC_ARROW ) ) ;
      ClipCursor( NULL ) ;
      npSliderInfo -> fDragging = FALSE ;

      // remove "slider"

      GetClientRect( hWnd, &rcClient ) ;
      npColumnInfo = (NPCOLUMNINFO) ( npSliderInfo -> hColumnInfo ) ;
      UpdateSliderPos( npSliderInfo -> hScreenDC, &rcClient,
                       (npColumnInfo + npSliderInfo -> nDragCol) -> nColPos,
                       -1 ) ;

      // we need to update

      InvalidateRect( hWnd, NULL, FALSE ) ;

      // force update

      UpdateWindow( hWnd ) ;

      // send a notification to associated control

      if (npSliderInfo -> hNotifyWnd)
          SendMessage( npSliderInfo -> hNotifyWnd, WM_COMMAND, 
                    GetWindowWord(hWnd,GWW_ID), MAKELONG(hWnd,SLN_ENDDRAG));
      else
          SendMessage( GetParent(hWnd), WM_COMMAND, 
                    GetWindowWord(hWnd,GWW_ID), MAKELONG(hWnd,SLN_ENDDRAG));
   }
   return ( (LRESULT) TRUE ) ;

} //*** MouseLButtonUP

//************************************************************************
//  UpdateSliderPos
//
//  Description:
//     Updates the "slider" position.  The slider is simply an
//     inverted rectangle.
//
//  Parameters:
//     HDC hDC
//        screen device context
//
//     LPRECT lpRect
//        far pointer to slider client rect
//
//     int nOldPos
//        old position of slider, -1 if no old position
//
//     int wNewPos
//        new position of slider, -1 if no new position
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

VOID NEAR UpdateSliderPos( HDC hDC, LPRECT lpRect, int nOldPos, int nNewPos )
{
   RECT  rcInvert ;
   UINT  wClientWidth ;

   if (nOldPos != -1)
   {
      wClientWidth = lpRect -> right - lpRect -> left ;
      rcInvert.left = LOWORD( (LONG) nOldPos *
                              (LONG) wClientWidth / 100L ) ;
      rcInvert.right = rcInvert.left + SHADOWWIDTH ;
      rcInvert.top = lpRect -> top ;
      rcInvert.bottom = lpRect -> bottom ;

      InvertRect( hDC, &rcInvert ) ;
   }

   if (nNewPos != -1)
   {
      wClientWidth = lpRect -> right - lpRect -> left ;
      rcInvert.left = LOWORD( (LONG) nNewPos *
                              (LONG) wClientWidth / 100L ) ;
      rcInvert.right = rcInvert.left + SHADOWWIDTH ;
      rcInvert.top = lpRect -> top ;
      rcInvert.bottom = lpRect -> bottom ;

      InvertRect( hDC, &rcInvert ) ;
   }

} //*** UpdateSliderPos
#endif

//*** EOF: slider.c
