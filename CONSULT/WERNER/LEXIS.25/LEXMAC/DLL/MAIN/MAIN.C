// Main.c
//
// Main box routines for LEXMAC.DLL
//
// Copyright (C) 1990 Werner Associates
// All Rights Reserved.
// 
#define WINDLL
#include <windows.h>
#include <stdlib.h>
#include <ctype.h>
#include "..\lexmac.h"


// strips ending whitspace from a string
void _strstrp( LPSTR lpszStr ) ;
BOOL Validate( HWND hDlg ) ;

//----------------------------------------------------------------
// Property list strings
//----------------------------------------------------------------
#define PROP "handle"

#define MAX_CITESETTINGS 4

//----------------------------------------------------------------
// Globals
//----------------------------------------------------------------
extern HANDLE hInst ;

//-----------------------------------------------------------------
//
//   BOOL FAR PASCAL
//   SelectMacroDialog( HWND    hwndParent,
//               LPSTR   lpszMacro,
//               LPSTR   lpszSelection,
//               LPSTR   lpszClient,
//               int     nPages,
//               int     nTimeOut ) ;
//-----------------------------------------------------------------

typedef struct tagMAINSTRUCT
{
   char  szMacro[64] ;
   char  szSelection[64] ;
   char  szClient[64] ;
   int   nPages ;
   int   nTimeOut ;
} MAINSTRUCT, *PMAINSTRUCT ;

char         szTemp[64] ;

BOOL FAR PASCAL
SelectMacroDialog(LPSTR   lpszMacro,
                  LPSTR   lpszSelection,
                  LPSTR   lpszClient,
                  LPSTR   lpszPages,
                  LPSTR   lpszTimeOut ) 
               
{
   FARPROC      lpfnDlgProc ;
   PMAINSTRUCT  pAM ;
   LOCALHANDLE  hlmemAM ;
   int          n ;
   HWND         hwndParent ;

   hwndParent = GetActiveWindow() ;
   D(dp( "hwndParent = %d", hwndParent )) ;

   if (!(hlmemAM = LocalAlloc( LHND, sizeof( MAINSTRUCT ) )))
      return DLG_ERROR ;

   pAM = (PMAINSTRUCT)LocalLock( hlmemAM ) ;

   D(dp( "In:" )) ;
   D(dp( "    lpszMacro = \"%s\"", (LPSTR)lpszMacro )) ;
   D(dp( "    lpszSelection = \"%s\"", (LPSTR)lpszSelection )) ;
   D(dp( "    lpszClient = \"%s\"", (LPSTR)lpszClient )) ;
   D(dp( "    lpszPages = \"%s\"", (LPSTR)lpszPages )) ;
   D(dp( "    lpszTimeOut = \"%s\"", (LPSTR)lpszTimeOut )) ;

   // Strip white space off end of all strings
   _strstrp( lpszMacro ) ;
   _strstrp( lpszSelection ) ;
   _strstrp( lpszClient ) ;
   _strstrp( lpszPages ) ;
   _strstrp( lpszTimeOut ) ;

   // Setup struct to pAMs to dlg box.
   lstrcpy( pAM->szMacro, lpszMacro ) ;
   lstrcpy( pAM->szSelection, lpszSelection ) ;
   lstrcpy( pAM->szClient, lpszClient ) ;
   lstrcpy( szTemp, lpszPages ) ;
   pAM->nPages = atoi( szTemp ) ;
   lstrcpy( szTemp, lpszTimeOut ) ;
   pAM->nTimeOut = atoi( szTemp ) ;

   LocalUnlock( hlmemAM ) ;

   lpfnDlgProc = MakeProcInstance( MainDlgProc, hInst ) ;

   n = DialogBoxParam(  hInst,		
                        "MAINDLG",
                        hwndParent,			 
                        lpfnDlgProc,
                        (LONG)hlmemAM ) ;

   FreeProcInstance( lpfnDlgProc ) ;

   if (n)
   {
      pAM = (PMAINSTRUCT)LocalLock( hlmemAM ) ;

      lstrcpy( lpszMacro, pAM->szMacro ) ;
      lstrcpy( lpszSelection, pAM->szSelection ) ;
      lstrcpy( lpszClient, pAM->szClient ) ;
      wsprintf( lpszPages, (LPSTR)"%d", pAM->nPages ) ;
      wsprintf( lpszTimeOut, (LPSTR)"%d", pAM->nTimeOut ) ;

      LocalUnlock( hlmemAM ) ;
   }

   LocalFree( hlmemAM ) ;

   D(dp( "Out:" )) ;
   D(dp( "    lpszMacro = \"%s\"", (LPSTR)lpszMacro )) ;
   D(dp( "    lpszSelection = \"%s\"", (LPSTR)lpszSelection )) ;
   D(dp( "    lpszClient = \"%s\"", (LPSTR)lpszClient )) ;
   D(dp( "    lpszPages = \"%s\"", (LPSTR)lpszPages )) ;
   D(dp( "    lpszTimeOut = \"%s\"", (LPSTR)lpszTimeOut )) ;

   return n ;
}


