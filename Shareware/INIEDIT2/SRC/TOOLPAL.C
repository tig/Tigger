/************************************************************************
 *
 *     Project:  Tool Palette
 *
 *      Module:  toolpal.c
 *
 *     Remarks:
 *
 *    This module implements a flexible tool palette window class.
 *    See the WinWord document TOOLPAL.DOC detailed specifications.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#define WINDLL
#define STRICT

#include <windows.h>
#include <wdb.h>
#include <memory.h>

#ifdef SAMPLE
   #include "toolpal.h"
   #include "itsybits.h"
   #include "bmutil.h"
#else
   #include "toolpal.h"
   #include "bmutil.h"
#endif

/************************************************************************
 * Internal constants
 ***********************************************************************/

#define DRAW_TO_MEMORY_DC

/* this should not be needed anymore since the 8514 driver has
 * been fixed in 3.1 (see fWin31)
 */
//#define USE_EXTTEXTOUTONLY   


//
// Default values
//
#define DEFAULT_NUMCOLS    2
#define DEFAULT_fREDRAW    TRUE

#define DEFAULT_ITEMEXTRAX (2 * BTN_BEVELWIDTH)
#define DEFAULT_ITEMEXTRAY (2 * BTN_BEVELWIDTH)

#define DEFAULT_BORDERWIDTH   1  // 4

#define DEFAULT_ITEMSIZEX  (DEFAULT_ITEMEXTRAX + 32)
#define DEFAULT_ITEMSIZEY  (DEFAULT_ITEMEXTRAY + 32)

//
// Width of the standard bevel
//
#define  BTN_BEVELWIDTH     2

//
// Width of the frame without the def pushbutton style.  Def pushbutton
// is 2 * BTN_FRAMEWIDTH
//
#define  BTN_FRAMEWIDTH     1

//
// Indexes into the "stored" bitmap (i.e. rows)
//
#define BMP_UP       0
#define BMP_DN       1
#define BMP_DISABLED 2

/************************************************************************
 * Internal Macros
 ***********************************************************************/
//
// Instanced data is LocalAlloc'd and the handle is stored in 
// the GWW_LHINSTDATA WindowWord.
//
#define NUMEXTRABYTES            (1*sizeof(LOCALHANDLE))
#define GWW_LHINSTDATA           0
#define GETLHINSTDATA(hwnd)      (LOCALHANDLE)GetWindowWord( hwnd, GWW_LHINSTDATA )
#define SETLHINSTDATA(hwnd,lh)   (LOCALHANDLE)SetWindowWord( hwnd, GWW_LHINSTDATA, (WORD)lh )
#define ALLOCINSTDATA()          LocalAlloc( LPTR, sizeof(INSTDATA) )
#define LOCKINSTDATA(lh)         (PINSTDATA)LocalLock( lh )
#define UNLOCKINSTDATA(lh)       LocalUnlock( lh )
#define FREEINSTDATA(lh)         LocalFree( lh )

//
// Macros for manipulating (private) toolpal items 
//
#define ALLOCITEM()       LocalAlloc( LPTR, sizeof(ITEM) )
#define ALLOCITEMS(n)     LocalAlloc( LPTR, n*sizeof(ITEM) )
#define REALLOCITEM(lh,n) LocalReAlloc( lh, sizeof(ITEM)*(n), LPTR )
#define LOCKITEM(lh)      (PITEM)LocalLock( lh )
#define UNLOCKITEM(lh)    LocalUnlock( lh )
#define FREEITEM(lh)      LocalFree( lh )

#define GETSTYLE(hwnd)    LOWORD(GetWindowLong(hwnd,GWL_STYLE))

#define ISCHILDWND(hwnd)\
   (BOOL)(HIWORD((DWORD)GetWindowLong(hwnd,GWL_STYLE) & (DWORD)WS_CHILD))

#ifdef USE_EXTTEXTOUTONLY
   #define DRAWFASTRECT(hdc,lprc) ExtTextOut(hdc,0,0,ETO_OPAQUE,lprc,NULL,0,NULL)
#else
   #define DRAWFASTRECT(hdc,lprc)\
   {\
      if (fWin31)\
         ExtTextOut(hdc,0,0,ETO_OPAQUE,lprc,NULL,0,NULL) ;\
      else\
      {\
         HBRUSH hbr = CreateSolidBrush( GetBkColor( hdc ) ) ;\
         hbr = SelectObject(hdc, hbr) ;\
         PatBlt(hdc,(lprc)->left,(lprc)->top,\
                (lprc)->right-(lprc)->left,\
                (lprc)->bottom-(lprc)->top,PATCOPY) ;\
         hbr = SelectObject(hdc, hbr) ;\
         DeleteObject( hbr ) ;\
      }\
   }
#endif

#define DPRECT( lprc )  {\
                        DP( hWDB, "top = %d, left = %d, bottom = %d, right = %d",\
                        lprc->top, lprc->left, lprc->bottom, lprc->right ) ;\
                        }
#define DPVALIDATE( lptp ) {\
                              DASSERTMSG( hWDB,\
                                (LPTOOLPALITEM)lptp->dwSize != sizeof(TOOLPALITEM),\
                                "Invalid TOOLPALITEM" ) ;\
                           }

/************************************************************************
 * Internal data structures
 ***********************************************************************/
typedef struct tagINSTDATA
{

   WORD           wNumItems ;
   LOCALHANDLE    lhrgItems ;    // array[wNumItems] of TOOLAPALITEMS

   short          nBmpSizeX ;    // size of the bitmaps in x
   short          nBmpSizeY ;    // size of the bitmaps in y

   short          nItemSizeX ;    // size of the item in x
   short          nItemSizeY ;    // size of the item in y

   short          nBorderWidth ; 

   WORD           wNumCols ;     // Number of rows or columns

   WORD           wCurSelected ; // Current item selected (TPS_AUTOSELECT)

   //
   // Bitmap handles.
   //
   HBITMAP        hbmTable ;      // All bitmaps (up, dn, and disabled)
   HBITMAP        hbmCur ;       // Current "snapshot" of the toolpal

   WORD           fRedraw    ; // Do we need to redraw any or just use the
                                  // snapshot?
   WORD           fRedrawAll ; // Do we need to redraw or just use the
                                  // snapshot?
   HWND           hwndParent ;

   WORD           wNotifyMsg ;

   #if 0
   BOOL           fRedrawOn ;    // have we gotten a WM_REDRAW?
   #endif

} INSTDATA, *PINSTDATA, FAR *LPINSTDATA ;


/*
 * State data for each item.  
 */
#define TP_MAXTEXTLEN   32

typedef struct tagITEM
{
   BOOL     fSelected ;
   BOOL     fEnabled ;
   WORD     wID ;
   BOOL     fChanged ;

} ITEM, *PITEM, FAR *LPITEM ;


/************************************************************************
 * Local variables
 ***********************************************************************/
static char    szToolPalClassName[] = TP_SZCLASSNAME ;
static BOOL    fWin31 ;

/************************************************************************
 * Local function prototypes
 ***********************************************************************/
