//*************************************************************
//  File name: dbbmp.c
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

HBITMAP NEAR PASCAL ConvertRLEtoBMP1( HBITMAP hBmp );
OFSTRUCT      gof2;
struct find_t gff2;

//*************************************************************
//
//  LoadFaceBitmap
//
//  Purpose:
//      Loads in the face bitmap (all styles of the face)
//
//
//  Parameters:
//      LPFACE lpFace
//      
//
//  Return: (HBITMAP)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//                
//*************************************************************

HBITMAP WINAPI LoadFaceBitmap( LPFACE lpFace )
{
    HINSTANCE hInstBmp;
    HBITMAP   hBmp;
    LPFAMILY  lpFam = PARENT(LPFAMILY,lpFace);
    LPPUB     lpPub = PARENT(LPPUB,lpFam);
    BITMAP    Bmp;

    //*** File was not found
    if (VI_ISBMPNOTFOUND(lpFace))
    {
        gwBitmapError = LBE_FILENOTFOUND;
        return NULL;
    }

    //*** Double check existance and then load.  Double checking seems
    //*** like a waste of time, but the user will be baffled if presented
    //*** with a 'Insert FS1230001.001 into Drive A:' MsgBox.

    SetCursor( LoadCursor(NULL, IDC_WAIT) );

    //*** AreThumbsPresent will load gof.szPathName!!
    if (!AreThumbsPresent( lpFam, FDB_TYPE_FS ))
    {
        VI_BMPNOTFOUND(lpFace);
        gwBitmapError = LBE_FILENOTFOUND;
        SetCursor( LoadCursor(NULL, IDC_ARROW) );
        return NULL;
    }

    //*** File was found, so load it
    hInstBmp = LoadLibrary( gof.szPathName );

    if (!hInstBmp)
    {
        gwBitmapError = LBE_LOADLIBRARY;
        SetCursor( LoadCursor(NULL, IDC_ARROW) );
        return NULL;
    }

    hBmp = LoadBitmap( hInstBmp, MAKEINTRESOURCE((ITEM_INDEX(lpFam,lpFace)+1)) );

    if (!hBmp)
    {
        FreeLibrary( hInstBmp );
        gwBitmapError = LBE_LOADBITMAP;
        SetCursor( LoadCursor(NULL, IDC_ARROW) );
        return NULL;
    }

    //*** Convert to monchrome BMP
    hBmp = ConvertRLEtoBMP1( hBmp );

    GetObject( hBmp, sizeof(BITMAP), (LPSTR)&Bmp );

    lpFace->wBmpWidth = Bmp.bmWidth;
    lpFace->wBmpHeight = Bmp.bmHeight / lpFace->Info.wStyles;

    FreeLibrary( hInstBmp );
    SetCursor( LoadCursor(NULL, IDC_ARROW) );
    return hBmp;

} //*** LoadFaceBitmap


//*************************************************************
//
//  LoadFamilyBitmap
//
//  Purpose:
//      Loads the family bitmap (BIG THUMB for family)
//
//
//  Parameters:
//      LPFAMILY lpFamily
//      
//
//  Return: (HBITMAP)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/22/92   MSM        Created
//
//*************************************************************

HBITMAP WINAPI LoadFamilyBitmap( LPFAMILY lpFamily )
{
    gwBitmapError = LBE_FILENOTFOUND;
    return NULL;

} //*** LoadFamilyBitmap


//*************************************************************
//
//  AreThumbsPresent
//
//  Purpose:
//      Checks to see if a thumb nail is on the system
//
//
//  Parameters:
//      LPFAMILY lpFam
//      WORD wType      - FDB_TYPE_????
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//      Other routines expect that this will load gof.szPathName with
//      the filename if it does exist!!!!!!!!!!!!!!!!
//
//  History:    Date       Author     Comment
//               3/ 8/92   MSM        Created
//
//*************************************************************

BOOL WINAPI AreThumbsPresent( LPFAMILY lpFam, WORD wType )
{
    LPPUB lpPub = PARENT(LPPUB,lpFam);
    WORD  w;

    ExpandFilename( PARENT(LPDB,lpPub), wType, lpPub->fiFSPub, lpFam->fiFF,
        (LPSTR)gof.szPathName );

    w = ValidateFileAndGetVersion(gof.szPathName);

    if (w!=-1 && w!=0)
        return TRUE;
    return FALSE;

} //*** AreThumbsPresent


//*************************************************************
//
//  ConvertRLEtoBMP1
//
//  Purpose:
//      Converts the bitmap from RLE8 to a monochrome bitmap
//
//
//  Parameters:
//      HBITMAP hBmp
//      
//
//  Return: (HBITMAP)
//
//
//  Comments:
//      Deletes the bitmap passed in and returns a new one
//
//  History:    Date       Author     Comment
//               3/ 8/92   MSM        Created
//
//*************************************************************

HBITMAP NEAR PASCAL ConvertRLEtoBMP1( HBITMAP hBmp )
{
    HBITMAP hbmp;
    BITMAP  bmp;

#ifndef MAKE_RLE8
    return hBmp;
#endif

    GetObject( hBmp, sizeof(BITMAP), &bmp );

    //*** Create a monochrome bitmap
    hbmp = CreateBitmap( bmp.bmWidth, bmp.bmHeight, 1, 1, NULL );
    if (hbmp)
    {
        HBITMAP hOld1, hOld2;
        HDC hMem1, hMem2, hDC = GetDC( NULL );

        hMem1 = CreateCompatibleDC( hDC );
        hMem2 = CreateCompatibleDC( hDC );
        ReleaseDC( NULL, hDC );

        //*** Select in RLE8 bitmap
        hOld1 = SelectObject( hMem1, hBmp );

        //*** Select in BMP1 bitmap
        hOld2 = SelectObject( hMem2, hbmp );

        //*** Transfer bits
        BitBlt( hMem2, 0, 0, bmp.bmWidth, bmp.bmHeight,hMem1,0,0,SRCCOPY );

        //*** Restore old bitmaps
        SelectObject( hMem1, hOld1 );
        SelectObject( hMem2, hOld2 );

        //*** Delete memory DCs
        DeleteDC( hMem1 );
        DeleteDC( hMem2 );
    }

    //*** Get rid of the old
    DeleteObject( hBmp );

    //*** return the new
    return hbmp;

} //*** ConvertRLEtoBMP1

//*** EOF: dbbmp.c
