//*************************************************************
//  File name: toolbar.c
//
//  Description: 
//      Code for the toolbar control
//
//  History:    Date       Author     Comment
//               4/21/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"

#include "ICEK.h"

#ifndef WIN32

LRESULT CALLBACK fnToolbar( HWND, UINT, WPARAM, LPARAM );

typedef struct
{
    DIMENS      Dimens;
    int         nPaintY;
    HLIST       hlstBtns;   // List of BtnInfo's
    UINT        uiState;    // State of the bar
    LPBTNINFO   lpB;        
    RECT        rc;

} TBHDR, *PTBHDR;

#define BTNSTATE        0x0001
#define BTNENABLE       0x0002

#define BTN_DEPRESSED   0x8000
#define TBST_IDLE       0x0000
#define TBST_TRACKING   0x0001
#define TBST_LASTHIT    0x0002


VOID NEAR CleanUpToolbar( PTBHDR pTBHdr );
void NEAR PASCAL ToolbarPaint( HWND hWnd );
LPBTNINFO NEAR PASCAL GetBtnInfo( HWND hWnd, UINT uiID );
VOID NEAR PASCAL FillBtnRect( PTBHDR, LPBTNINFO, LPRECT );
VOID NEAR PASCAL GetBtnRect( HWND, LPBTNINFO, LPRECT );
VOID NEAR PASCAL GetRightBtnRect( HWND hWnd, LPBTNINFO lpB, LPRECT lprc);
LPBTNINFO NEAR PASCAL BtnInfoFromPoint( HWND hWnd, POINT pt );

LONG NEAR PASCAL HandleMouseButton( HWND, UINT, WPARAM, LPARAM );
LONG NEAR PASCAL HandleMouseMove( HWND, UINT, WPARAM, LPARAM );

VOID NEAR PASCAL PaintButton( HWND, HDC, LPBTNINFO );
VOID NEAR PASCAL PaintTextButton( HWND hWnd, HDC hDC, LPBTNTEXT lpB);
void NEAR PASCAL DrawButtonFrame( HDC hDC, LPRECT lprc);

VOID NEAR PASCAL ButtonNotify( HWND, LPBTNINFO );

#define WE_PTBHDR   0
#define WE_HFONT    2

//************************************************************************
//
//  Description:
//     Registers the custom controls in this DLL.
//
//  Parameters:
//     HANDLE hInstance
//        instance of this DLL
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//               5/10/92   CEK        Moved to CEK.DLL
//
//************************************************************************

BOOL WINAPI ToolbarInit( HINSTANCE hInstPrev, HINSTANCE hInstance, LPSTR lpsz )
{
    WNDCLASS wc ;

    wc.style        = CS_VREDRAW|CS_HREDRAW|CS_GLOBALCLASS|CS_DBLCLKS;
    wc.lpfnWndProc  = (WNDPROC)fnToolbar;
                                
    wc.cbClsExtra   = 0;           
    wc.cbWndExtra   = 4;
    wc.hInstance    = hInstance;    
    wc.hIcon        = NULL;
    wc.hCursor      = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground= (HBRUSH)(COLOR_BTNFACE+1) ;
    wc.lpszMenuName = NULL;
    if (lpsz)
       wc.lpszClassName= lpsz ;
    else
       wc.lpszClassName= SZ_TOOLBARCLASSNAME;

    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;

} //*** RegisterToolbarClass

//************************************************************************
//  fnToolbar
//
//  Description:
//     Toolbar control
//
//  History:   Date       Author      Comment
//               4/21/92   MSM        Created
//
//************************************************************************

