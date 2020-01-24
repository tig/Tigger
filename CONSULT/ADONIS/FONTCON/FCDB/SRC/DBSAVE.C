//*************************************************************
//  File name: dbsave.c
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

//*** Some file specific macros, ASSUMES local variable wErr and fileIO_error!
#define FERROR(X)   {wErr=X; goto fileIO_error;}

//*************************************************************
//
//  SaveFace
//
//  Purpose:
//      Saves a face and its styles to the file
//
//
//  Parameters:
//      LPFACE lpFace
//      HFILE hFile
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

BOOL WINAPI SaveFace( LPFACE lpFace, HFILE hFile )
{
    WORD    wStyles=0, wLen, wErr = 0;    
    LPSTYLE lpStyle = GetFirstStyle( lpFace, NULL );

    STYLES(lpFace) = (WORD)fsmmListItems( GETHLIST(lpFace) );

    // Write face header
    lpFace->Info.dwPrice ^= PRICE_ENCODE;
    wLen = _lwrite( hFile, (LPSTR)&(lpFace->Info), sizeof(FACEINFO) );
    lpFace->Info.dwPrice ^= PRICE_ENCODE;

    if (wLen!=sizeof(FACEINFO))
        FERROR(IDS_FILEWRITEFAILED);

    // Write styles
    while (lpStyle)
    {
        wStyles++;

        lpStyle->Info.dwPrice ^= PRICE_ENCODE;
        wLen = _lwrite( hFile, (LPSTR)&(lpStyle->Info), sizeof(STYLEINFO) );
        lpStyle->Info.dwPrice ^= PRICE_ENCODE;

        if (wLen!=sizeof(STYLEINFO))
            FERROR(IDS_FILEWRITEFAILED);
        lpStyle = GetNextStyle( lpStyle, NULL );
    }

    // Sanity check
    if (wStyles != STYLES(lpFace))
        FERROR( IDS_DBCORRUPT );

    wErr = LE_OK;

fileIO_error:
    gwLoadError = wErr;

    return (wErr==LE_OK);

} //*** SaveFace


//*************************************************************
//
//  SaveFamily
//
//  Purpose:
//      Saves a family to its file
//
//
//  Parameters:
//      LPFAMILY lpFamily
//      WORD wFlags
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

BOOL WINAPI SaveFamily( LPFAMILY lpFamily, WORD wFlags )
{
    OFSTRUCT of;
    HFILE    hFile = NULL;
    WORD     wFaces=0, wLen, wErr = LE_OK;
    LPPUB    lpPub = PARENT(LPPUB,lpFamily);
    LPDB     lpDB  = PARENT(LPDB,lpPub);
    LPFACE   lpFace= GetFirstFace( lpFamily, NULL );

    FACES(lpFamily) = (WORD)fsmmListItems( GETHLIST(lpFamily) );
    STYLES(lpFamily) = 0;

    while (lpFace)
    {
        STYLES(lpFamily) += STYLES(lpFace);
        lpFace = GetNextFace( lpFace, NULL );
    }

    lpFace= GetFirstFace( lpFamily, NULL );

    ExpandFilename( lpDB, FDB_TYPE_FF|FDB_TYPE_DIR, lpPub->fiFSPub, 
        lpFamily->fiFF,  (LPSTR)of.szPathName );

    SetFileVersion( (LPSTR)of.szPathName, 1 );

    if ((hFile=OpenFile(of.szPathName,&of,OF_WRITE|OF_CREATE))==HFILE_ERROR)
    {
        FERROR(IDS_FILEOPENFAILED);
    }
    else
    {
        DWORD dwID = FSF_ID;

        // Write file ID out
        if (_lwrite( hFile, (LPSTR)&dwID, sizeof(DWORD) )!=sizeof(DWORD))
            FERROR(IDS_FILEWRITEFAILED);
    }
    // Write family header out
    wLen = _lwrite( hFile, (LPSTR)&(lpFamily->Info), sizeof(FSF) );
    if (wLen!=sizeof(FSF))
        FERROR(IDS_FILEWRITEFAILED);

    // Write faces out
    while (lpFace)
    {
        wFaces++;
        // This function just cracks me up when you read it out loud
        if (!SaveFace( lpFace, hFile ))
            FERROR(gwLoadError);
        lpFace = GetNextFace( lpFace, NULL );
    }

    // Sanity check
    if (wFaces != FACES(lpFamily))
        FERROR( IDS_DBCORRUPT );
    wErr = LE_OK;

fileIO_error:
    if (hFile && hFile != -1)
        _lclose( hFile );

    if (wErr!=LE_OK)
    {
        // delete the errored file
        OpenFile( (LPSTR)of.szPathName, &of, OF_DELETE );
    }

    gwLoadError = wErr;
    return (wErr==LE_OK);

} //*** SaveFamily