LRESULT FAR  PASCAL fnToolPal( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;
LRESULT NEAR PASCAL tpControl( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;
BOOL NEAR PASCAL tpPaint( HWND hWnd ) ;
VOID NEAR PASCAL tpDrawBevel( HWND hWnd, HDC hDC, LPRECT lprc, BOOL fUp, BOOL fBorders ) ;
BOOL NEAR PASCAL tpReSize( HWND hWnd, PINSTDATA pIDin ) ;
VOID NEAR PASCAL tpGetItemRect( HWND hWnd, WORD wID, LPRECT lprc ) ;
WORD NEAR PASCAL tpHitTest( HWND hWnd, short x, short y ) ;
BOOL NEAR PASCAL tpNotify( HWND hWnd, WORD wID, WORD wNotification ) ;

HBITMAP NEAR PASCAL
   tpGangInsertBitmap( HWND hWnd, WORD wIndex,
                       LPINSTDATA lpID, LPTOOLPALITEM lptpItem ) ;
HBITMAP NEAR PASCAL
   SetupInsert( HDC hdcDest, LPINSTDATA lpID, HBITMAP hbmUp,
                WORD wIndex, WORD wNumItems, WORD wNumNewItems ) ;

HBITMAP NEAR PASCAL
   tpRemoveBitmap( HWND hWnd, WORD wIndex, LPINSTDATA lpID ) ;

/************************************************************************
 * EXPORTED functions
 ***********************************************************************/
/****************************************************************
 *  BOOL WINAPI tpRegister( HINSTANCE hInstance )
 *
 *  Description: 
 *
 *    This function registers the toolpal window class.  It also
 *    sets up some internal color stuff?
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI tpRegister( HINSTANCE hInstance )
{
   WNDCLASS    wc ;

   fWin31 = (BOOL)(LOWORD( GetVersion() ) >= 0x0A03) ;

   wc.style         = CS_BYTEALIGNWINDOW ;
   (WNDPROC)wc.lpfnWndProc = fnToolPal ;  
                                 
   wc.cbClsExtra    = 0 ;
   wc.cbWndExtra    = NUMEXTRABYTES ;             
   wc.hInstance     = hInstance ; 
   wc.hIcon         = NULL ;
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW ) ;
   (WORD)wc.hbrBackground = COLOR_BTNFACE + 1 ;
   wc.lpszMenuName  = NULL ;
   wc.lpszClassName = szToolPalClassName ;

   return RegisterClass(&wc) ;

} /* tpRegister()  */

/************************************************************************
 * Local functions
 ***********************************************************************/
/****************************************************************
 *  LRESULT CALLBACK
 *    fnToolPal( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    The Toolpalette window procedure.  This function handles
 *    all standard Windows messages.  All "private" messages are
 *    passed on to tpControl().
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT CALLBACK fnToolPal( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{
   LOCALHANDLE    lh ;
   PINSTDATA      pID ;
   static WORD           wID ;
   LRESULT        lResult ;

   switch (wMsg)
   {
      case WM_LBUTTONDOWN:
         /* huh? Why is this here? */
         if (!IsWindowEnabled( hWnd ))
            return 0L ;

         wID = tpHitTest( hWnd, LOWORD( lParam ), HIWORD( lParam ) ) ;

         if (wID == 0xFFFF)
            break ;

         if (!(GETSTYLE( hWnd ) & TPS_TOOLBAR))
         {
            lResult = tpControl( hWnd, TP_TOGGLEITEMSTATE, (WPARAM)wID, 0L ) ;
            if (lResult != TP_ERR)
               tpNotify( hWnd, wID, TPN_STATECHANGE | LOWORD((DWORD)lResult) ? 0 : TPDEPRESSED ) ;

            break ;
         }

         if (GetCapture() != hWnd)  /* ignore multiple DOWN's */
         {
            tpControl( hWnd, TP_SETITEMSTATE, (WPARAM)wID, 
                       MAKELPARAM( TRUE, 0 ) ) ;

            SetCapture( hWnd ) ;

         }
      break ;

      case WM_MOUSEMOVE:
         if ((GETSTYLE( hWnd ) & TPS_TOOLBAR) &&
             (GetCapture() == hWnd))
         {
            WORD wIDup ;

            wIDup = tpHitTest( hWnd, LOWORD( lParam ), HIWORD( lParam ) ) ;

            if (wIDup == wID)
            {
               if (!tpControl( hWnd, TP_GETITEMSTATE, (WPARAM)wID, 0L ))
                  tpControl( hWnd, TP_SETITEMSTATE, (WPARAM)wID,
                           MAKELPARAM( TRUE, 0 ) ) ;
            }
            else
            {
               if (tpControl( hWnd, TP_GETITEMSTATE, (WPARAM)wID, 0L ))
                  tpControl( hWnd, TP_SETITEMSTATE, (WPARAM)wID,
                           MAKELPARAM( FALSE, 0 ) ) ;
            }
         }
         else
            return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
      break ;

      case WM_LBUTTONUP:
         if ((GETSTYLE( hWnd ) & TPS_TOOLBAR) &&
             (GetCapture() == hWnd))
         {
            ReleaseCapture() ;

            if (wID != tpHitTest( hWnd, LOWORD( lParam ), HIWORD( lParam ) ))
               break ;

            if (TP_ERR != (lResult = tpControl( hWnd, TP_SETITEMSTATE,
                                                (WPARAM)wID,
                                                MAKELPARAM( FALSE, 0 ) )))
            {
               tpNotify( hWnd, wID, TPN_STATECHANGE | TPDEPRESSED ) ;
               break ;
            }
         }
         else
            return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
      break ;

      case WM_CREATE:
      {
         LPTOOLPALITEM     lptpGang ;
         LPCREATESTRUCT    lpCS = (LPCREATESTRUCT)lParam ;
         PITEM             rgItems ;
         WORD              i ;

         //
         // Alloc the instanced data
         //
         if (lh = ALLOCINSTDATA())
            SETLHINSTDATA( hWnd, lh ) ;
         else
         {
            DP1( hWDB, "fnToolPal: Alloc instance data failed!" ) ;
            return (LRESULT)-1 ;
         }

         if (pID = LOCKINSTDATA( lh ))
         {
            pID->wNumItems    = 0 ;
            pID->lhrgItems    = NULL ;
            pID->nBmpSizeX = pID->nBmpSizeY = 0 ;
            pID->nItemSizeX = pID->nItemSizeY = 0 ;

            pID->wNotifyMsg = 0 ;

            pID->nBorderWidth = DEFAULT_BORDERWIDTH ;

            pID->wNumCols     = DEFAULT_NUMCOLS ;
            pID->wCurSelected = 0 ;

            pID->hbmTable      = NULL ;
            pID->hbmCur       = NULL ;

            pID->fRedrawAll   = DEFAULT_fREDRAW ;  // Redraw by default
            pID->fRedraw      = DEFAULT_fREDRAW ;  // Redraw by default

            pID->hwndParent   = GetParent( hWnd ) ;

            /*
             * Do gang load if needed
             */
            if (lptpGang = (LPTOOLPALITEM)lpCS->lpCreateParams) 
            {
               DASSERTMSG( hWDB,
                           lptpGang->dwSize == sizeof(TOOLPALITEM),
                           "Invalid TOOLPALITEM" ) ;

               if ((lptpGang->dwSize == sizeof(TOOLPALITEM)) &&
                   lptpGang->cGangLoadItems &&
                   tpGangInsertBitmap( hWnd, 0, pID, lptpGang ))
               {
                  pID->wNumItems = lptpGang[0].cGangLoadItems ;

                  if (pID->lhrgItems = ALLOCITEMS( lptpGang->cGangLoadItems ))
                  {
                     rgItems = LOCKITEM( pID->lhrgItems ) ;

                     DASSERT( hWDB, rgItems ) ;

                     for (i = 0 ; i < lptpGang->cGangLoadItems ; i++)
                     {
                        rgItems[i].wID = lptpGang[i].wID ;
                        rgItems[i].fChanged = TRUE ;
                        rgItems[i].fSelected = FALSE ;
                        rgItems[i].fEnabled = TRUE ;
                     }

                     UNLOCKITEM( pID->lhrgItems ) ;
                  }
               }
            }

            tpReSize( hWnd, pID ) ;

            UNLOCKINSTDATA( lh ) ;
         }

         DASSERT( hWDB, pID ) ;
      }
      break ;

      case WM_CLOSE:
         DP5( hWDB, "WM_CLOSE toolpal" ) ;

         if (pID = LOCKINSTDATA( lh = GETLHINSTDATA( hWnd ) ))
         {
            BOOL  fClose ;
            //
            // Notify parent that we're closing down.  If the parent
            // returns zero then we can close, otherwise he doesn't 
            // want us to go away...he'll probably want to hide us,
            // but that's up to him.
            //
            fClose = (BOOL)LOWORD( SendMessage( pID->hwndParent, WM_COMMAND,
                      GetWindowWord( hWnd, GWW_ID ),
                      MAKELPARAM( hWnd, TPN_QUERYCLOSE ) ) ) ;

            UNLOCKINSTDATA( lh ) ;

            if (!fClose)
               return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
         }
      break ;

      case WM_SETREDRAW:
         if (pID = LOCKINSTDATA( lh = GETLHINSTDATA( hWnd ) ))
         {
            pID->fRedraw = ((WORD)wParam ? TRUE : FALSE) ;
            UNLOCKINSTDATA( lh ) ;
         }
      break ;

      case WM_PAINT:
         if (!tpPaint( hWnd ))
            return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
      break ;

      case WM_SYSCOLORCHANGE:
         //
         // Get the stored bitmap, and do a bmColorTranslateDC() on
         // it.
         //
         if (pID = LOCKINSTDATA( lh = GETLHINSTDATA( hWnd ) ))
         {
            if (pID->hbmTable)
            {
               HDC      hDC ;
               HDC      hdcMem ;
               BITMAP   BM ;

               GetObject( pID->hbmTable, sizeof(BITMAP), (LPSTR)&BM ) ;

               hDC = GetDC( hWnd ) ;
               hdcMem = CreateCompatibleDC( hDC ) ;
               ReleaseDC( hWnd, hDC ) ;

               pID->hbmTable = SelectObject( hdcMem, pID->hbmTable ) ;

               bmColorTranslateDC( hdcMem, &BM, GetPixel( hdcMem, 0, 0),
                                   GetSysColor( COLOR_BTNFACE ), BM_NORMAL ) ;
                      
               pID->hbmTable = SelectObject( hdcMem, pID->hbmTable ) ;

               DeleteDC( hdcMem ) ;

               pID->fRedraw = pID->fRedrawAll = TRUE ;
            }

            UNLOCKINSTDATA( lh ) ;

            InvalidateRect( hWnd, NULL, FALSE ) ;
         }
      break ;

      case WM_DESTROY:
         DP5( hWDB, "WM_DESTROY toolpal" ) ;

         DASSERT( hWDB, GETLHINSTDATA( hWnd ) ) ;

         if (pID = LOCKINSTDATA( lh = GETLHINSTDATA( hWnd ) ))
         {
            if (pID->hbmCur)
               DeleteObject( pID->hbmCur ) ;

            if (pID->hbmTable)
               DeleteObject( pID->hbmTable ) ;

            //
            // Free any items
            //
            DP5( hWDB, "FREEITEM..." ) ;
            if (pID->lhrgItems)
               FREEITEM( pID->lhrgItems ) ;

            DP5( hWDB, "UNLOCKINSTDATA..." ) ;
            UNLOCKINSTDATA( lh ) ;
            FREEINSTDATA( lh ) ;
         }
      break ;

	   case WM_COMMAND:
         DP5( hWDB, "WM_COMMAND" ) ;
         return DefWindowProc( hWnd, WM_SYSCOMMAND, wParam, 0L ) ;
      break ;

      default:
         if (lResult = tpControl( hWnd, wMsg, wParam, lParam ))
            return lResult ;
         else
         {
            #ifdef _ITSYBITS_H_
            return ibDefWindowProc( hWnd, wMsg, wParam, lParam ) ;
            #else
            return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
            #endif
         }
   }

   return 0L ;
} /* fnToolPal()  */

BOOL NEAR PASCAL tpNotify( HWND hWnd, WORD wID, WORD wNot )
{
   LOCALHANDLE    lh ;
   PINSTDATA      pID ;
   PITEM          rgItems ;

   if (!(pID = LOCKINSTDATA( lh = GETLHINSTDATA( hWnd ) )))
   {
      DP1( hWDB, "tpNotify: Could not lock data" ) ;
      return FALSE ;

   }

   if (pID->wNotifyMsg && (rgItems = LOCKITEM( pID->lhrgItems )))
   {
      BOOL f = FALSE ;

      DASSERTMSG( hWDB, pID->hwndParent, "No hwndParent to notify!" ) ;

      if (pID->hwndParent)
      {
         /*
          * Check keyboard and set bits as appropriate.
          */
         if (GetKeyState( VK_SHIFT ) & 0x8000)
            wNot |= TPSHIFT ;

         if (GetKeyState( VK_CONTROL ) & 0x8000)
            wNot |= TPCTRL ;

         DP5( hWDB, "Posting message: %04X, %04X, %08lX",
            pID->wNotifyMsg, rgItems[wID].wID,
            MAKELPARAM( hWnd, wNot ) ) ;
         f = PostMessage( pID->hwndParent, pID->wNotifyMsg,
                          (WPARAM)rgItems[wID].wID,
                          MAKELPARAM( hWnd, wNot ) ) ;
      }

      UNLOCKITEM( pID->lhrgItems ) ;
      UNLOCKINSTDATA( lh ) ;

      return f ;
   }

   UNLOCKINSTDATA( lh ) ;

   DP3( hWDB, "tpNotify: No notification messages specified, using WM_COMMAND" ) ;

   if (ISCHILDWND( hWnd ))
      return (BOOL)(DWORD)SendMessage( GetParent(hWnd), WM_COMMAND,
                          (WPARAM)GetWindowWord( hWnd, GWW_ID ),
                          MAKELPARAM( hWnd, wID )) ;
}

