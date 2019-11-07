/************************************************************************
 *
 *     Project:  DBEDIT 
 *
 *      Module:  DBEDIT.c
 *
 *     Remarks:  Main module, contains WinMain and fnMainWnd
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "DBEDIT.h"
#include "version.h"
#include "config.h"
#include "init.h"
#include "wndmgr.h"
#include "menu.h"
#include "cmd.h"
#include "subdlgs.h"
#include "botstat.h"
#include "listbox.h"
#include "file.h"
#include "fileio.h"
#include "wintime.h"


VOID WINAPI DrawSample( HDC hDC, LPRECT lprc, LPSTR lpszSample ) ;
UINT WINAPI DrawCaption( HDC hDC, HFONT hfont, int x, int y ) ;

/*
 * Global variables extern'd in DBEDIT.h
 */
HWND        hwndApp ;
HWND        hwndLB ;
HWND        hwndStat ;

HINSTANCE   hinstApp ;

LPSTR       rglpsz[LAST_IDS - FIRST_IDS + 1] ;

char        szVerNum[80] ;

/*
 * 
 */
HFONT       hfontSmall ;

HFONT       hfontCur ;      // this is the one that is settable

char        szFaceName[LF_FACESIZE] ;
char        szFaceStyle[LF_FACESIZE] ;


char        szFont[LF_FACESIZE] ;
UINT        wFontSize ;

/*
 * State variables
 */
BOOL        fStatLine ;
short       xPos, yPos, nWidth, nHeight ;

HLIST       hlstFiles ;

HHEAP       hhOutBuf ;    // output buffers
HHEAP       hhLists ;     // all lists
HHEAP       hhStrings ;   // all strings

HHEAP       hhScratch ;    // scratch heap

SORTPROC    lpfnFileCompare ;

/*
 * Semaphores
 */
int         semCurFile ;


/*
 * Variables local to this module.
 */ 
static HCURSOR    hcurWait ;
static HCURSOR    hcurNorm ;
static HCURSOR    hcurSave ;
static UINT       fWaitCursor ;


#define CX_DEFAULT      4 
#define CY_DEFAULT      2

/****************************************************************
 *  LRESULT FAR PASCAL
 *    fnMainWnd( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Main Window procedure.
 *
 *  Comments:
 *
 ****************************************************************/

LRESULT CALLBACK fnMainWnd( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   switch( wMsg )
   {
      case WM_CREATE:

         /*
          * Create status bar
          */
         if (fStatLine)
         {
            if (hwndStat = CreateStatus( hWnd ))
            {
               SetWindowText( hwndStat, rglpsz[IDS_STAT_READY] ) ;
               ShowWindow( hwndStat, SW_NORMAL ) ;
            }
            else
               fStatLine = FALSE ;


         }

         CheckMenuItem( GetMenu( hWnd ), (UINT)IDM_OPT_STAT,
                        fStatLine ? MF_CHECKED : MF_UNCHECKED ) ;


         DP1( hWDB, "CreateLB..." ) ;
         /*
          * Create listbox
          */
         if (hwndLB = CreateLB( hWnd ))
            ShowWindow( hwndLB, SW_NORMAL ) ;
         else
         {
            DP1( hWDB, "Could not create ListBox, bailing out.") ;
            return (LRESULT)-1 ;
         }


         DP3( hWDB, "end of WM_CREATE" ) ;
      break ;

      case WM_SETFOCUS:
            if (hwndLB)
               SetFocus( hwndLB ) ;
            else
               return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

      break ;

      case WM_DESTROY:
         /*
          * Destroy status bar
          */
         if (hwndStat)
            DestroyWindow( hwndStat ) ;

         if (hlstFiles)
         {
            LPFONTFILE   lpFONT ;

            if (lpFONT = hlsqListFirstNode( hlstFiles ))
               do
               {
                  if (FONTFILE_hbmThumb( lpFONT ))
                     DeleteObject( FONTFILE_hbmThumb( lpFONT ) ) ;
               }
               while (lpFONT = hlsqListNextNode( hlstFiles, lpFONT )) ;

            hlsqListDestroy( hlstFiles ) ;
            hlstFiles = NULL ;
         }

         /*
          * Destroy listbox
          */
         DestroyLB() ;


         WinHelp( hWnd, rglpsz[IDS_HELPFILE], HELP_QUIT, 0L ) ;

         /*
          * We already are assured of being SW_RESORE so just
          * get the coords for the save (SaveDefaults)
          */
         PostQuitMessage( 0 ) ;

      break ;

      case WM_CLOSE :
      {
#if 0
         BOOL fShiftDown = (GetKeyState( VK_SHIFT ) & 0x8000) ;

         if (!fShiftDown)
            switch( ErrorResBox( hWnd, hinstApp,
                                 MB_YESNOCANCEL | MB_ICONQUESTION,
                                 IDS_APPTITLE, IDS_ERR_SAVECHANGES ))
            {
               case IDCANCEL :
                  return 0L ;
               break ;

               case IDYES :
//                  if (!DoSaveConfig())
//                  return 0L ;
               break ;

               case IDNO :
                  SaveWndPos( hWnd ) ;
                  return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
               break ;

               default:
                  MessageBeep( 0 ) ;
                  return 0L ;
               break ;
            }
#endif
         SaveWndPos( hWnd ) ;
         return DefWindowProc (hWnd, wMsg, wParam, lParam) ;
      }
      break ;

      case WM_GETMINMAXINFO:
      case WM_SIZE:
         DP5( hWDB, "WM_SIZE/WM_GETMINMAXINFO" );
         return SizeHandler( hWnd, wMsg, wParam, lParam ) ;

      case WM_COMMAND:
         DP5( hWDB, "WM_COMMAND" );
         return CmdHandler( hWnd, wMsg, wParam, lParam ) ;

      case WM_SYSCOMMAND:
         DP5( hWDB, "WM_SYSCOMMAND" );
         return SysCmdHandler( hWnd, wMsg, wParam, lParam ) ;

      case WM_INITMENU:
      case WM_MENUSELECT:
      case WM_INITMENUPOPUP:
         return MenuHandler( hWnd, wMsg, wParam, lParam ) ;

      case WM_SYSCOLORCHANGE:
         ResetLBColors() ;
      break ;

      case WM_DRAWITEM:
      case WM_MEASUREITEM:
      case WM_CHARTOITEM:
      case WM_VKEYTOITEM:
      case WMU_CURITEMCHANGE:
      case WM_DELETEITEM:
         return lbOwnerDraw( hWnd, wMsg, wParam, lParam ) ;
      break ;

      case WM_WININICHANGE:
         if (0 == lstrcmpi( "Intl", (LPSTR)lParam ))
         {
            TimeResetInternational() ;

            /*
             * Need to to something that will cause
             * the strings to be re calculated
             */
         }
      break ;

      default:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }

   return 0L ;

} /* fnMainWnd()  */

