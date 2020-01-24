// about.c
//
// About box routines for lexmac.DLL
//
// Copyright (C) 1990 Charles E. Kindel, Jr.
// 
// Modification history:
// 10/20/90 CEK   First version
// 11/19/90 CEK   Made use of window property lists.
//
#define WINDLL
#include <windows.h>
#include "..\lexmac.h"

//----------------------------------------------------------------
// Globals
//----------------------------------------------------------------
extern HANDLE hInst ;

//-----------------------------------------------------------------
// 
//
//   BOOL FAR PASCAL
//   AboutDialog( HWND    hwndParent )
//
//-----------------------------------------------------------------
BOOL FAR PASCAL
AboutDialog( )
{
   FARPROC      lpfnDlgProc ;
   int          n ;
   HWND         hwndParent = GetActiveWindow() ;

   D(dp( "hwndParent = %d", hwndParent )) ;

   lpfnDlgProc = MakeProcInstance( AboutDlgProc, hInst ) ;

   n = DialogBox(  hInst,		
                   "ABOUT",
                   hwndParent,			 
                   lpfnDlgProc ) ;

   FreeProcInstance( lpfnDlgProc ) ;

   return n ;
}


//-----------------------------------------------------------------
// About dlgproc
//
// 11/19/90 CEK   - Property lists
//-----------------------------------------------------------------
BOOL FAR PASCAL 
AboutDlgProc( HWND hDlg,
              unsigned iMessage,
              WORD wParam,
              LONG lParam ) 
{
   switch (iMessage)
   {
      case WM_INITDIALOG :
      {
         char           szVersion[16] ;
         char           szBuf[32] ;

         VerGetDLLVersion( szVersion ) ;

         wsprintf( szBuf, (LPSTR)"Version %s",
                          (LPSTR)szVersion ) ;
         SetDlgItemText( hDlg, IDD_VERSION, szBuf ) ;

         return TRUE ;
      }

      case WM_COMMAND :
         if (wParam == IDOK || wParam == IDCANCEL)
         {
            EndDialog (hDlg, TRUE) ;
            return TRUE ;
         }
         break ;

      case WM_PAINT :
      {
         PAINTSTRUCT ps ;
         TEXTMETRIC  tm ;
         HDC         hDC ;

         hDC = BeginPaint(hDlg, &ps ) ;
         GetTextMetrics( hDC, &tm ) ;
         DrawShadow( hDC, GetDlgItem( hDlg, IDD_BOX0 ),
                     GetStockObject( GRAY_BRUSH ), tm.tmAveCharWidth / 2 ) ;
         DrawShadow( hDC, GetDlgItem( hDlg, IDD_BOX1 ),
                     GetStockObject( GRAY_BRUSH ), tm.tmAveCharWidth / 2 ) ;
         EndPaint( hDlg, &ps ) ;
         return FALSE ;
      }
   }

   return FALSE ;
}

