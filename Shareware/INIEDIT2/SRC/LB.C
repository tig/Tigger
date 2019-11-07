/************************************************************************
 *
 *     Project:  Owner Draw Listbox 
 *
 *      Module:  lb.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "iniedit.h"
#include "lb.h"

#if 0

/****************************************************************
 *  BOOL FAR PASCAL lbProcessCmd( HWND hLB, WORD wCtlMsg )
 *
 *  Description: 
 *
 *    Processes a list box command message.  This function is
 *    responsible for expanding and collapsing branches and so forth.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL lbProcessCmd( HWND hLB, WORD wCtlMsg )
{
   WORD           wIndex ;

   DPF5( hWDB, "lbProcessCmd:" ) ;
   switch(wCtlMsg)
   {
      case LBN_DBLCLK:
         /*
          * On a double click we try to expand.  If it's a topic
          * (either a filename or a section) we can expand.  Other
          * wise we do nothing
          */
         wIndex = (WORD)SendMessage( hLB, LB_GETCURSEL, 0, 0L ) ;
         if (wIndex != LB_ERR)
         {
               //
               // Expand or contract this branch?  We determine this
               // by getting the next item.  If it's level is greater
               // than the selected one it must be a child.  If it does
               // not exist or it's level is less than or equal to then
               // we need to expand.
               //
            #if 0
               if ((lpTLBI2 = ListGetNextNode( hlList, lpTLBI )) &&
                   (lpTLBI->wLevel < lpTLBI2->wLevel))
                  lbCollapse( hLB, wIndex, lpTLBI ) ;
               else
                  lbExpand( hLB, wIndex, lpTLBI ) ;
            #endif
            }
            else
               DPS1( hWDB, "   lbProcessCmd: Get List Handle failed!" ) ;
         }
         else
            DPS1( hWDB, "   lbProcessCmd: LB_GETCURSEL failed!" ) ;
      break ;

      case LBN_SELCHANGE:
         /*
          * What to do here.  Check for help?
          */
      break ;

      default:
         return FALSE ;
   }
   return TRUE ;

} /* lbProcessCmd()  */

/****************************************************************
 *  LONG FAR PASCAL
 *    lbOwnerDraw( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *    HWND hWnd is the handle to the parent that received the
 *    WM_DRAWITEM or other owner draw message.
 * 
 *
 *
 *  Comments:
 *
 ****************************************************************/
LONG FAR PASCAL
   lbOwnerDraw( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam )
{
   switch( wMsg )
   {
      case WM_DRAWITEM:
         return lbDrawItem( hWnd, (LPDRAWITEMSTRUCT)lParam ) ;
      break ;

      case WM_MEASUREITEM:
         return lbMeasureItem( hWnd, (LPMEASUREITEMSTRUCT)lParam ) ;
      break ;

   }
}