typedef struct tagMYENUM
{
   HDC      hDC ;
   int      x ;
   int      dx ;
   int      y ;

}  MYENUM, FAR *LPMYENUM ;
int CALLBACK fnPaintEnum( LPLOGFONT lpLogFont,
      LPNEWTEXTMETRIC lpTextMetrics, short nFontType, LPMYENUM lpMy ) ;


/****************************************************************
 *  LRESULT CALLBACK fnlbWnd( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT CALLBACK fnlbWnd( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   switch( wMsg )
   {
      case WM_PAINT:
      {
         PAINTSTRUCT       ps ;
         RECT              rclb, rc ;
         HFONT             hfont ;

         UINT              x = CX_DEFAULT ;
         UINT              y = CY_DEFAULT ;

         char              szBuf[256] ;
         TEXTMETRIC        tm ;
         MYENUM            myenum ;
         FONTENUMPROC      lpfn ;

         GetClientRect( hWnd, &rclb ) ;

         BeginPaint( hWnd, &ps ) ;

         hfontCur = SelectObject( ps.hdc, hfontCur ) ;
         GetTextMetrics( ps.hdc, &tm ) ;
         GetTextFullName( ps.hdc, 256, szBuf ) ;

         /*
          * Use Times New Roman to draw our header if it's a symbol font or
          * if it looks like the font is a symbol font. 
          */
         DP( hWDB, "face = %s, charset = %d", (LPSTR)szFaceName, tm.tmCharSet ) ;
         if (TRUE) //tm.tmCharSet != ANSI_CHARSET && tm.tmCharSet != DEFAULT_CHARSET)
         {
            hfont = ReallyCreateFontEx( ps.hdc, "Times New Roman", "Bold", 12, 0 )  ;
            hfont = SelectObject( ps.hdc, hfont ) ;
         }

         TextOut( ps.hdc, x, y, szBuf, lstrlen(szBuf) ) ;

         y += tm.tmHeight + tm.tmExternalLeading + CY_DEFAULT ;

         if (TRUE)//tm.tmCharSet != ANSI_CHARSET && tm.tmCharSet != DEFAULT_CHARSET)
         {
            hfont = SelectObject( ps.hdc, hfont ) ;
            DeleteObject( hfont ) ;
         }

         MoveToEx( ps.hdc, x, y, NULL ) ;
         LineTo( ps.hdc, rclb.right-(x*2), y ) ;

         y += CY_DEFAULT ;


         /*
          * The header is now there and x,y is the starting point
          * for our display.
          */
         if (hfont = ReallyCreateFontEx( ps.hdc, szFaceName, szFaceStyle, 32, 0 ))
         {
            /*
             * Draw the main sample
             */
            hfont = SelectObject( ps.hdc, hfont ) ;

            rc.top = y ;
            rc.top += DrawCaption( ps.hdc, hfontSmall, x, rc.top) ;
            /*
             * If it's a symbol font then we can party on the whole
             * area.
             */
            if (tm.tmCharSet == SYMBOL_CHARSET)
            {
               char szBuf[256-32] ;
               UINT i ;

               for (i = 32 ; i < 256 ; i++)
                  szBuf[i-32] = (char)i ;

               rc.left = x ;
               rc.right = rclb.right - 2*x ;
               DrawSample( ps.hdc, &rc, szBuf ) ;
            }
            else
            {
               rc.left = x ;
               rc.right = (rclb.right-2*x) / 2 - CX_DEFAULT ;
               DrawSample( ps.hdc, &rc, rglpsz[IDS_SAMPLE_ALPHA] ) ;
            
               rc.top = rc.bottom ;
               DrawSample( ps.hdc, &rc, rglpsz[IDS_SAMPLE_DIGIT] ) ;
            }

            hfont = SelectObject( ps.hdc, hfont ) ;
            DeleteObject( hfont ) ;
         }

         /*
          * Enumerate the face.  For each style output it.  The
          * first style will be considered the 'main' and will
          * be big (24 point).  the others will be smaller (16 pt?).
          */
         myenum.hDC = ps.hdc ;
         myenum.x = x ;
         myenum.dx = rclb.right - (2*x) ;
         myenum.y = y ;

         lpfn = (FONTENUMPROC)MakeProcInstance( (FARPROC)fnPaintEnum, hinstApp ) ;
         EnumFontFamilies( ps.hdc, szFaceName, lpfn, (LPARAM)(LPVOID)&myenum ) ;
         FreeProcInstance( (FARPROC)lpfn ) ;

         /*
          * Now output the font info table:


            +------------------------------------------------------+
            | Full Name: Arial Bold Microsoft 1992                 |
            |------------------------------------------------------|
            | Font File Size(s):                                   |
            |   


          */

         y = max( myenum.y, rc.bottom ) ;

         hfontCur = SelectObject( ps.hdc, hfontCur ) ;

         EndPaint( hWnd, &ps ) ;
      }
      break ;
      
      default:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }

   return 0L ;
} /* fnlbWnd()  */


