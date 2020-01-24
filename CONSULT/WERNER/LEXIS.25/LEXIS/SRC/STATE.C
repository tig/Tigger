/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  state.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:  
 *
 *    This module handles the saving and restoring of Lexis' state
 *    information from the INI files.  It also handles stuff like
 *    changing (setting) the personality (i.e. CORP, LEGAL, COMBINED).
 *
 *   Revisions:  
 *     00.00.000  1/25/91 cek   First version
 *
 ************************************************************************/

#include "..\inc\undef.h"
#include <windows.h>
#include <cmndll.h>

#include "..\inc\lexis.h"
#include "..\inc\global.h"
#include "..\inc\profile.h"
#include "..\inc\state.h"
#include "..\inc\menu.h"
#include "..\inc\button.h"

/************************************************************************
 * Imported variables
 ************************************************************************/

/************************************************************************
 * Local Defines
 ************************************************************************/

/************************************************************************
 * Macros 
 ************************************************************************/

/************************************************************************
 * Local data structures
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/
BOOL NEAR PASCAL
WriteFont( HANDLE hInst, LPSTR szKey, LPEXTLOGFONT lplf ) ;
BOOL NEAR PASCAL
ReadFont( HANDLE hInst, LPSTR szKey, LPEXTLOGFONT lplf ) ;

BOOL NEAR PASCAL ReadFlag( HANDLE hInst, WORD wSection, WORD wFlag, BOOL f ) ;
int  NEAR PASCAL ReadInt( HANDLE hInst, WORD wSection, WORD wFlag, int n ) ;
BOOL NEAR PASCAL WriteFlag( HANDLE hInst, WORD wSection, WORD wKey, WORD wVal ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * External\exported Functions
 ************************************************************************/
/*************************************************************************
 *  BOOL FAR PASCAL SaveScreenPosition( HWND hWnd )
 *
 *  Description:
 *    This functions saves the current window position and whether the
 *    window was maximized or "normal".
 *
 *    The flags are saved to the lexis.ini file in the [Window Settings]
 *    section.
 *
 *  Type/Parameter
 *          Description
 *    HWND  hWnd
 *          Handle to the main window.
 * 
 *  Return Value
 *
 *            BOOL TRUE  sucessful
 *                 FALSE failure
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL SaveScreenPosition( HWND hWnd )
{
   HANDLE   hInst = GETHINST( hWnd ) ;
   char     szSection[MAX_INILEN+1] ;
   char     szKey[MAX_INILEN+1] ;
   RECT     rc ;

   if (IsIconic( hWnd ))
      return TRUE ;

   if (!LoadString( hInst, IDS_INI_WIN, szSection, MAX_INILEN ))
      return FALSE ;

   if (!LoadString( hInst, IDS_INI_WIN_NCMDSHOW, szKey, MAX_INILEN ))
      return FALSE ;

   if (IsZoomed( hWnd ))
   {
      ProfileWriteInt( szSection, szKey, SW_SHOWMAXIMIZED ) ;
      return TRUE ;
   }
   else
   {
      ProfileWriteInt( szSection, szKey, SW_SHOWNORMAL ) ;

      GetWindowRect( hWnd, &rc ) ;

      if (LoadString( hInst, IDS_INI_WIN_XPOS, szKey, MAX_INILEN ))
         ProfileWriteInt( szSection, szKey, rc.left ) ;

      if (LoadString( hInst, IDS_INI_WIN_YPOS, szKey, MAX_INILEN ))
         ProfileWriteInt( szSection, szKey, rc.top ) ;

      if (LoadString( hInst, IDS_INI_WIN_XSIZE, szKey, MAX_INILEN ))
         ProfileWriteInt( szSection, szKey, rc.right - rc.left ) ;

      if (LoadString( hInst, IDS_INI_WIN_YSIZE, szKey, MAX_INILEN ))
         ProfileWriteInt( szSection, szKey, rc.bottom - rc.top ) ;
   }

   return TRUE ;

}/* SaveScreenPosition() */

