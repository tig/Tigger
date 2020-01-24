//***************************************************************************
//
//  Module: dlghelp.c
//
//  Purpose:
//     Dialog box helper functions.
//
//  Description of functions:
//     Descriptions are contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              1/26/92   BryanW      Wrote it.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//***************************************************************************

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"
#include "mflhlib.h"
#include "dialog.h"
#include "session.h"

//************************************************************************
//  VOID GetPhoneINIPath( LPSTR lpINIFile )
//
//  Description:
//     Gets the path to the APACHE.PHN file.
//
//  Parameters:
//     LPSTR lpINIFile
//        buffer to store the created path
//
//  History:   Date       Author      Comment
//              2/18/92   BryanW      Wrote it.
//
//************************************************************************

VOID GetPhoneINIPath( LPSTR lpINIFile )
{
   int   nLen ;
   char  szWinPath[ MAXLEN_PATH ] ;

   // get Windows directory

   GetWindowsDirectory( szWinPath, MAXLEN_PATH ) ;

   // append the backslash if necessary and add file Name

   nLen = lstrlen( szWinPath ) ;
   if (szWinPath[ nLen - 1 ] != '\\')
   {
      szWinPath[ nLen ] = '\\' ;
      szWinPath[ nLen + 1 ] = NULL ;
   }
   wsprintf( lpINIFile, "%s%s", (LPSTR) szWinPath, (LPSTR) PHONE_FILE ) ;

} // end of GetPhoneINIPath()

//************************************************************************
//  BOOL NEAR DlgCompareItem( HWND hDlg, LPCOMPAREITEMSTRUCT lpCIS )
//
//  Description:
//     This handles the compare-item message from the owner-draw
//     controls.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//     LPCOMPAREITEMSTRUCT lpCIS
//        far pointer to COMPAREITEMSTRUCT
//
//  History:   Date       Author      Comment
//              2/18/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR DlgCompareItem( HWND hDlg, LPCOMPAREITEMSTRUCT lpCIS )
{
   BOOL     fResult ;
   LPSTR    lpItem1, lpItem2 ;

   if (lpCIS -> CtlType != ODT_LISTBOX)
      return ( 0 ) ;

   lpItem1 = FarLocalLock( lpCIS -> itemData1 ) ;
   lpItem2 = FarLocalLock( lpCIS -> itemData2 ) ;
   lpItem1 += sizeof( WORD ) * 2 ;
   lpItem2 += sizeof( WORD ) * 2 ;

   fResult = lstrcmp( lpItem1, lpItem2 ) ;

   FarLocalUnlock( lpCIS -> itemData1 ) ;
   FarLocalUnlock( lpCIS -> itemData2 ) ;

   return ( fResult ) ;

} // end of DlgCompareItem()

//************************************************************************
//  BOOL NEAR DlgDrawItem( HWND hDlg, LPDRAWITEMSTRUCT lpDIS )
//
//  Description:
//     This handles the drawing for owner-draw controls.
//
//  Parameters:
//     HWND hDlg
//        handle to parent dialog
//
//     LPDRAWITEMSTRUCT lpDIS
//        pointer to DRAWITEMSTRUCT
//
//  History:   Date       Author      Comment
//
//************************************************************************

