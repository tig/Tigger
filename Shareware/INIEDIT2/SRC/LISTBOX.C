/************************************************************************
 *
 *     Project:  INIEDIT 2.0
 *
 *      Module:  listbox.c
 *
 *     Remarks:  Handles the listbox child window.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#define WIN31
#include "iniedit.h"
#include <memory.h>
#include "listbox.h"
#include "cmd.h"
#include "font.h"
#include "wintime.h"
#include "dlghelp.h"
#include "bmutil.h"
#include "file.h"
#include "section.h"
#include "keyvalue.h"

/*
 * local defines
 */

#define CXBORDER     2        // extra space to left and right
#define CCHCOLWIDTH  3        // number of characters between columns.
#define CCHFILESIZE  4        // number of chars in a file size

#define CX_INIFILE   ((2*CXBORDER) + cxIcon)  // offset to text

#define CXEDITBORDEREXTRA  (CXBORDER+1)
#define CYEDITBORDEREXTRA  4

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
DWORD CALLBACK fnEditSub( HWND hwnd, WORD wMsg, WPARAM wP, LPARAM lP ) ;

LRESULT NEAR PASCAL lbMeasureItem( HWND hWnd, LPMEASUREITEMSTRUCT lpLBMItem ) ;
LRESULT NEAR PASCAL lbDrawItem( HWND hWnd, LPDRAWITEMSTRUCT lpDIS ) ;

WORD NEAR PASCAL FindItem( HWND hwndLB, char c, WORD w ) ;
BOOL NEAR PASCAL ItemStartsWith( HWND hwndLB, WORD w, char c ) ;

short NEAR PASCAL DrawAnIcon( HDC hDC, short x, short y, WORD wID,
                              BOOL bSelected ) ;


BOOL NEAR PASCAL PopupEdit( VOID ) ;
BOOL NEAR PASCAL HideEdit( BOOL fEscape ) ;
VOID NEAR PASCAL MoveEdit( VOID ) ;


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
static HBITMAP  hbmIcons ;
static HDC      hdcMemIcons ;
static short    cxIcon ;         // icon height
static short    cyIcon ;         // icon width ;

static short    cyItem ;         // item height
static short    cyEdit ;         // edit control height
static short    cyEditBorder ;   // edit control border 

static COLUMNSTRUCT  rgColumns[6] ; 

static LPSTR    szOutBuf ;  // scratch buffer for output
static LPSTR    szOutBuf2 ;  // scratch buffer for output

static WORD          wCurIndex ;

#define SUBCLASSLB

static WNDPROC  lpfnLB ;         // subclass proc
static WNDPROC  lpfnLBold ;       // subclass proc

static WNDPROC  lpfnEdit ;         // subclass proc
static WNDPROC  lpfnEditOld ;       // subclass proc

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
             LBS_WANTKEYBOARDINPUT | 
             WS_VSCROLL |
             LBS_NOTIFY) ;

   if (fWin31)
      dwStyle |= LBS_OWNERDRAWFIXED | LBS_DISABLENOSCROLL ;
   else
      dwStyle |= LBS_OWNERDRAWVARIABLE | WS_CLIPCHILDREN  ;

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

   SendMessage( hwnd, WM_SETFONT, (WPARAM)hfontLB, (LPARAM)0L ) ;

   if ((FARPROC)lpfnLB = MakeProcInstance( (FARPROC)fnLBSub, hinstApp ))
      (DWORD)lpfnLBold = SetWindowLong( hwnd, GWL_WNDPROC, (DWORD)lpfnLB ) ;

   DASSERT( hWDB, lpfnLB ) ;
   DASSERT( hWDB, lpfnLBold ) ;

   hwndEdit = CreateWindow
      (
         "edit",
         "",
         WS_CHILD |
//         WS_BORDER |
//         ES_NOHIDESEL |
         ES_AUTOHSCROLL,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         hwnd,                 
         (HMENU)NULL,
         hinstApp,            
         NULL                  
      ) ;

   SendMessage( hwndEdit, WM_SETFONT, (WPARAM)hfontLB, (LPARAM)0L ) ;

   if ((FARPROC)lpfnEdit = MakeProcInstance( (FARPROC)fnEditSub, hinstApp ))
      (DWORD)lpfnEditOld = SetWindowLong( hwndEdit, GWL_WNDPROC, (DWORD)lpfnEdit ) ;

   DASSERT( hWDB, lpfnEdit ) ;
   DASSERT( hWDB, lpfnEditOld ) ;

   return hwnd ;

} /* CreateLB()  */