//-----------------------------------------------------------------
// Main dlgproc
//
//-----------------------------------------------------------------
BOOL FAR PASCAL 
MainDlgProc( HWND hDlg,
              unsigned iMessage,
              WORD wParam,
              LONG lParam ) 
{
   PMAINSTRUCT   pAM ;
   LOCALHANDLE   hlmemPM ;

   hlmemPM = GetProp( hDlg, PROP ) ;

   switch (iMessage)
   {
      case WM_INITDIALOG :
      {
         D(dp( "MainDlgProc: WM_INITDIALOG" )) ;

         pAM = (PMAINSTRUCT)LocalLock( hlmemPM = (LOCALHANDLE)lParam ) ;
         SetProp( hDlg, PROP, hlmemPM ) ;

         D(dp( "pAM->szSelection == \"%s\"", (LPSTR)pAM->szSelection )) ;
         SetDlgItemText( hDlg, IDD_SELECTION, pAM->szSelection ) ;
         SetDlgItemText( hDlg, IDD_CLIENT, pAM->szClient ) ;
         SetDlgItemInt( hDlg, IDD_PAGES, pAM->nPages, 0 ) ;
         SetDlgItemInt( hDlg, IDD_TIMEOUT, pAM->nTimeOut, 0 ) ;

         LocalUnlock( (LOCALHANDLE)lParam ) ;
      }
      break ;

      case WM_COMMAND :
         switch (wParam)
         {
            case IDD_LEXSEE:
               if (Validate( hDlg ))
               {
                  pAM = (PMAINSTRUCT)LocalLock( hlmemPM ) ;

                  lstrcpy( pAM->szMacro, "LEXSEE" ) ;
                  goto Done ;
               }
            break ;

            case IDD_LEXSTAT:
               if (Validate( hDlg ))
               {
                  pAM = (PMAINSTRUCT)LocalLock( hlmemPM ) ;

                  lstrcpy( pAM->szMacro, "LEXSTAT" ) ;
                  goto Done ;
               }
            break ;

            case IDD_AUTOCITE:
               if (Validate( hDlg ))
               {
                  pAM = (PMAINSTRUCT)LocalLock( hlmemPM ) ;

                  lstrcpy( pAM->szMacro, "Auto-Cite" ) ;
                  goto Done ;
               }
            break ;

            case IDD_SHEPARDS:
               if (Validate( hDlg ))
               {
                  pAM = (PMAINSTRUCT)LocalLock( hlmemPM ) ;

                  lstrcpy( pAM->szMacro, "Shepard's" ) ;
                  goto Done ;
               }
            break ;

Done: 
               GetDlgItemText( hDlg, IDD_SELECTION, pAM->szSelection, 64 ) ;
               GetDlgItemText( hDlg, IDD_CLIENT, pAM->szClient, 64 ) ;

               pAM->nPages = (int)GetDlgItemInt( hDlg, IDD_PAGES, NULL, 0 ) ;
               D(dp( "pAM->nPages = %d", pAM->nPages )) ;

               pAM->nTimeOut = (int)GetDlgItemInt( hDlg, IDD_TIMEOUT, NULL, 0 ) ;
               D(dp( "pAM->nTimeOut = %d", pAM->nTimeOut )) ;

               LocalUnlock( hlmemPM ) ;
               RemoveProp( hDlg, PROP ) ;

               EndDialog (hDlg, 1) ;
               break ;

            case IDD_ABOUT:
               AboutDialog( ) ;
            break;

            case IDCANCEL:
               RemoveProp( hDlg, PROP ) ;
               EndDialog( hDlg, 0 ) ;
            break ;

            case IDD_LOGOFF:
               RemoveProp( hDlg, PROP ) ;
               EndDialog( hDlg, 2 ) ;
            break ;
         }
         break ;

      case WM_PAINT :
      {
         PAINTSTRUCT ps ;
         TEXTMETRIC  tm ;
         HDC         hDC ;

         hDC = BeginPaint(hDlg, &ps ) ;
         GetTextMetrics( hDC, &tm ) ;
         DrawShadow( hDC, GetDlgItem( hDlg, IDD_BOX0 ),
                     GetStockObject( GRAY_BRUSH ), tm.tmAveCharWidth / 2 ) ;
         DrawShadow( hDC, GetDlgItem( hDlg, IDD_BOX1 ),
                     GetStockObject( GRAY_BRUSH ), tm.tmAveCharWidth / 2 ) ;
         EndPaint( hDlg, &ps ) ;
         return FALSE ;
      }

      default:
         return FALSE ;
   }

   return TRUE ;
}


