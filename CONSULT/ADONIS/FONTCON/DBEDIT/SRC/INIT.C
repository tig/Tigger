//*************************************************************
//  File name: init.c
//
//  Description: 
//      Initialization Routine
//
//  History:    Date       Author     Comment
//               2/13/92   MSM        Created
//
//*************************************************************

#include "global.h"

//*************************************************************
//
//  InitApplication
//
//  Purpose:
//		Initializes the application (window classes)
//
//
//  Parameters:
//      HINSTANCE hInstance - hInstance from WinMain
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

BOOL InitApplication (HINSTANCE hInstance)
{
    WNDCLASS  wc;

    wc.style = CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW;
    wc.lpfnWndProc = MainWndProc;
                                 
    wc.cbClsExtra = 0;           
    wc.cbWndExtra = 0;           
    wc.hInstance = hInstance;    
    wc.hIcon = LoadIcon( hInstance, IDI_FSICON );
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject( LTGRAY_BRUSH );
    wc.lpszMenuName  = GetRCString( IDS_MAINMENU, hInstance );  
    wc.lpszClassName = GetRCString( IDS_MAINCLASS, hInstance );

    if ( !RegisterClass(&wc) )
        return FALSE;

    wc.style = CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW;
    wc.lpfnWndProc = FVWndProc;
                                 
    wc.cbClsExtra = 0;
    wc.cbWndExtra = FV_WNDEXTRA;
    wc.hInstance = hInstance;    
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = SZ_FONTVIEWCLASS;

    if ( !RegisterClass(&wc) )
        return FALSE;

    return TRUE;

} //*** InitApplication

//*************************************************************
//
//  InitInstance
//
//  Purpose:
//		Initializes each instance (window creation)
//
//
//  Parameters:
//      HINSTANCE hInstance
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
    LPWINDOWPLACEMENT lpwp;
    LPSTR lp1, lp2;

    if (!InitializeFCDB())
        return FALSE;

    if (!(glpDB = CreateDataBase()))
        return FALSE;
    
    glpDB->wDBType = DB_TYPE_DBEDIT;
    glpDB->uView = DB_VIEW_ALL;
    glpDB->uSort = DB_SORT_NAME;

    lp1 = GRCS( IDS_MAINCLASS );
    lp2 = GRCS( IDS_APPNAME );

    ghwndMain = CreateWindow( lp1, lp2,
        WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, 
        NULL, NULL, hInstance, NULL );

    if (!ghwndMain)
        return (FALSE);

    lpwp = RestoreConfiguration();

    if (lpwp)
    {
        SetWindowPlacement( ghwndMain, lpwp );
        nCmdShow = lpwp->showCmd;
    }

    ShowWindow( ghwndMain, nCmdShow );
    SyncDBELBToSlider( ghwndELB, ghwndSlider );

    return (TRUE);

} //*** InitInstance


//*************************************************************
//
//  CreateStatus
//
//  Purpose:
//      Creates the status bar
//
//
//  Parameters:
//      HWND hwndParent
//      UINT uID
//      
//
//  Return: (HWND WINAPI PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

HWND WINAPI PASCAL CreateStatus( HWND hwndParent, UINT uID )
{
    HWND hwndStat;
    LONG lNumStats = 0;

    if (!(hwndStat = CreateWindow( SZ_STATUSCLASSNAME, "",
                WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS, 0,0,1,1, 
                hwndParent, (HMENU)uID, ghInst, (LPSTR)&lNumStats )))
    {
        return 0 ;
    }

    return hwndStat;

} //*** CreateStatus


//*************************************************************
//
//  CreateSlider
//
//  Purpose:
//      Creates the slider bar
//
//
//  Parameters:
//      HWND hwndParent
//      UINT uID
//      
//
//  Return: (HWND FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

HWND FAR PASCAL CreateSlider( HWND hwndParent, UINT uID )
{
    HWND  hwndSlider ;

    if (!(hwndSlider = CreateWindow( SZ_SLIDERCLASS, "", 
                WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS | SLS_3DLOOK_OUT,
                0, 0, 1, 1,
                hwndParent, (HMENU)uID, ghInst, NULL )))
    {
        return NULL;
    }

    SendMessage( hwndSlider, SL_SETNUMCOLS, 4, 0L );
    SendMessage( hwndSlider, SL_SETCOLHEADER, NAME_COL, (LONG)GRCS(IDS_SLDRNAME) );
    SendMessage( hwndSlider, SL_SETCOLHEADER, PRICE_COL, (LONG)GRCS(IDS_SLDRPRICE) );
    SendMessage( hwndSlider, SL_SETCOLHEADER, SIZE_COL, (LONG)GRCS(IDS_SLDRSIZE) );
    SendMessage( hwndSlider, SL_SETCOLHEADER, PUB_COL, (LONG)GRCS(IDS_SLDRPUBLISHER) );

    SendMessage( hwndSlider, SL_SETCOLPOS, NAME_COL, (LONG)0L );
    SendMessage( hwndSlider, SL_SETCOLPOS, PRICE_COL, (LONG)40L );
    SendMessage( hwndSlider, SL_SETCOLPOS, SIZE_COL, (LONG)60L );
    SendMessage( hwndSlider, SL_SETCOLPOS, PUB_COL, (LONG)80L );

    return hwndSlider;

} //*** CreateSlider


//*************************************************************
//
//  CreateELB
//
//  Purpose:
//      Creates the ExtendedListbox
//
//
//  Parameters:
//      HWND hwndParent
//      UINT uID
//      
//
//  Return: (HWND FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 2/92   MSM        Created
//
//*************************************************************

HWND WINAPI CreateELB( HWND hwndParent, UINT uID )
{
    HWND hwndELB;

    hwndELB = CreateWindow( SZ_ELBCLASS, NULL, WS_VISIBLE|WS_CHILD|WS_BORDER,
            0, 0, 10, 10, hwndParent, (HMENU)ID_ELB, ghInst, NULL );

    if (hwndELB)
        ELBSetFont( hwndELB, ghfontUser );
    else
    {
        return NULL;
    }
    return hwndELB;

} //*** CreateELB


//*************************************************************
//
//  CreateFontView
//
//  Purpose:
//      Creates the ExtendedListbox
//
//
//  Parameters:
//      HWND hwndParent
//      UINT uID
//      
//
//  Return: (HWND FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 2/92   MSM        Created
//
//*************************************************************

HWND WINAPI CreateFontView( HWND hwndParent, UINT uID )
{
    HWND hwndFontView;

    hwndFontView = CreateWindow( SZ_FONTVIEWCLASS, NULL, WS_VISIBLE|WS_CHILD|WS_BORDER,
            0, 0, 10, 10, hwndParent, (HMENU)ID_FONTVIEW, ghInst, NULL );

    if (!hwndFontView)
    {
        return NULL;
    }
    return hwndFontView;

} //*** CreateFontView


//*** EOF: init.c
