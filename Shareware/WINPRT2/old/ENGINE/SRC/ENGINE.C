/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  ENGINE 
 *
 *      Module:  ENGINE.c
 * 
 *      This module contians the main entry points for the WinPrint 2.0
 *      print engine.  These entry points have to do with the basic
 *      print engine.  
 *
 *   Revisions:  
 *       9/3/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"

#define DEFINE_GLOBALS
#include "ENGINE.h"

UINT WINAPI engGetMeasureUnit( VOID )
{
    return uiMeasureUnit ;
}

DWORD WINAPI engGetLastError( VOID )
{
    return dwLastError ;
}    

LPENGINEJOB WINAPI engJobCreate( HWND hWnd, LPARAM lParam, DWORD dwFlags )
{
    LPENGINECONFIG lpEC = NULL ;
    LPENGINEJOB lpJob = NULL ;
    HCURSOR     hcur ;
    UINT        n ;

    VALIDATEPARAM( lParam ) ;

    hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

    switch (LOWORD( dwFlags ))
    {
        case CREATEJOB_BYENGINECONFIG:
            DP5( hWDB, "CREATEJOB_BYENGINECONFIG" ) ;
            lpEC = (LPENGINECONFIG)lParam ;
        break ;

        case CREATEJOB_BYCONFIGNAME:
        default:
            DP5( hWDB, "CREATEJOB_BYCONFIGNAME" ) ;

            if (!(lpEC = GlobalAllocPtr( GHND, sizeof (ENGINECONFIG) )))
                FAIL( ENGERR_GLOBALALLOC ) ;

            if (!engConfigRead( hWnd, (LPSTR)lParam, NULL, lpEC ))
            {
                DP1( hWDB, "engConfigRead Failed : Err = %d", engGetLastError() ) ;
                // don't use FAIL() because it will reset the error
                goto Fail ;
            }
        break ;
    }

    if (!(lpJob = GlobalAllocPtr( GHND, sizeof (ENGINEJOB) )))
        FAIL( ENGERR_GLOBALALLOC ) ;

    lpJob->uiStructSize = sizeof(ENGINEJOB) ;
    lpJob->dwFlags = dwFlags ;
    lpJob->lpEC = lpEC ;
    lpJob->hwndParent = hWnd ;
    lpEC->dwReserved1 = (DWORD)lpJob ;

    lpJob->hdc = engPrinterCreateDC( hWnd, lpEC ) ;

    /* Figure out the page dimensions and so forth... */

    /* Determine resolution */
    lpJob->nPageX = GetDeviceCaps( lpJob->hdc, HORZRES ) ;

    if (lstrcmpi( "PSCRIPT", lpEC->szDriver ) == 0)
        lpJob->nPageX-- ;

    lpJob->nPageY = GetDeviceCaps( lpJob->hdc, VERTRES ) ;
    DP3( hWDB, "Resolution = (%d, %d)", lpJob->nPageX, lpJob->nPageY ) ;

    /* Store pixels per 10CM (not 100ths of MM because of rounding).
     */
    DP3( hWDB, "LOGPIXELSX = %d, LOGPIXELSY = %d", 
        GetDeviceCaps( lpJob->hdc, LOGPIXELSX ), GetDeviceCaps( lpJob->hdc, LOGPIXELSY ) ) ;
    lpJob->nPixelPer10CMX = MulDiv( 1000, GetDeviceCaps( lpJob->hdc, LOGPIXELSX ), 254 ) ;
    lpJob->nPixelPer10CMY = MulDiv( 1000, GetDeviceCaps( lpJob->hdc, LOGPIXELSY ), 254 ) ;

    DP3( hWDB, "Pixels Per 10 CM = (%d, %d)", lpJob->nPixelPer10CMX, lpJob->nPixelPer10CMY ) ;

    /* Determine printable area */
    n = GETPRINTINGOFFSET ;
    if (!(fUsePrintOffset &&
          Escape( lpJob->hdc, QUERYESCSUPPORT, sizeof(int), (LPSTR)(LPINT)&n, NULL ) &&
          Escape( lpJob->hdc, GETPRINTINGOFFSET, NULL, NULL, &lpJob->ptPrintOffset )))
    {
        lpJob->ptPrintOffset.x = 0 ;
        lpJob->ptPrintOffset.y = 0 ;
    }

    DP3( hWDB, "PrintOffset = (%d, %d)", lpJob->ptPrintOffset.x, lpJob->ptPrintOffset.y ) ;

    engJobReCalc( lpJob, lpEC ) ;

    PutJobInSleazyTable( lpJob ) ;
              
    SetCursor( hcur ) ;

    return lpJob ;

Fail:
    if (lpEC && LOWORD( dwFlags ) == CREATEJOB_BYCONFIGNAME)
    {
        engConfigFree( lpEC ) ;
    }

    if (lpJob)
    {
        if (lpJob->hdc)
            DeleteDC( lpJob->hdc ) ;

        GlobalFreePtr( lpJob ) ;
    }

    SetCursor( hcur ) ;

    return NULL ;
}

