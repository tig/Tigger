//*************************************************************
//  File name: global.h
//
//  Description: 
//      Global include file for #defines and prototypes
//
//  History:    Date       Author     Comment
//              12/31/91   MSM        Created
//                 
//*************************************************************

#define STRICT
#define WIN31
#define WINVER 0x030a

#include <windows.h>
#include <windowsx.h>

//*** Function Prototypes
//*** UpShop.cpp
    int PASCAL WinMain (HINSTANCE, HINSTANCE, LPSTR, int);
    LRESULT CALLBACK MainWndProc (HWND, unsigned, WORD, LONG);
    BOOL InitApplication (HINSTANCE hInst, HINSTANCE hPrevInst);
    BOOL InitInstance (HINSTANCE hInstance, int nCmdShow);

//*** EOF: global.h
