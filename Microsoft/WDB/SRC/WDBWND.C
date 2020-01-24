/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** wdbwnd.c
 *
 *  DESCRIPTION: 
 *      This file contains the code needed to implement the output window.
 *
 *  HISTORY:
 *      4/30/91     cjp     got rid of some stupid code and should probably
 *                          get rid of some more...
 *      3/10/91     cjp     put in this comment
 *
 ** cjp */


/* the includes required to build this file */
    #include <windows.h>
    #include "wdb.h"
    #include "wdbi.h"


    /*  Macros to manipulate the printf window array of lines.  This array
     *  wraps around, thus the need for the modulo arithmetic
     */
    #define FIRST(pText) ((pText)->iFirst)
    #define TOP(pText)   (((pText)->iFirst + (pText)->iTop) % (pText)->iMaxLines)
    #define LAST(pText)  (((pText)->iFirst + (pText)->iCount-1) % (pText)->iMaxLines)
    #define INC(pText,x) ((x) = ++(x) % (pText)->iMaxLines)
    #define DEC(pText,x) ((x) = --(x) % (pText)->iMaxLines)

    /*  The pad values used between the edge of the window and the text.
     *  x = 1/2 ave char width, y = 1 pixel
     */
    #define OFFSETX (pText->dxFont/2)
    #define OFFSETY 1

    #define BOUND(x,min,max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))


/** void NEAR PASCAL wdbUpdateCursorPos( NPWDBTEXT pText )
 *
 *  DESCRIPTION: 
 *      This function updates the position of the cursor in the window
 *      if character input is enabled.
 *
 *      Character input is NOT supported in this version of WDB.
 *
 *  ARGUMENTS:
 *
 *  RETURN (void NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

void NEAR PASCAL wdbUpdateCursorPos( NPWDBTEXT pText )
{
    int     iLine;
    int     y, x;
    int     iLen;
    DWORD   dw;
    HDC     hDC;
    PSTR    pLine;
    #ifdef WIN32
    SIZE size ;      // for gettextextentpoint
    #endif

    /* if don't do char input, or don't have the focus, forget it */
    if ( !pText->hInQueue /* || (GetFocus() != pText->hWnd) */ )
        return;

    if ( hDC = GetDC( NULL ) )
    {
        SelectObject( hDC, pText->hFont );
        iLen = pText->rLines[ LAST(pText) ].iLen - pText->iLeft;


        if ( pLine = LocalLock( pText->rLines[ LAST(pText) ].hText ) )
        {
            #ifndef WIN32
            /* HACK HACK Need to account for tabs? */
            dw = GetTextExtent( hDC, pLine + pText->iLeft, iLen );
            #else
            /* HACK HACK Need to account for tabs? */
            GetTextExtentPoint( hDC, pLine + pText->iLeft, iLen, &size );
            #endif

            LocalUnlock( pText->rLines[ LAST(pText) ].hText );
        }

        else dw = 0;

        iLine = pText->iCount - pText->iTop;
        ReleaseDC( NULL, hDC );
    }

    y = OFFSETY + (iLine - 1) * pText->dyFont;

    #ifndef WIN32
    x = OFFSETX + LOWORD( dw );
    #else
    x = OFFSETX + size.cx ;
    #endif 
    SetCaretPos( x, y );
} /* wdbUpdateCursorPos() */


/** BOOL NEAR PASCAL wdbChangeLine( NPWDBTEXT pText, int iLine, LPSTR lpsz )
 *
 *  DESCRIPTION: 
 *      Changes line number iLine to be the string pointed to by lpsz.
 *      Frees any line currently occupying index iLine, and then alloc
 *      and stores text lpsz.
 *
 *  ARGUMENTS:
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

BOOL NEAR PASCAL wdbChangeLine( NPWDBTEXT pText, int iLine, LPSTR lpsz )
{
    int     iLen;
    PSTR    pLine;

    /* if there is already a line at this index, free it */
    if ( pText->rLines[ iLine ].hText )
        LocalFree( pText->rLines[ iLine ].hText );

    /* how big should the buffer be? */
    pText->rLines[ iLine ].iLen = iLen = lstrlen( lpsz );

    /* alloc a buffer big enough to hold the text */
    if ( !(pText->rLines[ iLine ].hText = LocalAlloc(WDBC_LMEM_FLAGS, iLen + 1)) )
        return ( FALSE );

    /* copy the string into this new buffer */
    if ( pLine = LocalLock( pText->rLines[ iLine ].hText ) )
    {
        lstrcpy( pLine, lpsz );
        LocalUnlock( pText->rLines[ iLine ].hText );
    }

    /* else Hmm... */

    /* return success */
    return ( TRUE );
} /* wdbChangeLine() */


/** void NEAR PASCAL wdbNewLine( NPWDBTEXT pText )
 *
 *  DESCRIPTION: 
 *      Adjusts a WDB window when adding a line to the circular array.
 *      iCount is the count of valid lines in the array.  If we haven't
 *      yet filled up the array, the count is merely increased.  Otherwise,
 *      if the array is full and we're about to wrap around, fixup the
 *      wrap-around.
 *
 *  ARGUMENTS:
 *
 *  RETURN (void NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

void NEAR PASCAL wdbNewLine( NPWDBTEXT pText )
{
    int     iLast = LAST( pText );
    RECT    rect;

    if ( pText->iCount == pText->iMaxLines )
    {
        /* if the array is full, check for wrap-around */
        LocalFree( pText->rLines[ pText->iFirst ].hText );
        pText->rLines[ pText->iFirst ].hText = NULL;
       
        INC( pText, pText->iFirst );
       
        if ( pText->iTop > 0 )
            pText->iTop--;

        else
        {
            GetClientRect( pText->hWnd, &rect );
            rect.left += OFFSETX;
            rect.top  += OFFSETY;
            ScrollWindow( pText->hWnd, 0, -pText->dyFont, &rect, &rect );
        }
    }

    else pText->iCount++;

    iLast = LAST( pText );
    pText->rLines[ iLast ].hText = NULL;
    pText->rLines[ iLast ].iLen = 0;
} /* wdbNewLine() */


