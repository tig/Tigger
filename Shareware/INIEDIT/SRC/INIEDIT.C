//  iniedit.c
//
//
//  INIedit
//
//  Windows 3.0 INI editor
//
//  Copyright (c) 1990 Charles E. Kindel, Jr. 
//
//  Author:  Charles E. Kindel, Jr.
//
//---------------------------------------------------------------------------
//  REVISIONS
//
//    Version  Date     Author         Comments
//    1.00.01  7/4/90   CEK            First version
//    1.00.20  8/12/90  CEK            New design
//    1.00.21  8/20/90  CEK            Full features
//    1.00.30  8/22/90  CEK            WinExec Notepad
//                                     Printing
//    1.00.31  8/26/90  CEK            UI issues
//                                     New icon and about box
//    1.00.32  8/27/90  CEK            move window to center
//    1.00.33  10/10/90 CEK            Allow only one instance
//                                     OpenFileDialog
//    1.00.34  10/29/90 CEK            Select Printer Dialog box
//    1.00.35  10/31/90 CEK            CEKLIB.DLL
//    1.00.36  11/1/90  CEK            More CEKLIB and new version
//                                     numbering (using BumpVer).
//                                     AboutBox and BigAboutBox.
//    1.00.37  11/20/90 CEK            Nuked "File.Save"
//                                     Check for SYSTEM.INI, Do not let
//                                     user edit [386Enh] section.
//                                     Changed Makefile to use a linkfile
//    1.00.045 11/20/90 CEK            Version number now has 3 build
//                                     digits.
//
//    1.00.100 11/27/90 CEK            Implementation of shareware
//                                     registration system.
//
//---------------------------------------------------------------------------
// DO TO LIST
//
//    Move printing to CEKLIB
//       fix problems with deskjet
//       add header and footer (page number and filename)
//    Implement Help
//    Implement Clipboard
//    Implement Undo
//    Remove Save from file menu
//    Implement FileOpen in CEKLIB and remove WINCOM
//    Allow icon to open AboutBox
//    Figure out some way to unobtusively tell user he can't edit 386Enh
//       section of SYSTEM.INI.  
//       Dont even bother to fill list box (it looks shitty).
//       Or maybe just check for multiples and only show one.
//          Use this instead of checking for 386Enh and SYSTEM.INI.
//          This is more flexible.
//
//    If user currently has notepad open with current file then don't 
//       allow edit?  
//       Add a "refresh" button?
//
//---------------------------------------------------------------------------
// KNOWN BUGS
//    Severity    Description
//       1        Something in the reading of a file can hang system.
//                Usually happens with FileOpen, but can happen at other
//                times.
//       3        Do not allow user to "add" a section or name that
//                already exists.
//
//---------------------------------------------------------------------------

#include <windows.h>
#include <ceklib.h>
#include "..\inc\iniedit.h"
#include "..\inc\inieditd.h"
#include "..\inc\ini_ver.h"    // for bumpver
#include "..\inc\ini_init.h"

char     szAppName [] = "INIedit" ;
char     szVersion [32] ;
HANDLE   hInst ;

void NEAR PASCAL WritePrivateProfileInt(LPSTR szSection, LPSTR szKey, int i, LPSTR szIniFile) ;
void NEAR PASCAL PlaceWindow( HWND hWnd ) ;
#define POS_STR      "Window Pos"
#define HEIGHT_STR   "Height"
#define WIDTH_STR    "Width"
#define X_STR        "X"
#define Y_STR        "Y"