LRESULT CALLBACK fnToolbar( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    PTBHDR pTBHdr = (PTBHDR)GetWindowWord( hWnd, WE_PTBHDR );

    switch (msg)
    {
        case WM_CREATE:
        {
            pTBHdr = (PTBHDR)LocalAlloc( LPTR, sizeof(TBHDR) );

            if (!pTBHdr)
                return -1L;

            pTBHdr->Dimens.nSpacerX = pTBHdr->Dimens.nButtonX = 
                pTBHdr->Dimens.nButtonY = 1;

            pTBHdr->nPaintY = 1;

            if ( !(pTBHdr->hlstBtns = cekmmListCreate( NULL )) )
            {
                LocalFree( (HLOCAL)pTBHdr );
                return -1L;
            }
            SetWindowWord( hWnd, WE_PTBHDR, (WORD)pTBHdr );
            SetWindowWord( hWnd, WE_HFONT, (WORD)GetStockObject(SYSTEM_FONT) );
            return 1L;    
        }
        break;

        case WM_SETFONT:
            if (wParam==NULL)
                SetWindowWord( hWnd, WE_HFONT, (WORD)GetStockObject(SYSTEM_FONT) );
            else    
                SetWindowWord( hWnd, WE_HFONT, wParam );

            if (lParam)
                InvalidateRect( hWnd, NULL, TRUE );
        break;

        case WM_LBUTTONDBLCLK:
        {
            POINT pt;
            LPBTNINFO lpB;

            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);

            if(lpB = BtnInfoFromPoint( hWnd, pt ))
                if (lpB->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER))
                    ButtonNotify( hWnd, lpB );
        }
        break;

        case WM_LBUTTONDOWN:
            pTBHdr->uiState=TBST_IDLE;
        // Fall through

        case WM_LBUTTONUP:
            return HandleMouseButton( hWnd, msg, wParam, lParam );
        break;

        case WM_MOUSEMOVE:
            return HandleMouseMove( hWnd, msg, wParam, lParam );
        break;

        case WM_SIZE:
        {
            RECT rc;

            GetClientRect( hWnd, &rc );
            if (rc.bottom > pTBHdr->Dimens.nButtonY+2)
                pTBHdr->nPaintY = (rc.bottom-pTBHdr->Dimens.nButtonY)/2 ;
            else
                pTBHdr->nPaintY = 1;
        }
        break;

        case TBM_SETDIMENS:
        {
            RECT rc;

            _fmemcpy( &(pTBHdr->Dimens), (LPDIMENS)lParam, sizeof(DIMENS) );

            GetClientRect( hWnd, &rc );
            if (rc.bottom > pTBHdr->Dimens.nButtonY+2)
                pTBHdr->nPaintY = (rc.bottom-pTBHdr->Dimens.nButtonY)/2 ;
            else
                pTBHdr->nPaintY = 1;

            InvalidateRect( hWnd, NULL, TRUE );
            return 1L;
        }
        break;

        case TBM_GETDIMENS:
        {
            *((LPDIMENS)lParam) = pTBHdr->Dimens;
            return 1L;
        }
        break;

        case TBM_ADDBUTTON:
        {
            LPBTNINFO lpNB, lpB = (LPBTNINFO)lParam;
            LPSTR lp = NULL;

            if (lpB)
            {
                if (lpB->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER))
                {
                    if (((LPBTNTEXT)lpB)->lpText)
                    {
                        HHEAP hhHeap = cekmmListHeap( pTBHdr->hlstBtns );

                        lp = cekmmHeapAllocString( hhHeap, LPTR, ((LPBTNTEXT)lpB)->lpText  );
                        if (!lp)
                            return 0L;
                    }
                }
            }

            lpNB = (LPBTNINFO)cekmmListAllocInsertNode(pTBHdr->hlstBtns, (LPSTR)lpB,
                        sizeof( BTNINFO), LIST_INSERTTAIL );

            if (!lpNB)
                return 0L;

            lpNB->dwData = (DWORD)lp;

            PaintButton( hWnd, NULL, lpNB );
            return 1L;
        }
        break;

        case TBM_REMOVEBUTTON:
        {
            LPBTNINFO lpB;

            if (wParam==1)
                lpB = GetBtnInfo( hWnd, (UINT)lParam );
            else    
                lpB = (LPBTNINFO)cekmmListItem2Node( pTBHdr->hlstBtns, lParam );

            if (!lpB)
                return 0L;

            if (lpB->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER))
            {
                if ( lpB->dwData )  // Free text
                {
                    HHEAP hhHeap = cekmmListHeap( pTBHdr->hlstBtns );

                    cekmmHeapUnlockFree( hhHeap, (LPSTR)lpB->dwData );
                }
            }

            cekmmListDeleteNode( pTBHdr->hlstBtns, (LPNODE)lpB );
            InvalidateRect( hWnd, NULL, TRUE );
            return 1L;
        }
        break;

        case TBM_GETBTNSTATE:
        {
            LPBTNINFO lpB = GetBtnInfo( hWnd, (UINT)wParam );

            if (lpB)
                return (lpB->uiBtnState & BTNSTATE);
            return -1L;
        }
        break;

        case TBM_SETBTNSTATE:
        {
            LPBTNINFO lpB = GetBtnInfo( hWnd, (UINT)wParam );
            long l;

            if (!lpB)
                return -1L;

            l = lpB->uiBtnState & BTNSTATE;

            if (lParam)
            {
                lpB->uiBtnState |= BTN_DOWN;

                if (lpB->uiBtnStyle&BTNS_TOGGLEEX)
                {
                    LPBTNINFO lpBI = lpB;

                    while(1)
                    {
                        LPBTNINFO lpBI2;

                        lpBI2 = (LPBTNINFO)cekmmListPrevNode(pTBHdr->hlstBtns, lpBI);

                        if (!lpBI2 || !(lpBI2->uiBtnID))
                            break;
                        else
                            lpBI = lpBI2;
                    }

                    while (lpBI && lpBI->uiBtnID)
                    {
                        if (lpBI!=lpB)
                        {
                            if (lpBI->uiBtnState & BTN_DOWN)
                            {
                                lpBI->uiBtnState &= ~BTN_DOWN;
                                PaintButton( hWnd, NULL, lpBI );
                            }
                        }
                        lpBI = (LPBTNINFO)cekmmListNextNode(pTBHdr->hlstBtns, lpBI);
                    }
                    lpB->uiBtnState |= BTN_DOWN;
                }
            }
            else
                lpB->uiBtnState &= ~BTN_DOWN;

            if (l != (long)(lpB->uiBtnState & BTNSTATE))
                PaintButton( hWnd, NULL, lpB );
            return l;

        }
        break;

        case TBM_ENABLEBTN:
        {
            LPBTNINFO lpB = GetBtnInfo( hWnd, (UINT)wParam );
            long l;

            if (!lpB)
                return 0L;

            l = lpB->uiBtnState & BTNENABLE;

            if (lParam)
                lpB->uiBtnState &= ~BTN_DISABLED;
            else
                lpB->uiBtnState |= BTN_DISABLED;
            if (!l == !lParam)
                PaintButton( hWnd, NULL, lpB );
            return !l;
        }
        break;

        case WM_PAINT:
            ToolbarPaint( hWnd );
        break;

        // wParam==ID, lParam == lData, lReturn = lOldData
        case TBM_SETBTNDATA:
        {
            LPBTNINFO lpB = GetBtnInfo( hWnd, (UINT)wParam );
            long l;

            if (!lpB)
                return -1L;

            if (lpB->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER))
                return -1L;

            l = lpB->dwData;
            lpB->dwData = lParam;
            return l;
        }
        break;

        // wParam==ID, lReturn == lData             
        case TBM_GETBTNDATA:
        {
            LPBTNINFO lpB = GetBtnInfo( hWnd, (UINT)wParam );

            if (!lpB)
                return -1L;

            if (lpB->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER))
                return -1L;

            return lpB->dwData;
        }
        break;

        // wParam==ID, lParam == lpText 
        case TBM_SETBTNTEXT:
        {
            LPBTNTEXT lpB = (LPBTNTEXT)GetBtnInfo( hWnd, (UINT)wParam );
            HHEAP hhHeap = cekmmListHeap( pTBHdr->hlstBtns );

            if (!lpB)
                return -1L;

            if (!(lpB->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER)))
                return -1L;

            if ( lpB->lpText )  // Free text
                cekmmHeapUnlockFree(hhHeap, lpB->lpText );
            lpB->lpText = NULL;

            if (lParam)
            {
                lpB->lpText = cekmmHeapAllocString(hhHeap, LPTR, (LPSTR)lParam);
                if (!lpB->lpText)
                    return -1L;
            }
            PaintTextButton( hWnd, NULL, lpB );
            return 1L;
        }
        break;

        // wParam==ID, lReturn == nLen
        case TBM_GETBTNTEXTLENGTH:
        {
            LPBTNTEXT lpB = (LPBTNTEXT)GetBtnInfo( hWnd, (UINT)wParam );

            if (!lpB)
                return -1L;

            if (!(lpB->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER)))
                return -1L;

            if ( lpB->lpText )
                return lstrlen( lpB->lpText );
            return 0L;
        }
        break;

        // wParam==ID, lParam == lBuffer
        case TBM_GETBTNTEXT:
        {
            LPSTR lp = (LPSTR)lParam;
            LPBTNTEXT lpB = (LPBTNTEXT)GetBtnInfo( hWnd, (UINT)wParam );

            if (!lpB)
                return -1L;

            if (!(lpB->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER)))
                return -1L;

            if ( lpB->lpText )
                lstrcpy( lp, lpB->lpText );
            else
                *lp = '\0';
            return 1L;
        }
        break;

        case WM_DESTROY:
        {
            
            CleanUpToolbar( pTBHdr );
        }
        break;
    }         
    return DefWindowProc( hWnd, msg, wParam, lParam );

} //*** fnToolbar


