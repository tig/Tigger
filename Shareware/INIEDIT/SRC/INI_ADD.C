//  ini_add.c
//
//
//  INIedit Add and Delete functions
//
//  Windows 3.0 INI editor
//
//  Copyright (c) 1990 Charles E. Kindel, Jr.
//
//  Author:  Charles E. Kindel, Jr. 
//


#include <windows.h>
#include <ceklib.h>
#include "..\inc\iniedit.h"
#include "..\inc\inieditd.h"
#include "..\inc\ini_add.h"
#include "..\inc\ini_ctrl.h"

extern char szFileName [] ;
extern char szSection  [] ;
extern char szName     [] ;
extern char szValue    [] ;
extern HWND hMainWnd ;
extern HANDLE hInst ;

void DelName( void )
{
   char szBuf [INI_MAXVALUELEN * 2] ;

   wsprintf( szBuf, (LPSTR)"Remove %s = %s?",
                     (LPSTR)szName,
                     (LPSTR)szValue ) ;
   if (IDYES == MessageBox ( hMainWnd, szBuf, "Confirm Remove",
      MB_ICONQUESTION | MB_YESNO ))
   {
      WritePrivateProfileString( szSection, szName, NULL, szFileName ) ;
      WritePrivateProfileString( NULL, NULL, NULL, szFileName ) ;
      FillNameLB () ;
   }
}

void DelSection( void )
{
   char szBuf [INI_MAXVALUELEN * 2] ;

   wsprintf( szBuf, (LPSTR)"Remove the section %s?", (LPSTR)szSection ) ;
   if (IDYES == MessageBox ( hMainWnd, szBuf, "Confirm Remove",
      MB_ICONQUESTION | MB_YESNO ))
   {
      WritePrivateProfileString( szSection, NULL, NULL, szFileName ) ;
      WritePrivateProfileString( NULL, NULL, NULL, szFileName ) ;
      FillSectionLB () ;
   }
}

void AddName( void )
{
   FARPROC lpfnDlgProc ;

   lpfnDlgProc = MakeProcInstance( AddNameDlgProc, hInst ) ;
   DialogBox( hInst, "AddName", hMainWnd, lpfnDlgProc ) ;
   FreeProcInstance( lpfnDlgProc ) ;
}

BOOL FAR PASCAL AddNameDlgProc( HWND hDlg,
                              unsigned iMessage,
                              WORD  wParam,
                              LONG lParam )
{
   char szBuf [256] ;
   char szTempValue [INI_MAXVALUELEN + 1] ;
   int i ;
   HDC   hDC ;
   TEXTMETRIC tm ;
   PAINTSTRUCT ps ;

   switch (iMessage)
   {
      case WM_INITDIALOG :
         SendDlgItemMessage( hDlg, IDD_NAME_EC,
                                   EM_LIMITTEXT, INI_MAXVALUELEN, 0L );
         SendDlgItemMessage( hDlg, IDD_VALUE_EC,
                                   EM_LIMITTEXT, INI_MAXVALUELEN, 0L );

         wsprintf( (LPSTR)szBuf,
                   (LPSTR)"Add Key = Value" ) ;
         SetWindowText( hDlg, szBuf ) ;
         break ;

      case WM_PAINT :
         hDC = BeginPaint(hDlg, &ps ) ;
         GetTextMetrics( hDC, &tm ) ;
         DrawShadow( hDC, GetDlgItem( hDlg, IDD_NAME_EC ),
                     GetStockObject( GRAY_BRUSH ), tm.tmAveCharWidth / 2 ) ;
         DrawShadow( hDC, GetDlgItem( hDlg, IDD_VALUE_EC ),
                     GetStockObject( GRAY_BRUSH ), tm.tmAveCharWidth / 2 ) ;
         EndPaint( hDlg, &ps ) ;
         return FALSE ;
         break ;

      case WM_COMMAND :
         switch (wParam)
         {
            case IDD_OK :
               GetDlgItemText( hDlg, IDD_NAME_EC,
                              (LPSTR)szTempValue,
                              INI_MAXVALUELEN ) ;
               if (lstrlen( szTempValue ) > 0)
                  for (i = 0 ; i < lstrlen( szTempValue ) ; i++)
                     if ( szTempValue[i] == '=' )
                     {
                        MessageBox( hDlg,
                        "Value may not contain '=', '[', or ']' characters.",
                        "Error",
                        MB_ICONEXCLAMATION ) ;
                        return TRUE ;
                     }

               lstrcpy( szName, szTempValue ) ;


               GetDlgItemText( hDlg, IDD_VALUE_EC,
                              (LPSTR)szTempValue,
                              INI_MAXVALUELEN ) ;

               if (lstrlen( szTempValue ) > 0)
                  for (i = 0 ; i < lstrlen( szTempValue ) ; i++)
                     if ( szTempValue[i] == '=' )
                     {
                        MessageBox( hDlg,
                        "Value may not contain '=', '[', or ']' characters.",
                        "Error",
                        MB_ICONEXCLAMATION ) ;
                        return TRUE ;
                     }
               lstrcpy( szValue, szTempValue ) ;

               WritePrivateProfileString( szSection,
                                          szName,
                                          szValue,
                                          szFileName ) ;
               WritePrivateProfileString( NULL, NULL, NULL, szFileName ) ;
               FillNameLB () ;
               EndDialog (hDlg, TRUE) ;
               break ;

            case IDCANCEL :
               EndDialog( hDlg, FALSE ) ;
               break ;

            default:
               return FALSE ;
         }
         break ;

      default:
         return FALSE ;
   }
   return TRUE ;
}

