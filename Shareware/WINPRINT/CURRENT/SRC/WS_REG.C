/* registration routines for WinPrint */
#include "PRECOMP.H"

#include "winprint.h"
#include "isz.h"
#include "dlgs.h"
#include "ws_reg.h"

#define MAGIC_COOKIE 7431

BOOL NEAR PASCAL CompareRegNums( LPSTR lpszNum1, LPSTR lpszNum2 ) ;
BOOL NEAR PASCAL BuildRegNum( LPSTR lpszAppName,
                              LPSTR lpszName,
                              LPSTR lpszNumber,
                              WORD  wFlags ) ;
BOOL NEAR PASCAL ValidateNewReg( LPSTR lpszName, LPSTR lpszNumber ) ;

PSTR NEAR PASCAL CreateRegistration (PSTR psNum, PSTR psName) ;


BOOL FAR PASCAL CheckRegistration (HANDLE hInst)
{
   char  szNum [REG_NUM_LEN+1] ;

   /*
    * First try the new style.  
    */
   if (GetPrivateProfileString( GRCS(IDS_INI_MAIN),
                                GRCS(IDS_INI_MAIN_NAME),
                                "", szRegisteredName, REG_NAME_LEN,
                                GRCS(IDS_INI_FILENAME)) &&
       GetPrivateProfileString( GRCS(IDS_INI_MAIN),
                                GRCS(IDS_INI_MAIN_CODE),
                                "", szNum, REG_NUM_LEN + 1,
                                GRCS(IDS_INI_FILENAME)) &&
       ValidateNewReg( szRegisteredName, szNum ))
   {
      DP1( hWDB, "Valid registration %s, %s",
         (LPSTR)szRegisteredName, (LPSTR)szNum ) ;
      return TRUE ;
   }

   /*
    * Ok, so the new style was not there.  There might be a possibilty
    * that the old style is in the WINRPITN.INI file somehow.
    */
   if (VerifyRegistration( szRegisteredName, szNum ) ||
      (GetProfileString( GRCS(IDS_APPNAME), GRCS(IDS_OLDINI_CODE), "",
                     szNum, REG_NUM_LEN + 1) &&
       GetProfileString( GRCS(IDS_APPNAME), GRCS(IDS_OLDINI_NAME),
                     GRCS(IDS_PLEASEPAY),
                     szRegisteredName, REG_NAME_LEN) &&
       VerifyRegistration( szRegisteredName, szNum )))
   {
      /*
       * Generate a new one using the name we found.
       */
      BuildRegNum( NULL, NULL, NULL, MAGIC_COOKIE ) ;
      if (!BuildRegNum( GRCS(IDS_APPNAME), szRegisteredName, szNum, 0))
      {
         DP1( hWDB, "Problem building NEW reg number" ) ;
         return FALSE ;
      }

      WritePrivateProfileString( GRCS(IDS_INI_MAIN),
                                 GRCS(IDS_INI_MAIN_NAME),
                                 szRegisteredName,
                                 GRCS(IDS_INI_FILENAME) ) ;

      WritePrivateProfileString( GRCS(IDS_INI_MAIN),
                                 GRCS(IDS_INI_MAIN_CODE),
                                 szNum,
                                 GRCS(IDS_INI_FILENAME) ) ;

      return TRUE ;                    
   }

   /*
    * Failed!
    */
   lstrcpy( szRegisteredName, GRCS(IDS_PLEASEPAY) ) ;

   return FALSE ;
}


PSTR NEAR PASCAL CreateRegistration (PSTR psNum, PSTR psName)
{
   PSTR  psCurrent ;
   PSTR  psTemp ;
   WORD  crc = 486 ;
   char  szName [REG_NAME_LEN + 1] ;
   char  szText [80] ;

   psNum[0] = '\0' ;

   lstrcpy (szName, psName) ;

   if (!(psCurrent = strtok (szName, " ")))
      return psNum ;

   psNum[0] = 'W' ;
   psNum[1] = '\0' ;

   do
   {
      for (psTemp = psCurrent  ; *psTemp != '\0' ; psTemp++)
         crc += *psTemp ;
      wsprintf ((LPSTR)szText, (LPSTR)"%2.2X", LOBYTE(crc)) ;
      lstrcat (psNum, szText) ;
      crc += 486 ; 
   }
   while ((lstrlen (psNum) < REG_NUM_LEN - 2)  && (psCurrent = strtok (NULL, " "))) ;

   crc = 0 ;
   while (lstrlen (psNum) < REG_NUM_LEN)
   {
      for (psTemp = psName ; *psTemp != '\0' ; psTemp++)
         crc += *psTemp ;

      wsprintf ((LPSTR)szText, (LPSTR)"%2.2X", LOBYTE (crc)) ;
      lstrcat (psNum, szText) ;
   }

   return psNum ;
}

BOOL FAR PASCAL VerifyRegistration (PSTR psName, PSTR psNumber)
{
   char  szNum [REG_NUM_LEN+1] ;

   if (ValidateNewReg( (LPSTR)psName, (LPSTR)psNumber ))
      return TRUE ;
   else
      return !lstrcmpi (CreateRegistration (szNum, psName), psNumber) ;
}

BOOL NEAR PASCAL CompareRegNums( LPSTR lpszNum1, LPSTR lpszNum2 )
{
   // do a string compare (case insensitive)
   return lstrcmpi( lpszNum1, lpszNum2 ) ? FALSE : TRUE ;
}


BOOL NEAR PASCAL ValidateNewReg( LPSTR lpszName, LPSTR lpszNumber )
{
   char szNum[REG_NEW_NUM_LEN+1] ;

   BuildRegNum( NULL, NULL, NULL, MAGIC_COOKIE ) ;
   if (!BuildRegNum( GRCS(IDS_APPNAME), lpszName, szNum, 0))
   {
      DP1( hWDB, "Problem building reg number for validation" ) ;
      return FALSE ;
   }

   return CompareRegNums( szNum, lpszNumber ) ;
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
      char szName [REG_NAME_LEN+1] ;
      LPSTR lpsFirst ;
      LPSTR lpsMiddle ;
      LPSTR lpsLast ;

      // Build CRC for AppName (wWW)
      for (lpsCur = lpszAppName ; *lpsCur != '\0' ; lpsCur++)
         wWW += (BYTE)*lpsCur ;

      // Break Name up into sub parts (separated by spaces)

      lstrcpy( szName, lpszName ) ;
      #ifdef WIN32
      lpsFirst  = strtok( szName, " " ) ;
      lpsMiddle = strtok( NULL, " " ) ;
      lpsLast   = strtok( NULL, " ") ;
      #else
      lpsFirst  = _fstrtok( szName, " " ) ;
      lpsMiddle = _fstrtok( NULL, " " ) ;
      lpsLast   = _fstrtok( NULL, " ") ;
      #endif

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

