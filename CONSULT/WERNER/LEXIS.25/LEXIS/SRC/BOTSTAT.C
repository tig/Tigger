/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  botstat.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *    This module is responsible for the bottom status window.  It uses the
 *    "status" window class exported from CMNDLL.
 *
 *   Revisions:  
 *     00.00.000  1/20/91 cek   First Version
 *
 *
 ************************************************************************/
#include "..\inc\undef.h"
#include <windows.h>
#include <cmndll.h>

#include "..\inc\lexis.h"
#include "..\inc\botstat.h"

#include "..\inc\global.h"

/************************************************************************
 * Local Defines
 ************************************************************************/
#define FONTNAME  "Helv"
#define FONTWEIGHT 400

#define MSGCOLOR        RGBBLACK
#define NUMLOCKCOLOR    RGBBLUE
#define CAPSLOCKCOLOR   RGBBLUE
#define DATECOLOR       RGBBLACK
#define CD_STATUSCOLOR  RGBRED
#define RECORDCOLOR     RGBBLACK
#define DIARYCOLOR      RGBBLACK
#define PRINTCOLOR      RGBBLACK

#define DUMMYWIDTH      0
/************************************************************************
 * Macros 
 ************************************************************************/

/************************************************************************
 * Local data structures
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/
static void NEAR PASCAL BuildBotBoxes( HWND hwndStat ) ;

static BOOL NEAR PASCAL
GetKeyStates( BOOL *bCapsLock, BOOL *bNumLock ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/
static char szCapsLock[MAX_STRLEN] ;
static BOOL bCapsLock ;

static char szNumLock[MAX_STRLEN] ;
static BOOL bNumLock ;

static char szPrint[MAX_STRLEN] ;
static char szRecord[MAX_STRLEN] ;
static char szDiary[MAX_STRLEN] ;

static char szCD[MAX_STRLEN] ;

static FARPROC lpfnBotStat ;        // new status window proc
static FARPROC lpfnOldBotStat ;     // old status window proc

static HWND    hwndBottomStat ;

/************************************************************************
 * Functions
 ************************************************************************/

/*************************************************************************
 *  HWND FAR PASCAL CreateBotStatus( HWND hwndParent ) ;
 *
 *  Description:
 *    This function creates, and displays the the bottom status window using
 *    the "status" window class exported from cmndll.dll.
 *
 *  Type/Parameter
 *          Description
 *
 *    HWND  hwndParent
 *          The parent window.
 * 
 *  Return Value
 *    Returns the handle to the bottom status window, 0 if it fails to
 *    create the window.
 *
 *  Comments:
 *
 *  The bottom status bar has six "stat boxes" and one "message box".
 *  The four stat boxes, in order of appearance are:
 *
 *     IDB_CAPSLOCK  - Status of capslock key (frame)
 *     IDB_SCRLLOCK  - Status of scroll lock key (frame)
 *     IDB_CD        - Carrier Detect (frame)
 *     IDB_DATE      - Current date (frame)
 *
 *  The syntax of the "status" window class makes the message box have
 *  the identifier 0xFFFF.
 *
 *  These boxes act very much like buttons, thus the IDB identifiers.
 *
 *  The strings for the labels are pulled out of the resource file.
 *  The strings for capslock, numlock, will be needed
 *  later, so we'll make them local static variables to this module.
 *
 *************************************************************************/
