//*************************************************************
//  File name: conxup.c
//
//  Description: 
//
//
//  History:    Date       Author     Comment
//               5/ 9/92   MSM        Created
//
//*************************************************************

#include "global.h"
#include <lzexpand.h>
#include <string.h>

HINSTANCE ghInst      = NULL;
HWND      ghwndMain   = NULL;
HWND      ghwndStatic = NULL;

char      szBuffer[4096];
char      szSrc[120];
char      szDest[120];


BOOL UpdateFile( LPSTR lpSrc, LPSTR lpDest );

//*************************************************************
//
//  WinMain
//
//  Purpose:
//              Entry point for all windows apps
//
//
//  Parameters:
//      HANDLE hInstance
//      HANDLE hPrevInstance
//      LPSTR lpCmdLine
//      int nCmdShow
//      
//
//  Return: (int PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/12/91   MSM        Created
//
//*************************************************************

int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    int nLen;
    LPSTR lp = (LPSTR)szBuffer;

    if (!hPrevInstance && !InitApplication(hInstance,hPrevInstance))
            return (FALSE);       

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    
    nLen = GetPrivateProfileString( "ConXUp", NULL, "", szBuffer, 4096, "CONXTION.INI");

    if (nLen == 4094)
    {
        MessageBox( NULL, "Too many files to update!  Please contact Adonis Technical Support at (800) 234-9497.",
            "ConXUp", MB_ICONSTOP|MB_OK );
        return FALSE;
    }

    while (nLen)
    {
        lstrcpy( szDest, lp );
        nLen -= (lstrlen( lp )+1);
        lp += lstrlen(lp) + 1;

        if (_fstricmp((LPSTR)szDest,(LPSTR)"Application")==0)
            continue;

        GetPrivateProfileString( "ConXUp", szDest, "", szSrc, 120, "CONXTION.INI");

        if (szSrc[0]=='\0')
            continue;

        if (UpdateFile( szSrc, szDest ))
            WritePrivateProfileString( "ConXUp", szDest, NULL, "CONXTION.INI");
        else
        {
            wsprintf( szBuffer, "Error updating %s.\n\nPlease contact Adonis Technical Support at (800) 234-9497.", 
                (LPSTR)szDest );
            MessageBox( NULL, szBuffer, "ConXUp", MB_ICONSTOP|MB_OK );
            return FALSE;
        }

    }

    SetTimer( ghwndMain, 42, 2000, NULL ) ;

    while (GetMessage(&msg, NULL, NULL, NULL))
    {
        TranslateMessage(&msg);      
        DispatchMessage(&msg);       
    }

    return (msg.wParam);      

} //*** WinMain


//*************************************************************
//
//  InitApplication
//
//  Purpose:
//              Initializes the application (window classes)
//
//
//  Parameters:
//      HANDLE hInst     - hInstance from WinMain
//      HANDLE hPrevInst - previous hInstance from WinMain
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/12/91   MSM        Created
//
//*************************************************************

BOOL InitApplication (HINSTANCE hInst, HINSTANCE hPrevInst)
{
    WNDCLASS  wc;

    wc.style = NULL;             
    wc.lpfnWndProc  = (WNDPROC)MainWndProc;
                                 
    wc.cbClsExtra   = 0;           
    wc.cbWndExtra   = 0;           
    wc.hInstance    = hInst;    
    wc.hIcon        = NULL;
    wc.hCursor      = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground= (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "ConXUp.WndClass";

    if ( !RegisterClass(&wc) )
        return FALSE;

   return TRUE;

} //*** InitApplication


//*************************************************************
//
//  InitInstance
//
//  Purpose:
//              Initializes each instance (window creation)
//
//
//  Parameters:
//      HANDLE hInstance
//      int nCmdShow
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/12/91   MSM        Created
//
//*************************************************************

BOOL InitInstance (HINSTANCE hInstance, int nCmdShow)
{
    ghwndMain = CreateWindow( "ConXUp.WndClass", "Upgrading...",
        WS_OVERLAPPED|WS_CAPTION|WS_BORDER,           
        50, 50, 350, 150,
        NULL, NULL, hInstance, NULL );

    if (!ghwndMain)
        return (FALSE);

    ghwndStatic = CreateWindow( "Static", "Upgrading...",
        WS_CHILD|WS_VISIBLE,
        20, 45, 310, 20,
        ghwndMain, (HMENU)100, hInstance, NULL );

    if (!ghwndStatic)
        return (FALSE);

    SetWindowPos( ghwndMain, HWND_TOP, (GetSystemMetrics(SM_CXSCREEN)-350)/2,
        (GetSystemMetrics(SM_CYSCREEN)-150)/2,0,0,SWP_NOSIZE );

    //*** Show Main window
    ShowWindow( ghwndMain, nCmdShow );
    UpdateWindow(ghwndMain);

} //*** InitInstance


//*************************************************************
//
//  MainWndProc
//
//  Purpose:
//              Main Window procedure
//
//
//  Parameters:
//      HWND hWnd
//      unsigned msg
//      WORD wParam
//      LONG lParam
//      
//
//  Return: (long FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/12/91   MSM        Created
//
//*************************************************************

LRESULT FAR PASCAL MainWndProc (HWND hWnd, unsigned msg, WORD wParam, LONG lParam)
{
    switch (msg) 
    {
        case WM_TIMER:

         GetPrivateProfileString( "ConXUp", "Application", "", szSrc, 120, "CONXTION.INI");

         if (szSrc[0]!='\0')
            WinExec( szSrc, SW_SHOWNORMAL );

         PostMessage( hWnd, WM_CLOSE, 0, 0L ) ;

        break ;

        case WM_DESTROY:
        {
            PostQuitMessage( 0 );
        }
        break;
    }
    return (DefWindowProc(hWnd, msg, wParam, lParam));

} //*** MainWndProc


//*************************************************************
//
//  UpdateFile
//
//  Purpose:
//      Update the dest file by copying the src, then deleting it
//
//
//  Parameters:
//      LPSTR lpSrc
//      LPSTR lpDest
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               5/ 9/92   MSM        Created
//
//*************************************************************

BOOL UpdateFile( LPSTR lpSrc, LPSTR lpDest )
{
    HFILE hSrc, hDest;
    OFSTRUCT ofSrc, ofDest;

    {
        char temp[300];

        wsprintf( temp, "Updating %s", lpDest );
        SetWindowText( ghwndStatic, temp );
//        UpdateWindow( ghwndStatic );
    }

    hDest = LZOpenFile( lpDest, &ofDest, OF_CREATE|OF_WRITE );

    if (!hDest)
        return FALSE;

    hSrc = LZOpenFile( lpSrc, &ofSrc, OF_READ );

    if (!hSrc)
    {
        LZClose( hDest );
        return FALSE;
    }

    if (LZCopy( hSrc, hDest )<=0)
    {
        LZClose( hDest );
        LZClose( hSrc );
        return FALSE;
    }

    LZClose( hDest );
    LZClose( hSrc );

    //*** Delete source file
    OpenFile( lpSrc, &ofSrc, OF_DELETE );

    return TRUE;

} //*** UpdateFile

//*** EOF: upshow.c
