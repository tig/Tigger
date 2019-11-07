/************************************************************************
 *
 *     Project:  DBEDIT 2.0
 *
 *      Module:  listbox.c
 *
 *     Remarks:  Handles the listbox child window.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "DBEDIT.h"
#include <memory.h>
#include "listbox.h"
#include "cmd.h"
#include "wintime.h"

VOID WINAPI DrawThumbNail( HDC hDC, short x, short y, short dx, short dy, LPSTR lpszSample ) ;

/*
 * local defines
 */

#define CXBORDER     2        // extra space to left and right
#define CCHCOLWIDTH  3        // number of characters between columns.
#define CCHFILESIZE  4        // number of chars in a file size

#define CX_FONTFILE   ((2*CXBORDER))  // offset to text

typedef struct tagCOLUMNSTRUCT
{
   int   nLeft ;       // starting x position of the column
   int   nRight ;      // ending x position of the column
   WORD  wFlags ;      // format flags

} COLUMNSTRUCT, *PCOLUMNSTRUCT, FAR *LPCOLUMNSTRUCT ;

LONG NEAR PASCAL ColumnTextOut( HDC hDC, int nX, int nY,
                                LPSTR lpsz, int cbString,
                                 int cColumns, LPCOLUMNSTRUCT rgColumns ) ;

DWORD CALLBACK fnLBSub( HWND hwnd, WORD wMsg, WPARAM wP, LPARAM lP ) ;

LRESULT NEAR PASCAL lbMeasureItem( HWND hWnd, LPMEASUREITEMSTRUCT lpLBMItem ) ;
LRESULT NEAR PASCAL lbDrawItem( HWND hWnd, LPDRAWITEMSTRUCT lpDIS ) ;

WORD NEAR PASCAL FindItem( HWND hwndLB, char c, WORD w ) ;
BOOL NEAR PASCAL ItemStartsWith( HWND hwndLB, WORD w, char c ) ;

static COLORREF rgbColorWindowText ;
static COLORREF rgbColorWindow ;
static COLORREF rgbColorHighlightText ;
static COLORREF rgbColorHighlight ;

static HBRUSH   hbrListboxSelect ;

static short    cxChar ;         // ave char width
static short    cyChar ;         // char cell height


/*
 * Cool, wayqzomo, memory DCs and bitmaps we keep around for speed
 * and anti-flicker warfare.
 */
static short    cyItem ;         // item height

static COLUMNSTRUCT  rgColumns[5] ; 

static LPSTR    szOutBuf ;  // scratch buffer for output
static LPSTR    szOutBuf2 ;  // scratch buffer for output

static WORD          wCurIndex ;

#define SUBCLASSLB

static WNDPROC  lpfnLB ;         // subclass proc
static WNDPROC  lpfnLBold ;       // subclass proc

static WNDPROC  lpfnEditOld ;       // subclass proc

static HDC      hdcMem ;
static LOGFONT  lf ;

#ifdef USE_EXTTEXTOUTONLY
   #define DRAWFASTRECT(hdc,lprc) ExtTextOut(hdc,0,0,ETO_OPAQUE,lprc,NULL,0,NULL)
#else
   #define DRAWFASTRECT(hdc,lprc)\
   {\
         ExtTextOut(hdc,0,0,ETO_OPAQUE,lprc,NULL,0,NULL) ;\
   }
#endif

/****************************************************************
 *  HWND WINAPI CreateLB( HWND hwndParent )
 *
 *  Description: 
 *
 *    Creates the listbox.
 *
 *  Comments:
 *
 ****************************************************************/
