//*************************************************************
//  File name: db.c
//
//  Description: 
//      Code for Database helper routines.
//
//  History:    Date       Author     Comment
//               2/20/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"

#include "intFCDB.h"

WORD     gwBitmapError = 0;
WORD     gwLoadError = 0;
OFSTRUCT gof;
char     szOld[120], szNew[120];
struct _find_t gff, gff2, gff3;

//*** Some file specific macros, ASSUMES local variable wErr and fileIO_error!
#define FERROR(X)   {wErr=X; goto fileIO_error;}

//*************************************************************
//
//  ShrinkFilename
//
//  Purpose:
//      Shrinks a file name into its word equivalent.  Depends on
//      MSPUB or F? prefix
//
//  Parameters:
//      LPSTR lpFile
//      
//
//  Return: (WORD)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

WORD WINAPI ShrinkFilename( LPSTR lpFile )
{
    LPSTR lp = lpFile + lstrlen(lpFile) - 1;
    WORD  wI, wVal=0;

    while (*lp!='\\' && *lp!=':' && lp>=lpFile)
        lp--;

    // Skip over \FSPUB or \FF123
    lp += 6;

    for (wI=0; wI<3; wI++)
    {
        if (!isxdigit( *lp ))
            return 0xFFFF;

        wVal *= 16;
        if (isdigit( *lp ))
            wVal += ( *lp - '0' );
        else
            wVal += ( toupper(*lp) - 'A' + 10 );
        lp++;
    }
    return wVal;

} //*** ShrinkFilename


//*************************************************************
//
//  ExpandFilename
//
//  Purpose:
//      Expands the file IDs into a file name with no extension
//
//
//  Parameters:
//      LPDB lpDB
//      WORD wFlags
//      FILEID fiPub
//      FILEID fiFamily
//      LPSTR lpBuff
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

VOID WINAPI ExpandFilename(LPDB lpDB, WORD wFlags, FILEID fiPub, FILEID fiFamily, LPSTR lpBuff)
{
    LPSTR lp;

    lstrcpy( lpBuff, lpDB->szPathName );
    lp = lpBuff + lstrlen( lpBuff );

    switch (wFlags&~FDB_TYPE_DIR)
    {
        case FDB_TYPE_FSPUB:
            if (wFlags&FDB_TYPE_DIR)
                wsprintf( lp, "FSPUB%03X\\FSPUB%03X", fiPub&0x0FFF,fiPub&0x0FFF);
            else    
                wsprintf( lp, "FSPUB%03X", fiPub&0x0FFF );
        break;

        case FDB_TYPE_FF:
            if (wFlags&FDB_TYPE_DIR)
                wsprintf( lp, "FSPUB%03X\\FF%03X%03X", fiPub&0x0FFF, fiPub&0x0FFF, fiFamily&0x0FFF );
            else    
                wsprintf( lp, "FF%03X%03X", fiPub&0x0FFF, fiFamily&0x0FFF );
        break;

        case FDB_TYPE_FS:
            if (wFlags&FDB_TYPE_DIR)
                wsprintf( lp, "FSPUB%03X\\FS%03X%03X", fiPub&0x0FFF, fiPub&0x0FFF, fiFamily&0x0FFF );
            else    
                wsprintf( lp, "FS%03X%03X", fiPub&0x0FFF, fiFamily&0x0FFF );
        break;

        case FDB_TYPE_FT:
            if (wFlags&FDB_TYPE_DIR)
                wsprintf( lp, "FSPUB%03X\\FT%03X%03X", fiPub&0x0FFF, fiPub&0x0FFF, fiFamily&0x0FFF );
            else    
                wsprintf( lp, "FT%03X%03X", fiPub&0x0FFF, fiFamily&0x0FFF );
        break;

        case FDB_TYPE_FY:
            if (wFlags&FDB_TYPE_DIR)
                wsprintf( lp, "FSPUB%03X\\FY%03X%03X", fiPub&0x0FFF, fiPub&0x0FFF, fiFamily&0x0FFF );
            else    
                wsprintf( lp, "FY%03X%03X", fiPub&0x0FFF, fiFamily&0x0FFF );
        break;

        default:
            *lp = '\0';
        break;
    }

} //*** ExpandFilename


//*************************************************************
//
//  SetFileVersion
//
//  Purpose:
//      Sets the extension on the file to the current version
//
//
//  Parameters:
//      LPSTR lpFilename
//      WORD wVersion
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

VOID WINAPI SetFileVersion( LPSTR lpFilename, WORD wVersion )
{
    char szExt[8];

    wsprintf( szExt, ".%03X", wVersion & 0x0FFF );
    fsmmSetExtension( lpFilename, szExt );

} //*** SetFileVersion


//*************************************************************
//
//  GetFileVersion
//
//  Purpose:
//      Gets the version on the file
//
//
//  Parameters:
//      LPSTR lpFilename
//      
//
//  Return: (WORD)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

