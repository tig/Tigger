#ifndef BUSYH
#define BUSYH

typedef WORD ICON_ID;

extern BOOL FAR PASCAL  BusyBox(HANDLE, HWND, WORD);
extern BOOL FAR PASCAL  BusyBoxWithIcon(HANDLE, HWND, WORD, ICON_ID);
extern VOID FAR PASCAL  BusyClear(BOOL bReEnable);
extern BOOL BusySetNewText (HWND hWnd, char* newText, ICON_ID Icon);
extern BOOL BusyLoadNewText (HWND hWnd, WORD msg, ICON_ID Icon);
extern void FAR PASCAL BusyLoadNewIcon (HWND hWndDlg, ICON_ID NewIcon);

extern HWND    hBusyBox;

/* for BusyBoxWithIcon */

#endif