/*************************************************************************
 *  WORD FAR PASCAL ReadScreenX( HANDLE hInst )
 *
 *  Description:
 *    Reads LEXIS.INI, getting the Xpos value.
 *
 *  Type/Parameter
 *          Description
 *
 *   HWND   hWnd
 *          Handle to the main window.
 * 
 *  Return Value
 *
 *    Returns the X position of the window as stored in the ini file.
 *
 *  Comments:
 *
 *************************************************************************/
WORD FAR PASCAL ReadScreenX( HANDLE hInst )
{
   return ReadInt( hInst, IDS_INI_WIN, IDS_INI_WIN_XPOS, CW_USEDEFAULT ) ;

}/* ReadScreenX() */

/*************************************************************************
 *  WORD FAR PASCAL ReadScreenY( HANDLE hInst )
 *
 *  Description:
 *    Reads LEXIS.INI, getting the Ypos value.
 *
 *  Type/Parameter
 *          Description
 *
 *   HWND   hWnd
 *          Handle to the main window.
 * 
 *  Return Value
 *
 *    Returns the Y position of the window as stored in the ini file.
 *
 *  Comments:
 *
 *************************************************************************/
WORD FAR PASCAL ReadScreenY( HANDLE hInst )
{
   return ReadInt( hInst, IDS_INI_WIN, IDS_INI_WIN_YPOS, CW_USEDEFAULT ) ;

}/* ReadScreenY() */

/*************************************************************************
 *  WORD FAR PASCAL ReadScreenXsize( HANDLE hInst )
 *
 *  Description:
 *    Reads LEXIS.INI, getting the Xpos value.
 *
 *  Type/Parameter
 *          Description
 *
 *   HWND   hWnd
 *          Handle to the main window.
 * 
 *  Return Value
 *
 *    Returns the X position of the window as stored in the ini file.
 *
 *  Comments:
 *
 *************************************************************************/
WORD FAR PASCAL ReadScreenXsize( HANDLE hInst )
{
   return ReadInt( hInst, IDS_INI_WIN, IDS_INI_WIN_XSIZE, CW_USEDEFAULT ) ;

}/* ReadScreenXsize() */

/*************************************************************************
 *  WORD FAR PASCAL ReadScreenYsize( HANDLE hInst )
 *
 *  Description:
 *    Reads LEYIS.INI, getting the Ypos value.
 *
 *  Type/Parameter
 *          Description
 *
 *   HWND   hWnd
 *          Handle to the main window.
 * 
 *  Return Value
 *
 *    Returns the Y position of the window as stored in the ini file.
 *
 *  Comments:
 *
 *************************************************************************/
WORD FAR PASCAL ReadScreenYsize( HANDLE hInst )
{
   return ReadInt( hInst, IDS_INI_WIN, IDS_INI_WIN_YSIZE, CW_USEDEFAULT ) ;

}/* ReadScreenYsize() */

/*************************************************************************
 *  int FAR PASCAL ReadScreenShow( HANDLE hInst ) ;
 *
 *  Description: 
 *    Reads LEYIS.INI, getting the nCmdShow value (how the window is shown)
 *
 *  Type/Parameter
 *          Description
 *
 *   HWND   hWnd
 *          Handle to the main window.
 * 
 *  Return Value
 *
 *    Returns the nCmdShow value (either SW_SHOWNORMAL or SW_SHOWMAXIMIZED)
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL ReadScreenShow( HANDLE hInst, int nCmdShow ) 
{
   return ReadInt( hInst, IDS_INI_WIN, IDS_INI_WIN_NCMDSHOW, nCmdShow ) ;

}/* ReadScreenShow() */


/*************************************************************************
 * Personality functions...
 *************************************************************************/

/*************************************************************************
 *  BOOL FAR PASCAL ReadPersonality( HANDLE hInst )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
WORD FAR PASCAL ReadPersonality( HANDLE hInst )
{
   return ReadInt( hInst, IDS_INI_USERPREF, IDS_INI_PERSONALITY, LEGAL ) ;

}/* ReadPersonality() */

