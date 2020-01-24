/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** testwdb.c
 *
 *  DESCRIPTION: 
 *      This app is designed to show how to use WDB to help debug your
 *      applications.  It doesn't do a whole lot--so you don't have to
 *      wade through a bunch of code trying to figure out how to use
 *      it.
 *
 *  HISTORY:
 *      5/15/91     cjp     finalized it for V1.00
 *
 ** cjp */


/* the includes we need */
    #include <windows.h>
    #include <time.h>
    #include <stdlib.h>
    #include "testwdb.h"


/* test unique or default (shared) session with WDB? */
    #define TEST_UNIQUE_SESSION     1       /* 0 for default session    */


/* prototypes for good measure */
    long FAR PASCAL testWndProc( HWND, unsigned, UINT, LONG );
    void NEAR PASCAL testDoSomething( HWND hWnd );


/* globals, no less */
    char    gszAppName[] = "TestWDB";
    char    gszAppINI[]  = "testwdb.ini";
    HANDLE  ghInst;

    UINT    wClientW, wClientH;


/** BOOL FAR PASCAL testAboutDlgProc( HWND, UINT, UINT, LONG )
 *
 *  DESCRIPTION: 
 *      This is the dlg proc for the about box.
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL testAboutDlgProc( HWND   hDlg,
                                  UINT   wMsg,
                                  UINT   wParam,
                                  LONG   lParam )
{
   #ifdef WIN32
   UNREFERENCED_PARAMETER(lParam) ;
   #endif

    switch ( wMsg )
    {
        case WM_INITDIALOG:
        {
            char    szVersion[ 80 ];

            #ifdef DEBUG
            {
                UINT    wWDBVer;

                /* get WDB.DLL version */
                wWDBVer = wdbLibVersion( 0 );

                if (wdbGetOutput( hWDB, NULL ) == WDB_OUTPUT_WDBLIB)
                   wsprintf( szVersion, "%s V%d.%.2d -- WDB V%d.%.2d (not loaded)", (LPSTR)gszAppName,
                                            TEST_VERMAJ, TEST_VERMIN,
                                            HIBYTE(wWDBVer), LOBYTE(wWDBVer) );
                else
                   wsprintf( szVersion, "%s V%d.%.2d -- WDB V%d.%.2d", (LPSTR)gszAppName,
                                            TEST_VERMAJ, TEST_VERMIN,
                                            HIBYTE(wWDBVer), LOBYTE(wWDBVer) );
            }
            #else
                /* no debug, print normal version info */
                wsprintf( szVersion, "%s V%d.%.2d", (LPSTR)gszAppName,
                                            TEST_VERMAJ, TEST_VERMIN );
            #endif

            /* put in some version information */
            SetDlgItemText( hDlg, IDD_VERSION, szVersion );

            /* I want the focus on the ok button */
            SetFocus( GetDlgItem( hDlg, IDOK ) );
        }
        break;

        case WM_COMMAND:
            if ( (wParam == IDOK) || (wParam == IDCANCEL) )
            {
                EndDialog( hDlg, TRUE );
                return ( TRUE );
            }
        break;
    }

    return ( FALSE );
} /* testAboutDlgProc() */


/** void NEAR PASCAL testDoSomething( HWND hWnd )
 *
 *  DESCRIPTION:
 *      This function is supposed to do something in the window specified.
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   The window to do something in.
 *
 *  RETURN (void):
 *      Something will have been done.
 *
 *  NOTES:
 *
 ** cjp */

void NEAR PASCAL testDoSomething( HWND hWnd )
{
    HBRUSH  hBrush;
    HDC     hDC;
    UINT    wLeft, wRight, wTop, wBottom;

    if ( !wClientW || !wClientH )
        return;

    hDC = GetDC( hWnd );

    hBrush = CreateSolidBrush( RGB(rand() % 256, rand() % 256, rand() % 256) );
    SelectObject( hDC, hBrush );

    wLeft   = rand() % wClientW;
    wRight  = rand() % wClientW;

    wTop    = rand() % wClientH;
    wBottom = rand() % wClientH;

    Rectangle( hDC, min( wLeft, wRight ), min( wTop, wBottom ),
                    max( wLeft, wRight ), max( wTop, wBottom ) );

    ReleaseDC( hWnd, hDC );

    DeleteObject( hBrush );
} /* testDoSomething() */


/** long FAR PASCAL testWndProc( HWND, unsigned, UINT, LONG )
 *
 *  DESCRIPTION: 
 *      This is just your normal everyday WndProc().
 *
 *  NOTES:
 *
 ** cjp */

