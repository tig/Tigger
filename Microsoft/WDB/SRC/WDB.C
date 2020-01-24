/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** wdb.c
 *
 *  DESCRIPTION: 
 *      This file contains most of the API exported by WDB.
 *
 *  HISTORY:
 *      4/30/91     cjp     increased size of max print string to 2kb--this
 *                          is by using gachBigTemp[].
 *      2/17/91     cjp     wrote it
 *
 ** cjp */


/* grab the includes we need */
    #include <windows.h>
    #include "wdb.h"
    #include "wdbi.h"



/** UINT FAR PASCAL wdbLibVersion( UINT wVersion )
 *
 *  DESCRIPTION: 
 *      This function checks wVersion against the actual version of
 *      this library.  If this library is compatible with the version
 *      requested, TRUE is returned.  All functions from wVersion should
 *      work.  If wVersion is incompatible with this library, then FALSE
 *      is returned.  If this is the case, the application should NOT
 *      use this library.
 *
 *  ARGUMENTS:
 *      UINT wVersion   :   Version to check against.  This is defined
 *                          in wdb.h as WDB_LIBVERSION.  If this argument
 *                          is NULL, the return is this .DLL's version.
 *                          HIBYTE() is major version number; LOBYTE()
 *                          is minor version number.
 *
 *  RETURN (UINT):
 *      If it is ok to use this library, TRUE is returned.  Otherwise,
 *      FALSE is returned and the calling application should not use
 *      this library.
 *
 *      If wVersion is NULL, then the HIBYTE() of the return value is
 *      the major version number of this library.  LOBYTE() is the
 *      minor version number.
 *
 *  NOTES:
 *
 ** cjp */

UINT FAR PASCAL wdbLibVersion( UINT wVersion )
{
    /*  For now, just verify that WDB_LIBVERSION is greater or equal
     *  to wVersion.  Although, there is no reason more elaborate 
     *  checks can't be made.  The HIBYTE() is the major version
     *  number; LOBYTE() is the minor version number.  Fixes should
     *  not change the compatibility of a library.
     */

    if ( wVersion )
        return ( (HIBYTE(wVersion) <=
                  HIBYTE(WDB_LIBVERSION) ) ? TRUE : FALSE );

    /* return version of this library */
    return ( WDB_LIBVERSION );
} /* wdbLibVersion() */


/** UINT FAR PASCAL wdbGetInfo( HWDB hWDB, UINT wIndex )
 *
 *  DESCRIPTION: 
 *      This function is used to quickly grab the configuration value
 *      of a session depicted by wIndex.  If hWDB is NULL or can not
 *      be GlobalLock()'d (ouch!), the value will be extracted from
 *      gWDBDefSession.
 *
 *  ARGUMENTS:
 *      HWDB hWDB   :   Handle to WDB session.  If this is NULL, then
 *                      the gWDBDefSession structure will be used.  This
 *                      will also be used if hWDB can not be locked.
 *
 *      UINT wIndex :   Index depicting the value to retrieve.  For
 *                      example: WDBI_HWND gets pSession->hWnd.
 *
 *  RETURN (UINT):
 *      Always a UINT.  Should then be cast to whatever wIndex is
 *      supposed to represent.
 *
 *  NOTES:
 *      This function makes the assumption that all fields in the
 *      WDBCONFIG and WDBSESSION structures are UINT's.  This is not
 *      the most memory efficient, but it is quite a bit faster for
 *      our case.
 *
 ** cjp */

UINT FAR PASCAL wdbGetInfo( HWDB hWDB, UINT wIndex )
{
    UINT   FAR *lpInfo;
    UINT        wReturn;

    /* lock it--if we can */
    if ( hWDB && (lpInfo = (UINT FAR *)GlobalLock( hWDB )) )
    {
        wReturn = lpInfo[ wIndex ];

        /* unlock the session block and return wReturn */
        GlobalUnlock( hWDB );

        /* return the UINT value */
        return ( wReturn );
    }

    /* return the default value */
    return ( ((UINT *)(&gWDBDefSession))[ wIndex ] );
} /* wdbGetInfo() */


/** UINT FAR PASCAL wdbSetInfo( HWDB hWDB, UINT wIndex, UINT wValue )
 *
 *  DESCRIPTION: 
 *      This is a DANGEROUS call.  Maybe it should not be exported.  If
 *      you change something, you better hope you know what you are doing.
 *
 *          *** THIS IS NOT HOW YOU CHANGE THE OUTPUT DEVICE ***
 *
 *      This function is used to quickly set the configuration value
 *      of a session depicted by wIndex.  If hWDB is NULL, the value will
 *      be set in gWDBDefSession.
 *
 *  ARGUMENTS:
 *      HWDB hWDB   :   Handle to WDB session.  If this is NULL, then
 *                      the gWDBDefSession structure will be used.
 *
 *      UINT wIndex :   Index depicting the value to set.  For example:
 *                      WDBI_HWND sets pSession->hWnd to wValue.
 *
 *      UINT wValue :   Value to set the WDBI_* UINT to.
 *
 *  RETURN (UINT):
 *      The old value of what wIndex represents.  Always a UINT.  Should
 *      then be cast to whatever wIndex is supposed to represent.
 *
 *  NOTES:
 *      This function makes the assumption that all fields in the
 *      WDBCONFIG and WDBSESSION structures are UINT's.  This is not
 *      the most memory efficient, but it is quite a bit faster for
 *      our case.
 *
 ** cjp */

