/************************************************************************
 *
 *     Project:  
 *
 *      Module:  listview.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "global.h"
#include <malloc.h>


typedef struct tagLV
{
   LPFACE      lpFace ;
   HFONT       hFont ;

} LV, FAR *LPLV ;

#define GWW_HLB      0
#define GWW_FLAGS    2
#define GWL_HHLB     4
#define CBWNDEXTRA   (sizeof( HWND ) + sizeof( WORD ) + sizeof( HHEAP ))

#define GET_HLB(hwnd)         (HWND)GetWindowWord(hwnd,GWW_HLB)
#define SET_HLB(hwnd,hwndLB)  (HWND)SetWindowWord(hwnd,GWW_HLB,(WORD)hwndLB)

#define GET_FLAGS(hwnd)         (WORD)GetWindowWord(hwnd,GWW_FLAGS)
#define SET_FLAGS(hwnd,hwndLB)  (WORD)SetWindowWord(hwnd,GWW_FLAGS,(WORD)hwndLB)
#define LVF_SHOWSAMPLE        0x0001

#define GET_HHLB(hwnd)        (HHEAP)GetWindowLong(hwnd,GWL_HHLB)
#define SET_HHLB(hwnd,hHeap)  (HHEAP)SetWindowLong(hwnd,GWL_HHLB,(DWORD)hwndLB)

static   int cyItem = 25 ;

LRESULT NEAR PASCAL ListViewOwnDraw( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;
BOOL NEAR PASCAL FillListBox( HWND hwnd, HWND hwndLB ) ;
int NEAR PASCAL     EmptyListBox( HWND hwnd, HWND hwndLB ) ;

/****************************************************************
 *  LRESULT CALLBACK fnListView( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT CALLBACK fnListView( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
   HWND           hwndLB = NULL ;
   int      n ;
   LPFACE   lpFace ;

   switch (uiMsg)
   {
      case WM_CREATE:

         hwndLB = CreateWindow
            (
               "listbox",
               "",
               WS_CHILD |
               WS_VISIBLE |
               WS_VSCROLL |
               LBS_NOINTEGRALHEIGHT |
               LBS_WANTKEYBOARDINPUT | 
               LBS_NOTIFY |
               LBS_OWNERDRAWVARIABLE |
               LBS_DISABLENOSCROLL,
               CW_USEDEFAULT,
               CW_USEDEFAULT,
               CW_USEDEFAULT,
               CW_USEDEFAULT,
               hwnd,                 
               (HMENU)0,
               ghinstApp,            
               NULL                  
            ) ;

         SET_HLB( hwnd, hwndLB ) ;

         SET_FLAGS( hwnd, LVF_SHOWSAMPLE ) ;

         SetWindowFont( hwndLB, ghfontSmall, FALSE ) ;

         FillListBox( hwnd, hwndLB ) ;

         SetFocus( hwndLB ) ;
      break ;

      case WM_FONTCHANGE:
         hwndLB = GET_HLB( hwnd ) ;
         EmptyListBox( hwnd, hwndLB ) ;
         FillListBox( hwnd, hwndLB ) ;
      break ;

      case WM_DRAWITEM:
      case WM_MEASUREITEM:
         return ListViewOwnDraw( hwnd, uiMsg, wParam, lParam ) ;
      break ; 

      case WM_SETFOCUS:
         hwndLB = GET_HLB( hwnd ) ;

         if (hwndLB && IsWindow( hwndLB ))
            SetFocus( hwndLB ) ;

      break ;

      case WM_SIZE:
         hwndLB = GET_HLB( hwnd ) ;

         /* Size lisbox to fit */

         if (hwndLB && IsWindow( hwndLB ))
         {
            SetWindowPos( hwndLB, NULL,
                           0, 0, LOWORD(lParam), HIWORD(lParam),
                        SWP_NOZORDER ) ;

         }
         goto CallDCP ;
      break ;

      case WM_GETMINMAXINFO:

         goto CallDCP ;
      break; 

      case WM_MDIACTIVATE:
         if (wParam)
            ghwndActiveMDIChild = hwnd ;
         else
            ghwndActiveMDIChild = NULL ;
      break ;

      case WM_DESTROY:
         hwndLB = GET_HLB( hwnd ) ;

         EmptyListBox( hwnd, hwndLB ) ;
         // FALLTHROUGH!

      default:
