/************************************************************************
 *
 *     Project:  Owner Draw Listbox For Road Runner
 *
 *      Module:  lb.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#define _WINDLL
#include <windows.h>
//#include <wdb.h>

#include "mlhlib.h"

#include "lb.h"

//
// Goofy macros to make saving the handle to the linked list easier
//

#define SAVELISTHANDLE(hLB, hlList) SetProp( hLB, "hlLO", LOWORD( hlList ) ) ;\
                                    SetProp( hLB, "hlHI", HIWORD( hlList ) )
#define GETLISTHANDLE(hLB)  MAKELONG(GetProp(hLB,"hlLO"),GetProp(hLB,"hlHI"))

#define NUKELISTHANDLE(hLB) RemoveProp(hLB,"hlLO") ; RemoveProp(hLB,"hlHI") 

#define GETHMEMDC(hLB)       GetProp( hLB, "hdcMem" )
#define SETHMEMDC(hLB,hdcMem)   SetProp( hLB, "hdcMem", hdcMem )
#define NUKEHMEMDC(hLB)      RemoveProp( hLB, "hdcMem" )

#define GETHINST(hWnd)        GetWindowWord(hWnd,GWW_HINSTANCE)

short NEAR PASCAL DrawAnIcon( HDC hDC, short x, short y, WORD wID, BOOL b ) ;

HDC   NEAR PASCAL LoadBitmaps( void ) ;

LONG NEAR PASCAL lbDrawItem( HWND hWnd, LPDRAWITEMSTRUCT lpDMS ) ;
LONG NEAR PASCAL lbMeasureItem( HWND hWnd, LPMEASUREITEMSTRUCT lpDIS ) ;
// Global vars
extern HANDLE hAppInstance ;

HDC      hdcMem ;
HBITMAP  hbmSave, hbmBitmaps ;
WORD     wBitmapsLoadedCount = 0 ;

// global vars for owner draw shit.
short    dyListBoxText = 16,
         dxListBoxText ;
short    dxIndent  ;
short    dxIconSize ;
short    dxFolder,
         dyFolder,
         dxText,
         dyText,
         dyBorder ;
VOID NEAR PASCAL lbSetTextShit( HWND hWnd ) ;


/****************************************************************
 *  BOOL FAR PASCAL lbReset( HWND hLB )
 *
 *  Description: 
 *
 *    Simply resets the list box.  Not really needed now, but may
 *    be later.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL lbReset( HWND hLB )
{
   HLIST    hlList ;
   HDC  hdcMem ;

   //
   // Delete a list if there is one!
   //
   if (hlList = GETLISTHANDLE( hLB ))
   {
      ListFree( hlList ) ;
      NUKELISTHANDLE( hLB ) ;
   }

   if (hdcMem = GETHMEMDC( hLB ))
   {
      if (!(--wBitmapsLoadedCount)) 
         DeleteDC( hdcMem ) ;
      NUKEHMEMDC( hLB ) ;
   }

   SendMessage( hLB, LB_RESETCONTENT, 0, 0L ) ;

   return TRUE ;

} /* lbReset()  */

