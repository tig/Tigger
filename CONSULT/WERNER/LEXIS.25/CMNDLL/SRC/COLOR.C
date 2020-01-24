/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  color.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This module handles color dialog contols.  In particular it
 *    hanldes the owner draw and setup of the select color combo box.
 *
 *   Revisions:  
 *     00.00.000  2/15/91 cek   First Version
 *
 *
 ************************************************************************/

#define WINDLL
#include "..\inc\undef.h"
#include <windows.h>

#include "..\inc\cmndll.h"

/************************************************************************
 * Imported variables
 ************************************************************************/

/************************************************************************
 * Local Defines
 ************************************************************************/

#define COLOR_HEIGHT    13

/************************************************************************
 * Macros 
 ************************************************************************/


/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/
void NEAR PASCAL PatB( HDC hDC, int x, int y, int dx, int dy, DWORD rgb ) ;

void NEAR PASCAL FillR( HDC hDC, LPRECT lprc, DWORD rgb ) ;

void NEAR PASCAL FrameR( HDC hDC, LPRECT lprc, DWORD rgb, int iFrame ) ;

typedef struct tagCOLORSTRUCT
{
   char  *szColor ;
   DWORD rgb ;
} COLORSTRUCT, *PCOLORSTRUCT, FAR *LPCOLORSTRUCT ;


/************************************************************************
 * Local Variables
 ************************************************************************/
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

#define NUM_CB_COLORS (sizeof( csColors ) / sizeof( csColors[0] ))

/************************************************************************
 * Exported Functions
 ************************************************************************/
/*************************************************************************
 *  LONG FAR PASCAL
 *    ColorCBOwnerDraw( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
LONG FAR PASCAL ColorCBOwnerDraw( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam )
{
   RECT  rc ;
   DWORD rgb ;
   

   #define lpMIS ((LPMEASUREITEMSTRUCT)lParam)
   #define lpDIS ((LPDRAWITEMSTRUCT)lParam)

   switch (wMsg)
   {
      case WM_MEASUREITEM:
         lpMIS->itemHeight = COLOR_HEIGHT ;
      return TRUE ;

      case WM_DRAWITEM:
         rc = lpDIS->rcItem ;
         if (lpDIS->itemID < NUM_CB_COLORS)
            rgb = csColors[lpDIS->itemID].rgb ;

         if (lpDIS->itemState & ODS_SELECTED)
         {
            FrameR( lpDIS->hDC, &rc, RGBBLACK, 2 ) ;
            InflateRect( &rc, -1, -1 ) ;
            FrameR( lpDIS->hDC, &rc, RGBWHITE, 2 ) ;
            InflateRect( &rc, -1, -1 ) ;
         }
         else
         {
            FrameR( lpDIS->hDC, &rc, GetSysColor( COLOR_WINDOW ), 2 ) ;
            InflateRect( &rc, -1, -1 ) ;
         }

         FillR( lpDIS->hDC, &rc, rgb ) ;
      return TRUE ;

      case WM_DELETEITEM:
         return TRUE ;
   }

   return TRUE ;

   #undef lpMIS
   #undef lpDIS
}/* ColorCBOwnerDraw() */

/*************************************************************************
 *  BOOL FAR PASCAL ColorCBSetup( HWND hCB )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL ColorCBSetup( HWND hCB )
{
   if (hCB)
   {
      WORD w ;

      SendMessage( hCB, CB_RESETCONTENT, 0, 0L ) ;
      for ( w = 0 ; w < NUM_CB_COLORS ; w++)
      {
         SendMessage( hCB, CB_ADDSTRING, 0,
                      (LONG)(LPSTR)csColors[w].szColor ) ;
      }

      return TRUE ;
   }
   else
      return FALSE ;

}/* ColorCBSetup() */

/*************************************************************************
 *  DWORD FAR PASCAL ColorCBGet( HWND hCB )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
DWORD FAR PASCAL ColorCBGet( HWND hCB )
{
   if (hCB)
   {
      WORD w ;

      w = (WORD)SendMessage( hCB, CB_GETCURSEL, 0, 0L ) ;

      if (w < NUM_CB_COLORS)
         return csColors[w].rgb ;
      else
         return 0L ;
   }
   else
      return 0L ;

}/* ColorCBGet() */

/*************************************************************************
 *  void FAR PASCAL ColorCBSetCurSel( HWND hCB, DWORD )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
void FAR PASCAL ColorCBSetCurSel( HWND hCB, DWORD dw )
{
   if (hCB)
   {
      WORD w ;
      WORD wSel = 0xFFFF ;

      for (w = 0 ; w < NUM_CB_COLORS ; w++)
         if (dw == csColors[w].rgb)
         {
            wSel = w ;
            break ;
         }

      SendMessage( hCB, CB_SETCURSEL, w, 0L ) ;
   }

   return ;

}/* ColorCBSetCurSel() */

/************************************************************************
 * Local Fuctions
 ************************************************************************/
