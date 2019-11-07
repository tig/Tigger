/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  ENGINE 
 *
 *      Module:  ENGINE.c
 * 
 *      This module supports the filemanager extensions.
 *
 *   Revisions:  
 *       9/18/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "wfext.h"
#include "ENGINE.h"


/*
 * Filemananger globals
 */
static   HMENU      hmenu;
static   UINT       wMenuDelta;
static   BOOL       bMultiple = FALSE;
static   BOOL       bLFN = FALSE;
static   WNDPROC    lpfnWinFileOld ;
static   EXT_BUTTON btns[] =
        {
            {0,0,TBSTYLE_SEP},
            {IDB_WINPRINT,IDB_WINPRINT, 0}
        };

BOOL INTERNAL UnSubclassWinFile( HWND hwndWinFile ) ;
BOOL INTERNAL SubclassWinFile( HWND hwndWinFile ) ;
BOOL INTERNAL AddMenuItems( HWND hwnd ) ;
BOOL INTERNAL RemoveMenuItems( HWND hwnd ) ;

LRESULT CALLBACK fnWinFileSubclass( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

static HHOOK hhook ;
DWORD CALLBACK fnCBTProc( int code, WPARAM wParam, LPARAM lParam ) ;

/****************************************************************
 *  LRESULT WINAPI FMExtensionProc( HWND hwnd, UINT uiMsg, LPARAM lParam)
 *
 *  Description: 
 *
 *    File Manager extension callback.
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT WINAPI FMExtensionProc( HWND hwnd, UINT uiMsg, LPARAM lParam )
{
    switch (uiMsg)
    {
        case FMEVENT_LOAD:
            DP3( hWDB, "FMEVENT_LOAD" ) ;
            #define lpload  ((LPFMS_LOAD)lParam)
            
            /* Save the menu-item delta value. */
            
            wMenuDelta = lpload->wMenuDelta;
            
            /* Fill the FMS_LOAD structure. */
            
            lpload->dwSize = sizeof(FMS_LOAD);
            lstrcpy(lpload->szMenuName, "Win&Print");
            
            /* Return the handle of the menu. */
            
            lpload->hMenu = LoadMenu( ghmodDLL, MAKEINTRESOURCE(MNU_WINFILE) ) ;
            
            SubclassWinFile( hwnd ) ;

            return MAKELRESULT( lpload->hMenu, 0 ) ;
        break;
        
        case FMEVENT_UNLOAD:
        {
            DP3( hWDB, "FMEVENT_UNLOAD: HWND = %04X", hwnd ) ;
            /* Because of the fact that WinFile gives us the
             * FMEVENT_UNLOAD event during the processing of a SendMessage
             * and we have WinFile's WndProc subclassed, and winfile
             * will "FreeLibrary()" on us after we return from this
             * event..
             */
            if (GetModuleUsage( ghmodDLL ) > 1)
                UnSubclassWinFile( hwnd ) ;

        }
        break;

        case FMEVENT_INITMENU:
            
            /* Copy the menu-item delta value and menu handle. */
            
            wMenuDelta = LOWORD(lParam);
            hmenu = (HMENU)HIWORD(lParam);
            
            /*
             * Add check marks to menu items as appropriate. Add menu-item
             * delta value to menu-item identifiers to specify the menu
             * items to check.
             */
            
        break;
#if 0
        case FMEVENT_TOOLBARLOAD:
        {
            LPFMS_TOOLBARLOAD   lpTool = (LPFMS_TOOLBARLOAD)lParam ;

            lpTool->lpButtons = (LPEXT_BUTTON)&btns ;
            lpTool->cButtons = sizeof( btns ) / sizeof( EXT_BUTTON ) ;
            lpTool->cBitmaps = 1 ;
            lpTool->idBitmap = IDB_WINPRINT ;
        }
        break ;
#endif

        case FMEVENT_HELPSTRING:
        {
            LPFMS_HELPSTRING    lpHelp = (LPFMS_HELPSTRING)lParam ;
            LoadString( ghmodDLL, IDB_WINPRINT, lpHelp->szHelp,
                        sizeof( lpHelp->szHelp ) ) ;
        }
        break ;
        
        /*
         * The following messages are generated when the user selects
         * items from the WinPrint menu.
         */
        case IDM_PRINT:
        {
            FMS_GETFILESEL file;
            char           szFile[MAX_PATHLEN] ;
            char           szCfg[MAX_CONFIGNAMELEN] ;

            SendMessage( hwnd, FM_GETFILESEL, 0, (LONG)(LPFMS_GETFILESEL)&file ) ;
            OemToAnsi( file.szName, szFile ) ;

            if (!ftdConfigGetName( szCfg, szFile ))
            {
                /* Pop up a msg box saying that there is no configuration
                 * associated with this file extension.  Tell him to use
                 * "File.Associate" to set this up.
                 *
                 * Or pop up a list of file types to use with this file.
                 *
                 */
                ErrorResBox( hwnd, ghmodDLL, MB_ICONINFORMATION, 
                         IDS_APPTITLE, IDS_ERR_NOASSOCIATION ) ;
                break ;
            }

            engPrint( hwnd, file.szName, szCfg ) ;
        }    
        break ;
        
        case IDM_PRINTERSETUP:
        {
            LPENGINECONFIG lpEC ;
            FMS_GETFILESEL file;
            char           szCfg[MAX_CONFIGNAMELEN] ;

            SendMessage( hwnd, FM_GETFILESEL, 0, (LONG)(LPFMS_GETFILESEL)&file ) ;
            OemToAnsi( file.szName, file.szName ) ;

            if (!ftdConfigGetName( szCfg, file.szName ))
            {
                /* Pop up a msg box saying that there is no configuration
                 * associated with this file extension.  Tell him to use
                 * "File.Associate" to set this up.
                 *
                 * Or pop up a list of file types to use with this file.
                 *
                 */
                ErrorResBox( hwnd, ghmodDLL, MB_ICONINFORMATION, 
                         IDS_APPTITLE, IDS_ERR_NOASSOCIATION ) ;
                break ;
            }

            lpEC = engConfigAlloc() ;
            if (!engConfigRead( hwnd, szCfg, NULL, lpEC ))
            {
                DP1( hWDB, "engConfigRead Failed : Err = %d", engGetLastError() ) ;
                engConfigFree( lpEC ) ;
                break ;
            }
            
            if (engPrinterChoose( hwnd, lpEC ))
            {
                if (!engConfigWrite( hwnd, szCfg, NULL, lpEC ))
                {
                    DP1( hWDB, "engConfigWrite Failed : Err = %d", engGetLastError() ) ;
                }
            }                                                     

            engConfigFree( lpEC ) ;

        }    
        break ;
        
        case IDM_PAGESETUP:
        {
            LPENGINECONFIG lpEC ;
            FMS_GETFILESEL file;
            char           szCfg[MAX_CONFIGNAMELEN] ;

            SendMessage( hwnd, FM_GETFILESEL, 0, (LONG)(LPFMS_GETFILESEL)&file ) ;
            OemToAnsi( file.szName, file.szName ) ;

            if (!ftdConfigGetName( szCfg, file.szName ))
            {
                /* Pop up a msg box saying that there is no configuration
                 * associated with this file extension.  Tell him to use
                 * "File.Associate" to set this up.
                 *
                 * Or pop up a list of file types to use with this file.
                 *
                 */
                ErrorResBox( hwnd, ghmodDLL, MB_ICONINFORMATION, 
                         IDS_APPTITLE, IDS_ERR_NOASSOCIATION ) ;
                break ;
            }

            lpEC = engConfigAlloc() ;

            if (!engConfigRead( hwnd, szCfg, NULL, lpEC ))
            {
                DP1( hWDB, "engConfigRead Failed : Err = %d", engGetLastError() ) ;
                engConfigFree( lpEC ) ;
                break ;
            }
            
            if (engPageConfigure( hwnd, lpEC ))
            {
                if (!engConfigWrite( hwnd, szCfg, NULL, lpEC ))
                {
                    DP1( hWDB, "engConfigWrite Failed : Err = %d", engGetLastError() ) ;
                }
            }

            engConfigFree( lpEC ) ;
        }    
        break ;

        case IDM_ASSOCIATE:
        {
            FMS_GETFILESEL file;
            UINT    count ;

            count = (int) SendMessage( hwnd, FM_GETSELCOUNT, 0, 0L ) ;
            
            while (count >= 1)
            {
                /* Selection indexes are zero-based (0 is first). */
                count--;
                SendMessage( hwnd, FM_GETFILESEL, count, (LONG)(LPFMS_GETFILESEL)&file ) ;
                OemToAnsi( file.szName, file.szName ) ;
                
                if (engAssociate( hwnd, GetExtension( file.szName ), NULL ))
                {
                    /* Now we need to fool WinFile into updating it's association
                     * list.  We do this by using a CBT hook.
                     *
                     * - Install a CBT hook.
                     * - Start WinFile's Associate dialog.
                     * - The CBT hook will get a HCBT_ACTIVATE message and
                     *   Send WM_COMMAND, IDOK message to the dialog.
                     */ 
                    #ifdef WIN32
                    hhook = SetWindowsHookEx( WH_CBT, fnCBTProc, GetWindowInstance( hwnd ), 
                                                                 GetWindowTask( hwnd ) ) ;
                    #else
                    // Pass a module handle to get around bug in 3.1
                    hhook = SetWindowsHookEx( WH_CBT, fnCBTProc, 
                                              GetModuleHandle( MAKELP( GetWindowInstance( hwnd ), 0 ) ), 
                                                                 GetWindowTask( hwnd ) ) ;
                    #endif

                    if (hhook != NULL)
                    {
                        if (lpfnWinFileOld)
                            CallWindowProc( (WNDPROC)lpfnWinFileOld, hwnd, WM_COMMAND, 
                                            IDM_WF_ASSOCIATE, 0L ) ;
                        
                        DP( hWDB, "Back from WM_COMMAND IDM_WF_ASSOCIATE..." ) ; 
                           
                        UnhookWindowsHookEx( hhook ) ;
                    }
                }
            }
        }
        break ;
        
        case IDM_OPTIONS:
             DP3( hWDB, "IDM_OPTIONS" ) ;
        break ;
        
        case IDM_ABOUT:
             DP3( hWDB, "IDM_ABOUT" ) ;
//             engAboutBox( hwnd );
        break ;

        case IDM_HELP:
            DP3( hWDB, "IDM_HELP" ) ;
        break ;

        case IDM_HELPONHELP:
            DP3( hWDB, "IDM_HELPONHELP" ) ;
        break ;


#if 0        
        case IDM_GETFOCUS:
             wsprintf( szBuf, "Focus %d",
                       (int)SendMessage(hwnd, FM_GETFOCUS, 0, 0L ) ) ;
             DP( hWDB, szBuf ) ;
             MessageBox(hwnd, szBuf, "Focus", MB_OK);
        break;
        
        case IDM_GETCOUNT:
            count = (int)SendMessage(hwnd,
               bLFN ? FM_GETSELCOUNTLFN : FM_GETSELCOUNT, 0, 0L);
            
            wsprintf(szBuf, "%d files selected", count);
            MessageBox(hwnd, szBuf, "Selection Count", MB_OK);
        break;
        
        case IDM_GETFILE:
        {
            FMS_GETFILESEL file;
            
            count = (int) SendMessage(hwnd,
            bLFN ? FM_GETSELCOUNTLFN : FM_GETSELCOUNT,
            FMFOCUS_DIR, 0L);
            
            while (count >= 1)
            {
            
                /* Selection indexes are zero-based (0 is first). */
                
                count--;
                SendMessage(hwnd, FM_GETFILESEL, count,
                
                (LONG) (LPFMS_GETFILESEL)&file);
                OemToAnsi(file.szName, file.szName);
                wsprintf(szBuf, "file %s\nSize %ld", (LPSTR)file.szName,
                file.dwSize);
                
                MessageBox(hwnd, szBuf, "File Information", MB_OK);
                
                if (!bMultiple)
                    break;
            }
        }
        break;
        
        case IDM_GETDRIVE:
        {
            FMS_GETDRIVEINFO drive;
            
            SendMessage(hwnd, FM_GETDRIVEINFO, 0,
            
            (LONG) (LPFMS_GETDRIVEINFO)&drive);
            
            OemToAnsi(drive.szVolume, drive.szVolume);
            OemToAnsi(drive.szShare, drive.szShare);
            
            wsprintf(szBuf,
               "%s\nFree Space %ld\nTotal Space %ld\nVolume %s\nShare %s",
               (LPSTR) drive.szPath, drive.dwFreeSpace,
               drive.dwTotalSpace, (LPSTR) drive.szVolume,
               (LPSTR) drive.szShare);
            MessageBox(hwnd, szBuf, "Drive Info", MB_OK);
        
        }
        break;
        
        
        case IDM_LFN:
             bLFN = !bLFN;
        break;
        
        case IDM_MULTIPLE:
             bMultiple = !bMultiple;
        break;
        
        case IDM_REFRESH:
        case IDM_REFRESHALL:
             SendMessage(hwnd, FM_REFRESH_WINDOWS, uiMsg == IDM_REFRESHALL, 0L);
        break;
        
        case IDM_RELOAD:
            PostMessage(hwnd, FM_RELOAD_EXTENSIONS, 0, 0L);
        break;
#endif
    }
    
    return NULL ;

} /* FMExtensionProc()  */


