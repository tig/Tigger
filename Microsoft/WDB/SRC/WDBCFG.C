/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** wdbcfg.c
 *
 *  DESCRIPTION: 
 *      Contains various functions for dealing with the configuration
 *      of WDB and its sessions.  A lot of the code in here is purely
 *      grunt work--it's not pretty, but it is necessary.
 *
 *  HISTORY:
 *      2/14/91     cjp     wrote it
 *
 ** cjp */


/* grab the includes we need */
    #include <windows.h>
    #include "wdb.h"
    #include "wdbi.h"


/** BOOL FAR PASCAL wdbInitConfig( void )
 *
 *  DESCRIPTION: 
 *      This function is called when the WDB.DLL is first loaded to 
 *      fill in some of the default configuration values and system
 *      information constants (run-time) for WDB.
 *
 *  ARGUMENTS:
 *      Not a one.
 *
 *  RETURN (BOOL FAR PASCAL):
 *      The return value specifies whether the function succeeded or
 *      not.  TRUE is good news.  FALSE is bad.
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbInitConfig( void )
{
    int     dy;
    DWORD   dwWinFlags;
    int   nCPU;
    LPSTR   lpsz;


    /* get default font values */
    lstrcpy( gWDBDefSession.wdbConfig.szFontName, gszDefFontName );
    gWDBDefSession.wdbConfig.wFontSize = WDBC_DEF_FONTSIZE;
    gWDBDefSession.wdbConfig.wFontBold = FALSE;


    /* determine default position of window (bottom 1/5 of screen) */
    dy = GetSystemMetrics( SM_CYSCREEN );
    gWDBDefSession.wdbConfig.nWindowX = 0;
    gWDBDefSession.wdbConfig.nWindowY = dy - (dy / 5);
    gWDBDefSession.wdbConfig.wWindowW = GetSystemMetrics( SM_CXSCREEN );
    gWDBDefSession.wdbConfig.wWindowH = dy / 5;


    /* create the section name for WDB */
    lstrcpy( gWDBDefSession.szSection, gszSectionPrefix );
    lstrcat( gWDBDefSession.szSection, gszWDB );
    lstrcpy( gWDBDefSession.szPrivate, gszINI );

#ifndef WIN32
    /* set up assert info */
    dwWinFlags = GetWinFlags();

    if ( dwWinFlags & WF_CPU086 )
        nCPU = 0;
    else if ( dwWinFlags & WF_CPU186 )
        nCPU = 1;
    else if ( dwWinFlags & WF_CPU286 )
        nCPU = 2;
    else if ( dwWinFlags & WF_CPU386 )
        nCPU = 3;
    else if ( dwWinFlags & WF_CPU486 )
        nCPU = 4;

    if ( dwWinFlags & WF_PMODE )
    {
        if ( dwWinFlags & WF_STANDARD )
            lpsz = "Standard";
        else
            lpsz = "Enhanced";
    }

    else lpsz = "Real";

    wsprintf( gszAssertInfo, "%s Mode ! 80%c86, %s", lpsz, nCPU + '0',
                (dwWinFlags & WF_80x87) ? (LPSTR)"80x87" : (LPSTR)"No FPU" );


    if ( dwWinFlags & WF_SMALLFRAME )
        lpsz = "Small";
    else if ( dwWinFlags & WF_LARGEFRAME )
        lpsz = "Large";

    else lpsz = "None";

    wsprintf( &gszAssertInfo[ lstrlen(gszAssertInfo) ], " ! EMS Frame: %s", lpsz );
#else
    UNREFERENCED_PARAMETER( nCPU ) ;
    UNREFERENCED_PARAMETER( lpsz ) ;
    UNREFERENCED_PARAMETER( dwWinFlags ) ;
    wsprintf( gszAssertInfo, "Windows NT" ) ;
#endif

    /* return success */
    return ( TRUE );
} /* wdbInitConfig() */



/** BOOL FAR PASCAL wdbWriteConfigEnable( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigEnable( LPSTR       lpszSection,
                                      LPSTR       lpszPrivate,
                                      LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;

    /* get an appropriate string argument */
    lstrcpy( gachTemp, lpWDBConfig->wEnable ? gszArgYes : gszArgNo );

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyEnable, 
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyEnable, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigEnable() */


