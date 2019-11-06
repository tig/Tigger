#include "markver.h"
#pragma comment (exestr, "File " __FILE__ ", Version " VERSION ", Last modified on " __TIMESTAMP__)
#include <windows.h>
#include <math.h>
#include "mark30d.h"
#include "mark30.h"

extern WORD wBits ;

BOOL FAR PASCAL AdvDlgProc (HWND hDlg, unsigned iMessage, WORD wParam, LONG lParam) 
{
#ifdef USEADVANCED
   HDC   hDC ;
   PAINTSTRUCT ps ;
   TEXTMETRIC  tm ;
   int i ;

   switch (iMessage)
   {
      case WM_INITDIALOG:
            for (i = IDD_BIT0 ; i <= IDD_BIT15 ; i++)
               CheckDlgButton (hDlg, i, bit(i - IDD_BIT0, wBits)) ;
            break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK:
               EndDialog (hDlg, 1) ;
               wBits = 0 ;
               for (i = IDD_BIT0 ; i <= IDD_BIT15 ; i++)
                  if (IsDlgButtonChecked (hDlg, i))
                     wBits = wBits | (WORD)pow (2, i - IDD_BIT0) ;
               break ;

            case IDD_CANCEL:
               EndDialog (hDlg, 0) ;
               break ;

            default :
               if (wParam >= IDD_BIT0 && wParam <= IDD_BIT15)
                  CheckDlgButton (hDlg, wParam, !IsDlgButtonChecked (hDlg, wParam)) ;
               return FALSE ;
         }
         break ;

      case WM_PAINT :
         hDC = BeginPaint (hDlg, &ps) ;
         GetTextMetrics (hDC, &tm) ;
         DrawShadow (hDC, GetDlgItem (hDlg, IDD_BOX0), GetStockObject(GRAY_BRUSH), tm.tmAveCharWidth / 2) ;
         DrawShadow (hDC, GetDlgItem (hDlg, IDD_BOX1), CreateSolidBrush(RGB(255,0,0)), tm.tmAveCharWidth / 2) ;
         EndPaint (hDlg, &ps) ;
         return FALSE ;

      default:
         return FALSE ;
   }
   return TRUE ;
#endif
}

