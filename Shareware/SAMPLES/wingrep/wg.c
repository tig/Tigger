/** wg.c
 *
 *
 ** Grog! */


/* the includes that we need */
   #include <windows.h>
   #include "wg.h"
   #include "dos.h"
   #include "string.h"
   #include "fgrep.h"

/* miscillaneous defines */
   #define WS_MAXTOKENS    10       /* maximum number of search tokens  */
   #define WS_MAXLINE      255      /* a reasonable string length       */


/* the global variables used */
   BOOL  fDialogEnd;
   HWND  hDlg;
   char  szTemp[ WS_MAXLINE + 1 ];
   BOOL  fGO;
   BOOL  fPan;    /* flag to scroll about bitmap on first paint   */
   BOOL  fGrep;

   char *aszTokens[ WS_MAXTOKENS + 1 ];
   char  szSearchString[ WS_MAXLINE + 1 ];


   HANDLE hInst;

   FARPROC lpwgDlgProc;


short BuildSearchTable( char **aszTokens, char *pString, short maxTokens );


/** short BuildSearchTable( char **aszTokens, char *pString, short maxTokens )
 *
 * DESCRIPTION:
 *    This function is responsible for building a search table that
 * is usable by the fgBuildTable() function.  Basically, it breaks
 * pString up into tokens and points the corresponding elements of
 * aszTokens at the individual tokens.  This routine will not excede
 * maxTokens.  Also note that aszTokens should be at least maxTokens
 * + 1 in size.
 *
 * PARAMETERS:
 *    char **aszTokens  :  Pointer to an array of pointers to put the
 *                         pointers to the tokens into.  This array
 *                         must be at least maxTokens + 1 in size.  It
 *                         must be able to NULL the last token slot.
 *
 *    char *pString     :  Pointer to a null terminated buffer that
 *                         needs to be parsed into a search table for
 *                         use with the fgBuildTable() function.
 *                         This string will be overwritten, so don't
 *                         pass a const string.
 *
 *    short maxTokens   :  The maximum number of tokens to break
 *                         pString into.  If more are possible, only
 *                         maxTokens will be created.
 *
 * RETURN (short):
 *    The return value is the number of tokens actually created.  It
 * is zero if no tokens were found (in the event that pString did not
 * contain text).
 *
 * NOTE:
 *    pString will be destroyed!  Nulls will be placed between the
 * tokens.  Also, this routine does not support \" yet!
 *
 ** Grog! */

short BuildSearchTable( char **aszTokens, char *pString, short maxTokens )
{
   short i;             /* counter for the number of tokens created  */
   char  c;             /* a tempory character value for quickness   */
   short j;             /* counter for length of quoted tokens       */

   /* step through pString and break it up into tokens */
   for ( i = 0; (i < maxTokens) && (c = *pString); /* DON'T i++ */ )
   {
      /* step over any 'white-space' */
      while ( isspace( c ) )
      {
         if ( ! (c = *(++pString)) )
            break;
      }

      /* if we are at a quoted string, then break out up to next quote */
      if ( c == '\"' )
      {
         /* if not "" then tokenize it */
         if ( *(++pString) != '\"' )
         {
            /* point next token at one past quote */
            aszTokens[ i++ ] = ++pString;

            /* step to next quote or end of string */
            while ( (c = *pString) && (c != '\"') )
               pString++;

            /* null terminate token and increment pointer */
            *pString = '\0';  pString++;
         }

         /* otherwise, someone is being stupid! ignore them */
         else pString++;
      }

      /* just break out until next white-space */
      else if ( c != '\0' )
      {
         /* point next token here and increment to next char */
         aszTokens[ i++ ] = pString++;

         /* while there are more chars and not white-space */
         while ( (c = *pString) && !isspace( c ) )
            pString++;

         /* null terminate token and increment pointer */
         *pString = '\0';  pString++;
      }
   }

   /* now null terminate the array of tokens */
   aszTokens[ i ] = NULL;

   /* return the number of tokens created */
   return ( i );
} /* BuildSearchTable() */


/** BOOL FAR PASCAL AboutDlgProc( hDlg, message, wParam, lParam )
 *
 *
 *
 ** Grog! */