VOID WINAPI DestroyLB( void )
{
   if (hwndLB)
   {
      if (hdcMemIcons)
      {
         hbmIcons = SelectObject( hdcMemIcons, hbmIcons ) ;
         DeleteDC( hdcMemIcons ) ;
      }

      if (hbmIcons)
         DeleteObject( hbmIcons ) ;


      if ((DWORD)lpfnLB)
      {   
         SetWindowLong( hwndLB, GWL_WNDPROC, (DWORD)lpfnLBold ) ;
         FreeProcInstance( (FARPROC)lpfnLB ) ;
         lpfnLB = NULL ;
      }

      if ((DWORD)lpfnEdit)
      {   
         SetWindowLong( hwndEdit, GWL_WNDPROC, (DWORD)lpfnEditOld ) ;
         FreeProcInstance( (FARPROC)lpfnEdit ) ;
         lpfnEdit = NULL ;
      }

      if (hwndEdit)
         DestroyWindow( hwndEdit ) ;

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
   if (hwndEdit && IsWindowVisible( hwndEdit ))
      HideEdit( TRUE ) ;

   ResetLBColors() ;

   if (!(hDC = GetDC( NULL )))
      return ;

   if (hfontLB)
   {
      TEXTMETRIC  tm ;

      hfontLB = SelectObject( hDC, hfontLB ) ;
      cxChar = GetTextMetricsCorrectly( hDC, &tm ) ;
      hfontLB = SelectObject( hDC, hfontLB ) ;

      cyChar = (tm.tmHeight + tm.tmExternalLeading) ;

      cyItem = max( (cyChar + 1), cyIcon ) ; ;

      /*
       * The edit control has a border (WS_BORDER).  This border is
       * cyEditBorder pixels wide.
       */
      cyEditBorder = CYEDITBORDEREXTRA ;
      cyEdit = (cyChar + 1) + (cyEditBorder * 2) ;

      /*
       * Set up columns (using ColumnTextOut())
       *
       *    Col 0 - Filename  DT_LEFT
       *    Col 1 - Size      DT_RIGHT
       *    Col 2 - Date      DT_RIGHT
       *    Col 3 - Time      DT_RIGHT
       *    Col 4 - Dir       DT_LEFT
       */

      rgColumns[0].nLeft = 0 ;
      rgColumns[0].nRight = rgColumns[0].nLeft +
         cxChar * (MAX_FNAME + MAX_EXT + CCHCOLWIDTH) ;
      rgColumns[0].wFlags = DT_LEFT ; 

      rgColumns[1].nRight = rgColumns[0].nRight + 
                   cxChar * (CCHFILESIZE + CCHCOLWIDTH) ;
      rgColumns[1].wFlags = DT_RIGHT ;

      rgColumns[2].nRight = rgColumns[1].nRight +
         cxChar * (lstrlen( TimeGetCurDate( szOutBuf, DATE_SHORTDATE ) )
                   + CCHCOLWIDTH) ;
      rgColumns[2].wFlags = DT_RIGHT ;

      rgColumns[3].nRight = rgColumns[2].nRight +
         cxChar * (lstrlen( TimeGetCurTime( szOutBuf, 0 ) ) + CCHCOLWIDTH) ;
      rgColumns[3].wFlags = DT_RIGHT ;

      rgColumns[4].nRight = rgColumns[3].nRight +
         cxChar * (CCHCOLWIDTH) ;
      rgColumns[4].wFlags = DT_LEFT ;

      rgColumns[5].nRight = rgColumns[4].nRight +
         cxChar * (MAX_DIR + CCHCOLWIDTH) ;
      rgColumns[5].wFlags = DT_LEFT ;
   }
   else
   {
      DP1( hWDB, "Could not get DC or hFontLB is NULL" );
      return ;
   }


   if (hwndLB)
   {
      if (fWin31)
         SendMessage( hwndLB, LB_SETITEMHEIGHT, 0,
                              MAKELPARAM( cyItem, 0 ) ) ;
      else
      {
         int      nItems ;
         int      n ;
         LPVOID   FAR* rglp ;
         WORD     wID ;

         /*
          * Since 3.0 is stupid and does not allow us to send
          * LB_SETITEMHEIGHT, we have to remove all items
          * and add them again!
          */
         /*
          * make a list of all pointers and their order
          */
         wID = LOWORD((DWORD)SendMessage( hwndLB,
                              LB_GETCURSEL, 0, 0L )) ;
         nItems = (int)LOWORD((DWORD)SendMessage( hwndLB,
                              LB_GETCOUNT, 0, 0L )) ;

         if ((LPVOID)rglp = ALLOCFROMHEAP( hhScratch, sizeof( LPVOID ) * nItems ))
         {
            for ( n = 0 ; n < nItems ; n++)
               rglp[n] = (LPVOID)SendMessage( hwndLB,
                              LB_GETITEMDATA, n, 0L ) ;

            SendMessage( hwndLB, LB_RESETCONTENT, 0, 0L ) ;

            for ( n = 0 ; n < nItems ; n++)
               LBAddString( hwndLB, (LPSTR)rglp[n] ) ;

            SendMessage( hwndLB, LB_SETCURSEL, (WPARAM)wID, 0L ) ;

            FREEFROMHEAP( hhScratch, rglp ) ;
         }
      }
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

   /*
    * GetBitmap and DC for icons
    */
   if (hdcMemIcons)
   {
      hbmIcons = SelectObject( hdcMemIcons, hbmIcons ) ;
      DeleteDC( hdcMemIcons ) ;
   }

   if (hbmIcons)
      DeleteObject( hbmIcons ) ;

   cxIcon = 0 ;
   cyIcon = 0 ;

   if (hbmIcons = LoadBitmap( hinstApp, MAKEINTRESOURCE( IDB_LISTBOXICONS ) ))
   {
      BITMAP   bmBits ;

      /*
       * hdcMemIcons is a global.  We keep it around so we don't have
       * to keep re-createing it!
       */
      if (hdcMemIcons = CreateCompatibleDC( hDC ))
      {
         //
         // Get the bitmap struct needed by bmColorTranslateDC()
         //
         GetObject( hbmIcons, sizeof( BITMAP ), (LPSTR)&bmBits ) ;
         cxIcon = bmBits.bmWidth  / NUMICONS ;
         cyIcon = bmBits.bmHeight / 2 ;

         //
         // Select our bitmap into the memory DC
         //
         hbmIcons = SelectObject( hdcMemIcons, hbmIcons ) ;

         /*
          * Translate the background for the first row (not selected)
          */
         bmColorTranslateDC( hdcMemIcons, &bmBits, GetPixel(hdcMemIcons,0,0),
                              rgbColorWindow, 0 ) ;

         /*
          * Translate the background for the second row (selected)
          */
         bmColorTranslateDC( hdcMemIcons, &bmBits, GetPixel(hdcMemIcons,0,cxIcon+1),
                              rgbColorHighlight, 0 ) ;

      }
   }

   DASSERT( hWDB, hdcMemIcons ) ;

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
               case INIFILE_TAG:
                  FileDoubleClick( wIndex, (LPINIFILE)lp ) ;
               break ;

               case SECTION_TAG:
                  SectionDoubleClick( wIndex, (LPSECTION)lp ) ;                  
               break ;

               case KEYVALUE_TAG:
               default:
                  /*
                   * If it's a keyvalue that was double clicked on,
                   * we probably want to do the same thing we do
                   * when it's clicked on a second time...put in the
                   * cool wayqzomo in-situ edit control
                   */
                  PopupEdit() ;

               break ;

               
            }


               
         }
         else
            DPS1( hWDB, "   lbProcessCmd: LB_GETCURSEL failed!" ) ;

      break ;

      case LBN_SELCHANGE:
         if (IsWindowVisible( hwndEdit ))
         {
            DP1( hWDB, "LBN_SELCHANGE with a visible edit!" ) ;
            break ;
         }

         wIndex = LOWORD((DWORD)SendMessage( hLB, LB_GETCURSEL, 0, 0L )) ;
         /*
          * What to do here.  Check for help?
          */
         if (!semCurFile)
         {
            semCurFile++ ;
            PostMessage( hwndApp, WMU_CURFILECHANGE, (WPARAM)wIndex, 0L ) ;
         }
      break ;

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
            case INIFILE_TAG:
            {

               y = disCur.rcItem.top + ((disCur.rcItem.bottom - disCur.rcItem.top)/2);

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

                  /*
                  * Draw our icon
                  */
                  DrawAnIcon( hDC,
                              CXBORDER,
                              y - ( cyIcon / 2 ),
                              INIFILE_wIcon(lpHeader),
                              disCur.itemState & ODS_SELECTED) ;

                  if (!INIFILE_lpszOutBuf(lpHeader))
                  {
                     char  szTime[64] ;
                     char  szDate[64] ;
                     struct tm FAR *lpTM ;
                     time_t t ;

                     DP( hWDB, "Generating output string" ) ;

                     /*
                     * Build output string.  
                     */
                     lstrcpy( szOutBuf2, INIFILE_lpszFile(lpHeader) ) ;
                     szOutBuf2[INIFILE_wOffsetFile(lpHeader) - 1] = '\0' ;


                     t = INIFILE_tTimeDate(lpHeader) ;

                     lpTM = localtime( &t ) ;

                     TimeFormatTime( szTime, lpTM, 0 ) ;

                     TimeFormatDate( szDate, lpTM, DATE_SHORTDATE ) ;

                     wsprintf( szOutBuf, "%s\t%lu\t%s\t%s\t\t%s\\",
                        (LPSTR)szOutBuf2 + INIFILE_wOffsetFile(lpHeader),
                        (DWORD)(INIFILE_dwFileSize(lpHeader)),
                        (LPSTR)szDate,
                        (LPSTR)szTime,
                        (LPSTR)szOutBuf2 ) ;


                     if (INIFILE_lpszOutBuf(lpHeader) =
                           (LPSTR)ALLOCFROMHEAP( hhOutBuf,
                                                 lstrlen( szOutBuf ) + 1 ))
                     {                                          
                        lstrcpy( INIFILE_lpszOutBuf(lpHeader), szOutBuf ) ;
                     }
                  }

                  if (INIFILE_lpszOutBuf(lpHeader))
                     lpszOutBuf = INIFILE_lpszOutBuf(lpHeader) ;
                  else
                     lpszOutBuf = szOutBuf ;

                  ColumnTextOut( hDC, 
                                 CX_INIFILE,
                                 y - ( cyChar / 2 ),
                                 lpszOutBuf,
                                 lstrlen( lpszOutBuf ),
                                 sizeof( rgColumns ) / sizeof( COLUMNSTRUCT ),
                                 rgColumns ) ;
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

            case SECTION_TAG:
            {
               y = disCur.rcItem.top + ((disCur.rcItem.bottom - disCur.rcItem.top)/2);

               disCur.rcItem.left += CX_INIFILE ;

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

                  /*
                  * Draw our icon
                  */
                  DrawAnIcon( hDC,
                              disCur.rcItem.left + CXBORDER,
                              y - ( cyIcon / 2 ),
                              SECTION_wIcon(lpHeader),
                              disCur.itemState & ODS_SELECTED) ;

                  ExtTextOut( hDC,
                     disCur.rcItem.left + CX_INIFILE,
                                 y - ( cyChar / 2 ),
                                 0, &disCur.rcItem,
                                 SECTION_lpszSec(lpHeader),
                                 lstrlen( SECTION_lpszSec(lpHeader) ),
                                 NULL ) ;
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

            case KEYVALUE_TAG:
            {
               y = disCur.rcItem.top + ((disCur.rcItem.bottom - disCur.rcItem.top)/2);

               disCur.rcItem.left += CX_INIFILE * 2 ;

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

                  /*
                  * Draw our icon
                  */
                  DrawAnIcon( hDC,
                              disCur.rcItem.left + CXBORDER,
                              y - ( cyIcon / 2 ),
                              KEYVALUE_wIcon(lpHeader),
                              disCur.itemState & ODS_SELECTED) ;


                  if (!KEYVALUE_lpszOutBuf(lpHeader))
                  {
                     if (!KEYVALUE_lpszValue(lpHeader))
                        wsprintf( szOutBuf, "%s = ",
                           (LPSTR)KEYVALUE_lpszKey(lpHeader) ) ;
                     else
                        wsprintf( szOutBuf, "%s = %s",
                           (LPSTR)KEYVALUE_lpszKey(lpHeader),
                           (LPSTR)KEYVALUE_lpszValue(lpHeader) ) ;


                     if (KEYVALUE_lpszOutBuf(lpHeader) =
                           (LPSTR)ALLOCFROMHEAP( hhOutBuf,
                                                 lstrlen( szOutBuf ) + 1 ))
                     {                                          
                        lstrcpy( KEYVALUE_lpszOutBuf(lpHeader), szOutBuf ) ;
                     }
                  }

                  if (KEYVALUE_lpszOutBuf(lpHeader))
                     lpszOutBuf = KEYVALUE_lpszOutBuf(lpHeader) ;
                  else
                     lpszOutBuf = szOutBuf ;

                  ExtTextOut( hDC,
                     disCur.rcItem.left + CX_INIFILE,
                                 y - ( cyChar / 2 ),
                                 0, &disCur.rcItem,
                                 lpszOutBuf,
                                 lstrlen( lpszOutBuf ),
                                 NULL ) ;
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

         DP5( hWDB, "WM_MEASUREITEM" ) ;

         lpMIS = (LPMEASUREITEMSTRUCT)lParam ;
         
         lpMIS->itemHeight = cyItem ;
      }
      break ;

      case WMU_CURFILECHANGE:
      {
         LPNODEHEADER lp = (LPNODEHEADER)SendMessage( hwndLB, LB_GETITEMDATA,
                                         (WPARAM)wParam, 0L ) ;
         if (lp != (LPNODEHEADER)LB_ERR)
         {
            semCurFile-- ;

            lpCurNode = lp ;

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
            if (!IsWindowVisible( hwndEdit ))
            {
               PopupEdit( ) ;
               return -2 ;
            }
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
         case INIFILE_TAG:
            if (toupper( *(LPSTR)INIFILE_lpszFileName(lpn) ) == toupper( c ))
               return TRUE ;
         break ;

         case SECTION_TAG:
            if (toupper( *(LPSTR)SECTION_lpszSec(lpn) ) == toupper( c ))
               return TRUE ;
         break ;

         case KEYVALUE_TAG:
            if (toupper( *(LPSTR)KEYVALUE_lpszKey(lpn) ) == toupper( c ))
               return TRUE ;
         break ;
      }
   }

   return FALSE ;

} /* ItemStartsWith()  */

/****************************************************************
 *  short NEAR PASCAL DrawAnIcon( HDC hDC, short x, short y, WORD wID )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
short NEAR PASCAL DrawAnIcon( HDC hDC, short x, short y, WORD wID,
                              BOOL bSelected )
{
   if (!hdcMemIcons)
      return FALSE ;

   BitBlt( hDC,
           x,
           y,
           cxIcon,
           cyIcon, 
           hdcMemIcons,
           wID * cxIcon,
           bSelected ? cyIcon : 0, SRCCOPY ) ;

   return TRUE ;

} /* DrawAnIcon()  */

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
   POINT pt ;
   WORD  wID ;
   RECT  rc ;

   switch (wMsg)
   {
      case WM_RBUTTONDOWN:
         /*
          * if the click is on an item
          */
         pt.x = LOWORD( lP ) ;
         pt.y = HIWORD( lP ) ;

         wID = (WORD)(DWORD)SendMessage( hwndLB, LB_GETCURSEL, 0, 0L ) ;
         if (wID != -1)
            SendMessage( hwndLB, LB_GETITEMRECT, wID, (LPARAM)(LPRECT)&rc ) ;
         else
            break ;

         if (PtInRect( &rc, pt ))
         {   
            if (PopupEdit( ))
               return 0L ;
            else
               CallWindowProc( lpfnLBold, hwnd, wMsg, wP, lP ) ;
         }
         else
         {
            /*
             * If the edit window is up, the user clicked somewhere
             * else in the listbox.
             */
            if (HideEdit( FALSE ))
               return CallWindowProc( lpfnLBold, hwnd, wMsg, wP, lP ) ;
            else
               return 0L ;
         }
      break ;

      case WM_LBUTTONDOWN:
         if (IsWindowVisible( hwndEdit ))
            HideEdit( FALSE ) ;
      break ;

      #if 0
      case WM_CTLCOLOR:
         /*
          * Make the background of the edit control the same as that
          * of the listbox highlight
          */
         if ((HWND)LOWORD( lP ) == hwndEdit)
         {
            if (HIWORD( lP ) == CTLCOLOR_EDIT ||
                HIWORD( lP ) == CTLCOLOR_MSGBOX)
               return hbrListboxSelect ;
         }
      break ;
      #endif

      case WM_RBUTTONDBLCLK:

      break ;

      case WM_SIZE:
      {
         LRESULT lr ;

         lr = CallWindowProc( lpfnLBold, hwnd, wMsg, wP, lP ) ;
         /*
          * when the listbox changes size, we have to size the
          * edit control to (horz only).
          */
         MoveEdit() ;

         return lr ;
      }
      break ;

   }

   return CallWindowProc( lpfnLBold, hwnd, wMsg, wP, lP ) ;

} /* fnLBSub()  */


/****************************************************************
 *  BOOL NEAR PASCAL PopupEdit( VOID )
 *
 *  Description: 
 *
 *    Pops up the edit window over the currently selected item.
 *    Puts the text associated with the item in the edit
 *    control.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL PopupEdit( VOID )
{
   RECT rc ;
   LPNODEHEADER lpN ;

   if (IsWindowVisible( hwndEdit ))
      return FALSE ;

   wCurIndex = (WORD)(DWORD)SendMessage( hwndLB, LB_GETCURSEL, 0, 0L ) ;
   if (wCurIndex == -1)
      return FALSE ;

   SendMessage( hwndLB, LB_GETITEMRECT, wCurIndex, (LPARAM)(LPRECT)&rc ) ;

   lpN = (LPNODEHEADER)SendMessage( hwndLB, LB_GETITEMDATA, (WPARAM)wCurIndex,
                                    0L ) ;

   switch (NODEHEADER_wMagic( lpN ))
   {
      case SECTION_TAG:
         SetWindowText( hwndEdit, SECTION_lpszSec( lpN ) ) ;
      break ;

      case KEYVALUE_TAG:
         if (!KEYVALUE_lpszValue( lpN ))
            wsprintf( szOutBuf, "%s = ", (LPSTR)KEYVALUE_lpszKey( lpN ) ) ;
         else
            wsprintf( szOutBuf, "%s = %s",
               (LPSTR)KEYVALUE_lpszKey( lpN ),
               (LPSTR)KEYVALUE_lpszValue( lpN ) ) ;

          SetWindowText( hwndEdit, szOutBuf ) ;
      break ;

      case INIFILE_TAG:
      default:
         return FALSE ;
      break ;
   }

   MoveEdit( ) ;

   SetWindowPos( hwndEdit, 0, 0, 0, 0, 0,
                  SWP_SHOWWINDOW |
                  SWP_NOSIZE |
                  SWP_DRAWFRAME |
                  SWP_NOMOVE ) ;

   SendMessage( hwndEdit, EM_SETSEL, 0,
                MAKELPARAM( GetWindowTextLength( hwndEdit ),
                            GetWindowTextLength( hwndEdit ) ) ) ;
   SetFocus( hwndEdit ) ;

   if (!fWin31)
      InvalidateRect( hwndEdit, NULL, TRUE ) ;

   //UpdateWindow( hwndEdit ) ;

} /* PopupEdit()  */

/****************************************************************
 *  BOOL NEAR PASCAL HideEdit( BOOL fEscape )
 *
 *  Description: 
 *
 *    Hides the edit control and copies the text to the
 *    currently selected item.
 *
 *    If fEscape is TRUE the text is not copied.
 *
 *    If FALSE is returned, the thing wasn't hidden.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL HideEdit( BOOL fEscape )
{
   RECT rc ;
   LPNODEHEADER lpN ;

   if (!IsWindowVisible( hwndEdit ))
      return FALSE ;

   SendMessage( hwndLB, LB_GETITEMRECT, wCurIndex, (LPARAM)(LPRECT)&rc ) ;

   lpN = (LPNODEHEADER)SendMessage( hwndLB, LB_GETITEMDATA, (WPARAM)wCurIndex,
                                    0L ) ;

   switch (NODEHEADER_wMagic( lpN ))
   {
      case SECTION_TAG:
         if (fEscape)
            break ;

         if (!UpdateSection( hwndEdit ))
            return FALSE ;
      break ;

      case KEYVALUE_TAG:
         if (fEscape)
            break ;

         if (!UpdateKeyValue( hwndEdit ))
            return FALSE ;
      break ;

      case INIFILE_TAG:
      default:
         return FALSE ;
      break ;
   }

   if (IsWindowVisible( hwndEdit ))
      ShowWindow( hwndEdit, SW_HIDE ) ;

   return TRUE ;

} /* HideEdit()  */

/****************************************************************
 *  VOID NEAR PASCAL MoveEdit( VOID )
 *
 *  Description: 
 *
 *    Positions the edit control so that it fits just right.
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL MoveEdit( VOID )
{
   RECT rc ;
   LPNODEHEADER lpN ;
   WORD  wFlags ;

   if ((DWORD)-1 == (DWORD)SendMessage( hwndLB, LB_GETITEMRECT,
                                 (WPARAM)wCurIndex, (LPARAM)(LPRECT)&rc ))
   {
      DP1( hWDB, "MoveEdit: LB_GETITEMRECT failed!" ) ;
      return ;
   }

   lpN = (LPNODEHEADER)SendMessage( hwndLB, LB_GETITEMDATA, (WPARAM)wCurIndex,
                                    0L ) ;

   DASSERT( hWDB, lpN ) ;

   /*
    * The edit control needs to be cyEdit high.  Thus it needs to
    * be placed in the y direction at
    * (rc.bottom - rc.top) / 2 - cyEdit / 2
    */

   if (fWin31)
      wFlags = SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOZORDER ;
   else
      wFlags = SWP_NOACTIVATE | SWP_NOZORDER ;

   switch( NODEHEADER_wMagic(lpN) )
   {
      case SECTION_TAG:
         SetWindowPos( hwndEdit, 0,
            rc.left + 2 * CX_INIFILE - CXEDITBORDEREXTRA,
            rc.top - ((cyEdit / 2) - ((rc.bottom - rc.top) / 2) ),
            rc.right - rc.left - 2 * CX_INIFILE + CXEDITBORDEREXTRA,
            cyEdit,
            wFlags ) ;
      break ;

      case KEYVALUE_TAG:
         SetWindowPos( hwndEdit, 0,
            rc.left + 3 * CX_INIFILE - CXEDITBORDEREXTRA,
            rc.top - ((cyEdit / 2) - ((rc.bottom - rc.top) / 2) ),
            rc.right - rc.left - 3 * CX_INIFILE + CXEDITBORDEREXTRA,
            cyEdit,
            wFlags ) ;
      break ;

      default:
         return ;
   }

} /* MoveEdit()  */


