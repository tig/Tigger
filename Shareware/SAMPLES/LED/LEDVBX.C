//---------------------------------------------------------------------------
//        Copyright (C) 1991-92, Microsoft Corporation
//
// You have a royalty-free right to use, modify, reproduce and distribute
// the Sample Custom Control Files (and/or any modified version) in any way
// you find useful, provided that you agree that Microsoft has no warranty,
// obligation or liability for any Custom Control File.
//---------------------------------------------------------------------------
// LEDVBX.C
//---------------------------------------------------------------------------
// Contains control procedure for LED control
//---------------------------------------------------------------------------

#include <windows.h>
#include <vbapi.h>   
#include "ledvbx.h"


static BOOL NEAR PASCAL PaintLED( HCTL hctl, HWND hWnd, HDC hDC ) ;
static BOOL NEAR PASCAL DrawRoundLED( HDC hDC, int x, int y, int nDiameter ) ;
static BOOL NEAR PASCAL DrawSquareLED( HDC hDC, int x, int y, int x1, int y1) ;
static COLORREF NEAR PASCAL tdGetHighlightColor( COLORREF rgb ) ;
//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
HANDLE hmodDLL;


//---------------------------------------------------------------------------
// Local Prototypes
//---------------------------------------------------------------------------
VOID NEAR PaintLEDVBX(HCTL hctl, HWND hwnd, HDC hdc);
VOID NEAR RecalcArea(HCTL hctl, HWND hwnd);
VOID NEAR FlashLEDVBX(HCTL hctl, HDC hdc);
BOOL NEAR InLEDVBX(HCTL hctl, SHORT xcoord, SHORT ycoord);
VOID NEAR FireClickIn(HCTL hctl, SHORT x, SHORT y);
VOID NEAR FireClickOut(HCTL hctl);


