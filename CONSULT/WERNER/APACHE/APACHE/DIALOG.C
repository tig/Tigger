//***************************************************************************
//
//  Module: dialog.c
//
//  Purpose:
//     Dialog box handler functions for APACHE.
//
//  Description of functions:
//     Contained in the function headers.
//
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//           (?)6/25/91   BryanW      Initial version.
//              2/ 2/92   BryanW      Added this comment block.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//***************************************************************************

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"
#include "dialog.h"
#include "dlldesc.h"
#include "ini.h"
#include "mflhlib.h"
#include "session.h"
#include "setup.h"

#include "dialhlp.h"

//************************************************************************
//  BOOL NEAR SessionDirDlgInit( HWND hDlg )
//
//  Description:
//     Performs the initialization of the Session Directory
//     dialog box.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//  History:   Date       Author      Comment
//              1/26/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR SessionDirDlgInit( HWND hDlg )
{
   int         i ;
   char        szTemp[ MAXLEN_TEMPSTR ] ;
   BMPBTNINFO  BmpBtnInfo ;
   BOOL NEAR * npDisplayOptions ;       
   LPWORD      lpBtnInfo ;

   // disable appropriate controls

   EnableSessionDirControls( hDlg, FALSE ) ;

   // allocate a buffer used for storing display options

   if (NULL == (npDisplayOptions =
                   (BOOL NEAR * ) LocalAlloc( LPTR, sizeof( WORD ) *
                                              MAXLEN_SESSIONDIRCOLS )))
   {
      DP1( hWDB, "Unable to allocate buffer for Display Options!." ) ;
      return ( FALSE ) ;
   }
   else
      SET_PROP( hDlg, ATOM_DISPLAYOPTIONS, (HANDLE) npDisplayOptions ) ;

   // fill the listbox and associated slider

   DlgFillSessionDirLB( hDlg, npDisplayOptions ) ;

   // setup button information

   if (NULL != (lpBtnInfo = GetBtnInfo( GETHINST( hDlg ),
                                        SESSIONDIRBTNINFO )))
   {
      BmpBtnInfo.hInstance = GETHINST( hDlg ) ;
      for (i = 0; i < MAXLEN_SESSIONDIRBTNS; i++)
      {
         GetWindowText( GetDlgItem( hDlg, lpBtnInfo[ i * 3 ] ), szTemp,
                        MAXLEN_TEMPSTR ) ;
         BmpBtnInfo.lpText = (LPSTR) szTemp ;
         BmpBtnInfo.bmpUP = MAKEINTRESOURCE( lpBtnInfo[ i * 3 + 1 ] ) ;
         BmpBtnInfo.bmpDN = MAKEINTRESOURCE( lpBtnInfo[ i * 3 + 2 ] ) ;
         SendDlgItemMessage( hDlg, lpBtnInfo[ i * 3 ], BB_SETBMPBTNINFO,
                             NULL, (LPARAM) (LPBMPBTNINFO) &BmpBtnInfo ) ;
      }
      ReleaseBtnInfo( lpBtnInfo ) ;
   }
   else
      DP1( hWDB, "Unable to load BtnInfo resource!" ) ;

   return( TRUE ) ;

} // end of SessionDirDlgInit()

//************************************************************************
//  BOOL NEAR DlgFillSessionDirLB( HWND hDlg, BOOL NEAR *npDisplayOptions )
//
//  Description:
//     Fills the given listbox with session data (session directory
//     entries).
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//     int nIDListBox
//        listbox control ID
//
//  History:   Date       Author      Comment
//              2/17/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR DlgFillSessionDirLB( HWND hDlg, BOOL NEAR *npDisplayOptions )
{
   int        i ;
   char       szColumnName[ MAXLEN_TEMPSTR ],
              szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ] ;
   EPSSTRUCT  EPSStruct ;
   FARPROC    lpEnumProc ;
   HWND       hCtrlWnd ;
   WORD       wNumCols, wCurCol ;

   // read display option settings

   wNumCols = ReadSessionDirDisplayOptions( GETHINST( hDlg ), npDisplayOptions ) ;

   // get slider control handle

   hCtrlWnd = GetDlgItem( hDlg, IDD_SESSIONDIRSLIDER ) ;

   // set number of columns in listbox

   SendMessage( hCtrlWnd, SL_SETNUMCOLS, (WPARAM) wNumCols, NULL ) ;

   // set up slider header information

   wCurCol = 0 ;
   for (i = 0; i < MAXLEN_SESSIONDIRCOLS; i++)
   {
      if (npDisplayOptions[ i ])
      {
         LoadString( GETHINST( hDlg ), IDS_SESSIONDIRCOL_NAME + i, szColumnName,
                     MAXLEN_TEMPSTR ) ;
         SendMessage( hCtrlWnd, SL_SETCOLHEADER, wCurCol,
                      (LPARAM) (LPSTR) szColumnName ) ;
         wCurCol++ ;
      }
   }

   // get the listbox handle and clear the entries

   hCtrlWnd = GetDlgItem( hDlg, IDD_SESSIONDIRLB ) ;
   SendMessage( hCtrlWnd, LB_RESETCONTENT, NULL, NULL ) ;

   GetPhoneINIPath( szINIFile ) ;

   EPSStruct.hCtrlWnd = hCtrlWnd ;
   EPSStruct.npDisplayOptions = npDisplayOptions ;
   lstrcpy( EPSStruct.szINIFile, szINIFile ) ;
   EPSStruct.hInstance = GETHINST( hDlg ) ;

   lpEnumProc = MakeProcInstance( (FARPROC) EPSCallbackProc,
                                  GETHINST( hDlg ) ) ;

   EnumProfileSections( szINIFile, (LPENUMPROFSECTPROC) lpEnumProc,
                        (LPARAM) (LPSTR) &EPSStruct ) ;

   // enable dialing controls if there is at least one item

   if (SendDlgItemMessage( hDlg, IDD_SESSIONDIRLB, LB_GETCOUNT, NULL, NULL) > 0)
      EnableSessionDirControls( hDlg, TRUE ) ;

   FreeProcInstance( lpEnumProc ) ;

   return ( TRUE ) ;

} // end of DlgFillSessionDirLB()

//************************************************************************
//  BOOL FAR PASCAL EPSCallbackProc( LPSTR lpSection, LPARAM lParam )
//
//  Description:
//    Called back from EnumProfileSections().  LOWORD( lParam )
//    contains the listbox handle.
//
//  Parameters:
//     LPSTR lpSection
//        enumerated section name
//
//     LPARAM lParam
//        as passed to EnumProfileSections.
//
//  History:   Date       Author      Comment
//              2/ 2/92   BryanW      Wrote it.
//
//************************************************************************

BOOL FAR PASCAL EPSCallbackProc( LPSTR lpSection, LPARAM lParam )
{
   int     i, nSize ;
   char    szColData[ MAXLEN_COLUMNDATA ], szKey[ MAXLEN_TEMPSTR ],
           szTemp[ MAXLEN_TEMPSTR ] ;
   DWORD   dwColData ;
   NPSTR   npCurPos ;
   LPSTR   lpColData ;
   WORD    wSize ;

   if ((NULL != _fstrstr( lpSection, "Entry" )) &&
       (NULL == _fstrchr( lpSection, ',' )))
   {
      npCurPos = (NPSTR) szColData ;

      // first "column" contains the entry number

      *((WORD NEAR *)npCurPos) = AtoI( lpSection + 5 ) ;
      npCurPos += sizeof( WORD ) ;

      for (i = 0; i < MAXLEN_SESSIONDIRCOLS; i++)
      {
         if (NPDSPOPT( lParam )[ i ])
         {
            LoadString( HINST( lParam ), IDS_INI_ENTRYNAME + i, szKey,
                        MAXLEN_TEMPSTR ) ;
            GetPrivateProfileString( lpSection, szKey, "(none)", szTemp,
                                     MAXLEN_TEMPSTR, SZINIFILE( lParam ) ) ;
            wSize = lstrlen( szTemp ) + 1 ;
            *((WORD NEAR *) npCurPos) = wSize ;
            npCurPos += sizeof( WORD ) ;
            lstrcpy( npCurPos, szTemp ) ;
            npCurPos += wSize ;
         }
      }

      // create a buffer and add row data to listbox

      nSize = (int) (npCurPos - (NPSTR) &szColData) ;
      dwColData = FarLocalAlloc( LHND, nSize ) ;
      lpColData = FarLocalLock( dwColData ) ;
      _fmemcpy( lpColData, &szColData, nSize ) ;
      FarLocalUnlock( dwColData ) ;
      SendMessage( HCTRLWND( lParam ), LB_ADDSTRING, NULL,
                   (LPARAM) dwColData ) ;
   }

   return ( 1 ) ;

} // end of EPSCallbackProc()

//************************************************************************
//  WORD NEAR ReadSessionDirDisplayOptions( HINSTANCE hInstance,
//                                       BOOL NEAR *npDisplayOptions )
//
//  Description:
//    Fills the given array with the selected columns and
//    returns the total number of selected columns.
//
//  Parameters:
//     HINSTANCE hInstance
//        handle to application instance
//
//     BOOL NEAR *npDisplayOptions
//        array of display flags
//
//  History:   Date       Author      Comment
//              2/17/92   BryanW      Wrote it.
//
//************************************************************************

WORD NEAR ReadSessionDirDisplayOptions( HINSTANCE hInstance,
                                     BOOL NEAR *npDisplayOptions )
{
   int   i ;
   char  szKey[ MAXLEN_TEMPSTR ], szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ] ;
   WORD  wNumCols ;

   GetPhoneINIPath( szINIFile ) ;

   // read the settings

   wNumCols = 0 ;
   for (i = 0; i < MAXLEN_SESSIONDIRCOLS; i++)
   {
      LoadString( hInstance, IDS_INI_DISPLAYNAME + i, szKey,
                  MAXLEN_TEMPSTR ) ;
      npDisplayOptions[ i ] =
         GetPrivateProfileTrueFalse( "Options", szKey, "true", szINIFile ) ;
      if (npDisplayOptions[ i ])
         wNumCols++ ;
   }
   return ( wNumCols ) ;

} // end of ReadSessionDirDisplayOptions()

//************************************************************************
//  BOOL NEAR WriteSessionDirDisplayOptions( HINSTANCE hInstance,
//                                        BOOL NEAR *npDisplayOptions )
//
//  Description:
//     Writes the selected columns (display options) to the
//     session directory .INI file.
//
//  Parameters:
//     HINSTANCE hInstance
//        handle to application instance
//
//     BOOL NEAR *npDisplayOptions
//        array of display flags
//
//  History:   Date       Author      Comment
//              2/17/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR WriteSessionDirDisplayOptions( HINSTANCE hInstance,
                                      BOOL NEAR *npDisplayOptions )
{
   int   i ;
   char  szKey[ MAXLEN_TEMPSTR ], szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ] ;

   GetPhoneINIPath( szINIFile ) ;

   // write the settings

   for (i = 0; i < MAXLEN_SESSIONDIRCOLS; i++)
   {
      LoadString( hInstance, IDS_INI_DISPLAYNAME + i, szKey,
                  MAXLEN_TEMPSTR ) ;
      WritePrivateProfileTrueFalse( "Options", szKey, npDisplayOptions[ i ],
                                    szINIFile ) ;
   }

   return ( TRUE ) ;

} // end of WriteSessionDirDisplayOptions()

//************************************************************************
//  BOOL NEAR EnableSessionDirControls( HWND hDlg, BOOL fEnable )
//
//  Description:
//     Enables/disable dial controls.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//     BOOL fEnable
//        enable flag
//
//  History:   Date       Author      Comment
//              2/ 9/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR EnableSessionDirControls( HWND hDlg, BOOL fEnable )
{

   EnableWindow( GetDlgItem( hDlg, IDB_DIAL ), fEnable ) ;
   EnableWindow( GetDlgItem( hDlg, IDB_CONNECT ), fEnable ) ;
   EnableWindow( GetDlgItem( hDlg, IDB_EDIT ), fEnable ) ;
   EnableWindow( GetDlgItem( hDlg, IDB_DELETE ), fEnable ) ;

   return ( TRUE ) ;

} // end of EnableSessionDirControls()

