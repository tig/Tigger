//*************************************************************
//  File name: pubinfo.c
//
//  Description: 
//      Handles reading the .PUB files
//
//  History:    Date       Author     Comment
//               2/29/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"

#include "intFCDB.h"

#define MB_ERR  (MB_ICONSTOP|MB_OK)
#define MB_WARN (MB_ICONINFORMATION|MB_OK)
#define READ_INI()  GetPrivateProfileString(szSec, szKey, "", lpBuff, 255, lpFilename )

//*** Local buffers
static char szSec[80];
static char szKey[32];
static char szBuffer[255];

BOOL NEAR PASCAL LoadPubFamilies( LPPUB lpPub, LPSTR lpFilename );
BOOL NEAR PASCAL LoadPubFamily( LPFAMILY lpFam, LPSTR lpFilename );
BOOL NEAR PASCAL LoadPubFace( LPFACE lpFace, LPSTR lpFilename );

//*************************************************************
//
//  LoadPubInfo
//
//  Purpose:
//      Loads in a .PUB info file
//
//
//  Parameters:
//      LPDB lpDB
//      LPSTR lpFilename
//      
//
//  Return: (LPPUB FAR PASCAL)
//
//
//  Comments:
//
//  History:    Date       Author     Comment
//               2/29/92   MSM        Created
//
//*************************************************************

LPPUB FAR PASCAL LoadPubInfo( LPDB lpDB, LPSTR lpFilename )
{
    LPSTR lpBuff = (LPSTR)szBuffer;
    LPPUB lpPub ;
    HWND  hMyWnd = GetActiveWindow();
    WORD  wFamily = 1;

    // Store file path in DB Header.
    // In the end another directory will be stripped off.
    lstrcpy( lpDB->szPathName, lpFilename );
    {
        LPSTR lp = lpDB->szPathName;

        while (*lp != ':' && *lp != '\\' && lp>lpDB->szPathName)
            lp--;
        if (lp>lpDB->szPathName)
            *(++lp) = '\0';
    }

    //*** Allocate a publisher node
    if (!(lpPub = CreatePublisher( lpDB )  ))
    {
        ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_MEMALLOC );
        gwLoadError = IDS_MEMALLOC;
        return NULL ;
    }

    //*** Read a publisher name
    lstrcpy( szSec, GetRCString( IDS_PUBLISHER, hInstLib ) );
    lstrcpy( szKey, GetRCString( IDS_NAME, hInstLib ) );

    if (!READ_INI())
    {
        ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        FreePublisher( lpPub ) ;
        gwLoadError = IDS_ININOTFOUND;
        return NULL ;
    }

    if (!(lpPub->lpszPublisher = ALLOC_STRING( lpPub, lpBuff )))
    {
        ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_MEMALLOC );
        FreePublisher( lpPub ) ;
        gwLoadError = IDS_MEMALLOC;
        return NULL ;
    }

    //*** Read a publisher short copyright
    lstrcpy( szKey, GetRCString( IDS_SHORTCOPYRIGHT, hInstLib ) );

    if (!READ_INI())
    {
        ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        FreePublisher( lpPub ) ;
        gwLoadError = IDS_ININOTFOUND;
        return NULL ;
    }

    if (!(lpPub->lpszSCopyright = ALLOC_STRING( lpPub, lpBuff )))
    {
        ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_MEMALLOC );
        FreePublisher( lpPub ) ;
        gwLoadError = IDS_MEMALLOC;
        return NULL ;
    }
    lpPub->PubInfo.wSCopyrightSize = lstrlen(lpBuff);

    //*** Read a publisher long copyright
    lstrcpy( szKey, GetRCString( IDS_LONGCOPYRIGHT, hInstLib ) );

    if (READ_INI())
    {
        OFSTRUCT of ;
        HFILE    hFile ;
        LPSTR    lp = (LPSTR)of.szPathName;

        wsprintf( lp, "%s%s", (LPSTR)lpDB->szPathName, lpBuff );

        if (hFile = OpenFile( lp, &of, OF_READ | OF_SHARE_DENY_NONE ))
        {
            UINT cb = (UINT)filelength( hFile );

            if (lpPub->lpszLCopyright = (LPSTR)ALLOCFROMHEAP(lpPub,cb+1))
            {
                if(_lread( hFile, lpPub->lpszLCopyright, cb )!=cb)
                {
                    ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, 
                        IDS_FILEREADFAILED, lp );
                    FreePublisher( lpPub ) ;
                    gwLoadError = IDS_FILEREADFAILED;
                    return NULL ;
                }
                lpPub->lpszLCopyright[cb] = '\0' ;
                lpPub->PubInfo.wLCopyrightSize = cb + 1 ;
            }
            else
            {
                ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_MEMALLOC );
                FreePublisher( lpPub ) ;
                gwLoadError = IDS_MEMALLOC;
                return NULL ;
            }
            _lclose( hFile ) ;
        }
        else
        {
            ErrorResBox( hMyWnd, hInstLib, MB_WARN, IDS_FS, IDS_FILENOTFOUND2,
                (LPSTR)szSec, (LPSTR)szKey, lp );
            lpPub->PubInfo.wLCopyrightSize = 0 ;
        }
    }

    if (LoadPubFamilies( lpPub, lpFilename ))
    {
        LPSTR lp = lpDB->szPathName;

        // Knock off another directory. Go up a level
        while (*lp != ':' && *lp != '\\' && lp>lpDB->szPathName)
            lp--;
        if (lp>lpDB->szPathName)
            *(++lp) = '\0';

        return lpPub;
    }
    return NULL;

} //*** LoadPubInfo


