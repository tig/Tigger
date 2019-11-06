/************************************************************************
 *
 *     Project:  Windows Sample Applications
 *
 *      Module:  fontutil.c
 *
 *     Remarks:  
 *
 *    This file contains font handling routines that are useful when
 *    dealing with creating and manipulating fonts in Windows.
 *
 ***********************************************************************/

#ifndef _FONTUTIL_H_
#define _FONTUTIL_H_

HFONT WINAPI
ReallyCreateFont( HDC hDC, LPSTR lpszFace, short nPointSize, WORD wFlags ) ;

HFONT WINAPI
   ReallyCreateFontEx( HDC hDC, LPSTR lpszFace, LPSTR lpszStyle, short nPointSize, WORD wFlags ) ;

#define RCF_NORMAL      0x0000
#define RCF_ITALIC      0x0001
#define RCF_UNDERLINE   0x0002
#define RCF_STRIKEOUT   0x0004
#define RCF_BOLD        0x0008
#define RCF_NODEFAULT   0x0010
#define RCF_TRUETYPEONLY 0x0011

int WINAPI
   GetTextMetricsCorrectly( HDC hDC, LPTEXTMETRIC lpTM ) ;

UINT WINAPI GetTextPointSize( HDC hDC ) ;
UINT WINAPI GetTextStyle( HDC hDC, UINT cbBuffer, LPSTR lpszStyle ) ;
UINT WINAPI GetTextFullName( HDC hDC, UINT cbBuffer, LPSTR lpszName ) ;
UINT WINAPI GetTextExtentABCPoint( HDC hDC, LPSTR lpsz, int cb, LPSIZE lpSize ) ; 
//DWORD WINAPI GetTextBoundingSize( HDC hDC, LPSTR lpsz, int cb, LPINT lpxStart, LPSIZE lpSize ) ;

#endif
/************************************************************************
 * End of File: fontutil.h
 ***********************************************************************/