/****************************************************************
 *  DWORD CALLBACK fnEditSub( HWND hwnd, WORD wMsg, WPARAM wP, LPARAM lP )
 *
 *  Description: 
 *
 *    Subclass procedure for the edit control.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD CALLBACK fnEditSub( HWND hwnd, WORD wMsg, WPARAM wP, LPARAM lP )
{
   switch (wMsg)
   {
      case WM_KEYDOWN:
      case WM_KEYUP:
         switch( (WORD)wP )
         {
            case VK_UP:
            case VK_DOWN:
               HideEdit( FALSE ) ;
               SendMessage( hwndLB, wMsg, wP, lP ) ;
            return 0L ;
         }   
      break ;

      case WM_CHAR:
         switch ((WORD)wP)
         {
            case  VK_ESCAPE:
               HideEdit( TRUE ) ;
            return 0L ;

            case  VK_RETURN:
               HideEdit( FALSE ) ;
            return 0L ;

            case  VK_TAB:
               /*
                * Save changes, move the selection, then
                * pop er up again.
                */
               HideEdit( FALSE ) ;
               if (GetKeyState( VK_SHIFT ) & 0x8000)
                  SendMessage( hwndLB, WM_KEYDOWN, VK_UP, 0L ) ;
               else
                  SendMessage( hwndLB, WM_KEYDOWN, VK_DOWN, 0L ) ;
               PopupEdit() ;
            return 0L ;
         }
      break ;

      case WM_NCCALCSIZE:
      {
         LRESULT  lr ;
         lr = CallWindowProc( lpfnEditOld, hwnd, wMsg, wP, lP ) ;

         InflateRect( (LPRECT)lP, -CXEDITBORDEREXTRA, -CYEDITBORDEREXTRA ) ;

         return lr ;
      }
      break ;

      case WM_NCPAINT:
      {
         RECT     rc ;
         RECT     rcNC ;
         HDC      hDC = GetWindowDC( hwnd ) ;

         GetWindowRect( hwnd, &rc ) ;
         rcNC.left = 0 ;
         rcNC.top = 0 ;
         rcNC.right = rc.right - rc.left ;
         rcNC.bottom = rc.bottom - rc.top ;


         SetBkColor( hDC, rgbColorWindow ) ;

         rc = rcNC ;
         rc.bottom = 4 ;
         DRAWFASTRECT( hDC, &rc ) ;

         rc = rcNC ;
         rc.top = rc.bottom - 4 ;
         DRAWFASTRECT( hDC, &rc ) ;
         
         rc = rcNC ;
         rc.right = 3 ;
         DRAWFASTRECT( hDC, &rc ) ;

         rc = rcNC ;
         rc.left = rc.right - 3 ;
         DRAWFASTRECT( hDC, &rc ) ;

         SetBkColor( hDC, rgbColorHighlight ) ;
         rc = rcNC ;
         rc.bottom = 2 ;
         DRAWFASTRECT( hDC, &rc ) ;

         rc = rcNC ;
         rc.top = rc.bottom - 2 ;
         DRAWFASTRECT( hDC, &rc ) ;
         
         rc = rcNC ;
         rc.right = 1 ;
         DRAWFASTRECT( hDC, &rc ) ;

         rc = rcNC ;
         rc.left = rc.right - 1 ;
         DRAWFASTRECT( hDC, &rc ) ;

         ReleaseDC( hwnd, hDC ) ;

         return 0L ;
      }
   }

   return CallWindowProc( lpfnEditOld, hwnd, wMsg, wP, lP ) ;

} /* fnEditSub()  */

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