/****************************************************************
 *  LRESULT NEAR PASCAL
 *    tpControl( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    This function handles all messages private to ToolPal.
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT NEAR PASCAL tpControl( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{
   LRESULT        lResult = 0L ;
   
   LOCALHANDLE    lh ;
   PINSTDATA      pID ;

   LOCALHANDLE    lhItems ;
   PITEM          rgItems ;
   WORD           w, i ;
   LPTOOLPALITEM  lptp = (LPTOOLPALITEM)lParam ;
   WORD           wNumNewItems ;

   RECT           rc ;

   if (wMsg < TP_FIRSTMESSAGE || wMsg > TP_LASTMESSAGE)
      return FALSE ;

   if (!(pID = LOCKINSTDATA( lh = GETLHINSTDATA( hWnd ) )))
   {
      DP1( hWDB, "tpControl: Could not lock data" ) ;
      return TP_ERR ;
   }

   switch (wMsg)
   {
      case TP_SETNUMCOLS:
         DP4( hWDB, "TP_SETNUMCOLS:    %04X, %08lX", wParam, lParam ) ;

         lResult = MAKELRESULT( pID->wNumCols, 0 ) ;

         if (wParam)
            pID->wNumCols = (WORD)wParam ;
         else
         {   
            lResult = TP_ERR ;
            break ;
         }

         tpReSize( hWnd, pID ) ;

         InvalidateRect( hWnd, NULL, TRUE ) ;
         UpdateWindow( hWnd ) ;
      break ;

      case TP_ADDITEM:
         DP4( hWDB, "TP_ADDITEM:       %04X, %08lX", wParam, lParam ) ;

         //
         // We cheat here and just "insert" the item to the end of 
         // the list.
         //
         wParam = (WPARAM)0xFFFF ;

      /********** FALL THROUGH!! **********/

      case TP_INSERTITEM:
         DP4( hWDB, "TP_INSERTITEM:    %04X, %08lX", wParam, lParam ) ;

         DASSERT( hWDB, lptp ) ;

         if (!lptp)
         {
            DP4( hWDB, "TP_INSERTITEM: lParam invalid" ) ;
            lResult = TP_ERR ;
         }

         /*
          * Force wParam to be valid
          */
         (WORD)wParam = min( (WORD)wParam, pID->wNumItems ) ;

         DASSERTMSG( hWDB, lptp->dwSize == sizeof(TOOLPALITEM),
                           "Invalid TOOLPALITEM" ) ;

         if ((lptp->dwSize == sizeof(TOOLPALITEM)) &&
               tpGangInsertBitmap( hWnd, (WORD)wParam, pID, lptp ))
         {
            /*
             * HACK! We allow cGangLoadItems to be 0
             */
            wNumNewItems = max( 1, lptp[0].cGangLoadItems ) ;

            /*
             * If there are no items, then allocate
             */
            if (!pID->wNumItems)
            {
               if (!(pID->lhrgItems = ALLOCITEMS( wNumNewItems )))
               {
                  DP1( hWDB, "Could not allocate first item!" ) ;
                  lResult = TP_ERR ;
                  break ;
               }
            }
            else
            {
               /*
                * There already are some, so re-alloc
                */
               if (!(lhItems = REALLOCITEM( pID->lhrgItems,
                                            pID->wNumItems+wNumNewItems )))
               {
                  DP1( hWDB, "Could not re-allocate ITEMS!" ) ;
                  lResult = TP_ERR ;
                  break ;
               }
               pID->lhrgItems = lhItems ;
            }

            rgItems = LOCKITEM( pID->lhrgItems ) ;

            DASSERT( hWDB, rgItems ) ;

            /*
             * If we inserted then the states need to be updated.  If
             * we added (we will add if wParam is >= the number of
             * current items) we are pretty much done.
             */
            if ((WORD)wParam < pID->wNumItems)
            {
               /*
                * Insert
                * 
                * Do some shifting
                */

               for (w = pID->wNumItems + wNumNewItems - 1 ;
                    w >= (WORD)wParam + wNumNewItems ; w--)
               {
                  rgItems[w] = rgItems[w - wNumNewItems] ;
                  rgItems[w].fChanged = TRUE ;
               }

               if (pID->wCurSelected >= (WORD)wParam)
                  pID->wCurSelected += wNumNewItems ;
            }
            /*
             * All the new guys need to be initialized
             */
            for (i = (WORD)wParam  ; i < ((WORD)wParam + wNumNewItems) ; i++)
            {
               rgItems[i].wID = lptp[i].wID ;
               rgItems[i].fChanged = TRUE ;
               rgItems[i].fSelected = FALSE ;
               rgItems[i].fEnabled = TRUE ;
            }

            UNLOCKITEM( pID->lhrgItems ) ;

            pID->wNumItems += wNumNewItems;

            tpReSize( hWnd, pID ) ;
            UpdateWindow( hWnd ) ;
         }
         else
         {
            DP4( hWDB, "TP_INSERTITEM returning error" ) ;
            lResult = TP_ERR ;
         }
      break ;

      #if 0
      case TP_REMOVEITEM:
         DP4( hWDB, "TP_REMOVEITEM:    %04X, %08lX", wParam, lParam ) ;

         wParam = -1 ;

      /********** FALL THROUGH!! **********/
      #endif 

      case TP_DELITEM:
         DP4( hWDB, "TP_DELITEM:       %04X, %08lX", wParam, lParam ) ;

         if (!pID->lhrgItems || !pID->wNumItems)
         {
            DP1( hWDB, "No items!" ) ;
            lResult = TP_ERR ;
         }

         rgItems = LOCKITEM( pID->lhrgItems ) ;

         if ((WORD)wParam < pID->wNumItems)
         {
            //
            // Shift all past the one being deleted down
            //
            for ( w = (WORD)wParam ; w < pID->wNumItems - 1 ; w++)
            {
               rgItems[w] = rgItems[w+1] ;
               rgItems[w].fChanged = TRUE ;
               //tpGetItemRect( hWnd, w, &rc ) ;
            }

            if (pID->wCurSelected > (WORD)wParam)
               pID->wCurSelected-- ;
            else
               if (pID->wCurSelected == (WORD)wParam)
                  pID->wCurSelected = 0xFFFF ;
         }
         else
         {
            UNLOCKITEM( pID->lhrgItems ) ;
            lResult = TP_ERR ;
            break ;
         }

         UNLOCKITEM( pID->lhrgItems ) ;
         //
         // De-allocate the last guy
         //
         if (pID->wNumItems == 1)
         {
            pID->lhrgItems = FREEITEM( pID->lhrgItems ) ;
            DASSERTMSG( hWDB, !pID->lhrgItems, "FREEITEM failed." ) ;
         }
         else
            if (!(pID->lhrgItems = REALLOCITEM( pID->lhrgItems,
                                                pID->wNumItems - 1 )))
            {
               pID->wNumItems = 0 ;
               lResult = TP_ERR ;
               DASSERTMSG( hWDB, 0, "REALLOCITEM failed." ) ;
               break ;
            }
         //
         // Remove a chunk from the bitmap!!!
         //
         if (tpRemoveBitmap( hWnd, (WORD)wParam,
                             pID ))
         {
            lResult = MAKELRESULT((WORD)--(pID->wNumItems), 0 ) ;
            tpReSize( hWnd, pID ) ;
            UpdateWindow( hWnd ) ;
         }
         else
         {
            DP4( hWDB, "TP_DELITEM returning error" ) ;
            lResult = TP_ERR ;
         }
      break ;

      case TP_CHANGEITEM:
         DP4( hWDB, "TP_CHANGEITEM:    %04X, %08lX", (WORD)wParam, lParam ) ;

         DASSERTMSG( hWDB, lptp->dwSize == sizeof(TOOLPALITEM),
                           "Invalid TOOLPALITEM" ) ;

         //rgItems[(WORD)wParam].wID = TRUE ;
         //rgItems[(WORD)wParam].fChanged = TRUE ;

         lResult = 0L ;
      break ;

      case TP_FORCERESIZE:
         DP4( hWDB, "TP_FORCERESIZE:   %04X, %08lX", (WORD)wParam, lParam ) ;

         tpReSize( hWnd, pID ) ;

         UpdateWindow( hWnd ) ;

         lResult = 0L ;
      break ;

      case TP_SETITEMSTATE:
         DP4( hWDB, "TP_SETITEMSTATE:  %04X, %08lX", (WORD)wParam, lParam ) ;

         if (((WORD)wParam < pID->wNumItems) &&
             (rgItems = LOCKITEM( pID->lhrgItems )))
         {
            if (!rgItems[(WORD)wParam].fEnabled)
            {
               UNLOCKITEM( pID->lhrgItems ) ;
               lResult = TP_ERR ;
               break ;
            }

            lResult = MAKELRESULT( rgItems[(WORD)wParam].fSelected, 0 ) ;

            if (GETSTYLE( hWnd ) & TPS_AUTOSELECT)
            {
               //
               // If we are "selecting" an item
               //
               if (LOWORD(lParam) == TRUE)
               {
                  //
                  // If this is the currently selected item ignore it.
                  // Otherwise make sure all others are up.
                  //
                  if ((WORD)wParam != pID->wCurSelected)
                  {
                     rgItems[(WORD)wParam].fSelected = TRUE ;
                     rgItems[(WORD)wParam].fChanged = TRUE ;

                     if (pID->wCurSelected != 0xFFFF)
                     {
                        rgItems[pID->wCurSelected].fSelected = FALSE ;
                        rgItems[pID->wCurSelected].fChanged = TRUE ;
                     }

                     tpGetItemRect( hWnd, (WORD)wParam, &rc ) ;
                     InvalidateRect( hWnd, &rc, FALSE ) ;
                     if (pID->wCurSelected != 0xFFFF)
                     {
                        tpGetItemRect( hWnd, pID->wCurSelected, &rc ) ;
                        InvalidateRect( hWnd, &rc, FALSE ) ;
                     }

                     pID->wCurSelected = (WORD)wParam ;
                  }
               }
               else  
               {
                  // we are "unselecting" an item.  if the item is
                  // already not selected, ignore.  Otherwise
                  // unselect it and set wCurSelected to 0xFFFF to indicate
                  // that there is no current item.
                  //
                  // In other words, if we are AutoSelect then only act 
                  // upon the current selected guy.
                  //
                  if ((WORD)wParam == pID->wCurSelected)
                  {
                     rgItems[(WORD)wParam].fSelected = FALSE ;
                     rgItems[(WORD)wParam].fChanged = TRUE ;

                     tpGetItemRect( hWnd, (WORD)wParam, &rc ) ;
                     InvalidateRect( hWnd, &rc, FALSE ) ;

                     pID->wCurSelected = 0xFFFF ;
                  }
               }
            }
            else
            {
               rgItems[(WORD)wParam].fSelected = (LOWORD(lParam) ? TRUE : FALSE ) ;
               rgItems[(WORD)wParam].fChanged = TRUE ;
               tpGetItemRect( hWnd, (WORD)wParam, &rc ) ;
               InvalidateRect( hWnd, &rc, FALSE ) ;
            }

            UNLOCKITEM( pID->lhrgItems ) ;

            pID->fRedraw = TRUE ;

            UpdateWindow( hWnd ) ;
         }
         else
            lResult = TP_ERR ;
      break ;

      case TP_GETITEMSTATE:
         DP4( hWDB, "TP_GETITEMSTATE:  %04X, %08lX", (WORD)wParam, lParam ) ;

         if (((WORD)wParam < pID->wNumItems) &&
             (rgItems = LOCKITEM( pID->lhrgItems )))
         {
            lResult = MAKELRESULT(rgItems[(WORD)wParam].fSelected, 0 ) ;

            UNLOCKITEM( pID->lhrgItems ) ;
         }
         else
            lResult = TP_ERR ;
      break ;

      case TP_TOGGLEITEMSTATE:
         DP4( hWDB, "TP_TOGGLEITEMSTATE:  %04X, %08lX", (WORD)wParam, lParam ) ;

         if (((WORD)wParam < pID->wNumItems) &&
             (rgItems = LOCKITEM( pID->lhrgItems )))
         {
            BOOL  fDown ;

            if (!rgItems[(WORD)wParam].fEnabled)
            {
               UNLOCKITEM( pID->lhrgItems ) ;
               lResult = TP_ERR ;
               break ;
            }

            lResult = MAKELRESULT( rgItems[(WORD)wParam].fSelected, 0 ) ;

            fDown = ((BOOL)LOWORD( (DWORD)lResult) ? TRUE : FALSE ) ;

            //
            // If AutoSelect is on
            //
            if (GETSTYLE( hWnd ) & TPS_AUTOSELECT)
            {
               //
               // if the user clicked on the currently "down" item
               // ignore the bastard.  Use wCurSelected...and force it
               // to be the ONLY one!!!!
               //
               if ((WORD)wParam != pID->wCurSelected)
               {
                  rgItems[(WORD)wParam].fSelected = TRUE ;
                  rgItems[(WORD)wParam].fChanged = TRUE ;

                  if (pID->wCurSelected != 0xFFFF)
                  {
                    rgItems[pID->wCurSelected].fSelected = FALSE ;
                    rgItems[pID->wCurSelected].fChanged = TRUE ;
                  }

                  tpGetItemRect( hWnd, (WORD)wParam, &rc ) ;
                  InvalidateRect( hWnd, &rc, FALSE ) ;
                  if (pID->wCurSelected != 0xFFFF)
                  {
                     tpGetItemRect( hWnd, pID->wCurSelected, &rc ) ;
                     InvalidateRect( hWnd, &rc, FALSE ) ;
                  }

                  pID->wCurSelected = (WORD)wParam ;
               }
            }
            else
            {
               tpGetItemRect( hWnd, (WORD)wParam, &rc ) ;
               InvalidateRect( hWnd, &rc, FALSE ) ;
               rgItems[(WORD)wParam].fSelected = !fDown ;
               rgItems[(WORD)wParam].fChanged = TRUE ;
            }

            pID->fRedraw = TRUE ;

            UpdateWindow( hWnd ) ;

            UNLOCKITEM( pID->lhrgItems ) ;
         }
         else
            lResult = TP_ERR ;
      break ;

      case TP_GETSELECTEDITEM:
         DP4( hWDB, "TP_GETSELECTEDITEM: %04X, %08lX", (WORD)wParam, lParam ) ;

         lResult = MAKELRESULT( pID->wCurSelected, 0 ) ;
      break ;

      case TP_ENABLEITEM:
         DP4( hWDB, "TP_ENABLEITEM:    %04X, %08lX", (WORD)wParam, lParam ) ;

         if (((WORD)wParam < pID->wNumItems) &&
             (rgItems = LOCKITEM( pID->lhrgItems )))
         {
            lResult = MAKELRESULT( rgItems[(WORD)wParam].fEnabled, 0 ) ;

            rgItems[(WORD)wParam].fEnabled = (LOWORD(lParam) ? TRUE : FALSE ) ;
            rgItems[(WORD)wParam].fChanged = TRUE ;

            //
            // We can't allow a disabled item to be selected.
            //
            if (LOWORD(lParam) == FALSE &&
               (GETSTYLE( hWnd ) & TPS_AUTOSELECT) &&
               (rgItems[(WORD)wParam].fSelected == TRUE))
            {
               rgItems[(WORD)wParam].fSelected = FALSE ;
               pID->wCurSelected = 0xFFFF ;
            }
               
            UNLOCKITEM( pID->lhrgItems ) ;

            pID->fRedraw = TRUE ;

            tpGetItemRect( hWnd, (WORD)wParam, &rc ) ;
            InvalidateRect( hWnd, NULL, /*&rc,*/ FALSE ) ;
            UpdateWindow( hWnd ) ;
         }
         else
            lResult = TP_ERR ;
      break ;

      case TP_SETITEMSIZE:
         DP4( hWDB, "TP_SETITEMSIZE:    %04X, %08lX", (WORD)wParam, lParam ) ;

         lResult = MAKELRESULT( pID->nItemSizeX, pID->nItemSizeY ) ;

         if (LOWORD( lParam ))
            pID->nItemSizeX = LOWORD( lParam ) ;

         if (HIWORD( lParam ))
            pID->nItemSizeY = HIWORD( lParam ) ;

         tpReSize( hWnd, pID ) ;

         pID->fRedrawAll = TRUE ;

         InvalidateRect( hWnd, NULL, FALSE ) ;
         UpdateWindow( hWnd ) ;
      break ;

      case TP_SETBORDERWIDTH:
         DP4( hWDB, "TP_SETBORDERWIDTH:   %04X, %08lX", (WORD)wParam, lParam ) ;

         lResult = MAKELRESULT( pID->nBorderWidth, 0 ) ;

         pID->nBorderWidth = (WORD)wParam ;

         tpReSize( hWnd, pID ) ;

         pID->fRedrawAll = TRUE ;

         InvalidateRect( hWnd, NULL, FALSE ) ;
         UpdateWindow( hWnd ) ;
      break ;

      case TP_SETNOTIFYMSG:
         DP5( hWDB, "TP_SETNOTIFYMSG:   %04X", (WORD)wParam ) ;

         lResult = MAKELRESULT( pID->wNotifyMsg, 0 ) ;

         pID->wNotifyMsg = (WORD)wParam ;
      break ;

      default:
      //   DP1( hWDB, "tpControl: Unknown message = 0x%04X", wMsg ) ;
         lResult = 0L ;
      break ;
   }

   UNLOCKINSTDATA( lh ) ;
   
   return lResult ;

} /* tpControl()  */

