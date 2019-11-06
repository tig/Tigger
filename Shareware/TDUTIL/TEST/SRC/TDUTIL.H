/************************************************************************
 *
 *     Project:  3D effect DLL
 *
 *               Copyright (c) 1991-1992, Charles E. Kindel, Jr.
 *               All Rights Reserved.
 *
 *      Module:  TDUTIL.H
 *
 *     Remarks:  Contains macros, prototypes, and external declarations
 *               for TDUTIL.DLL.
 *
 *               All TDUTIL APIs are prefixed with 'td' (Three Dee).
 *
 *   Revisions:  
 *     01.00.001  8/19/91 cek   First version of TDUTIL
 *     02.00.001  2/10/92 cek   "Strictified" and added tdShadowControl
 *     02.10.001  5/11/92 cek   Added 3d radio/check button support.
 *     02.11.001  5/11/92 cek   Cleaned up some more.
 *     02.12.001  5/22/92 cek   Added new custom controls.
 *
 ***********************************************************************/

#ifndef _TDUTIL_H_
#define _TDUTIL_H_

/*
 * The SPEC version can be used by an application to determine if the
 * installed version of this DLL is old or new.  The app should call
 * tdCheckVersion( _TDUTIL_SPEC_VER_ ).  If this function returns
 * FALSE then the DLL installed is old and should not be called.
 *
 * The version number is calculated with the following formula:
 *
 *  #define _TDUTIL_SPEC_VER_   (VER_MAJOR * 100)
 *  dwVersion   = MAKELONG( (_TDUTIL_SPEC_VER_) + (VER_MINOR), VER_BUILD ) ;
 *
 */
#define _TDUTIL_SPEC_VER_   ((WORD)200)

/* Custom control definitions:
 *
 */
#define SZ_TDUTILSTATIC    "TDUTIL.STATIC"

/* Window styles for the tdutil.static control
 */

/* Text Alignment */
#define TDSS_LEFT             0x00000001L
#define TDSS_CENTER           0x00000002L
#define TDSS_RIGHT            0x00000003L
#define TDSS_LEFTNOWORDWRAP   0x00000004L

/* 3D 'chisel' frame around control that matches CTLCOLOR_DLG */
#define TDSS_3DFRAME          0x00000010L

/* TDSS_3DFRAME modifiers */
#define TDSS_CREASE           0x00000020L    // crease effect
#define TDSS_DRAW3D_OUT       0x00000040L    // effect 'sticks out'

/* Text modifiers.  Modify appearance of text.  Mutually exclusive. */
#define TDSS_EDGETEXT         0x00000080L    // 'drop shadow' like effect
#define TDSS_INDENTTEXT       0x00000100L    // Text looks indented

/* Depth of frame effects */
#define TDSS_FRAME_1DEEP      0x00000200L    // Frame is 1 pixel deep
#define TDSS_FRAME_2DEEP      0x00000300L    // Frame is 2 pixels deep
#define TDSS_FRAME_3DEEP      0x00000400L    // Frame is 3 pixels deep
#define TDSS_FRAME_4DEEP      0x00000500L    // Frame is 4 pixels deep
#define TDSS_FRAME_5DEEP      0x00000600L    // Frame is 5 pixels deep
#define TDSS_FRAME_6DEEP      0x00000700L    // Frame is 6 pixels deep
#define TDSS_FRAME_7DEEP      0x00000800L    // Frame is 7 pixels deep

/* Depth of text effects */
#define TDSS_TEXT_1DEEP       0x00000900L    // Text effect is 1 pixel deep
#define TDSS_TEXT_2DEEP       0x00000A00L    // Text effect is 2 pixels deep
#define TDSS_TEXT_3DEEP       0x00000B00L    // Text effect is 3 pixels deep
#define TDSS_TEXT_4DEEP       0x00000C00L    // Text effect is 4 pixels deep
#define TDSS_TEXT_5DEEP       0x00000D00L    // Text effect is 5 pixels deep
#define TDSS_TEXT_6DEEP       0x00000E00L    // Text effect is 6 pixels deep
#define TDSS_TEXT_7DEEP       0x00000F00L    // Text effect is 7 pixels deep

