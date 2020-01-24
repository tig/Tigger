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

#include "global.h"
#include <io.h>
#include <stdlib.h>
#include <string.h>

#define READ_INI()  GetPrivateProfileString(szSec, szKey, "", lpBuff, 255, lpFilename )

extern char szPSBuff[120];

//*** Local buffers                           
static char szSec[80];
static char szKey[32];
static char szBuffer[255];
static char szPubFile[255];

WORD gwLoadError;

BOOL NEAR PASCAL LoadCollections( LPPUB lpPub, LPSTR lpFilename );
BOOL NEAR PASCAL LoadPubFamilies( LPPUB lpPub, LPSTR lpFilename );
BOOL NEAR PASCAL LoadPubFamily( LPFAMILY lpFam, LPSTR lpFilename );
BOOL NEAR PASCAL LoadPubFace( LPFACE lpFace, LPSTR lpFilename );
LPSTR LoadTextFile( HHEAP hhHeap, LPSTR lpFile, LPWORD lpwLen );

long glStylesLoaded = 0;

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
    WORD  wFamily = 1;

    glStylesLoaded = 0;

    SetCursor( LoadCursor(NULL,IDC_WAIT) );

    //*** store publisher filename for retrieving thumb text
    lstrcpy( szPubFile, lpFilename );

    //*** Store file path in DB Header.
    //*** In the end another directory will be stripped off.
    lstrcpy( lpDB->szPathName, lpFilename );
    {
        LPSTR lp = lpDB->szPathName;

        lp += lstrlen(lp) - 1;

        while (*lp != ':' && *lp != '\\' && lp>lpDB->szPathName)
            lp--;
        if (lp>lpDB->szPathName)
            *(++lp) = '\0';
    }

    //*** Allocate a publisher node
    if (!(lpPub = CreatePublisher( lpDB )  ))
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_MEMDENIED );
        gwLoadError = IDS_MEMDENIED;
        SetCursor( LoadCursor(NULL,IDC_ARROW) );
        return NULL ;
    }

    //*** Read a publisher ID
    lstrcpy( szSec, GRCS( IDS_PUBLISHER ) );
    lstrcpy( szKey, GRCS( IDS_ID ) );

    if (!READ_INI())
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        FreePublisher( lpPub ) ;
        gwLoadError = IDS_ININOTFOUND;
        SetCursor( LoadCursor(NULL,IDC_ARROW) );
        return NULL ;
    }
    else
    {
        LPSTR lp = szBuffer;
        int nConv, i = (int)ParseOffNumber((LPSTR FAR *)&lp, &nConv );

        if ( !nConv || i<=0 || i>= 0x1000)
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_INVALIDID,
                (LPSTR)szSec, (LPSTR)szKey,
                (LPSTR)szBuffer );
            FreePublisher( lpPub ) ;
            gwLoadError = IDS_INVALIDID;
            SetCursor( LoadCursor(NULL,IDC_ARROW) );
            return NULL ;
        }
        lpPub->fiFSPub = (WORD)i;
    }

    //*** Read a publisher name
    lstrcpy( szSec, GRCS( IDS_PUBLISHER ) );
    lstrcpy( szKey, GRCS( IDS_NAME ) );

    if (!READ_INI())
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        FreePublisher( lpPub ) ;
        gwLoadError = IDS_ININOTFOUND;
        SetCursor( LoadCursor(NULL,IDC_ARROW) );
        return NULL ;
    }

    if (!(lpPub->lpszPublisher = ALLOC_STRING( lpPub, lpBuff )))
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_MEMDENIED );
        FreePublisher( lpPub ) ;
        gwLoadError = IDS_MEMDENIED;
        SetCursor( LoadCursor(NULL,IDC_ARROW) );
        return NULL ;
    }
    lpPub->PubInfo.wNameSize = lstrlen(lpBuff) + 1;

    //*** Read a publisher short copyright
    lstrcpy( szKey, GRCS( IDS_SHORTCOPYRIGHT ) );

    if (!READ_INI())
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        FreePublisher( lpPub ) ;
        gwLoadError = IDS_ININOTFOUND;
        SetCursor( LoadCursor(NULL,IDC_ARROW) );
        return NULL ;
    }

    if (!(lpPub->lpszSCopyright = ALLOC_STRING( lpPub, lpBuff )))
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_MEMDENIED );
        FreePublisher( lpPub ) ;
        gwLoadError = IDS_MEMDENIED;
        SetCursor( LoadCursor(NULL,IDC_ARROW) );
        return NULL ;
    }
    lpPub->PubInfo.wSCopyrightSize = lstrlen(lpBuff) + 1;

    //*** Read a publisher long copyright
    lstrcpy( szKey, GRCS( IDS_LONGCOPYRIGHT ) );

    if (READ_INI())
    {
        char  szTemp[120];
        LPSTR lp = (LPSTR)szTemp;
        WORD  wSize;

        wsprintf( lp, "%s%s", (LPSTR)lpDB->szPathName, lpBuff );

        lpPub->lpszLCopyright = LoadTextFile( GETHHEAP(lpPub), lp, &wSize );
        lpPub->PubInfo.wLCopyrightSize = wSize;
    }

    if (!LoadCollections(lpPub, lpFilename))
    {
        FreePublisher( lpPub );
        SetCursor( LoadCursor(NULL,IDC_ARROW) );
        return NULL ;
    }

    if (LoadPubFamilies( lpPub, lpFilename ))
    {
        LPSTR lp = lpDB->szPathName;

        lp += lstrlen(lp) - 2;

        // Knock off another directory. Go up a level
        while (*lp != ':' && *lp != '\\' && lp>lpDB->szPathName)
            lp--;
        if (lp>lpDB->szPathName)
            *(++lp) = '\0';

        SetCursor( LoadCursor(NULL,IDC_ARROW) );
        SortDatabase( lpDB );
        return lpPub;
    }
    SetCursor( LoadCursor(NULL,IDC_ARROW) );
    return NULL;

} //*** LoadPubInfo