BOOL FAR PASCAL AboutDlgProc( hDlg, message, wParam, lParam )
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
   switch ( message )
   {
      case WM_INITDIALOG:
      {
         HBITMAP  hBitmap;
         BITMAP   bm;

         hBitmap = LoadBitmap( hInst, "ABOUT" );
         GetObject( hBitmap, sizeof(BITMAP), (LPSTR)&bm );
         SetWindowPos( hDlg, NULL, 0, 0,
                  bm.bmWidth + GetSystemMetrics(SM_CXDLGFRAME) + 8,
                  bm.bmHeight + GetSystemMetrics(SM_CYDLGFRAME) + 8,
                  SWP_NOMOVE|SWP_NOREDRAW|SWP_NOACTIVATE|SWP_NOZORDER );
         SetWindowPos( GetDlgItem(hDlg, IDOK), NULL, 0, 0,
                  bm.bmWidth, bm.bmHeight,
                  SWP_NOMOVE|SWP_NOREDRAW|SWP_NOACTIVATE|SWP_NOZORDER );
      }
      break;

      case WM_LBUTTONDOWN:
         EndDialog( hDlg, TRUE );
      break;

      case WM_COMMAND:
         if ( wParam == IDOK )
            EndDialog( hDlg, TRUE );
         else
            return ( FALSE );
      break;

      default:
         return ( FALSE );    
      break;
   }

   /* return that everything is cool */
   return ( TRUE );
} /* AboutDlgProc() */


/** void DrawBitmap( hDC, pRect, hBitmap )
 *
 *
 *
 *
 ** Grog! */

void DrawBitmap( hDC, pRect, hBitmap )
HDC      hDC;
LPRECT   pRect;
HBITMAP  hBitmap;
{
   BITMAP   bm;
   DWORD    dwSize;
   HDC      hMemDC;
   POINT    pt;
   HBITMAP  hOldBitmap;

   hMemDC = CreateCompatibleDC( hDC );
   hOldBitmap = SelectObject( hMemDC, hBitmap );
   SetMapMode( hMemDC, GetMapMode(hDC) );

   GetObject( hBitmap, sizeof(BITMAP), (LPSTR)&bm );
   pt.x = bm.bmWidth;  pt.y = bm.bmHeight;
   DPtoLP ( hDC, &pt, 1 );

   BitBlt( hDC, pRect->left, pRect->top, pt.x, pt.y, 
            hMemDC, 0, 0, (DWORD)SRCCOPY );

   SelectObject( hMemDC, hOldBitmap );
   DeleteDC( hMemDC );
} /* Drawbitmap() */



/** long FAR PASCAL BitClassWndProc( hWnd, wMsg, wParam, lParam )
 *
 *
 *
 ** Grog! */

long FAR PASCAL BitClassWndProc( hWnd, wMsg, wParam, lParam )
HWND hWnd;
WORD wMsg;
WORD wParam;
LONG lParam;
{
   HBITMAP  hBitmap;
   HDC      hDC;
   PAINTSTRUCT ps;
   RECT     rcClient, rcPan;
   int      i;

   switch ( wMsg )
   {
      case WM_CREATE:
         fPan = TRUE;
      break;

      case WM_PAINT:
         hDC = BeginPaint( hWnd, &ps );

         hBitmap = LoadBitmap( hInst, "ABOUT" );

         GetClientRect( hWnd, &rcClient );

         if ( hBitmap != NULL ) 
         {
            if ( fPan )
            {
               for( i = 100; i > 0; i = i - 5 ) 
               {
                  CopyRect( &rcPan, &rcClient );
                  rcPan.top += i;
                  DrawBitmap( hDC, (LPRECT)&rcPan, hBitmap );
               }
            }

            DrawBitmap( hDC, (RECT far *) &rcClient, hBitmap );

            fPan = FALSE;
            DeleteObject(hBitmap);
         }

         else FillRect( hDC, &rcClient, GetStockObject(GRAY_BRUSH) );

         EndPaint( hWnd, &ps );
      break;

      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
      case WM_KEYDOWN:
         SendMessage( GetParent(hWnd), WM_COMMAND,
               GetWindowWord(hWnd, GWW_ID), MAKELONG(hWnd, BN_CLICKED) );
      break;

      default:
         return ( DefWindowProc( hWnd, wMsg, wParam, lParam ) );
      break;
   }

   return ( 0L );
} /* BitClassWndProc() */



