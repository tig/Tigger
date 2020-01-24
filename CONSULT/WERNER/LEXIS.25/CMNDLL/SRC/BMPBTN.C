/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CMNDLL.DLL, Common DLL for Werner Apps
 *
 *      Module:  bmpbtn.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 * 
 *     Remarks:
 *
 *    The '^' (read bitmap from file) option is not implemented yet.
 *
 *    The '%' (search for appropriate resolution bitmap) is not
 *    implemented yet.  It may never be implemented because it might
 *    not be needed.  
 *
 *   Revisions:
 *     00.00.000  1/12/91 cek   First version (not in cmndll)
 *     00.00.128  1/27/91 cek   Added ability change colors via messages.
 *
 *
 *-----------------------------------------------------------------------
 *
 *  BmpBtnInit() registers a button class called "bmpbtn".
 *  a "bmpbtn" behaves exactly like a normal windows push button, with the
 *  following enhancments:
 *
 * BITMAPS:
 *    If the window text of the button begins with a '~' and the string
 *    following names a bitmap resource, the bitmap will be displayed as
 *    the button using the syntax described in the SYNTAX section below.
 *
 *    If the window text of the button begins with a '#' then 
 *    bmpbtn will use as it's text the string in the string table with
 *    the id specified after the '#'.  The string in the string table
 *    can have the same format as a '~' string.
 *
 *    If the window text begins with a '^' then the bitmap names specify
 *    a full pathname to a BMP file which will be used in the button.
 *
 * SYNTAX:
 *    The button names will be interpreted as follows:
 *
 *    General Syntax:  [#|~|^][%][normal bmp]|[depress bmp]|[text]
 *
 *    If the '%' paramter is used the resources will be searched for
 *    a bitmap that corresponds the the current device resolution
 *    (using the dpi values as a key) in the following order:
 *
 *        fooWxHxC
 *        fooWxH
 *        foo
 *
 *        W is the horizontal dpi for the current display
 *        H is the vertical dpi value of the current display
 *        C is the number of colors of the current display
 *
 *    For example a VGA specific resource would be named "foo96x96x16"
 *
 *    The [text] field is the text that will be placed on the bitmap face
 *    if the bitmap resources cannot be found or cannot be loaded due to
 *    low memory conditions.
 *
 *    Examples:
 *       If you wanted to use a bitmap named "bar" for both the normal and
 *       depressed state and wanted the bmpbtn to automatically select
 *       the correct resolution you would specify
 *       the following as the window text:
 *
 *          "~%bar||Bar"
 *
 *       If you have a string resource at id IDS_FOOBAR and you want to
 *       have bmpbtn do everything for you, use the following code to
 *       set up the window text string:
 *
 *          wsprintf( szWndText, "#%d", IDS_FOOBAR ) ;
 *
 *       In the resource file you should have a string at id IDS_FOOBAR
 *       that looks something like this:
 *
 *          "~%foobarup|foobardown|Foo Bar"
 *
 *       Thus the depressed state would have the foobardown bitmap, the up
 *       state would be the foobarup bitmap, and if there was not enough
 *       memory the text "Foo Bar" would be used instead of a bitmap.
 *
 *       If you want to use an external file for the bitmap use the
 *       following:
 *
 *          "^C:\WIN\FOOUP.BMP|C:\WIN\FOODWN.BMP|Foo"
 *
 * TRANSPARENT COLOR:
 *    The first pixel of color bitmap's will be used as the "transparent"
 *    color.  All pixels that match the transparent color will be set
 *    equal to the button face color
 *
 * BUTTON STYLES:
 *    BS_STRETCH    - stretch bitmaps to fit button
 *    BS_NOFOCUS    - dont steal focus on mouse messages
 *
 ************************************************************************/


#define WINDLL
#define NOCOMM
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "..\inc\cmndll.h"
//#include "bmpbtn.h"

/***************************************************************************
 * Local Defines                                      
 ***************************************************************************/

#define BEVELWIDTH   2
#define FRAMEWIDTH   1
#define MAX_WNDTEXTLEN  128

#define GWW_STATE   0
#define GWW_HBMUP   2
#define GWW_HBMDN   4
#define GWW_FLAGS   6
#define GWW_CHECK   8
#define GWW_HFONT   10
#define GWW_COLORS  12

#define WNDEXTRABYTES 8*sizeof(WORD)

/***************************************************************************
 * Macros                                      
 ***************************************************************************/
#define rgbWhite   RGB(255,255,255)
#define rgbBlack   RGB(0,0,0)

#define ISDIGIT(c)  ((c) >= '0' && (c) <= '9')

#define GETSTYLE(hWnd) LOWORD( GetWindowLong( hWnd, GWL_STYLE ) )
#define GETHINST(hWnd) GetWindowWord( hWnd, GWW_HINSTANCE )
#define GETSTATE(hWnd) GetWindowWord( hWnd, GWW_STATE )
#define GETFLAGS(hWnd) GetWindowWord( hWnd, GWW_FLAGS )
#define GETCHECK(hWnd) GetWindowWord( hWnd, GWW_CHECK )
#define GETHBMUP(hWnd)   GetWindowWord( hWnd, GWW_HBMUP )
#define GETHBMDN(hWnd)   GetWindowWord( hWnd, GWW_HBMDN )
#define GETHFONT(hWnd)   GetWindowWord( hWnd, GWW_HFONT )
#define GETCOLORS(hWnd)  GetWindowWord( hWnd, GWW_COLORS ) 

#define lpCreate ((LPCREATESTRUCT)lParam)

#define EraseButton(hWnd,hDC,prc) ExtTextOut(hDC,0,0,ETO_OPAQUE,prc,NULL,0,NULL)
#define NearestColor(hDC,rgb) (GetNearestColor(hDC,rgb) & 0x00FFFFFFL)

/***************************************************************************
 *  Local data structures
 ***************************************************************************/
typedef struct tagBMPBTNCOLOR
{
   DWORD rgbFace ;
   DWORD rgbText ;
   DWORD rgbShadow ;
   DWORD rgbFocus ;
} BMPBTNCOLOR, *PBMPBTNCOLOR, FAR *LPBMPBTNCOLOR ;

/**************************************************************************
 * Prototypes for local functions
 **************************************************************************/

static BOOL NEAR PASCAL
Create( HWND hWnd, LONG lParam ) ;

static VOID NEAR PASCAL
DrawGrayButton( HWND, HDC, LPRECT, WORD, BOOL ) ;

static VOID NEAR PASCAL
DrawButtonFace( HWND, HDC, LPRECT, WORD, BOOL ) ;

static BOOL NEAR PASCAL
PaintButton( HWND, HDC ) ;

static VOID NEAR PASCAL
NotifyParent( HWND ) ;

static VOID NEAR PASCAL
PatB( HDC, int, int, int, int, DWORD) ;

static BOOL NEAR PASCAL
GetBitmaps( HWND hWnd, LPSTR lpszSpec, HANDLE FAR *phBMup,
            HANDLE FAR *phBMdn, LPSTR lpszText ) ;

BOOL FAR PASCAL
LocalBmpBtnInit( HANDLE hPrev, HANDLE hInst ) ;

#define LoadBitmapResource(hInst,lpName) LoadBitmap(hInst,lpName)
#define BitmapColorTranslate(hDCBits,pbm,rgb) 0

//
// Window proc for buttons, THIS FUNCTION MUST BE EXPORTED
//
LONG FAR PASCAL fnBmpBtn (HWND, unsigned, WORD, LONG);
LONG FAR PASCAL fnFastText   (HWND, unsigned, WORD, LONG);


/****************************************************************************
 * Local Variables                                     
 ****************************************************************************/

HBRUSH hbrGray ;                 // Gray for text
HBRUSH hbrFocus ;                // focus for text

DWORD  rgbButtonFocus ;
DWORD  rgbButtonFace ;
DWORD  rgbButtonText ;
DWORD  rgbButtonShadow ;


/****************************************************************************
 * window class names
 ****************************************************************************/
char szBmpBtn[] = "bmpbtn" ;
char szFastText[]   = "fasttext" ;


/************************************************************************
 *  BOOL FAR PASCAL BmpBtnInit( HANDLE hPrev, HANDLE hInst )
 *
 *  Description:
 *    This function initializes the btnbmp class.  It should be called
 *    from the LibMain or WinMain function.
 *
 *  Type/Parameter
 *          Description
 *    HANDLE   hPrev
 *          instance handle of previous instance
 *
 *    HANDLE   hInst
 *          instance handle of current instance
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *             TRUE
 *                      Successful
 *             FLASE
 *                      not successful
 *
 *  Comments:
 *
 ************************************************************************/
BOOL FAR PASCAL
BmpBtnInit( HANDLE hPrev, HANDLE hInst )
{
   return TRUE ;
}

BOOL FAR PASCAL
LocalBmpBtnInit( HANDLE hPrev, HANDLE hInst ) 
{
   WNDCLASS    wc ;
   long        patGray[4] ;
   HBITMAP     hBMGray ;
   int         i ;
   HDC         hDC ;

   //
   // initialize the brushes
   //
   for (i=0 ; i < 4 ; i++)
     patGray[i] = 0xAAAA5555L ;   //  0x11114444L ; // lighter gray

   if (hBMGray = CreateBitmap( 8, 8, 1, 1, (LPSTR)patGray ))
   {
      hbrGray = CreatePatternBrush( hBMGray ) ;
      DeleteObject( hBMGray ) ;
   }
   else
      hbrGray = GetStockObject( LTGRAY_BRUSH ) ;

   rgbButtonFace   = GetSysColor( COLOR_BTNFACE ) ;
   rgbButtonShadow = GetSysColor( COLOR_BTNSHADOW ) ;
   rgbButtonText   = GetSysColor( COLOR_BTNTEXT ) ;
   rgbButtonFocus  = rgbButtonText ; // rgbWhite ;

   if (hDC = GetDC( NULL ))
   {
      rgbButtonFace   = NearestColor( hDC, rgbButtonFace ) ;
      rgbButtonShadow = NearestColor( hDC, rgbButtonShadow ) ;
      rgbButtonText   = NearestColor( hDC, rgbButtonText ) ;
      rgbButtonFocus  = NearestColor( hDC, rgbButtonFocus ) ;

      ReleaseDC( NULL, hDC ) ;
   }
   if (rgbButtonFocus == rgbButtonFace)
      rgbButtonFocus = rgbButtonText ;

   hbrFocus = CreateSolidBrush( rgbButtonFocus ) ;

   if (!hPrev)
   {
      wc.hCursor        = LoadCursor( NULL,IDC_ARROW ) ;
      wc.hIcon          = NULL ;
      wc.lpszMenuName   = NULL ;
      wc.lpszClassName  = (LPSTR)szBmpBtn ;
      wc.hbrBackground  = (HBRUSH)COLOR_WINDOW+1 ;
      wc.hInstance      = hInst ;
      wc.style          = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS ;
      wc.lpfnWndProc    = fnBmpBtn ;
      wc.cbClsExtra     = 0 ;
      wc.cbWndExtra     = WNDEXTRABYTES ;
      if (!RegisterClass( &wc ))
         return FALSE ;

      wc.hCursor        = LoadCursor( NULL,IDC_ARROW ) ;
      wc.hIcon          = NULL ;
      wc.lpszMenuName   = NULL ;
      wc.lpszClassName  = (LPSTR)szFastText ;
      wc.hbrBackground  = (HBRUSH)COLOR_WINDOW+1 ;
      wc.hInstance      = hInst ;
      wc.style          = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS ;
      wc.lpfnWndProc    = fnFastText ;
      wc.cbClsExtra     = 0 ;
      wc.cbWndExtra     = 0 ;
      if (!RegisterClass( &wc ))
         return FALSE ;
   }

   return TRUE ;
}/* BmpBtnInit() */

BOOL FAR PASCAL BmpBtnUnInit( HANDLE hInst )
{
   if (hbrGray)
      DeleteObject( hbrGray ) ;

   if (hbrFocus)
      DeleteObject( hbrFocus ) ;

   UnregisterClass( szFastText, hInst ) ;

   return UnregisterClass( szBmpBtn, hInst ) ;

}



/************************************************************************
 *  BOOL ButtonState( HWND hWnd, BOOL f )
 *
 *  Description:
 *    Compares the passed state with the current state.  If they are
 *    different the button is invalidated and TRUE is returned.
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *             TRUE
 *                      if f is different than the current state.
 *
 ************************************************************************/
BOOL ButtonState( HWND hWnd, BOOL f )
{
   WORD wState ;

   wState =  GETSTATE( hWnd ) ;

   if (wState != (WORD)f)
   {
      SetWindowWord( hWnd, GWW_STATE, (WORD)f ) ;
      InvalidateRect( hWnd, NULL, TRUE ) ;
      UpdateWindow( hWnd ) ;
      return TRUE ;
   }
   return FALSE ;
}/* ButtonState() */

/*************************************************************************
 *  LONG FAR PASCAL fnBmpBtn( HWND hWnd, unsigned iMessage,
 *                            WORD wParam, LONG lParam )
 *
 *  Description:
 *    Window proc for the famous "bitmap button"
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
LONG FAR PASCAL
fnBmpBtn( HWND hWnd, unsigned iMessage, WORD wParam, LONG lParam )
{
   HANDLE      hBMup ;
   HANDLE      hBMdn ;
   PAINTSTRUCT ps ;
   RECT        rc ;
   LONG        l ;

   switch (iMessage)
   {
      case WM_CREATE:
         Create( hWnd, lParam ) ;
      break ;

      case WM_LBUTTONDOWN:
         if (!IsWindowEnabled( hWnd ))
            return 0L ;

         if (GetCapture() != hWnd)  /* ignore multiple DOWN's */
         {
            ButtonState( hWnd,TRUE ) ;
            SetCapture( hWnd ) ;
            if (!(GETFLAGS( hWnd ) & BS_NOFOCUS))
               SetFocus( hWnd ) ;
         }
         return 0L ;

      case WM_MOUSEMOVE:
         if (GetCapture() == hWnd)
         {
            GetClientRect( hWnd, &rc ) ;
            ButtonState( hWnd, PtInRect( &rc, MAKEPOINT( lParam ) ) ) ;
         }
         return 0L ;

      case WM_LBUTTONUP:
         if (GetCapture() == hWnd)
         {
            ReleaseCapture() ;
            if (ButtonState( hWnd, FALSE ))
               NotifyParent( hWnd ) ;
         }
         return 0L ;

      case WM_SETTEXT:
         if (hBMup = GETHBMUP( hWnd ))
            DeleteObject( hBMup ) ;
         if (hBMdn = GETHBMDN( hWnd ))
            DeleteObject( hBMdn ) ;

         if (*(LPSTR)lParam == BMPBTN_BMPFLAG)
         {
            LOCALHANDLE lhMem ;
            
            lhMem = LocalAlloc( LHND, 1 + lstrlen( (LPSTR)lParam ) ) ;

            if (lhMem)
            {
               LPSTR lpszText = (LPSTR)LocalLock( lhMem ) ;

               if (!GetBitmaps( hWnd,
                              (LPSTR)lParam+1,
                              (HANDLE FAR *)&hBMup,
                              (HANDLE FAR *)&hBMdn,
                              lpszText ))
               {
                  hBMup = hBMdn = NULL ;
               }

               /*
                * WHOA!! Major problem here.  This appears not to
                * work in Windows 3.1!!!  Tried on build 46.
                */
               lstrcpy( (LPSTR)lParam, lpszText ) ;

               LocalUnlock( lhMem ) ;
               LocalFree( lhMem ) ;
            }
            else
            {
               DP1( hWDB,"bmpbtn: No local memory" ) ;
            }
         }
         else
         {
            hBMup = hBMdn = NULL ;
            if (*(LPSTR)lParam == BMPBTN_RESNUMFLAG)
            {
               int i ;
               char szBuf[MAX_WNDTEXTLEN+1] ;

               i = AtoI((LPSTR)lParam+1) ;
               if (LoadString( GETHINST( hWnd ), i, szBuf, MAX_WNDTEXTLEN ))
                  lstrcpy( (LPSTR)lParam, szBuf ) ;
               else
               {
                  DP1( hWDB,"bmpbtn: LoadString failed" ) ;
               }
            }
         }
         SetWindowWord( hWnd, GWW_HBMUP, hBMup ) ;
         SetWindowWord( hWnd, GWW_HBMDN, hBMdn ) ;
         InvalidateRect( hWnd, NULL, TRUE ) ;
         break ;

      case WM_ENABLE:
      case WM_KILLFOCUS:
      case WM_SETFOCUS:
         InvalidateRect( hWnd, NULL, TRUE ) ;
         break ;

      case WM_KEYDOWN:
         if ((wParam == VK_SPACE && IsWindowEnabled( hWnd )) ||
             (wParam == VK_RETURN &&
               LOBYTE(GETSTYLE( hWnd )) == BS_DEFPUSHBUTTON))
            ButtonState( hWnd, TRUE ) ;
         break ;

      case WM_KEYUP:
      case WM_SYSKEYUP:
         if ((wParam == VK_SPACE && IsWindowEnabled(hWnd)) ||
             (wParam == VK_RETURN &&
               LOBYTE(GETSTYLE( hWnd )) == BS_DEFPUSHBUTTON))
         {
            if (ButtonState( hWnd, FALSE ))
               NotifyParent( hWnd ) ;
         }
         break ;

      case BB_SETTEXTCOLOR:
      {
         LPBMPBTNCOLOR lpBBC ;
         LOCALHANDLE   lhBBC ;

         if (!(lhBBC = GETCOLORS( hWnd )))
         {
            lhBBC = LocalAlloc( LHND, sizeof( BMPBTNCOLOR ) ) ;
            SetWindowWord( hWnd, GWW_COLORS, lhBBC ) ;
         }

         lpBBC = (LPBMPBTNCOLOR)LocalLock( lhBBC ) ;
         lpBBC->rgbText = lParam ;
         lpBBC->rgbFocus = lParam ;
         LocalUnlock( lhBBC ) ;
         InvalidateRect( hWnd, NULL, TRUE ) ;
      }
      break ;

      case BB_SETSHADOWCOLOR:
      {
         LPBMPBTNCOLOR lpBBC ;
         LOCALHANDLE   lhBBC ;

         if (!(lhBBC = GETCOLORS( hWnd )))
         {
            lhBBC = LocalAlloc( LHND, sizeof( BMPBTNCOLOR ) ) ;
            SetWindowWord( hWnd, GWW_COLORS, lhBBC ) ;
         }

         lpBBC = (LPBMPBTNCOLOR)LocalLock( lhBBC ) ;
         lpBBC->rgbShadow = lParam ;
         LocalUnlock( lhBBC ) ;
         InvalidateRect( hWnd, NULL, TRUE ) ;
      }
      break ;

      case BB_SETFACECOLOR:
      {
         LPBMPBTNCOLOR lpBBC ;
         LOCALHANDLE   lhBBC ;

         if (!(lhBBC = GETCOLORS( hWnd )))
         {
            lhBBC = LocalAlloc( LHND, sizeof( BMPBTNCOLOR ) ) ;
            SetWindowWord( hWnd, GWW_COLORS, lhBBC ) ;
         }

         lpBBC = (LPBMPBTNCOLOR)LocalLock( lhBBC ) ;
         lpBBC->rgbFace = lParam ;
         LocalUnlock( lhBBC ) ;
         InvalidateRect( hWnd, NULL, TRUE ) ;
      }
      break ;

      case BB_SETFOCUSCOLOR:
      {
         LPBMPBTNCOLOR lpBBC ;
         LOCALHANDLE   lhBBC ;

         if (!(lhBBC = GETCOLORS( hWnd )))
         {
            lhBBC = LocalAlloc( LHND, sizeof( BMPBTNCOLOR ) ) ;
            SetWindowWord( hWnd, GWW_COLORS, lhBBC ) ;
         }

         lpBBC = (LPBMPBTNCOLOR)LocalLock( lhBBC ) ;
         lpBBC->rgbFocus = lParam ;
         LocalUnlock( lhBBC ) ;
         InvalidateRect( hWnd, NULL, TRUE ) ;
      }
      break ;


      case BM_GETSTATE:
         return((LONG)GETSTATE( hWnd )) ;

      case BM_SETSTATE:
         if (ButtonState( hWnd,wParam ) && !wParam)
            NotifyParent( hWnd ) ;
         break ;

      case BM_GETCHECK:
         return((LONG)GETCHECK( hWnd )) ;

      case BM_SETCHECK:
         SetWindowWord( hWnd, GWW_CHECK, wParam) ;
         break ;

      case BM_SETSTYLE:
         l = GetWindowLong( hWnd, GWL_STYLE ) ;
         SetWindowLong( hWnd, GWL_STYLE, MAKELONG( wParam, HIWORD(l) ) ) ;
         if (lParam)
            InvalidateRect( hWnd, NULL, TRUE ) ;
         break ;

      case WM_GETDLGCODE:
         switch (LOBYTE(GETSTYLE( hWnd )))
         {
            case BS_DEFPUSHBUTTON:
               wParam = DLGC_DEFPUSHBUTTON ;
               break ;

            case BS_PUSHBUTTON:
               wParam = DLGC_UNDEFPUSHBUTTON ;
               break ;

            default:
               wParam = 0 ;
         }
         return((LONG)(wParam | DLGC_BUTTON)) ;

      case WM_GETFONT:
         return GETHFONT( hWnd ) ;
      break ;

      case WM_SETFONT:
      {
         HFONT hFontOld = GETHFONT( hWnd ) ;

         if (wParam)
            SetWindowWord( hWnd, GWW_HFONT, wParam ) ;
         else
            SetWindowWord( hWnd, GWW_HFONT, GetStockObject( SYSTEM_FONT ) ) ;

         if (lParam)
            InvalidateRect( hWnd, NULL, TRUE ) ;

         return hFontOld ;
      }
      break ;

      case WM_ERASEBKGND:
         return 0L ;

      case WM_MOVE:
         InvalidateRect( hWnd, NULL, TRUE ) ;
         break ;

      case WM_PAINT:
         if (BeginPaint( hWnd, &ps ))
         {

            if (!GETCOLORS( hWnd ))
            {
               rgbButtonFace   = GetSysColor( COLOR_BTNFACE ) ;
               rgbButtonShadow = GetSysColor( COLOR_BTNSHADOW ) ;
               rgbButtonText   = GetSysColor( COLOR_BTNTEXT ) ;
               rgbButtonFocus  = rgbButtonText ; // rgbWhite ; 

               if (rgbButtonFocus == rgbButtonFace)
                  rgbButtonFocus = rgbButtonText ;
            }

            PaintButton( hWnd,ps.hdc ) ;
            EndPaint( hWnd, &ps ) ;
         }
      return 0L ;

      case WM_DESTROY:
      {
         LOCALHANDLE lhBBC ;

         if (hBMup = GETHBMUP( hWnd ))
             DeleteObject( hBMup ) ;
         if (hBMdn = GETHBMDN( hWnd ))
             DeleteObject( hBMdn ) ;

         if (lhBBC = GETCOLORS( hWnd ))
            LocalFree( lhBBC ) ;
      }
      break ;

   }
   return DefWindowProc(hWnd, iMessage, wParam, lParam) ;

}/* fnBmpBtn() */

