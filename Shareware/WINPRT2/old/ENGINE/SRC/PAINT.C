/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  PAINT 
 *
 *      Module:  PAINT.C
 * 
 *
 *   Revisions:  
 *       9/11/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "ENGINE.h"

BOOL INTERNAL DrawBorder( HDC hdc, LPRECT lprc, DWORD dw, BOOL fPreview )  ;
VOID INTERNAL GetExtent( HDC hdc, LPSTR lpsz, UINT cb, LPSIZE lpSize ) ;

BOOL WINAPI engPaintPage( LPENGINEJOB lpJob, HDC hdc )
{
    LPENGINECONFIG lpEC = lpJob->lpEC ;
    RECT    rc ;
    UINT    cy ;

    VALIDATEPARAM( lpJob ) ;
    VALIDATEPARAM( lpEC ) ;

    if (hdc == NULL)
        hdc = lpJob->hdc ;

    engPaintFrame( lpJob, hdc ) ;

    /* Paint header */

    cy = MulDiv( (UINT)lpEC->Header.uiHeight, lpJob->nPixelPer10CMY, 10000 ) ;
    rc.left = lpJob->rcPhysWorkspace.left - 1 ;
    rc.right = lpJob->rcPhysWorkspace.left + lpJob->rcPhysWorkspace.right ;
    rc.bottom = lpJob->rcPhysWorkspace.top - 1 ;
    rc.top = rc.bottom - cy ;
    engPaintHF( lpJob, hdc, &lpEC->Header, &rc ) ;

    /* Paint Footer */
    cy = MulDiv( (UINT)lpEC->Footer.uiHeight, lpJob->nPixelPer10CMY, 10000 ) ;
    rc.top = lpJob->rcPhysWorkspace.top + lpJob->rcPhysWorkspace.bottom - 2 ;
    rc.bottom = rc.top + cy ;
    engPaintHF( lpJob, hdc, &lpEC->Footer, &rc ) ;

    return TRUE ;
}

BOOL WINAPI engPaintFrame( LPENGINEJOB lpJob, HDC hdc ) 
{
    LPENGINECONFIG lpEC ;
    RECT    rc ;
    RECT    rcClip ;
    HBRUSH  hbr ;
    HRGN    hrgn ;
    UINT row, col ;
    RECT rc2 ;
    UINT uiX, uiY ;
    UINT uiHorzSep, uiVertSep ;
    BOOL fPreview = FALSE ;

    VALIDATEPARAM( lpJob ) ;

    lpEC = lpJob->lpEC ;
    
    if (hdc == NULL)
        hdc = lpJob->hdc ;
    else
        if (hdc != lpJob->hdc)
            fPreview = TRUE ;

#ifndef SAVERESTOREDC
    GetClipBox( hdc, &rcClip ) ;
#else
    SaveDC( hdc ) ;
#endif 
    
    SelectObject( hdc, GetStockObject( NULL_BRUSH ) ) ;


    /* If there are mulitple pages up then we need to draw the
     * horz and vert separators.
     */
    rc2 = lpJob->rcPhysWorkspace ;
    uiX = lpJob->uiSubPageX ;
    uiY = lpJob->uiSubPageY ;
    uiVertSep = MulDiv( (UINT)lpEC->uiVertSep, lpJob->nPixelPer10CMX, 10000 ) ;
    uiHorzSep = MulDiv( (UINT)lpEC->uiHorzSep, lpJob->nPixelPer10CMY, 10000 ) ;

    if (lpEC->dwBorderFlags)
        SelectObject( hdc, GetStockObject( BLACK_PEN ) ) ;

    for (row = 0 ; row < lpEC->uiPageRows ; row++ )
    {
        for (col = 0 ; col < lpEC->uiPageColumns ; col++ )
        {
#ifdef ORIG            
            rc.left = rc2.left + (col * (uiX + uiVertSep)) ;
            rc.right = rc.left + uiX - 1 ;

            rc.top = rc2.top + (row * (uiY + uiHorzSep)) ;
            rc.bottom = rc.top + uiY - 1 ;
#endif

            rc.left = rc2.left + COL_POS(col, lpJob) + (col * uiVertSep) ;
            rc.right = rc.left + uiX - 1 ;

            rc.top = rc2.top + ROW_POS(row, lpJob) + (row * uiHorzSep) ;
            rc.bottom = rc.top + uiY - 1 ;

            InflateRect( &rc, 1, 1 ) ;
            DP5( hWDB, "Drawing Border: (%d, %d), (%d, %d)",
                        rc.left, rc.top,
                        rc.right, rc.bottom ) ;
            DrawBorder( hdc, &rc, lpEC->dwBorderFlags, fPreview ) ;

            if (lpEC->dwShadeFlags)
                ExcludeClipRect( hdc, rc.left, rc.top, rc.right+1, rc.bottom+1 ) ;
        }
    }

    /* If shading is on, then shade...
     */
    if (lpEC->dwShadeFlags)
    {
        if (lpEC->dwShadeFlags & ENG_SHADE_LTGRAY)
            hbr = GetStockObject( LTGRAY_BRUSH ) ;
        else
            hbr = GetStockObject( GRAY_BRUSH ) ;

        rc.left = rc.top = 0 ;
        rc.right = lpJob->nPageX ;
        rc.bottom = lpJob->nPageY ;
        FillRect( hdc, &rc, hbr ) ;
    }

#ifndef SAVERESTOREDC
    LPtoDP( hdc, (LPPOINT)&rcClip, 2 ) ;
    hrgn = CreateRectRgn( rcClip.left, rcClip.top, rcClip.right, rcClip.bottom ) ;
    SelectClipRgn( hdc, hrgn ) ;
    DeleteObject( hrgn ) ;
#else
    RestoreDC( hdc, -1 ) ;
#endif    
    return TRUE ;

} 

