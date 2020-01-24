/************************************************************************
 *
 *     Project:  Windows sample applications
 *
 *      Module:  status.h
 *
 *     Remarks:  This include file is for the status window class
 *
 ***********************************************************************/

#ifndef _STATUS_H_
#define _STATUS_H_

#ifdef WIN32
#ifndef HINSTANCE
#define HINSTANCE HANDLE
#endif
#endif

/*
 * This is the "class name" for the status custom control.  It is
 * important to make your class names unique because there are 1000's
 * of other Windows programmers out there who might create a window
 * class with the same name as yours.
 */
#define SZ_STATUSCLASSNAME    "Status"


#define ST_GETSTATUSWIDTH (WM_USER + 0x01 + 0x7000)
#define ST_SETSTATBOXSIZE (WM_USER + 0x02 + 0x7000)
#define ST_SETSTATBOX     (WM_USER + 0x03 + 0x7000)
#define ST_SETBORDER      (WM_USER + 0x04 + 0x7000)
#define ST_SETJUSTIFY     (WM_USER + 0x05 + 0x7000)
#define ST_GETHEIGHT      (WM_USER + 0x06 + 0x7000)
#define ST_SETCOLOR       (WM_USER + 0x07 + 0x7000)
#define ST_SETDEFAULTTEXT (WM_USER + 0x08 + 0x7000)
#define ST_SETOWNERDRAW   (WM_USER + 0x09 + 0x7000)
#define ST_GETSTATBOXRECT (WM_USER + 0x0A + 0x7000)

/*
 * Onwer draw stuff
 */
#define ODT_STATUS   42

BOOL WINAPI StatusInit( HINSTANCE hPrev, HINSTANCE hInstance, LPSTR lpszClassName ) ;

//
// Macros for setting stat boxes up. ID's are always USHORT.
//
#define StatusSetStatBoxSize(hWnd,nID,nWidth) SendMessage(hWnd,ST_SETSTATBOXSIZE,(UINT)nID,(LPARAM)nWidth)
#define StatusSetStatBox(hWnd,nID,lpszBuf)    SendMessage(hWnd,ST_SETSTATBOX,(UINT)nID,(LPARAM)(LPSTR)lpszBuf) 
#define StatusGetWidth( hWnd, lpszText )      (int)SendMessage(hWnd,ST_GETSTATUSWIDTH,0,(LPARAM)(LPSTR)lpszText )


#define RGBBLACK     RGB(0,0,0)
#define RGBWHITE     RGB(255,255,255)
#define RGBGRAY      RGB(128,128,128)
#define RGBLTGRAY    RGB(192,192,192)

#define RGBBLUE      RGB(0,0,128)
#define RGBLTBLUE    RGB(0,0,255)
#define RGBGREEN     RGB(0,128,0)

#define RGBCYAN      RGB(0,128,128)
#define RGBLTGREEN   RGB(0,255,0)
#define RGBLTCYAN    RGB(0,255,255)

#define RGBRED       RGB(128,0,0)
#define RGBMAGENTA   RGB(128,0,128)
#define RGBBROWN     RGB(128,128,0)

#define RGBLTRED     RGB(255,0,0)
#define RGBLTMAGENTA RGB(255,0,255)
#define RGBYELLOW    RGB(255,255,0)

#ifdef SHADOWSTUFF

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

VOID WINAPI tdDrawFaceFrame( HDC hdc, LPRECT lprc, UINT wWidth );
VOID WINAPI tdDraw3DRect(    HDC hdc, LPRECT lprc, UINT wWidth, UINT wFlags );
VOID WINAPI tdDraw3DLine(    HDC hdc, UINT x, UINT y, UINT nLen, UINT wWidth, UINT wFlags );
BOOL WINAPI tdDrawDropShadow(HDC hDC, HWND hWnd, short nShadowWidth ) ;
VOID WINAPI tdDraw3DLine1( HDC hDC, UINT x, UINT y, UINT nLen, UINT wFlags ) ;

DWORD WINAPI tdGetHighlightColor( DWORD rgb ) ;
DWORD WINAPI tdGetShadowColor( DWORD rgb ) ;
DWORD WINAPI tdMapTextColor( DWORD rgb ) ;

#endif // _SHADOW_H_

#endif

#endif

/************************************************************************
 * End of File: status.h
 ***********************************************************************/

