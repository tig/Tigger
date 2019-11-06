// test 
//

#include "windows.h"	  
#include "test.h"	  
#include "testd.h"
#include "..\inc\ceklib.h" 

HANDLE   hInst;
char     szAppName [] = "Test" ;
char     szDescription [] =
"Test application to exercise CEKLIB.DLL.  \
If this were a real application the user would \
have to pay a registration fee of $25.00." ;
WORD     wFileOpenFlags =      DLGOPEN_MUSTEXIST |
                               DLGOPEN_OPEN ;

void DoSelPrn( HWND hWnd, WORD wFlags ) ;
BOOL PaintInfo( HDC hDC, int xChar, int yChar ) ;
BOOL FAR PASCAL GenDlgProc( HWND, unsigned, WORD, LONG ) ;

BOOL FAR PASCAL
GetKeyStates( BOOL *bCapsLock, BOOL *bNumLock, BOOL *bScrollLock ) ;

char szDevice[256] ;
char szDriver[256] ;
char szPort[256] ;

char szName[REG_MAX_NAME+1] ;
char szNumber[REG_MAX_NUMBER+1] ;

char szFileName[80] ;
char szFileSpec[80] ;


BOOL  bCapsLock ;
BOOL  bNumLock ;
BOOL  bScrollLock ;
BOOL  fAllowSetup = TRUE ;

int PASCAL WinMain(  HANDLE hInstance,
                     HANDLE hPrevInstance,
                     LPSTR lpCmdLine,		  
                     int nCmdShow )			  
{
   MSG msg;			  
                        
   if (!hPrevInstance)	
	   if (!InitApplication( hInstance )) 
	       return FALSE ;	  


   if (!InitInstance( hInstance, nCmdShow ))
        return FALSE ;

   while (GetMessage( &msg, NULL, NULL, NULL ))	 
  	{
      TranslateMessage( &msg ) ;
     	DispatchMessage( &msg ) ;	  
   }
   return msg.wParam ;	  
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
    wc.hbrBackground = GetStockObject( GRAY_BRUSH ); 
    wc.lpszMenuName  = szAppName;
    wc.lpszClassName = szAppName;

    /* Register the window class and return success/failure code. */

    return (RegisterClass(&wc));

}


BOOL InitInstance(   HANDLE hInstance,
                     int    nCmdShow )
{
   HWND            hWnd;               /* Main window handle.                */

   int             x, y ;
   short           i ;

   hInst = hInstance;

   x = GetSystemMetrics( SM_CXSCREEN ) ;
   y = GetSystemMetrics( SM_CYSCREEN ) ;

   hWnd = CreateWindow(   szAppName,
                        szAppName,   
                        WS_OVERLAPPEDWINDOW,  
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, y / 3,
                        NULL,                 
                        NULL,                 
                        hInstance,            
                        NULL                  
                     ) ;


   if (!hWnd)
       return FALSE ;

   ShowWindow( hWnd, nCmdShow ) ;

   UpdateWindow( hWnd ) ;

   return TRUE ;             
}