WORD WINAPI GetFileVersion( LPSTR lpFilename )
{
    LPSTR lp = lpFilename + lstrlen( lpFilename) - 1;

    while (*lp!='.' && lp>=lpFilename)
        lp--;
    if (*lp=='.')
    {
        WORD wI, wVer=0;

        lp++;

        for (wI=0; wI<3; wI++)
        {
            if (!isxdigit( *lp ))
                return 0xFFFF;

            wVer *= 16;
            if (isdigit( *lp ))
                wVer += ( *lp - '0' );
            else
                wVer += ( toupper(*lp) - 'A' + 10);

            lp++;
        }
        return wVer;
    }
    return 0xFFFF;

} //*** GetFileVersion


//*************************************************************
//
//  ValidateFileAndGetVersion
//
//  Purpose:
//      Validates the presence of a file on the disk and retrieves a 
//      version
//
//  Parameters:
//      LPSTR lpFilename - filename with no extension
//      
//
//  Return: (WORD)
//      The version # if found or -1 if FileNotFound
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

WORD WINAPI ValidateFileAndGetVersion(LPSTR lpFilename )
{
    WORD   wI, wVer = 0;
    LPSTR  lpExt = lpFilename + lstrlen( lpFilename );

    if (lpFilename != (LPSTR)gof.szPathName)
        lstrcpy( gof.szPathName, lpFilename );
    lstrcat( gof.szPathName, ".*" );

    if (!_dos_findfirst( gof.szPathName, _A_NORMAL, &gff2 ))
    {
        PSTR ptr = (PSTR)gff2.name;

        while (*ptr!='.' && *ptr!='\0')
            ptr++;
        if (*ptr=='\0')
            return 0xFFFF;

        lstrcpy( lpExt, ptr );
        ptr++;

        for (wI=0; wI<3; wI++)
        {
            if (!isxdigit( *ptr ))
                return 0xFFFF;

            wVer *= 16;
            if (isdigit( *ptr ))
                wVer += ( *ptr - '0' );
            else
                wVer += ( toupper(*ptr) - 'A' + 10);
            ptr++;
        }
        return wVer;
    }
    return 0xFFFF;

} //*** ValidateFileAndGetVersion


//*************************************************************
//
//  CreateStyle
//
//  Purpose:
//      Creates a style and inserts it into the face list
//
//
//  Parameters:
//      LPFACE lpFace - pointer to face to create style for
//      
//
//  Return: (LPSTYLE)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/20/92   MSM        Created
//
//*************************************************************

LPSTYLE WINAPI CreateStyle( LPFACE lpFace )
{
    LPSTYLE lpStyle = (LPSTYLE)ALLOC_LIST_NODE( lpFace, sizeof(STYLE) );

    if (lpStyle)
    {
        lpStyle->dbn.wNodeType = NT_STYLE;
        lpStyle->dbn.lpParent = (LPDBNODE)lpFace;
        ADD_LIST_NODE( lpFace, lpStyle );
        lpStyle->dbn.wViewInfo = VIB_NONEXPANDABLE | VIB_DOWNLOAD;
    }
    return lpStyle;

} //*** CreateStyle


//*************************************************************
//
//  FreeStyle
//
//  Purpose:
//      Frees the style entry and removes it from the FACE list
//
//
//  Parameters:
//      LPSTYLE lpStyle
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

VOID WINAPI FreeStyle( LPSTYLE lpStyle )
{
    // Free actual node
    DELETE_LIST_NODE( PARENT(LPFACE,lpStyle), lpStyle );

} //*** FreeStyle


//*************************************************************
//
//  CreateFace
//
//  Purpose:
//      Creates a face and inserts it into the family list
//
//
//  Parameters:
//      LPFAMILY lpFamily - pointer to family to create face for
//      
//
//  Return: (LPSTYLE)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/20/92   MSM        Created
//
//*************************************************************

LPFACE WINAPI CreateFace( LPFAMILY lpFamily )
{
    LPFACE lpFace = (LPFACE)ALLOC_LIST_NODE( lpFamily, sizeof(FACE) );

    if (lpFace)
    {
        lpFace->hlstList = fsmmListCreate( GETHHEAP(lpFamily) );

        if (!lpFace->hlstList)
            return NULL;
        lpFace->dbn.wNodeType = NT_FACE;
        lpFace->dbn.lpParent = (LPDBNODE)lpFamily;
        lpFace->dbn.wViewInfo = VIB_DOWNLOAD;

        ADD_LIST_NODE( lpFamily, lpFace );
    }
    return lpFace;

} //*** CreateFace


//*************************************************************
//
//  FreeFace
//
//  Purpose:
//      Free the face and all child styles
//
//
//  Parameters:
//      LPFACE lpFace
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

VOID WINAPI FreeFace( LPFACE lpFace )
{
    LPSTYLE lpStyle = GetFirstStyle( lpFace, NULL );

    // Free children
    while (lpStyle)
    {
        FreeStyle( lpStyle );
        lpStyle = GetFirstStyle( lpFace, NULL  );
    }

    // Free bitmap
    if (lpFace->hBitmap)
        DeleteObject( lpFace->hBitmap );

    // Free child list
    fsmmListDestroy( lpFace->hlstList );

    // Free actual node
    DELETE_LIST_NODE( PARENT(LPFAMILY,lpFace), lpFace );

} //*** FreeFace


//*************************************************************
//
//  LoadFace
//
//  Purpose:
//      Loads in a face, and styles, from the file
//
//
//  Parameters:
//      LPFAMILY lpFamily
//      HFILE hFile
//      
//
//  Return: (LPFACE)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

