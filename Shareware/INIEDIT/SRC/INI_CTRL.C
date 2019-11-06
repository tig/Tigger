//  ini_ctrl.c
//
//  Control routines for INIedit.  These routines handle the list boxes.
//
//  Windows 3.0 INI editor
//
//  Copyright (c) 1990 Charles E. Kindel, Jr.
//
//  Author:  Charles E. Kindel, Jr. 
//
#include <windows.h>
#include <ceklib.h>
#include <stdio.h>
#include <string.h>
#include "..\inc\inieditd.h"
#include "..\inc\iniedit.h"
#include "..\inc\ini_dlg.h"
#include "..\inc\ini_ctrl.h"
#include "..\inc\ini_add.h"
#include "..\inc\ini_io.h"

extern HANDLE  hInst ;
extern HWND    hMainWnd ;
extern char    szSysDir [] ;
extern char    szFileSpec[];
extern char    szAppName[] ;
extern HWND    hwndCombo, hwndName, hwndSection ;
extern HCURSOR hHourGlass ;

char  szFileName [INI_MAXPATH] ;
char  szSection  [INI_MAXSECTIONLEN] ;
char  szName     [INI_MAXNAMELEN] ;
char  szValue    [INI_MAXVALUELEN] ;

BOOL _DlgDirSelectComboBox(   HWND     hwndDlg,
                              LPSTR    lpszPathSpec,
                              int      idListBox ) ;