/** BOOL FAR PASCAL wdbReadConfigEnable( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigEnable( LPSTR       lpszSection,
                                     LPSTR       lpszPrivate,
                                     LPWDBCONFIG lpWDBConfig )
{
    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        GetPrivateProfileString( lpszSection, gszKeyEnable, gszDefzYz,
                                    gachTemp, sizeof( gachTemp ),
                                    lpszPrivate );
    }

    else
    {
        GetProfileString( lpszSection, gszKeyEnable, gszDefzYz,
                                gachTemp, sizeof( gachTemp ) );
    }

    /* was there a gszKeyEnable in the .INI file? */
    if ( !lstrcmpi( gachTemp, gszDefzYz ) )
    {
        /* no! grab default value--check actually isn't necessary */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
            lpWDBConfig->wEnable = gWDBDefSession.wdbConfig.wEnable;

        /* now write this default to the .INI file */
        wdbWriteConfigEnable( lpszSection, lpszPrivate, lpWDBConfig );
    }

    /* set flag appropriately */
    else lpWDBConfig->wEnable = !lstrcmpi( gachTemp, gszArgYes );

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigEnable() */


/** BOOL FAR PASCAL wdbWriteConfigFontName( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigFontName( LPSTR       lpszSection,
                                        LPSTR       lpszPrivate,
                                        LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;
    LPSTR   lpsz;

    /* create an appropriate string */
    lpsz = (LPSTR)(*lpWDBConfig->szFontName ? lpWDBConfig->szFontName :
                                              gszDefFontName );

    /* force double-quotes so spaces are preserved */
    gachTemp[ 0 ] = '\"';
    lstrcpy( &gachTemp[ 1 ], lpsz );
    gachTemp[ LF_FACESIZE - 2 ];
    lstrcat( gachTemp, "\"" );

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyFontName,
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyFontName, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigFontName() */


/** BOOL FAR PASCAL wdbReadConfigFontName( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigFontName( LPSTR       lpszSection,
                                       LPSTR       lpszPrivate,
                                       LPWDBCONFIG lpWDBConfig )
{
    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        GetPrivateProfileString( lpszSection, gszKeyFontName, gszDefzYz,
                                    gachTemp, sizeof( gachTemp ),
                                    lpszPrivate );
    }

    else
    {
        GetProfileString( lpszSection, gszKeyFontName, gszDefzYz,
                                gachTemp, sizeof( gachTemp ) );
    }

    /* was there a gszKeyFontName in the .INI file? */
    if ( !lstrcmpi( gachTemp, gszDefzYz ) )
    {
        /* no! grab default value */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
        {
            lstrcpy( lpWDBConfig->szFontName,
                                gWDBDefSession.wdbConfig.szFontName );
        }

        /* set default default */
        else lstrcpy( lpWDBConfig->szFontName, gszDefFontName );

        /* now write this default to the .INI file */
        wdbWriteConfigFontName( lpszSection, lpszPrivate, lpWDBConfig );
    }

    else lstrcpy( lpWDBConfig->szFontName, gachTemp );

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigFontName() */


/** BOOL FAR PASCAL wdbWriteConfigFontSize( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigFontSize( LPSTR       lpszSection,
                                        LPSTR       lpszPrivate,
                                        LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;

    if ( lpWDBConfig->wFontSize > WDBC_MAX_FONTSIZE )
        lpWDBConfig->wFontSize = WDBC_MAX_FONTSIZE;

    /* get an appropriate string argument */
    wsprintf( gachTemp, "%u", lpWDBConfig->wFontSize );

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyFontSize,
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyFontSize, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigFontSize() */


/** BOOL FAR PASCAL wdbReadConfigFontSize( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigFontSize( LPSTR       lpszSection,
                                       LPSTR       lpszPrivate,
                                       LPWDBCONFIG lpWDBConfig )
{
    int     nSize;

    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        nSize = GetPrivateProfileInt( lpszSection, gszKeyFontSize,
                                            0, lpszPrivate );
    }

    else nSize = GetProfileInt( lpszSection, gszKeyFontSize, 0 );

    /* was there a gszKeyFontSize in the .INI file? */
    if ( !nSize )
    {
        /* no! grab default value--check actually isn't necessary */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
            lpWDBConfig->wFontSize = gWDBDefSession.wdbConfig.wFontSize;

        /* now write this default to the .INI file */
        wdbWriteConfigFontSize( lpszSection, lpszPrivate, lpWDBConfig );

        /* set nLines to what we wrote */
        nSize = lpWDBConfig->wFontSize;
    }

    /* sanity check--set width appropriately */
    lpWDBConfig->wFontSize = (nSize > WDBC_MAX_FONTSIZE) ?
                                WDBC_MAX_FONTSIZE : nSize;

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigFontSize() */