/*************************************************************************
 *  static BOOL NEAR PASCAL Create( HWND hWnd, LONG lParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
static BOOL NEAR PASCAL Create( HWND hWnd, LONG lParam )
{
   SetWindowWord( hWnd, GWW_HBMUP, NULL ) ;
   
   SetWindowWord( hWnd, GWW_HBMDN, NULL ) ;
   
   SetWindowWord( hWnd, GWW_STATE, 0 ) ;
   
   SetWindowWord( hWnd, GWW_FLAGS, (WORD)lpCreate->style & 0xFF00 ) ;

   SetWindowWord( hWnd, GWW_HFONT, GetStockObject( SYSTEM_FONT )) ;

   SetWindowWord( hWnd, GWW_COLORS, 0 ) ;

   //
   // If the first character of the string is a '#' then load the
   // appropriate string from the resources
   //
   if (*lpCreate->lpszName == BMPBTN_RESNUMFLAG)
   {
      int i ;
      char szBuf[MAX_WNDTEXTLEN+1] ;

      i = AtoI(lpCreate->lpszName+1) ;
      if (LoadString( GETHINST( hWnd ), i, szBuf, MAX_WNDTEXTLEN ))
         SetWindowText( hWnd, szBuf) ;
      else
      {
         lstrcpy( szBuf, lpCreate->lpszName ) ;
         SetWindowText( hWnd, szBuf ) ;
      }
   }
   else
   {
      char szBuf[MAX_WNDTEXTLEN+1] ;

      lstrcpy( szBuf, lpCreate->lpszName ) ;
      SetWindowText( hWnd, szBuf ) ;
   }

   SetWindowLong( hWnd, GWL_STYLE, lpCreate->style & 0xFFFF00FF ) ;

   return TRUE ;

}/* Create() */