//*************************************************************
//
//  CleanUpToolbar
//
//  Purpose:
//      Cleans up the toolbar resources
//
//
//  Parameters:
//      PTBHDR pTBHdr
//      
//
//  Return: (VOID NEAR)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/24/92   MSM        Created
//
//*************************************************************

VOID NEAR CleanUpToolbar( PTBHDR pTBHdr )
{
    if (!pTBHdr)
        return;

    if (pTBHdr->hlstBtns)
        cekmmListDestroy( pTBHdr->hlstBtns );
    LocalFree( (HLOCAL)pTBHdr );

} //*** CleanUpToolbar


//*************************************************************
//
//  HandleMouseButton
//
//  Purpose:
//      Handles the mouse buttons
//
//
//  Parameters:
//      HWND hWnd
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (LONG NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/22/92   MSM        Created
//
//*************************************************************

LONG NEAR PASCAL HandleMouseButton(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PTBHDR pTBHdr = (PTBHDR)GetWindowWord( hWnd, WE_PTBHDR );
    LPBTNINFO lpB;
    POINT pt;

    if (pTBHdr->uiState==TBST_IDLE && msg==WM_LBUTTONUP)
        return 0L;

    if (msg==WM_LBUTTONDOWN)
    {
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        if (!(lpB = BtnInfoFromPoint( hWnd, pt )))
            return 0L;

        if (lpB->uiBtnState&BTN_DISABLED || lpB->uiBtnID==0)
            return 0L;

        if (lpB->uiBtnState & (BTNS_TEXT|BTNS_TEXTBORDER))
            return 0L;

        pTBHdr->uiState = TBST_TRACKING|TBST_LASTHIT;
        pTBHdr->lpB = lpB;
        GetBtnRect( hWnd, lpB, &(pTBHdr->rc) );
        lpB->uiBtnState |= BTN_DEPRESSED;
        PaintButton( hWnd, NULL, lpB );
        SetCapture( hWnd );
        return 0L;    
    }

    if (msg==WM_LBUTTONUP)
    {
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        SetCapture( NULL );

        lpB = BtnInfoFromPoint( hWnd, pt );

        pTBHdr->lpB->uiBtnState &= ~BTN_DEPRESSED;
        pTBHdr->uiState = TBST_IDLE;

        if (lpB!=pTBHdr->lpB)
        {
            pTBHdr->lpB = NULL;
            return 0L;    
        }
        pTBHdr->lpB = NULL;

        if (lpB->uiBtnStyle&BTNS_TOGGLEEX)
        {
            LPBTNINFO lpBI = lpB;

            while(1)
            {
                LPBTNINFO lpBI2;

                lpBI2 = (LPBTNINFO)cekmmListPrevNode(pTBHdr->hlstBtns, lpBI);

                if (!lpBI2 || !(lpBI2->uiBtnID))
                    break;
                else
                    lpBI = lpBI2;
            }

            while (lpBI && lpBI->uiBtnID)
            {
                if (lpBI!=lpB)
                {
                    if (lpBI->uiBtnState & BTN_DOWN)
                    {
                        lpBI->uiBtnState &= ~BTN_DOWN;
                        PaintButton( hWnd, NULL, lpBI );
                    }
                }
                lpBI = (LPBTNINFO)cekmmListNextNode(pTBHdr->hlstBtns, lpBI);
            }

            lpB->uiBtnState |= BTN_DOWN;
            PaintButton( hWnd, NULL, lpB );
            ButtonNotify( hWnd, lpB );
            return 1L;
        }

        if (lpB->uiBtnStyle&BTNS_TOGGLE)
        {
            lpB->uiBtnState ^= BTN_DOWN;
            PaintButton( hWnd, NULL, lpB );
            ButtonNotify( hWnd, lpB );
            return 1L;
        }

        lpB->uiBtnState &= ~BTN_DOWN;
        PaintButton( hWnd, NULL, lpB );
        ButtonNotify( hWnd, lpB );
        return 1L;
    }

} //*** HandleMouseButton


//*************************************************************
//
//  HandleMouseMove
//
//  Purpose:
//      Handles the mouse moving
//
//
//  Parameters:
//      HWND hWnd
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (LONG NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/22/92   MSM        Created
//
//*************************************************************

LONG NEAR PASCAL HandleMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PTBHDR pTBHdr = (PTBHDR)GetWindowWord( hWnd, WE_PTBHDR );
    LPBTNINFO lpB;
    POINT     pt;

    if (pTBHdr->uiState==TBST_IDLE)
        return 0L;

    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);

    lpB = BtnInfoFromPoint( hWnd, pt );

    //*** Changing from on to off
    if (lpB!=pTBHdr->lpB && (pTBHdr->uiState&TBST_LASTHIT) )
    {
        pTBHdr->lpB->uiBtnState &= ~BTN_DEPRESSED;
        pTBHdr->uiState &= ~TBST_LASTHIT;
        PaintButton( hWnd, NULL, pTBHdr->lpB );
        return 0L;
    }

    //*** Changing from off to on
    if (lpB==pTBHdr->lpB && !(pTBHdr->uiState&TBST_LASTHIT) )
    {
        pTBHdr->lpB->uiBtnState |= BTN_DEPRESSED;
        pTBHdr->uiState |= TBST_LASTHIT;
        PaintButton( hWnd, NULL, pTBHdr->lpB );
        return 0L;
    }

    return 1L;

} //*** HandleMouseMove