/****************************************************************
 *  int CALLBACK fnPaintEnum( LPLOGFONT lpLogFont,
 *       LPNEWTEXTMETRIC lpTextMetrics, short nFontType, LPMYENUM lpMy )
 *
 *  Description: 
 *
 *    EnumFontFamilies callback for the paint routine.
 *
 *  Comments:
 *
 ****************************************************************/
int CALLBACK fnPaintEnum( LPLOGFONT lpLogFont,
      LPNEWTEXTMETRIC lpTextMetrics, short nFontType, LPMYENUM lpMy )
{
   HFONT hfont ;
   RECT  rc ;

   if (!(nFontType & TRUETYPE_FONTTYPE))
      return 1 ;

   if (lpTextMetrics->tmCharSet == SYMBOL_CHARSET)
      return 1 ;

   /*
      * On the right, draw the other samples.
      */
   lpLogFont->lfHeight = -MulDiv( 18,
                           GetDeviceCaps( lpMy->hDC, LOGPIXELSY),
                           72 )  ;
   lpLogFont->lfWidth = 0 ;

   if (hfont = CreateFontIndirect( lpLogFont ))
   {
      hfont = SelectObject( lpMy->hDC, hfont ) ;

      rc.top = lpMy->y ;
      rc.left = lpMy->x + (lpMy->dx / 2) + 2*CX_DEFAULT ;
      rc.right = lpMy->dx ;

      rc.top += DrawCaption( lpMy->hDC, hfontSmall, rc.left, rc.top ) ;

      /*
         * Draw the main sample
         */
      DrawSample( lpMy->hDC, &rc, rglpsz[IDS_SAMPLE_PANOSE] ) ;
         
      /*
         * draw a caption
         */
      lpMy->y = rc.bottom ;

      hfont = SelectObject( lpMy->hDC, hfont ) ;
      DeleteObject( hfont ) ;
   }

   return 1 ;

} /* fnPaintEnum()  */

