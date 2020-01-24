/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  write.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  IBMPC Terminal Emulation (write routines)
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.002  2/28/91 baw   Moved to self-contained DLL
 *     01.00.010  5/ 4/91 baw   Added features
 *     01.00.012  8/10/91 baw   Added a flag for the ATTRIBUTE_OEMCHAR
 *     01.00.013  9/29/91 baw   Converted for IBMPC.EMU
 *
 ************************************************************************/

#include "precomp.h"

#include "ibmpc.h"

/************************************************************************
 *  BOOL WriteEmulatorBlock( HWND hWnd, LPSTR lpBlock, int nLen )
 *
 *  Description:
 *     This function is the IBMPC interface.  This provides all
 *     functionality of the IBMPC emulator.  It is byte driven
 *     because of the CONTROL SEQUENCES.  Although it is byte
 *     driven, information comes in blocks and therefore the
 *     for loop.
 *
 *  Comments:
 *     5/19/91  baw   No such thing as WriteEmulatorByte() anymore.
 *                    It was kinda stupid... For each call, it had
 *                    to GetWindowWord(), LockHandles() etc. DUMB!
 *                    But not anymore!
 *
 *     8/ 7/91  baw   Found the damned bug that was causing problems
 *                    with the DEC Call-Management-System.
 *                    The culprit: an early assumption that control
 *                    characters were 0x20 and below... not TRUE!
 *                    Control characters are in the ranges [0, 0x20) &
 *                    (0x7F, 0xFF].
 *
 ************************************************************************/

BOOL WriteEmulatorBlock( HWND hWnd, LPSTR lpBlock, int nLen )
{
   int          i ;
   BOOL         fCursorWasShowing ;
   BYTE         bByte ;
   NPECB        npECB ;
   LOCALHANDLE  hECB ;

   // Get Emulator Control Block
   hECB = GETECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   fCursorWasShowing = HideEmulatorCursor( hWnd ) ;

   for (i = 0; i < nLen; i++)
   {
      bByte = lpBlock[i] ;

      switch (SEQUENCESTATE)
      {
         case SEQUENCE_ESCAPE:
            EscapeSequence( hWnd, npECB, bByte ) ;
            break ;

         case SEQUENCE_CONTROL:
            ControlSequence( hWnd, npECB, bByte ) ;
            break ;
   
         default:
            if (bByte >= 0x20 && bByte <= 0x7F)
               StandardCharacter( hWnd, npECB, bByte ) ;
            else
               ControlCharacter( hWnd, npECB, bByte ) ;
      }
   }
   UpdateWindow( hWnd ) ;
   if (fCursorWasShowing)
      ShowEmulatorCursor( hWnd ) ;

   LocalUnlock( hECB ) ;
   return ( TRUE ) ;

} /* end of WriteEmulatorBlock() */

/************************************************************************
 *  VOID NEAR StandardCharacter( HWND hWnd, NPECB npECB, BYTE bByte )
 *
 *  Description:
 *     This writes a "standard" character to the screen.  There is
 *     no parsing of control sequences for this character.
 *
 *  Comments:
 *      5/18/91  baw  Pulled from WriteEmulatorByte()
 *
 ************************************************************************/

VOID NEAR StandardCharacter( HWND hWnd, NPECB npECB, BYTE bByte )
{
   WORD  wGraphic ;

   // check for graphics status here

   wGraphic = (bByte > 0x7F || bByte < 0x20) ? ATTRIBUTE_OEMCHAR : 0 ;

   // output single character to buffer

   *(LPWORD) (ATTRBUF + CURROW * MAXCOLS + CURCOL) = (ATTR | wGraphic) ;
   *(LPSTR) (CHARBUF + CURROW * MAXCOLS + CURCOL) = bByte ;

   PaintRun( npECB, SCREENDC, (ATTR | wGraphic), CURROW, CURCOL, 1, FALSE ) ;

   // perform line wrap
       
   if (CURCOL < (MAXCOLS - 1))
      CURCOL++ ;
   else if (AUTOWRAP)
   {
      ControlCharacter( hWnd, npECB, ASCII_CR ) ;
      ControlCharacter( hWnd, npECB, ASCII_LF ) ;
   }

} /* end of StandardCharacter() */

/************************************************************************
 *  VOID NEAR ControlCharacter( HWND hWnd, NPECB npECB, BYTE bByte )
 *
 *  Description: 
 *     This character was determined to be a control character
 *     (e.g. < 0x20 ).  Proper action is taken on appropriate
 *     characters.
 *
 *  Comments:
 *      5/18/91  baw  Pulled from WriteEmulatorByte()
 *      8/ 7/91  baw  Actually, the control characters are < 0x20 and
 *                    > 0x7F.
 *
 ************************************************************************/