#define TDSS_RESERVED         0x00008000L    // Reserved bit

/*
 * These defines are used in the uiFlags and uiStyle parameters to
 * the TDUTIL functions.
 */
#define DRAW3D_IN           0x0100     // effect looks indented
#define DRAW3D_OUT          0x0200     // effect 'sticks out'
#define DRAW3D_CREASE       0x0400     // for that 'creased' look

#define DRAW3D_TOPLINE      0x0800
#define DRAW3D_BOTTOMLINE   0x1000
#define DRAW3D_LEFTLINE     0x2000
#define DRAW3D_RIGHTLINE    0x4000

/*
 * Very useful set of macros for dealing with the 'base' 16 colors
 */
#ifndef _RGB_H_
#define _RGB_H_
   #define RGBBLACK     RGB(0,0,0)
   #define RGBRED       RGB(128,0,0)
   #define RGBGREEN     RGB(0,128,0)
   #define RGBBLUE      RGB(0,0,128)
   #define RGBBROWN     RGB(128,128,0)
   #define RGBMAGENTA   RGB(128,0,128)
   #define RGBCYAN      RGB(0,128,128)
   #define RGBLTGRAY    RGB(192,192,192)
   #define RGBGRAY      RGB(128,128,128)
   #define RGBLTRED     RGB(255,0,0)
   #define RGBLTGREEN   RGB(0,255,0)
   #define RGBLTBLUE    RGB(0,0,255)
   #define RGBYELLOW    RGB(255,255,0)
   #define RGBLTMAGENTA RGB(255,0,255)
   #define RGBLTCYAN    RGB(0,255,255)
   #define RGBWHITE     RGB(255,255,255)
#endif

/* tdGethInst()
 *
 * Returns the HMODULE of TDUTIL.DLL.
 */
HMODULE WINAPI tdGethInst( VOID ) ;

/* tdCheckVersion()
 *
 * Returns TRUE if dwVer is less than or equal to the spec version of
 * TDUTIL.  Otherwise it returns false.
 *
 * The dwVer value passed should be _TDUTIL_SPEC_VER_ defined above.
 *
 * The idea here is that TDUTIL.DLL will always retain backwards
 * compatibility. If this function returns FALSE, an *older* version
 * of TDUTIL is running than what the app expects.
 */
BOOL WINAPI tdCheckVersion( DWORD dwVer ) ;

/* tdGetVersion()
 *
 * Returns the version number of TDUTIL and copies a formatted
 * version string into the buffer pointed to by lpszVersion in the
 * form "%d.%02d[.%03d]"
 *
 * Examples:
 *
 *    If the version of TDUTIL is 1.16.256 the return value will
 *    be 0x00FF0074 and the string will be "1.26.256".
 *
 *    If the version of TDUTIL is 2.11.000 the return value will be
 *    0x000000D3 and the string will be "2.11".
 */
DWORD WINAPI tdGetVersion( LPSTR lpszVersion )  ;

