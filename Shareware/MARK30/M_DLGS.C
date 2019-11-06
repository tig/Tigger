// m_dlgs.c

#include "markver.h"
#pragma comment (exestr, "File " __FILE__ ", Version " VERSION ", Last modified on " __TIMESTAMP__)
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "mark30.h"
#include "mark30d.h" // resource identifiers

extern char szAppName [] ;
extern char szVerNum [] ;
extern char szFileName [] ;
char *msgs [] =
{
   "The application you have selected, %s, may have been designed for a previous version of Windows.",
   "If you press the \"Mark The File\" button, %s will be marked as a Windows 3.0 compatible application.  DO THIS AT YOUR OWN RISK!!!",
   "If you mark %s for Windows 3.0 and attempt to run it, you might encounter compatiblity problems that could cause the application or Windows to terminate unexpectedly!"
} ;

BOOL FAR PASCAL AboutDlgProc (HWND hDlg, unsigned iMessage, WORD wParam, LONG lParam) 
{
char  szTemp [64] ;
HDC   hDC ;
PAINTSTRUCT ps ;
TEXTMETRIC  tm ;

   switch (iMessage)
   {
      case WM_INITDIALOG:
            wsprintf( (LPSTR)szTemp,
                     (LPSTR)"Mark Three-Oh\nVersion %s",
                     (LPSTR)szVerNum ) ;
            SetDlgItemText (hDlg, IDD_APPNAME, szTemp) ;
            wsprintf( (LPSTR)szTemp, "About Mark Three-Oh" ) ;
            SetWindowText( hDlg, szTemp ) ;
            break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK:
               EndDialog (hDlg, 0) ;
               break ;

            default :
               return FALSE ;
         }
         break ;

      case WM_PAINT :
         hDC = BeginPaint (hDlg, &ps) ;
         GetTextMetrics (hDC, &tm) ;
         DrawShadow (hDC, GetDlgItem (hDlg, IDD_BOX0), GetStockObject(GRAY_BRUSH), tm.tmAveCharWidth / 2) ;
         EndPaint (hDlg, &ps) ;
         return FALSE ;

      default:
         return FALSE ;
   }
   return TRUE ;
}


BOOL FAR PASCAL WarnDlgProc (HWND hDlg, unsigned iMessage, WORD wParam, LONG lParam) 
{
   static DWORD dwBrush ;
   char  szTemp [256] ;
   HDC   hDC ;
   PAINTSTRUCT ps ;
   TEXTMETRIC  tm ;
   short i, c ;
   static BOOL  bTimer ;

   switch (iMessage)
   {
      case WM_INITDIALOG:
            bTimer = SetTimer (hDlg, 1, 300, NULL) ;
            c = 0 ;
            MessageBeep (0) ;
            for (i = 0 ; i <= 2 ; i++)
            {
               wsprintf( (LPSTR)szTemp, (LPSTR)msgs[i], (LPSTR)szFileName ) ;
               SetDlgItemText (hDlg, i+301, szTemp) ;
            }
            MessageBeep(0) ;
            break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDD_MARK:
               if (bTimer)
                  KillTimer (hDlg, 1) ;
               EndDialog (hDlg, 1) ;
               break ;

            case IDD_DONT:
               if (bTimer)
                  KillTimer (hDlg, 1) ;
               EndDialog (hDlg, 0) ;
               break ;

            default :
               return FALSE ;
         }
         break ;


      case WM_PAINT :
         hDC = BeginPaint (hDlg, &ps) ;
         GetTextMetrics (hDC, &tm) ;
         DrawShadow (hDC, GetDlgItem (hDlg, IDD_BOX0), CreateSolidBrush (RGB(0,0,255)), tm.tmAveCharWidth / 2) ;
         DrawShadow (hDC, GetDlgItem (hDlg, IDD_BOX1), CreateSolidBrush (RGB(255,0,0)), tm.tmAveCharWidth / 2) ;
         EndPaint (hDlg, &ps) ;
         return FALSE ;

      case WM_TIMER :
         switch (c)
         {
            case 0 :
               dwBrush = RGB (255,0,0) ;
               c = 1 ;
               break;
            case 1 :
               dwBrush = RGB (0,255,0) ;
               c = 2 ;
               break;
            case 2 :
               dwBrush = RGB (255,255,0) ;
               c = 0 ;
               break;
         }
         hDC = GetDC (GetDlgItem (hDlg, IDD_WARN)) ;
         ColorText (hDC, GetDlgItem (hDlg, IDD_WARN), dwBrush, "WARNING! WARNING! WARNING! WARNING!") ;
         ReleaseDC (GetDlgItem (hDlg, IDD_WARN), hDC) ;
         break;

      default:
         return FALSE ;
   }
   return TRUE ;
}