//************************************************************************
//  BOOL NEAR SessionDirDlgTerm( HWND hDlg )
//
//  Description:
//     Performs clean-up (termination) for dialog box.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//  History:   Date       Author      Comment
//              2/17/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR SessionDirDlgTerm( HWND hDlg )
{
   BOOL NEAR *npDisplayOptions ;

   // free memory for display options array

   npDisplayOptions = (BOOL NEAR *) GET_PROP( hDlg, ATOM_DISPLAYOPTIONS ) ;
   LocalFree( (HLOCAL) npDisplayOptions ) ;

   REMOVE_PROP( hDlg, ATOM_XSIZE ) ;
   REMOVE_PROP( hDlg, ATOM_YSIZE ) ;
   REMOVE_PROP( hDlg, ATOM_DISPLAYOPTIONS ) ;

   // notify parent that we are shutting down

   PostMessage( GetParent( hDlg ), WM_COMMAND, SESSIONDIRDLG,
                MAKELONG( hDlg, DN_ENDDIALOG ) ) ;

   return ( TRUE ) ;

} // end of SessionDirDlgTerm()

//************************************************************************
//  BOOL FAR PASCAL SessionDirDlgProc( HWND hDlg, UINT uMsg,
//                                  WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This is the dialing directory dialog box.
//
//  Parameters:
//     Same as all standard dialog procedures.
//
//  History:   Date       Author      Comment
//              2/ 4/92   BryanW      Added this comment block.
//
//************************************************************************

BOOL FAR PASCAL SessionDirDlgProc( HWND hDlg, UINT uMsg,
                                WPARAM wParam, LPARAM lParam )
{
   static SESSIONDIRDLGWNDOFFSETS  DialDlgWndOffsets ;

   switch (uMsg)
   {
      case WM_INITDIALOG:
      {
         HWND            hCtrlWnd ;
         LPWORD          lpBtnInfo ;
         WORD            wYSize, wXSize ;
         RECT            rcDlg, rcControl ;

         if (!SessionDirDlgInit( hDlg ))
            return ( FALSE ) ;
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;

         // get and save initial size of dialog box

         GetWindowRect( hDlg, &rcDlg ) ;
         wYSize = rcDlg.bottom - rcDlg.top ;
         wXSize = rcDlg.right - rcDlg.left ;
         SET_PROP( hDlg, ATOM_XSIZE, (HANDLE) wXSize ) ;
         SET_PROP( hDlg, ATOM_YSIZE, (HANDLE) wYSize ) ;

         // compute offsets for frame rect

         hCtrlWnd = GetDlgItem( hDlg, IDD_DLGFRAME1 ) ;
         GetWindowRect( hCtrlWnd, &rcControl ) ;

         DialDlgWndOffsets.ptFrameTop.x = rcControl.left - rcDlg.left ;
         DialDlgWndOffsets.ptFrameTop.y = rcControl.top - rcDlg.top ;
         DialDlgWndOffsets.ptFrameBottom.x = rcDlg.right - rcControl.right ;
         DialDlgWndOffsets.ptFrameBottom.y = rcDlg.bottom - rcControl.bottom ;

         // compute offsets for listbox

         hCtrlWnd = GetDlgItem( hDlg, IDD_SESSIONDIRLB ) ;
         GetWindowRect( hCtrlWnd, &rcControl ) ;

         DialDlgWndOffsets.ptLBTop.x = rcControl.left - rcDlg.left ;
         DialDlgWndOffsets.ptLBTop.y = rcControl.top - rcDlg.top ;
         DialDlgWndOffsets.ptLBBottom.x = rcDlg.right - rcControl.right ;
         DialDlgWndOffsets.ptLBBottom.y = rcDlg.bottom - rcControl.bottom ;

         // compute offsets for slider

         hCtrlWnd = GetDlgItem( hDlg, IDD_SESSIONDIRSLIDER ) ;
         GetWindowRect( hCtrlWnd, &rcControl ) ;

         DialDlgWndOffsets.ptSliderTop.x = rcControl.left - rcDlg.left ;
         DialDlgWndOffsets.ptSliderTop.y = rcControl.top - rcDlg.top ;
         DialDlgWndOffsets.ptSliderBottom.x = rcDlg.right - rcControl.right ;
         DialDlgWndOffsets.ptSliderBottom.y = rcDlg.bottom - rcControl.bottom ;

         // compute offsets for buttons

         if (NULL != (lpBtnInfo = GetBtnInfo( GETHINST( hDlg ),
                                              SESSIONDIRBTNINFO )))
         {
            hCtrlWnd = GetDlgItem( hDlg, lpBtnInfo[ 0 ] ) ;
            GetWindowRect( hCtrlWnd, &rcControl ) ;
            DialDlgWndOffsets.ptButtons.x = rcDlg.right - rcControl.left ;
            ReleaseBtnInfo( lpBtnInfo ) ;
         }
         else
            DP1( hWDB, "Unable to load BtnInfo resource!" ) ;

         return ( ShadowControl( hDlg, uMsg, wParam, RGBLTGRAY ) ) ;
      }

      case WM_MEASUREITEM:
         DlgMeasureItem( hDlg, (LPMEASUREITEMSTRUCT) lParam ) ;
         break ;

      case WM_DRAWITEM:
         DlgDrawItem( hDlg, (LPDRAWITEMSTRUCT) lParam ) ;
         break ;

      case WM_COMPAREITEM:
         return( DlgCompareItem( hDlg, (LPCOMPAREITEMSTRUCT) lParam ) ) ;

      case WM_GETMINMAXINFO:
      {
         WORD  wXSize, wYSize ;

         wXSize = (WORD) GET_PROP( hDlg, ATOM_XSIZE ) ;
         wYSize = (WORD) GET_PROP( hDlg, ATOM_YSIZE ) ;

         ((MINMAXINFO FAR *) lParam) -> ptMinTrackSize.x = wXSize ;
         ((MINMAXINFO FAR *) lParam) -> ptMinTrackSize.y = wYSize ;
         ((MINMAXINFO FAR *) lParam) -> ptMaxTrackSize.y = wYSize ;
      }
      break ;

      case WM_SIZE:
      {
         int     i ;
         POINT   ptTop, ptBottom ;
         RECT    rcDlg, rcButton ;
         HWND    hCtrlWnd ;
         HANDLE  hWndPosInfo ;
         LPWORD  lpBtnInfo ;

         GetWindowRect( hDlg, &rcDlg ) ;

         hWndPosInfo = BeginDeferWindowPos( MAXLEN_SESSIONDIRBTNS + 3 ) ;

         // move frame into position

         ptTop.x = rcDlg.left + DialDlgWndOffsets.ptFrameTop.x ;
         ptTop.y = rcDlg.top + DialDlgWndOffsets.ptFrameTop.y ;
         ScreenToClient( hDlg, &ptTop ) ;

         ptBottom.x = rcDlg.right - DialDlgWndOffsets.ptFrameBottom.x ;
         ptBottom.y = rcDlg.bottom - DialDlgWndOffsets.ptFrameBottom.y ;
         ScreenToClient( hDlg, &ptBottom ) ;

         hCtrlWnd = GetDlgItem( hDlg, IDD_DLGFRAME1 ) ;

         DeferWindowPos( hWndPosInfo, hCtrlWnd, NULL, ptTop.x, ptTop.y,
                         ptBottom.x - ptTop.x, ptBottom.y - ptTop.y,
                         SWP_NOZORDER | SWP_SHOWWINDOW ) ;

         // move listbox into position

         ptTop.x = rcDlg.left + DialDlgWndOffsets.ptLBTop.x ;
         ptTop.y = rcDlg.top + DialDlgWndOffsets.ptLBTop.y ;
         ScreenToClient( hDlg, &ptTop ) ;

         ptBottom.x = rcDlg.right - DialDlgWndOffsets.ptLBBottom.x ;
         ptBottom.y = rcDlg.bottom - DialDlgWndOffsets.ptLBBottom.y ;
         ScreenToClient( hDlg, &ptBottom ) ;

         hCtrlWnd = GetDlgItem( hDlg, IDD_SESSIONDIRLB ) ;
         DeferWindowPos( hWndPosInfo, hCtrlWnd, NULL, ptTop.x, ptTop.y,
                         ptBottom.x - ptTop.x, ptBottom.y - ptTop.y,
                         SWP_NOZORDER | SWP_SHOWWINDOW ) ;

         // move slider into position

         ptTop.x = rcDlg.left + DialDlgWndOffsets.ptSliderTop.x ;
         ptTop.y = rcDlg.top + DialDlgWndOffsets.ptSliderTop.y ;
         ScreenToClient( hDlg, &ptTop ) ;

         ptBottom.x = rcDlg.right - DialDlgWndOffsets.ptSliderBottom.x ;
         ptBottom.y = rcDlg.bottom - DialDlgWndOffsets.ptSliderBottom.y ;
         ScreenToClient( hDlg, &ptBottom ) ;

         hCtrlWnd = GetDlgItem( hDlg, IDD_SESSIONDIRSLIDER ) ;
         DeferWindowPos( hWndPosInfo, hCtrlWnd, NULL, ptTop.x, ptTop.y,
                         ptBottom.x - ptTop.x, ptBottom.y - ptTop.y,
                         SWP_NOZORDER | SWP_SHOWWINDOW ) ;

         // compute locations and move buttons

         if (NULL != (lpBtnInfo = GetBtnInfo( GETHINST( hDlg ),
                                              SESSIONDIRBTNINFO )))
         {
            for (i = 0; i < MAXLEN_SESSIONDIRBTNS; i++)
            {
               hCtrlWnd = GetDlgItem( hDlg, lpBtnInfo[ i * 3 ] ) ;
               GetWindowRect( hCtrlWnd, &rcButton ) ;
               ptTop.x = rcDlg.right - DialDlgWndOffsets.ptButtons.x ;
               ptTop.y = rcButton.top ;
               ptBottom.x = ptTop.x + (rcButton.right - rcButton.left) ;
               ptBottom.y = ptTop.y + (rcButton.bottom - rcButton.top) ;
               ScreenToClient( hDlg, &ptTop ) ;
               ScreenToClient( hDlg, &ptBottom ) ;
               DeferWindowPos( hWndPosInfo, hCtrlWnd, NULL, ptTop.x, ptTop.y,
                               ptBottom.x - ptTop.x, ptBottom.y - ptTop.y,
                               SWP_NOZORDER | SWP_SHOWWINDOW ) ;
            }
            ReleaseBtnInfo( lpBtnInfo ) ;
         }
         else
            DP1( hWDB, "Unable to load BtnInfo resource!" ) ;

         EndDeferWindowPos( hWndPosInfo ) ;

         // invalidate listbox

         InvalidateRect( GetDlgItem( hDlg, IDD_SESSIONDIRLB ), NULL, FALSE ) ;

         // invalidate dialog box

         InvalidateRect( hDlg, NULL, TRUE ) ;
      }
      break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDD_SESSIONDIRSLIDER:
            {
               HWND  hLB ;
               RECT  rcClient ;

               hLB = GetDlgItem( hDlg, IDD_SESSIONDIRLB ) ;
               GetClientRect( hLB, &rcClient ) ;

               switch (HIWORD( lParam ))
               {
                  case SLN_STARTDRAG:
                  {
                     int nNewSliderPos ;

                     nNewSliderPos = 
                        (int) LOWORD( SendDlgItemMessage( hDlg,
                                                          IDD_SESSIONDIRSLIDER,
                                                          SL_GETCOLPOS,
                                                          LOWORD( lParam ),
                                                          NULL ) ) ;
                     UpdateSliderPos( hLB, &rcClient, -1, nNewSliderPos ) ;
                     SET_PROP( hDlg, ATOM_SLIDERPOS, (HANDLE) nNewSliderPos ) ;
                  }
                  break ;

                  case SLN_INDRAG:
                  {
                     int nOldSliderPos, nNewSliderPos ;

                     nOldSliderPos =
                        (int) GET_PROP( hDlg, ATOM_SLIDERPOS ) ;
                     nNewSliderPos =
                        (int) LOWORD( SendDlgItemMessage( hDlg,
                                                          IDD_SESSIONDIRSLIDER,
                                                          SL_GETCOLPOS,
                                                          LOWORD( lParam ),
                                                          NULL ) ) ;
                     UpdateSliderPos( hLB, &rcClient, nOldSliderPos,
                                      nNewSliderPos ) ;
                     SET_PROP( hDlg, ATOM_SLIDERPOS, (HANDLE) nNewSliderPos ) ;
                  }
                  break ;

                  case SLN_ENDDRAG:
                  {
                     int nOldSliderPos ;

                     nOldSliderPos =
                        (int) GET_PROP( hDlg, ATOM_SLIDERPOS ) ; 
                     UpdateSliderPos( hLB, &rcClient, nOldSliderPos, -1 ) ;
                     REMOVE_PROP( hDlg, ATOM_SLIDERPOS ) ;
                     InvalidateRect( hLB, NULL, FALSE ) ;
                  }
                  break ;
               }
            }
            break ;

            case IDB_DIAL:
            {
               BOOL    fFocus ;
               DWORD   dwItemData ;
               WORD    wCurSel, wNumSel ;
               HLOCAL  hSelArray ;
               PWORD   npSelArray ;
               LPWORD  lpItemData ;

               wNumSel =
                  LOWORD( SendDlgItemMessage( hDlg, IDD_SESSIONDIRLB,
                                              LB_GETSELCOUNT, NULL, NULL ) ) ;

               fFocus = FALSE ;
               if (0 == wNumSel || LB_ERR == wNumSel)
               {
                  // there is only one selection - the one with focus

                  fFocus = TRUE ;
                  wNumSel = 1 ;
               }
               if (NULL == (hSelArray = LocalAlloc( LHND,
                                                    wNumSel * sizeof( WORD ) )))
               {
                  // NEED: put up ASSERTION box
                  // failure return false

                  MessageBeep( 0 ) ;
                  return ( FALSE ) ;
               }
               npSelArray = (PWORD) LocalLock( hSelArray ) ;

               // fill the selection array

               if (fFocus)
                  *npSelArray =
                     LOWORD( SendDlgItemMessage( hDlg, IDD_SESSIONDIRLB,
                                                 LB_GETCARETINDEX,
                                                 NULL, NULL ) ) ;
               else
                  SendDlgItemMessage( hDlg, IDD_SESSIONDIRLB,
                                      LB_GETSELITEMS, wNumSel,
                                      (LPARAM) (LPWORD) npSelArray ) ;


               // get the entry number for each selected item

               for (wCurSel = 0; wCurSel < wNumSel; wCurSel++)
               {
                  dwItemData =
                     SendDlgItemMessage( hDlg, IDD_SESSIONDIRLB,
                                         LB_GETITEMDATA, npSelArray[ wCurSel ],
                                         NULL ) ;
                  lpItemData = (LPWORD) FarLocalLock( dwItemData ) ;
                  npSelArray[ wCurSel ] = *lpItemData ;
                  FarLocalUnlock( dwItemData ) ;
               }

               // perform dialing operation... if returns
               // true we will post a notification to close
               // this dialog box

               if (DoModalDialogBoxParam( GETHINST( hDlg ),
                                          MAKEINTRESOURCE( DIALINGDLG ),
                                          hDlg, DialingDlgProc,
                                          MAKELONG( wNumSel, npSelArray ) ))
                  PostMessage( hDlg, WM_COMMAND, IDOK, NULL ) ;

               // clean up

               LocalUnlock( hSelArray ) ;
               LocalFree( hSelArray ) ;
            }
            break ;

            case IDB_CONNECT:
            {
               char             szEntryName[ MAXLEN_TEMPSTR ] ;
               DWORD            dwItemData ;
               HWND             hMDIChildWnd ;
               WORD             wCurSel, wEntry ;
               LPWORD           lpItemData ;
               MDICREATESTRUCT  MDIcs ;

               wCurSel =
                  LOWORD( SendDlgItemMessage( hDlg, IDD_SESSIONDIRLB,
                                              LB_GETCARETINDEX,
                                              NULL, NULL ) ) ;
               if (LB_ERR == wCurSel)
               {
                  MessageBeep( 1 ) ;
                  return ( FALSE ) ;
               }

               // get entry slot number

               dwItemData =
                  SendDlgItemMessage( hDlg, IDD_SESSIONDIRLB, LB_GETITEMDATA,
                                      wCurSel, NULL ) ;
               lpItemData = (LPWORD) FarLocalLock( dwItemData ) ;
               wEntry = *lpItemData ;
               FarLocalUnlock( dwItemData ) ;
               wsprintf( szEntryName, "Entry%04d", wEntry ) ;

               // create the MDI window

               MDIcs.szClass = szTerminalClass ;
               MDIcs.szTitle = "Terminal" ;
               MDIcs.hOwner =  GETHINST( hDlg ) ;
               MDIcs.x =       CW_USEDEFAULT ;
               MDIcs.y =       CW_USEDEFAULT ;
               MDIcs.cx =      CW_USEDEFAULT ;
               MDIcs.cy =      CW_USEDEFAULT ;
               MDIcs.style =   0 ;
               MDIcs.lParam =  (LONG) (LPSTR) szEntryName ;

               hMDIChildWnd =
                  (HWND) LOWORD( SendMessage( hClientWnd, WM_MDICREATE, 0,
                                              (LONG) (LPMDICREATESTRUCT) &MDIcs ) ) ;
               return ( TRUE ) ;
            }

            case IDB_DELETE:
               break ;

            case IDB_EDIT:
            case IDB_NEW:
            {
               BOOL NEAR  *npDisplayOptions ;
               DWORD      dwItemData ;
               WORD       wCurSel, wEntry ;
               LPWORD     lpItemData ;

               if (IDB_EDIT == wParam)
               {
                  wCurSel =
                     LOWORD( SendDlgItemMessage( hDlg, IDD_SESSIONDIRLB,
                                                 LB_GETCARETINDEX,
                                                 NULL, NULL ) ) ;
                  if (LB_ERR == wCurSel)
                  {
                     MessageBeep( 1 ) ;
                     return ( FALSE ) ;
                  }

                  // get entry slot number for editing

                  dwItemData =
                     SendDlgItemMessage( hDlg, IDD_SESSIONDIRLB, LB_GETITEMDATA,
                                         wCurSel, NULL ) ;
                  lpItemData = (LPWORD) FarLocalLock( dwItemData ) ;
                  wEntry = *lpItemData ;
                  FarLocalUnlock( dwItemData ) ;

               }
               else
                  wEntry = 0 ;

               npDisplayOptions =
                  (BOOL NEAR *) GET_PROP( hDlg, ATOM_DISPLAYOPTIONS ) ;

               if (DoModalDialogBoxParam( GETHINST( hDlg ),
                                          MAKEINTRESOURCE( SESSIONDIRENTRYDLG ),
                                          hDlg, SessionDirEntryDlgProc,
                                          MAKELONG( wEntry, 0 )))
                  DlgFillSessionDirLB( hDlg, npDisplayOptions ) ;
            }
            break ;

            case IDB_OPTIONS:
            {
               BOOL NEAR  *npDisplayOptions ;

               npDisplayOptions =
                  (BOOL NEAR *) GET_PROP( hDlg, ATOM_DISPLAYOPTIONS ) ;
               if (DoModalDialogBoxParam( GETHINST( hDlg ),
                                          MAKEINTRESOURCE( SESSIONDIROPTIONSDLG ),
                                          hDlg, SessionDirOptionsDlgProc,
                                          MAKELONG( npDisplayOptions, NULL )))
                  DlgFillSessionDirLB( hDlg, npDisplayOptions ) ;
            }
            break ;

            case IDOK:
            case IDCANCEL:
               SessionDirDlgTerm( hDlg ) ;
               DestroyWindow( hDlg ) ;
               return ( TRUE ) ;
         }
         break ;

      case WM_DESTROY:
         return ( ShadowControl( hDlg, uMsg, wParam, lParam ) ) ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;

         BeginPaint(hDlg, &ps ) ;
         EndPaint( hDlg, &ps ) ;
      }

      // ... Fall through ...

      default:
         return ShadowControl( hDlg, uMsg, wParam, lParam ) ;
   }
   return ( FALSE ) ;

} // end of SessionDirDlgProc()