/** int NEAR PASCAL wdbInsertString( NPWDBTEXT pText, LPSTR lpsz )
 *
 *  DESCRIPTION: 
 *      
 *
 *  ARGUMENTS:
 *
 *  RETURN (int NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

int NEAR PASCAL wdbInsertString( NPWDBTEXT pText, LPSTR lpsz )
{
    int         iBuf, iBufLast;
    int         iLast = LAST(pText);
    int         cLine = 0;
    PSTR        psz;
    HANDLE      hLine;
    char        buf[ WDBC_MAX_BUFLEN ];


    /* is there already a string in the 'last' position? */
    if ( hLine = pText->rLines[ iLast ].hText )
    {
        /* copy the string already there */
        if ( psz = (PSTR)LocalLock( hLine ) )
        {
            iBufLast = iBuf = lstrlen( psz );
            lstrcpy( buf, psz );

            /* unlock the line */
            LocalUnlock( hLine );
        }

        else OD( "WDB: BAD BAD BAD!!!!\r\n" );
    }

    /* zero 'old' line buffer length */
    else iBufLast = iBuf = 0;


    while ( *lpsz )
    {
        while ( *lpsz && (*lpsz != '\n') && (iBuf < WDBC_MAX_BUFLEN - 2) )
        {
            switch ( *lpsz )
            {
                case '\a':
                    MessageBeep( MB_ICONEXCLAMATION );
                    lpsz++;
                break;

                case '\b':
                    /* backspace, blow away one character */
                    if ( iBuf > 0 )
                        iBuf--;
                    lpsz++;
                break;

                case '\r':
                    /* carriage return, go back to beginning of line */
                    lpsz++;
                    iBuf = 0;
                break;
        
                default:
                    /* otherwise, add this char to line */
                    buf[ iBuf++ ] = *lpsz++;
                    iBufLast++;
                break;
            }
        }

        /* cap the sucker */
        buf[ iBufLast++ ] = 0;

        /* presto chango add the line--buf must be asciiz string */
        wdbChangeLine( pText, iLast, buf );

        /* now do the next string after the \n */
        if ( *lpsz == '\n' )
        {
            lpsz++;
            iBufLast = iBuf = 0;
            cLine++;
            wdbNewLine( pText );
            INC( pText, iLast );
        }
    }

    /* the number of new lines added to list */
    return ( cLine );
} /* wdbInsertString() */


/** int NEAR PASCAL wdbLinesInWindow( HWND hWnd )
 *
 *  DESCRIPTION: 
 *      Returns the height in lines of the window.
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   Handle to the window.
 *
 *  RETURN (int NEAR PASCAL):
 *      The return value is the number of lines in the window--that is
 *      currently visible.
 *
 *  NOTES:
 *
 ** cjp */

int NEAR PASCAL wdbLinesInWindow( HWND hWnd )
{
    NPWDBTEXT   pText;
    HWDBTEXT    hText;
    RECT        rRect;
    int         iLines;

    GetClientRect( hWnd, &rRect );
    iLines = 0;

    /* count current number of lines *visible* */
    hText = (HWDBTEXT)GETHTEXT( hWnd );
    if ( pText = (NPWDBTEXT)LocalLock( hText ) )
    {
        iLines = (rRect.bottom - rRect.top - OFFSETY) / pText->dyFont;
        iLines = min( iLines, pText->iMaxLines );
        LocalUnlock( hText );
    }

    else iLines = 0;

    /* return the number of lines that are visible */
    return ( iLines );
} /* wdbLinesInWindow() */


/** int NEAR PASCAL wdbCharsInWindow( HWND hWnd )
 *
 *  DESCRIPTION: 
 *      Returns the width in characters of the window.
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   Handle to the window.
 *
 *  RETURN (int NEAR PASCAL):
 *      Returns approximately the number of characters will fit in the
 *      width of the window.
 *
 *  NOTES:
 *
 ** cjp */

int NEAR PASCAL wdbCharsInWindow( HWND hWnd )
{
    int         iReturn;
    RECT        rRect;
    NPWDBTEXT   pText;
    HWDBTEXT    hText;

    GetClientRect( hWnd, &rRect );

    hText = (HWDBTEXT)GETHTEXT( hWnd );
    if ( pText = (NPWDBTEXT)LocalLock( hText ) )
    {
        if ( pText )
            iReturn = (rRect.right - rRect.left - OFFSETX) / pText->dxFont;

        LocalUnlock( hText );
    }

    else iReturn = 0;

    return ( iReturn );
} /* wdbCharsInWindow() */


/** void NEAR PASCAL wdbSetMaxLineLen( NPWDBTEXT pText )
 *
 *  DESCRIPTION: 
 *      This function sets the pText->iMaxLen field to be the length in
 *      characters of the longest string currently being stored by the
 *      WDB window.
 *
 *  ARGUMENTS:
 *      NPWDBTEXT pText     :   Pointer to WDB text info struct.
 *
 *  RETURN (void NEAR PASCAL):
 *      None.
 *
 *  NOTES:
 *
 ** cjp */

void NEAR PASCAL wdbSetMaxLineLen( NPWDBTEXT pText )
{
    int     iQueue;
    int     iLast;
    int     iLen;

#if 0
    HANDLE  hLine;
    DWORD   dwLen;
    HDC     hDC;

    hDC = GetDC( NULL );

    if ( pText->hFont )
        SelectObject( hDC, pText->hFont );
#endif

    iLast = LAST( pText );
    iQueue  = TOP( pText);
    pText->iMaxLen = 0;

    for (;;)
    {
        iLen = pText->rLines[ iQueue ].iLen;

#if 0
        if ( hLine = pText->rLines[iQueue].hText )
        {
            PSTR    psz = (PSTR)LocalLock( hLine );

            if ( pText->nTabs )
                dwLen = GetTabbedTextExtent( hDC, (LPSTR)psz, iLen,
                                    pText->nTabs, (LPINT)pText->pTabs );
            else
                dwLen = GetTextExtent( hDC, (LPSTR)psz, iLen );

            /* unlock the line */
            LocalUnlock( hLine );
        }

        iLen = LOWORD(dwLen) / pText->dxFont + 1;
#endif

        if ( iLen > pText->iMaxLen )
            pText->iMaxLen = iLen;

        if ( iQueue == iLast )
            break;

        INC( pText, iQueue );
    }

//    ReleaseDC( NULL, hDC );
} /* wdbSetMaxLineLen() */


/** void NEAR PASCAL wdbVertScroll( HWND hWnd, NPWDBTEXT pText, int n )
 *
 *  DESCRIPTION: 
 *      Vertical scroll the window by n number of lines.
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   Handle to the window.
 *
 *  RETURN (void NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

void NEAR PASCAL wdbVertScroll( HWND hWnd, NPWDBTEXT pText, int n )
{
    RECT        rect;
    int         iRange;

//    GetScrollRange ( hWnd, SB_VERT, &iMinPos, &iMaxPos );

    iRange = pText->iRangeV;
    GetClientRect( hWnd, &rect );
    rect.left += OFFSETX;
    rect.top += OFFSETY;

    n = BOUND( pText->iTop + n, 0, iRange ) - pText->iTop;
    pText->iTop += n;
    ScrollWindow( hWnd, 0, -n * pText->dyFont, &rect, &rect );
    SetScrollPos( hWnd, SB_VERT, pText->iTop, pText->fRedraw );
    UpdateWindow( hWnd );
} /* wdbVertScroll() */


