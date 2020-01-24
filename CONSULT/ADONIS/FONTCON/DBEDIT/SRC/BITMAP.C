//*************************************************************
//  File name: bitmap.c
//
//  Description: 
//      Bitmap routines
//
//  History:    Date       Author     Comment
//               3/ 7/92   MSM        Created
//
//*************************************************************

#include "global.h"
#include <memory.h>
#include <stdlib.h>
#include <io.h>

HBITMAP  NEAR PASCAL BitmapToDIB( HBITMAP hBitmap, HPALETTE hPal ) ;
HANDLE   NEAR PASCAL ReadDIBFile( HFILE hFile) ;
HANDLE   NEAR PASCAL WinDibFromBitmap( HBITMAP hBitmap, DWORD dwStyle, WORD wBits, HPALETTE hPal ) ;
HANDLE   NEAR PASCAL PMDibFromBitmap( HBITMAP hbm, DWORD biStyle, WORD biBits, HPALETTE hpal) ;
BOOL     NEAR PASCAL WriteDIB( LPSTR szFile, HBITMAP hdib) ;

LPSTR    NEAR PASCAL FindDIBBits( LPSTR lpbi ) ;
WORD     NEAR PASCAL DIBNumColors( LPSTR lpbi ) ;
WORD     NEAR PASCAL PaletteSize( LPSTR lpbi ) ;
HBITMAP  NEAR PASCAL DIBToBitmap( HBITMAP hDIB, HPALETTE hPal ) ;
VOID     NEAR PASCAL InitBitmapInfoHeader( LPBITMAPINFOHEADER lpBmInfoHdr, DWORD dwWidth, DWORD dwHeight, int nBPP ) ;

DWORD    NEAR PASCAL DBGetThumbSize( LPFACE lpFace );

#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')
#define PALVERSION          0x300
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD) (lpbi)) == sizeof (BITMAPINFOHEADER))
#define BI_PM               3L


//*************************************************************
//
//  SaveBitmapFile
//
//  Purpose:
//      Saves the BMP to a file
//
//
//  Parameters:
//      LPSTR lpszFile
//      HBITMAP hDDB
//      
//
//  Return: (BOOL FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 7/92   MSM        Ported from old dbedit
//
//*************************************************************

BOOL FAR PASCAL SaveBitmapFile( LPSTR lpszFile, HBITMAP hDDB )
{
    HBITMAP  hdib ;
    BITMAPFILEHEADER	hdr;
    LPBITMAPINFOHEADER  lpbi;
    int                 fh;
    OFSTRUCT            of;

#ifdef MAKE_RLE8
    hdib = WinDibFromBitmap( hDDB, BI_RLE8, 8, NULL ) ;
#else
    hdib = WinDibFromBitmap( hDDB, BI_RGB, 1, NULL ) ;
#endif
   
    //*** Write out the DIB in the specified format.
    if (!hdib)
   	    return FALSE;

    fh = OpenFile (lpszFile, &of, OF_CREATE|OF_WRITE);
    if (fh == -1)
	   return FALSE;

    lpbi = (VOID FAR *)GlobalLock (hdib);

    //*** Fill in the fields of the file header
    hdr.bfType		= DIB_HEADER_MARKER;
    hdr.bfSize		= GlobalSize (hdib) + sizeof (BITMAPFILEHEADER);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;
    hdr.bfOffBits   = (DWORD)sizeof(BITMAPFILEHEADER) + lpbi->biSize +
                        PaletteSize((LPSTR)lpbi);

    //*** Write the file header
    if (!_lwrite (fh, (LPSTR)&hdr, sizeof (BITMAPFILEHEADER)))
    {
        GlobalUnlock (hdib);
        _lclose (fh);
        GlobalFree( hdib);
        return FALSE;
    }

    //*** Write the DIB header and the bits
    if (!_hwrite (fh, (LPSTR)lpbi, GlobalSize (hdib)))
    {
        GlobalUnlock (hdib);
        _lclose (fh);
        GlobalFree( hdib);
        return FALSE;
    }
    _lclose (fh);
    GlobalUnlock (hdib);
    GlobalFree( hdib);
    return TRUE;

} //*** SaveBitmapFile