//
//       int ProcessFileNameCB (LONG lParam)
//
int ProcessFileNameCB (LONG lParam)
{
   WORD  nIndex ;

   switch (HIWORD (lParam))
   {
      case CBN_SELCHANGE :
         nIndex = (WORD) SendMessage (hwndCombo, CB_GETCURSEL, 0, 0L) ;

         // user has changed the selection in the list box
         if (_DlgDirSelectComboBox (hMainWnd, (LPSTR) szFileName, IDD_FNAME_CB))
         {
            lstrcat (szFileName, szFileSpec) ;
            DlgDirListComboBox ( hMainWnd,
                                 szFileName,
                                 IDD_FNAME_CB,
                                 0,
                                 INI_FILETYPE ) ;
            nIndex = (WORD) SendMessage( hwndCombo,
                                       CB_FINDSTRING,
                                       -1,
                                       (LONG)(LPSTR) DEFAULT_FILENAME ) ;
            SendMessage(   hwndCombo,
                           CB_SETCURSEL,
                           nIndex == CB_ERR ? 0 : nIndex,
                           0L ) ;
         }
         else
            return FillSectionLB() ;
         break ;

      default :
         return FALSE ;

   }

   return TRUE ;
}
//       int FillSectionLB 
//          Fills the Section ListBox with sections from the current file.
//          Returns an error code if there was a problem (negative).
//
int FillSectionLB (void)
{
   PSTR     pszMemory ;
   PSTR     psCur ;
   char     szBuf [80] ;
   char     szStrToAdd [INI_MAXSECTIONLEN + 1] ;
   int i = 0 ;
   HANDLE   hMemory ;
   HCURSOR  hTmpCursor ;


   int nIndex = (WORD) SendMessage( hwndCombo,
                                    CB_GETCURSEL,
                                    0, 0L ) ;

   if (nIndex == CB_ERR)
   {
      if (!lstrlen( szFileName ))    // no file name
         return FALSE ;
   }
   else
      SendMessage( hwndCombo,
                     CB_GETLBTEXT,
                     nIndex,
                     (LONG)(LPSTR) szFileName ) ;
   
   SetCapture( hMainWnd ) ;
   hTmpCursor = SetCursor( hHourGlass ) ;

   SendMessage( hwndSection, WM_SETREDRAW, FALSE, 0L ) ;
   SendMessage( hwndSection, LB_RESETCONTENT, 0, 0L ) ;

   // Read file
   if (!(hMemory = LocalAlloc( LHND, INI_MAXSECTIONLEN * 64 )))
   {
      wsprintf( (LPSTR)szBuf,
               (LPSTR)"Not enough memory!(a)" ) ;
      goto Error2 ;
   }

   pszMemory = LocalLock ( hMemory ) ;
   if (-1 == GetPrivateProfileSections( pszMemory, szFileName ))
   {
      lstrcpy( szBuf, pszMemory ) ;
      goto Error1 ;
   }
   else
   {
//      if (!*psCur)
//      {
//         wsprintf( szBuf, "The file '%s' is empty", (LPSTR)szFileName ) ;
//         goto Error1 ;
//      }
      D(dp("szFileName = %s", (LPSTR)szFileName )) ;

      for (psCur = pszMemory ; *psCur != NULL ; psCur++)
      {
         wsprintf( szStrToAdd, (LPSTR)"[%s]", (LPSTR)psCur ) ;
         if (LB_ERR == (WORD)SendMessage( hwndSection,
                                          LB_ADDSTRING,
                                          0, (LONG)(LPSTR)szStrToAdd ))
         {
            SendMessage( hwndSection, WM_SETREDRAW, TRUE, 0L ) ;
            InvalidateRect( hwndSection, NULL, TRUE ) ;
            UpdateWindow( hwndSection ) ;
            wsprintf( szBuf, (LPSTR)"List box out of memory!" ) ;
            goto Error1 ;
         }

         while (*(++psCur) != NULL)
            ;
      }
   }

   LocalUnlock( hMemory ) ;
   LocalFree( hMemory ) ;

   if ((WORD) SendMessage( hwndSection, LB_GETCOUNT, 0, 0L ))
   {
      SendMessage( hwndSection, LB_SETCURSEL, 0, 0L ) ;
      SendMessage( hwndSection, LB_GETTEXT, 0, (LONG)(LPSTR)szSection ) ;
      lstrcpy( szSection, (PSTR)szSection+1 ) ;
      szSection[lstrlen( szSection ) - 1] = '\0' ;
   }
   else
      lstrcpy( szSection, "") ;

   SendMessage( hwndSection, WM_SETREDRAW, TRUE, 0L ) ;
   InvalidateRect( hwndSection, NULL, TRUE ) ;
   UpdateWindow( hwndSection ) ;
   SetCursor( hTmpCursor ) ;
   ReleaseCapture() ;
   FillNameLB() ;

   wsprintf( szBuf, (LPSTR)"%s [%s]", (LPSTR)szAppName, (LPSTR)szFileName ) ;
   SetWindowText( hMainWnd, szBuf ) ;
   SetFocus( hwndSection ) ;

   return TRUE ;


// Error handlers
Error1:
   LocalUnlock( hMemory ) ;
   LocalFree( hMemory ) ;
Error2:
   MessageBox( hMainWnd, (LPSTR)szBuf, "Error!", MB_ICONEXCLAMATION ) ;
   SendMessage( hwndSection, WM_SETREDRAW, TRUE, 0L ) ;
   InvalidateRect( hwndSection, NULL, TRUE ) ;
   UpdateWindow( hwndSection ) ;
   lstrcpy( szSection, "" ) ;
   lstrcpy( szName, "" ) ;
   lstrcpy( szValue, "" ) ;
   SetCursor( hTmpCursor ) ;
   ReleaseCapture() ;

   SendMessage( hwndName, LB_RESETCONTENT, 0, 0L) ;

   wsprintf( szBuf, (LPSTR)"%s [ERROR:%s]", (LPSTR)szAppName, (LPSTR)szFileName ) ;
   SetWindowText( hMainWnd, szBuf ) ;
   i = (WORD) SendMessage( hwndCombo,
                                CB_FINDSTRING,
                                -1,
                                (LONG)(LPSTR) DEFAULT_FILENAME ) ;
   SendMessage ( hwndCombo,
                 CB_SETCURSEL,
                 i == CB_ERR ? 0 : i,
                 0L ) ;

   SetFocus( hwndCombo ) ;

   return FALSE ;
}