/** void NEAR PASCAL wdbHorzScroll( HWND hWnd, NPWDBTEXT pText, int n )
 *
 *  DESCRIPTION: 
 *      Horizontally scrolls the window by n number of character widths.
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   Handle to the window.
 *
 *  RETURN (void NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

void NEAR PASCAL wdbHorzScroll( HWND hWnd, NPWDBTEXT pText, int n )
{
    RECT    rect;
    int     iRange;

//    GetScrollRange( hWnd, SB_HORZ, &iMinPos, &iMaxPos );

    iRange = pText->iRangeH;
    GetClientRect( hWnd, &rect );
    rect.left += OFFSETX;
    rect.top += OFFSETY;

    n = BOUND( pText->iLeft + n, 0, iRange ) - pText->iLeft;
    pText->iLeft += n;
    ScrollWindow( hWnd, -n * pText->dxFont, 0, &rect, &rect );
    SetScrollPos( hWnd, SB_HORZ, pText->iLeft, pText->fRedraw );
    UpdateWindow( hWnd );
} /* wdbHorzScroll() */


/** void NEAR PASCAL wdbSetScrollRange( HWND hWnd, BOOL fRedraw )
 *
 *  DESCRIPTION: 
 *      This function sets the scrollbar ranges according to the current
 *      character/line contents of the window.  Both the horizontal and
 *      vertical scrollbars are adjusted.
 * 
 *      This function then calls wdbVScroll/wdbHScroll to adjust the
 *      scrollbar position accordingly.
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   Handle to the window.
 *
 *      BOOL fRedraw:   Tells whether to redraw or not.
 *
 *  RETURN (void NEAR PASCAL):
 *      None.
 *
 *  NOTES:
 *
 ** cjp */

void NEAR PASCAL wdbSetScrollRange( HWND hWnd, BOOL fRedraw )
{
    NPWDBTEXT   pText;
    HWDBTEXT    hText;
    int         iRange;

    hText = (HWDBTEXT)GETHTEXT( hWnd );

    if ( pText = (NPWDBTEXT)LocalLock( hText ) );
    {
        pText->fRedraw = fRedraw;

        /* update the scroll bars */
        iRange = pText->iCount - wdbLinesInWindow( hWnd ) + 1;

        /* adjust for blank last line? */
        if ( pText->rLines[ LAST(pText) ].iLen == 0 );
            iRange -= 1;

        if ( iRange < 0 )
            iRange = 0;
    
        /* set the scrollbar range to that calculated */
        pText->iRangeV = iRange;
        SetScrollRange( hWnd, SB_VERT, 0, iRange, FALSE );
        wdbVertScroll( hWnd, pText, 0 );

        /* setup the horizontal scrollbar range */
        wdbSetMaxLineLen( pText );
        iRange = pText->iMaxLen - wdbCharsInWindow( hWnd ) + 1;

        if ( iRange < 0 )
            iRange = 0;

        pText->iRangeH = iRange;

        SetScrollRange( hWnd, SB_HORZ, 0, iRange, FALSE );
        wdbHorzScroll( hWnd, pText, 0 );

        pText->fRedraw = TRUE;

        LocalUnlock( hText );
    }
} /* wdbSetScrollRange() */


/** BOOL FAR PASCAL wdbWriteWindow( LPWDBSESSION lpSession, LPSTR lpsz )
 *
 *  DESCRIPTION: 
 *      This function 'writes' lpsz into the window.  The text is scrolled
 *      if it needs to be--among other things.
 *
 *  ARGUMENTS:
 *
 *  RETURN (BOOL FAR PASCAL):
 *      The return value specifies whether the call succeeded.  It can 
 *      fail if the window handle is not valid.
 *
 *  NOTES:
 *      It is assumed that this function does NOT use gachBigTemp.  In this
 *      way, functions can (and do) pass gachBigTemp as the lpsz.  This
 *      also applies to gachTemp.
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteWindow( LPWDBSESSION lpSession, LPSTR lpsz )
{
    NPWDBTEXT   pText;
    HWDBTEXT    hText;
    RECT        rect;
    int         iLine;
    HWND        hWnd;

    /* fail if bad window handle */
    if ( !(hWnd = lpSession->hOutput) || !IsWindow( hWnd ) )
        return ( FALSE );

    hText = (HWDBTEXT)GETHTEXT( hWnd );
    if ( pText = (NPWDBTEXT)LocalLock( hText ) )
    {
        iLine = pText->iCount - pText->iTop;

        /* invalidate the line to bottom of window so new text will be painted */
        GetClientRect( hWnd, &rect );
        rect.top += ( iLine - 1 ) * pText->dyFont;
        InvalidateRect( hWnd, &rect, FALSE );

        /* insert text in the queue */
        wdbInsertString( pText, lpsz );

        if ( (iLine = (pText->iCount - pText->iTop) - iLine) > 0 )
        {
            wdbSetScrollRange( hWnd, FALSE );

            /* scroll all the way to bottom */
            wdbVertScroll( hWnd, pText, pText->iCount );
        }

#if 0
        /** we don't need to do this **/
        else wdbSetScrollRange( hWnd, TRUE );
#endif

        wdbUpdateCursorPos( pText );
        LocalUnlock( hText );
        UpdateWindow( hWnd );
    }

    else return ( FALSE );

    return ( TRUE );
} /* wdbWriteWindow() */


/** void NEAR PASCAL wdbClearWindow( HWND hWnd )
 *
 *  DESCRIPTION: 
 *      Clears all text from the window.  Frees all allocated memory.
 *      The current queue is not modified?
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   Handle to the window.
 *
 *  RETURN (void NEAR PASCAL):
 *      The window will be blank.
 *
 *  NOTES:
 *
 ** cjp */

void NEAR PASCAL wdbClearWindow( HWND hWnd )
{
    NPWDBTEXT   pText;
    HWDBTEXT    hText;
    int         i, iQueue;

    hText = (HWDBTEXT)GETHTEXT( hWnd );

    /* lock it */
    if ( pText = (NPWDBTEXT)LocalLock( hText ) )
    {
        iQueue = FIRST( pText );

        for ( i = 0; i < pText->iCount; i++, INC( pText, iQueue ) )
        {
            if ( pText->rLines[ iQueue ].hText != NULL )
                LocalFree( pText->rLines[ iQueue ].hText );
        }

        /* set queue up to have 1 NULL line */
        pText->iFirst           = 0;
        pText->iCount           = 1;
        pText->iTop             = 0;
        pText->iLeft            = 0;
        pText->iMaxLen          = 0;
        pText->rLines[0].hText  = NULL;
        pText->rLines[0].iLen   = 0;

        /* unlock it */
        LocalUnlock( hText );

        InvalidateRect( hWnd, NULL, TRUE );
        wdbSetScrollRange( hWnd, TRUE );
        UpdateWindow( hWnd );
    }
} /* wdbClearWindow() */