HWND FAR PASCAL CreateBotStatus( HWND hwndParent ) 
{
   HWND  hwndStat ;
   LONG  lNumStats = 10 ;
   HFONT hFont ;
   LOGFONT lf ;
   TEXTMETRIC tm ;
   HDC   hDC ;

   DP5( hWDB, "CreateBotStatus" ) ;
   if (!(hwndStat = CreateWindow(
                                 "status",
                                 "",
                                 WS_CHILD,
                                 0,
                                 0,
                                 1,
                                 1,
                                 hwndParent,
                                 IDB_BOTSTAT,
                                 GETHINST( hwndParent ),
                                 (LPSTR)&lNumStats )))
   {
      DP1( hWDB, "MsgWnd create failed!" ) ;
      return 0 ;
   }

   //
   // Subclass the status window so we can send it timer messages
   //
   lpfnBotStat = MakeProcInstance((FARPROC)fnBotStat, GETHINST( hwndParent )) ;

   if (lpfnBotStat)
   {
      lpfnOldBotStat = (FARPROC) GetWindowLong( hwndStat, GWL_WNDPROC ) ;

      SetWindowLong( hwndStat, GWL_WNDPROC, (LONG)lpfnBotStat ) ;
   }

   FontInitLogFont( &lf ) ;

   if (hDC = GetDC( hwndStat ))
   {
      HFONT hTempFont = GetStockObject( SYSTEM_FIXED_FONT ) ;

      if (hTempFont)
         SelectObject( hDC,  hTempFont ) ;

      GetTextMetrics( hDC, &tm ) ;

      lstrcpy( lf.lfFaceName, FONTNAME ) ;

      lf.lfHeight = tm.tmHeight - tm.tmExternalLeading ;

      lf.lfWeight = FONTWEIGHT ;
      if (!FontSelectIndirect( hDC, &lf ))
      {
         DP1( hWDB,"FontSelectIndirect failed!!") ;
      }
      ReleaseDC( hwndStat, hDC ) ;
   }

   FontSelectIndirect( NULL, &lf ) ;

   if (hFont = CreateFontIndirect( &lf ))
      SendMessage( hwndStat, WM_SETFONT, hFont, TRUE ) ;

   BuildBotBoxes( hwndStat ) ;

   GetKeyStates( &bCapsLock, &bNumLock ) ;
   StatusSetStatBox( hwndStat, IDB_CAPSLOCK - IDB_BOTSTAT,
                     (LPSTR)(bCapsLock ? szCapsLock : "" )) ;

   StatusSetStatBox( hwndStat, IDB_NUMLOCK - IDB_BOTSTAT,
                     (LPSTR)(bNumLock ? szNumLock : "") ) ;

   return hwndBottomStat = hwndStat ;

}/* CreateBotStatus() */

/*************************************************************************
 *  void FAR PASCAL SetCDIndicator( HWND hwndStat, BOOL fOn )
 *
 *  Description:
 *    This function sets the CD status indicator on and off.  
 *
 *  Type/Parameter
 *          Description
 *  BOOL    fOn
 *          Indicates whether the status should be on or off.  If this
 *          value is TRUE then the status is turned on.
 * 
 *  Comments:
 *
 *************************************************************************/
void FAR PASCAL SetCDIndicator( HWND hwndStat, BOOL fOn )
{
   if (hwndStat)
      StatusSetStatBox( hwndStat, IDB_CD_STATUS - IDB_BOTSTAT,
                        (LPSTR)(fOn ? szCD : "") ) ;

}/* SetCDIndicator() */

/*************************************************************************
 *  void FAR PASCAL SetRecordIndicator( HWND hwndStat, BOOL fOn )
 *
 *  Description:
 *    This function sets the Recording status indicator on and off.  
 *
 *  Type/Parameter
 *          Description
 *  BOOL    fOn
 *          Indicates whether the status should be on or off.  If this
 *          value is TRUE then the status is turned on.
 * 
 *  Comments:
 *
 *************************************************************************/
void FAR PASCAL SetRecordIndicator( HWND hwndStat, BOOL fOn )
{
   if (hwndStat)
      StatusSetStatBox( hwndStat, IDB_RECORD_STATUS - IDB_BOTSTAT,
                        (LPSTR)(fOn ? szRecord : "") ) ;

}/* SetRecordIndicator() */

