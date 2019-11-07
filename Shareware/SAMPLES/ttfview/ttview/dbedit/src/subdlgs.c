/************************************************************************
 *
 *     Project:  DBEDIT
 *
 *      Module:  subdlgs.c
 *
 *     Remarks:  Contains stubs for sub dialogs (fileopen, font)
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "DBEDIT.h"
#include <print.h>
#include <sys\types.h>
#include <sys\stat.h>         // time shit
#include <stdlib.h>
#include <cderr.h>      // commdlg errors
#include <string.h>
#include <direct.h>
#include "subdlgs.h"
#include "dlgs.h"
#include "config.h"


static OPENFILENAME  ofn ;
static char          szDirName[256] ;  /* directory name array  */
static char          szFile[256],
                     szFileTitle[256] ;/* file and title arrays */
static char          szDefExt[MAX_EXT*2] ;

static DWORD         dwCurFontFile ;

HLIST NEAR PASCAL GetFontList( HLIST hlst ) ;

int CALLBACK fnGetFontListEnum( LPLOGFONT lpLogFont,
      LPNEWTEXTMETRIC lpTextMetrics, short nFontType, HLIST FAR *lphList ) ;

BOOL NEAR PASCAL PutFontInList( HLIST hlst, LPLOGFONT lpLogFont,
                                LPNEWTEXTMETRIC lpTM, short nFontType,
                                HFONT hFont )  ;

/****************************************************************
 *  BOOL WINAPI DoFileNew( ) ;
 *
 *  Description: 
 *
 *    Pops up the new file dialog box.  This allows the user to
 *    specify a new directory for the database.
 *
 *  Comments:
 *
 *    All this really does is set the cwd to a specific dir.
 *    If there are files in the dir it will erase them (asking
 *    first of course).
 *
 ****************************************************************/
BOOL WINAPI DoFileNew( VOID ) 
{

   getcwd( szDirName, 255 ) ;

#if 0
   SetWaitCursor() ;
   /* Initialize the OPENFILENAME members */

   szFile[0] = '\0';
   szFileTitle[0] = '\0';
   ofn.lStructSize = sizeof(OPENFILENAME) ;
   ofn.hwndOwner = hwndApp ;
   ofn.hInstance = NULL ;
   ofn.lpstrFilter = NULL ;
   ofn.lpstrCustomFilter = (LPSTR) NULL ;
   ofn.nMaxCustFilter = 0L ;
   ofn.lpstrFile= szFile ;
   ofn.nMaxFile = sizeof(szFile) ;
   ofn.lpstrFileTitle = szFileTitle ;
   ofn.nMaxFileTitle = sizeof(szFileTitle) ;

   ofn.nFilterIndex = 1L ;


   ofn.lpstrInitialDir = szDirName ;

   ofn.lpstrTitle = (LPSTR) NULL ;

   ofn.Flags = OFN_SHOWHELP |
               OFN_HIDEREADONLY ;

   ofn.nFileOffset = 0 ;
   ofn.nFileExtension = 0 ;
   ofn.lpstrDefExt = NULL ;

   /*
    * Call the GetOpenFilename function
    */

   SetNormalCursor() ;

   if (GetOpenFileName(&ofn))
   {
#endif
      /*
       * Does the dir exist?
       */

      /*
       * If not, create it.
       */

      /*
       * Change to it
       */

      /*
       * If we're ok then clear the list box, and fill it with
       * all fonts in the system.
       */
      SetWaitCursor() ;
      hlstFiles = GetFontList(NULL ) ;
      SetNormalCursor() ;

      DASSERT( hWDB, hlstFiles ) ;

      /*
       * Put in list box
       */
      if (hlstFiles)
      {
         LPFONTFILE   lpFONT ;
         WORD        w = 0;
         WORD        wCur = -1;

         if (lpFONT = hlsqListFirstNode( hlstFiles ))
            do
            {
               SendMessage( hwndLB, LB_ADDSTRING, 0, (LPARAM)lpFONT ) ;
               w++ ;
            }
            while (lpFONT = hlsqListNextNode( hlstFiles, lpFONT )) ;

         SendMessage( hwndLB, LB_SETCURSEL, (WPARAM)wCur, 0L ) ;
      }

#if 0
   }
   else
   {
      /*
       * Some sort of Commdlg error
       */
      DP1( hWDB, "GetOpenFileName returned false.  CommDlgExtendedError = %08lX",
          CommDlgExtendedError() ) ;

      return FALSE ;
   }
#endif

   return TRUE ;

} /* DoFileNew()  */


