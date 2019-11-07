/************************************************************************
 *
 *     Project:  Windows Sample Applications
 *
 *      Module:  colrutil.c
 *
 *     Remarks:  
 *
 *    This file contains font handling routines that are useful when
 *    dealing with creating and manipulating fonts in Windows.
 *
 ***********************************************************************/

#define RGBBLACK     RGB(0,0,0)
#define RGBRED       RGB(128,0,0)
#define RGBGREEN     RGB(0,128,0)
#define RGBBLUE      RGB(0,0,128)
#define RGBBROWN     RGB(128,128,0)
#define RGBMAGENTA   RGB(128,0,128)
#define RGBCYAN      RGB(0,128,128)
#define RGBLTGRAY    RGB(192,192,192)
#define RGBGRAY      RGB(128,128,128)

#define RGBLTRED     RGB(255,0,0)
#define RGBLTGREEN   RGB(0,255,0)
#define RGBLTBLUE    RGB(0,0,255)
#define RGBYELLOW    RGB(255,255,0)
#define RGBLTMAGENTA RGB(255,0,255)
#define RGBLTCYAN    RGB(0,255,255)
#define RGBWHITE     RGB(255,255,255)


/* The following is handy for giving a listbox of colors */

#ifdef _USECOLORSTRUCT
   typedef struct tagCOLORSTRUCT
   {
      char  *szColor ;
      DWORD rgb ;
   } COLORSTRUCT, *PCOLORSTRUCT, FAR *LPCOLORSTRUCT ;

   COLORSTRUCT csColors[] =
   {
      "Red",            RGBRED,
      "Green",          RGBGREEN,
      "Blue",           RGBBLUE,
      "Brown",          RGBBROWN,
      "Magenta",        RGBMAGENTA,
      "Cyan",           RGBCYAN,
      "LightRed",       RGBLTRED,
      "LightGreen",     RGBLTGREEN,
      "LightBlue",      RGBLTBLUE,
      "Yellow",         RGBYELLOW,
      "LightMagenta",   RGBLTMAGENTA,
      "LightCyan",      RGBLTCYAN,
      "White",          RGBWHITE,
      "LightGray",      RGBLTGRAY,
      "Gray",           RGBGRAY,
      "Black",          RGBBLACK
   } ;

   #define _NUM_COLORS (sizeof( csColors ) / sizeof( csColors[0] ))

#endif




DWORD FAR PASCAL colorReverseRGB( DWORD rgb ) ;
DWORD FAR PASCAL colorGetHighlightColor( DWORD rgb ) ;
DWORD FAR PASCAL colorGetShadowColor( DWORD rgb ) ;
DWORD FAR PASCAL colorMapTextColor( DWORD rgb ) ;



/************************************************************************
 * End of File: colrutil.h
 ***********************************************************************/

