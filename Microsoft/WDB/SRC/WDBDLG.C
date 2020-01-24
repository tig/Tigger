/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** wdbdlg.c
 *
 *  DESCRIPTION: 
 *      This file contains all of the 'configuration dialog box' code for
 *      WDB--and the default menu handling code.
 *
 *  HISTORY:
 *      3/10/91     cjp     wrote it
 *
 ** cjp */


/* grab the includes we need */
    #include <windows.h>
    #include "wdb.h"
    #include "wdbi.h"


/** int FAR PASCAL wdbFontEnumCallback( LPLOGFONT, LPTEXTMETRIC, UINT, LONG )
 *
 *  DESCRIPTION: 
 *      Fills a combo box with requested information
 *
 *  ARGUMENTS:
 *      ( LPLOGFONT lplf, LPTEXTMETRIC lptm, UINT nft, LONG lpData )
 *
 *  RETURN (int FAR PASCAL):
 *
 *
 *  NOTES:
 *      Previously there was only one wdbFontEnumCallBack functions and
 *      cjp was using the HI and LO words of lData for hCombo and fType
 *      respectively.  This does not bode well in a 32 bit world, so
 *      I broke it into two functions.  One for enumerating faces and the
 *      other for enumerating sizes.
 *
 ** cjp */

int FAR PASCAL wdbFontEnumCallback( LPLOGFONT lplf, LPTEXTMETRIC lptm, UINT nft, LONG lData )
{
    HWND hCombo ;

   #ifndef WIN32
    hCombo = (HWND)LOWORD( lData ) ;
   #else
    UNREFERENCED_PARAMETER(lptm) ;
    UNREFERENCED_PARAMETER(nft) ;
    hCombo = (HWND)lData ;
   #endif
   
    SendMessage( hCombo, CB_ADDSTRING, 0, (LONG)(LPSTR)lplf->lfFaceName );

    /* party on... */
    return ( 1 );
} /* wdbFontEnumCallback() */


/** int FAR PASCAL wdbSizeEnumCallback( LPLOGFONT, LPTEXTMETRIC, UINT, LONG )
 *
 *  DESCRIPTION: 
 *      Fills a combo box with requested information
 *
 *  ARGUMENTS:
 *      ( LPLOGFONT lplf, LPTEXTMETRIC lptm, UINT nft, LONG lpData )
 *
 *  RETURN (int FAR PASCAL):
 *
 *
 *  NOTES:
 *      Previously there was only one wdbFontEnumCallBack functions and
 *      cjp was using the HI and LO words of lData for hCombo and fType
 *      respectively.  This does not bode well in a 32 bit world, so
 *      I broke it into two functions.  One for enumerating faces and the
 *      other for enumerating sizes.
 *
 ** cjp */

int FAR PASCAL wdbSizeEnumCallback( LPLOGFONT lplf, LPTEXTMETRIC lptm, UINT nft, LONG lData )
{
    HWND    hCombo ;
    char    tmp[ 10 ];
    UINT    wPoints;
    UINT    wPixels ;
    HDC     hDC ;

   #ifndef WIN32
    hCombo = (HWND)LOWORD( lData ) ;
   #else
    hCombo = (HWND)lData ;
    UNREFERENCED_PARAMETER(lplf) ;
    UNREFERENCED_PARAMETER(nft) ;
   #endif

    wPixels = lptm->tmHeight - lptm->tmInternalLeading;

    /* enumerate point size? if not, then face name... */

    hDC = GetDC( NULL ) ;
    wPoints = MulDiv( wPixels, 72, GetDeviceCaps( hDC, LOGPIXELSY ) );
    ReleaseDC( NULL, hDC ) ;

    wsprintf( tmp, "%d", wPoints );

    /* don't put in stupid duplicates */
    if ( SendMessage( hCombo, CB_FINDSTRING, 0, (LONG)(LPSTR)tmp ) == CB_ERR )
            SendMessage( hCombo, CB_ADDSTRING, 0, (LONG)(LPSTR)tmp );

    /* party on... */
    return ( 1 );
} /* wdbSizeEnumCallback() */


/** BOOL FAR PASCAL wdbConfigDlgProc( HWND, UINT, UINT, LONG )
 *
 *  DESCRIPTION: 
 *      This is the message processor for the configuration dialog box.
 *
 *  NOTES:
 *      This really could use cleaning up.
 *
 ** cjp */

