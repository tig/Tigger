//*************************************************************
//  File name: fontview.c
//
//  Description: 
//      Code for the font view control
//
//  History:    Date       Author     Comment
//               3/ 3/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include "global.h"
#include "panose.h"


//*************************************************************
//
//  FontViewWndProc
//
//  Purpose:
//      Window procedure for the FontView control
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
//               3/ 5/92   MSM        Created
//
//*************************************************************

LONG FAR PASCAL FVWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PFVQ pFVQ = GETPFVQ( hWnd );

    switch (msg)
    {
        case WM_CREATE:
            pFVQ = (PFVQ)LocalAlloc( LPTR, sizeof( FVQ ) );
            if (!pFVQ)
                return -1L;
            SETPFVQ( hWnd, pFVQ );
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            FVPaintBitmap( hWnd );
        }
        break;

        case WM_DESTROY:
            if (pFVQ)
                LocalFree( (HLOCAL)pFVQ );
        break;
    }
    return DefWindowProc( hWnd, msg, wParam, lParam );

} //*** FontViewWndProc


//*************************************************************
//
//  FVSetCurrentNode
//
//  Purpose:
//      Sets the current node into the view
//
//
//  Parameters:
//      HWND hWnd
//      LPDBNODE lpdb
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

BOOL WINAPI FVSetCurrentNode(HWND hWnd, LPDBNODE lpdb)
{
    PFVQ pFVQ = GETPFVQ( hWnd );

    if (!pFVQ)
        return FALSE;

    pFVQ->lpdb = lpdb;

    if (lpdb && lpdb->wNodeType==NT_STYLE)
    {
        FVAddFaceToQ( pFVQ, (LPFACE)lpdb->lpParent );

        //*** Check for panose sorting
        if (IsWindow(ghwndPanoseDlg))
        {
            gPanose = ((LPSTYLE)lpdb)->Info.Panose;
            FillPanoseDialog( ghwndPanoseDlg );
        }
    }
    else
    {
        if (lpdb && lpdb->wNodeType==NT_FACE)
            FVAddFaceToQ( pFVQ, (LPFACE)lpdb );
        else
            FVAddFaceToQ( pFVQ, NULL );
    }

    FVSetStatusToNode( lpdb );
    InvalidateRect( hWnd, NULL, FALSE );


} //*** FVSetCurrentNode


//*************************************************************
//
//  FVAddFaceToQ
//
//  Purpose:
//      Adds the face to the Q
//
//
//  Parameters:
//      PFVQ pFVQ
//      LPFACE lpFace
//      
//
//  Return: (VOID NEAR PASCAL)
//
//
//  Comments:
//      A NULL face is valid and simply means NO STYLE BMP
//
//
//  History:    Date       Author     Comment
//               3/ 5/92   MSM        Created
//
//*************************************************************

VOID NEAR PASCAL FVAddFaceToQ( PFVQ pFVQ, LPFACE lpFace )
{
    LPFACE *plpF, lpT;
    int  i;

    plpF = pFVQ->alpFaces;

    //*** If this node is already the current node then just return
    if ( (lpT = *plpF) == lpFace )
        return;

    //*** Make the current node this one
    *plpF = lpFace;
    plpF++;

    //*** Move down the list, dropping all faces by one
    //*** looking for a NULL or another occurance of lpFace
    //*** If either is found, we can stop there.
    //*** Otherwise go to the end of the list and bump the
    //*** node at the bottom off.

    for (i=1; i<FV_BMPQSIZE; i++)
    {
        if (*plpF == lpFace || *plpF==NULL)
        {
            *plpF = lpT;
            return;
        }
        else
        {
            // Move node down and store next node in lpT
            LPFACE lpTMP = *plpF;
            *plpF = lpT;
            lpT = lpTMP;
        }
        plpF++;
    }
    DP4( hWDB, "FontViewQ is purging a bitmap face (%#08lx)", lpT );
    FVFreeBitmap( lpT );

} //*** FVAddFaceToQ


//*************************************************************
//
//  FVFreeQ
//
//  Purpose:
//      Frees up the Bitmaps in the Q
//
//
//  Parameters:
//      HWND hWnd
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 5/92   MSM        Created
//
//*************************************************************

