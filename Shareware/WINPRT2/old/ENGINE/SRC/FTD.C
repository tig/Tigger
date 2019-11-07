/************************************************************************
 *                
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  FTD
 *
 *      Module:  FTD.C
 * 
 *      This module contains the file type driver support routines.
 *      These routines are used by the file type drivers (FTDs) and
 *      file type driver dlls (FTDDs) to manage file types.
 *
 *   Revisions:  
 *       9/21/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include <shellapi.h>

#include "ENGINE.h"

#include "FTD.H"

LRESULT CALLBACK fnFTDChoose( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

/*----------------------------------------------------------------------
 * Functions for obtaining info about an FTD
 */

/* Given a file name (in lpszFN), return configuration name in the buffer pointed
 * to by lpszCfg
 */
LPSTR WINAPI ftdConfigGetName( LPSTR lpszCfg, LPSTR lpszFN )
{
    LPSTR   lpszExt ;  // points to period
    char    szFile[MAX_PATHLEN] ;
    char    szTemp[MAX_PATHLEN] ;
    LONG    cb ;
       
    lstrcpy( szFile, lpszFN ) ;
     
    /* Look in registry to get root for this extension.
     * If the root exists, follow it to the
     * name.
     * Otherwise, look in the WINPRINT.INI file for the
     * user's default configuration name
     */
    StripPath( szFile ) ;
    if (lpszExt = GetExtension( szFile ))
    {
        /* Since GetExtension skips the '.', we jump back one
         * to get the '.'.
         */
        lpszExt-- ;  
        cb = sizeof( szTemp ) ;
        if (RegQueryValue( HKEY_CLASSES_ROOT, lpszExt, szTemp, &cb ) == ERROR_SUCCESS)
        {
            /* szTemp now contains short name
             */
            cb = MAX_CONFIGNAMELEN ;
            if (RegQueryValue( HKEY_CLASSES_ROOT, szTemp, lpszCfg, &cb ) == ERROR_SUCCESS)
            {
                DP3( hWDB, "Configuration = '%s'", (LPSTR)lpszCfg ) ;
                return lpszCfg ;
            }
            else
            {
                /* This means that the extension is registered, but the '.ext key name'
                 * is not associated with a class.
                 *
                 * This is an error.  The user should be told about it, and should
                 * be forced to fix it.  Do not attempt to continue!
                 */
                DP1( hWDB, "Extension, %s, has an .ext key name (%s), but no class in the registry!", (LPSTR)lpszExt, (LPSTR)szTemp ) ;
                return NULL ;
            }
        }
    }

    /* This means that the thing's not registered at all.
     */
#if 0   // it's up to the filetype driver to decide this!

    /* First make sure he's not trying to print a program.
     */
    if (IsProgram( szFile ))
    {
        DP1( hWDB, "Idiot! Don't try to print a program!" ) ;
        return NULL ;
    }
#endif
     
    /* If the guy has set is global configuration to say "use the             
     * default configuration for any files that don't have
     * associations" then get him the default conifg.  Otherwise
     * fail.
     */
//    if (gfAutoAssociate)
    {
        /* Return whatever the default configuration is...
         * 
         * We store the default configuration in WINPRINT.INI
         */
        DP3( hWDB, "Returning default configuration" ) ;

        return NULL /* lpszDefault */ ;

    }

    return NULL ;
}

/* Simple wrapper around FTD to get config info.
 */
BOOL WINAPI ftdConfigRead( HWND hwnd, LPENGINECONFIG lpEC, LPSTR lpszSection, LPSTR lpszFile )
{
    FTDPROC     lpfnFTD ;

    /* First make sure we actually have FTD info at all! 
     */
    if (lpEC->szFTDD[0] == '\0' || lpEC->szFTD[0] == '\0')
    {
        DP3( hWDB, "No FTD specified for read!" ) ;
        return FALSE ;
    }

    if (lpfnFTD = ftdLoadFTD( lpEC->szFTDD, lpEC->szFTD ))
    {
        /* Read config.  
         */
        if (!(*lpfnFTD)( FTD_READCONFIG, (LPARAM)&lpEC->FTDInfo, 
                         (LPARAM)lpszSection, (LPARAM)lpszFile ))
        {
            DP1( hWDB, "FTD_READCONFIG failed!" ) ;
//            ftdFreeFTD( lpfnFTD ) ;
//            return FALSE ;
        }
        ftdFreeFTD( lpfnFTD ) ;
        return TRUE ;
    }
    
    return FALSE ;
}   