//*************************************************************
//
//  ToolbarPaint
//
//  Purpose:
//      Paints the tool bar
//
//
//  Parameters:
//      HWND hWnd
//      
//
//  Return: (void NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/21/92   MSM        Created
//
//*************************************************************

void NEAR PASCAL ToolbarPaint( HWND hWnd )
{
    RECT         r1;
    COLORREF     rgbBtnFace;
    PAINTSTRUCT  ps;
    PTBHDR       pTBHdr = (PTBHDR)GetWindowWord( hWnd, WE_PTBHDR );
    LPBTNINFO    lpB;

    GetClientRect( hWnd, &r1 ) ;

    if (!BeginPaint( hWnd, &ps ))
        return ;

    if (!RectVisible( ps.hdc, &r1 ))
    {
        EndPaint( hWnd, &ps ) ;
        return ;
    }

    rgbBtnFace = GetSysColor( COLOR_BTNFACE ) ;

    SetBkColor( ps.hdc, GetNearestColor( ps.hdc, rgbBtnFace ) ) ;
    SetBkMode( ps.hdc, TRANSPARENT ) ;

    tdDraw3DRect( ps.hdc, &r1, 1, DRAW3D_OUT ) ;

    lpB = (LPBTNINFO)cekmmListLastNode( pTBHdr->hlstBtns );

    while (lpB)
    {
        if (lpB->uiBtnID)
            PaintButton( hWnd, ps.hdc, lpB );

        lpB = (LPBTNINFO)cekmmListPrevNode( pTBHdr->hlstBtns, lpB );
    }

    EndPaint( hWnd, &ps ) ;

} //*** ToolbarPaint


