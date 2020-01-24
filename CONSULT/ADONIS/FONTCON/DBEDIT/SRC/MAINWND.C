//*************************************************************
//  File name: mainwnd.c
//
//  Description: 
//      Handles the dialog box and it procedure
//
//  History:    Date       Author     Comment
//               2/13/92   MSM        Created
//
//*************************************************************

#include "global.h"
#include <commdlg.h>
#include <memory.h>

//*************************************************************
//
//  MainWndProc
//
//  Purpose:
//      Window procedure for the main window
//
//
//  Parameters:
//      HWND hWnd
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (BOOL FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/13/92   MSM        Created
//
//*************************************************************

LONG CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            //*** Create status bar
            if (ghwndStat = CreateStatus( hWnd, ID_STATUS ))
            {
                SetWindowText( ghwndStat, GRCS(IDS_STAT_READY) ) ;
                ShowWindow( ghwndStat, SW_NORMAL ) ;
            }
            else
                return -1L;
            
            //*** Create slider
            if (ghwndSlider = CreateSlider( hWnd, ID_SLIDER ))
                ShowWindow( ghwndSlider, SW_NORMAL ) ;
            else
                return -1L;

            //*** Create ExtendedListbox
            if (ghwndELB = CreateELB( hWnd, ID_ELB ))
                ShowWindow( ghwndELB, SW_NORMAL ) ;
            else
                return -1L;

            ELBSetDatabase( ghwndELB, glpDB );

            //*** Create Font thumb nail Viewer
            if (ghwndFontView = CreateFontView( hWnd, ID_FONTVIEW ))
                ShowWindow( ghwndFontView, SW_NORMAL ) ;
            else
                return -1L;
        }
        break;

        case WM_INITMENUPOPUP:
        case WM_INITMENU:
        case WM_MENUSELECT:
            return MenuHandler( msg, wParam, lParam );
        break;

        case WM_GETMINMAXINFO:
        case WM_SIZE:
            SizeHandler( hWnd, msg, wParam, lParam );
        break;

        case WM_COMMAND:
            return CmdHandler(wParam,lParam);
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            BeginPaint( hWnd, &ps );

            tdDrawDropShadow( ps.hdc, ghwndSlider, SHADOW );
            tdDrawDropShadow( ps.hdc, ghwndELB, SHADOW );
            tdDrawDropShadow( ps.hdc, ghwndFontView, SHADOW );

            EndPaint( hWnd, &ps );
        }
        break;

        case WM_CLOSE:
            SaveConfiguration();
        break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
        break;
    }
    return DefWindowProc( hWnd, msg, wParam, lParam );

} //*** MainWndProc


//*************************************************************
//
//  CmdHandler
//
//  Purpose:
//      Handles all the WM_COMMAND messages for the main window
//
//
//  Parameters:
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (LRESULT WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

LRESULT WINAPI CmdHandler(WPARAM wParam,LPARAM lParam)
{
    switch (wParam)
    {
        case IDM_FILE_EXIT:
            PostMessage( ghwndMain, WM_SYSCOMMAND, SC_CLOSE, 0L );
        break;

        case IDM_FILE_FONT:
            DoChooseFont();
        break;

        case ID_SLIDER:
            SyncDBELBToSlider( ghwndELB, ghwndSlider );
        break;

        case ID_ELB:
        {
            if (HIWORD(lParam)==ELBN_SELCHANGE)
            {
                FVSetCurrentNode( ghwndFontView, 
                    (LPDBNODE)ELBGetCurSel( ghwndELB, NULL ) );
            }
            return HandleDBELBCommand(ghwndELB, WM_COMMAND, wParam, lParam);
        }
        break;

        case IDM_FILE_IMPORT:
        {
            OPENFILENAME of;
            char         szFile[120];

            memset( &of, 0, sizeof(OPENFILENAME) );

            szFile[0] = 0;

            of.lStructSize  = sizeof(OPENFILENAME);
            of.hwndOwner    = ghwndMain;
            of.hInstance    = ghInst;
            of.lpstrFilter = (LPSTR)"PubInfo Files\0*.PUB\0\0" ;
            of.nFilterIndex = 0;
            of.lpstrFile    = (LPSTR)szFile;
            of.nMaxFile     = (DWORD)sizeof(szFile);
            of.lpstrTitle = (LPSTR)"Import Publisher";
            of.Flags        = OFN_HIDEREADONLY|OFN_FILEMUSTEXIST;
            of.lpstrDefExt  = (LPSTR)"PUB";

            if( !GetOpenFileName( &of ) )
                return 0L;

            EmptyDataBase( glpDB );
            if (LoadPubInfo( glpDB, szFile ))
            {
                SyncDBELBToSlider( ghwndELB, ghwndSlider );
                FillDBELB( ghwndELB ); 
            }
        }
        break;

        case IDM_FILE_SAVE:
        {
            BOOL fSave;

            SetCursor( LoadCursor( NULL, IDC_WAIT ) );
            if (!SaveDataBase( glpDB, 0 ))
            {
                LPSTR lp = "";

                ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                    IDS_SAVEFAILED, lp );
                SetCursor( LoadCursor( NULL, IDC_ARROW ) );
                return FALSE;
            }
            fSave = DBSaveStuffToMakeDLLs( glpDB );
            SetWindowText( ghwndStat, GRCS(IDS_STAT_READY) ) ;
            SetCursor( LoadCursor( NULL, IDC_ARROW ) );

            return (LRESULT)fSave;
        }
        break;
    }
    return DefWindowProc( ghwndMain, WM_COMMAND, wParam, lParam );

} //*** CmdHandler