BOOL Validate( HWND hDlg )
{
   char  szSelection[64] ;
   char  szClient[64] ;
   short nPages ;
   short nTimeOut ;

   GetDlgItemText( hDlg, IDD_SELECTION,szSelection, 64 ) ;
   if (!lstrlen( szSelection ))
   {
      MessageBeep( 0 ) ;
      MessageBox( hDlg,
         "You must specifiy a cite.", "Error", MB_ICONEXCLAMATION ) ;
      SetFocus( GetDlgItem( hDlg, IDD_SELECTION ) ) ;
      return FALSE ;
   }

   GetDlgItemText( hDlg, IDD_CLIENT, szClient, 64 ) ;
   if (!lstrlen( szClient ))
   {
      MessageBeep( 0 ) ;
      MessageBox( hDlg,
         "You must specifiy a client.", "Error", MB_ICONEXCLAMATION ) ;
      SetFocus( GetDlgItem( hDlg, IDD_CLIENT ) ) ;
      return FALSE ;
   }

   nPages = (int)GetDlgItemInt( hDlg, IDD_PAGES, NULL, 0 ) ;
   if (nPages < 1)
   {
      MessageBeep( 0 ) ;
      wsprintf( szClient,
      (LPSTR)"\"%d\" is an invalid number of pages.\nMust be greater than 0.",
                nPages ) ;
      MessageBox( hDlg,
                  szClient, "Error", MB_ICONEXCLAMATION ) ;
      SetFocus( GetDlgItem( hDlg, IDD_PAGES ) ) ;
      return FALSE ;
   }

   nTimeOut = (int)GetDlgItemInt( hDlg, IDD_TIMEOUT, NULL, 0 ) ;
   if (nTimeOut < 5 || nTimeOut > 30)
   {
      MessageBeep( 0 ) ;
      wsprintf( szClient,
      (LPSTR)"\"%d\" is an invalid time out value.\nMust be between 5 and 30.",
                nTimeOut ) ;
      MessageBox( hDlg,
                  szClient, "Error", MB_ICONEXCLAMATION ) ;
      SetFocus( GetDlgItem( hDlg, IDD_TIMEOUT ) ) ;
      return FALSE ;
   }
   
   return TRUE ;
}


void _strstrp( LPSTR lpszStr )
{
   LPSTR lpsCur ;
   short n ;

   n = lstrlen( lpszStr ) ;

   if (n)
   {
      // Strip all control chars.
      for( lpsCur = lpszStr ; *lpsCur != '\0' ; lpsCur++ )
         if (iscntrl(*lpsCur))
            *lpsCur = ' ' ;

      // Strip all trailing blanks
      for (lpsCur = lpszStr + n - 1;
            (lpsCur >= lpszStr) && isspace( *lpsCur ) ;
            lpsCur-- )
        *lpsCur = '\0' ;

      // Strip all leading blanks
      lpsCur = lpszStr ;
      while (*lpsCur == ' ')
         lstrcpy( lpszStr, lpsCur + 1 ) ;
   }


   
}