//*************************************************************
//
//  SavePublisher
//
//  Purpose:
//      Saves the publisher file out to disk
//
//
//  Parameters:
//      LPPUB lpPub
//      WORD wFlags
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

BOOL WINAPI SavePublisher( LPPUB lpPub, WORD wFlags)
{
    OFSTRUCT of;
    HFILE    hFile = NULL;
    WORD     wFamilies=0, wLen, wErr = LE_OK;
    LPDB     lpDB  = PARENT(LPDB,lpPub);
    LPFAMILY lpFamily= GetFirstFamily( lpPub, NULL );

    lpPub->wFamilies = (WORD)fsmmListItems( GETHLIST(lpPub) );

    ExpandFilename( lpDB, FDB_TYPE_FSPUB|FDB_TYPE_DIR, lpPub->fiFSPub, 0,
        (LPSTR)of.szPathName );

    SetFileVersion( (LPSTR)of.szPathName, 1 );

    if ((hFile=OpenFile(of.szPathName,&of,OF_WRITE|OF_CREATE))==HFILE_ERROR)
    {
        FERROR(IDS_FILEOPENFAILED);
    }
    else
    {
        DWORD dwID = FSPUB_ID;

        // Write file ID out
        if (_lwrite( hFile, (LPSTR)&dwID, sizeof(DWORD) )!=sizeof(DWORD))
            FERROR(IDS_FILEWRITEFAILED);
    }
    // Write publisher header out
    wLen = _lwrite( hFile, (LPSTR)&(lpPub->PubInfo), sizeof(FSP) );
    if (wLen!=sizeof(FSP))
        FERROR(IDS_FILEWRITEFAILED);

    // Write strings out
    wLen = _lwrite(hFile, lpPub->lpszPublisher, lpPub->PubInfo.wNameSize);
    if (wLen!=lpPub->PubInfo.wNameSize)
        FERROR(IDS_FILEWRITEFAILED);

    if (lpPub->PubInfo.wSCopyrightSize)
    {
        wLen = _lwrite(hFile, lpPub->lpszSCopyright, 
                        lpPub->PubInfo.wSCopyrightSize);
        if (wLen!=lpPub->PubInfo.wSCopyrightSize)
            FERROR(IDS_FILEWRITEFAILED);
    }

    if (lpPub->PubInfo.wLCopyrightSize)
    {
        wLen = _lwrite(hFile, lpPub->lpszLCopyright, 
                        lpPub->PubInfo.wLCopyrightSize);
        if (wLen!=lpPub->PubInfo.wLCopyrightSize)
            FERROR(IDS_FILEWRITEFAILED);
    }

    if (lpPub->PubInfo.wCollections)
    {
        LPCOLL lpColl = GetFirstCollection( lpPub );

        while (lpColl)
        {
            wLen = _lwrite(hFile, (LPSTR)&lpColl->Info, sizeof( COLLINFO ));
            if (wLen!=sizeof(COLLINFO))
                FERROR(IDS_FILEWRITEFAILED);
            lpColl = GetNextCollection( lpColl );
        }

        lpColl = GetFirstCollection( lpPub );

        while (lpColl)
        {
            wLen = _lwrite(hFile, lpColl->lpszText, lpColl->Info.wTextSize );
            if (wLen!=lpColl->Info.wTextSize)
                FERROR(IDS_FILEWRITEFAILED);

            lpColl = GetNextCollection( lpColl );
        }
    }
    _lclose( hFile );
    hFile = NULL;

    while (lpFamily)
    {
        if (!SaveFamily( lpFamily, wFlags ))
            FERROR( gwLoadError );

        wFamilies++;
        lpFamily = GetNextFamily( lpFamily, NULL );
    }

    if (wFamilies != lpPub->wFamilies)
        FERROR( IDS_DBCORRUPT );
    wErr = LE_OK;

fileIO_error:
    if (hFile && hFile != -1)
        _lclose( hFile );

    if (wErr!=LE_OK)
    {
        // delete the errored file
        OpenFile( (LPSTR)of.szPathName, &of, OF_DELETE );
    }

    gwLoadError = wErr;
    return (wErr==LE_OK);

} //*** SavePublisher


//*************************************************************
//
//  SaveDataBase
//
//  Purpose:
//      Saves the database to disk
//
//
//  Parameters:
//      LPDB lpDB
//      WORD wFlags
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

BOOL WINAPI SaveDataBase( LPDB lpDB, WORD wFlags )
{
    WORD     wErr = LE_OK;
    LPPUB    lpPub = GetFirstPublisher( lpDB, NULL );

    while (lpPub)
    {
        if (!SavePublisher( lpPub, wFlags ))
            FERROR( gwLoadError );
        lpPub = GetNextPublisher( lpPub, NULL );
    }
    wErr = LE_OK;

fileIO_error:

    gwLoadError = wErr;
    return (wErr==LE_OK);

} //*** SaveDataBase

//*** EOF: dbsave.c
