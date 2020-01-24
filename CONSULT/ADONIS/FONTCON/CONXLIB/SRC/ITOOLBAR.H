//*************************************************************
//  File name: itoolbar.h
//
//  Description: 
//      Header file for the toolbar control
//
//  History:    Date       Author     Comment
//               4/21/92   MSM        Created
//
//*************************************************************

#ifndef _ITOOLBAR_H_
#define _ITOOLBAR_H_

#include "toolbar.h"
#include "..\..\inc\fsmemman.h"
#include "..\..\inc\struct.h"   // must be included first
#include "..\..\inc\CONXLIB.h"

LRESULT CALLBACK ToolbarWndProc( HWND, WORD, WPARAM, LPARAM );

typedef struct
{
    DIMENS      Dimens;
    int         nPaintY;
    HLIST       hlstBtns;   // List of BtnInfo's
    UINT        uiState;    // State of the bar
    LPBTNINFO   lpB;        
    RECT        rc;

} TBHDR, *PTBHDR;

#define BTNSTATE        0x0001
#define BTNENABLE       0x0002

#define BTN_DEPRESSED   0x8000
#define TBST_IDLE       0x0000
#define TBST_TRACKING   0x0001
#define TBST_LASTHIT    0x0002

#endif // _ITOOLBAR_H_

//*** EOF: itoolbar.h