/*************************************************************************
 * static BOOL NEAR PASCAL
 * GetBitmaps( HWND hWNd, LPSTR lpszSpec, HANDLE FAR *hBMup,
 *                                        HANDLE FAR *hBMdn ) ;
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
static BOOL NEAR PASCAL
GetBitmaps( HWND hWnd, LPSTR lpszSpec, HANDLE FAR *phBMup,
            HANDLE FAR *phBMdn, LPSTR lpszText )
{
   HANDLE hInst = GETHINST( hWnd ) ;
   LPSTR  lpszBMdn = NULL ;
   LPSTR  lpszTmp ;
   char   szBMup[MAX_WNDTEXTLEN] ;
   BOOL   fLoadFromFile = FALSE ;

   //
   // If the load from file flag is set
   //
   if (*lpszSpec == BMPBTN_FILEFLAG)
   {
      DP1( hWDB, "Filespecs in bitmap names is not supported yet!" ) ;
      // 
      // We don't support this yet. 
      // return false
      //
      return FALSE ;
   }

   lstrcpy( szBMup, lpszSpec ) ;

   //
   // if the first bar is there
   //
   if (lpszBMdn = _fstrchr( szBMup, '|' ))
   {
      //
      // replace it with a null, saving the position
      //
      *(lpszBMdn++) = '\0' ;

      // 
      // now check for a text string (2nd |)
      //
      if (lpszTmp = _fstrchr( lpszBMdn, '|' ))
      {
         *(lpszTmp++) = '\0' ;
         lstrcpy( lpszText, lpszTmp ) ;
      }
   }

   // 
   // Now we have three LPSTRs for three strings.
   //
   // load the UP bitmap
   if (!(*phBMup = LoadBitmapResource( hInst, (LPSTR)szBMup )))
   {
      DP1( hWDB,"bmpbtn: LoadBitmapResource failed!" ) ;
   }

   if (lpszBMdn && lstrlen( lpszBMdn ))
   {
      if (!(*phBMdn = LoadBitmapResource( hInst, lpszBMdn )))
      {
         DP1( hWDB,"bmpbtn: LoadBitmapResource failed!" ) ;
      }
   }
   else
      *phBMdn = 0 ;

   return TRUE ;
}


/************************************************************************
 *  static BOOL NEAR PASCAL PaintButton( HWND hWnd, HDC hDC )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ************************************************************************/