BOOL NEAR DlgDrawItem( HWND hDlg, LPDRAWITEMSTRUCT lpDIS )
{
   int     i, nAlignHorz, nColPos, nColumnWidth, nIDSlider,
           nItemWidth, nNumCols ;
   char    szColBuf[ MAXLEN_COLUMNDATA ] ;
   DWORD   rgbBG, rgbFG, dwItemData ;
   HFONT   hFont ;
   HBRUSH  hbrBG ;
   LPSTR   lpItemData ;
   RECT    rcColumn ;
   WORD    wAlignment ;

   if (lpDIS -> CtlType != ODT_LISTBOX)
      return ( FALSE ) ;

   switch (lpDIS -> CtlID)
   {
      case IDD_SESSIONDIRLB:
         nIDSlider = IDD_SESSIONDIRSLIDER ;
         break ;

      default:
         return ( FALSE ) ;
   }

   // get colors from the shadow control

   if (NULL == GetProp( hDlg, "rgbBG_L" ))
   {
      if (lpDIS -> itemState & ODS_SELECTED)
      {
         FillRect( lpDIS -> hDC, &(lpDIS -> rcItem),
                   GetStockObject( BLACK_BRUSH ) ) ;
         SetTextColor( lpDIS -> hDC, RGBWHITE ) ;
         SetBkColor( lpDIS -> hDC, RGBBLACK ) ;
      }
      else
      {
         FillRect( lpDIS -> hDC, &(lpDIS -> rcItem),
                   GetStockObject( WHITE_BRUSH ) ) ;
         SetTextColor( lpDIS -> hDC, RGBBLACK ) ;
         SetTextColor( lpDIS -> hDC, RGBWHITE ) ;
      }
   }
   else
   {
      if (lpDIS -> itemState & ODS_SELECTED)
      {
         rgbFG = MAKELONG( GetProp( hDlg, "rgbBG_L" ),
                           GetProp( hDlg, "rgbBG_H" ) ) ;
         rgbBG = MapTextColor( rgbFG ) ;
         hbrBG = CreateSolidBrush( rgbBG ) ;
         FillRect( lpDIS -> hDC, &(lpDIS -> rcItem), hbrBG ) ;
         SetTextColor( lpDIS -> hDC, rgbFG ) ;
         SetBkColor( lpDIS -> hDC, rgbBG ) ;
         DeleteObject( hbrBG ) ;
      }
      else
      {
         rgbBG = MAKELONG( GetProp( hDlg, "rgbBG_L" ),
                           GetProp( hDlg, "rgbBG_H" ) ) ;
         rgbFG = MapTextColor( rgbBG ) ;
         hbrBG = CreateSolidBrush( rgbBG ) ;
         FillRect( lpDIS -> hDC, &(lpDIS -> rcItem), hbrBG ) ;
         SetTextColor( lpDIS -> hDC, rgbFG ) ;
         SetBkColor( lpDIS -> hDC, rgbBG ) ;
         DeleteObject( hbrBG ) ;
      }
   }
   if (NULL !=
         (hFont = (HFONT) LOWORD( SendMessage( hDlg, WM_GETFONT,
                                               NULL, NULL ) )))
      SelectObject( lpDIS -> hDC, hFont ) ;

   // get pointer to data

   dwItemData = SendMessage( lpDIS -> hwndItem, LB_GETITEMDATA,
                             lpDIS -> itemID, NULL ) ;
   if ((DWORD) LB_ERR != dwItemData && NULL != dwItemData)
   {
      lpItemData = (LPSTR) FarLocalLock( dwItemData ) ;

      DASSERT( hWDB, lpItemData ) ;

      lpItemData += sizeof( WORD ) ;
      nNumCols = LOWORD( SendDlgItemMessage( hDlg, nIDSlider,
                                             SL_GETNUMCOLS, NULL, NULL ) ) ;
      rcColumn.top = lpDIS -> rcItem.top + 1 ;
      rcColumn.bottom = lpDIS -> rcItem.bottom - 1 ;
      nItemWidth = lpDIS -> rcItem.right - lpDIS -> rcItem.left ;
      for (i = 0; i < nNumCols; i++)
      {
         GetColumnData( lpItemData, i + 1, szColBuf, MAXLEN_COLUMNDATA - 1 ) ;
         nColPos = LOWORD( SendDlgItemMessage( hDlg, nIDSlider, SL_GETCOLPOS,
                                               i, NULL ) ) ;
         rcColumn.left = lpDIS -> rcItem.left +
                         LOWORD( (LONG) nItemWidth *
                                 (LONG) nColPos / 100L ) + 1 ;
         if (i == nNumCols - 1)
            rcColumn.right = lpDIS -> rcItem.right - 1 ;
         else
         {
            nColPos = LOWORD( SendDlgItemMessage( hDlg, nIDSlider,
                                                  SL_GETCOLPOS,
                                                  i + 1, NULL ) ) ;
            rcColumn.right = lpDIS -> rcItem.left +
                             LOWORD( (LONG) nItemWidth *
                                     (LONG) nColPos / 100L ) - 1 ;
         }
         nColumnWidth = rcColumn.right - rcColumn.left - 1 ;
         wAlignment = LOWORD( SendDlgItemMessage( hDlg, nIDSlider,
                                                  SL_GETCOLALIGN,
                                                  i, NULL ) ) ;
         switch (wAlignment)
         {
            case SA_RIGHT:
            case SA_CENTER:
               nAlignHorz = nColumnWidth -
                            LOWORD( GetTextExtent( lpDIS -> hDC, szColBuf,
                                                   lstrlen( szColBuf ) ) ) ;
               if (SA_CENTER == wAlignment)
                  nAlignHorz /= (int) 2 ;
               break ;
   
            default:
               nAlignHorz = 0 ;
               break ;
         }
         ExtTextOut( lpDIS -> hDC, rcColumn.left + nAlignHorz + 1,
                     rcColumn.top, ETO_CLIPPED | ETO_OPAQUE, &rcColumn,
                     szColBuf, lstrlen( szColBuf ), NULL ) ;
      }
      FarLocalUnlock( dwItemData ) ;
   }

   // If we are gaining focus, draw the focus rect AFTER drawing
   // the text

   if ((lpDIS -> itemAction & ODA_FOCUS) &&
       0 == (lpDIS -> itemState & ODS_FOCUS))
      DrawFocusRect( lpDIS -> hDC, &(lpDIS -> rcItem) ) ;

   return ( TRUE ) ;

} // end of DlgDrawItem()

