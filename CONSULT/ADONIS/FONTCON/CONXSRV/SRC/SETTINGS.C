/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CONXSRV
 *
 *      Module:  settings.c
 *
 *     Remarks:  Deals with reading settings from the clients ini file...
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "CONXSRV.h"
#include "connect.h"
#include "settings.h"
#include "fsm.h"

static char szPhoneNumber[MAX_PHONENUMBER+1] ;

VOID  NEAR PASCAL MungeModemString( LPSTR lpIn ) ;

/****************************************************************
 *  VOID NEAR PASCAL MungeModemString( LPSTR lpIn )
 *
 *  Description: 
 *
 *    Converts control character specifiers in the modem string
 *    into 'machine readable' form...
 *
 *    Modifies the passed string, converting '^M' to ASCII(^M).
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL MungeModemString( LPSTR lpIn )
{
   LPSTR p ;

   for (p = lpIn ; *p ; p++)
   {
      if (*p == '^')
      {
         /* Control char prefix so munge away
          */
         if (*(p+1))
         {
            LPSTR q ;

            *p = *(p+1) - (BYTE)'A' + (BYTE)1 ;

            for (q = p+1 ; *q ; q++)
               *q = *(q+1) ;
         }
      }
   }

} /* MungeModemString()  */

/****************************************************************
 *  BOOL WINAPI ReadModemSettings( LPCONNECTFSM lpFSM )
 *
 *  Description: 
 *
 *    Reads the MODEM.INI file, using the CommSettings.szModemType
 *    member of lpFSM to determine which modem to use.
 *
 *    Fills out the CONNECTFSM structure.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI ReadModemSettings( LPCONNECTFSM lpFSM )
{
   char  szModemINI[144] ;
   PSTR  pszModem ;
   char  szBuf[MAX_MODEMCMD+1] ;
   LPSTR lpszNumber ;

   GetModuleFileName( hinstApp, szModemINI, 144 ) ;
   if (pszModem = strrchr( szModemINI, '\\' ))
      lstrcpy( pszModem+1, rglpsz[IDS_MODEMINI] ) ;
   else
      lstrcpy( szModemINI, rglpsz[IDS_MODEMINI] ) ;

#ifdef DISABLE   
   /*
    * First do a sanity check to make sure the modem is even
    * in the file.
    */
   if (!GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                                 NULL, "", szBuf, 8, szModemINI ))

   {
      DP1( hWDB, "ReadModemSettings failed: No section for %s",
                  (LPSTR)lpFSM->CommSettings.szModemType ) ;
      return FALSE ;
   }