CallDCP:
         return DefMDIChildProc( hwnd, uiMsg, wParam, lParam ) ;

   }

   return 0 ;

} /* fnListView()  */

/****************************************************************
 *  BOOL NEAR PASCAL FillListBox( HWND hwnd, HWND hwndLB )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL FillListBox( HWND hwnd, HWND hwndLB )
{
   LPFACE      lpFace ;
   LPLV        lpLV ;
   HHEAP       hhLB = GET_HHLB( hwnd ) ;
   TEXTMETRIC  tm ;
   int         cyHeader ;
   int         cy ;
   int         n ;
   HDC         hdc ;
   HFONT       hfont ;
   WORD        wFlags = GET_FLAGS( hwnd ) ;

   if (!(hdc = GetDC( hwnd )))
      return FALSE ;

   if (hfont = GetWindowFont( hwndLB ))
   {
      hfont = SelectObject( hdc, hfont ) ;
      GetTextMetrics( hdc, &tm ) ;
      hfont = SelectObject( hdc, hfont ) ;
   }
   else
      GetTextMetrics( hdc, &tm ) ;

   cyHeader = tm.tmHeight+tm.tmExternalLeading + 2 ;

   SetWindowRedraw( hwndLB, FALSE ) ;

   if (hhLB)
      cekmmHeapDestroy( hhLB ) ;

   hhLB = cekmmHeapCreate( 1024, 0 ) ;
   DASSERT( hWDB, hhLB ) ;

   SET_HHLB( hwnd, hhLB ) ;

   /* Fill listbox from hlFaces...
    */
   if (lpFace = cekmmListFirstNode( ghlFaces ))
   {
      do
      {
         cy = cyHeader ;

         if (lpLV = (LPLV)cekmmHeapAllocLock( hhLB, 0, sizeof( LV ) ))
         {
            lpLV->lpFace = lpFace ;

            if (wFlags & LVF_SHOWSAMPLE)
            {
               if (lpLV->hFont = CreateFontIndirect( &lpFace->elf.elfLogFont ))
               {
                  hfont = SelectObject( hdc, lpLV->hFont ) ;
                  GetTextMetrics( hdc, &tm ) ;
                  SelectObject( hdc, hfont ) ;
                  cy += tm.tmHeight+tm.tmExternalLeading + 2 ;
               }
            }

            n = ListBox_AddItemData( hwndLB, lpLV ) ;

            ListBox_SetItemHeight( hwndLB, n, cy ) ;
         }

      }
      while (lpFace = cekmmListNextNode( ghlFaces, lpFace )) ;
   }

   SetWindowRedraw( hwndLB, TRUE ) ;

   ReleaseDC( hwnd, hdc ) ;

} /* FillListBox()  */

int NEAR PASCAL EmptyListBox( HWND hwnd, HWND hwndLB )
{
   LPLV     lpLV ;
   HHEAP    hhLB ;
   int n ;
   int m ;

   if ((m = n = ListBox_GetCount( hwndLB )) > 0)
   {
      while (n)
      {
         if (lpLV = (LPLV)ListBox_GetItemData( hwndLB, n - 1 ))
         {
            if (lpLV->hFont)
               DeleteObject( lpLV->hFont ) ;
         }

         n-- ;
      }
   }

   hhLB = GET_HHLB( hwnd ) ;
   if (hhLB)
      cekmmHeapDestroy( hhLB ) ;

   return m ;
}

