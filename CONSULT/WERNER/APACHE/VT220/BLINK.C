/************************************************************************
 *
 *    Copyright (C) 1991 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  blink.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Changes the blink state and invalidates
 *               rects corresponding to the attributes for the
 *               characters.
 *
 *   Revisions:  
 *     01.00.010  5/19/91 baw   Wrote it.
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "vt220.h"

/************************************************************************
 *  BOOL BlinkEmulator( HWND hWnd )
 *
 *  Description: 
 *     Called from WM_TIMER message.  This routine changes the blink
 *     state and invalidates corresponding areas.
 *
 *  Comments:
 *      5/19/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL BlinkEmulator( HWND hWnd )
{
   int    nRow, nCol, nEndRow, nEndCol, nCount, nHorzPos, nVertPos,
          nCurPos, nRun, nMaxCol ;
   BYTE   bAttr ;
   HECB   hECB ;
   NPECB  npECB ;
   RECT   rcTemp, rcInvalid ;
   
   // Get Emulator Control Block
   hECB = (HECB) GETHECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( (HLOCAL) hECB )))
      return ( FALSE ) ;

   BLINKSTATE( npECB ) = !BLINKSTATE( npECB ) ;

   GetClientRect( hWnd, &rcTemp ) ;

   nRow = max( 0, (rcTemp.top + YOFFSET( npECB )) / YCHAR( npECB ) ) ;
   nEndRow = min( MAXROWS( npECB ) - 1,
                  (rcTemp.bottom + YOFFSET( npECB )) / YCHAR( npECB ) ) ;
   nCol = max( 0, (rcTemp.left + XOFFSET( npECB )) / XCHAR( npECB ) ) ;
   nEndCol = min( MAXCOLS( npECB ) - 1,
                  (rcTemp.right + XOFFSET( npECB )) / XCHAR( npECB ) ) ;
   nCount = nEndCol - nCol + 1 ;

   for (; nRow <= nEndRow; nRow++ )
   {
      nCurPos = nCol ;
      nMaxCol = nEndCol ;
      while (nCurPos <= nMaxCol)
      {
         bAttr = *(LPSTR) (ATTRBUF( npECB ) + nRow * MAXCOLS( npECB ) +
                           nCurPos) ;
         if (bAttr & (BYTE) (ATTRIBUTE_DBLSIZE))
            nMaxCol = nEndCol / 2 ;
         nRun = 1 ;
         while ((nCurPos + nRun <= nMaxCol) &&
                (bAttr == (BYTE) *(LPSTR) (ATTRBUF( npECB ) +
                                           nRow * MAXCOLS( npECB ) +
                                           nCurPos + nRun)))
            nRun++ ;

         // get rect of run area if attribute contains ATTRIBUTE_BLINKING

         if (bAttr & ATTRIBUTE_BLINKING)
         {
            if (bAttr & (BYTE) (ATTRIBUTE_DBLSIZE))
            {
               GetDblCharRect( npECB, &rcTemp, nCurPos, nRow ) ;
               nVertPos = rcTemp.top ;
               nHorzPos = rcTemp.left ;
               GetDblCharRect( npECB, &rcInvalid, nCurPos + nRun, nRow ) ;
               UnionRect( &rcInvalid, &rcInvalid, &rcTemp ) ;
            }
            else
            {
               GetStdCharRect( npECB, &rcTemp, nCurPos, nRow ) ;
               nVertPos = rcTemp.top ;
               nHorzPos = rcTemp.left ;
               GetStdCharRect( npECB, &rcInvalid, nCurPos + nRun, nRow ) ;
               UnionRect( &rcInvalid, &rcInvalid, &rcTemp ) ;
            }
            InvalidateRect( hWnd, &rcInvalid, FALSE ) ;
            UpdateWindow( hWnd ) ;
         }
         nCurPos += nRun ;
      }
   }

   LocalUnlock( (HLOCAL) hECB ) ;

   return ( TRUE ) ;

} /* end of BlinkEmulator() */

/************************************************************************
 * End of File: blink.c
 ************************************************************************/