long FAR PASCAL testWndProc( HWND hWnd, unsigned iMsg, UINT wParam, LONG lParam )
{
    FARPROC lpfnDlgProc;

    switch ( iMsg ) 
    {
        case WM_SIZE:
            /* add 1 for mod operation in testDoSomething() */
            wClientW = LOWORD( lParam ) + 1;
            wClientH = HIWORD( lParam ) + 1;
        break;

        case WM_DESTROY:
            /*  This DPS() will cause an output 'window' to re-open!  So,
             *  if you have the debugging level set to 5 and you have your
             *  window as the parent to WDB's window, the window will be
             *  closed and then re-opened...
             */
            DPS5( hWDB, "WM_DESTROY" );

            PostQuitMessage( 0 );
        break;

        case WM_COMMAND:
        {
            switch ( wParam )
            {
                case IDM_FILE_EXIT:
                    SendMessage( hWnd, WM_CLOSE, 0, 0L );
                break;

                case IDM_ASSERT_ASSERT:
                    DPS( hWDB, "IDM_ASSERT_ASSERT" );
                    DASSERT( hWDB, 0 );
                break;

                case IDM_ASSERT_ASSERTMSG:
                    DPS( hWDB, "IDM_ASSERT_ASSERTMSG" );
                    DASSERTMSG( hWDB, 0, "This is a DASSERTMSG()" );
                break;

                case IDM_PUTS_DPS:
                    DPS( hWDB, "DPS() being tested here" );
                break;

                case IDM_PUTS_DPS1:
                    DPS1( hWDB, "DPS1() being tested here" );
                break;

                case IDM_PUTS_DPS2:
                    DPS2( hWDB, "DPS2() being tested here" );
                break;

                case IDM_PUTS_DPS3:
                    DPS3( hWDB, "DPS3() being tested here" );
                break;

                case IDM_PUTS_DPS4:
                    DPS4( hWDB, "DPS4() being tested here" );
                break;

                case IDM_PUTS_DPS5:
                    DPS5( hWDB, "DPS5() being tested here" );
                break;

                case IDM_PRINTF_DPF:   
                    DPF( hWDB, "DPF() being tested here (%s, %d)\n", (LPSTR)"foo", wParam );
                break;

                case IDM_PRINTF_DPF1:  
                    DPF1( hWDB, "DPF1() being tested here (%s, %d)\n", (LPSTR)"foo", wParam );
                break;

                case IDM_PRINTF_DPF2:  
                    DPF2( hWDB, "DPF2() being tested here (%s, %d)\n", (LPSTR)"foo", wParam );
                break;

                case IDM_PRINTF_DPF3:  
                    DPF3( hWDB, "DPF3() being tested here (%s, %d)\n", (LPSTR)"foo", wParam );
                break;

                case IDM_PRINTF_DPF4:  
                    DPF4( hWDB, "DPF4() being tested here (%s, %d)\n", (LPSTR)"foo", wParam );
                break;

                case IDM_PRINTF_DPF5:  
                    DPF5( hWDB, "DPF5() being tested here (%s, %d)\n", (LPSTR)"foo", wParam );
                break;


                case IDM_VPRINTF_DVPF:
                    DVPF( hWDB, "DVPF() being tested here\n", NULL );
                break;

                case IDM_VPRINTF_DVPF1:
                    DVPF1( hWDB, "DVPF1() being tested here\n", NULL );
                break;

                case IDM_VPRINTF_DVPF2:
                    DVPF2( hWDB, "DVPF2() being tested here\n", NULL );
                break;

                case IDM_VPRINTF_DVPF3:
                    DVPF3( hWDB, "DVPF3() being tested here\n", NULL );
                break;

                case IDM_VPRINTF_DVPF4:
                    DVPF4( hWDB, "DVPF4() being tested here\n", NULL );
                break;

                case IDM_VPRINTF_DVPF5:
                    DVPF5( hWDB, "DVPF5() being tested here\n", NULL );
                break;


                case IDM_PRINT_DP:     
                    DP( hWDB, "DP() being tested here (%s, %d)", (LPSTR)"foo", lstrlen("foo") );
                break;

                case IDM_PRINT_DP1:    
                    DP1( hWDB, "DP1() being tested here (%s, %d)", (LPSTR)"foo", lstrlen("foo")  );
                break;

                case IDM_PRINT_DP2:    
                    DP2( hWDB, "DP2() being tested here (%s, %d)", (LPSTR)"foo", lstrlen("foo")  );
                break;

                case IDM_PRINT_DP3:    
                    DP3( hWDB, "DP3() being tested here (%s, %d)", (LPSTR)"foo", lstrlen("foo")  );
                break;

                case IDM_PRINT_DP4:    
                    DP4( hWDB, "DP4() being tested here (%s, %d)", (LPSTR)"foo", lstrlen("foo")  );
                break;

                case IDM_PRINT_DP5:
                    DP5( hWDB, "DP5() being tested here (%s, %d)", (LPSTR)"foo", lstrlen("foo")  );
                break;


#ifdef DEBUG
                case IDM_WDB_CONFIG:
                    /* configure WDB session */
                    wdbConfigDlg( hWDB );

                    /*  If the output location changes (or you change the
                     *  number of lines in a window) then the output device
                     *  will not be re-opened automatically (by design).
                     *  If you want the window to re-open immediately, then
                     *  throw some output at WDB just after wdbConfigDlg().
                     */
                break;

                case IDM_WDB_DUMP:
                    /* dump all kinds of info from WDB */
                    wdbDumpDebugInfo( hWDB );
                break;
#endif

                case IDM_HELP_ABOUT:
                    /* standard stuff */
                    lpfnDlgProc = MakeProcInstance( (FARPROC)testAboutDlgProc, ghInst );
                    DialogBox( ghInst, "DLG_TEST_ABOUT", hWnd, (FARPROC)lpfnDlgProc );
                    FreeProcInstance( lpfnDlgProc );
                break;
            }
        }
        break;

        default:
        {
            /** set to level 5 for those *rare* occasions **/
            DP5( hWDB, "DefWindowProc( %.4Xh, %5d, %.4Xh, %.8Xh )",
                                        hWnd, iMsg, wParam, lParam );
            return ( DefWindowProc( hWnd, iMsg, wParam, lParam ) );
        }
    }

    return ( 0L );
} /* testWndProc() */