//*************************************************************
//
//  LoadCollections
//
//  Purpose:
//      Loads the collections, if any into memory
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
//               3/19/92   MSM        Created
//
//*************************************************************

BOOL NEAR PASCAL LoadCollections( LPPUB lpPub, LPSTR lpFilename )
{
    LPSTR  lp, lpBuff = (LPSTR)szBuffer;
    WORD   w=0;
    LPCOLL lpColl;
    int    i,n;

    lstrcpy( szSec, GRCS( IDS_PUBLISHER ) );
    SetWindowText( ghwndStat, "Loading Collections" );
    UpdateWindow( ghwndStat );

    for (i=1; i<=32767; i++)
    {
        LPSTR lpOld = lp;
        int nConv;

        wsprintf( szKey, GRCS( IDS_COLLECTION_F), i );

        //*** No more collections
        if (!READ_INI())
            break;

        lpColl = CreateCollection( lpPub );

        //*** Heres some more single buffer magic
        lp = lpBuff;

        // Get collection name
        if (!ParseOffString( (LPSTR FAR *)&lp, lpBuff ))
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_COLLNOTFOUND,
                (LPSTR)szSec, (LPSTR)szKey );
            gwLoadError = IDS_COLLNOTFOUND;
            return FALSE;
        }
        _fstrncpy( lpColl->Info.szName, lpBuff, sizeof(lpColl->Info.szName) );
        lpColl->Info.szName[sizeof( lpColl->Info.szName )] = 0;

        //*** Get file name
        if (!ParseOffString( (LPSTR FAR *)&lp, lpBuff ))
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_COLLFILENOTFOUND,
                (LPSTR)szSec, (LPSTR)szKey );
            gwLoadError = IDS_COLLFILENOTFOUND;
            return FALSE;
        }

        //*** lpBuff now contains the file to load, but we're
        //*** saving it for later

        //*** Get collection price
        lpOld = lp;
        lpColl->Info.dwPrice = (DWORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
        if (!nConv)
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_INVALIDPRICE,
                (LPSTR)szSec, (LPSTR)szKey,
                (LPSTR)lpOld );
            gwLoadError = IDS_INVALIDPRICE;
            return FALSE;
        }

        lpOld = lp;
        for (n=0; n<5; n++)
        {
            lpColl->Info.rgShippingRates[n] = (DWORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
            if (!nConv)
            {
                ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_INVALIDSHIPPINGRATES,
                    (LPSTR)szSec, (LPSTR)szKey,
                    (LPSTR)lpOld );
                gwLoadError = IDS_INVALIDSHIPPINGRATES;
                return FALSE;
            }
        }

        lpOld = lp;
        lpColl->Info.dwID = (DWORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
        if (!nConv)
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_INVALIDIID,
                (LPSTR)szSec, (LPSTR)szKey,
                (LPSTR)lpOld );
            gwLoadError = IDS_INVALIDIID;
            return FALSE;
        }

        {
            char  szTemp[120];
            LPSTR lp = (LPSTR)szTemp;
            WORD  wSize;
            LPDB  lpDB = PARENT(LPDB,lpPub);

            wsprintf( lp, "%s%s", (LPSTR)lpDB->szPathName, lpBuff );

            lpColl->lpszText = LoadTextFile( GETHHEAP(lpPub), lp, &wSize );

            if (!lpColl->lpszText)
                return FALSE;
            lpColl->Info.wTextSize = wSize;
        }
    }

    lpPub->PubInfo.wCollections = (WORD)i-1;
    return TRUE;

} //*** LoadCollections


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
//               2/29/92   MSM        Created
//
//*************************************************************