/* Simple wrapper around FTD to allocate lpData struct
 */
BOOL WINAPI ftdConfigInit( HWND hwnd, LPENGINECONFIG lpEC )
{
    FTDPROC     lpfnFTD ;

    /* First make sure we actually have FTD info at all! 
     */
    if (lpEC->szFTDD[0] == '\0' || lpEC->szFTD[0] == '\0')
    {
        DP3( hWDB, "No FTD specified for init!" ) ;
        return FALSE ;
    }

    if (lpfnFTD = ftdLoadFTD( lpEC->szFTDD, lpEC->szFTD ))
    {
        /* Read config.  
         */
        if (!(*lpfnFTD)( FTD_INITCONFIG, (LPARAM)&lpEC->FTDInfo, 0L, 0L ))
        {
            DP1( hWDB, "FTD_INITCONFIG failed!" ) ;
            ftdFreeFTD( lpfnFTD ) ;
            return FALSE ;
        }
        ftdFreeFTD( lpfnFTD ) ;
        return TRUE ;
    }
    return FALSE ;
}   


BOOL WINAPI ftdChoose( HWND hwndParent, LPENGINECONFIG lpEC ) 
{
    FTDINFO     FTDInfo ;
    char        szFTD[MAX_CONFIGNAMELEN], szFTDD[MAX_PATHLEN] ;

    /* Copy FTDInfo structure of ENGINECONFIG
     */
    lstrcpy( szFTD, lpEC->szFTD ) ;
    lstrcpy( szFTDD, lpEC->szFTDD ) ;
    FTDInfo.uiStructSize = lpEC->FTDInfo.uiStructSize ;
    if (FTDInfo.uiStructSize)
    {
        FTDInfo.lpData = (LPARAM)GlobalAllocPtr( GHND, FTDInfo.uiStructSize ) ;
        _fmemcpy( (LPSTR)FTDInfo.lpData, (LPSTR)lpEC->FTDInfo.lpData, FTDInfo.uiStructSize ) ;
    }

    if (DialogBoxParam( ghmodDLL, MAKEINTRESOURCE( DLG_FTDCONFIGURE ), 
                            hwndParent, (DLGPROC)fnFTDChoose, (LPARAM)lpEC ))
    {
        /* Something was changed, so *don't* copy back into lpEC
         */
        if (FTDInfo.uiStructSize)
            GlobalFreePtr( FTDInfo.lpData ) ;
        return TRUE ;
    }
    else
    {
        /* revert back to original
         */
        lstrcpy( lpEC->szFTD, szFTD ) ;
        lstrcpy( lpEC->szFTDD, szFTDD ) ;
        if (FTDInfo.uiStructSize)
        {
            _fmemcpy( (LPSTR)lpEC->FTDInfo.lpData, (LPSTR)FTDInfo.lpData, FTDInfo.uiStructSize ) ;
            GlobalFreePtr( FTDInfo.lpData ) ;
        }

        return FALSE ;
    }
}