//*************************************************************
//
//  WinDibFromBitmap
//
//  Purpose:
//      Converts a HBITMAP to a DIB
//
//
//  Parameters:
//      HBITMAP hBitmap
//      DWORD dwStyle
//      WORD wBits
//      HPALETTE hPal
//      
//
//  Return: (HANDLE NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 7/92   MSM        Ported from old DBEdit
//
//*************************************************************

HANDLE NEAR PASCAL WinDibFromBitmap( HBITMAP hBitmap, DWORD dwStyle, WORD wBits, HPALETTE hPal )
{
    BITMAP               bm;
    BITMAPINFOHEADER     bi;
    BITMAPINFOHEADER FAR *lpbi;
    DWORD                dwLen;
    HANDLE               hDIB;
    HANDLE               h;
    HDC                  hDC;

    if (!hBitmap)
        return NULL;

    if (hPal == NULL)
        hPal = GetStockObject (DEFAULT_PALETTE);

    GetObject (hBitmap, sizeof (bm), (LPSTR)&bm);

    if (wBits == 0)
        wBits =  bm.bmPlanes * bm.bmBitsPixel;

    if (wBits <= 1)
        wBits = 1;
    else if (wBits <= 4)
        wBits = 4;
    else if (wBits <= 8)
        wBits = 8;
    else
        wBits = 24;

    bi.biSize               = sizeof (BITMAPINFOHEADER);
    bi.biWidth              = bm.bmWidth;
    bi.biHeight             = bm.bmHeight;
    bi.biPlanes             = 1;
    bi.biBitCount           = wBits;
    bi.biCompression        = dwStyle;
    bi.biSizeImage          = 0;
    bi.biXPelsPerMeter      = 0;
    bi.biYPelsPerMeter      = 0;
    bi.biClrUsed            = 0;
    bi.biClrImportant       = 0;

    dwLen  = bi.biSize + PaletteSize ((LPSTR) &bi);
    hDIB = GlobalAlloc(GHND,dwLen);

    if (!hDIB)
        return NULL;

    lpbi   = (VOID FAR *)GlobalLock(hDIB);
    *lpbi  = bi;
    hDC    = GetDC (NULL);
    hPal   = SelectPalette (hDC, hPal, FALSE);
    RealizePalette(hDC);



    //*** Call GetDIBits with a NULL lpBits param, so it will calculate the
    //*** biSizeImage field for us
    GetDIBits( hDC, hBitmap, 0, (WORD) bi.biHeight, NULL, (LPBITMAPINFO)lpbi, 
                DIB_RGB_COLORS);

    bi = *lpbi;
    GlobalUnlock(hDIB);

    //*** If the driver did not fill in the biSizeImage field, make one up
    if (bi.biSizeImage == 0)
    {
        bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * wBits) * bm.bmHeight;

        if (dwStyle != BI_RGB)
            bi.biSizeImage = (bi.biSizeImage * 3) / 2;
    }

    //*** realloc the buffer big enough to hold all the bits
    dwLen = bi.biSize + PaletteSize((LPSTR)&bi) + bi.biSizeImage;
    if (h = GlobalReAlloc(hDIB,dwLen,0))
        hDIB = h;
    else
    {
        GlobalFree(hDIB);
        hDIB = NULL;
        SelectPalette(hDC,hPal,FALSE);
        ReleaseDC(NULL,hDC);
        return NULL;
    }

    //*** Call GetDIBits with a NON-NULL lpBits param, and actualy get the
    //*** bits this time
    lpbi = (VOID FAR *)GlobalLock(hDIB);

    if (GetDIBits( hDC, hBitmap, 0, (WORD) bi.biHeight,
        (LPSTR) lpbi + (WORD) lpbi->biSize + PaletteSize((LPSTR) lpbi),
        (LPBITMAPINFO) lpbi, DIB_RGB_COLORS) == 0)
    {
        GlobalUnlock (hDIB);
        hDIB = NULL;
        SelectPalette (hDC, hPal, FALSE);
        ReleaseDC (NULL, hDC);
        return NULL;
    }

    bi = *lpbi;
    GlobalUnlock (hDIB);

    SelectPalette (hDC, hPal, FALSE);
    ReleaseDC (NULL, hDC);
    return hDIB;

} //*** WinDibFromBitmap