LONG NEAR PASCAL lbDrawItem( HWND hWnd, LPDRAWITEMSTRUCT lpDIS )
{
   char                 szMsg[256] ;
   TOPICLBITEM          tlbi ;
   RECT                 rcFocus ;
   BOOL                 bSelected ;
   BOOL                 fOpen ;
   short                y ;
   WORD                 wBitMapID ;
   HDC                  hDC ;
   HBRUSH               hBrush, hOld ;
   HLIST                hlList ;
   LPTOPICLBITEM        lpnNextNode ;

   if (lpDIS->CtlType != ODT_LISTBOX)
      return 0L ;

   if (!lbGet( lpDIS->hwndItem, &tlbi, lpDIS->itemID ))
      return -1 ;

   hDC = lpDIS->hDC ;

   //
   // Get the next node.  We will need it later...
   //
   if (hlList = GETLISTHANDLE( lpDIS->hwndItem ))
   {
      lpnNextNode = ListGetNode( hlList, lpDIS->itemID + 1 ) ;
   }
   else
   {
      DP1( hWDB, "lbDrawItem: Could not get list handle!" ) ;
   }

   if ((lpDIS->itemState) & (ODS_SELECTED))
   {
      SetTextColor( hDC, GetSysColor( COLOR_HIGHLIGHTTEXT ) ) ;
      SetBkColor( hDC, GetSysColor( COLOR_HIGHLIGHT ) ) ;
      bSelected = TRUE ;
   }
   else
   {
      SetTextColor( hDC, GetSysColor( COLOR_WINDOWTEXT ) ) ;
      SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;
      bSelected = FALSE ;
   }

   //
   // if we are loosing focus, remove the focus rect before
   // drawing.
   //
   rcFocus = lpDIS->rcItem ;

   rcFocus.left += tlbi.wLevel * dxIndent ;

   if ((lpDIS->itemAction) & (ODA_FOCUS))
      if (!((lpDIS->itemState) & (ODS_FOCUS)))
         DrawFocusRect( hDC, &rcFocus ) ;

   wsprintf( szMsg, "%s", (LPSTR)tlbi.szDesc ) ;

   y = lpDIS->rcItem.top +
         ((lpDIS->rcItem.bottom - lpDIS->rcItem.top)/2);

   ExtTextOut( hDC,
               rcFocus.left + 4 + dxIconSize,
               y - ( dyText / 2 ),
               ETO_OPAQUE,
               &rcFocus,
               szMsg,
               lstrlen(szMsg),
               NULL
               ) ;

   #ifdef DRAWSTUPIDLINES
   //
   // Now draw tree lines
   //
   if (hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOWTEXT)))
   {
	   hOld = SelectObject(hDC, hBrush);

      if (tlbi.wLevel > 0)
      {
         short x, y, dy ;

         x = (tlbi.wLevel * dxText * 2) - dxText + dyBorder * 2 ;
         dy = lpDIS->rcItem.bottom - lpDIS->rcItem.top ;
         y = lpDIS->rcItem.top + (dy/2) ;

	      /* Draw the horizontal line over to the (possible) folder. */
	      PatBlt (hDC, x, y, dyText, dyBorder, PATCOPY ) ;

	      /* Draw the top part of the vertical line. */
	      PatBlt( hDC, x, lpDIS->rcItem.top, dyBorder, dy/2, PATCOPY ) ;

	      /* If not the end of a node, draw the bottom part... */
	      if (!lpnNextNode)
   	      PatBlt( hDC, x, y+dyBorder, dyBorder, dy/2, PATCOPY ) ;

         #if 0
	      /* Draw the verticals on the left connecting other nodes. */
	      lpnTemp = GetPrevNode( pNode->pParent ;

	      while (pNTemp)
         {
	         nLevel-- ;
	         if (!(pNTemp->wFlags & TF_LASTLEVELENTRY))
	            PatBlt( hDC, (nLevel * dxText * 2) - dxText + dyBorderx2,
                     lpDIS->rcItem.top, dyBorder,dy, PATCOPY ) ;

	         pNTemp = pNTemp->pParent;
	      }
         #endif
      }
      SelectObject( hDC, hOld ) ;
   }
   #endif

   if (tlbi.nType < 0)
   {
      if (lpnNextNode)
         fOpen = (lpnNextNode->wLevel > tlbi.wLevel ) ;
      else
         fOpen = FALSE ;

      if (fOpen) // (lpDIS->itemState) & (ODS_SELECTED))
      {
         wBitMapID = BM_TOPIC_OPEN ;
      }
      else
      {
         wBitMapID = BM_TOPIC_CLOSE ;
      }

   }
   else
   {
      switch(tlbi.nType)
      {

         case INC_SPECIAL:
            wBitMapID = BM_INCIDENT_SPECIAL ;
         break ;

         case INC_IMPORTANT:
            wBitMapID = BM_INCIDENT_BANG ;
         break ;

         case INC_QUESTION:
            wBitMapID = BM_INCIDENT_QUESTION ;
         break ;

         case INC_BORING:
            wBitMapID = BM_INCIDENT_BLANK ;
         break ;

         case INC_NORMAL:
         default:
            wBitMapID = BM_INCIDENT_TEXT ;
         break ;
      }
   }

   DrawAnIcon( hDC,
               lpDIS->rcItem.left + (tlbi.wLevel * dxIndent) + 2,
               lpDIS->rcItem.top,
               wBitMapID,
               bSelected ) ;

   //
   // if we are gaining focus draw the focus rect after drawing
   // the text.
   if ((lpDIS->itemAction) & (ODA_FOCUS))
      if ((lpDIS->itemState) & (ODS_FOCUS))
         DrawFocusRect( hDC, &rcFocus ) ;


   if ((lpDIS->itemState) & (ODS_SELECTED))
   {
      SetTextColor( hDC, GetSysColor( COLOR_WINDOWTEXT ) ) ;
      SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;
   }

   return 0L ;

} /* lbOwnerDraw()  */

LONG NEAR PASCAL lbMeasureItem( HWND hWnd, LPMEASUREITEMSTRUCT lpLBMItem )
{
   DP5( hWDB, "WM_MEASUREITEM" ) ;
	lpLBMItem->itemHeight = dyListBoxText ;

   return 0L ;
}   

/****************************************************************
 *  short NEAR PASCAL DrawAnIcon( HDC hDC, short x, short y, WORD wID )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
short NEAR PASCAL DrawAnIcon( HDC hDC, short x, short y, WORD wID,
                              BOOL bSelected )
{
   BitBlt( hDC,
           x + dyBorder,
           y, //-(dyFolder/2),
           dxFolder,
           dyFolder, 
           hdcMem,
           wID * dxFolder,
           bSelected ? dyFolder : 0, SRCCOPY ) ;

   return TRUE ;

} /* DrawAnIcon()  */

#define BACKGROUND	0x000000FF	// bright blue
#define BACKGROUNDSEL	0x00FF00FF	// bright blue
#define BUTTONFACE	0x00C0C0C0	// bright grey
#define BUTTONSHADOW	0x00808080	// dark grey