/*************************************************************************
 *  void FAR PASCAL SetDiaryIndicator( HWND hwndStat, BOOL fOn )
 *
 *  Description:
 *    This function sets the Diary status indicator on and off.  
 *
 *  Type/Parameter
 *          Description
 *  BOOL    fOn
 *          Indicates whether the status should be on or off.  If this
 *          value is TRUE then the status is turned on.
 * 
 *  Comments:
 *
 *************************************************************************/
void FAR PASCAL SetDiaryIndicator( HWND hwndStat, BOOL fOn )
{
   if (hwndStat)
      StatusSetStatBox( hwndStat, IDB_DIARY_STATUS - IDB_BOTSTAT,
                        (LPSTR)(fOn ? szDiary : "") ) ;

}/* SetDiaryIndicator() */

/*************************************************************************
 *  void FAR PASCAL SetPrintIndicator( HWND hwndStat, BOOL fOn )
 *
 *  Description:
 *    This function sets the Printing status indicator on and off.  
 *
 *  Type/Parameter
 *          Description
 *  BOOL    fOn
 *          Indicates whether the status should be on or off.  If this
 *          value is TRUE then the status is turned on.
 * 
 *  Comments:
 *
 *************************************************************************/
void FAR PASCAL SetPrintIndicator( HWND hwndStat, BOOL fOn )
{
   if (hwndStat)
      StatusSetStatBox( hwndStat, IDB_PRINT_STATUS - IDB_BOTSTAT,
                        (LPSTR)(fOn ? szPrint : "") ) ;

}/* SetPrintIndicator() */

/*************************************************************************
 *  BOOL FAR PASCAL DestroyBotStatus( HWND hwndStatus ) ;
 *
 *  Description:
 *    This function destroys the top status window.
 *
 *  Type/Parameter
 *          Description
 *    HWND  hwndStatus
 *          Handle to the status window obtained from CreateBotStatus...
 * 
 *  Return Value
 *    TRUE if it succeeded, FALSE otherwise.
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL DestroyBotStatus( HWND hwndStatus ) 
{
   if (hwndStatus)
   {
      BOOL b ;
      HFONT hFont = (HFONT)SendMessage( hwndStatus, WM_GETFONT, 0, 0L ) ;

      if (hFont)
         DeleteObject( hFont ) ;

      if (lpfnBotStat)
      {
         SetWindowLong( hwndStatus, GWL_WNDPROC, (LONG) lpfnOldBotStat ) ;

         FreeProcInstance( lpfnBotStat ) ;
      }

      b = DestroyWindow( hwndStatus ) ;

      hwndBottomStat = 0 ;

      return b ;
   }
   else
      return FALSE ;

}/* DestroyBotStatus() */


/*************************************************************************
 *  void FAR PASCAL CheckKeyStates( void )
 *
 *  Description:
 *
 *  Comments:
 *
 *************************************************************************/
void FAR PASCAL CheckKeyStates( void )
{
   BOOL bCaps = bCapsLock,
         bNum  = bNumLock ;

   if (!hwndBottomStat)
      return ;

   //
   // if any changed
   //
   if (GetKeyStates( &bCapsLock, &bNumLock ))
   {
      if (bCapsLock != bCaps)
         StatusSetStatBox( hwndBottomStat, IDB_CAPSLOCK - IDB_BOTSTAT,
                           (LPSTR)(bCapsLock ? szCapsLock : "" )) ;

      if (bNumLock != bNum)
         StatusSetStatBox( hwndBottomStat, IDB_NUMLOCK - IDB_BOTSTAT,
                           (LPSTR)(bNumLock ? szNumLock : "") ) ;
   }

}

/************************************************************************
 * Local/Private functions
 ************************************************************************/

/*************************************************************************
 *  long FAR PASCAL fnBotStat( HWND hWnd, WORD iMessage,
 *                             WORD wParam, LONG lParam )
 *
 *  Description:
 *    New window procedure for the status control.  We are subclassing
 *    the status window, mainly so we can send it timer messages.
 *
 *  Comments:
 *
 *************************************************************************/