/*************************************************************************
 *  BOOL FAR PASCAL WritePersonality( HANDLE hInst, WORD wPer )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL WritePersonality( HANDLE hInst, WORD wPer )
{
   return WriteFlag( hInst, IDS_INI_USERPREF, IDS_INI_PERSONALITY, wPer ) ;

}/* WritePersonality() */


/*************************************************************************
 *  BOOL FAR PASCAL ReadBotStat( HANDLE hInst )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL ReadBotStat( HANDLE hInst )
{
   return ReadFlag( hInst, IDS_INI_USERPREF, IDS_INI_BOTSTAT, TRUE ) ;

}/* ReadBotStat() */

/*************************************************************************
 *  BOOL FAR PASCAL WriteBotStat( HANDLE hInst, BOOL fStat )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL WriteBotStat( HANDLE hInst, BOOL fStat )
{
   return WriteFlag( hInst, IDS_INI_USERPREF, IDS_INI_BOTSTAT, fStat ) ;

}/* WriteBotStat() */

/*************************************************************************
 *  BOOL FAR PASCAL ReadTopStat( HANDLE hInst )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL ReadTopStat( HANDLE hInst )
{
   return ReadFlag( hInst, IDS_INI_USERPREF, IDS_INI_TOPSTAT, TRUE ) ;

}/* ReadTopStat() */

/*************************************************************************
 *  BOOL FAR PASCAL WriteTopStat( HANDLE hInst, BOOL fStat )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL WriteTopStat( HANDLE hInst, BOOL fStat )
{
   return WriteFlag( hInst, IDS_INI_USERPREF, IDS_INI_TOPSTAT, fStat ) ;

}/* WriteTopStat() */

/*************************************************************************
 *  BOOL FAR PASCAL ReadButtonWnd( HANDLE hInst )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL ReadButtonWnd( HANDLE hInst )
{
   return ReadFlag( hInst, IDS_INI_USERPREF, IDS_INI_BUTTONWND, TRUE ) ;

}/* ReadButtonWnd() */

/*************************************************************************
 *  BOOL FAR PASCAL WriteButtonWnd( HANDLE hInst, BOOL fStat )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL WriteButtonWnd( HANDLE hInst, BOOL fStat )
{
   return WriteFlag( hInst, IDS_INI_USERPREF, IDS_INI_BUTTONWND, fStat ) ;

}/* WriteButtonWnd() */

/*************************************************************************
 *  BOOL FAR PASCAL ReadFastPrint( HANDLE hInst )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL ReadFastPrint( HANDLE hInst )
{
   return ReadFlag( hInst, IDS_INI_USERPREF, IDS_INI_FASTPRINT, TRUE ) ;

}/* ReadFastPrint() */

/*************************************************************************
 *  BOOL FAR PASCAL WriteFastPrint( HANDLE hInst, BOOL fStat )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL WriteFastPrint( HANDLE hInst, BOOL fStat )
{
   return WriteFlag( hInst, IDS_INI_USERPREF, IDS_INI_FASTPRINT, fStat ) ;

}/* WriteFastPrint() */


/************************************************************************
 * Directory info functions
 ************************************************************************/