static BOOL NEAR PASCAL
PaintButton( HWND hWnd, HDC hDC )
{
   WORD   wStyle ;
   RECT   rect ;
   BOOL   f ;

   HDC     hDCMem ;
   HBITMAP hBMMem, hBMT ;

   GetClientRect( hWnd, &rect ) ;

   if (!RectVisible( hDC, &rect ))
      return TRUE ;

   wStyle = GETSTYLE( hWnd ) | (GETFLAGS( hWnd ) & 0xFF00) ;
   f      = GETSTATE( hWnd ) ;

   if (!(hDCMem = CreateCompatibleDC(hDC)))
   {
      DP1( hWDB,"bmpbtn: CreateCompatibleDC failed" ) ;
      return FALSE ;
   }

   if (!(hBMMem = CreateCompatibleBitmap(hDC,rect.right,rect.bottom)))
   {
      DP1( hWDB,"bmpbtn: CreateCompatibleBitmap failed" ) ;
      DeleteDC( hDCMem ) ;
      return FALSE ;
   }

   switch (LOBYTE(wStyle))
   {
      case BS_PUSHBUTTON:
      case BS_DEFPUSHBUTTON:
         if (hDCMem && hBMMem)
         {
            hBMT = SelectObject( hDCMem, hBMMem ) ;
            DrawGrayButton( hWnd, hDCMem, &rect, wStyle, f ) ;

            BitBlt( hDC, 0, 0, rect.right, rect.bottom,
                    hDCMem, 0, 0, SRCCOPY ) ;
            SelectObject( hDCMem, hBMT ) ;
         }
         else
         {
            DrawGrayButton( hWnd, hDC, &rect, wStyle, f ) ;
         }
         break ;
   }

   if (hBMMem)
       DeleteObject( hBMMem ) ;
   if (hDCMem)
       DeleteDC( hDCMem ) ;

   return TRUE ;

}/* PaintButton() */