//*************************************************************
//
//  GetBtnInfo
//
//  Purpose:
//      Retrieves the button with the ID
//
//
//  Parameters:
//      HWND hWnd
//      UINT uiID
//      
//
//  Return: (LPBTNINFO NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/21/92   MSM        Created
//
//*************************************************************

LPBTNINFO NEAR PASCAL GetBtnInfo( HWND hWnd, UINT uiID )
{
    PTBHDR pTBHdr = (PTBHDR)GetWindowWord( hWnd, WE_PTBHDR );
    LPBTNINFO lpB = (LPBTNINFO)cekmmListFirstNode( pTBHdr->hlstBtns );

    while (lpB)
    {
        if (lpB->uiBtnID == uiID)
            break;
        lpB = (LPBTNINFO)cekmmListNextNode( pTBHdr->hlstBtns, lpB );
    }
    return lpB;

} //*** GetBtnInfo


//*************************************************************
//
//  FillBtnRect
//
//  Purpose:
//      Fills the rectangle from the left member
//
//
//  Parameters:
//      LPBTNINFO lpB
//      LPRECT lprc
//      PTBHDR pTBHdr
//
//  Return: (VOID NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/24/92   MSM        Created
//
//*************************************************************

VOID NEAR PASCAL FillBtnRect( PTBHDR pTBHdr, LPBTNINFO lpB, LPRECT lprc )
{
    int fText = lpB->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER);

    if (lpB->uiBtnID && !fText)
        lprc->right = lprc->left + pTBHdr->Dimens.nButtonX - 1;
    else
    {
        if (fText || lpB->nBmpX)
            lprc->right = lprc->left + lpB->nBmpX-1;
        else
            lprc->right = lprc->left + pTBHdr->Dimens.nSpacerX-1;
    }
    lprc->top   = pTBHdr->nPaintY;
    lprc->bottom= lprc->top + pTBHdr->Dimens.nButtonY - 1;

} //*** FillBtnRect


//*************************************************************
//
//  GetBtnRect
//
//  Purpose:
//      Retrieves the rectangle for a button
//
//
//  Parameters:
//      HWND hWnd
//      LPBTNINFO lpB
//      LPRECT lprc
//      
//
//  Return: (VOID NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/22/92   MSM        Created
//
//*************************************************************

