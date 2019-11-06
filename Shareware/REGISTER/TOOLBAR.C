/************************************************************************
 *
 *     Project:  ShareReg
 *
 *      Module:  toolbar.c
 *
 *     Remarks:  3D tool bar for Road runner
 *
 *    The tool bar has <n> items on it.  The window handle for each
 *    item can be accessed by using the GETXXXXX() macros with the item's
 *    ID as the window word index.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include <windows.h>

#include "sharereg.h"
#include "resource.h"

#define NUMCHILDREN        3

#define GETID(hwnd)        GetWindowWord(hwnd, GWW_ID)

extern   HANDLE hAppInst ;

HWND     rghwndChild[NUMCHILDREN] ;

static   WORD wCurFocus = IDD_NAMEEDIT ;

//
// Need to subclass to handle key board
//
FARPROC  rglpfnOldChild[NUMCHILDREN] ;
FARPROC  lpfnChild ;

LONG FAR PASCAL fnChild( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam ) ;

/****************************************************************
 *  BOOL FAR PASCAL tbRegister( VOID )
 *
 *  Description: 
 *
 *    Registers the tool bar window class.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL tbRegister( VOID )
{
   WNDCLASS wndClass ;

   wndClass.style          = CS_HREDRAW | CS_VREDRAW ;
   wndClass.lpfnWndProc    = fnToolBar ;
   wndClass.cbClsExtra     = 0 ;
   wndClass.cbWndExtra     = 0 ;
   wndClass.hInstance      = hAppInst ;
   wndClass.hIcon          = NULL ;
   wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW) ;
   wndClass.hbrBackground  = COLOR_BTNFACE + 1 ;
   wndClass.lpszMenuName   = NULL ;
   wndClass.lpszClassName  = "SRtoolbar" ;

   if (!RegisterClass(&wndClass))
      return FALSE ;

   return TRUE ;

} /* tbRegister()  */

/****************************************************************
 *  HWND FAR PASCAL tbCreate( HWND hwndParent )
 *
 *  Description: 
 *
 *  Comments:
 *
 ****************************************************************/
HWND FAR PASCAL tbCreate( HWND hwndParent )
{
   HWND hWnd ;

   hWnd = CreateWindow(
                        "SRtoolbar",
                        "ToolBar",
                        WS_CHILD,
                        0, 0,
                        10, 10,
                        hwndParent,
                        NULL,
                        hAppInst,
                        NULL ) ;

   return hWnd ;
} /* tbCreate()  */

/****************************************************************
 *  BOOL FAR PASCAL tbDestroy( HWND hwndTB )
 *
 *  Description: 
 *
 *    Destroys the Tool bar
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL tbDestroy( HWND hwndTB )
{
   return TRUE ;
} /* tbDestroy()  */

/****************************************************************
 *  LONG FAR PASCAL
 *    fnToolBar( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description:
 *
 *    Window proc for the tool bar.
 *
 *
 *  Comments:
 *
 ****************************************************************/