/****************************************************************
 *  BOOL NEAR PASCAL tpPaint( HWND hWnd )
 *
 *  Description: 
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL tpPaint( HWND hWnd )
{
   LOCALHANDLE    lh ;
   PINSTDATA      pID ;
   PAINTSTRUCT    ps ;

   PITEM          rgItems ;

   RECT           rcClient ;

   RECT           rcItem ;

   WORD           i ;
   BOOL           fColumns = TRUE ;
   WORD           wNumCols ;
   WORD           wNumItems ;
   short          nBmpSizeX, nBmpSizeY ;  // bitmap size
   short          nItemSizeX, nItemSizeY ;  // bitmap size
   short          x, y ;                  // current x,y in "items"
   short          nX, nY ;
   short          xSpace, ySpace ;
   short          dx, dy ;

   short          xT, yT ;                // Temp utiltity

   HDC            hdcMem ;
   HBITMAP        hbmCurOld ;
   HDC            hdcTemp ;
   HBITMAP        hbmTemp ;

   WORD           wStyle ;
   BOOL           fStretchBlt ;
   HBRUSH         hbrParentBG = NULL ;
   HWND           hwndParent ;

   DP5( hWDB, "tpPaint" ) ;

   if (!IsWindowVisible( hWnd ))
      return FALSE ;

   wStyle = GETSTYLE( hWnd ) ;

   if (pID = LOCKINSTDATA( lh = GETLHINSTDATA( hWnd ) ))
   {
      #if 0
      if (pID->fRedrawOn == FALSE)
      {
         UNLOCKINSTDATA( lh ) ;
         return FALSE ;
      }
      #endif

      if (BeginPaint( hWnd, &ps ))
      {
         GetClientRect( hWnd, &rcClient ) ;

         //
         // We draw everything to memory first...
         //
#ifdef DRAW_TO_MEMORY_DC
         if (hdcMem = CreateCompatibleDC( ps.hdc ))
         {
            /*
            * Use WM_CTLCOLOR to find out what the parent has for
            * a background.
            */
            if (ISCHILDWND( hWnd ) &&
                (hwndParent = pID->hwndParent))
            {
               hbrParentBG = (HBRUSH)(DWORD)SendMessage( hwndParent,
                                 WM_CTLCOLOR, (WPARAM)hdcMem,
                                 MAKELPARAM( hWnd, CTLCOLOR_BTN ) ) ;
            }

            //
            // hbmCur will be destroyed during WM_DESTROY or 
            // during a tpSize.
            //
            if (!pID->hbmCur)
            {
               DPF4( hWDB, "Snap shot does not exist... creating bitmap: " ) ;
               pID->hbmCur = CreateCompatibleBitmap( ps.hdc,
                                                     rcClient.right,
                                                     rcClient.bottom ) ;
               DP4( hWDB, "hbm = %04X", pID->hbmCur ) ;
               DASSERTMSG( hWDB, pID->hbmCur, "Could not create snapshot bitmap" ) ;

               hbmCurOld = SelectObject( hdcMem, pID->hbmCur ) ;
               /*
               * Erase the background for the entire control.
               */
               if (hbrParentBG)
               {
                  hbrParentBG = SelectObject( hdcMem, hbrParentBG ) ;
                  PatBlt( hdcMem, rcClient.left, rcClient.top,
                           rcClient.right - rcClient.left,
                           rcClient.bottom - rcClient.top, PATCOPY ) ;
                  hbrParentBG = SelectObject( hdcMem, hbrParentBG ) ;
               }
               else
               {
                  SetBkColor( hdcMem, GetSysColor( COLOR_BTNFACE ) ) ;
                  DRAWFASTRECT( hdcMem, &rcClient ) ;
               }
            }
            else
            {
               DP4( hWDB, "Snap shot exists:  hbm = %04X", pID->hbmCur ) ;
               hbmCurOld = SelectObject( hdcMem, pID->hbmCur ) ;
            }

         }
         else