LPFACE WINAPI LoadFace( LPFAMILY lpFamily, HFILE hFile )
{
    LPFACE lpFace = CreateFace( lpFamily );
    WORD   wStyles, wLen, wErr = 0;    

    if (!lpFace)
        FERROR(IDS_MEMALLOC);

    wLen = _lread( hFile, (LPSTR)&(lpFace->Info), sizeof(FACEINFO) );
    if (wLen!=sizeof(FACEINFO))
    {
        if (wLen==HFILE_ERROR)
        {
            FERROR(IDS_FILEREADFAILED);
        }
        else
        {
            FERROR(IDS_DBCORRUPT);
        }
    }

    //*** Is the price encoded
    if (lpFace->Info.dwPrice & 0x80000000)
        lpFace->Info.dwPrice ^= PRICE_ENCODE;

    for (wStyles=0; wStyles<STYLES(lpFace); wStyles++)
    {
        LPSTYLE lpStyle = CreateStyle( lpFace );

        if (!lpStyle)
            FERROR(IDS_MEMALLOC);

        wLen = _lread( hFile, (LPSTR)&(lpStyle->Info), sizeof(STYLEINFO) );
        if (wLen!=sizeof(STYLEINFO))
        {
            if (wLen==HFILE_ERROR)
            {
                FERROR(IDS_FILEREADFAILED);
            }
            else
            {
                FERROR(IDS_DBCORRUPT);
            }
        }

        lpStyle->wBitmapIndex = wStyles;

        //*** Is the price encoded
        if (lpStyle->Info.dwPrice & 0x80000000)
            lpStyle->Info.dwPrice ^= PRICE_ENCODE;

    }
    wErr = LE_OK;

fileIO_error:
    if (wErr!=LE_OK)
    {
        FreeFace( lpFace );
        lpFace = NULL;
    }
    gwLoadError = wErr;
    return lpFace;

} //*** LoadFace


//*************************************************************
//
//  CreateFamily
//
//  Purpose:
//      Creates a family and inserts it into the Publisher list
//
//
//  Parameters:
//      LPPUB lpPub
//      
//
//  Return: (LPFAMILY)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/21/92   MSM        Created
//
//*************************************************************

LPFAMILY WINAPI CreateFamily( LPPUB lpPub )
{
    LPFAMILY lpFamily = (LPFAMILY)ALLOC_LIST_NODE( lpPub, sizeof(FAMILY) );

    if (lpFamily)
    {
        lpFamily->hlstList = fsmmListCreate( GETHHEAP(lpPub) );

        if (!lpFamily->hlstList)
            return NULL;
        lpFamily->dbn.wNodeType = NT_FAMILY;
        lpFamily->dbn.lpParent = (LPDBNODE)lpPub;
        lpFamily->dbn.wViewInfo = VIB_DROPSHIP|VIB_STATUSOPEN;

        ADD_LIST_NODE( lpPub, lpFamily );
    }
    return lpFamily;

} //*** CreateFamily


//*************************************************************
//
//  FreeFamily
//
//  Purpose:
//      Frees a family and all its children
//
//
//  Parameters:
//      LPFAMILY lpFamily
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

VOID WINAPI FreeFamily( LPFAMILY lpFamily )
{
    LPFACE lpFace = GetFirstFace( lpFamily, NULL  );

    // Free children
    while (lpFace)
    {
        FreeFace( lpFace );
        lpFace = GetFirstFace( lpFamily, NULL  );
    }

    // Free child list
    fsmmListDestroy( lpFamily->hlstList );

    // Free actual node
    DELETE_LIST_NODE( PARENT(LPPUB,lpFamily), lpFamily );

} //*** FreeFamily


//*************************************************************
//
//  LoadFamily
//
//  Purpose:
//      Loads in a family from the disk
//
//
//  Parameters:
//      LPPUB lpPub
//      LPSTR lpFile
//      
//
//  Return: (LPFAMILY)
//
//
//  Comments:
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