#endif

   switch( lpFSM->CommSettings.dwBaudRate )
   {
      case 9600:
      case 4800:
         lpszNumber = rglpsz[IDS_PHONE9600] ;
      break ;

      case 14000:
         lpszNumber = rglpsz[IDS_PHONE14K] ;
      break ;

      default:
         lpszNumber = rglpsz[IDS_PHONE2400] ;
      break ;
   }

   GetPrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS],
                            rglpsz[IDS_INI_PHONENUMBER],
                            lpszNumber,
                            szPhoneNumber,
                            MAX_PHONENUMBER+1,
                            rglpsz[IDS_INI_FILENAME] ) ;


   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                                 rglpsz[IDS_MODEMINI_INIT],
                                 rglpsz[IDS_MODEMINI_DEF_INIT],
                                 lpFSM->szInit,
                                 MAX_MODEMCMD+1,
                                 szModemINI ) ;
   MungeModemString( lpFSM->szInit ) ;
   lpFSM->lpszInitCur = lpFSM->szInit ;

   /*
    * We don't need to check the results of the rest 'cause we
    * can use defaults.
    */
   if (lpFSM->CommSettings.uiDialType == DIALTYPE_TONE)
   {   
      GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                              rglpsz[IDS_MODEMINI_DIALT],
                              rglpsz[IDS_MODEMINI_DEF_DIALT],
                              szBuf,
                              MAX_MODEMCMD+1,
                              szModemINI ) ;
   }
   else
   {   
      GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                              rglpsz[IDS_MODEMINI_DIALP],
                              rglpsz[IDS_MODEMINI_DEF_DIALP],
                              szBuf,
                              MAX_MODEMCMD+1,
                              szModemINI ) ;
   }

   if (lpFSM->CommSettings.uiLocationFlags == LOCATION_OTHER)
   {
      wsprintf( lpFSM->szDial, "%s%s%s%s%s^M",
             (LPSTR)szBuf,
             (LPSTR)lpFSM->CommSettings.szPrefix,
             (LPSTR)rglpsz[IDS_ADONISPHONEPREFIX],
             (LPSTR)szPhoneNumber,
             (LPSTR)lpFSM->CommSettings.szSuffix ) ;
   }
   else
   {
      wsprintf( lpFSM->szDial, "%s%s%s%s^M",
             (LPSTR)szBuf,
             (LPSTR)lpFSM->CommSettings.szPrefix,
             (LPSTR)szPhoneNumber,
             (LPSTR)lpFSM->CommSettings.szSuffix ) ;
   }

   MungeModemString( lpFSM->szDial ) ;

   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEMINI_HANGUP],
                            rglpsz[IDS_MODEMINI_DEF_HANGUP],
                            lpFSM->szHangUp,
                            MAX_MODEMCMD+1,
                            szModemINI ) ;

   MungeModemString( lpFSM->szHangUp ) ;
   lpFSM->lpszHangUpCur = lpFSM->szHangUp ;

   /* Modem response strings
    */
   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEMINI_CONN],
                            rglpsz[IDS_MODEMINI_DEF_CONN],
                            lpFSM->szConnect,
                            MAX_MODEMRESP+1,
                            szModemINI ) ;
   MungeModemString( lpFSM->szConnect ) ;

   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEMINI_CONN300],
                            rglpsz[IDS_MODEMINI_DEF_CONN300],
                            lpFSM->szConnect300,
                            MAX_MODEMRESP+1,
                            szModemINI ) ;
   MungeModemString( lpFSM->szConnect300 ) ;

   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEMINI_CONN1200],
                            rglpsz[IDS_MODEMINI_DEF_CONN1200],
                            lpFSM->szConnect1200,
                            MAX_MODEMRESP+1,
                            szModemINI ) ;
   MungeModemString( lpFSM->szConnect1200 ) ;

   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEMINI_CONN2400],
                            rglpsz[IDS_MODEMINI_DEF_CONN2400],
                            lpFSM->szConnect2400,
                            MAX_MODEMRESP+1,
                            szModemINI ) ;
   MungeModemString( lpFSM->szConnect2400 ) ;

   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEMINI_CONN4800],
                            rglpsz[IDS_MODEMINI_DEF_CONN4800],
                            lpFSM->szConnect4800,
                            MAX_MODEMRESP+1,
                            szModemINI ) ;
   MungeModemString( lpFSM->szConnect4800 ) ;

   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEMINI_CONN9600],
                            rglpsz[IDS_MODEMINI_DEF_CONN9600],
                            lpFSM->szConnect9600,
                            MAX_MODEMRESP+1,
                            szModemINI ) ;
   MungeModemString( lpFSM->szConnect9600 ) ;

   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEMINI_BUSY],
                            rglpsz[IDS_MODEMINI_DEF_BUSY],
                            lpFSM->szBusy,
                            MAX_MODEMRESP+1,
                            szModemINI ) ;
   MungeModemString( lpFSM->szBusy ) ;

   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEMINI_NOTONE],
                            rglpsz[IDS_MODEMINI_DEF_NOTONE],
                            lpFSM->szNoDialtone,
                            MAX_MODEMRESP+1,
                            szModemINI ) ;
   MungeModemString( lpFSM->szNoDialtone ) ;

   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEMINI_NOCARRIER],
                            rglpsz[IDS_MODEMINI_DEF_NOCARRIER],
                            lpFSM->szNoCarrier,
                            MAX_MODEMRESP+1,
                            szModemINI ) ;
   MungeModemString( lpFSM->szNoCarrier ) ;

   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEMINI_ERROR],
                            rglpsz[IDS_MODEMINI_DEF_ERROR],
                            lpFSM->szError,
                            MAX_MODEMRESP+1,
                            szModemINI ) ;
   MungeModemString( lpFSM->szError ) ;

   GetPrivateProfileString( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEMINI_OK],
                            rglpsz[IDS_MODEMINI_DEF_OK],
                            lpFSM->szOk,
                            MAX_MODEMRESP+1,
                            szModemINI ) ;
   MungeModemString( lpFSM->szOk ) ;

   lpFSM->fIgnoreCD = GetPrivateProfileInt( lpFSM->CommSettings.szModemType,
                            rglpsz[IDS_MODEM_INI_IGNORECD],
                            FALSE,
                            szModemINI ) ;


   return TRUE ;
} /* ReadModemSettings()  */



/************************************************************************
 * End of File: settings.c
 ***********************************************************************/