#endif
         {
            DP4( hWDB, "could not create memory DC, using real DC" ) ;
            hdcMem = ps.hdc ; //is this kosher?

            /*
             * Use WM_CTLCOLOR to find out what the parent has for
             * a background.
             */
            if (ISCHILDWND( hWnd ) &&
                (hwndParent = GetParent( hWnd )))
               hbrParentBG = (HBRUSH)(DWORD)SendMessage( hwndParent,
                                 WM_CTLCOLOR, (WPARAM)hdcMem,
                                 MAKELPARAM( hWnd, CTLCOLOR_BTN ) ) ;

            /*
             * Erase the background for the entire control.
             */
            if (hbrParentBG)
            {
               hbrParentBG = SelectObject( hdcMem, hbrParentBG ) ;
               PatBlt( hdcMem, rcClient.left, rcClient.top,
                        rcClient.right - rcClient.left,
                        rcClient.bottom - rcClient.top, PATCOPY ) ;
               hbrParentBG = SelectObject( hdcMem, hbrParentBG ) ;
            }
            else
            {
               SetBkColor( hdcMem, GetSysColor( COLOR_BTNFACE ) ) ;
               DRAWFASTRECT( hdcMem, &rcClient ) ;
            }

            pID->fRedrawAll = TRUE ;
         }

         //
         // See if we require any redrawing.  If not, then simply
         // bitblt the "snapshot" of the palette.
         //
         if (pID->fRedraw ||           // if any items need re-drawing
             pID->fRedrawAll)          // if the whole thing needs it.
         {
            //
            // Speed things up by only referencing the inst data once 
            //
            wNumCols = pID->wNumCols ;
            wNumItems = pID->wNumItems ;

            DASSERTMSG( hWDB, wNumCols,
                        "You are about to divide by zero in tpPaint!" ) ;

            //
            // Are we column or row oriented? 
            //
            if (wStyle & TPS_ROWORIENT) 
            {
               //
               // Row oriented
               //
               fColumns = FALSE ;
               nX = (wNumItems - 1) / wNumCols ;   // max index across
               nY = wNumCols - 1 ;                 // max index down
            }
            else
            {
               fColumns = TRUE ;
               nX = wNumCols - 1 ;                 // max index across
               nY = (wNumItems - 1) / wNumCols ;   // max index down
            }

            nBmpSizeX = pID->nBmpSizeX ;
            nBmpSizeY = pID->nBmpSizeY ;

            nItemSizeX = pID->nItemSizeX ;
            nItemSizeY = pID->nItemSizeY ;

            dx = nItemSizeX  ;
            dy = nItemSizeY  ;

            /*
             * We have several ways of blitting the bitmaps.
             *
             * If the window has the TPS_STRETCHBLT style then
             * we will always stretchblt the bitmap to fill the item.
             *
             * If the TPS_STRETCHBLT bit is not set, then we will only
             * strecth the bitmap if it is too big to fit.
             */
            if (((nItemSizeX - DEFAULT_ITEMEXTRAX) < nBmpSizeX) ||
                ((nItemSizeY - DEFAULT_ITEMEXTRAY) < nBmpSizeY) ||
                wStyle & TPS_STRETCHBLT)
            {
               fStretchBlt = TRUE ;
               SetStretchBltMode( hdcMem, WHITEONBLACK ) ;
            }
            else
            {
               fStretchBlt = FALSE ;
            }

            //
            // Lock down the item data
            //
            if ((pID->lhrgItems == 0) ||
                !(rgItems = LOCKITEM( pID->lhrgItems )))
            {
               // 
               // This warning used to be DP1, but I removed it
               // because if there is a border we will get a paint
               // before any items are added.  Thus it is not bad
               // to get to this point.
               // DP5( hWDB, "Could not lock items!!!! (tpPaint)" ) ;

               if (hdcMem != ps.hdc)
               {
                  pID->hbmCur = SelectObject( hdcMem, hbmCurOld ) ;
                  DeleteDC( hdcMem ) ;
               }
               UNLOCKINSTDATA( lh ) ;
               EndPaint( hWnd, &ps ) ;
               return FALSE ;
            }

            for (i = 0 ; i < pID->wNumItems ; i++)
            {
               if (rgItems[i].fChanged || pID->fRedrawAll) 
               {
                  DP4( hWDB, "Item # %d needs painting", i ) ;
                  //
                  // The code that calculates rcItem is duplicated in
                  // the function tpGetItemRect.  Code must be updated
                  // in both places.
                  //
                  if (fColumns)
                  {
                     x = i % wNumCols ;
                     y = i / wNumCols ;
                  }
                  else
                  {
                     x = i / wNumCols ;
                     y = i % wNumCols ;
                  }

                  xSpace = -x + pID->nBorderWidth - 1 ;
                  ySpace = -y + pID->nBorderWidth - 1 ;

                  rcItem.left   = rcClient.left + (dx * x) + xSpace ;
                  rcItem.top    = rcClient.top  + (dy * y) + ySpace ;
                  rcItem.right  = rcItem.left   + dx ;
                  rcItem.bottom = rcItem.top    + dy ;

                  #if 0
                  SelectClipRgn( hdcMem, NULL ) ;
                  IntersectClipRect( hdcMem, rcItem.left, rcItem.top,
                                             rcItem.right, rcItem.bottom ) ;
                  #endif

                  if (rgItems[i].fSelected && rgItems[i].fEnabled)
                  {

                     /*
                      * Erase this item. First draw in area where the
                      * border goes.  This way if ROUNDLOOK is set the
                      * parent's color will come through.
                      */
                     if (hbrParentBG)
                     {
                        hbrParentBG = SelectObject( hdcMem, hbrParentBG ) ;
                        PatBlt( hdcMem, rcItem.left, rcItem.top,
                              rcItem.right - rcItem.left,
                              rcItem.bottom - rcItem.top, PATCOPY ) ;
                        hbrParentBG = SelectObject( hdcMem, hbrParentBG ) ;
                     }
                     else
                     {
                        SetBkColor( hdcMem, GetSysColor( COLOR_BTNFACE ) ) ;
                        DRAWFASTRECT( hdcMem, &rcItem ) ;
                     }

                     /*
                      * Now erase what's inside
                      */
                     InflateRect( &rcItem, -1, -1 ) ;

                     if (GETSTYLE( hWnd) & TPS_LIGHTBKGND)
                        SetBkColor( hdcMem, GetSysColor( COLOR_WINDOW ) ) ;
                     else
                        SetBkColor( hdcMem, GetSysColor( COLOR_BTNFACE ) ) ;

                     DRAWFASTRECT( hdcMem, &rcItem ) ;
                     InflateRect( &rcItem, 1, 1 ) ;

                     // 
                     // Draw face.
                     //
                     hdcTemp = CreateCompatibleDC( hdcMem ) ;
                     hbmTemp = SelectObject( hdcTemp, pID->hbmTable ) ;

                     if (fStretchBlt)
                     {
                        //
                        // Blt the bitmap. Stretch.
                        //
                        xT = rcItem.left + 2*BTN_BEVELWIDTH ;
                        yT = rcItem.top + 2*BTN_BEVELWIDTH ;
               
                        StretchBlt( hdcMem, xT, yT,
                              nItemSizeX - DEFAULT_ITEMEXTRAX,
                              nItemSizeY - DEFAULT_ITEMEXTRAY,
                              hdcTemp,
                              nBmpSizeX * i,
                              nBmpSizeY * BMP_DN,
                              nBmpSizeX,
                              nBmpSizeY,
                              SRCCOPY ) ;
                     }
                     else
                     {
                        //
                        // Blt the bitmap.  Center it on the face.
                        //
                        xT = rcItem.left + BTN_BEVELWIDTH +
                           ((nItemSizeX - nBmpSizeX) / 2) ;
                        yT = rcItem.top + BTN_BEVELWIDTH +
                           ((nItemSizeY- nBmpSizeY) / 2) ;

                        BitBlt( hdcMem, xT, yT,
                              nBmpSizeX,
                              nBmpSizeY,
                              hdcTemp,
                              nBmpSizeX * i,
                              nBmpSizeY * BMP_DN,
                              SRCCOPY ) ;

                     }

                     SelectObject( hdcTemp, hbmTemp ) ;
                     DeleteDC( hdcTemp ) ;

                     //
                     // Draw bevel for a depressed button
                     //
                     tpDrawBevel( hWnd, hdcMem, &rcItem, FALSE, wStyle & (WORD)TPS_BORDERS ) ;

                  }
                  else
                  {
                     /*
                      * Erase this item. First draw in area where the
                      * border goes.  This way if ROUNDLOOK is set the
                      * parent's color will come through.
                      */
                     if (hbrParentBG)
                     {
                        hbrParentBG = SelectObject( hdcMem, hbrParentBG ) ;
                        PatBlt( hdcMem, rcItem.left, rcItem.top,
                              rcItem.right - rcItem.left,
                              rcItem.bottom - rcItem.top, PATCOPY ) ;
                        hbrParentBG = SelectObject( hdcMem, hbrParentBG ) ;
                     }
                     else
                     {
                        SetBkColor( hdcMem, GetSysColor( COLOR_BTNFACE ) ) ;
                        DRAWFASTRECT( hdcMem, &rcItem ) ;
                     }


                     //
                     // Erase what's there already
                     //
                     InflateRect( &rcItem, -1, -1 ) ;
                     SetBkColor( hdcMem, GetSysColor( COLOR_BTNFACE ) ) ;
                     DRAWFASTRECT( hdcMem, &rcItem ) ;
                     InflateRect( &rcItem, 1, 1 ) ;

                     // 
                     // Draw face.
                     //
                     hdcTemp = CreateCompatibleDC( hdcMem ) ;
                     hbmTemp = SelectObject( hdcTemp, pID->hbmTable ) ;

                     if (fStretchBlt)
                     {
                        //
                        // Blt the bitmap. Stretch.
                        //
                        xT = rcItem.left + BTN_BEVELWIDTH ; ;
                        yT = rcItem.top + BTN_BEVELWIDTH ;

                        #if 0
                        xT = rcItem.left + 
                           ((nItemSizeX - ()) / 2) ;
                        yT = rcItem.top + 
                           ((nItemSizeY - ()) / 2) ;
                        #endif

                        StretchBlt( hdcMem, xT, yT,
                              nItemSizeX - DEFAULT_ITEMEXTRAX,
                              nItemSizeY - DEFAULT_ITEMEXTRAY,
                              hdcTemp,
                              nBmpSizeX * i,
                              nBmpSizeY *
                                 (rgItems[i].fEnabled ? BMP_UP : BMP_DISABLED),
                              nBmpSizeX,
                              nBmpSizeY,
                              SRCCOPY ) ;
                     }
                     else
                     {
                        //
                        // Blt the bitmap.  Center it on the face.
                        //
                        xT = rcItem.left + 
                           ((nItemSizeX - nBmpSizeX) / 2) ;
                        yT = rcItem.top + 
                           ((nItemSizeY - nBmpSizeY) / 2) ;

                        BitBlt( hdcMem, xT, yT,
                              nBmpSizeX,
                              nBmpSizeY,
                              hdcTemp,
                              nBmpSizeX * i,
                              nBmpSizeY *
                              (rgItems[i].fEnabled ? BMP_UP : BMP_DISABLED),
                              SRCCOPY ) ;
                     }
                     SelectObject( hdcTemp, hbmTemp ) ;
                     DeleteDC( hdcTemp ) ;

                     //
                     // Draw bevel for an "up" button 
                     //
                     tpDrawBevel( hWnd, hdcMem, &rcItem, TRUE, wStyle & (WORD)TPS_BORDERS ) ;

                  }

               }
               rgItems[i].fChanged = FALSE ;
            }

            UNLOCKITEM( pID->lhrgItems ) ;

            pID->fRedraw = FALSE ;
            pID->fRedrawAll = FALSE ;
            
         }

         //
         // BitBlt the snapshot
         //
         if (hdcMem != ps.hdc)
         {
            //
            // is this rcPaint shit necessary if we've set a clipping region?
            //
            BitBlt( ps.hdc, 0, 0, rcClient.right, rcClient.bottom,
                    hdcMem, 0, 0,
                    SRCCOPY ) ;

            //
            // Now Select the old bitmap in, getting our "snapshot" back
            //
            pID->hbmCur = SelectObject( hdcMem, hbmCurOld ) ;
            DP5( hWDB, "pID->hbmCur (snapshot) = %04X", pID->hbmCur ) ;
            DeleteDC( hdcMem ) ;
         }   

         EndPaint( hWnd, &ps ) ;
      }

      UNLOCKINSTDATA( lh ) ;
   }
   DASSERT( hWDB, pID ) ;

   return TRUE ;

} /* tpPaint()  */