/* tdShadowControl()
 *
 * TDUTIL can work in "autopilot" mode or manually.  The tdShadowControl()
 * function is the autopilot engine.  Sprinkle a few calls to
 * this function in your dialog and window procedures and you'll auto-
 * magically get cool 3d effects, including the new radiobuttons and
 * checkboxes!
 *
 * A sample DlgProc is provided below to show it's used:
 *
 * BOOL CALLBACK fnTest(  HWND hDlg, UINT uiMsg,
 *                        WPARAM wParam, LPARAM lParam )
 * {
 *    switch (uiMsg)
 *    {
 *       case WM_INITDIALOG:
 *          //
 *          // The ShadowControl is turned on by calling tdShadowControl
 *          // with the WM_INITDIALOG message.  The lParam is used to
 *          // specify the RGB value you want for the background of the
 *          // window.  If lParam == 0L, RGBLTGRAY is assumed.
 *          //
 *          // If you already subclass any of Windows' standard controls
 *          // in your WM_INITDIALOG case, you should call tdShadowControl
 *          // *first*.
 *          //
 *          // You can call tdShadowControl with WM_INITDIALOG at any
 *          // time to change the background color.
 *          //
 *          return tdShadowControl( hDlg, uiMsg, wParam, RGBLTGRAY ) ;
 *
 *       case WM_COMMAND:
 *          if (wParam == IDCANCEL)
 *          {
 *    	   	   EndDialog( hDlg, TRUE ) ;
 *   	   	   return TRUE ;
 * 	      }
 *	         break ;
 * 
 *       case WM_DESTROY:
 *          //
 *          // Always call tdShadowControl() with the WM_DESTROY message
 *          // if you've called it with the WM_INITDIALOG message.  It
 *          // uses this message to un-initialize itself and clean up.
 *          //
 *          return tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;
 * 
 *       case WM_CTLCOLOR:
 *          //
 *          // Call tdShadowControl() on WM_CTLCOLOR messages.
 *          //
 *          return tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;
 * 
 *       case WM_PAINT:
 *          //
 *          // Always call tdShadowControl() for WM_PAINT messags.  This
 *          // is very important.
 *          //
 *          return tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;
 * 
 *       default:
 *          //
 *          // Always call tdShadowControl() in the default case.
 *          // tdShadowControl() uses a registered window message to
 *          // do some trickery and the only way it'll ever get that message
 *          // is if you pass it on in the default case.
 *          //
 *          return tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;
 *    }
 *    return FALSE ;
 * } 
 *
 */
