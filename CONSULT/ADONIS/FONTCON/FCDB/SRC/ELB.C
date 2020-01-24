//*************************************************************
//  File name: elb.c
//
//  Description: 
//      Code for the Extended ListBox
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"

#include "intFCDB.h"

#define IDCHILDLB       0x424c
#define ELBEXTRA        2

#define INDENT_FACTOR(BMPW)   (BMPW)

LONG    WINAPI      ELBWndProc(HWND,UINT,WPARAM,LPARAM);
LONG    WINAPI      LBSubClassProc(HWND,UINT,WPARAM,LPARAM);
LONG    NEAR PASCAL ELBDrawItem(PES, LPDRAWITEMSTRUCT);
VOID    NEAR PASCAL ELBSetItemHeight(PES);

WNDPROC gwpOldLB;

//*************************************************************
//
//  ELBWndProc
//
//  Purpose:
//      Window procedure for the Extended ListBox class
//
//
//  Parameters:
//      HWND hWnd
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (LONG FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LONG FAR PASCAL ELBWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PES pes = (PES)GetWindowWord(hWnd, WE_PES);

    switch (msg)
    {
        case WM_CREATE:
        {
            HINSTANCE hInst = (HINSTANCE)GetWindowWord( hWnd, GWW_HINSTANCE );
            LONG l = GetWindowLong( hWnd, GWL_STYLE );

            //*** Strip border, we handle it
            l &= ~WS_BORDER;
            SetWindowLong( hWnd, GWL_STYLE, l );


            pes = (PES)LocalAlloc( LPTR, sizeof( ELBSTUFF ) );

            if (!pes)
            {
                return -1L;
            }

            pes->hELB = hWnd;
            pes->hFont = GetStockObject( SYSTEM_FONT );

            SetWindowWord( hWnd, WE_PES, (WPARAM)pes );

            pes->hLB = CreateWindow( "LISTBOX", NULL, WS_VSCROLL|WS_CHILD|WS_BORDER|
                WS_VISIBLE|LBS_NOINTEGRALHEIGHT|LBS_NOTIFY|LBS_DISABLENOSCROLL|
                LBS_OWNERDRAWFIXED,0,0,0,0,hWnd,(HMENU)IDCHILDLB,hInst,NULL);

            if (!pes->hLB)
            {
                LocalFree( (HLOCAL)pes );
                return -1L;
            }

            pes->lpfnSort = DBELBSortProc;
            pes->lpfnPaint = DBELBPaintProc;

            // this way we know it's 'new'
            pes->disPrev.hwndItem = NULL ;

            ELBSetBitmap(hWnd, NULL, NUM_FOLDERS );

            //*** Subclass the listbox
            gwpOldLB = (WNDPROC)SetWindowLong( pes->hLB, GWL_WNDPROC,
                                    (LONG)LBSubClassProc );
            return 1L;
        }
        break;

        case WM_COMMAND:
        {
            if (wParam==IDCHILDLB)
            {
                switch (HIWORD(lParam))
                {
                    case LBN_KILLFOCUS:
                    case LBN_SETFOCUS:
                    case LBN_DBLCLK:
                    case LBN_SELCHANGE:
                    case ELBN_ENTER:
                    case ELBN_RDBLCLK:
                    case ELBN_SPACE:
                    {
                        HWND hParent = GetParent( hWnd );

                        // Pass notification up to parent
                        if (hParent)
                            SendMessage(hParent,msg, GetWindowWord(hWnd,GWW_ID),
                                (lParam&0xFFFF0000)|(LONG)(WORD)hWnd );
                    }
                    break;
                }
            }
        }
        break;

        case WM_SIZE:
        {
            if (pes->hLB)
                MoveWindow(pes->hLB,0,0,LOWORD(lParam),HIWORD(lParam),TRUE);
        }
        break;

        case WM_MEASUREITEM:
        {
            LPMEASUREITEMSTRUCT lpmi = (LPMEASUREITEMSTRUCT)lParam;

            if (lpmi->CtlID==IDCHILDLB)
            {
                TEXTMETRIC tm;
                HDC     hDC = GetDC( NULL );

                pes->hFont = SelectObject( hDC, pes->hFont );
                GetTextMetrics( hDC, &tm );
                pes->hFont = SelectObject( hDC, pes->hFont );

                pes->wFontH = lpmi->itemHeight = abs(tm.tmHeight);
                ReleaseDC( NULL, hDC );
                return 1L;
            }
        }
        break;

        case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT lpdi = (LPDRAWITEMSTRUCT)lParam;

            if (lpdi->CtlID==IDCHILDLB)
                return ELBDrawItem( pes, lpdi );
        }
        break;

        case WM_SYSCOLORCHANGE:
         ELBSetBitmap( hWnd, NULL, NUM_FOLDERS ) ;
        break ;

        case WM_SETFOCUS:
            if (IsWindow( pes->hLB ))
               SetFocus( pes->hLB ) ;
        break ;

        case WM_DESTROY:
            if (pes->hBitmap)
            {
               DeleteObject( pes->hBitmap ) ;
            }

            LocalFree( (HLOCAL)pes );
        break;
    }
    return DefWindowProc( hWnd, msg, wParam, lParam );

} //*** ELBWndProc


