/************************************************************************
 *
 *     Project:  INIEDIT 2.0
 *
 *      Module:  config.c
 *
 *     Remarks:  Saves and restores default values
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "iniedit.h"
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "wsscanf.h"
#include "font.h"

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

   wsprintf( szBuf, rglpsz[IDS_INI_OPTIONS_SPEC],
      fToolBar, fStatLine, fToolBarLabels ) ;
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
      wsscanf( szBuf, rglpsz[IDS_INI_POS_SPEC],
               (LPSTR)&fToolBar,
               (LPSTR)&fStatLine,
               (LPSTR)&fToolBarLabels ) ;
   }
   else
   {
      fStatLine = TRUE ;
      fToolBar = TRUE ;
      fToolBarLabels = TRUE ;
   }

   //  !!!! NOT IMPLEMENTED
   fSortSections = TRUE ;
   fSortKeys = TRUE ;

   if (GetPrivateProfileString( rglpsz[IDS_INI_MAIN],
                                rglpsz[IDS_INI_FONT],
                                rglpsz[IDS_DEFAULTFONT],
                                szBuf,
                                64,
                                rglpsz[IDS_INI_FILENAME] ))
   {
      PSTR psz ;

#ifdef DEBUGBRYANSBUG
      short i ;
      
      DP1( hWDB, "hhOutBuf = %04X", hhOutBuf ) ;
      DPF1( hWDB, "   szFont = ( " ) ;
      for (i = 0 ; i < LF_FACESIZE ; i++)
      {
         DPF1( hWDB, "%02.2X ", szFont[i] ) ;
      }
      DP1( hWDB, ")" ) ;


      DP1( hWDB, "   &hhOutBuf   = %08lX", (DWORD)&hhOutBuf ) ;
      DP1( hWDB, "   &wFontSize  = %08lX", (DWORD)&wFontSize ) ;
      DP1( hWDB, "   &wFontStyle = %08lX", (DWORD)&wFontStyle ) ;
      DP1( hWDB, "   &szFont     = %08lX", (DWORD)&szFont ) ;
      DPF1( hWDB, "szBuf = %s\n( ", (LPSTR)szBuf ) ;
      for (i = 0 ; i < 64 ; i++)
      {
         DPF1( hWDB, "%02X ", (WORD)(BYTE)szBuf[i] ) ;
      }
      DP1( hWDB, ")" ) ;
#endif

      /*
       * HACK!!!!
       *
       * wsscanf() has a bug in it (It's bryan's fault).  The code
       * that scans for %s does not stop on a '\0' but keeps on marching
       * until it either gets to n characters or a space.  The hack here
       * is to use a format string with a field width (1).
       */
      wsscanf( szBuf, "%d,%04X,%1s", //rglpsz[IDS_INI_FONT_SPEC],
               (LPWORD)&wFontSize,
               (LPWORD)&wFontStyle,
               (LPSTR)szFont ) ;    // szFont is there just for saftey
                                    // 'cause spec is "%d,%d,%s"

#ifdef DEBUGBRYANSBUG
      DP1( hWDB, "After wsscanf() : hhOutBuf = %04X", hhOutBuf ) ;

      DPF1( hWDB, "   szFont = %s ( ", (LPSTR)szFont ) ;
      for (i = 0 ; i < LF_FACESIZE ; i++)
      {
         DPF1( hWDB, "%02X ", (WORD)(BYTE)szFont[i] ) ;
      }
      DP1( hWDB, ")" ) ;
#endif

      /*
       * HACK!!
       *
       * wsscanf (and sscanf) always scan strings up to space characters
       * thus if the string (i.e. the font) has spaces in it, wsscanf will
       * only return the first word (i.e. "MS Sans Serif" will result in
       * "MS".
       *
       * To get around this we do some parsing ourself and ignore what
       * wsscanf() gave us.
       *
       */
      strtok( szBuf, ", " ) ;
      strtok( NULL, ", " ) ;

      if (psz = strtok( NULL, "," ))
         lstrcpy( szFont, psz ) ;
      else
         lstrcpy( szFont, "Helv" ) ;

   }

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
   DPF5( hWDB, "RestoreWndPos..." ) ;

   if (nWidth)
      SetWindowPos( hWnd, NULL, xPos, yPos, nWidth, nHeight, SWP_NOZORDER ) ;

   if (wShow == SW_SHOWMAXIMIZED)
      ShowWindow( hWnd, wShow ) ;
   else
      ShowWindow( hWnd, nCmdShow ) ;

   DP5( hWDB, "  Done!" ) ;

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
 *  VOID WINAPI SaveOtherFile( LPSTR lpszFile )
 *
 *  Description: 
 *
 *    Saves the file lpszFile to the [Other File] section of the
 *    INIEDIT.INI file.  
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI SaveOtherFile( LPSTR lpszFile )
{
   WritePrivateProfileString( rglpsz[IDS_INI_OTHERFILES],
                              lpszFile, lpszFile,
                              rglpsz[IDS_INI_FILENAME] ) ;
} /* SaveOtherFile()  */

/****************************************************************
 *  VOID WINAPI KillOtherFile( LPSTR lpszFile )
 *
 *  Description: 
 *
 *    Removes the file specified from the other file list.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI KillOtherFile( LPSTR lpszFile )
{
   WritePrivateProfileString( rglpsz[IDS_INI_OTHERFILES],
                              lpszFile, NULL,
                              rglpsz[IDS_INI_FILENAME] ) ;
} /* KillOtherFile()  */


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