long FAR PASCAL MainWndProc( HWND     hWnd, 
                             unsigned iMessage,  
                             WORD wParam,			 
                             LONG lParam )			  
{
   HDC      hDC ;
   HANDLE   hFont, hOldFont ;
   TEXTMETRIC tm ;
   PAINTSTRUCT ps ;
   static   int xChar, yChar ;
static  WORD     Flag[] =
            {
               DLGOPEN_MUSTEXIST,
               DLGOPEN_NOSHOWSPEC,
               DLGOPEN_SHOWDEFAULT,
               DLGOPEN_SAVE,
               DLGOPEN_OPEN,
               DLGOPEN_NOBEEPS,
               DLGOPEN_SEARCHPATH,
               DLGOPEN_NOCHANGESPEC
            } ;
   int      i ;

   switch (iMessage)
   {
      case WM_CREATE:
         lstrcpy (szDevice, "szDevice" ) ;
         lstrcpy( szDriver, "szDriver" ) ;
         lstrcpy( szPort, "szPort" ) ;

         hDC = GetDC (hWnd) ;
         hFont = CreateFont(  8, 8, 0, 0, 0, 0, 0, 0, ANSI_CHARSET,
                              OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS,
                              PROOF_QUALITY,
                              FIXED_PITCH,
                              "Courier" ) ;

         hOldFont = SelectObject( hDC, hFont ) ;

         GetTextMetrics( hDC, &tm ) ;
         xChar = tm.tmAveCharWidth ;
         yChar = tm.tmHeight + tm.tmExternalLeading ;
         DeleteObject (hOldFont) ;
         ReleaseDC (hWnd, hDC) ;

         if (!IsRegisteredUser( szAppName, szName, szNumber, REGISTER_READ ))
         {
            lstrcpy( szName, "none" ) ;
            lstrcpy( szNumber, "WWXXYYZZ" ) ;
         }

         lstrcpy( szFileSpec, "*.c" ) ;
         for (i = IDM_FILEOPEN ; i < IDM_SEARCHPATH ; i++)
         {
            if (wFileOpenFlags & Flag[i - (IDM_FILEOPEN)])
            {
               CheckMenuItem( GetMenu( hWnd ),
                              i+1,
                              MF_BYCOMMAND | MF_CHECKED ) ;
            }
         }

         break ;

	   case WM_COMMAND:
         switch (wParam)
         {
            case IDM_ABOUT:
               {
               char szVersion [64] ;
               VerMakeVersionNumber( szVersion, 99, 99, 999 ) ;
               AboutDialog(   hWnd,
                              "CEKLIB.DLL TestApp",
                              szAppName,
                              szVersion,
                              szDescription,
                              ABOUT_STANDARD ) ;
               }
               break;

            case IDM_BIGABOUT:
               {
               char szVersion [64] ;
               VerMakeVersionNumber( szVersion, 99, 99, 999 ) ;
               AboutDialog(   hWnd,
                              "CEKLIB.DLL TestApp",
                              szAppName,
                              szVersion,
                              szDescription,
                              ABOUT_BIG ) ;
               }
               break;

            case IDM_REGABOUT:
               {
               char szVersion [64] ;
               VerMakeVersionNumber( szVersion, 99, 99, 999 ) ;
               AboutDialog(   hWnd,
                              "CEKLIB.DLL TestApp",
                              szAppName,
                              szVersion,
                              szDescription,
                              ABOUT_REGISTERED ) ;
               }
               break;


            case IDM_REG:
               RegistrationDialog( hWnd,
                                   szAppName,
                                   0 ) ;
               break;

            case IDM_GENNUM:
            {
               FARPROC lpfnDlgProc ;

               lpfnDlgProc = MakeProcInstance( GenDlgProc, hInst ) ;

               DialogBox(  hInst,		
                           "GENNUMBER",
                           hWnd,			 
                           lpfnDlgProc ) ;

               FreeProcInstance( lpfnDlgProc ) ;
            }
            break ;


            case IDM_GETDEFAULT:
               DoSelPrn( hWnd, SELPRN_GETDEFAULT ) ;
               break ;

            case IDM_USEDEFAULT:
               if (fAllowSetup)
                  DoSelPrn( hWnd, SELPRN_STANDARD |
                                  SELPRN_USEDEFAULT |
                                  SELPRN_ALLOWSETUP) ;
               else
                  DoSelPrn( hWnd, SELPRN_STANDARD |
                                  SELPRN_USEDEFAULT ) ;
               break ;

            case IDM_STANDARD:
               if (fAllowSetup)
                  DoSelPrn( hWnd, SELPRN_STANDARD | SELPRN_ALLOWSETUP) ;
               else
                  DoSelPrn( hWnd, SELPRN_STANDARD ) ;
               break ;

            case IDM_ALLOWSETUP:
               if (fAllowSetup)
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              IDM_ALLOWSETUP,
                              MF_BYCOMMAND | MF_UNCHECKED ) ;
               }
               else
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              IDM_ALLOWSETUP,
                              MF_BYCOMMAND | MF_CHECKED ) ;
               }
               fAllowSetup = !fAllowSetup ;
               return 0L ;
               break ;

            case IDM_FILEOPEN:
            {
               int f ;
               D(dp("IDM_FILEOPEN: wFileOpenFlags == %#08X",
                                 wFileOpenFlags )) ;
               FileOpenDialog( hWnd,
                                 "Open a File",
                                 szFileSpec,
                                 wFileOpenFlags,
                                 szFileName,
                                 80 ) ;
               D(dp(" FileOpenDialog ==  %#08X", f)) ;
            }
            break ;

            case IDM_MUSTEXIST:
            case IDM_NOSHOWSPEC:
            case IDM_SHOWDEFAULT:
            case IDM_SAVE:
            case IDM_OPEN:
            case IDM_NOBEEPS:
            case IDM_SEARCHPATH:
            case IDM_NOCHANGESPEC:
               if (wFileOpenFlags & Flag[wParam - (IDM_FILEOPEN + 1)])
               {
                  CheckMenuItem( GetMenu( hWnd ),
                                 wParam,
                                 MF_BYCOMMAND | MF_UNCHECKED ) ;
                  wFileOpenFlags ^= Flag[wParam - (IDM_FILEOPEN + 1)] ;
               }
               else
               {
                  CheckMenuItem( GetMenu( hWnd ),
                                 wParam,
                                 MF_BYCOMMAND | MF_CHECKED ) ;
                  wFileOpenFlags |= Flag[wParam - (IDM_FILEOPEN + 1)] ;
               }
            break ;

            default:
               return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
         }
         InvalidateRect( hWnd, NULL, TRUE ) ;
         break ;

      case WM_PAINT:
      {
         RECT rect ;
         char  szBuf [80] ;
         HANDLE hOldFont ;

            hDC = BeginPaint (hWnd, &ps) ;

            hFont = CreateFont(  8, 8, 0, 0, 0, 0, 0, 0, ANSI_CHARSET,
                                 OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS,
                                 PROOF_QUALITY,
                                 FIXED_PITCH,
                                 "Courier" ) ;

            hOldFont = SelectObject( hDC, hFont ) ;

            SetBkMode( hDC, OPAQUE ) ;
            SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;
            PaintInfo( hDC, xChar, yChar) ;

            DeleteObject (hOldFont) ;
            EndPaint (hWnd, &ps) ;
      }
      break ;

	   case WM_DESTROY:
         D(dp( "WM_DESTROY" )) ;
	      PostQuitMessage( 0 ) ;
	      break;

	   default:		
	      return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
   }
   return 0L ;
}