VOID NEAR PASCAL GetBtnRect( HWND hWnd, LPBTNINFO lpB, LPRECT lprc)
{
    LPBTNINFO lpBI;
    PTBHDR    pTBHdr = (PTBHDR)GetWindowWord( hWnd, WE_PTBHDR );
    int       nLeft;

    SetRectEmpty( lprc );

    if (lpB->uiBtnStyle & BTNS_RIGHTBTN)
    {
        GetRightBtnRect( hWnd, lpB, lprc );
        return;
    }

    nLeft = pTBHdr->Dimens.nSpacerX;

    lpBI = (LPBTNINFO)cekmmListFirstNode( pTBHdr->hlstBtns );

    while (lpBI)
    {
        int fText = lpBI->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER);

        if (lpBI==lpB)
            break;

        if (!(lpBI->uiBtnStyle & BTNS_RIGHTBTN))
        {
            if (lpBI->uiBtnID && !fText)
                nLeft += pTBHdr->Dimens.nButtonX+1;
            else
            {
                if (fText || lpBI->nBmpX)
                    nLeft += lpBI->nBmpX+1;
                else
                    nLeft += pTBHdr->Dimens.nSpacerX+1;
            }
        }

        lpBI = (LPBTNINFO)cekmmListNextNode( pTBHdr->hlstBtns, lpBI );
    }

    if (lpBI)
    {
        lprc->left  = nLeft;

        FillBtnRect( pTBHdr, lpB, lprc );
    }

} //*** GetBtnRect

//*************************************************************
//
//  GetRightBtnRect
//
//  Purpose:
//      Retrieves the rectangle for a button that is right justified
//
//
//  Parameters:
//      HWND hWnd
//      LPBTNINFO lpB
//      LPRECT lprc
//      
//
//  Return: (VOID NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/22/92   MSM        Created
//
//*************************************************************

VOID NEAR PASCAL GetRightBtnRect( HWND hWnd, LPBTNINFO lpB, LPRECT lprc)
{
    LPBTNINFO lpBI;
    PTBHDR    pTBHdr = (PTBHDR)GetWindowWord( hWnd, WE_PTBHDR );
    int       nLeft = 0;
    int       nThisLeft=-1;

    SetRectEmpty( lprc );

    lpBI = (LPBTNINFO)cekmmListFirstNode( pTBHdr->hlstBtns );

    while (lpBI)
    {
        int fText = lpBI->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER);

        if (lpBI==lpB)  // Remember this buttons rect
            nThisLeft = nLeft;

        if (lpBI->uiBtnStyle & BTNS_RIGHTBTN)
        {
            if (lpBI->uiBtnID && !fText)
                nLeft += pTBHdr->Dimens.nButtonX+1;
            else
            {
                if (fText || lpBI->nBmpX)
                    nLeft += lpBI->nBmpX+1;
                else
                    nLeft += pTBHdr->Dimens.nSpacerX+1;
            }
        }

        lpBI = (LPBTNINFO)cekmmListNextNode( pTBHdr->hlstBtns, lpBI );
    }

    if (nThisLeft!=-1)
    {
        RECT rc;

        GetClientRect( hWnd, &rc );

        lprc->left = rc.right - pTBHdr->Dimens.nSpacerX - nLeft + nThisLeft;
        FillBtnRect( pTBHdr, lpB, lprc );
    }

} //*** GetRightBtnRect


//*************************************************************
//
//  BtnInfoFromPoint
//
//  Purpose:
//      Returns the button under the point
//
//
//  Parameters:
//      HWND hWnd
//      POINT pt
//      
//
//  Return: (LPBTNINFO NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/22/92   MSM        Created
//
//*************************************************************