/****************************************************************
 *  HLIST NEAR PASCAL GetFontList( HLIST hlst )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
HLIST NEAR PASCAL GetFontList( HLIST hlst )
{
   FONTENUMPROC lpfn ;

   if (hlst || (hlst = hlsqListCreate( hhLists )))
   {
      HDC   hDC ;

      if (hDC = GetDC(hwndApp))
      {
         lpfn = (FONTENUMPROC)MakeProcInstance( (FARPROC)fnGetFontListEnum,
                                                hinstApp ) ;
         EnumFontFamilies( hDC, NULL, lpfn, (LPARAM)(LPVOID)&hlst ) ;
         FreeProcInstance( (FARPROC)lpfn ) ;

         ReleaseDC( hwndApp, hDC ) ;
      }

      return hlst ;
   }
   else
   {
      DP1( hWDB, "hlsqListCreate failed" ) ;
      return NULL ;
   }
} /* GetFontList()  */


int CALLBACK fnGetFontListEnum( LPLOGFONT lpLogFont,
      LPNEWTEXTMETRIC lpTextMetrics, short nFontType, HLIST FAR *lphList ) 
{
   HLIST hlst = *lphList ;

   if (!(nFontType & TRUETYPE_FONTTYPE))
      return 1 ;

   PutFontInList( hlst, lpLogFont, lpTextMetrics, nFontType, NULL ) ;

   return 1 ;

} /* fnGetFontListEnum()  */

/****************************************************************
 *  BOOL NEAR PASCAL PutFontInList( HLIST hlst, LPLOGFONT lpLogFont, LPNEWTEXTMETRIC lpTM, short nFontType, HFONT hFont )
 *
 *  Description: 
 *
 *    Puts the font in the list (not the listbox).
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL PutFontInList( HLIST hlst, LPLOGFONT lpLogFont,
                                LPNEWTEXTMETRIC lpTM, short nFontType,
                                HFONT hFont ) 
{
   char           szBuf[MAX_PATH] ;
   struct stat    Stat ;
   LPFONTFILE      lpFONT ;
   WORD           wOffsetFile ;

   if (lpFONT = (LPFONTFILE)hlsqListAllocNode( hlst, NULL, sizeof(FONTFILE) ))
   {
      /*
       * List item header stuff
       */
      NODEHEADER_dwSize( lpFONT ) = sizeof( FONTFILE ) ;
      NODEHEADER_wMagic( lpFONT ) = FONTFILE_TAG ;
      NODEHEADER_wFlags( lpFONT ) = 0 ;

      /*
       * Generate font file number...
       */
      FONTFILE_dwFontFile(lpFONT)   = dwCurFontFile++ ;

      /*
       * Build fully qualified pathname...
       */
      wOffsetFile = lstrlen( szDirName ) + 1 ;  

      /* FONTFILE_lpszFile(lp)     */
      wsprintf( FONTFILE_lpszFile( lpFONT ), "%s\\FST%05lX.000",
                (LPSTR)szDirName,  dwCurFontFile-1 ) ;


      FONTFILE_wOffsetFile(lpFONT) = wOffsetFile ;

      /* !! HACK !! */
//      lstrcpy( szBuf, lpFONT->lpszFile ) ;
//      stat( szBuf, &Stat ) ;

      FONTFILE_tTimeDate(lpFONT)    = 0;//Stat.st_atime ;
      FONTFILE_dwFileSize(lpFONT)   = 0;//Stat.st_size ;
      FONTFILE_lpszOutBuf(lpFONT)   = NULL ;

      /* FONTFILE_lpszFileName(lp) */

      FONTFILE_dwFamilyPrice(lpFONT)    = 0 ;
      FONTFILE_wFonts(lpFONT)           = 0 ;    /* Calc with enum */

      FONTFILE_dwFamilySize(lpFONT)     = 0 ;

      lstrcpy( FONTFILE_szFamilyName(lpFONT), lpLogFont->lfFaceName ) ;

      FONTFILE_wFlags(lpFONT)       = 0 ;
      FONTFILE_dwReserved(lpFONT)   = 0 ;

      FONTFILE_hbmThumb(lpFONT) = NULL;

      if (!hlsqListInsertSorted( hlst, lpFONT, lpfnFileCompare, 0L))
      {
         DP1( hWDB, "Could not add node in GetDirectoryList" ) ;
         hlsqListFreeNode( hlst, lpFONT ) ;
         return NULL ;
      }
   }
   else
   {
      DP1( hWDB, "Could not allocate node in GetDirectoryList" ) ;
      return NULL ;
   }

} /* PutFontInList()  */