/****************************************************************
 *  BOOL FAR PASCAL lbFill( HWND hLB, LPSTR lpData, WORD wFlags )
 *
 *  Description: 
 *
 *    Fills a topic/incident list box with the items pointed to
 *    by lpData.  For test purposes this is a pointer to an
 *    INI file that contains the topics and incidents.
 *
 *    Currently the string that this pointed to is copied and a
 *    pointer to the copy is stored as a window prop.  In the future
 *    this window prop will simply contain a pointer to the data.
 *
 *    wFlags specifies what the LB will be used for and which data
 *    should be made available to the user.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL lbFill( HWND hLB, LPSTR lpData, WORD wFlags )
{
   TOPICLBITEM    tlbi ;
   WORD           wCurIndex ;
   HDC        hdcMem ;

   //
   // Start with loading the bitmap that contains the icons
   // If it's already been loaded don't bother.
   //
   if (!(hdcMem = GETHMEMDC( hLB )))
   {
      if (!wBitmapsLoadedCount)
      {
         if (hdcMem = LoadBitmaps( ))
            SETHMEMDC( hLB, hdcMem ) ;

         lbSetTextShit( hLB ) ;
      }
      else
         SETHMEMDC( hLB, hdcMem ) ;

      wBitmapsLoadedCount++ ;
   }

   SendMessage( hLB, WM_SETREDRAW, FALSE, 0L ) ;

   //
   // Fill the lb with the data...(top level only)
   //
   wCurIndex = 0 ;

   if (lbFindFirstData( lpData, "sections", wFlags, &tlbi ))
   {
      tlbi.wLevel = 0 ;
      lbInsert( hLB, &tlbi, wCurIndex ) ;

      while (lbFindNextData( lpData, "sections", wFlags, &tlbi ))
      {
         tlbi.wLevel = 0 ;
         if (!lbInsert( hLB, &tlbi, ++wCurIndex ))
         {
            break ;
         }
      }
   }
   else
   {
   }

   //
   // turn draw back on...
   //
   SendMessage( hLB, WM_SETREDRAW, TRUE, 0L ) ;
   InvalidateRect( hLB, NULL, TRUE ) ;

   return TRUE ;

} /* lbFill()  */


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
   HLIST          hlList ;
   LPTOPICLBITEM  lpTLBI ;
   LPTOPICLBITEM  lpTLBI2 ;

   //DPF5( hWDB, "lbProcessCmd:" ) ;
   switch(wCtlMsg)
   {
      case LBN_DBLCLK:
         wIndex = (WORD)SendMessage( hLB, LB_GETCURSEL, 0, 0L ) ;
         if (wIndex != LB_ERR)
         {
            if (hlList = GETLISTHANDLE( hLB ))
            {
               lpTLBI = ListGetNode( hlList, wIndex ) ;
               
               //
               // Expand or contract this branch?  We determine this
               // by getting the next item.  If it's level is greater
               // than the selected one it must be a child.  If it does
               // not exist or it's level is less than or equal to then
               // we need to expand.
               //
               if ((lpTLBI2 = ListGetNextNode( hlList, lpTLBI )) &&
                   (lpTLBI->wLevel < lpTLBI2->wLevel))
                  lbCollapse( hLB, wIndex, lpTLBI ) ;
               else
                  lbExpand( hLB, wIndex, lpTLBI ) ;
            }
            else
            {
               //DPS1( hWDB, "   lbProcessCmd: Get List Handle failed!" ) ;
            }
         }
         else
         {
            //DPS1( hWDB, "   lbProcessCmd: LB_GETCURSEL failed!" ) ;
         }
      break ;

      case LBN_SELCHANGE:
      break ;

      default:
         return FALSE ;
   }
   return TRUE ;

} /* lbProcessCmd()  */

/****************************************************************
 *  BOOL FAR PASCAL lbExpand( HWND hLB, WORD wIndex, LPTOPICLBITEM lpTLBI )
 *
 *  Description: 
 *
 *    Expands the specified branch.  If the branch has not children
 *    nothing happens.  wIndex specifies the index of the item in
 *    the list box.  lpTLBI points to the data for that item.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL lbExpand( HWND hLB, WORD wIndex, LPTOPICLBITEM lpTLBI )
{
   TOPICLBITEM    tlbi ;
   WORD           wCurIndex ;

   if (lpTLBI->nType >= 0)
      return FALSE ;

   SendMessage( hLB, WM_SETREDRAW, FALSE, 0L ) ;

   wCurIndex = wIndex + 1 ;

   //DP5( hWDB, "Expanding %s (%d)", (LPSTR)lpTLBI->szDesc, wIndex ) ;

   if (lbFindFirstData( "LB.INI", lpTLBI->szDesc, 0, &tlbi ))
   {
      tlbi.wLevel = lpTLBI->wLevel + 1 ;
      lbInsert( hLB, &tlbi, wCurIndex ) ;

      while (lbFindNextData( "LB.INI", lpTLBI->szDesc, 0, &tlbi ))
      {
         tlbi.wLevel = lpTLBI->wLevel + 1 ;
         if (!lbInsert( hLB, &tlbi, ++wCurIndex ))
         {
            //DP3( hWDB, "lbExpand: lbInsert Failed!" ) ;
            break ;
         }
      }
   }
   else
   {
      //DP4( hWDB, "lbExpand: lbFindFirstData failed!" ) ;
      SendMessage( hLB, WM_SETREDRAW, TRUE, 0L ) ;
      return FALSE ;
   }

   //
   // turn draw back on...
   //
   SendMessage( hLB, WM_SETREDRAW, TRUE, 0L ) ;
   InvalidateRect( hLB, NULL, TRUE ) ;

   return TRUE ;

} /* lbExpand()  */

