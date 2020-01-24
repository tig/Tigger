/************************************************************************
 *
 *     Project:  XEDIT 2.0
 *
 *      Module:  about.c
 *
 *     Remarks:  About box and shareware stuff
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "..\inc\XEDIT.h"
#include "..\inc\dlgs.h"
#include "..\inc\dlghelp.h"
#include "..\inc\about.h"
//#include "..\help\map.h"

#include <string.h>

BOOL NEAR PASCAL PrintRegForm( VOID ) ;
BOOL CALLBACK fnAbout( HWND hDlg, WORD wMsg, WPARAM wParam, LPARAM lParam) ;
BOOL CALLBACK fnShareware( HWND hDlg, WORD wMsg, WPARAM wParam, LPARAM lParam) ;
#define MAGIC_COOKIE 7431

BOOL NEAR PASCAL CompareRegNums( LPSTR lpszNum1, LPSTR lpszNum2 ) ;
BOOL NEAR PASCAL BuildRegNum( LPSTR lpszAppName,
                              LPSTR lpszName,
                              LPSTR lpszNumber,
                              WORD  wFlags ) ;
BOOL NEAR PASCAL ValidateNewReg( LPSTR lpszName, LPSTR lpszNumber ) ;

PSTR NEAR PASCAL CreateRegistration (PSTR psNum, PSTR psName) ;


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

BOOL WINAPI DoSharewareBox( VOID )
{
   DLGPROC  lpfnDlgProc ;

   (FARPROC)lpfnDlgProc = MakeProcInstance( (FARPROC)fnShareware, hinstApp ) ;
   DialogBox( hinstApp,
              MAKEINTRESOURCE( DLG_SHAREWARE ),
              hwndApp,
              lpfnDlgProc ) ;
   FreeProcInstance( (FARPROC)lpfnDlgProc ) ;

   return TRUE ;
}

BOOL CALLBACK fnAbout( HWND hDlg, WORD wMsg, WPARAM wParam, LPARAM lParam ) 
{
   char  szNum [MAX_REGNUM_LEN+1] ;
   char  szTemp [MAX_REGNAME_LEN+1] ;

   switch (wMsg)
   {
      case WM_INITDIALOG:

         SendDlgItemMessage( hDlg, IDD_USERNAME,
                             WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;
#ifdef SHOWREGNUMBER
         SendDlgItemMessage( hDlg, IDD_REGNUMBER,
                             WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;
#endif

         wsprintf( szTemp, "%s %s - %s",
                   (LPSTR)rglpsz[IDS_VERSION], (LPSTR)szVerNum,
                   (LPSTR)__DATE__ ) ;
         SetDlgItemText (hDlg, IDD_VERSION, szTemp ) ;

         wsprintf( szTemp, " %s", (LPSTR)szRegisteredName ) ;
         SetDlgItemText (hDlg, IDD_USERNAME, szTemp ) ;

#ifdef SHOWREGNUMBER
         if (lstrcmpi( szRegisteredName, rglpsz[IDS_PLEASEPAY] ))
         {
            GetPrivateProfileString( rglpsz[IDS_INI_REG],
                                     rglpsz[IDS_INI_REG_CODE],
                                     "", szNum, MAX_REGNUM_LEN,
                                     rglpsz[IDS_INI_FILENAME]) ;
            wsprintf( szTemp, " %s", (LPSTR)szNum ) ;
            SetDlgItemText (hDlg, IDD_REGNUMBER, szTemp) ;
         }
#endif

         return tdShadowControl( hDlg, wMsg, (WORD)wParam, 0L ) ;
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

      #if 0
      case WM_ERASEBKGND:
      {
         RECT rc ;
         HDC hdc ;

         hdc = GetDC( GetDlgItem( hDlg, IDD_BOX0 ) ) ;

         GetClientRect( GetDlgItem( hDlg, IDD_BOX0 ), &rc ) ;

         #define RGBLTGRAY    RGB(192,192,192)
         SetBkColor( hdc, RGBLTGRAY ) ;
         ExtTextOut( hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;

         ReleaseDC( GetDlgItem( hDlg, IDD_BOX0 ), hdc ) ;

      }
      return TRUE ;
      #endif

      default:
         return tdShadowControl( hDlg, wMsg, (WORD)wParam, (LONG)lParam ) ;
   }
   return TRUE ;
}

BOOL CALLBACK fnShareware (HWND hDlg, WORD wMsg, WPARAM wParam, LPARAM lParam) 
{
   char  szName [MAX_REGNAME_LEN+1] ;
   char  szNum [MAX_REGNAME_LEN+1] ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
            SendMessage( GetDlgItem( hDlg, IDD_USERNAME ),
                         EM_LIMITTEXT, (WPARAM)MAX_REGNAME_LEN,
                         0L ) ;

            SetDlgItemText( hDlg, IDD_USERNAME, szRegisteredName ) ;

            SendMessage( GetDlgItem( hDlg, IDD_REGNUMBER ),
                         EM_LIMITTEXT, (WPARAM)MAX_REGNUM_LEN,
                         0L ) ;

      return tdShadowControl( hDlg, wMsg, (WORD)wParam, (LONG)0L ) ;

      case WM_COMMAND:
         switch ((WORD)wParam)
         {
            case IDD_CLOSE:
               MessageBeep( MB_ICONHAND ) ;
               MessageBeep( MB_ICONQUESTION ) ;
               EndDialog (hDlg, 0) ;
               break ;

            case IDOK :
               GetDlgItemText( hDlg, IDD_USERNAME,
                               szName, MAX_REGNAME_LEN+1 ) ;
               GetDlgItemText( hDlg, IDD_REGNUMBER,
                               szNum, MAX_REGNUM_LEN+1 ) ;

               if (!ValidateNewReg( szName, szNum ))
               {
                  ErrorResBox( hDlg, hinstApp, MB_ICONEXCLAMATION,
                               IDS_APPTITLE, IDS_ERR_BADREGISTRATION ) ;
                  SetFocus (GetDlgItem (hDlg, IDD_USERNAME)) ;
               }
               else
               {
                  WritePrivateProfileString(
                                       rglpsz[IDS_INI_REG], 
                                       rglpsz[IDS_INI_REG_NAME],
                                       szName,
                                       rglpsz[IDS_INI_FILENAME]) ;

                  WritePrivateProfileString(
                                       rglpsz[IDS_INI_REG],
                                       rglpsz[IDS_INI_REG_CODE],
                                       AnsiUpper( szNum ),
                                       rglpsz[IDS_INI_FILENAME]) ;

                  lstrcpy (szRegisteredName, szName) ;
                  EndDialog (hDlg, 1) ;
               }
               break ;

            case IDD_PRINTREGFORM:
               PrintRegForm() ;
            break ;

            case pshHelp:
//               WinHelp( hwndApp, rglpsz[IDS_HELPFILE], HELP_CONTEXT,
//                        HLP_SHAREWARE ) ;
            break ;


            default :
               return FALSE ;
         }
         break ;

      default:
         return tdShadowControl( hDlg, wMsg, (WORD)wParam, (LONG)lParam ) ;
   }
   return TRUE ;
}


BOOL NEAR PASCAL PrintRegForm( VOID )
{
#if 0  
   LPSTR          lpText ;
   LPSTR          lpCur ;
   char           szFile[144] ;
   HANDLE         hTemp ;
   HANDLE         hRes ;
   int            hfTempFile ;
   int            n ;

   StartPrintUI() ;

   if (!(hTemp = FindResource( hinstApp,
         MAKEINTRESOURCE(ID_RCDATA_ORDER_FRM), "TEXTFILE" )))
   {
      DASSERTMSG( hWDB, 0, "Cannot read resource to print order form!" ) ;
      EndPrintUI() ;
      return FALSE ;
   }

   if ((hRes = LoadResource( hinstApp, hTemp )) &&
       (lpText = LockResource( hRes )))
   {
      GetTempFileName( GetTempDrive( 0 ), "wpr", 0, (LPSTR)szFile ) ;

      DP3( hWDB, "Temp file == %s", (LPSTR)szFile ) ;

      hfTempFile = _lcreat((LPSTR)szFile, 0 ) ;

      if (hfTempFile == -1)
      {
         DP1( hWDB, "Temp file == %s", (LPSTR)szFile ) ;
         DASSERTMSG( hWDB, 0, "Could not create temp file!" ) ;

         UnlockResource( hRes ) ;
         FreeResource( hRes ) ;
         EndPrintUI() ;
         return FALSE ;
      }

      /*
       * find out how big it is by searching for the ^Z
       */
      lpCur = lpText ;
      n = 0 ;
      while( *lpCur++ != '\x1A' )
         n++ ;

      _lwrite( hfTempFile, (LPSTR)lpText, n ) ;

      _lclose( hfTempFile ) ;


      UnlockResource( hRes ) ;
      FreeResource( hRes ) ;

      /*
       * now print
       */
      PrintFiles( hwndApp, NULL, NULL, szFile ) ;
   }

   /*
    * Delete the file
    */

   EndPrintUI() ;