//************************************************************************
//  VOID NEAR UpdateSliderPos( HWND hWnd, LPRECT lpRect,
//                             int nOldPos, int nNewPos )
//
//  Description:
//     Updates the "slider" position.  The slider is simply an
//     inverted rectangle.
//
//  Parameters:
//     HWND hWnd
//        screen device context
//
//     LPRECT lpRect
//        far pointer to slider client rect
//
//     int nOldPos
//        old position of slider, -1 if no old position
//
//     int wNewPos
//        new position of slider, -1 if no new position
//
//  History:   Date       Author      Comment
//             12/ 8/91   BryanW      Wrote it.
//              2/29/92   BryanW      Borrowed from SLIDER.C
//
//************************************************************************

VOID NEAR UpdateSliderPos( HWND hWnd, LPRECT lpRect,
                           int nOldPos, int nNewPos )
{
   HDC   hDC ;
   RECT  rcInvert ;
   WORD  wClientWidth ;

   hDC = GetDC( hWnd ) ;
   if (nOldPos != -1)
   {
      wClientWidth = lpRect -> right - lpRect -> left ;
      rcInvert.left = LOWORD( (LONG) nOldPos *
                              (LONG) wClientWidth / 100L ) ;
      rcInvert.right = rcInvert.left + SHADOWWIDTH ;
      rcInvert.top = lpRect -> top ;
      rcInvert.bottom = lpRect -> bottom ;

      InvertRect( hDC, &rcInvert ) ;
   }

   if (nNewPos != -1)
   {
      wClientWidth = lpRect -> right - lpRect -> left ;
      rcInvert.left = LOWORD( (LONG) nNewPos *
                              (LONG) wClientWidth / 100L ) ;
      rcInvert.right = rcInvert.left + SHADOWWIDTH ;
      rcInvert.top = lpRect -> top ;
      rcInvert.bottom = lpRect -> bottom ;

      InvertRect( hDC, &rcInvert ) ;
   }
   ReleaseDC( hWnd, hDC ) ;

} // end of UpdateSliderPos()

//************************************************************************
//  VOID NEAR GetColumnData( LPSTR lpRowData, int nColumn, NPSTR npBuffer,
//                           int nBufferLen )
//
//  Description:
//     Walks through item data a fills the given buffer with the
//     string at nColumn.
//
//  Parameters:
//     LPSTR lpRowData
//        pointer to item (or row) data
//
//     int nColumn
//        column number to retrieve
//
//     NPSTR npBuffer
//        buffer to store data
//
//     int nBufferLen
//        max length of buffer
//
//  History:   Date       Author      Comment
//             10/28/91   BryanW      Wrote it.
//
//************************************************************************

VOID NEAR GetColumnData( LPSTR lpRowData, int nColumn, NPSTR npBuffer,
                         int nBufferLen )
{
   int    i ;
   LPSTR  lpCurPos ;
   WORD   wDataLen ;

   lpCurPos = lpRowData ;
   for (i = 1; i < nColumn; i++)
   {
      wDataLen = *((WORD FAR *) lpCurPos) ;
      lpCurPos += sizeof( WORD ) + wDataLen ;
   }
   wDataLen = *((WORD FAR *) lpCurPos) ;
   lpCurPos += sizeof( WORD ) ;

   _fmemset( npBuffer, NULL, nBufferLen ) ;
   _fstrncpy( npBuffer, lpCurPos, nBufferLen - 1 ) ;
   npBuffer[ nBufferLen ] = NULL ;

} // end of GetColumnData()