LPBTNINFO NEAR PASCAL BtnInfoFromPoint( HWND hWnd, POINT pt )
{
    LPBTNINFO lpBI;
    PTBHDR    pTBHdr = (PTBHDR)GetWindowWord( hWnd, WE_PTBHDR );
    RECT      rc;
                 
    SetRectEmpty( &rc );

    rc.left = pTBHdr->Dimens.nSpacerX;
    rc.top   = pTBHdr->nPaintY;
    rc.bottom= rc.top + pTBHdr->Dimens.nButtonY - 1;

    if ((pt.y < rc.top)||(pt.y>rc.bottom))
        return NULL;

    lpBI = (LPBTNINFO)cekmmListFirstNode( pTBHdr->hlstBtns );

    while (lpBI)
    {
        int fText = lpBI->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER);

        if (lpBI->uiBtnStyle & BTNS_RIGHTBTN)
        {
            RECT rt;

            GetRightBtnRect( hWnd, lpBI, &rt);
            if (PtInRect( &rt, pt ))
                return lpBI;
        }
        else
        {
            if (lpBI->uiBtnID && !fText)
                rc.right = rc.left + pTBHdr->Dimens.nButtonX+1;
            else
            {
                if (fText || lpBI->nBmpX)
                    rc.right = rc.left + lpBI->nBmpX + 1;
                else
                    rc.right = rc.left + pTBHdr->Dimens.nSpacerX+1;
            }
            if (PtInRect( &rc, pt ))
                return lpBI;
            else
                rc.left = rc.right;
        }

        lpBI = (LPBTNINFO)cekmmListNextNode( pTBHdr->hlstBtns, lpBI );
    }
    return NULL;

} //*** BtnInfoFromPoint


//*************************************************************
//
//  PaintButton
//
//  Purpose:
//      Paints just the button
//
//
//  Parameters:
//      HWND hWnd
//      HDC hDC
//      LPBTNINFO lpB
//      
//
//  Return: (VOID NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/21/92   MSM        Created
//
//*************************************************************

VOID NEAR PASCAL PaintButton( HWND hWnd, HDC hDC, LPBTNINFO lpB)
{
    PTBHDR    pTBHdr = (PTBHDR)GetWindowWord( hWnd, WE_PTBHDR );
    RECT      rc;
    int       nMyDC = TRUE;
    COLORREF  rgbBtnFace = GetSysColor( COLOR_BTNFACE ) ;
    

    if (!hDC)
        hDC = GetDC( hWnd );
    else
        nMyDC = FALSE;

    if (lpB->uiBtnStyle & (BTNS_TEXT|BTNS_TEXTBORDER))
    {
        PaintTextButton( hWnd, hDC, (LPBTNTEXT)lpB );
        if (nMyDC)
            ReleaseDC( hWnd, hDC );
        return;
    }

    GetBtnRect( hWnd, lpB, &rc);

    SetBkColor( hDC, GetNearestColor( hDC, rgbBtnFace ) );
    SetBkMode( hDC, TRANSPARENT );

    DrawButtonFrame(hDC,&rc);

    SetBkColor( hDC, GetNearestColor( hDC, rgbBtnFace ) );
    ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL );

    if (lpB->hBitmap)
    {
        HDC hMDC = CreateCompatibleDC( hDC );
        HBITMAP hOldBmp = SelectObject( hMDC, lpB->hBitmap );
        UINT uX, uY;

        uX = lpB->nBmpX;
        uY = lpB->nBmpY;

        if (lpB->uiBtnState & BTN_DISABLED)
        {
            uX += 2 * pTBHdr->Dimens.nButtonX;
        }
        else
        {
            if (lpB->uiBtnState & BTN_DEPRESSED)
                uX += 3 * pTBHdr->Dimens.nButtonX;
            else
            {
                if (lpB->uiBtnState & BTN_DOWN)
                    uX += pTBHdr->Dimens.nButtonX;
            }
        }

        BitBlt( hDC, rc.left, rc.top, pTBHdr->Dimens.nButtonX,
            pTBHdr->Dimens.nButtonY, hMDC, uX, uY, SRCAND );

        BitBlt( hDC, rc.left, rc.top, pTBHdr->Dimens.nButtonX,
            pTBHdr->Dimens.nButtonY,hMDC,uX,uY+pTBHdr->Dimens.nButtonY,SRCPAINT);

        SelectObject( hMDC, hOldBmp );
        DeleteDC( hMDC );
    }

    if (nMyDC)
        ReleaseDC( hWnd, hDC );

} //*** PaintButton


//*************************************************************
//
//  PaintTextButton
//
//  Purpose:
//      Paints a text button
//
//
//  Parameters:
//      HWND hWnd
//      HDC hDC
//      LPBTNINFO lpB
//      
//
//  Return: (VOID NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/24/92   MSM        Created
//
//*************************************************************

