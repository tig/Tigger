/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CONFIG 
 *
 *      Module:  CONFIG.c
 * 
 *      This module contains the configuration management entry points
 *      for the WinPRint 2.0 engine.
 *
 *   Revisions:  
 *       9/3/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"

#include "ENGINE.h"

BOOL INTERNAL ReadEngineHeaderFooter( LPSTR lpszSection, LPSTR lpszKey, LPENGINEHF lpEHF      ,LPSTR lpszFile) ;
BOOL INTERNAL WriteEngineHeaderFooter( LPSTR szSection, LPSTR lpszKey, LPENGINEHF lpHF         ,LPSTR lpszFile) ;
BOOL INTERNAL ReadEngineHeaderSection( LPSTR lpszSection, LPSTR lpszKey, LPENGINEHF_SECT lpEHFS,LPSTR lpszFile) ;
BOOL INTERNAL WriteEngineHeaderSection( LPSTR szSection, LPSTR szKey, LPENGINEHF_SECT lpEHFS    ,LPSTR lpszFile) ;

BOOL WINAPI engConfigSetDefaults( HWND hwnd, LPSTR lpszName, LPSTR lpszFile, LPENGINECONFIG lpEC );
BOOL INTERNAL DefaultEngineHF( LPENGINECONFIG lpEC );

#define WRITESTRING(key,s)  WritePrivateProfileString(szSection, key, s,lpszFile)

#define READSTRING(key,d,s,n) GetPrivateProfileString(szSection, key, d, (LPSTR)s, n, lpszFile)

LPENGINECONFIG WINAPI engConfigAlloc( VOID )
{
    LPENGINECONFIG lpEC ;

    DP4( hWDB, "Allocating ENGINECONFIG..." ) ;
    if (lpEC = (LPENGINECONFIG)GlobalAllocPtr( GHND, sizeof( ENGINECONFIG ) ))
        lpEC->uiStructSize = sizeof( ENGINECONFIG ) ;

    DASSERT( hWDB, lpEC ) ;

    return lpEC ;
}

LPENGINECONFIG WINAPI engConfigCopy( LPENGINECONFIG lpECout, LPENGINECONFIG lpECin )
{
    UINT  uiSize = lpECin->FTDInfo.uiStructSize ;

    VALIDATEPARAM( lpECin ) ;

    if (lpECout == NULL)
    {
        if (!(lpECout = engConfigAlloc()))
        {
            DP1( hWDB, "engConfigAlloc failed!" ) ;
            return NULL ;
        }
    }

    /* copy FILETYPE stuff
     */
    if (lpECout->FTDInfo.uiStructSize && lpECout->FTDInfo.lpData)
    {
        GlobalFreePtr( lpECout->FTDInfo.lpData ) ;
        lpECout->FTDInfo.uiStructSize = 0 ;
        lpECout->FTDInfo.lpData = NULL ;
    }

    *lpECout = *lpECin ;

    if (uiSize)
    {
        lpECout->FTDInfo.lpData = (LPARAM)GlobalAllocPtr( GHND, uiSize ) ;
        _fmemcpy( (LPVOID)lpECout->FTDInfo.lpData, (LPVOID)lpECin->FTDInfo.lpData, (WORD)uiSize ) ;
        lpECout->FTDInfo.uiStructSize = uiSize ;
    }
    
    return lpECout ;
}

BOOL WINAPI engConfigFree( LPENGINECONFIG lpEC )
{
    VALIDATEPARAM( lpEC ) ;

    if (lpEC->FTDInfo.uiStructSize)
    {
        DP4( hWDB, "engConfigFree: Freeing lpData..." ) ;
        GlobalFreePtr( lpEC->FTDInfo.lpData ) ;
    }

    DP4( hWDB, "engConfigFree: Freeing ENGINECONFIG..." ) ;
    GlobalFreePtr( lpEC ) ;
       
    return TRUE ;
}
                 
