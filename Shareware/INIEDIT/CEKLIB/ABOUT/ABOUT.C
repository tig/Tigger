// about.c
//
// About box routines for CEKLIB.DLL
//
// Copyright (C) 1990 Charles E. Kindel, Jr.
// 
// Modification history:
// 10/20/90 CEK   First version
// 11/19/90 CEK   Made use of window property lists.
//
#define WINDLL
#include "..\inc\undef.h"
#include <windows.h>
#include "..\inc\ceklib.h"

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
//          ABOUT_BIG
//                Shareware dialog box.
//
//          ABOUT_REGISTERED
//                User is registered.  His name and registration number
//                appear in the dialog box.
//                szDescription is not used.
//
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

   D(dp( "AboutDialog..." )) ;
   if (!(hlmemAS = LocalAlloc( LHND, sizeof( ABOUTSTRUCT ) )))
      return DLG_ERROR ;

   D(dp( "Memory allocated ok...") ) ;
   pAS = (PABOUTSTRUCT)LocalLock( hlmemAS ) ;

   // Setup struct to pass to dlg box.
   lstrcpy( pAS->szTitle, lpszTitle ) ;
   lstrcpy( pAS->szAppName, lpszAppName ) ;
   lstrcpy( pAS->szVersion, lpszVersion ) ;
   if (wFlags != ABOUT_REGISTERED)
      lstrcpy( pAS->szDescription, lpszDescription ) ;
   pAS->wFlags = wFlags ;

   LocalUnlock( hlmemAS ) ;

   lpfnDlgProc = MakeProcInstance( AboutDlgProc, hInst ) ;

   switch( wFlags )
   {
      case ABOUT_BIG:   // shareware
         lstrcpy( szResourceName, "BIGABOUTBOX" ) ;
         break ;

      case ABOUT_REGISTERED:
         lstrcpy( szResourceName, "REGABOUTBOX" ) ;
         break ;

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
              unsigned iMessage,
              WORD wParam,
              LONG lParam ) 
{
   HICON        hIcon ;

   // Get hIcon from property list if it's there
   hIcon = GetProp( hDlg, ICON_PROP ) ;

   switch (iMessage)
   {
      case WM_INITDIALOG :
      {
         PABOUTSTRUCT   pAS ;
         char           szBuf [64] ;
         char           szBuf2[64] ;
         HANDLE         hInstance ;

         D(dp( "AboutDlgProc: WM_INITDIALOG" )) ;
         pAS = (PABOUTSTRUCT)LocalLock( (LOCALHANDLE)lParam ) ;

         SetWindowText( hDlg, pAS->szTitle ) ;
         SetDlgItemText( hDlg, IDD_APPNAME, pAS->szAppName ) ;

         wsprintf( szBuf, (LPSTR)"Version %s",
                          (LPSTR)pAS->szVersion ) ;
         SetDlgItemText( hDlg, IDD_VERSION, szBuf ) ;

         if (pAS->wFlags != ABOUT_REGISTERED)
            SetDlgItemText( hDlg, IDD_DESCRIPTION, pAS->szDescription ) ;

         wsprintf( szBuf, (LPSTR)"ceklib v%s",
                          (LPSTR)VerGetDLLVersion( szBuf2 ) ) ;
         SetDlgItemText( hDlg, IDD_DLLVERSION, szBuf ) ;

         hInstance = GetWindowWord( GetParent(hDlg), GWW_HINSTANCE ) ;
         hIcon = LoadIcon( hInstance, pAS->szAppName ) ;

         // Store hIcon in the property list for the window
         if (!SetProp( hDlg, ICON_PROP, hIcon ))
         {
            D(dp( "Could not SetProp(hIcon)" )) ;
            return FALSE ;
         }

         if (pAS->wFlags == ABOUT_REGISTERED)
         {
            char szName[REG_MAX_NAME+1] ;
            char szNum[REG_MAX_NUMBER+1] ;

            IsRegisteredUser( pAS->szAppName, szName, szNum, REGISTER_READ ) ;
            SetDlgItemText( hDlg, IDD_NAME, szName ) ;
            SetDlgItemText( hDlg, IDD_NUM, szNum ) ;
         }

         LocalUnlock( (LOCALHANDLE)lParam ) ;
         return TRUE ;
      }

      case WM_COMMAND :
         if (wParam == IDOK || wParam == IDCANCEL)
         {
            // Remove the property from the icon list
            RemoveProp( hDlg, ICON_PROP ) ;
            EndDialog (hDlg, TRUE) ;
            return TRUE ;
         }
         break ;

      case WM_PAINT :
      {
         RECT        Rect ;
         PAINTSTRUCT ps ;
         TEXTMETRIC  tm ;
         HDC         hDC ;

         hDC = BeginPaint(hDlg, &ps ) ;
         GetTextMetrics( hDC, &tm ) ;
         DrawShadow( hDC, GetDlgItem( hDlg, IDD_BOX_0 ),
                     GetStockObject( GRAY_BRUSH ), tm.tmAveCharWidth / 2 ) ;
         DrawShadow( hDC, GetDlgItem( hDlg, IDD_BOX_1 ),
                     GetStockObject( GRAY_BRUSH ), tm.tmAveCharWidth / 2 ) ;
         GetWindowRect( GetDlgItem( hDlg, IDD_BOX_0 ), &Rect ) ;
         DrawIcon( hDC,
                   tm.tmAveCharWidth * 3,
                   (tm.tmHeight / 2) * 3,
                   hIcon ) ;
         EndPaint( hDlg, &ps ) ;
         return FALSE ;
      }
   }

   return FALSE ;
}

