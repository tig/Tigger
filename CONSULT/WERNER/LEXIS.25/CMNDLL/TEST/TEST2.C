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

//#define NOBOTSTAT
//#define NOTOPSTAT
#define NOBUTTONS

#define FONTHEIGHT 13
#define FONT "Tms Rmn"


HANDLE   hInst;

char     szAppName [] = "Test2" ;
char     szDescription [] = "Test application to exercise CMNDLL.DLL." ;

void DoSelPrn( HWND hWnd, WORD wFlags );
BOOL FAR PASCAL fnTest(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;

#ifndef NOBOTSTAT
   BOOL FAR PASCAL
   GetKeyStates( BOOL *bCapsLock, BOOL *bNumLock, BOOL *bScrollLock ) ;
   HWND  hWndBottom ;
   #define INDEX_CAPSLOCK     0
   #define INDEX_NUMLOCK      1
   #define INDEX_SCROLLLOCK   2
   #define INDEX_DATE         3
   #define INDEX_TIME         4

   #define NUM_BOTBOXES 5

   struct
   {
      char *szStat[2] ;
   }
   rgszStat [] =
   {
      "", "CAPS",
      "", "NUM",
      "", "SCRL",
      "Thursday, February 28, 1991", "DATE",
      "00:25:34 am","00:25:34 am"
   } ;
   #define NUM_MSGS 5

   WORD wMsg = 0 ;

   char *rgszMsg[] =
   {
      "This is Msg #1...",
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
      "1234567890!@#%$^&*()-+}{[]",
      "abcdefghijklmnopqrstuvwxyz",
      "jzypq"
   } ;
   BOOL  bCapsLock ;
   BOOL  bNumLock ;
   BOOL  bScrollLock ;
   BOOL  bBottom = TRUE ;
   BOOL  bVisible = TRUE ;
   #define ID_TIMER 1
#endif

#ifndef NOTOPSTAT
   HWND  hWndTop ;


   #define INDEX_CLIENT_LBL   0
   #define INDEX_CLIENT       1
   #define INDEX_LIB_LBL      2
   #define INDEX_LIB          3

   #define NUM_TOPBOXES 4

   struct
   {
      char *szTop[2] ;
   }
   rgszTop [] =
   {
      "Client:", "Client:",
      "XXXXXXXXXXXXXXXXXXXXXXXXX", "WinLexis Testing/CEK",
      "Library/File:", "Library/File:",
      "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", "Some Far Out Library"
   } ;

#endif 

#ifndef NOBUTTONS
   HWND  rghWndButton[IDB_END - IDB_BEGIN + 1] ;
#endif

static   WORD wStatusHeight ;


char szDevice[256] ;
char szDriver[256] ;
char szPort[256] ;
char szFileName[80] ;
char szFileSpec[80] ;

BOOL FAR PASCAL
fnLEDbox( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam) ;


BOOL PaintInfo( HDC hDC, int xChar, int yChar ) ;

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
   wc.hbrBackground = GetStockObject( WHITE_BRUSH ); 
   wc.lpszMenuName  = szAppName;
   wc.lpszClassName = szAppName;

   /* Register the window class and return success/failure code. */

   b = RegisterClass( &wc ) ;
   if (!b)
      return FALSE ;

   return TRUE ;
}

WORD  MyMax( WORD w1, WORD w2)
{
   return ((w1 > w2) ? w1 : w2) ;
}


void SetUpStatBox( HWND hWnd, WORD nID, WORD nWidth )
{
   SendMessage( hWnd, ST_SETSTATBOXSIZE, nID, (LONG)nWidth ) ;
}

void SetStatus( HWND hWnd, WORD nID, LPSTR lpszBuf )
{
   SendMessage( hWnd, ST_SETSTATBOX, nID, (LONG)lpszBuf ) ;
}