HWND WINAPI CreateLB( HWND hwndParent )
{
   HWND  hwnd ;
   DWORD dwStyle ;
   HDC   hDC ;

   if (!(szOutBuf = ALLOCFROMHEAP( hhScratch, 2048 )))
      return NULL ;

   if (!(szOutBuf2 = ALLOCFROMHEAP( hhScratch, 2048 )))
      return NULL ;

   /*
    * Calculate all the global varaibles.
    */
   ResetLBValues() ;

   dwStyle = (DWORD)
             (WS_CHILD |
             WS_BORDER |
             LBS_NOINTEGRALHEIGHT |
             LBS_OWNERDRAWFIXED |
             LBS_WANTKEYBOARDINPUT | 
             WS_VSCROLL |
             LBS_NOTIFY) ;

   hwnd = CreateWindow
      (
         "listbox",
         "",
         dwStyle,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         hwndParent,                 
         (HMENU)IDCHILD_LISTBOX,
         hinstApp,            
         NULL                  
      ) ;

   if (!hwnd)
      return NULL ;

   if (hDC = GetDC( hwndApp ))
   {
      lf.lfHeight = -MulDiv( 30, GetDeviceCaps( hDC, LOGPIXELSY), 72 )  ;
      lf.lfOutPrecision = OUT_TT_ONLY_PRECIS ;
      lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE ;
      lf.lfCharSet = DEFAULT_CHARSET ;
      lf.lfQuality = DEFAULT_QUALITY ;

      if (!(hdcMem = CreateCompatibleDC( hDC )))
      {
         DP1( hWDB, "CreateCompatibleDC() failed!" ) ;
         ReleaseDC( hwndApp, hDC ) ;
         return NULL ;
      }
      ReleaseDC( hwndApp, hDC ) ;
   }

   SendMessage( hwnd, WM_SETFONT, (WPARAM)hfontCur, (LPARAM)0L ) ;

   if ((FARPROC)lpfnLB = MakeProcInstance( (FARPROC)fnLBSub, hinstApp ))
      (DWORD)lpfnLBold = SetWindowLong( hwnd, GWL_WNDPROC, (DWORD)lpfnLB ) ;

   DASSERT( hWDB, lpfnLB ) ;
   DASSERT( hWDB, lpfnLBold ) ;

   return hwnd ;

} /* CreateLB()  */

VOID WINAPI DestroyLB( void )
{
   if (hwndLB)
   {

      if ((DWORD)lpfnLB)
      {   
         SetWindowLong( hwndLB, GWL_WNDPROC, (DWORD)lpfnLBold ) ;
         FreeProcInstance( (FARPROC)lpfnLB ) ;
         lpfnLB = NULL ;
      }

      if (hdcMem)
         DeleteDC( hdcMem ) ;

      DestroyWindow( hwndLB ) ;

      if (hbrListboxSelect)
         DeleteObject( hbrListboxSelect ) ;

      if (szOutBuf)
         FREEFROMHEAP( hhScratch, szOutBuf ) ;

      if (szOutBuf2)
         FREEFROMHEAP( hhScratch, szOutBuf2 ) ;

   }
}

/****************************************************************
 *  VOID WINAPI ResetLBValues( VOID )
 *
 *  Description: 
 *
 *    To reduce the number of far function calls during drawing
 *    we only get the colors used for the boxes when a
 *    WM_SYSCOLORCHANGE comes through.  This function is called
 *    from CreateLB() and the main window proc.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI ResetLBValues( VOID )
{
   HDC   hDC ;

   /*
    * This is considered a CANCEL type operation.
    */

   ResetLBColors() ;

   if (!(hDC = GetDC( NULL )))
      return ;

   if (hfontCur)
   {
      TEXTMETRIC  tm ;

      hfontCur = SelectObject( hDC, hfontCur ) ;
      cxChar = GetTextMetricsCorrectly( hDC, &tm ) ;
      hfontCur = SelectObject( hDC, hfontCur ) ;

      cyChar = (tm.tmHeight + tm.tmExternalLeading) ;

      cyItem = cyChar * 2 + 80 ;

      /*
       * Set up columns (using ColumnTextOut())
       *
       *    Col 0 - Family    DT_LEFT
       *    Col 1 - Filename  DT_LEFT
       *    Col 2 - Size      DT_RIGHT
       *    Col 3 - Date      DT_RIGHT
       *    Col 4 - Time      DT_RIGHT
       */

      rgColumns[0].nLeft = 0 ;
      rgColumns[0].nRight = rgColumns[0].nLeft +
         cxChar * (MAX_FNAME + MAX_EXT + CCHCOLWIDTH) ;
      rgColumns[0].wFlags = DT_LEFT ;

      rgColumns[1].nRight = rgColumns[0].nRight +
         cxChar * (LF_FACESIZE + CCHCOLWIDTH) ;
      rgColumns[1].wFlags = DT_LEFT ;

      rgColumns[2].nRight = rgColumns[1].nRight + 
                   cxChar * (CCHFILESIZE + CCHCOLWIDTH) ;
      rgColumns[2].wFlags = DT_RIGHT ;

      rgColumns[3].nRight = rgColumns[2].nRight +
         cxChar * (lstrlen( TimeGetCurDate( szOutBuf, DATE_SHORTDATE ) )
                   + CCHCOLWIDTH) ;
      rgColumns[3].wFlags = DT_RIGHT ;

      rgColumns[4].nRight = rgColumns[3].nRight +
         cxChar * (lstrlen( TimeGetCurTime( szOutBuf, 0 ) ) + CCHCOLWIDTH) ;
      rgColumns[4].wFlags = DT_RIGHT ;

   }
   else
   {
      DP1( hWDB, "Could not get DC or hfontCur is NULL" );
      return ;
   }


   if (hwndLB)
   {
      SendMessage( hwndLB, LB_SETITEMHEIGHT, 0,
                               MAKELPARAM( cyItem, 0 ) ) ;
   }

   ReleaseDC( NULL, hDC ) ;

} /* ResetLBValues()  */


