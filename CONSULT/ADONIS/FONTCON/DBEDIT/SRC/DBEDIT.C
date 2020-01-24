//*************************************************************
//  File name: dbedit.c
//
//  Description: 
//      Entry point for DBEdit
//
//  History:    Date       Author     Comment
//               2/14/92   MSM        Created
//
//*************************************************************

#include "global.h"

HINSTANCE   ghInst;
HFONT       ghfontSmall;
HFONT       ghfontUser;
HWND        ghwndMain;
HWND        ghwndStat;
HWND        ghwndSlider;
HWND        ghwndELB;
HWND        ghwndFontView;
char        szDBPath[120];

LPDB        glpDB=NULL;

//*************************************************************
//
//  WinMain
//
//  Purpose:
//		Entry point for all windows apps
//
//
//  Parameters:
//      HINSTANCE hInstance
//      HINSTANCE hPrevInstance
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

    ghInst = hInstance;

    if (!hPrevInstance && !InitApplication(hInstance))
        return FALSE;

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    if (lpCmdLine && *lpCmdLine!='\0')
    {
       if (LoadPubInfo( glpDB, lpCmdLine ))
       {
          SyncDBELBToSlider( ghwndELB, ghwndSlider );
          FillDBELB( ghwndELB ); 
       }
    }

    while (GetMessage(&msg, NULL, NULL, NULL))
    {
        TranslateMessage(&msg);      
        DispatchMessage(&msg);       
    }

    if (ghfontUser != GetStockObject( SYSTEM_FONT ))
        DeleteObject( ghfontUser );
    DeleteObject( ghfontSmall );

    if (glpDB)
        FreeDataBase( glpDB );

    return (msg.wParam);      

} //*** WinMain

//*** EOF: dbedit.c