VOID NEAR PASCAL tpDrawBevel( HWND hWnd, HDC hDC, LPRECT lprc, BOOL fUp, BOOL fBorders )
{
   RECT     rc2 = *lprc ;
   RECT     rc ;
   COLORREF rgbOld ;

   if (fBorders)
   {
      rgbOld = SetBkColor( hDC, GetSysColor( COLOR_WINDOWFRAME ) ) ;

      if (GETSTYLE( hWnd ) & TPS_ROUNDLOOK)
      {
         rc = rc2 ;
         rc.top = rc.bottom - BTN_FRAMEWIDTH ;
         rc.left++ ;
         rc.right-- ;
         DRAWFASTRECT( hDC, &rc ) ;
      
         rc.top = lprc->top ;
         rc.bottom = rc.top + BTN_FRAMEWIDTH ;
         DRAWFASTRECT( hDC, &rc ) ;

         rc = rc2 ;
         rc.left = rc.right - BTN_FRAMEWIDTH ;
         rc.top++ ;
         rc.bottom-- ;
         DRAWFASTRECT( hDC, &rc ) ;

         rc.left = lprc->left ;
         rc.right = rc.left +  BTN_FRAMEWIDTH ;
         DRAWFASTRECT( hDC, &rc ) ;
      }
      else
      {
         rc = rc2 ;
         rc.top = rc.bottom - BTN_FRAMEWIDTH ;
         DRAWFASTRECT( hDC, &rc ) ;
      
         rc.top = lprc->top ;
         rc.bottom = rc.top + BTN_FRAMEWIDTH ;
         DRAWFASTRECT( hDC, &rc ) ;

         rc = rc2 ;
         rc.left = rc.right - BTN_FRAMEWIDTH ;
         DRAWFASTRECT( hDC, &rc ) ;

         rc.left = lprc->left ;
         rc.right = rc.left +  BTN_FRAMEWIDTH ;
         DRAWFASTRECT( hDC, &rc ) ;
      }


      SetBkColor( hDC, rgbOld ) ;
   }

   InflateRect( &rc2, -1, -1 ) ;
   rc = rc2 ;
   rgbOld = SetBkColor( hDC, GetSysColor( COLOR_BTNSHADOW ) ) ;
   if (fUp)
   {
      rc.top = rc.bottom - BTN_BEVELWIDTH ;
      DRAWFASTRECT( hDC, &rc ) ;

      rc = rc2 ;
      rc.left = rc.right - BTN_BEVELWIDTH ;
      DRAWFASTRECT( hDC, &rc ) ;

      if (fWin31)      
         SetBkColor( hDC, GetSysColor( COLOR_BTNHIGHLIGHT ) ) ;
      else
         SetBkColor( hDC, RGBWHITE ) ;
      rc = rc2 ;
      rc.right-- ;
      rc.bottom = rc.top + 1 ;
      DRAWFASTRECT( hDC, &rc ) ;

      rc = rc2 ;
      rc.bottom-- ;
      rc.right = rc.left + 1 ;
      DRAWFASTRECT( hDC, &rc ) ;
   }
   else
   {
      rc.bottom = rc.top + BTN_BEVELWIDTH ;
      DRAWFASTRECT( hDC, &rc ) ;

      rc = rc2 ;
      rc.right = rc.left + BTN_BEVELWIDTH ;
      DRAWFASTRECT( hDC, &rc ) ;
   }
   SetBkColor( hDC, rgbOld ) ;
}


/****************************************************************
 *  HBITMAP NEAR PASCAL
 *    tpGangInsertBitmap( HWND hWnd, WORD wIndex,
 *                        LPINSTDATA lpID, LPTOOLPALITEM lptpItem )
 *
 *  Description: 
 *
 *    Does a gang add of many items.  Actually does not insert,
 *    will only add to the end.  Currently the wIndex paramter
 *    is ignored.
 *
 *  Comments:
 *
 ****************************************************************/
HBITMAP NEAR PASCAL
   tpGangInsertBitmap( HWND hWnd, WORD wIndex,
                       LPINSTDATA lpID, LPTOOLPALITEM lptpItem )
{
   HDC      hdc         = NULL ;

   HDC      hdcDest     = NULL ;
   HBITMAP  hbmDest     = NULL ;
   HBITMAP  hbmDestOld  = NULL ;

   HDC      hdcTemp     = NULL ;
   HBITMAP  hbmTemp     = NULL ;
   HBITMAP  hbmTempOld  = NULL ;

   DPF5( hWDB, "tpGangInsertBitmap..." ) ;

   /*
    * Create the final destination DC and a temp DC.
    */
   if ((hdc = GetDC( hWnd )) &&
       (hdcDest = CreateCompatibleDC( hdc )) &&
       (hdcTemp = CreateCompatibleDC( hdc )))
   {
      WORD     wNumItems ;
      WORD     wNumNewItems ;

      WORD     w ;
      WORD     i ;

      BOOL     fSynthDisabled = FALSE ;

      COLORREF    rgbBtnFace     = GetSysColor( COLOR_BTNFACE ) ;
      COLORREF    rgbWindow      = GetSysColor( COLOR_WINDOW ) ;

      HBITMAP  hbmUp       = NULL ;
      HBITMAP  hbmDn       = NULL ;
      HBITMAP  hbmDis      = NULL ;
      HBITMAP  hbmTemp     = NULL ;

      short    dx, dy, x ;
      BITMAP   BM ;

      /*
       * release that dc
       */
      ReleaseDC( hWnd, hdc ) ;
      hdc = NULL ;

      wNumItems    = lpID->wNumItems ;
      wNumNewItems = max( 1, lptpItem[0].cGangLoadItems ) ;

      /*
       * For each item specified in the gang load, get it's bitmaps
       * and add them to the end.
       */
      DP5( hWDB, "Getting bitmaps (%d new items)", wNumNewItems ) ;
      for( i = 0 ; i < wNumNewItems ; i++ )
      {
         DP5( hWDB, " i = %d", i ) ;
         /*
          * Load the up bitmap.  If it fails break out of loop.
          */
         hbmUp  = bmLoadBitmap( lptpItem[i].hModule,
                                 lptpItem[i].lpszUpBitmap ) ;

         DASSERTMSG( hWDB, hbmUp, "The up bitmap could not be loaded!" ) ;

         if (!hbmUp)
            continue ;

         /*
          * On the first item setup the destination dc
          *
          * After we do this our hbmDest is selected into hdcDest...ready to
          * party.
          */
         if (i == 0)
         {
            if (hbmDest = SetupInsert( hdcDest, lpID, hbmUp,
                                 wIndex, wNumItems, wNumNewItems ))
            {
               hbmDestOld = SelectObject( hdcDest, hbmDest ) ;
      
               dx = lpID->nBmpSizeX ;
               dy = lpID->nBmpSizeY ;
               x = dx * wIndex ;
            }
            else
            {
               /*
                * this is bad.  We have no destination bitmap so
                * we might as well jam out of here.  There is really
                * no way to recover cleanly, so let's just clean up
                * any objects we have...and return.
                *
                */
               DeleteObject( hbmUp ) ;
               DeleteDC( hdcDest ) ;
               DeleteDC( hdcTemp ) ;

               DP1( hWDB, "Could not setup destination DC for insert, failing!" ) ;
               return FALSE ;
            }
         }

         /*
          * Load the down bitmap.  If it fails synthisize.
          */
         if (!(hbmDn  = bmLoadBitmap( lptpItem[i].hModule,
                                      lptpItem[i].lpszDnBitmap )))
         {
            /*
             * Copy the up bitmap since there was no down
             * bitmap found.
             */
            hbmDn = bmCopy( hbmUp ) ;
         }

         if (!(hbmDis = bmLoadBitmap( lptpItem[i].hModule,
                                      lptpItem[i].lpszDisBitmap )))
         {
           /*
            * Copy the up bitmap since there was no disabled
            * bitmap found.
            */
            hbmDis = bmCopy( hbmUp ) ;
            fSynthDisabled = TRUE ;
         }
   
         DASSERTMSG( hWDB, hbmUp, "BAD! hbmUp == NULL" ) ;
         DASSERTMSG( hWDB, hbmDn, "BAD! hbmDn == NULL" ) ;
         DASSERTMSG( hWDB, hbmDis, "BAD! hbmDis == NULL" ) ;

         /*
          * Now. For each item (each time through the loop) blt the
          * up, down, and disabled bitmaps into the dest.
          *
          *
          * Blt the specified bitmaps into the new one
          *
          *       01234567         01234567
          *      0XXXX XXX        0XXXXxXXX
          *      1YYYY YYY  -->   1YYYYyYYY
          *      2ZZZZ ZZZ        2ZZZZzZZZ
          *
          */

         /*
          ***** Draw the up bitmap on to dest.
          */
         if (hbmTemp = SelectObject( hdcTemp, hbmUp ))
         {
            GetObject( hbmUp, sizeof( BITMAP ), (LPSTR)&BM ) ;

            /*
             *  Translate the background color
             */
            bmColorTranslateDC( hdcTemp, (LPBITMAP)&BM,
                                 GetPixel( hdcTemp, 0, 0 ), rgbBtnFace,
                                 BM_NORMAL ) ;

            w = BitBlt( hdcDest, x+(i*dx), 0, dx, dy,
                        hdcTemp, 0, 0, SRCCOPY ) ;
            DASSERT( hWDB, w ) ;
            hbmUp = SelectObject( hdcTemp, hbmTemp ) ;
         }
         DASSERT( hWDB, hbmUp ) ;
         DeleteObject( hbmUp );
         hbmUp = NULL ;

         /*
          ***** Draw the down bitmap on to dest.
          */
         if (hbmTemp = SelectObject( hdcTemp, hbmDn ))
         {
            GetObject( hbmDn, sizeof( BITMAP ), (LPSTR)&BM ) ;
            bmColorTranslateDC( hdcTemp, (LPBITMAP)&BM,
                                 GetPixel( hdcTemp, 0, 0 ),
                                 (GETSTYLE( hWnd) & TPS_LIGHTBKGND) ?
                                 rgbWindow : rgbBtnFace,
                                 BM_NORMAL ) ;

            w = BitBlt( hdcDest, x+(i*dx), BMP_DN * dy, dx, dy,
                        hdcTemp, 0, 0, SRCCOPY ) ;
            DASSERT( hWDB, w ) ;
            hbmDn = SelectObject( hdcTemp, hbmTemp ) ;
         }
         DASSERT( hWDB, hbmDn ) ;
         DeleteObject( hbmDn );
         hbmDn = NULL ;

         /*
          ***** Draw the disabled bitmap on to dest.
          */
         if (hbmTemp = SelectObject( hdcTemp, hbmDis ))
         {
            GetObject( hbmDis, sizeof( BITMAP ), (LPSTR)&BM ) ;

            bmColorTranslateDC( hdcTemp, (LPBITMAP)&BM,
                              GetPixel( hdcTemp, 0, 0 ),
                              rgbBtnFace,
                              fSynthDisabled ? BM_DISABLED : BM_NORMAL ) ;

            w = BitBlt( hdcDest, x+(i*dx), BMP_DISABLED * dy, dx, dy,
                        hdcTemp, 0, 0, SRCCOPY ) ;

            DASSERT( hWDB, w ) ;
            hbmDis = SelectObject( hdcTemp, hbmTemp ) ;
         }
         DASSERT( hWDB, hbmDis ) ;
         DeleteObject( hbmDis );
         hbmDis = NULL ;

      } /* for loop */

   
      DASSERTMSG( hWDB, (!hbmUp  || !hbmDn  || !hbmDis),
                  "About to delete a DC with a bitmap selected in" ) ;

      DeleteDC( hdcTemp ) ;
      hdcTemp = NULL ;

      /*
       * Get the destination bitmap out of the DC
       */
      if (hbmDestOld)
      {
         hbmDest = SelectObject( hdcDest, hbmDestOld ) ;
         DASSERT( hWDB, hbmDest ) ;

         //
         // The destination bitmap now has our neato stuff in it
         //
         lpID->hbmTable = hbmDest ;
      }

      DeleteDC( hdcDest ) ;
      hdcDest = NULL ;

   }
   else
   {
      DP1( hWDB, "Could not get DCs for insert, failing!" ) ;
      return NULL ;
   }

   return hbmDest ;

} /* tpGangInsertBitmap()  */