/****************************************************************
 *  VOID WINAPI ResetLBColors( VOID )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI ResetLBColors( VOID )
{
   HDC hDC ;

   rgbColorWindowText = GetSysColor( COLOR_WINDOWTEXT ) ;
   rgbColorWindow     = GetSysColor( COLOR_WINDOW ) ;

   rgbColorHighlightText = GetSysColor( COLOR_HIGHLIGHTTEXT ) ;
   rgbColorHighlight     = GetSysColor( COLOR_HIGHLIGHT ) ;

   if (hbrListboxSelect)
      DeleteObject( hbrListboxSelect ) ;

   hbrListboxSelect = CreateSolidBrush( rgbColorHighlight ) ;

   if (!(hDC = GetDC( NULL )))
      return ;


   ReleaseDC( NULL, hDC ) ;

} /* ResetLBColors()  */

/****************************************************************
 *  BOOL WINAPI lbProcessCmd( HWND hLB, WORD wCtlMsg )
 *
 *  Description: 
 *
 *    Processes a list box command message.  This function is
 *    responsible for expanding and collapsing branches and so forth.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI lbProcessCmd( HWND hLB, WORD wCtlMsg )
{
   WORD           wIndex ;

   DPF5( hWDB, "lbProcessCmd:" ) ;
   switch(wCtlMsg)
   {
#if 0
      case LBN_DBLCLK:
         /*
          * On a double click we try to expand.
          */
         wIndex = (WORD)SendMessage( hLB, LB_GETCURSEL, 0, 0L ) ;

         if (wIndex != LB_ERR)
         {
            LPNODEHEADER lp ;

            /*
             * We don't care if the item is open or closed.  We simply
             * call the function appropriate for this item.
             * 
             */
            lp = (LPNODEHEADER)SendMessage( hwndLB, LB_GETITEMDATA,
                                            (WPARAM)wIndex, 0L ) ;
            switch( NODEHEADER_wMagic(lp) )
            {
               case FONTFILE_TAG:
                  FileDoubleClick( wIndex, (LPFONTFILE)lp ) ;
               break ;

               case SECTION_TAG:
                  SectionDoubleClick( wIndex, (LPSECTION)lp ) ;                  
               break ;

               case KEYVALUE_TAG:
               default:
               break ;

               
            }


               
         }
         else
            DPS1( hWDB, "   lbProcessCmd: LB_GETCURSEL failed!" ) ;

      break ;

      case LBN_SELCHANGE:
         wIndex = LOWORD((DWORD)SendMessage( hLB, LB_GETCURSEL, 0, 0L )) ;
         /*
          * What to do here.  Check for help?
          */
         if (!semCurFile)
         {
            semCurFile++ ;
            PostMessage( hwndApp, WMU_CURITEMCHANGE, (WPARAM)wIndex, 0L ) ;
         }
      break ;

#endif

      default:
         return FALSE ;
   }
   return TRUE ;

} /* lbProcessCmd()  */