LRESULT CALLBACK fnWinFileSubclass( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uiMsg )
    {
        case WM_COMMAND:
        {
            #ifdef WIN32
            WORD wNotifyCode = HIWORD(wParam);  
            WORD wID = LOWORD(wParam);
            HWND hwndCtl = (HWND)lParam;     
            #else
            WORD wNotifyCode = HIWORD(lParam) ;
            WORD wID = wParam ;
            HWND hwndCtl = (HWND)LOWORD(lParam) ;
            #endif

            DP5(hWDB, "WinFile->WM_COMMAND: %04X, %08lX", wParam, lParam ) ;

            switch (wID)
            {
                case IDM_WF_EXIT:
                    if (UnSubclassWinFile( hwnd ))
                    {
                        PostMessage( hwnd, uiMsg, wParam, lParam ) ;
                        return 0L ;
                    }
                break ;

                case IDM_WF_ASSOCIATE:
                    FMExtensionProc( hwnd, IDM_ASSOCIATE, lParam ) ;
                break ;

                case IDM_WF_PRINT:
                    FMExtensionProc( hwnd, IDM_PRINT, lParam ) ;
                return 0L ;

                case IDM_WP_PAGESETUP:      
                    FMExtensionProc( hwnd, IDM_PAGESETUP, lParam ) ;
                return 0L ;

                case IDM_WP_PRINTERSETUP:
                    FMExtensionProc( hwnd, IDM_PRINTERSETUP, lParam ) ;
                return 0L ;

                default:
                break ;
            }
        }
        break ;

        case WM_INITMENUPOPUP:
        {
            HMENU   hMenu = (HMENU)wParam ;
            UINT    ui = LOWORD( lParam ) ;
            LRESULT lr ;
            HWND    hwndMDIClient ;
            char    szBuf[64] ;
 
            hwndMDIClient = GetWindow( hwnd, GW_CHILD ) ;
            do
            {
                GetClassName( hwndMDIClient, szBuf, 64  ) ;
                if (lstrcmpi( szBuf, "MDIClient" ) == 0)
                    break ;
            } while (hwndMDIClient = GetWindow( hwndMDIClient, GW_HWNDNEXT )) ;

            if (hwndMDIClient)
                ui -= HIWORD(SendMessage(hwndMDIClient, WM_MDIGETACTIVE, 0, 0L ) ) ;

            /* Cheat!
             */
            lr = CallWindowProc( (WNDPROC)lpfnWinFileOld, hwnd, uiMsg, wParam, lParam ) ;

            /*  Disable print items if there are no selected files.
             */
            if (ui == 0)  // File menu
            {
                ui = GetMenuState( hMenu, IDM_WF_PRINT, MF_BYCOMMAND ) ;
                if (GetMenuString( hMenu, IDM_WP_PAGESETUP, szBuf, 64, MF_BYCOMMAND ))
                    EnableMenuItem( hMenu, IDM_WP_PAGESETUP, ui | MF_BYCOMMAND ) ;
                if (GetMenuString( hMenu, IDM_WP_PRINTERSETUP, szBuf, 64, MF_BYCOMMAND ))
                    EnableMenuItem( hMenu, IDM_WP_PRINTERSETUP, ui | MF_BYCOMMAND ) ;

            }
            return lr ;
        }
        break ;

        /* anything that would cause WinFile to close down needs to
         * try to un-subclass it.
         */
        case WM_SYSCOMMAND:
            DP5( hWDB, "WinFile->WM_SYSCOMMAND = %04X", hwnd ) ;
            if (wParam == SC_CLOSE)
            {
                if (UnSubclassWinFile( hwnd ))
                {
                    PostMessage( hwnd, uiMsg, wParam, lParam ) ;
                    return 0L ;
                }
            }
        break ;

        case WM_CLOSE:    
            DP5( hWDB, "WinFile->WM_CLOSE = %04X", hwnd ) ;
            if (UnSubclassWinFile( hwnd ))
            {
                PostMessage( hwnd, uiMsg, wParam, lParam ) ;
                return 0L ;
            }
        break ;
            
        case WM_DESTROY:
            DP1( hWDB, "WinFile->WM_DESTROY = %04X", hwnd ) ;
        break ;
    }
    
    if (lpfnWinFileOld)
    {
        return CallWindowProc( (WNDPROC)lpfnWinFileOld, hwnd, uiMsg, wParam, lParam ) ;
    }
    else
    {
        DP1( hWDB, "Attempt to CallWindowProc on a NULL pointer!" ) ;
        return CallWindowProc( (WNDPROC)GetWindowLong( hwnd, GWL_WNDPROC ), 
                                        hwnd, uiMsg, wParam, lParam ) ;
    }
}

