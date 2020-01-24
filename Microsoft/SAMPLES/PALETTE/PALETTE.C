// palette.c 
//
// Palette demonstration
//

#include "windows.h"      
#include "palette.h"      
#include "paletted.h" 
//#include "\windev\dprintf.c"


#define NUMENTRIES   128
#define STEP         64

HANDLE         hInst;
char           szAppName [] = "PaletteDemo" ;
char           szPopupClass[] = "PopupWindow" ;
HPALETTE       hPal, hOldPal, hNewPal ;
PALETTEENTRY   PalEntry[NUMENTRIES] ;
RECT           rClient ;
int            red, blue, green, curpoint ;
HWND           hWndMain ;
BOOL           bAnimate ;

void PeekYield( void ) ;
long FAR PASCAL PopupWndProc (HWND, unsigned, WORD, LONG) ;

int PASCAL WinMain(  HANDLE hInstance,
                     HANDLE hPrevInstance,
                     LPSTR lpCmdLine,         
                     int nCmdShow )           
{
    MSG msg;
         
//   dprintf(" ********** %s *********** \n", (LPSTR)szAppName ) ;            
   if (!hPrevInstance)  
       if (!InitApplication(hInstance)) 
           return (FALSE);

   if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

   while (GetMessage(&msg, NULL, NULL, NULL))    
    {
      TranslateMessage(&msg) ;
        DispatchMessage(&msg) ;   
   }
//   dprintf( "----------- Normal Exit ------------\n" );
   return (msg.wParam) ;
}



BOOL InitApplication( HANDLE hInstance )
{
   WNDCLASS  wc;

   wc.style         = NULL ;      
   wc.lpfnWndProc   = MainWndProc;  
   wc.cbClsExtra    = 0;             
   wc.cbWndExtra    = 0;             
   wc.hInstance     = hInstance;      
   wc.hIcon         = LoadIcon( hInstance, szAppName );
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = GetStockObject( WHITE_BRUSH ); 
   wc.lpszMenuName  = szAppName;
   wc.lpszClassName = szAppName;

   if (!RegisterClass(&wc))
      return FALSE ;

   wc.style          = CS_HREDRAW | CS_VREDRAW ;
   wc.lpfnWndProc    = PopupWndProc ;
   wc.cbClsExtra     = 0 ;
   wc.cbWndExtra     = 0 ;
   wc.hInstance      = hInstance ;
   wc.hIcon          = NULL ;
   wc.hCursor        = LoadCursor (NULL, IDC_ARROW) ;
   wc.hbrBackground  = GetStockObject (LTGRAY_BRUSH ) ;
   wc.lpszMenuName   = szPopupClass ;
   wc.lpszClassName  = szPopupClass ;

   return (RegisterClass(&wc));

}


BOOL InitInstance(   HANDLE hInstance,
                     int    nCmdShow )
{
   HWND            hWnd;
   int xScreen, yScreen ;

   hInst = hInstance;

   hWnd = CreateWindow(   szAppName,
                        szAppName,   
                        WS_OVERLAPPEDWINDOW,  
                        0, 0,
                        CW_USEDEFAULT, 0,
                        NULL,                 
                        NULL,                 
                        hInstance,            
                        NULL                  
                     ) ;

   if (!hWnd)
      return (FALSE);

   ShowWindow(hWnd, nCmdShow) ;
   UpdateWindow(hWnd) ;         

   hWndMain = hWnd ;

   xScreen = GetSystemMetrics (SM_CXSCREEN) ;
   yScreen = GetSystemMetrics (SM_CYSCREEN) ;

   hWnd = CreateWindow (   szPopupClass,
                           "Overlapped  With out Parent",
                           WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
                           WS_THICKFRAME |WS_MAXIMIZEBOX,
                           xScreen / 9, yScreen / 8,
                           xScreen / 3, yScreen / 2,
                           hWndMain,
                           NULL,
                           hInstance,
                           NULL ) ;

   ShowWindow (hWnd, SW_SHOWNORMAL) ;
   UpdateWindow (hWnd) ;

   hWnd = CreateWindow (   szPopupClass,
                           "Popup Without Parent",
                           WS_POPUP | WS_CAPTION | WS_SYSMENU |
                           WS_THICKFRAME |WS_MAXIMIZEBOX,
                           5 * xScreen / 9, yScreen / 8,
                           xScreen / 3, yScreen / 2,
                           NULL,
                           NULL,
                           hInstance,
                           NULL ) ;

   ShowWindow (hWnd, SW_SHOWNORMAL) ;
   UpdateWindow (hWnd) ;

   return (TRUE) ;
}