#ifndef NOBOTSTAT
   void BuildBottomBoxes( HWND hWnd )
   {
      int i ;

      for (i = 0 ; i < NUM_BOTBOXES ; i++)
      {
         SetUpStatBox( hWnd, i,
            MyMax
            (
               StatusGetWidth( hWnd,
                                 rgszStat[i].szStat[0] ),
               StatusGetWidth( hWnd,
                                 rgszStat[i].szStat[1] )
            )) ;

         SetStatus( hWnd, i, rgszStat[i].szStat[0] ) ;
      }
   }
#endif   

#ifndef NOTOPSTAT
   void BuildTopBoxes( HWND hWnd )
   {
      int i ;

      for (i = 0 ; i < NUM_TOPBOXES ; i++)
      {
         SetUpStatBox( hWnd, i,
               StatusGetWidth( hWnd, rgszTop[i].szTop[0] ) ) ;

         SetStatus( hWnd, i, rgszTop[i].szTop[1] ) ;
      }
   }
#endif

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

#ifndef NOBOTSTAT
   if (!SetTimer( hWnd, ID_TIMER, 1000, NULL ))
   {
      return FALSE ;
   }
#endif

   ShowWindow( hWnd, nCmdShow ) ;

   UpdateWindow( hWnd ) ;

   return TRUE ;             
}