/*************************************************************************
 *  static VOID NEAR PASCAL
 *  DrawGrayButton( HWND hWnd, HDC hDC,
 *                  LPRECT lpRect, WORD wStyle, BOOL fInvert)
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
static VOID NEAR PASCAL
DrawGrayButton( HWND hWnd, HDC hDC, LPRECT lpRect, WORD wStyle, BOOL fInvert)
{
   RECT        Rect ;
   int         dx, dy ;
   HBRUSH      hbr ;
   int         i ;
   int         iFrame ;
   LOCALHANDLE    lhBBC ;
   LPBMPBTNCOLOR  lpBBC ;

   if (lhBBC = GETCOLORS( hWnd ))
      lpBBC = (LPBMPBTNCOLOR)LocalLock( lhBBC ) ;


   SetBkColor( hDC, GetSysColor( COLOR_WINDOW) ) ;

   hbr = (HBRUSH)SendMessage( GetParent( hWnd ), WM_CTLCOLOR,
                              hDC, MAKELONG( hWnd, CTLCOLOR_BTN )) ;
   FillRect( hDC, lpRect, hbr ) ;

   Rect = *lpRect ;
   dx = Rect.right  - Rect.left ;
   dy = Rect.bottom - Rect.top ;

   if (!(wStyle & BS_STATIC))
   {
      iFrame = FRAMEWIDTH ;

      if (LOBYTE(wStyle) == BS_DEFPUSHBUTTON)
         iFrame *= 2 ;

      PatB( hDC, Rect.left+1,       Rect.top,           dx-2,   iFrame,
            rgbBlack ) ;

      PatB( hDC, Rect.left+1,       Rect.bottom-iFrame, dx-2,   iFrame,
            rgbBlack ) ;
            
      PatB( hDC, Rect.left,         Rect.top+1,         iFrame, dy-2,
            rgbBlack ) ;

      PatB( hDC, Rect.right-iFrame, Rect.top+1,         iFrame, dy-2,
            rgbBlack ) ;

      InflateRect( &Rect, -iFrame, -iFrame ) ;
      dx = Rect.right  - Rect.left ;
      dy = Rect.bottom - Rect.top ;

      if (lhBBC)
         SetBkColor( hDC, lpBBC->rgbFace ) ;
      else
         SetBkColor( hDC, rgbButtonFace ) ;

      EraseButton( hWnd, hDC, &Rect ) ;

      if (fInvert)
      {
         Rect.left += BEVELWIDTH*2 ;
         Rect.top  += BEVELWIDTH*2 ;
      }
      else
      {
         for (i=0 ; i<BEVELWIDTH ; i++)
         {        
            PatB( hDC, Rect.left,   Rect.top,      1,    dy,   rgbWhite ) ;
            PatB( hDC, Rect.left,   Rect.top,      dx,   1,    rgbWhite ) ;
            if (lhBBC)
            {
               PatB( hDC,
                     Rect.right-1,Rect.top+1,    1,    dy-1, lpBBC->rgbShadow ) ;
               PatB( hDC,
                     Rect.left+1, Rect.bottom-1, dx-1, 1,    lpBBC->rgbShadow ) ;
            }
            else
            {
               PatB( hDC,
                     Rect.right-1,Rect.top+1,    1,    dy-1, rgbButtonShadow ) ;
               PatB( hDC,
                     Rect.left+1, Rect.bottom-1, dx-1, 1,    rgbButtonShadow ) ;
            }

            InflateRect( &Rect,-1,-1 ) ;
            dx -= 2 ;
            dy -= 2 ;
         }
      }

      if (lhBBC)
      {
         SetBkColor( hDC, lpBBC->rgbFace ) ;

         if ( GetFocus() == hWnd )
            SetTextColor( hDC, lpBBC->rgbFocus ) ;
         else
            SetTextColor( hDC, lpBBC->rgbText ) ;
      }
      else
      {
         SetBkColor( hDC, rgbButtonFace ) ;

         if ( GetFocus() == hWnd )
            SetTextColor( hDC, rgbButtonFocus ) ;
         else
            SetTextColor( hDC, rgbButtonText ) ;
      }
   }

   if (lhBBC)
      LocalUnlock(lhBBC) ;

   DrawButtonFace( hWnd, hDC, (LPRECT)&Rect, wStyle, fInvert ) ;

}/* DrawGrayButton() */