#endif

   return TRUE ;
}


BOOL WINAPI CheckRegistration (HANDLE hinstApp)
{
   char  szNum [MAX_REGNUM_LEN+1] ;

   /*
    * First try the new style.  
    */
   if (GetPrivateProfileString( rglpsz[IDS_INI_REG],
                                rglpsz[IDS_INI_REG_NAME],
                                "", szRegisteredName, MAX_REGNAME_LEN,
                                rglpsz[IDS_INI_FILENAME]) &&
       GetPrivateProfileString( rglpsz[IDS_INI_REG],
                                rglpsz[IDS_INI_REG_CODE],
                                "", szNum, MAX_REGNUM_LEN + 1,
                                rglpsz[IDS_INI_FILENAME]) &&
       ValidateNewReg( szRegisteredName, szNum ))
   {
      DP1( hWDB, "Valid registration %s, %s",
         (LPSTR)szRegisteredName, (LPSTR)szNum ) ;
      return TRUE ;
   }

   lstrcpy( szRegisteredName, rglpsz[IDS_PLEASEPAY] ) ;

   return FALSE ;
}

BOOL NEAR PASCAL CompareRegNums( LPSTR lpszNum1, LPSTR lpszNum2 )
{
   // do a string compare (case insensitive)
   return lstrcmpi( lpszNum1, lpszNum2 ) ? FALSE : TRUE ;
}