/** BOOL FAR PASCAL wdbWriteConfigFontBold( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigFontBold( LPSTR       lpszSection,
                                        LPSTR       lpszPrivate,
                                        LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;

    /* get an appropriate string argument */
    lstrcpy( gachTemp, lpWDBConfig->wFontBold ? gszArgYes : gszArgNo );

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyFontBold, 
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyFontBold, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigFontBold() */


/** BOOL FAR PASCAL wdbReadConfigFontBold( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigFontBold( LPSTR       lpszSection,
                                       LPSTR       lpszPrivate,
                                       LPWDBCONFIG lpWDBConfig )
{
    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        GetPrivateProfileString( lpszSection, gszKeyFontBold, gszDefzYz,
                                    gachTemp, sizeof( gachTemp ),
                                    lpszPrivate );
    }

    else
    {
        GetProfileString( lpszSection, gszKeyFontBold, gszDefzYz,
                                gachTemp, sizeof( gachTemp ) );
    }

    /* was there a gszKeyFontBold in the .INI file? */
    if ( !lstrcmpi( gachTemp, gszDefzYz ) )
    {
        /* no! grab default value--check actually isn't necessary */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
            lpWDBConfig->wFontBold = gWDBDefSession.wdbConfig.wFontBold;

        /* now write this default to the .INI file */
        wdbWriteConfigFontBold( lpszSection, lpszPrivate, lpWDBConfig );
    }

    /* set flag appropriately */
    else lpWDBConfig->wFontBold = lstrcmpi( gachTemp, gszArgNo );

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigFontBold() */


/** BOOL FAR PASCAL wdbWriteConfigWindowX( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigWindowX( LPSTR       lpszSection,
                                       LPSTR       lpszPrivate,
                                       LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;

    /* get an appropriate string argument */
    wsprintf( gachTemp, "%d", lpWDBConfig->nWindowX );

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyWindowX,
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyWindowX, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigWindowX() */


/** BOOL FAR PASCAL wdbReadConfigWindowX( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigWindowX( LPSTR       lpszSection,
                                      LPSTR       lpszPrivate,
                                      LPWDBCONFIG lpWDBConfig )
{
    int     nX;

    /* read as string cuz GetProfileInt() is stupid--no negs!!! */
    if ( lpszPrivate )
    {
        GetPrivateProfileString( lpszSection, gszKeyWindowX, gszDefzYz,
                                    gachTemp, sizeof( gachTemp ),
                                    lpszPrivate );
    }

    else
    {
        GetProfileString( lpszSection, gszKeyWindowX, gszDefzYz,
                                gachTemp, sizeof( gachTemp ) );
    }

    /* was there a gszKeyWindowX in the .INI file? */
    if ( !lstrcmpi( gachTemp, gszDefzYz ) )
    {
        /* no! grab default value--check actually isn't necessary */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
            lpWDBConfig->nWindowX = gWDBDefSession.wdbConfig.nWindowX;

        /* now write this default to the .INI file */
        wdbWriteConfigWindowX( lpszSection, lpszPrivate, lpWDBConfig );

        /* set nLines to what we wrote */
        nX = lpWDBConfig->nWindowX;
    }

    /* parse the sucker */
    else nX = atoi( gachTemp );

    /* set X appropriately */
    lpWDBConfig->nWindowX = nX;

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigWindowX() */


/** BOOL FAR PASCAL wdbWriteConfigWindowY( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigWindowY( LPSTR       lpszSection,
                                          LPSTR       lpszPrivate,
                                          LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;

    /* get an appropriate string argument */
    wsprintf( gachTemp, "%d", lpWDBConfig->nWindowY );

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyWindowY,
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyWindowY, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigWindowY() */


/** BOOL FAR PASCAL wdbReadConfigWindowY( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigWindowY( LPSTR       lpszSection,
                                         LPSTR       lpszPrivate,
                                         LPWDBCONFIG lpWDBConfig )
{
    int     nY;

    /* read as string cuz GetProfileInt() is stupid--no negs!!! */
    if ( lpszPrivate )
    {
        GetPrivateProfileString( lpszSection, gszKeyWindowY, gszDefzYz,
                                    gachTemp, sizeof( gachTemp ),
                                    lpszPrivate );
    }

    else
    {
        GetProfileString( lpszSection, gszKeyWindowY, gszDefzYz,
                                gachTemp, sizeof( gachTemp ) );
    }

    /* was there a gszKeyWindowY in the .INI file? */
    if ( !lstrcmpi( gachTemp, gszDefzYz ) )
    {
        /* no! grab default value--check actually isn't necessary */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
            lpWDBConfig->nWindowY = gWDBDefSession.wdbConfig.nWindowY;

        /* now write this default to the .INI file */
        wdbWriteConfigWindowY( lpszSection, lpszPrivate, lpWDBConfig );

        /* set nLines to what we wrote */
        nY = lpWDBConfig->nWindowY;
    }

    /* parse the sucker */
    else nY = atoi( gachTemp );

    /* set X appropriately */
    lpWDBConfig->nWindowY = nY;

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigWindowY() */