/****************************************************************
 *  LRESULT NEAR PASCAL ListViewOwnDraw( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT NEAR PASCAL ListViewOwnDraw( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
   static DRAWITEMSTRUCT   disPrev ;
   DRAWITEMSTRUCT          disCur ;
   HDC                     hDC ;
   LPSTR                   lpszOutBuf ;
   short                   y ;
   short                   dx ;
   HFONT                   hfont ;
   LPLV                    lpLV ;

   switch( uiMsg )
   {
      case WM_DRAWITEM:
      {
         disCur = *(LPDRAWITEMSTRUCT)lParam ;

         if (!(lpLV = (LPLV)disCur.itemData))
         {
            DP1( hWDB, "ListViewOwnDraw(): itemData bad" ) ;
            disPrev = disCur ;
            return (LRESULT)-1 ;
         }

         hfont = GetWindowFont( disCur.hwndItem ) ;
DASSERT( hWDB, hfont ) ;
         dx = disCur.rcItem.right - disCur.rcItem.left ;

         /*
         * Get everything into local's for speed.
         */
         hDC = disCur.hDC ;

         if ((disCur.itemState & ODS_SELECTED))
         {
            SetTextColor( hDC, GetSysColor( COLOR_HIGHLIGHTTEXT ) ) ;
            SetBkColor( hDC, GetSysColor( COLOR_HIGHLIGHT ) ) ;
         }
         else
         {
            SetTextColor( hDC, GetSysColor( COLOR_WINDOWTEXT ) ) ;
            SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;
         }

         if ((disCur.itemAction & ODA_FOCUS) &&
             !(disCur.itemState  & ODS_FOCUS))
            DrawFocusRect( hDC, &disCur.rcItem ) ;

         if ((disCur.itemID != disPrev.itemID) ||
             (disCur.itemAction & ODA_DRAWENTIRE) || 
             (disCur.itemState  & ODS_SELECTED) !=
             (disPrev.itemState & ODS_SELECTED)    )
         {
            TEXTMETRIC    tm ;

            ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &disCur.rcItem, NULL, 0, NULL) ;
            SetBkMode( hDC, TRANSPARENT ) ;

            hfont = SelectObject( hDC, hfont ) ;

            GetTextMetrics( hDC, &tm ) ;

            TextOut( hDC, disCur.rcItem.left, disCur.rcItem.top,
               lpLV->lpFace->elf.elfLogFont.lfFaceName,
               lstrlen( lpLV->lpFace->elf.elfLogFont.lfFaceName ) ) ;

            hfont = SelectObject( hDC, hfont ) ;
         }

         /*
          * if we are gaining focus draw the focus rect after drawing
          * the text.
          */
         if ((disCur.itemAction & ODA_FOCUS) &&
               (disCur.itemState & ODS_FOCUS))
               DrawFocusRect( hDC, &disCur.rcItem ) ;

         disPrev = disCur ;

      }
      break ;

      default:
         return DefMDIChildProc( hwnd, uiMsg, wParam, lParam ) ;

   }

   return 0L ;


} /* ListViewOwnDraw()  */


/****************************************************************
 *  BOOL WINAPI RegisterListView( HINSTANCE hinst )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI RegisterListView( HINSTANCE hinst )
{
   WNDCLASS    wc ;

   wc.style             = 0L ;
   wc.lpfnWndProc       = fnListView ;
   wc.cbClsExtra        = 0 ;
   wc.cbWndExtra        = CBWNDEXTRA ;
   wc.hInstance         = hinst ;
   wc.hIcon             = LoadIcon( hinst,
                                    MAKEINTRESOURCE( IDI_TTVIEWCHILD ) ) ;
   wc.hCursor           = LoadCursor( NULL, IDC_ARROW ) ;
   (WORD)wc.hbrBackground  = COLOR_WINDOW + 1 ;
   wc.hIcon             = LoadIcon( hinst,
                                    MAKEINTRESOURCE( IDI_TTVIEWCHILD ) ) ;
   wc.lpszMenuName      = NULL ;
   wc.lpszClassName     = "TTVIEW.LIST" ;

   if (!RegisterClass (&wc))
      return FALSE ;

} /* RegisterListView()  */

/****************************************************************
 *  HWND WINAPI CreateListView(VOID)
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
HWND WINAPI CreateListView(VOID)
{
   HWND  hwnd ;
   MDICREATESTRUCT mcs ;

   mcs.szTitle = "TrueType Font List View" ;
   mcs.szClass = "TTVIEW.LIST" ;
   mcs.hOwner = ghinstApp ;
   mcs.x = mcs.cx = CW_USEDEFAULT ;
   mcs.y = mcs.cy = CW_USEDEFAULT ;
   mcs.style = 0 ;

   hwnd = (HWND)SendMessage( ghwndMDIClient,
                                 WM_MDICREATE,
                                 0,
                                 (LPARAM)(LPMDICREATESTRUCT)&mcs ) ;

   return hwnd ;
} /* CreateListView()  */

/************************************************************************
 * End of File: listview.c
 ***********************************************************************/