LRESULT CALLBACK fnFTDChoose( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    LPENGINECONFIG lpEC ;

    switch( uiMsg )
    {
        case WM_INITDIALOG:
        {
            HCURSOR hcur ;
            int     nTab ;
            RECT    rc ;
    
            hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

            SetWindowLong( hDlg, DWL_USER, lParam ) ;
            lpEC = (LPENGINECONFIG)lParam ;

            /* Fill list box with all FTDs
             *
             * Use DaveE's trick of tabbing extra data off to the
             * right in the list box.  That's were we'll store the
             * szFTDD!
             */
            GetClientRect( GetDlgItem( hDlg, IDD_FILETYPEDRIVER ), &rc ) ;
            nTab = (rc.right - rc.left + 10) ;
            ListBox_SetTabStops( GetDlgItem( hDlg, IDD_FILETYPEDRIVER ), 
                                 1, &nTab ) ;

            ftdFillListBoxWithFTDs( GetDlgItem( hDlg, IDD_FILETYPEDRIVER ) ) ;

            if (!ftdSelectFTDInListBox( GetDlgItem( hDlg, IDD_FILETYPEDRIVER ), 
                                        lpEC->szFTDD, lpEC->szFTD ))
                ListBox_SetCurSel( GetDlgItem( hDlg, IDD_FILETYPEDRIVER ), 0 ) ;

            DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;

            SetCursor( hcur ) ;

            SetFocus( GetDlgItem( hDlg, IDD_FILETYPEDRIVER ) ) ;
            Ctl3dSubclassDlg( hDlg, CTL3D_ALL ) ;
            return FALSE ;
        }
        break ;

        case WM_CTLCOLOR:
            return (BOOL)Ctl3dCtlColorEx( uiMsg, wParam, lParam ) ;
        break ;

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

            if (!(lpEC = (LPENGINECONFIG)GetWindowLong( hDlg, DWL_USER )))
            {
                DASSERT( hWDB, lpEC ) ;
                EndDialog( hDlg, FALSE ) ;
            }
                
            switch (wID)
            {
                case IDOK:
                {
                    if (ftdGetSelectedFTDFromListBox( GetDlgItem( hDlg, 
                                                    IDD_FILETYPEDRIVER ),
                                                  lpEC->szFTDD, lpEC->szFTD ))
                    {
                        ftdConfigInit( hDlg, lpEC ) ;
                        EndDialog( hDlg, TRUE ) ;
                    }                                  
                    else
                        EndDialog( hDlg, FALSE ) ;
                }
                break ;

                case IDCANCEL:
                    EndDialog (hDlg, FALSE) ;
                break ;

                case IDD_HELP:
                    DASSERTMSG( hWDB, 0, "Yo! No help yet." ) ;   
                break ;

                default :
                   return FALSE ;
            }
        }
        break ;

        default:
            return FALSE ;
   }

   return TRUE ;
}

/* Simple wrapper around FTD to write config info.
 */
BOOL WINAPI ftdConfigWrite( HWND hwnd, LPENGINECONFIG lpEC, LPSTR lpszSection, LPSTR lpszFile )
{
    FTDPROC     lpfnFTD ;

    /* First make sure we actually have FTD info at all! 
     */
    if (lpEC->szFTDD[0] == '\0' || lpEC->szFTD[0] == '\0')
    {
        DP3( hWDB, "No FTD info to write" ) ;
        return FALSE ;
    }

    if (lpfnFTD = ftdLoadFTD( lpEC->szFTDD, lpEC->szFTD ))
    {
        /* Write config.  
         */
        if (!(*lpfnFTD)( FTD_WRITECONFIG, (LPARAM)&lpEC->FTDInfo, 
                         (LPARAM)lpszSection, (LPARAM)lpszFile ))
        {
            DP1( hWDB, "FTD_WRITECONFIG failed!" ) ;
//            ftdFreeFTD( lpfnFTD ) ;
//            return FALSE ;
        }
        ftdFreeFTD( lpfnFTD ) ;
        return TRUE ;
    }
    
    return FALSE ;
} 

/* Given a FTDD and FTD name return a pointer to the FTD proc.
 *
 * This function loads the DLL with LoadLibrary(), as such
 * the function ftdFreeFTD should be called when the caller is
 * done using the proc address.
 */
FTDPROC WINAPI ftdLoadFTD( LPSTR lpszFTDD, LPSTR lpszFTD ) 
{
    HMODULE hmodFTDD ;
    char    szFTDD[144] ;
    FTDPROC lpfnFTD = NULL ;
    LPFN_GPA lpfnGPA = NULL ;

    VALIDATEPARAM( lpszFTDD ) ;
    VALIDATEPARAM( lpszFTD ) ;
    
    wsprintf( szFTDD, "%s.FTD", (LPSTR)lpszFTDD ) ;
    if (HINSTANCE_ERROR >= (hmodFTDD = LoadLibrary( szFTDD )))
    {
        DP1( hWDB, "LoadLibrary failed on %s", (LPSTR)szFTDD ) ;
        #pragma message( "Set error code to appropriate error" )
        return NULL ;
    }

    if (!(lpfnGPA = (LPFN_GPA)GetProcAddress( hmodFTDD, MAKEINTRESOURCE( ORD_FTDD_GPA ) )))
    {
        DP1( hWDB, "Could not get proc address of GPA function!" ) ;
        FreeLibrary( hmodFTDD ) ;
        return NULL ;
    }

    if (!(lpfnFTD = (*lpfnGPA)( lpszFTD )))
    {
        DP1( hWDB, "The FTD (%s) does not exist in %s!", lpszFTD, lpszFTDD ) ;
        FreeLibrary( hmodFTDD ) ;
        return NULL ;
    }

    return lpfnFTD ;    
}