/** HFONT NEAR PASCAL wdbSetFont( HWND hWnd, HFONT hFont )
 *
 *  DESCRIPTION: 
 *      Changes the font of a WDB printf window to be the specified handle.
 *      Rebuilds the internal character size measurements, and causes the
 *      window to repaint.
 * 
 *      Is there a problem with scroll ranges changing here?
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   Handle to the window.
 *
 *      HFONT hFont :   Font handle for the font to use.
 *
 *  RETURN (HFONT NEAR PASCAL):
 *      The return value is hFont if this called succeeded.  It is NULL
 *      if the font could not be set.
 *
 *  NOTES:
 *
 ** cjp */

HFONT NEAR PASCAL wdbSetFont( HWND hWnd, HFONT hFont )
{
    HWDBTEXT    hText;
    NPWDBTEXT   pText;
    HDC         hDC;
    TEXTMETRIC  tm;
    HFONT       hReturn = NULL;

    hText = (HWDBTEXT)GETHTEXT( hWnd );

    /* lock it */
    if ( pText = (NPWDBTEXT)LocalLock( hText ) )
    {
        /* get the return value before we munge it */
        hReturn = pText->hFont;

        /* find out the size of a char in the font */
        if ( hFont && (hDC = GetDC( hWnd )) )
        {
            pText->hFont = hFont;

            SelectObject( hDC, hFont );
            GetTextMetrics( hDC, (LPTEXTMETRIC)&tm );

            pText->dyFont = tm.tmHeight;
            pText->dxFont = tm.tmAveCharWidth;
            ReleaseDC( hWnd, hDC );

            /* clear the window--force repaint with new font */
            InvalidateRect( hWnd, NULL, TRUE );
        }

        /* unlock it */
        LocalUnlock( hText );
    }

    /* return handle to old font */
    return ( hReturn );
} /* wdbSetFont() */


/** void NEAR PASCAL wdbPaint( HWND hWnd, HDC hDC )
 *
 *  DESCRIPTION: 
 *      The paint function.
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   Window to paint to.
 *
 *      HDC hDC     :   Handle to update region's display context.
 *
 *  RETURN (void NEAR PASCAL):
 *      The window will have been painted.
 *
 *  NOTES:
 *
 ** cjp */

void NEAR PASCAL wdbPaint( HWND hWnd, HDC hDC )
{
    HWDBTEXT    hText;
    NPWDBTEXT   pText;
    char       *pLine;
    int         iQueue;
    int         xco;
    int         yco;
    int         iLast;
    RECT        rc;
    RECT        rcClip;
    HFONT       hFontOld;
    
    /* need this for just about everything... */
    hText = (HWDBTEXT)GETHTEXT( hWnd );

    if ( pText = (NPWDBTEXT)LocalLock( hText ) )
    {
        GetClientRect( hWnd, &rc );
        rc.left += OFFSETX;
        rc.top += OFFSETY;
        IntersectClipRect( hDC, rc.left, rc.top, rc.right, rc.bottom );
        SetTextColor( hDC, GetSysColor( COLOR_WINDOWTEXT ) );
        SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) );

        /* if a font (other than the system font) has been specified, use it */
        if ( pText->hFont )
            hFontOld = SelectObject( hDC, pText->hFont );

        /*  Setup counters as appropriate.  Get indexes of first and last
        *  lines visible within the window.
        */
        iLast = LAST( pText );
        iQueue  = TOP( pText );

        /*  The x and y initial points for the text line.  xco is shifted
        *  left to account for any horizonal scrolling that may be going on.
        */
        xco = OFFSETX - pText->iLeft * pText->dxFont;

        /* starting y pix value    */
        yco = OFFSETY;

        /*  RC is the bounding rect for the current line.
        *
        *  Calc initial line bounding rect.. top = top of window (padded),
        *  bottom = top + height of one line.
        */
        rc.left = xco;
        rc.top = yco;
        rc.bottom = yco + pText->dyFont;

        /* get the clipping rectangle */
        GetClipBox( hDC, &rcClip );

        /*  step through all lines that are visible--if the bounding rect
        *  for the current line intersects the clip rect, draw the line.
        */
        for (;;)
        {
            if ( rc.bottom >= rcClip.top )
            {
                 
                if (pText->rLines[ iQueue ].hText &&
                    (pLine = (char *)LocalLock( pText->rLines[ iQueue ].hText )) )
                {
                    /* if we're using tabs, then tab out the text */
                    if ( pText->nTabs > 0 )
                    {
                        /* erase the background */
                        ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL );
            
                        /* blast it out with the tabs */
                        TabbedTextOut( hDC, xco, yco, (LPSTR)pLine,
                                    pText->rLines[ iQueue ].iLen,
                                    pText->nTabs, pText->pTabs, xco );
                    }

                    else
                    {
                        /* otherwise, blow it out using ExtTextOut (fast!) */
                        ExtTextOut( hDC, xco, yco, ETO_OPAQUE, &rc, (LPSTR)pLine,
                                    pText->rLines[ iQueue ].iLen, NULL );
                    }

                    LocalUnlock( pText->rLines[ iQueue ].hText );
                }
            }

            /* bail out when finished printing window contents */
            if ( iQueue == iLast )
                break;

            INC( pText, iQueue );

            /* advance the boundry rect & char positions down one line */
            yco = rc.top = rc.bottom;
            rc.bottom += pText->dyFont;

            if ( yco > rcClip.bottom )
                break;
        }

        /* restore the old font */
        if ( pText->hFont )
            SelectObject( hDC, hFontOld );

        /* if you only knew how long I spent remembering to put this in!!! */
        LocalUnlock( hText );
    }
} /* wdbPaint() */


/** void wdbSnapshotWindow( NPWDBTEXT pText )
 *
 *  DESCRIPTION: 
 *      This function is responsible for saving the size and position of
 *      the WDB output window to the .INI file.  And also for when it is
 *      re-opened during the same session.
 *
 *  ARGUMENTS:
 *      NPWDBTEXT pText :   Pointer to WDB text info for window.
 *
 *  RETURN (void):
 *      None.  The position and size will have been saved if all went
 *      well.
 *
 *  NOTES:
 *
 ** cjp */