/*************************************************************************
 *    static
 *    VOID NEAR PASCAL DrawButtonFace( HWND hWnd, HDC hDC, LPRECT lpRect,
 *                                     WORD wStyle, BOOL fInvert )
 *
 *  Description:
 *    Draws the text or bitmap onto the face of the button.
 *
 *  Type/Parameter
 *          Description
 *    HWND hWnd
 *          window handle of button
 *
 *    HDC  hDC
 *          Buttons DC
 *
 *    PRECT pRect
 *          clipping rect for button face
 *
 *    wStyle WORD
 *          style (push button or default pushbutton)
 * 
 *  Comments:
 *
 *************************************************************************/
static VOID NEAR PASCAL
DrawButtonFace( HWND hWnd, HDC hDC, LPRECT lpRect, WORD wStyle, BOOL fInvert )
{
   char        sz[MAX_WNDTEXTLEN+1] ;
   DWORD       dw ;
   int         len ;
   int         x,y ;
   RECT        Rect ;
   RECT        rc2 ;
   HBITMAP     hBM ;
   HDC         hDCBits ;
   BITMAP      bm ;
   BOOL        fMono ;
   LOCALHANDLE    lhBBC ;
   LPBMPBTNCOLOR  lpBBC ;

   if (lhBBC = GETCOLORS( hWnd ))
      lpBBC = (LPBMPBTNCOLOR)LocalLock( lhBBC ) ;

   rc2 = Rect = *lpRect ;

   SaveDC(hDC) ;

   IntersectClipRect(hDC, lpRect->left, lpRect->top, 
                          lpRect->right, lpRect->bottom) ;

   if (!fInvert || !(hBM = GETHBMDN( hWnd )))
       hBM = GETHBMUP(hWnd) ;
   else
   {
      if (fInvert)
      {
         Rect.top  -= BEVELWIDTH*2 ;
         Rect.left -= BEVELWIDTH*2 ;
      }
   }

   if (hBM)
   {
      if (!(hDCBits = CreateCompatibleDC( hDC )))
      {
         RestoreDC( hDC, -1 ) ;
         if (lhBBC)
            LocalUnlock( lhBBC ) ;
         return ;
      }

      SelectObject( hDCBits, hBM ) ;

      GetObject( hBM,sizeof(bm), (LPSTR)&bm ) ;

      fMono = (bm.bmPlanes == 1) && (bm.bmBitsPixel == 1) ;

      if (lhBBC)
         BitmapColorTranslate( hDCBits, &bm, lpBBC->rgbFace ) ;
      else
         BitmapColorTranslate( hDCBits, &bm, rgbButtonFace ) ;

      if (!(wStyle & BS_STRETCH))
      {
         // now center this thing on the button face
         Rect.left += (Rect.right - Rect.left - bm.bmWidth) / 2 ;
         Rect.top += (Rect.bottom - Rect.top - bm.bmHeight) / 2 ;
         Rect.right  = Rect.left + bm.bmWidth ;
         Rect.bottom = Rect.top + bm.bmHeight ;
      }

      SetStretchBltMode( hDC, fMono ? BLACKONWHITE : COLORONCOLOR ) ;

      if (IsWindowEnabled( hWnd ))
      {
         StretchBlt( hDC, Rect.left, Rect.top,
                     Rect.right  - Rect.left,
                     Rect.bottom - Rect.top,
                     hDCBits,0,0,
                     bm.bmWidth,bm.bmHeight, SRCCOPY) ;

         InflateRect( &rc2, -2, -2 ) ;

         if (lhBBC)
         {
            if (lpBBC->rgbFocus == lpBBC->rgbText && GetFocus() == hWnd)
               FrameRect( hDC, &rc2, hbrGray ) ;
         }
         else
         {
            if (rgbButtonFocus == rgbButtonText && GetFocus() == hWnd)
               FrameRect( hDC, &rc2, hbrGray ) ;
         }
      }
      else
      {
          SetBkColor(hDC,rgbWhite) ;
          SetTextColor(hDC,rgbBlack) ;
          SelectObject(hDC,hbrGray) ;

          StretchBlt(hDC,Rect.left,Rect.top,
              Rect.right  - Rect.left,
              Rect.bottom - Rect.top,
              hDCBits,0,0,
              bm.bmWidth,bm.bmHeight, 0x008E1d7cL ) ; // SSDxPDxax
      }
      DeleteDC( hDCBits ) ;
   }
   else
   {
      int xText, yText ;

      SelectObject( hDC, GETHFONT( hWnd ) ) ;
      GetWindowText( hWnd, sz, MAX_WNDTEXTLEN ) ;
      len = lstrlen( sz ) ;

      dw = GetTextExtent( hDC, sz, len ) ;

      x = (Rect.right + Rect.left) / 2 ;
      y = (Rect.bottom + Rect.top) / 2 ;

      xText = LOWORD(dw) / 2 ;
      yText = HIWORD(dw) / 2 ;

      if ((x = x - xText) < 0)
         x = 0 ;

      if ((y = y - yText) < 0)
         y = 0 ;

      Rect.left   = x ;
      Rect.top    = y ;
      Rect.right  = x + LOWORD(dw) ;
      Rect.bottom = y + HIWORD(dw) ;

      if (IsWindowEnabled(hWnd))
      {
         DrawText(hDC,sz,len,&Rect,DT_CENTER|DT_NOCLIP) ;

         if (LOBYTE(wStyle) == BS_DEFPUSHBUTTON)
         {
            Rect.left++ ;
            SetBkMode( hDC, TRANSPARENT ) ;
            DrawText( hDC, sz , len, &Rect, DT_CENTER | DT_NOCLIP ) ;
         }

         if (lhBBC)
         {
            if (lpBBC->rgbFocus == lpBBC->rgbText && GetFocus() == hWnd)
            {
               InflateRect( &Rect, 1, 0 ) ;
               FrameRect( hDC, &Rect, hbrGray ) ;
            }
         }
         else
            if (rgbButtonFocus == rgbButtonText && GetFocus() == hWnd)
            {
               InflateRect( &Rect, 1, 0 ) ;
               FrameRect( hDC, &Rect, hbrGray ) ;
            }
      }
      else
      {
         GrayString( hDC, NULL, NULL, (LONG)(LPSTR)sz, len,
                     Rect.left, Rect.top, 0, 0 ) ;
      }
   }

   RestoreDC(hDC, -1) ;

   if (lhBBC)
      LocalUnlock( lhBBC ) ;

}/* DrawButtonFace() */