long FAR PASCAL MainWndProc( HWND hWnd,
                             unsigned message,
                             WORD wParam,
                             LONG lParam )
{
   FARPROC     lpProcAbout ;
   PAINTSTRUCT ps ;
   WORD        nNumColors ;
   HDC         hDC ;
   int         i ;
   LOGPALETTE  *pPal ;

   switch (message)
   {
      case WM_CREATE :
         bAnimate = FALSE ;
         hWndMain = hWnd ;
         nNumColors = NUMENTRIES ;
         // Allocate for logical palette structure
         pPal = (LOGPALETTE*)LocalAlloc( LPTR,
                sizeof(LOGPALETTE) + nNumColors * sizeof( PALETTEENTRY ) ) ;
         if (!pPal)
            return NULL ;

         pPal->palNumEntries = nNumColors ;
         pPal->palVersion    = 0x300 ;

         // Fill in the palette entries

         for (i = 0, red = 0, green = 128, blue = 128 ;
              i < NUMENTRIES ;
              i++, red++, green++, blue++)
         {
            PalEntry[i].peRed   = pPal->palPalEntry[i].peRed   = LOBYTE( red ) ;
            PalEntry[i].peGreen = pPal->palPalEntry[i].peGreen = LOBYTE( green ) ;
            PalEntry[i].peBlue  = pPal->palPalEntry[i].peBlue  = LOBYTE( blue ) ;
            pPal->palPalEntry[i].peFlags = PC_RESERVED ;
         }


         hPal = CreatePalette( pPal ) ;
         LocalFree( (HANDLE)pPal ) ;
         hDC = GetDC( hWnd ) ;

         pPal = (LOGPALETTE*)LocalAlloc( LPTR,
                sizeof(LOGPALETTE) + nNumColors * sizeof( PALETTEENTRY ) ) ;
         if (!pPal)
            return NULL ;

         pPal->palNumEntries = 1 ;
         pPal->palVersion    = 0x300 ;
         pPal->palPalEntry[NUMENTRIES - 1].peRed   = 255 ;
         pPal->palPalEntry[NUMENTRIES - 1].peGreen = 255 ;
         pPal->palPalEntry[NUMENTRIES - 1].peBlue  = 255 ;
         pPal->palPalEntry[NUMENTRIES - 1].peFlags = NULL ;

         hNewPal = CreatePalette( pPal ) ;
         LocalFree( (HANDLE)pPal ) ;

         hOldPal = SelectPalette( hDC, hPal, FALSE ) ;
         RealizePalette( hDC ) ;
         ReleaseDC( hWnd, hDC ) ;
         red = green = blue = TRUE ;
         break ;

       case WM_COMMAND:   
         switch (wParam)
         {
             case IDM_ABOUT:
               lpProcAbout = MakeProcInstance(About, hInst);
                DialogBox(  hInst,      
                          "AboutBox",    
                           hWnd,             
                           lpProcAbout );
               FreeProcInstance(lpProcAbout);
               break;

            case IDM_ANIMATE:
//               if (bAnimate)
//               {
//                  bAnimate = FALSE ;
//                  ModifyMenu( GetMenu( hWnd ),
//                              IDM_ANIMATE,
//                              MF_STRING  | MF_BYCOMMAND,
//                              IDM_ANIMATE,
//                              "&Animate!" ) ;
//                  DrawMenuBar( hWnd ) ;
//               }
//               else
//               {
//                  bAnimate = TRUE ;
//                  ModifyMenu( GetMenu( hWnd ),
//                              IDM_ANIMATE,
//                              MF_STRING  | MF_BYCOMMAND,
//                              IDM_ANIMATE,
//                              "&Stop Animation!" ) ;
//                  DrawMenuBar( hWnd ) ;
//               }
                  hDC = GetDC( hWnd ) ;
                  DoAnimate( hWnd, hDC ) ;
                  ReleaseDC (hWnd, hDC ) ;
                  hDC = 0 ;
               break ;

            case IDM_PALETTE:
               hDC = GetDC( hWnd ) ;
               DoPalette( hWnd, hDC ) ;
               ReleaseDC( hWnd, hDC ) ;
               break ;

            default:
               return DefWindowProc( hWnd, message, wParam, lParam ) ;
         }
         break ;

      case WM_PAINT:
         hDC = BeginPaint( hWnd, &ps ) ;
         DoPalette( hWnd, hDC ) ;
         EndPaint( hWnd, &ps ) ;
         break ;

      case WM_DESTROY :
         bAnimate = FALSE ;
         if (hDC)
            ReleaseDC( hWnd, hDC ) ;
         DeleteObject( hPal ) ;
         PostQuitMessage( 0 ) ;
         break ;

       default:           /* Passes it on if unproccessed    */
          return (DefWindowProc(hWnd, message, wParam, lParam));
   }
   return 0L ;
}


