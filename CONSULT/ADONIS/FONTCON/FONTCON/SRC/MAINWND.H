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

/*
 * These defines are used in the main window proc, and in
 * SizeHanlder() to determine the 3d effects.
 *
 * If DROPSHADOW is defined, the dropshadow type effects will
 * be used, otherwise we'll get 3D indent style effects
 */

//#define DROPSHADOW   

#ifdef DROPSHADOW
   #define SHADOW_THICKNESS    3
#else
   #define SHADOW_THICKNESS    2
#endif

#define BLANKSPACE          5

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
    BOOL  CALLBACK AboutBoxProc(HWND,UINT,WPARAM,LPARAM);
    VOID WINAPI UpdateStatLine( HWND hwndStatus, LPDB lpdb );
    VOID WINAPI UpdateToolbar( HWND hwndToolbar, LPDB lpdb );
    VOID WINAPI UpdateGroupingButtons(VOID);
    VOID WINAPI UpdateSortingButtons(VOID);
    BOOL WINAPI ReloadDatabase(VOID);

    VOID NEAR PASCAL InitSettingsMenu(HMENU);
    VOID NEAR PASCAL InitActionMenu(HMENU);

    VOID           SizeFontViewAndList(VOID);
    VOID           PaintMainWnd(VOID);

#endif

//*** EOF: mainwnd.h