//---------------------------------------------------------------------------
// LEDVBX Control Procedure
//---------------------------------------------------------------------------
LONG FAR PASCAL _export LEDVBXCtlProc
(
    HCTL   hctl,
    HWND   hwnd,
    USHORT msg,
    USHORT wp,
    LONG   lp
)
{
    switch (msg)
    {
    case WM_NCCREATE:
        {
        LPLED lpLED = LpLEDDEREF(hctl);

        lpLED->LEDVBXShape = TRUE;
        lpLED->FlashColor = 128L;
        VBSetControlProperty(hctl, IPROP_LEDVBX_BACKCOLOR, 255L);
        // *** lpLED may now be invalid due to call to VB API ***
        break;
        }
#if 0
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        if (InLEDVBX(hctl, (SHORT)lp, (SHORT)HIWORD(lp)))
        {
            HDC hdc = GetDC(hwnd);

            FlashLEDVBX(hctl, hdc);
            ReleaseDC(hwnd, hdc);
            FireClickIn(hctl, (SHORT)lp, (SHORT)HIWORD(lp));
        }
        else
            FireClickOut(hctl);
        break;

        case WM_LBUTTONUP:
        if (InLEDVBX(hctl, (SHORT)lp, (SHORT)HIWORD(lp)))
        {
            HDC hdc = GetDC(hwnd);

            PaintLEDVBX(hctl, hwnd, hdc);
            ReleaseDC(hwnd, hdc);
        }
        break;
#endif

        case WM_PAINT:
            if (wp)
                PaintLEDVBX(hctl, hwnd, (HDC)wp);
            else
            {
                PAINTSTRUCT ps;

                BeginPaint(hwnd, &ps);
                PaintLED(hctl, hwnd, ps.hdc);
                EndPaint(hwnd, &ps);
            }
        break;

        case WM_SIZE:
            RecalcArea(hctl, hwnd);
        break;

        case VBM_SETPROPERTY:
        switch (wp)
        {
            case IPROP_LEDVBX_SHAPE:
                LpLEDDEREF(hctl)->LEDVBXShape = (SHORT)lp;
                RecalcArea(hctl, hwnd);
                InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        break;

    }

    return VBDefControlProc(hctl, hwnd, msg, wp, lp);
}


//---------------------------------------------------------------------------
// Handle painting by drawing circle into the given hdc.
//---------------------------------------------------------------------------
VOID NEAR PaintLEDVBX
(
    HCTL hctl,
    HWND hwnd,
    HDC  hdc
)
{
    HBRUSH hbr;
    HBRUSH hbrOld = NULL;
    LPRECT lprect = &LpLEDDEREF(hctl)->rectDrawInto;

    hbr = (HBRUSH)SendMessage(GetParent(hwnd), WM_CTLCOLOR,
                  hdc, MAKELONG(hwnd, 0));
    if (hbr)
        hbrOld = SelectObject(hdc, hbr);
    Ellipse(hdc, lprect->left, lprect->top, lprect->right, lprect->bottom);
    if (hbrOld)
        SelectObject(hdc, hbrOld);  // Restore old brush
}


//---------------------------------------------------------------------------
// Paint the circle in the FlashColor.
//---------------------------------------------------------------------------
VOID NEAR FlashLEDVBX
(
    HCTL hctl,
    HDC  hdc
)
{
    HBRUSH  hbr;
    HBRUSH  hbrOld = NULL;
    LPLED  lpLED = LpLEDDEREF(hctl);
    LPRECT  lprect = &lpLED->rectDrawInto;

    hbr = CreateSolidBrush(RGBCOLOR(lpLED->FlashColor));
    if (hbr)
        hbrOld = SelectObject(hdc, hbr);
        
    Ellipse(hdc, lprect->left, lprect->top, lprect->right, lprect->bottom);
    if (hbr)
    {
        SelectObject(hdc, hbrOld);
        DeleteObject(hbr);
    }
}


//---------------------------------------------------------------------------
// Use the hwnd's client size to determine the bounding rectangle for the
// circle.  If LEDVBXShape is TRUE, then we need to calculate a square
// centered in lprect.
//---------------------------------------------------------------------------
VOID NEAR RecalcArea
(
    HCTL hctl,
    HWND hwnd
)
{
    LPLED lpLED = LpLEDDEREF(hctl);
    LPRECT lprect = &lpLED->rectDrawInto;

    GetClientRect(hwnd, lprect);
    if (!lpLED->LEDVBXShape)
        return;
    if (lprect->right > lprect->bottom)
    {
        lprect->left  = (lprect->right - lprect->bottom) / 2;
        lprect->right = lprect->left + lprect->bottom;
    }
    else if (lprect->bottom > lprect->right)
    {
        lprect->top    = (lprect->bottom - lprect->right) / 2;
        lprect->bottom = lprect->top + lprect->right;
    }
}


//---------------------------------------------------------------------------
// Return TRUE if the given coordinates are inside of the circle.
//---------------------------------------------------------------------------
BOOL NEAR InLEDVBX
(
    HCTL  hctl,
    SHORT xcoord,
    SHORT ycoord
)
{
    double  a, b;
    double  x, y;
    LPRECT  lprect = &LpLEDDEREF(hctl)->rectDrawInto;

    a = (lprect->right  - lprect->left) / 2;
    b = (lprect->bottom - lprect->top)  / 2;
    x = xcoord - (lprect->left + lprect->right)  / 2;
    y = ycoord - (lprect->top  + lprect->bottom) / 2;
    return ((x * x) / (a * a) + (y * y) / (b * b) <= 1);
}


//---------------------------------------------------------------------------
// TYPEDEF for parameters to the ClickIn event.
//---------------------------------------------------------------------------
typedef struct tagCLICKINPARMS
    {
    float far *Y;
    float far *X;
    LPVOID     Index;
    } CLICKINPARMS;


//---------------------------------------------------------------------------
// Fire the ClickIn event, passing the x,y coords of the click.
//---------------------------------------------------------------------------
VOID NEAR FireClickIn
(
    HCTL  hctl,
    SHORT x,
    SHORT y
)
{
    CLICKINPARMS params;
    float    xTwips, yTwips;

    xTwips = (float)VBXPixelsToTwips(x);
    yTwips = (float)VBYPixelsToTwips(y);
    params.X = &xTwips;
    params.Y = &yTwips;
    VBFireEvent(hctl, IEVENT_LEDVBX_CLICKIN, &params);
}


//---------------------------------------------------------------------------
// Fire the ClickOut event.
//---------------------------------------------------------------------------
VOID NEAR FireClickOut
(
    HCTL hctl
)
{
    VBFireEvent(hctl, IEVENT_LEDVBX_CLICKOUT, NULL);
}

/************************************************************************
 *  static BOOL NEAR PASCAL PaintLED( HWND hWnd, HDC hDC )
 *
 *  Description: 
 *
 *    This function paints the LED.
 *
 *  Comments:
 *
 ************************************************************************/
BOOL NEAR PASCAL PaintLED( HCTL hctl, HWND hWnd, HDC hDC )
{
   RECT     rc ;
   HBRUSH   hbrColor ;
   HPEN     hPen ;
   COLORREF    rgbNorm ;
   COLORREF    rgbHigh ;
   LPLED lpLED = LpLEDDEREF(hctl);

   GetClientRect( hWnd, &rc ) ;

   rgbNorm = lpLED->FlashColor ;

   hbrColor = CreateSolidBrush( rgbNorm ) ;

   hbrColor = SelectObject( hDC, hbrColor ) ;

   rgbHigh = tdGetHighlightColor( rgbNorm ) ;

   hPen = CreatePen( PS_SOLID, 1, rgbHigh ) ;

   hPen = SelectObject( hDC, hPen ) ;

//   if (lpLED->fShape == FALSE) // default is round
      DrawRoundLED( hDC, rc.left, rc.top, rc.right - rc.left ) ;
//   else
#ifdef NOT_VBX
      if (GETSTYLE( hWnd ) & LED_HORZ)
         DrawSquareLED( hDC, rc.left, rc.top,
                             rc.left + DEF_HORZX, rc.top + DEF_HORZY ) ;
      else
         if (GETSTYLE( hWnd ) & LED_VERT)
            DrawSquareLED( hDC, rc.left, rc.top,
                              rc.left + DEF_VERTX, rc.top + DEF_VERTY ) ;
         else
            DrawSquareLED( hDC, rc.left, rc.top, rc.right, rc.bottom ) ;

#else 
       //DrawSquareLED( hDC, rc.left, rc.top, rc.right, rc.bottom ) ;
#endif
   hbrColor = SelectObject( hDC, hbrColor ) ;
   DeleteObject( hbrColor ) ;

   hPen = SelectObject( hDC, hPen ) ;
   DeleteObject( hPen ) ;

   return TRUE ;

}/* PaintLED() */

/****************************************************************
 *  static BOOL NEAR PASCAL DrawRoundLED( HDC, int x, int y, int nDiameter )
 *
 *  Description: 
 *
 *    This function draws a 3D looking LED light using the
 *    current brush.
 *
 *  Comments:
 *
 ****************************************************************/
static BOOL NEAR PASCAL DrawRoundLED( HDC hDC, int x, int y, int nDiameter )
{
   int    nShadeX, nShadeY ;
   HPEN     hOldPen ;
   LOGPEN   lp ;

   hOldPen = SelectObject( hDC, GetStockObject( BLACK_PEN ) ) ;

   Ellipse( hDC, x, y, x + nDiameter, y + nDiameter ) ;

   //
   // Calculate where the shading goes...
   // 
   nShadeX = x + nDiameter / 3 ;
   nShadeY = y + nDiameter / 3 ;

   GetObject( hOldPen, sizeof( LOGPEN ), (LPSTR)&lp ) ;

   SetPixel( hDC, nShadeX - 1 , nShadeY + 1  , lp.lopnColor ) ;
   SetPixel( hDC, nShadeX     , nShadeY + 1  , lp.lopnColor ) ;
   SetPixel( hDC, nShadeX     , nShadeY      , lp.lopnColor ) ;
   SetPixel( hDC, nShadeX + 1 , nShadeY      , lp.lopnColor ) ;
   SetPixel( hDC, nShadeX + 1 , nShadeY - 1  , lp.lopnColor ) ;

   SelectObject( hDC, hOldPen ) ;

   return TRUE ;

} /* DrawRoundLED()  */

/****************************************************************
 *  static BOOL NEAR PASCAL
 *    DrawSquareLED( HDC, int x, int y, int nSizeX, int nSizeY )
 *
 *  Description: 
 *
 *    This function draws a 3D looking LED light using the
 *    current brush.
 *
 *  Comments:
 *
 ****************************************************************/
static BOOL NEAR PASCAL
DrawSquareLED( HDC hDC, int x, int y, int x1, int y1)
{
   HPEN     hOldPen ;

   hOldPen = SelectObject( hDC, GetStockObject( BLACK_PEN ) ) ;

   Rectangle( hDC, x, y, x1, y1 ) ;

   SelectObject( hDC, hOldPen ) ;

   MoveTo( hDC, x + 2, y1 - 3 ) ;
   LineTo( hDC, x + 2, y + 2 ) ;
   LineTo( hDC, x1 - 2, y + 2 ) ;

   return TRUE ;

} /* DrawSquareLED()  */


/****************************************************************
 *  COLORREF WINAPI tdGetHighLightColor( COLORREF rgb )
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
COLORREF NEAR PASCAL tdGetHighlightColor( COLORREF rgb )
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

} /* tdGetHighlightColor()  */


//---------------------------------------------------------------------------
// Initialize library. This routine is called when the first client loads
// the DLL.
//---------------------------------------------------------------------------
int FAR PASCAL LibMain
(
    HANDLE hModule,
    WORD   wDataSeg,
    WORD   cbHeapSize,
    LPSTR  lpszCmdLine
)
{
    // Avoid warnings on unused (but required) formal parameters
    wDataSeg    = wDataSeg;
    cbHeapSize    = cbHeapSize;
    lpszCmdLine = lpszCmdLine;

    hmodDLL = hModule;

    return 1;
}


//---------------------------------------------------------------------------
// Register custom control. This routine is called by VB when the custom
// control DLL is loaded for use.
//---------------------------------------------------------------------------
BOOL FAR PASCAL _export VBINITCC
(
    USHORT usVersion,
    BOOL   fRuntime
)
{
    // Avoid warnings on unused (but required) formal parameters
    fRuntime  = fRuntime;
    usVersion = usVersion;

    // Register control(s)
    return VBRegisterModel(hmodDLL, &modelLEDVBX);
}


//---------------------------------------------------------------------------
// WEP
//---------------------------------------------------------------------------
// C7 and QCWIN provide default a WEP:
//---------------------------------------------------------------------------
#if (_MSC_VER < 610)

int FAR PASCAL WEP(int fSystemExit);

//---------------------------------------------------------------------------
// For Windows 3.0 it is recommended that the WEP function reside in a
// FIXED code segment and be exported as RESIDENTNAME.  This is
// accomplished using the alloc_text pragma below and the related EXPORTS
// and SEGMENTS directives in the .DEF file.
//
// Read the comments section documenting the WEP function in the Windows
// 3.1 SDK "Programmers Reference, Volume 2: Functions" before placing
// any additional code in the WEP routine for a Windows 3.0 DLL.
//---------------------------------------------------------------------------
#pragma alloc_text(WEP_TEXT,WEP)

//---------------------------------------------------------------------------
// Performs cleanup tasks when the DLL is unloaded.  WEP() is
// called automatically by Windows when the DLL is unloaded (no
// remaining tasks still have the DLL loaded).    It is strongly
// recommended that a DLL have a WEP() function, even if it does
// nothing but returns success (1), as in this example.
//---------------------------------------------------------------------------
int FAR PASCAL WEP
(
    int fSystemExit
)
{
    // Avoid warnings on unused (but required) formal parameters
    fSystemExit = fSystemExit;

    return 1;
}
#endif // C6

//---------------------------------------------------------------------------