BOOL NEAR PASCAL ValidateNewReg( LPSTR lpszName, LPSTR lpszNumber )
{
   char szNum[MAX_REGNUM_LEN+1] ;

   BuildRegNum( NULL, NULL, NULL, MAGIC_COOKIE ) ;
   if (!BuildRegNum( rglpsz[IDS_APPNAME], lpszName, szNum, 0))
   {
      DP1( hWDB, "Problem building reg number for validation" ) ;
      return FALSE ;
   }

   return CompareRegNums( szNum, lpszNumber ) ;
}



//-----------------------------------------------------------------
//
// BOOL WINAPI
// BuildRegNum( LPSTR lpszAppName,
//             LPSTR lpszName,
//             LPSTR lpszNumber,
//             WORD  wFlags );
//
// Description:
//    Builds a registration number.
//    Number is of the form:
//
//       WWXXYYZZ where:
//       
//       WW = LOBYTE(crc of AppName) + 42
//       XX = LOBYTE(crc of First Name) + 9 + lstrlen(name)
//       YY = LOBYTE(crc of Middle Name) + 11+ lstrlen(name)
//       ZZ = LOBYTE(crc of Last Name) + lstrlen(name)
//
//    If there is no middle name then YY = LOBYTE(WW+XX)
//    If there is no last name then ZZ = LOBYTE(XX+YY)
//
//    Note WW, XX, YY, and ZZ are all in hex.
//
// Must be called twice.  Once with wFlags == MAGIC_COOKIE and
// Then with flags == 0.  This will keep anyone from figuring out
// how to call it.
//                  
//-----------------------------------------------------------------
BOOL NEAR PASCAL BuildRegNum( LPSTR lpszAppName,
                              LPSTR lpszName,
                              LPSTR lpszNumber,
                              WORD  wFlags )
{
   static WORD wMagic ;

   // Needs to be called this way first.
   if (wFlags == MAGIC_COOKIE)
   {
      wMagic = wFlags + MAGIC_COOKIE ;
      return TRUE ;
   }

   // It was already called with MAGIC_COOKIE so do it!
   if (wMagic == (MAGIC_COOKIE + MAGIC_COOKIE))
   {
      WORD wWW = 42 ;
      WORD wXX = 9 + lstrlen( lpszName ) ;
      WORD wYY = 11 + lstrlen( lpszName ) ;
      WORD wZZ = lstrlen( lpszName ) ;
      LPSTR lpsCur ;
      char szName [MAX_REGNAME_LEN+1] ;
      LPSTR lpsFirst ;
      LPSTR lpsMiddle ;
      LPSTR lpsLast ;

      // Build CRC for AppName (wWW)
      for (lpsCur = lpszAppName ; *lpsCur != '\0' ; lpsCur++)
         wWW += (BYTE)*lpsCur ;

      // Break Name up into sub parts (separated by spaces)

      lstrcpy( szName, lpszName ) ;
      lpsFirst  = _fstrtok( szName, " " ) ;
      lpsMiddle = _fstrtok( NULL, " " ) ;
      lpsLast   = _fstrtok( NULL, " ") ;

      // Build CRC for First name (XX)
      if (!lpsFirst)  // there's an error
      {
         return FALSE ;
      }
      else
      {
         for (lpsCur = lpsFirst ; *lpsCur != '\0' ; lpsCur++)
            wXX += (BYTE)*lpsCur ;
      }

      // Build CRC for Middle name (YY)
      if (!lpsMiddle)  // YY = WW+XX
      {
         wYY = wWW + wXX ;
      }
      else
      {
         for (lpsCur = lpsMiddle ; *lpsCur != '\0' ; lpsCur++)
            wYY += (BYTE)*lpsCur ;
      }

      // Build CRC for Last name (ZZ)
      if (!lpsLast)  // ZZ = XX+YY
      {
         wZZ = wXX + wYY;
      }
      else
      {
         for (lpsCur = lpsLast ; *lpsCur != '\0' ; lpsCur++)
            wZZ += (BYTE)*lpsCur ;
      }

      // We now have wWW, wXX, wYY, and wZZ
      wsprintf( lpszNumber,
                (LPSTR)"%2.2X%2.2X%2.2X%2.2X",
                LOBYTE(wWW),
                LOBYTE(wXX),
                LOBYTE(wYY),
                LOBYTE(wZZ) ) ;

      wMagic = 0 ;

      return TRUE ;
   }

   // Return false if they don't know MAGIC_COOKIE
   return FALSE ;
}



/************************************************************************
 * End of File: about.c
 ***********************************************************************/