BOOL NEAR PASCAL LoadPubFamilies( LPPUB lpPub, LPSTR lpFilename )
{
    LPSTR lpBuff = (LPSTR)szBuffer;
    WORD  wFamily = 1;

    while (wFamily)
    {
        LPFAMILY lpFam;

        //*** Read a family name
        lstrcpy( szSec,  GRCS( IDS_PUBLISHER) );
        wsprintf( szKey, GRCS( IDS_FAMILY_F), wFamily );

        if (!READ_INI())
            break;
        
        if ( !(lpFam = CreateFamily( lpPub )) )
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_MEMDENIED );
            FreePublisher( lpPub ) ;
            gwLoadError = IDS_MEMDENIED;
            return FALSE;
        }
        _fstrncpy( NODENAME(lpFam), lpBuff, LF_FACESIZE );
        NODENAME(lpFam)[LF_FACESIZE] = 0;

        wsprintf( szPSBuff, "Loading %s", (LPSTR)NODENAME(lpFam) );
        SetWindowText( ghwndStat, szPSBuff );
        UpdateWindow( ghwndStat );

        if (!LoadPubFamily( lpFam, lpFilename ))
            return FALSE;
        lpFam->fiFF = wFamily;
        wFamily++;
    }

    if (wFamily==1)     // No families found
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_NOFAMILIESFOUND );
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
    WORD  wFace = 1;
    int   nConv;
    LPSTR lp;

    lstrcpy( szSec, NODENAME(lpFam) );
    lstrcpy( szKey, GRCS( IDS_PRICE ) );

    if (!READ_INI())
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        gwLoadError = IDS_ININOTFOUND;
        return FALSE;
    }

    lp = lpBuff;
    FONTPRICE( lpFam ) = (DWORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv || (FONTPRICE(lpFam)&0x80000000))
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_INVALIDPRICE,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)lpBuff );
        gwLoadError = IDS_INVALIDPRICE;
        return FALSE;
    }

    lstrcpy( szKey, GRCS( IDS_INGRESID ) );

    if (!READ_INI())
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        gwLoadError = IDS_ININOTFOUND;
        return FALSE;
    }

    lp = lpBuff;
    lpFam->Info.dwID = (DWORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv || (FONTPRICE(lpFam)&0x80000000))
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_INVALIDIID,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)lpBuff );
        gwLoadError = IDS_INVALIDIID;
        return FALSE;
    }

    FONTSIZE( lpFam ) = 0;
    STYLES(lpFam) = 0;

    while (wFace)
    {
        LPFACE lpFace;

        //*** Read a face name
        lstrcpy( szSec, NODENAME(lpFam) );
        wsprintf( szKey, GRCS( IDS_FACE_F ), wFace );

        if (!READ_INI())
            break;
        
        if ( !(lpFace = CreateFace( lpFam )) )
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_MEMDENIED );
            gwLoadError = IDS_MEMDENIED;
            return FALSE;
        }
        _fstrncpy( NODENAME(lpFace), lpBuff, LF_FACESIZE );
        NODENAME(lpFace)[LF_FACESIZE] = 0;

        if (!LoadPubFace( lpFace, lpFilename ))
            return FALSE;

        FONTSIZE(lpFam) += FONTSIZE( lpFace );
        STYLES(lpFam) +=STYLES( lpFace );

        wFace++;
    }

    if (wFace==1)     // No faces found
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        gwLoadError = IDS_ININOTFOUND;
        return FALSE;
    }
    else
    {
        WORD wFaces = GetPrivateProfileInt(szSec,GRCS(IDS_FACES),0,lpFilename);

        wFace--;
        if (wFaces!=wFace)
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                IDS_FACESNOTFOUND, wFace, wFaces );
            gwLoadError = IDS_FACESNOTFOUND;
            return FALSE;
        }
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
//               2/29/92   MSM        Created
//
//*************************************************************