BOOL WINAPI engPaintHF( LPENGINEJOB lpJob, HDC hdc, LPENGINEHF lpehf, LPRECT lprcHF) 
{
    ENGINEHF        ehf ;
    LPENGINECONFIG  lpEC ;
    HBRUSH          hbr ;
    BOOL            fPreview = FALSE ;
    RECT            rcClip ;
    HRGN            hrgn ;
    UINT            cy ;

    RECT            rcHF ;
    RECT            rcLeft ;
    RECT            rcCenter ;
    RECT            rcRight ;
    LPSTR           lpszLeft = NULL ;
    LPSTR           lpszCenter = NULL ;
    LPSTR           lpszRight = NULL ;                   
    UINT            nLeft ;
    UINT            nCenter ;
    UINT            nRight ;
    HFONT           hfontLeft ;
    HFONT           hfontCenter ;
    HFONT           hfontRight ;

    SIZE            size ;

    VALIDATEPARAM( lpJob ) ;

    lpEC = lpJob->lpEC ;

    VALIDATEPARAM( lpEC ) ;
    
    ehf = *lpehf ;
    rcHF = *lprcHF ;
    
    if (hdc == NULL)
        hdc = lpJob->hdc ;
    else
        if (hdc != lpJob->hdc)
            fPreview = TRUE ;

#ifndef SAVERESTOREDC
    GetClipBox( hdc, &rcClip ) ;
#else
    SaveDC( hdc ) ;
#endif 

    hbr = SelectObject( hdc, GetStockObject( NULL_BRUSH ) ) ;

    cy = rcHF.bottom - rcHF.top ;

    /* Use the shade flags from the ENGINECONFIG.  Someday we may
     * use the shade flags in the EINGINEHF structure...
     */
    if (lpEC->dwShadeFlags)
    {
        if (lpEC->dwShadeFlags & ENG_SHADE_LTGRAY)
            hbr = GetStockObject( LTGRAY_BRUSH ) ;
        else
            hbr = GetStockObject( GRAY_BRUSH ) ;
        FillRect( hdc, &rcHF, hbr ) ;
    }

    /* Put down the text...
     *
     * Expand the format strings.  Calculate their horizontal extents (and
     * coordinates) and save them off.
     */
    lpszLeft = engHFExpand( lpJob, ehf.Left.szText ) ;
    lpszCenter = engHFExpand( lpJob, ehf.Center.szText ) ;
    lpszRight = engHFExpand( lpJob, ehf.Right.szText ) ;

    hfontLeft = SelectObject( hdc, ReallyCreateFontEx( hdc, ehf.Left.szFace, ehf.Left.szStyle, ehf.Left.wPoints, 0 ) ) ;

    if (lpszLeft)
    {
        GetExtent( hdc, lpszLeft, nLeft = lstrlen(lpszLeft), &size ) ;
    }
    else
    {
        size.cx = 0 ;
        size.cy = 0 ;
    }
    rcLeft.left = rcHF.left + 1 ;
    rcLeft.right = rcLeft.left + size.cx ;
    rcLeft.top = rcHF.top + (cy - size.cy) / 2 ;    
    rcLeft.bottom = rcLeft.top + size.cy ;
    hfontLeft = SelectObject( hdc, hfontLeft ) ;

    hfontCenter = SelectObject( hdc, ReallyCreateFontEx( hdc, ehf.Center.szFace, ehf.Center.szStyle, ehf.Center.wPoints, 0 ) ) ;
    if (lpszCenter)
        GetExtent( hdc, lpszCenter, nCenter = lstrlen(lpszCenter), &size ) ;
    else
        size.cx = 0 ;
    rcCenter.left = rcHF.left + (rcHF.right - rcHF.left - size.cx) / 2 ;
    rcCenter.right = rcCenter.left + size.cx ;
    rcCenter.top = rcHF.top + (cy - size.cy) / 2 ;    
    rcCenter.bottom = rcCenter.top + size.cy ;
    hfontCenter = SelectObject( hdc, hfontCenter ) ;

    hfontRight = SelectObject( hdc, ReallyCreateFontEx( hdc, ehf.Right.szFace, ehf.Right.szStyle, ehf.Right.wPoints, 0 ) ) ;
    if (lpszRight)
        GetExtent( hdc, lpszRight, nRight = lstrlen(lpszRight), &size ) ;
    else
        size.cx = 0 ;
    rcRight.left = rcHF.right - size.cx - 1 ;
    rcRight.right = rcRight.left + size.cx ;
    rcRight.top = rcHF.top + (cy - size.cy) / 2 ;    
    rcRight.bottom = rcRight.top + size.cy ;
    hfontRight = SelectObject( hdc, hfontRight ) ;

    /* Figure out clipping.  If Left section extends over Center (or right)
     * section's text, clip it.  Otherwise let it flow.
     */
    #pragma message( "Implement clipping of header/footer sections" )

    SetBkMode( hdc, TRANSPARENT ) ;
    /* Output the Left text, centered vertically.
     */
    hfontLeft = SelectObject( hdc, hfontLeft ) ;
    DrawText( hdc, lpszLeft, nLeft, &rcLeft, DT_LEFT | DT_VCENTER | DT_EXTERNALLEADING | DT_NOPREFIX ) ;
    DeleteObject( SelectObject( hdc, hfontLeft ) ) ;
     
    /* If Center section extends over right, clip it.  Otherwise let
     * it flow.
     */

    /* Output the center text.
     */
    hfontCenter = SelectObject( hdc, hfontCenter ) ;
    DrawText( hdc, lpszCenter, nCenter, &rcCenter, DT_CENTER | DT_VCENTER | DT_EXTERNALLEADING | DT_NOPREFIX ) ;
    DeleteObject( SelectObject( hdc, hfontCenter ) ) ;

    /* Output the right text.
     */
    hfontRight = SelectObject( hdc, hfontRight ) ;
    DrawText( hdc, lpszRight, nRight, &rcRight, DT_RIGHT | DT_VCENTER | DT_EXTERNALLEADING | DT_NOPREFIX ) ;
    DeleteObject( SelectObject( hdc, hfontRight ) ) ;

    if (lpszLeft)
        GlobalFreePtr( lpszLeft ) ;

    if (lpszCenter)
        GlobalFreePtr( lpszCenter ) ;

    if (lpszRight)
        GlobalFreePtr( lpszRight ) ;

    DrawBorder( hdc, &rcHF, ehf.dwBorderFlags, FALSE /*fPreview*/ ) ;

    SelectObject( hdc, hbr ) ;

#ifndef SAVERESTOREDC
    LPtoDP( hdc, (LPPOINT)&rcClip, 2 ) ;
    hrgn = CreateRectRgn( rcClip.left, rcClip.top, rcClip.right, rcClip.bottom ) ;
    SelectClipRgn( hdc, hrgn ) ;
    DeleteObject( hrgn ) ;
#else
    RestoreDC( hdc, -1 ) ;
#endif    

    return TRUE ;
}    

