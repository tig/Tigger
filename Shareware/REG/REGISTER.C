 /*                           REGISTER.C

               Register - Registration number generator
           (c) Copyright 1989, 1990, Charles E. Kindel, Jr.

REVISIONS

   DATE        VERSION     CHANGES
*/
/* ------------------ IMPORTS ------------------- */
#include <windows.h>
#include <winundoc.h>
#include <stdlib.h>
#include <string.h>
#include "ws_defs.h"
#include "ws_types.h"
#include "ws_reg.h"
#include "register.h"

#ifdef DEBUG
#include "auxwrite.h"
#endif


/* ----------------- GLOBAL DATA --------------------- */
char        szAppName [] = "Register" ;
HWND        hDlg ;
BOOL DrawShadow (HDC hDC, HWND hWnd, HBRUSH hBrush, short ShadowWidth) ;


int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) 
{
HWND     hWnd ;         
MSG      msg ;
HANDLE   hAccel ;


   if (!hPrevInstance)
      if (!InitWnd (hInstance))
      {
         MessageBox (NULL, "Not enough memory to run", szAppName, MB_ICONHAND | MB_SYSTEMMODAL) ;
         return FALSE ;
      }

   hWnd = CreateWindow (szAppName, "Registration Numbers",
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                        CW_USEDEFAULT, 0,
                        CW_USEDEFAULT, 0,
                        NULL, NULL, hInstance, NULL) ;

   ShowWindow (hWnd, nCmdShow) ;
   UpdateWindow (hWnd) ;

   while (GetMessage (&msg, NULL, 0, 0))
   {
      if (hDlg == 0 || !IsDialogMessage (hDlg, &msg))
      {
         TranslateMessage (&msg) ;
         DispatchMessage (&msg) ;
      }
   }
   return msg.wParam ;
}


long FAR PASCAL WndProc (HWND hWnd, unsigned iMessage, WORD wParam, LONG lParam)
{
HANDLE   hInstance ;
FARPROC  lpfnDlgProc ;
char     szText [80] ;

switch (iMessage)
   {
      case WM_CREATE:
         hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;
         lpfnDlgProc = MakeProcInstance (DlgProc, hInstance) ;
         hDlg = CreateDialog (hInstance, szAppName, hWnd, lpfnDlgProc) ;
         break ;

      case WM_SETFOCUS:
         SetFocus (hDlg) ;
         break ;

      case WM_COMMAND:
         break ;

      case WM_DESTROY:
         FreeProcInstance (lpfnDlgProc) ;
         PostQuitMessage (0) ;
         break ;

      default:
         return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
   }

   return 0L ;
}

BOOL FAR PASCAL DlgProc (HWND hDlg, unsigned iMessage, WORD wParam, LONG lParam)
{
LONG        Seed ;
unsigned    iResult ;
PAINTSTRUCT ps ;
TEXTMETRIC  tm ;
HDC         hDC ;

char     szName [REG_NAME_LEN+1],
         szNum  [REG_NUM_LEN+1] ;

BOOL     bTranslated ;

   switch (iMessage)
   {
      case WM_INITDIALOG:
         SetDlgItemInt (hDlg, IDD_INFO, 42, FALSE) ;
         /* fall through */

      case WM_SETFOCUS:
         SetFocus (GetDlgItem (hDlg, IDD_NAME)) ;
         return FALSE ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDD_OK:
               Seed = (LONG) GetDlgItemInt (hDlg, IDD_INFO, (BOOL FAR *) &bTranslated, NULL) ;
               iResult = GetDlgItemText (hDlg, IDD_NAME, szName, REG_NAME_LEN) ;
               if (Seed == 0  || iResult == 0)
               {
                  MessageBeep (0) ;
                  break ;
               }
               CreateRegistration (szNum, szName) ;
               SetDlgItemText (hDlg, IDD_REGNUMBER, szNum) ;
               break ;

         }
         break ;

      case WM_PAINT :
         hDC = BeginPaint (hDlg, &ps) ;
         GetTextMetrics (hDC, &tm) ;
         DrawShadow (hDC, GetDlgItem (hDlg, IDD_NAME), CreateSolidBrush (RGB(0,255,0)), tm.tmAveCharWidth / 2) ;
         DrawShadow (hDC, GetDlgItem (hDlg, IDD_INFO), CreateSolidBrush (RGB(255,0,0)), tm.tmAveCharWidth / 2) ;
         DrawShadow (hDC, GetDlgItem (hDlg, IDD_OK), CreateSolidBrush (RGB(225,225,0)), tm.tmAveCharWidth / 2) ;
         EndPaint (hDlg, &ps) ;
         return FALSE ;
         break ;

      default:
         return FALSE ;
   }
   return TRUE ;
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
   TEXTMETRIC tm ;

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
   pWndClass->cbWndExtra        = 0 ;
   pWndClass->hInstance         = hInstance ;
   pWndClass->hIcon             = LoadIcon (hInstance, szAppName) ;
   pWndClass->hCursor           = LoadCursor (NULL, IDC_ARROW) ;
   pWndClass->hbrBackground     = GetStockObject (WHITE_BRUSH) ;
   pWndClass->lpszMenuName      = NULL ;
   pWndClass->lpszClassName     = szAppName ;

   bSuccess = RegisterClass (pWndClass) ;

   LocalUnlock (hMemory) ;
   LocalFree (hMemory) ;

   return (bSuccess) ;
}