UINT FAR PASCAL wdbSetInfo( HWDB hWDB, UINT wIndex, UINT wValue )
{
    UINT   FAR *lpInfo;
    UINT        wReturn;

    /* iff hWDB is not NULL */
    if ( hWDB )
    {
        /* try to lock it */
        if ( lpInfo = (UINT FAR *)GlobalLock( hWDB ) )
        {
            /* get the return and set the new value */
            wReturn = lpInfo[ wIndex ];
            lpInfo[ wIndex ] = wValue;

            /* unlock the session block and return wReturn */
            GlobalUnlock( hWDB );
        }

        /* something is hosed--don't make matters worse! */
        wReturn = NULL;
    }

    /* hWDB is NULL, so go for the jugular! */
    else
    {
        lpInfo = (UINT *)(&gWDBDefSession);

        /* get the return and set the new value */
        wReturn = lpInfo[ wIndex ];
        lpInfo[ wIndex ] = wValue;
    }

    /* return the UINT value */
    return ( wReturn );
} /* wdbSetInfo() */


/** BOOL FAR PASCAL wdbGetOutputFile( HWDB hWDB, LPSTR lpDest )
 *
 *  DESCRIPTION: 
 *      This function is used to quickly grab the output file name.  It
 *      will succeed if hWDB is valid.  This does not automatically mean
 *      that file output is active... use wdbGetInfo() to determine that
 *      information.
 *
 *  ARGUMENTS:
 *      HWDB hWDB   :   Handle to WDB session.
 *
 *      LPSTR lpDest:   Pointer to buffer to receive the output file name.
 *                      This buffer must be at least WDBC_MAX_FILELEN (144)
 *                      chars in length.
 *
 *  RETURN (BOOL):
 *      The return value is TRUE if the function is successful.  FALSE
 *      if it fails.
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbGetOutputFile( HWDB hWDB, LPSTR lpDest )
{
    LPWDBCONFIG lpWDBConfig;
    BOOL        fReturn = FALSE;

    /* safety feature */
    lpDest[ 0 ] = '\0';

    /* default or unique session? */
    if ( hWDB )
    {
        /* lock 'er down */
        if ( lpWDBConfig = (LPWDBCONFIG)GlobalLock( hWDB ) )
        {
            /* copy the bits to the caller's buffer */
            lstrcpy( lpDest, lpWDBConfig->szOutFile );
            fReturn = TRUE;

            /* unlock the session block and return wReturn */
            GlobalUnlock( hWDB );
        }

        /* else error */
    }

    else
    {
        /* copy the bits to the caller's buffer */
        lstrcpy( lpDest, gWDBDefSession.wdbConfig.szOutFile );
        fReturn = TRUE;
    }

    /* return outcome of function */
    return ( fReturn );
} /* wdbGetOutputFile() */


/** BOOL FAR PASCAL wdbGetOutputPrelude( HWDB hWDB, LPSTR lpDest )
 *
 *  DESCRIPTION: 
 *      This function is used to quickly grab the output prelude string.
 *
 *  ARGUMENTS:
 *      HWDB hWDB   :   Handle to WDB session.
 *
 *      LPSTR lpDest:   Pointer to buffer to receive the prelude string.
 *                      This buffer must be at least WDBC_MAX_PRELUDE (30)
 *                      chars in length.
 *
 *  RETURN (BOOL):
 *      The return value is TRUE if there is a prelude string associated
 *      with hWDB.  It is FALSE if there is no prelude or hWDB is invalid.
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbGetOutputPrelude( HWDB hWDB, LPSTR lpDest )
{
    LPWDBCONFIG lpWDBConfig;
    BOOL        fReturn = FALSE;

    /* safety feature */
    lpDest[ 0 ] = '\0';

    /* default or unique session? */
    if ( hWDB )
    {
        /* lock 'er down */
        if ( lpWDBConfig = (LPWDBCONFIG)GlobalLock( hWDB ) )
        {
            /* clone a few bits into caller's buffer */
            lstrcpy( lpDest, lpWDBConfig->szPrelude );

            /* check _after_ copy so caller's buffer is empty also... */
            if ( *lpDest )
                fReturn = TRUE;

            /* unlock the session block and return wReturn */
            GlobalUnlock( hWDB );
        }
    }

    else
    {
        /* copy the bits to the caller's buffer */
        lstrcpy( lpDest, gWDBDefSession.wdbConfig.szPrelude );
        fReturn = TRUE;
    }

    /* return outcome of function */
    return ( fReturn );
} /* wdbGetOutputPrelude() */