#if 0 // DoFileOpen goes here
         /*
          * Get files in WINDOWS dir.
          */
         hlstFiles = GetDirectoryList( NULL, NULL, NULL ) ;
         DASSERT( hWDB, hlstFiles ) ;

         /*
          * Get files in INIEDIT.INI
          */

         /*
          * Put in list box
          */
         if (hlstFiles)
         {
            LPFONTFILE   lpFONT ;
            WORD        w = 0;
            WORD        wCur = -1;

            if (lpFONT = hlsqListFirstNode( hlstFiles ))
               do
               {
                  SendMessage( hwndLB, LB_ADDSTRING, 0, (LPARAM)lpFONT ) ;
                  w++ ;
               }
               while (lpFONT = hlsqListNextNode( hlstFiles, lpFONT )) ;

            SendMessage( hwndLB, LB_SETCURSEL, (WPARAM)wCur, 0L ) ;
         }
#endif

/****************************************************************
 *  BOOL WINAPI DoChooseFont( void )
 *
 *  Description: 
 *
 *    Pops up the choosefont dialog
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI DoChooseFont( void )
{
   HDC hdc;
   LOGFONT lf;
   CHOOSEFONT chf;

   SetWaitCursor() ;

   hdc = GetDC(hwndApp);
   chf.hDC = CreateCompatibleDC(hdc);
   ReleaseDC(hwndApp, hdc);

   GetObject( hfontCur, sizeof( LOGFONT ), (LPSTR)&lf ) ;

   chf.lStructSize = sizeof( CHOOSEFONT ) ;
   chf.hwndOwner = hwndApp ;
   chf.lpLogFont = &lf ;
   chf.Flags = CF_SCREENFONTS | CF_USESTYLE |
               CF_INITTOLOGFONTSTRUCT | CF_SHOWHELP ;
   chf.rgbColors = 0L ;
   chf.lCustData = 0L ;
   chf.lpfnHook = (FARPROC) NULL ;
   chf.lpTemplateName = (LPSTR) NULL ;
   chf.hInstance = (HANDLE) NULL ;
   chf.iPointSize = 0 ;
   chf.lpszStyle = (LPSTR)szFaceStyle ;
   chf.nFontType = SCREEN_FONTTYPE ;
   chf.nSizeMin = 0 ;
   chf.nSizeMax = 0 ;

   SetNormalCursor() ;

   if (ChooseFont(&chf))
   {
      char szBuf[64] ;

      SetWaitCursor() ;

      DeleteObject( hfontCur ) ;

      lstrcpy( szFaceName, lf.lfFaceName ) ;

      hfontCur = CreateFontIndirect( &lf ) ;

      /*
       * Now write it to the INI file
       */
      wsprintf( szBuf, rglpsz[IDS_INI_FONT_SPEC],
                chf.iPointSize / 10,
                (LPSTR)szFaceName,
                (LPSTR)szFaceStyle ) ;

      DP3( hWDB, "Writing profile : %s", (LPSTR)szBuf ) ;

      WritePrivateProfileString( rglpsz[IDS_INI_MAIN],
                                 rglpsz[IDS_INI_FONT],
                                 szBuf,
                                 rglpsz[IDS_INI_FILENAME] ) ;
      

      DeleteDC( chf.hDC ) ;

      SetNormalCursor() ;
      return TRUE ;
   }
   else
   {
      DeleteDC( chf.hDC ) ;
      return FALSE ;
   }

   return TRUE ;


} /* DoChooseFont()  */

BOOL CALLBACK fnAbout( HWND hDlg, WORD wMsg, WPARAM wParam, LPARAM lParam) ;

BOOL WINAPI DoAboutBox( VOID )
{
   DLGPROC  lpfnDlgProc ;

   (FARPROC)lpfnDlgProc = MakeProcInstance( (FARPROC)fnAbout, hinstApp ) ;
   DialogBox( hinstApp,
              MAKEINTRESOURCE( DLG_ABOUTBOX ),
              hwndApp,
              lpfnDlgProc ) ;
   FreeProcInstance( (FARPROC)lpfnDlgProc ) ;

   return TRUE ;
}

BOOL CALLBACK fnAbout( HWND hDlg, WORD wMsg, WPARAM wParam, LPARAM lParam ) 
{
   char  szTemp [256] ;

   switch (wMsg)
   {
      case WM_INITDIALOG:

         wsprintf( szTemp, "%s %s - %s",
                   (LPSTR)rglpsz[IDS_VERSION], (LPSTR)szVerNum,
                   (LPSTR)__DATE__ ) ;
         SetDlgItemText (hDlg, IDD_VERSION, szTemp ) ;
         break ;

      case WM_COMMAND:
         switch ((WORD)wParam)
         {
            case IDOK:
            case IDCANCEL:
               EndDialog (hDlg, 0) ;
            break ;

            default :
               return FALSE ;
         }
         break ;

   }
   return FALSE ;
}


/************************************************************************
 * End of File: subdlgs.c
 ***********************************************************************/