/*************************************************************************
 *  BOOL FAR PASCAL
 *  WriteDirectories( HANDLE hInst,
 *                    LPSTR lpszDiary, LPSTR lpszNotePad,
 *                    LPSTR lpszRecord, LPSTR lpszDoc )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL
WriteDirectories( HANDLE hInst,
                  LPSTR lpszDiary, LPSTR lpszNotePad,
                  LPSTR lpszRecord, LPSTR lpszDoc )
{
   char szSection[MAX_INILEN+1] ;
   char szKey[MAX_INILEN+1] ;

   if (!LoadString( hInst, IDS_INI_DIRS, szSection, MAX_INILEN ))
      return FALSE ;

   if (LoadString( hInst, IDS_INI_DIRS_DIARY, szKey, MAX_INILEN ))
      ProfileWrite( szSection, szKey, lpszDiary ) ;
   else
      return FALSE ;

   if (LoadString( hInst, IDS_INI_DIRS_NOTEPAD, szKey, MAX_INILEN ))
      ProfileWrite( szSection, szKey, lpszNotePad ) ;
   else
      return FALSE ;

   if (LoadString( hInst, IDS_INI_DIRS_RECORD, szKey, MAX_INILEN ))
      ProfileWrite( szSection, szKey, lpszRecord ) ;
   else
      return FALSE ;

   if (LoadString( hInst, IDS_INI_DIRS_DOCUMENT, szKey, MAX_INILEN ))
      ProfileWrite( szSection, szKey, lpszDoc ) ;
   else
      return FALSE ;

   return TRUE ;

}/* WriteDirectories() */

/*************************************************************************
 *  BOOL FAR PASCAL
 *  ReadDirectories( HANDLE hInst, LPSTR lpszDefault,
 *                    LPSTR lpszDiary, LPSTR lpszNotePad,
 *                    LPSTR lpszRecord, LPSTR lpszDoc )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL
ReadDirectories( HANDLE hInst, LPSTR lpszDefault,
                  LPSTR lpszDiary, LPSTR lpszNotePad,
                  LPSTR lpszRecord, LPSTR lpszDoc )
{
   char szSection[MAX_INILEN+1] ;
   char szKey[MAX_INILEN+1] ;

   if (!LoadString( hInst, IDS_INI_DIRS, szSection, MAX_INILEN ))
      return FALSE ;

   if (LoadString( hInst, IDS_INI_DIRS_DIARY, szKey, MAX_INILEN ))
      ProfileRead( szSection, szKey, lpszDefault, lpszDiary ) ;
   else
      return FALSE ;

   if (LoadString( hInst, IDS_INI_DIRS_NOTEPAD, szKey, MAX_INILEN ))
      ProfileRead( szSection, szKey, lpszDefault, lpszNotePad ) ;
   else
      return FALSE ;

   if (LoadString( hInst, IDS_INI_DIRS_RECORD, szKey, MAX_INILEN ))
      ProfileRead( szSection, szKey, lpszDefault, lpszRecord ) ;
   else
      return FALSE ;

   if (LoadString( hInst, IDS_INI_DIRS_DOCUMENT, szKey, MAX_INILEN ))
      ProfileRead( szSection, szKey, lpszDefault, lpszDoc ) ;
   else
      return FALSE ;

   return TRUE ;

}/* ReadDirectories() */


/*************************************************************************
 *  BOOL FAR PASCAL
 *   WriteHeaderFooter( HANDLE hInst, LPSTR lpszHeader, LPSTR lpszFooter )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL
WriteHeaderFooter( HANDLE hInst, LPSTR lpszHeader, LPSTR lpszFooter )
{
   char szSection[MAX_INILEN+1] ;
   char szKey[MAX_INILEN+1] ;

   if (LoadString( hInst, IDS_INI_PRINT, szSection, MAX_INILEN))
   {
      if (LoadString( hInst, IDS_INI_PRINT_HEADER, szKey, MAX_INILEN ))
         ProfileWrite( szSection, szKey, lpszHeader ) ;

      if (LoadString( hInst, IDS_INI_PRINT_FOOTER, szKey, MAX_INILEN ))
         ProfileWrite( szSection, szKey, lpszFooter ) ;

      return TRUE ;
   }
   else
      return FALSE ;

}/* WriteHeaderFooter() */