//*************************************************************
//
//  LoadPubFamilies
//
//  Purpose:
//      Loads in all the publisher familes
//
//
//  Parameters:
//      LPPUB lpPub
//      LPSTR lpFilename
//      
//
//  Return: (BOOL NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

BOOL NEAR PASCAL LoadPubFamilies( LPPUB lpPub, LPSTR lpFilename )
{
    LPSTR lpBuff = (LPSTR)szBuffer;
    HWND  hMyWnd = GetActiveWindow();
    WORD  wFamily = 1;

    while (wFamily)
    {
        LPFAMILY lpFam;

        //*** Read a family name
        lstrcpy( szSec, GetRCString( IDS_PUBLISHER, hInstLib ) );
        wsprintf( szKey, GetRCString( IDS_FAMILY_F, hInstLib ), wFamily );

        if (!READ_INI())
            break;
        
        if ( !(lpFam = CreateFamily( lpPub )) )
        {
            ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_MEMALLOC );
            FreePublisher( lpPub ) ;
            gwLoadError = IDS_MEMALLOC;
            return FALSE;
        }
        _fstrncpy( NODENAME(lpFam), lpBuff, LF_FACESIZE );
        NODENAME(lpFam)[LF_FACESIZE] = 0;

        if (!LoadPubFamily( lpFam, lpFilename ))
            return FALSE;
        wFamily++;
    }

    if (wFamily==1)     // No families found
    {
        ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_NOFAMILIESFOUND );
        FreePublisher( lpPub ) ;
        gwLoadError = IDS_NOFAMILIESFOUND;
        return FALSE;
    }
    return TRUE;

} //*** LoadPubFamilies


//*************************************************************
//
//  LoadPubFamily
//
//  Purpose:
//      Loads in the family info from the INI file
//
//
//  Parameters:
//      LPFAMLIY lpFam
//      LPSTR lpFilename
//      
//
//  Return: (BOOL NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

BOOL NEAR PASCAL LoadPubFamily( LPFAMILY lpFam, LPSTR lpFilename )
{
    LPSTR lpBuff = (LPSTR)szBuffer;
    HWND  hMyWnd = GetActiveWindow();
    WORD  wFace = 1;
    int   nConv;
    LPSTR lp;

    lstrcpy( szSec, NODENAME(lpFam) );
    wsprintf( szKey, GetRCString( IDS_PRICE, hInstLib ) );

    if (!READ_INI())
    {
        ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        gwLoadError = IDS_ININOTFOUND;
        return FALSE;
    }

    lp = (LPSTR)szSec;
    FONTPRICE( lpFam ) = (DWORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv || (FONTPRICE(lpFam)&0x80000000))
    {
        ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_INVALIDPRICE,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)lpBuff );
    }

    FONTSIZE( lpFam ) = 0;

    while (wFace)
    {
        LPFACE lpFace;

        //*** Read a face name
        lstrcpy( szSec, NODENAME(lpFam) );
        wsprintf( szKey, GetRCString( IDS_FACE_F, hInstLib ), wFace );

        if (!READ_INI())
            break;
        
        if ( !(lpFace = CreateFace( lpFam )) )
        {
            ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_MEMALLOC );
            gwLoadError = IDS_MEMALLOC;
            return FALSE;
        }
        _fstrncpy( NODENAME(lpFace), lpBuff, LF_FACESIZE );
        NODENAME(lpFace)[LF_FACESIZE] = 0;

        if (!LoadPubFace( lpFace, lpFilename ))
            return FALSE;

        FONTSIZE(lpFam) += FONTSIZE( lpFace );

        wFace++;
    }

    if (wFace==1)     // No families found
    {
        ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        gwLoadError = IDS_ININOTFOUND;
        return FALSE;
    }
    return TRUE;

} //*** LoadPubFamily