/** BOOL FAR PASCAL RegisterBitClass( HANDLE hInstance )
 *
 *
 *
 ** Grog! */

BOOL FAR PASCAL RegisterBitClass( HANDLE hInstance )
{
   WNDCLASS wndclass;

   wndclass.style          = CS_HREDRAW | CS_VREDRAW;
   wndclass.lpfnWndProc    = BitClassWndProc;
   wndclass.cbClsExtra     = 0;
   wndclass.cbWndExtra     = 0;
   wndclass.hInstance      = hInstance;
   wndclass.hIcon          = NULL;
   wndclass.hCursor        = NULL;
   wndclass.hbrBackground  = GetStockObject (NULL_BRUSH);
   wndclass.lpszMenuName   = NULL;
   wndclass.lpszClassName  = "BITCLASS";

   if ( !RegisterClass( &wndclass ) )
      return ( FALSE );
} /* RegisterBitClass() */


/** void PeekYield( void )
 *
 *
 *
 *
 ** Grog! */

void PeekYield( void )
{
   MSG msg;

   if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
   {
      if( !IsDialogMessage( hDlg, &msg ) ) 
      {
         TranslateMessage( (LPMSG)&msg );
         DispatchMessage( (LPMSG)&msg );
      }
   }
} /* PeekYield() */


/** int SearchDir( char *szPath, char *szFind, short flags )
 *
 *
 *
 *
 ** Grog! */

int SearchDir( char *szPath, char *szFind, short flags )
{
   int   nResult = 0;
   char *szEndPath;
   struct find_t  DTABuffer;
   char numBuf[15];
   short numFound;

   PeekYield();

   if ( fDialogEnd || (!fGO) )
      return ( -1 );

   /* display status line */
   strcpy( szTemp, "STALKING: " );  strcat( szTemp, szPath );
   SetWindowText( GetDlgItem(hDlg, ID_STATUS), szTemp );

   szEndPath = szPath + strlen(szPath) + 1;

   /* append '\' and name to search for to end of path */
   strcat( szPath, "\\" );  strcat( szPath, szFind );

   nResult = _dos_findfirst( szPath, _A_NORMAL, &DTABuffer );

   while( nResult == 0 )
   {
      *szEndPath = '\0';

      /* append name found */
      strcat(szPath, DTABuffer.name);

      /* if we are supposed to grep the file as we go */
      if ( fGrep )
      {
         if ( numFound = fgEnumFile( szPath, aszTokens, NULL, flags ) )
         {
            sprintf( numBuf, "   [%d]", numFound );
            strcat( szPath, numBuf );
         }
      }


      /* add name to list box of found files */
      SendDlgItemMessage( hDlg, ID_SRCHFOUND, LB_ADDSTRING, 0, 
                           (LONG)(LPSTR)szPath );


      nResult = _dos_findnext( &DTABuffer );
   }

   *szEndPath = '\0';

   /* append '*.*' to path */
   strcat( szPath, "*.*" );

   nResult = _dos_findfirst( szPath, _A_SUBDIR, &DTABuffer );

   while ( nResult == 0 )
   {
      if ( (DTABuffer.attrib == _A_SUBDIR /* 0x10 */) &&
            strcmp( DTABuffer.name, "." /* name != "." */) &&
            strcmp( DTABuffer.name, ".." /* name != ".." */) )
      {
         *szEndPath = '\0';

         /* append dir name */
         strcat( szPath, DTABuffer.name );

         nResult = SearchDir( szPath, szFind, flags );
         if ( nResult != 0 )
            return ( -1 );
      }
      nResult = _dos_findnext( &DTABuffer );
   }

   *(--szEndPath) = '\0';
   return ( 0 );
} /* SearchDir() */


/** void StalkDrives( short flags )
 *
 *
 *
 *
 ** Grog! */