/****************************************************************
 *  BOOL FAR PASCAL lbCollapse( HWND hLB, WORD wIndex, LPTOPICLBITEM lpTLBI )
 *
 *  Description: 
 *
 *    Collapses the specified branch.  If the branch is not expanded
 *    nothing happens.  wIndex specifies the index of the item in
 *    the list box.  lpTLBI points to the data for that item.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL lbCollapse( HWND hLB, WORD wIndex, LPTOPICLBITEM lpTLBI )
{
   WORD           wCurIndex ;
   LPTOPICLBITEM  lpTLBI2 ;
   HLIST          hlList ;

   SendMessage( hLB, WM_SETREDRAW, FALSE, 0L ) ;

   //DP5( hWDB, "Collapsing %s (%d)", (LPSTR)lpTLBI->szDesc, wIndex ) ;

   wCurIndex = wIndex + 1 ;

   if (hlList = GETLISTHANDLE( hLB ))
   {
      while ((lpTLBI2 = ListGetNextNode( hlList, lpTLBI )) &&
            (lpTLBI2->wLevel > lpTLBI->wLevel))
      {
         ListDeleteNode( hlList, lpTLBI2 ) ;
         SendMessage( hLB, LB_DELETESTRING, wCurIndex, 0L ) ;
      }

      SendMessage( hLB, WM_SETREDRAW, TRUE, 0L ) ;
      InvalidateRect( hLB, NULL, TRUE ) ;
   }
   else
      SendMessage( hLB, WM_SETREDRAW, TRUE, 0L ) ;

   return TRUE ;
} /* lbCollapse()  */

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
      //DP1( hWDB, "lbDrawItem: Could not get list handle!" ) ;
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

   /*
    * Andy Rouse!!!!
    */
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
   /*
    * Andy Rouse!!!!
    */
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
   //DP5( hWDB, "WM_MEASUREITEM" ) ;
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
  HDC			hdc;
  HANDLE		h;
  DWORD FAR		*p;
  LPSTR 		lpBits;
  HANDLE		hRes;
  LPBITMAPINFOHEADER	lpBitmapInfo;
  int numcolors;
  DWORD rgbSelected;
  DWORD rgbUnselected;

  rgbSelected = RGBToBGR(GetSysColor(COLOR_HIGHLIGHT));
  rgbUnselected = RGBToBGR(GetSysColor(COLOR_WINDOW));

  h = FindResource(hAppInstance, "ICONS", RT_BITMAP);
  hRes = LoadResource(hAppInstance, h);

  /* Lock the bitmap and get a pointer to the color table. */
  lpBitmapInfo = (LPBITMAPINFOHEADER)LockResource(hRes);

  if (!lpBitmapInfo)
  	return FALSE;

  p = (DWORD FAR *)((LPSTR)(lpBitmapInfo) + lpBitmapInfo->biSize);

  /* Search for the Solid Blue entry and replace it with the current
   * background RGB.
   */
  numcolors = 16;

  while (numcolors-- > 0) {
      if (*p == BACKGROUND)
	  *p = rgbUnselected;
      else if (*p == BACKGROUNDSEL)
	  *p = rgbSelected;
      else if (*p == BUTTONFACE)
          *p = FlipColor(GetSysColor(COLOR_BTNFACE));
      else if (*p == BUTTONSHADOW)
          *p = FlipColor(GetSysColor(COLOR_BTNSHADOW));
      
      p++;
  }
  UnlockResource(hRes);

  /* Now create the DIB. */
  lpBitmapInfo = (LPBITMAPINFOHEADER)LockResource(hRes);

  /* First skip over the header structure */
  lpBits = (LPSTR)(lpBitmapInfo + 1);

  /* Skip the color table entries, if any */
  lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

  /* Create a color bitmap compatible with the display device */
  hdc = GetDC(NULL);
  if (hdcMem = CreateCompatibleDC(hdc)) {

   if (hbmBitmaps = CreateDIBitmap(hdc, lpBitmapInfo, (DWORD)CBM_INIT, lpBits, (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS))
		hbmSave = SelectObject(hdcMem, hbmBitmaps);

  }


  ReleaseDC(NULL, hdc);

  GlobalUnlock(hRes);
  FreeResource(hRes);


  //
  // now setup global vars
  //


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
         //DP1( hWDB, "lbSetTextShit: WM_GETFONT failed !" ) ;
         hFont = GetStockObject( SYSTEM_FONT ) ;
      }

      SelectObject( hDC, hFont ) ;

      dwText = GetTextExtent(hDC, "M", 1);
      dyText = HIWORD(dwText);
      dxText = LOWORD(dwText);

      ReleaseDC( hLB, hDC ) ;
   }
   else
   {
     // DP1( hWDB, "lbSetTextShit: Could not get DC!" ) ;
   }

   dxFolder = dyFolder = 16 ;

   dyBorder = GetSystemMetrics(SM_CYBORDER) ;

   dxIndent    = 20 ;

   dxIconSize  = 20 ;
   // these are all dependant on the text metrics

   dxListBoxText = dxFolder + (12*dxText);
   //DP1( hWDB, "dyText = %d, dyFolder = %d", dyText, dyFolder ) ;
   dyListBoxText = max(dyText, dyFolder);	//  + dyBorder;
}