//*************************************************************
//
//  DIBNumColors
//
//  Purpose:
//      Given a pointer to a DIB, returns number of colors
//      in the DIB's color table.
//
//  Parameters:
//      LPSTR lpbi
//      
//
//  Return: (WORD NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 7/92   MSM        Ported from old DBEdit
//
//*************************************************************

WORD NEAR PASCAL DIBNumColors( LPSTR lpbi )
{
   WORD wBitCount;


   // If this is a Windows style DIB, the number of colors in the
   //  color table can be less than the number of bits per pixel
   //  allows for (i.e. lpbi->biClrUsed can be set to some value).
   //  If this is the case, return the appropriate value.

   if (IS_WIN30_DIB (lpbi))
   {
      DWORD dwClrUsed;

      dwClrUsed = ((LPBITMAPINFOHEADER) lpbi)->biClrUsed;

      if (dwClrUsed)
         return (WORD) dwClrUsed;
   }


   // Calculate the number of colors in the color table based on
   //  the number of bits per pixel for the DIB.

   if (IS_WIN30_DIB (lpbi))
      wBitCount = ((LPBITMAPINFOHEADER) lpbi)->biBitCount;
   else
      wBitCount = ((LPBITMAPCOREHEADER) lpbi)->bcBitCount;

   switch (wBitCount)
   {
      case 1:
         return 2;

      case 4:
         return 16;

      case 8:
         return 256;

      default:
         return 0;
   }

} //*** DIBNumColors


//*************************************************************
//
//  PaletteSize
//
//  Purpose:
//      Given a pointer to a DIB, returns number of bytes
//      in the DIB's color table.
//
//  Parameters:
//      LPSTR lpbi
//      
//
//  Return: (WORD NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 7/92   MSM        Ported from old DBEdit
//
//*************************************************************

WORD NEAR PASCAL PaletteSize( LPSTR lpbi )
{
   if (IS_WIN30_DIB (lpbi))
      return (DIBNumColors (lpbi) * sizeof (RGBQUAD));
   else
      return (DIBNumColors (lpbi) * sizeof (RGBTRIPLE));

} //*** PaletteSize


//*************************************************************
//
//  DBMakeThumb
//
//  Purpose:
//      Makes a thumb nail for the face's styles
//
//
//  Parameters:
//      LPFACE lpFace
//      WORD   wBits
//
//  Return: (HBITMAP WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 5/92   MSM        Created
//
//*************************************************************