/*************************************************************************
 *  static VOID NEAR PASCAL NotifyParent( HWND hWnd )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
static VOID NEAR PASCAL NotifyParent( HWND hWnd )
{
    PostMessage( GetParent(hWnd), WM_COMMAND,
                 GetWindowWord( hWnd, GWW_ID ), MAKELONG( hWnd,BN_CLICKED )) ;
}/* NotifyParent() */


/*************************************************************************
 *  static VOID NEAR PASCAL
 *  PatB( HDC hDC, int x, int y, int dx, int dy, DWORD rgb )
 *
 *  Description: 
 *    Fast Solid color PatBlt() using ExtTextOut()
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
static VOID NEAR PASCAL
PatB( HDC hDC, int x, int y, int dx, int dy, DWORD rgb )
{
    RECT    Rect ;

    SetBkColor(hDC,rgb) ;
    Rect.left   = x ;
    Rect.top    = y ;
    Rect.right  = x + dx ;
    Rect.bottom = y + dy ;

    ExtTextOut(hDC,0,0,ETO_OPAQUE,&Rect,NULL,0,NULL) ;
}/* PatB() */

/*************************************************************************
 *  LONG FAR PASCAL fnFastText( hWnd, msg, wParam, lParam )
 *
 *  Description: 
 *    static text control that uses ExtTextOut() IE no flicker!
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
LONG FAR PASCAL
fnFastText( HWND hWnd, unsigned msg, WORD wParam, LONG lParam )
{

   PAINTSTRUCT ps ;
   RECT Rect ;
   char        ach[128] ;
   int  len ;

   switch (msg)
   {
   case WM_SETTEXT:
       DefWindowProc(hWnd, msg, wParam, lParam) ;
       InvalidateRect(hWnd,NULL,FALSE) ;
       UpdateWindow(hWnd) ;
       return 0L ;

   case WM_ERASEBKGND:
       return 0L ;

   case WM_PAINT:
       BeginPaint(hWnd, &ps) ;
       GetClientRect(hWnd,&Rect) ;

       len = GetWindowText( hWnd, ach, sizeof(ach) ) ;
       SetBkColor( ps.hdc, GetSysColor( COLOR_WINDOW ) ) ;
       SetTextColor(ps.hdc, GetSysColor( COLOR_WINDOWTEXT ) ) ;
       ExtTextOut( ps.hdc, 0, 0, ETO_OPAQUE, &Rect, ach, len, NULL) ;

       EndPaint( hWnd, &ps ) ;
       return 0L ;
   }
   return DefWindowProc( hWnd, msg, wParam, lParam ) ;
}/* fnFastText() */


/************************************************************************
 * End of File: bmpbtn.c
 ************************************************************************/