void DoPalette( HWND hWnd, HDC hDC )
{
   int i ;
   BYTE color ;
   int row ;
   RECT  rect ;
   HBRUSH hBrush ;

   GetClientRect( hWnd, (LPRECT)&rClient ) ;
   SelectPalette( hDC, hPal, FALSE ) ;
   RealizePalette( hDC ) ;

   color = 64 ;
   hBrush = CreateSolidBrush( PALETTEINDEX( color )) ;

   SetRectEmpty( &rect ) ;
   row = 0 ;
   rect.right = rClient.right ;
   rect.left = 0 ;
   while (rect.bottom < rClient.bottom)
   {
      for (i = 0, color = 0 ; i < NUMENTRIES ; i++)
      {
         rect.top = row * 2 ;
         rect.bottom = row * 2 + 2 ;
         if (rect.bottom >= rClient.bottom)
            break ;
         hBrush = CreateSolidBrush( PALETTEINDEX( color )) ;
         FillRect( hDC, &rect, hBrush ) ;
         DeleteObject( hBrush ) ;
         color++ ;
         row++ ;
      }
   }
}

//void PeekYield( void )
//{
//   MSG msg;
//
//   if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
//   {
//      if( !IsDialogMessage( hWndMain, &msg ) ) 
//      {
//         TranslateMessage( (LPMSG)&msg );
//         DispatchMessage( (LPMSG)&msg );
//      }
//   }
//} /* PeekYield() */

void DoAnimate( HWND hWnd, HDC hDC )
{
   int i, row ;

   SelectPalette( hDC, hPal, FALSE ) ;

   for ( row = 0 ; row < STEP ; row++)
   {
      for (i = 0 ; i < NUMENTRIES ; i++)
      {
         if (red)
         {
            if (++PalEntry[i].peRed == 255)
               red = FALSE ;
         }
         else
         {
            if (--PalEntry[i].peRed == 0)
               red = TRUE ;
         }

         if (green)
         {
            if (++PalEntry[i].peGreen == 255)
               green = FALSE ;
         }
         else
         {
            if (--PalEntry[i].peGreen == 0)
               green = TRUE ;
         }

         if (blue)
         {
            if (++PalEntry[i].peBlue == 255)
               blue = FALSE ;
         }
         else
         {
            if (--PalEntry[i].peBlue == 0)
               blue = TRUE ;
         }
         PalEntry[i].peFlags = PC_RESERVED ;
      }
   }
   AnimatePalette( hPal, 0, NUMENTRIES, (LPPALETTEENTRY) &PalEntry[0] ) ;
}


BOOL FAR PASCAL About( HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
   HDC hDC ;

   switch (message)
   {
      case WM_INITDIALOG:
         return (TRUE);

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK :
            case IDCANCEL :
               hDC = GetDC( hWndMain ) ;
               DoAnimate( hWndMain, hDC ) ;
               ReleaseDC( hWndMain, hDC ) ;
               EndDialog(hDlg, TRUE) ;
               return (TRUE);

            case IDD_ANIMATE :
               break ;
          }
        break;
    }

    return (FALSE);   
}

long FAR PASCAL PopupWndProc (HWND hWnd, unsigned iMessage, WORD wParam, LONG lParam ) 
{
   HDC hDC ;
   switch (iMessage)
   {
      case WM_COMMAND :
         switch (wParam)
         {
            case IDM_ANIMATE:
               hDC = GetDC( hWndMain ) ;
               DoAnimate( hWndMain, hDC ) ;
               ReleaseDC( hWndMain, hDC ) ;
               break ;

            default :
               return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
         }
         break ;

      default :
         return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
   }
   return 0L ;
}   

