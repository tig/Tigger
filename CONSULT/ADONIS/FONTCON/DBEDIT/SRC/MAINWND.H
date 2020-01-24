//*************************************************************
//  File name: mainwnd.h
//
//  Description: 
//      Header file for the main window stuff
//
//  History:    Date       Author     Comment
//               2/13/92   MSM        Created
//
//*************************************************************

#ifndef _MAINWND_H_
#define _MAINWND_H_

#define SHADOW_THICKNESS    3

//*** Control IDs
    #define IDC_STATUS          0x0090
    #define IDC_TOOLBAR         0x0091
    #define IDC_FONTLIST        0x0092
    #define IDC_FONTVIEW        0x0093


//*** Function prototypes
    LRESULT CALLBACK MainWndProc(HWND,UINT,WPARAM,LPARAM);
    LRESULT WINAPI CmdHandler(WPARAM,LPARAM);
    LRESULT WINAPI SizeHandler(HWND,UINT,WPARAM,LPARAM);
    LRESULT WINAPI MenuHandler(UINT,WPARAM,LPARAM);

    VOID          SizeFontViewAndList(VOID);
    VOID          PaintMainWnd(VOID);

#endif

//*** EOF: mainwnd.h