BOOL WINAPI engConfigRead( HWND hwnd, LPSTR lpszName, LPSTR lpszFile, LPENGINECONFIG lpEC )
{
    char    szBuf[256] ;
    char    szSection[MAX_CONFIGNAMELEN + 9] ; 
    char    szFileName[144] ;
    LPSTR   lpsz ;
        
    DASSERT( hWDB, lpEC ) ;
    DASSERT( hWDB, lpszName ) ;
    VALIDATEPARAM( lpEC ) ;
    VALIDATEPARAM( lpszName ) ;

    lstrcpy( szSection, lpszName ) ; 

    if (lpszFile == NULL)
    {
        lstrcpy( szFileName, GRCS(IDS_INI_FILENAME) ) ;
        lpszFile = szFileName ;
    }
 
    /* Verify config exists.
     */
    if (!GetPrivateProfileString( szSection, NULL, "", szBuf, 255, lpszFile ))
    {
        /* The configuration %s was not found.  Do you want to create it
         * using default values?  Yes/No
         */
        if (IDNO == ErrorResBox( hwnd, ghmodDLL, MB_ICONQUESTION | MB_YESNO, 
                                 IDS_APPTITLE, 
                                 IDS_ERR_CONFIGNOTFOUND, (LPSTR)lpszName ))
            FAIL( ENGERR_CONFIGNOTFOUND ) ;

        engConfigSetDefaults( hwnd, lpszName, lpszFile, lpEC ) ;

        if (!ftdConfigRead( hwnd, lpEC, szSection, lpszFile ))
        {
            DP3( hWDB, "ftdConfigRead( %s, %s ) failed.  Using default.",
                        (LPSTR)lpEC->szFTDD, (LPSTR)lpEC->szFTD ) ;
            lpEC->FTDInfo.uiStructSize = 0 ;
            lpEC->FTDInfo.lpData = NULL ;
        }

        return TRUE ;
    }

    lstrcpy( lpEC->szConfigName, lpszName ) ;

    /* Get the filetype driver
     */
    READSTRING( GRCS(IDS_KEY_FTD), "", szBuf, MAX_CONFIGNAMELEN ) ;
    lpsz = szBuf ;
    ParseOffString( (LPSTR FAR *)&lpsz, lpEC->szFTDD ) ;
    ParseOffString( (LPSTR FAR *)&lpsz, lpEC->szFTD ) ;

    /* Call into FTD to read custom data.  Note we do not abort the
     * config read here if we can't get the FTD info.  We just assume
     * defaults.
     */
    if (!ftdConfigRead( hwnd, lpEC, szSection, lpszFile ))
    {
        DP3( hWDB, "ftdConfigRead( %s, %s ) failed.  Using default.",
                    (LPSTR)lpEC->szFTDD, (LPSTR)lpEC->szFTD ) ;
        lpEC->FTDInfo.uiStructSize = 0 ;
        lpEC->FTDInfo.lpData = NULL ;
    }

    DP5( hWDB, "Read FTD: %s, %s, %lu", (LPSTR)lpEC->szFTDD, (LPSTR)lpEC->szFTD, (DWORD)lpEC->FTDInfo.uiStructSize ) ;

    READSTRING( GRCS(IDS_KEY_FLAGS), "", szBuf, 64 ) ;
    DP5( hWDB, "Read Flags = %s", (LPSTR)szBuf ) ;
    lpsz = szBuf ;
    lpEC->dwFlags = ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->dwBorderFlags = ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->dwShadeFlags = ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;

    if (lpEC->dwFlags & ENGCFG_USEDEFAULTPRN)
    {
        *lpEC->szDriver = '\0' ;
        *lpEC->szDevice = '\0' ;
        *lpEC->szOutput = '\0' ;
    }
    else
    {    
        READSTRING( GRCS(IDS_KEY_DRIVER), "", szBuf, MAX_DRIVERLEN+MAX_DEVICELEN+MAX_OUTPUTLEN+3 ) ;
        lpsz = szBuf ;
        ParseOffString( (LPSTR FAR *)&lpsz, lpEC->szDriver ) ;
        ParseOffString( (LPSTR FAR *)&lpsz, lpEC->szDevice ) ;
        ParseOffString( (LPSTR FAR *)&lpsz, lpEC->szOutput ) ;
    }

    READSTRING( GRCS(IDS_KEY_DEVMODE), "", szBuf, 256 ) ;
    DP5( hWDB, "Read DevMode = %s", (LPSTR)szBuf ) ;
    lpsz = szBuf ;
    lpEC->dmFields          = ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->dmOrientation      = (short)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->dmPaperSize        = (short)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->dmPaperLength      = (short)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->dmPaperWidth       = (short)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->dmScale            = (short)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->dmCopies           = (short)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->dmDefaultSource    = (short)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->dmPrintQuality     = (short)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->dmColor            = (short)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->dmDuplex           = (short)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
//        lpEC->dmCollate          = (short)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;

    READSTRING( GRCS(IDS_KEY_MARGINS), "", szBuf, 64 ) ;
    DP5( hWDB, "Read Margins = %s", (LPSTR)szBuf ) ;
    lpsz = szBuf ;
    lpEC->uiTopMargin = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->uiBottomMargin = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->uiLeftMargin = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->uiRightMargin = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;

    if (!ReadEngineHeaderFooter( szSection, 
                      GRCS( IDS_KEY_HEADER ),
                      &lpEC->Header, lpszFile ))
    {
        DP1( hWDB, "Error reading Header info" ) ;
        return FALSE ;
    }

    if (!ReadEngineHeaderFooter( szSection, 
                      GRCS( IDS_KEY_FOOTER ),
                      &lpEC->Footer, lpszFile ))
    {
        DP1( hWDB, "Error reading Header info" ) ;
        return FALSE ;
    }

    READSTRING( GRCS(IDS_KEY_MULTIPAGESUP), "", szBuf, 64 ) ;
    lpsz = szBuf ;
    lpEC->uiPageRows = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->uiPageColumns = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->uiHorzSep = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpEC->uiVertSep = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;

    return TRUE ;

Fail:

    return FALSE ;
}