//*************************************************************
//
//  ELBDrawItem
//
//  Purpose:
//      Draws an ELB item
//
//
//  Parameters:
//      HWND hWnd
//      LPDRAWITEMSTRUCT lpDIS
//      
//
//  Return: (LONG)
//
//
//  Comments:
//      This routine draws the folders and then calls the paint routine
//      to paint in the given text.
//
//
//
//  History:    Date       Author     Comment
//               1/29/92   MSM        Created
//
//*************************************************************

LONG NEAR PASCAL ELBDrawItem(PES pes, LPDRAWITEMSTRUCT lpDIS)
{
   LPELBNODE lpen = ELBGetItem(pes->hELB,lpDIS->itemID);

   RECT      rcItem ;      /* Same as lpIDS->rcItem */
   RECT      rcFocus ;     /* where we put the focus rect */
   RECT      rc ;
   UINT      wTab ;
   UINT      wMX, wMY, wSX, wSY;
   COLORREF  rgbText ;
   COLORREF  rgbBk ;

   if (!lpen)
      return 0L ;

   /* Setup our colors based on selected or
    * non-selected
    */
   if (lpDIS->itemState & ODS_SELECTED)
   {
      rgbText =   GetSysColor( COLOR_HIGHLIGHTTEXT ) ;
      rgbBk   = GetSysColor( COLOR_HIGHLIGHT ) ;
   }
   else
   {
      rgbText =   GetSysColor( COLOR_WINDOWTEXT ) ;
      rgbBk   = GetSysColor( COLOR_WINDOW ) ;
   }

   /*
    * Very first thing we need to do is figure out the 'focus rect'.
    * The focus rect (rcFocus) will be drawn around the icon
    * and text only.
    */
   rcItem = lpDIS->rcItem ;
   wTab = VI_GETTABLEVEL( lpen );

   /* Get offset of icon
    */
   wSX = pes->wBmpW + 2 + (wTab * INDENT_FACTOR( pes->wBmpW )) + 1 ;
   wSY = rcItem.top + (rcItem.bottom-rcItem.top - pes->wBmpH)/2;

   /* Indent the focus rect the appropriate amount
    */
   rcFocus.left = wSX - 2 ;
   rcFocus.top = rcItem.top ;
   rcFocus.right = rcItem.right ;
   rcFocus.bottom = rcItem.bottom ;

   if ( (lpDIS->itemAction & ODA_FOCUS) &&
         !(lpDIS->itemState & ODS_FOCUS))
      DrawFocusRect( lpDIS->hDC, &rcFocus ) ;

   if ( (lpDIS->itemID != pes->disPrev.itemID) ||
         (lpDIS->itemAction & ODA_DRAWENTIRE) ||
         (lpDIS->itemState & ODS_SELECTED) !=
         (pes->disPrev.itemState & ODS_SELECTED) &&
        pes->hBitmap)
   {
      HDC     hCompDC;

      SetBkMode( lpDIS->hDC, TRANSPARENT );
      SetTextColor( lpDIS->hDC, rgbText ) ;

      SetBkColor( lpDIS->hDC, GetSysColor( COLOR_WINDOW ) ) ;

      if (hCompDC = CreateCompatibleDC( lpDIS->hDC ))
      {
         pes->hBitmap = SelectObject( hCompDC, pes->hBitmap );

         /*
          * On the far left are the check boxes.  Nuke out
          * (i.e. COLOR_WINDOW) the area where they belong.
          */
         rc.left = 0 ;
         rc.top = rcItem.top ;
         rc.right = pes->wBmpW+1 ;
         rc.bottom = rcItem.bottom ;

         ExtTextOut( lpDIS->hDC,0,0,ETO_OPAQUE, &rc,NULL,0,NULL);



         //*** Draw the appropriate check box (empty, Checked
         //*** 'P'urchased or 'D'ownload
         if (!IsAncestorChecked( (LPDBNODE)lpen ))
         {
            if (!VI_ISCHECKABLE( lpen ))
            {
                wMX = NO_ICON * pes->wBmpW;
                wMY = 2*pes->wBmpH;
            }
            else
            {
                wMX = pes->wBmpW * ( (VI_ISSHIPABLE(lpen))?SHIP_ICON:DOWNLOAD_ICON);
                wMY = VI_ISCHECKED(lpen)?0:pes->wBmpH;
            }
         }
         else
         {
            wMX = GRAYCHECKBOX_ICON * pes->wBmpW;
            wMY = 2*pes->wBmpH;
         }

        BitBlt( lpDIS->hDC, 1, wSY, pes->wBmpW, pes->wBmpH,
            hCompDC, wMX, wMY, SRCCOPY );

         
         /*
          * If it's not the first level icon, then there needs to
          * be some 'clear' area between the checkbox column and
          * the icon.  Draw that (COLOR_WINDOW).
          */
         rc.left = rc.right ;
         rc.right = wSX ;
         ExtTextOut( lpDIS->hDC,0,0,ETO_OPAQUE, &rc,NULL,0,NULL);

         /*
          * Then draw rgbBk over where the icon is gonna go.  We
          * do this 'cause the item height might be taller than
          * the icon.
          */
         SetBkColor( lpDIS->hDC, rgbBk ) ;

         rc.left = wSX - 1 ;
         rc.right = rc.left + pes->wBmpW*4 / 3 ;
         ExtTextOut( lpDIS->hDC,0,0,ETO_OPAQUE, &rc,NULL,0,NULL);


         /*
          * Draw the icon
          */
         wMX = (WORD)lpen->wNodeType * pes->wBmpW;
         wMY = (VI_GETOPENSTATUS(lpen))?pes->wBmpH:0;

         if (lpDIS->itemState & ODS_SELECTED)
            wMY += (2*pes->wBmpH) ;
      
         BitBlt( lpDIS->hDC, wSX, wSY, pes->wBmpW, pes->wBmpH,
            hCompDC, wMX, wMY, SRCCOPY );

         //*** Clean up
         pes->hBitmap = SelectObject( hCompDC, pes->hBitmap );
         DeleteDC( hCompDC );
      }
      else
      {
         return 0L ;
      }

      rc.left = rc.right ;
      rc.right = rcItem.right ;

      if (pes->lpfnPaint)
      {
         pes->hFont = SelectObject( lpDIS->hDC, pes->hFont );

         pes->lpfnPaint( (LPES)pes, lpDIS->hDC, lpen, (LPRECT)&rc );

         pes->hFont = SelectObject( lpDIS->hDC, pes->hFont );
      }
   }

  if ((lpDIS->itemAction & ODA_FOCUS) &&
      (lpDIS->itemState & ODS_FOCUS ))
        DrawFocusRect( lpDIS->hDC, &rcFocus );

  pes->disPrev = *lpDIS ;

  return 1L;

} //*** ELBDrawItem