LPFAMILY WINAPI LoadFamily( LPPUB lpPub, LPSTR lpFile )
{
    HFILE    hFile = NULL;
    LPFAMILY lpFamily = CreateFamily( lpPub );
    WORD     wFaces, wTStyles=0, wLen, wErr = LE_OK;
    BOOL     fThumbs;

    if (!lpFamily)
    {
        gwLoadError = IDS_MEMALLOC;
        return NULL;
    }

    if ((lpFamily->fiFF = ShrinkFilename( lpFile )) == -1 )
        FERROR(IDS_DBCORRUPT);

    if ((lpFamily->wFileVersion = GetFileVersion( lpFile )) == -1 )
        FERROR(IDS_FILENOTFOUND);

    if ((hFile = OpenFile( lpFile, &gof, OF_READ )) == HFILE_ERROR )
    {
        FERROR(IDS_FILEOPENFAILED);
    }
    else
    {
        DWORD dwID = 0L;

        wLen = _lread( hFile, (LPSTR)&dwID, sizeof(DWORD) );
        if ( dwID!=FSF_ID )
        {
            if (wLen==HFILE_ERROR)
            {
                FERROR(IDS_FILEREADFAILED);
            }
            else
            {
                FERROR(IDS_DBCORRUPT);
            }
        }
    }
    wLen = _lread( hFile, (LPSTR)&(lpFamily->Info), sizeof(FSF) );
    if (wLen!=sizeof(FSF))
    {
        if (wLen==HFILE_ERROR)
        {
            FERROR(IDS_FILEREADFAILED);
        }
        else
        {
            FERROR(IDS_DBCORRUPT);
        }
    }

    //*** Check to see if the thumbnails are present
    fThumbs = AreThumbsPresent( lpFamily, FDB_TYPE_FS );

    for (wFaces=0; wFaces<FACES(lpFamily); wFaces++)
    {
        LPFACE lpFace = LoadFace( lpFamily, hFile );

        if (!lpFace)
            FERROR(gwLoadError);

        if (fThumbs)
            VI_BMPFOUND( lpFace );
        else
            VI_BMPNOTFOUND( lpFace );

        wTStyles += STYLES(lpFace);
    }
    if (wTStyles!=STYLES(lpFamily))
        FERROR( IDS_DBCORRUPT );
    wErr = LE_OK;

fileIO_error:
    if (hFile)
        _lclose( hFile );

    if (wErr==IDS_DBCORRUPT)
    {
        lstrcpy( szOld, lpFile );
        lstrcpy( szNew, lpFile );
        SetFileVersion( szNew, 0 );
        OpenFile( szNew, &gof, OF_DELETE );
        if( rename( szOld, szNew ) )
            wErr = IDS_RENAMEFAILURE;
    }

    if (wErr!=LE_OK)
    {
        FreeFamily( lpFamily );
        lpFamily = NULL;
    }
    gwLoadError = wErr;
    return lpFamily;

} //*** LoadFamily


//*************************************************************
//
//  CreateCollection
//
//  Purpose:
//      Creates a collection node in the publisher
//
//
//  Parameters:
//      LPPUB lpPub
//      
//
//  Return: (LPCOLL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/22/92   MSM        Created
//
//*************************************************************

LPCOLL WINAPI CreateCollection( LPPUB lpPub )
{
    LPCOLL lpColl = (LPCOLL)fsmmListAllocNode(lpPub->hlstCollections,
        NULL,sizeof(COLL));

    if (lpColl)
    {
        lpColl->dbn.wNodeType = NT_COLLECTION;
        lpColl->dbn.lpParent = (LPDBNODE)lpPub;
        lpColl->dbn.wViewInfo = VIB_DROPSHIP;

        fsmmListInsertNode(lpPub->hlstCollections,(LPNODE)lpColl,
            LIST_INSERTTAIL);
    }
    return lpColl;

} //*** CreateCollection


//*************************************************************
//
//  FreeCollection
//
//  Purpose:
//      Frees the collection from the Collection
//
//
//  Parameters:
//      LPCOLL lpColl
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/22/92   MSM        Created
//
//*************************************************************

VOID WINAPI FreeCollection( LPCOLL lpColl )
{
    LPPUB lpPub = PARENT( LPPUB, lpColl );

    if (lpColl->lpszText)
        FREE_STRING( lpPub, lpColl->lpszText );

    fsmmListDeleteNode(lpPub->hlstCollections, (LPNODE)lpColl);

} //*** FreeCollection


//*************************************************************
//
//  LoadPublisherCollections
//
//  Purpose:
//      Loads the publisher collections
//
//
//  Parameters:
//      LPPUB lpPub
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/21/92   MSM        Created
//
//*************************************************************

BOOL WINAPI LoadPublisherCollections( LPPUB lpPub )
{
    LPCOLL lpColl;
    LPDB   lpDB = PARENT(LPDB,lpPub);
    HFILE  hFile;
    DWORD  dwOff;
    WORD   wLen, wI, wErr;

    //*** Already loaded
    if (GetFirstCollection( lpPub ))
        return TRUE;

    ExpandFilename(lpDB, FDB_TYPE_FSPUB, lpPub->fiFSPub, 0, gof.szPathName);
    SetFileVersion( gof.szPathName, lpPub->wFileVersion );

    hFile = OpenFile( gof.szPathName, &gof, OF_READ );

    if (hFile == HFILE_ERROR)
    {
        gwLoadError = IDS_FILEOPENFAILED;
        return NULL;
    }

    dwOff = sizeof(DWORD) + sizeof(FSP) + lpPub->PubInfo.wNameSize +
            lpPub->PubInfo.wSCopyrightSize + lpPub->PubInfo.wLCopyrightSize;

    //*** Move to array of collections
    _llseek( hFile, dwOff, 0 );


    for (wI=1; wI<=lpPub->PubInfo.wCollections; wI++)
    {
        if (!(lpColl = CreateCollection( lpPub )))
            FERROR( IDS_MEMALLOC );

        wLen = _lread(hFile, &lpColl->Info, sizeof( COLLINFO ) );
        if (wLen!=sizeof(COLLINFO))
        {
            if (wLen==HFILE_ERROR)
            {
                FERROR(IDS_FILEREADFAILED);
            }
            else
            {
                FERROR(IDS_DBCORRUPT);
            }
        }
        lpColl->wShipping = (WORD)-1;
    }
    _lclose( hFile );
    return TRUE;

fileIO_error:
    _lclose( hFile );
    FreePublisherCollections( lpPub );
    return FALSE;

} //*** LoadPublisherCollections