BOOL INTERNAL ReadEngineHeaderFooter( LPSTR szSection, LPSTR lpszKey, LPENGINEHF lpHF, LPSTR lpszFile )
{
    char    szKey[64] ;
    char    szBuf[64] ;
    LPSTR   lpsz = szBuf ;

    wsprintf( szKey, "%s%s", lpszKey, GRCS(IDS_KEY_FLAGS) ) ;
    READSTRING( szKey, "", szBuf, 64 ) ;

    DP5( hWDB, "Read HF Flags = %s", (LPSTR)szBuf ) ;
    lpHF->dwFlags = ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpHF->uiHeight = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpHF->dwBorderFlags = ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
    lpHF->dwShadeFlags = ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;

    wsprintf( szKey, "%s%s", lpszKey, GRCS(IDS_KEY_LEFT) ) ;
    if (!ReadEngineHeaderSection( szSection, szKey, &lpHF->Left, lpszFile ))
    {
        DP1( hWDB, "Error Reading EngineHeader: %s", (LPSTR)szKey ) ;
        return FALSE  ;
    }
    wsprintf( szKey, "%s%s", lpszKey, GRCS(IDS_KEY_CENTER) ) ;
    if (!ReadEngineHeaderSection( szSection, szKey, &lpHF->Center, lpszFile ))
    {
        DP1( hWDB, "Error Reading EngineHeader: %s", (LPSTR)szKey ) ;
        return FALSE  ;
    }
    wsprintf( szKey, "%s%s", lpszKey, GRCS(IDS_KEY_RIGHT) ) ;
    if (!ReadEngineHeaderSection( szSection, szKey, &lpHF->Right, lpszFile))
    {
        DP1( hWDB, "Error Reading EngineHeader: %s", (LPSTR)szKey ) ;
        return FALSE  ;
    }

    return TRUE ;
}    