//*************************************************************
//
//  ELBSetItemHeight
//
//  Purpose:
//      Sets the height of the items in the listbox
//
//
//  Parameters:
//      PES pes
//      
//
//  Return: (UINT)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

VOID NEAR PASCAL ELBSetItemHeight(PES pes)
{
   if (IsWindow( pes->hLB ))
       SendMessage( pes->hLB, LB_SETITEMHEIGHT,0,(LONG)max(pes->wBmpH,pes->wFontH) );

} //*** ELBSetItemHeight


//*************************************************************
//
//  ELBAddInto
//
//  Purpose:
//      Adds a node into the sub list of the node given.  Tabbing
//      level will automatically be set.  Uses the sorting proc.
//
//  Parameters:
//      HWND hELB
//      UINT uIndex     - node to become parent (-1 for root)
//      LPELBNODE lpen  - child node
//      
//
//  Return: (LONG WINAPI)
//      Index added at (from beginning)
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LONG WINAPI ELBAddInto( HWND hELB, UINT uIndex, LPELBNODE lpNode )
{
    PES       pes = (PES)GetWindowWord( hELB, WE_PES );
    int       nTab;
    LPELBNODE lpen;
    int       nHi, nLow;
    LPDB      lpDB = ELBGetDatabase( hELB );

    if (!pes)
        return -1L;

    if (uIndex == -1 )
        nTab = -1;
    else
    {
        lpen = (LPELBNODE)SendMessage(pes->hLB, LB_GETITEMDATA,uIndex,0L);
        if ((LONG)lpen<=0L)
            return -1L;

        nTab = (int)VI_GETTABLEVEL(lpen);
    }
    uIndex++;
    nTab++;
    VI_SETTABLEVEL( lpNode, nTab );

    //*** No sort proc, enter here
    if (!pes->lpfnSort)
        return SendMessage(pes->hLB,LB_INSERTSTRING,uIndex,(LONG)lpNode);

    if (lpDB->uView==DB_VIEW_STYLES) // No grouping, make this a quick insert
    {
        nLow = uIndex;
        nHi  = ListBox_GetCount( pes->hLB ) - 1;

do_quick_insert:
        if (nHi==(UINT)-1)
            return SendMessage(pes->hLB,LB_INSERTSTRING,0xFFFF,(LONG)lpNode);
                
        while (nHi>nLow)
        {
            uIndex = (nHi + nLow)/2;

            if ((lpen = (LPELBNODE)SendMessage(pes->hLB, LB_GETITEMDATA,uIndex,0L))<=0L)
                return SendMessage(pes->hLB,LB_INSERTSTRING,0xFFFF,(LONG)lpNode);

            if (pes->lpfnSort( hELB, lpen, lpNode )<=0)
                nLow = uIndex+1;
            else
                nHi = uIndex-1;
        }

        if (nHi==nLow)    // Insert where???
        {
            if ((lpen = (LPELBNODE)SendMessage(pes->hLB, LB_GETITEMDATA,nLow,0L))<=0L)
                return SendMessage(pes->hLB,LB_INSERTSTRING,0xFFFF,(LONG)lpNode);

            if (pes->lpfnSort( hELB, lpen, lpNode )<=0)
                return SendMessage(pes->hLB,LB_INSERTSTRING,nLow+1,(LONG)lpNode);
        }
        return SendMessage(pes->hLB,LB_INSERTSTRING,nLow,(LONG)lpNode);
    }
    else    // Otherwise we need to wade through folders to do an insertion
    {

        if (NODETYPE(lpNode)==NT_STYLE) // We can use binary for styles
        {
            LPELBNODE lpen2;

            nLow = uIndex;
            nHi = uIndex;

            lpen2 = (LPELBNODE)SendMessage(pes->hLB, LB_GETITEMDATA,nHi,0L);

            //*** Scan to bottom of list (in current folder)
            while ((LONG)lpen2>0L)
            {
                //*** Quit when we change tabbing levels
                if (nTab != (int)VI_GETTABLEVEL(lpen2))
                        break;

                nHi++;
                lpen2 = (LPELBNODE)SendMessage(pes->hLB, LB_GETITEMDATA,nHi,0L);
            }

            //*** nHi will always point to the node that FAILED
            nHi--;
 
            //*** The following speeds opening folders up since the
            //*** database is already sorted
            if (nHi>0)
            {
                if ((lpen = (LPELBNODE)SendMessage(pes->hLB, LB_GETITEMDATA,nHi,0L))>0L)
                {
                    if (pes->lpfnSort( hELB, lpen, lpNode )<=0)
                        return SendMessage(pes->hLB,LB_INSERTSTRING,nHi+1,(LONG)lpNode);
                }
            }

            goto do_quick_insert;
        }

        while (TRUE)
        {
            lpen = (LPELBNODE)SendMessage(pes->hLB, LB_GETITEMDATA,uIndex,0L);
            if ((LONG)lpen<=0L)
                return SendMessage(pes->hLB,LB_INSERTSTRING,0xFFFF,(LONG)lpNode);

            if (nTab == (int)VI_GETTABLEVEL(lpen))
            {
                LONG lRes = -1L;

                if (pes->lpfnSort)
                    lRes = pes->lpfnSort( hELB, lpen, lpNode );

                if (lRes>0)
                    return SendMessage(pes->hLB,LB_INSERTSTRING,uIndex,(LONG)lpNode);
            }
            else
            {
                if (nTab > (int)VI_GETTABLEVEL(lpen))
                    return SendMessage(pes->hLB,LB_INSERTSTRING,uIndex,(LONG)lpNode);
            }
            uIndex++;
        }
    }
    return -1L;

} //*** ELBAddInto


