// register.c
//
// Shareware Registration routines for CEKLIB.DLL
//
// Copyright (C) 1990 Charles E. Kindel, Jr.
// 
// Modification history:
// 11/20/90 CEK  First Version
//
#define WINDLL
#include "..\inc\undef.h"
#include <windows.h>
#include <string.h>
#include "..\inc\ceklib.h"

//----------------------------------------------------------------
// Globals
//----------------------------------------------------------------
extern HANDLE hInst ;

//----------------------------------------------------------------
// Local functions
//----------------------------------------------------------------
BOOL CompareRegNums( LPSTR lpszNum1, LPSTR lpszNum2 ) ;

//----------------------------------------------------------------
// Local defines
//----------------------------------------------------------------
#define MAGIC_COOKIE 7431

//-----------------------------------------------------------------
//
//   BOOL FAR PASCAL
//   IsRegisteredUser(  LPSTR lpszAppName,
//                      LPSTR lpszName,
//                      LPSTR lpszNumber,
//                      WORD    wFlags ) 
//
// Description:
//    Reads and/or writes a registration number.
//    Reads the ini file specified by lpszAppName (lpszAppName.INI)
//    in the section [Registration].  If a valid registration number
//    is present for the current application TRUE is returned, FALSE
//    otherwise.  lpszName and lpszNumber are filled with the name
//    and registration number.
//
//    If wFlags == REGISTER_WRITE then the name and number passed in
//    are written to the INI file.  They are verified first.
// 
//
// Type/Parameter
//          Description
// 
// LPSTR lpszAppName
//          Specifies the name of the application.  Is used as
//          the filename of the INI file, thus must be 8 or less
//          characters long.
//
// LPSTR lpszName
// LPSTR lpszNumber
//          If lpszName and lpszNumber are not NULL then upon return
//          the strings they point to will contain the name and 
//          registration number.
//
//          They can be null if the name and number are not needed, but
//          only if wFlags == REGISTER_READ
//
// WORD wFlags
//          Specifies flags, and may be one of the following.
// 
//          Value
//                Meaning
//          REGISTER_READ
//                The INI file is read and TRUE or FALSE is returned
//                depending on the contents of the file.
//                If lpszName is not NULL then lpszName and lpszNumber
//                are filled with the name and number.
//
//          REGISTER_WRITE
//                lpszName and lpszNumber are verified.  If they are good
//                then they are written to the INI file and TRUE is
//                returned, otherwise nothing is written and FALSE is
//                returned.
// 
//          REGISTER_WRITE
//                lpszName and lpszAppName are used to generate a new 
//                number.
//
// Return Value
//          Returns one of the following values:
// 
//             Value
//                      Meaning
// 
//             FALSE
//                      The registration information is invalid, or
//                      does not exist.
//             TRUE
//                      The registration information is correct.
// 
//
// Comments
//    May add flags later.
// 
// Modifications
//    CEK   11/20/90    First version
//
//-----------------------------------------------------------------
BOOL FAR PASCAL
IsRegisteredUser( LPSTR lpszAppName,
                  LPSTR lpszName,
                  LPSTR lpszNumber,
                  WORD  wFlags )
{
   char szNumber[REG_MAX_NUMBER+1] ;
   char szName[REG_MAX_NAME+1] ;
   char szBuildNumber[REG_MAX_NUMBER+1] ;
   int   n ;

   if (wFlags == REGISTER_GENERATE)
   {
      // Call first with magic cookie number
      BuildRegNum( NULL, NULL, NULL, MAGIC_COOKIE ) ;
      if (!BuildRegNum( lpszAppName, lpszName, lpszNumber, wFlags))
      {
         D(dp( "IsRegisteredUser: BuildRegNum failed" )) ;
         return FALSE ;
      }
      return TRUE ;
   }

   if (wFlags == REGISTER_WRITE)
   {
      // Call first with magic cookie number
      BuildRegNum( NULL, NULL, NULL, MAGIC_COOKIE ) ;
      // Build a registration number based on INI file
      if (!BuildRegNum( lpszAppName, lpszName, szBuildNumber, wFlags))
      {
         D(dp( "IsRegisteredUser: BuildRegNum failed" )) ;
         return FALSE ;
      }

      // If they compare then write to disk
      if (CompareRegNums( lpszNumber, szBuildNumber ))
      {
         char szTemp[13] ;
         wsprintf( szTemp, (LPSTR)"%s.ini", lpszAppName ) ;

         WritePrivateProfileString( "Registration",
                                    "Name",
                                    lpszName,
                                    szTemp ) ;

         WritePrivateProfileString( "Registration",
                                    "Number",
                                    szBuildNumber,
                                    szTemp ) ;
      }
      else
         return FALSE ;
   }
   else
   {
      char szTemp[13] ;
      wsprintf( szTemp, (LPSTR)"%s.ini", lpszAppName ) ;

      // wFlags == REGISTER_READ == 0
      GetPrivateProfileString( "Registration",
                                 "Name",
                                 "Not Registered",
                                 szName,
                                 REG_MAX_NAME+1,
                                 szTemp ) ;
      D(dp( "Got Name: '%s'", (LPSTR)szName )) ;

      GetPrivateProfileString( "Registration",
                                 "Number",
                                 "********",
                                 szNumber,
                                 REG_MAX_NAME+1,
                                 szTemp ) ;
                                 
      D(dp( "Got Number: '%s'", (LPSTR)szNumber )) ;

      // Call first with magic cookie number
      BuildRegNum( NULL, NULL, NULL, MAGIC_COOKIE ) ;

      // Build a registration number based on INI file
      if (!BuildRegNum( lpszAppName, szName, szBuildNumber, wFlags))
      {
         D(dp( "IsRegisteredUser: BuildRegNum failed" )) ;
         return FALSE ;
      }

      n = CompareRegNums( szBuildNumber, szNumber ) ;

      // if they compare and lpszName is not NULL copy
      if (n && lpszName)   
      {
         lstrcpy( lpszName, szName ) ;
         lstrcpy( lpszNumber, szBuildNumber ) ; // use uppercase
      }
 
      D(dp( "IsRegisteredUser == %d", n )) ;
      return n ;
   }
   D(dp( "Is RegisteredUser == TRUE" )) ;
   return TRUE ;
}