/* Given a FTDPROC, undo a call to ftdLoadFTD
 */
BOOL WINAPI ftdFreeFTD( FTDPROC lpfnFTD )
{
    VALIDATEPARAM( lpfnFTD ) ;

    FreeLibrary( (HMODULE)(*lpfnFTD)( FTD_GETMODULEHANDLE, 0L, 0L, 0L ) ) ;
    return TRUE ;
}

/*----------------------------------------------------------------------
 * Functions for obtaining the currently available FTDDs and FTDs.
 */
LPSTR WINAPI FTDD_FindFirstFTD( LPSTR lpszFTD_Name ) ;
LPSTR WINAPI FTDD_FindNextFTD( LPSTR lpszPrevFTD, LPSTR lpszFTD_Name ) ;

BOOL WINAPI ftdFillListBoxWithFTDs( HWND hwnd ) 
{
    char    szFTD[MAX_CONFIGNAMELEN], szFTDD[MAX_PATHLEN] ;
    char    szBuf[MAX_CONFIGNAMELEN+MAX_PATHLEN+1] ;
    char    szPath[MAX_PATHLEN] ;
    
    SetWindowRedraw( hwnd, FALSE ) ;

    /* Reset the contents */
    ListBox_ResetContent( hwnd ) ;

    /* ENGINE.DLL FTDs first */
    GetModuleFileName( ghmodDLL, szFTDD, MAX_PATHLEN ) ;
    StripPath( szFTDD ) ;
    if (FTDD_FindFirstFTD( szFTD ))
        do
        {
            wsprintf( szBuf, "%s\t,%s", (LPSTR)szFTD, (LPSTR)szFTDD ) ;
            if (LB_ERR == ListBox_FindString( hwnd, 0, szBuf ))
                ListBox_AddString( hwnd, szBuf ) ; 
        
        } while (FTDD_FindNextFTD( szFTD, szFTD )) ;
    

    /* [Options] FTDDs= */

    /* FTDDs found in ENGINE.DLL's directory */
    GetModuleFileName( ghmodDLL, szPath, MAX_PATHLEN ) ;
    StripFilespec( szPath ) ;

    SetWindowRedraw( hwnd, TRUE ) ;

    return TRUE ;
}

BOOL WINAPI ftdGetSelectedFTDFromListBox( HWND hwnd, LPSTR lpszFTDD, LPSTR lpszFTD )
{
    char    szBuf[MAX_CONFIGNAMELEN+MAX_PATHLEN+1] ;
    LPSTR   lpsz ;

    if (LB_ERR == ListBox_GetText( hwnd, ListBox_GetCurSel( hwnd ), szBuf ))
    {
        DP1( hWDB, "No selection in listbox!" ) ;
        *lpszFTDD = *lpszFTD = '\0' ;
        return FALSE ;
    }

    lpsz = szBuf ;
    ParseOffString( (LPSTR FAR *)&lpsz, lpszFTD ) ;
    ParseOffString( (LPSTR FAR *)&lpsz, lpszFTDD ) ;

    return TRUE ;
}

BOOL    WINAPI ftdSelectFTDInListBox( HWND hwnd, LPSTR lpszFTDD, LPSTR lpszFTD )
{
    char    szBuf[MAX_CONFIGNAMELEN+MAX_PATHLEN+1] ;
    wsprintf( szBuf, "%s\t,%s", (LPSTR)lpszFTD, (LPSTR)lpszFTDD ) ;

    return (LB_ERR != ListBox_SelectString( hwnd, 0, szBuf )) ;
}


/*---------------------------------------------------------------------- 
 * Default FTD procedure.  Kinda like DefWindowProc.
 *
 * All of the FTDs call this function when they get commands (uiCmd) that
 * they do not recognize.  In many cases the default behavior is just 
 * what the FTD wants.
 *
 * Note that any changes made to this function will probably affect all
 * FTDs.
 *
 */