/****************************************************************
 *  LRESULT WINAPI
 *    lbOwnerDraw( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    HWND hWnd is the handle to the parent that received the
 *    WM_DRAWITEM or other owner draw message.
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT WINAPI
   lbOwnerDraw( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{
   static DRAWITEMSTRUCT   disPrev ;
   DRAWITEMSTRUCT          disCur ;
   LPNODEHEADER            lpHeader ;
   WORD                    wItemType ;
   HDC                     hDC ;
   LPSTR                   lpszOutBuf ;
   short                   y ;
   short                   dx ;
   HBITMAP                 hbm ;

   switch( wMsg )
   {
      case WM_DRAWITEM:
      {
         disCur = *(LPDRAWITEMSTRUCT)lParam ;

         //if (disCur.CtlType != ODT_LISTBOX)
         //   return 0L ;

         if (!(lpHeader = (LPNODEHEADER)disCur.itemData))
         {
            DP1( hWDB, "lbOwnerDraw(): itemData bad" ) ;
            disPrev = disCur ;
            return (LRESULT)-1 ;
         }

         dx = disCur.rcItem.right - disCur.rcItem.left ;

         /*
         * Get everything into local's for speed.
         */
         hDC = disCur.hDC ;

         /*
          * Do things that all three require.
          */
         if ((disCur.itemState & ODS_SELECTED))
         {
            SetTextColor( hDC, rgbColorHighlightText ) ; 
            SetBkColor( hDC, rgbColorHighlight ) ;
         }
         else
         {
            SetTextColor( hDC, rgbColorWindowText ) ;
            SetBkColor( hDC, rgbColorWindow ) ;
         }

         /*
          * lpHeader points to our item structure.
          *
          * We have 3 drawing routines, one for each item type.
          *
          */
         wItemType = lpHeader->wMagic ;

         switch (wItemType)
         {
            case FONTFILE_TAG:
            {

               y = disCur.rcItem.top + 2 ;

               if ((disCur.itemAction & ODA_FOCUS) &&
                   !(disCur.itemState  & ODS_FOCUS))
                  DrawFocusRect( hDC, &disCur.rcItem ) ;

               if ((disCur.itemID != disPrev.itemID) ||
                   (disCur.itemAction & ODA_DRAWENTIRE) || 
                   (disCur.itemState  & ODS_SELECTED) !=
                   (disPrev.itemState & ODS_SELECTED)    )
               {
                  ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &disCur.rcItem, NULL, 0, NULL) ;
                  SetBkMode( hDC, TRANSPARENT ) ;

                  if (!FONTFILE_lpszOutBuf(lpHeader))
                  {
                     char  szTime[64] ;
                     char  szDate[64] ;
                     struct tm FAR *lpTM ;
                     time_t t ;

                     /*
                     * Build output string.  
                     */
                     lstrcpy( szOutBuf2, FONTFILE_lpszFile(lpHeader) ) ;
                     szOutBuf2[FONTFILE_wOffsetFile(lpHeader) - 1] = '\0' ;

                     t = FONTFILE_tTimeDate(lpHeader) ;

                     lpTM = localtime( &t ) ;

                     TimeFormatTime( szTime, lpTM, 0 ) ;

                     TimeFormatDate( szDate, lpTM, DATE_SHORTDATE ) ;
                     
                     wsprintf( szOutBuf, "%s\t%s\t%lu\t%s\t%s",
                        (LPSTR)szOutBuf2 + FONTFILE_wOffsetFile(lpHeader),
                        (LPSTR)"foo",//FONTFILE_szFamilyName(lpHeader),
                        (DWORD)0,//(FONTFILE_dwFileSize(lpHeader)),
                        (LPSTR)szDate,
                        (LPSTR)szTime ) ;


                     if (FONTFILE_lpszOutBuf(lpHeader) =
                           (LPSTR)ALLOCFROMHEAP( hhOutBuf,
                                                 lstrlen( szOutBuf ) + 1 ))
                     {                                          
                        lstrcpy( FONTFILE_lpszOutBuf(lpHeader), szOutBuf ) ;
                     }
                  }

                  if (FONTFILE_lpszOutBuf(lpHeader))
                     lpszOutBuf = FONTFILE_lpszOutBuf(lpHeader) ;
                  else
                     lpszOutBuf = szOutBuf ;

                  ColumnTextOut( hDC, 
                                 CX_FONTFILE,
                                 y,
                                 lpszOutBuf,
                                 lstrlen( lpszOutBuf ),
                                 sizeof( rgColumns ) / sizeof( COLUMNSTRUCT ),
                                 rgColumns ) ;

                  if (0)//hbm =  FONTFILE_hbmThumb( lpHeader ))
                  {
                     /*
                     * Blt the sample
                     */
                     hbm = SelectObject( hdcMem, hbm ) ;
                     BitBlt( hDC, cxChar, y + cyChar + 1, 576, 80, hdcMem, 0, 0, SRCCOPY ) ;
                     hbm = SelectObject( hdcMem, hbm ) ;
                  }
                  else
                  {
                     HFONT             hfont ;
                     HBRUSH            hbr ;
                     HPEN              hpen      ;

                     lstrcpy(lf.lfFaceName, "Arial" ) ;//FONTFILE_szFamilyName(lpHeader) ) ;

                     hfont = CreateFontIndirect( &lf ) ;
                     DASSERT( hWDB, hfont ) ;
                     hfont = SelectObject( hDC, hfont ) ;
                     hbr = SelectObject( hDC, GetStockObject( NULL_BRUSH ) ) ;
                     hpen = SelectObject( hDC, GetStockObject( BLACK_PEN ) ) ;

                     Rectangle( hDC, cxChar, y+cyChar, cxChar+576, y+cyChar+80 ) ;

                     DrawThumbNail( hDC, cxChar, y+cyChar, 576, 80, rglpsz[IDS_SAMPLE_PANOSE] ) ;

                     hfont = SelectObject( hDC, hfont ) ;
                     SelectObject( hDC, hpen ) ;
                     SelectObject( hDC, hbr ) ;
                     DeleteObject( hfont ) ;
                  }

               }

               /*
                * if we are gaining focus draw the focus rect after drawing
                * the text.
                */
               if ((disCur.itemAction & ODA_FOCUS) &&
                   (disCur.itemState & ODS_FOCUS))
                     DrawFocusRect( hDC, &disCur.rcItem ) ;
            }
            break ;

            default:
               DP1( hWDB, "Unknown TAG type = %d", wItemType ) ;
               disPrev = disCur ;
               return (LRESULT)-1 ;
            break ;
         }

         disPrev = disCur ;
      } 
      break ;

      case WM_MEASUREITEM:
      {
         LPMEASUREITEMSTRUCT lpMIS ;

         DP4( hWDB, "WM_MEASUREITEM" ) ;

         lpMIS = (LPMEASUREITEMSTRUCT)lParam ;
         
         lpMIS->itemHeight = cyItem ;
      }
      break ;

      case WMU_CURITEMCHANGE:
      {
         LPNODEHEADER lp = (LPNODEHEADER)SendMessage( hwndLB, LB_GETITEMDATA,
                                         (WPARAM)wParam, 0L ) ;
         if (lp != (LPNODEHEADER)LB_ERR)
         {
            semCurFile-- ;

            //lpCurNode = lp ;

            UpdateInfo( ) ;

         }
      }
      break ;

      case WM_DELETEITEM:
      break ;

      case WM_CHARTOITEM:
      {
         /*
          * We get this message when the listbox gets WM_CHAR messages.
          * wParam is the key and HIWORD(lParam) is the caret pos,
          * LOWORD(lparam) is hwndLB
          */
         char  c = (char)(WORD)wParam  ;
         WORD  wIndex = HIWORD( (DWORD)lParam ) ;
         WORD  w ;

         switch(c)
         {
            case VK_RETURN:
               /*
                * Same as a double click
                */
               PostMessage( hwndApp, WM_COMMAND, (WPARAM)IDCHILD_LISTBOX,
                      MAKELPARAM( hwndLB, LBN_DBLCLK ) ) ;

            break ;

            default:
               /*
                * Search the listbox for the next item that begins with the
                * character.  If it's not found below, then start at the
                * top again.
                */
               w = FindItem( hwndLB, c, wIndex ) ;
               if (w == wIndex)
                  return (LRESULT)-1 ;
               else
                  return MAKELRESULT( w, 0 ) ;
         }
      }
      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

      case WM_VKEYTOITEM:
         /*
          * We get this message when the listbox gets WM_KEYDOWN messages.
          * wParam is the vkey code and HIWORD(lParam) is the caret pos,
          * LOWORD(lparam) is hwndLB
          */
         if ((WORD)wParam == VK_SPACE)
         {
            /*
             * Same as a right mouse button down
             */
         }
         else
            return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
      break ;

      default:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

   }

   return 0L ;
}

