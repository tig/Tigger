/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  PREVIEW 
 *
 *      Module:  PREVIEW.C
 * 
 *
 *   Revisions:  
 *       9/11/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "ENGINE.h"

#define SHOW_NON_PRINTABLE_AREA  
#define CLIP_NON_PRINTABLE_AREA  

BOOL WINAPI engPagePaintPreview( LPENGINECONFIG lpEC, HWND hwndClient, HDC hdcClient ) ;

LRESULT CALLBACK fnPreview( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

static char szPreviewClassName[64] ;

BOOL WINAPI RegisterPreview( HINSTANCE hinst, LPSTR lpszClass )
{
    WNDCLASS wc ;
    wc.style             = CS_GLOBALCLASS | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS ;
    wc.lpfnWndProc       = fnPreview ;
    wc.cbClsExtra        = 0 ;
    wc.cbWndExtra        = 4 ;
    wc.hInstance         = hinst ;
    wc.hIcon             = NULL ;
    wc.hCursor           = LoadCursor( NULL, IDC_ARROW ) ;
    wc.hbrBackground     = GetStockObject( LTGRAY_BRUSH )  ;
    wc.lpszMenuName      = NULL ;
    wc.lpszClassName     = lpszClass ;

    lstrcpy( szPreviewClassName, lpszClass ) ;
    
    return RegisterClass (&wc) ;
}

HWND WINAPI engPreviewCreate( HWND hwndParent, DWORD dwFlags, int x, int y, int dx, int dy, LPENGINECONFIG lpEC ) 
{
    char szBuf[MAX_CONFIGNAMELEN] ;
    HWND hwndPreview ;

    wsprintf( szBuf, "Preview - %s", (LPSTR)lpEC->szConfigName ) ;

    hwndPreview =  CreateWindow( szPreviewClassName, 
                                 szBuf,
                                 dwFlags, 
                                 x,y,dx,dy, 
                                 hwndParent, 
                                 NULL, 
                                 ghmodDLL, 
                                 (LPVOID)NULL ) ;

    DASSERT( hWDB,  hwndParent ) ;

    engPreviewSetConfig( hwndPreview, lpEC ) ;
    return hwndPreview ;

}

LPENGINECONFIG WINAPI engPreviewSetConfig( HWND hwndPreview, LPENGINECONFIG lpEC ) 
{
    LPENGINECONFIG  lpECold ;

    if (!IsWindow( hwndPreview ))
        return NULL ;

    if (lpECold = engPreviewGetConfig( hwndPreview ))
    {
        if (lpECold->dwReserved1 != NULL)
        {
            DP5( hWDB, "----------------------------->Destroying Job" ) ;
            engJobDestroy( (LPENGINEJOB)lpECold->dwReserved1 ) ;
            lpECold->dwReserved1 = NULL ;
        }
    }

    return (LPENGINECONFIG)SetWindowLong( hwndPreview, 0, (LONG)lpEC ) ;
}

LPENGINECONFIG WINAPI engPreviewGetConfig( HWND hwndPreview ) 
{
    if (!IsWindow( hwndPreview ))
        return NULL ;

    return (LPENGINECONFIG)GetWindowLong( hwndPreview, 0 ) ;
}

BOOL WINAPI engPreviewDestroy( HWND hwndPreview )
{
    if (!IsWindow( hwndPreview ))
        return FALSE ;

    return DestroyWindow( hwndPreview ) ;
}

VOID WINAPI engPreviewReDraw( HWND hwndPreview, BOOL fReset, BOOL fErase )
{
    LPENGINECONFIG lpEC ;

    if (!IsWindow( hwndPreview ))
        return ;

    if (!(lpEC = engPreviewGetConfig( hwndPreview )))
    {
        return ;
    }

    if (fReset && (lpEC->dwReserved1 != NULL))
    {
        DP5( hWDB, "----------------------------->Destroying Job" ) ;
        engJobDestroy( (LPENGINEJOB)lpEC->dwReserved1 ) ;
        lpEC->dwReserved1 = NULL ;
    }

    if (IsWindowVisible( hwndPreview ))
    {
        InvalidateRect( hwndPreview, NULL, fErase || fReset ) ;
        UpdateWindow( hwndPreview ) ;
    }
}


BOOL WINAPI engPreviewPaintPage( LPENGINECONFIG lpEC, HWND hwndClient, HDC hdcClient )
{
    RECT    rcClient ;
    RECT    rcPage ;                
    RECT    rc ;
    HPEN    hpen ;
    HRGN    hrgn;
    LPENGINEJOB lpJob ;

    VALIDATEPARAM( lpEC ) ;
    VALIDATEPARAM( hwndClient) ;
    VALIDATEPARAM( hdcClient ) ;

    lpJob = (LPENGINEJOB)lpEC->dwReserved1 ;
    if (lpJob == NULL)
    {
        DP1( hWDB, "No job in paintpreveiw" );
        return FALSE ;
    }

    GetClientRect( hwndClient, &rcClient ) ;
//                    DP1( hWDB, "nPageX = %u, nPageY = %u", lpJob->nPageX, lpJob->nPageY ) ;
//                    DP1( hWDB, "rcClient.right = %d, rcClient.bottom = %d", rcClient.right, rcClient.bottom ) ;
    
    /* Provide some space between the edge of the window and
     * the page.
     */
    InflateRect( &rcClient, -10, -10 ) ;

    /* Setup ISOTROPIC mapping mode.
     */
    if ((int)(lpJob->nPageX + 2*lpJob->ptPrintOffset.x) >= (rcClient.right - rcClient.left) &&
        (int)(lpJob->nPageY + 2*lpJob->ptPrintOffset.y) >= (rcClient.bottom - rcClient.top))
    {
        SetMapMode( hdcClient, MM_ISOTROPIC ) ;
        SetWindowExt( hdcClient, lpJob->nPageX + (2*lpJob->ptPrintOffset.x), 
                              lpJob->nPageY + (2*lpJob->ptPrintOffset.y) ) ;
        SetViewportExt( hdcClient, rcClient.right, rcClient.bottom ) ;
    }

    rcPage.left = rcPage.top = 0 ;
    rcPage.right = lpJob->nPageX - 1 ;
    rcPage.bottom = lpJob->nPageY - 1 ;
    LPtoDP( hdcClient, (LPPOINT)&rcPage, 2 ) ;
    SetViewportOrg( hdcClient, (rcClient.right - (rcPage.right - rcPage.left)) / 2 + 5, 
                          (rcClient.bottom - (rcPage.bottom - rcPage.top)) / 2 + 5 ) ;

    /* Make the whole 'sheet' white
     */
    rcPage.left = -lpJob->ptPrintOffset.x ;
    rcPage.top =  -lpJob->ptPrintOffset.y ;
    rcPage.right = lpJob->nPageX - 1 + lpJob->ptPrintOffset.x ;
    rcPage.bottom = lpJob->nPageY - 1 + lpJob->ptPrintOffset.y ;
    FillRect(hdcClient, &rcPage, GetStockObject( WHITE_BRUSH ) ) ;

#ifdef CLIP_NON_PRINTABLE_AREA
    /* Set clipping region so we don't print in the non-printable
     * area.
     */
    rcPage.left = rcPage.top = 0 ;
    rcPage.right = lpJob->nPageX - 1 ;
    rcPage.bottom = lpJob->nPageY - 1 ;
    DP3( hWDB, "Clipping Rect for Non-Printable area = (%d, %d), (%d, %d)",
                rcPage.left, rcPage.top, rcPage.right, rcPage.bottom ) ;
    LPtoDP( hdcClient, (LPPOINT)&rcPage, 2 ) ;
    hrgn = CreateRectRgn( rcPage.left, rcPage.top, rcPage.right, rcPage.bottom ) ;
    SelectClipRgn( hdcClient, hrgn ) ;
    DeleteObject( hrgn ) ;
#endif
      
    engPaintPage( lpJob, hdcClient ) ;

    SelectClipRgn( hdcClient, NULL ) ;

#ifdef SHOW_NON_PRINTABLE_AREA
    /* Dotted line indicates non printable area.
     */
    SetBkMode( hdcClient, TRANSPARENT ) ;
    hpen = SelectObject( hdcClient, CreatePen( PS_DOT, 1, RGBLTGREEN ) ) ;
    SelectObject( hdcClient, GetStockObject( NULL_BRUSH ) ) ;
    Rectangle( hdcClient, -1, -1, lpJob->nPageX, lpJob->nPageY ) ;
    hpen = SelectObject( hdcClient, hpen ) ;
    DeleteObject( hpen ) ;
#endif

    /* Solid rectangle to crisp it up
     */
    SelectObject( hdcClient, GetStockObject( BLACK_PEN ) ) ;
    rcPage.left     = -1 - lpJob->ptPrintOffset.x ;
    rcPage.top      = -1 - lpJob->ptPrintOffset.y ;
    rcPage.right    = lpJob->nPageX + lpJob->ptPrintOffset.x ;
    rcPage.bottom   = lpJob->nPageY + lpJob->ptPrintOffset.y ;

    Rectangle( hdcClient, rcPage.left, rcPage.top, rcPage.right, rcPage.bottom ) ; 

    /* Shadow */
    SelectObject( hdcClient, GetStockObject( NULL_PEN ) ) ;
    SelectObject( hdcClient, GetStockObject( GRAY_BRUSH ) ) ;

    rc = rcPage ;
    LPtoDP( hdcClient, (LPPOINT)&rc, 2 ) ;
    rc.left += 5 ;
    rc.right += 5 ;
    rc.top  = rc.bottom ;
    rc.bottom += 5 ;
    DPtoLP( hdcClient, (LPPOINT)&rc, 2 ) ;
    Rectangle( hdcClient, rc.left, rc.top, rc.right, rc.bottom ) ;

    rc = rcPage ;
    LPtoDP( hdcClient, (LPPOINT)&rc, 2 ) ;
    rc.left = rc.right ;
    rc.right += 5 ;
    rc.top += 5 ;
    rc.bottom += 5 ;
    DPtoLP( hdcClient, (LPPOINT)&rc, 2 ) ;
    Rectangle( hdcClient, rc.left, rc.top, rc.right, rc.bottom ) ;

    return TRUE ;
}


LRESULT CALLBACK fnPreview( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uiMsg )
    {
        case WM_ERASEBKGND:
        {
            HBRUSH hbr ;
            HDC    hdc = GetDC( hWnd ) ;

            if (hbr = (HBRUSH)SendMessage( GetParent( hWnd ), WM_CTLCOLOR,
                              (WPARAM)hdc, MAKELONG( hWnd, CTLCOLOR_DLG )))
            {
                hbr = (HBRUSH)SetClassWord( hWnd, GCW_HBRBACKGROUND, (WORD)hbr ) ;
                DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
                hbr = (HBRUSH)SetClassWord( hWnd, GCW_HBRBACKGROUND, (WORD)hbr ) ;
            }
            else
                DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
    
            ReleaseDC( hWnd, hdc ) ;
        }
        break ;

        case WM_PAINT:
        {
            PAINTSTRUCT     ps ;
            LPENGINECONFIG lpEC = engPreviewGetConfig( hWnd ) ;

            if (!lpEC)
                return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;

            if (BeginPaint( hWnd, &ps ))
            {
                LPENGINEJOB lpJob ;
                
                if (lpEC->dwReserved1 == NULL )
                {
                    DP5( hWDB, "----------------------------->Creating Job" ) ;
                    lpJob = engJobCreate( GetParent( hWnd ), (LPARAM)lpEC, 
                                                   CREATEJOB_BYENGINECONFIG ) ;

                    lpEC->dwReserved1 = (DWORD)lpJob ;
                }
                else
                    engJobReCalc( lpJob = (LPENGINEJOB)lpEC->dwReserved1, lpEC ) ;

                #if 0
                if (lpJob->hdc)
                {
                    DeleteDC( lpJob->hdc ) ;
                    lpJob->hdc = NULL ;
                }
                #endif

                engPreviewPaintPage( lpEC, hWnd, ps.hdc ) ;
                
                EndPaint( hWnd, &ps ) ;
            }
        }
        return 0L ;

        case WM_LBUTTONDBLCLK:
            DP( hWDB, "WM_LBUTTONDBLCLK" ) ;
            SendMessage( GetParent( hWnd ), WM_COMMAND, GetDlgCtrlID( hWnd ), 0L ) ;
        break ;

        case WM_DESTROY:
        {
            LPENGINECONFIG lpEC ;

            if (lpEC = engPreviewGetConfig( hWnd ) )
            {
                if (lpEC->dwReserved1 != NULL)
                {
                    DP5( hWDB, "----------------------------->Destroying Job" ) ;
                    engJobDestroy( (LPENGINEJOB)lpEC->dwReserved1 ) ;
                    lpEC->dwReserved1 = NULL ;
                }
            }
        }
        break ;

        default:
            return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
        break ;
    }

    return 0L ;
}

/************************************************************************
 * End of File: PREVIEW.C
 ***********************************************************************/