//
//       int ProcessSectionLB (LONG lParam)
//          Processes messages for the NameLB
//
int ProcessSectionLB (LONG lParam)
{

   switch (HIWORD( lParam ))
   {
      case LBN_SELCHANGE :
         SendMessage( hwndSection, LB_GETTEXT,
                      (WORD) SendMessage( hwndSection, LB_GETCURSEL, 0, 0L ),
                      (LONG)(LPSTR)szSection ) ;
         lstrcpy( szSection, (PSTR)szSection+1 ) ;
         szSection[lstrlen( szSection ) - 1] = '\0' ;
         FillNameLB () ;
         break ;  // LBN_SELCHANGE

      case LBN_DBLCLK :
         break ;

      default:
         return FALSE ;
      }

   return TRUE ;
}


//       int FillNameLB
//          Fils the Item Name ListBox with item names from the current
//          file.  Same error scheme as above.
//
int FillNameLB (void)
{
   char     szErr[80] ;
   char szTempName[INI_MAXNAMELEN + INI_MAXVALUELEN + 1] ;
   PSTR     psBuffer ;
   HANDLE   hMemory ;
   int      nLen ;
   WORD     wIndex ;
   HCURSOR  hTmpCursor ;

   SetCapture( hMainWnd ) ;
   hTmpCursor = SetCursor( hHourGlass ) ;
   if (lstrlen( szSection ))
   {

      if (!(hMemory = LocalAlloc( LHND, BUFFERSIZE )))
      {
         wsprintf( (LPSTR)szErr, (LPSTR)"Not enough memory!(c)" ) ;
         goto Error1 ;
      }

      psBuffer = LocalLock ( hMemory ) ;
            
      if ((BUFFERSIZE - 2) == (nLen = GetPrivateProfileString( szSection,
                                       NULL,
                                       "<error>",
                                       psBuffer,
                                       BUFFERSIZE,
                                       szFileName )))
      {
         wsprintf( szErr, "Cannot read profile keys." ) ;
         goto Error1 ;
         // pg. 4-200 of reference manual explains this -2 thingy.
      }


      if (!ProfileToListBox( hwndName, psBuffer ))
      {
         wsprintf( (LPSTR)szErr, "Cannot parse profile keys." ) ;
         goto Error1 ;
      }

      LocalUnlock( hMemory ) ;
      LocalFree( hMemory ) ;

      if (wIndex = (WORD)SendMessage( hwndName, LB_GETCOUNT, 0, 0L ) > 0)
      {
         if (LB_ERR == (wIndex = (WORD)SendMessage( hwndName, LB_FINDSTRING, -1,
            (LONG)(LPSTR)szName )))
            wIndex = 0 ;
         SendMessage( hwndName, LB_SETCURSEL, wIndex, 0L ) ;
         SendMessage( hwndName, LB_GETTEXT, wIndex, (LONG)(LPSTR)szTempName ) ;
         ParseKeyName ( szTempName ) ;
         EnableWindow( GetDlgItem( hMainWnd, IDD_NAMEADD_BTN ), TRUE ) ;
         EnableWindow( GetDlgItem( hMainWnd, IDD_NAMEDEL_BTN ), TRUE ) ;
         EnableWindow( GetDlgItem( hMainWnd, IDD_EDIT_BTN ), TRUE ) ;
      }
      else
      {
         EnableWindow( GetDlgItem( hMainWnd, IDD_NAMEDEL_BTN ), FALSE ) ;
         EnableWindow( GetDlgItem( hMainWnd, IDD_EDIT_BTN ), FALSE ) ;
         lstrcpy( szName, "" ) ;
      }

   }
   else
   {
         EnableWindow( GetDlgItem( hMainWnd, IDD_NAMEADD_BTN ), FALSE ) ;
         EnableWindow( GetDlgItem( hMainWnd, IDD_NAMEDEL_BTN ), FALSE ) ;
         EnableWindow( GetDlgItem( hMainWnd, IDD_EDIT_BTN ), FALSE ) ;
         SendMessage( hwndName, LB_RESETCONTENT, 0, 0L ) ;
         return TRUE ;
         wsprintf( (LPSTR)szErr, "Invalid section name." ) ;
         goto Error2 ;
   }


   SetCursor( hTmpCursor ) ;
   ReleaseCapture() ;
   return TRUE ;

Error1:
   LocalUnlock( hMemory ) ;
   LocalFree( hMemory ) ;
Error2:
   SetCursor( hTmpCursor ) ;
   ReleaseCapture() ;
   MessageBox( hMainWnd, (LPSTR)szErr, "Error!", MB_ICONEXCLAMATION ) ;
   return FALSE ;
}