/** BOOL FAR PASCAL wdbGetFontName( HWDB hWDB, LPSTR lpDest )
 *
 *  DESCRIPTION: 
 *      This function is used to quickly grab the font face name.
 *
 *  ARGUMENTS:
 *      HWDB hWDB   :   Handle to WDB session.
 *
 *      LPSTR lpDest:   Pointer to buffer to receive the font face name.
 *                      This buffer must be at least LF_FACESIZE (32)
 *                      chars in length.
 *
 *  RETURN (BOOL):
 *      The return value is TRUE if there is a font face name associated
 *      with hWDB.  It is FALSE if there is no font or hWDB is invalid.
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbGetFontName( HWDB hWDB, LPSTR lpDest )
{
    LPWDBCONFIG lpWDBConfig;
    BOOL        fReturn = FALSE;

    /* safety feature */
    lpDest[ 0 ] = '\0';

    /* default or unique session? */
    if ( hWDB )
    {
        /* lock 'er down */
        if ( lpWDBConfig = (LPWDBCONFIG)GlobalLock( hWDB ) )
        {
            /* blast a few bits into caller's buffer */
            lstrcpy( lpDest, lpWDBConfig->szFontName );

            /* check _after_ copy so caller's buffer is empty also... */
            if ( *lpDest )
                fReturn = TRUE;

            /* unlock the session block and return wReturn */
            GlobalUnlock( hWDB );
        }
    }

    else
    {
        /* copy the bits to the caller's buffer */
        lstrcpy( lpDest, gWDBDefSession.wdbConfig.szFontName );
        fReturn = TRUE;
    }

    /* return outcome of function */
    return ( fReturn );
} /* wdbGetFontName() */


/** void FAR PASCAL wdbDumpDebugInfo( HWDB hWDB )
 *
 *  DESCRIPTION: 
 *      This function will dump the current session information to the
 *      current session output device.  This has two uses:  1. some people
 *      have a fetish for stupid statistics  2. it helps me debug WDB!
 *
 *  ARGUMENTS:
 *      HWDB hWDB   :   The session handle to dump the information for.
 *                      If NULL, then the default WDB session is queried.
 *
 *  RETURN (void):
 *      The session information will have been dumped to its current
 *      output device.
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbDumpDebugInfo( HWDB hWDB )
{
    UINT    wOutput;
    char    szFileName[ WDBC_MAX_FILELEN ];

    wdbPuts( hWDB, "\nWDB Session Configuration Dump:" );
    wdbPrint( hWDB, "      WDBI_WENABLE: %u", wdbGetInfo( hWDB, WDBI_WENABLE ) );
    wdbPrint( hWDB, "      WDBI_WASSERT: %u", wdbGetInfo( hWDB, WDBI_WASSERT ) );
    wdbPrint( hWDB, "       WDBI_WLEVEL: %u", wdbGetInfo( hWDB, WDBI_WLEVEL ) );
    wdbPrint( hWDB, "      WDBI_WOUTPUT: %u", wOutput = wdbGetOutput( hWDB, szFileName ) );

    wdbPrint( hWDB, "       OUTPUT FILE: \"%s\"", (LPSTR)szFileName );

    wdbPrint( hWDB, "    WDBI_WLFTOCRLF: %u", wdbGetInfo( hWDB, WDBI_WLFTOCRLF ) );
    wdbPrint( hWDB, "    WDBI_WMAXLINES: %u", wdbGetInfo( hWDB, WDBI_WMAXLINES ) );

    if ( wdbGetFontName( hWDB, szFileName ) )
        wdbPrint( hWDB, "          FONTNAME: \"%s\"", (LPSTR)szFileName );

    wdbPrint( hWDB, "    WDBI_WFONTSIZE: %u", wdbGetInfo( hWDB, WDBI_WFONTSIZE ) );
    wdbPrint( hWDB, "    WDBI_WFONTBOLD: %u", wdbGetInfo( hWDB, WDBI_WFONTBOLD ) );

    wdbPrint( hWDB, "     WDBI_NWINDOWX: %d", wdbGetInfo( hWDB, WDBI_NWINDOWX ) );
    wdbPrint( hWDB, "     WDBI_NWINDOWY: %d", wdbGetInfo( hWDB, WDBI_NWINDOWY ) );
    wdbPrint( hWDB, "     WDBI_WWINDOWW: %u", wdbGetInfo( hWDB, WDBI_WWINDOWW ) );
    wdbPrint( hWDB, "     WDBI_WWINDOWH: %u", wdbGetInfo( hWDB, WDBI_WWINDOWH ) );


    if ( wdbGetOutputPrelude( hWDB, szFileName ) )
        wdbPrint( hWDB, "           PRELUDE: \"%s\"", (LPSTR)szFileName );

    wdbPrint( hWDB, "      WDBI_HOUTPUT: %.4Xh", wdbGetInfo( hWDB, WDBI_HOUTPUT ) );
    wdbPrint( hWDB, "         WDBI_HWND: %.4Xh", wdbGetInfo( hWDB, WDBI_HWND ) );
    wdbPrint( hWDB, "        WDBI_HFONT: %.4Xh", wdbGetInfo( hWDB, WDBI_HFONT ) );
    wdbPrint( hWDB, "   WDBI_WSEMAPHORE: %u\n", wdbGetInfo( hWDB, WDBI_WSEMAPHORE ) );

    /* return success */
    return ( TRUE );
} /* wdbDumpDebugInfo() */