//*************************************************************
//
//  FreePublisherCollections
//
//  Purpose:
//      Frees all the publishers collections
//
//
//  Parameters:
//      LPPUB lpPub
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/28/92   MSM        Created
//
//*************************************************************

VOID WINAPI FreePublisherCollections( LPPUB lpPub )
{
    LPCOLL lpColl = GetFirstCollection( lpPub );

    while (lpColl)
    {
        LPCOLL lpC = GetNextCollection( lpColl );
        FreeCollection( lpColl );
        lpColl = lpC;
    }
} //*** FreePublisherCollections


//*************************************************************
//
//  LoadPublisherCollectionsText
//
//  Purpose:
//      Loads in the text associated with each collection
//
//
//  Parameters:
//      LPPUB lpPub
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/27/92   MSM        Created
//
//*************************************************************

BOOL WINAPI LoadPublisherCollectionsText( LPPUB lpPub )
{
    LPCOLL lpColl = GetFirstCollection( lpPub );
    LPDB   lpDB = PARENT(LPDB,lpPub);
    HFILE  hFile;
    DWORD  dwOff;
    WORD   wLen, wErr;

    if (!lpColl)
        return FALSE;

    FreePublisherCollectionsText( lpPub );

    ExpandFilename(lpDB, FDB_TYPE_FSPUB, lpPub->fiFSPub, 0, gof.szPathName);
    SetFileVersion( gof.szPathName, lpPub->wFileVersion );

    hFile = OpenFile( gof.szPathName, &gof, OF_READ );

    if (hFile == HFILE_ERROR)
    {
        gwLoadError = IDS_FILEOPENFAILED;
        return FALSE;
    }

    dwOff = sizeof(DWORD) + sizeof(FSP) + lpPub->PubInfo.wNameSize +
        lpPub->PubInfo.wSCopyrightSize + lpPub->PubInfo.wLCopyrightSize +
        (lpPub->PubInfo.wCollections*sizeof(COLLINFO));

    //*** Move to array of collection text
    _llseek( hFile, dwOff, 0 );

    while (lpColl)
    {
        lpColl->lpszText = ALLOCFROMHEAP( lpPub, lpColl->Info.wTextSize );
        if (!lpColl->lpszText)
            FERROR(IDS_MEMALLOC);
            
        wLen = _lread(hFile, lpColl->lpszText, lpColl->Info.wTextSize );
        if (wLen!=lpColl->Info.wTextSize)
        {
            if (wLen==HFILE_ERROR)
            {
                FERROR(IDS_FILEREADFAILED);
            }
            else
            {
                FERROR(IDS_DBCORRUPT);
            }
        }
        lpColl = GetNextCollection( lpColl );
    }
    _lclose( hFile );
    return TRUE;

fileIO_error:
    _lclose( hFile );
    FreePublisherCollectionsText( lpPub );
    return FALSE;

} //*** LoadPublisherCollectionsText


//*************************************************************
//
//  FreePublisherCollectionsText
//
//  Purpose:
//      Frees the info associated with publisher collections
//
//
//  Parameters:
//      LPPUB lpPub
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/19/92   MSM        Created
//
//*************************************************************

VOID WINAPI FreePublisherCollectionsText( LPPUB lpPub )
{
    LPCOLL lpColl = GetFirstCollection( lpPub );

    while (lpColl)
    {
        if (lpColl->lpszText)
            FREE_STRING( lpPub, lpColl->lpszText );
        lpColl->lpszText = NULL;

        lpColl = GetNextCollection( lpColl );
    }

} //*** FreePublisherCollectionsText


//*************************************************************
//
//  CreatePublisher
//
//  Purpose:
//      Creates a publisher and adds it to the DB list
//
//
//  Parameters:
//      LPDB lpDB
//      
//
//  Return: (LPPUB)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/21/92   MSM        Created
//
//*************************************************************

LPPUB WINAPI CreatePublisher( LPDB lpDB )
{
    LPPUB lpPub = (LPPUB)ALLOC_LIST_NODE( lpDB, sizeof(PUB) );

    if (lpPub)
    {

        lpPub->hlstList = fsmmListCreate( lpDB->hhHeap );

        if (!lpPub->hlstList)
            return NULL;

        lpPub->hlstCollections = fsmmListCreate( lpDB->hhHeap );

        if (!lpPub->hlstCollections)
        {
            fsmmListDestroy( lpPub->hlstList );
            return NULL;
        }

        lpPub->dbn.wNodeType = NT_PUB;
        lpPub->dbn.lpParent = (LPDBNODE)lpDB;

        ADD_LIST_NODE( lpDB, lpPub );
    }
    return lpPub;

} //*** CreatePublisher


//*************************************************************
//
//  FreePublisher
//
//  Purpose:
//      Frees a family and all its children
//
//
//  Parameters:
//      LPPUB lpPublisher
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

