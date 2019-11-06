/*                          MARK30.C

         Windows 3.0 New Executable File Marking Application
           (c) Copyright 1989, 1990, Charles E. Kindel, Jr.

REVISIONS

   DATE        VERSION     CHANGES
   5/30/90     1.00        First try

   5/31/90     1.01        Fix dialog box typos.
                           "Microsoft files" are bad!

   5/31/90     1.30        Windows 3.0 libararies and features!

   6/1/90      1.32        Refined detection of types of files.

   6/1/90      1.33        Moved dialog to center of screen.

   6/2/90      1.40        Added "advanced" dialog box, redesigned
                           handling of bits.

   6/5/90      1.41        Changed Title to Mark Three Oh.
                           Sped up color cycle and changed blue to
                           yellow.  Fixed up advanced box.

   6/23/90      1.42       Fixed bug in dir. name.
                           Gray Advanced menu...

   6/24/90     1.50a       Second major release ("a" means Advanced is activated)
                           Smoother.
                           New address.
                           Fixed annoying bugs.


*/

/* ------------------ IMPORTS ------------------- */
#include "markver.h"
#pragma comment (exestr, "File " __FILE__ ", Version " VERSION ", Last modified on " __TIMESTAMP__)
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mark30.h"
#include "mark30d.h" // resource identifiers

/* ----------------- GLOBAL DATA --------------------- */
char        szAppName [] = "Mark30" ;
char        szVerNum  [] = VERSION ;
HANDLE      hInst ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) 
{
HWND     hWnd ;         
MSG      msg ;

   if (!hPrevInstance)
      if (!InitWnd (hInstance))
      {
         MessageBox (NULL, "Not enough memory to run", szAppName, MB_ICONHAND | MB_SYSTEMMODAL) ;
         return FALSE ;
      }
   hInst = hInstance ;
   hWnd = CreateDialog (hInstance, szAppName, 0, NULL) ;

   if (hWnd == -1)
      {
         MessageBox (NULL, "Not enough memory to run", szAppName, MB_ICONHAND | MB_SYSTEMMODAL) ;
         return FALSE ;
      }

   SendMessage (hWnd, WM_MYINIT, 0, 0L) ;
   ShowWindow (hWnd, nCmdShow) ;

   while (GetMessage (&msg, NULL, 0, 0))
   {
      if (!IsDialogMessage (hWnd, &msg))
      {
         TranslateMessage (&msg) ;
         DispatchMessage (&msg) ;
      }
   }
   return msg.wParam ;
}


/**************************************************************************
 *
 *  BOOL DrawShadow (HDC hDC, HWND hWnd, HBRUSH hBrush, short ShadowWidth)
 *
 *  This function draws shadows along the bottom and right sides
 *  of a window.
 *
 *  Paramter         Type/Description
 *  ---------------------------------------------------------------
 *  hDC              HDC   Device context previously obtained.
 *  hWnd             HWND  Handle to the window or dialog control
 *                         that is to be shadowed.
 *  hBrush           HBRUSH Handle to the brush that is to be used to
 *                          draw the shadow.
 *  ShadowWidth      short The width, in device units, of the shadow.
 *
 *  RETURN VALUE
 *    Returns TRUE if successful, FALSE if not.
 *
 *  EXAMPLE
 *     
 *
 *       // in a window function 
 *          .
 *          .
 *          .
 *
 *       case WM_PAINT:
 *          hDC = BeginPaint (hDlg, &ps) ;                  // Get hDC
 *          GetTextMetrics (hDC, &tm) ;                     // tm is of type TEXTMETRIC
 *          DrawShadow (hDC,                                
 *                      GetDlgItem (hDlg, IDD_GROUPBOX1),   // Use GetDlgItem to get the handle of the edit
 *                                                          // control "IDD_GROUPBOX1"
 *                      GetStockObject (GRAY_BRUSH),        // Use GetStockObject to get a handle to a brush
 *                      tm.tmAveCharWidth / 2) ;            // 1/2 the system character width is nice looking.
 *          EndPaint (hDlg, &ps) ;
 *          return DefWindowProc (hDlg, iMessage, wParam, lParam) ;  // DO NOT "return 0L"
 *          .
 *          .
 *          .
 *
 *  NOTES
 *    This function should draw a shadow around any window, whether
 *    it is a dialog control or not.
 *
 *  AUTHOR
 *    Charles E. Kindel, Jr.
 *
 *  REVSIONS
 *    5/17/90     Initial version  (CEK)
 *
 ****************************************************************************/