/** BOOL FAR PASCAL wdbAssertBox( HWDB hWDB, LPSTR lpsz, LPSTR lpszFile, UINT wLine )
 *
 *  DESCRIPTION: 
 *      This function displays an assertion message box (usually) to the
 *      user.  It is called from the DASSERT() debugging macro.  When
 *      the message box is displayed, the user may choose either to
 *      ignore the assertion by pressing the "Ignore" button, break into
 *      the debugger by using the "Retry" button, or exit the application
 *      with the "Abort" button.
 *
 *      Also note that if you press shift-Ignore, then the assert is logged
 *      to the current WDB output device.
 *
 *  ARGUMENTS:
 *      HWDB hWDB       :   Handle to WDB session.  If it is NULL, the
 *                          default assert configuration is used.
 *
 *      LPSTR lpsz      :   Optional message to display.
 *
 *      LPSTR lpszFile  :   Points to the filename which contains the
 *                          assertion failure.  This is usually generated
 *                          by the C preprocessor as the symbol __FILE__.
 *
 *      UINT wLine      :   Specifies the line which contains the asser-
 *                          tion failure.  This is usually generated by
 *                          the C preprocessor as the symbol __LINE__.
 *
 *  RETURN (BOOL):
 *      If this routine returns, the return value is always FALSE (0).
 *
 *  NOTES:
 *      You should not call this function directly.  Instead, use the
 *      DASSERT(), DASSERTMSG(), and DASSERTEVAL() macros.
 *
 *      The alloc_text #pragma is used to place this assert code in a
 *      'preload' segment and non-discardable (with the WEP).  In this
 *      way, bad things can happen and we have a 'better' chance of 
 *      getting info displayed.  Note that the SYSMODAL assert box
 *      type is *THE* one to use under nasty memory crunch situations!
 *
 *      MB_SYSTEMMODAL | MB_ICONHAND gives you your best chance of getting
 *      the info displayed.  With this, the message box is literally
 *      painted DIRECTLY onto the screen!
 *
 ** cjp */

   #ifndef WIN32
    #pragma alloc_text( LIBMAIN, wdbAssertBox )
   #endif