BOOL FAR PASCAL wdbConfigDlgProc( HWND hDlg, UINT wMsg, UINT wParam, LONG lParam )
{
    HWDB    hWDB;

    switch ( wMsg )
    {
        case WM_INITDIALOG:
        {
            HWND    hWnd;
            HDC     hDC;
            HWND    hCombo;
            HWND    hPoint;
            UINT    wSel;

            /* attach the hWDB to the hDlg window */
            if ( !SetProp( hDlg, gszWDB, hWDB = (HWDB)LOWORD(lParam) ) )
                return ( FALSE );

            /* prime a few things */
            hDC    = GetDC( hDlg );
            hCombo = GetDlgItem( hDlg, IDD_WDB_FONTNAME );
            hPoint = GetDlgItem( hDlg, IDD_WDB_FONTSIZE );

            EnumFonts( hDC, NULL, (FARPROC)wdbFontEnumCallback,
                        (LPSTR)hCombo ); 

            wdbGetFontName( hWDB, gachTemp );
            OD( "WDB: FONT NAME: [" );  OD( gachTemp );  OD( "]\r\n" );

            wSel = (UINT)SendMessage( hCombo, CB_FINDSTRING, (WPARAM)-1, (LONG)(LPSTR)gachTemp );
            if ( wSel == CB_ERR )
            {
                OD( "WDB: CB_ERR on CB_FINDSTRING for Face Name!!!\r\n" );
                wSel = 0;
            }

            PostMessage( hCombo, CB_SETCURSEL, wSel, 0L );

            SendMessage( hPoint, CB_RESETCONTENT, 0, 0L );
            SendMessage( hCombo, CB_GETLBTEXT, wSel,
                            (LONG)(LPSTR)gachTemp );
            EnumFonts( hDC, (LPSTR)gachTemp, (FARPROC)wdbSizeEnumCallback,
                            (LPSTR) hPoint ); 

            ReleaseDC( hDlg, hDC );

            wParam = wdbGetInfo( hWDB, WDBI_WFONTSIZE );
            wsprintf( gachTemp, "%u", wParam );
            SetDlgItemText( hDlg, IDD_WDB_FONTSIZE, gachTemp );


            hWnd = GetDlgItem( hDlg, IDD_WDB_ASSERT );
            SendMessage( hWnd, CB_ADDSTRING, 0, (LONG)(LPSTR)gszArgOff );
            SendMessage( hWnd, CB_ADDSTRING, 0, (LONG)(LPSTR)gszArgWindow );
            SendMessage( hWnd, CB_ADDSTRING, 0, (LONG)(LPSTR)gszArgSysModal );
            SendMessage( hWnd, CB_ADDSTRING, 0, (LONG)(LPSTR)gszArgBeep );
            SendMessage( hWnd, CB_SETCURSEL, wdbGetInfo( hWDB, WDBI_WASSERT ), 0 );

            hWnd = GetDlgItem( hDlg, IDD_WDB_OUTPUT );
            SendMessage( hWnd, CB_ADDSTRING, 0, (LONG)(LPSTR)gszArgOff );
            SendMessage( hWnd, CB_ADDSTRING, 0, (LONG)(LPSTR)gszArgWindow );
            SendMessage( hWnd, CB_ADDSTRING, 0, (LONG)(LPSTR)gszArgAux );
            SendMessage( hWnd, CB_ADDSTRING, 0, (LONG)(LPSTR)gszArgFile );
            SendMessage( hWnd, CB_ADDSTRING, 0, (LONG)(LPSTR)gszArgOds );
            SendMessage( hWnd, CB_ADDSTRING, 0, (LONG)(LPSTR)gszArgMono );
            SendMessage( hWnd, CB_SETCURSEL, wdbGetInfo( hWDB, WDBI_WOUTPUT ), 0 );

            CheckDlgButton( hDlg, IDD_WDB_ENABLE,
                                wdbGetInfo( hWDB, WDBI_WENABLE ) );

            CheckDlgButton( hDlg, IDD_WDB_LFTOCRLF,
                                wdbGetInfo( hWDB, WDBI_WLFTOCRLF ) );

            CheckDlgButton( hDlg, IDD_WDB_FONTBOLD,
                                wdbGetInfo( hWDB, WDBI_WFONTBOLD ) );

            SendDlgItemMessage( hDlg, IDD_WDB_FILENAME,
                                EM_LIMITTEXT, WDBC_MAX_FILELEN - 1, 0L );
            wdbGetOutputFile( hWDB, gachTemp );
            SetDlgItemText( hDlg, IDD_WDB_FILENAME, gachTemp );

            SendDlgItemMessage( hDlg, IDD_WDB_PRELUDE,
                                EM_LIMITTEXT, WDBC_MAX_PRELUDE - 1, 0L );
            wdbGetOutputPrelude( hWDB, gachTemp );
            SetDlgItemText( hDlg, IDD_WDB_PRELUDE, gachTemp );

            hWnd = GetDlgItem( hDlg, IDD_WDB_LEVEL );
            for ( wParam = 0; wParam <= WDB_LEVEL_MAX; wParam++ )
            {
                wsprintf( gachTemp, "%d", wParam );
                SendMessage( hWnd, CB_ADDSTRING, 0, (LONG)(LPSTR)gachTemp );
            }
            SendMessage( hWnd, CB_SETCURSEL, wdbGetInfo( hWDB, WDBI_WLEVEL ), 0 );

            wParam = wdbGetInfo( hWDB, WDBI_WMAXLINES );
            wsprintf( gachTemp, "%u", wParam );
            SetDlgItemText( hDlg, IDD_WDB_MAXLINES, gachTemp );

            /* we're ok! */
            return ( TRUE );
        }

        case WM_COMMAND:
        {
            UINT wNotifyCode ;

            #ifndef WIN32
            wNotifyCode = HIWORD( lParam ) ;
            #else
            wNotifyCode = HIWORD( wParam ) ;
            #endif

            switch ( LOWORD( wParam ) )
            {
                case IDD_WDB_FONTNAME:
                {
                    if ( wNotifyCode == CBN_SELCHANGE )
                    {
                        HWND    hPoint = GetDlgItem( hDlg, IDD_WDB_FONTSIZE );
                        HDC     hDC    = GetDC( hDlg );
                        UINT    wSel;   
                                         
                        SendMessage( hPoint, CB_RESETCONTENT, 0, 0L );
                        wSel = (UINT)SendMessage( (HWND)LOWORD(lParam), CB_GETCURSEL, 0, 0L ); 
                        if ( wSel != CB_ERR )
                        {
                            SendMessage( (HWND)LOWORD(lParam), CB_GETLBTEXT, wSel,
                                            (LONG)(LPSTR)gachTemp );
                            EnumFonts( hDC, (LPSTR)gachTemp,
                                            (FARPROC)wdbSizeEnumCallback,
                                            (LPSTR)hPoint ); 
                        }

                        ReleaseDC( hDlg, hDC );
                        PostMessage( hPoint, CB_SETCURSEL, 0, 0L ); 
                    }
                }
                break;

                case IDOK:
                {
                    LPWDBSESSION    lpSession;
                    LPWDBCONFIG     lpConfig;
                    LPSTR           lpszPrivate;
                    short           i;
                    HWND            hWnd;

                    OD( "WDB: WDB_DLG_ABLE OK\r\n" );

                    if ( hWDB = GetProp( hDlg, gszWDB ) )
                        lpSession = (LPWDBSESSION)GlobalLock( hWDB );
                    else
                        lpSession = (LPWDBSESSION)&gWDBDefSession;

                    if ( !lpSession )
                        goto idCancel;

                    lpConfig = &lpSession->wdbConfig;

                    lpConfig->wEnable = IsDlgButtonChecked( hDlg, IDD_WDB_ENABLE );
                    lpConfig->wLFtoCRLF = IsDlgButtonChecked( hDlg, IDD_WDB_LFTOCRLF );

                    hWnd = GetDlgItem( hDlg, IDD_WDB_ASSERT );
                    wdbSetInfo( hWDB, WDBI_WASSERT,
                            (UINT)SendMessage( hWnd, CB_GETCURSEL, 0, 0 ) );

                    hWnd = GetDlgItem( hDlg, IDD_WDB_LEVEL );
                    lpConfig->wLevel = (UINT)SendMessage( hWnd, CB_GETCURSEL, 0, 0 );

                    GetDlgItemText( hDlg, IDD_WDB_PRELUDE,
                                        gachTemp, WDBC_MAX_PRELUDE );
                    lstrcpy( lpConfig->szPrelude, gachTemp );

                    /* set the output device */
                    GetDlgItemText( hDlg, IDD_WDB_FILENAME,
                                        gachTemp, WDBC_MAX_FILELEN );
                    lstrcpy( lpConfig->szOutFile, gachTemp );
                    hWnd = GetDlgItem( hDlg, IDD_WDB_OUTPUT );
                    wdbSetOutput( hWDB, (UINT)SendMessage( hWnd, CB_GETCURSEL, 0, 0 ), gachTemp );


                    /* set our 'window change' flag to false */
                    i = 0;

                    wParam = GetDlgItemInt( hDlg, IDD_WDB_FONTSIZE, NULL, FALSE );

                    wsprintf( gachTemp, "WDB: FontSize: %d\r\n", wParam );
                    OD( gachTemp );

                    if ( (wParam <= 0) || (wParam > WDBC_MAX_FONTSIZE) )
                        wParam = WDBC_MAX_FONTSIZE;
                    if ( lpConfig->wFontSize != wParam )
                        lpConfig->wFontSize = wParam, i++;

                    wParam = IsDlgButtonChecked( hDlg, IDD_WDB_FONTBOLD );
                    if ( wParam != lpConfig->wFontBold )
                        lpConfig->wFontBold = wParam, i++;

                    hWnd = GetDlgItem( hDlg, IDD_WDB_FONTNAME );
                    wParam = (UINT)SendMessage( hWnd, CB_GETCURSEL, 0, 0L );
                    SendMessage( hWnd, CB_GETLBTEXT, wParam, (LONG)(LPSTR)gachTemp );
                    if ( lstrcmpi( lpConfig->szFontName, gachTemp ) )
                        lstrcpy( lpConfig->szFontName, gachTemp ), i++;

                    wParam = GetDlgItemInt( hDlg, IDD_WDB_MAXLINES, NULL, FALSE );
                    if ( wParam > WDBC_MAX_MAXLINES )
                        wParam = WDBC_MAX_MAXLINES;
                    if ( lpConfig->wMaxLines != wParam )
                        lpConfig->wMaxLines = wParam, i = -1;

                    if ( (lpConfig->wOutput == WDB_OUTPUT_WINDOW) && i )
                    {
                        /* if maxlines changed, reopen the window */
                        if ( i < 0 )
                        {
                            wdbCloseWindow( lpSession );
                        }

                        else wdbChangeFont( lpSession, lpSession->hOutput );
                    }

                    /* using a private .INI file? */
                    lpszPrivate = lpSession->szPrivate[ 0 ] ?
                                        lpSession->szPrivate : NULL;

                    /* write the configuration info for session */
                    wdbWriteConfigInfo( lpSession->szSection, lpszPrivate,
                                        (LPWDBCONFIG)lpSession );

                    if ( hWDB && lpSession )  GlobalUnlock( hWDB );

                    /** fall through **/
                }

                case IDCANCEL:
idCancel:
                    if ( RemoveProp( hDlg, gszWDB ) )
                        OD( "WDB: Prop Removed!\r\n" );

                    EndDialog( hDlg, TRUE );
                    return ( TRUE );
            }
        }
        break;
    }

    /* let the dialog mangler handle it */
    return ( FALSE );
} /* wdbConfigDlgProc() */


/** BOOL FAR PASCAL wdbConfigDlg( HWDB hWDB )
 *
 *  DESCRIPTION: 
 *      This function will display a 'configuration dialog box' for
 *      the WDB session specified.  If you choose 'Ok,' then the new
 *      configuration becomes active immediately.  It is also saved to
 *      the .INI file.
 *
 *  ARGUMENTS:
 *      HWDB hWDB   :   Session handle to modify ables for.
 *
 *  RETURN (BOOL FAR PASCAL):
 *      TRUE if successful.  FALSE otherwise.
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbConfigDlg( HWDB hWDB )
{
    HWND    hWnd ;

    hWnd = (HWND)wdbGetInfo( hWDB, WDBI_HWND );

    if ( !hWnd )
        hWnd = GetActiveWindow();

    /* pop up the ables dialog box */
    DialogBoxParam( ghModule, DLG_WDB_CONFIG, hWnd,
                        wdbConfigDlgProc, (LONG)hWDB );

    /* return success */
    return ( TRUE );
} /* wdbConfigDlg() */


/** EOF: wdbdlg.c **/