BOOL INTERNAL ReadEngineHeaderSection( LPSTR szSection, LPSTR szKey, LPENGINEHF_SECT lpEHFS, LPSTR lpszFile )
{
    LPSTR   lpszBuf = GlobalAllocPtr( GHND, 1024 ) ;
    LPSTR   lpsz = (LPSTR)lpszBuf ;

    if (!lpsz)
    {
        DP1( hWDB, "GlobalAlloc failed in ReadEngineHeaderSection()" ) ;
        dwLastError = ENGERR_GLOBALALLOC ;
        return FALSE ;
    }

    wsprintf( lpszBuf, "%s%s", (LPSTR)szKey, GRCS(IDS_KEY_LPSZ) ) ;
    READSTRING( lpszBuf, "", lpszBuf, 1024 ) ;
    ShrinkStringFromPrintable( lpszBuf ) ;
    _fstrncpy( lpEHFS->szText, lpszBuf, MAX_HFTEXTLEN ) ;
    lpEHFS->szText[MAX_HFTEXTLEN-1] = '\0' ;
    DP5( hWDB, "Read HF lpsz = %s", (LPSTR)lpEHFS->szText ) ;

    wsprintf( lpszBuf, "%s%s", (LPSTR)szKey, GRCS(IDS_KEY_FONT) ) ;
    READSTRING( lpszBuf, "", lpszBuf, 128 ) ;
    DP5( hWDB, "Read HF Font = %s", (LPSTR)lpszBuf ) ;

    if (!ParseOffString( (LPSTR FAR *)&lpsz, lpEHFS->szFace ))
        FAIL( ENGERR_CONFIGCORRUPT ) ;

    if (!ParseOffString( (LPSTR FAR *)&lpsz, lpEHFS->szStyle ))
        FAIL( ENGERR_CONFIGCORRUPT ) ;

    if (!(lpEHFS->wPoints = (WORD)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL )))
        FAIL( ENGERR_CONFIGCORRUPT ) ;

    GlobalFreePtr( lpszBuf ) ;
    return TRUE ;

Fail:
    GlobalFreePtr( lpszBuf ) ;
    return FALSE ;

}