VOID INTERNAL GetExtent( HDC hdc, LPSTR lpsz, UINT cb, LPSIZE lpSize )
{
    LPSTR   p, pcur ;
    UINT    cbCur ;
    UINT    uiLen = 0 ;
    UINT    uiHt = 0 ;

    lpSize->cx = lpSize->cy = 0 ;

    /* For each line find the length.
     */
    for (cbCur = 0, pcur = p = lpsz; *p ; p++ )
    {
        cbCur++ ;
        if (*p == '\n')
        {
            *p = '\0' ; // HACK!
            GetTextExtentPoint( hdc, pcur, cbCur, lpSize ) ;
            uiLen = max( (int)uiLen, lpSize->cx ) ;
            uiHt += lpSize->cy ;
            cbCur = 0 ;
            pcur = p+1 ;
            *p = '\n' ; // HACK!
        }
    }

    if (pcur != '\0' && cbCur)
    {
        GetTextExtentPoint( hdc, pcur, cbCur, lpSize ) ;
        uiLen = max( (int)uiLen, lpSize->cx ) ;
        uiHt += lpSize->cy ;
    }

    lpSize->cx = uiLen ;
    lpSize->cy = uiHt ;
}

BOOL INTERNAL DrawBorder( HDC hdc, LPRECT lprc, DWORD dw, BOOL fPreview ) 
{
    HPEN    hpen ;

    if (fPreview)
        hpen = CreatePen( PS_DASH, 1, RGBLTCYAN ) ;

    if (dw & ENG_TOPBORDER)
    {
        MoveToEx(   hdc, lprc->left,    lprc->top, NULL ) ;
        LineTo(     hdc, lprc->right, lprc->top  ) ;
    }
    else if (fPreview)
    {
        hpen = SelectObject( hdc, hpen ) ;
        MoveToEx(   hdc, lprc->left,    lprc->top, NULL ) ;
        LineTo(     hdc, lprc->right, lprc->top  ) ;
        hpen = SelectObject( hdc, hpen ) ;
    }

    if (dw & ENG_LEFTBORDER)
    {
        MoveToEx(   hdc, lprc->left,    lprc->top, NULL ) ;
        LineTo(     hdc, lprc->left,    lprc->bottom  ) ;
    }
    else if (fPreview)
    {
        hpen = SelectObject( hdc, hpen ) ;
        MoveToEx(   hdc, lprc->left,    lprc->top, NULL ) ;
        LineTo(     hdc, lprc->left,    lprc->bottom  ) ;
        hpen = SelectObject( hdc, hpen ) ;
    }

    if (dw & ENG_BOTTOMBORDER)
    {
        MoveToEx(   hdc, lprc->left,    lprc->bottom, NULL ) ;
        LineTo(     hdc, lprc->right, lprc->bottom ) ;
    } else if (fPreview)
    {
        hpen = SelectObject( hdc, hpen ) ;
        MoveToEx(   hdc, lprc->left,    lprc->bottom, NULL ) ;
        LineTo(     hdc, lprc->right, lprc->bottom ) ;
        hpen = SelectObject( hdc, hpen ) ;
    }

    if (dw & ENG_RIGHTBORDER)
    {
        MoveToEx(   hdc, lprc->right, lprc->top, NULL ) ;
        LineTo(     hdc, lprc->right, lprc->bottom ) ;
    }
    else if (fPreview)
    {
        hpen = SelectObject( hdc, hpen ) ;
        MoveToEx(   hdc, lprc->right, lprc->top, NULL ) ;
        LineTo(     hdc, lprc->right, lprc->bottom ) ;
        hpen = SelectObject( hdc, hpen ) ;
    }

    if (fPreview)
        DeleteObject( hpen ) ;

    return TRUE ;
}
/************************************************************************
 * End of File: PAINT.C
 ***********************************************************************/