void wdbSnapshotWindow( NPWDBTEXT pText )
{
    LPWDBSESSION    lpSession;
    LPSTR           lpszPrivate;
    RECT            rect;

    /* don't save position if iconic */
    if ( IsIconic( pText->hWnd ) )
        return;

    /* snapshot the window position */
    GetWindowRect( pText->hWnd, &rect );

    if ( lpSession = pText->hWDB ? (LPWDBSESSION)GlobalLock( pText->hWDB ) :
                                   (LPWDBSESSION)&gWDBDefSession )
    {
        lpszPrivate = ( *lpSession->szPrivate ) ? lpSession->szPrivate : NULL;

        /* save the size and offset of the window */
        if ( rect.left != lpSession->wdbConfig.nWindowX )
        {
            lpSession->wdbConfig.nWindowX = rect.left;
            wdbWriteConfigWindowX( lpSession->szSection, lpszPrivate,
                                    (LPWDBCONFIG)lpSession );
        }

        if ( rect.top != lpSession->wdbConfig.nWindowY )
        {
            lpSession->wdbConfig.nWindowY = rect.top;
            wdbWriteConfigWindowY( lpSession->szSection, lpszPrivate,
                                    (LPWDBCONFIG)lpSession );
        }

        if ( (rect.right - rect.left) != (int)lpSession->wdbConfig.wWindowW )
        {
            lpSession->wdbConfig.wWindowW = (UINT)(rect.right - rect.left);
            wdbWriteConfigWindowW( lpSession->szSection, lpszPrivate,
                                    (LPWDBCONFIG)lpSession );
        }

        if ( (rect.bottom - rect.top) != (int)lpSession->wdbConfig.wWindowH )
        {
            lpSession->wdbConfig.wWindowH = (UINT)(rect.bottom - rect.top);
            wdbWriteConfigWindowH( lpSession->szSection, lpszPrivate,
                                    (LPWDBCONFIG)lpSession );
        }

        /* unlock it now */
        if ( pText->hWDB )  GlobalUnlock( pText->hWDB );
    }
} /* wdbSnapshotWindow() */


/** LONG FAR PASCAL wdbWndProc( HWND hWnd, unsigned uiMessage,
 *
 *  DESCRIPTION: 
 *      Window proc for WDB windows.
 *
 *  ARGUMENTS:
 *      ( HWND hWnd, unsigned uiMessage,
 *
 *  RETURN (LONG FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** cjp */