VOID NEAR ControlCharacter( HWND hWnd, NPECB npECB, BYTE bByte )
{
   switch (bByte)
   {
      case ASCII_NUL:
      case ASCII_STX:
      case ASCII_ETX:
         break ;

      case ASCII_ENQ:
         if (NULL != ANSWERBACK[0])
            SendMessage( GetParent( hWnd ), WM_CMD_WRITEBLK,
                         (WORD) lstrlen( ANSWERBACK ),
                         (LONG) (LPSTR) ANSWERBACK ) ;
         break ;

      case ASCII_BEL:
         // bell
         MessageBeep( 0 ) ;
         break ;

      case ASCII_BS:
         // backspace
         if (CURCOL > 0)
         {
            CURCOL -= 1 ;

            // check for destructive BS here
            StandardCharacter( hWnd, npECB, ' ' ) ;

            CURCOL -= 1 ;
         }
         break ;

      case ASCII_HT:
         // tab
         do
         {
            StandardCharacter( hWnd, npECB, ' ' ) ;
         }
         while (!GetTabState(npECB, CURCOL) && (CURCOL < (MAXCOLS - 1))) ;
         break ;

      case ASCII_CR:
         // carriage return
         CURCOL = 0 ;

         // check for CR/LF and don't break
         break ;

      case ASCII_LF:
         // linefeed

         if (CURROW++ == MAXROWS - 1)
         {
            CURROW-- ;
            ScrollRegionUp( hWnd, npECB, 0, MAXROWS - 1, 1 ) ;
         }
         if (NEWLINE)
            CURCOL = 0 ;
         break ;

      case ASCII_FF:
      {
         int  i ;

         CURCOL = 0 ;
         CURROW = 0 ;
         _fmemset( (LPSTR) CHARBUF, ' ', MAXROWS * MAXCOLS ) ;
         for (i = 0; i < MAXROWS * MAXCOLS; i++)
            *(LPWORD)(ATTRBUF + i) = ATTRIBUTE_DEFAULT ;
         InvalidateRect( hWnd, NULL, FALSE ) ;
      }
      break ;

      case ASCII_ESC:
         SEQUENCESTATE = SEQUENCE_ESCAPE ;
         break ;

      default:
         StandardCharacter( hWnd, npECB, bByte ) ;
         break ;
   }

} /* end of ControlCharacter() */

/************************************************************************
 *  VOID NEAR ControlSequence( HWND hWnd, NPECB npECB, BYTE bByte )
 *
 *  Description: 
 *     A control sequence been detected.  This handles the subsequent
 *     characters and changes the state as necessary.
 *
 *  Comments:
 *      5/18/91  baw  Moved from WriteEmulatorByte()
 *
 ************************************************************************/

