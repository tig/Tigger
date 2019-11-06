/************************************************************************
 *
 *     Project:  WinWord support module for CEKLIB (registration)
 *
 *      Module:  reglib.c
 *
 *     Remarks:  Main module
 *
 *   Revisions:  
 *
 ***********************************************************************/

#define STRICT
#define WINDLL
#include <windows.h>
#include <string.h>

#include <wdb.h>

#include "reglib.h"
#include "version.h"


//----------------------------------------------------------------
// Local defines
//----------------------------------------------------------------
#define MAGIC_COOKIE    7431

HANDLE   hInstDLL ;
DWORD    dwVersion ;       // HIWORD is build, LOWORD is version * 100
char     szVersion[32] ;
char     szModule[144] ;

LPSTR NEAR PASCAL CreateOldWinPrintReg( LPSTR lpszNum, LPSTR lpszName);

BOOL WINAPI BuildRegNum( LPSTR lpszAppName,
             LPSTR lpszName,
             LPSTR lpszNumber,
             WORD  wFlags ) ;

int FAR PASCAL LibMain( HANDLE hModule, WORD wDS, WORD cbHeap, LPSTR lpszCmdLine )
{
   if (cbHeap == 0)
      return FALSE ;

   hInstDLL    = hModule ;
   dwVersion   = MAKELONG( (VER_MAJOR * 100) + (VER_MINOR), VER_BUILD ) ;


   D( hWDB = wdbOpenSession( NULL, "reglib", "wdb.ini", WDB_LIBVERSION ) ) ;

//   D( w = wdbGetOutput( hWDB ) ) ;

   D( rlGetVersion( szVersion ) ) ;
   D( GetModuleFileName( hInstDLL, szModule, 144 ) ) ;  
   D( DP( hWDB, "\n*** %s Version %s ***", (LPSTR)szModule,
                                       (LPSTR)szVersion ) ) ;

//   D( wdbSetOutput( hWDB, w, NULL ) ) ;

   return TRUE ;
}

/*************************************************************************
 *  HANDLE FAR PASCAL rlGethInst( VOID )
 *
 *  Description: 
 *
 *    Returns the hInsrlLL of the reglib.
 *
 *  Comments:
 *
 *************************************************************************/
HANDLE __export  WINAPI rlGethInst( VOID )
{
   return hInstDLL ;

}/* rlGethInst() */


BOOL __export WINAPI rlCheckVersion( DWORD dwVer )
{
   return (LOWORD( dwVer ) <= _REGLIB_SPEC_VER_ ) ;
}

DWORD __export WINAPI rlGetVersion( LPSTR lpszVersion ) 
{
   if ( VER_BUILD )
      wsprintf( lpszVersion, (LPSTR)"%d.%.2d.%.3d",
                VER_MAJOR, VER_MINOR, VER_BUILD ) ;
   else
      wsprintf( lpszVersion, (LPSTR)"%d.%.2d", VER_MAJOR, VER_MINOR ) ;

   return dwVersion ;
}


/****************************************************************
 *  VOID FAR PASCAL rlGetRegNumber( LPSTR lpszApp, LPSTR lpszName, LPSTR lpszNum )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID __export WINAPI rlGetRegNumber( LPSTR lpszApp, LPSTR lpszName, LPSTR lpszNum )
{
   DP1( hWDB, "rlGetRegNumber( %s, %s, %s )",
               (LPSTR)lpszApp,
               (LPSTR)lpszName,
               (LPSTR)lpszNum ) ;

   if (!lstrcmpi( lpszApp, "WinPrint1.3" ))
   {
      CreateOldWinPrintReg( lpszNum, lpszName ) ;
      return ;
   }

   if (!BuildRegNum( NULL, NULL, NULL, MAGIC_COOKIE ))
   {
      DP1( hWDB, "BuildRegNum failed to register the magic cookie" ) ;
   }

   if (!BuildRegNum( lpszApp, lpszName, lpszNum, 0 ))
   {
      DP1( hWDB, "BuildRegNum failed!" ) ;
   }

   return ;

} /* rlGetRegNumber()  */



LPSTR NEAR PASCAL CreateOldWinPrintReg( LPSTR lpszNum, LPSTR lpszName)
{
   LPSTR  lpszCurrent ;
   LPSTR  lpszTemp ;
   WORD  crc = 486 ;
   char  szName [64 + 1] ;
   char  szText [80] ;

   lstrcpy (lpszNum, "W") ;         

   lstrcpy (szName, lpszName) ;
   lpszCurrent = _fstrtok( szName, " " ) ;
   do
   {
      for (lpszTemp = lpszCurrent  ; *lpszTemp != '\0' ; lpszTemp++)
         crc += *lpszTemp ;
      wsprintf( szText, "%2.2X", LOBYTE(crc)) ;
      lstrcat (lpszNum, szText) ;
      crc += 486 ; 
   }
   while((lstrlen(lpszNum) < 11 - 2)  && (lpszCurrent = _fstrtok (NULL, " "))) ;

   crc = 0 ;
   while( lstrlen (lpszNum) < 11)
   {
      for (lpszTemp = lpszName ; *lpszTemp != '\0' ; lpszTemp++)
         crc += *lpszTemp ;

      wsprintf (szText, "%2.2X", LOBYTE (crc)) ;
      lstrcat (lpszNum, szText) ;
   }

   return lpszNum ;
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

BOOL WINAPI BuildRegNum( LPSTR lpszAppName,
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
      lpsFirst  = strtok( szName, (LPSTR)" " ) ;
      lpsMiddle = strtok( NULL, (LPSTR)" " ) ;
      lpsLast   = strtok( NULL, (LPSTR)" ") ;

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
 * End of File: reglib.c
 ************************************************************************/