LONG FAR PASCAL wdbWndProc( HWND hWnd, unsigned uiMessage,
                                  UINT wParam, LONG lParam )
{
    PAINTSTRUCT ps;
    HWDBTEXT    hText;
    NPWDBTEXT   pText;
    HWDB        hWDB;
    int       i;
    int       iQueue;

    /* need this for just about everything... */
    hText = (HWDBTEXT)GETHTEXT( hWnd );

    switch ( uiMessage )
    {
        case WM_CREATE:
            #define lpCreate    ((LPCREATESTRUCT)lParam)

         #ifndef WIN32
            hWDB = (HWDB)LOWORD( (DWORD)lpCreate->lpCreateParams );
         #else
            hWDB = (HWDB)lpCreate->lpCreateParams ;
         #endif

            i = wdbGetInfo( hWDB, WDBI_WMAXLINES );

            /*  Allocate and initialize the window instance structure
             *  The storage for the current lines is placed at the end
             *  end of the instance data structure--allocate room for it.
             */
            hText = (HWDBTEXT)LocalAlloc( WDBC_LMEM_FLAGS, sizeof(WDBTEXT) +
                                                (i * sizeof(WDBLINE)) );

            if ( !hText )
                return ( -1L );

            if ( pText = (NPWDBTEXT)LocalLock( hText ) )
            {
                pText->hWDB      = hWDB;
                pText->hWnd      = hWnd;
               #ifndef WIN32
                pText->wID       = HIWORD( lpCreate->lpCreateParams );
               #else
                pText->wID       = (UINT)lpCreate->lpCreateParams ;
               #endif
                pText->iMaxLines = i;
                pText->iCount    = 1;

#ifdef WE_WANNA_WASTE_TIME
                pText->iFirst    = 0;   /* initially 1 null line        */
                pText->iTop      = 0;   /* gotta start somewhere        */
                pText->iLeft     = 0;   /* no initial hscroll offset    */
                pText->iMaxLen   = 0;   /* current max line len is zero */
                pText->nTabs     = 0;   /* no tabs yet                  */
#endif

                /* if user specified character input, allocate a buffer */
                if ( lpCreate->style & WDB_CHARINPUT )
                    pText->hInQueue = (HINQUEUE)LocalAlloc( WDBC_LMEM_FLAGS,
                                                        sizeof(INQUEUE) );

#ifdef WE_WANNA_WASTE_MORE_TIME
                /* null initial first line */
                pText->rLines[ 0 ].hText = NULL;
                pText->rLines[ 0 ].iLen = 0;
#endif

                /* store the structure pointer onto the window */
                SETHTEXT( hWnd, (WORD)hText );
        
                /* setup to use the system font by default */
                wdbSetFont( hWnd, GetStockObject( SYSTEM_FONT ) );
        
                LocalUnlock( (HANDLE)hText );
            }
        break;

        case WM_DESTROY:
        {
            if ( pText = (NPWDBTEXT)LocalLock( hText ) )
            {
                /* snapshot the window position and size */
                wdbSnapshotWindow( pText );

                /* kill the output handle */
                wdbSetInfo( pText->hWDB, WDBI_HOUTPUT, NULL );

                /* blow away all lines held by the window */
                iQueue = FIRST( pText );
                for ( i = 0; i < pText->iCount; i++, INC( pText, iQueue ) )
                {
                    if ( pText->rLines[ iQueue ].hText )
                        LocalFree( (HANDLE)pText->rLines[ iQueue ].hText );
                }

                /* and kill char input and tab stop storage */
                if ( pText->hInQueue )
                    LocalFree( pText->hInQueue );

                if ( pText->pTabs )
                    LocalFree( (HANDLE)pText->pTabs );
    
                LocalUnlock( hText );
                LocalFree( hText );
            }
        }
        break;

        case WM_PAINT:
            BeginPaint( hWnd, &ps );
            wdbPaint ( hWnd, ps.hdc );
            EndPaint( hWnd, &ps );
        break;

        case WM_SETFONT:
            wdbSetFont( hWnd, (HFONT)wParam );
            return ( 0L );

        case WM_GETFONT:
            return (LONG)( wdbSetFont( hWnd, NULL ) );

        case WDBM_SETNLINES:
            return ( 0L );

        case WDBM_GETNLINES:
        {
            int     iMaxLines = 0;

            if ( pText = (NPWDBTEXT)LocalLock( hText ) )
            {
                iMaxLines = pText->iMaxLines;
                LocalUnlock( hText );
            }

            return ( iMaxLines );
        }



#if 0
        case WDBM_SETTABSTOPS:
            wdbSetTabs( hWnd, wParam, (LPINT)lParam );
            return ( 0L );

        case WDBM_GETNUMTABS:
            return ( pText->pTabs ? pText->nTabs : 0 );
        
        case WDBM_GETTABSTOPS:
            return ( (LONG)wdbGetTabs( hWnd, (LPINT)lParam ) );

        case WM_CHAR:
            wdbQueueChar( hText, wParam );
        break;
#endif

        case WDBM_SETOUTPUT:
            if ( pText = (NPWDBTEXT)LocalLock( hText ) )
            {
                lParam = (LONG)wdbSetOutput( pText->hWDB, wParam, (LPSTR)lParam );

                LocalUnlock( hText );
            }

            return ( lParam );


        case WDBM_GETOUTPUT:
            if ( pText = (NPWDBTEXT)LocalLock( hText ) )
            {
                lParam = (LONG)wdbGetInfo( pText->hWDB, WDBI_WOUTPUT );

                LocalUnlock( hText );
            }

            return ( lParam );


        case WDBM_CLEARWINDOW:
            wdbClearWindow( hWnd );
        break;
        
        case WM_SIZE:
            if ( pText = (NPWDBTEXT)LocalLock( hText ) )
            {
                /*  it is possible to get WM_SIZE's as a result of
                 *  messing with scrollbars... avoid race conditions
                 */
                if ( !pText->fScrollSemaphore )
                {
                    pText->fScrollSemaphore++;
                    wdbSetScrollRange( hWnd, TRUE );
                    wdbUpdateCursorPos( pText );
                    pText->fScrollSemaphore--;
                }

                LocalUnlock( hText );
            }
        break;

        case WM_VSCROLL:
        {
            WORD wScrollCode ;
            int  nPos ;

            wScrollCode = LOWORD( wParam ) ;
            #ifndef WIN32
            nPos = (int)LOWORD( lParam ) ;
            #else
            nPos = HIWORD( wParam ) ;
            #endif

            if ( pText = (NPWDBTEXT)LocalLock( hText ) )
            {
                switch ( wScrollCode )
                {
                    case SB_LINEDOWN:
                        wdbVertScroll( hWnd, pText, 1 );
                    break;

                    case SB_LINEUP:
                        wdbVertScroll( hWnd, pText, -1 );
                    break;

                    case SB_PAGEUP:
                        wdbVertScroll( hWnd, pText, -wdbLinesInWindow(hWnd) );
                    break;

                    case SB_PAGEDOWN:
                        wdbVertScroll( hWnd, pText, wdbLinesInWindow(hWnd) );
                    break;

                    case SB_THUMBTRACK:
                        wdbVertScroll( hWnd, pText, nPos - pText->iTop );
                    break;

                    case SB_THUMBPOSITION:
                        wdbVertScroll( hWnd, pText, nPos - pText->iTop );

                        /** fall through **/
        
                    case SB_ENDSCROLL:
                        wdbSetScrollRange( hWnd, TRUE );
                        wdbUpdateCursorPos( pText );
                    break;
                }

                LocalUnlock( hText );
            }
        }
        break;

        case WM_HSCROLL:
        {
            WORD wScrollCode ;
            int  nPos ;

            wScrollCode = LOWORD( wParam ) ;
            #ifndef WIN32
            nPos = (int)LOWORD( lParam ) ;
            #else
            nPos = HIWORD( wParam ) ;
            #endif

            if ( pText = (NPWDBTEXT)LocalLock( hText ) )
            {
                switch ( wScrollCode )
                {
                    case SB_LINEDOWN:
                        wdbHorzScroll( hWnd, pText, 1 );
                    break;

                    case SB_LINEUP:
                        wdbHorzScroll( hWnd, pText, -1 );
                    break;

                    case SB_PAGEUP:
                        wdbHorzScroll( hWnd, pText, -wdbCharsInWindow(hWnd) );
                    break;

                    case SB_PAGEDOWN:
                        wdbHorzScroll( hWnd, pText, wdbCharsInWindow(hWnd) );
                    break;

                    case SB_THUMBTRACK:
                        wdbHorzScroll( hWnd, pText, nPos - pText->iLeft );
                    break;

                    case SB_THUMBPOSITION:
                        wdbHorzScroll( hWnd, pText, nPos - pText->iLeft );

                        /** fall through **/
        
                    case SB_ENDSCROLL:
                        wdbSetScrollRange( hWnd, TRUE );
                        wdbUpdateCursorPos( pText );
                    break;
                }

                LocalUnlock( hText );
            }
        }
        break;

        /*  handle focus messages to hide and show the caret
         *  if the WDB window allows for character input.
         */
        case WM_SETFOCUS:
            if ( pText = (NPWDBTEXT)LocalLock( hText ) )
            {
                if ( pText->hInQueue )
                {
                    CreateCaret( hWnd, 0, 1, pText->dyFont );
                    wdbUpdateCursorPos( pText );
                    ShowCaret( hWnd );
                }

                LocalUnlock( hText );
            }
        break;

        case WM_KILLFOCUS:
            if ( pText = (NPWDBTEXT)LocalLock( hText ) )
            {
                if ( pText->hInQueue )
                    DestroyCaret();

                LocalUnlock( hText );
            }
        break;

        /* allow keyboard scrolling */
        case WM_KEYDOWN:
            switch ( wParam )
            {
                case VK_UP:
                    PostMessage( hWnd, WM_VSCROLL, SB_LINEUP, 0L );
                break;

                case VK_DOWN:
                    PostMessage( hWnd, WM_VSCROLL, SB_LINEDOWN, 0L );
                break;

                case VK_PRIOR:                     
                    PostMessage( hWnd, WM_VSCROLL, SB_PAGEUP, 0L );
                break;

                case VK_NEXT:
                    PostMessage( hWnd, WM_VSCROLL, SB_PAGEDOWN, 0L );
                break;

                case VK_HOME:
                    PostMessage( hWnd, WM_HSCROLL, SB_PAGEUP, 0L );
                break;

                case VK_END:
                    PostMessage( hWnd, WM_HSCROLL, SB_PAGEDOWN, 0L );
                break;

                case VK_LEFT:
                    PostMessage( hWnd, WM_HSCROLL, SB_LINEUP, 0L );
                break;

                case VK_RIGHT:
                    PostMessage( hWnd, WM_HSCROLL, SB_LINEDOWN, 0L );
                break;
            }
        break;


        case WM_KEYUP:
            switch ( wParam )
            {
#if 0
                case VK_F3:
                    wdbQueueChar( hText, VK(wParam) );
                break;
#endif

                /*  send endscroll when the key goes up--allows for
                 *  type-a-matic action.
                 */
                case VK_UP:
                case VK_DOWN:
                case VK_PRIOR:
                case VK_NEXT:
                    PostMessage( hWnd, WM_VSCROLL, SB_ENDSCROLL, 0L );
                break;

                case VK_HOME:
                case VK_END:
                case VK_LEFT:
                case VK_RIGHT:
                    PostMessage( hWnd, WM_HSCROLL, SB_ENDSCROLL, 0L );
                break;
            }
        break;

        default:
            return ( DefWindowProc( hWnd, uiMessage, wParam, lParam ) );
    }

    return ( 0L );
} /* wdbWndProc() */