/** int PASCAL WinMain( HANDLE, HANDLE, LPSTR, int )
 *
 *  DESCRIPTION: 
 *      This is just your normal everyday WinMain() with a little bit
 *      of extra code to open and close a WDB session.
 *
 *  NOTES:
 *
 ** cjp */

int PASCAL WinMain( HANDLE  hInstance,
                    HANDLE  hPrevInstance,
                    LPSTR   lpszCmdLine,
                    int     nCmdShow )
{
    WNDCLASS    wndclass;
    HWND        hWnd;
    MSG         msg;

   #if WIN32
   UNREFERENCED_PARAMETER(lpszCmdLine) ;
   UNREFERENCED_PARAMETER(nCmdShow) ;
   #endif

    ghInst = hInstance;

    if ( !hPrevInstance ) 
    {
        wndclass.style         = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc   = testWndProc;
        wndclass.cbClsExtra    = 0;
        wndclass.cbWndExtra    = 0;
        wndclass.hInstance     = hInstance;
        wndclass.hIcon         = LoadIcon( hInstance, "ICON_TEST" );
        wndclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
      #ifndef WIN32
        wndclass.hbrBackground = COLOR_WINDOW + 1;
      #else
        wndclass.hbrBackground = CreateSolidBrush( GetSysColor( COLOR_WINDOW ));
      #endif
        wndclass.lpszMenuName  = "MENU_TEST";
        wndclass.lpszClassName = gszAppName;

        if ( !RegisterClass( &wndclass ) )
            return ( FALSE );
    }

    hWnd = CreateWindow( gszAppName, gszAppName,
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         TEST_WINDOW_WIDTH, TEST_WINDOW_HEIGHT,
                         NULL, NULL, hInstance, NULL );             

    ShowWindow( hWnd, SW_SHOW );
    UpdateWindow( hWnd );

/** establish a debugging session with WDB **/
#if TEST_UNIQUE_SESSION
    /* open a UNIQUE session with WDB */
    D(  if ( hWDB = wdbOpenSession(hWnd, gszAppName, gszAppINI, WDB_LIBVERSION) )
        {
            DPS( hWDB, "WDB is active!" );
        }

        else
        {
            MessageBox( hWnd, "Bad version of WDB.DLL!", gszAppName, MB_OK );
            return ( FALSE );
        }
     );
#else
    /* use the DEFAULT session with WDB */
    D(  hWDB = NULL;
        if ( wdbLibVersion( WDB_LIBVERSION ) )
        {
            DPS( hWDB, "WDB is active!" );
        }

        else
        {
            MessageBox( hWnd, "Bad version of WDB.DLL!", gszAppName, MB_OK );
            return ( FALSE );
        }
    );
#endif
    
    /* put a little pizzazz in life */
    srand( (unsigned)time( NULL ) );

    /* Rock & Roll! */
    for (;;)
    {
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            if ( msg.message == WM_QUIT )
                break;

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

        else testDoSomething( hWnd );
    }

    /** close the debugging session--if active **/
    D( hWDB = wdbCloseSession( hWDB ) );

    return ( msg.wParam );
} /* WinMain() */


/** EOF: testwdb.c **/