BOOL NEAR PASCAL LoadPubFace( LPFACE lpFace, LPSTR lpFilename )
{
    LPSTR lpFilePath;
    LPSTR lpBuff = (LPSTR)szBuffer;
    WORD  wStyle = 1;
    int   nConv;
    LPFAMILY lpFam = PARENT( LPFAMILY, lpFace );
    LPSTR lp, lpOld;

    wsprintf( szSec,"%s:%s",(LPSTR)NODENAME(lpFam),(LPSTR)NODENAME(lpFace));
    lstrcpy( szKey, GRCS( IDS_PRICE ) );

    SetWindowText( ghwndStat, szSec );
    UpdateWindow( ghwndStat );

    if (!READ_INI())
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        gwLoadError = IDS_ININOTFOUND;
        return FALSE;
    }

    lp = (LPSTR)lpBuff;
    FONTPRICE( lpFace ) = (DWORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv || (FONTPRICE(lpFace)&0x80000000))
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_INVALIDPRICE,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)lpBuff );
        gwLoadError = IDS_INVALIDPRICE;
        return FALSE;
    }

    lstrcpy( szKey, GRCS( IDS_INGRESID ) );

    if (!READ_INI())
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        gwLoadError = IDS_ININOTFOUND;
        return FALSE;
    }

    lp = (LPSTR)lpBuff;
    lpFace->Info.dwID = (DWORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv || (FONTPRICE(lpFace)&0x80000000))
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_INVALIDIID,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)lpBuff );
        gwLoadError = IDS_INVALIDIID;
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
        wsprintf( szKey, GRCS( IDS_STYLE_F ), wStyle );

        if (!READ_INI())
            break;
        
        if ( !(lpStyle = CreateStyle( lpFace )) )
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_MEMDENIED );
            gwLoadError = IDS_MEMDENIED;
            return FALSE;
        }

        // Heres some more single buffer magic
        lp = lpBuff;

        // Get Style name
        if (!ParseOffString( (LPSTR FAR *)&lp, lpBuff ))
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_STYLENOTFOUND,
                (LPSTR)szSec, (LPSTR)szKey );
            gwLoadError = IDS_STYLENOTFOUND;
            return FALSE;
        }
        _fstrncpy( NODENAME(lpStyle), lpBuff, LF_FACESIZE );
        NODENAME(lpStyle)[LF_FACESIZE] = 0;

        // Get TTF file name
        if (!ParseOffString( (LPSTR FAR *)&lp, lpBuff ))
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_TTFNOTFOUND,
                (LPSTR)szSec, (LPSTR)szKey );
            gwLoadError = IDS_TTFNOTFOUND;
            return FALSE;
        }
        _fstrncpy( lpStyle->Info.szTTFFile, lpBuff, TTF_LENGTH );
        lpStyle->Info.szTTFFile[TTF_LENGTH] = 0;

        // Get font style price
        lpOld = lp;                
        FONTPRICE( lpStyle ) = (DWORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
        if (!nConv || (FONTPRICE(lpStyle)&0x80000000))
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_INVALIDPRICE,
                (LPSTR)szSec, (LPSTR)szKey,
                (LPSTR)lpOld );
            gwLoadError = IDS_INVALIDPRICE;
            return FALSE;
        }

        // Get font style ID
        lpOld = lp;                
        lpStyle->Info.dwID = (DWORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
        if (!nConv)
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_INVALIDIID,
                (LPSTR)szSec, (LPSTR)szKey,
                (LPSTR)lpOld );
            gwLoadError = IDS_INVALIDIID;
            return FALSE;
        }

        // Now make sure we have the font installed
        {
            HFONT hFont = DBGetFont( lpStyle, THUMB_POINTSIZE );
            
            if (hFont)
            {
                HDC hDC = GetDC( NULL );
                OUTLINETEXTMETRIC otm;
                int i, n0s=0;
                LPBYTE lpPanBytes;

                hFont = SelectFont( hDC, hFont );

                if (!GetOutlineTextMetrics(hDC,sizeof(OUTLINETEXTMETRIC),&otm))
                {
                    ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_GETTMFAILED,
                        (LPSTR)NODENAME(lpFace), (LPSTR)NODENAME(lpStyle) );
                    hFont = SelectFont( hDC, hFont );
                    ReleaseDC( NULL, hDC );
                    DeleteObject( hFont );
                    gwLoadError = IDS_GETTMFAILED;
                    return FALSE;
                }
                hFont = SelectFont( hDC, hFont );
                ReleaseDC( NULL, hDC );
                DeleteObject( hFont );

                lpStyle->Info.Panose = otm.otmPanoseNumber;

                lpPanBytes = (LPBYTE)&(lpStyle->Info.Panose);

                n0s=0;

                //*** Test for a valid PANOSE number
                for (i=0; i< (sizeof(PANOSE)/sizeof(BYTE)); i++)
                {
                    if ( lpPanBytes[i]==0)
                        n0s++;
                }

                //*** Too many 0's to be a useful PANOSE number.
                //*** Michael Delorantis from Elseware gave me this.
                if (n0s>=4)
                    for (i=0; i< (sizeof(PANOSE)/sizeof(BYTE)); i++)
                        lpPanBytes[i]=0;
            }
            else
            {
                ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_INVALIDFONT,
                    (LPSTR)szSec, (LPSTR)szKey,
                    (LPSTR)NODENAME(lpFace), (LPSTR)NODENAME(lpStyle) );
                gwLoadError = IDS_INVALIDFONT;
                return FALSE;
            }
            glStylesLoaded++;

            if (glStylesLoaded >= 75)
            {
                glStylesLoaded = 0;
                GlobalCompact( -1L );
            }
        }


        // Now get the length of that file
        wsprintf(lpBuff,"%s%s",lpFilePath,(LPSTR)lpStyle->Info.szTTFFile);

        {
            OFSTRUCT of;
            HFILE    hFile;
            LPSTR    lpt = lpBuff + lstrlen(lpBuff) - 1;

            // Put in an _ since these should be compressed
            *lpt = '_';

            hFile = OpenFile( lpBuff, &of, OF_READ );
            if (hFile<=0)
            {
                if (ErrorResBox( ghwndMain, ghInst, MB_WARN, IDS_APPNAME, IDS_FILENOTFOUND2,
                    (LPSTR)szSec, (LPSTR)szKey, (LPSTR)lpBuff )==IDCANCEL)
                {
                    gwLoadError = IDS_TTFNOTFOUND;
                    return FALSE;
                }
                FONTSIZE(lpStyle) = 0L;
            }
            else
            {
                FONTSIZE(lpStyle) = (DWORD)_filelength( hFile );
                _lclose( hFile );
            }
            FONTSIZE( lpFace ) += FONTSIZE( lpStyle );
        }

        lpStyle->wBitmapIndex = wStyle-1;
        wStyle++;
    }

    if (wStyle==1)     // No Styles found
    {
        ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_ININOTFOUND,
            (LPSTR)szSec, (LPSTR)szKey,
            (LPSTR)szKey );
        gwLoadError = IDS_ININOTFOUND;
        return FALSE;
    }
    STYLES(lpFace) = wStyle-1;
    return TRUE;

} //*** LoadPubFace