/** BOOL FAR PASCAL wdbWriteConfigWindowW( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigWindowW( LPSTR       lpszSection,
                                          LPSTR       lpszPrivate,
                                          LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;

    /* get an appropriate string argument */
    wsprintf( gachTemp, "%u", lpWDBConfig->wWindowW );

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyWindowW,
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyWindowW, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigWindowW() */


/** BOOL FAR PASCAL wdbReadConfigWindowW( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigWindowW( LPSTR       lpszSection,
                                         LPSTR       lpszPrivate,
                                         LPWDBCONFIG lpWDBConfig )
{
    int     nX;

    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        nX = GetPrivateProfileInt( lpszSection, gszKeyWindowW,
                                            0, lpszPrivate );
    }

    else nX = GetProfileInt( lpszSection, gszKeyWindowW, 0 );

    /* was there a gszKeyWindowW in the .INI file? */
    if ( !nX )
    {
        /* no! grab default value--check actually isn't necessary */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
            lpWDBConfig->wWindowW = gWDBDefSession.wdbConfig.wWindowW;

        /* now write this default to the .INI file */
        wdbWriteConfigWindowW( lpszSection, lpszPrivate, lpWDBConfig );

        /* set nLines to what we wrote */
        nX = lpWDBConfig->wWindowW;
    }

    /* set X appropriately */
    lpWDBConfig->wWindowW = nX;

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigWindowW() */


/** BOOL FAR PASCAL wdbWriteConfigWindowH( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigWindowH( LPSTR       lpszSection,
                                          LPSTR       lpszPrivate,
                                          LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;

    /* get an appropriate string argument */
    wsprintf( gachTemp, "%u", lpWDBConfig->wWindowH );

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyWindowH,
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyWindowH, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigWindowH() */


/** BOOL FAR PASCAL wdbReadConfigWindowH( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigWindowH( LPSTR       lpszSection,
                                         LPSTR       lpszPrivate,
                                         LPWDBCONFIG lpWDBConfig )
{
    int     nX;

    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        nX = GetPrivateProfileInt( lpszSection, gszKeyWindowH,
                                            0, lpszPrivate );
    }

    else nX = GetProfileInt( lpszSection, gszKeyWindowH, 0 );

    /* was there a gszKeyWindowH in the .INI file? */
    if ( !nX )
    {
        /* no! grab default value--check actually isn't necessary */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
            lpWDBConfig->wWindowH = gWDBDefSession.wdbConfig.wWindowH;

        /* now write this default to the .INI file */
        wdbWriteConfigWindowH( lpszSection, lpszPrivate, lpWDBConfig );

        /* set nLines to what we wrote */
        nX = lpWDBConfig->wWindowH;
    }

    /* set X appropriately */
    lpWDBConfig->wWindowH = nX;

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigWindowH() */


/** BOOL FAR PASCAL wdbWriteConfigMaxLines( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *                                      
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigMaxLines( LPSTR       lpszSection,
                                        LPSTR       lpszPrivate,
                                        LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;

    /* get an appropriate string argument */
    wsprintf( gachTemp, "%u", lpWDBConfig->wMaxLines );

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyMaxLines,
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyMaxLines, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigMaxLines() */


/** BOOL FAR PASCAL wdbReadConfigMaxLines( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigMaxLines( LPSTR       lpszSection,
                                       LPSTR       lpszPrivate,
                                       LPWDBCONFIG lpWDBConfig )
{
    int     nLines;

    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        nLines = GetPrivateProfileInt( lpszSection, gszKeyMaxLines,
                                            0, lpszPrivate );
    }

    else nLines = GetProfileInt( lpszSection, gszKeyMaxLines, 0 );

    /* was there a gszKeyMaxLines in the .INI file? */
    if ( !nLines )
    {
        /* no! grab default value--check actually isn't necessary */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
            lpWDBConfig->wMaxLines = gWDBDefSession.wdbConfig.wMaxLines;

        /* now write this default to the .INI file */
        wdbWriteConfigMaxLines( lpszSection, lpszPrivate, lpWDBConfig );

        /* set nLines to what we wrote */
        nLines = lpWDBConfig->wMaxLines;
    }

    /* sanity check--set number of lines appropriately */
    lpWDBConfig->wMaxLines = (nLines > WDBC_MAX_MAXLINES) ?
                                WDBC_MAX_MAXLINES : nLines;

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigMaxLines() */


