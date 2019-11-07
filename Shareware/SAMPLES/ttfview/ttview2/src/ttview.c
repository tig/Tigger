/************************************************************************
 *
 *     Project:  TTVIEW 
 *
 *      Module:  TTVIEW.c
 *
 *     Remarks:  Main module, contains WinMain and fnMainWnd
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "GLOBAL.H"

/*
 * Global variables extern'd in TTVIEW.h
 */
HWND        ghwndApp ;
HWND        ghwndStat ;
HWND        ghwndMDIClient ;
HWND        ghwndActiveMDIChild ;

HINSTANCE   ghinstApp ;

LPSTR       rglpsz[LAST_IDS - FIRST_IDS + 1] ;

/*
 * 
 */
HFONT       ghfontSmall ;
HFONT       ghfontCur ;      // this is the one that is settable

/*
 * State variables
 */
BOOL        gfStatLine = TRUE ;

/*
 * Printer setup stuff
 */
GLOBALHANDLE ghDevNames ;
GLOBALHANDLE ghDevMode ;

HLIST      ghlFaces ;     // list of all tt fonts on system


/*
 * Variables local to this module.
 */ 
static HCURSOR    hcurWait ;
static HCURSOR    hcurNorm ;
static HCURSOR    hcurSave ;
static UINT       fWaitCursor ;


#define CX_DEFAULT      4 
#define CY_DEFAULT      2


HLIST WINAPI GetFaceList( HWND hwnd, HLIST hlFace ) ;

typedef struct tagFACEENUM
{
   HDC      hdc ;
   HLIST    hlFace ;

} FACEENUM, FAR * LPFACEENUM ;
int CALLBACK fnFaceEnum( LPENUMLOGFONT lpELF,
   LPNEWTEXTMETRIC lpTextMetrics, short nFontType, LPFACEENUM lpEnum ) ;

/****************************************************************
 *  LRESULT CALLBACK
 *    fnMainWnd( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Main Window procedure.
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT CALLBACK fnMainWnd( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
   switch( uiMsg )
   {
      case WM_CREATE:
      {
         CLIENTCREATESTRUCT ccs ;
         ccs.hWindowMenu = GetSubMenu( GetMenu( hWnd), 2 ) ;
         ccs.idFirstChild = IDM_WINDOW_CHILD ;


         /*
          * Create status bar
          */
         if (gfStatLine)
         {
            if (ghwndStat = CreateStatus( hWnd ))
            {
               SetWindowText( ghwndStat, rglpsz[IDS_STAT_READY] ) ;
               ShowWindow( ghwndStat, SW_NORMAL ) ;
            }
            else
               gfStatLine = FALSE ;


         }

         /* Create Toolbar
          */



         /* Create MDI Client
          */
         if (ghwndMDIClient = CreateWindow( "mdiclient", NULL,
                           WS_CHILD | WS_CLIPCHILDREN |
                           WS_VSCROLL | WS_HSCROLL,
                           0,0,
                           0,0,
                           hWnd,
                           (HMENU)1,
                           ghinstApp,
                           (LPSTR)&ccs ))
         {
            ShowWindow( ghwndMDIClient, SW_SHOW ) ;
         }

         DASSERT( hWDB, ghwndMDIClient ) ;

         CheckMenuItem( GetMenu( hWnd ), (UINT)IDM_OPT_STAT,
                        gfStatLine ? MF_CHECKED : MF_UNCHECKED ) ;

         ghlFaces = GetFaceList( hWnd, ghlFaces ) ;
      }
      break ;

      case WM_DESTROY:
         /*
          * Destroy status bar
          */
         if (ghwndStat)
            DestroyWindow( ghwndStat ) ;

         WinHelp( hWnd, rglpsz[IDS_HELPFILE], HELP_QUIT, 0L ) ;

         /*
          * We already are assured of being SW_RESORE so just
          * get the coords for the save (SaveDefaults)
          */
         PostQuitMessage( 0 ) ;

      break ;

      case WM_CLOSE :
         DestroyWindow( hWnd ) ;
      break ;

      case WM_GETMINMAXINFO:
      case WM_SIZE:
         DP5( hWDB, "WM_SIZE/WM_GETMINMAXINFO" );
         return SizeHandler( hWnd, uiMsg, wParam, lParam ) ;

      case WM_COMMAND:
         DP5( hWDB, "WM_COMMAND" );
         return CmdHandler( hWnd, uiMsg, wParam, lParam ) ;

      case WM_SYSCOMMAND:
         DP5( hWDB, "WM_SYSCOMMAND" );
         return SysCmdHandler( hWnd, uiMsg, wParam, lParam ) ;

      case WM_INITMENU:
      case WM_MENUSELECT:
      case WM_INITMENUPOPUP:
         return MenuHandler( hWnd, uiMsg, wParam, lParam ) ;

      default:
         if (ghwndMDIClient)
            return DefFrameProc( hWnd, ghwndMDIClient, uiMsg, wParam, lParam ) ;
         else
            return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
   }

   return 0L ;

} /* fnMainWnd()  */


HLIST WINAPI GetFaceList( HWND hwnd, HLIST hlFace )
{
   HDC            hdc ;
   FONTENUMPROC   lpfn ;
   FACEENUM       faceEnum ;

   /* Enumerate all true type fonts on the system.
    */
   hdc = GetDC( hwnd ) ;

   faceEnum.hdc = hdc ;
   faceEnum.hlFace = cekmmListCreate( NULL ) ;
   DASSERT( hWDB, faceEnum.hlFace ) ;

   lpfn = (FONTENUMPROC)MakeProcInstance( (FARPROC)fnFaceEnum,
                                          ghinstApp ) ;
   EnumFontFamilies( hdc, NULL, lpfn, (LPARAM)(LPVOID)&faceEnum) ;
   FreeProcInstance( (FARPROC)lpfn ) ;

   ReleaseDC( hwnd, hdc ) ;

   if (hlFace)
      cekmmListDestroy( hlFace ) ;

   return faceEnum.hlFace ;
}


int CALLBACK fnFaceEnum( LPENUMLOGFONT lpELF,
   LPNEWTEXTMETRIC lpTextMetrics, short nFontType, LPFACEENUM lpEnum )
{
   LPFACE         lpFace ;
   TEXTMETRIC     tm ;
   HFONT          hfont ;
   int            cy ;
   int            nItem ;


   if (!(nFontType & TRUETYPE_FONTTYPE))
      return 1 ;

   if (lpFace = (LPFACE)cekmmListAllocNode( lpEnum->hlFace,
                        NULL, sizeof(FACE) ))
   {
      lpFace->elf = *lpELF ;

      /* Build the list of styles
       */

      cekmmListInsertNode( lpEnum->hlFace, lpFace, LIST_INSERTTAIL ) ;
   }

   return 1 ;

} /* ()  */



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

   DP5( hWDB, "Entering message loop..." ) ;
   while (GetMessage (&msg, NULL, 0, 0))
   {
      if (!TranslateAccelerator( ghwndApp, haccl, &msg ))
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
 * End of File: TTVIEW.c
 ***********************************************************************/