BOOL WINAPI engConfigWrite( HWND hwnd, LPSTR lpszName, LPSTR lpszFile, LPENGINECONFIG lpEC ) 
{
    char szBuf[128] ;
    char szSection[MAX_CONFIGNAMELEN + 9] ;
    char szFileName[144] ;
    LPSTR   lpsz = NULL ;
    
    DASSERT( hWDB, lpEC ) ;
    DASSERT( hWDB, lpszName ) ;
    VALIDATEPARAM( lpEC ) ;
    VALIDATEPARAM( lpszName ) ;
    
    if (lpEC->uiStructSize != sizeof( ENGINECONFIG ))
    {
        DP1( hWDB, "engConfigWrite: Structure size wrong (%ul)", lpEC->uiStructSize ) ;
        dwLastError = ENGERR_INVALIDSTRUCTSIZE ;
        return FALSE ;
    }

    lstrcpy( szSection, lpszName ) ; 

    if (lpszFile == NULL)
    {
        lstrcpy( szFileName, GRCS(IDS_INI_FILENAME) ) ;
        lpszFile = szFileName ;
    }

    /* Filetype driver
     */
    if (ftdConfigWrite( hwnd, lpEC, szSection, lpszFile ))
    {
        wsprintf( szBuf, "%s,%s", (LPSTR)lpEC->szFTDD, lpEC->szFTD ) ;
        WRITESTRING( GRCS(IDS_KEY_FTD), szBuf ) ;
    }

    wsprintf( szBuf, "%#08lX,%#08lX,%#08lX", (DWORD)lpEC->dwFlags, 
                                      (DWORD)lpEC->dwBorderFlags,
                                      (DWORD)lpEC->dwShadeFlags ) ;
    DP5( hWDB, "Flags = %s", (LPSTR)szBuf ) ;
    WRITESTRING( GRCS(IDS_KEY_FLAGS), szBuf ) ;

    if (lpEC->dwFlags & ENGCFG_USEDEFAULTPRN)
    {
        WRITESTRING( GRCS(IDS_KEY_DRIVER), "" ) ;
    }
    else
    {
        if (lpsz = GlobalAllocPtr( GHND, lstrlen( lpEC->szDriver ) +
                                         lstrlen( lpEC->szDevice ) +
                                         lstrlen( lpEC->szOutput ) + 3 ))
        {
            wsprintf( lpsz, "%s,%s,%s",  (LPSTR)lpEC->szDriver, 
                                         (LPSTR)lpEC->szDevice,
                                         (LPSTR)lpEC->szOutput ) ;
            WRITESTRING( GRCS(IDS_KEY_DRIVER), lpsz ) ;
        }
        else
            FAIL(  ENGERR_GLOBALALLOC ) ;
        GlobalFreePtr( lpsz ) ;
    }

    wsprintf( szBuf, "%#08lX,%#04X,%#04X,%#04X,%#04X,%#04X,%#04X,%#04X,%#04X,%#04X,%#04X",
                (DWORD)lpEC->dmFields,
                lpEC->dmOrientation,
                lpEC->dmPaperSize,
                lpEC->dmPaperLength,
                lpEC->dmPaperWidth,
                lpEC->dmScale,
                lpEC->dmCopies,
                lpEC->dmDefaultSource,
                lpEC->dmPrintQuality,
                lpEC->dmColor,
                lpEC->dmDuplex ) ;
//                lpEC->dmCollate ) ;
    DP5( hWDB, "DevMode = %s", (LPSTR)szBuf ) ;
    WRITESTRING( GRCS(IDS_KEY_DEVMODE), szBuf ) ;

    wsprintf( szBuf, "%#04X,%#04X,%#04X,%#04X", 
                     lpEC->uiTopMargin,
                     lpEC->uiBottomMargin,
                     lpEC->uiLeftMargin,
                     lpEC->uiRightMargin ) ;
    WRITESTRING( GRCS(IDS_KEY_MARGINS), szBuf ) ;

    if (!WriteEngineHeaderFooter( szSection, 
                      GRCS( IDS_KEY_HEADER ),
                      &lpEC->Header,
                      lpszFile )) 
    {
        DP1( hWDB, "Error reading Header info" ) ;
        FAIL( ENGERR_CONFIGCORRUPT ) ;
    }

    if (!WriteEngineHeaderFooter( szSection, 
                      GRCS( IDS_KEY_FOOTER ),
                      &lpEC->Footer, 
                      lpszFile ) )
    {
        DP1( hWDB, "Error reading Header info" ) ;
        FAIL( ENGERR_CONFIGCORRUPT ) ;
    }

    wsprintf( szBuf, "%d,%d,%#04X,%#04X", lpEC->uiPageRows, lpEC->uiPageColumns,
                    lpEC->uiHorzSep, lpEC->uiVertSep ) ;
    WRITESTRING( GRCS(IDS_KEY_MULTIPAGESUP), szBuf ) ;

    return TRUE ;

Fail:
    return FALSE ;

}

BOOL INTERNAL WriteEngineHeaderFooter( LPSTR szSection, LPSTR lpszKey, LPENGINEHF lpHF, LPSTR lpszFile )
{
    char    szKey[64] ;
    char    szBuf[64] ;

    wsprintf( szKey, "%s%s", lpszKey, GRCS(IDS_KEY_FLAGS) ) ;
    wsprintf( szBuf, "%#08lX,%#08lX,%#08lX,%#08lX", (DWORD)lpHF->dwFlags,
                                            (DWORD)lpHF->uiHeight,
                                            (DWORD)lpHF->dwBorderFlags,
                                            (DWORD)lpHF->dwShadeFlags ) ;
    WRITESTRING( szKey, szBuf ) ;

    wsprintf( szKey, "%s%s", lpszKey, GRCS(IDS_KEY_LEFT) ) ;
    if (!WriteEngineHeaderSection( szSection, szKey, &lpHF->Left, lpszFile ))
    {
        DP1( hWDB, "Error Writing EngineHeader: %s", (LPSTR)szKey ) ;
        return FALSE  ;
    }
    wsprintf( szKey, "%s%s", lpszKey, GRCS(IDS_KEY_CENTER) ) ;
    if (!WriteEngineHeaderSection( szSection, szKey, &lpHF->Center, lpszFile ))
    {
        DP1( hWDB, "Error Writing EngineHeader: %s", (LPSTR)szKey ) ;
        return FALSE  ;
    }
    wsprintf( szKey, "%s%s", lpszKey, GRCS(IDS_KEY_RIGHT) ) ;
    if (!WriteEngineHeaderSection( szSection, szKey, &lpHF->Right, lpszFile ))
    {
        DP1( hWDB, "Error Writing EngineHeader: %s", (LPSTR)szKey ) ;
        return FALSE  ;
    }

    return TRUE ;
}    