HBITMAP WINAPI DBMakeThumb( LPFACE lpFace, WORD wBits )
{
    HBITMAP hBitmap, hOld;
    HDC     hDC;
    LPSTYLE lpStyle;
    int     fErr = 0;
    RECT    rc;
    DWORD   dwTE;

#define CX_THUMB    LOWORD(dwTE)
#define CY_THUMB    HIWORD(dwTE)

    SetCursor( LoadCursor(NULL,IDC_WAIT) );
    dwTE = DBGetThumbSize( lpFace );

    if (!dwTE)
    {
        SetCursor( LoadCursor(NULL,IDC_ARROW) );
        return NULL;
    }

    lpFace->wBmpWidth  = CX_THUMB;
    lpFace->wBmpHeight = CY_THUMB;

    hBitmap = CreateBitmap( CX_THUMB, CY_THUMB*STYLES(lpFace), 1, wBits, NULL );
    if  (!hBitmap)
    {
        SetCursor( LoadCursor(NULL,IDC_ARROW) );
        return NULL;
    }
    else
    {
        HDC hTDC = GetDC( NULL );

        hDC = CreateCompatibleDC( hTDC );
        ReleaseDC( NULL, hTDC );
    }

    //*** Select in our Bitmap and save the old one
    hOld = SelectObject( hDC, hBitmap );

    lpStyle = GetFirstStyle( lpFace, NULL );

    rc.left   = 0;
    rc.top    = 0;
    rc.right  = CX_THUMB;
    rc.bottom = rc.top + CY_THUMB;

    //*** Walk through styles and build a bitmap of all styles in face
    while (lpStyle)
    {
        HFONT   hFont = DBGetFont( lpStyle, THUMB_POINTSIZE );
        LPSTR   lp = GetThumbString( lpStyle );
        int     nLen, nH, nW;
        SIZE    size;
        UINT    uiOff;

        if (!hFont)
        {
            fErr = 1;
            break;
        }

        nLen = lstrlen( lp );

        //*** Select Font in
        hFont = SelectObject( hDC, hFont );

        //*** Get font extents for centering
        uiOff = GetTextExtentABCPoint( hDC, lp, nLen, &size );
        nW = max( 0, (int)((int)CX_THUMB-size.cx)/2);
        nH = max( 0, (int)((int)CY_THUMB-size.cy)/2);

        //*** Capture font in bitmap
        ExtTextOut(hDC,nW+uiOff,rc.top+nH,ETO_OPAQUE|ETO_CLIPPED,&rc,lp,nLen,NULL);

        //*** Select Font out
        hFont = SelectObject( hDC, hFont );

        //*** Kill font
        DeleteObject( hFont );

        rc.top   +=CY_THUMB;
        rc.bottom+=CY_THUMB;

        //*** Get next style
        lpStyle = GetNextStyle( lpStyle, NULL );
    }

    //*** Select out our Bitmap and restore the old one
    SelectObject( hDC, hOld );
    DeleteDC( hDC );
    if (fErr)
    {
        DeleteObject( hBitmap );
        hBitmap = NULL;
    }

    SetCursor( LoadCursor(NULL,IDC_ARROW) );
    return hBitmap;

} //*** DBMakeThumb


//*************************************************************
//
//  DBGetThumbSize
//
//  Purpose:
//      Calculates the thumb size for each style
//
//
//  Parameters:
//      LPFACE lpFace
//      
//
//  Return: (DWORD NEAR PASCAL)
//      LOWORD = width
//      HIWORD = height
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/ 4/92   MSM        Created
//
//*************************************************************

DWORD NEAR PASCAL DBGetThumbSize( LPFACE lpFace )
{
    HDC     hDC = GetDC( NULL );
    LPSTYLE lpStyle;
    WORD    wWidth=0, wHeight=0;
    SIZE    size;

    lpStyle = GetFirstStyle( lpFace, NULL );

    while (lpStyle)
    {
        HFONT hFont = DBGetFont( lpStyle, THUMB_POINTSIZE );
        LPSTR lp;
        int   nLen;

        if (!hFont)
            continue;

        lp = GetThumbString( lpStyle );
        nLen = lstrlen( lp );

        //*** Select Font in
        hFont = SelectObject( hDC, hFont );

        //*** Get font extents
        GetTextExtentABCPoint( hDC, lp, nLen, &size );

        //*** Select Font out
        hFont = SelectObject( hDC, hFont );

        //*** Kill font
        DeleteObject( hFont );

        if ((int)wWidth<size.cx)
            wWidth = size.cx;

        if ((int)wHeight<size.cy)
            wHeight = size.cy;

        //*** Get next style
        lpStyle = GetNextStyle( lpStyle, NULL );
    }
    ReleaseDC( NULL, hDC );
    return MAKELONG( min(wWidth,512), min(wHeight,64) );

} //*** DBGetThumbSize

//*** EOF: bitmap.c
