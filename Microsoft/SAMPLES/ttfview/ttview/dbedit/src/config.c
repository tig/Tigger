/************************************************************************
 *
 *     Project:  DBEDIT 
 *
 *      Module:  config.c
 *
 *     Remarks:  Saves and restores default values
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "DBEDIT.h"
#include <string.h>
#include <stdio.h>
#include "config.h"

/* window xPos,yPos, widht and height */
static WORD  wShow ;          // ShowWindow

/****************************************************************
 *  VOID FAR PASCAL SaveDefaults( VOID )
 *
 *  Description: 
 *
 *    Saves all default values to the INI file.
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL SaveDefaults( VOID )
{
   if (xPos == 0 && yPos == 0 && nWidth == 0 && nHeight == 0)
      return ;

   WriteProfileWORD( rglpsz[IDS_INI_MAIN], rglpsz[IDS_INI_WSHOW], wShow );

   if (wShow == SW_SHOWNORMAL)
   {
      char szBuf[64] ;

      wsprintf( szBuf, rglpsz[IDS_INI_POS_SPEC],
                xPos, yPos, nWidth, nHeight ) ;

      WritePrivateProfileString( rglpsz[IDS_INI_MAIN], rglpsz[IDS_INI_POS],
                                 szBuf,
                                 rglpsz[IDS_INI_FILENAME] ) ;
      DP5( hWDB, "SaveDefaults : %s", (LPSTR)szBuf ) ;
   }

} /* SaveDefaults()  */

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

   wShow = GetProfileWORD( rglpsz[IDS_INI_MAIN],
                           rglpsz[IDS_INI_WSHOW], SW_SHOWNORMAL ) ;

   if (GetPrivateProfileString( rglpsz[IDS_INI_MAIN],
                                rglpsz[IDS_INI_POS],
                                "",
                                szBuf,
                                64,
                                rglpsz[IDS_INI_FILENAME] ))
   {
      wsscanf( szBuf, rglpsz[IDS_INI_POS_SPEC],
               (LPSTR)&xPos,
               (LPSTR)&yPos,
               (LPSTR)&nWidth,
               (LPSTR)&nHeight ) ;
   }

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

   wsscanf( szBuf, "%d", (LPWORD)&wFontSize ) ;
                                     
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
         lstrcpy( szFaceName, "Arial" ) ;
   }
   else
   {
      lstrcpy( szFaceName, "Arial" ) ;
      lstrcpy( szFaceStyle, "Regular" ) ;
   }

   DP3( hWDB, "Read font: Name = %s, Style = %s", (LPSTR)szFaceName, (LPSTR)szFaceStyle ) ;

} /* RestoreDefaults()  */


/****************************************************************
 *  VOID FAR PASCAL RestoreWndPos( HWND hWnd, WORD nCmdShow )
 *
 *  Description: 
 *
 *    Moves and sizes the main window based on the INI file
 *    settings.
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL RestoreWndPos( HWND hWnd, WORD nCmdShow )
{
   DP4( hWDB, "SetWindow Pos..." ) ;

   if (nWidth)
      SetWindowPos( hWnd, NULL, xPos, yPos, nWidth, nHeight, SWP_NOZORDER ) ;

   DP4( hWDB, "ShowWindow..." ) ;

   if (wShow == SW_SHOWMAXIMIZED)
      ShowWindow( hWnd, wShow ) ;
   else
      ShowWindow( hWnd, nCmdShow ) ;

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
   RECT  rc ;

   wShow = SW_SHOWNORMAL ;

   if (IsZoomed( hWnd ))
   {
      wShow = SW_SHOWMAXIMIZED ;
      return ;
   }
    
   if (IsIconic( hWnd ))
   {
      wShow = SW_SHOWMINIMIZED ;
      return ;
   }

   GetWindowRect( hWnd, &rc ) ;
   xPos = rc.left ;
   yPos = rc.top ;
   nWidth = rc.right - rc.left ;
   nHeight = rc.bottom - rc.top ;
} /* SaveWndPos()  */


/****************************************************************
 *  VOID FAR PASCAL WriteProfileWORD( LPSTR lpszSec, LPSTR lpszKey, WORD w )
 *
 *  Description: 
 *
 *    Writes a profile entry in the form of Key=0xPos0000.
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL WriteProfileWORD( LPSTR lpszSec, LPSTR lpszKey, WORD w )
{
   char     ach[32] ;
   WORD     w2 ;

    if (w2 != GetProfileWORD( lpszSec, lpszKey, w ))
    {
        wsprintf( ach, rglpsz[IDS_HEXWORD], w ) ;
        WritePrivateProfileString( lpszSec, lpszKey,
                                    ach, rglpsz[IDS_INI_FILENAME] ) ;
    }

} /* WriteProfileWORD()  */

/****************************************************************
 *  VOID FAR PASCAL
 *    WriteProfileDWORD( LPSTR lpszSec, LPSTR lpszKey, DWORD dw )
 *
 *  Description: 
 *
 *    Writes a profile entry in the form of Key=0xPos00000000.
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL WriteProfileDWORD( LPSTR lpszSec, LPSTR lpszKey, DWORD dw )
{
   char     ach[32] ;
   DWORD    dw2 ;

    if (dw2 != GetProfileDWORD( lpszSec, lpszKey, dw ))
    {
        wsprintf( ach, rglpsz[IDS_HEXDWORD], dw ) ;
        WritePrivateProfileString( lpszSec, lpszKey,
                                   ach, rglpsz[IDS_INI_FILENAME] ) ;
    }

} /* WriteProfileDWORD()  */

/****************************************************************
 *  WORD FAR PASCAL GetProfileWORD( LPSTR lpszSec, LPSTR lpszKey, WORD wDef )
 *
 *  Description: 
 *
 *    Gets a profile entry in the form of key=0xPos0000
 *
 *  Comments:
 *
 ****************************************************************/
WORD FAR PASCAL GetProfileWORD( LPSTR lpszSec, LPSTR lpszKey, WORD wDef )
{
   char     ach[32] ;
   WORD     w ;

   if (GetPrivateProfileString( lpszSec, lpszKey, "",
                                ach, 32, rglpsz[IDS_INI_FILENAME] ) &&
       wsscanf( ach, rglpsz[IDS_HEXWORD], (LPWORD)&w ))
      return w ;
   else
      return wDef ;

} /* GetProfileWORD()  */

/****************************************************************
 *  DWORD FAR PASCAL GetProfileDWORD( LPSTR lpszSec, LPSTR lpszKey, DWORD wDef )
 *
 *  Description: 
 *
 *    Gets a profile entry in the form of key=0xPos0000
 *
 *  Comments:
 *
 ****************************************************************/
DWORD FAR PASCAL GetProfileDWORD( LPSTR lpszSec, LPSTR lpszKey, DWORD dwDef )
{
   char     ach[32] ;
   DWORD    dw ;

   if (GetPrivateProfileString( lpszSec, lpszKey, "",
                                ach, 32, rglpsz[IDS_INI_FILENAME] ) &&
       wsscanf( ach, rglpsz[IDS_HEXDWORD], (LPDWORD)&dw ))
      return dw ;
   else
      return dwDef ;

} /* GetProfileDWORD()  */


/************************************************************************
 * End of File: config.c
 ***********************************************************************/