LONG FAR PASCAL
fnBotStat( HWND hwndStat, WORD iMessage, WORD wParam, LONG lParam )
{
   static char szPrevDate[MAX_STRLEN] ;

   switch (iMessage)
   {
      case WM_TIMER:
      {
         char szBuf[MAX_STRLEN] ;

         TimeGetCurDate( szBuf, 0 ) ;
         if (lstrcmpi( szBuf, szPrevDate ))
         {
            StatusSetStatBox( hwndStat, IDB_DATE - IDB_BOTSTAT, szBuf ) ;
            lstrcpy( szPrevDate, szBuf ) ;
         }
      }
      break ;

      case WM_WININICHANGE:
      {
         char szBuf[MAX_STRLEN] ;

         TimeResetInternational() ;

         TimeGetCurDate( szBuf, 0 ) ;
         StatusSetStatBoxSize(  hwndStat, IDB_DATE - IDB_BOTSTAT,
                        StatusGetWidth( hwndStat, szBuf ) ) ;
         StatusSetStatBox( hwndStat, IDB_DATE - IDB_BOTSTAT, szBuf ) ;
      }
      break ;

      case WM_MOUSEMOVE:
      case WM_SETFOCUS:
      case WM_KEYDOWN :
         CheckKeyStates( ) ;
      break ;

      case WM_SETTEXT:
         if (*(LPSTR)lParam == '\0')
         {
            LPLEXISSTATE lpLS ;
            
            if (lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals ))
            {
               lParam = (LONG)(LPSTR)lpLS->szCurrentState ;
               GlobalUnlock( ghGlobals ) ;
            }
         }
      break ;
   }

   return CallWindowProc( lpfnOldBotStat, hwndStat, iMessage, wParam, lParam ) ;

}/* fnBotStat() */

/*************************************************************************
 *  void NEAR PASCAL BuildBotBoxes( hwndStat )
 *
 *  Description:
 *    This function figures out the top status boxes and set's them up.
 *
 *  Comments:
 *
 *************************************************************************/