/*************************************************************************
 *  BOOL FAR PASCAL
 *  ReadHeaderFooter( HANDLE hInst, LPSTR lpszHeader, LPSTR lpszFooter ) ;
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL
ReadHeaderFooter( HANDLE hInst, LPSTR lpszHeader, LPSTR lpszFooter )
{
   char szSection[MAX_INILEN+1] ;
   char szKey[MAX_INILEN+1] ;

   if (!LoadString( hInst, IDS_INI_PRINT, szSection, MAX_INILEN ))
      return FALSE ;

   if (LoadString( hInst, IDS_INI_PRINT_HEADER, szKey, MAX_INILEN ))
      ProfileRead( szSection, szKey, "", lpszHeader ) ;
   else
      return FALSE ;

   if (LoadString( hInst, IDS_INI_PRINT_FOOTER, szKey, MAX_INILEN ))
      ProfileRead( szSection, szKey, "", lpszFooter ) ;
   else
      return FALSE ;

   return TRUE ;

}/* ReadHeaderFooter() */


/*************************************************************************
 *  BOOL FAR PASCAL
 *  WriteFonts( HANDLE hInst, LPEXTLOGFONT lplfNorm,LPEXTLOGFONT lplfHL )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL
WriteFonts( HANDLE hInst, LPEXTLOGFONT lplfNorm,LPEXTLOGFONT lplfHL )
{
   char szSection[MAX_INILEN+1] ;

   if (LoadString( hInst, IDS_INI_NORMALFONT, szSection, MAX_INILEN))
      WriteFont( hInst, szSection, lplfNorm ) ;
   else
      return FALSE ;

   if (LoadString( hInst, IDS_INI_HIGHLIGHTFONT, szSection, MAX_INILEN))
      WriteFont( hInst, szSection, lplfHL ) ;
   else
      return FALSE ;

   return TRUE ;

}/* WriteFonts() */

BOOL NEAR PASCAL
WriteFont( HANDLE hInst, LPSTR szSection, LPEXTLOGFONT lplf )
{
  char szKey[MAX_INILEN+1] ;

   if (LoadString( hInst, IDS_INI_FACENAME, szKey, MAX_INILEN ))
      ProfileWrite( szSection, szKey, lplf->lfFaceName ) ;

   if (LoadString( hInst, IDS_INI_FACESIZEX, szKey, MAX_INILEN ))
      ProfileWriteInt( szSection, szKey, lplf->lfWidth ) ;

   if (LoadString( hInst, IDS_INI_FACESIZEY, szKey, MAX_INILEN ))
      ProfileWriteInt( szSection, szKey, lplf->lfHeight ) ;

   if (LoadString( hInst, IDS_INI_BOLD, szKey, MAX_INILEN ))
      ProfileWriteInt( szSection, szKey, lplf->lfWeight ) ;

   if (LoadString( hInst, IDS_INI_ITALIC, szKey, MAX_INILEN ))
      ProfileWriteInt( szSection, szKey, lplf->lfItalic ) ;

   if (LoadString( hInst, IDS_INI_UNDERLINE, szKey, MAX_INILEN ))
      ProfileWriteInt( szSection, szKey, lplf->lfUnderline ) ;

   if (LoadString( hInst, IDS_INI_STRIKEOUT, szKey, MAX_INILEN ))
      ProfileWriteInt( szSection, szKey, lplf->lfStrikeOut ) ;

   if (LoadString( hInst, IDS_INI_FGCOLOR, szKey, MAX_INILEN ))
      ProfileWriteDW( szSection, szKey, lplf->lfFGcolor ) ;

   if (LoadString( hInst, IDS_INI_BGCOLOR, szKey, MAX_INILEN ))
      ProfileWriteDW( szSection, szKey, lplf->lfBGcolor ) ;

   return TRUE ;
}


/*************************************************************************
 * BOOL FAR PASCAL
 * ReadFonts( HANDLE hInst, LPEXTLOGFONT lplfNorm,LPEXTLOGFONT lplfHL )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL
ReadFonts( HANDLE hInst, LPEXTLOGFONT lplfNorm,LPEXTLOGFONT lplfHL )
{
   char szSection[MAX_INILEN+1] ;

   if (LoadString( hInst, IDS_INI_NORMALFONT, szSection, MAX_INILEN))
      ReadFont( hInst, szSection, lplfNorm ) ;
   else
      return FALSE ;

   if (LoadString( hInst, IDS_INI_HIGHLIGHTFONT, szSection, MAX_INILEN))
      ReadFont( hInst, szSection, lplfHL ) ;
   else
      return FALSE ;

   return TRUE ;

}/* ReadFonts() */