//*************************************************************
//
//  ELBCloseNode
//
//  Purpose:
//      Deletes all the children of a node and closes the node
//
//
//  Parameters:
//      HWND hELB
//      UINT uIndex
//      UINT uAll   - closes all chidren also
//      
//
//  Return: (LONG WINAPI)
//      
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LONG WINAPI ELBCloseNode( HWND hELB, UINT uIndex, UINT uAll )
{
    LPELBNODE lpen;
    PES       pes = (PES)GetWindowWord( hELB, WE_PES );
    int       nTab;

    if (!pes)
        return 0L;

    lpen = (LPELBNODE)SendMessage(pes->hLB, LB_GETITEMDATA,uIndex,0L);
    if (!lpen || lpen==(LPELBNODE)LB_ERR)
        return 0L;

    if (!VI_EXPANDABLE(lpen))
        return 0L;

    nTab = (int)VI_GETTABLEVEL(lpen);
    VI_CLOSENODE( lpen );
    uIndex++;

    SendMessage( pes->hLB, WM_SETREDRAW, 0, 0L );

    //*** Kill all the children
    while (TRUE)
    {
        lpen = (LPELBNODE)SendMessage(pes->hLB, LB_GETITEMDATA,uIndex,0L);
        if (!lpen || lpen==(LPELBNODE)LB_ERR)
            break;

        if (nTab >= (int)VI_GETTABLEVEL(lpen))
            break;

        if (uAll)
            VI_CLOSENODE( lpen );

        SendMessage( pes->hLB, LB_DELETESTRING, uIndex, 0L );
    }

    SendMessage( pes->hLB, WM_SETREDRAW, 1, 0L );

    return 1L;

} //*** ELBCloseNode