/** BOOL FAR PASCAL wdbInitWindow( HANDLE hInstance )
 *
 *  DESCRIPTION: 
 *      Registers the WDB window class if it has not been.
 *
 *  ARGUMENTS:
 *      HANDLE hInstance    :   Instance handle to use for class regis-
 *                              tration.
 *
 *  RETURN (BOOL):
 *      The return value is TRUE if the class registration was successful,
 *      or was already registered.  It is FALSE if the class can not be
 *      registered.
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbInitWindow( HANDLE hInstance )
{
    WNDCLASS    rClass;

    /* if we're already registered, then no need to do this */
    if ( !gfInitWindow )
    {
        /* fill in our WDB window class structure */
        rClass.hCursor       = LoadCursor( NULL, IDC_ARROW );
        rClass.hIcon         = (HICON)LoadIcon( ghModule, "ICON_WDB" );
        rClass.lpszMenuName  = (LPSTR)NULL;
        rClass.lpszClassName = gszWindowClass;
      #ifndef WIN32
        rClass.hbrBackground = (HBRUSH)COLOR_WINDOW + 1;
      #else
        /* NT cleans up for you */
        rClass.hbrBackground = CreateSolidBrush( GetSysColor( COLOR_WINDOW ) ) ;
      #endif
        rClass.hInstance     = hInstance;
        rClass.style         = CS_GLOBALCLASS;
        rClass.lpfnWndProc   = wdbWndProc;
        rClass.cbWndExtra    = sizeof( HWDBTEXT );
        rClass.cbClsExtra    = 0;

        /* go for it! */
        if ( !RegisterClass( &rClass ) )
            return ( FALSE );

        /* success; set our init flag appropriately */
        gfInitWindow++;
    }

    /* you bet, made it! */
    return ( TRUE );
} /* wdbInitWindow() */


/** HWND FAR PASCAL wdbCreateWindow( HWND, HANDLE, DWORD, UINT, UINT,
 *                                    UINT, UINT, UINT, UINT )
 *
 *  DESCRIPTION: 
 *      This function creates a text output window.  WDB windows allow
 *      printf() style output and line oriented input.  WDB windows also
 *      remember a fixed number of lines of previous output for scrolling
 *      back.
 *
 *  ARGUMENTS:
 *      LPSESSION lpSession :   Specifies the session for the new window.
 * 
 *      HANDLE hInst    :   Specifies the module instance handle of the
 *                          DLL owner.  If the argument is NULL, the module
 *                          instance handle of the WDB.DLL is used.
 * 
 *      LPSTR lpszTitle :   Points to the window title.  This information
 *                          is ignored when the style specified by dwStyle
 *                          does not create a title bar for the window.
 * 
 *      DWORD dwStyle   :   Specifies the window style flags.  All standard
 *                          window style flags are valid.  The WDB window
 *                          class also defines the following additional
 *                          flags:
 * 
 *                          WDB_CHARINPUT:  The WDB window allows the user
 *                                          to input characters and sends its
 *                                          parent WDB_NCHAR and WDB_NTEXT
 *                                          messages.
 * 
 *      UINT x          :   Specifies the x position of the window.
 * 
 *      UINT y          :   Specifies the y position of the window.
 * 
 *      UINT dx         :   Specifies the width of the window.
 * 
 *      UINT dy         :   Specifies the height of the window.
 * 
 *      UINT wID        :   Specifies the window ID of the WDB window.  This
 *                          code is used in WM_COMMAND message to notify the
 *                          owner of the WDB window when key events have
 *                          occurred.
 *
 *  RETURN (HWND):
 *      The return value is the handle to the new WDB window.  It is NULL
 *      if the window could not be created.  The returned window handle
 *      may be used with the normal Windows window-management API's.
 *
 *  NOTES:
 *      A WDB window behaves like a partial Windows control.  The owner
 *      may change the parameters of a WDB window by sending control
 *      messages (including WM_SETFONT, WM_GETFONT, and the other WDB
 *      control messages documented below).  The WDB window notifies
 *      its owner of state changes by sending the owner WM_COMMAND
 *      messages with a control ID of wID.  WDB windows are not full
 *      controls, however, as they can not be used in dialog boxes.  WDB
 *      windows also do not respond to WM_GETTEXT and WM_SETTEXT messages.
 *
 *      Below is a list of control messages that WDB windows listen to:
 *
 *          WM_GETFONT          gets the current font in use by WDB
 *          WM_SETFONT          sets the font used by WDB
 *
 *          WDBM_SETNLINES      not implemented
 *          WDBM_GETNLINES      returns number of lines being buffered
 *          WDBM_SETTABSTOPS    not implemented
 *          WDBM_GETTABSTOPS    not implemented
 *          WDBM_GETNUMTABS     not implemented
 *          WDBM_SETOUTPUT      sets the output location to another device
 *          WDBM_GETOUTPUT      returns the current output device
 *          WDBM_CLEARWINDOW    clears all lines for history buffer
 *
 ** cjp */