BOOL INTERNAL WriteEngineHeaderSection( LPSTR szSection, LPSTR lpszKey, LPENGINEHF_SECT lpEHFS, LPSTR lpszFile)
{
    char szKey[64] ;
    LPSTR   lpszBuf = GlobalAllocPtr( GHND, (lstrlen( lpEHFS->szText ) + 1) * 5 ) ;

    if (!lpszBuf)
    {
        DP1( hWDB, "GlobalAlloc failed in ReadEngineHeaderSection()" ) ;
        dwLastError = ENGERR_GLOBALALLOC ;
        return FALSE ;
    }

    wsprintf( szKey, "%s%s", (LPSTR)lpszKey, GRCS(IDS_KEY_LPSZ) ) ;

    lstrcpy( lpszBuf, lpEHFS->szText ) ;
    ExpandStringToPrintable( lpszBuf ) ;

    WRITESTRING( szKey, lpszBuf ) ;

    wsprintf( lpszBuf, "%s,%s,%d", (LPSTR)lpEHFS->szFace,
                                 (LPSTR)lpEHFS->szStyle,
                                 lpEHFS->wPoints ) ;

    wsprintf( szKey, "%s%s", (LPSTR)lpszKey, GRCS(IDS_KEY_FONT) ) ;
    WRITESTRING( szKey, lpszBuf ) ;

    GlobalFreePtr( lpszBuf ) ;

    return TRUE ;
}


BOOL WINAPI engConfigSetDefaults( HWND hwnd, LPSTR lpszName, LPSTR lpszFile, LPENGINECONFIG lpEC )
{
    char    szDefault[MAX_CONFIGNAMELEN] ;

    /* First see if there is a default config specified.  This is specified in
     * the [Options] section as DefaultConfig=
     */
    if (GetPrivateProfileString( GRCS( IDS_SEC_OPTIONS ), 
                                 GRCS( IDS_KEY_DEFAULTCONFIG ),
                                 "", szDefault, MAX_CONFIGNAMELEN, lpszFile ))
    {
        /* Read it */
        if (engConfigRead( hwnd, szDefault, lpszFile, lpEC ))
        {
            /* Write it back out */
            engConfigWrite( hwnd, lpszName, lpszFile, lpEC ) ;
            return TRUE ;
        }
    }

    lstrcpy( lpEC->szConfigName, lpszName ) ;

    lstrcpy( lpEC->szFTDD, GRCS(IDS_DEFAULT_FTDD) ) ;
    lstrcpy( lpEC->szFTD, GRCS(IDS_DEFAULT_FTD) ) ;

    lpEC->FTDInfo.uiStructSize = 0 ;
    lpEC->FTDInfo.lpData = NULL ;

    lpEC->dwFlags = ENGCFG_USEDEFAULTPRN ;
    lpEC->dwBorderFlags = ENG_TOPBORDER   |
                          ENG_LEFTBORDER  |
                          ENG_RIGHTBORDER | 
                          ENG_BOTTOMBORDER ;
    lpEC->dwShadeFlags = ENG_SHADE_LTGRAY ;

    *lpEC->szDriver = '\0' ;
    *lpEC->szDevice = '\0' ;
    *lpEC->szOutput = '\0' ;

    lpEC->dmFields          = 0 ;
    lpEC->dmOrientation     = DMORIENT_PORTRAIT ;
    lpEC->dmPaperSize       = DMPAPER_FIRST ;
    lpEC->dmPaperLength     = 0 ;
    lpEC->dmPaperWidth      = 0 ;
    lpEC->dmScale           = 100 ;
    lpEC->dmCopies          = 0 ;
    lpEC->dmDefaultSource   = DMBIN_AUTO ;
    lpEC->dmPrintQuality    = DMRES_HIGH ;
    lpEC->dmColor           = 0 ;
    lpEC->dmDuplex          = 0 ;

    if (fUsePrintOffset)
    {
        lpEC->uiTopMargin       = 0X0000027B ;  // .25 inch
        lpEC->uiBottomMargin    = 0X0000027B ;
        lpEC->uiLeftMargin      = 0X0000027B ;
        lpEC->uiRightMargin     = 0X0000027B ;
    }
    else
    {
        lpEC->uiTopMargin       = 0;
        lpEC->uiBottomMargin    = 0;
        lpEC->uiLeftMargin      = 0;
        lpEC->uiRightMargin     = 0;
    }

    DefaultEngineHF( lpEC ) ;

    lpEC->uiPageRows        = 1 ;
    lpEC->uiPageColumns     = 1 ;
    lpEC->uiHorzSep  = 0x13E ;       // .125 inch
    lpEC->uiVertSep  = 0x13E ;
    return TRUE ;
}

