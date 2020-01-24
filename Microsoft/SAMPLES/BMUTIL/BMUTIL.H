/************************************************************************
 *
 *     Project:  Windows Sample Applications
 *
 *      Module:  bmutil.h
 *
 ***********************************************************************/

#ifndef _BMUTIL_H_
#define _BMUTIL_H_

BOOL FAR PASCAL bmMaskBlt( HDC hdcD, short xD, short yD,
                           short dx, short dy, HDC hdcS, 
                           HDC hdcMask, short xS, short yS ) ;

BOOL FAR PASCAL bmTransBlt( HDC hdcD, short xD, short yD,
                            short dx, short dy, HDC hdcS,
                            short xS, short yS ) ;

HBITMAP FAR PASCAL
   bmCopyTranslate( HBITMAP hbmSrc, COLORREF rgb,
                    short x, short y, WORD wStyle ) ;

HBITMAP FAR PASCAL bmCopy( HBITMAP hbmSrc ) ;

VOID FAR PASCAL bmColorTranslate( HBITMAP hbmSrc, COLORREF rgbOld,
                                  COLORREF rgbNew, WORD wStyle ) ;

VOID FAR PASCAL bmColorTranslateDC( HDC hdcBM, LPBITMAP lpBM,
                                    COLORREF rgbOld, COLORREF rgbNew,
                                    WORD wStyle ) ;


#define IDFLAG    '#'
#define FILEFLAG  '^'
#define AUTOFLAG  '%'
#define ICONFLAG  '@'                                 

//
// wStyle flags for Translate functions
//
#define BM_NORMAL    0x0000
#define BM_DISABLED  0x0001

#define BM_HBITMAP   0x8000

HBITMAP FAR PASCAL bmLoadBitmapTranslate( HANDLE hInst, LPSTR lpszResName,
                                          DWORD rgbNew, short x, short y,
                                          WORD wStyle ) ;

HBITMAP FAR PASCAL bmLoadBitmap( HANDLE hInst, LPSTR lpszResName ) ;

HBITMAP FAR PASCAL bmLoadBitmapFromFile( LPSTR lpszFile ) ;


#endif 

/************************************************************************
 * End of File: bmutil.h
 ***********************************************************************/