/** BOOL FAR PASCAL wdbWriteConfigAssert( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigAssert( LPSTR       lpszSection,
                                      LPSTR       lpszPrivate,
                                      LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;
    LPSTR   lpszAssert;

    /* get an appropriate string argument */
    switch ( lpWDBConfig->wAssert )
    {
        case WDB_ASSERT_OFF:
            lpszAssert = (LPSTR)gszArgOff;
        break;

        case WDB_ASSERT_SYSMODAL:
            lpszAssert = (LPSTR)gszArgSysModal;
        break;

        case WDB_ASSERT_BEEP:
            lpszAssert = (LPSTR)gszArgBeep;
        break;

        default:
            lpszAssert = (LPSTR)gszArgWindow;
        break;
    }

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyAssert,
                                                lpszAssert, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyAssert, lpszAssert );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigAssert() */


/** BOOL FAR PASCAL wdbReadConfigAssert( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigAssert( LPSTR       lpszSection,
                                     LPSTR       lpszPrivate,
                                     LPWDBCONFIG lpWDBConfig )
{
    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        GetPrivateProfileString( lpszSection, gszKeyAssert, gszDefzYz,
                                    gachTemp, sizeof( gachTemp ),
                                    lpszPrivate );
    }

    else
    {
        GetProfileString( lpszSection, gszKeyAssert, gszDefzYz,
                                gachTemp, sizeof( gachTemp ) );
    }

    /* was there a gszKeyAssert in the .INI file? */
    if ( !lstrcmpi( gachTemp, gszDefzYz ) )
    {
        /* no! grab default value--check actually isn't necessary */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
            lpWDBConfig->wAssert = gWDBDefSession.wdbConfig.wAssert;

        /* now write this default to the .INI file */
        wdbWriteConfigAssert( lpszSection, lpszPrivate, lpWDBConfig );
    }

    /* set assert type appropriately */
    else
    {
        UINT    wType = WDB_ASSERT_OFF;

        /* determine type: WDB_ASSERT_OFF, _WINDOW, _BEEP, _SYSMODAL */
        if ( lstrcmpi( gachTemp, gszArgOff ) && ++wType &&
             lstrcmpi( gachTemp, gszArgWindow ) && ++wType &&
             lstrcmpi( gachTemp, gszArgSysModal ) && ++wType &&
             lstrcmpi( gachTemp, gszArgBeep ) && ++wType )
            ;

        /* one last sanity check */
        if ( wType > WDB_ASSERT_MAX )
            lpWDBConfig->wAssert = WDB_ASSERT_WINDOW;
        else
            lpWDBConfig->wAssert = wType;
    }

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigAssert() */


/** BOOL FAR PASCAL wdbWriteConfigLevel( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigLevel( LPSTR       lpszSection,
                                     LPSTR       lpszPrivate,
                                     LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;
    UINT    wLevel;

    wLevel = lpWDBConfig->wLevel;

    /* hack any garbage */
    if ( wLevel > WDB_LEVEL_MAX )
        wLevel = WDB_LEVEL_MAX;

    gachTemp[0] = (char)(wLevel + '0');
    gachTemp[1] = '\0';

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyLevel,
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyLevel, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigLevel() */


/** BOOL FAR PASCAL wdbReadConfigLevel( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigLevel( LPSTR       lpszSection,
                                    LPSTR       lpszPrivate,
                                    LPWDBCONFIG lpWDBConfig )
{
    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        GetPrivateProfileString( lpszSection, gszKeyLevel, gszDefzYz,
                                    gachTemp, sizeof( gachTemp ),
                                    lpszPrivate );
    }

    else
    {
        GetProfileString( lpszSection, gszKeyLevel, gszDefzYz,
                                gachTemp, sizeof( gachTemp ) );
    }

    /* was there a gszKeyLevel in the .INI file? */
    if ( !lstrcmpi( gachTemp, gszDefzYz ) )
    {
        /* no! grab default value--check actually isn't necessary */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
            lpWDBConfig->wLevel = gWDBDefSession.wdbConfig.wLevel;

        /* now write this default to the .INI file */
        wdbWriteConfigLevel( lpszSection, lpszPrivate, lpWDBConfig );
    }

    /* set level appropriately */
    else
    {
        UINT wLevel = (UINT)(gachTemp[0] - '0');

        /* this is cheezy--taste good on pizza? */
        if ( wLevel > WDB_LEVEL_MAX )
            wLevel = WDB_LEVEL_MAX;

        lpWDBConfig->wLevel = wLevel;
    }

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigLevel() */