//************************************************************************
//  BOOL NEAR SessionDirEntryDlgInit( HWND hDlg, LPARAM lParam )
//
//  Description:
//     Initialization procedure for the Phone Book Entry Dialog.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//  History:   Date       Author      Comment
//              2/ 5/92   BryanW      Added this comment block.
//
//************************************************************************

BOOL NEAR SessionDirEntryDlgInit( HWND hDlg, LPARAM lParam )
{
   char  szBuf[ MAXLEN_TEMPSTR ], szEntryName[ MAXLEN_TEMPSTR ],
         szTemp[ MAXLEN_TEMPSTR ],
         szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ] ;
   WORD  wEntry, wSelection ;

   GetPhoneINIPath( szINIFile ) ;

   // disable OK button - will be enabled by updating edit controls

   EnableWindow( GetDlgItem( hDlg, IDOK ), FALSE ) ;

   // list available connectors & emulators

   DlgModuleListCB( hDlg, IDD_CONNECTORCB, CONNECTOR_EXT ) ;
   DlgModuleListCB( hDlg, IDD_EMULATORCB, EMULATOR_EXT ) ;
   DlgModuleListCB( hDlg, IDD_PROTOCOLCB, PROTOCOL_EXT ) ; 

   // list available modems

   DlgModemListCB( hDlg, IDD_MODEMCB, IDS_MODEM_2224B ) ;

   // set maximums for name and number

   SendDlgItemMessage( hDlg, IDD_NAMEEC, EM_LIMITTEXT,
                       MAXLEN_SESSIONDIRNAME, NULL ) ;

   SendDlgItemMessage( hDlg, IDD_NUMBEREC, EM_LIMITTEXT,
                       MAXLEN_SESSIONDIRNUMBER, NULL ) ;

   // get the slot for temporary storage (or to read current
   // settings if we're editing)

   if (NULL == lParam)
   {
      wEntry = GetPrivateProfileWord( "Options", "Entries",
                                      "0", szINIFile ) + 1 ;
      SET_PROP( hDlg, ATOM_INEDITMODE, (HANDLE) FALSE ) ;
   }
   else
   {
      wEntry = LOWORD( lParam ) ;
      wsprintf( szEntryName, "Entry%04d", wEntry ) ;

      // read settings into session directory entry

      GetPrivateProfileString( szEntryName, "Name", "(none)",
                               szTemp, MAXLEN_TEMPSTR, szINIFile ) ;
      SetDlgItemText( hDlg, IDD_NAMEEC, szTemp ) ;

      GetPrivateProfileString( szEntryName, "Number", "(none)",
                               szTemp, MAXLEN_TEMPSTR, szINIFile ) ;
      SetDlgItemText( hDlg, IDD_NUMBEREC, szTemp ) ;

      // set initial connector setting

      GetPrivateProfileString( szEntryName, "Connector", "(none)",
                               szTemp, MAXLEN_TEMPSTR, szINIFile ) ;
      wSelection =
         CBFindModule( GetDlgItem( hDlg, IDD_CONNECTORCB ), szTemp ) ;
      if (-1 != wSelection)
         SendDlgItemMessage( hDlg, IDD_CONNECTORCB, CB_SETCURSEL,
                             wSelection, NULL ) ;

      // set initial emulator setting

      GetPrivateProfileString( szEntryName, "Emulator", "(none)",
                               szTemp, MAXLEN_TEMPSTR, szINIFile ) ;
      wSelection =
         CBFindModule( GetDlgItem( hDlg, IDD_EMULATORCB ), szTemp ) ;
      if (-1 != wSelection)
         SendDlgItemMessage( hDlg, IDD_EMULATORCB, CB_SETCURSEL,
                             wSelection, NULL ) ;

      // set initial protocol setting

      GetPrivateProfileString( szEntryName, "Protocol", "(none)",
                               szTemp, MAXLEN_TEMPSTR, szINIFile ) ;
      wSelection =
         CBFindModule( GetDlgItem( hDlg, IDD_PROTOCOLCB ), szTemp ) ;
      if (-1 != wSelection)
         SendDlgItemMessage( hDlg, IDD_PROTOCOLCB, CB_SETCURSEL,
                             wSelection, NULL ) ;


      // set initial modem setting

      GetPrivateProfileString( szEntryName, "Modem", "(none)",
                               szTemp, MAXLEN_TEMPSTR, szINIFile ) ;
      SendDlgItemMessage( hDlg, IDD_MODEMCB, CB_SELECTSTRING,
                          (WPARAM) 0xFFFF, (LPARAM) (LPSTR) szTemp ) ;

      SET_PROP( hDlg, ATOM_INEDITMODE, (HANDLE) TRUE ) ;
   }

   // set title of dialog box

   GetWindowText( hDlg, szTemp, sizeof( szTemp ) ) ;
   wsprintf( szBuf, szTemp,
             (lParam == NULL) ? (LPSTR) "Creating" : "Editing" ) ;
   SetWindowText( hDlg, szBuf ) ;


   DPF3( hWDB, "SessionDirEntryDlgInit: wEntry = %d\r\n", wEntry ) ;
   SET_PROP( hDlg, ATOM_ENTRY, (HANDLE) wEntry ) ;
   SET_PROP( hDlg, ATOM_MODEM, (HANDLE) FALSE ) ;

   return ( TRUE ) ;

} // end of SessionDirEntryDlgInit()

