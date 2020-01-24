// util.h
//
// Header file for utiltiy functions used by DEFO2V
//
// WOSA/XRT XRTView Sample Application Version 1.00.003
//
// Copyright (c) 1994 Microsoft Corp.  All Rights Reserved.
//
// Revisions:
//  August 6, 1993  cek     First implementation.
//
#ifndef _UTIL_H_
#define _UTIL_H_

/// Utilities
#define RCF_NORMAL      0x0000
#define RCF_ITALIC      0x0001
#define RCF_UNDERLINE   0x0002
#define RCF_STRIKEOUT   0x0004
#define RCF_BOLD        0x0008
#define RCF_NODEFAULT   0x0010
#define RCF_TRUETYPEONLY 0x0011

HFONT WINAPI ReallyCreateFont( HDC hDC, LPTSTR lpszFace, LPTSTR lpszStyle, UINT nPointSize, UINT uiFlags ) ;


void WINAPI DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient ) ;

int WINAPI GetTextMetricsCorrectly( HDC hDC, LPTEXTMETRIC lpTM ) ;

typedef struct FAR tagCOLUMNSTRUCT
{
   int   nLeft ;       // starting x position of the column
   int   nRight ;      // ending x position of the column
   UINT  uiFlags ;      // format flags

} COLUMNSTRUCT, *PCOLUMNSTRUCT, FAR *LPCOLUMNSTRUCT ;

void WINAPI ColumnTextOut( HDC hdc, int nX, int nY, LPTSTR lpszIN, int cColumns, LPCOLUMNSTRUCT rgColumns ) ;
LPTSTR HRtoString( HRESULT hr ) ;
LPTSTR VTtoString( VARTYPE vt ) ;
LPTSTR VTtoString2( VARTYPE vt ) ;
//LPTSTR TYPEDESCtoString( TYPEDESC FAR* lptdesc ) ;




#endif