/****************************************************************
 *  HBITMAP NEAR PASCAL
 *    SetupInsert( HDC        hdcDest,    Destination DC
 *                 LPINSTDATA lpID,       Instance data
 *                 HBITMAP    hbmUp,      up bitmap, used for sizing
 *                 WORD       wIndex,     where to put them
 *                 WORD       wNumItems,  Current # items
 *                 WORD       wNumNewItems ) how many?
 *
 *  Description: 
 *
 *    This function sets up the destination dc for an insert.  It
 *    returns a handle to the dest bitmap.
 * 
 *    The bitmap returned has been manipulated such that it is
 *    ready to have then new bitmaps inserted (or added) to it.
 *
 *  Comments:
 *
 ****************************************************************/
HBITMAP NEAR PASCAL
   SetupInsert( HDC hdcDest, LPINSTDATA lpID, HBITMAP hbmUp,
                WORD wIndex, WORD wNumItems, WORD wNumNewItems )
{
   COLORREF rgbBtnFace     = GetSysColor( COLOR_BTNFACE ) ;

   HDC      hdc = NULL ;

   HBITMAP  hbmDest     = NULL ;
   HBITMAP  hbmDestOld  = NULL ;

   short    x ;         // x position in the bitmap in pixels
   short    dx, dy ;    // bitmap size (each small one)
   short    dxS, dyS ;  // source bitmap size
   short    dxD, dyD ;  // final dest bitmap size
   RECT     rc ;

   /*
    * Calculate the offsets into the bitmap
    *
    * If pID->nBmpSizeX is 0 then the bitmap size has never been 
    * calculated and this is the first bitmap.  If so use GetObject()
    * to get the bitmap and it's size...
    *
    * It would be nice to have this outside of the loop, but since
    * the calculations require the size of a bitmap, and the bitmaps
    * are not loaded until we are in the loop...
    */
   if (lpID->nBmpSizeX == 0)
   {
      BITMAP   BM ;

      GetObject( hbmUp, sizeof( BITMAP ), (LPSTR)&BM ) ;

      dx = lpID->nBmpSizeX = BM.bmWidth ;
      dy = lpID->nBmpSizeY = BM.bmHeight ;

      lpID->nItemSizeX = dx + DEFAULT_ITEMEXTRAX + 2*BTN_BEVELWIDTH ;
      lpID->nItemSizeY = dy + DEFAULT_ITEMEXTRAY + 2*BTN_BEVELWIDTH ;
   }
   else
   {
      dx = lpID->nBmpSizeX ;
      dy = lpID->nBmpSizeY ;
   }

   dxD = dx * (wNumItems + wNumNewItems ) ;
   dyD = dy * 3 ;    // 3 'cause there is an up,dn,and dis

   dxS = dx * wNumItems ;
   dyS = dy * 3 ;                    // 3 'cause there is an up,dn,and dis

   /*
    * x is the offset into the bitmap where we
    * are going to insert the new images
    */
   x = dx * wIndex ;

   /*
    * Create the new bitmap that we'll return in pID->hbmTable
    */
   if ((hdc = GetDC( NULL )) &&
       (hbmDest = CreateCompatibleBitmap( hdc, dxD, dyD )))
   {
      HDC      hdcSrc      = NULL ;
      HBITMAP  hbmSrc      = NULL ;
      HBITMAP  hbmSrcOld   = NULL ;
      WORD     w ;

      ReleaseDC( NULL, hdc ) ;
      hdc = NULL ;

      /*
       * Select the new bitmap into the dest DC
       */
      hbmDestOld = SelectObject( hdcDest, hbmDest ) ;
      DASSERT( hWDB, hbmDestOld ) ;

      /*
       * If there were never any items in the toolpal before then there
       * is no need to create what we are calling the source bitmap.
       * We'll just use the dest instead.
       */

      /*
       * At this point we either have a source and destination
       * bitmap or just a destintation.  If we only have a dest, then
       * we just blt everything to it.  Otherwise we
       * are probably inserting or adding, and might have to shift
       */
      if ((hbmSrc = lpID->hbmTable) &&
          (hdc = GetDC( NULL )) &&
          (hdcSrc = CreateCompatibleDC( hdc )))
      {
         ReleaseDC( NULL, hdc ) ;
         hdc = NULL ;

         //
         // Select the old bitmap into the source DC
         //
         hbmSrcOld = SelectObject( hdcSrc, hbmSrc ) ;
         DASSERT( hWDB, hbmSrcOld ) ;

         /*
          *                            
          * Blt the old bitmap onto the new one, leaving a gap for the
          * specified bitmaps:
          *
          *       0123456         01234567      wIndex = 4
          *      0XXXXXXX        0XXXX XXX      wNumitems = 7
          *      1YYYYYYY  -->   1YYYY YYY
          *      2ZZZZZZZ        2ZZZZ ZZZ
          *                            
          */

         /*
          * Move "left" part over
          */
         DP5( hWDB, "BitBlt( hdcDest, %d, %d, %d, %d, hdcSrc, %d, %d, SRCCOPY )",
                     0,
                     0,
                     x + dx,
                     dyS,
                     0,
                     0 ) ;
         w = BitBlt( hdcDest,
                     0,       
                     0,
                     x + dx,
                     dyS,
                     hdcSrc,
                     0,
                     0,
                     SRCCOPY ) ;
         DASSERT( hWDB, w ) ;

         /*
          * Move "right" part over if we are "inserting"  Don't do
          * anything if we are "adding"
          */
         if (wIndex < wNumItems)
         {
            DP5( hWDB, "BitBlt( hdcDest, %d, %d, %d, %d, hdcSrc, %d, %d, SRCCOPY )",
                        x + (dx * wNumNewItems),
                        0,
                        dx * ((wNumItems) - wIndex),
                        dyS,
                        x,
                        0 ) ;
            w = BitBlt( hdcDest,
                        x + (dx * wNumNewItems),
                        0,
                        dx * ((wNumItems) - wIndex),
                        dyS,
                        hdcSrc,
                        x,
                        0,
                        SRCCOPY ) ;
            DASSERT( hWDB, w ) ;
         }

         /*
          * Delete the old bitmap (hbmSrc) because all we need to
          * do now is blt to our destination now that we've made a
          * nice big hole in it from (x) to (x + (dx * wNumNewItems))
          */
         hbmSrc = SelectObject( hdcSrc, hbmSrcOld ) ;
         DASSERT( hWDB, hbmSrc ) ;
         DeleteObject( hbmSrc ) ;
         hbmSrc = NULL ;

         DeleteDC( hdcSrc ) ;
         hdcSrc = NULL ;
      }

      /*
       * Clear out the new area in the dest.
       */
      SetBkColor( hdcDest, rgbBtnFace ) ;
      SetRect( &rc, x, 0,
                  x + (dx * wNumNewItems), dyS ) ;
      ExtTextOut( hdcDest, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;

      hdc = NULL ;

      hbmDest = SelectObject( hdcDest, hbmDestOld ) ;
   }

   return hbmDest ;

} /* SetupInsert()  */

/****************************************************************
 *  HBITMAP NEAR PASCAL 
 *    tpRemoveBitmap( HWND hWnd, WORD wIndex, LPINSTDATA lpID )
 *
 *  Description: 
 *
 *    This function removes a bitmap from the "stored" bitmaps.
 *
 ****************************************************************/
HBITMAP NEAR PASCAL
   tpRemoveBitmap( HWND hWnd, WORD wIndex, LPINSTDATA lpID )
{
   HDC      hdc ;

   HDC      hdcDest ;
   HBITMAP  hbmDest ;
   HBITMAP  hbmDestOld ;

   HDC      hdcSrc ;
   HBITMAP  hbmSrc ;
   HBITMAP  hbmSrcOld ;

   short    x ;         // x position in the bitmap in pixels
   short    dx, dy ;    // bitmap size (each small one)
   short    dxS, dyS ;  // source bitmap size
   short    dxD, dyD ;  // final dest bitmap size

   WORD     wNumItems ;

   WORD     w ;

   // locals to save some segment loading
   wNumItems   = lpID->wNumItems ;
   hbmSrc      = lpID->hbmTable ;

   if ((!wNumItems) || (wIndex >= wNumItems))
   {
      DP1( hWDB, "tpRemoveBitmap called with an invalid wIndex" ) ;
      return NULL ;
   }

   //
   // Calculate the offsets into the bitmap
   //
   // If pID->nBmpSizeX is 0 then the bitmap size has never been 
   // calculated and this is the first bitmap.  If so use GetObject()
   // to get the bitmap and it's size...
   //
   dx = lpID->nBmpSizeX ;
   dy = lpID->nBmpSizeY ;

   dxS = dx * wNumItems ;
   dyS = dy * 3 ;                    // 3 'cause there is an up,dn,and dis

   dxD = dx * (wNumItems - 1) ;
   dyD = dy * 3 ;                    // 3 'cause there is an up,dn,and dis

   x = dx * wIndex ;

   if (!(hdc = GetDC( hWnd )))
   {
      DASSERT( hWDB, hdc ) ;
      return NULL ;
   }

   //
   // Create the final destination DC
   //
   if (hdcDest = CreateCompatibleDC( hdc ))
   {
      //
      // Create the source DC
      //    
      if (hdcSrc = CreateCompatibleDC( hdc ))
      {
         //
         // Create the new bitmap that we'll save
         // This bitmap will be destroyed in the WM_DESTROY case.
         //
         hbmDest = CreateCompatibleBitmap( hdc, dxD, dyD ) ;
         DASSERT( hWDB, hbmDest ) ;

         if (!hbmDest)
            goto error1 ;

         //
         // Create the old bitmap if it does not already exist.
         //
         if (!hbmSrc)
         {
            hbmSrc = CreateCompatibleBitmap( hdc, dxS, dyS ) ;
            DASSERT( hWDB, hbmSrc ) ;

            if (!hbmSrc)
            {
               DeleteObject( hbmDest ) ;
               hbmDest = NULL ;
               goto error1 ;
            }
         }

         ReleaseDC( hWnd, hdc ) ;
         hdc = NULL ;

         //
         // Select the new bitmap into the dest DC
         //
         hbmDestOld = SelectObject( hdcDest, hbmDest ) ;
         DASSERT( hWDB, hbmDestOld ) ;

         //
         // Select the old bitmap into the source DC
         //
         hbmSrcOld = SelectObject( hdcSrc, hbmSrc ) ;
         DASSERT( hWDB, hbmSrcOld ) ;

         //-----------------------------------------------------------
         //                            
         // Blt the old bitmap onto the new one, nuking the column that
         // is being removed:
         //
         //       0123456         012345      wIndex = 4
         //      0XXXXxXX        0XXXXXX      wNumitems = 7
         //      1YYYYyYY  -->   1YYYYYY
         //      2ZZZZzZZ        2ZZZZZZ
         //                            
         //-----------------------------------------------------------
         if (wIndex)
         {
            DP5( hWDB, "BitBlt( hdcDest, %d, %d, %d, %d, hdcSrc, %d, %d, SRCCOPY )",
                        0,
                        0,
                        x,
                        dyS,
                        0,
                        0 ) ;
            w = BitBlt( hdcDest,
                        0,
                        0,
                        x,
                        dyS,
                        hdcSrc,
                        0,
                        0,
                        SRCCOPY ) ;
            DASSERT( hWDB, w ) ;
         }


         DP5( hWDB, "BitBlt( hdcDest, %d, %d, %d, %d, hdcSrc, %d, %d, SRCCOPY )",
                     x,
                     0,
                     dx * ((wNumItems-1) - wIndex),
                     dyS,
                     x+dx,
                     0 ) ;
         w = BitBlt( hdcDest,
                     x,
                     0,
                     dx * ((wNumItems-1) - wIndex),
                     dyS,
                     hdcSrc,
                     x+dx,
                     0,
                     SRCCOPY ) ;
         DASSERT( hWDB, w ) ;

         //
         // Delete the old bitmap
         //
         hbmSrc = SelectObject( hdcSrc, hbmSrcOld ) ;
         DASSERT( hWDB, hbmSrc ) ;
         DeleteObject( hbmSrc ) ;
         hbmSrc = NULL ;

         DeleteDC( hdcSrc ) ;
         hdcSrc = NULL ;

         //
         // Get the destination bitmap out of the DC
         //
         hbmDest = SelectObject( hdcDest, hbmDestOld ) ;
         DASSERT( hWDB, hbmDestOld ) ;
         DeleteDC( hdcDest ) ;
         hdcDest = NULL ;

         //
         // The destination bitmap now has our neato stuff in it
         //
         lpID->hbmTable = hbmDest ;
      }

   }

   return hbmDest ;

error1:
   //
   // Delete the DCs
   //
   if (hdcSrc)
      DeleteDC( hdcSrc ) ;

   if (hdcDest)
      DeleteDC( hdcDest ) ;

   if (hdc)
      ReleaseDC( hWnd, hdc ) ;

   DASSERTMSG( hWDB, 0, "Label 'error1' was reached in tpRemoveBitmap()" ) ;

   return NULL ;

} /* tpRemoveBitmap()  */


/****************************************************************
 *  BOOL NEAR PASCAL tpReSize( HWND hWnd, PINSTDATA pID )
 *
 *  Description:
 *
 *    This function is used to size the window.  It will size
 *    the window so that it is just big enuf for the current
 *    number of items.  If pID is NULL the pID will be taken
 *    from the window words.  In other words you don't have
 *    to pass pID, or you can if you want.  It does not matter.
 *    Do what you want.  I don't care.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL tpReSize( HWND hWnd, PINSTDATA pIDin )
{
   PINSTDATA   pID ;
   LOCALHANDLE lh ;

   short       x, y ;
   short       xSize, ySize ;
   WORD        wNumCols ;
   WORD        wNumItems ;
   RECT        rc ;

   if (!pIDin)
   {
      if (!(pID = LOCKINSTDATA( lh = GETLHINSTDATA( hWnd ) )))
      {
         DP1( hWDB, "tpReSize: Could not lock data" ) ;
         return FALSE ;
      }
   }
   else
      pID = pIDin ;

   //
   // OK! The ToolPalette window is always sized to hold just
   // the current number of items plus the "inside" frames.
   //

   wNumCols = pID->wNumCols ;
   wNumItems = pID->wNumItems ;

   DP5( hWDB, "wNumCols = %d, wNumItems = %d", wNumCols, wNumItems ) ;

   DASSERTMSG( hWDB, wNumCols, "About to divide by zero in tpReSize!" ) ;

   if (wNumItems)
   {
      if (GETSTYLE( hWnd ) & TPS_ROWORIENT)
      {
                  //
         // Row oriented
         //
         x = (wNumItems - 1) / wNumCols + 1 ;
         y = wNumCols ;


         xSize = x * (pID->nItemSizeX - 1) ;

         ySize = y * (pID->nItemSizeY - 1) ;

      }
      else
      {
         //
         // Column oriented (default)
         //
         x = wNumCols ;
         y = (wNumItems - 1) / wNumCols + 1 ;

         xSize = x * (pID->nItemSizeX - 1) ;

         ySize = y * (pID->nItemSizeY - 1) ;
      }

      xSize += pID->nBorderWidth * 2 - 1 ;
      ySize += pID->nBorderWidth * 2 - 1 ;

   }
   else
   {
      if (GETSTYLE( hWnd ) & TPS_ROWORIENT)
      {
         xSize = DEFAULT_ITEMSIZEX ;
         ySize = DEFAULT_ITEMSIZEY * wNumCols ;
      }
      else
      {
         xSize = DEFAULT_ITEMSIZEX * wNumCols ;
         ySize = DEFAULT_ITEMSIZEY ;
      }
   }

   DP5( hWDB, "Sizing: %d, %d", xSize, ySize ) ;

   SetRect( &rc, 0, 0, xSize, ySize ) ;

   /*
    * AdjustWindowRect doesn't know about our custom styles so we
    * fool it in itsybitsy.c
    */
   #ifdef _ITSYBITS_H_
   ibAdjustWindowRect( hWnd, &rc ) ;
   #else
   #endif

   SetWindowPos( hWnd, NULL, 0, 0,
                 rc.right - rc.left, rc.bottom - rc.top,
                 SWP_NOMOVE | SWP_NOZORDER ) ;
   //
   // Now the bitmap that we have stored is no longer valid.  If
   // there *is* a bitmap, we must destroy it.  The tpPaint() 
   // function will create a new one when it needs to.
   //
   if (pID->hbmCur)
   {
      DeleteObject( pID->hbmCur ) ;
      pID->hbmCur = NULL ;
   }

   InvalidateRect( hWnd, NULL, TRUE ) ;
   pID->fRedrawAll = TRUE ;

   if (pID != pIDin)
      UNLOCKINSTDATA( lh ) ;

   return TRUE ;

} /* tpReSize()  */