BOOL WINAPI tdShadowControl( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

/* tdDraw3DRect()
 * 
 * Draws a rectangle (non-filled) that has 3D effects.
 *
 *    lprc     Defines the rectangle in client coordinates
 *    uiWidth  Specifies the width of the lines that make up the rectangle.
 *    wFlags   Either DRAW3D_IN or DRAW3D_OUT.  May be combined with
 *             DRAW3D_CREASED (uiWidth is ignored).
 */
VOID WINAPI tdDraw3DRect( HDC hdc, LPRECT lprc, UINT uiWidth, UINT wFlags );

/* tdDraw3DLine()
 *
 * Draws a component of a 3D rectangle.  The tdDraw3DRect() function uses
 * this function to draw the rectangles.  This function is optimized
 * for speed.  For one pixel wide lines it uses ExtTextOut(), for others
 * it use Polygon() ('cause line ends are not square).
 *
 *    x, y     Defines the line in client coordinates
 *    uiWidth   Specifies the width of the lines that make up the rectangle.
 *    wFlags   May be a combination of either DRAW3D_IN or DRAW3D_OUT and
 *             DRAW3D_TOP, DRAW3D_LEFT, DRAW3D_BOTTOM, or DRAW3D_RIGHT.
 */
VOID WINAPI tdDraw3DLine( HDC hdc, UINT x, UINT y, UINT nLen, UINT uiWidth, UINT wFlags );

/*
 *  This function draws shadows along the bottom and right sides
 *  of a window.
 *
 *  hDC              Device context of parent window previously obtained.
 *  hWnd             Handle to the window or dialog control
 *                   that is to be shadowed.
 *  ShadouiWidth      The width, in device units of the shadow.
 *
 *    This function should draw a shadow around any window, whether
 *    it is a dialog control or not.
 */
BOOL WINAPI tdDrawDropShadow(HDC hDC, HWND hWnd, UINT uiShadouiWidth ) ;

/* tdDrawFaceFrame()
 *
 * Draws a rectangle using the current background color.  This function
 * is useful for drawing fast rectangles (it' uses ExtTextOut() for speed).
 *
 * Restrictions are that it does not do patterned lines, or fill the
 * rectangle.  But it's a hell of a lot faster than Rectangle().
 *
 * tdDrawRect() is really a macro that calls tdDrawFaceFrame() which
 * is mis-named (blame curtisp).
 */
#define tdDrawRect tdDrawFaceFrame
VOID WINAPI tdDrawFaceFrame( HDC hdc, LPRECT lprc, UINT uiWidth );


/* tdDraw3DCrease()
 * 
 * Draws a 3D 'crease' effect using the specified rectangle.  This
 * effect is often used for 3d group boxes.  uiFlags dictates whether
 * the crease is indented (DRAW3D_IN) or sticks out (DRAW_3DOUT).
 */
VOID WINAPI tdDraw3DCrease( HDC hdc, LPRECT lprc, UINT uiFlags ) ;

/* tdGetHighlightColor()
 *
 * TDUTIL uses the concept of a normal color, highlight color, and shadow
 * color to do it's drawing.  Both the highlight color and shadow colors
 * are derived from the normal color.  This function takes a 'normal'
 * color, and returns the appropriate highlight color.
 *
 * For example if RGBLTGRAY is passed in as rgb, RGBWHITE will be returned.
 */
COLORREF WINAPI tdGetHighlightColor( COLORREF rgb ) ;

/* tdGetShadowColor()
 *
 * TDUTIL uses the concept of a normal color, highlight color, and shadow
 * color to do it's drawing.  Both the highlight color and shadow colors
 * are derived from the normal color.  This function takes a 'normal'
 * color, and returns the appropriate shadow color.
 *
 * For example if RGBLTGRAY is passed in as rgb, RGBGRAY will be returned.
 */
COLORREF WINAPI tdGetShadowColor( COLORREF rgb ) ;

/**********************************************************************
 * Less useful and oddball functions that exist only for reasons
 * of backwards compatibility.
 **********************************************************************/

/* tdExtTextOut3D()
 *
 * Just like ExtTextOut() with ETO_OPAQUE, but borders the clipping
 * rect with a 3d shadow.   You can combine DRAW3D_IN or DRAW3D_OUT
 * with the normal ETO_* options in uiOptions.
 */
BOOL WINAPI tdExtTextOut3D(  HDC hDC, int x, int y, UINT uiOptions,
                                 LPRECT lpRect, LPSTR lpString,
                                 int nChars, LPINT lpDx, UINT uiWidth) ;

/* tdTextOut3D()
 *
 * Just like TextOut() with but borders the text with a 3d shadow.
 * uiStyle can be DRAW3D_IN or DRAW3D_OUT
 */
BOOL     WINAPI tdTextOut3D( HDC hDC, int xPos, int yPos, LPSTR lpsz,
                           int nCount, UINT uiWidth, UINT uiStyle ) ;

/* tdGetChildRect()
 *
 * Useful little api for obtaining the window rectangle of a child
 * window in the parent window's client coordinates.
 */
VOID WINAPI tdGetChildRect( HWND hwndChild, LPRECT lprc ) ;

/* tdDrawChildren3D()
 *
 * Goes through each child window and draws an appropriate shadow
 * effect around it using the tdDrawChild3DRect() function.
 *
 * The hDC parameter should be retrieved by a call to GetDC() duing
 * a message that was posted to the parent in the WM_PAINT message.
 * In otherwords don't try to use this function in the WM_PAINT
 * case directly.
 */
BOOL WINAPI tdDrawChildren3D( HDC hDC, HWND hWnd ) ;

/* tdDrawChild3DRect()
 *
 * Draws the appropriate shadow effect around a child window.  The hDC
 * should be retrieved by a call to GetDC() duing a message that
 * was posted to the parent in the WM_PAINT message.  In otherwords
 * don't try to use this function in the WM_PAINT case directly.
 */
VOID WINAPI tdDrawChild3DRect( HDC hDC, HWND hwndChild, UINT uiWidth, UINT wFlags ) ;

/* tdMapTextColor()
 *
 * This function attempts to return an appropriate foreground text
 * color given a 'normal' color.  Unfortunately it doesn't do a very
 * good job so it's probably not very useful.
 */
COLORREF WINAPI tdMapTextColor( COLORREF rgb ) ;

#endif // _TDUTIL_H_

/************************************************************************
 * End of File: TDUTIL.h
 ***********************************************************************/