void StalkDrives( short flags )
{
   short nOccurances;
   short i;
   short nDlgCount;
   char  szFind[13];
   char  szPath[256];   /* must make this large, because the search  */
                        /* code appends to construct paths           */

   GetDlgItemText( hDlg, ID_SRCHFILE, szFind, 12 );
   nDlgCount = SendDlgItemMessage( hDlg, ID_SRCHDRIVES, LB_GETCOUNT, 0, 0L );

   /* if we are GREPing, the create the stalker tokens */
   if ( fGrep )
   {
      /* create the GREPable token array */
      GetDlgItemText( hDlg, ID_SRCHSTRING, szSearchString, WS_MAXLINE );

      /* if no tokens and user wants grep, the tell them no dice */
      if ( !BuildSearchTable( aszTokens, szSearchString, WS_MAXTOKENS ) )
      {
         MessageBox( hDlg, "You MUST supply a GREP string!", 
                     "YOU MORON!!!", IDOK );
         return;
      }
   }

   /* go ahead and stalk our prey */
   for ( i = 0; i<nDlgCount; i++ )
   {
      if ( SendDlgItemMessage(hDlg, ID_SRCHDRIVES, LB_GETSEL, i, 0L) > 0 ) 
      {
         SendDlgItemMessage( hDlg, ID_SRCHDRIVES, LB_GETTEXT, i, (long)(LPSTR)szPath );

         szPath[0] = szPath[2];  szPath[1] = ':';  szPath[2] = '\0';

         fGO = TRUE;

         SendDlgItemMessage( hDlg, IDOK, WM_CANCELMODE, 0, 0L );
         nOccurances = SearchDir( szPath, szFind, flags );

         if ( nOccurances == -1 )
            break;

         fGO = FALSE;
      }
   }
} /* StalkDrives() */


/** BOOL FAR PASCAL wgDlgProc( hDlg, message, wParam, lParam )
 *
 *
 *
 *
 ** Grog! */

BOOL FAR PASCAL wgDlgProc( hDlg, message, wParam, lParam )
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
   short nOccurances;
   short i;
   short nDlgCount;
   short nItemState;
   FARPROC lpAboutDlgProc;
   HICON hIcon;
   HDC   hDC;
   short flags;
   char  szPath[256];   /* must make this large, because the search  */
                        /* code appends to construct paths           */


   switch ( message )
   {
      case WM_COMMAND:
         switch ( wParam ) 
         {
            case IDOK:
               if ( !fGO ) 
               {
                  fGrep = IsDlgButtonChecked( hDlg, ID_CHKGREP );

                  flags = IsDlgButtonChecked(hDlg,ID_CHKCASE)?0:FGF_NOCASE;

                  /* search */
                  SetWindowText( GetDlgItem(hDlg, IDOK), "STOP!" );
                  SendDlgItemMessage( hDlg, ID_SRCHFOUND, LB_RESETCONTENT, 0, 0L );

                  StalkDrives( flags );

                  SetWindowText( GetDlgItem(hDlg, IDOK), "GO!" );
                  SetWindowText( GetDlgItem(hDlg, ID_STATUS), "...Done!" );
               }
         
               else fGO = FALSE;
            break;

            case ID_EXIT:
            case IDCANCEL:
               fDialogEnd = TRUE;
               PostMessage( GetParent(hDlg),WM_CLOSE, 0, 0L );
            break;

            case ID_ABOUT:
               lpAboutDlgProc = MakeProcInstance( (FARPROC)AboutDlgProc, hInst );
               DialogBox( hInst, "ABOUT", hDlg, lpAboutDlgProc );
               FreeProcInstance( lpAboutDlgProc );
            break;

            default:
               return ( FALSE );
            break;
         }
      break;

      case WM_INITDIALOG:
         DlgDirList( hDlg, "*.*", ID_SRCHDRIVES, NULL, 0xC000 );
         getcwd( szTemp, WS_MAXLINE );
         nDlgCount = SendDlgItemMessage( hDlg, ID_SRCHDRIVES, LB_GETCOUNT, 0, 0L );

         for ( i = 0; i < nDlgCount; i++ )
         {
            SendDlgItemMessage( hDlg, ID_SRCHDRIVES, LB_GETTEXT, i, (long)(LPSTR)szPath );
            AnsiUpper( szPath );  AnsiUpper( szTemp );
            if ( szPath[2] == szTemp[0] )
               SendDlgItemMessage( hDlg, ID_SRCHDRIVES, LB_SETSEL, 1, (LONG)i );
         }
         return ( TRUE );
      break;

      default:
         return ( FALSE );
   }

   return ( TRUE );
} /* wgDlgProc() */