LRESULT WINAPI ftdDefaultProc( UINT uiCmd, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3 )
{
    LPENGINEJOB     lpJob ;

    switch (uiCmd)
    {
/******************* Setup/Dlg box time functions ***************************/

        case FTD_GETMODULEHANDLE:
            /* lParam1 -> NULL
             * lParam2 -> NULL
             * lParam3 -> NULL
             */
            return MAKELRESULT( ghmodDLL, 0L ) ;
        break ;

        case FTD_READCONFIG:
            /* lParam1 -> LPFTDINFO
             * lParam2 -> LPSTR lpszSection
             * lParam3 -> LPSTR lpszINIFile
             */
        break ;

        case FTD_WRITECONFIG:
            /* lParam1 -> LPFTDINFO
             * lParam2 -> LPSTR lpszSection
             * lParam3 -> LPSTR lpszINIFile
             */
        break ;

        case FTD_INITCONFIG:
            /* lParam1 -> LPFTDINFO
             * lParam2 -> NULL
             * lParam3 -> NULL
             */
        break ;

        case FTD_GETDLGTEMPLATE:
            /* lParam1 -> NULL
             * lParam2 -> NULL
             * lParam3 -> NULL
             */
        break ;

        case FTD_CONFIGDLGMSG:
            /* lParam1 -> LPMSG
             * lParam2 -> NULL
             * lParam3 -> NULL
             */
        break ;

/******************* Print time functions *********************************/

        case FTD_STARTDOC:
        {
            OFSTRUCT of ;
            /* lParam1 -> LPJOB
             *
             * If there's a file in lpJob->lpszFile then we'll try to
             * open it, placing the handle in lpJob->hFile.  Since
             * almost all FTDs will want this done, they can just
             * call the default FTD.
             */
            lpJob = (LPENGINEJOB)lParam1 ;
            if (lpJob->lpszFile && 
                (lpJob->hFile = OpenFile(lpJob->lpszFile, &of, OF_READ | OF_SHARE_DENY_WRITE )))
            {
                lpJob->dwBytes = lpJob->dwBytesRemaining = (LPARAM)_filelength( lpJob->hFile ) ;
                DP3( hWDB, "File '%s', was opened OK (%04X)", (LPSTR)lpJob->lpszFile, lpJob->hFile ) ;
                return (LRESULT)lpJob ;
            }
            else
                return 0L ;

        }
        break ;

        case FTD_ENDDOC:
            /* lParam1 -> LPJOB
             *
             * If lpJob->hFile is non-zero, close the file.
             */
            lpJob = (LPENGINEJOB)lParam1 ;
            if (lpJob->hFile)
            {
                DP3( hWDB, "File is open...closing it..." ) ;
                _lclose( lpJob->hFile ) ;
                lpJob->hFile = NULL ;
                lpJob->dwBytesRemaining = 0 ;
            }
            return (LRESULT)lpJob ;
        break ;

        case FTD_OUTPUTPAGE:
            /* lParam1 -> LPJOB
             * lParam2 -> (UINT)cxPage
             * lParam3 -> (UINT)cyPage
             *
             * Returns TRUE if successful, FALSE if error.
             */
        break ;

        case FTD_QUERYDATALEFT:
        {
            /* lParam1 -> LPJOB
             *
             * Returns DWORD representing amount of data left
             * to be processed.  0 means FTD is done.
             */
             lpJob = (LPENGINEJOB)lParam1 ;
             return (LRESULT)lpJob->dwBytesRemaining ;
        }
        break ;

    }
    return 0L ;
}

#define CODEBIT       0x80
#define BUTTONCODE    0x80
#define EDITCODE      0x81
#define STATICCODE    0x82
#define LISTBOXCODE   0x83
#define SCROLLBARCODE 0x84
#define COMBOBOXCODE  0x85