/****************************************************************
 *  VOID WINAPI DrawSample( HDC hDC, LPRECT lprc, LPSTR lpszSample )
 *
 *  Description: 
 *
 *    Draws the given sample within the rect.  Upon return
 *    lprc points to the actual rect that it fit in.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI DrawSample( HDC hDC, LPRECT lprc, LPSTR lpszSample )
{
   LPSTR             lps, lps2 ;
   UINT              n ;
   SIZE              size ;
   int               dx ;
   int               x, y;
   UINT              cyChar ;
   TEXTMETRIC        tm ;

   x = lprc->left ;
   y = lprc->top ;
   dx = lprc->right - x ;

   GetTextMetrics( hDC, &tm ) ;
   cyChar = tm.tmHeight + tm.tmExternalLeading ;

   for (lps = lps2 = lpszSample, n = 0 ; *lps2 ; lps2++)
   {
      GetTextExtentPoint( hDC, lps, n+2, &size ) ;
      if (size.cx > dx)
      {
         GetTextExtentPoint( hDC, lps, n+1, &size ) ;
         TextOut( hDC, x+(dx-size.cx)/2, y, lps, n+1 ) ;
         y += cyChar ;

         n = 0 ;

         lps = lps2 + 1 ;
      }
      else
         n++ ;

   }

   if (*lps)
   {
      GetTextExtentPoint( hDC, lps, n, &size ) ;
         TextOut( hDC, x+(dx-size.cx)/2, y, lps, n ) ;
      y += cyChar ;
   }

   lprc->bottom = y ;

} /* DrawSample()  */

/****************************************************************
 *  VOID WINAPI DrawCaption( HDC hDC, HFONT hfont, int x, int y )
 *
 *  Description: 
 *
 *    Draws a little caption using hfont indicating
 *    what the current font is.
 *
 *    Returns the hight of the caption
 *
 *  Comments:
 *
 ****************************************************************/
UINT WINAPI DrawCaption( HDC hDC, HFONT hfont, int x, int y )
{
   char        szStyle[LF_FACESIZE] ;
   char        szFace[LF_FACESIZE] ;
   char        szBuf[LF_FACESIZE * 4] ;
   TEXTMETRIC  tm ;

   GetTextFace( hDC, LF_FACESIZE, szFace ) ;
   GetTextStyle( hDC, LF_FACESIZE, szStyle ) ;

   wsprintf( szBuf, "%s %s %d point",
      (LPSTR)szFace,
      (LPSTR)szStyle,
      GetTextPointSize( hDC ) ) ;

   hfont = SelectObject( hDC, hfont ) ;
   GetTextMetrics( hDC, &tm ) ;
   TextOut( hDC, x, y, szBuf, lstrlen(szBuf) ) ;
   hfont = SelectObject( hDC, hfont ) ;

   return tm.tmHeight + tm.tmExternalLeading ;

} /* DrawCaption()  */

/****************************************************************
 *  VOID WINAPI SetWaitCursor( VOID )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
UINT nCurCount ;

VOID WINAPI SetWaitCursor( VOID )
{
   DASSERT( hWDB, (short)fWaitCursor >= 0 ) ;

   if (!hcurWait)
      hcurWait = LoadCursor( NULL, IDC_WAIT ) ;

   fWaitCursor++ ;

   SetCursor( hcurWait ) ;

} /* SetWaitCursor()  */

/****************************************************************
 *  VOID WINAPI SetNormalCursor( VOID )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI SetNormalCursor( VOID )
{
   DASSERT( hWDB, (short)fWaitCursor >= 0 ) ;
   if (!hcurNorm)
      hcurNorm = LoadCursor( NULL, IDC_ARROW ) ;

   fWaitCursor-- ;

   if (fWaitCursor)
      return ;

   SetCursor( hcurNorm ) ;

} /* SetNormalCursor()  */

/****************************************************************
 *  int FAR PASCAL MessageLoop( HACCEL haccl )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
int FAR PASCAL MessageLoop( HACCEL haccl )
{
   MSG            msg ;

   DP5( hWDB, "Entering message loop." ) ;
   while (GetMessage (&msg, NULL, 0, 0))
   {
      if (!TranslateAccelerator( hwndApp, haccl, &msg ))
      {
         TranslateMessage( &msg ) ;
         DispatchMessage( &msg ) ;
      }
   }

   DP( hWDB, "fWaitCursor = %d", fWaitCursor ) ;
   return (int)msg.wParam ;

} /* MessageLoop()  */

/****************************************************************
 *  int PASCAL
 *    WinMain( HANDLE hinst, HANDLE hinstPrev, LPSTR lpszCmd, in nCmdShow )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
int PASCAL WinMain( HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpszCmd, int nCmdShow )
{
   return MyWinMain( hinst, hinstPrev, lpszCmd, nCmdShow ) ;

} /* WinMain */


/************************************************************************
 * End of File: DBEDIT.c
 ***********************************************************************/