VOID WINAPI FreePublisher( LPPUB lpPub )
{
    LPFAMILY lpFamily = GetFirstFamily( lpPub, NULL  );
    LPCOLL lpColl = GetFirstCollection( lpPub );

    // Free children
    while (lpFamily)
    {
        FreeFamily( lpFamily );
        lpFamily = GetFirstFamily( lpPub, NULL  );
    }

    // Free strings
    if (lpPub->lpszPublisher)
        FREE_STRING( lpPub, lpPub->lpszPublisher );

    if (lpPub->lpszSCopyright)
        FREE_STRING(lpPub, lpPub->lpszSCopyright);

    FreePublisherLCopyright( lpPub );
    FreePublisherCollections( lpPub );

    // Free child list
    fsmmListDestroy( lpPub->hlstList );
    fsmmListDestroy( lpPub->hlstCollections );

    // Free actual node
    DELETE_LIST_NODE( PARENT(LPDB,lpPub), lpPub );

} //*** FreePublisher


//*************************************************************
//
//  LoadPublisher
//
//  Purpose:
//      Loads a publisher and all its families
//
//
//  Parameters:
//      LPDB  lpDB
//      LPSTR lpFile
//      HWND  hStatus      
//
//  Return: (LPPUB)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

LPPUB WINAPI LoadPublisher( LPDB lpDB, LPSTR lpFile, HWND hStatus )
{
    HFILE    hFile = NULL;
    LPPUB    lpPub = CreatePublisher( lpDB );
    WORD     wLen, wErr = LE_OK;
    LPSTR    lp, lpBuff;
    int      fDone = 0;

    if (!lpPub)
    {
        gwLoadError = IDS_MEMALLOC;
        return NULL;
    }

    if ((lpPub->fiFSPub = ShrinkFilename( lpFile )) == -1 )
        FERROR(IDS_DBCORRUPT);

    if ((lpPub->wFileVersion = GetFileVersion( lpFile )) == -1 )
        FERROR(IDS_DBCORRUPT);

    if ((hFile = OpenFile( lpFile, &gof, OF_READ )) == HFILE_ERROR )
    {
        FERROR(IDS_FILEOPENFAILED);
    }
    else
    {
        DWORD dwID = 0L;

        wLen = _lread( hFile, (LPSTR)&dwID, sizeof(DWORD) );
        if ( dwID!=FSPUB_ID )
        {
            if (wLen==HFILE_ERROR)
            {
                FERROR(IDS_FILEREADFAILED);
            }
            else
            {
                FERROR(IDS_DBCORRUPT);
            }
        }
    }
    wLen = _lread( hFile, (LPSTR)&(lpPub->PubInfo), sizeof(FSP) );
    if (wLen!=sizeof(FSP))
    {
        if (wLen==HFILE_ERROR)
        {
            FERROR(IDS_FILEREADFAILED);
        }
        else
        {
            FERROR(IDS_DBCORRUPT);
        }
    }

    lpPub->lpszPublisher = ALLOCFROMHEAP( lpPub, lpPub->PubInfo.wNameSize );

    if (!lpPub->lpszPublisher)
        FERROR( IDS_MEMALLOC );

    wLen = _lread( hFile, lpPub->lpszPublisher, lpPub->PubInfo.wNameSize );
    if ( wLen!=lpPub->PubInfo.wNameSize )
    {
        if (wLen==HFILE_ERROR)
        {
            FERROR(IDS_FILEREADFAILED);
        }
        else
        {
            FERROR(IDS_DBCORRUPT);
        }
    }

    if (IsWindow(hStatus))
    {
        char szTemp[256];

        wsprintf( szTemp, "Loading %s", lpPub->lpszPublisher );
        SetWindowText( hStatus, szTemp );
        UpdateWindow( hStatus );
    }

    lpPub->lpszSCopyright = ALLOCFROMHEAP( lpPub, lpPub->PubInfo.wSCopyrightSize );

    if (!lpPub->lpszSCopyright)
        FERROR( IDS_MEMALLOC );

    wLen = _lread( hFile, lpPub->lpszSCopyright, lpPub->PubInfo.wSCopyrightSize );
    if ( wLen!=lpPub->PubInfo.wSCopyrightSize )
    {
        if (wLen==HFILE_ERROR)
        {
            FERROR(IDS_FILEREADFAILED);
        }
        else
        {
            FERROR(IDS_DBCORRUPT);
        }
    }

    _lclose( hFile );
    hFile = NULL;

    // Now that the header and name are loaded, we can enumerate all
    // the families and load them.

    lpBuff = (LPSTR)gof.szPathName;

    lstrcpy( lpBuff, lpDB->szPathName );
    lp = lpBuff + lstrlen( lpBuff );
    wsprintf( lp, "FF%03X*.*", lpPub->fiFSPub&0x0FFF );

    // Check for the first family
    fDone = _dos_findfirst( gof.szPathName, _A_NORMAL, &gff3 );

    while (!fDone)
    {
        lstrcpy( lpBuff, lpDB->szPathName );
        lstrcat( lpBuff, gff3.name );

        if (GetFileVersion( lpBuff ))
        {
            if ( LoadFamily(lpPub, lpBuff) )
                lpPub->wFamilies++;
            else
                FERROR( gwLoadError );
        }
        else
            lpDB->wDBType = DB_TYPE_CORRUPT;
        fDone = _dos_findnext( &gff3 );
    }
    if (lpPub->wFamilies==0)
        FERROR( IDS_DBCORRUPT );

    wErr = LE_OK;

fileIO_error:
    if (hFile)
        _lclose( hFile );

    if (wErr==IDS_DBCORRUPT)
    {
        lstrcpy( szOld, lpFile );
        lstrcpy( szNew, lpFile );
        SetFileVersion( szNew, 0 );
        OpenFile( szNew, &gof, OF_DELETE );
        if( rename( szOld, szNew ) )
            wErr = IDS_RENAMEFAILURE;
    }

    if (wErr!=LE_OK)
    {
        FreePublisher( lpPub );
        lpPub = NULL;
    }
    gwLoadError = wErr;
    return lpPub;

} //*** LoadPublisher