BOOL INTERNAL SubclassWinFile( HWND hwndWinFile )
{
    DP5( hWDB, "Subclassing  %#04X", hwndWinFile ) ;

    if (lpfnWinFileOld != (WNDPROC)GetWindowLong( hwndWinFile, GWL_WNDPROC ))
    {
        if (lpfnWinFileOld = SubclassWindow(hwndWinFile, fnWinFileSubclass ))
        {
            DP5( hWDB, "    sub-classed!" ) ;

            AddMenuItems( hwndWinFile ) ;
        }
        else
        {
            DP5( hWDB, "    SubclassWindow failed!!" ) ;
            return FALSE ;
        }
    }
    else
    {
        DP5( hWDB, "    Already sub-classed" ) ;
        return TRUE ;
    }

    return TRUE ;
}

BOOL INTERNAL UnSubclassWinFile( HWND hwndWinFile )
{
    /* Remove the subclass
     */
    WNDPROC lpfn ;
    DP5( hWDB, "Un-Subclassing %#04X", hwndWinFile ) ;

    if (hwndWinFile == NULL || !IsWindow( hwndWinFile ))
        if (!(hwndWinFile = FindWindow( "WFS_Frame", NULL )))
            return FALSE ;

    if (lpfnWinFileOld)
    {
        if (!(lpfn = (WNDPROC)GetWindowLong( hwndWinFile, GWL_WNDPROC )))
        {
            DP5( hWDB, "   Invalid hwnd (%04X)", hwndWinFile ) ;
            return FALSE ;
        }

        if (fnWinFileSubclass == lpfn)
        {
            SetWindowLong( hwndWinFile, GWL_WNDPROC, (LONG)lpfnWinFileOld ) ;
            DP5( hWDB, "    Un-subclassed!" ) ;
            lpfnWinFileOld = NULL ;

            RemoveMenuItems( hwndWinFile ) ;
        }
        else
        {
            DP5( hWDB, "    No need because it's not subclassed" ) ;
            lpfnWinFileOld = NULL ;
            return FALSE ;

        }
    }
    else
    {
        DP5( hWDB, "        No need because the window isn't there anymore" );
        return FALSE ;
    }

    return TRUE ;

}