BOOL WINAPI engJobReCalc( LPENGINEJOB lpJob, LPENGINECONFIG lpEC )
{
    HCURSOR hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

    /* Determine header height */
    lpEC->Header.uiHeight = engHFCalcHeight( lpJob, &lpEC->Header ) ;
    
    /* Determine footer height */
    lpEC->Footer.uiHeight = engHFCalcHeight( lpJob, &lpEC->Footer ) ;
    
    /* The workspace is defined by the margins (margins are in 100ths of mm)
     * Nothing can be printed outside of the margines (i.e. workspace) except
     * headers and footers.
     */
    lpJob->rcPhysWorkspace.left = MulDiv( lpEC->uiLeftMargin, lpJob->nPixelPer10CMX, 10000 ) - 
                                  lpJob->ptPrintOffset.x ;

    if (lpEC->dwBorderFlags & ENG_LEFTBORDER || 
        lpEC->Header.dwBorderFlags & ENG_LEFTBORDER ||
        lpEC->Footer.dwBorderFlags & ENG_LEFTBORDER )
        lpJob->rcPhysWorkspace.left++ ;

    lpJob->rcPhysWorkspace.right = (lpJob->nPageX - 
                                    lpJob->rcPhysWorkspace.left - 
                                    MulDiv( lpEC->uiRightMargin, lpJob->nPixelPer10CMX, 10000 )) + 
                                   lpJob->ptPrintOffset.x  ;

    if (lpEC->dwBorderFlags & ENG_RIGHTBORDER || 
        lpEC->Header.dwBorderFlags & ENG_RIGHTBORDER ||
        lpEC->Footer.dwBorderFlags & ENG_RIGHTBORDER )
        lpJob->rcPhysWorkspace.right-- ;
    
    lpJob->rcPhysWorkspace.top = MulDiv( lpEC->uiTopMargin + lpEC->Header.uiHeight,
                                         lpJob->nPixelPer10CMY, 10000 ) -
                                 lpJob->ptPrintOffset.y ;
    if (lpEC->dwBorderFlags & ENG_TOPBORDER || 
        lpEC->Header.dwBorderFlags & ENG_TOPBORDER ||
        lpEC->Footer.dwBorderFlags & ENG_TOPBORDER )
        lpJob->rcPhysWorkspace.top++ ;

    lpJob->rcPhysWorkspace.bottom = (lpJob->nPageY -
                                     lpJob->rcPhysWorkspace.top -
                                     MulDiv( lpEC->uiBottomMargin + lpEC->Footer.uiHeight,
                                            lpJob->nPixelPer10CMY, 10000 )) + 
                                     lpJob->ptPrintOffset.y ;
    if (lpEC->dwBorderFlags & ENG_BOTTOMBORDER || 
        lpEC->Header.dwBorderFlags & ENG_BOTTOMBORDER ||
        lpEC->Footer.dwBorderFlags & ENG_BOTTOMBORDER )
        lpJob->rcPhysWorkspace.bottom-- ;

    DP3( hWDB, "Workspace = (%d, %d, %d, %d)  (%d,%d), (%d,%d)",
                lpJob->rcPhysWorkspace.left,
                lpJob->rcPhysWorkspace.top,
                lpJob->rcPhysWorkspace.right,
                lpJob->rcPhysWorkspace.bottom,
                lpJob->rcPhysWorkspace.left,
                lpJob->rcPhysWorkspace.top,
                lpJob->rcPhysWorkspace.left + lpJob->rcPhysWorkspace.right - 1,
                lpJob->rcPhysWorkspace.top + lpJob->rcPhysWorkspace.bottom - 1 ) ;
                                
    /* Determine sub page size */ 
    lpEC->uiPageColumns = max( 1, lpEC->uiPageColumns ) ;
    lpJob->uiSubPageX = MulDiv(1, lpJob->rcPhysWorkspace.right -
                               (MulDiv( lpEC->uiVertSep, 
                                        lpJob->nPixelPer10CMY, 10000 ) * 
                                 (lpEC->uiPageColumns-1)),
                              lpEC->uiPageColumns ) ;

    lpEC->uiPageRows = max( 1, lpEC->uiPageRows ) ;
    lpJob->uiSubPageY = MulDiv(1, lpJob->rcPhysWorkspace.bottom - 
                                    (MulDiv( lpEC->uiHorzSep, 
                                             lpJob->nPixelPer10CMX, 10000 ) * 
                                    (lpEC->uiPageRows-1)),
                               lpEC->uiPageRows ) ;

    DP3( hWDB, "SubPage   = (%d, %d)", lpJob->uiSubPageX, lpJob->uiSubPageY ) ;

    SetCursor( hcur ) ;

    return TRUE ;

}

