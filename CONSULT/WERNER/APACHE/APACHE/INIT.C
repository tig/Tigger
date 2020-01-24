/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  init.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Initialization routines
 *
 *   Revisions:  
 *     01.00.001  1/ 7/91 baw   Initial Version, first build
 *
 ************************************************************************/

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"
#include "init.h"
#include "misc.h"
#include "version.h"

/************************************************************************
 *  BOOL FAR PASCAL InitApplication( HANDLE hInstance )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ************************************************************************/

BOOL FAR PASCAL InitApplication( HANDLE hInstance )
{
   WNDCLASS  wndclass ;

   // initialize status window class

#ifdef NOT_USED
   if (!StatusInit( 0, hInstance ))
      return ( FALSE ) ;
#endif

   // register frame window class 

   wndclass.style =         0 ;
   wndclass.lpfnWndProc =   FrameWndProc ;
   wndclass.cbClsExtra =    0 ;
   wndclass.cbWndExtra =    0 ;
   wndclass.hInstance =     hInstance ;
   wndclass.hIcon =         LoadIcon( hInstance,
                                      MAKEINTRESOURCE( APACHEICON ) ) ;
   wndclass.hCursor =       LoadCursor( NULL, IDC_ARROW ) ;
   wndclass.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1) ;
   wndclass.lpszMenuName =  NULL ;
   wndclass.lpszClassName = szFrameClass ;

   RegisterClass( &wndclass ) ;

   // register terminal window class

   wndclass.style =         0 ;
   wndclass.lpfnWndProc =   TerminalWndProc ;
   wndclass.cbClsExtra =    0 ;
   wndclass.cbWndExtra =    GWW_TERMEXTRABYTES ;
   wndclass.hInstance =     hInstance ;
   wndclass.hIcon =         LoadIcon( hInstance,
                                      MAKEINTRESOURCE( TERMINALICON  )) ;
   wndclass.hCursor =       LoadCursor( NULL, IDC_ARROW ) ;
   wndclass.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1) ;
   wndclass.lpszMenuName =  NULL ;
   wndclass.lpszClassName = szTerminalClass ;

   RegisterClass( &wndclass ) ;

   // register splash window class

   wndclass.style =         0 ;
   wndclass.lpfnWndProc =   SplashWndProc ;
   wndclass.cbClsExtra =    0 ;
   wndclass.cbWndExtra =    0 ;
   wndclass.hInstance =     hInstance ;
   wndclass.hIcon =         NULL ;
   wndclass.hCursor =       LoadCursor( NULL, IDC_ARROW ) ;
   wndclass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1) ;
   wndclass.lpszMenuName =  NULL ;
   wndclass.lpszClassName = szSplashClass ;

   RegisterClass( &wndclass ) ;

   return ( TRUE ) ;

} /* end of InitApplication() */

/************************************************************************
 *  BOOL FAR PASCAL InitInstance( HANDLE hInstance, int nCmdShow )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL FAR PASCAL InitInstance( HANDLE hInstance, int nCmdShow )
{
   int   i ;
   HWND  hSplashWnd ;

   hAppInst = hInstance ;

   // clear array for modeless dialog box handles

   for (i = 0; i < MAXLEN_MODELESSDLGS; i++)
      ahModelessDlgs[ i ] = NULL ;

   // get menu handles

   hMenuInit = LoadMenu( hInstance, "ApacheMenuInit" ) ;
   hMenuTerminal = LoadMenu( hInstance, "ApacheMenuTerminal" );

   hMenuInitWindow = GetSubMenu( hMenuInit, INIT_MENU_POS ) ;
   hMenuTerminalWindow = GetSubMenu( hMenuTerminal, TERMINAL_MENU_POS ) ;

   // load accelerators

   hAccel = LoadAccelerators( hInstance, "ApacheAccel" ) ;

   // create the frame window

   hFrameWnd = CreateWindow( szFrameClass, szTitle,
                             WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             NULL, hMenuInit, hInstance, NULL ) ;

   if (NULL == hFrameWnd)
      return (FALSE) ;

   // Client window is created when frame window is created
   // (e.g. client is a child of the frame)

   hClientWnd = GetWindow( hFrameWnd, GW_CHILD ) ;

   ShowWindow( hFrameWnd, nCmdShow ) ;
   UpdateWindow( hFrameWnd ) ;

   // create and display splash window

   hSplashWnd = CreateWindow( szSplashClass, szSplashClass,
                              WS_POPUP | WS_DLGFRAME,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              hFrameWnd, NULL, hInstance, NULL ) ;
   ShowWindow( hSplashWnd, SW_SHOW ) ;
   UpdateWindow( hSplashWnd ) ;

   return ( TRUE ) ;

} /* end of InitInstance() */

/************************************************************************
 * End of File: init.c
 ************************************************************************/