//*************************************************************
//
//  ELBDeleteNode
//
//  Purpose:
//      Deletes a node and all its children from the list
//
//
//  Parameters:
//      HWND hELB
//      UINT uIndex
//      
//
//  Return: (LONG WINAPI)
//
//
//  Comments:
//
//           
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LONG WINAPI ELBDeleteNode( HWND hELB, UINT uIndex )
{
    PES       pes = (PES)GetWindowWord( hELB, WE_PES );

    ELBCloseNode( hELB, uIndex, 0 );
    ELBRemoveNode( hELB, uIndex );

    return 0L;

} //*** ELBDeleteNode


//*************************************************************
//
//  ELBGetCurSel
//
//  Purpose:
//      Retrieves the current selection node
//
//
//  Parameters:
//      HWND hELB
//      LPUINT lpIndex
//
//  Return: (LPELBNODE WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LPELBNODE WINAPI ELBGetCurSel( HWND hELB, LPINT lpIndex )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
    {
        int nIndex = (int)SendMessage( pes->hLB, LB_GETCURSEL,0,0L);

        if (lpIndex)
            *lpIndex = nIndex;

        if (nIndex>=0)
        {
            LPELBNODE lpen =
                (LPELBNODE)SendMessage(pes->hLB, LB_GETITEMDATA,nIndex,0L);
            if (lpen==(LPELBNODE)LB_ERR)
                return NULL;
            return lpen;
        }
    }
    return NULL;

} //*** ELBGetCurSel


//*************************************************************
//
//  ELBGetDatabase
//
//  Purpose:
//      Retrieves the current database
//
//
//  Parameters:
//      HWND hELB
//      LPUINT lpIndex
//
//  Return: (LPDB WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LPDB WINAPI ELBGetDatabase( HWND hELB )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
        return pes->lpDB;
    return NULL;

} //*** ELBGetDatabase


//*************************************************************
//
//  ELBGetItem
//
//  Purpose:
//      Retrieves the node information for the index
//
//
//  Parameters:
//      HWND hELB
//      UINT uIndex
//      
//
//  Return: (LPELBNODE WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LPELBNODE WINAPI ELBGetItem( HWND hELB, UINT uIndex )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
    {
        LPELBNODE lpen =
            (LPELBNODE)SendMessage(pes->hLB, LB_GETITEMDATA,uIndex,0L);
        if (lpen==(LPELBNODE)LB_ERR)
            return NULL;
        return lpen;
    }
    return NULL;

} //*** ELBGetItem


//*************************************************************
//
//  ELBGetPaintProc
//
//  Purpose:
//      Retrieves the painting procedure
//
//
//  Parameters:
//      HWND hELB
//      
//
//  Return: (ELBPAINTPROC WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

ELBPAINTPROC WINAPI ELBGetPaintProc( HWND hELB )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
        return pes->lpfnPaint;
    return NULL;

} //*** ELBGetPaintProc


//*************************************************************
//
//  ELBGetSortProc
//
//  Purpose:
//      Retrieves the sort procedure in use
//
//
//  Parameters:
//      HWND hELB
//      
//
//  Return: (ELBSORTPROC WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

ELBSORTPROC WINAPI ELBGetSortProc( HWND hELB )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
        return pes->lpfnSort;
    return NULL;

} //*** ELBGetSortProc