BOOL FAR PASCAL wdbAssertBox( HWDB hWDB, LPSTR lpsz, LPSTR lpszFile, UINT wLine )
{
    LPWDBSESSION    lpSession;
    UINT            wType;
    UINT            wStyle;
    char            szTitle[ 80 ];

    /* if our session pointer is valid */
    if ( lpSession = hWDB ? (LPWDBSESSION)GlobalLock(hWDB) :
                            (LPWDBSESSION)&gWDBDefSession )
    {
        /* if wEnable and wAssert are on for this session */
        if ( !(lpSession->wdbConfig.wEnable &&
               (wType = lpSession->wdbConfig.wAssert)) )
            goto assertExit;
            
        /* if just beep, beep */
        if ( wType == WDB_ASSERT_BEEP )
        {
            /* beep, boop, bonk, burp, or belch */
            MessageBeep( MB_ICONHAND );
            goto assertExit;
        }

        /* format a quick message with source file name and line number */
        if ( lpsz )
            wsprintf( gachTemp, "    TEXT: %s\n    FILE: %s\n    LINE: %u",
                        lpsz, lpszFile, wLine );
        else
            wsprintf( gachTemp, "    FILE: %s\n    LINE: %u", lpszFile, wLine );


        /* (Hand) Abort, Retry, <Ignore> */
        wStyle = MB_ICONHAND | MB_DEFBUTTON3 | MB_ABORTRETRYIGNORE;

        /* if user wants system modal assert message box (uhg!) */
        if ( wType == WDB_ASSERT_SYSMODAL )
            wStyle |= MB_SYSTEMMODAL;

        /* create a message box title */
        wsprintf( szTitle, "<%s%s>",
                    lpSession->wdbConfig.szPrelude, (LPSTR)gszAssertTitle );

        /* toss up an 'assert' message box */
        switch ( MessageBox( GetActiveWindow(), gachTemp, szTitle, wStyle ) )
        {
            case IDABORT:
               #ifndef WIN32
                _asm    mov     ax, 4CFFh   ; Terminate with -1 Return Code

                DOS3Call();
               #else
               /*
                * In Win32 we call ExitProcess!
                */
               ExitProcess(1) ;
               #endif

                /** we will never get here **/


            case IDRETRY:
                _asm    int     03h         ; break to debugger if active
            break;

            case IDIGNORE:
                /* is the shift key held? if so, log this assert */
                if ( GetKeyState( VK_SHIFT ) < 0 )
                {
                    /* slap on a LF */
                    lstrcat( gachTemp, "\n" );

                    /* log it */
                    wdbAssertLog( lpSession, gachTemp );
                }
            break;
        }

assertExit:
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* always return FALSE */
    return ( FALSE );
} /* wdbAssertBox() */


/** BOOL FAR PASCAL wdbSetOutput( HWDB hWDB, UINT wOutput, LPSTR lpszFile )
 *
 *  DESCRIPTION: 
 *      Changes the output location of the session to be the location
 *      designated by wOutput, one of the WDB_OUTPUT_* codes.  If this
 *      specifies a file, lParam points to the filename.
 * 
 *      If the new output location cannot be opened/used, the previous
 *      output location is not altered and FALSE is returned.  Otherwise,
 *      the previous output location is closed (for files) and TRUE is
 *      returned.
 *
 *  ARGUMENTS:
 *      HWDB hWDB       :   Handle to WDB session to change output device.
 *
 *      UINT wOutput    :   Output code to change to.  WDB_OUTPUT_*.
 *
 *      LPSTR lpszFile  :   For WDB_OUTPUT_FILE.  Optional file name for
 *                          new output device--NULL if same file name.
 *
 *  RETURN (BOOL):
 *      The return value is TRUE if this function succeeds.  It is FALSE
 *      if the output device could not be changed.
 *
 *  NOTES:
 *      This function does NOT update the .INI file.  The reason for this
 *      is this:
 *
 *      If you want to debug a special piece of code (say LibMain() in a
 *      .DLL) that can NOT have output set to a window (because Windows
 *      is not stable enough for that), you will need to set the output
 *      device to AUX.  Then, when you are leaving LibMain(), you set the
 *      output (wdbSetOutput()) location to a window.
 *
 *      If this function changed the .INI file, you would have to RESET
 *      the .INI file BACK to AUX before you ran your program again.  This
 *      is a pain.  So, if you want WDB to _save_ the new output location
 *      you can  1. use the wdbConfigDlg() function (but NOT in LibMain!!!)
 *      or 2. wdbSetOutput() followed by wdbWriteConfigInfo().
 *
 ** cjp */

BOOL FAR PASCAL wdbSetOutput( HWDB hWDB, UINT wOutput, LPSTR lpszFile )
{
    LPWDBSESSION    lpSession;
  
    /* check for invalid output code */
    if ( wOutput >  WDB_OUTPUT_MAX )
    {
        return ( FALSE );
    }

    /* is the output location different from what the current setting is? */
    if ( wOutput == wdbGetInfo( hWDB, WDBI_WOUTPUT ) )
        return ( TRUE );

    /* default or normal session? */
    if ( hWDB )
    {
        /* lock the session handle */
        if ( !(lpSession = (LPWDBSESSION)GlobalLock( hWDB )) )
            return ( FALSE );
    }

    /* use the default session */
    else lpSession = (LPWDBSESSION)&gWDBDefSession;

    /* close the current output */
    wdbCloseOutput( lpSession );

    /* if this is for a file I/O and the user supplied a name */
    if ( (wOutput == WDB_OUTPUT_FILE) && lpszFile )
    {
        /* copy the new name over */
        lstrcpy( lpSession->wdbConfig.szOutFile, lpszFile );
    }

    /* set the new output device */
    wdbSetInfo( hWDB, WDBI_WOUTPUT, wOutput );


    /*  The following line is commented out because we want to delay the
     *  open until the _first_ write to this session.  This is done so
     *  a window is not opened if you are not using it... 3/4/91
     */

    /* now open the output location */
//    wdbOpenOutput( lpSession );


    /** should we save the new output location to the .INI file?? **/


    /* unlock if we locked */
    if ( hWDB )  GlobalUnlock( hWDB );

    /* return success */
    return ( TRUE );
} /* wdbSetOutput() */


/** UINT FAR PASCAL wdbGetOutput( HWDB hWDB, LPSTR lpszFile )
 *
 *  DESCRIPTION: 
 *      Returns the current output location for the WDB session specified.
 *      This will be one of the following:
 *      
 *          WDB_OUTPUT_OFF      0   :   output disabled          
 *          WDB_OUTPUT_WINDOW   1   :   to a window--of course   
 *          WDB_OUTPUT_AUX      2   :   output to stdaux         
 *          WDB_OUTPUT_FILE     3   :   log to a file (or append)
 *          WDB_OUTPUT_ODS      4   :   output using OutputDebugString
 *      
 *      If you supply a destination buffer for the output file name, then
 *      it will be copied into your buffer.  This is done regardless of
 *      whether the current output location is WDB_OUTPUT_FILE or not.
 *
 *  ARGUMENTS:
 *      HWDB hWDB       :   Handle to WDB session.  If NULL, returns the
 *                          default output location.
 *
 *      LPSTR lpszFile  :   Buffer to receive the output file name for the
 *                          session.  If this is NULL, no file name is 
 *                          returned.
 *
 *  RETURN (UINT):
 *      One of the WDB_OUTPUT_* values listed above.  Also, the buffer for
 *      the output file (lpszFile) will contain the output file name--
 *      regardless of the current output device.
 *
 *  NOTES:
 *      This is not very efficient...
 *
 ** cjp */

UINT FAR PASCAL wdbGetOutput( HWDB hWDB, LPSTR lpszFile )
{
    /* if there is a destination filename buffer, get it */
    if ( lpszFile )
        wdbGetOutputFile( hWDB, lpszFile );

    /* return the output location */
    return ( wdbGetInfo( hWDB, WDBI_WOUTPUT ) );
} /* wdbGetOutput() */


/** HWDB FAR PASCAL wdbOpenSession( HWND, LPSTR, LPSTR, UINT )
 *
 *  DESCRIPTION: 
 *      Establishes a session with WDB.  This includes reading the
 *      config info from the .INI file--using the defaults if none
 *      exists.  Opening log files, windows, allocating memory, etc.
 *
 *  ARGUMENTS:
 *      HWND hWndParent     :   Handle to application's window.  Can
 *                              be NULL.  If it is not, then the output
 *                              window will be a child of hWndParent.
 *
 *      LPSTR lpszAppName   :   Pointer to calling application's name.
 *
 *      LPSTR lpszPrivate   :   Pointer to private .INI file for the
 *                              calling application.  If this is NULL,
 *                              WIN.INI is used (shy away from this!!!)
 *
 *      UINT wVersion       :   WDB.DLL version requirement.  If 0, then
 *                              *any version*--dangerous.  Should be
 *                              WDB_LIBVERSION.
 *
 *  RETURN (HWDB):
 *      The return value is NULL if a session could not be established.
 *      It is a handle to a WDB session if successful.
 *
 *  NOTES:
 *
 ** cjp */

HWDB FAR PASCAL wdbOpenSession( HWND    hWndParent,
                                LPSTR   lpszAppName,
                                LPSTR   lpszPrivate,
                                UINT    wVersion )
{
    HWDB            hWDB;
    LPWDBSESSION    lpSession;

    /* sanity check on WDB library version */
    if ( !wdbLibVersion( wVersion ) )
        return ( NULL );
                        
    /* first things first--allocate a block of mem for session info */
    if ( hWDB = GlobalAlloc( WDBC_GMEM_FLAGS, sizeof(WDBSESSION) ) )
    {
        /* if this fails, something is really hosed... */
        if ( lpSession = (LPWDBSESSION)GlobalLock( hWDB ) )
        {
            /* copy user info into session block */
            lpSession->hWnd = hWndParent;

            /* create the section name */
            lstrcpy( lpSession->szSection, gszSectionPrefix );
            lstrcat( lpSession->szSection, lpszAppName );

            /* set up the private .INI file name */
            if ( lpszPrivate )
                lstrcpy( lpSession->szPrivate, lpszPrivate );
            else
                lpSession->szPrivate[ 0 ] = '\0';

            /* read in the configuration info for session */
            wdbReadConfigInfo( lpszAppName, lpSession->szSection,
                                lpszPrivate, (LPWDBCONFIG)lpSession );
            


    /*  The following line is commented out because we want to delay the
     *  open until the _first_ write to this session.  This is done so
     *  a window is not opened if you are not using it...  3/4/91
     */

            /*  Should there be a check here for output to a window??
             *  In other words, should the window be opened automatically
             *  when the session is opened or delayed until the first
             *  output??  ...wdbValidateSession() is already set up for
             *  this kind of thing, so the first 'write' will open the
             *  window, or file, or aux, or...   3/1/91
             */

            /* now initialize the output 'device' */
//            wdbOpenOutput( lpSession );

            /* unlock the session block */
            GlobalUnlock( hWDB );
        }

        /* bad RAM or somethin'--try gettin' rid o' the thing */
        else hWDB = GlobalFree( hWDB );
    }

    /* hopefully return a valid session handle */
    return ( hWDB );
} /* wdbOpenSession() */


/** HWDB FAR PASCAL wdbCloseSession( HWDB hWDB )
 *
 *  DESCRIPTION: 
 *      This function closes an active session with WDB.  This will
 *      include closing any open log files, closing windows, freeing
 *      memory, etc.
 *
 *  ARGUMENTS:
 *      HWDB hWDB   :   WDB session handle to close.
 *
 *  RETURN (HWDB):
 *      The return value is NULL if WDB was able to close the session.
 *      If something failed, the return is hWDB.
 *
 *  NOTES:
 *
 ** cjp */

HWDB FAR PASCAL wdbCloseSession( HWDB hWDB )
{
    LPWDBSESSION    lpSession;

    /* sanity check */
    if ( hWDB )
    {
        /* need to lock 'er down */
        if ( lpSession = (LPWDBSESSION)GlobalLock( hWDB ) )
        {
            /* close open window, file, or whatever... */
            wdbCloseOutput( lpSession );

            /* unlock it now */
            GlobalUnlock( hWDB );
        }
        
        /* now, free the memory used by hWDB */
        hWDB = GlobalFree( hWDB );
    }

    /* return hWDB--it should be NULL by now */
    return ( hWDB );
} /* wdbCloseSession() */


/** int FAR PASCAL wdbPuts( HWDB hWDB, LPSTR lpsz )
 *
 *  DESCRIPTION: 
 *      You should NOT use this function directly.  Instead, use the DPS()
 *      macros.  This will ensure that when compiling with DEBUG *NOT* 
 *      defined, then the code will NOT be included--therefore severing
 *      all links between your application and WDB.
 *
 *      This function prints lpsz to the device selected for output
 *      on hWDB.  As with puts(), a LF is automatically appended.
 *
 *  ARGUMENTS:
 *      HWDB hWDB   :   Handle to WDB session.
 *
 *      LPSTR lpsz  :   Pointer to string to output.
 *
 *  RETURN (int):
 *      Keeping with the tradition of puts(), this function returns 0
 *      if it fails.  A non-zero return signifies success.
 *
 *  NOTES:
 *
 ** cjp */

int FAR PASCAL wdbPuts( HWDB hWDB, LPSTR lpsz )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 1 ) )
    {
        /* create string with \n appended--inefficient as managers */
        lstrcpy( gachBigTemp, lpsz );  lstrcat( gachBigTemp, "\n" );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPuts() */

int FAR PASCAL wdbPuts2( HWDB hWDB, LPSTR lpsz )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 2 ) )
    {
        /* create string with \n appended--inefficient as managers */
        lstrcpy( gachBigTemp, lpsz );  lstrcat( gachBigTemp, "\n" );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPuts2() */

int FAR PASCAL wdbPuts3( HWDB hWDB, LPSTR lpsz )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 3 ) )
    {
        /* create string with \n appended--inefficient as managers */
        lstrcpy( gachBigTemp, lpsz );  lstrcat( gachBigTemp, "\n" );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPuts3() */

int FAR PASCAL wdbPuts4( HWDB hWDB, LPSTR lpsz )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 4 ) )
    {
        /* create string with \n appended--inefficient as managers */
        lstrcpy( gachBigTemp, lpsz );  lstrcat( gachBigTemp, "\n" );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPuts4() */

int FAR PASCAL wdbPuts5( HWDB hWDB, LPSTR lpsz )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 5 ) )
    {
        /* create string with \n appended--inefficient as managers */
        lstrcpy( gachBigTemp, lpsz );  lstrcat( gachBigTemp, "\n" );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPuts5() */


/** int FAR _cdecl wdbPrintf( HWDB hWDB, LPSTR lpszFormat, ... )
 *
 *  DESCRIPTION: 
 *      You should NOT use this function directly.  Instead, use the DPF()
 *      macros.  This will ensure that when compiling with DEBUG *NOT* 
 *      defined, then the code will NOT be included--therefore severing
 *      all links between your application and WDB.
 *
 *      This function is used to output printf() style debugging messages
 *      from an application.  The debugging messages are sent to the output
 *      device selected for hWDB.  If hWDB is NULL, then the default device
 *      set for WDB is used.  
 *
 *  ARGUMENTS:
 *      HWDB hWDB           :   Handle to WDB session.  If NULL, then the
 *                              default output device is used.
 *
 *      LPSTR lpszFormat    :   Points to the output format string.  This
 *                              string is identical to that passed to
 *                              wsprintf.
 *
 *      [ arguments, ... ]  :   Specifies optional arguments corresponding
 *                              to the formatting specification in
 *                              lpszFormat.  Note that FAR pointers must
 *                              be used for string arguments.
 *
 *  RETURN (int):
 *      Returns the number of characters printed.
 *
 *  NOTES:
 *
 ** cjp */

int FAR _cdecl wdbPrintf( HWDB hWDB, LPSTR lpszFormat, ... )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 1 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, (LPSTR)(&lpszFormat + 1) );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPrintf() */

