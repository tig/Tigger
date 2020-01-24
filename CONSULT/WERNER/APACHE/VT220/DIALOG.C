/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  dialog.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Dialog box routines
 *
 *   Revisions:  
 *     01.00.001  1/ 7/91 baw   Initial Version, first build
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "vt220.h"
#include "dialog.h"

/************************************************************************
 *  BOOL SettingsDlgInit( HWND hDlg, HECB hECB,
 *                        LPEXTLOGFONT lplfN, LPEXTLOGFONT lplfH )
 *  Description:
 *     Init stuff for dialog box.
 *
 *  Comments:
 *      6/23/91  baw  No comment.
 *      7/ 4/91  baw  Stolen from STDCOMM.
 *
 ************************************************************************/

BOOL SettingsDlgInit( HWND hDlg, HECB hECB, LPEXTLOGFONT lplfN,
                      LPEXTLOGFONT lplfH )
{
   int    i ;
   HWND   hCtrlWnd ;
   NPECB  npECB ;
   WORD   wIndex ;

   static char *aszTranslations[] = { "British",
                                      "Dutch",
                                      "Finnish",
                                      "French",
                                      "French Canadian",
                                      "German",
                                      "Italian",
                                      "Norwegian/Danish",
                                      "Spanish",
                                      "Swedish",
                                      "Swiss",
                                      NULL     } ;

   if (NULL == (npECB = (NPECB) LocalLock( (HLOCAL) hECB )))
      return ( FALSE ) ;

   // setup up translation NRCs

   hCtrlWnd = GetDlgItem( hDlg, IDD_TRANSLATECB ) ;
   i = 0 ;
   while (aszTranslations[i] != NULL)
   {
      wIndex = LOWORD( SendMessage( hCtrlWnd, CB_ADDSTRING, NULL,
                                    (LONG) (LPSTR) aszTranslations[i] ) ) ;
      if (i == DECGL( npECB ) - CHARSET_NRCSBRITISH)
         SendMessage( hCtrlWnd, CB_SETCURSEL, wIndex, 0L ) ;
      i++ ;
   }
   wIndex = LOWORD( SendMessage( hCtrlWnd, CB_ADDSTRING,
                                 0, (LONG) (LPSTR) "(none)" ) ) ;
   if (DECGL( npECB ) == CHARSET_DEFAULT)
      SendMessage( hCtrlWnd, CB_SETCURSEL, wIndex, 0L ) ;

   // local echo

   CheckDlgButton( hDlg, IDD_LOCALECHO, LOCALECHO( npECB ) ) ;

   // new line

   CheckDlgButton( hDlg, IDD_NEWLINE, NEWLINE( npECB ) ) ;

   // use NumLock as PF1

   CheckDlgButton( hDlg, IDD_USENUMLOCK, USENUMLOCK( npECB ) ) ;

   // Send DEL on BS

   // CheckDlgButton( hDlg, IDD_SENDDEL, SENDDEL( npECB ) ) ;

   // auto line wrap

   CheckDlgButton( hDlg, IDD_LINEWRAP, AUTOWRAP( npECB ) ) ;

   // copy fonts to work space

   _fmemcpy( lplfN, &NORMALFONT( npECB ), sizeof( EXTLOGFONT ) ) ;
   _fmemcpy( lplfH, &HIGHLIGHTFONT( npECB ), sizeof( EXTLOGFONT ) ) ;

   LocalUnlock( (HLOCAL) hECB ) ;

   return( TRUE ) ;

} /* end of SettingsDlgInit() */

/************************************************************************
 *  BOOL SettingsDlgTerm( HWND hDlg, HECB hECB, LPEXTLOGFONT lplfN,
 *                        LPEXTLOGFONT lplfH )
 *
 *  Description:
 *     SettingsDlg termination routine.  Saves information to the ECB
 *     structure.
 *
 *  Comments:
 *      6/24/91  baw  Wrote this comment.
 *
 ************************************************************************/

BOOL SettingsDlgTerm( HWND hDlg, HECB hECB, LPEXTLOGFONT lplfN,
                      LPEXTLOGFONT lplfH )
{
   NPECB  npECB ;
   WORD   wIndex ;

   if (NULL == (npECB = (NPECB) LocalLock( (HLOCAL) hECB )))
      return ( FALSE ) ;

   // get settings for translation NRCS

   wIndex = LOWORD( SendDlgItemMessage( hDlg, IDD_TRANSLATECB, CB_GETCURSEL,
                                        NULL, NULL ) ) ;
   if (wIndex > 0)
      DECGL( npECB ) = CHARSET_NRCSBRITISH + wIndex - 1 ;
   else
      DECGL( npECB ) = CHARSET_DEFAULT ;

   // local echo setting

   LOCALECHO( npECB ) = IsDlgButtonChecked( hDlg, IDD_LOCALECHO ) ;

   // new line

   NEWLINE( npECB ) = IsDlgButtonChecked( hDlg, IDD_NEWLINE ) ;

   // use NumLock as PF1

   USENUMLOCK( npECB ) = IsDlgButtonChecked( hDlg, IDD_USENUMLOCK ) ;

   // Send DEL on BS

   // SENDDEL( npECB ) = IsDlgButtonChecked( hDlg, IDD_SENDDEL ) ;

   // auto line wrap

   AUTOWRAP( npECB ) = IsDlgButtonChecked( hDlg, IDD_LINEWRAP ) ;

   // copy fonts from workspace

   _fmemcpy( &NORMALFONT( npECB ), lplfN, sizeof( EXTLOGFONT ) ) ;
   _fmemcpy( &HIGHLIGHTFONT( npECB ), lplfH, sizeof( EXTLOGFONT ) ) ;

   LocalUnlock( (HLOCAL) hECB ) ;

   return ( TRUE ) ;

} /* end of SettingsDlgTerm() */