/****************************************************************
 *  VOID NEAR PASCAL tpGetItemRect( HWND hWnd, WORD wID, lprc )
 *
 *  Description: 
 *
 *    This function fills rc with the the rectangle of the
 *    given item.
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL tpGetItemRect( HWND hWnd, WORD wID, LPRECT lprc )
{
   RECT  rcClient ;

   PINSTDATA   pID ;
   LOCALHANDLE lh ;

   short x, y ;
   short xSpace, ySpace ;
   short dx, dy ;

   if (!(pID = LOCKINSTDATA( lh = GETLHINSTDATA( hWnd ) )))
   {
      DP1( hWDB, "tpGetItemRect: Could not lock data" ) ;
      return ;
   }

   GetClientRect( hWnd, &rcClient ) ;

   dx = pID->nItemSizeX ;
   dy = pID->nItemSizeY ;

   if (!(GETSTYLE( hWnd ) & TPS_ROWORIENT))
   {
      x = wID % pID->wNumCols ;
      y = wID / pID->wNumCols ;
   }
   else
   {
      x = wID / pID->wNumCols ;
      y = wID % pID->wNumCols ;
   }

   xSpace = -x - 1 ;
   ySpace = -y - 1 ;

   lprc->left    = rcClient.left + (dx * x) + xSpace ;
   lprc->top     = rcClient.top  + (dy * y) + ySpace ;
   lprc->right   = lprc->left    + dx ;
   lprc->bottom  = lprc->top     + dy ;

   OffsetRect( lprc, pID->nBorderWidth, pID->nBorderWidth ) ;
 
   UNLOCKINSTDATA( lh ) ;

} /* tpGetItemRect()  */


/****************************************************************
 *  WORD NEAR PASCAL tpHitTest( HWND hWnd, short x, short y )
 *
 *  Description: 
 *
 *    This function returns the ID of the button that the point
 *    (x,y) falls in.
 *
 *  Comments:
 *
 ****************************************************************/
WORD NEAR PASCAL tpHitTest( HWND hWnd, short x, short y )
{
   RECT  rcClient ;

   PINSTDATA   pID ;
   LOCALHANDLE lh ;

   WORD  wID ;
   short dx, dy ;
   short nRow, nCol ;

   if (!(pID = LOCKINSTDATA( lh = GETLHINSTDATA( hWnd ) )))
   {
      DP1( hWDB, "tpHitTest: Could not lock data" ) ;
      return 0xFFFF ;
   }

   GetClientRect( hWnd, &rcClient ) ;


   //
   // See if we're in the border
   //
   if (!(x >= pID->nBorderWidth &&
         x <= rcClient.right - pID->nBorderWidth &&
         y >= pID->nBorderWidth &&
         y <= rcClient.bottom - pID->nBorderWidth))
   {
      return 0xFFFF ;
   }

   x -= pID->nBorderWidth ;
   y -= pID->nBorderWidth ;

   dx = pID->nItemSizeX - 1;
   dy = pID->nItemSizeY - 1;

   if (!(GETSTYLE( hWnd ) & TPS_ROWORIENT))
   {
      nCol = x / dx ; 
      nRow = y / dy ; 
   }
   else
   {
      nCol = y / dy ;
      nRow = x / dx ;
   }

   wID = (nRow * pID->wNumCols) + nCol ;

   UNLOCKINSTDATA( lh ) ;

   return wID ;

} /* tpHitTest()  */

/************************************************************************
 * End of File: toolpal.c
 ***********************************************************************/