long FAR PASCAL MainWndProc( HWND     hWnd, 
                             unsigned iMessage,  
                             WORD wParam,                
                             LONG lParam )                 
{
   static   int xChar, yChar ;

   #ifndef NOBOTSTAT
   static   char szPrevDate[64], szPrevTime[64] ;
   #endif

   static   HFONT hTopFont ;

   static   EXTLOGFONT  lf, lfDefault ;

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

   HDC      hDC ;
   HANDLE   hFont ;
   TEXTMETRIC tm ;
   PAINTSTRUCT ps ;
   int      i ;
   char     szBuf[80] ;
   static   RECT rcOld ;
   RECT     rc ;
   EXTLOGFONT  lfTemp ;

   switch (iMessage)
   {
      case WM_CREATE:
      {
         HANDLE        hOldFont ;
         unsigned int  i ;
         LONG     lNumStats ;

         D( if (hWDB = wdbOpenSession( hWnd,
                                       szAppName, "wdb.ini", WDB_LIBVERSION ))
            {
               DP( hWDB, "WDB is active!" ) ;
            }
         ) ;

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


         lstrcpy( szFileSpec, "*.*" ) ;
         PrintGetDefaultPrn( 
                            (LPSTR)szDriver,
                            (LPSTR)szDevice,
                            (LPSTR)szPort
                            ) ;

         hDC = GetDC (hWnd) ;

         hTopFont = ReallyCreateFont( hDC, "Helv", 8, RCF_NORMAL ) ;

         GetObject( hTopFont, sizeof(LOGFONT), &lf ) ;

         lf.lfFGcolor = RGBBLACK ;
         lf.lfBGcolor = RGBWHITE ;

         lfDefault = lf ;

         hOldFont = SelectObject( hDC, hTopFont ) ;

         GetTextMetrics( hDC, &tm ) ;
         xChar = tm.tmAveCharWidth ;
         yChar = tm.tmHeight + tm.tmExternalLeading ;

         hTopFont = SelectObject( hDC, hOldFont ) ;

         ReleaseDC (hWnd, hDC) ;

     #ifndef NOBUTTONS
         #define BITMAPSIZEX 48
         #define BITMAPSIZEY 32

         for (i = IDB_BEGIN ; i <= IDB_END ; i++)
         {
            wsprintf( szBuf, "#%d", i ) ;
            rghWndButton[i-IDB_BEGIN] = CreateWindow(
                                 "bmpbtn",
                                 szBuf,
                                 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                 (BITMAPSIZEX + (yChar)) * (i - IDB_BEGIN),
                                 yChar * 5,
                                 BITMAPSIZEX+yChar,
                                 BITMAPSIZEY+yChar,
                                 hWnd,                 
                                 i,  
                                 hInst,            
                                 NULL 
                              ) ;
            if (rghWndButton[i-IDB_BEGIN] == 0)
            {
               DP( hWDB,  "Create \"bmpbtn\" %d failed!", i ) ;
               return FALSE ;
            }
         }
      #endif


         GetClientRect( hWnd, &rcOld ) ;
         GetClientRect( hWnd, &rc ) ;

      #ifndef NOTOPSTAT
         // create status window
         //
         lNumStats = NUM_TOPBOXES ;
         if (!(hWndTop = CreateWindow(
               "status",
               NULL,
               WS_CHILD,
               0,
               0,
               1,
               1,
               hWnd,
               IDD_TOP,
               hInst,
               (LPSTR)&lNumStats
            )))
         {
            DP( hWDB,  "MsgWnd create failed!" ) ;
            return TRUE ;
         }

         SendMessage( hWndTop, WM_SETFONT, hTopFont, 0L ) ;

         wStatusHeight = (WORD)SendMessage( hWndTop, ST_GETHEIGHT, 0, 0L ) ;

         MoveWindow( hWndTop,
                     rc.left,
                     rc.top - 1,
                     rc.right,
                     wStatusHeight,
                     FALSE ) ;

         SendMessage( hWndTop, ST_SETCOLOR, INDEX_LIB_LBL, RGB( 255, 0, 0 ) ) ;
         SendMessage( hWndTop, ST_SETCOLOR, INDEX_CLIENT_LBL, RGB( 255, 0, 0 ) ) ;
         BuildTopBoxes( hWndTop ) ;

         SendMessage( hWndTop, ST_SETBORDER, INDEX_CLIENT_LBL, (LONG)FALSE ) ;
         SendMessage( hWndTop, ST_SETJUSTIFY, INDEX_CLIENT_LBL, (LONG)DT_RIGHT) ;
         SendMessage( hWndTop, ST_SETJUSTIFY, INDEX_CLIENT, (LONG)DT_LEFT) ;

         SendMessage( hWndTop, ST_SETBORDER, INDEX_LIB_LBL, (LONG)FALSE ) ;
         SendMessage( hWndTop, ST_SETJUSTIFY, INDEX_LIB_LBL, (LONG)DT_RIGHT ) ;
         SendMessage( hWndTop, ST_SETJUSTIFY, INDEX_LIB, (LONG)DT_LEFT ) ;

         ShowWindow( hWndTop, SW_NORMAL ) ;

         UpdateWindow( hWndTop ) ;

      #endif


      #ifndef NOBOTSTAT
         lNumStats = NUM_BOTBOXES ;
         if (!(hWndBottom = CreateWindow(
               "status",
               "Hello World",
               WS_CHILD,
               0,
               (rc.bottom - wStatusHeight) + 1,
               rc.right,
               wStatusHeight,
               hWnd,
               IDD_BOTTOM,
               hInst,
               (LPSTR)&lNumStats
            )))
         {
            DP( hWDB,  "MsgWnd create failed!" ) ;
            return TRUE ;
         }

         BuildBottomBoxes( hWndBottom ) ;

         ShowWindow( hWndBottom, SW_NORMAL ) ;

         UpdateWindow( hWndBottom ) ;

         GetKeyStates( &bCapsLock, &bNumLock, &bScrollLock ) ;
         SetStatus( hWndBottom, INDEX_CAPSLOCK,
                    rgszStat[INDEX_CAPSLOCK].szStat[bCapsLock] ) ;
         SetStatus( hWndBottom, INDEX_NUMLOCK,
                    rgszStat[INDEX_NUMLOCK].szStat[bNumLock] ) ;
         SetStatus( hWndBottom, INDEX_SCROLLLOCK,
                    rgszStat[INDEX_SCROLLLOCK].szStat[bScrollLock] ) ;

         SendMessage( hWndBottom, ST_SETCOLOR, INDEX_CAPSLOCK, RGB( 0, 0, 255 ) ) ;

         SendMessage( hWndBottom, ST_SETCOLOR, -1, RGB( 0, 255, 0 ) ) ;
      #endif

      }
      break ;

      #ifndef NOBOTSTAT
      case WM_TIMER:
         TimeGetCurTime( szBuf, 0 ) ;
         if (lstrcmpi( szBuf, szPrevTime ))
         {
            lstrcpy( szPrevTime, szBuf ) ;
            SetStatus( hWndBottom, 4, szBuf ) ;
         }

         TimeGetCurDate( szBuf, 0 ) ;
         if (lstrcmpi( szBuf, szPrevDate ))
         {
            lstrcpy( szPrevDate, szBuf ) ;
            SetStatus( hWndBottom, 3, szBuf ) ;
         }
      break ;

      case WM_WININICHANGE:
         if (!lstrcmpi( (LPSTR)lParam, "intl" ))
            TimeResetInternational() ;
         break ;

      case WM_MOUSEMOVE:
      case WM_SETFOCUS:
      case WM_KEYDOWN :
      {
         BOOL bCaps = bCapsLock,
               bNum  = bNumLock,
               bScroll = bScrollLock ;
         //
         // if any changed
         //
         if (GetKeyStates( &bCapsLock, &bNumLock, &bScrollLock ))
         {
            if (bCapsLock != bCaps)
               SetStatus( hWndBottom, INDEX_CAPSLOCK,
                           rgszStat[INDEX_CAPSLOCK].szStat[bCapsLock] ) ;

            if (bNumLock != bNum)
               SetStatus( hWndBottom, INDEX_NUMLOCK,
                           rgszStat[INDEX_NUMLOCK].szStat[bNumLock] ) ;

            if (bScrollLock != bScroll)
               SetStatus( hWndBottom, INDEX_SCROLLLOCK,
                           rgszStat[INDEX_SCROLLLOCK].szStat[bScrollLock] ) ;
         }
         return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
      }
      break ;
      #endif

      #ifndef NOBUTTONS
      case WM_KILLFOCUS:
         if (wParam && hWnd == GetParent( wParam ))
         {
            SetFocus( hWnd ) ;
            return 0L ;
         }
         else
            return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
      break ;
      #endif


      case WM_COMMAND:
         switch (wParam)
         {
            #ifndef NOBOTSTAT
            case IDM_MSGVISIBLE:
               if (GetMenuState( GetMenu( hWnd ), wParam, MF_BYCOMMAND ) &&
                   MF_CHECKED)
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              wParam,
                              MF_BYCOMMAND | MF_UNCHECKED ) ;
                  ShowWindow( hWndBottom, SW_HIDE ) ;
               }
               else
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              wParam,
                              MF_BYCOMMAND | MF_CHECKED ) ;
                  ShowWindow( hWndBottom, SW_NORMAL ) ;
               }

            break ;
            #endif

            case IDM_SELFONT:
            {
               WORD w ;

               w = FontSelectDialog( hWnd, "Select-A-Font-O-Rama",
                                 NULL,
                                 wFontSelType,
                                 wFontSelFlags,
                                 &lf, &lfDefault ) ;
               if (w == DLG_OK)
               {
                  #ifndef NOTOPSTAT
                  if (hTopFont)
                     DeleteObject( hTopFont ) ;

                  if (hTopFont = CreateFontIndirect( &lf ))
                  {
                     SendMessage( hWndTop, WM_SETREDRAW, FALSE, 0L ) ;
                     SendMessage( hWndTop, WM_SETFONT, hTopFont, TRUE ) ;
                     BuildTopBoxes( hWndTop ) ;
                     SendMessage( hWndTop, WM_SETREDRAW, TRUE, 0L ) ;
                     GetWindowRect( hWnd, &rc ) ;
                     SendMessage( hWnd, WM_SIZE, SIZENORMAL,
                           MAKELONG( rc.right - rc.left, rc.bottom - rc.top ) ) ;
                  }
                  #endif
               }
            }
            break ;

            case IDM_DEFAULT:
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


            case IDM_FILEOPEN:
               FileOpenDialog( hWnd,
                             "Open a File",
                              szFileSpec,
                              DLGOPEN_MUSTEXIST |
                              DLGOPEN_OPEN,
                              szFileName,
                              80 ) ;
               #ifndef NOTOPSTAT
               wsprintf( szBuf, (LPSTR)"Spec = %s, File = %s",(LPSTR) szFileSpec, (LPSTR)szFileName ) ;
               SetStatus( hWndTop, INDEX_LIB, szBuf ) ;
               #endif
            break ;

            case IDM_FILEEXIT:
               SendMessage( hWnd, WM_CLOSE, 0, 0L ) ;
               break ;

            case IDM_SELPRN:
               DoSelPrn( hWnd, SELPRN_STANDARD | SELPRN_ALLOWSETUP) ;

               #ifndef NOBOTSTAT
               wsprintf(szBuf, (LPSTR) "%s (%s) on %s", (LPSTR)szDevice,
                               (LPSTR)szDriver, (LPSTR)szPort );
               SetWindowText( hWndBottom, szBuf) ;
               #endif
               break ;

            #ifndef NOBOTSTAT
            case IDM_NEXTMSG:
               wMsg++ ;
               if (wMsg == NUM_MSGS)
                  wMsg = 0 ;

               SetWindowText( hWndBottom, rgszMsg[wMsg] ) ;
            break ;

            case IDD_BOTTOM:
            {
               char szBuf[80] ;

               if (HIWORD( lParam ) == 0xFFFF)
               {
                  char szVersion [64] ;
                  VerMakeVersionNumber( szVersion, 1, 0, 0 ) ;
                  AboutDialog(   hWnd,
                                 "CMNDLL Test App",
                                 szAppName,
                                 szVersion,
                                 "This dialog box was opened by clicking on the message line!",
                                 ABOUT_STANDARD ) ;
               }


               SendMessage( hWndBottom, ST_SETCOLOR, INDEX_CAPSLOCK, RGB( 0, 0, 0  ) ) ;
               wsprintf( szBuf, "BoxID %d ", HIWORD( lParam ) ) ;
               if (LOWORD( lParam ) & MK_CONTROL)
                  lstrcat( szBuf, "(Ctrl) " ) ;

               if (LOWORD( lParam ) & MK_SHIFT)
                  lstrcat( szBuf, "(Shift) " ) ;

               if (LOWORD( lParam ) & MK_LBUTTON)
                  lstrcat( szBuf, "(left) " ) ;

               if (LOWORD( lParam ) & MK_MBUTTON)
                  lstrcat( szBuf, "(middle) " ) ;

               if (LOWORD( lParam ) & MK_RBUTTON)
               {
                  lstrcat( szBuf, "(right) " ) ;
               }

               SetWindowText( hWndBottom, szBuf ) ;
            }
            break ;
            #endif

            #ifndef NOTOPSTAT
            case IDD_TOP:
            {
               char szBuf[80] ;
               static int i ;

               if (LOWORD( lParam ) == MK_LBUTTON &&
                   HIWORD( lParam ) >= 2)
               {
                  FileOpenDialog( hWnd,
                              "Open a File",
                                 szFileSpec,
                                 DLGOPEN_MUSTEXIST |
                                 DLGOPEN_OPEN,
                                 szFileName,
                                 80 ) ;
                  #ifndef NOTOPSTAT
                  wsprintf( szBuf, (LPSTR)"Spec = %s, File = %s",(LPSTR) szFileSpec, (LPSTR)szFileName ) ;
                  SetStatus( hWndTop, INDEX_LIB, szBuf ) ;
                  #endif
               }
            }
            break ;
            #endif

            #ifndef NOBUTTONS
            case IDB_1:
               ErrorResBox( hWnd, NULL, MB_ICONEXCLAMATION, IDB_1, IDS_LPSTRFMT,
                            (LPSTR)"szAppName", 0xabcd, (LPSTR)szAppName) ;
               #ifndef NOBOTSTAT
               wsprintf( szBuf, "Button #%d", wParam ) ;
               SetWindowText( hWndBottom, szBuf ) ;
               #endif
               break ;
               
            case IDB_2:
               SendMessage( hWndTop, WM_SETFONT, hTopFont, TRUE ) ;
               #ifndef NOBOTSTAT
               wsprintf( szBuf, "Button #%d", wParam ) ;
               SetWindowText( hWndBottom, szBuf ) ;
               #endif
               break ;

            case IDB_3:
               SendMessage( hWndTop, WM_SETFONT, GetStockObject( SYSTEM_FONT ), TRUE ) ;
               #ifndef NOBOTSTAT
               wsprintf( szBuf, "Button #%d", wParam ) ;
               SetWindowText( hWndBottom, szBuf ) ;
               #endif
               break ;

            case IDB_4:
            case IDB_5:
            case IDB_6:
            case IDB_7:
            case IDB_8:
               #ifndef NOBOTSTAT
               wsprintf( szBuf, "Button #%d", wParam ) ;
               SetWindowText( hWndBottom, szBuf ) ;
               #endif
               break ;
            #endif

            case IDM_HELPABOUT:
               {
               char szVersion [64] ;
               VerMakeVersionNumber( szVersion, 99, 99, 999 ) ;
               AboutDialog(   hWnd,
                              "cmndll.DLL TestApp (2)",
                              szAppName,
                              szVersion,
                              szDescription,
                              ABOUT_STANDARD ) ;
               }

               #ifndef NOBOTSTAT
               SendMessage( hWndBottom, ST_SETCOLOR, -1, RGB( 0, 0, 0  ) ) ;
               #endif

               break;


            case IDM_LED:
            {
               FARPROC lpfnLED ;

               lpfnLED = MakeProcInstance(fnLEDbox, hInst);
               DialogBox(  hInst,      
                           "LEDBOX",  
                           hWnd,        
                           lpfnLED );

               FreeProcInstance( lpfnLED );
            }
            break;

            case IDM_TEST:
            {
               FARPROC lpfnTest ;
               lpfnTest = MakeProcInstance( fnTest, hInst ) ;

      		   DialogBoxParam(  hInst,		
		                          "TESTBOX",	 
                                hWnd,			 
                                lpfnTest,
                                RGBGRAY ) ;

               FreeProcInstance( lpfnTest ) ;
            }
            break;


            default:
               return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
         }
         break ;

      case WM_PAINT:
      {
         HANDLE hOldFont ;

         hDC = BeginPaint (hWnd, &ps) ;

         hFont = ReallyCreateFont( hDC, "Helv", 8, RCF_BOLD | RCF_ITALIC ) ;

         hOldFont = SelectObject( hDC, hFont ) ;

         GetTextMetrics( hDC, &tm ) ;
         xChar = tm.tmAveCharWidth ;
         yChar = tm.tmHeight + tm.tmExternalLeading ;

         PaintInfo( hDC, xChar, yChar) ;

         SelectObject (hDC, hOldFont) ;

         DeleteObject( hFont ) ;
         EndPaint (hWnd, &ps) ;
      }
      break ;

      case WM_SIZE:
      {
         WORD     wStatusHeight ;
         unsigned int i ;

         #ifndef NOTOPSTAT
         GetClientRect( hWndTop, &rc ) ;
         if (hWndTop) // && ((WORD)(rc.right - rc.left)) < LOWORD(lParam))
         {
            wStatusHeight = (WORD)SendMessage( hWndTop, ST_GETHEIGHT, 0, 0L ) ;
            MoveWindow( hWndTop,
                        0,
                        0,
                        LOWORD(lParam),
                        wStatusHeight,
                        TRUE ) ;
         }
         #endif


         #ifndef NOBOTSTAT
         wStatusHeight = (WORD)SendMessage( hWndBottom, ST_GETHEIGHT, 0, 0L ) ;
         if (hWndBottom)
         {
            MoveWindow( hWndBottom,
                           0,
                           (HIWORD(lParam) - wStatusHeight) + 1,
                           LOWORD(lParam),
                           wStatusHeight,
                           TRUE ) ;
         }
         #endif

         #ifndef NOBUTTONS
         for (i = IDB_BEGIN ; i <= IDB_END ; i ++)
         {
            if (rghWndButton[i-IDB_BEGIN])
            {
               GetClientRect( rghWndButton[i-IDB_BEGIN], &rc ) ;

               MoveWindow( rghWndButton[i-IDB_BEGIN],
                           (rc.right - rc.left + 20) * (i - IDB_BEGIN),
                           (HIWORD(lParam) - wStatusHeight + 1) -
                              (rc.bottom - rc.top),
                           rc.right - rc.left,
                           rc.bottom - rc.top,
                           TRUE ) ;
            }
         }
         #endif
      }
      break ;

        case WM_DESTROY:
      {
         unsigned int i ;

         #ifndef NOTOPSTAT
         if (hWndTop)
            DestroyWindow( hWndTop ) ;
         #endif

         #ifndef NOBOTSTAT
         if (hWndBottom)
            DestroyWindow( hWndBottom ) ;
         KillTimer( hWnd, ID_TIMER ) ;
         #endif

         #ifndef NOBUTTONS
         for (i = IDB_BEGIN ; i <= IDB_END ; i++)
         {
            if (rghWndButton[i-IDB_BEGIN])
               DestroyWindow( rghWndButton[i-IDB_BEGIN] ) ;
         }
         #endif

         DeleteObject( hTopFont ) ;
         DP( hWDB, "Normal exit" ) ;
         D( hWDB = wdbCloseSession( hWDB ) ) ;
         PostQuitMessage( 0 ) ;
      }
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

      case DLG_ERROR:
         MessageBox( hWnd, "Select Printer Failed.",
                           "Error", MB_ICONEXCLAMATION ) ;
         break ;
   }
}