/****************************************************************
 * VOID WINAPI DrawThumbNail( HDC hDC, short x, short y, short dx, short dy, LPSTR lpszSample )
 *
 *  Description: 
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI DrawThumbNail( HDC hDC, short x, short y, short dx, short dy, LPSTR lpszSample )
{
   LPSTR             lps, lps2 ;
   UINT              n ;
   SIZE              size ;

   for (lps = lps2 = lpszSample, n = 0 ; *lps2 ; lps2++)
   {
      GetTextExtentPoint( hDC, lps, n+2, &size ) ;
      if (size.cx > dx)
      {
         GetTextExtentPoint( hDC, lps, n+1, &size ) ;
         TextOut( hDC, x+(dx-size.cx)/2, y+(dy-size.cy)/2, lps, n+1 ) ;

         n = 0 ;

         return ;
      }
      else
         n++ ;

   }

   if (*lps)
   {
      GetTextExtentPoint( hDC, lps, n, &size ) ;
         TextOut( hDC, x+(dx-size.cx)/2, y+(dy-size.cy)/2, lps, n ) ;
   }


} /* DrawThumbNail()  */


/****************************************************************
 *  WORD NEAR PASCAL FindItem( HWND hwndLB, char c, WORD w )
 *
 *  Description: 
 *
 *    Returns the index of the next item in the listbox that starts
 *    with the letter c (starting search at index w).
 *
 *  Comments:
 *
 ****************************************************************/