//*************************************************************
//
//  LoadTextFile
//
//  Purpose:
//      Loads the file and returns the memory block
//
//
//  Parameters:
//      HHEAP hhHeap
//      LPSTR lpFile
//      LPWORD lpwLen
//
//  Return: (LPSTR)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/19/92   MSM        Created
//
//*************************************************************

LPSTR LoadTextFile( HHEAP hhHeap, LPSTR lpFile, LPWORD lpwLen )
{
    OFSTRUCT of ;
    HFILE    hFile ;
    LPSTR    lp;

    if ((hFile = OpenFile( lpFile, &of, OF_READ | OF_SHARE_DENY_NONE ))>0)
    {
        UINT cb = (UINT)_filelength( hFile );

        if (lp = (LPSTR)fsmmHeapAllocLock(hhHeap, LPTR, cb+1))
        {
            if(_lread( hFile, lp, cb )!=cb)
            {
                ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, 
                    IDS_READFAILED, lpFile );
                fsmmHeapUnlockFree( hhHeap, lp ) ;
                gwLoadError = IDS_READFAILED;
                return NULL ;
            }
            *lpwLen = cb+1;
        }
        else
        {
            ErrorResBox( ghwndMain, ghInst, MB_ERR, IDS_APPNAME, IDS_MEMDENIED );
            gwLoadError = IDS_MEMDENIED;
            return NULL ;
        }
        _lclose( hFile ) ;
        return lp;
    }
    else
    {
        if (ErrorResBox( ghwndMain, ghInst, MB_WARN, IDS_APPNAME, IDS_FILENOTFOUND2,
            (LPSTR)szSec, (LPSTR)szKey, lp ) == IDCANCEL)
            gwLoadError = IDS_FILENOTFOUND;
        else
            gwLoadError = 0;
        return NULL ;
    }

} //*** LoadTextFile