/************************************************************************
 *  BOOL FAR PASCAL
 *  GetKeyStates( BOOL *bCapsLock , BOOL *bNumLock, BOOL *bScrollLock ) ;
 *
 *  Description:
 *    Gets the states of the toggle keys.
 *
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *             TRUE
 *                      The states changed.
 *
 *             FALSE
 *                      The states did not change
 *
 *  Comments:
 *
 ************************************************************************/
BOOL FAR PASCAL
GetKeyStates( BOOL *bCapsLock, BOOL *bNumLock, BOOL *bScrollLock )
{
   BOOL bChanged = FALSE ;

   if (*bCapsLock != (GetKeyState( VK_CAPITAL ) & 0x01 ))
   {
      *bCapsLock = !*bCapsLock ;
      bChanged = TRUE ;
   }
   if (*bNumLock != (GetKeyState( VK_NUMLOCK ) & 0x01 ))
   {
      *bNumLock = !*bNumLock ;
      bChanged = TRUE ;
   }

   if (*bScrollLock != (GetKeyState( 0x91 ) & 0x01 ))
   {
      *bScrollLock = !*bScrollLock ;
      bChanged = TRUE ;
   }
   return bChanged ;
}/* GetKeyStates() */



BOOL PaintInfo( HDC hDC, int xChar, int yChar )
{
   char szBuffer[256] ;
   int x, y ;

   x = 2 ;
   y = 4 ;  

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
            (LPSTR)"To demonstrate some neat stuff do the following:" )) ;
   y++ ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
            (LPSTR)"  Double click on the stat boxes on the bottom." )) ;
   y++ ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
            (LPSTR)"     Try it with the right mouse button." )) ;
   y++ ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
            (LPSTR)"     Try it with the shift key down." )) ;
   y++ ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
            (LPSTR)"     Try it with the ctrl key down." )) ;
   y++ ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
            (LPSTR)"  Double click on the message box on the bottom." )) ;
   y++ ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
            (LPSTR)"  Double click on the \"Client\" box above." )) ;
   y++ ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
            (LPSTR)"  Double click on the \"Library\" box above." )) ;
   y++ ;

   return TRUE ;
}

