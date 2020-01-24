//*************************************************************
//  File name: pubsave.c
//
//  Description: 
//      Handles the saving and bitmaps for .PUB files
//
//  History:    Date       Author     Comment
//               3/ 7/92   MSM        Created
//
//*************************************************************

#include "global.h"

char szPSBuff[120];

//*************************************************************
//
//  DBSaveStuffToMakeDLLs
//
//  Purpose:
//      Saves the .BMP, .RC, .DEF and makeall.bat files
//
//
//  Parameters:
//      LPDB lpdb
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 7/92   MSM        Created
//
//*************************************************************

BOOL WINAPI DBSaveStuffToMakeDLLs( LPDB lpDB )
{
    LPPUB lpPub = GetFirstPublisher( lpDB, NULL );
    char  szPath[255];
    LPSTR lpPath = (LPSTR)szPath;
    LPSTR lpPubName;
    int i = 0;

    //*** Setup path
    lstrcpy( lpPath, lpDB->szPathName );
    lpPubName = lpPath + lstrlen( lpPath );

    while (lpPub)
    {
        LPFAMILY lpFam = GetFirstFamily( lpPub, NULL );

        //*** add publisher ID to DB path
        wsprintf( lpPubName, "FSPUB%03X\\", lpPub->fiFSPub&0x0FFF );

        while (lpFam)
        {
            LPFACE lpFace = GetFirstFace( lpFam, NULL );
            UINT   uFace = 1;
            
            while (lpFace)
            {
                if (!DBSaveBMP( lpFace, uFace, lpPath ))
	        	    return FALSE;

    		    i += lpFace->Info.wStyles;

        		if( i > 100 )
        		{
		            GlobalCompact((DWORD)-1L);
        		    i = 0;
		        }

                lpFace = GetNextFace( lpFace, NULL );
                uFace++;
            }

            if (!DBSaveRC( lpFam, lpPath ))
                return FALSE;

            if (!DBSaveDEF( lpFam, lpPath ))
                return FALSE;

            lpFam = GetNextFamily( lpFam, NULL );
        }

        if (!DBSaveMAKEALL( lpPub, lpPath ))
            return FALSE;

        lpPub = GetNextPublisher( lpPub, NULL );
    }
    return TRUE;

} //*** DBSaveStuffToMakeDLLs


//*************************************************************
//
//  DBSaveBMP
//
//  Purpose:
//      Saves the BMP file
//
//
//  Parameters:
//      LPFACE lpFace
//      UINT   uFace
//      LPSTR  lpPath
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 7/92   MSM        Created
//
//*************************************************************

BOOL WINAPI DBSaveBMP( LPFACE lpFace, UINT uFace, LPSTR lpPath  )
{
    LPFAMILY lpFam = PARENT( LPFAMILY, lpFace );
    LPSTR    lp = lpPath + lstrlen( lpPath );
    HBITMAP  hBmp;
    BOOL     fSave = TRUE;

    wsprintf( lp, "FS%03X%03X.BMP", lpFam->fiFF&0x0FFF, uFace&0x0FFF );

    wsprintf( szPSBuff, "Saving %s for: %s", lpPath, NODENAME( lpFace ) );
    SetWindowText( ghwndStat, szPSBuff );
    UpdateWindow( ghwndStat );

    //*** Save old bitmap
    hBmp = lpFace->hBitmap;

#ifdef MAKE_RLE8
    //*** Make an RLE8 bitmap for saving
    lpFace->hBitmap = DBMakeThumb( lpFace, MT_SAVERLE8 );
#else
    //*** Make an monochrome bitmap for saving
    lpFace->hBitmap = DBMakeThumb( lpFace, MT_DISPLAY );
#endif

    SetCursor( LoadCursor(NULL,IDC_WAIT) );

    if (!lpFace->hBitmap)
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
            IDS_ERRCREATEBMP, NODENAME(lpFace) );
        fSave = FALSE;
    }
    else
    {
        //*** Write BMP to disk
        fSave = SaveBitmapFile( lpPath, lpFace->hBitmap );

        if (!fSave)
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                IDS_ERRSAVEBMP, lpPath );
        }

        //*** Delete RLE8 bitmap
        DeleteObject( lpFace->hBitmap );
    }

    //*** Strip of file name
    *lp = 0;

    //*** Restore old bitmap
    lpFace->hBitmap = hBmp;

    return fSave;

} //*** DBSaveBMP