//------------------------------------------------------------------------
//  BOOL NEAR DlgModemListCB( HWND hDlg, int nIDComboBox, int nIDString )
//
//  Description:
//     Fills the modem combo box with the available modem list
//     and associates the string ID with each entry.
//
//  Parameters:
//     HWND hDlg
//        handle to parent dialog box
//
//     WORD wEntry
//        entry number for creating/editing
//
//     int nIDComboBox
//        id of the modem combo box
//
//     int nIDString
//        id of the first modem string resource
//
//  History:   Date       Author      Comment
//              2/23/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR DlgModemListCB( HWND hDlg, int nIDComboBox, int nIDString )
{
   int    i ;
   char   szModemConfig[ MAXLEN_MODEMSTR ] ;
   WORD   wPosition ;
   LPSTR  lpModemNameTerm ;

   for (i = 0; i < MAXLEN_MODEMLIST; i++)
   {
      // load the string from the string resources and parse
      // the modem name

      LoadString( GETHINST( hDlg ), nIDString + i,
                  szModemConfig, sizeof( szModemConfig ) ) ;
      lpModemNameTerm = _fstrchr( szModemConfig, ',' ) ;
      if (NULL != lpModemNameTerm)
      {
         *lpModemNameTerm = NULL ;

         wPosition =
            LOWORD( SendDlgItemMessage( hDlg, nIDComboBox, CB_ADDSTRING,
                                        NULL,
                                        (LPARAM) (LPSTR) szModemConfig ) ) ;

         // use item data to store the actual string id

         SendDlgItemMessage( hDlg, nIDComboBox, CB_SETITEMDATA,
                           (WPARAM) wPosition,
                           (LPARAM) (LONG) (nIDString + i ) ) ;
      }
   }

   // NEED: find a default setting for modem, perhaps HAYES
   // is a better selection

   // select first item in combo box

   SendDlgItemMessage( hDlg, nIDComboBox, CB_SETCURSEL, (WPARAM) 1,
                       NULL ) ;
   return ( TRUE ) ;

} // end of DlgModemListCB()

//************************************************************************
//  WORD NEAR CBFindModule( HWND hCB, LPSTR lpModule )
//
//  Description:
//     Returns the index of the item associated with atomModule or
//     -1 if not found or an error occurs.
//
//  Parameters:
//     HWND hB
//        handle to combo-box
//
//     LPSTR lpModule
//        pointer to module name string
//
//  History:   Date       Author      Comment
//              2/17/92   BryanW      Wrote it.
//
//************************************************************************

WORD NEAR CBFindModule( HWND hCB, LPSTR lpModule )
{
   ATOM  atomModule;
   WORD  wCount, wCurrent, wItem ;

   wItem = 0xFFFF ;
   if (wCount = LOWORD( SendMessage( hCB, CB_GETCOUNT, NULL, NULL ) ))
   {
      atomModule = AddAtom( lpModule ) ;
      for (wCurrent = 0; wCurrent < wCount; wCurrent++)
      {
         if (atomModule == LOWORD( SendMessage( hCB, CB_GETITEMDATA,
                                                (WPARAM) wCurrent, NULL ) ))
         {
            wItem = wCurrent ;
            break ;
         }
      }
      DeleteAtom( atomModule ) ;
   }
   return ( wItem ) ;

} // end of CBFindModule()

//************************************************************************
//  BOOL NEAR SessionDirEntryDlgTerm( HWND hDlg, BOOL fExitOK )
//
//  Description:
//     Handles the termination (clean-up) of the session directory
//     entry dialog box.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//     BOOL fExitOK
//        flag to designate exiting with "cancel" or "ok".
//
//  History:   Date       Author      Comment
//              2/ 9/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR SessionDirEntryDlgTerm( HWND hDlg, BOOL fExitOK )
{
   int    i ;
   char   szTemp[ MAXLEN_TEMPSTR ], szEntryName[ MAXLEN_TEMPSTR ],
          szModuleName[ MAXLEN_MODULENAME ], szKey[ MAXLEN_TEMPSTR ],
          szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ],
          szModemConfig[ MAXLEN_MODEMSTR ] ;
   ATOM   atomModuleName ;
   WORD   wEntry, wItemSel, wModem ;
   LPSTR  lpCur, lpEnd ;

   if (fExitOK)
   {
      // create section name

      wEntry = (WORD) GET_PROP( hDlg, ATOM_ENTRY ) ;
      wsprintf( szEntryName, "Entry%04d", wEntry ) ;

      GetPhoneINIPath( szINIFile ) ;

      // save settings into session directory

      GetDlgItemText( hDlg, IDD_NAMEEC, szTemp, MAXLEN_SESSIONDIRNAME ) ;
      WritePrivateProfileString( szEntryName, "Name", szTemp, szINIFile ) ;

      GetDlgItemText( hDlg, IDD_NUMBEREC, szTemp, MAXLEN_SESSIONDIRNUMBER ) ;
      WritePrivateProfileString( szEntryName, "Number", szTemp, szINIFile ) ;

      // get module names for connector, emulator and protocol and write
      // to session directory entry

      wItemSel =
         LOWORD( SendDlgItemMessage( hDlg, IDD_CONNECTORCB, CB_GETCURSEL,
                                     NULL, NULL ) ) ;
      atomModuleName =
         LOWORD( SendDlgItemMessage( hDlg, IDD_CONNECTORCB, CB_GETITEMDATA,
                                     wItemSel, NULL ) ) ;
      GetAtomName( atomModuleName, szModuleName, MAXLEN_MODULENAME ) ;
      WritePrivateProfileString( szEntryName, "Connector", szModuleName,
                                 szINIFile ) ;

      wItemSel =
         LOWORD( SendDlgItemMessage( hDlg, IDD_EMULATORCB, CB_GETCURSEL,
                                     NULL, NULL ) ) ;
      atomModuleName =
         LOWORD( SendDlgItemMessage( hDlg, IDD_EMULATORCB, CB_GETITEMDATA,
                                     wItemSel, NULL ) ) ;
      GetAtomName( atomModuleName, szModuleName, MAXLEN_MODULENAME ) ;
      WritePrivateProfileString( szEntryName, "Emulator", szModuleName,
                                 szINIFile ) ;

      wItemSel =
         LOWORD( SendDlgItemMessage( hDlg, IDD_PROTOCOLCB, CB_GETCURSEL,
                                     NULL, NULL ) ) ;
      atomModuleName =
         LOWORD( SendDlgItemMessage( hDlg, IDD_PROTOCOLCB, CB_GETITEMDATA,
                                     wItemSel, NULL ) ) ;
      GetAtomName( atomModuleName, szModuleName, MAXLEN_MODULENAME ) ;
      WritePrivateProfileString( szEntryName, "Protocol", szModuleName,
                                 szINIFile ) ;

      // if the selection wasn't already made through the
      // the modem setup dialog, write settings for modem
      // strings

      if (!(BOOL) GET_PROP( hDlg, ATOM_MODEM ))
      {
         wItemSel =
            LOWORD( SendDlgItemMessage( hDlg, IDD_MODEMCB, CB_GETCURSEL,
                                        NULL, NULL ) ) ;
         if (CB_ERR == wItemSel)
         {
            MessageBeep( 1 ) ;
            return( FALSE ) ;
         }
         wModem =
            LOWORD( SendDlgItemMessage( hDlg, IDD_MODEMCB, CB_GETITEMDATA,
                                        (WPARAM) wItemSel, NULL ) ) ;

         // load the configuration string

         LoadString( GETHINST( hDlg ), wModem, szModemConfig,
                     sizeof( szModemConfig ) ) ;

         // write the modem name

         lpEnd = _fstrchr( szModemConfig, ',' ) ;
         *lpEnd++ = NULL ;

         LoadString( GETHINST( hDlg ), IDS_INI_ENTRYMODEM, szKey,
                     sizeof( szKey ) ) ;
         WritePrivateProfileString( szEntryName, szKey, szModemConfig,
                                    szINIFile ) ;
         lpCur = lpEnd ;

         // write each modem string

         for (i = IDS_INI_DIALPREFIX; i <= IDS_INI_RINGING; i++)
         {
            if (IDS_INI_RINGING != i)
            {
               lpEnd = _fstrchr( lpCur, ',' ) ;

               // null terminate current string

               if (NULL != lpEnd)
                  *lpEnd++ = NULL ;
            }

            LoadString( GETHINST( hDlg ), i, szKey, sizeof( szKey ) ) ;
            WritePrivateProfileString( szEntryName, szKey, lpCur,
                                       szINIFile ) ;
            lpCur = lpEnd ;
         }
      }


      // update entries count

      if (!((BOOL) GET_PROP( hDlg, ATOM_INEDITMODE )))
         WritePrivateProfileWord( "Options", "Entries", wEntry, szINIFile ) ;

   }

   REMOVE_PROP( hDlg, ATOM_MODEM ) ;
   REMOVE_PROP( hDlg, ATOM_ENTRY ) ;
   REMOVE_PROP( hDlg, ATOM_INEDITMODE ) ;
   return ( TRUE ) ;

} // end of SessionDirEntryDlgTerm()

//************************************************************************
//  BOOL FAR PASCAL SessionDirEntryDlgProc( HWND hDlg, UINT uMsg,
//                                       WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This is the session directory entry dialog procedure.
//
//  Parameters:
//     Same as all standard dialog procedures.
//
//  History:   Date       Author      Comment
//              2/ 4/92   BryanW      Added this comment block.
//
//************************************************************************

BOOL FAR PASCAL SessionDirEntryDlgProc( HWND hDlg, UINT uMsg,
                                     WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_INITDIALOG:
         if (!SessionDirEntryDlgInit( hDlg, lParam ))
            return ( FALSE ) ;
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;
         return ( ShadowControl( hDlg, uMsg, wParam, RGBLTGRAY ) ) ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDD_NAMEEC:
            {
               switch (HIWORD( lParam ))
               {
                  case EN_UPDATE:
                     if (SendDlgItemMessage( hDlg, IDD_NAMEEC,
                                             WM_GETTEXTLENGTH, NULL, NULL ))
                        EnableWindow( GetDlgItem( hDlg, IDOK ), TRUE ) ;
                     else
                        EnableWindow( GetDlgItem( hDlg, IDOK ), FALSE ) ;
               }
            }
            break ;

            case IDD_CONNECTORSETUP:
            {
               char  szEntryName[ MAXLEN_TEMPSTR ] ;
               WORD  wEntry ;

               wEntry = (WORD) GET_PROP( hDlg, ATOM_ENTRY ) ;
               wsprintf( szEntryName, "Entry%04d", wEntry ) ;
               DlgConnectorSetup( hDlg, szEntryName, IDD_CONNECTORCB ) ;
            }
            break ;

            case IDD_EMULATORSETUP:
            {
               char  szEntryName[ MAXLEN_TEMPSTR ] ;
               WORD  wEntry ;

               wEntry = (WORD) GET_PROP( hDlg, ATOM_ENTRY ) ;
               wsprintf( szEntryName, "Entry%04d", wEntry ) ;
               DlgEmulatorSetup( hDlg, szEntryName, IDD_EMULATORCB ) ;
            }
            break ;

            case IDD_MODEMSETUP:
            {
               WORD  wEntry, wSelection, wModem ;

               wEntry = (WORD) GET_PROP( hDlg, ATOM_ENTRY ) ;
               wSelection =
                  LOWORD( SendDlgItemMessage( hDlg, IDD_MODEMCB,
                                              CB_GETCURSEL,
                                              NULL, NULL ) ) ;
               if (CB_ERR == wSelection)
               {
                  MessageBeep( 1 ) ;
                  return( FALSE ) ;
               }
               wModem =
                  LOWORD( SendDlgItemMessage( hDlg, IDD_MODEMCB,
                                              CB_GETITEMDATA,
                                              (WPARAM) wSelection, NULL ) ) ;
               if (DoModalDialogBoxParam( GETHINST( hDlg ),
                                          MAKEINTRESOURCE( MODEMSETTINGSDLG ),
                                          hDlg, ModemSettingsDlgProc,
                                          MAKELONG( wEntry, wModem )))
                  SET_PROP( hDlg, ATOM_MODEM, (HANDLE) TRUE ) ;
            }
            break ;

            case IDOK:
            case IDCANCEL:
               SessionDirEntryDlgTerm( hDlg, (IDOK == wParam )) ;
               EndDialog( hDlg, (IDOK == wParam) ) ;
         }
         break ;

      case WM_DESTROY:
         return ( ShadowControl( hDlg, uMsg, wParam, lParam ) ) ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;

         BeginPaint(hDlg, &ps ) ;
         EndPaint( hDlg, &ps ) ;
      }

      // ... Fall through ...

      default:
         return ShadowControl( hDlg, uMsg, wParam, lParam ) ;
   }
   return ( FALSE ) ;

} // end of SessionDirEntryDlgProc()

