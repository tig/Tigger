//=======================================================================
//
//    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
//
//-----------------------------------------------------------------------
//
//     Project:  CMNDLL, Common DLL for Werner Apps
//
//      Module:  test2.c  Test #2 for cmndll
//
//   Author(s):  Brian A. Woodruff (baw)
//               Charles E. Kindel, Jr. (cek)
//
//=======================================================================
#include "windows.h"	  
#include "test2.h"	  
#include "test2d.h"
#include "cmndll.h" 


#define FONTHEIGHT 13
#define FONT "Tms Rmn"

HANDLE   hInst;

char     szAppName [] = "Test2" ;

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
   BOOL      b ;

   wc.style         = 0 ;      
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

   b = RegisterClass( &wc ) ;
   if (!b)
      return FALSE ;

   return TRUE ;
}

BOOL InitInstance(   HANDLE hInstance,
                     int    nCmdShow )
{
   HWND            hWnd;               /* Main window handle.                */

   int  x, y ;


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
   static   LOGFONT  lf, lfDefault ;

   static   WORD wFontSelFlags = FONTSEL_DEFAULT ;
   static   BOOL rgfFontSelFlags[IDM_NOSTRIKEOUT-IDM_USEPRINTER+1] =
            {
               FONTSEL_USEPRINTER ,
               FONTSEL_NOPRINTER  ,
               FONTSEL_NOFGCOLOR  ,
               FONTSEL_NOBGCOLOR  ,
               FONTSEL_NODEFAULT  ,
               FONTSEL_NOBOLD     ,
               FONTSEL_NOITALIC   ,
               FONTSEL_NOUNDERLINE,
               FONTSEL_NOSTRIKEOUT
            } ;
   static   WORD wFontSelType = DEFAULT_FONTTYPE ;
   static   BOOL rgfFontSelType[IDM_FIXED-IDM_RASTER+1] =
            {
               RASTER_FONTTYPE,
               DEVICE_FONTTYPE,

               ANSI_FONTTYPE,     
               SYMBOL_FONTTYPE,   
               SHIFTJIS_FONTTYPE, 
               OEM_FONTTYPE,      

               VARIABLE_FONTTYPE, 
               FIXED_FONTTYPE
            } ;

   HDC hDC ;

   switch (iMessage)
   {
      case WM_CREATE:
      {
         unsigned int  i ;

         for (i = IDM_RASTER ; i <= IDM_FIXED ; i++)
         {
            if (wFontSelType & rgfFontSelType[i - IDM_RASTER])
               CheckMenuItem( GetMenu( hWnd ),
                              i,
                              MF_BYCOMMAND | MF_CHECKED ) ;
            else
               CheckMenuItem( GetMenu( hWnd ),
                              i,
                              MF_BYCOMMAND | MF_UNCHECKED ) ;
         }

         for (i = IDM_USEPRINTER ; i <= IDM_NOSTRIKEOUT ; i++)
         {
            if (wFontSelFlags & rgfFontSelFlags[i - IDM_USEPRINTER])
               CheckMenuItem( GetMenu( hWnd ),
                              i,
                              MF_BYCOMMAND | MF_CHECKED ) ;
            else
               CheckMenuItem( GetMenu( hWnd ),
                              i,
                              MF_BYCOMMAND | MF_UNCHECKED ) ;
         }


         hDC = GetDC (hWnd) ;

         lfDefault.lfHeight = FONTHEIGHT ;
         lfDefault.lfWeight = 700 ;    // bold
         lfDefault.lfItalic = TRUE ;
         lstrcpy( lfDefault.lfFaceName, FONT ) ;

         D(dp("FontSelectIndirect...")) ;
         FontSelectIndirect( hDC, &lfDefault ) ;

         lf = lfDefault ;

         ReleaseDC (hWnd, hDC) ;
      }
      break ;

	   case WM_COMMAND:
         switch (wParam)
         {
            case IDM_SELFONT:
            {
               WORD w ;

               w = FontSelectDialog( hWnd, "Select-A-Font-O-Rama",
                                 NULL,
                                 wFontSelType,
                                 wFontSelFlags,
                                 &lf, &lfDefault ) ;
            }
            break ;

            case IDM_DEFAULT:
            {
               int i ;

               wFontSelType = DEFAULT_FONTTYPE ;
               for (i = IDM_RASTER ; i <= IDM_FIXED ; i++)
               {
                  if (wFontSelType & rgfFontSelType[i - IDM_RASTER])
                     CheckMenuItem( GetMenu( hWnd ),
                                    i,
                                    MF_BYCOMMAND | MF_CHECKED ) ;
                  else
                     CheckMenuItem( GetMenu( hWnd ),
                                    i,
                                    MF_BYCOMMAND | MF_UNCHECKED ) ;
               }
               wFontSelFlags = FONTSEL_DEFAULT ;
               for (i = IDM_USEPRINTER ; i <= IDM_NOSTRIKEOUT ; i++)
               {
                  if (wFontSelFlags & rgfFontSelFlags[i - IDM_USEPRINTER])
                     CheckMenuItem( GetMenu( hWnd ),
                                    i,
                                    MF_BYCOMMAND | MF_CHECKED ) ;
                  else
                     CheckMenuItem( GetMenu( hWnd ),
                                    i,
                                    MF_BYCOMMAND | MF_UNCHECKED ) ;
               }
            }
            break ;


            case IDM_RASTER:
            case IDM_DEVICE:
            case IDM_ANSI:
            case IDM_SYMBOL:
            case IDM_SHIFTJIS:
            case IDM_OEM:
            case IDM_VARIABLE:
            case IDM_FIXED:
               if (wFontSelType & rgfFontSelType[wParam - IDM_RASTER])
               {
                  CheckMenuItem( GetMenu( hWnd ),
                                 wParam,
                                 MF_BYCOMMAND | MF_UNCHECKED ) ;
                  wFontSelType ^= rgfFontSelType[wParam - IDM_RASTER] ;
               }
               else
               {
                  CheckMenuItem( GetMenu( hWnd ),
                                 wParam,
                                 MF_BYCOMMAND | MF_CHECKED ) ;
                  wFontSelType |= rgfFontSelType[wParam - IDM_RASTER] ;
               }
            break ;

			   case IDM_USEPRINTER :
			   case IDM_NOPRINTER  :
			   case IDM_NOFGCOLOR  :
			   case IDM_NOBGCOLOR  :
			   case IDM_NODEFAULT  :
			   case IDM_NOBOLD     :
			   case IDM_NOITALIC   :
			   case IDM_NOUNDERLINE:
			   case IDM_NOSTRIKEOUT:
               if (wFontSelFlags & rgfFontSelFlags[wParam - IDM_USEPRINTER])
               {
                  CheckMenuItem( GetMenu( hWnd ),
                                 wParam,
                                 MF_BYCOMMAND | MF_UNCHECKED ) ;
                  wFontSelFlags ^= rgfFontSelFlags[wParam - IDM_USEPRINTER] ;
               }
               else
               {
                  CheckMenuItem( GetMenu( hWnd ),
                                 wParam,
                                 MF_BYCOMMAND | MF_CHECKED ) ;
                  wFontSelFlags |= rgfFontSelFlags[wParam - IDM_USEPRINTER] ;
               }
            break ;


            case IDM_FILEEXIT:
               SendMessage( hWnd, WM_CLOSE, 0, 0L ) ;
               break ;

            case IDM_HELPABOUT:
               {
               char szVersion [64] ;
               VerMakeVersionNumber( szVersion, 99, 99, 999 ) ;
               AboutDialog(   hWnd,
                              "cmndll.DLL TestApp (2)",
                              szAppName,
                              szVersion,
                              "Blah! Blah! Blah!",
                              ABOUT_STANDARD ) ;
               }
               break;

            default:
               return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
         }
         break ;

	   case WM_DESTROY:
      {
	      PostQuitMessage( 0 ) ;
      }
	   break;

	   default:		
	      return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
   }
   return 0L ;
}


//=======================================================================
// End of File: test2.c
//=======================================================================