//************************************************************************
//  VOID NEAR DlgMeasureItem( HWND hDlg, LPMEASUREITEMSTRUCT lpMIS )
//
//  Description:
//     Computes the item height for the owner-draw items.  Based on
//     the selected font.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//     LPMEASUREITEMSTRUCT lpMIS
//        pointer to MIS, no not Management Information Services,
//        a MEASUREITEMSTRUCT
//
//  History:   Date       Author      Comment
//              1/27/92   BryanW      Wrote it.
//
//************************************************************************

VOID NEAR DlgMeasureItem( HWND hDlg, LPMEASUREITEMSTRUCT lpMIS )
{
   int         cyBorder, nFontHeight ;
   HDC         hDC ;
   HFONT       hFont ;
   TEXTMETRIC  tm ;

   hDC = GetDC( hDlg ) ;
   if (NULL !=
         (hFont = (HFONT) LOWORD( SendMessage( hDlg, WM_GETFONT,
                                               NULL, NULL ) ) ))
      SelectObject( hDC, hFont ) ;

   // get text metrics and compute font height

   cyBorder = GetSystemMetrics( SM_CYBORDER ) ;
   GetTextMetrics( hDC, &tm ) ;
   nFontHeight = tm.tmHeight + tm.tmExternalLeading + (cyBorder << 1) ;
   ReleaseDC( hDlg, hDC ) ;
  
   lpMIS -> itemHeight = nFontHeight ;

} // end of DlgMeasureItem()

//************************************************************************
//  LPWORD NEAR GetBtnInfo( HINSTANCE hInstance, int nResID )
//
//  Description:
//    Finds and loads the binary information from the resource
//    file.
//
//  Parameters:
//     HINSTANCE hInstance
//        handle to instance
//
//     int nResID
//        resource ID
//
//  History:   Date       Author      Comment
//              2/ 5/92   BryanW      Wrote it.
//
//************************************************************************

LPWORD NEAR GetBtnInfo( HINSTANCE hInstance, int nResID )
{
   HRSRC    hresBtnInfo ;
   HGLOBAL  hBtnInfo ;

   hresBtnInfo =
      FindResource( hInstance, MAKEINTRESOURCE( nResID ), RT_RCDATA ) ;
   hBtnInfo = LoadResource( hInstance, hresBtnInfo ) ;
   return( (LPWORD) LockResource( hBtnInfo ) ) ;

} // end of GetBtnInfo()

//************************************************************************
//  BOOL NEAR ReleaseBtnInfo( LPWORD lpBtnInfo )
//
//  Description:
//     Releases the BtnInfo resource.
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//  History:   Date       Author      Comment
//              2/ 5/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR ReleaseBtnInfo( LPWORD lpBtnInfo )
{
   HGLOBAL  hBtnInfo ;

   hBtnInfo = (HGLOBAL) GlobalHandle( (UINT) HIWORD( lpBtnInfo ) ) ;
   UnlockResource( hBtnInfo ) ;
   return( FreeResource( hBtnInfo ) ) ;

} // end of ReleaseBtnInfo()

//------------------------------------------------------------------------
//  BOOL FAR PASCAL LinkDialog( HWND hDlg )
//
//  Description:
//     Links a modeless dialog box to the dialog chain.
//
//  Parameters:
//     HWND hDlg
//        handle to modeless dialog box
//
//  History:   Date       Author      Comment
//              3/ 7/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL FAR PASCAL LinkDialog( HWND hDlg )
{
   int  i ;

   for (i = 0; i < MAXLEN_MODELESSDLGS; i++)
      if (ahModelessDlgs[ i ] == NULL)
      {
         ahModelessDlgs[ i ] = hDlg ;
         return ( TRUE ) ;
      }

   return ( FALSE ) ;

} // end of LinkDialog()

//------------------------------------------------------------------------
//  BOOL FAR PASCAL UnlinkDialog( HWND hDlg )
//
//  Description:
//     Unlinks the given modeless dialog box from the dialog chain.
//
//  Parameters:
//     HWND hDlg
//        handle to modeless dialog box
//
//  History:   Date       Author      Comment
//              3/ 7/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL FAR PASCAL UnlinkDialog( HWND hDlg )
{
   int  i ;

   for (i = 0; i < MAXLEN_MODELESSDLGS; i++)
      if (ahModelessDlgs[ i ] == hDlg)
      {
         ahModelessDlgs[ i ] = NULL ;
         return ( TRUE ) ;
      }

   return ( FALSE ) ;

} // end of UnlinkDialog()

//***************************************************************************
//  End of File: dlghelp.c
//***************************************************************************