BOOL ProfileToListBox( HWND hLB, PSTR psBuffer )
{
   PSTR psCur = psBuffer ;
   char szCurValue[ INI_MAXVALUELEN + 1] ;
   char szStrToAdd[ INI_MAXVALUELEN + INI_MAXNAMELEN + 1] ;

   SendMessage( hLB, WM_SETREDRAW, FALSE, 0L ) ;
   SendMessage( hLB, LB_RESETCONTENT, 0, 0L) ;
   do
   {
      if (*psCur != NULL)
      {
         GetPrivateProfileString( szSection,
                                  psCur,
                                  "",
                                  szCurValue,
                                  INI_MAXVALUELEN,
                                  szFileName ) ;

         wsprintf( szStrToAdd, (LPSTR)"%s = %s", (LPSTR)psCur,
                                                   (LPSTR)szCurValue ) ;
         if (LB_ERR == (WORD)SendMessage( hLB,
                                          LB_ADDSTRING,
                                          0, (LONG)(LPSTR)szStrToAdd ))
         {
            SendMessage( hLB, WM_SETREDRAW, TRUE, 0L ) ;
            InvalidateRect( hLB, NULL, TRUE ) ;
            UpdateWindow( hLB ) ;
            return FALSE ;
         }

         while (*(++psCur) != NULL)
            ;
      }
   } while (*(++psCur) != NULL) ;

   SendMessage( hLB, WM_SETREDRAW, TRUE, 0L ) ;
   InvalidateRect( hLB, NULL, TRUE ) ;
   UpdateWindow( hLB ) ;
   return TRUE ;
}


//
//          Processes messages for the NameLB
int ProcessNameLB (LONG lParam)
{
   HWND hLB ;
   HWND hwndCur ;
   FARPROC lpfnDlgProc ;
   char szTempName[INI_MAXNAMELEN + INI_MAXVALUELEN + 1] ;

   switch (HIWORD( lParam ))
   {
      case LBN_SELCHANGE :
         hLB = GetDlgItem( hMainWnd, IDD_NAME_LB ) ;
         SendMessage( hLB, LB_GETTEXT,
                      (WORD)SendMessage( hLB, LB_GETCURSEL, 0, 0L ),
                      (LONG)(LPSTR)szTempName ) ;
         ParseKeyName( szTempName ) ;
         break ;

      case LBN_DBLCLK :
         hLB = GetDlgItem( hMainWnd, IDD_NAME_LB ) ;
         SendMessage( hLB, LB_GETTEXT,
                      (WORD)SendMessage( hLB, LB_GETCURSEL, 0, 0L ),
                      (LONG)(LPSTR)szTempName ) ;
         ParseKeyName( szTempName ) ;

         PostMessage( hMainWnd, WM_COMMAND, IDD_EDIT_BTN, 0L );

         break ;

      default:
         return FALSE ;
      }

   return TRUE ;
}

void ParseKeyName( LPSTR lpszBuf ) 
{
   int i ;

   for (i = 0 ; i < lstrlen( lpszBuf ) ; i++)
      if ( lpszBuf[i] == '=' )
      {
         lpszBuf[i - 1] = '\0' ;
         break ;
      }
   lstrcpy( szName, lpszBuf ) ;
   lstrcpy( szValue, &lpszBuf[i+2] ) ;
}


//  BOOL FillValueEC (void) 
//
BOOL FillValueEC( void )
{
   lstrcpy( szValue, "" ) ;
   if (GetPrivateProfileString( szSection, szName, "", szValue,
                                 INI_MAXVALUELEN, szFileName )
         == INI_MAXVALUELEN)
   {
      MessageBox( hMainWnd, "Profile string too long!",
                  "Error!", MB_ICONEXCLAMATION ) ;
      return FALSE ;
   }

   return TRUE ;
}