static void NEAR PASCAL BuildBotBoxes( HWND hwndStat ) 
{
   char szBuf[MAX_STRLEN] ;

   StatusSetStatBoxSize(  hwndStat, IDB_DUMMY1_LBL - IDB_BOTSTAT, DUMMYWIDTH ) ;
   StatusSetStatBoxSize(  hwndStat, IDB_DUMMY2_LBL - IDB_BOTSTAT, DUMMYWIDTH ) ;
   StatusSetStatBoxSize(  hwndStat, IDB_DUMMY3_LBL - IDB_BOTSTAT, DUMMYWIDTH ) ;

   if (LoadString( GETHINST( hwndStat ), IDS_STATBOX_RECORD_STATUS, szRecord,
                    MAX_STRLEN ))
      StatusSetStatBoxSize(  hwndStat, IDB_RECORD_STATUS - IDB_BOTSTAT,
                             StatusGetWidth( hwndStat, szRecord ) ) ;

   if (LoadString( GETHINST( hwndStat ), IDS_STATBOX_DIARY_STATUS, szDiary,
                    MAX_STRLEN ))
      StatusSetStatBoxSize(  hwndStat, IDB_DIARY_STATUS - IDB_BOTSTAT,
                             StatusGetWidth( hwndStat, szDiary ) ) ;

   if (LoadString( GETHINST( hwndStat ), IDS_STATBOX_PRINT_STATUS, szPrint,
                    MAX_STRLEN ))
      StatusSetStatBoxSize(  hwndStat, IDB_PRINT_STATUS - IDB_BOTSTAT,
                             StatusGetWidth( hwndStat, szPrint ) ) ;


   if (LoadString( GETHINST( hwndStat ), IDS_STATBOX_CAPSLOCK, szCapsLock,
                    MAX_STRLEN ))
      StatusSetStatBoxSize(  hwndStat, IDB_CAPSLOCK - IDB_BOTSTAT,
                             StatusGetWidth( hwndStat, szCapsLock ) ) ;

   if (LoadString( GETHINST( hwndStat ), IDS_STATBOX_NUMLOCK, szNumLock,
                    MAX_STRLEN ))
      StatusSetStatBoxSize(  hwndStat, IDB_NUMLOCK - IDB_BOTSTAT,
                             StatusGetWidth( hwndStat, szNumLock ) ) ;
                  
   TimeGetCurDate( szBuf, 0 ) ;
   StatusSetStatBoxSize(  hwndStat, IDB_DATE - IDB_BOTSTAT,
                  StatusGetWidth( hwndStat, szBuf ) ) ;
   StatusSetStatBox( hwndStat, IDB_DATE - IDB_BOTSTAT, szBuf ) ;

   if (LoadString( GETHINST( hwndStat ), IDS_STATBOX_CD_STATUS, szCD,
                    MAX_STRLEN ))
      StatusSetStatBoxSize(  hwndStat, IDB_CD_STATUS - IDB_BOTSTAT,
                             StatusGetWidth( hwndStat, szCD ) ) ;

   SendMessage( hwndStat, ST_SETBORDER,  IDB_DUMMY1_LBL - IDB_BOTSTAT,
                (LONG)FALSE ) ;
   SendMessage( hwndStat, ST_SETBORDER,  IDB_DUMMY2_LBL - IDB_BOTSTAT,
                (LONG)FALSE ) ;
   SendMessage( hwndStat, ST_SETBORDER,  IDB_DUMMY3_LBL - IDB_BOTSTAT,
                (LONG)FALSE ) ;

   SendMessage( hwndStat, ST_SETCOLOR,
                IDB_RECORD_STATUS - IDB_BOTSTAT, RECORDCOLOR ) ;

   SendMessage( hwndStat, ST_SETCOLOR,
                IDB_DIARY_STATUS - IDB_BOTSTAT, DIARYCOLOR ) ;

   SendMessage( hwndStat, ST_SETCOLOR,
                IDB_PRINT_STATUS - IDB_BOTSTAT, PRINTCOLOR ) ;

   SendMessage( hwndStat, ST_SETCOLOR,
                IDB_CAPSLOCK - IDB_BOTSTAT, CAPSLOCKCOLOR ) ;

   SendMessage( hwndStat, ST_SETCOLOR,
                IDB_NUMLOCK - IDB_BOTSTAT, NUMLOCKCOLOR ) ;

   SendMessage( hwndStat, ST_SETCOLOR,
                IDB_DATE - IDB_BOTSTAT, DATECOLOR ) ;

   SendMessage( hwndStat, ST_SETCOLOR,
                IDB_CD_STATUS - IDB_BOTSTAT, CD_STATUSCOLOR ) ;

}/* BuildTopBoxes() */

/************************************************************************
 *  BOOL NEAR PASCAL
 *  GetKeyStates( BOOL *bCapsLock , BOOL *bNumLock) ;
 *
 *  Description:
 *    Gets the states of the toggle keys.
 *
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *             TRUE
 *                      The states changed.
 *
 *             FALSE
 *                      The states did not change
 *
 *  Comments:
 *
 ************************************************************************/
static BOOL NEAR PASCAL
GetKeyStates( BOOL *bCapsLock, BOOL *bNumLock )
{
   BOOL bChanged = FALSE ;

   if (*bCapsLock != (GetKeyState( VK_CAPITAL ) & 0x01 ))
   {
      *bCapsLock = !*bCapsLock ;
      bChanged = TRUE ;
   }
   if (*bNumLock != (GetKeyState( VK_NUMLOCK ) & 0x01 ))
   {
      *bNumLock = !*bNumLock ;
      bChanged = TRUE ;
   }

   return bChanged ;
}/* GetKeyStates() */

/************************************************************************
 * End of File: BotStat.c
 ************************************************************************/