void NEAR PASCAL PatB( HDC hDC, int x, int y, int dx, int dy, DWORD rgb )
{
   RECT     rc ;

   SetBkColor( hDC, rgb ) ;
   rc.left = x ;
   rc.top = y ;
   rc.right = x + dx ;
   rc.bottom = y + dy ;

   ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;
}

void NEAR PASCAL FillR( HDC hDC, LPRECT lprc, DWORD rgb )
{
   SetBkColor( hDC, rgb ) ;
   ExtTextOut( hDC, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL ) ;
}

void NEAR PASCAL FrameR( HDC hDC, LPRECT lprc, DWORD rgb, int iFrame )
{
   int   dx, dy ;

   dx = lprc->right - lprc->left ;
   dy = lprc->bottom - lprc->top  - 2 * iFrame ;

   PatB( hDC, lprc->left, lprc->top,               dx, iFrame,    rgb ) ;
   PatB( hDC, lprc->left, lprc->bottom - iFrame,   dx, iFrame,    rgb ) ;

   PatB( hDC, lprc->left,           lprc->top + iFrame, iFrame, dy,    rgb ) ;
   PatB( hDC, lprc->right - iFrame, lprc->top + iFrame, iFrame, dy,    rgb ) ;
}


/****************************************************************
 *  DWORD FAR PASCAL GetHighLightColor( DWORD rgb )
 *
 *  Description: 
 *
 *    This function returns the highlight color that corresponds
 *    to the given rgb value.  If there is no "high intensity"
 *    color that matches, white is used (or yellow if the color
 *    is white).
 *
 *  Comments:
 *
 ****************************************************************/
DWORD FAR PASCAL GetHighlightColor( DWORD rgb )
{
   BYTE  cRed, cGreen, cBlue ;

   if (rgb == RGBLTRED     ||
       rgb == RGBLTGREEN   ||
       rgb == RGBLTBLUE    ||
       rgb == RGBLTMAGENTA ||
       rgb == RGBLTCYAN    ||
       rgb == RGBLTGRAY    ||
       rgb == RGBYELLOW)
      return RGBWHITE ;

   if (rgb == RGBWHITE)
      return RGBLTGRAY ;

   if (rgb == RGBBLACK || rgb == RGBGRAY)
      return RGBLTGRAY ;

   cRed = (BYTE)(rgb & 0x000000FF) ;

   cGreen = (BYTE)((rgb & 0x0000FF00) >> 8) ;

   cBlue = (BYTE)((rgb & 0x00FF0000) >> 16) ;

   if (cRed == 128)
      cRed += 64 ;

   if (cGreen == 128)
      cGreen += 64 ;

   if (cBlue == 128)
      cBlue += 64 ;

   return RGB( cRed, cGreen, cBlue ) ;

} /* GetHighLightColor()  */


/****************************************************************
 *  DWORD FAR PASCAL GetShadowColor( DWORD rgb )
 *
 *  Description: 
 *
 *    Returns an appropriate shadow color for the given rgb.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD FAR PASCAL GetShadowColor( DWORD rgb )
{
   BYTE  cRed, cGreen, cBlue ;

   if (rgb == RGBBLACK)
      return RGBGRAY ;

   if (rgb == RGBRED     ||
       rgb == RGBGREEN   ||
       rgb == RGBBLUE    ||
       rgb == RGBBROWN   ||
       rgb == RGBMAGENTA ||
       rgb == RGBCYAN    ||
       rgb == RGBWHITE   ||
       rgb == RGBGRAY)
      return RGBBLACK ;

   if (rgb == RGBLTGRAY)
      return RGBGRAY ;

   cRed = (BYTE)(rgb & 0x000000FF) ;

   cGreen = (BYTE)((rgb & 0x0000FF00) >> 8) ;

   cBlue = (BYTE)((rgb & 0x00FF0000) >> 16) ;

   if (cRed > 128)
      cRed -= 64 ;

   if (cGreen > 128)
      cGreen -= 64 ;

   if (cBlue > 128)
      cBlue -= 64 ;

   return RGB( cRed, cGreen, cBlue ) ;
} /* GetShadowColor()  */

/****************************************************************
 *  DWORD FAR PASCAL MapTextColor( DWORD rgb )
 *
 *  Description: 
 *
 *    Returns an appropriate text color for the given bg.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD FAR PASCAL MapTextColor( DWORD rgb )
{
   return rgb = GetSysColor( COLOR_WINDOWTEXT ) ;

   if (rgb == RGBRED     ||
       rgb == RGBGREEN   ||
       rgb == RGBBLUE    ||
       rgb == RGBBROWN   ||
       rgb == RGBMAGENTA ||
       rgb == RGBGRAY    ||
       rgb == RGBBLACK)
      return RGBWHITE ;

   return RGBBLACK ;

} /* MapTextColor()  */


/************************************************************************
 * End of File: color.c
 ************************************************************************/

