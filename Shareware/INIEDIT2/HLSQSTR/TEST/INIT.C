/** init.c
 *
 *  DESCRIPTION: 
 *      Initializes application and instances
 *
 *  HISTORY:
 *
 ** MSM! */

#include "global.h"

/** BOOL InitApplication(hInstance)
 *
 *  DESCRIPTION: 
 *      Initializes the window class
 *
 *  ARGUMENTS:
 *      (hInstance)
 *
 *  RETURN (BOOL):
 *
 *
 *  NOTES:
 *
 ** MSM! */

BOOL InitApplication(HANDLE hInstance)
{
    WNDCLASS  wc;

    wc.style = NULL;             
    wc.lpfnWndProc = MainWndProc;
                                 
    wc.cbClsExtra = 0;           
    wc.cbWndExtra = 0;           
    wc.hInstance = hInstance;    
    wc.hIcon = LoadIcon(hInstance, "MAINICON");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = COLOR_APPWORKSPACE+1;
    wc.lpszMenuName  = szMainMenu;  
    wc.lpszClassName = szMainClass;

    if ( !RegisterClass(&wc) )
        return FALSE;

    return TRUE;

} /* InitApplication() */


/** BOOL InitInstance(HANDLE hInstance, int nCmdShow)
 *
 *  DESCRIPTION: 
 *      Creates the main overlapped window
 *
 *  ARGUMENTS:
 *      (HANDLE hInstance, int nCmdShow)
 *
 *  RETURN (BOOL):
 *
 *
 *  NOTES:
 *
 ** MSM! */

BOOL InitInstance(HANDLE hInstance, int nCmdShow)
{

    ghInst = hInstance;

    ghWndMain = CreateWindow( szMainClass, 
        "Generic Sample Application",  
        WS_OVERLAPPEDWINDOW,           
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL );

    if (!ghWndMain)
        return (FALSE);

    ShowWindow(ghWndMain, nCmdShow);
    UpdateWindow(ghWndMain);
    return (TRUE);

} /* InitInstance() */

/** EOF: init.c **/