WORD NEAR PASCAL FindItem( HWND hwndLB, char c, WORD w )
{
   WORD  wNumItems = (WORD)SendMessage( hwndLB, LB_GETCOUNT, 0, 0L ) ;

   for (++w; w < wNumItems ; w++)
   {
      if (ItemStartsWith( hwndLB, w, c ))
         return w ;
   }

   /*
    * If we fell out here then we need to start at the top
    */
   for ( w = 0 ; w < wNumItems ; w++)
   {
      if (ItemStartsWith( hwndLB, w, c ))
         return w ;
   }

   /*
    * If we fell out here, then the item simply does not exist
    */
   return LB_ERR ;

} /* FindItem()  */


/****************************************************************
 *  BOOL NEAR PASCAL ItemStartsWith( HWND hwndLB, WORD w, char c )
 *
 *  Description: 
 *
 *    Returns TRUE if the item in hwndLB identified by index w
 *    starts with the character in c.
 *
 ****************************************************************/
BOOL NEAR PASCAL ItemStartsWith( HWND hwndLB, WORD w, char c )
{
   LPNODEHEADER lpn = (LPNODEHEADER)SendMessage( hwndLB, LB_GETITEMDATA,
                                                 (WPARAM)w, 0L ) ;

   if (lpn && ((DWORD)lpn != LB_ERR))
   {
      switch( lpn->wMagic )
      {
         case FONTFILE_TAG:
            if (toupper( *(LPSTR)FONTFILE_lpszFile(lpn) ) ==
                toupper( c ))
               return TRUE ;
         break ;
      }
   }

   return FALSE ;

} /* ItemStartsWith()  */

/****************************************************************
 *  LONG NEAR PASCAL ColumnTextOut( HDC hDC, int nX, int nY, LPSTR lpsz, int cbString, int cColumns, LPCOLUMNSTRUCT rgColumns )
 *
 *  Description: 
 *
 *    The ColumntTextOut function writes a character string at
 *    the specified location, using tabs to identify column breaks.  Each
 *    column is aligned according to the array of COLUMNSTRUCTs.
 *
 *    A COLUMNSTRUCT looks like this:
 *
 *    {
 *       int   nLeft ;       // starting x position of the column
 *       int   nRight ;      // ending x position of the column
 *       WORD  wFlags ;      // format flags
 *    }
 *
 *    If a column has another column to the left of it, it's nLeft member
 *    is ignored.
 *
 *    wFlags can be any of the following:
 *
 *          #define DT_LEFT      0x0000
 *          #define DT_CENTER    0x0001
 *          #define DT_RIGHT     0x0002
 *
 *  Comments:
 *
 ****************************************************************/