//*************************************************************
//
//  LoadPublisherLCopyright
//
//  Purpose:
//      Loads in the large copyright notice for a publisher
//
//
//  Parameters:
//      LPPUB lpPub
//      
//
//  Return: (LPSTR)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

LPSTR WINAPI LoadPublisherLCopyright( LPPUB lpPub )
{
    LPDB lpDB = PARENT(LPDB,lpPub);
    HFILE hFile;
    DWORD dwOff;
    WORD  wLen, wReadLen;

    //*** Use the default    
    if (lpPub->PubInfo.wLCopyrightSize==0L)
    {
        HRSRC hRSrc = FindResource( ghInstLib, "COPYRIGHT", "FILES" );

        if (!hRSrc)
        {
            gwLoadError = 0;
            return NULL;
        }

        wReadLen = (WORD)SizeofResource( ghInstLib, hRSrc );
        hFile = (HFILE)AccessResource( ghInstLib, hRSrc );
    }
    else
    {

        ExpandFilename(lpDB, FDB_TYPE_FSPUB, lpPub->fiFSPub, 0, gof.szPathName);
        SetFileVersion( gof.szPathName, lpPub->wFileVersion );

        hFile = OpenFile( gof.szPathName, &gof, OF_READ );

        if (!hFile)
        {
            gwLoadError = IDS_FILEOPENFAILED;
            return NULL;
        }

        dwOff = sizeof(DWORD) +sizeof(FSP) + lpPub->PubInfo.wNameSize + 
                lpPub->PubInfo.wSCopyrightSize;

        _llseek( hFile, dwOff, 0 );
        wReadLen = lpPub->PubInfo.wLCopyrightSize - 1;
    }

    lpPub->lpszLCopyright = ALLOCFROMHEAP( lpPub, wReadLen+1 );

    if (!lpPub->lpszLCopyright)
    {
        _lclose( hFile );
        gwLoadError = IDS_MEMALLOC;
        return NULL;
    }

    wLen = _lread( hFile, lpPub->lpszLCopyright, wReadLen );
    if ( wLen!=wReadLen )
    {
        _lclose( hFile );
        FREE_STRING(lpPub, lpPub->lpszLCopyright);
    
        if (wLen==HFILE_ERROR)
            gwLoadError =IDS_FILEREADFAILED;
        else
            gwLoadError =IDS_DBCORRUPT;
    
        return NULL;
    }
    lpPub->lpszLCopyright[ wLen ] = '\0';

    _lclose( hFile );
    return lpPub->lpszLCopyright;

} //*** LoadPublisherLCopyright


//*************************************************************
//
//  FreePublisherLCopyright
//
//  Purpose:
//      Frees up the memory associated with the large copyright
//
//
//  Parameters:
//      LPPUB lpPub
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

VOID WINAPI FreePublisherLCopyright( LPPUB lpPub )
{
    if (lpPub->lpszLCopyright)
        FREE_STRING(lpPub, lpPub->lpszLCopyright);

    lpPub->lpszLCopyright = NULL;

} //*** FreePublisherLCopyright


//*************************************************************
//
//  CreateDataBase
//
//  Purpose:
//      Creates and empty database
//
//
//  Parameters:
//      VOID
//      
//
//  Return: (LPDB)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

LPDB WINAPI CreateDataBase( VOID )
{
    LPDB  lpDB;
    HHEAP hhHeap = fsmmHeapCreate( 2048, HT_MULTIPLE );
    // First create a multiple far local heap

    if (!hhHeap)
        return NULL;

    lpDB = (LPDB)fsmmHeapAlloc( hhHeap, LPTR, sizeof(DB) );
    if (lpDB)
    {
        lpDB->hlstList = fsmmListCreate( hhHeap );

        if (!lpDB->hlstList)
        {
            fsmmHeapDestroy( hhHeap );
            return NULL;
        }
        lpDB->hhHeap = hhHeap;
        lpDB->dbn.wNodeType = NT_DB;
    }
    return lpDB;

} //*** CreateDataBase


//*************************************************************
//
//  FreeDataBase
//
//  Purpose:
//      Frees the database and all its contents
//
//
//  Parameters:
//      LPDB lpDB
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

VOID WINAPI FreeDataBase( LPDB lpDB )
{
    LPPUB lpPub = GetFirstPublisher( lpDB, NULL  );

    // Free children
    while (lpPub)
    {
        FreePublisher( lpPub );
        lpPub = GetFirstPublisher( lpDB, NULL  );
    }

    // Kill entire heap
    fsmmHeapDestroy( lpDB->hhHeap );

} //*** FreeDataBase


