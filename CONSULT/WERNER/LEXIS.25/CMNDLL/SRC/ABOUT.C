/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CMNDLL, Common DLL for Werner Apps
 *
 *      Module:  about.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/


#define WINDLL
#include "..\inc\undef.h"
#include <windows.h>
#include "..\inc\cmndll.h"
// Include the dialog defines
#include "..\inc\cmndlld.h"

//----------------------------------------------------------------
// Property list strings
//----------------------------------------------------------------
#define ICON_PROP "icon"


//----------------------------------------------------------------
// Globals
//----------------------------------------------------------------
extern HANDLE hInst ;

//-----------------------------------------------------------------
// 
//
//   BOOL FAR PASCAL
//   AboutDialog( HWND    hwndParent,
//               LPSTR   lpszTitle,
//               LPSTR   lpszAppName,
//               LPSTR   lpszVersion,
//               LPSTR   lpszDescription,
//               WORD    wFlags ) ;
//
// Description:
//
//       This function opens a standard About Box Dialog Box.  The 
//       format of the dialog box is like this:
//       
//       +------------------- lpszTitle ----------------------+
//       |                   lpszAppName                      |
//       |               Version lpszVersion                  |
//       |                                                    |
//       |                   Icon with                        |
//       |               res name of lpszAppName              |
//       |                                                    |
//       |                  lpszDescritpion                   |
//       |                                                    |
//       |                                                    |
//       |      Copyright (c) 1990 Charles E. Kindel, Jr.     |
//       |                                                    |
//       |                     | OK |                         |
//       +----------------------------------------------------+
//                          
//       
// Type/Parameter
//          Description
// 
// HWND hwndParent
//          Specifies the window to be used as the parent of the dialog box.
//          This must be a valid window handle.
// 
// LPSTR lpszTitle
//          Specifies the title of the dialog box.  May be NULL if no title
//          is desired.  This string is not altered.
// 
// LPSTR lpszAppName
//          Specifies the name of the application.  When wFlags is 
//          ABOUT_REGISTERED the lpszAppName parameter will be used
//          as the INI file name.
//
// LPSTR lpszDescription
//          A string, less than 256 chars, that describes the product.
//          Should include a shareware price.
//          if ABOUT_REGISTERED is use this string should be NULL.
//       .
//       .
//
// WORD wFlags
//          Specifies flags, and may be any of the following.
// 
//          Value
//                Meaning
// 
//          ABOUT_STANDARD
//                You get the above dialog box.
//      
// Return Value
//          Returns one of the following values:
// 
//             Value
//                      Meaning
// 
//             DLG_CANCEL
//                      The user pressed the Cabcel button.
// 
//             DLG_OK
//                      The user pressed the OK button.  
// 
//             DLG_ERROR
//                      An error occured. 
//
// Comments
//    Will add more styles later.
// 
// Modifications
//    CEK   10/31/90    First version.
//    CEK   11/20/90    Shareware stuff.
//
//-----------------------------------------------------------------

typedef struct tagABOUTSTRUCT
{
   char  szTitle[64] ;
   char  szAppName[64] ;
   char  szVersion[23] ;
   char  szDescription[256] ;
   WORD  wFlags ;
} ABOUTSTRUCT, *PABOUTSTRUCT ;


BOOL FAR PASCAL
AboutDialog( HWND    hwndParent,
             LPSTR   lpszTitle,
             LPSTR   lpszAppName,
             LPSTR   lpszVersion,
             LPSTR   lpszDescription,
             WORD    wFlags )
{
   FARPROC      lpfnDlgProc ;
   PABOUTSTRUCT pAS ;
   LOCALHANDLE  hlmemAS ;
   char         szResourceName[16] ;
   int          n ;

   if (!(hlmemAS = LocalAlloc( LHND, sizeof( ABOUTSTRUCT ) )))
      return DLG_ERROR ;

   pAS = (PABOUTSTRUCT)LocalLock( hlmemAS ) ;

   // Setup struct to pass to dlg box.
   lstrcpy( pAS->szTitle, lpszTitle ) ;
   lstrcpy( pAS->szAppName, lpszAppName ) ;
   lstrcpy( pAS->szVersion, lpszVersion ) ;

   lstrcpy( pAS->szDescription, lpszDescription ) ;
   pAS->wFlags = wFlags ;

   LocalUnlock( hlmemAS ) ;

   if (lpfnDlgProc = MakeProcInstance( AboutDlgProc, hInst ))
   {
      switch( wFlags )
      {
         case ABOUT_STANDARD:
            // Fall through

         default:
            lstrcpy( szResourceName, "ABOUTBOX" ) ;
      }

      n = DialogBoxParam(  hInst,		
                           szResourceName,
                           hwndParent,			 
                           lpfnDlgProc,
                           (LONG)hlmemAS ) ;

      FreeProcInstance( lpfnDlgProc ) ;
   }
   else
      return DLG_ERROR ;

   LocalFree( hlmemAS ) ;

   return n ;
}