int FAR _cdecl wdbPrintf2( HWDB hWDB, LPSTR lpszFormat, ... )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 2 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, (LPSTR)(&lpszFormat + 1) );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPrintf2() */

int FAR _cdecl wdbPrintf3( HWDB hWDB, LPSTR lpszFormat, ... )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 3 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, (LPSTR)(&lpszFormat + 1) );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPrintf3() */

int FAR _cdecl wdbPrintf4( HWDB hWDB, LPSTR lpszFormat, ... )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 4 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, (LPSTR)(&lpszFormat + 1) );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPrintf4() */

int FAR _cdecl wdbPrintf5( HWDB hWDB, LPSTR lpszFormat, ... )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 5 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, (LPSTR)(&lpszFormat + 1) );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPrintf5() */


/** int FAR _cdecl wdbPrint( HWDB hWDB, LPSTR lpszFormat, ... )
 *
 *  DESCRIPTION: 
 *      You should NOT use this function directly.  Instead, use the DP()
 *      macros.  This will ensure that when compiling with DEBUG *NOT* 
 *      defined, then the code will NOT be included--therefore severing
 *      all links between your application and WDB.
 *
 *      This function is used to output printf() style debugging messages
 *      from an application.  The debugging messages are sent to the output
 *      device selected for hWDB.  If hWDB is NULL, then the default device
 *      set for WDB is used.
 *
 *      The difference between wdbPrint() and wdbPrintf() is that a LF is
 *      automatically appended--like wdbPuts() with formatting.  Maybe it
 *      should be called wdbPutsf()?
 *
 *  ARGUMENTS:
 *      HWDB hWDB           :   Handle to WDB session.  If NULL, then the
 *                              default output device is used.
 *
 *      LPSTR lpszFormat    :   Points to the output format string.  This
 *                              string is identical to that passed to
 *                              wsprintf().
 *
 *      [ arguments, ... ]  :   Specifies optional arguments corresponding
 *                              to the formatting specification in
 *                              lpszFormat.  Note that FAR pointers must
 *                              be used for string arguments.
 *
 *  RETURN (int):
 *      Returns the number of characters printed.
 *
 *  NOTES:
 *      I thought about having another config flag that would force a LF
 *      at the end of wdbPrintf, if enabled.  But then there would be no
 *      way to suppress it without making things too complicated...  So,
 *      I compromised and made this 'print' routine...
 *
 ** cjp */