/** BOOL FAR PASCAL wdbWriteConfigOutput( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigOutput( LPSTR       lpszSection,
                                      LPSTR       lpszPrivate,
                                      LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;
    LPSTR   lpszOutput;

    /* get an appropriate string argument */
    switch ( lpWDBConfig->wOutput )
    {
        case WDB_OUTPUT_OFF:
            lpszOutput = (LPSTR)gszArgOff;
        break;

        case WDB_OUTPUT_WINDOW:
            lpszOutput = (LPSTR)gszArgWindow;
        break;

        case WDB_OUTPUT_FILE:
            lpszOutput = (LPSTR)gszArgFile;
        break;

        case WDB_OUTPUT_ODS:
            lpszOutput = (LPSTR)gszArgOds;
        break;

        case WDB_OUTPUT_MONO:
            lpszOutput = (LPSTR)gszArgMono;
        break;

        default:
            lpszOutput = (LPSTR)gszArgAux;
        break;
    }

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyOutput,
                                                lpszOutput, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyOutput, lpszOutput );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigOutput() */


/** BOOL FAR PASCAL wdbReadConfigOutput( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigOutput( LPSTR       lpszSection,
                                     LPSTR       lpszPrivate,
                                     LPWDBCONFIG lpWDBConfig )
{
    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        GetPrivateProfileString( lpszSection, gszKeyOutput, gszDefzYz,
                                    gachTemp, sizeof( gachTemp ),
                                    lpszPrivate );
    }

    else
    {
        GetProfileString( lpszSection, gszKeyOutput, gszDefzYz,
                                gachTemp, sizeof( gachTemp ) );
    }

    /* was there a gszKeyOutput in the .INI file? */
    if ( !lstrcmpi( gachTemp, gszDefzYz ) )
    {
        /* no! grab default value--check actually isn't necessary */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
            lpWDBConfig->wOutput = gWDBDefSession.wdbConfig.wOutput;

        /* now write this default to the .INI file */
        wdbWriteConfigOutput( lpszSection, lpszPrivate, lpWDBConfig );
    }

    /* set output type appropriately */
    else
    {
        UINT    wType = WDB_OUTPUT_OFF;

        /* determine dest: WDB_OUTPUT_OFF, _WINDOW, _AUX, <file> */
        if ( lstrcmpi( gachTemp, gszArgOff ) && ++wType &&
             lstrcmpi( gachTemp, gszArgWindow ) && ++wType &&
             lstrcmpi( gachTemp, gszArgAux ) && ++wType &&
             lstrcmpi( gachTemp, gszArgFile ) && ++wType &&
             lstrcmpi( gachTemp, gszArgOds ) && ++wType &&
             lstrcmpi( gachTemp, gszArgMono ) && ++wType)
            ;

        /* sanity check */
        if ( wType > WDB_OUTPUT_MAX )  wType = WDB_OUTPUT_ODS;

        /* set output type */
        lpWDBConfig->wOutput = wType;

    }

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigOutput() */


/** BOOL FAR PASCAL wdbWriteConfigPrelude( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigPrelude( LPSTR       lpszSection,
                                       LPSTR       lpszPrivate,
                                       LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;
    LPSTR   lpsz;

    /* create an appropriate string */
    lpsz = (LPSTR)(*lpWDBConfig->szPrelude ? lpWDBConfig->szPrelude : gszArgNo);

    /* force double-quotes so spaces are preserved */
    gachTemp[ 0 ] = '\"';
    lstrcpy( &gachTemp[ 1 ], lpsz );
    gachTemp[ WDBC_MAX_PRELUDE - 2 ];
    lstrcat( gachTemp, "\"" );
        
    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyPrelude,
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyPrelude, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigPrelude() */