//************************************************************************
//  BOOL NEAR SessionDirOptionsDlgInit( HWND hDlg, LPARAM lParam )
//
//  Description:
//     Initialization procedure for the Session Directory Options
//     dialog box.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//  History:   Date       Author      Comment
//              2/17/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR SessionDirOptionsDlgInit( HWND hDlg, LPARAM lParam )
{
   int         i ;
   char        szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ] ;
   BOOL NEAR   *npDisplayOptions ;


   GetPhoneINIPath( szINIFile );

   // get pointer to display options

   npDisplayOptions = (BOOL NEAR *) LOWORD( lParam ) ;

   // get state information for display options

   for (i = 0; i < MAXLEN_SESSIONDIRCOLS; i++ )
      CheckDlgButton( hDlg, IDD_OPTION_NAME + i, npDisplayOptions[ i ] ) ;

   // save dialog property

   SET_PROP( hDlg, ATOM_DISPLAYOPTIONS, (HANDLE) npDisplayOptions ) ;

   return ( TRUE ) ;

} // end of SessionDirOptionsDlgInit()

//************************************************************************
//  BOOL NEAR SessionDirOptionsDlgTerm( HWND hDlg, BOOL fExitOK )
//
//  Description:
//     Handles the termination (clean-up) of the Session Directory
//     Options dialog box.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//     BOOL fExitOK
//        flag to designate exiting with "cancel" or "ok".
//
//  History:   Date       Author      Comment
//              2/17/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR SessionDirOptionsDlgTerm( HWND hDlg, BOOL fExitOK )
{
   int        i ;
   BOOL NEAR  *npDisplayOptions ;

   if (fExitOK)
   {
      // get pointer to display option settings
      npDisplayOptions =
         (BOOL NEAR *) GET_PROP( hDlg, ATOM_DISPLAYOPTIONS ) ;

      // get display option settings 
      for (i = 0; i < MAXLEN_SESSIONDIRCOLS; i++)
         npDisplayOptions[ i ] =
            IsDlgButtonChecked( hDlg, IDD_OPTION_NAME + i ) ;

      // NEED: prompt for permanent settings here

      WriteSessionDirDisplayOptions( GETHINST( hDlg ), npDisplayOptions ) ;
   }
   REMOVE_PROP( hDlg, ATOM_DISPLAYOPTIONS ) ;

   return ( TRUE ) ;

} // end of SessionDirOptionsDlgTerm()

//************************************************************************
//  BOOL FAR PASCAL SessionDirOptionsDlgProc( HWND hDlg, UINT uMsg,
//                                            WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This is the session directory entry dialog procedure.
//
//  Parameters:
//     Same as all standard dialog procedures.
//
//  History:   Date       Author      Comment
//              2/ 4/92   BryanW      Added this comment block.
//
//************************************************************************

BOOL FAR PASCAL SessionDirOptionsDlgProc( HWND hDlg, UINT uMsg,
                                     WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_INITDIALOG:
         if (!SessionDirOptionsDlgInit( hDlg, lParam ))
            return ( FALSE ) ;
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;
         return ( ShadowControl( hDlg, uMsg, wParam, RGBLTGRAY ) ) ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK:
            case IDCANCEL:
               SessionDirOptionsDlgTerm( hDlg, (IDOK == wParam )) ;
               EndDialog( hDlg, (IDOK == wParam) ) ;
         }
         break ;

      case WM_DESTROY:
         return ( ShadowControl( hDlg, uMsg, wParam, lParam ) ) ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;

         BeginPaint(hDlg, &ps ) ;
         EndPaint( hDlg, &ps ) ;
      }

      // ... Fall through ...

      default:
         return ShadowControl( hDlg, uMsg, wParam, lParam ) ;
   }
   return ( FALSE ) ;

} // end of SessionDirOptionsDlgProc()

//************************************************************************
//  BOOL NEAR ModemSettingsDlgInit( HWND hDlg, LPARAM lParam )
//
//  Description:
//     Initialization procedure for the Modem Settings
//     dialog box.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//  History:   Date       Author      Comment
//              2/17/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR ModemSettingsDlgInit( HWND hDlg, LPARAM lParam )
{
   int    i ;
   char   szModemConfig[ MAXLEN_MODEMSTR ], szBuf[ MAXLEN_TEMPSTR ],
          szTemp[ MAXLEN_TEMPSTR ], szKey[ MAXLEN_TEMPSTR ],
          szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ],
          szEntryName[ MAXLEN_TEMPSTR ] ;
   LPSTR  lpCur, lpEnd ;
   WORD   wEntry, wModem ;

   wEntry = LOWORD( lParam ) ;
   wModem = HIWORD( lParam ) ;

   // load the configuration string

   LoadString( GETHINST( hDlg ), wModem, szModemConfig,
               sizeof( szModemConfig ) ) ;

   // display the modem name in the title

   lpEnd = _fstrchr( szModemConfig, ',' ) ;
   *lpEnd++ = NULL ;

   GetWindowText( hDlg, szTemp, sizeof( szTemp ) ) ;
   wsprintf( szBuf, szTemp, (LPSTR) szModemConfig ) ;
   SetWindowText( hDlg, szBuf ) ;

   // load settings if current modem is the one
   // selected

   wsprintf( szEntryName, "Entry%04d", wEntry ) ;
   GetPhoneINIPath( szINIFile ) ;

   LoadString( GETHINST( hDlg ), IDS_INI_ENTRYMODEM, szKey,
               sizeof( szKey ) ) ;
   GetPrivateProfileString( szEntryName, szKey, "(none)", szTemp,
                            sizeof( szTemp ), szINIFile ) ;
   if (0 == lstrcmp( szTemp, szModemConfig ))
   {
      for (i = IDS_INI_DIALPREFIX; i <= IDS_INI_RINGING; i++)
      {
         LoadString( GETHINST( hDlg ), i, szKey, sizeof( szKey ) ) ;
         GetPrivateProfileString( szEntryName, szKey, "", szTemp,
                                  sizeof( szTemp ), szINIFile ) ;
         SetDlgItemText( hDlg, i - IDS_INI_DIALPREFIX + IDD_DIALPREFIXEC,
                         szTemp ) ;
      }
   }
   else
   {
      lpCur = lpEnd ;

      for (i = IDD_DIALPREFIXEC; i <= IDD_RINGINGEC; i++)
      {
         if (IDD_RINGINGEC != i)
         {
            lpEnd = _fstrchr( lpCur, ',' ) ;

            // null terminate current string

            if (NULL != lpEnd)
               *lpEnd++ = NULL ;
         }

         DPF3( hWDB, "Modem Setting: %s\r\n", lpCur ) ;

         SetDlgItemText( hDlg, i, lpCur ) ;
         lpCur = lpEnd ;
      }
   }
   SET_PROP( hDlg, ATOM_ENTRY, (HANDLE) wEntry ) ;
   SET_PROP( hDlg, ATOM_MODEM, (HANDLE) wModem ) ;

   return ( TRUE ) ;

} // end of ModemSettingsDlgInit()

//************************************************************************
//  BOOL NEAR ModemSettingsDlgTerm( HWND hDlg, BOOL fExitOK )
//
//  Description:
//     Handles the termination (clean-up) of the Modem Settings
//     dialog box.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//     BOOL fExitOK
//        flag to designate exiting with "cancel" or "ok".
//
//  History:   Date       Author      Comment
//              2/17/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR ModemSettingsDlgTerm( HWND hDlg, BOOL fExitOK )
{
   int    i ;
   char   szModemConfig[ MAXLEN_MODEMSTR ], szTemp[ MAXLEN_TEMPSTR ],
          szKey[ MAXLEN_TEMPSTR ], szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ],
          szEntryName[ MAXLEN_TEMPSTR ] ;
   LPSTR  lpEnd ;
   WORD   wEntry, wModem ;

   if (fExitOK)
   {
      wEntry = (WORD) GET_PROP( hDlg, ATOM_ENTRY ) ;
      wModem = (WORD) GET_PROP( hDlg, ATOM_MODEM ) ;

      // load the configuration string

      LoadString( GETHINST( hDlg ), wModem, szModemConfig,
                  sizeof( szModemConfig ) ) ;

      // get the modem name

      lpEnd = _fstrchr( szModemConfig, ',' ) ;
      *lpEnd++ = NULL ;

      // save settings and selected modem

      wsprintf( szEntryName, "Entry%04d", wEntry ) ;
      GetPhoneINIPath( szINIFile ) ;

      LoadString( GETHINST( hDlg ), IDS_INI_ENTRYMODEM, szKey,
                  sizeof( szKey ) ) ;
      WritePrivateProfileString( szEntryName, szKey, szModemConfig,
                                 szINIFile ) ;
      for (i = IDS_INI_DIALPREFIX; i <= IDS_INI_RINGING; i++)
      {
         GetDlgItemText( hDlg, i - IDS_INI_DIALPREFIX + IDD_DIALPREFIXEC,
                         szTemp, sizeof( szTemp ) ) ;
         LoadString( GETHINST( hDlg ), i, szKey, sizeof( szKey ) ) ;
         WritePrivateProfileString( szEntryName, szKey, szTemp, szINIFile ) ;
      }
   }

   REMOVE_PROP( hDlg, ATOM_ENTRY ) ;
   REMOVE_PROP( hDlg, ATOM_MODEM ) ;

   return ( TRUE ) ;

} // end of ModemSettingsDlgTerm()

//************************************************************************
//  BOOL FAR PASCAL ModemSettingsDlgProc( HWND hDlg, UINT uMsg,
//                                        WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This is the session directory entry dialog procedure.
//
//  Parameters:
//     Same as all standard dialog procedures.
//
//  History:   Date       Author      Comment
//              2/ 4/92   BryanW      Added this comment block.
//
//************************************************************************

BOOL FAR PASCAL ModemSettingsDlgProc( HWND hDlg, UINT uMsg,
                                      WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_INITDIALOG:
         if (!ModemSettingsDlgInit( hDlg, lParam ))
            return ( FALSE ) ;
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;
         return ( ShadowControl( hDlg, uMsg, wParam, RGBLTGRAY ) ) ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK:
            case IDCANCEL:
               ModemSettingsDlgTerm( hDlg, (IDOK == wParam )) ;
               EndDialog( hDlg, (IDOK == wParam) ) ;
         }
         break ;

      case WM_DESTROY:
         return ( ShadowControl( hDlg, uMsg, wParam, lParam ) ) ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;

         BeginPaint(hDlg, &ps ) ;
         EndPaint( hDlg, &ps ) ;
      }

      // ... Fall through ...

      default:
         return ShadowControl( hDlg, uMsg, wParam, lParam ) ;
   }
   return ( FALSE ) ;

} // end of ModemSettingsDlgProc()

