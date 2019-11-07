/************************************************************************
 *
 *     Project:  3D effect DLL
 *
 *      Module:  3dutil.h
 *
 *     Remarks:  Contains macros, prototypes, and external declarations
 *               for 3DUTIL.DLL.
 *
 *               All 3DUTIL APIs are prefixed with 'td' (Three Dee).
 *
 *   Revisions:  
 *
 ***********************************************************************/

#ifndef _3DUTIL_H_
#define _3DUTIL_H_

#ifndef _RGB_H_
#include <rgb.h>
#endif

/*
 * The SPEC version can be used by an application to determine if the
 * installed version of this DLL is old or new.  The app should call
 * tdCheckVersion( _3DUTIL_SPEC_VER_ ).  If this function returns
 * FALSE then the DLL installed is old and should not be called.
 */
#define _3DUTIL_SPEC_VER_   ((WORD)200)

/* misc. control flag defines */
#define DRAW3D_IN           0x0100
#define DRAW3D_OUT          0x0200

#define DRAW3D_TOPLINE      0x0400
#define DRAW3D_BOTTOMLINE   0x0800
#define DRAW3D_LEFTLINE     0x1000
#define DRAW3D_RIGHTLINE    0x2000

// Windows.h defines these:
//#define ETO_GRAYED          0x0001
//#define ETO_OPAQUE          0x0002
//#define ETO_CLIPPED         0x0004

/* public function prototypes */

BOOL WINAPI tdCheckVersion( DWORD dwVer ) ;
DWORD WINAPI tdGetVersion( LPSTR lpszVersion )  ;
HINSTANCE WINAPI tdGethInst( VOID ) ;

WORD WINAPI tdChildKind( HWND hwndChild ) ;
#define          CHILD_UNKNOWN         0
#define          CHILD_BUTTON          1
#define          CHILD_COMBO           2
#define          CHILD_DROPDOWNCOMBO   3
#define          CHILD_EDIT            4
#define          CHILD_LISTBOX         5
#define          CHILD_SCROLLBAR       6
#define          CHILD_TEXT            7
#define          CHILD_BLACKFRAME      8
#define          CHILD_BLACKRECT       9
#define          CHILD_GRAYFRAME       10
#define          CHILD_GRAYRECT        11
#define          CHILD_ICON            12
#define          CHILD_WHITEFRAME      13
#define          CHILD_WHITERECT       14
#define          CHILD_GROUPBOX        15

VOID WINAPI tdDrawFaceFrame( HDC hdc, LPRECT lprc, WORD wWidth );
VOID WINAPI tdDraw3DRect(    HDC hdc, LPRECT lprc, WORD wWidth, WORD wFlags );
VOID WINAPI tdDraw3DLine(    HDC hdc, WORD x, WORD y, WORD nLen, WORD wWidth, WORD wFlags );
BOOL WINAPI tdDrawDropShadow(HDC hDC, HWND hWnd, short nShadowWidth ) ;
BOOL WINAPI tdExtTextOut3D(  HDC hDC, short x, short y, WORD wOptions,
                                 LPRECT lpRect, LPSTR lpString,
                                 short nChars, LPINT lpDx, WORD wWidth) ;

BOOL WINAPI tdTextOut3D( HDC hDC, short xPos, short yPos, LPSTR lpsz,
                           short nCount, WORD wWidth, WORD wStyle ) ;
VOID WINAPI tdDraw3DLine1( HDC hDC, WORD x, WORD y, WORD nLen, WORD wFlags ) ;

BOOL WINAPI tdShadowControl( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam ) ;
VOID WINAPI tdGetChildRect( HWND hwndChild, LPRECT lprc ) ;
VOID WINAPI tdDrawChild3DRect( HDC hDC, HWND hwndChild, WORD wWidth, WORD wFlags ) ;
BOOL WINAPI tdDrawChildren3D( HDC hDC, HWND hWnd ) ;
BOOL WINAPI tdHideSomeChildren( HWND hWnd ) ;

DWORD WINAPI tdGetHighlightColor( DWORD rgb ) ;
DWORD WINAPI tdGetShadowColor( DWORD rgb ) ;
DWORD WINAPI tdMapTextColor( DWORD rgb ) ;

#endif // _3DUTIL_H_

/************************************************************************
 * End of File: 3dutil.h
 ***********************************************************************/

