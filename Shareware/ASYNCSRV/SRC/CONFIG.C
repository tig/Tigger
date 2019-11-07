/************************************************************************
 *
 *     Copyright (c) 1992-93 C.E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  ASYNCSRV 
 *
 *      Module:  config.c
 *
 *     Remarks:  Saves and restores default values
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"

#include "ASYNCSRV.h"
#include "config.h"

/****************************************************************
 *  VOID WINAPI SaveOptions()
 *
 *  Description: 
 *
 *    Saves options settable by the Options menu item.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI SaveOptions()
{
   char szBuf[256] ;

   wsprintf( szBuf, rglpsz[IDS_INI_OPTIONS_SPEC], fStatLine ) ;

   WritePrivateProfileString( rglpsz[IDS_INI_MAIN],
                           rglpsz[IDS_INI_OPTIONS],
                           szBuf,
                           rglpsz[IDS_INI_FILENAME] ) ;
} /* SaveOptions()  */

/****************************************************************
 *  VOID FAR PASCAL RestoreDefaults( VOID )
 *
 *  Description: 
 *
 *    Restores all default values from the ini.
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL RestoreDefaults( VOID )
{
   char szBuf[64] ;
   PSTR psz ;

   fInternal = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                     rglpsz[IDS_INI_INTERNAL],
                                     FALSE, rglpsz[IDS_INI_FILENAME] ) ;

   if (GetPrivateProfileString( rglpsz[IDS_INI_MAIN],
                                rglpsz[IDS_INI_OPTIONS],
                                rglpsz[IDS_DEFAULTOPT],
                                szBuf,
                                64,
                                rglpsz[IDS_INI_FILENAME] ))
   {
      wsscanf( szBuf, rglpsz[IDS_INI_POS_SPEC], (LPSTR)&fStatLine ) ;
   }
   else
   {
      fStatLine = TRUE ;
   }

   szBuf[0] = '\0' ;

   GetPrivateProfileString( rglpsz[IDS_INI_MAIN],
                            rglpsz[IDS_INI_FONT],
                            rglpsz[IDS_DEFAULTFONT],
                            szBuf,
                            64,
                            rglpsz[IDS_INI_FILENAME] ) ;
   if (szBuf[0] == '\0')
      lstrcpy( szBuf, rglpsz[IDS_DEFAULTFONT] ) ;

   wsscanf( szBuf, "%d", (LPWORD)&wFaceSize ) ;
                                     
   if (strtok( szBuf, "," ))
   {
      if (psz = strtok( NULL, "," ))
      {
         lstrcpy( szFaceName, psz ) ;
         if (psz = strtok( NULL, "," ))
            lstrcpy( szFaceStyle, psz ) ;
         else
            lstrcpy( szFaceStyle, "Regular" ) ;

      }
      else
         lstrcpy( szFaceName, "Courier New" ) ;
   }
   else
   {
      lstrcpy( szFaceName, "Courier New" ) ;
      lstrcpy( szFaceStyle, "Regular" ) ;
   }

   DP5( hWDB, "Read font: Name = %s, Style = %s", (LPSTR)szFaceName, (LPSTR)szFaceStyle ) ;

} /* RestoreDefaults()  */


/****************************************************************
 *  VOID FAR PASCAL RestoreWndPos( HWND hWnd )
 *
 *  Description: 
 *
 *    Moves and sizes the main window based on the INI file
 *    settings.
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL RestoreWndPos( HWND hWnd )
{
   if (fInternal)
   {
      SetWindowPlacement( hWnd,
         GetPrivateProfileWndPos( rglpsz[IDS_INI_MAIN],
                                  rglpsz[IDS_INI_POS],
                                  rglpsz[IDS_INI_FILENAME] ) ) ;

      ShowWindow( hWnd, SW_SHOW ) ;
   }

} /* RestoreWndPos()  */

/****************************************************************
 *  VOID FAR PASCAL SaveWndPos( HWND hWnd )
 *
 *  Description: 
 *
 *    Saves the current window pos and size.
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL SaveWndPos( HWND hWnd )
{
   if (fInternal)
      WritePrivateProfileWndPos( rglpsz[IDS_INI_MAIN],
                              rglpsz[IDS_INI_POS],
                              hWnd,
                              rglpsz[IDS_INI_FILENAME] ) ;

} /* SaveWndPos()  */



/************************************************************************
 * End of File: config.c
 ***********************************************************************/