//*************************************************************
//
//  SizeHandler
//
//  Purpose:
//      Handles the sizing of the window
//
//
//  Parameters:
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (LRESULT WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

LRESULT WINAPI SizeHandler( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if (!ghwndStat || !ghwndSlider || !ghwndELB || !ghwndFontView)
        return DefWindowProc( hwnd, msg, wParam, lParam ) ;

    if (msg == WM_SIZE)
    {
        HDWP hdwp ;
        short dy = HIWORD( lParam );
        short dx = LOWORD( lParam );
        short dh, dyThumb = THUMB_CY+THUMB_PAD;
        short cx = BLANKSPACE, cy = dy;

        if (!(hdwp = BeginDeferWindowPos( 1 )))
            return DefWindowProc( hwnd, msg, wParam, lParam ) ;

        dh = (short)SendMessage(ghwndStat,ST_GETHEIGHT,0,(LPARAM)0L);

        //*** Resize Status
        cy -= dh-1;
        hdwp = DeferWindowPos( hdwp, ghwndStat, NULL,
                    -1, cy, dx+2, dh, SWP_NOZORDER ) ;
        dx -= 2*BLANKSPACE + SHADOW;

        //*** Resize FontView
        cy -= BLANKSPACE + SHADOW + dyThumb;
        hdwp = DeferWindowPos( hdwp, ghwndFontView, NULL,
                    cx, cy, dx, dyThumb, SWP_NOZORDER ) ;

        //*** Resize Slider
        hdwp = DeferWindowPos( hdwp, ghwndSlider, NULL,
                    cx, BLANKSPACE,dx,dh, SWP_NOZORDER ) ;
        
        //*** Resize ELB
        dy = cy - (2*BLANKSPACE) - SHADOW - dh + 1;
        cy = BLANKSPACE + dh - 1;
        dh = dy;

        hdwp = DeferWindowPos( hdwp, ghwndELB, NULL,
                    cx, cy, dx, dh, SWP_NOZORDER ) ;

        EndDeferWindowPos( hdwp ) ;
        SyncDBELBToSlider( ghwndELB, ghwndSlider );
        return (LRESULT)0L ;
    }

    if (msg==WM_GETMINMAXINFO)
    {
        MINMAXINFO FAR *lpmmi = (MINMAXINFO FAR *)lParam;
        short dh, dw;

        dh = (short)SendMessage(ghwndStat,ST_GETHEIGHT,0,(LPARAM)0L);
        dh = 8*(dh+SHADOW) + 3*BLANKSPACE + THUMB_CY;
        dw = GetSystemMetrics( SM_CXMINTRACK ) * 4;

        lpmmi->ptMinTrackSize.x = dw;
        lpmmi->ptMinTrackSize.y = dh;
    }
    return DefWindowProc( hwnd, msg, wParam, lParam ) ;

} //*** SizeHandler


//*************************************************************
//
//  MenuHandler
//
//  Purpose:
//      Handles all the menu help and check marking
//
//
//  Parameters:
//      UINT wMsg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (LRESULT FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

LRESULT WINAPI MenuHandler( UINT wMsg, WPARAM wParam, LPARAM lParam )
{
    static HMENU hMenu;

    switch ( wMsg )
    {
        case WM_INITMENU:
            // 
            // Get the handle to the window here, this way we store
            // it a way and dont have to waste time getting it later
            //
            hMenu = (HMENU)wParam ;
        break ;

        case WM_INITMENUPOPUP:
            return DefWindowProc( ghwndMain, wMsg, wParam, lParam ) ;
        break ;

        case WM_MENUSELECT:
            if (!IsWindowVisible(ghwndStat))
            {
                break ;
            }

            if (LOWORD(lParam) == 0xFFFF || HIWORD(lParam) == 0 || (WORD)wParam == 0)
            {
                SetWindowText( ghwndStat, GRCS(IDS_STAT_READY) ) ;
                break ;
            }

            if (LOWORD(lParam) & MF_HILITE)
            {   
                if ((LOWORD(lParam) & MF_POPUP))
                {
                    WORD wIndex;

                    for (wIndex=IDS_POPUPMENUSTART; wIndex<=IDS_POPUPMENUEND; wIndex++)
                    {
                        if (GetSubMenu( hMenu, wIndex-IDS_POPUPMENUSTART ) 
                            == (HMENU)wParam)
                        {
                            SetWindowText( ghwndStat, GRCS(wIndex) ) ;
                            return DefWindowProc( ghwndMain, wMsg, wParam, lParam ) ;
                        }
                    }
                    SetWindowText( ghwndStat, " " ) ;
                }
                else /* if MF_POPUP */
                {
                    if (LOWORD((DWORD)lParam & MF_SYSMENU))
                    {
                        // 
                        // It is a system menu item.
                        //
                        switch ((WORD)wParam)
                        {
                            case SC_SIZE:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_SIZE) ) ;
                            break ;

                            case SC_MOVE:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_MOVE) ) ;
                            break ;

                            case SC_MINIMIZE:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_MINIMIZE) ) ;
                            break ;

                            case SC_MAXIMIZE:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_MAXIMIZE) ) ;
                            break ;

                            case SC_CLOSE:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_CLOSE) ) ;
                            break ;

                            case SC_RESTORE:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_RESTORE) ) ;
                            break ;

                            case SC_TASKLIST:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_TASKLIST) ) ;
                            break ;
                        }
                    } /* MF_SYSMENU */
                    else
                    {
                        //
                        // It's a normal menu item, wParam has the IDM
                        //
                        SetWindowText( ghwndStat, GRCS(wParam) ) ;
                    }
                }
            } /* MF_HILITE */

        break ;
    }
    
    return DefWindowProc( ghwndMain, wMsg, wParam, lParam ) ;

} //*** MenuHandler

//*** EOF: mainwnd.c