VOID NEAR ControlSequence( HWND hWnd, NPECB npECB, BYTE bByte )
{
   if (isdigit( bByte ))
   {
      CONTROLSEQ( lstrlen( (LPSTR) &CONTROLSEQ( 0 ) ) ) = bByte ;
      return ;
   }
   PARAM( PARAMCOUNT++ ) = atoi( &CONTROLSEQ( 0 ) ) ;
   _fmemset( (LPSTR) &CONTROLSEQ( 0 ), NULL, MAXLEN_CONTROLSEQ ) ;

   switch (bByte)
   {
      case ';':
         return ;

      case ASCII_CAN:
      case ASCII_SUB:
         break ;

      case 'A':
         CURROW = max( CURROW - max( 1, PARAM(0) ), 0 ) ;
         break ;

      case 'B':
         CURROW = min( MAXROWS - 1, CURROW + max( 1, PARAM(0) ) ) ;
         break ;

      case 'C':
         CURCOL = min( MAXCOLS - 1, CURCOL + max( 1, PARAM(0) ) ) ;
         break ;

      case 'D':
         CURCOL = max( CURCOL - max( 1, PARAM(0) ), 0 ) ;
         break ;

      case 'f':
      case 'H':
         CURROW = min( MAXROWS - 1, max( 0, PARAM(0) - 1 ) ) ;
         CURCOL = min( MAXCOLS - 1, max( 0, PARAM(1) - 1 ) ) ;
         break ;

      case 'J':
      {
         int  i ;

         switch (PARAM(0))
         {
            case 2:
               _fmemset( (LPSTR) CHARBUF, ' ', MAXROWS * MAXCOLS ) ;
               for (i = 0; i < MAXROWS * MAXCOLS; i++)
                  *(LPWORD)(ATTRBUF + i) = ATTRIBUTE_DEFAULT ;
               InvalidateRect( hWnd, NULL, FALSE ) ;
               break ;
         }
      }
      break ;

      case 'K':
      {
         // Erase in Line (EL)

         int   i ; 
         RECT  rcErase ;

         // Standard erase

         switch (PARAM(0))
         {
            case 0:
               _fmemset( (LPSTR) (CHARBUF + CURROW * MAXCOLS + CURCOL),
                         ' ',
                         MAXCOLS - CURCOL ) ;
               for (i = 0; i < MAXCOLS - CURCOL; i++)
                  *(LPWORD)(ATTRBUF + CURROW * MAXCOLS + CURCOL + i) =
                     ATTRIBUTE_DEFAULT ;
               rcErase.top = CURROW * YCHAR - YOFFSET ;
               rcErase.bottom = rcErase.top + YCHAR ;
               rcErase.left = CURCOL * XCHAR - XOFFSET ;
               rcErase.right = MAXCOLS * XCHAR - XOFFSET ;
               InvalidateRect( hWnd, &rcErase, FALSE ) ;
               break ;
         }
      }
      break ;

      case 's':
         SAVEROW = CURROW ;
         SAVECOL = CURCOL ;
         break ;

      case 'u':
         CURROW = SAVEROW ;
         CURCOL = SAVECOL ;
         break ;

      case 'm':
      {
         int   i ;

         for  (i = 0; i < PARAMCOUNT; i++)
         {
            switch (PARAM(i))
            {
               case 0:
                  // cancel all attributes
                  ATTR = ATTRIBUTE_DEFAULT ;
                  break ;

               case 1:
                  // bold
                  ATTR = ATTR | ATTRIBUTE_INTENSE ;
                  break ;

               case 4:
                  // underline
                  ATTR = ATTR | ATTRIBUTE_UNDERLINE ;
                  break ;

               case 5:
                  // blinking
                  ATTR = ATTR | ATTRIBUTE_BLINKING ;
                  break ;

               case 7:
                  ATTR = ATTR | ATTRIBUTE_INVERSE ;
                  break ;

               // foreground color

               case 30:
               case 31:
               case 32:
               case 33:
               case 34:
               case 35:
               case 36:
               case 37:
                  ATTR = (ATTR & 0xFF0F) | (PARAM(i) - 30 << 4) ;
                  break ;

               // background color

               case 40:
               case 41:
               case 42:
               case 43:
               case 44:
               case 45:
               case 46:
               case 47:
                  ATTR = (ATTR & 0xFFF0) | (PARAM(i) - 40) ;
                  break ;
            }
         }
      }
      break ;

      case 'n':
      {
         char szBuf[ MAXLEN_DEVATTR ] ;

         switch (PARAM(0))
         {
            case 6:
               wsprintf( szBuf, "%c[%d;%dR", ASCII_ESC, CURROW + 1,
                         CURCOL + 1 ) ;
               SendMessage( GetParent( hWnd ), WM_CMD_WRITEBLK,
                            (WORD) lstrlen( szBuf ), (LONG) (LPSTR) szBuf ) ;
               break ;
         }
      }
      break ;

   }
   SEQUENCESTATE = SEQUENCE_NONE ;

} /* end of ControlSequence() */

/************************************************************************
 *  VOID NEAR EscapeSequence( HWND hWnd, NPECB npECB, BYTE bByte )
 *
 *  Description:
 *     An escape has been detected.  This handles the subsequent
 *     characters and changes the state as necessary.
 *
 *  Comments:
 *      5/18/91  baw  Moved from WriteEmulatorByte()
 *
 ************************************************************************/

VOID NEAR EscapeSequence( HWND hWnd, NPECB npECB, BYTE bByte )
{
   switch (bByte)
   {
      case ASCII_CAN:
         break ;

      case '[':
         SEQUENCESTATE = SEQUENCE_CONTROL ;
         ResetControlSeqParams( npECB ) ;
         return ;
   }
   SEQUENCESTATE = SEQUENCE_NONE ;

} /* end of EscapeSequence() */

/************************************************************************
 *  VOID NEAR ResetControlSeqParams( NPECB npECB )
 *
 *  Description: 
 *     Resets control sequence parameters.
 *
 *  Comments:
 *      5/20/91  baw  Pulled from WriteEmulatorByte()
 *
 ************************************************************************/