int FAR _cdecl wdbPrint( HWDB hWDB, LPSTR lpszFormat, ... )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 1 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, (LPSTR)(&lpszFormat + 1) );

        /* slap on a LF */
        lstrcat( gachBigTemp, "\n" );  iReturn++;

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPrint() */

int FAR _cdecl wdbPrint2( HWDB hWDB, LPSTR lpszFormat, ... )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 2 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, (LPSTR)(&lpszFormat + 1) );

        /* slap on a LF */
        lstrcat( gachBigTemp, "\n" );  iReturn++;

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPrint2() */

int FAR _cdecl wdbPrint3( HWDB hWDB, LPSTR lpszFormat, ... )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 3 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, (LPSTR)(&lpszFormat + 1) );

        /* slap on a LF */
        lstrcat( gachBigTemp, "\n" );  iReturn++;

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPrint3() */

int FAR _cdecl wdbPrint4( HWDB hWDB, LPSTR lpszFormat, ... )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 4 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, (LPSTR)(&lpszFormat + 1) );

        /* slap on a LF */
        lstrcat( gachBigTemp, "\n" );  iReturn++;

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPrint4() */

int FAR _cdecl wdbPrint5( HWDB hWDB, LPSTR lpszFormat, ... )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 5 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, (LPSTR)(&lpszFormat + 1) );

        /* slap on a LF */
        lstrcat( gachBigTemp, "\n" );  iReturn++;

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbPrint5() */


