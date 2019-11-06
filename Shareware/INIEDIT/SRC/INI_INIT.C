// ini_init.c
//
// Initialization for INIedit
//
// Copyright 1990, CE Kindel
//
#include <windows.h>
#include <stdio.h>
#include <ceklib.h>
#include "..\inc\iniedit.h"
#include "..\inc\ini_ver.h"
#include "..\inc\ini_dlg.h"

HCURSOR  hHourGlass ;
extern char szAppName [] ;
extern char szVersion [] ;

BOOL InitTheClass ( HANDLE hInstance )
{
   HANDLE      hMemory ;
   PWNDCLASS   pWndClass ;
   BOOL        bSuccess ;

   hMemory = LocalAlloc (LPTR, sizeof(WNDCLASS)) ;
   pWndClass = (PWNDCLASS) LocalLock (hMemory) ;

   pWndClass->style        = CS_HREDRAW | CS_VREDRAW ;
   pWndClass->lpfnWndProc  = MainDlgProc ;
   pWndClass->cbClsExtra   = 0 ;
   pWndClass->cbWndExtra   = DLGWINDOWEXTRA ;   // use this 'cause we're registering a dialgo box.
   pWndClass->hInstance    = hInstance ;
   pWndClass->hIcon        = LoadIcon (hInstance, szAppName) ;
   pWndClass->hCursor      = LoadCursor (NULL, IDC_ARROW) ;
   pWndClass->hbrBackground = COLOR_WINDOW + 1 ;
   pWndClass->lpszMenuName = szAppName ;
   pWndClass->lpszClassName = szAppName ;

   hHourGlass = LoadCursor( NULL, IDC_WAIT ) ;

   bSuccess = RegisterClass( pWndClass ) ;

   LocalUnlock( hMemory ) ;
   LocalFree( hMemory ) ;

   return bSuccess ;
}