VOID WINAPI FVFreeQ(HWND hWnd)
{
    PFVQ   pFVQ = GETPFVQ( hWnd );
    LPFACE *plpF;
    int  i;

    if (!pFVQ)
        return;

    plpF = pFVQ->alpFaces;

    for (i==0; i<FV_BMPQSIZE; i++)
    {
        if (*plpF)
        {
            FVFreeBitmap( *plpF );
            *plpF = NULL;
        }
        plpF++;
    }

} //*** FVFreeQ


//*************************************************************
//
//  FVPaintBitmap
//
//  Purpose:
//      Paints the bitmap into the window
//
//
//  Parameters:
//      HWND hWnd
//      
//
//  Return: (BOOL NEAR PASCAL)
//
//
//  Comments:
//      3 things can happen:
//          1) Bitmap is loaded, present and painted
//          2) Bitmap is present but not loaded and a BMP PRESENT msg is dis.
//          2) Bitmap is not present and a BMP NOT PRESENT msg is displayed
//
//
//
//  History:    Date       Author     Comment
//               3/ 5/92   MSM        Created
//
//*************************************************************

VOID NEAR PASCAL FVPaintBitmap( HWND hWnd )
{
    HDC     hDC;
    PFVQ    pFVQ = GETPFVQ( hWnd );
    LPFACE  lpFace;
    RECT    rc;
    HBITMAP hBmp;
    LPSTR   lp = "";
    BOOL    fLargeThumb=FALSE;
    DWORD   dw;
    int     h, w;
    LPSTYLE lpS = (LPSTYLE)pFVQ->lpdb;
    WORD    wPurge=FALSE;

    GetClientRect( hWnd, &rc );
    hDC = GetDC( hWnd );

// BUGFIX #0020 5/27/92 (cek)
    if (!pFVQ || !pFVQ->lpdb)
        goto out_text;

   if (pFVQ->lpdb->wNodeType==NT_FAMILY || pFVQ->lpdb->wNodeType==NT_PUB)
   {
      /* Draw the "FONT CONNECTION" bitmap */
      HDC      hMDC = CreateCompatibleDC( hDC );
      BITMAP   bm ;

      if (ghbmpFontCon)
      {
         GetObject( ghbmpFontCon, sizeof(BITMAP), &bm ) ;

         w = max( 0, (rc.right-(int)bm.bmWidth)/2);
         h = max( 0, (rc.bottom-(int)bm.bmHeight)/2);

         //*** Erase background
         ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL );

         //*** Paint the portion of the bitmap into the window
         ghbmpFontCon = SelectObject( hMDC, ghbmpFontCon );
         BitBlt(hDC,w,h,bm.bmWidth,bm.bmHeight,hMDC,0,0, SRCCOPY );
         ghbmpFontCon = SelectObject( hMDC, ghbmpFontCon );
         DeleteDC( hMDC );

         ReleaseDC( hWnd, hDC ) ;
      }
      else
         goto out_text ;

      return ;
   }

// BUGFIX #0020 5/27/92 (cek)

    lpFace = pFVQ->alpFaces[0];

load_bmp:
    //*** If we have a face, then we draw the default font
    if (pFVQ->lpdb->wNodeType==NT_FACE)
    {
        LPSTYLE lpS2;

        hBmp = FVGetSmallThumb( (LPFACE)pFVQ->lpdb );
        lpS = lpS2  = GetFirstStyle( (LPFACE)pFVQ->lpdb, NULL );

        while (lpS2)
        {
            lpS = lpS2;
            lpS2 = GetNextStyle( lpS2, NULL );
        }
    }
    else
        hBmp = FVGetSmallThumb( lpFace );

    // Do we have a bitmap to draw
    if (!hBmp)
    {
        if (VI_ISBMPFOUND(lpFace))
        {
            if (!wPurge)
            {
                FVFreeQ( hWnd );
                wPurge=TRUE;
                goto load_bmp;
            }
            lp = GRCS( IDS_NOBMPMEMORY );
        }
        else
            lp = GRCS( IDS_NOBMPPRESENT );

out_text:
        dw = GetTextExtent( hDC, lp, lstrlen(lp) );
        w = max( 0, (rc.right-(int)LOWORD(dw))/2);
        h = max( 0, (rc.bottom-(int)HIWORD(dw))/2);

        ExtTextOut( hDC, w, h, ETO_OPAQUE, &rc, lp, lstrlen(lp), NULL );
    }
    else // We have the bitmap
    {
        int y = lpS->wBitmapIndex * lpFace->wBmpHeight;
        HDC hMDC = CreateCompatibleDC( hDC );
        HBITMAP hOld;

        w = max( 0, (rc.right-(int)lpFace->wBmpWidth)/2);
        h = max( 0, (rc.bottom-(int)lpFace->wBmpHeight)/2);

        //*** Erase background
        ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL );

        //*** Paint the portion of the bitmap into the window
        hOld = SelectObject( hMDC, hBmp );
        BitBlt(hDC,w,h,lpFace->wBmpWidth,lpFace->wBmpHeight,hMDC,0,y,SRCCOPY);
        SelectObject( hMDC, hOld );
        DeleteDC( hMDC );
    }

    ReleaseDC( hWnd, hDC );

} //*** FVPaintBitmap