//-----------------------------------------------------------------
// About dlgproc
//
// 11/19/90 CEK   - Property lists
//-----------------------------------------------------------------
BOOL FAR PASCAL 
AboutDlgProc( HWND hDlg,
              WORD wMsg,
              WORD wParam,
              LONG lParam ) 
{
   HICON        hIcon ;

   // Get hIcon from property list if it's there
   hIcon = GetProp( hDlg, ICON_PROP ) ;

   switch (wMsg)
   {
      case WM_INITDIALOG :
      {
         PABOUTSTRUCT   pAS ;
         char           szBuf [64] ;
         char           szBuf2[64] ;
         HANDLE         hInstance ;
 
         DPS5( hWDB,  "AboutDlgProc: WM_INITDIALOG" ) ;
         if (pAS = (PABOUTSTRUCT)LocalLock( (LOCALHANDLE)lParam ))
         {
            SetWindowText( hDlg, pAS->szTitle ) ;
            //SetDlgItemText( hDlg, IDD_APPNAME, pAS->szTitle ) ;

            wsprintf( szBuf, (LPSTR)"Version %s",
                           (LPSTR)pAS->szVersion ) ;
            SetDlgItemText( hDlg, IDD_VERSION, szBuf ) ;

            SetDlgItemText( hDlg, IDD_DESCRIPTION, pAS->szDescription ) ;

            wsprintf( szBuf, (LPSTR)"CMNDLL V%s",
                           (LPSTR)VerGetCMNDLLVersion( szBuf2 ) ) ;
            SetDlgItemText( hDlg, IDD_DLLVERSION, szBuf ) ;

            hInstance = GetWindowWord( GetParent(hDlg), GWW_HINSTANCE ) ;
            hIcon = LoadIcon( hInstance, pAS->szAppName ) ;

            // Store hIcon in the property list for the window
            if (!SetProp( hDlg, ICON_PROP, hIcon ))
            {
               DPS3( hWDB,  "Could not SetProp(hIcon)" ) ;
               EndDialog( hDlg, DLG_ERROR ) ;
               return FALSE ;
            }

            LocalUnlock( (LOCALHANDLE)lParam ) ;
         }
         else
         {
            EndDialog( hDlg, DLG_ERROR ) ;
            return FALSE ;
         }

         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;

      }
      return ShadowControl( hDlg, wMsg, wParam, RGBLTGRAY ) ;

      case WM_COMMAND :
         if (wParam == IDOK || wParam == IDCANCEL)
         {
            // Remove the property from the icon list
            RemoveProp( hDlg, ICON_PROP ) ;
            EndDialog ( hDlg, DLG_ERROR ) ;
            return TRUE ;
         }
      break ;

      case WM_DESTROY:
         return ShadowControl( hDlg, wMsg, wParam, lParam ) ;

      case WM_PAINT :
      {
         RECT        rc ;
         PAINTSTRUCT ps ;
         TEXTMETRIC  tm ;

         if (BeginPaint(hDlg, &ps ))
         {
            GetTextMetrics( ps.hdc, &tm ) ;
            GetClientRect( hDlg, &rc ) ;
            DrawIcon( ps.hdc,
                     ((rc.right - rc.left) - GetSystemMetrics( SM_CXICON )) / 2,
                     tm.tmHeight - (tm.tmHeight / 3),
                     hIcon ) ;

            EndPaint( hDlg, &ps ) ;
         }
      }
      //
      // Fall through
      //
      default:
         return ShadowControl( hDlg, wMsg, wParam, lParam ) ;
   }

   return FALSE ;
}
/************************************************************************
 * End of File: about.c
 ************************************************************************/


