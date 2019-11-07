////////////////////////////////////////////////////////////////////////
//
//  Project:  ItsyBits Test Application
//  Module:  ITSYTEST.C
//
//   Revisions:  
//      8/13/92     cek     Wrote it.
//      2/24/93     cek     Updated to support min/max buttons.
//                          Added more test cases.
//      3/24/93     cek     More comments.  Fixed DS_MODALDIALOG style
//                          problem.  Use auto precompiled headers
//                          in MSVC.
//
////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <windowsx.h>
#include "itsybits.h"


// Variables local to this module.
//
static HWND          hwndApp ;
static HINSTANCE     hinstApp ;

// In this sample, we have a bunch of itsybits windows we create.
// We store their window handles in these static HWNDs.
//
static HWND hwnd1 ;
static HWND hwnd2 ;
static HWND hwnd3 ;
static HWND hwnd4 ;
static HWND hwnd5 ;
static HWND hwnd6 ;



// Function Prototypes
//
BOOL WINAPI InitClass( HINSTANCE hInstance ) ;
HWND WINAPI CreateMain( VOID ) ;

////////////////////////////////////////////////////////////////
//  LRESULT CALLBACK fnItsyWnd( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
//
//  Window proc for sample ItsyBits windows.  We trap the L and R
//  mouse buttons and increase or decrease the size of the caption
//  on each click.
//
////////////////////////////////////////////////////////////////
LRESULT CALLBACK fnItsyWnd( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uiMsg )
    {
        case WM_COMMAND:                                       
            // The system menu functionality of ItsyBits relies on WM_COMMAND
            // messages being generated (TrackPopupMenu generates WM_COMMAND
            // messages, which ibDefWindowProc() translates into the
            // appropriate WM_SYSCOMMAND/SC_ messages.)
            //
            // This means that you must do two things in your WM_COMMAND
            // processing:
            //
            // 1) Call ibDefWindowProc() for any WM_COMMAND messages
            //    you do not process. 
            //
            // 2) Make sure that your control IDs (wParam) do not have
            //    the SC_* values (i.e. SC_CLOSE or SC_MOVE).  In other
            //    words, just make sure that your control IDs that
            //    send notifications to your itsy wnd do not have ID values
            //    of 0xF000 through 0xF150.
            //
        return ibDefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
    
        case WM_LBUTTONDBLCLK:
            if (wParam & MK_SHIFT)
                ibSetCaptionSize( hWnd, ibGetCaptionSize( hWnd ) - 1 ) ;
            else
                ibSetCaptionSize( hWnd, ibGetCaptionSize( hWnd ) + 1 ) ;
        break ;

        case WM_RBUTTONDBLCLK:
        {
            DWORD dw = GetWindowLong( hWnd, GWL_STYLE ) ;
            RECT rc ;
                       
            if (dw & IBS_HORZCAPTION)
            {
                dw &= ~IBS_HORZCAPTION ;
                dw |= IBS_VERTCAPTION ;
            }
            else
            {
                dw &= ~IBS_VERTCAPTION ;
                dw |= IBS_HORZCAPTION ;
            }            
            SetWindowLong( hWnd, GWL_STYLE, (LONG)dw) ;

            GetWindowRect( hWnd, &rc ) ;
        
            // Once we change the window style, we need a WM_NCCALCRECT
            // to be generated.
            //
            // SWP_FRAMECHANGED is not documented in the 3.1 SDK docs,
            // but *is* in WINDOWS.H.
            //
            SetWindowPos( hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | 
                                SWP_NOSIZE | SWP_NOMOVE | 
                                SWP_NOACTIVATE | SWP_NOZORDER) ;

        }    
        break ;

        case WM_PAINT:
        {
            PAINTSTRUCT ps ;
            HFONT       hfont ;
            RECT        rc ;
            char        sz[] = "Left DblClk (+)\n\n"\
                               "Shift Left DblClk (-)\n\n"\
                               "Right DblClk swaps" ;

            BeginPaint( hWnd, &ps ) ;
            GetClientRect( hWnd, &rc ) ;
            InflateRect( &rc, -3, -3 ) ;

            hfont = CreateFont( -8, 0, 0, 0, 0, 0, 
                                0, 0, ANSI_CHARSET, 0, 0, 0,
                                FF_SWISS, "MS Sans Serif" ) ;
            hfont = SelectObject( ps.hdc, hfont ) ;
            DrawText( ps.hdc, sz, lstrlen(sz), &rc, DT_NOPREFIX | DT_WORDBREAK ) ;
            hfont = SelectObject( ps.hdc, hfont ) ;
            DeleteObject( hfont ) ;

            EndPaint( hWnd, &ps ) ;
        }
        break ;

        default:
            return ibDefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
    }

    return 0L ;

} // fnItsyWnd() 