//*************************************************************
//
//  EmptyDataBase
//
//  Purpose:
//      Emptys the database, but not the LPDB node
//
//
//  Parameters:
//      LPDB lpDB
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

VOID WINAPI EmptyDataBase( LPDB lpDB )
{
    LPPUB lpPub = GetFirstPublisher( lpDB, NULL  );

    // Free children
    while (lpPub)
    {
        FreePublisher( lpPub );
        lpPub = GetFirstPublisher( lpDB, NULL  );
    }

} //*** EmptyDataBase


//*************************************************************
//
//  LoadDataBase
//
//  Purpose:
//      Loads the database
//
//
//  Parameters:
//      LPSTR lpPath
//      
//
//  Return: (LPDB)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

LPDB WINAPI LoadDataBase( LPSTR lpPath, HWND hStatus )
{
    HFILE    hFile = NULL;
    LPDB     lpDB = CreateDataBase();
    WORD     wErr = LE_OK;
    LPSTR    lp, lpBuff;
    int      fDone = 0;

    if (!lpDB)
    {
        gwLoadError = IDS_MEMALLOC;
        return NULL;
    }

    lstrcpy( lpDB->szPathName, lpPath );

    // Now that the header is allocated, we can enumerate all
    // the publishers and load them.

    lpBuff = (LPSTR)gof.szPathName;

    lstrcpy( lpBuff, lpDB->szPathName );
    lp = lpBuff + lstrlen( lpBuff );
    wsprintf( lp, "FSPUB*.*" );

    // Check for the first publisher
    fDone = _dos_findfirst( gof.szPathName, _A_NORMAL, &gff );

    while (!fDone)
    {
        lstrcpy( lpBuff, lpDB->szPathName );
        lstrcat( lpBuff, gff.name );

        if (GetFileVersion( lpBuff))
        {
            if ( !LoadPublisher(lpDB, lpBuff,hStatus) )
                FERROR( gwLoadError );
        }
        else
            lpDB->wDBType = DB_TYPE_CORRUPT;

        fDone = _dos_findnext( &gff );
    }
    if ( fsmmListItems(lpDB->hlstList)==0)
        FERROR( IDS_DBNOTFOUND );

    wErr = LE_OK;

fileIO_error:
    if (hFile && hFile != HFILE_ERROR)
        _lclose( hFile );

    if (wErr!=LE_OK)
    {
        FreeDataBase( lpDB );
        lpDB = NULL;
    }
    else
   {


#define OFFSETIN(struc,field)    ((int)&(((struc *)0)->field))

    //*** Sort publishers
    SortList(lpDB->hlstList, -1 * OFFSETIN(PUB,lpszPublisher));

   }
    gwLoadError = wErr;

    return lpDB;

} //*** LoadDataBase


//*************************************************************
//
//  GetLoadBitmapError
//
//  Purpose:
//      Returns the last error when loading a bitmap
//
//
//  Parameters:
//      
//      
//
//  Return: (WORD)
//      #define LBE_OK              0x0000
//      #define LBE_FILENOTFOUND    0x0001
//      #define LBE_LOADLIBRARY     0x0002
//      #define LBE_LOADBITMAP      0x0003
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

WORD WINAPI GetLoadBitmapError( )
{
    return gwBitmapError;

} //*** GetLoadBitmapError


//*************************************************************
//
//  GetLoadError
//
//  Purpose:
//      Returns the last error when loading a DB file
//
//
//  Parameters:
//      
//      
//
//  Return: (WORD)
//      returns LE_OK or a IDS_???? error value
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

WORD WINAPI GetLoadError( )
{
    return gwLoadError;

} //*** GetLoadError


//*************************************************************
//
//  ReportFontshopError
//
//  Purpose:
//      Same as message box with the optional string load and tech number
//
//
//  Parameters:
//      HWND hParent
//      LPSTR lpText    - can be a string resource
//      LPSTR lpTitle
//      UINT  uiFlags   - can include MB_TECHNUMBER
//      UINT  uiError   - if !0 then added at the end (MB_TECHNUMBER required!)
//      
//
//  Return: (WORD WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/25/92   MSM        Created
//
//*************************************************************

WORD WINAPI ReportFontshopError( HWND hParent, LPSTR lpText, LPSTR lpTitle, UINT uiFlags, UINT uiError )
{
    char szText[512];
    LPSTR lp;

    szText[0] = 0;

    if (HIWORD( (LONG)lpText )==0)
        LoadString( ghInstLib, LOWORD((LONG)lpText), szText, 256 );
    else
        lstrcpy( szText, lpText );

    lp = (LPSTR)szText + lstrlen(szText);

    if (uiFlags & MB_TECHNUMBER)
    {
        uiFlags &= ~MB_TECHNUMBER;

        LoadString( ghInstLib, IDS_TECHSUPPORT, lp, 60 );
        lp += lstrlen(lp);

        if (uiError)
            wsprintf( lp, " (Error #%u)", uiError );
    }

    return MessageBox( hParent, szText, lpTitle, uiFlags );

} //*** ReportFontshopError

//*** EOF: db.c