VOID NEAR ResetControlSeqParams( NPECB npECB )
{
   int  i ;

   PARAMCOUNT = 0 ;
   _fmemset( (LPSTR) &CONTROLSEQ( 0 ), NULL, MAXLEN_CONTROLSEQ ) ;
   for (i = 0; i < MAXLEN_PARAMS; i++)
      PARAM( i ) = 0 ;

} /* end of ResetControlSeqParams() */

/************************************************************************
 *  VOID ScrollRegionUp( HWND hWnd, NPECB npECB,
 *                       int nRegionTop, nRegionBottom,
 *                       int nLines)
 *
 *  Description:
 *     Scrolls the character and attribute buffers up.  The bottom line
 *     is blanked with spaces and attributes are set to 0.
 *
 *  Comments:
 *      5/18/91  baw  Pulled from WriteEmulatorByte()
 *      5/26/91  baw  Physically scrolls screen DC now.  It was
 *                    invalidating the ENTIRE screen rect.
 *                    How STUPID!  Oh well, we live learn!
 *
 ************************************************************************/

VOID ScrollRegionUp( HWND hWnd, NPECB npECB,
                     int nRegionTop, int nRegionBottom,
                     int nLines )
{
   int   i, nStartRow, nEndRow, nStartCol, nEndCol ;
   BOOL  fShowCursor ;
//   HDC   hDC ;
   RECT  rcClient, rcScroll, rcInvalid ;

   // Update the window before scrolling

   UpdateWindow( hWnd ) ;

   _fmemmove( (LPSTR) (CHARBUF + (nRegionTop * MAXCOLS)),
              (LPSTR) (CHARBUF + ((nRegionTop + nLines) * MAXCOLS)),
              (nRegionBottom - (nRegionTop + nLines) + 1) * MAXCOLS ) ;
   _fmemset( (LPSTR) (CHARBUF + (nRegionBottom - nLines + 1) * MAXCOLS),
             ' ', MAXCOLS * nLines ) ;
   _fmemmove( (LPSTR) (ATTRBUF + (nRegionTop * MAXCOLS)),
              (LPSTR) (ATTRBUF + ((nRegionTop + nLines) * MAXCOLS)),
              (nRegionBottom - (nRegionTop + nLines) + 1) *
                 MAXCOLS * sizeof( WORD ) ) ;
   for (i = 0; i < MAXCOLS * nLines; i++)
      *(LPWORD)(ATTRBUF + (nRegionBottom - nLines + 1) * MAXCOLS + i) =
         ATTRIBUTE_DEFAULT ;

   // compute actual visible portion of screen to scroll

   GetClientRect( hWnd, &rcClient ) ;
   nStartRow = max( nRegionTop, (rcClient.top + YOFFSET) / YCHAR ) ;
   nEndRow = min( nRegionBottom, (rcClient.bottom + YOFFSET) / YCHAR  - 1 ) ;
   nStartCol = max( 0, (rcClient.left + XOFFSET) / XCHAR ) ;
   nEndCol = min( MAXCOLS - 1, (rcClient.right + XOFFSET) / XCHAR - 1 ) ;

   // convert back to client coordinates

   rcScroll.top = nStartRow * YCHAR - YOFFSET ;
   rcScroll.bottom = nEndRow * YCHAR - YOFFSET + YCHAR ;
   rcScroll.left = nStartCol * XCHAR - XOFFSET ;
   rcScroll.right = nEndCol * XCHAR - XOFFSET + XCHAR ;

   // hide the caret ( if necessary )

   fShowCursor = HideEmulatorCursor( hWnd ) ;

   // scroll the rect

   ScrollDC( SCREENDC, 0, -(nLines * YCHAR), &rcScroll, &rcScroll,
             NULL, &rcInvalid ) ;

   // and show the cursor (if previously showing)

   if (fShowCursor)
      ShowEmulatorCursor( hWnd ) ;

   // invalidate new portion

   InvalidateRect( hWnd, &rcInvalid, FALSE ) ;

   // Force new portion of screen to clear

   UpdateWindow( hWnd ) ;

} /* end of ScrollRegionUp() */

/************************************************************************
 *  ScrollRegionDown( HWND hWnd, NPECB npECB,
 *                    int nRegionTop, int nRegionBottom,
 *                    int nLines )
 *
 *     Scrolls the character and attribute buffers down.  The top line
 *     is blanked with spaces and attributes are set to 0.
 *
 *  Comments:
 *      5/18/91  baw  Pulled from WriteEmulatorByte()
 *      5/26/91  baw  Physically scrolls screen DC now.  It was
 *                    invalidating the ENTIRE screen rect.
 *
 ************************************************************************/