//*************************************************************
//
//  LoadPubFace
//
//  Purpose:
//      Loads in the face info from the INI file
//
//
//  Parameters:
//      LPFAMLIY lpFam
//      LPSTR lpFilename
//      
//
//  Return: (BOOL NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

BOOL NEAR PASCAL LoadPubFace( LPFACE lpFace, LPSTR lpFilename )
{
    LPSTR lpFilePath;
    LPSTR lpBuff = (LPSTR)szBuffer;
    HWND  hMyWnd = GetActiveWindow();
    WORD  wStyle = 1;
    int   nConv;
    LPFAMILY lpFam = PARENT( LPFAMILY, lpFace );
    LPSTR lp, lpOld;

    wsprintf( szSec,"%s:%s",(LPSTR)NODENAME(lpFam),(LPSTR)NODENAME(lpFace));
    wsprintf( szKey, GetRCString( IDS_PRICE, hInstLib ) );

    if (!READ_INI())
    {
        ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        gwLoadError = IDS_ININOTFOUND;
        return FALSE;
    }

    lp = (LPSTR)szSec;
    FONTPRICE( lpFace ) = (DWORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv || (FONTPRICE(lpFace)&0x80000000))
    {
        ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_INVALIDPRICE,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)lpBuff );
        gwLoadError = IDS_INVALIDPRICE;
        return FALSE;
    }

    // Get File Path
    {
        LPDB lpDB = PARENT( LPDB, PARENT(LPPUB,lpFam) );
        lpFilePath = lpDB->szPathName;
    }
    FONTSIZE( lpFace ) = 0;


    while (wStyle)
    {
        LPSTYLE lpStyle;

        //*** Read a style name
        wsprintf( szKey, GetRCString( IDS_STYLE_F, hInstLib ), wStyle );

        if (!READ_INI())
            break;
        
        if ( !(lpStyle = CreateStyle( lpFace )) )
        {
            ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_MEMALLOC );
            gwLoadError = IDS_MEMALLOC;
            return FALSE;
        }

        // Heres some more single buffer magic
        lp = lpBuff;

        if (!ParseOffString( (LPSTR FAR *)&lp, lpBuff ))
        {
            ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_STYLENOTFOUND,
                (LPSTR)szSec, (LPSTR)szKey );
            gwLoadError = IDS_STYLENOTFOUND;
            return FALSE;
        }
        _fstrncpy( NODENAME(lpStyle), lpBuff, LF_FACESIZE );
        NODENAME(lpStyle)[LF_FACESIZE] = 0;

        if (!ParseOffString( (LPSTR FAR *)&lp, lpBuff ))
        {
            ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_TTFNOTFOUND,
                (LPSTR)szSec, (LPSTR)szKey );
            gwLoadError = IDS_TTFNOTFOUND;
            return FALSE;
        }
        _fstrncpy( lpStyle->Info.szTTFFile, lpBuff, TTF_LENGTH );
        lpStyle->Info.szTTFFile[TTF_LENGTH] = 0;

        lpOld = lp;                
        FONTPRICE( lpStyle ) = (DWORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
        if (!nConv || (FONTPRICE(lpStyle)&0x80000000))
        {
            ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_INVALIDPRICE,
                (LPSTR)szSec, (LPSTR)szKey,
                (LPSTR)lpOld );
            gwLoadError = IDS_INVALIDPRICE;
            return FALSE;
        }

        // Now get the length of that file
        wsprintf(lpBuff,"%s%s",lpFilePath,(LPSTR)lpStyle->Info.szTTFFile);

        {
            OFSTRUCT of;
            HFILE hFile;

            hFile = OpenFile( lpBuff, &of, OF_READ );
            if (!hFile)
            {
                ErrorResBox( hMyWnd, hInstLib, MB_WARN, IDS_FS, IDS_FILENOTFOUND2,
                    (LPSTR)szSec, (LPSTR)szKey,
                    (LPSTR)lpBuff );
                FONTSIZE(lpStyle) = 0L;
            }
            else
            {
                FONTSIZE(lpStyle) = (DWORD)filelength( hFile );
                _lclose( hFile );
            }
            FONTSIZE( lpFace ) += FONTSIZE( lpStyle );
        }

        wStyle++;
    }

    if (wStyle==1)     // No families found
    {
        ErrorResBox( hMyWnd, hInstLib, MB_ERR, IDS_FS, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        gwLoadError = IDS_ININOTFOUND;
        return FALSE;
    }
    return TRUE;

} //*** LoadPubFace

//*** EOF: pubinfo.c