/** long FAR PASCAL wgWndProc( hWnd, message, wParam, lParam )
 *
 *
 *
 ** Grog! */

long FAR PASCAL wgWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
   if ( message == WM_DESTROY )
   {
      PostQuitMessage(0);
      return ( 0L );
   }

   return  ( DefWindowProc( hWnd, message, wParam, lParam ) );
} /* wgWndProc() */



/** BOOL wgInit( hInstance )
 *
 *
 *
 ** Grog! */

BOOL wgInit( hInstance )
HANDLE hInstance;
{
   PWNDCLASS   pHelloClass;

   if ( pHelloClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) ) )
   {
      pHelloClass->hCursor       = NULL;
      pHelloClass->hIcon         = LoadIcon( hInstance, "WINGREP" );
      pHelloClass->lpszMenuName  = (LPSTR)"WINGREPMENU";
      pHelloClass->lpszClassName = (LPSTR)"WINGREP";
      pHelloClass->hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
      pHelloClass->hInstance     = hInstance;
      pHelloClass->style         = CS_HREDRAW | CS_VREDRAW;
      pHelloClass->lpfnWndProc   = wgWndProc;

      if ( !RegisterClass( (LPWNDCLASS)pHelloClass ) )
      {
         /* Windows will automatically deallocate all allocated memory */
         return FALSE;
      }

      LocalFree( (HANDLE)pHelloClass );
   }

   /* Initialization succeeded */
   return ( TRUE );
} /* wgInit() */


/** int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
 *
 *
 *
 ** Grog! */

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
   char  szFind[13];
   char *szFoo;
   int   nOccurances = 0;
   HWND  hWnd;
   MSG   msg;
   RECT  rcDlg;

   lstrcpy( szTemp, lpszCmdLine );
   szFoo = strtok( szTemp, " /-" );
   strncpy( szFind, szFoo, 12 );

   if ( !hPrevInstance )
   {
      wgInit( hInstance );
      RegisterBitClass( hInstance );
   }

   hInst = hInstance;

   hWnd = CreateWindow( (LPSTR)"WINGREP",
                        (LPSTR)"WinGREP!",
                        WS_BORDER | WS_CAPTION | WS_SYSMENU
                           | WS_OVERLAPPED | WS_MINIMIZEBOX,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        (HWND)NULL,       /* no parent                  */
                        (HMENU)NULL,      /* use class menu             */
                        (HANDLE)hInstance,/* handle to window instance  */
                        (LPSTR)NULL       /* no params to pass on       */
                      );

   if ( hWnd == NULL )  
      return ( FALSE );

   /* add the menu */
   // SetMenu( hWnd, LoadMenu(hInstance, "WINGREPMENU") );


   lpwgDlgProc = MakeProcInstance( (FARPROC)wgDlgProc, hInstance );

   if ( hDlg = CreateDialog( hInstance, "WINGREP", hWnd, lpwgDlgProc ) )
      SetDlgItemText( hDlg, ID_SRCHFILE, szFind );
   else 
      return ( FALSE );

   GetClientRect( hDlg, &rcDlg );
   SetWindowPos( hWnd, NULL, 0, 0,
                 rcDlg.right + GetSystemMetrics(SM_CXBORDER) * 2,
                 rcDlg.bottom + GetSystemMetrics(SM_CYBORDER) * 2 +
                     GetSystemMetrics(SM_CYCAPTION) + 
                     GetSystemMetrics(SM_CYMENU),
                 SWP_NOMOVE | SWP_NOZORDER );

   ShowWindow( hWnd, cmdShow );
   UpdateWindow( hWnd );

   fDialogEnd = fGO = FALSE;

   while ( GetMessage( (LPMSG)&msg, NULL, 0, 0 ) )
   {
      if ( !IsDialogMessage(hDlg, &msg) )
      {
         TranslateMessage( (LPMSG)&msg );
         DispatchMessage( (LPMSG)&msg );
      }
   }

   return ( msg.wParam );
} /* WinMain() */


/** EOF: wg.c  **/