LONG NEAR PASCAL ColumnTextOut( HDC hDC, int nX, int nY,
                                LPSTR lpsz, int cbString,
                               int cColumns, LPCOLUMNSTRUCT rgColumns )
{

#define ETOFLAGS ETO_CLIPPED

   COLUMNSTRUCT   CS ;              /* local copy for speed */
   RECT           rc ;              /* current cell rect */
   int            cIteration = 0 ;  /* what column */
   LPSTR          lpNext ;          /* next string (current is lpsz) */
   int            cch ;             /* count of chars in current string */
   DWORD          dwExtent ;        /* extent of current string */
   short          dx ;              /* column width */

   if (cColumns == 0)
      return NULL ;

   rc.left = nX ;
   rc.top = nY ;
   rc.right = 0 ;

   /*
    * For each sub string (bracketed by a tab)
    */
   while (*lpsz)
   {
      if (cIteration > cColumns)
         return NULL ;

      for (cch = 0, lpNext = lpsz ;
           *lpNext != '\t' && *lpNext ;
           lpNext++, cch++)
         ;

      CS = rgColumns[cIteration] ;

      /*
       * If it's the leftmost column use
       */
      if (cIteration == 0)
      {
         rc.left = nX + CS.nLeft ;
         rc.right = nX + CS.nRight ;
      }
      else
         rc.right = nX + CS.nRight ;

      dwExtent = GetTextExtent( hDC, lpsz, cch ) ;
      rc.bottom = rc.top + HIWORD( dwExtent ) ;

      /*
       * If the width of the column is 0 do nothing
       */
      if ((dx = (rc.right - rc.left)) > 0)
      {
         switch(CS.wFlags)
         {
            case DT_CENTER:
            
               ExtTextOut( hDC,
                           rc.left + ((dx - LOWORD( dwExtent )) / (int)2),
                           rc.top, ETOFLAGS,
                           &rc, lpsz, cch, NULL ) ;
            break ;

            case DT_RIGHT:
               /*
                * If it's right justified then make the left border 0
                */
               //rc.left = nX + rgColumns[0].nLeft ;

               ExtTextOut( hDC,
                           rc.left + (dx - LOWORD( dwExtent )),
                           rc.top, ETOFLAGS,
                           &rc, lpsz, cch, NULL ) ;
            break ;

            case DT_LEFT:
            default:
               ExtTextOut( hDC, rc.left, rc.top,
                           ETOFLAGS,
                           &rc, lpsz, cch, NULL ) ;
            break ;
         }

      }

      rc.left = rc.right ;
      cIteration++ ;
      lpsz = lpNext + 1 ;
   }

} /* ColumnTextOut()  */


/****************************************************************
 *  DWORD CALLBACK fnLBSub( HWND hwnd, WORD wMsg, WPARAM wP, LPARAM lP )
 *
 *  Description: 
 *
 *    Subclass procedure for the listbox.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD CALLBACK fnLBSub( HWND hwnd, WORD wMsg, WPARAM wP, LPARAM lP )
{
   return CallWindowProc( lpfnLBold, hwnd, wMsg, wP, lP ) ;

} /* fnLBSub()  */


/****************************************************************
 *  VOID WINAPI SetLBRedraw( BOOL f )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI SetLBRedraw( BOOL f )
{
   static    BOOL     fRedrawOff = 0 ;

   if (f)
   {
      if (fRedrawOff)
         fRedrawOff-- ;
   }
   else
      fRedrawOff++ ;
   
   if (fRedrawOff)
      SendMessage( hwndLB, WM_SETREDRAW, FALSE, 0L ) ;
   else
      SendMessage( hwndLB, WM_SETREDRAW, TRUE, 0L ) ;

} /* SetListboxRedraw()  */

/************************************************************************
 * End of File: listbox.c
 ***********************************************************************/

