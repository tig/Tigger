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

#include "precomp.h"

#include "ibmpc.h"

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
   NPECB  npECB ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   // local echo

   CheckDlgButton( hDlg, IDD_LOCALECHO, LOCALECHO ) ;

   // new line

   CheckDlgButton( hDlg, IDD_NEWLINE, NEWLINE ) ;

   // auto line wrap

   CheckDlgButton( hDlg, IDD_LINEWRAP, AUTOWRAP ) ;

   // copy fonts to work space

   _fmemcpy( lplfN, &NORMALFONT, sizeof( EXTLOGFONT ) ) ;
   _fmemcpy( lplfH, &HIGHLIGHTFONT, sizeof( EXTLOGFONT ) ) ;

   LocalUnlock( hECB ) ;

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

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   // local echo setting

   LOCALECHO = IsDlgButtonChecked( hDlg, IDD_LOCALECHO ) ;

   // new line

   NEWLINE = IsDlgButtonChecked( hDlg, IDD_NEWLINE ) ;

   // auto line wrap

   AUTOWRAP = IsDlgButtonChecked( hDlg, IDD_LINEWRAP ) ;

   // copy fonts from workspace

   _fmemcpy( &NORMALFONT, lplfN, sizeof( EXTLOGFONT ) ) ;
   _fmemcpy( &HIGHLIGHTFONT, lplfH, sizeof( EXTLOGFONT ) ) ;

   LocalUnlock( hECB ) ;

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
   static  HECB        hECB ;
   static  EXTLOGFONT  lfNormal, lfHighlight ;

   switch (uMsg)
   {
      case WM_INITDIALOG:
         hECB = (HECB) LOWORD( lParam ) ;
         if (!SettingsDlgInit( hDlg, hECB, &lfNormal, &lfHighlight ))
            return ( FALSE ) ;

         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;

         return ShadowControl( hDlg, uMsg, wParam, RGBLTGRAY ) ;

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
               if (SettingsDlgTerm( hDlg, hECB, &lfNormal, &lfHighlight ))
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

      default:
         return ShadowControl( hDlg, uMsg, wParam, lParam ) ;

   }
   return ( FALSE ) ;

} /* end of SettingsDlgProc() */

/************************************************************************
 * End of File: dialog.c
 ************************************************************************/