//*************************************************************
//
//  DBSaveRC
//
//  Purpose:
//      Saves the RC file
//
//
//  Parameters:
//      LPFAMILY lpFamily
//      LPSTR lpPath
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 7/92   MSM        Created
//
//*************************************************************

BOOL WINAPI DBSaveRC( LPFAMILY lpFam, LPSTR lpPath  )
{
    LPSTR    lp = lpPath + lstrlen( lpPath );
    OFSTRUCT of;
    HFILE    hFile;
    BOOL     fSave = TRUE;

    wsprintf( lp, "FS%03X.RC", lpFam->fiFF&0x0FFF );

    wsprintf( szPSBuff, "Saving %s for: %s", lpPath, NODENAME( lpFam ) );
    SetWindowText( ghwndStat, szPSBuff );
    UpdateWindow( ghwndStat );

    if ((hFile = OpenFile( lpPath, &of, OF_CREATE|OF_WRITE ))==-1)
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
            IDS_OPENFAILED, lpPath );
        fSave = FALSE;
    }
    else
    {
        int len, n = 1;
        

        for (n=1; n<=(int)FACES(lpFam); n++)
        {
            len = wsprintf(szPSBuff,"%d   BITMAP  FS%03X%03X.BMP\r\n",
                        n,lpFam->fiFF&0x0FFF,n);

            if ( _lwrite( hFile, szPSBuff, len)!=(UINT)len )
            {
                ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                    IDS_WRITEFAILED, lpPath );
                fSave = FALSE;
                break;
            }
        }
        _lclose( hFile );

    }
    *lp = 0;
    return fSave;

} //*** DBSaveRC


//*************************************************************
//
//  DBSaveDEF
//
//  Purpose:
//      Saves the DEF file
//
//
//  Parameters:
//      LPFAMILY lpFamily
//      LPSTR lpPath
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 7/92   MSM        Created
//
//*************************************************************

BOOL WINAPI DBSaveDEF( LPFAMILY lpFam, LPSTR lpPath  )
{
    LPSTR    lp = lpPath + lstrlen( lpPath );
    LPPUB    lpPub = PARENT( LPPUB, lpFam );
    OFSTRUCT of;
    HFILE    hFile;
    BOOL     fSave = TRUE;

    wsprintf( lp, "FS%03X.DEF", lpFam->fiFF&0x0FFF );

    wsprintf( szPSBuff, "Saving %s for: %s", lpPath, NODENAME( lpFam ) );
    SetWindowText( ghwndStat, szPSBuff );
    UpdateWindow( ghwndStat );


    if ((hFile = OpenFile( lpPath, &of, OF_CREATE|OF_WRITE ))==-1)
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
            IDS_OPENFAILED, lpPath );
    }
    else
    {
        int len;
        
        len = wsprintf(szPSBuff,GRCS(IDS_DEF_LIBRARY), 
                    lpPub->fiFSPub&0x0FFF, lpFam->fiFF&0x0FFF);

        if ( _lwrite( hFile, szPSBuff, len)!=(UINT)len )
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                IDS_WRITEFAILED, lpPath );
            fSave = FALSE;
        }
        else
        {
            len = wsprintf(szPSBuff,GRCS(IDS_DEF_DESC) ); 
            if ( _lwrite( hFile, szPSBuff, len)!=(UINT)len )
            {
                ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                    IDS_WRITEFAILED, lpPath );
                fSave = FALSE;
            }
            else
            {
                len = wsprintf(szPSBuff,GRCS(IDS_DEF_EXECODEDATA) ); 
                if ( _lwrite( hFile, szPSBuff, len)!=(UINT)len )
                {
                    ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                        IDS_WRITEFAILED, lpPath );
                    fSave = FALSE;
                }
            }
        }
        _lclose( hFile );

    }
    *lp = 0;
    return fSave;

} //*** DBSaveDEF


//*************************************************************
//
//  DBSaveMAKEALL
//
//  Purpose:
//      Saves the BMP file
//
//
//  Parameters:
//      LPPUB lpPub
//      LPSTR lpPath
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 7/92   MSM        Created
//
//*************************************************************