BOOL CompareRegNums( LPSTR lpszNum1, LPSTR lpszNum2 )
{
   // do a string compare (case insensitive)
   return lstrcmpi( lpszNum1, lpszNum2 ) ? FALSE : TRUE ;
}

//-----------------------------------------------------------------
//
// BOOL FAR PASCAL
// RegistrationDialog(  HWND  hWnd,
//                      LPSTR lpszAppName,
//                      WORD  wFlags )
//
// Description:
//    Displays a dialog box allowing the user to input his name and
//    registration number.
//
// Type/Parameter
//          Description
//
// HWND  hWnd
//          Identifies the Window of the caller.
// 
// LPSTR lpszAppName
//          Specifies the name of the application.  Is used as
//          the filename of the INI file, thus must be 8 or less
//          characters long.
//
// WORD wFlags
//          Specifies flags, and may be any of the following.
// 
//          Value
//                Meaning
// 
// Return Value
//          Returns one of the following values:
// 
//             Value
//                      Meaning
// 
//             FALSE
//                      The registration information is invalid, or
//                      does not exist. (the user hit Ignore)
//             TRUE
//                      The registration information is correct.
//
// Comments
//    May add flags later.
// 
// Modifications
//    CEK   11/20/90    First version
//-----------------------------------------------------------------
typedef struct tagREGSTRUCT
{
   char  szAppName[64] ;
   char  szName[REG_MAX_NAME+1] ;
   char  szNumber[REG_MAX_NUMBER+1] ;
   WORD  wFlags ;
} REGSTRUCT, *PREGSTRUCT ;


// #14
BOOL FAR PASCAL
RegistrationDialog(  HWND  hwndParent,
                     LPSTR lpszAppName,
                     WORD  wFlags )
{
   FARPROC      lpfnDlgProc ;
   PREGSTRUCT   pRS ;
   LOCALHANDLE  hlmemRS ;
   int          n ;

   if (!(hlmemRS = LocalAlloc( LHND, sizeof( REGSTRUCT ) )))
      return DLG_ERROR ;

   pRS = (PREGSTRUCT)LocalLock( hlmemRS ) ;

   // Setup struct to pass to dlg box.
   lstrcpy( pRS->szAppName, lpszAppName ) ;
   lstrcpy( pRS->szName, "" ) ;
   pRS->wFlags = wFlags ;

   LocalUnlock( hlmemRS ) ;

   lpfnDlgProc = MakeProcInstance( RegisterDlgProc, hInst ) ;

   n = DialogBoxParam(  hInst,      
                        "REGISTERBOX",
                        hwndParent,        
                        lpfnDlgProc,
                        (LONG)hlmemRS ) ;

   FreeProcInstance( lpfnDlgProc ) ;

   LocalFree( hlmemRS ) ;
   D(dp( "RegistrationDialog == %d", n)) ;

   return n == IDCANCEL ? FALSE : n ;
}