LPDLGBOXHEADER WINAPI GetDlgBoxRes( HMODULE ghmodDLL, LPSTR lpszTemplate )
{
    HGLOBAL         hRes ;
    HRSRC           hDlgRes ;
    LPSTR           lpDlgRes = NULL ;
    LPDLGBOXHEADER  lpDlgBoxHdr = NULL ;
    LPCTRLDATA      lpCtrl ;
    LPSTR           lpResData ;
    UINT            cb ;
    BYTE            nItems ;       
    UINT            n ;
       
    if (hDlgRes = FindResource( ghmodDLL, lpszTemplate, RT_DIALOG ))
    {
        if (hRes = LoadResource( ghmodDLL, hDlgRes ))
            lpDlgRes = LockResource( hRes ) ;
        else
        {
            DP1( hWDB, "LoadResource failed!" ) ;
            FreeResource( hDlgRes ) ;
            return NULL ;
        }

        nItems = *(lpDlgRes + sizeof( DWORD )) ;

        cb = (UINT)SizeofResource( ghmodDLL, hDlgRes ) ;

        if (lpDlgBoxHdr = (LPDLGBOXHEADER)GlobalAllocPtr( GHND, cb + 1 +
                                                sizeof(DLGBOXHEADER) + 
                                                (sizeof(CTRLDATA)*nItems) ))
        {
            /* First copy the original template to the end of our
             * block.
             */
            lpResData = ((LPSTR)lpDlgBoxHdr) + sizeof(DLGBOXHEADER) + 
                            (sizeof(CTRLDATA)*nItems) ;
            _fmemcpy( lpResData, lpDlgRes, cb ) ;

            /* Fill in the DLGBOXHEADER structure
             */
            lpDlgBoxHdr->lStyle =           *((DWORD FAR *) lpResData)++ ;
            lpDlgBoxHdr->bNumberOfItems =   *((BYTE FAR *) lpResData)++ ;
            lpDlgBoxHdr->x =                *((int FAR *) lpResData)++ ;
            lpDlgBoxHdr->y =                *((int FAR *) lpResData)++ ;
            lpDlgBoxHdr->cx =               *((int FAR *) lpResData)++ ;
            lpDlgBoxHdr->cy =               *((int FAR *) lpResData)++ ;

            lpDlgBoxHdr->szMenuName =       lpResData ;
            lpResData += lstrlen( lpResData ) + 1 ;

            lpDlgBoxHdr->szClassName =      lpResData ;
            lpResData += lstrlen( lpResData ) + 1 ;

            lpDlgBoxHdr->szCaption   =      lpResData ;
            lpResData += lstrlen( lpResData ) + 1 ;

            lpDlgBoxHdr->wPointSize =       *((UINT FAR *) lpResData)++ ;
            lpDlgBoxHdr->szFaceName =       lpResData ;
            lpResData += lstrlen( lpResData ) + 1 ;
             
            /* Now move on to each of the DLGITEMS
             */
            for (n = 0 ; n < nItems ; n++)
            {
                lpCtrl = (LPCTRLDATA)(((LPSTR)lpDlgBoxHdr)+ sizeof(DLGBOXHEADER) +
                                               (sizeof(CTRLDATA) * n)) ;

                lpCtrl->x               = *((int FAR *) lpResData)++ ;
                lpCtrl->y               = *((int FAR *) lpResData)++ ;
                lpCtrl->cx              = *((int FAR *) lpResData)++ ;
                lpCtrl->cy              = *((int FAR *) lpResData)++ ;
                lpCtrl->wID             = *((int FAR *) lpResData)++ ;
                lpCtrl->lStyle          = *((DWORD FAR *) lpResData)++ ;

                lpCtrl->szClass         = (LPSTR) lpResData ;

                if (*lpCtrl->szClass & CODEBIT)
                {
                    switch( *lpCtrl->szClass )
                    {
                        case BUTTONCODE:
                            lpCtrl->szClass = "BUTTON" ;
                        break ;

                        case EDITCODE:
                            lpCtrl->szClass = "EDIT" ;
                        break ;

                        case STATICCODE:
                            lpCtrl->szClass = "STATIC" ;
                        break ;

                        case LISTBOXCODE:
                            lpCtrl->szClass = "LISTBOX" ;
                        break ;

                        case SCROLLBARCODE:
                            lpCtrl->szClass = "SCROLLBAR" ;
                        break ;

                        case COMBOBOXCODE:
                            lpCtrl->szClass = "COMBOBOX" ;
                        break ;

                        default:
                            lpCtrl->szClass = "" ;    
                        break ;
                    }
                    lpCtrl->szText = (LPSTR)++lpResData ;
                    lpResData += lstrlen( lpCtrl->szText ) + 1 ;
                }
                else
                {
                    lpResData += lstrlen( lpCtrl->szClass ) + 1 ;
                    lpCtrl->szText = (LPSTR)lpResData ;
                    lpResData += lstrlen( lpCtrl->szText ) + 1 ;
                }

                /* lpCreateParams is at the end.
                 */
                lpResData = lpResData + 1 + *lpResData ;

            }
        }
        else
        {
            DP1( hWDB, "GetDlgBoxRes: GlobalAllocPtr failed!" ) ;
        }

        UnlockResource( hRes ) ;
        FreeResource( hRes ) ;
    }

    return lpDlgBoxHdr ;

}

/************************************************************************
 * End of File: FTD.C
 ***********************************************************************/

