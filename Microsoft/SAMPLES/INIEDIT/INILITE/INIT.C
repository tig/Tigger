//*************************************************************
//  File name: init.c
//
//  Description:
//      Routines which initialize the application and instance.
//
//  History:    Date       Author     Comment
//              1/1/92     JJJ        Created
//
// by Microsoft Developer Support, Windows Developer Support
// Copyright (c) 1992 Microsoft Corp. All rights reserved.
//*************************************************************
#include "precomp.h"
#include <sys\types.h>
#include <sys\stat.h>
#include "memory.h"
#include "inilite.h"
#include "global.h"


//*************************************************************
//
//  InitApplication()
//
//  Purpose:
//		Initializes the application (window classes)
//
//
//  Parameters:
//      HINSTANCE hInstance - Instance handle from WinMain.
//      
//
//  Return: (BOOL)
//      TRUE  - Window class was initialized and registered.
//      FALSE - Window class not initialized and registered.
//
//
//  Comments:
//
//  History:    Date       Author     Comment
//              5/1/92     JJJ        Created
//
//*************************************************************

BOOL InitApplication ( HINSTANCE hInstance )
{
    WNDCLASS  wc;

    wc.style         = NULL;             
    wc.lpfnWndProc   = (WNDPROC)MainWndProc;
    wc.cbClsExtra    = 0;           
    wc.cbWndExtra    = 0;           
    wc.hInstance     = hInstance;    
    wc.hIcon         = LoadIcon(hInstance, "MAINICON");
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE+1);
    wc.lpszMenuName  = szMainMenu;  
    wc.lpszClassName = szMainClass;

    if ( RegisterClass(&wc) )
        return(TRUE);

    return(FALSE);

} // InitApplication() 

//*************************************************************
//
//  InitInstance()
//
//  Purpose:
//		Initializes each instance (window creation)
//
//  Parameters:
//      HINSTANCE hInstance - Instance handle of application.
//      int       nCmdShow  - How window is initially displayed.
//
//  Return: (BOOL)
//      TRUE  - Window created and displayed.
//      FALSE - Window not created.
//
//  Comments:
//
//  History:    Date       Author     Comment
//              1/1/92     JJJ        Created
//
//*************************************************************
BOOL InitInstance ( HINSTANCE hInstance, int nCmdShow )
{

    ghInst = hInstance;

    ghWndMain = CreateWindow( szMainClass, 
                              "Inilite Driver",  
                              WS_OVERLAPPEDWINDOW,           
                              CW_USEDEFAULT, CW_USEDEFAULT, 
                              CW_USEDEFAULT, CW_USEDEFAULT, 
                              NULL, NULL, hInstance, NULL );

    if (!ghWndMain)
        return(FALSE);

    ShowWindow(ghWndMain, nCmdShow);
    UpdateWindow(ghWndMain);

    return(TRUE);

} // InitInstance() 

// EOF: init.c