//************************************************************************
//  BOOL NEAR DialingDlgInit( HWND hDlg, LPARAM lParam )
//
//  Description:
//     Initialization procedure for the "Dialing..."
//     dialog box.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//  History:   Date       Author      Comment
//              2/17/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR DialingDlgInit( HWND hDlg, LPARAM lParam )
{
   int         i ;
   char        szTemp[ MAXLEN_TEMPSTR ] ;
   BMPBTNINFO  BmpBtnInfo ;
   LPWORD      lpBtnInfo ;

   if (NULL != (lpBtnInfo = GetBtnInfo( GETHINST( hDlg ),
                                        DIALINGBTNINFO )))
   {
      BmpBtnInfo.hInstance = GETHINST( hDlg ) ;
      for (i = 0; i < MAXLEN_DIALINGBTNS; i++)
      {
         GetWindowText( GetDlgItem( hDlg, lpBtnInfo[ i * 3 ] ), szTemp,
                        MAXLEN_TEMPSTR ) ;
         BmpBtnInfo.lpText = (LPSTR) szTemp ;
         BmpBtnInfo.bmpUP = MAKEINTRESOURCE( lpBtnInfo[ i * 3 + 1 ] ) ;
         BmpBtnInfo.bmpDN = MAKEINTRESOURCE( lpBtnInfo[ i * 3 + 2 ] ) ;
         SendDlgItemMessage( hDlg, lpBtnInfo[ i * 3 ], BB_SETBMPBTNINFO,
                             NULL, (LPARAM) (LPBMPBTNINFO) &BmpBtnInfo ) ;
      }
      ReleaseBtnInfo( lpBtnInfo ) ;
   }
   else
      DP1( hWDB, "Unable to load BtnInfo resource!" ) ;

   // save selection information

   SET_PROP( hDlg, ATOM_NUMSEL, (HANDLE) LOWORD( lParam ) ) ;
   SET_PROP( hDlg, ATOM_SELARRAY, (HANDLE) HIWORD( lParam ) ) ;

   // initialize FSM

   DialingDlgFSM( hDlg, DDF_INIT ) ;

   // set a timer for our FSM

   SetTimer( hDlg, TIMERID_DLGFSM, TIMERINTERVAL_DLGFSM, NULL ) ;

   return ( TRUE ) ;

} // end of DialingDlgInit()

//************************************************************************
//  BOOL NEAR DialingDlgTerm( HWND hDlg )
//
//  Description:
//     Handles the termination (clean-up) of the "Dialing..."
//     dialog box.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//  History:   Date       Author      Comment
//              3/ 1/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR DialingDlgTerm( HWND hDlg )
{
   // stop FSM and clear timer

   DialingDlgFSM( hDlg, DDF_CANCEL ) ;
   KillTimer( hDlg, TIMERID_DLGFSM ) ;

   // clean up

   REMOVE_PROP( hDlg, ATOM_NUMSEL ) ;
   REMOVE_PROP( hDlg, ATOM_SELARRAY ) ;

   return ( TRUE ) ;

} // end of DialingDlgTerm()

