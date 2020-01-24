/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  userpref.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *    This module handles the "User Preferences Dialog Box"...
 *
 *   Revisions:  
 *     00.00.000  2/ 3/91 cek   First version
 *
 ************************************************************************/

#include <windows.h>
#include <cmndll.h>
#include "..\inc\lexis.h"
#include "..\inc\lexdlgd.h"
#include "..\inc\global.h"
#include "..\inc\topstat.h"
#include "..\inc\botstat.h"
#include "..\inc\button.h"
#include "..\inc\state.h"
#include "..\inc\mainwnd.h"
#include "..\inc\help.h"
#include "..\inc\term.h"

#include "..\inc\userpref.h"
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
int FAR PASCAL fnUserPref( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;
BOOL NEAR PASCAL
DrawSample( HWND hDlg, LPEXTLOGFONT lplfN, LPEXTLOGFONT lplfH ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/
BOOL fNormChanged = FALSE ;
BOOL fHLChanged = FALSE ;

/*************************************************************************
 *  BOOL FAR PASCAL UserPrefsDialog( HWND hWnd )
 *
 *  Description:
 *    This function is called by MainWnd.C and handles the User Prefs
 *    dialog box.
 *
 *  Return Value
 *    TRUE if the user hit OK, FALSE otherwise.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL UserPrefsDialog( HWND hWnd )
{
   BOOL n ;
   FARPROC lpfnDlgProc ;
   WORD wPer ;
   BOOL fFastPrint ;
   LPLEXISSTATE lpLS ;
   HCURSOR  hCur ;

   if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
      return FALSE ;

   //
   // Get state before dialog...
   //
   wPer = lpLS->wPersonality ;
   fFastPrint = lpLS->fFastPrint ;

   //
   // Bring up the dialog box
   //

   if (lpfnDlgProc = MakeProcInstance( (FARPROC)fnUserPref,
                                       GETHINST( hWnd ) ))
   {
      n = DialogBox( GETHINST( hWnd ), "USERPREF", hWnd, lpfnDlgProc ) ;
      FreeProcInstance( lpfnDlgProc ) ;
   }
   else
   {
      GlobalUnlock( ghGlobals ) ;
      return FALSE ;
   }

   hCur = SetHourGlass() ;
   if (n != IDCANCEL && n != DLG_ERROR)
   {
      HANDLE hInst = GETHINST( hWnd ) ;

      // To minimize "redraw" and goofy looks we'll first figure out
      // if anything needs to be changed...
      //
      HWND hwndTopStat = GETHWNDTOPSTAT( hWnd ) ;
      HWND hwndBotStat = GETHWNDBOTSTAT( hWnd ) ;
      HWND hwndButtons = GETHWNDBUTTON( hWnd ) ;
      HWND hwndTerm    = GETHWNDTERM( hWnd ) ;

      BOOL fTopStat = hwndTopStat ? TRUE : FALSE ;
      BOOL fBotStat = hwndBotStat ? TRUE : FALSE ;
      BOOL fButtons = hwndButtons ? TRUE : FALSE ;

      //
      // A menu change makes for all sorts of wierd looking stuff so 
      // if the menu changes just hide all the windows...
      //
      // If any of the windows are changed then hide them all
      //
      if (wPer != lpLS->wPersonality ||
          (lpLS->fTopStat   != fTopStat) ||
          (lpLS->fBotStat   != fBotStat) ||
          (lpLS->fButtonWnd != fButtons))
      {
         if (hwndButtons)
            ShowWindow( hwndButtons, SW_HIDE ) ;

         if (hwndTopStat)
            ShowWindow( hwndTopStat, SW_HIDE ) ;

         if (hwndBotStat)
            ShowWindow( hwndBotStat, SW_HIDE ) ;
      }

      //
      // Now go about changing the windows...
      //

      //
      // TopStat
      //
      if (lpLS->fTopStat != fTopStat )
      {
         WriteTopStat( hInst, lpLS->fTopStat ) ;

         if (hwndTopStat)
         {
            DestroyTopStatus( hwndTopStat ) ;
            hwndTopStat = NULL ;
            SetWindowWord( hWnd, GWW_HWNDTOPSTAT, 0 ) ;
         }
         else
            if (hwndTopStat = CreateTopStatus( hWnd ))
               SetWindowWord( hWnd, GWW_HWNDTOPSTAT, hwndTopStat ) ;
      }

      //
      // BotStat
      //
      if (lpLS->fBotStat != fBotStat )
      {
         WriteBotStat( hInst, lpLS->fBotStat ) ;

         if (hwndBotStat)
         {
            DestroyBotStatus( hwndBotStat ) ;
            hwndBotStat = NULL ;
            SetWindowWord( hWnd, GWW_HWNDBOTSTAT, 0 ) ;
         }
         else
            if (hwndBotStat = CreateBotStatus( hWnd ))
               SetWindowWord( hWnd, GWW_HWNDBOTSTAT, hwndBotStat ) ;
      }

      //
      // Buttons
      //
      if (lpLS->fButtonWnd != fButtons )
      {
         WriteButtonWnd( hInst, lpLS->fButtonWnd ) ;

         if (hwndButtons)
         {
            DestroyButtonWnd( hwndButtons ) ;
            hwndButtons = NULL ;
            SetWindowWord( hWnd, GWW_HWNDBUTTON, 0 ) ;
         }
         else
            if (hwndButtons = CreateButtonWnd( hWnd ))
               SetWindowWord( hWnd, GWW_HWNDBUTTON, hwndButtons ) ;
      }

      //
      // Do what it takes to change the menu (if necessary)
      //
      if (wPer != lpLS->wPersonality)
      {
         WritePersonality( hInst, lpLS->wPersonality ) ;
         SetPersonality( hWnd, lpLS->wPersonality ) ;
      }


      if (wPer != lpLS->wPersonality ||
          (lpLS->fTopStat   != fTopStat) ||
          (lpLS->fBotStat   != fBotStat) ||
          (lpLS->fButtonWnd != fButtons))
      {
         RECT rc ;

         GetClientRect( hWnd, &rc ) ;

         PlaceChildWindows( hWnd, rc.right - rc.left, rc.bottom - rc.top ) ;
      }

      if (hwndBotStat)
      {
         ShowWindow( hwndBotStat, SW_NORMAL ) ;
      }

      if (hwndButtons)
      {
         ShowWindow( hwndButtons, SW_NORMAL ) ;
      }

      if (hwndTopStat)
      {
         ShowWindow( hwndTopStat, SW_NORMAL ) ;
      }

      if (fNormChanged || fHLChanged)
      {
         WriteFonts( hInst, &(lpLS->lfNormal), &(lpLS->lfHighlight) ) ;
         ResetTerm( hwndTerm ) ;
      }

      if (lpLS->fFastPrint != fFastPrint)
         WriteFastPrint( hInst, lpLS->fFastPrint ) ;

   }

   GlobalUnlock( ghGlobals ) ;

   //
   // If one of the "other dialog" buttons was hit to end the dialog
   // box, then go to that dialog box by simulating a Menu selection
   //
   if (n == IDD_HARDWARE_DLG)
      PostMessage( hWnd, WM_COMMAND, IDM_SETUP_HARDWARE, 0L ) ;

   if (n == IDD_DIRECTORIES_DLG)
      PostMessage( hWnd, WM_COMMAND, IDM_SETUP_DIRECTORIES, 0L ) ;


   ResetHourGlass( hCur ) ;
   return TRUE ;

}/* UserPrefsDialog() */


/*************************************************************************
 *  int FAR PASCAL
 *    fnUserPref( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description:
 *    Window procedure for the User Preferences dialog box.
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL fnUserPref( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
{
   static   EXTLOGFONT lfNormal ;
   static   EXTLOGFONT lfHighlight ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         LPLEXISSTATE lpLS ;

         fNormChanged = FALSE ;
         fHLChanged = FALSE ;

         if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
         {
            EndDialog( hDlg, DLG_ERROR ) ;
            return FALSE ;
         }

         //
         // LexisState contains all the stuff we need...
         //
         CheckRadioButton( hDlg, IDD_LEGAL, IDD_COMBINED,
                           IDD_LEGAL + lpLS->wPersonality ) ;

         CheckDlgButton( hDlg, IDD_TOPSTAT, lpLS->fTopStat ) ;
         CheckDlgButton( hDlg, IDD_BOTSTAT, lpLS->fBotStat ) ;
         CheckDlgButton( hDlg, IDD_BUTTONBAR, lpLS->fButtonWnd ) ;
         CheckDlgButton( hDlg, IDD_FASTPRINT, lpLS->fFastPrint ) ;

         //
         // Center the dialog within the main window
         //
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;

         lfNormal = lpLS->lfNormal ;
         lfHighlight = lpLS->lfHighlight ;

         GlobalUnlock( ghGlobals ) ;

      }
      break ;

      case WM_COMMAND:
         switch( wParam )
         {
            case IDD_HELP:
               Help( HELP_HELPONHELP, NULL, 0 );
            break ;


            case IDD_NORMAL:
               if (DLG_OK == FontSelectDialog( hDlg, "Setup Normal Font",
                                               NULL,
                                               OEM_FONTTYPE |
                                               ANSI_FONTTYPE |
                                               RASTER_FONTTYPE |
                                               FIXED_FONTTYPE,
                                               FONTSEL_NOPRINTER,
                                               &lfNormal,
                                               &lfNormal ))
               {
                  fNormChanged = TRUE ;
                  lfHighlight.lfWidth = lfNormal.lfWidth ;
                  lfHighlight.lfHeight = lfNormal.lfHeight ;
                  lstrcpy( lfHighlight.lfFaceName, lfNormal.lfFaceName ) ;

                  DrawSample( hDlg, &lfNormal, &lfHighlight ) ;
                  DP( hWDB,  "face = %s, height = %d, width = %d",
                        (LPSTR)lfNormal.lfFaceName,
                        lfNormal.lfHeight, lfNormal.lfWidth ) ;
               }
            break ;

            case IDD_HIGHLIGHT:
               if (DLG_OK == FontSelectDialog( hDlg, "Setup Highlight Font",
                                               NULL,
                                               OEM_FONTTYPE |
                                               ANSI_FONTTYPE |
                                               RASTER_FONTTYPE |
                                               FIXED_FONTTYPE,
                                               FONTSEL_NOPRINTER,
                                               &lfHighlight,
                                               &lfHighlight ))
               {
                  fHLChanged = TRUE ;
                  lfNormal.lfWidth = lfHighlight.lfWidth ;
                  lfNormal.lfHeight = lfHighlight.lfHeight ;
                  lstrcpy( lfNormal.lfFaceName, lfHighlight.lfFaceName ) ;

                  DrawSample( hDlg, &lfNormal, &lfHighlight ) ;
                  
                  DP( hWDB,  "face = %s, height = %d, width = %d",
                        (LPSTR)lfHighlight.lfFaceName,
                        lfHighlight.lfHeight, lfHighlight.lfWidth )  ;
               }
            break ;

            case IDD_HARDWARE_DLG:
            case IDD_DIRECTORIES_DLG:
            case IDOK:
            {
               HANDLE hInst = GETHINST( GetParent( hDlg )) ;

               LPLEXISSTATE lpLS ;

               if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
               {
                  EndDialog( hDlg, DLG_ERROR ) ;
                  return FALSE ;
               }
               //
               // If OK was pressed, save the settings to the INI file
               //
               lpLS->wPersonality = DlgWhichRadioButton( hDlg,
                                             IDD_LEGAL,
                                             IDD_COMBINED,
                                             IDD_LEGAL ) - IDD_LEGAL ;

               lpLS->fTopStat = IsDlgButtonChecked( hDlg, IDD_TOPSTAT ) ;

               lpLS->fBotStat = IsDlgButtonChecked( hDlg, IDD_BOTSTAT ) ;

               lpLS->fButtonWnd = IsDlgButtonChecked( hDlg, IDD_BUTTONBAR ) ;

               lpLS->fFastPrint = IsDlgButtonChecked( hDlg, IDD_FASTPRINT ) ;

               lpLS->lfNormal = lfNormal ;
               lpLS->lfHighlight = lfHighlight ;

               GlobalUnlock( ghGlobals ) ;

               //
               // This has to be here because the terminal caret is 
               // sized in the WM_SETFOCUS case in term.c.  If ResetTerm
               // is called after the dialog box goes away, the WM_SETFOCUS
               // will see old data.
               //
               if (fNormChanged || fHLChanged)
                  ResetTerm( GETHWNDTERM( GetParent( hDlg ) ) ) ;

               EndDialog( hDlg, wParam ) ;
            }
            break ;

            case IDCANCEL:
               EndDialog( hDlg, wParam ) ;
            break ;
         }

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;
         TEXTMETRIC  tm ;
         HDC         hDC ;

         hDC = BeginPaint(hDlg, &ps ) ;

         GetTextMetrics( hDC, &tm ) ;
         DrawDropShadow( hDC, GetDlgItem( hDlg, IDD_BOX0 ),
                        tm.tmAveCharWidth / 2 ) ;

         DrawSample( hDlg, &lfNormal, &lfHighlight ) ;

         EndPaint( hDlg, &ps ) ;

      }
      return FALSE ;

      default:
         return FALSE ;
   }

   return TRUE ;

}/* fnUserPref() */


BOOL NEAR PASCAL
DrawSample( HWND hDlg, LPEXTLOGFONT lplfN, LPEXTLOGFONT lplfH )
{
   RECT        rc ;
   HFONT       hFont ;
   HFONT       hOldFont ;
   HWND        hwnd ;
   HDC         hDC ;
   
   hDC = GetDC( hwnd = GetDlgItem( hDlg, IDD_SAMPLE ) );

   if (!hDC)
      return FALSE ;

   GetClientRect( hwnd, &rc ) ;
   InflateRect( &rc, -1, -1 ) ;

   // 
   // First draw some normal text...
   //
   SetTextColor( hDC, lplfN->lfFGcolor ) ;
   SetBkColor( hDC, lplfN->lfBGcolor ) ;

   // Make background of sample area normal text bg...
   ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;

   InflateRect( &rc, -3, -3 ) ;

   if (hFont = CreateFontIndirect( (LPLOGFONT)lplfN ))
   {
      RECT rc1 ;

      hOldFont = SelectObject( hDC, hFont ) ;


      CopyRect( &rc1, &rc ) ;

      rc1.bottom = (rc.bottom - rc.top) / 2 ;

      DrawText( hDC, "Normal Text.", -1, &rc1,
         DT_CENTER | DT_TOP | DT_WORDBREAK ) ;

      if (hOldFont)
         SelectObject( hDC, hOldFont ) ;
      DeleteObject( hFont ) ;
   }

   if (hFont = CreateFontIndirect( (LPLOGFONT)lplfH ))
   {
      RECT rc1 ;

      hOldFont = SelectObject( hDC, hFont ) ;

      CopyRect( &rc1, &rc ) ;

      rc1.top = ((rc.bottom - rc.top) / 2) + 5 ;

      SetTextColor( hDC, lplfH->lfFGcolor ) ;
      SetBkColor( hDC, lplfH->lfBGcolor ) ;

      DrawText( hDC, "Highlight Text.", -1, &rc1,
         DT_CENTER | DT_TOP | DT_WORDBREAK ) ;

      if (hOldFont)
         SelectObject( hDC, hOldFont ) ;
      DeleteObject( hFont ) ;
   }

   ReleaseDC( hwnd, hDC ) ;

   return TRUE ;
}


/************************************************************************
 * End of File: userpref.c
 ************************************************************************/