//*************************************************************
//
//  FVGetSmallThumb
//
//  Purpose:
//      Retrieves a bitmap from a face
//
//
//  Parameters:
//      LPFACE  lpFace
//      
//
//  Return: (HBITMAP WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 5/92   MSM        Created
//
//*************************************************************

HBITMAP WINAPI FVGetSmallThumb( LPFACE lpFace )
{
    if (!lpFace)
        return NULL;

    if (lpFace->hBitmap)
        return lpFace->hBitmap;

    lpFace->hBitmap = LoadFaceBitmap( lpFace );
    return lpFace->hBitmap;

} //*** FVGetSmallThumb


//*************************************************************
//
//  FVFreeBitmap
//
//  Purpose:
//      Frees the associated bitmap
//
//
//  Parameters:
//      LPFACE lpFace
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 5/92   MSM        Created
//
//*************************************************************

VOID WINAPI FVFreeBitmap(LPFACE lpFace)
{
    if (!lpFace)
        return;

    if (lpFace->hBitmap)
    {
        DeleteObject( lpFace->hBitmap );
        lpFace->hBitmap = NULL;
    }

} //*** FVFreeBitmap


//*************************************************************
//
//  FVSetStatusToNode
//
//  Purpose:
//      Updates the status bar to reflect the node
//
//
//  Parameters:
//      LPDBNODE lpdb
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 6/92   MSM        Created
//
//*************************************************************

VOID WINAPI FVSetStatusToNode( LPDBNODE lpdb )
{
    char szTemp[255];
    LPSTR lp1, lp2, lpC;
    
    if (!lpdb)
    {
        SetWindowText( ghwndStat, GRCS( IDS_STAT_READY ) );
        return;
    }

    switch (lpdb->wNodeType)
    {
        case NT_STYLE:
        {
            //*** Get style name
            lp2 = (LPSTR)NODENAME( ((LPSTYLE)lpdb) );
            lpdb= lpdb->lpParent;

            //*** Get face name
            lp1 = (LPSTR)NODENAME( ((LPFACE)lpdb) );

            //*** Climb to publisher
            lpdb= lpdb->lpParent;
            lpdb= lpdb->lpParent;

            lpC = ((LPPUB)lpdb)->lpszSCopyright;
        }
        break;

        case NT_FACE:
        {
            //*** Get face name
            lp1 = (LPSTR)NODENAME( ((LPFACE)lpdb) );
            lp2 = "";

            //*** Climb to publisher
            lpdb= lpdb->lpParent;
            lpdb= lpdb->lpParent;

            lpC = ((LPPUB)lpdb)->lpszSCopyright;
        }
        break;

        case NT_FAMILY:
        {
            //*** Get family name
            lp1 = (LPSTR)NODENAME( ((LPFAMILY)lpdb) );
            lp2 = "";

            //*** Climb to publisher
            lpdb= lpdb->lpParent;

            lpC = ((LPPUB)lpdb)->lpszSCopyright;
        }
        break;

        case NT_PUB:
        {
            //*** Get publisher name
            lp1 = ((LPPUB)lpdb)->lpszPublisher;
            lp2 = "";
            lpC = "";
        }
        break;
    }

    if (*lpC)
        wsprintf( szTemp, "%s", lpC );
//        wsprintf( szTemp, "%s %s,  %s", lp1, lp2, lpC );
    else
        wsprintf( szTemp, "%s %s", lp1, lp2 );

    SetWindowText( ghwndStat, szTemp );

} //*** FVSetStatusToNode

//*** EOF: fontview.c