//*************************************************************
//
//  GetThumbString
//
//  Purpose:
//      Given a style, load the thumb text
//
//
//  Parameters:
//      LPSTYLE lpStyle
//      
//
//  Return: (LPSTR WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/14/92   MSM        Created
//
//*************************************************************

LPSTR WINAPI GetThumbString( LPSTYLE lpStyle )
{
    LPFACE lpFace = PARENT( LPFACE, lpStyle );
    LPFAMILY lpFam = PARENT( LPFAMILY, lpFace );
    LPSTR lp;

    wsprintf( szSec,"%s:%s",(LPSTR)NODENAME(lpFam),(LPSTR)NODENAME(lpFace));
    wsprintf( szKey,"StyleText%d", (int)ITEM_INDEX(lpFace,lpStyle)+1 );

    lp = szBuffer;

    if (!GetPrivateProfileString(szSec, szKey, "", lp, 255, szPubFile ))
        return GRCS( IDS_PANOSE_STRING );

    ShrinkStringFromPrintable( lp );
    return (LPSTR)lp;

} //*** GetThumbString


//*************************************************************
//
//  SetThumbString
//
//  Purpose:
//      Given a style, load the thumb text
//
//
//  Parameters:
//      LPSTYLE lpStyle
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/14/92   MSM        Created
//
//*************************************************************

VOID WINAPI SetThumbString( LPSTYLE lpStyle, LPSTR lpText )
{
    LPFACE   lpFace = PARENT( LPFACE, lpStyle );
    LPFAMILY lpFam = PARENT( LPFAMILY, lpFace );
    LPSTR    lp;

    wsprintf( szSec,"%s:%s",(LPSTR)NODENAME(lpFam),(LPSTR)NODENAME(lpFace));
    wsprintf( szKey,"StyleText%d", (int)ITEM_INDEX(lpFace,lpStyle)+1 );

    lp = szBuffer;
    lstrcpy( lp, lpText );
    ExpandStringToPrintable( lp );

    if ( lstrcmp(lpText,GRCS(IDS_PANOSE_STRING)) )
        WritePrivateProfileString(szSec, szKey, lp, szPubFile );
    else //*** don't save if it's thw default
        WritePrivateProfileString(szSec, szKey, NULL, szPubFile );

} //*** SetThumbString

//*** EOF: pubinfo.c