////////////////////////////////////////////////////////////////
//  LRESULT FAR PASCAL
//     fnMainWnd( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
//
//  Main Window procedure.
//
//  Do boring stuff like create all of our itsybits windows.    
//
////////////////////////////////////////////////////////////////
LRESULT CALLBACK fnMainWnd( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uiMsg )
    {
        case WM_CREATE:
        {
            UINT      cx, cy, dx, dy ;

            hwndApp = hWnd ;

            #define CX 25 
            #define CY 50 
            #define DX 125 
            #define DY 175

            cx = CX ;
            cy = CY ;
            dx = DX ;
            dy = DY ;

            // This one has 'everything' (i.e. a sys menu, min/max buttons,
            // and a thick frame).
            //
            // It has a horizontal caption.
            //
            hwnd1 = CreateWindow( 
                                    "ItsyWnd",
                                    "ItsyBits #1",
                                    IBS_HORZCAPTION |
                                    WS_MAXIMIZEBOX |
                                    WS_MINIMIZEBOX |
                                    WS_SYSMENU|
                                    WS_THICKFRAME |
                                    WS_BORDER |
                                    WS_POPUP,
                                    cx,
                                    cy,
                                    dx,
                                    dy ,
                                    hWnd,
                                    NULL,
                                    hinstApp,
                                    NULL ) ;

            // Create an ItsyBits window with a sys menu, min button, and
            // a standard border (non-sizable).
            //
            // It has a horizontal caption.
            //
            hwnd2 = CreateWindow(
                                    "ItsyWnd",
                                    "ItsyBits #2",
                                    IBS_HORZCAPTION |
                                    WS_MINIMIZEBOX |
                                    WS_SYSMENU|
                                    WS_BORDER |
                                    WS_POPUP,
                                    cx += dx + 10, 
                                    cy, 
                                    dx,
                                    dy,
                                    hWnd,
                                    NULL,
                                    hinstApp,
                                    NULL ) ;
        
            // Create the most boring ItsyBits window possible (no sys   
            // menu, no min/max, and a non-sizable border.)
            //
            // It has a tiny horizontal caption.
            //
            hwnd3 = CreateWindow(
                                    "ItsyWnd",
                                    "",
                                    IBS_HORZCAPTION |
                                    WS_SYSMENU|
                                    WS_BORDER |
                                    WS_POPUP,
                                    cx += dx + 10, 
                                    cy, 
                                    dx,
                                    dy,
                                    hWnd,
                                    NULL,
                                    hinstApp,
                                    NULL ) ;
            ibSetCaptionSize( hwnd3, ibGetCaptionSize( hwnd3 ) - 3 ) ;

            // This is the last of the horizontal caption ones.  It
            // is just like ItsyBits #1 but has a caption that's a bit
            // larger. 
            //
            // It has a horizontal caption.
            //
            hwnd4 = CreateWindow(
                                    "ItsyWnd",
                                    "ItsyBits #4 (No Parent)",
                                    IBS_HORZCAPTION |
                                    WS_MAXIMIZEBOX |
                                    WS_MINIMIZEBOX |
                                    WS_SYSMENU|
                                    WS_THICKFRAME |
                                    WS_BORDER |
                                    WS_POPUP,
                                    cx += dx + 10, 
                                    cy, 
                                    dx * 2,
                                    dy,
                                    NULL, // No parent 'cause it has a min button
                                    NULL,
                                    hinstApp,
                                    NULL ) ;
            ibSetCaptionSize( hwnd4, ibGetCaptionSize( hwnd4 ) + 4 ) ;


            // Create a vertical captioned ItsyBits window.  
            // This one has 'everything' (i.e. a sys menu, min/max buttons,
            // and a thick frame).
            //
            hwnd5 = CreateWindow( 
                                    "ItsyWnd",
                                    "ItsyBits #5",
                                    IBS_VERTCAPTION |
                                    WS_MAXIMIZEBOX |
                                    WS_MINIMIZEBOX |
                                    WS_SYSMENU|
                                    WS_THICKFRAME |
                                    WS_BORDER |
                                    WS_POPUP,
                                    CX,
                                    cy += dy + 10,
                                    DY,
                                    DX,
                                    hWnd,
                                    NULL,
                                    hinstApp,
                                    NULL ) ;
            ibSetCaptionSize( hwnd5, ibGetCaptionSize( hwnd5 ) + 3 ) ;

            // Create a vertical captioned ItsyBits window.  
            // Make this one have only a system menu.
            //
            // This one's vertical caption is the same widht as
            // the standard Windows' captions are high.
            //
            hwnd6 = CreateWindow( 
                                    "ItsyWnd",
                                    "ItsyBits #6",
                                    IBS_VERTCAPTION |
                                    WS_SYSMENU|
                                    WS_BORDER |
                                    WS_POPUP,
                                    CX,
                                    cy + DX + 10,
                                    DY,
                                    DX,
                                    hWnd,
                                    NULL,
                                    hinstApp,
                                    NULL ) ;
            ibSetCaptionSize( hwnd6, GetSystemMetrics( SM_CYCAPTION ) ) ;
        }
        break ;

        case WM_DESTROY:
            if (hwnd1 && IsWindow( hwnd1 ))
                DestroyWindow( hwnd1 ) ;

            if (hwnd2 && IsWindow( hwnd2 ))
                DestroyWindow( hwnd2 ) ;

            if (hwnd3 && IsWindow( hwnd3 ))
                DestroyWindow( hwnd4 ) ;

            if (hwnd4 && IsWindow( hwnd4 ))
                DestroyWindow( hwnd4 ) ;

            if (hwnd5 && IsWindow( hwnd5 ))
                DestroyWindow( hwnd5 ) ;

            if (hwnd5 && IsWindow( hwnd5 ))
                DestroyWindow( hwnd5 ) ;

            PostQuitMessage( 0 ) ;
        break ;

        default:
            return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
    }

    return 0L ;

} // fnMainWnd() ///