/** int FAR _cdecl wdbVPrintf( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs )
 *
 *  DESCRIPTION: 
 *      You should NOT use this function directly.  Instead, use the DVPF()
 *      macros.  This will ensure that when compiling with DEBUG *NOT* 
 *      defined, then the code will NOT be included--therefore severing
 *      all links between your application and WDB.
 *
 *      This function prints a string to a WDB window (or redirected output
 *      device) using printf style formatting codes.  This function is the
 *      same as the wdbPrintf function, except that arguments to the format
 *      string are placed in an array of WORDs or DWORDs.
 *
 *  ARGUMENTS:
 *      HWND hWDB       :   Handle to WDB session.  If NULL, then the default
 *                          output location is used.
 *
 *      LPSTR lpszFormat:   Points to the output string format specification.
 *                          This string uses the same formatting codes as
 *                          the Windows wsprintf() function.
 *
 *      LPSTR pArgs     :   Points to an array of words, each of which 
 *                          specifies an argument for the format string
 *                          lpszFormat.  The number, type, and interpretation
 *                          of the arguments depend on the corresponding
 *                          format control sequences in lpszFormat.
 *
 *  RETURN (int):
 *      Returns the number of characters output.  If output to the WDB
 *      output is disabled, zero is returned.  The returned count of
 *      characters output does not include the translation of newline
 *      characters into carriage return newline sequences.
 *
 *  NOTES:
 *
 ** cjp */

int FAR _cdecl wdbVPrintf( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 1 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, pArgs );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbVPrintf() */

int FAR _cdecl wdbVPrintf2( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 2 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, pArgs );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbVPrintf2() */

int FAR _cdecl wdbVPrintf3( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 3 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, pArgs );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbVPrintf3() */

int FAR _cdecl wdbVPrintf4( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 4 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, pArgs );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbVPrintf4() */

int FAR _cdecl wdbVPrintf5( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs )
{
    LPWDBSESSION    lpSession;
    int             iReturn = 0;

    /* validate output device handle */
    if ( lpSession = wdbValidateSession( hWDB, 5 ) )
    {
        /* format the expression */
        wvsprintf( gachBigTemp, lpszFormat, pArgs );

        /* write the information to the current output device */
        iReturn = wdbWriteOutput( lpSession, gachBigTemp );

        /* unlock if hWDB was locked by wdbValidateSession() */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return the outcome of the function */
    return ( iReturn );
} /* wdbVPrintf5() */


/** EOF: wdb.c **/