/************************************************************************
 * Data dependent functions:
 *
 *  The following functions are dependent on the data used in the
 *  list box.
 *
 ***********************************************************************/

#define CURSIZE      2048

static char    s_lpData[CURSIZE] ;
static LPSTR   s_lpCur ;

/****************************************************************
 *  BOOL NEAR PASCAL
 *    lbFindFirstData( LPSTR lpData, LPSTR lpSubData,
 *                     WORD wFlags, LPTOPICLBITEM lpTLBI )
 *
 *  Description: 
 *
 *    This function finds the first data item in the data set pointed to
 *    by lpData that falls under the topic pointed to by lpSubData.  If
 *    lpSubData is NULL, then the top level data is used.
 *
 *    The list box item pointed to by lpTLBI is filled with the data.
 *
 *    If lpSubData cannot be found in the data FALSE is returned.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL
lbFindFirstData( LPSTR lpData, LPSTR lpSubData,
                 WORD wFlags, LPTOPICLBITEM lpTLBI )
{
   s_lpCur = s_lpData ;

   if (!GetPrivateProfileString( lpSubData, NULL, NULL, s_lpData, CURSIZE,
                                 lpData ))
      return FALSE ;

   return lbFindNextData( lpData, lpSubData, wFlags, lpTLBI ) ;

} /* lbFindFirstData()  */

/****************************************************************
 *  BOOL NEAR PASCAL
 *    lbFindNextData( LPSTR lpData, LPSTR lpSubData,
 *                     WORD wFlags, LPTOPICLBITEM lpTLBI )
 *
 *  Description: 
 *
 *    This function finds the next data item in the data set pointed to
 *    by lpData that falls under the topic pointed to by lpSubData.  If
 *    lpSubData is NULL, then the top level data is used.
 *
 *    The list box item pointed to by lpTLBI is filled with the data.
 *
 *    If lpSubData cannot be found in the data FALSE is returned.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL
lbFindNextData( LPSTR lpData, LPSTR lpSubData,
                 WORD wFlags, LPTOPICLBITEM lpTLBI )
{
   //DPF5( hWDB, "lpFindNextData: " ) ;

   if (*s_lpCur == '\0')
   {
      //DP5( hWDB, "Done!!" ) ;
      return FALSE ;
   }
   else
   {
      char szBuf[MAX_DESCLEN + 1] ;

      if (MAX_DESCLEN == GetPrivateProfileString( lpSubData,
                                                  s_lpCur, NULL,
                                                  szBuf, MAX_DESCLEN,
                                                  lpData ))
      {
         return FALSE ;
      }

      s_lpCur = s_lpCur + lstrlen( s_lpCur ) + 1 ;

      if (szBuf[0] == 'I' && szBuf[1] == ':')
      {
         lpTLBI->nType = (short)(szBuf[2] - '0') ;
         lstrcpy( lpTLBI->szDesc, &szBuf[3] ) ;
      }
      else
      {
         lpTLBI->nType = -1 ; 
         lstrcpy( lpTLBI->szDesc, szBuf ) ;
      }

      //DP5( hWDB, "\"%s\"", (LPSTR)lpTLBI->szDesc ) ;

      return TRUE ;
   }

} /* lbFindNextData()  */