VOID NEAR PASCAL PaintTextButton( HWND hWnd, HDC hDC, LPBTNTEXT lpB)
{
    PTBHDR      pTBHdr = (PTBHDR)GetWindowWord( hWnd, WE_PTBHDR );
    RECT        rc;
    int         nMyDC = TRUE;
    COLORREF    rgbBtnFace = GetSysColor( COLOR_BTNFACE ) ;

    if (!hDC)
        hDC = GetDC( hWnd );
    else
        nMyDC = FALSE;

    GetBtnRect( hWnd, (LPBTNINFO)lpB, &rc);

    SetBkColor( hDC, GetNearestColor( hDC, rgbBtnFace ) );
    SetBkMode( hDC, TRANSPARENT );

    if (lpB->uiBtnStyle & BTNS_TEXTBORDER )
    {
        rc.left--; rc.right++; rc.top--; rc.bottom++;
        tdDraw3DRect( hDC, &rc, 1, DRAW3D_IN );
        rc.left++; rc.right--; rc.top++; rc.bottom--;
    }

    SetBkColor( hDC, GetNearestColor( hDC, rgbBtnFace ) );

    if (!lpB->lpText)
        ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL );
    else
    {
        HFONT   hFont;
        DWORD   dwEX;
        int     nLen, x, y;

        if (lpB->hFont)
            hFont = SelectFont( hDC, lpB->hFont );
        else
            hFont = SelectFont( hDC, GetWindowWord(hWnd, WE_HFONT) );

        nLen = lstrlen( lpB->lpText );

        dwEX = GetTextExtent( hDC, lpB->lpText, nLen );

        y = max( 0, ((rc.bottom-rc.top-(int)HIWORD(dwEX))/2) );

        if ((int)LOWORD(dwEX) >= (rc.right-rc.left))
            x = 1;
        else
        {
            switch (lpB->nJustify)
            {
                case BTNJ_LEFT:
                    x = 3;
                break;

                case BTNJ_CENTER:
                    x = (rc.right-rc.left-LOWORD(dwEX))/2;
                break;

                case BTNJ_RIGHT:
                    x = (rc.right-rc.left-LOWORD(dwEX)) - 3;
                break;
            }
        }

        ExtTextOut( hDC, rc.left+x, rc.top+y, ETO_OPAQUE|ETO_CLIPPED, 
            &rc, lpB->lpText, nLen, NULL );
        SelectFont( hDC, hFont );
    }

    if (nMyDC)
        ReleaseDC( hWnd, hDC );

} //*** PaintTextButton


//*************************************************************
//
//  DrawButtonFrame
//
//  Purpose:
//      Draws a button frame AROUND the RECT
//
//
//  Parameters:
//      HDC hDC
//      LPRECT lprc
//      
//
//  Return: (void NEAR PACAL)
//
//
//  Comments:
//
//           ooooo
//          oRRRRRo
//          oRRRRRo
//          oRRRRRo
//          oRRRRRo
//           ooooo
//
//  History:    Date       Author     Comment
//               4/21/92   MSM        Created
//
//*************************************************************

void NEAR PASCAL DrawButtonFrame( HDC hDC, LPRECT lprc)
{
    MoveTo( hDC, lprc->left, lprc->top-1 );
    LineTo( hDC, lprc->right+1, lprc->top-1 );

    MoveTo( hDC, lprc->right+1, lprc->top );
    LineTo( hDC, lprc->right+1, lprc->bottom+1 );

    MoveTo( hDC, lprc->right, lprc->bottom+1 );
    LineTo( hDC, lprc->left-1, lprc->bottom+1 );

    MoveTo( hDC, lprc->left-1, lprc->bottom );
    LineTo( hDC, lprc->left-1, lprc->top-1 );

} //*** DrawButtonFrame


//*************************************************************
//
//  ButtonNotify
//
//  Purpose:
//      Notifies the parent of a button click
//
//
//  Parameters:
//      HWND hWnd
//      LPBTNINFO lpB
//      
//
//  Return: (VOID NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/22/92   MSM        Created
//
//*************************************************************

VOID NEAR PASCAL ButtonNotify( HWND hWnd, LPBTNINFO lpB )
{
    HWND hParent = GetParent( hWnd );
    UINT uiID = GetWindowWord( hWnd, GWW_ID );

    if (!IsWindow(hParent))
        return;

    //*** Report like a menu option
    if (lpB->uiBtnStyle & BTNS_MENUID)
    {
        LONG lStyle = GetWindowLong( hParent, GWL_STYLE );

        while(IsWindow(hParent))
        {
            if (!(lStyle&WS_CHILD))
            {
                SendMessage(hParent,WM_COMMAND,lpB->uiBtnID,0L);
                return;
            }
            hParent = GetParent( hParent );
            lStyle = GetWindowLong( hParent, GWL_STYLE );
        }
    }
    else
        SendMessage(hParent,WM_COMMAND,uiID,MAKELONG(hWnd,lpB->uiBtnID) );

} //*** ButtonNotify
#endif

//*** EOF: toolbar.c
