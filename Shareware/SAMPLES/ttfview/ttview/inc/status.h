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

/*
 * This is the "class name" for the status custom control.  It is
 * important to make your class names unique because there are 1000's
 * of other Windows programmers out there who might create a window
 * class with the same name as yours.
 */
#define SZ_STATUSCLASSNAME    "TTVIEW.Status"


#define ST_GETSTATUSWIDTH (WM_USER + 1 + 0x7000)
#define ST_SETSTATBOXSIZE (WM_USER + 2 + 0x7000)
#define ST_SETSTATBOX     (WM_USER + 3 + 0x7000)
#define ST_SETBORDER      (WM_USER + 4 + 0x7000)
#define ST_SETJUSTIFY     (WM_USER + 5 + 0x7000)
#define ST_GETHEIGHT      (WM_USER + 6 + 0x7000)
#define ST_SETCOLOR       (WM_USER + 7 + 0x7000)
#define ST_SETDEFAULTTEXT (WM_USER + 8 + 0x7000)

BOOL WINAPI StatusInit( HINSTANCE hPrev, HINSTANCE hInstance ) ;
int WINAPI StatusGetWidth( HWND hMsgWnd, LPSTR lpszText ) ;

//
// Macros for setting stat boxes up. ID's are always USHORT.
//
#define StatusSetStatBoxSize(hWnd,nID,nWidth) SendMessage(hWnd,ST_SETSTATBOXSIZE,(UINT)nID,(LPARAM)nWidth)
#define StatusSetStatBox(hWnd,nID,lpszBuf)    SendMessage(hWnd,ST_SETSTATBOX,(UINT)nID,(LPARAM)(LPSTR)lpszBuf) 


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

#endif

/************************************************************************
 * End of File: status.h
 ***********************************************************************/

