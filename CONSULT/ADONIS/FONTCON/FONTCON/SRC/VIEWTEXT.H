//*************************************************************
//  File name: viewtext.h
//
//  Description: 
//      Header file for the ViewText API
//
//  History:    Date       Author     Comment
//               4/14/92   MSM        Created
//
//*************************************************************

WORD WINAPI ViewText(HWND hParent,LPSTR lpText,LPSTR lpTitle,UINT uiFlags);
BOOL CALLBACK ViewTextProc(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam);

#ifndef IDB_HELP
#define IDB_HELP 0x85c0
#endif

//*** uiFlags
    #define VT_OK_CANCEL        0x0000
    #define VT_AGREE_DISAGREE   0x0001
    #define VT_LOADFILE         0x0002
    #define VT_NOCOPY           0x0004
    #define VT_NOHELP           0x0008
    #define VT_NOPRINT          0x0010

    #define IDDISAGREE          IDCANCEL
    #define IDAGREE             IDOK

    #define IDB_COPY            0x0100
    #define IDE_TEXT            0x0101
    #define IDB_PRINT           0x0102

    //*** Help notification
    #define VTN_HELP            WM_USER+0x85c

//*** EOF: viewtext.h