BOOL NEAR PASCAL
ReadFont( HANDLE hInst, LPSTR szSection, LPEXTLOGFONT lplf )
{
  char szKey[MAX_INILEN+1] ;

   if (LoadString( hInst, IDS_INI_FACENAME, szKey, MAX_INILEN ))
      ProfileRead( szSection, szKey, lplf->lfFaceName, lplf->lfFaceName ) ;

   if (LoadString( hInst, IDS_INI_FACESIZEX, szKey, MAX_INILEN ))
      lplf->lfWidth = ProfileReadInt( szSection, szKey, lplf->lfWidth ) ;

   if (LoadString( hInst, IDS_INI_FACESIZEY, szKey, MAX_INILEN ))
      lplf->lfHeight = ProfileReadInt( szSection, szKey, lplf->lfHeight ) ;

   if (LoadString( hInst, IDS_INI_BOLD, szKey, MAX_INILEN ))
      lplf->lfWeight = ProfileReadInt( szSection, szKey, lplf->lfWeight ) ;

   if (LoadString( hInst, IDS_INI_ITALIC, szKey, MAX_INILEN ))
      lplf->lfItalic = (BYTE)ProfileReadInt( szSection, szKey, lplf->lfItalic ) ;

   if (LoadString( hInst, IDS_INI_UNDERLINE, szKey, MAX_INILEN ))
      lplf->lfUnderline = (BYTE)ProfileReadInt( szSection, szKey, lplf->lfUnderline ) ;

   if (LoadString( hInst, IDS_INI_STRIKEOUT, szKey, MAX_INILEN ))
      lplf->lfStrikeOut = (BYTE)ProfileReadInt( szSection, szKey, lplf->lfStrikeOut ) ;

   if (LoadString( hInst, IDS_INI_FGCOLOR, szKey, MAX_INILEN ))
      lplf->lfFGcolor = ProfileReadDW( szSection, szKey, lplf->lfFGcolor ) ;

   if (LoadString( hInst, IDS_INI_BGCOLOR, szKey, MAX_INILEN ))
      lplf->lfBGcolor = ProfileReadDW( szSection, szKey, lplf->lfBGcolor ) ;

   return TRUE ;
}

/************************************************************************
 * Internal APIS
 ************************************************************************/
BOOL NEAR PASCAL ReadFlag( HANDLE hInst, WORD wSection, WORD wFlag, BOOL f )
{
   char szSection[MAX_INILEN+1] ;
   char szKey[MAX_INILEN+1] ;

   if (LoadString( hInst, wSection, szSection, MAX_INILEN ) &&
       LoadString( hInst, wFlag, szKey, MAX_INILEN ))
      return ProfileReadTrueFalse( szSection, szKey, f ) ;
   else
      return f ;
}

int NEAR PASCAL ReadInt( HANDLE hInst, WORD wSection, WORD wFlag, int w )
{
   char szSection[MAX_INILEN+1] ;
   char szKey[MAX_INILEN+1] ;

   if (LoadString( hInst, wSection, szSection, MAX_INILEN ) &&
       LoadString( hInst, wFlag, szKey, MAX_INILEN ))
      return ProfileReadInt( szSection, szKey, w ) ;
   else
      return w ;
}

BOOL NEAR PASCAL WriteFlag( HANDLE hInst, WORD wSection, WORD wKey, WORD wVal )
{
   char szSection[MAX_INILEN+1] ;
   char szKey[MAX_INILEN+1] ;

   if (LoadString( hInst, wSection, szSection, MAX_INILEN ) &&
      LoadString( hInst, wKey, szKey, MAX_INILEN ))
      return ProfileWriteInt( szSection, szKey, wVal ) ;
   else
      return FALSE ;

}/* WriteFlag() */
/************************************************************************
 * End of File: state.c
 ************************************************************************/