//------------------------------------------------------------------------
//  VOID NEAR DialingDlgFSM( HWND hDlg, WPARAM wParam )
//
//  Description:
//     A finite state machine which performs the dialing
//     operation.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//     WPARAM wParam
//        forced function such as DDF_RECYCLE, DDF_INIT
//        or DDF_CANCEL or NULL.
//
//  History:   Date       Author      Comment
//              3/ 1/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR DialingDlgFSM( HWND hDlg, WPARAM wParam )
{
   switch (wParam)
   {
      case DDF_INIT:
      {
         char szBuf[ MAXLEN_TEMPSTR ] ;

         // we start calling the first entry

         SET_PROP( hDlg, ATOM_CALLING, (HANDLE) 0 ) ;
         SET_PROP( hDlg, ATOM_PASSNUMBER, (HANDLE) 1 ) ;

         // initial settings for pause time and wait time

         SetDlgItemInt( hDlg, IDD_PAUSEEC, 2, FALSE ) ;
         SetDlgItemInt( hDlg, IDD_WAITEC, 45, FALSE ) ;

         SET_PROP( hDlg, ATOM_PAUSETIME, (HANDLE) 2 ) ;
         SET_PROP( hDlg, ATOM_WAITTIME, (HANDLE) 45 ) ;
         
         // get current time and set dial time

         TimeGetCurTime( szBuf, 0 ) ;
         SetDlgItemText( hDlg, IDD_DIALTIME, szBuf ) ;

         // no last result

         SetDlgItemText( hDlg, IDD_LASTRESULT, "None" ) ;

         // first state is "Next Call"

         SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_NEXTCALL ) ;

      }
      break ;

      case DDF_CLEANUP:
      {
         WORD  wFSMState ;

         // determine state and clean up appropriately

         wFSMState = (WORD) GET_PROP( hDlg, ATOM_FSMSTATE ) ;
         switch (wFSMState)
         {
            case DDS_PAUSE:
               REMOVE_PROP( hDlg, ATOM_TIMEL ) ;
               REMOVE_PROP( hDlg, ATOM_TIMEH ) ;
               break ;

            case DDS_WAITINITRESPONSE:
               REMOVE_PROP( hDlg, ATOM_INITTIME ) ;

               // ... fall through ...

            case DDS_WAITDIALRESPONSE:
            {
               NPSTR npModemResult ;

               npModemResult = (NPSTR) GET_PROP( hDlg, ATOM_MODEMRESULT ) ;
               if (npModemResult)
                  LocalFree( npModemResult ) ;

               REMOVE_PROP( hDlg, ATOM_MODEMRESULT ) ;
               REMOVE_PROP( hDlg, ATOM_RESULTPOS ) ;
               REMOVE_PROP( hDlg, ATOM_TIMEL ) ;
               REMOVE_PROP( hDlg, ATOM_TIMEH ) ;
            }
            break ;
         }

      }
      break ;

      case DDF_CANCEL:
      {
         WORD  wFSMState ;

         // determine state and clean up appropriately

         wFSMState = (WORD) GET_PROP( hDlg, ATOM_FSMSTATE ) ;
         DialingDlgFSM( hDlg, DDF_CLEANUP ) ;

         // disconnect if necessary

         switch (wFSMState)
         {
            case DDS_WAITINITRESPONSE:
            case DDS_WAITDIALRESPONSE:
            case DDS_INITMODEM:
            case DDS_DISCONNECT:
               // force the disconnect

               SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_DISCONNECT ) ;
               DialingDlgFSM( hDlg, NULL ) ;
               break ;
         }
         REMOVE_PROP( hDlg, ATOM_CALLING ) ;
         REMOVE_PROP( hDlg, ATOM_FSMSTATE ) ;
         REMOVE_PROP( hDlg, ATOM_PAUSETIME ) ;
         REMOVE_PROP( hDlg, ATOM_WAITTIME ) ;
         REMOVE_PROP( hDlg, ATOM_PASSNUMBER ) ;
      }
      break ;

      case DDF_RECYCLE:
      {
         WORD  wFSMState ;

         // notify that we are recycling

         SetDlgItemText( hDlg, IDD_LASTRESULT, "Cycling..." ) ;

         // clean-up

         DialingDlgFSM( hDlg, DDF_CLEANUP ) ;

         // determine state and recycle 

         wFSMState = (WORD) GET_PROP( hDlg, ATOM_FSMSTATE ) ;
         switch (wFSMState)
         {
            case DDS_WAITINITRESPONSE:
            case DDS_WAITDIALRESPONSE:
            case DDS_INITMODEM:
               // force the disconnect

               SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_DISCONNECT ) ;
               break ;

            case DDS_PAUSE:
               SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_NEXTCALL ) ;
               break ;
         }
      }
      break ;

      case DDF_DELETE:
      {
         WORD   wFSMState, wCurSel, wNumSel ;
         PWORD  pSelArray ;

         // notify that we are deleting the current entry

         SetDlgItemText( hDlg, IDD_LASTRESULT, "Deleting..." ) ;

         // clean-up

         DialingDlgFSM( hDlg, DDF_CLEANUP ) ;

         // determine state and recycle 

         wFSMState = (WORD) GET_PROP( hDlg, ATOM_FSMSTATE ) ;
         switch (wFSMState)
         {
            case DDS_WAITINITRESPONSE:
            case DDS_WAITDIALRESPONSE:
            case DDS_INITMODEM:
               // force the disconnect

               SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_DISCONNECT ) ;
               break ;

            case DDS_PAUSE:
               SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_NEXTCALL ) ;
               break ;
         }
         pSelArray = (PWORD) GET_PROP( hDlg, ATOM_SELARRAY ) ;
         wCurSel = (WORD) GET_PROP( hDlg, ATOM_CALLING ) ;
         if (wCurSel > 0)
         {
            pSelArray[ wCurSel - 1 ] = NULL ;
            wNumSel = (WORD) GET_PROP( hDlg, ATOM_NUMSEL ) ;
            for (wCurSel = 0; wCurSel < wNumSel; wCurSel++)
            {
               if (NULL != pSelArray[ wCurSel ])
                  break ;
            }
            if (wCurSel >= wNumSel)
               PostMessage( hDlg, WM_COMMAND, (WPARAM) IDCANCEL, NULL ) ;
         }
      }
      break ;

      case DDF_UPDATE:
      {
         char   szEntryName[ MAXLEN_TEMPSTR ], szTemp[ MAXLEN_TEMPSTR ],
                szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ] ;
         WORD   wEntry, wCurSel, wPassNumber, wTemp ;
         PWORD  pSelArray ;

         GetPhoneINIPath( szINIFile ) ;

         pSelArray = (PWORD) GET_PROP( hDlg, ATOM_SELARRAY ) ;
         wCurSel = (WORD) GET_PROP( hDlg, ATOM_CALLING ) ;
         if (wCurSel > 0)
         {
            wEntry = pSelArray[ wCurSel - 1 ] ;
            wsprintf( szEntryName, "Entry%04d", wEntry ) ;

            // read settings from phone file and update dialog box

            GetPrivateProfileString( szEntryName, "Name", "(none)",
                                     szTemp, MAXLEN_TEMPSTR, szINIFile ) ;
            SetDlgItemText( hDlg, IDD_NAME, szTemp ) ;

            GetPrivateProfileString( szEntryName, "Number", "(none)",
                                     szTemp, MAXLEN_TEMPSTR, szINIFile ) ;
            SetDlgItemText( hDlg, IDD_NUMBER, szTemp ) ;

            GetPrivateProfileString( szEntryName, "LastCall", "(none)",
                                     szTemp, MAXLEN_TEMPSTR, szINIFile ) ;
            SetDlgItemText( hDlg, IDD_LASTCONNECT, szTemp ) ;


            wTemp =
               GetPrivateProfileInt( szEntryName, "TotalCalls", 0, szINIFile ) ;
            SetDlgItemInt( hDlg, IDD_TOTALCALLS, wTemp, FALSE ) ;

            // display pass number

            wPassNumber = (WORD) GET_PROP( hDlg, ATOM_PASSNUMBER ) ;
            SetDlgItemInt( hDlg, IDD_PASSNUMBER, wPassNumber, FALSE ) ;
         }
      }
      break ;

      default:
      {
         WORD  wFSMState ;

         wFSMState = (WORD) GET_PROP( hDlg, ATOM_FSMSTATE ) ;
         switch (wFSMState)
         {
            case DDS_PAUSE:
            {
               time_t  lTimeThen, lTimeNow, lTimeDiff ;
               WORD    wPauseTime ;

               if (NULL == GET_PROP( hDlg, ATOM_TIMEL))
               {
                  SetDlgItemText( hDlg, IDD_LASTRESULT, "Pausing..." ) ;
                  time( &lTimeNow ) ;
                  SET_PROP( hDlg, ATOM_TIMEL, (HANDLE) LOWORD( lTimeNow ) ) ;
                  SET_PROP( hDlg, ATOM_TIMEH, (HANDLE) HIWORD( lTimeNow ) ) ;
                  SetDlgItemInt( hDlg, IDD_ELAPSEDTIME, 0, FALSE ) ;
               }
               else
               {
                  lTimeThen =
                     MAKELONG( (WORD) GET_PROP( hDlg, ATOM_TIMEL ),
                               (WORD) GET_PROP( hDlg, ATOM_TIMEH ) ) ;
                  time( &lTimeNow ) ;

                  // NEED: check for wrap around midnight

                  // compute interval, update the dialog box and
                  // check if we're done waiting

                  lTimeDiff = lTimeNow - lTimeThen ;

                  wPauseTime = GetDlgItemInt( hDlg, IDD_ELAPSEDTIME, NULL,
                                              FALSE ) ;
                  if (wPauseTime != LOWORD( lTimeDiff ))
                     SetDlgItemInt( hDlg, IDD_ELAPSEDTIME,
                                    LOWORD( lTimeDiff ), FALSE ) ;

                  wPauseTime = (WORD) GET_PROP( hDlg, ATOM_PAUSETIME ) ;
                  if (wPauseTime <= LOWORD( lTimeDiff ))
                  {
                     // pause completed

                     REMOVE_PROP( hDlg, ATOM_TIMEL ) ;
                     REMOVE_PROP( hDlg, ATOM_TIMEH ) ;
                     SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_NEXTCALL ) ;
                  }
               }
            }
            break ;

            case DDS_NEXTCALL:
            {
               char    szBuf[ MAXLEN_TEMPSTR ] ;
               WORD    wCurSel, wNumSel, wPassNumber ;
               PWORD   pSelArray ;

               if (NULL == GET_PROP( hDlg, ATOM_TIMEL))
               {
                  SetDlgItemText( hDlg, IDD_LASTRESULT, "Next call..." ) ;

                  // get current time and set dial time

                  TimeGetCurTime( szBuf, 0 ) ;
                  SetDlgItemText( hDlg, IDD_CALLTIME, szBuf ) ;

                  // update current dialing info

                  wCurSel = (WORD) GET_PROP( hDlg, ATOM_CALLING ) ;
                  wNumSel = (WORD) GET_PROP( hDlg, ATOM_NUMSEL ) ;
                  wPassNumber = (WORD) GET_PROP( hDlg, ATOM_PASSNUMBER ) ;
                  pSelArray = (PWORD) GET_PROP( hDlg, ATOM_SELARRAY ) ;

                  do
                  {
                     wCurSel++ ;
                     if (wCurSel > wNumSel)
                     {
                        wCurSel = 1 ;

                        // NEED: determine maximum number of retries

                        wPassNumber++ ;
                     }
                  }
                  while ( pSelArray[ wCurSel - 1 ] == NULL ) ;

                  SET_PROP( hDlg, ATOM_CALLING, (HANDLE) wCurSel ) ;
                  SET_PROP( hDlg, ATOM_PASSNUMBER, (HANDLE) wPassNumber ) ;
                  DialingDlgFSM( hDlg, DDF_UPDATE ) ;
               }
               SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_CONNECT ) ;
            }
            break ;

            case DDS_CONNECT:
            {
               HSCB   hSCB ;
               WORD   wCurSel, wEntry ;
               PWORD  pSelArray ;

               pSelArray = (PWORD) GET_PROP( hDlg, ATOM_SELARRAY ) ;
               wCurSel = (WORD) GET_PROP( hDlg, ATOM_CALLING ) ;
               if (wCurSel > 0)
               {
                  wEntry = pSelArray[ wCurSel - 1 ] ;

                  SetDlgItemText( hDlg, IDD_LASTRESULT, "Opening port..." ) ;
                  if (NULL == (hSCB = OpenPort( hDlg, wEntry )))
                  {
                     SetDlgItemText( hDlg, IDD_LASTRESULT,
                                     "Port not available." ) ;
                     SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_PAUSE ) ;
                  }
                  else
                  {
                     SET_PROP( hDlg, ATOM_HSCB, (HANDLE) hSCB ) ;
                     SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_INITMODEM ) ;
                  }
               }
            }
            break ;

            case DDS_INITMODEM:
            {
               char   szModemInit[ MAXLEN_TEMPSTR ] ;
               HSCB   hSCB ;
               WORD   wCurSel, wEntry, wInitTime, wModemStrIndex ;
               NPSTR  npModemResult, npModemInit, npPos ;
               PWORD  pSelArray ;

               wModemStrIndex = (WORD) GET_PROP( hDlg, ATOM_MODEMSTRINDEX ) ;
               pSelArray = (PWORD) GET_PROP( hDlg, ATOM_SELARRAY ) ;
               wCurSel = (WORD) GET_PROP( hDlg, ATOM_CALLING ) ;

               // NEED: check for direct connect

               wEntry = pSelArray[ wCurSel - 1 ] ;
               GetPhoneINIString( GETHINST( hDlg ), wEntry,
                                    IDS_INI_MODEMINIT, szModemInit,
                                    sizeof( szModemInit ) ) ;

               SetDlgItemText( hDlg, IDD_LASTRESULT, "Initalizing..." ) ;

               if (NULL != wModemStrIndex)
               {
                  // determine if we are at the end of the init string
                  if (strlen( szModemInit ) == wModemStrIndex)
                  {
                     // completed - set destination state

                     SetDlgItemText( hDlg, IDD_LASTRESULT, "Init complete." ) ;
                     SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_DIALMODEM ) ;
                     break ;
                  }
               }

               // allocate modem result buffer

               npModemResult =
                  (NPSTR) LocalAlloc( LPTR, MAXLEN_MODEMRESULT ) ;

               DASSERTMSG( hWDB, npModemResult,
                           "npModemResult == NULL" ) ;

               SET_PROP( hDlg, ATOM_MODEMRESULT, (HANDLE) npModemResult ) ;
               SET_PROP( hDlg, ATOM_RESULTPOS, (HANDLE) 0 ) ;

               wInitTime = 1 ;
               npModemInit = &szModemInit[ wModemStrIndex ] ;
               npPos = strchr( npModemInit, MODEM_WAITCHAR ) ;
               if (NULL != npPos)
               {
                  *npPos++ = NULL ;
                  while (*npPos == MODEM_WAITCHAR)
                  {
                     wInitTime++ ;
                     *npPos++ = NULL ;
                  }
                  wModemStrIndex = npPos - szModemInit ;
               }
               else
                  wModemStrIndex = strlen( szModemInit ) ;

               SET_PROP( hDlg, ATOM_MODEMSTRINDEX, (HANDLE) wModemStrIndex ) ;
               SET_PROP( hDlg, ATOM_INITTIME, (HANDLE) wInitTime ) ;

               hSCB = (HSCB) GET_PROP( hDlg, ATOM_HSCB ) ;
               WriteModemString( hSCB, npModemInit ) ;

               SET_PROP( hDlg, ATOM_FSMSTATE,
                           (HANDLE) DDS_WAITINITRESPONSE ) ;
            }
            break;

            case DDS_WAITINITRESPONSE:
            {
               int     nResult ;
               time_t  lTimeThen, lTimeNow, lTimeDiff ;
               NPSTR   npModemResult ;
               PWORD   pSelArray ;
               WORD    wCurSel, wEntry, wInitTime, wBufPos ;

               if (NULL == GET_PROP( hDlg, ATOM_TIMEL))
               {
                  SetDlgItemText( hDlg, IDD_LASTRESULT, "Waiting..." ) ;
                  time( &lTimeNow ) ;
                  SET_PROP( hDlg, ATOM_TIMEL, (HANDLE) LOWORD( lTimeNow ) ) ;
                  SET_PROP( hDlg, ATOM_TIMEH, (HANDLE) HIWORD( lTimeNow ) ) ;
                  SetDlgItemInt( hDlg, IDD_ELAPSEDTIME, 0, FALSE ) ;
               }
               else
               {
                  // get current entry position

                  pSelArray = (PWORD) GET_PROP( hDlg, ATOM_SELARRAY ) ;
                  wCurSel = (WORD) GET_PROP( hDlg, ATOM_CALLING ) ;
                  wEntry = pSelArray[ wCurSel - 1 ] ;

                  lTimeThen =
                     MAKELONG( (WORD) GET_PROP( hDlg, ATOM_TIMEL ),
                               (WORD) GET_PROP( hDlg, ATOM_TIMEH ) ) ;
                  time( &lTimeNow ) ;

                  // NEED: check for wrap around midnight

                  // compute interval, update the dialog box and
                  // check if we're done waiting

                  lTimeDiff = lTimeNow - lTimeThen ;

                  wInitTime = GetDlgItemInt( hDlg, IDD_ELAPSEDTIME, NULL,
                                             FALSE ) ;
                  if (wInitTime != LOWORD( lTimeDiff ))
                     SetDlgItemInt( hDlg, IDD_ELAPSEDTIME,
                                    LOWORD( lTimeDiff ), FALSE ) ;

                  // check for response

                  npModemResult = (NPSTR) GET_PROP( hDlg, ATOM_MODEMRESULT ) ;
                  nResult = CheckModemResult( hDlg, wEntry, npModemResult,
                                              &wBufPos ) ;
                  switch (nResult)
                  {
                     case IDS_INI_COMMANDOK:
                        // clean-up

                        DialingDlgFSM( hDlg, DDF_CLEANUP ) ;

                        SetDlgItemText( hDlg, IDD_LASTRESULT,
                                        "Init OK." ) ;
                        SET_PROP( hDlg, ATOM_FSMSTATE,
                                  (HANDLE) DDS_INITMODEM ) ;
                        break;

                     case IDS_INI_COMMANDERROR:
                        // clean-up

                        DialingDlgFSM( hDlg, DDF_CLEANUP ) ;

                        REMOVE_PROP( hDlg, ATOM_MODEMSTRINDEX ) ;

                        SetDlgItemText( hDlg, IDD_LASTRESULT,
                                        "Modem init error!" ) ;
                        SET_PROP( hDlg, ATOM_FSMSTATE,
                                  (HANDLE) DDS_DISCONNECT ) ;
                        break ;

                     default:
                        if (wBufPos == strlen( npModemResult ))
                        {
                           memset( npModemResult, 0, wBufPos ) ;
                           SET_PROP( hDlg, ATOM_RESULTPOS, 0 ) ;
                        }
                        // check to see if wait completed

                        wInitTime = (WORD) GET_PROP( hDlg, ATOM_INITTIME ) ;
                        if (wInitTime <= LOWORD( lTimeDiff ))
                        {
                           // clean-up

                           DialingDlgFSM( hDlg, DDF_CLEANUP ) ;

                           REMOVE_PROP( hDlg, ATOM_MODEMSTRINDEX ) ;

                           SetDlgItemText( hDlg, IDD_LASTRESULT,
                                          "Modem init error!" ) ;
                           SET_PROP( hDlg, ATOM_FSMSTATE,
                                     (HANDLE) DDS_DISCONNECT ) ;
                        }
                  }
               }
            }
            break ;

            case DDS_DIALMODEM:
            {
               char   szDialString[ MAXLEN_TEMPSTR ],
                      szEntryName[ MAXLEN_TEMPSTR ], szTemp[ MAXLEN_TEMPSTR ],
                      szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ] ;
               HSCB   hSCB ;
               NPSTR  npModemResult ;
               PWORD  pSelArray ;
               WORD   wCurSel, wEntry ;

               pSelArray = (PWORD) GET_PROP( hDlg, ATOM_SELARRAY ) ;
               wCurSel = (WORD) GET_PROP( hDlg, ATOM_CALLING ) ;
               if (wCurSel > 0)
               {
                  SetDlgItemText( hDlg, IDD_LASTRESULT, "Dialing..." ) ;

                  GetPhoneINIPath( szINIFile ) ;

                  wEntry = pSelArray[ wCurSel - 1 ] ;
                  wsprintf( szEntryName, "Entry%04d", wEntry ) ;

                  // allocate modem result buffer

                  npModemResult =
                     (NPSTR) LocalAlloc( LPTR, MAXLEN_MODEMRESULT ) ;

                  DASSERTMSG( hWDB, npModemResult,
                              "npModemResult == NULL" ) ;

                  SET_PROP( hDlg, ATOM_MODEMRESULT, (HANDLE) npModemResult ) ;
                  SET_PROP( hDlg, ATOM_RESULTPOS, (HANDLE) 0 ) ;

                  // get dial command

                  GetPhoneINIString( GETHINST( hDlg ), wEntry,
                                     IDS_INI_DIALPREFIX, szTemp,
                                     sizeof( szTemp ) ) ;
                  lstrcpy( szDialString, szTemp ) ;

                  GetPrivateProfileString( szEntryName, "Number", "", szTemp,
                                           sizeof( szTemp ), szINIFile ) ;
                  lstrcat( szDialString, szTemp ) ;

                  GetPhoneINIString( GETHINST( hDlg ), wEntry,
                                     IDS_INI_DIALSUFFIX, szTemp,
                                     sizeof( szTemp ) ) ;
                  lstrcat( szDialString, szTemp ) ;

                  hSCB = (HSCB) GET_PROP( hDlg, ATOM_HSCB ) ;
                  WriteModemString( hSCB, szDialString ) ;

                  SET_PROP( hDlg, ATOM_FSMSTATE,
                            (HANDLE) DDS_WAITDIALRESPONSE ) ;
               }
               else
               {
                  SetDlgItemText( hDlg, IDD_LASTRESULT, "Fatal error!" ) ;
                  SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_DISCONNECT ) ;
               }
            }
            break ;

            case DDS_WAITDIALRESPONSE:
            {
               int     nResult ;
               time_t  lTimeThen, lTimeNow, lTimeDiff ;
               HSCB    hSCB ;
               NPSCB   npSCB ;
               NPSTR   npModemResult ;
               PWORD   pSelArray ;
               WORD    wCurSel, wEntry, wWaitTime, wBufPos ;

               if (NULL == GET_PROP( hDlg, ATOM_TIMEL))
               {
                  SetDlgItemText( hDlg, IDD_LASTRESULT, "Waiting..." ) ;
                  time( &lTimeNow ) ;
                  SET_PROP( hDlg, ATOM_TIMEL, (HANDLE) LOWORD( lTimeNow ) ) ;
                  SET_PROP( hDlg, ATOM_TIMEH, (HANDLE) HIWORD( lTimeNow ) ) ;
                  SetDlgItemInt( hDlg, IDD_ELAPSEDTIME, 0, FALSE ) ;
               }
               else
               {
                  // get current entry position

                  pSelArray = (PWORD) GET_PROP( hDlg, ATOM_SELARRAY ) ;
                  wCurSel = (WORD) GET_PROP( hDlg, ATOM_CALLING ) ;
                  wEntry = pSelArray[ wCurSel - 1 ] ;

                  lTimeThen =
                     MAKELONG( (WORD) GET_PROP( hDlg, ATOM_TIMEL ),
                               (WORD) GET_PROP( hDlg, ATOM_TIMEH ) ) ;
                  time( &lTimeNow ) ;

                  // NEED: check for wrap around midnight

                  // compute interval, update the dialog box and
                  // check if we're done waiting

                  lTimeDiff = lTimeNow - lTimeThen ;

                  wWaitTime = GetDlgItemInt( hDlg, IDD_ELAPSEDTIME, NULL,
                                             FALSE ) ;
                  if (wWaitTime != LOWORD( lTimeDiff ))
                     SetDlgItemInt( hDlg, IDD_ELAPSEDTIME,
                                    LOWORD( lTimeDiff ), FALSE ) ;

                  // check for response

                  npModemResult = (NPSTR) GET_PROP( hDlg, ATOM_MODEMRESULT ) ;
                  nResult = CheckModemResult( hDlg, wEntry, npModemResult,
                                              &wBufPos ) ;
                  switch (nResult)
                  {
                     case IDS_INI_ARQCONNECT:
                     case IDS_INI_BAUDCONNECT:
                     case IDS_INI_CONNECT:
                     {
                        char  szBuf[ MAXLEN_TEMPSTR ],
                              szEntryName[ MAXLEN_TEMPSTR ],
                              szINIFile[ MAXLEN_PATH + MAXLEN_FILENAME ] ;
                        WORD  wTotalCalls ;

                        // INI filname and section stuff

                        wsprintf( szEntryName, "Entry%04d", wEntry ) ;
                        GetPhoneINIPath( szINIFile ) ;

                        // create emulator window and
                        // change notification window

                        hSCB = (HSCB) GET_PROP( hDlg, ATOM_HSCB ) ;
                        CreateEmulator_New( hDlg, hSCB, wEntry ) ;
                        npSCB = (NPSCB) LocalLock( hSCB ) ;
                        EscapeConnectorFunction( SESSIONCCB,
                                                 ECF_SETNOTIFYWND,
                                                 MAKELONG( GetParent( EMULATORWND ), 0 ),
                                                 NULL ) ;

                        SendMessage( EMULATORWND, WM_CMD_TXBLK,
                                     (WORD) strlen( npModemResult + wBufPos ),
                                     (LONG) (LPSTR) (npModemResult + wBufPos) ) ;

                        // set focus to emulator window

                        SetFocus( EMULATORWND ) ;

                        // update entries in the phone file

                        TimeGetCurDate( szBuf, DATE_SHORTDATE ) ;
                        WritePrivateProfileString( szEntryName, "LastCall",
                                                   szBuf, szINIFile ) ;

                        wTotalCalls =
                           GetDlgItemInt( hDlg, IDD_TOTALCALLS, NULL,
                                          FALSE ) + 1 ;
                        WritePrivateProfileWord( szEntryName, "TotalCalls",
                                                 wTotalCalls, szINIFile ) ;

                        // clean-up

                        DialingDlgFSM( hDlg, DDF_CLEANUP ) ;

                        // display result

                        SetDlgItemText( hDlg, IDD_LASTRESULT,
                                        "Connected." ) ;

                        // alert user
 
                        MessageBeep( 1 ) ;

                        // we're outta here

                        SET_PROP( hDlg, ATOM_FSMSTATE,
                                  (HANDLE) DDS_CONNECTED ) ;
                        PostMessage( hDlg, WM_COMMAND, IDOK, NULL ) ;
                     }
                     break;

                     case IDS_INI_BUSY:
                        // clean-up

                        DialingDlgFSM( hDlg, DDF_CLEANUP ) ;

                        // display result

                        SetDlgItemText( hDlg, IDD_LASTRESULT,
                                        "Busy." ) ;
                        SET_PROP( hDlg, ATOM_FSMSTATE,
                                  (HANDLE) DDS_DISCONNECT ) ;
                        break;

                     case IDS_INI_COMMANDERROR:
                        // clean-up

                        DialingDlgFSM( hDlg, DDF_CLEANUP ) ;

                        // display result

                        SetDlgItemText( hDlg, IDD_LASTRESULT,
                                        "Modem error!" ) ;
                        SET_PROP( hDlg, ATOM_FSMSTATE,
                                  (HANDLE) DDS_DISCONNECT ) ;
                        break ;

                     default:
                        if (wBufPos == strlen( npModemResult ))
                        {
                           memset( npModemResult, 0, wBufPos ) ;
                           SET_PROP( hDlg, ATOM_RESULTPOS, 0 ) ;
                        }

                        // check to see if wait completed

                        wWaitTime = (WORD) GET_PROP( hDlg, ATOM_WAITTIME ) ;
                        if (wWaitTime <= LOWORD( lTimeDiff ))
                        {
                           // clean-up

                           DialingDlgFSM( hDlg, DDF_CLEANUP ) ;

                           // display result

                           SetDlgItemText( hDlg, IDD_LASTRESULT,
                                          "Modem init error!" ) ;
                           SET_PROP( hDlg, ATOM_FSMSTATE,
                                     (HANDLE) DDS_DISCONNECT ) ;
                        }
                  }
               }
            }
            break ;

            case DDS_DISCONNECT:
            {
               HSCB  hSCB ;

               if (NULL != (hSCB = (HSCB) GET_PROP( hDlg, ATOM_HSCB )))
               {
                  SetDlgItemText( hDlg, IDD_LASTRESULT, "Closing port..." ) ;

                  // NEED: determine how we will terminate the dial

                  // WriteModemString( hSCB, "^M" ) ;

                  ClosePort( hSCB ) ;
                  REMOVE_PROP( hDlg, ATOM_HSCB ) ;
               }
               SET_PROP( hDlg, ATOM_FSMSTATE, (HANDLE) DDS_PAUSE ) ;
            }
            break ;
         }
      }
   }

} // end of DialingDlgFSM()