HWND FAR PASCAL wdbCreateWindow( LPWDBSESSION lpSession, HANDLE hInst,
                                 LPSTR lpszTitle, DWORD dwStyle,
                                 UINT x, UINT y, UINT dx, UINT dy,
                                 UINT wID )
{
    HWND    hWnd;
    HWDB    hWDB;

    /* return NULL if the class can not be registered */
    if ( !gfInitWindow && !wdbInitWindow( ghModule ) )
        return ( NULL );

    /* if this is for the default WDB session... */
    if ( lpSession == (LPWDBSESSION)&gWDBDefSession )
        hWDB = NULL;

    /* otherwise, get the handle for lpSession */
    else
#ifndef WIN32
      if ( !(hWDB = LOWORD( GlobalHandle( HIWORD(lpSession) ) )) )
        return ( NULL );
#else
      if ( !(hWDB = GlobalHandle( (LPVOID)lpSession )) )
        return ( NULL );
#endif 

    /* use WDB's module handle if user didn't give us one */
    if ( !hInst )
        hInst = ghModule;
  

    /*  If lpSession->hWnd is not NULL, should there be a check to ensure
     *  that the hWnd IS valid (IsWindow()).  And if so, what should we
     *  do if it is NOT valid? ...open with a NULL handle?  This way, 
     *  WDB would still throw out some output even if the 'parent'
     *  window failed to open and the app didn't check it before calling
     *  WDB...
     */

    OD( "WDB: CreateWindow() BEFORE\r\n" );

    /* attempt to create a new WDB window */
    hWnd = CreateWindow( (LPSTR)gszWindowClass,
                         (LPSTR)lpszTitle,
                         dwStyle,
                         x,
                         y,
                         dx,
                         dy,
                         (HWND)lpSession->hWnd,
                         (HMENU)NULL,
                         (HANDLE)hInst,
                         (LPSTR)MAKELONG( hWDB, wID )
                       );

    OD( "WDB: CreateWindow() AFTER\r\n" );

    /* change the font */
    if ( hWnd )  wdbChangeFont( lpSession, hWnd );

    /* return handle to newly created window */
    return ( hWnd );
} /* wdbCreateWindow() */


/** void wdbKillFont( LPWDBSESSION lpSession )
 *
 *  DESCRIPTION: 
 *      This function kills the font currently associated with lpSession.
 *
 *  ARGUMENTS:
 *      ( LPWDBSESSION lpSession )
 *
 *  RETURN (void):
 *
 *
 *  NOTES:
 *
 ** cjp */

void wdbKillFont( LPWDBSESSION lpSession )
{
    /* is there really a font here? */
    if ( lpSession->hFont )
    {
        /*  Because wdbReallyCreateFont() ALWAYS does a CreateFontIndirect()
         *  we must ALWAYS DeleteObject().  No special casing... this is
         *  why the if () is commented out.  It would not hurt anything if
         *  it was still here--it would just always be !=...
         */

        /* don't delete the system stock font */
//        if ( lpSession->hFont != GetStockObject( SYSTEM_FONT ) )
        {
            /* get rid 'o the thing */
            DeleteObject( lpSession->hFont );
        }

        /* invalidate the font handle */
        lpSession->hFont = NULL;
    }
} /* wdbKillFont() */


/** void wdbChangeFont( LPWDBSESSION lpSession, HWND hWnd )
 *
 *  DESCRIPTION: 
 *      This function changes the current font in the WDB window.
 *
 *  ARGUMENTS:
 *      LPWDBSESSION lpSession  :   Session info pointer.
 *
 *      HWND hWnd               :   Handle to output window to change the
 *                                  font in.
 *
 *  RETURN (void):
 *      The font will have been changed.
 *
 *  NOTES:
 *
 ** cjp */

void wdbChangeFont( LPWDBSESSION lpSession, HWND hWnd )
{
    /* select the font that lpSession has selected into it */
    if ( hWnd )
    {
        /* do we already have a font?  kill it if so... */
        wdbKillFont( lpSession );

        /* attempt to get something close to what the user wants */
        lpSession->hFont = wdbReallyCreateFont( NULL,
                                            lpSession->wdbConfig.szFontName,
                                            lpSession->wdbConfig.wFontSize,
                                            lpSession->wdbConfig.wFontBold ?
                                                RCF_BOLD : RCF_NORMAL );


#if 0
        /*  This really shouldn't be necessary... In other words, this
         *  code should never be executed.  wdbReallyCreateFont() will
         *  (should) ALWAYS succeed.  Its default is SYSTEM_FONT.
         *
         *  The other thing that wdbReallyCreateFont() does is ALWAYS
         *  CreateFontIndirect() so we don't have to special case the
         *  DeleteObject() in wdbKillFont()...
         */

        /* if we didn't get a font, use the system fixed font */
        if ( !lpSession->hFont )
        {
            lpSession->hFont = GetStockObject( SYSTEM_FONT );

            /* system font--.INI face name not found or something */
            lstrcpy( lpSession->wdbConfig.szFontName, gszArgSystem );
        }
#endif

        /* get it selected to the window NOW */
        SendMessage( hWnd, WM_SETFONT, (WPARAM)lpSession->hFont, 0 );
    }
} /* wdbChangeFont() */


/** HWND FAR PASCAL wdbOpenWindow( LPWDBSESSION lpSession )
 *
 *  DESCRIPTION: 
 *      This function is responsible for opening the output window so
 *      information can be displayed.
 *
 *  ARGUMENTS:
 *      LPWDBSESSION lpSession  :   Pointer to WDB session.
 *
 *  RETURN (HWND NEAR PASCAL):
 *      The return value is the window handle for the new window.  It
 *      is NULL if the window could not be created.
 *
 *  NOTES:
 *
 ** cjp */

HWND FAR PASCAL wdbOpenWindow( LPWDBSESSION lpSession )
{
    HWND    hWnd;
    int   dy;
    char    szTitle[ 80 ];

    /* need this for computing the window size and position */
    dy = GetSystemMetrics( SM_CYSCREEN );

    /* create a message box title */
    wsprintf( szTitle, "<%s%s>",
                lpSession->wdbConfig.szPrelude, (LPSTR)gszWindowTitle );

    /* attempt to create the WDB window */
    hWnd = wdbCreateWindow( lpSession,
                            ghModule,
                            szTitle,
                            WS_OVERLAPPEDWINDOW | WS_VSCROLL |
                                WS_VISIBLE | WS_HSCROLL,
                            lpSession->wdbConfig.nWindowX,
                            lpSession->wdbConfig.nWindowY,
                            lpSession->wdbConfig.wWindowW,
                            lpSession->wdbConfig.wWindowH,
                            0 );

    /* return handle to window--NULL if failure */
    return ( hWnd );
} /* wdbOpenWindow() */


/** HWND FAR PASCAL wdbCloseWindow( LPWDBSESSION lpSession )
 *
 *  DESCRIPTION: 
 *      This function is used to destroy the output window--safely?
 *
 *  ARGUMENTS:
 *      LPWDBSESSION lpSession  :   Pointer to WDB session.
 *
 *  RETURN (HWND FAR PASCAL):
 *      The return value is NULL to encourage the caller to invalidate
 *      the hWnd handle--cuz it will no longer be valid.
 *
 *  NOTES:
 *
 ** cjp */

HWND FAR PASCAL wdbCloseWindow( LPWDBSESSION lpSession )
{
    /* make sure it is a valid window handle */
    if ( IsWindow( lpSession->hOutput ) )
    {
        DestroyWindow( lpSession->hOutput );
    }

    /* return NULL to invalidate handle */
    return ( NULL );
} /* wdbCloseWindow() */


/** EOF: wdbwnd.c **/
