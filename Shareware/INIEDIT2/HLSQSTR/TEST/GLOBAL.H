/** global.h
 *
 *  DESCRIPTION: 
 *      Global include/prototype file
 *
 *  HISTORY:
 *
 ** MSM! */

#include <windows.h>


/* Menu Defines */
    #define IDM_ABOUT 100

/* Function Prototypes */

/* Main.c */
    int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
    long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);
    BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);

/* Init.c */
    BOOL InitApplication(HANDLE);
    BOOL InitInstance(HANDLE, int);

/* EXTERNS for Global Variables */
    extern HANDLE       ghInst;
    extern HWND         ghWndMain;

    extern char         szMainMenu[];
    extern char         szMainClass[];

/** EOF: global.h **/