////////////////////////////////////////////////////////////////
//  int PASCAL
//     WinMain( HANDLE hinst, HANDLE hinstPrev, LPSTR lpszCmd, in nCmdShow )
//
//  Description: 
//
//
//
//  Comments:
//
////////////////////////////////////////////////////////////////
int PASCAL WinMain( HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpszCmd, int nCmdShow )
{
    int     n ;
    MSG     msg ;

    hinstApp = hinst ;

    if (!hinstPrev)
        if (!InitClass( hinst ))
        {
            goto HellInAHandBasket2 ;
        }

    // Read defaults
    //
    if (!(hwndApp = CreateMain()))
    {
        goto HellInAHandBasket2 ;
    }

    ShowWindow( hwndApp, nCmdShow ) ;
    UpdateWindow( hwndApp ) ;

    // Make all of the itsybits windows visible.
    //
    ShowWindow( hwnd1, SW_SHOWNORMAL ) ;
    UpdateWindow( hwnd1 ) ;

    ShowWindow( hwnd2, SW_SHOWNORMAL ) ;
    UpdateWindow( hwnd2 ) ;

    ShowWindow( hwnd3, SW_SHOWNORMAL ) ;
    UpdateWindow( hwnd3 ) ;

    ShowWindow( hwnd4, SW_SHOWNORMAL ) ;
    UpdateWindow( hwnd4 ) ;

    ShowWindow( hwnd5, SW_SHOWNORMAL ) ;
    UpdateWindow( hwnd5 ) ;

    ShowWindow( hwnd6, SW_SHOWNORMAL ) ;
    UpdateWindow( hwnd6 ) ;

    SetActiveWindow( hwnd4 ) ;

    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage( &msg ) ;
        DispatchMessage( &msg ) ;
    }

    return (int)msg.wParam ;

