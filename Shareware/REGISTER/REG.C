/************************************************************************
 *
 *     Project:  Shareware Registration System
 *
 *      Module:  reg.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include <windows.h>
#include <stdlib.h>
#include <ceklib.h>

#include "sharereg.h"
#include "resource.h"

/************************************************************************
 * Global vars
 ***********************************************************************/
extern HANDLE  hAppInst ;
extern char    szAppName[] ;
extern HWND    hwndTB ;
extern HWND    rghwndChild[] ;

/************************************************************************
 * Macros
 ***********************************************************************/

/************************************************************************
 * Other defines
 ***********************************************************************/

/************************************************************************
 * Functions
 ***********************************************************************/
BOOL NEAR PASCAL
FindFirstData( LPSTR lpData, LPSTR lpSubData,
                 WORD wFlags, LPSTR lpszResult ) ;

BOOL NEAR PASCAL
FindNextData( LPSTR lpData, LPSTR lpSubData,
                 WORD wFlags, LPSTR lpszResult ) ;

/****************************************************************
 *  BOOL FAR PASCAL FillAppNameCB( VOID )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL FillAppNameCB( VOID )
{
   char szName[_MAX_FNAME+1] ;

   SendMessage( rghwndChild[IDD_APPCB], CB_RESETCONTENT, 0, 0L ) ;
   //
   // Fill the cb with the data
   //
   if (FindFirstData( "SHAREREG.INI", "AppNames", 0, szName ))
   {
      if (CB_ERR == (WORD)SendMessage( rghwndChild[IDD_APPCB], CB_ADDSTRING,
                                 0, (LONG)(LPSTR)szName )) 
      {
         DP1( hWDB, "FillAppName: Insert (1) Failed!" ) ;
         return FALSE ;
      }

      while (FindNextData( "SHAREREG.INI", "AppNames", 0, szName ))
      {
         if (CB_ERR == (WORD)SendMessage( rghwndChild[IDD_APPCB], CB_ADDSTRING,
                                    0, (LONG)(LPSTR)szName )) 
         {
            DP1( hWDB, "FillAppName: Insert (2) Failed!" ) ;
            return FALSE ;
         }
      }

      SendMessage( rghwndChild[IDD_APPCB], CB_SETCURSEL, 0, 0L ) ;

   }
   else
   {
      DP1( hWDB, "FillAppName: FindFirstData failed!" ) ;
      return FALSE ;
   }

   return TRUE ;
} /* FillAppNameCB()  */



#define CURSIZE      2048

static char    s_lpData[CURSIZE] ;
static LPSTR   s_lpCur ;

/****************************************************************
 *  BOOL NEAR PASCAL
 *    FindFirstData( LPSTR lpData, LPSTR lpSubData,
 *                     WORD wFlags, LPSTR lpResult )
 *
 *  Description: 
 *
 *    This function finds the first data item in the data set pointed to
 *    by lpData that falls under the topic pointed to by lpSubData.  If
 *    lpSubData is NULL, then the top level data is used.
 *
 *    The list box item pointed to by lpResult is filled with the data.
 *
 *    If lpSubData cannot be found in the data FALSE is returned.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL
FindFirstData( LPSTR lpData, LPSTR lpSubData,
                 WORD wFlags, LPSTR lpResult )
{
   s_lpCur = s_lpData ;

   if (!GetPrivateProfileString( lpSubData, NULL, "", s_lpData, CURSIZE,
                                 lpData ))
      return FALSE ;

   DP5( hWDB, "lpSubData = %s, lpData = %s, s_lpCur -> %s",
               (LPSTR)lpSubData, (LPSTR)lpData, (LPSTR)s_lpCur ) ;

   return FindNextData( lpData, lpSubData, wFlags, lpResult ) ;

} /* FindFirstData()  */

/****************************************************************
 *  BOOL NEAR PASCAL
 *    FindNextData( LPSTR lpData, LPSTR lpSubData,
 *                     WORD wFlags, LPSTR lpResult )
 *
 *  Description: 
 *
 *    This function finds the next data item in the data set pointed to
 *    by lpData that falls under the topic pointed to by lpSubData.  If
 *    lpSubData is NULL, then the top level data is used.
 *
 *    The list box item pointed to by lpResult is filled with the data.
 *
 *    If lpSubData cannot be found in the data FALSE is returned.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL
FindNextData( LPSTR lpData, LPSTR lpSubData,
                 WORD wFlags, LPSTR lpResult )
{
   DPF5( hWDB, "FindNextData: " ) ;

   if (*s_lpCur == '\0')
   {
      DP5( hWDB, "Done!!" ) ;
      return FALSE ;
   }
   else
   {
      char szBuf[_MAX_FNAME + 1] ;

      if (_MAX_FNAME == GetPrivateProfileString( lpSubData,
                                                  s_lpCur, "",
                                                  szBuf, _MAX_FNAME,
                                                  lpData ))
      {
         return FALSE ;
      }

      s_lpCur = s_lpCur + lstrlen( s_lpCur ) + 1 ;

      lstrcpy( lpResult, szBuf ) ;

      DP5( hWDB, "\"%s\"", (LPSTR)lpResult ) ;

      return TRUE ;
   }

} /* FindNextData()  */


/************************************************************************
 * End of File: reg.c
 ***********************************************************************/

