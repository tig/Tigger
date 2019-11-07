/************************************************************************
 *                      FONT.C - FONT SELECTION FUNCTIONS
 ************************************************************************/

#ifndef _FONT_H_
#define _FONT_H_

HFONT WINAPI
ReallyCreateFont( HDC hDC, LPSTR lpszFace, short nPointSize, WORD wFlags ) ;

#define RCF_NORMAL      0x0000
#define RCF_ITALIC      0x0001
#define RCF_UNDERLINE   0x0002
#define RCF_STRIKEOUT   0x0004
#define RCF_BOLD        0x0008

int WINAPI
   GetTextMetricsCorrectly( HDC hDC, LPTEXTMETRIC lpTM ) ;

HFONT WINAPI
GetDeviceDefaultFont( HDC hDC, short nPointSize, WORD wFlags ) ;

#endif