BOOL INTERNAL AddMenuItems( HWND hwnd )
{
    HMENU hmenu ;
    char  szBuf[64] ;

    if (hmenu = GetMenu( hwnd ))
    {
        /* Make damn sure this is the same menu we're used to 
         * dealing with, and someone else hasn't fucked with it
         */
        GetMenuString( hmenu, IDM_WF_PRINT, szBuf, 64, MF_BYCOMMAND ) ;
        if (0 == lstrcmp( szBuf, GRCS( IDM_WF_PRINT )) &&
            !GetMenuString( hmenu, IDM_WP_PAGESETUP, szBuf, 64, MF_BYCOMMAND ))
        {
            if (!InsertMenu( hmenu, IDM_WF_ASSOCIATE, 
                                    MF_BYCOMMAND | MF_STRING, 
                                    IDM_WP_PAGESETUP, 
                                    GRCS( IDM_WP_PAGESETUP )))
            {
                DP1( hWDB, "Could not insert menu item (%s)", GRCS(IDM_WP_PAGESETUP) ) ;
                return FALSE ;
            }

            if (!InsertMenu( hmenu, IDM_WF_ASSOCIATE, 
                                    MF_BYCOMMAND | MF_STRING, 
                                    IDM_WP_PRINTERSETUP, 
                                    GRCS( IDM_WP_PRINTERSETUP )))
            {
                DP1( hWDB, "Could not insert menu item (%s)", GRCS(IDM_WP_PRINTERSETUP) ) ;
                return FALSE ;
            }

        }
        else
        {
            DP1( hWDB, "Something is different about WinFile's menu!" ) ;
            return FALSE ;
        }
    }
    else
    {
        DP1( hWDB, "GetMenu( %04X ) on WinFile failed!", hwnd ) ;
        return FALSE ;
    }
    return TRUE ;
}