void AddSection( void )
{
   FARPROC lpfnDlgProc ;

   lpfnDlgProc = MakeProcInstance( AddSectionDlgProc, hInst ) ;
   DialogBox( hInst, "AddSection", hMainWnd, lpfnDlgProc ) ;
   FreeProcInstance( lpfnDlgProc ) ;
}

BOOL FAR PASCAL AddSectionDlgProc( HWND hDlg,
                              unsigned iMessage,
                              WORD  wParam,
                              LONG lParam )
{
   char szBuf [256] ;
   char szTempValue [INI_MAXVALUELEN + 1] ;
   int i ;
   HDC   hDC ;
   TEXTMETRIC tm ;
   PAINTSTRUCT ps ;

   switch (iMessage)
   {
      case WM_INITDIALOG :
         SendDlgItemMessage( hDlg, IDD_SECTION_EC,
                                   EM_LIMITTEXT, INI_MAXSECTIONLEN, 0L );

         wsprintf( (LPSTR)szBuf,
                   (LPSTR)"Add Section" ) ;
         SetWindowText( hDlg, szBuf ) ;
         break ;

      case WM_PAINT :
         hDC = BeginPaint(hDlg, &ps ) ;
         GetTextMetrics( hDC, &tm ) ;
         DrawShadow( hDC, GetDlgItem( hDlg, IDD_SECTION_EC ),
                     GetStockObject( GRAY_BRUSH ), tm.tmAveCharWidth / 2 ) ;
         EndPaint( hDlg, &ps ) ;
         return FALSE ;
         break ;

      case WM_COMMAND :
         switch (wParam)
         {
            case IDD_OK :
               GetDlgItemText( hDlg, IDD_SECTION_EC,
                              (LPSTR)szTempValue,
                              INI_MAXVALUELEN ) ;
               if (lstrlen( szTempValue ) > 0)
                  for (i = 0 ; i < lstrlen( szTempValue ) ; i++)
                     if ( szTempValue[i] == '[' && szTempValue[i] == ']' )
                     {
                        MessageBox( hDlg,
                        "Value may not contain '[', or ']' characters.",
                        "Error",
                        MB_ICONEXCLAMATION ) ;
                        return TRUE ;
                     }

               lstrcpy( szSection, szTempValue ) ;

               WritePrivateProfileString( szSection,
                                          "foo",
                                          "bar",
                                          szFileName ) ;
               WritePrivateProfileString( szSection,
                                          "foo",
                                          NULL,
                                          szFileName ) ;
               WritePrivateProfileString( NULL, NULL, NULL, szFileName ) ;
               FillSectionLB () ;
               EndDialog (hDlg, TRUE) ;
               break ;

            case IDCANCEL :
               EndDialog( hDlg, FALSE ) ;
               break ;

            default:
               return FALSE ;
         }
         break ;

      default:
         return FALSE ;
   }
   return TRUE ;
}