/****************************************************************
 *  DWORD NEAR PASCAL RGBToBGR(DWORD rgb)
 *
 *  Description: 
 *
 *
 ****************************************************************/
DWORD NEAR PASCAL RGBToBGR(DWORD rgb)
{
  BYTE	    green;
  BYTE	    blue;

  green = (BYTE)((WORD)rgb >> 8);
  blue	= (BYTE)(HIWORD(rgb));
  return(RGB(blue, green, (BYTE)rgb));
} /* RGBToBGR()  */

DWORD NEAR PASCAL FlipColor(DWORD rgb)
{
	return RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb));
}

/****************************************************************
 *  HDC NEAR PASCAL LoadBitmaps( void )
 *
 *  Description:
 *
 *    We load the bitmaps here.  Also all "measure" variables
 8    are set.
 *
 *
 *  Comments:
 *
 ****************************************************************/
HDC NEAR PASCAL LoadBitmaps()
{
   HDC			      hdc ;
   HANDLE		      h ;
   DWORD FAR		   *p ;
   LPSTR 		      lpBits ;
   HANDLE		      hRes ;
   LPBITMAPINFOHEADER   lpBitmapInfo ;
   int               numcolors ;
   DWORD             rgbSelected ; 
   DWORD             rgbUnselected ;

   rgbSelected = RGBToBGR( GetSysColor( COLOR_HIGHLIGHT ) ) ;
   rgbUnselected = RGBToBGR( GetSysColor( COLOR_WINDOW ) ) ;

   h = FindResource( hAppInstance, "ICONS", RT_BITMAP ) ;
   hRes = LoadResource( hAppInstance, h ) ;

   /* Lock the bitmap and get a pointer to the color table. */
   lpBitmapInfo = (LPBITMAPINFOHEADER)LockResource( hRes ) ;

   if (!lpBitmapInfo)
      return FALSE;

   p = (DWORD FAR *)((LPSTR)(lpBitmapInfo) + lpBitmapInfo->biSize);

   /* Search for the Solid Blue entry and replace it with the current
    * background RGB.
    */
   numcolors = 16;

   while (numcolors-- > 0)
   {
      if (*p == BACKGROUND)
         *p = rgbUnselected ;
      else if (*p == BACKGROUNDSEL)
         *p = rgbSelected ;
      else if (*p == BUTTONFACE)
         *p = FlipColor( GetSysColor( COLOR_BTNFACE ) ) ;
      else if (*p == BUTTONSHADOW)
         *p = FlipColor( GetSysColor( COLOR_BTNSHADOW ) ) ;

      p++;
   }

   UnlockResource( hRes ) ;

   /* Now create the DIB. */
   lpBitmapInfo = (LPBITMAPINFOHEADER)LockResource(hRes);

   /* First skip over the header structure */
   lpBits = (LPSTR)(lpBitmapInfo + 1);

   /* Skip the color table entries, if any */
   lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

   /* Create a color bitmap compatible with the display device */
   hdc = GetDC(NULL);
   if (hdcMem = CreateCompatibleDC(hdc))
   {
      if (hbmBitmaps = CreateDIBitmap( hdc, lpBitmapInfo,
                                       (DWORD)CBM_INIT, lpBits,
                                       (LPBITMAPINFO)lpBitmapInfo,
                                       DIB_RGB_COLORS ))
      hbmSave = SelectObject( hdcMem, hbmBitmaps ) ; 

   }

   ReleaseDC(NULL, hdc);

   GlobalUnlock(hRes);
   FreeResource(hRes);

   /*
    * Upon return hdcMem has our bitmap selected into it
    */
   return hdcMem ;

} /* LoadBitmaps()  */


VOID NEAR PASCAL lbSetTextShit( HWND hLB )
{
   DWORD  dwText ;
   HDC    hDC ;
   HFONT hFont ;

   if (hDC = GetDC( hLB ))
   {
      if (!(hFont = (HFONT)SendMessage( hLB, WM_GETFONT, 0, 0L )))
      {
         DP1( hWDB, "lbSetTextShit: WM_GETFONT failed !" ) ;
         hFont = GetStockObject( SYSTEM_FONT ) ;
      }

      SelectObject( hDC, hFont ) ;

      dwText = GetTextExtent(hDC, "M", 1);
      dyText = HIWORD(dwText);
      dxText = LOWORD(dwText);

      ReleaseDC( hLB, hDC ) ;
   }
   else
      DP1( hWDB, "lbSetTextShit: Could not get DC!" ) ;

   dxFolder = dyFolder = 16 ;

   dyBorder = GetSystemMetrics(SM_CYBORDER) ;

   dxIndent    = 20 ;

   dxIconSize  = 20 ;
   // these are all dependant on the text metrics

   dxListBoxText = dxFolder + (12*dxText);
   DP1( hWDB, "dyText = %d, dyFolder = %d", dyText, dyFolder ) ;
   dyListBoxText = max(dyText, dyFolder);	//  + dyBorder;
}

#endif 

/************************************************************************
 * End of File: lb.c
 ***********************************************************************/