BOOL FAR PASCAL
fnLEDbox( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam)
{
   static HANDLE hTimer ;
   static DWORD rgrgb[] =
   {
      RGBBLACK,    
      RGBRED,      
      RGBGREEN,    
      RGBBLUE,     
      RGBBROWN,    
      RGBMAGENTA,  
      RGBCYAN,     
      RGBLTGRAY,   
      RGBGRAY,     
      RGBLTRED,    
      RGBLTGREEN,  
      RGBLTBLUE,   
      RGBYELLOW,   
      RGBLTMAGENTA,
      RGBLTCYAN,   
      RGBWHITE,    
   } ;
   static WORD wCount ;
   static WORD wCount2 ;
   static WORD wCount3 = 9 ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         short i ;
         hTimer = SetTimer( hDlg, 42, 50, NULL );
         for (i = 100 ; i <= 115 ; i++)
         {
            SendMessage( GetDlgItem( hDlg, i), LED_SETCOLOR,
                         0, RGBGRAY ) ; //rgrgb[i-100] );
            SendMessage( GetDlgItem( hDlg, i + 100), LED_SETCOLOR,
                         0, RGBGRAY ) ; //rgrgb[i-100] );
            SendMessage( GetDlgItem( hDlg, i + 200), LED_SETCOLOR,
                         0, rgrgb[i-100] );
         }

         SendMessage( GetDlgItem( hDlg, 500), LED_SETCOLOR,
                        0, RGBLTRED ) ;
         SendMessage( GetDlgItem( hDlg, 501), LED_SETCOLOR,
                        0, RGBLTGREEN ) ;
         return (TRUE);
      }

      case WM_TIMER:
      {
         WORD wTemp = wCount ;

         if (++wCount >= 16)
         {
            if (++wCount2 >= 16)
               wCount2 = 0 ;

            wCount = 0 ;
         }

         SendMessage( GetDlgItem( hDlg, 100 + wCount), LED_SETCOLOR,
                        0, rgrgb[wCount2] ) ;
         SendMessage( GetDlgItem( hDlg, 100 + wTemp), LED_SETCOLOR,
                        0, RGBGRAY ) ;

         SendMessage( GetDlgItem( hDlg, 215 - wCount), LED_SETCOLOR,
                        0, RGBLTRED ) ;
         SendMessage( GetDlgItem( hDlg, 215 - wTemp), LED_SETCOLOR,
                        0, RGBGRAY ) ;


      }
      break;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK :
            case IDCANCEL :
               if ( hTimer )
                  KillTimer( hDlg, 42 );
               EndDialog(hDlg, TRUE) ;
               return (TRUE);

         }
       break;

    }

    return (FALSE);    
}

/****************************************************************
 *  BOOL FAR PASCAL
 *    fnTest(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *    About box window procedure
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL fnTest(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
{

   switch (wMsg)
   {
      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK :
            case IDCANCEL :
   	   	   EndDialog( hDlg, TRUE ) ;
   	   	   return TRUE ;
	      }
	      break ;

      case WM_INITDIALOG:
         SetDlgItemText( hDlg, IDD_DESC, "Cool 3D Shadow Routines" ) ;
         return ShadowControl( hDlg, wMsg, wParam, lParam ) ;

      case WM_DESTROY:
         return ShadowControl( hDlg, wMsg, wParam, lParam ) ;

      default:
         return ShadowControl( hDlg, wMsg, wParam, lParam ) ;
   }

   return FALSE ;
} /* fnTest()  */






//=======================================================================
// End of File: test2.c
//=======================================================================