BOOL WINAPI engJobDestroy( LPENGINEJOB lpJob )
{
    HCURSOR hcur ;

    VALIDATEPARAM( lpJob ) ;

    hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

    RemoveJobFromSleazyTable( lpJob ) ;

    /* Delete the DC
     */
    if (lpJob->hdc != NULL)
        DeleteDC( lpJob->hdc ) ;

    /* Free memory */
    if (lpJob->lpEC && LOWORD( lpJob->dwFlags ) == CREATEJOB_BYCONFIGNAME)
    {
        engConfigFree( lpJob->lpEC ) ;
    }

    GlobalFreePtr( lpJob ) ;

    SetCursor( hcur ) ;

    return TRUE ;
}    

BOOL WINAPI engJobReset( LPENGINEJOB lpJob, LPENGINECONFIG lpEC )
{
    LPDEVMODE lpdm ;
    HDC  hdc ;
    HCURSOR hcur ;
    
    if (lpEC == NULL)
        lpEC = lpJob->lpEC ;

    VALIDATEPARAM( lpEC ) ;

    hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

    if (!(lpdm = engPrinterGetDevMode( lpJob->hwndParent, lpJob->lpEC )))
    {
        SetCursor( hcur ) ;
        DP1( hWDB, "(GetSetOrient) engPrinterGetDevMode failed!!" ) ;
        return NULL ;
    }

    lpdm->dmFields           = lpEC->dmFields        ;
    lpdm->dmOrientation      = lpEC->dmOrientation ;
    lpdm->dmPaperSize        = lpEC->dmPaperSize     ;
    lpdm->dmPaperLength      = lpEC->dmPaperLength   ;
    lpdm->dmPaperWidth       = lpEC->dmPaperWidth    ;
    lpdm->dmScale            = lpEC->dmScale         ;
    lpdm->dmCopies           = lpEC->dmCopies        ;
    lpdm->dmDefaultSource    = lpEC->dmDefaultSource ;
    lpdm->dmPrintQuality     = lpEC->dmPrintQuality  ;
    lpdm->dmColor            = lpEC->dmColor         ;
    lpdm->dmDuplex           = lpEC->dmDuplex        ;

    hdc = ResetDC( lpJob->hdc, lpdm ) ;

    GlobalFreePtr( lpdm ) ;
    SetCursor( hcur ) ;

    return TRUE ;
    
}



/************************************************************************
 * End of File: ENGINE.c
 ***********************************************************************/