//*************************************************************
//
//  ELBInsertNode
//
//  Purpose:
//      Inserts a node in the Extended ListBox before the uIndex.  Use 
//      -1 to insert at the end of the list.
//
//  Parameters:
//      HWND hELB
//      UINT uIndex     - node to insert BEFORE
//      LPELBNODE lpen  - node
//      
//
//  Return: (LONG WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LONG WINAPI ELBInsertNode( HWND hELB, UINT uIndex, LPELBNODE lpen )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
        return SendMessage( pes->hLB, LB_INSERTSTRING, uIndex, (LONG)lpen );
    return -1L;

} //*** ELBInsertNode


//*************************************************************
//
//  ELBRemoveNode
//
//  Purpose:
//      Removes the node (just the node) from the list
//
//
//  Parameters:
//      HWND hELB
//      UINT uIndex
//      
//
//  Return: (LONG WINAPI)
//      Number of items remaining
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LONG WINAPI ELBRemoveNode( HWND hELB, UINT uIndex )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
        return SendMessage( pes->hLB, LB_DELETESTRING, uIndex, 0L );
    return -1L;

} //*** ELBRemoveNode


//*************************************************************
//
//  ELBResetContent
//
//  Purpose:
//      Resets the contents of the Extended ListBox
//
//
//  Parameters:
//      HWND hELB
//      
//
//  Return: (LONG WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LONG WINAPI ELBResetContent( HWND hELB )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
        SendMessage( pes->hLB, LB_RESETCONTENT, 0, 0L );
    return 0L;

} //*** ELBResetContent


//*************************************************************
//
//  ELBSetBitmap
//
//  Purpose:
//      Sets the bitmap to use in the Extended ListBox
//
//
//  Parameters:
//      HWND hELB
//      HBITMAP hBmp    - the bitmap
//      UINT uBitmaps   - # of folders wide
//      
//
//  Return: (LONG WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LONG WINAPI ELBSetBitmap( HWND hELB, HBITMAP hBmp, UINT uBitmaps )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
    {
        BITMAP bmp;

        if (pes->hBitmap)
        {
            DeleteObject( pes->hBitmap ) ;
        }

        pes->hBitmap = LoadLBBitmap( ghInstLib, MAKEINTRESOURCE(ELB_FOLDERS) );

        GetObject( pes->hBitmap, sizeof(BITMAP), &bmp );
        pes->wBmpH = bmp.bmHeight/4;
        pes->wBmpW = bmp.bmWidth/uBitmaps;

        ELBSetItemHeight( pes );
    }
    return 0L;

} //*** ELBSetBitmap


//*************************************************************
//
//  ELBSetCurSel
//
//  Purpose:
//      Sets the current selection in the Extended ListBox
//
//
//  Parameters:
//      HWND hELB
//      UINT uIndex
//      
//
//  Return: (LONG WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LONG WINAPI ELBSetCurSel( HWND hELB, UINT uIndex )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
    {
        WORD wID = GetWindowWord( hELB, GWW_ID );
        LONG l = SendMessage( pes->hLB, LB_SETCURSEL, uIndex, 0L );
        PostMessage( GetParent(hELB), WM_COMMAND, wID, MAKELONG(hELB,LBN_SELCHANGE) );
    }
    return 0L;

} //*** ELBSetCurSel


//*************************************************************
//
//  ELBSetDatabase
//
//  Purpose:
//      Sets the current database
//
//
//  Parameters:
//      HWND hELB
//      LPDB lpDB
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

BOOL WINAPI ELBSetDatabase( HWND hELB, LPDB lpDB )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
        pes->lpDB = lpDB;
    return TRUE;

} //*** ELBSetDatabase


//*************************************************************
//
//  ELBSetFont
//
//  Purpose:
//      Sets the font in the Extended ListBox
//
//
//  Parameters:
//      HWND hELB
//      HFONT hFont
//      
//
//  Return: (LONG WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LONG WINAPI ELBSetFont( HWND hELB, HFONT hFont )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
    {
        TEXTMETRIC  tm;
        HDC         hDC = GetDC( NULL );

        if (hFont)
            pes->hFont = hFont;
        else
            pes->hFont = GetStockObject( SYSTEM_FONT );

        pes->hFont = SelectObject( hDC, pes->hFont );
        GetTextMetrics( hDC, &tm );
        pes->hFont = SelectObject( hDC, pes->hFont );
        ReleaseDC( NULL, hDC );

        pes->wFontH = abs(tm.tmHeight);
        ELBSetItemHeight( pes);
        InvalidateRect( hELB, NULL, TRUE );
    }
    return 0L;

} //*** ELBSetFont


//*************************************************************
//
//  ELBSetPaintProc
//
//  Purpose:
//      Sets the painting procedure for the Extended ListBox
//
//             
//  Parameters:
//      HWND hELB
//      ELBPAINTPROC lpfnPaint
//      
//
//  Return: (LONG WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LONG WINAPI ELBSetPaintProc( HWND hELB, ELBPAINTPROC lpfnPaint )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
        pes->lpfnPaint = lpfnPaint;
    return 0L;

} //*** ELBSetPaintProc