VOID ScrollRegionDown( HWND hWnd, NPECB npECB,
                       int nRegionTop, int nRegionBottom,
                       int nLines )
{
   int   i, nStartRow, nEndRow, nStartCol, nEndCol ;
   BOOL  fShowCursor ;
//   HDC   hDC ;
   RECT  rcClient, rcScroll, rcInvalid ;

   // Update the window before scrolling

   UpdateWindow( hWnd ) ;

   _fmemmove( (LPSTR) (CHARBUF + ((nRegionTop + nLines) * MAXCOLS)),
              (LPSTR) (CHARBUF + (nRegionTop * MAXCOLS)),
              (nRegionBottom - (nRegionTop + nLines) + 1) * MAXCOLS ) ;
   _fmemset( (LPSTR) (CHARBUF + nRegionTop * MAXCOLS),
             ' ', MAXCOLS * nLines ) ;
   _fmemmove( (LPSTR) (ATTRBUF + ((nRegionTop + nLines) * MAXCOLS)),
              (LPSTR) (ATTRBUF + (nRegionTop * MAXCOLS)),
              (nRegionBottom - (nRegionTop + nLines) + 1) *
                 MAXCOLS * sizeof( WORD ) ) ;
   for (i = 0; i < MAXCOLS * nLines; i++)
      *(LPWORD)(ATTRBUF + nRegionTop * MAXCOLS + i) =
         ATTRIBUTE_DEFAULT ;

   // compute actual visible portion of screen to scroll

   GetClientRect( hWnd, &rcClient ) ;
   nStartRow = max( nRegionTop, (rcClient.top + YOFFSET) / YCHAR ) ;
   nEndRow = min( nRegionBottom, (rcClient.bottom + YOFFSET) / YCHAR - 1 ) ;
   nStartCol = max( 0, (rcClient.left + XOFFSET) / XCHAR ) ;
   nEndCol = min( MAXCOLS - 1, (rcClient.right + XOFFSET) / XCHAR - 1 ) ;

   // convert back to client coordinates

   rcScroll.top = nStartRow * YCHAR - YOFFSET ;
   rcScroll.bottom = nEndRow * YCHAR - YOFFSET + YCHAR ;
   rcScroll.left = nStartCol * XCHAR - XOFFSET ;
   rcScroll.right = nEndCol * XCHAR - XOFFSET + XCHAR ;

   // hide the caret ( if necessary )

   fShowCursor = HideEmulatorCursor( hWnd ) ;

   // scroll the rect

   ScrollDC( SCREENDC, 0, nLines * YCHAR, &rcScroll, &rcScroll,
             NULL, &rcInvalid ) ;

   // and show the cursor (if previously showing)

   if (fShowCursor)
      ShowEmulatorCursor( hWnd ) ;

   // invalidate new portion

   InvalidateRect( hWnd, &rcInvalid, FALSE ) ;

   // Force new portion of screen to clear

   UpdateWindow( hWnd ) ;

} /* end of ScrollRegionDown() */

/************************************************************************
 *  VOID WriteEmulatorError( HWND hWnd, WORD wParam, int nError )
 *
 *  Description:
 *     This is the error function called when the connector
 *     receives an error.  This function MUST be defined before
 *     the WriteEmulatorFunction() otherwise a compiler warning
 *     will occur.  The warning will occur because of the tricky
 *     prototyping that is done.
 *
 *  Comments:
 *      6/ 1/90  baw  Wrote this comment block.
 *
 ************************************************************************/

VOID WriteEmulatorError( HWND hWnd, int nError )
{
   WriteEmulatorFormat( hWnd, (LPSTR) "<ERR-%d>", nError ) ;

} /* end of WriteEmulatorError() */

/************************************************************************
 *  BOOL WriteEmulatorFormat( HWND hWnd, LPSTR lpszFmt, ... )
 *
 *  Description:
 *     Writes format to emulator window in printf() format.
 *
 *  Comments:
 *      6/20/91  baw  Wrote this comment.
 *
 ************************************************************************/

BOOL WriteEmulatorFormat( HWND hWnd, LPSTR lpszFmt, LPSTR lpszParams, ... )
{
   BYTE     abBlk[MAXLEN_FMTSTR] ;
   int      nLen ;

   nLen = wvsprintf( (LPSTR) abBlk, (LPSTR) lpszFmt, (LPSTR) &lpszParams ) ;
   return ( WriteEmulatorBlock( hWnd, abBlk, nLen ) ) ;

} /* end of WriteEmulatorFormat() */

/************************************************************************
 * End of File: write.c
 ************************************************************************/