BOOL DrawShadow ( HDC      hDC,           // handle to the Device Context
                  HWND     hWnd,          // handle to the window to be shadowed
                  HBRUSH   hBrush,        // brush to draw shadow with 
                  short    ShadowWidth )  // Width (in pixels) of the shadow
{
   LONG     Origin ;
   RECT     Rect ;
   POINT    Points[7] ;

   GetWindowRect (hWnd, (LPRECT) &Rect) ; 
   Origin = GetDCOrg (hDC) ;

   Rect.left   -= LOWORD(Origin) ;
   Rect.top    -= HIWORD(Origin) ;
   Rect.right  -= LOWORD(Origin) ;
   Rect.bottom -= HIWORD(Origin) ;

   Points [0].x = Rect.left + ShadowWidth ;
   Points [0].y = Rect.bottom ;

   Points [1].x = Rect.left + ShadowWidth ;
   Points [1].y = Rect.bottom + ShadowWidth ;

   Points [2].x = Rect.right + ShadowWidth ;
   Points [2].y = Rect.bottom + ShadowWidth ;

   Points [3].x = Rect.right + ShadowWidth ;
   Points [3].y = Rect.top + ShadowWidth ;

   Points [4].x = Rect.right ;
   Points [4].y = Rect.top + ShadowWidth ;

   Points [5].x = Rect.right ;
   Points [5].y = Rect.bottom ;

   Points [6].x = Rect.left + ShadowWidth ;
   Points [6].y = Rect.bottom ;

   if (!SelectObject (hDC, hBrush))
      return FALSE ;

   if (!SelectObject (hDC, GetStockObject (NULL_PEN)))
      return FALSE ;

   if (!Polygon (hDC, (LPPOINT) Points, 7))
      return FALSE ;

   return TRUE ;
} /* end of DrawShadow */


void OkMessageBox (HWND hDlg, char *szString, char *szFileName) 
{
   char szBuffer [128] ;
   wsprintf( (LPSTR)szBuffer, (LPSTR)szString, (LPSTR)szFileName) ;
   MessageBox(hDlg, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION) ;
}


BOOL InitWnd (HANDLE hInstance)
{
HANDLE      hMemory ;
PWNDCLASS   pWndClass ;
BOOL        bSuccess ;

   hMemory = LocalAlloc (LPTR, sizeof (WNDCLASS)) ;
   pWndClass = (PWNDCLASS) LocalLock (hMemory) ;

   pWndClass->style             = CS_HREDRAW | CS_VREDRAW ;
   pWndClass->lpfnWndProc       = WndProc ;
   pWndClass->cbClsExtra        = 0 ;
   pWndClass->cbWndExtra        = DLGWINDOWEXTRA ;
   pWndClass->hInstance         = hInstance ;
   pWndClass->hIcon             = LoadIcon (hInstance, szAppName) ;
   pWndClass->hCursor           = LoadCursor (NULL, IDC_ARROW) ;
   pWndClass->hbrBackground     = COLOR_WINDOW + 1 ;
   pWndClass->lpszMenuName      = szAppName ;
   pWndClass->lpszClassName     = szAppName ;

   bSuccess = RegisterClass (pWndClass) ;

   LocalUnlock (hMemory) ;
   LocalFree (hMemory) ;

   return (bSuccess) ;
}



BOOL ColorText (HDC hDC, HWND hWnd, DWORD dwBrush, LPSTR lpszMsg)
{
   RECT     Rect ;

   GetClientRect (hWnd, (LPRECT) &Rect) ;

   SetTextColor (hDC, dwBrush) ;

   if (!DrawText (hDC, lpszMsg, -1, (LPRECT) &Rect, DT_CENTER | DT_NOCLIP |DT_VCENTER))
      return FALSE ;
}