//-----------------------------------------------------------------
// BOOL FAR PASCAL RegisterDlgProc
//-----------------------------------------------------------------
// #16
BOOL FAR PASCAL
   RegisterDlgProc( HWND hWnd,
                    unsigned iMessage,
                    WORD wParam,
                    LONG lParam )
{
   LOCALHANDLE hlmemRS ;
   PREGSTRUCT  pRS ;

   hlmemRS = GetProp( hWnd, "REGSTRUCT" ) ;

   switch (iMessage)
   {
      case WM_INITDIALOG:
            hlmemRS = (LOCALHANDLE)lParam ;
            pRS = (PREGSTRUCT)LocalLock( hlmemRS ) ;
            
            SetDlgItemText( hWnd, IDD_APPNAME, pRS->szAppName ) ;

            SendMessage( GetDlgItem( hWnd, IDD_NAME ),
                        EM_LIMITTEXT,
                        REG_MAX_NAME, 0L ) ;
            SetDlgItemText( hWnd, IDD_NAME, pRS->szName ) ;

            SendMessage( GetDlgItem( hWnd, IDD_NUM ),
                         EM_LIMITTEXT, REG_MAX_NUMBER, 0L ) ;

            LocalUnlock( hlmemRS ) ;
            SetProp( hWnd, "REGSTRUCT", hlmemRS ) ;
            break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDCANCEL :       // Ignore button
               MessageBeep( 0 ) ;
               MessageBeep( 0 ) ;
               RemoveProp( hWnd, "REGSTRUCT" ) ;
               EndDialog( hWnd, DLG_CANCEL ) ;
               break ;

            case IDOK :
            {
               char  szName [REG_MAX_NAME+1] ;
               char  szNum  [REG_MAX_NUMBER+1] ;

               hlmemRS = GetProp( hWnd, "REGSTRUCT" ) ;
               pRS = (PREGSTRUCT)LocalLock( hlmemRS ) ;

               GetDlgItemText( hWnd, IDD_NAME, szName, REG_MAX_NAME+1) ;
               GetDlgItemText( hWnd, IDD_NUM, szNum, REG_MAX_NUMBER+1) ;
               if (!IsRegisteredUser( pRS->szAppName,
                                      szName,
                                      szNum,
                                      REGISTER_WRITE ))
               {
                  MessageBox( hWnd,
                       "Invalid registration information.\nPlease re-enter.",
                       pRS->szAppName, MB_ICONEXCLAMATION ) ;
                  SetFocus( GetDlgItem( hWnd, IDD_NAME ) ) ;
                  LocalUnlock( hlmemRS ) ;
               }
               else
               {
                  lstrcpy( pRS->szName, szName ) ;
                  lstrcpy( pRS->szNumber, szNum ) ;
                  LocalUnlock( hlmemRS ) ;
                  RemoveProp( hWnd, "REGSTRUCT" ) ;
                  EndDialog( hWnd, DLG_OK ) ;
               }
               break ;
            }

            default :
               return FALSE ;
         }
         break ;

      case WM_PAINT :
      {
         HDC   hDC ;
         PAINTSTRUCT ps ;
         TEXTMETRIC  tm ;

         hDC = BeginPaint (hWnd, &ps) ;
         GetTextMetrics (hDC, &tm) ;
         DrawShadow (hDC, GetDlgItem (hWnd, IDD_BOX_0), GetStockObject(GRAY_BRUSH), tm.tmAveCharWidth / 2) ;
         DrawShadow (hDC, GetDlgItem (hWnd, IDD_BOX_1), GetStockObject(GRAY_BRUSH), tm.tmAveCharWidth / 2) ;
         EndPaint (hWnd, &ps) ;
         return FALSE ;
      }

      default:
         return FALSE ;
   }
   return TRUE ;
}

//-----------------------------------------------------------------
//
// BOOL FAR PASCAL
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
// #15
BOOL FAR PASCAL
BuildRegNum( LPSTR lpszAppName,
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
      char szName [REG_MAX_NAME+1] ;
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