BOOL FAR PASCAL ValueDlgProc( HWND hDlg,
                              unsigned iMessage,
                              WORD  wParam,
                              LONG lParam )
{
   char szBuf [256] ;
   static char szTempValue [INI_MAXVALUELEN] ;
   HDC   hDC ;
   TEXTMETRIC tm ;
   PAINTSTRUCT ps ;

   switch (iMessage)
   {
      case WM_INITDIALOG :
         SendDlgItemMessage( hDlg, IDD_VALUE_EC,
                                   EM_LIMITTEXT, INI_MAXVALUELEN, 0L );
         lstrcpy( szTempValue, szValue ) ;
         SetDlgItemText( hDlg, IDD_VALUE_EC, szValue ) ;

         wsprintf( (LPSTR)szBuf,
                   (LPSTR)"[%s] : %s",
                   (LPSTR)szSection,
                   (LPSTR)szName ) ;
         SetWindowText( hDlg, szBuf ) ;
         wsprintf( (LPSTR)szBuf,
                   (LPSTR)"%s =",
                   (LPSTR)szName ) ;
         SetDlgItemText( hDlg, IDD_NAME_TXT, szBuf ) ;
         break ;

      case WM_PAINT :
         hDC = BeginPaint(hDlg, &ps ) ;
         GetTextMetrics( hDC, &tm ) ;
         DrawShadow( hDC, GetDlgItem( hDlg, IDD_VALUE_EC ),
                     GetStockObject( GRAY_BRUSH ), tm.tmAveCharWidth / 2 ) ;
         EndPaint( hDlg, &ps ) ;
         return FALSE ;

      case WM_COMMAND :
         switch (wParam)
         {
            case IDD_OK :
               GetDlgItemText( hDlg, IDD_VALUE_EC,
                              (LPSTR)szValue,
                              INI_MAXVALUELEN ) ;

               if (lstrcmp( szValue, szTempValue ))
               {
                  WritePrivateProfileString( szSection,
                                             szName,
                                             szValue,
                                             szFileName ) ;
                  EndDialog( hDlg, TRUE ) ;
               }
               else
                  EndDialog( hDlg, FALSE ) ;
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



//------------------------------------------------------

#define TABCHAR		'\t'


BOOL _DlgDirSelectComboBox(   HWND     hwndDlg,
                              LPSTR    lpszPathSpec,
                              int      idComboBox )
{
  register int cch;
  LPSTR        lpchFile;
  BOOL	       fDir;
  int	       item;
  register int cchFile;
  LPSTR        lpchT;
  char         rgch[INI_MAXPATH];

  /* CheckHwnd not needed since SendDlgItemMessage does it. */
  item = (int)SendDlgItemMessage(hwndDlg, idComboBox, CB_GETCURSEL, 0, 0L);
  if (item < 0)
      return(FALSE);

  SendDlgItemMessage(hwndDlg, idComboBox, CB_GETLBTEXT, item, (DWORD)(LPSTR)rgch);
  lpchFile = (LPSTR)rgch;
  fDir = (*rgch == '[');
   
  /* Only the file name is to be returned. Find the end of the filename. */
  lpchT = lpchFile ;
  while ((*lpchT) && (*lpchT != TABCHAR))
      lpchT++;
  *lpchT = '\0';

  cch = lstrlen(lpchFile);

  /* Selection is drive or directory. */
  if (fDir)
    {
      lpchFile++;
      cch--;
      *(lpchFile + cch - 1) = '\\';

      /* Selection is drive */
      if (rgch[1] == '-')
	   {
	      lpchFile++;
	      cch--;
	      *(lpchFile+1) = ':';
	      *(lpchFile+2) = 0;
	   }

    }
   else
    {
      /* Selection is file.  If filename has no extension, append '.' */
      for (cchFile=lstrlen(lpchT = lpchFile);
           (cchFile > 0) && (*lpchT != TABCHAR);
           cchFile--, lpchT = AnsiNext(lpchT))
	   {
	      if (*lpchT == '.')
	         goto Exit;
   	}
      *lpchT++ = '.';
      *lpchT = 0;
    }

Exit:
  lstrcpy(lpszPathSpec, lpchFile);
  return(fDir);

}   