BOOL WINAPI DBSaveMAKEALL( LPPUB lpPub, LPSTR lpPath )
{
    LPSTR    lpRC, lp = lpPath + lstrlen( lpPath );
    OFSTRUCT of;
    HFILE    hFile;
    BOOL     fSave = TRUE;

    wsprintf( lp, "MAKEALL.BAT" );

    wsprintf( szPSBuff, "Saving %s for: %s", lpPath, lpPub->lpszPublisher );
    SetWindowText( ghwndStat, szPSBuff );
    UpdateWindow( ghwndStat );

    if ((hFile = OpenFile( lpPath, &of, OF_CREATE|OF_WRITE ))==-1)
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
            IDS_OPENFAILED, lpPath );
    }
    else
    {
        int  len;        
        WORD wPID, wFID;

        wPID = lpPub->fiFSPub&0x0FFF;

        lpRC = GRCS( IDS_MAKEALL_ASM );
        len = lstrlen( lpRC );

        if ( _lwrite( hFile, lpRC, len)!=(UINT)len )
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                IDS_WRITEFAILED, lpPath );
            fSave = FALSE;
            goto save_err;
        }

        for (wFID=1; wFID<=lpPub->wFamilies; wFID++)
        {
            len = wsprintf( szPSBuff, GRCS( IDS_MAKEALL_RCR ), wFID );
            if ( _lwrite( hFile, szPSBuff, len)!=(UINT)len )
            {
                ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                    IDS_WRITEFAILED, lpPath );
                fSave = FALSE;
                goto save_err;
            }

            len = wsprintf( szPSBuff, GRCS( IDS_MAKEALL_LINK ),wPID,wFID,wFID);
            if ( _lwrite( hFile, szPSBuff, len)!=(UINT)len )
            {
                ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                    IDS_WRITEFAILED, lpPath );
                fSave = FALSE;
                goto save_err;
            }

            len = wsprintf( szPSBuff, GRCS( IDS_MAKEALL_RCV ),wFID,wPID,wFID);
            if ( _lwrite( hFile, szPSBuff, len)!=(UINT)len )
            {
                ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                    IDS_WRITEFAILED, lpPath );
                fSave = FALSE;
                goto save_err;
            }
        }

        lpRC = GRCS( IDS_MAKEALL_DONE );
        len = lstrlen( lpRC );

        if ( _lwrite( hFile, lpRC, len)!=(UINT)len )
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                IDS_WRITEFAILED, lpPath );
            fSave = FALSE;
        }

save_err:
        _lclose( hFile );
    }

    if (fSave)
        WinExec( lpPath, SW_SHOWMINIMIZED );

    *lp = 0;
    return fSave;

} //*** DBSaveMAKEALL


//*************************************************************
//
//  DBGetFont
//
//  Purpose:
//      Attempts to create a font that matches the style
//
//
//  Parameters:
//      LPSTYLE lpS
//      UINT uPointSize
//
//  Return: (HFONT WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 5/92   MSM        Created
//
//*************************************************************

HFONT WINAPI DBGetFont( LPSTYLE lpS, UINT uPointSize )
{
    LPSTR lpFace, lpStyle;
    HFONT hFont;

    lpStyle = NODENAME( lpS );
    lpFace  = NODENAME( PARENT(LPFACE,lpS) );

    hFont = ReallyCreateFontEx(NULL,lpFace,lpStyle,uPointSize,RCF_NODEFAULT);

    //*** If we are not creating a thumb, just return
    if (uPointSize!=THUMB_POINTSIZE)
        return hFont;

    //*** If we are, then we need to make sure height < THUMB_CY
    while (hFont)
    {
        HDC hDC = GetDC( NULL );
        TEXTMETRIC tm;

        hFont = SelectObject( hDC, hFont );
        GetTextMetrics( hDC, &tm );
        hFont = SelectObject( hDC, hFont );
        ReleaseDC( NULL, hDC );

        if (tm.tmHeight <= THUMB_CY)
            break;

        //*** Try again
        DeleteObject( hFont );

        //*** knock it down 2 point sizes
        uPointSize -= 2;
        hFont = ReallyCreateFontEx(NULL,lpFace,lpStyle,uPointSize,RCF_NODEFAULT);
    }
    return hFont;

} //*** DBGetFont

//*** EOF: pubsave.c