HellInAHandBasket2:
                                
    return n ;

} // MyWinMain() ///


////////////////////////////////////////////////////////////////
//  BOOL WINAPI InitClass( HINSTANCE hInstance )
//
//  Description: 
//
//     Registers the window classes.
//
//  Comments:
//
////////////////////////////////////////////////////////////////
BOOL WINAPI InitClass( HINSTANCE hInstance )
{
    WNDCLASS    wc ;
    BOOL        f = TRUE ;

    wc.style            = 0L ;
    wc.lpfnWndProc      = fnMainWnd ;
    wc.cbClsExtra       = 0 ;
    wc.cbWndExtra       = 0 ;
    wc.hInstance        = hInstance ;
    wc.hIcon            = LoadIcon( hInstance, "IDI_ITSYTEST" ) ;
    wc.hCursor          = LoadCursor( NULL, IDC_ARROW ) ;
    wc.hbrBackground    = (HBRUSH)(COLOR_APPWORKSPACE + 1) ;
    wc.lpszMenuName     = NULL ;
    wc.lpszClassName    = "ItsyTest" ;

    if (!RegisterClass( &wc ))
        return FALSE ;

    wc.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW ;
    wc.lpfnWndProc      = fnItsyWnd;  
    wc.hIcon            = LoadIcon( hInstance, "IDI_ITSYTEST" ) ;
    wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1) ;
    wc.lpszClassName    = "ItsyWnd";

    // Register the window class and return success/failure code.///

    return RegisterClass( &wc ) ;

} // InitClass() ///


////////////////////////////////////////////////////////////////
//  HWND WINAPI CreateMain( VOID )
//
//  Description: 
//
//
//
//  Comments:
//
////////////////////////////////////////////////////////////////
HWND WINAPI CreateMain( VOID )
{
    HWND hwnd ;

#ifdef WIN32
    hwnd = CreateWindow
        (
            "ItsyTest",
            "ItsyBits Test Application (32 bits)",
            WS_OVERLAPPEDWINDOW |
            WS_CLIPCHILDREN,  
            5,
            5,
            GetSystemMetrics( SM_CXFULLSCREEN ) - 10,
            GetSystemMetrics( SM_CYCAPTION ) * 4,
            NULL,                      
            NULL,                      
            hinstApp,                
            NULL                        
        ) ;
#else
    hwnd = CreateWindow
        (
            "ItsyTest",
            "ItsyBits Test Application (16 bits)",
            WS_OVERLAPPEDWINDOW |
            WS_CLIPCHILDREN,  
            5,
            5,
            GetSystemMetrics( SM_CXFULLSCREEN ) - 10,
            GetSystemMetrics( SM_CYCAPTION ) * 4,
            NULL,                      
            NULL,                      
            hinstApp,                
            NULL                        
        ) ;
#endif

    return hwnd ;

} // CreateMain() ///


////////////////////////////////////////////////////////////////////////
// End of File: ITSYTEST.c
////////////////////////////////////////////////////////////////////////