BOOL INTERNAL DefaultEngineHF( LPENGINECONFIG lpEC )
{
    lpEC->Header.dwFlags       = 0 ;
    lpEC->Header.uiHeight      = 0 ;
#if 1
    lpEC->Header.dwBorderFlags = 0 ;
#else
    lpEC->Header.dwBorderFlags = ENG_TOPBORDER   |
                                 ENG_LEFTBORDER  |
                                 ENG_RIGHTBORDER | 
                                 ENG_BOTTOMBORDER ;
#endif
    lpEC->Header.dwShadeFlags  = 0 ;

    lstrcpy( lpEC->Header.Left.szText, GRCS(IDS_DEFAULT_HEADER_LEFT) ) ;
    lstrcpy( lpEC->Header.Left.szFace, "" ) ;
    lstrcpy( lpEC->Header.Left.szStyle, "" ) ;
    lpEC->Header.Left.wPoints = 8 ;

    lstrcpy( lpEC->Header.Center.szText, GRCS(IDS_DEFAULT_HEADER_CENTER) ) ;
    lstrcpy( lpEC->Header.Center.szFace, "" ) ;
    lstrcpy( lpEC->Header.Center.szStyle, "" ) ;
    lpEC->Header.Center.wPoints = 8 ;

    lstrcpy( lpEC->Header.Right.szText, GRCS(IDS_DEFAULT_HEADER_RIGHT) ) ;
    lstrcpy( lpEC->Header.Right.szFace, "" ) ;
    lstrcpy( lpEC->Header.Right.szStyle, "" ) ;
    lpEC->Header.Right.wPoints = 8 ;

    lpEC->Footer.dwFlags       = 0 ;
    lpEC->Footer.uiHeight      = 0 ;
#if 1
    lpEC->Footer.dwBorderFlags = 0 ;
#else
    lpEC->Footer.dwBorderFlags = ENG_TOPBORDER   |
                                 ENG_LEFTBORDER  |
                                 ENG_RIGHTBORDER | 
                                 ENG_BOTTOMBORDER ;
#endif
    lpEC->Footer.dwShadeFlags  = 0 ;

    lstrcpy( lpEC->Footer.Left.szText, GRCS(IDS_DEFAULT_FOOTER_LEFT) ) ;
    lstrcpy( lpEC->Footer.Left.szFace, "" ) ;
    lstrcpy( lpEC->Footer.Left.szStyle, "" ) ;
    lpEC->Footer.Left.wPoints = 8 ;

    lstrcpy( lpEC->Footer.Center.szText, GRCS(IDS_DEFAULT_FOOTER_CENTER) ) ;
    lstrcpy( lpEC->Footer.Center.szFace, "" ) ;
    lstrcpy( lpEC->Footer.Center.szStyle, "" ) ;
    lpEC->Footer.Center.wPoints = 8 ;

    lstrcpy( lpEC->Footer.Right.szText, GRCS(IDS_DEFAULT_FOOTER_RIGHT) ) ;
    lstrcpy( lpEC->Footer.Right.szFace, "" ) ;
    lstrcpy( lpEC->Footer.Right.szStyle, "" ) ;
    lpEC->Footer.Right.wPoints = 8 ;

    return TRUE ;
}    


/************************************************************************
 * End of File: CONFIG.c
 ***********************************************************************/