/****************************************************************
 *  BOOL FAR PASCAL lbInsert( HWND hLB, LPTOPICLBITEM lpTLBI,
 *                            WORD wIndex )
 *
 *  Description: 
 *
 *    Inserts an item into the lb.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL
   lbInsert( HWND hLB, LPTOPICLBITEM lpTLBI, WORD wIndex )
{
   WORD           wCount ;
   HLIST          hlList ;
   LPTOPICLBITEM  lpCurNode ;

   //
   // First see if this is the first guy to be inserted.  If so then
   // we need to allocate the memory through the list manager.
   //
   if (!(wCount = (WORD)SendMessage( hLB, LB_GETCOUNT, 0, 0L)))
   {
      if (hlList = ListCreate( sizeof( TOPICLBITEM ) ))
         SAVELISTHANDLE( hLB, hlList ) ;
   }
   else
   {
      hlList = GETLISTHANDLE( hLB ) ;
   }

   if (lpCurNode = ListGetNode( hlList, MAKELONG( wIndex, 0 ) ))
   {
      if (!ListAllocInsertNode( hlList, lpCurNode, lpTLBI ))
      {
         //DP1( hWDB, "ListAllocInsertNode failed!" ) ;
         return FALSE ;
      }
      else
      {
         //DP5( hWDB, "'%s' inserted before '%s'",
         //            (LPSTR)lpTLBI->szDesc, 
         //            (LPSTR)lpCurNode->szDesc) ;
      }
   }
   else
   {
      //DP5( hWDB, "No nodes in list, adding '%s'", (LPSTR)lpTLBI->szDesc ) ;
      if (!ListAllocInsertNode( hlList, NULL, lpTLBI ))
      {
         //DP1( hWDB, "ListAllocInsertNode (with lpAtNode == NULL) failed!" ) ;
         return FALSE ;
      }
   }

   if ((LONG)LB_ERR == SendMessage( hLB, LB_INSERTSTRING, wIndex, (LONG)(LPSTR)lpTLBI->szDesc ))
      return FALSE ;
   else
      return TRUE ;
   
} /* lbInsert()  */

/****************************************************************
 *  BOOL FAR PASCAL lbGet( HWND hLB, LPTOPICLBITEM lpTLBI,
 *                         WORD wIndex )
 *
 *  Description: 
 *
 *    Gets an item from the lb.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL
   lbGet( HWND hLB, LPTOPICLBITEM lpTLBI, WORD wIndex )
{
   char           szBuf [256] ;
   HLIST          hlList ;
   LPTOPICLBITEM  lpnNode ;

   if (hlList = GETLISTHANDLE( hLB ))
   {
      if (lpnNode = ListGetNode( hlList, wIndex ))
      {
         *lpTLBI = *lpnNode ;
         //DP5( hWDB, "Got index %d: '%s'", wIndex, (LPSTR)lpnNode->szDesc ) ;
      }
      else
      {
         //DP5( hWDB, "Could not get index %d", wIndex ) ;
         return FALSE ;
      }
   }
   else
   {
      //DP1( hWDB, "lbGet: Could not get list handle!" ) ;
      return FALSE ;
   }

   return TRUE ;
#if 0
   if ((LONG)LB_ERR == SendMessage( hLB, LB_GETTEXT, wIndex,
                              (LONG)(LPSTR)szBuf ))
   {
      //DP5( hWDB, "SendMessage got '%s'", (LPSTR)szBuf ) ;
      return FALSE ;
   }
   else
   {
      //DP5( hWDB, "SendMessage got '%s'", (LPSTR)szBuf ) ;
      return TRUE ;
   }
#endif   
} /* lbGet()  */


/************************************************************************
 * End of File: lb.c
 ***********************************************************************/