//************************************************************************
//  BOOL FAR PASCAL DialingDlgProc( HWND hDlg, UINT uMsg,
//                                  WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This is the "Dialing..." dialog procedure.
//
//  Parameters:
//     Same as all standard dialog procedures.
//
//  History:   Date       Author      Comment
//              2/ 4/92   BryanW      Added this comment block.
//
//************************************************************************

BOOL FAR PASCAL DialingDlgProc( HWND hDlg, UINT uMsg,
                                WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_INITDIALOG:
         if (!DialingDlgInit( hDlg, lParam ))
            return ( FALSE ) ;
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;
         return ( ShadowControl( hDlg, uMsg, wParam, RGBLTGRAY ) ) ;

      case WM_TIMER:
         DialingDlgFSM( hDlg, NULL ) ;
         break;

      case WM_CONNECTOR_NOTIFY:
      {
         HSCB   hSCB ;
         NPSTR  npModemResult ;
         WORD   wResultPos ;

         hSCB = (HSCB) GET_PROP( hDlg, ATOM_HSCB ) ;
         npModemResult = (NPSTR) GET_PROP( hDlg, ATOM_MODEMRESULT ) ;
         wResultPos = (WORD) GET_PROP( hDlg, ATOM_RESULTPOS ) ;

         if ((NULL != hSCB) && (NULL != npModemResult))
         {
            DPF3( hWDB, "WM_CONNECTOR_NOTIFY received, LOWORD = %04x, HIWORD = %04x\r\n",
                  LOWORD( lParam ), HIWORD( lParam ) ) ;
            switch (LOWORD( lParam ))
            {
               case CN_RECEIVE:
               case CN_EVENT:
               {
                  int    nBlkLen ;
                  BYTE   abTemp[ MAXLEN_INBLOCK + 1 ] ;
                  NPSCB  npSCB ;

                  npSCB = (NPSCB) LocalLock( hSCB ) ;
                  while (0 < (nBlkLen = ReadConnectorBlock( SESSIONCCB,
                                                            MAXLEN_INBLOCK,
                                                            abTemp )))
                  {
                     _fmemcpy( &npModemResult[ wResultPos ], abTemp,
                               nBlkLen ) ;
                     wResultPos += nBlkLen ;
                     npModemResult[ wResultPos ] = NULL ;
                  }
                  LocalUnlock( hSCB ) ;
                  SET_PROP( hDlg, ATOM_RESULTPOS, (HANDLE) wResultPos ) ;
               }
               break ;
            }
         }
      }
      break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDD_WAITEC:
            {
               switch (HIWORD( lParam ))
               {
                  case EN_CHANGE:
                  {
                     WORD  wWaitTime ;

                     // NEED: Save setting in [Options] section

                     wWaitTime = GetDlgItemInt( hDlg, IDD_WAITEC, NULL,
                                                FALSE ) ;
                     SET_PROP( hDlg, ATOM_WAITTIME, (HANDLE) wWaitTime ) ;
                  }
                  break ;
               }
            }
            break ;

            case IDD_PAUSEEC:
            {
               switch (HIWORD( lParam ))
               {
                  case EN_CHANGE:
                  {
                     WORD  wPauseTime ;

                     // NEED: Save setting in [Options] section

                     wPauseTime = GetDlgItemInt( hDlg, IDD_PAUSEEC, NULL,
                                                 FALSE ) ;
                     SET_PROP( hDlg, ATOM_PAUSETIME, (HANDLE) wPauseTime ) ;
                  }
                  break ;
               }
            }
            break ;

            case IDB_RECYCLE:
               DialingDlgFSM( hDlg, DDF_RECYCLE ) ;
               break ;

            case IDB_DELETE:
               DialingDlgFSM( hDlg, DDF_DELETE ) ;
               break ;

            case IDOK:
            case IDCANCEL:
               DialingDlgTerm( hDlg ) ;
               EndDialog( hDlg, (wParam == IDOK) ) ;
         }
         break ;

      case WM_DESTROY:
         return ( ShadowControl( hDlg, uMsg, wParam, lParam ) ) ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;

         BeginPaint(hDlg, &ps ) ;
         EndPaint( hDlg, &ps ) ;
      }

      // ... Fall through ...

      default:
         return ShadowControl( hDlg, uMsg, wParam, lParam ) ;
   }
   return ( FALSE ) ;

} // end of DialingDlgProc()

//***************************************************************************
//  End of File: dialog.c
//***************************************************************************