/** BOOL FAR PASCAL wdbReadConfigPrelude( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigPrelude( LPSTR       lpszAppName,
                                      LPSTR       lpszSection,
                                      LPSTR       lpszPrivate,
                                      LPWDBCONFIG lpWDBConfig )
{
    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        GetPrivateProfileString( lpszSection, gszKeyPrelude, gszDefzYz,
                                    gachTemp, sizeof( gachTemp ),
                                    lpszPrivate );
    }

    else
    {
        GetProfileString( lpszSection, gszKeyPrelude, gszDefzYz,
                                gachTemp, sizeof( gachTemp ) );
    }

    /* was there a gszKeyPrelude in the .INI file? */
    if ( !lstrcmpi( gachTemp, gszDefzYz ) )
    {
        /* no! create default value */
        lstrcpy( lpWDBConfig->szPrelude, lpszAppName );
        lstrcat( lpWDBConfig->szPrelude, ": " );

        /* now write this default to the .INI file */
        wdbWriteConfigPrelude( lpszSection, lpszPrivate, lpWDBConfig );
    }

    /* set prelude appropriately */
    else
    {
        /* does the user want a prelude? */
        if ( lstrcmpi( gachTemp, gszArgNo ) )
            lstrcpy( lpWDBConfig->szPrelude, gachTemp );
        else
            lpWDBConfig->szPrelude[ 0 ] = '\0';
    }

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigPrelude() */


/** BOOL FAR PASCAL wdbWriteConfigFile( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigFile( LPSTR       lpszSection,
                                    LPSTR       lpszPrivate,
                                    LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;
    LPSTR   lpsz;

    /* create an appropriate string */
    lpsz = (LPSTR)(*lpWDBConfig->szOutFile ? lpWDBConfig->szOutFile : gszDefFile);

    /* force double-quotes so spaces are preserved */
    gachTemp[ 0 ] = '\"';
    lstrcpy( &gachTemp[ 1 ], lpsz );
    gachTemp[ WDBC_MAX_FILELEN - 2 ];
    lstrcat( gachTemp, "\"" );
        
    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyFile,
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyFile, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigFile() */


/** BOOL FAR PASCAL wdbReadConfigFile( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigFile( LPSTR       lpszSection,
                                   LPSTR       lpszPrivate,
                                   LPWDBCONFIG lpWDBConfig )
{
    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        GetPrivateProfileString( lpszSection, gszKeyFile, gszDefzYz,
                                    gachTemp, sizeof( gachTemp ),
                                    lpszPrivate );
    }

    else
    {
        GetProfileString( lpszSection, gszKeyFile, gszDefzYz,
                                gachTemp, sizeof( gachTemp ) );
    }

    /* was there a gszKeyFile in the .INI file? */
    if ( !lstrcmpi( gachTemp, gszDefzYz ) )
    {
        /* no! create default value */
        lstrcpy( lpWDBConfig->szOutFile, gszDefFile );

        /* now write this default to the .INI file */
        wdbWriteConfigFile( lpszSection, lpszPrivate, lpWDBConfig );
    }

    /* set output file name appropriately */
    else lstrcpy( lpWDBConfig->szOutFile, gachTemp );

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigFile() */


/** BOOL FAR PASCAL wdbWriteConfigLFtoCRLF( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigLFtoCRLF( LPSTR       lpszSection,
                                        LPSTR       lpszPrivate,
                                        LPWDBCONFIG lpWDBConfig )
{
    BOOL    fReturn;

    /* get an appropriate string argument */
    lstrcpy( gachTemp, lpWDBConfig->wLFtoCRLF ? gszArgYes : gszArgNo );

    /* write to the correct .INI file */
    if ( lpszPrivate )
    {
        fReturn = WritePrivateProfileString( lpszSection, gszKeyLFtoCRLF, 
                                                gachTemp, lpszPrivate );
    }

    else fReturn = WriteProfileString( lpszSection, gszKeyLFtoCRLF, gachTemp );

    /* return the result */
    return ( fReturn );
} /* wdbWriteConfigLFtoCRLF() */


/** BOOL FAR PASCAL wdbReadConfigLFtoCRLF( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      ( LPSTR lpszSection,
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigLFtoCRLF( LPSTR       lpszSection,
                                       LPSTR       lpszPrivate,
                                       LPWDBCONFIG lpWDBConfig )
{
    /* grab info from appropriate .INI file */
    if ( lpszPrivate )
    {
        GetPrivateProfileString( lpszSection, gszKeyLFtoCRLF, gszDefzYz,
                                    gachTemp, sizeof( gachTemp ),
                                    lpszPrivate );
    }

    else
    {
        GetProfileString( lpszSection, gszKeyLFtoCRLF, gszDefzYz,
                                gachTemp, sizeof( gachTemp ) );
    }

    /* was there a gszKeyLFtoCRLF in the .INI file? */
    if ( !lstrcmpi( gachTemp, gszDefzYz ) )
    {
        /* no! grab default value--check actually isn't necessary */
        if ( lpWDBConfig != (LPWDBCONFIG)&gWDBDefSession )
            lpWDBConfig->wLFtoCRLF = gWDBDefSession.wdbConfig.wLFtoCRLF;

        /* now write this default to the .INI file */
        wdbWriteConfigLFtoCRLF( lpszSection, lpszPrivate, lpWDBConfig );
    }

    /* set flag appropriately */
    else lpWDBConfig->wLFtoCRLF = lstrcmpi( gachTemp, gszArgNo );

    /* everything cool */
    return ( TRUE );
} /* wdbReadConfigLFtoCRLF() */