//*************************************************************
//
//  ELBSetSortProc
//
//  Purpose:
//      Sets the sort proc for the Extended ListBox
//
//
//  Parameters:
//      HWND hELB
//      ELBSORTPROC lpfnSort
//      
//
//  Return: (LONG WINAPI)
//      Unused
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

LONG WINAPI ELBSetSortProc( HWND hELB, ELBSORTPROC lpfnSort )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );

    if (pes)
        pes->lpfnSort = lpfnSort;
    return 0L;

} //*** ELBSetSortProc


//*************************************************************
//
//  LBSubClassProc
//
//  Purpose:
//      Handles the subclassing of the listbox
//
//
//  Parameters:
//      HWND hWnd
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (LONG WINAPI)
//
//
//  Comments:
//      Adds the following functionality:
//          Track with the right mouse button
//          Right mouse button dbl clicks
//          Enter simulates LBUTTONDBLCLK
//          Shift-Enter simulates RBUTTONDBLCLK
//
//  History:    Date       Author     Comment
//               3/ 8/92   MSM        Created
//
//*************************************************************

LONG WINAPI LBSubClassProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    WORD wID = GetWindowWord( hWnd, GWW_ID );

    if (msg==WM_KEYDOWN && wParam==VK_RETURN)
    {
        if (GetKeyState(VK_SHIFT)&0x8000)
            SendMessage( GetParent( hWnd ), WM_COMMAND, wID, 
                MAKELONG( hWnd, ELBN_RDBLCLK ) );
        else
            SendMessage( GetParent( hWnd ), WM_COMMAND, wID, 
                MAKELONG( hWnd, ELBN_ENTER ) );
    }

    if (msg==WM_KEYDOWN && wParam==VK_SPACE)
        SendMessage( GetParent( hWnd ), WM_COMMAND, wID, 
            MAKELONG( hWnd, ELBN_SPACE ) );

    if (msg==WM_RBUTTONDBLCLK)
        SendMessage( GetParent( hWnd ), WM_COMMAND, wID, 
            MAKELONG( hWnd, ELBN_RDBLCLK ) );

    //*** This simulates the left button actions on the right button
    //*** Note we trap the opposite button when using one or the other
    switch (msg)
    {
        case WM_LBUTTONDOWN:
        {
            PES pes = (PES)GetWindowWord( GetParent( hWnd ), WE_PES );
            UINT x ;
            x = LOWORD( lParam ) ;

            if (wParam & MK_RBUTTON)
                return 1L;
            
            if (x <= pes->wBmpW)
                PostMessage( GetParent( hWnd ), WM_COMMAND, wID, 
                             MAKELONG( hWnd, ELBN_SPACE ) );
        }
        break ;

        case WM_LBUTTONUP:
            if (wParam & MK_RBUTTON)
                return 1L;
        break;

        case WM_RBUTTONDOWN:
            if (wParam & MK_LBUTTON)
                return 1L;
            msg = WM_LBUTTONDOWN;
        break;

        case WM_RBUTTONUP:
            if (wParam & MK_LBUTTON)
                return 1L;
            msg = WM_LBUTTONUP;
        break;
    }
    return CallWindowProc( gwpOldLB, hWnd, msg, wParam, lParam );

} //*** LBSubClassProc

/****************************************************************
 *  HBITMAP WINAPI LoadLBBitmap( HINSTANCE hinst, LPCSTR lpsz )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
HBITMAP WINAPI LoadLBBitmap( HINSTANCE hinst, LPCSTR lpsz )
{
   HBITMAP hbmSrc = NULL ;
   HDC      hDC ;
   HDC      hdcMem ;
   BITMAP   bmBits ;
   COLORREF rgbOld;

   if (!(hbmSrc = LoadBitmap( ghInstLib, MAKEINTRESOURCE(ELB_FOLDERS) )))
      return NULL ;

   if (hDC = GetDC( NULL ))
   {
      if (hdcMem = CreateCompatibleDC( hDC ))
      {
         //
         // Get the bitmap struct needed by bmColorTranslateDC()
         //
         GetObject( hbmSrc, sizeof( BITMAP ), (LPSTR)&bmBits ) ;

         //
         // Select our bitmap into the memory DC
         //
         hbmSrc = SelectObject( hdcMem, hbmSrc ) ;

         rgbOld = GetPixel( hdcMem, 0, 0 ) ;
         //
         // Translate the sucker
         //
         bmColorTranslateDC( hdcMem,
                             &bmBits,
                             rgbOld,
                             GetSysColor( COLOR_WINDOW ),
                             0 ) ;

         rgbOld = GetPixel( hdcMem, 0, bmBits.bmHeight / 2+1 ) ;
         bmColorTranslateDC( hdcMem,
                             &bmBits,
                             rgbOld,
                             GetSysColor( COLOR_HIGHLIGHT ),
                             0 ) ;

         //
         // Unselect our bitmap before deleting the DC
         //
         hbmSrc = SelectObject( hdcMem, hbmSrc ) ;

         DeleteDC( hdcMem ) ;
      }
      DASSERT( hWDB, hdcMem ) ;

      ReleaseDC( NULL, hDC ) ;
   }

   return hbmSrc ;

} /* LoadLBBitmap()  */