BOOL INTERNAL RemoveMenuItems( HWND hwnd )
{
    HMENU hmenu ;
    char  szBuf[64] ;

    if (hmenu = GetMenu( hwnd ))
    {
        /* Make damn sure this is the same menu we're used to 
         * dealing with, and someone else hasn't fucked with it
         */
        GetMenuString( hmenu, IDM_WF_PRINT, szBuf, 64, MF_BYCOMMAND ) ;
        if (0 == lstrcmp( szBuf, GRCS( IDM_WF_PRINT )))
        {
            if (GetMenuString( hmenu, IDM_WP_PAGESETUP, szBuf, 64, MF_BYCOMMAND ))
                DeleteMenu( hmenu, IDM_WP_PAGESETUP, MF_BYCOMMAND ) ;

            if (GetMenuString( hmenu, IDM_WP_PRINTERSETUP, szBuf, 64, MF_BYCOMMAND ))
                DeleteMenu( hmenu, IDM_WP_PRINTERSETUP, MF_BYCOMMAND ) ;
        }
        else
        {
            DP1( hWDB, "Something is different about WinFile's menu!" ) ;
            return FALSE ;
        }
    }
    else
    {
        DP1( hWDB, "GetMenu( %04X ) on WinFile failed!", hwnd ) ;
        return FALSE ;
    }

    return TRUE ;
}