void DoSelPrn( HWND hWnd, WORD wFlags )
{
   switch (PrintSelPrnDialog( hWnd,
                              (LPSTR)szAppName,
                              (LPSTR)szDriver,
                              (LPSTR)szDevice,
                              (LPSTR)szPort,
                              wFlags ))
   {
      case DLG_CANCEL:
         break ;

      case DLG_OK:
         InvalidateRect( hWnd, NULL, TRUE ) ;
         break ;

      case DLG_ERROR:
         MessageBox( hWnd, "Select Printer Failed.",
                           "Error", MB_ICONEXCLAMATION ) ;
         break ;
   }
}

BOOL PaintInfo( HDC hDC, int xChar, int yChar )
{
   char szBuffer[256] ;
   int x, y ;

   x = 1 ;
   y = 1 ;  

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Current Printer: %s (%s) on %s",
                      (LPSTR)szDevice,
                      (LPSTR)szDriver,
                      (LPSTR)szPort ) ) ;
   y++ ;

   if (!IsRegisteredUser( szAppName,
                          szName,
                          szNumber,
                          REGISTER_READ ))
      TextOut( hDC, xChar * x, yChar * y, szBuffer,
               wsprintf( szBuffer,
                        (LPSTR)"Not registered: %s, %s, %s",
                        (LPSTR)szAppName,
                        (LPSTR)szName,
                        (LPSTR)szNumber ) ) ;
   else
      TextOut( hDC, xChar * x, yChar * y, szBuffer,
               wsprintf( szBuffer,
                        (LPSTR)"Registered: %s, %s, %s",
                        (LPSTR)szAppName,
                        (LPSTR)szName,
                        (LPSTR)szNumber ) ) ;


   TextOut( hDC, xChar * x, yChar * ++y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Current FileName: %s", (LPSTR)szFileName )) ;
   TextOut( hDC, xChar * x, yChar * ++y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Current FileSpec: %s", (LPSTR)szFileSpec )) ;
   return TRUE ;
}


BOOL FAR PASCAL About(  HWND hDlg,
                        unsigned iMessage,
                        WORD wParam,
                        LONG lParam )
{
   switch (iMessage)
   {
      case WM_INITDIALOG:
         return TRUE ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK :
            case IDCANCEL :
   	   	   EndDialog( hDlg, TRUE ) ;
   	   	   return TRUE ;
	      }
	      break ;
    }

    return FALSE ;	  
}

BOOL FAR PASCAL
     Draw3DEdit( HDC      hDC,
                 HWND     hWnd ) ;