/** BOOL FAR PASCAL wdbReadConfigInfo( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      LPSTR lpszAppName       :   Pointer to sz application name buffer.
 *                                  This is used for the section name like
 *                                  this:  [wdb-AppName].  The 'wdb-' is
 *                                  automatically prepended
 *
 *      LPSTR lpszPrivate       :   Pointer to private .INI file containing
 *                                  the configuration info to get.  If
 *                                  this is NULL, WIN.INI is used.
 *
 *      LPWDBCONFIG lpWDBConfig :   Pointer to WDBCONFIG structure to hold
 *                                  the configuration info.
 *
 *  RETURN (BOOL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbReadConfigInfo( LPSTR       lpszAppName,
                                   LPSTR       lpszSection,
                                   LPSTR       lpszPrivate,
                                   LPWDBCONFIG lpWDBConfig )
{
    OD( "WDB: wdbReadConfigInfo\r\n" ) ;

    wdbHourGlass( TRUE );

#if 0
    /*  Flush private profile .INI file cache.  If not private .INI,
     *  I am assuming that the WM_WININICHANGE message has been sent
     *  if the WIN.INI file was changed.  NOTEPAD.EXE does this, in
     *  case you're curious.
     */
    if ( lpszPrivate )
        WritePrivateProfileString( NULL, NULL, NULL, lpszPrivate );

#endif

    /* read the config info */
    wdbReadConfigEnable( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigAssert( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigLevel( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigOutput( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigFile( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigPrelude( lpszAppName, lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigLFtoCRLF( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigMaxLines( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigFontName( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigFontSize( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigFontBold( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigWindowX( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigWindowY( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigWindowW( lpszSection, lpszPrivate, lpWDBConfig );
    wdbReadConfigWindowH( lpszSection, lpszPrivate, lpWDBConfig );

    wdbHourGlass( FALSE );

    /* return success */
    return ( TRUE );
} /* wdbReadConfigInfo() */



/** BOOL FAR PASCAL wdbWriteConfigInfo( LPSTR, LPSTR, LPWDBCONFIG )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *      LPSTR lpszAppName       :   Pointer to sz application name buffer.
 *                                  This is used for the section name like
 *                                  this:  [wdb-AppName].  The 'wdb-' is
 *                                  automatically prepended
 *
 *      LPSTR lpszPrivate       :   Pointer to private .INI file to receive
 *                                  the configuration info.  If this is
 *                                  NULL, WIN.INI is the destination.
 *
 *      LPWDBCONFIG lpWDBConfig :   Pointer to WDBCONFIG structure to write
 *                                  the configuration info from.
 *
 *  RETURN (BOOL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteConfigInfo( LPSTR       lpszSection,
                                    LPSTR       lpszPrivate,
                                    LPWDBCONFIG lpWDBConfig )
{
    wdbHourGlass( TRUE );

    /* write the config info */
    wdbWriteConfigEnable( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigAssert( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigLevel( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigOutput( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigFile( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigPrelude( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigLFtoCRLF( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigMaxLines( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigFontName( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigFontSize( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigFontBold( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigWindowX( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigWindowY( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigWindowW( lpszSection, lpszPrivate, lpWDBConfig );
    wdbWriteConfigWindowH( lpszSection, lpszPrivate, lpWDBConfig );

    /*  Did we mess with the WIN.INI?  I hope not!  The WIN.INI is
     *  for *WINDOWS*, not someone's bloody Windows application.
     *  Use _private_ profiles if at all possible.  This improves
     *  overall performance and keeps your users happy.
     */
    if ( !lpszPrivate )
        SendMessage( (HWND)-1, WM_WININICHANGE, 0, (LONG)(LPSTR)lpszSection );

    wdbHourGlass( FALSE );

    /* return success */
    return ( TRUE );
} /* wdbWriteConfigInfo() */


/** EOF: wdbcfg.c **/