LONG FAR PASCAL fnToolBar( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam )
{
   static HFONT   hFont ;
   static HBRUSH  hbrBG ;
   static DWORD   rgbBG ;
   static DWORD   rgbFG ;
   static WORD    wHeight ;
   static WORD    wWidth ;

   switch (wMsg)
   {
      case WM_CREATE:
      {
         #define  XSTART   4
         #define  YSTART   4
         #define  XSPACE   6
         #define  YSPACE   4

         HDC   hDC ;
         short x, y, xSize, ySize ;
         short xChar, yChar ;

         DP5( hWDB, "fnToolBar: WM_CREATE" ) ;
         
         if (hDC = GetDC( hWnd ))
         {
            DWORD dw ;
            RECT  rc ;
            short i ;

            //
            // Make the procinstance for the subclass
            //
            lpfnChild = MakeProcInstance( (FARPROC)fnChild, hAppInst ) ;

            hFont = ReallyCreateFont( hDC, "Helv", 8, RCF_BOLD ) ;

            dw = GetTextExtent( hDC, "M", 1 ) ;

            xChar = LOWORD( dw ) ;
            yChar = HIWORD( dw ) ;

            // create the App CB
            dw = GetTextExtent( hDC, "MMMMMMMM", 8 ) ;
            x = XSTART ;
            y = YSTART ;
            xSize = LOWORD( dw ) + 16 ;
            ySize = HIWORD( dw ) + 100 ;
            i = IDD_APPCB ;
            rghwndChild[i] = CreateWindow( "combobox", "<App>", WS_CHILD |
                                       CBS_DROPDOWNLIST,
                                       x,
                                       y,
                                       xSize,
                                       ySize,
                                       hWnd,
                                       i,
                                       hAppInst,
                                       NULL ) ;
            SendMessage( rghwndChild[i], WM_SETFONT, hFont, 1L ) ;
            ShowWindow( rghwndChild[i], SW_NORMAL ) ;
            //
            // Now Subclass the sucker
            //
            if (lpfnChild)
            {   
               rglpfnOldChild[i] = (FARPROC)GetWindowLong( rghwndChild[i],
                                                           GWL_WNDPROC ) ;
               SetWindowLong( rghwndChild[i], GWL_WNDPROC, (LONG)lpfnChild ) ;
            }
            GetWindowRect( rghwndChild[i], &rc ) ;
            wHeight = (rc.bottom - rc.top) + 2 * YSPACE + 1 ;


            // create the Name Edit control
            dw = GetTextExtent( hDC, "MMMMMMMMMMMMMMMMMMMMMMMMM", 25 ) ;
            x += xSize + XSPACE ;
            xSize = LOWORD( dw ) + 16 ;
            ySize = HIWORD( dw ) + 4 ;
            i = IDD_NAMEEDIT ;
            rghwndChild[i] = CreateWindow( "edit", "",
                                       WS_CHILD | WS_BORDER |
                                       //ES_NOHIDESEL |
                                       ES_AUTOHSCROLL,
                                       x,
                                       y,
                                       xSize,
                                       ySize,
                                       hWnd,
                                       i,
                                       hAppInst,
                                       NULL ) ;
            SendMessage( rghwndChild[i], WM_SETFONT, hFont, 1L ) ;
            ShowWindow( rghwndChild[i], SW_NORMAL ) ;
            //
            // Now Subclass the sucker
            //
            if (lpfnChild)
            {   
               rglpfnOldChild[i] = (FARPROC)GetWindowLong( rghwndChild[i],
                                                           GWL_WNDPROC ) ;
               SetWindowLong( rghwndChild[i], GWL_WNDPROC, (LONG)lpfnChild ) ;
            }


            // create the "DoIt!" button
            dw = GetTextExtent( hDC, "&Do It!", 5 ) ;
            x += xSize + XSPACE ;
            y = YSTART - 2 ;
            xSize = LOWORD( dw ) + 16 ;
            ySize = HIWORD( dw ) + (HIWORD( dw ) / 2) ;
            i = IDD_DOITBTN ;
            rghwndChild[i] = CreateWindow( "button", "Do It!",
                                       WS_CHILD | BS_DEFPUSHBUTTON,
                                       x,
                                       y,
                                       xSize,
                                       ySize,
                                       hWnd,
                                       i,
                                       hAppInst,
                                       NULL ) ;
            SendMessage( rghwndChild[i], WM_SETFONT, hFont, 1L ) ;
            ShowWindow( rghwndChild[i], SW_NORMAL ) ;
            //
            // Now Subclass the sucker
            //
            if (lpfnChild)
            {   
               rglpfnOldChild[i] = (FARPROC)GetWindowLong( rghwndChild[i],
                                                           GWL_WNDPROC ) ;
               SetWindowLong( rghwndChild[i], GWL_WNDPROC, (LONG)lpfnChild ) ;
            }

            wWidth = x + xSize + XSTART ;
            ReleaseDC( hWnd, hDC ) ;
         }

         rgbBG = GetSysColor( COLOR_BTNFACE ) ;
         rgbFG = GetSysColor( COLOR_BTNTEXT ) ;

         hbrBG = CreateSolidBrush( rgbBG ) ;
      }
      break ;

      case WM_SETFOCUS:
         SetFocus( rghwndChild[wCurFocus] ) ;
         break ;

      case WM_CTLCOLOR:

         switch (HIWORD( lParam ))
         {
            case CTLCOLOR_BTN:
            case CTLCOLOR_MSGBOX:
            case CTLCOLOR_EDIT:
            case CTLCOLOR_LISTBOX:
            case CTLCOLOR_STATIC:
            case CTLCOLOR_DLG:
               SetBkColor( (HDC)wParam, rgbBG ) ;
               SetTextColor( (HDC)wParam, rgbFG ) ;
               return hbrBG ;

            case CTLCOLOR_SCROLLBAR:
               return FALSE ;

            default:
               return FALSE ;

         }
      break ;

      case WM_PAINT:
      {
         PAINTSTRUCT    ps ;
         RECT           rc ;

         GetClientRect( hWnd, &rc ) ;

         BeginPaint( hWnd, &ps ) ;
         SetBkColor( ps.hdc, rgbBG ) ;

         rc.bottom -= 1 ;
         Draw3DRect( ps.hdc, &rc, 1, DRAW3D_OUT ) ;

         rc.top += rc.bottom ;
         rc.bottom += 1 ;

         SetBkColor( ps.hdc, RGBBLACK ) ;
         ExtTextOut( ps.hdc, rc.left,
                     rc.bottom, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;

         //
         // Now paint all the 3D effects for the children...
         //
         SetBkColor( ps.hdc, rgbBG ) ;
         SetTextColor( ps.hdc, MapTextColor( rgbBG ) ) ;
         DrawChildren3D( ps.hdc, hWnd ) ;

         EndPaint( hWnd, &ps ) ;
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
      }
      break ;

      //
      // Send a TB_HOWBIG message to get the height I want to be!
      //
      case TB_HOWBIG:
         return MAKELONG( wWidth, wHeight ) ;

      case WM_COMMAND:
         SendMessage( GetParent( hWnd ), wMsg, wParam, lParam ) ;
      break ;

      case WM_DESTROY:
      {
         short i ;

         for (i = IDD_APPCB ; i <= IDD_DOITBTN ; i++)
            if (rghwndChild[i])
               DestroyWindow( rghwndChild[i] ) ;

         if (lpfnChild)
            FreeProcInstance( lpfnChild ) ;

         if (hFont)
            DeleteObject( hFont ) ;
         if (hbrBG)
            DeleteObject( hbrBG ) ;
      }   
      break ;

      default:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }

   return 0L ;

} /* fnToolBar()  */

LONG FAR PASCAL fnChild( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam )
{
   short    ID ;
   short    i ;
   static BOOL     fKeyDown = FALSE ;

   i = ID = GETID( hWnd ) ;

   switch (wMsg)
   {
      case WM_SETFOCUS:
         wCurFocus = ID ;
      break ;

      case WM_KEYDOWN:
         switch (wParam)
         {
            case VK_TAB:
            {
               i += (short)(GetKeyState( VK_SHIFT ) < 0 ? -1 : 1) ;

               if (i < IDD_APPCB)
                  i = IDD_DOITBTN ;
               else
                  if (i > IDD_DOITBTN)
                     i = IDD_APPCB ;

               if (rghwndChild[i])
               {
                  SetFocus( rghwndChild[i] ) ;
                  return 0L ;
               }
            }
            break ;

            case VK_RETURN:
               if (ID == IDD_NAMEEDIT || ID == IDD_DOITBTN)
               {
                  fKeyDown = TRUE ;
                  SendMessage( rghwndChild[IDD_DOITBTN], BM_SETSTATE, TRUE, 0L ) ;
                  return 0L ;
               }
            break ;
         }
      break ;

      case WM_KEYUP:
         switch (wParam)
            case VK_RETURN:
            {
               if (!fKeyDown)
                  break ;

               if (ID == IDD_NAMEEDIT || ID == IDD_DOITBTN)
               {
                  fKeyDown = FALSE ;
                  SendMessage( rghwndChild[IDD_DOITBTN], BM_SETSTATE, FALSE, 0L ) ;
                  SendMessage( GetParent( rghwndChild[IDD_DOITBTN] ),
                               WM_COMMAND, IDD_DOITBTN,
                               MAKELONG( rghwndChild[IDD_DOITBTN], BN_CLICKED ) ) ;
                  return 0L ;
               }
            }
            break ;
      break ;

      case WM_CHAR:
         //
         // eat tab and return chars for edit control
         //
         if (hWnd == rghwndChild[IDD_NAMEEDIT])
            if (wParam == VK_TAB || wParam == VK_RETURN)
               return 0L ;

   }

   if (rglpfnOldChild[ID])
      return CallWindowProc( rglpfnOldChild[ID], hWnd, wMsg, wParam, lParam ) ;
   else
      return 0L ;
}

/************************************************************************
 * End of File: toolbar.c
 ***********************************************************************/