/************************************************************************
 *  BOOL DrawSample( HWND hDlg, LPEXTLOGFONT lplfN, LPEXTLOGFONT lplfH )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ************************************************************************/

BOOL DrawSample( HWND hDlg, LPEXTLOGFONT lplfN, LPEXTLOGFONT lplfH )
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

      hOldFont = (HFONT) SelectObject( hDC, (HGDIOBJ) hFont ) ;


      CopyRect( &rc1, &rc ) ;

      rc1.bottom = (rc.bottom - rc.top) / 2 ;

      DrawText( hDC, "Normal Text.", -1, &rc1,
         DT_CENTER | DT_TOP | DT_WORDBREAK ) ;

      if (hOldFont)
         SelectObject( hDC, (HGDIOBJ) hOldFont ) ;
      DeleteObject( (HGDIOBJ) hFont ) ;
   }

   if (hFont = CreateFontIndirect( (LPLOGFONT) lplfH ))
   {
      RECT rc1 ;

      hOldFont = (HFONT) SelectObject( hDC, (HGDIOBJ) hFont ) ;

      CopyRect( &rc1, &rc ) ;

      rc1.top = ((rc.bottom - rc.top) / 2) + 5 ;

      SetTextColor( hDC, lplfH->lfFGcolor ) ;
      SetBkColor( hDC, lplfH->lfBGcolor ) ;

      DrawText( hDC, "Highlight Text.", -1, &rc1,
         DT_CENTER | DT_TOP | DT_WORDBREAK ) ;

      if (hOldFont)
         SelectObject( hDC, (HGDIOBJ) hOldFont ) ;
      DeleteObject( (HGDIOBJ) hFont ) ;
   }

   ReleaseDC( hwnd, hDC ) ;

   return TRUE ;

} /* end of DrawSample() */

/************************************************************************
 *  BOOL FAR PASCAL SettingsDlgProc( HWND hDlg, UINT uMsg,
 *                                   WPARAM wParam, LPARAM lParam )
 *
 *  Description
 *     This is the settings dialog handler.
 *
 *  Comments:
 *      6/24/91  baw  Wrote this comment block.
 *
 ************************************************************************/

BOOL FAR PASCAL SettingsDlgProc( HWND hDlg, UINT uMsg,
                                 WPARAM wParam, LPARAM lParam )
{
   static HECB        hECB ;
   static EXTLOGFONT  lfNormal, lfHighlight ;

   switch (uMsg)
   {
      case WM_INITDIALOG:
         hECB = (HECB) LOWORD( lParam ) ;
         if (!SettingsDlgInit( hDlg, hECB, &lfNormal, &lfHighlight ))
            return ( FALSE ) ;
         SET_HECB_PROP( hDlg, hECB ) ;
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;
         return ( ShadowControl( hDlg, uMsg, wParam, RGBCYAN ) ) ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDD_BASEFONT:
               if (DLG_OK == FontSelectDialog( hDlg, "Setup Base Font",
                                               NULL,
                                               OEM_FONTTYPE |
                                               ANSI_FONTTYPE |
                                               RASTER_FONTTYPE |
                                               FIXED_FONTTYPE,
                                               FONTSEL_NOPRINTER |
                                               FONTSEL_NOBOLD |
                                               FONTSEL_NOITALIC |
                                               FONTSEL_NOUNDERLINE |
                                               FONTSEL_NOSTRIKEOUT,
                                               &lfNormal,
                                               &lfNormal ))
               {
                  lfHighlight.lfWidth = lfNormal.lfWidth ;
                  lfHighlight.lfHeight = lfNormal.lfHeight ;
                  lstrcpy( lfHighlight.lfFaceName, lfNormal.lfFaceName ) ;
                  DrawSample( hDlg, &lfNormal, &lfHighlight ) ;
               }
               break ;

            case IDD_HIGHLIGHTFONT:
               if (DLG_OK == FontSelectDialog( hDlg, "Setup Highlight Font",
                                               NULL,
                                               OEM_FONTTYPE |
                                               ANSI_FONTTYPE |
                                               RASTER_FONTTYPE |
                                               FIXED_FONTTYPE,
                                               FONTSEL_NOPRINTER |
                                               FONTSEL_NOBOLD |
                                               FONTSEL_NOITALIC |
                                               FONTSEL_NOUNDERLINE |
                                               FONTSEL_NOSTRIKEOUT,
                                               &lfHighlight,
                                               &lfHighlight ))
               {
                  lfNormal.lfWidth = lfHighlight.lfWidth ;
                  lfNormal.lfHeight = lfHighlight.lfHeight ;
                  lstrcpy( lfNormal.lfFaceName, lfHighlight.lfFaceName ) ;
                  DrawSample( hDlg, &lfNormal, &lfHighlight ) ;
               }
               break ;

            case IDD_OK:
               if (SettingsDlgTerm( hDlg, GET_HECB_PROP( hDlg ),
                                    &lfNormal, &lfHighlight ))
               {
                  EndDialog( hDlg, TRUE ) ;
                  return ( TRUE ) ;
               }
               break ;

            case IDD_CANCEL:
               EndDialog( hDlg, FALSE ) ;
               return ( TRUE ) ;
         }
         break ;
 
      case WM_DESTROY:
         REMOVE_HECB_PROP( hDlg ) ;

      /// fall through ... ///

      default:
         return ShadowControl( hDlg, uMsg, wParam, lParam ) ;

   }
   return ( FALSE ) ;

} /* end of SettingsDlgProc() */

/************************************************************************
 * End of File: dialog.c
 ************************************************************************/