int PASCAL WinMain ( HANDLE   hInstance,
                     HANDLE   hPrevInstance,
                     LPSTR    lpszCmdLine,
                     int      nCmdShow )
{
   HWND     hWnd ;
   HWND     hWndLast ;
   MSG      msg ;
   HACCEL   haccl ;

   D( dp( "------------- INIEDIT START" )) ;
   if (hPrevInstance)
   {
      GetInstanceData( hPrevInstance, (PSTR) &hWnd, 2 ) ;
      hWndLast = GetLastActivePopup( hWnd ) ;
      SetWindowPos( hWnd, 0, 0, 0, 0, 0,
                           SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW ) ;
      if (IsIconic( hWnd ))
         ShowWindow( hWnd, SW_RESTORE ) ;
      else
         SetActiveWindow( hWndLast ) ;
      return FALSE ;
    }

   if (!InitTheClass (hInstance))
      return FALSE ;

   hInst = hInstance ;

   VerMakeVersionNumber( szVersion, VER_MAJOR, VER_MINOR, VER_BUILD ) ;

   D(dp( "INIedit Version %s", (LPSTR)szVersion )) ;

   if (!(hWnd = CreateDialog (hInstance, szAppName, 0, NULL)))
      MessageBox ( NULL, "Not enough memory!", NULL,
                     MB_ICONHAND | MB_SYSTEMMODAL ) ;

   SendMessage ( hWnd, WM_MYINITDIALOG, 0, 0L ) ;

   #if 0
   GetWindowRect( hWnd, &rect ) ;
   
   MoveWindow( hWnd,
               (GetSystemMetrics( SM_CXSCREEN ) - (rect.right - rect.left)) / 2,
               (GetSystemMetrics( SM_CYSCREEN ) - (rect.bottom - rect.top)) / 2,
               rect.right-rect.left,
               rect.bottom-rect.top,
               FALSE ) ;

   #endif

   PlaceWindow( hWnd ) ;

   ShowWindow( hWnd, nCmdShow ) ;

   haccl = LoadAccelerators( hInstance, MAKEINTRESOURCE( 1 ) ) ;

   while (GetMessage (&msg, NULL, 0, 0))
   {

      if (TranslateAccelerator( hWnd, haccl, &msg ))
         DispatchMessage( &msg ) ;
      else
         if (!IsDialogMessage( hWnd, &msg ))  
         {
            TranslateMessage( &msg ) ;
            DispatchMessage( &msg ) ;
         }

   }
   D( dp( "------------- INIEDIT NORMAL EXIT" )) ;
   
   return msg.wParam ;
}



/****************************************************************
 *  void NEAR PASCAL PlaceWindow( HWND hWnd )
 *
 *  Description: 
 *
 *    Places the window based on the private INI file.
 *
 *  Comments:
 *
 ****************************************************************/
void NEAR PASCAL PlaceWindow( HWND hWnd )
{
   short x, y ;
   short w, h ;
   char  szFileName[14] ;

   wsprintf( szFileName, "%s.INI", (LPSTR)szAppName) ;

   w = GetPrivateProfileInt( POS_STR, WIDTH_STR, 0, szFileName ) ;

   //
   // return if there is nothing saved.
   //
   if (!w)
      return ;

   h = GetPrivateProfileInt( POS_STR, HEIGHT_STR, 0, szFileName ) ;

   x = GetPrivateProfileInt( POS_STR, X_STR, 0, szFileName ) ;
   y = GetPrivateProfileInt( POS_STR, Y_STR, 0, szFileName ) ;

   SetWindowPos( hWnd, NULL, x, y, w, h, SWP_NOZORDER ) ;

} /* PlaceWindow()  */

/****************************************************************
 *  void NEAR PASCAL WritePrivateProfileInt()
 *
 *  Description: 
 *
 *    write a integer to a INI file
 *
 *  Comments:
 *
 ****************************************************************/

void NEAR PASCAL WritePrivateProfileInt(LPSTR szSection, LPSTR szKey, int i, LPSTR szIniFile)
{
    char     ach[32] ;

    if (i != (int)GetPrivateProfileInt(szSection, szKey, ~i, szIniFile))
    {
        wsprintf(ach, "%d", i);
        WritePrivateProfileString(szSection, szKey, ach, szIniFile);
    }
}

/****************************************************************
 *  void NEAR PASCAL SaveWindowPos( HWND hWnd )
 *
 *  Description: 
 *
 *    Saves the window position.
 *
 *  Comments:
 *
 ****************************************************************/
void FAR PASCAL SaveWindowPos( HWND hWnd )
{
   RECT     rc ;
   char  szFileName[14] ;

   wsprintf( szFileName, "%s.INI", (LPSTR)szAppName) ;

   if (IsIconic( hWnd ))
      return ;

   GetWindowRect( hWnd, &rc ) ;

   WritePrivateProfileInt(POS_STR, X_STR, rc.left, szFileName);
   WritePrivateProfileInt(POS_STR, Y_STR, rc.top, szFileName);
   WritePrivateProfileInt(POS_STR, WIDTH_STR,  rc.right-rc.left, szFileName);
   WritePrivateProfileInt(POS_STR, HEIGHT_STR, rc.bottom-rc.top, szFileName);

} /* SaveWindowPos()  */