/****************************************************************
 *  VOID FAR PASCAL bmColorTranslateDC( HDC hdcBM, LPBITMAP lpBM,
 *                                    COLORREF rgbOld, COLORREF rgbNew,
 *                                    WORD  wStyle )
 *
 *  Description: 
 *
 *    This function makes all pixels in the given DC that have the
 *    color rgbOld have the color rgbNew.
 *
 *    wSytle can be BM_NORMAL or BM_DISABLED.  If it is BM_DISABLED
 *    the bitmap will be translated so that it looks disabled.
 *
 *  Comments:
 *
 ****************************************************************/
VOID FAR PASCAL bmColorTranslateDC( HDC hdcBM, LPBITMAP lpBM,
                                  COLORREF rgbOld, COLORREF rgbNew,
                                  WORD  wStyle )
{
   HDC      hdcMask ;
   HBITMAP  hbmMask, hbmT ;
   HBRUSH   hbrT ;

   #ifdef DEBUG
   if (!lpBM)
   {
      return ;
   }
   #endif 

   //
   // if the bitmap is mono we have nothing to do
   //
   if (lpBM->bmPlanes == 1 && lpBM->bmBitsPixel == 1)
      return ;

   //
   //  Create the mask bitmap and associated DC
   //
   if (hbmMask = CreateBitmap( lpBM->bmWidth, lpBM->bmHeight, 1, 1, NULL ))
   {
      if (hdcMask = CreateCompatibleDC( hdcBM ))
      {
         //
         // Select th mask bitmap into the mono DC
         //
         hbmT = SelectObject( hdcMask, hbmMask ) ;

         // 
         // Create the brush and select it into the bits DC
         //
         hbrT = SelectObject( hdcBM, CreateSolidBrush( rgbNew ) ) ;

         // 
         // Do a color to mono bitblt to build the mask.  Generate
         // 1's where the source is equal to the background.
         //
         SetBkColor( hdcBM, rgbOld ) ;
         BitBlt( hdcMask, 0, 0, lpBM->bmWidth, lpBM->bmHeight,
                 hdcBM, 0, 0, SRCCOPY ) ;

         //
         // Where the mask is 1, lay down the brush, where it is 0, leave
         // the destination.
         //
         SetBkColor( hdcBM, RGBWHITE ) ;
         SetTextColor( hdcBM, RGBBLACK ) ;
#define DSPDxax   0x00E20746L
         BitBlt( hdcBM, 0, 0, lpBM->bmWidth, lpBM->bmHeight,
                 hdcMask, 0, 0, DSPDxax ) ;

         SelectObject( hdcMask, hbmT ) ;

         hbrT = SelectObject( hdcBM, hbrT ) ;
         DeleteObject( hbrT ) ;

         DeleteDC( hdcMask ) ;
      }
      else
      {
         return ;
      }

      DeleteObject( hbmMask ) ;
   }
   else
   {
      return ;
   }

} /* bmColorTranslateDC()  */


//*************************************************************
//
//  SelectByNode
//
//  Purpose:
//      Selects the node in the list box, or the first one if not found
//
//
//  Parameters:
//      HWND hELB
//      LPELBNODE lpen
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/14/92   MSM        Created
//
//*************************************************************

VOID WINAPI SelectByNode( HWND hELB, LPELBNODE lpen )
{
    PES pes = (PES)GetWindowWord( hELB, WE_PES );
    WORD wCount, wI;

    wCount = ListBox_GetCount( pes->hLB );

    for (wI=0; wI<wCount; wI++)
    {
        if ( ELBGetItem( hELB, wI ) == lpen )
        {
            ELBSetCurSel( hELB, wI );
            return;
        }
    }

    //*** not found, set to the first one in the list
    ELBSetCurSel( hELB, 0 );

} //*** SelectByNode

//*** EOF: elb.c