BOOL WINAPI engWinFileInstall( VOID )
{
    HWND hwndFM ;
    char szFile[256] ;

    GetModuleFileName( ghmodDLL, szFile, 256 ) ;
    WritePrivateProfileString( "AddOns", 
                               "WinPrint 2.0",  
                               szFile, 
                               "WINFILE.INI" ) ;

    if (hwndFM = FindWindow( "WFS_Frame", NULL ))
        PostMessage( hwndFM, FM_RELOAD_EXTENSIONS, 0, 0L ) ;
    
    return TRUE ;
}

BOOL WINAPI engWinFileRemove( VOID )
{
    HWND hwndFM ;

    WritePrivateProfileString( "AddOns", 
                               "WinPrint 2.0", 
                               NULL, 
                               "WINFILE.INI" ) ;

    if (hwndFM = FindWindow( "WFS_Frame", NULL ))
        PostMessage( hwndFM, FM_RELOAD_EXTENSIONS, 0, 0L ) ;

    return TRUE ;
}

DWORD CALLBACK fnCBTProc( int code, WPARAM wParam, LPARAM lParam )
{
    switch (code)
    {
        case HCBT_ACTIVATE:
        {
            HWND            hwnd = (HWND)wParam ;
            char            szTitle[32] ;

            szTitle[0] = '\0' ;
           
            GetWindowText( hwnd, szTitle, 32 ) ;


            if (lstrcmpi( szTitle, "Associate" ) == 0)
            {
                DP1( hWDB, "Sending IDOK to %04X", GetDlgItem( hwnd, IDOK ) ) ;

                #ifdef WIN32
                PostMessage( GetDlgItem( hwnd, IDOK ), WM_COMMAND, 
                #else
                SendMessage( hwnd, WM_COMMAND, IDOK, 
                             MAKELPARAM( GetDlgItem( hwnd, IDOK ), BN_CLICKED ) ) ;
                #endif
                DP1( hWDB, "Done Sending IDOK to %04X", GetDlgItem( hwnd, IDOK ) ) ;

                return 1 ;

            }
        }
        break ;
    }

    return CallNextHookEx( hhook, code, wParam, lParam ) ;
}



/************************************************************************
 * End of File: ENGINE.c
 ***********************************************************************/
