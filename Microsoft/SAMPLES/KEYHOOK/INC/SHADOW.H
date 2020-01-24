/************************************************************************
 *
 *     Project:  Windows Sample Applications
 *
 *      Module:  shadow.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#ifndef _SHADOW_H_
#define _SHADOW_H_

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

WORD FAR PASCAL ChildKind( HWND hwndChild ) ;
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


#ifndef RGBBLACK

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

/* public function prototypes */
void FAR PASCAL DrawFaceFrame( HDC, LPRECT, WORD );
void FAR PASCAL Draw3DRect( HDC, LPRECT, WORD, WORD );
void FAR PASCAL Draw3DLine( HDC, WORD, WORD, WORD, WORD, WORD );
BOOL FAR PASCAL DrawDropShadow( HDC hDC, HWND hWnd, HBRUSH hBrush, 
                                short ShadowWidth ) ;
BOOL FAR PASCAL
ExtTextOut3D( HDC hDC, short x, short y, WORD wOptions,
              LPRECT lpRect, LPSTR lpString,
              short nChars, LPINT lpDx, WORD wWidth) ;

BOOL FAR PASCAL
TextOut3D( HDC hDC, short xPos, short yPos, LPSTR lpsz,
           short nCount, WORD wWidth, WORD wStyle ) ;
void FAR PASCAL
Draw3DLine1( HDC hDC, WORD x, WORD y, WORD nLen, WORD wFlags ) ;

BOOL FAR PASCAL
  ShadowControl( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam ) ;
void FAR PASCAL GetChildRect( HWND hwndChild, LPRECT lprc ) ;
void FAR PASCAL DrawChild3DRect( HDC hDC, HWND hwndChild, WORD wWidth, WORD wFlags ) ;
BOOL FAR PASCAL DrawChildren3D( HDC hDC, HWND hWnd ) ;
BOOL FAR PASCAL HideSomeChildren( HWND hWnd ) ;

DWORD FAR PASCAL GetHighlightColor( DWORD rgb ) ;
DWORD FAR PASCAL GetShadowColor( DWORD rgb ) ;
DWORD FAR PASCAL MapTextColor( DWORD rgb ) ;

#endif

/************************************************************************
 * End of File: shadow.h
 ***********************************************************************/

