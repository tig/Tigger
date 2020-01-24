//*************************************************************
//  File name: toolbar.h
//
//  Description: 
//      Header file for the toolbar control
//
//  History:    Date       Author     Comment
//               4/21/92   MSM        Created
//
//*************************************************************

#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

#include <windows.h>
#include <windowsx.h>

BOOL WINAPI RegisterToolbarClass( HINSTANCE );

#define SZ_TOOLBARCLASS "QUICK.TOOLBAR"

//*** Messages
        // lParam -> LPDIMENS
    #define TBM_SETDIMENS           (WM_USER+98)

        // lParam -> LPDIMENS
    #define TBM_GETDIMENS           (WM_USER+99)

        // lParam -> LPBTNINFO, NULL for SPACER
    #define TBM_ADDBUTTON           (WM_USER+100)

        // wParam -> wType (1-BTNID,2-INDEX), lParam==ID | Index
    #define TBM_REMOVEBUTTON        (WM_USER+101)

        // wParam==ID, lParam == 0-UP, 1-DOWN
    #define TBM_SETBTNSTATE         (WM_USER+102)

        // wParam==ID, lReturn == 0-UP, 1-DOWN
    #define TBM_GETBTNSTATE         (WM_USER+103)

        // wParam==ID, lParam == TRUE/FALSE
    #define TBM_ENABLEBTN           (WM_USER+104)

        // wParam==ID, lParam == lData, lReturn = lOldData
    #define TBM_SETBTNDATA          (WM_USER+105)

        // wParam==ID, lReturn == lData             
    #define TBM_GETBTNDATA          (WM_USER+106)

        // wParam==ID, lParam == lpText 
    #define TBM_SETBTNTEXT          (WM_USER+107)

        // wParam==ID, lReturn == nLen
    #define TBM_GETBTNTEXTLENGTH    (WM_USER+108)

        // wParam==ID, lParam == lBuffer
    #define TBM_GETBTNTEXT          (WM_USER+109)

    typedef struct
    {
        int     nSpacerX;
        int     nButtonX;
        int     nButtonY;
    } DIMENS, FAR *LPDIMENS;

//*** Button stuff
    typedef struct
    {
        UINT    uiBtnID;        // 0 is a SPACER!
        UINT    uiBtnStyle;
        UINT    uiBtnState;
        HBITMAP hBitmap;        
        int     nBmpX;          // Is width of space if space, 0 use def
        int     nBmpY;
        DWORD   dwData;
    } BTNINFO, FAR *LPBTNINFO;

    typedef struct
    {
        UINT    uiBtnID;
        UINT    uiBtnStyle;
        UINT    uiBtnState;
        HFONT   hFont;          // If NULL use the window's font
        int     nWidth;
        int     nJustify;
        LPSTR   lpText;
    } BTNTEXT, FAR *LPBTNTEXT;


    #define BTNS_TOGGLE     0x0001
    #define BTNS_TOGGLEEX   0x0002
    #define BTNS_MENUID     0x0004
    #define BTNS_RIGHTBTN   0x8000
    #define BTNS_TEXT       0x0008
    #define BTNS_TEXTBORDER 0x0010

    #define BTNJ_LEFT       0x0000
    #define BTNJ_CENTER     0x0001
    #define BTNJ_RIGHT      0x0002

    #define BTN_UP          0x0000
    #define BTN_DOWN        0x0001
    #define BTN_ENABLED     0x0000
    #define BTN_DISABLED    0x0002

//*** hBitmap should look like.  nBmpX & Y are offsets into the bitmap
//        nBmpX
//  nBmpY o---------o----------o-------------o
//        | Up AND  | Down AND | Disable AND |
//        o---------o---------o-------------o
//        | Up OR   | Down OR  | Disabled OR |
//        o---------o----------o-------------o
//****

#endif // _TOOLBAR_H_

//*** EOF: toolbar.h