BOOL FAR PASCAL GenDlgProc(  HWND hDlg,
                        unsigned iMessage,
                        WORD wParam,
                        LONG lParam )
{
   switch (iMessage)
   {
      case WM_INITDIALOG:
         SendMessage( GetDlgItem( hDlg, IDD_NUM ),
                         EM_LIMITTEXT, REG_MAX_NUMBER, 0L ) ;
         SendMessage( GetDlgItem( hDlg, IDD_APPNAME ),
                         EM_LIMITTEXT, 8, 0L ) ;
         return TRUE ;
      break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK:
            {
               int iAppName, iName ;
               char szNewName[REG_MAX_NAME+1] ;
               char szNewNum[REG_MAX_NUMBER+1] ;
               char szNewAppName[8+1] ;

               iName = GetDlgItemText (hDlg, IDD_NAME, szNewName, REG_MAX_NAME) ;
               iAppName = GetDlgItemText (hDlg, IDD_APPNAME, szNewAppName, REG_MAX_NAME) ;
               if (iAppName == 0  || iName == 0)
               {
                  MessageBeep (0) ;
                  break ;
               }
               if (IsRegisteredUser( szNewAppName, szNewName,
                     szNewNum, REGISTER_GENERATE))
                  SetDlgItemText (hDlg, IDD_NUM, szNewNum) ;
               else
                  MessageBeep (0) ;
            }
            break ;

            case IDCANCEL:
               EndDialog( hDlg, TRUE ) ;
               return TRUE ;
         }
      break ;

      case WM_PAINT :
      {
         PAINTSTRUCT ps ;
         HDC         hDC ;

         hDC = BeginPaint(hDlg, &ps ) ;
         Draw3DEdit( hDC, GetDlgItem( hDlg, IDD_APPNAME ) ) ;
         Draw3DEdit( hDC, GetDlgItem( hDlg, IDD_NAME ) ) ;
         EndPaint( hDlg, &ps ) ;
         return FALSE ;
      }
      break ;
    }

    return FALSE ;	  
}

BOOL FAR PASCAL
   DrawCoolIndent( HDC hDC, RECT Rect, 
                            WORD wWidth ) ;

BOOL FAR PASCAL
     Draw3DEdit( HDC      hDC,
                 HWND     hWnd )
{
   LONG     Origin ;
   RECT     Rect ;
   WORD     ShadowWidth = 3 ;

   GetWindowRect( hWnd, (LPRECT)&Rect ) ; 
   Origin = GetDCOrg( hDC ) ;

   // First get rectangle of control
   Rect.left   -= LOWORD( Origin ) ;
   Rect.top    -= HIWORD( Origin ) ;
   Rect.right  -= LOWORD( Origin ) ;
   Rect.bottom -= HIWORD( Origin ) ;

   // Draw indented shadow
   DrawCoolIndent( hDC, Rect, ShadowWidth ) ;

   return TRUE ;
}


BOOL FAR PASCAL
   DrawCoolIndent( HDC hDC, RECT Rect,
                            WORD nWidth )
{
   POINT    Points[7] ;

   // Draw bottom and right
   Points [0].x = Rect.left ;
   Points [0].y = Rect.bottom ;

   Points [1].x = Rect.left - nWidth  ;
   Points [1].y = Rect.bottom + nWidth ;

   Points [2].x = Rect.right + nWidth ;
   Points [2].y = Rect.bottom + nWidth ;

   Points [3].x = Rect.right + nWidth ;
   Points [3].y = Rect.top - nWidth ;

   Points [4].x = Rect.right ;
   Points [4].y = Rect.top ;

   Points [5].x = Rect.right ;
   Points [5].y = Rect.bottom ;

   Points [6].x = Rect.left ;
   Points [6].y = Rect.bottom ;

   if (!SelectObject (hDC, GetStockObject (NULL_PEN)))
      return FALSE ;

   if (!SelectObject (hDC, GetStockObject( WHITE_BRUSH )))
      return FALSE ;

   if (!Polygon (hDC, (LPPOINT) &Points, 7))
      return FALSE ;

   // Draw left and top
   Points [0].x = Rect.left ;
   Points [0].y = Rect.bottom ;

   Points [1].x = Rect.left - nWidth  ;
   Points [1].y = Rect.bottom + nWidth ;

   Points [2].x = Rect.left - nWidth ;
   Points [2].y = Rect.top - nWidth ;

   Points [3].x = Rect.right + nWidth ;
   Points [3].y = Rect.top - nWidth ;

   Points [4].x = Rect.right ;
   Points [4].y = Rect.top ;

   Points [5].x = Rect.left ;
   Points [5].y = Rect.top ;

   Points [6].x = Rect.left ;
   Points [6].y = Rect.bottom ;

   if (!SelectObject (hDC, GetStockObject( GRAY_BRUSH )))
      return FALSE ;

   if (!Polygon (hDC, (LPPOINT) &Points, 7))
      return FALSE ;

   return TRUE ;
}

