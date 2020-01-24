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
 *     Remarks:  VT220 Terminal Emulation (write routines)
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.002  2/28/91 baw   Moved to self-contained DLL
 *     01.00.010  5/ 4/91 baw   Added features
 *     01.00.012  8/10/91 baw   Added a flag for the OEM_CHARSET
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "vt220.h"

/************************************************************************
 *  BOOL WriteEmulatorBlock( HWND hWnd, LPSTR lpBlock, int nLen )
 *
 *  Description:
 *     This function is the VT220 interface.  This provides all
 *     functionality of the VT220 emulator.  It is byte driven
 *     because of the CONTROL SEQUENCES.  Although it is byte
 *     driven, information comes in blocks and therefore the
 *     for loop.
 *
 *     This emulation will NOT support downloadable character sets.
 *     Why not, you ask?  Because a font would have to be created
 *     on the fly... NOT easy and NOT feasible - so there!
 *     Attitude borrowed from cjp (Curtis J. Palmer).
 *
 *  Comments:
 *     5/ 4/ 91  baw  Not a big performance hit anymore -
 *                    this is a NEAR call no prolog/epilog code
 *                    to worry about.
 *
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
   int    i ;
   BOOL   fCursorWasShowing ;
   BYTE   bByte ;
   NPECB  npECB ;
   HECB   hECB ;

   // Get Emulator Control Block
   hECB = GETHECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   fCursorWasShowing = HideEmulatorCursor( hWnd ) ;

   for (i = 0; i < nLen; i++)
   {
      bByte = lpBlock[i] ;

      // strip 8th bit if VT100 level compatibility

      if (COMPATIBILITYLEVEL( npECB ) == LEVEL_VT100)
         bByte &= 0x7F ;

#ifdef NOT_USED
      D(
         char szDebug[ 2 ] ;

         wsprintf( szDebug, "%c", bByte ) ;
         ODS( szDebug ) ;
      ) ;
#endif

      switch (SEQUENCESTATE( npECB ))
      {
         case SEQUENCE_ESCAPE:
            EscapeSequence( hWnd, npECB, bByte ) ;
            break ;
   
         case SEQUENCE_CONTROL:
            ControlSequence( hWnd, npECB, bByte ) ;
            break ;
   
         case SEQUENCE_LINEATTR:
            LineAttribute( hWnd, npECB, bByte ) ;
            break ;
   
         case SEQUENCE_CHARSET:
            SelectCharacterSet( hWnd, npECB, bByte ) ;
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
   int   nTemp ;
   BYTE  bAttr ;
   WORD  wTranslated ;

   if (SNGLSHFT( npECB ) != CHARSET_DEFAULT)
   {
      nTemp = DECGL( npECB ) ;
      DECGL( npECB ) = SNGLSHFT( npECB ) ;
   }

   wTranslated = TranslateEmulatorByte( npECB, bByte ) ;

   if (SNGLSHFT( npECB ) != CHARSET_DEFAULT)
   {
      DECGL( npECB ) = nTemp ;
      SNGLSHFT( npECB ) = CHARSET_DEFAULT ;
   }

   // output single character to buffer

   bAttr =
      (BYTE) *(LPSTR) (ATTRBUF( npECB ) +
                       CURROW( npECB ) * MAXCOLS( npECB ) +
                       CURCOL( npECB )) ;
   bAttr &= (BYTE) ATTRIBUTE_DBLSIZE ;
   bAttr |= ATTR( npECB ) ;

   if (SELERASE( npECB ))
      bAttr |= ATTRIBUTE_ERASEABLE ;
   if (LOBYTE( wTranslated ) != 0x00)
      bAttr |= ATTRIBUTE_OEMCHARSET ;

   *(LPSTR) (ATTRBUF( npECB ) +
             CURROW( npECB ) * MAXCOLS( npECB ) +
             CURCOL( npECB )) = bAttr ;
   *(LPSTR) (CHARBUF( npECB ) +
             CURROW( npECB ) * MAXCOLS( npECB ) +
             CURCOL( npECB )) = HIBYTE( wTranslated ) ;

   PaintRun( npECB, SCREENDC( npECB ), bAttr,
             CURROW( npECB ), CURCOL( npECB ), 1, FALSE ) ;

   // perform line wrap

   if (bAttr & (BYTE) (ATTRIBUTE_DBLSIZE))
   {
      if (CURCOL( npECB ) < (MAXCOLS( npECB ) / 2 - 1))
         CURCOL( npECB )++ ;
      else if (AUTOWRAP( npECB ))
      {
         ControlCharacter( hWnd, npECB, ASCII_CR ) ;
         ControlCharacter( hWnd, npECB, ASCII_LF ) ;
      }
   }
   else
   {
      if (CURCOL( npECB ) < (MAXCOLS( npECB ) - 1))
         CURCOL( npECB )++ ;
      else if (AUTOWRAP( npECB ))
      {
         ControlCharacter( hWnd, npECB, ASCII_CR ) ;
         ControlCharacter( hWnd, npECB, ASCII_LF ) ;
      }
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
      case ASCII_BEL:
         // bell
         MessageBeep( 0 ) ;
         break ;

      case ASCII_BS:
         // backspace
         if (CURCOL( npECB ) > 0)
            CURCOL( npECB ) -= 1 ;
         break ;

      case ASCII_DEL:
         // Delete character - ignored when received
         break ;

      case ASCII_HT:
         // tab
         do
         {
            StandardCharacter( hWnd, npECB, ' ' ) ;
         }
         while (!GetTabState( npECB, CURCOL( npECB ) ) &&
                 (CURCOL( npECB ) < (MAXCOLS( npECB ) - 1))) ;
         break ;

      case ASCII_CR:
         // carriage return
         CURCOL( npECB ) = 0 ;
         break ;

      case ASCII_VT:
      case ASCII_FF:
      case ASCII_LF:
         // linefeed

         if (CURROW( npECB ) > SCROLLREGIONBOTTOM( npECB ))
         {
            // out of scroll region
            if (CURROW( npECB )++ == MAXROWS( npECB ))
               CURROW( npECB )-- ;
         }
         else if (CURROW( npECB )++ == SCROLLREGIONBOTTOM( npECB ))
         {
            CURROW( npECB )-- ;
            ScrollRegionUp( hWnd, npECB, SCROLLREGIONTOP( npECB ),
                            SCROLLREGIONBOTTOM( npECB ), 1 ) ;
         }
         if (NEWLINE( npECB ))
            CURCOL( npECB ) = 0 ;
         break ;

      case ASCII_SI:
         // invoke G0 character set into GL
         DECGL( npECB ) = 0 ;
         break ;

      case ASCII_SO:
         // invoke G1 character set into GL
         DECGL( npECB ) = 1 ;
         break ;

      case ASCII_SS2:
         // single shift G2 into GL
         SNGLSHFT( npECB ) = 2 ;
         break ;

      case ASCII_SS3:
         // single shift G3 into GL
         SNGLSHFT( npECB ) = 3 ;
         break ;

      case ASCII_ESC:
         SEQUENCESTATE( npECB ) = SEQUENCE_ESCAPE ;
         SEQUENCEFLAG( npECB ) = SEQFLAG_NONE ;
         break ;

      case ASCII_CSI:
         SEQUENCESTATE( npECB ) = SEQUENCE_CONTROL ;
         SEQUENCEFLAG( npECB ) = SEQFLAG_NONE ;
         ResetControlSeqParams( npECB ) ;
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
      CONTROLSEQ( npECB, lstrlen( (LPSTR) &CONTROLSEQ( npECB, 0 ) ) ) = bByte ;
      return ;
   }
   PARAM( npECB, PARAMCOUNT( npECB )++ ) = atoi( &CONTROLSEQ( npECB, 0 ) ) ;
   _fmemset( (LPSTR) &CONTROLSEQ( npECB, 0 ), NULL, MAXLEN_CONTROLSEQ ) ;

   switch (bByte)
   {
      case ';':
         return ;

      case '?':
         SEQUENCEFLAG( npECB ) = SEQFLAG_QUESTION ;
         return ;

      case '"':
         SEQUENCEFLAG( npECB ) = SEQFLAG_QUOTE ;
         return ;

      case '!':
         SEQUENCEFLAG( npECB ) = SEQFLAG_BANG ;
         return ;

      case '>':
         SEQUENCEFLAG( npECB ) = SEQFLAG_GREATER ;
         return ;

      case 'A':
         if (ORIGIN( npECB ))
            CURROW( npECB ) =
               max( CURROW( npECB ) - max( 1, PARAM( npECB, 0 ) ),
                    SCROLLREGIONTOP( npECB ) ) ;
         else
            CURROW( npECB ) =
               max( CURROW( npECB ) - max( 1, PARAM( npECB, 0) ), 0 ) ;
         break ;

      case 'B':
         if (ORIGIN( npECB ))
            CURROW( npECB ) =
               min( SCROLLREGIONBOTTOM( npECB ),
                    CURROW( npECB ) + max( 1, PARAM( npECB, 0 ) ) ) ;
         else
            CURROW( npECB ) =
               min( MAXROWS( npECB ) - 1,
                    CURROW( npECB ) + max( 1, PARAM( npECB, 0 ) ) ) ;
         break ;

      case 'C':
         CURCOL( npECB ) =
            min( MAXCOLS( npECB ) - 1,
                 CURCOL( npECB ) + max( 1, PARAM( npECB, 0 ) ) ) ;
         break ;

      case 'D':
         CURCOL( npECB ) =
            max( CURCOL( npECB ) - max( 1, PARAM( npECB, 0 ) ), 0 ) ;
         break ;

      case 'H':
         switch (PARAMCOUNT( npECB ))
         {
            case 1:
               if (ORIGIN( npECB ))
                  CURROW( npECB ) =
                     min( SCROLLREGIONBOTTOM( npECB ),
                          max( SCROLLREGIONTOP( npECB ),
                               SCROLLREGIONTOP( npECB ) +
                                  PARAM( npECB, 0 ) - 1 ) ) ;
               else
                  CURROW( npECB ) =
                     min( MAXROWS( npECB ) - 1,
                          max( 0, PARAM( npECB, 0 ) - 1 ) ) ;
               CURCOL( npECB ) = 0 ;
               break ;

            case 2:
               if (ORIGIN( npECB ))
                  CURROW( npECB ) =
                     min( SCROLLREGIONBOTTOM( npECB ),
                          max( SCROLLREGIONTOP( npECB ),
                               SCROLLREGIONTOP( npECB ) +
                                  PARAM( npECB, 0 ) - 1 ) ) ;
               else
                  CURROW( npECB ) =
                     min( MAXROWS( npECB ) - 1,
                          max( 0, PARAM( npECB, 0 ) - 1 ) ) ;
               CURCOL( npECB ) =
                  min( MAXCOLS( npECB ) - 1,
                       max( 0, PARAM( npECB, 1 ) - 1 ) ) ;
               break ;
         }
         break ;

      case 'f':
         switch (PARAMCOUNT( npECB ))
         {
            case 2:
               if (ORIGIN( npECB ))
                  CURROW( npECB ) =
                     min( SCROLLREGIONBOTTOM( npECB ),
                          max( SCROLLREGIONTOP( npECB ),
                               SCROLLREGIONTOP( npECB ) +
                                  PARAM( npECB, 0 ) - 1 ) )  ;
               else
                  CURROW( npECB ) =
                     min( MAXROWS( npECB ) - 1,
                          max( 0, PARAM( npECB, 0 ) - 1 ) ) ;
               CURCOL( npECB ) =
                  min( MAXCOLS( npECB ) - 1,
                       max( 0, PARAM( npECB, 1 ) - 1 ) ) ;
               break ;

            default:
               if (ORIGIN( npECB ))
               {
                  CURROW( npECB ) = SCROLLREGIONTOP( npECB ) ;
                  CURCOL( npECB ) = 0 ;
               }
               else
               {
                  CURROW( npECB ) = 0 ;
                  CURCOL( npECB ) = 0 ;
               }
         }
         break ;

      case 'J':
         if (SEQUENCEFLAG( npECB ) == SEQFLAG_QUOTE)
         {
            // this should be implemented in assembly

            int  i ;

            switch (PARAM( npECB, 0 ))
            {
               case 0:
                  for (i = 0;
                       i < MAXROWS( npECB ) * MAXCOLS( npECB ) -
                           (CURROW( npECB ) * MAXCOLS( npECB ) +
                            CURCOL( npECB ) + 1);
                       i++)
                     if (*(LPSTR)(ATTRBUF( npECB ) +
                                  CURROW( npECB ) * MAXCOLS( npECB ) +
                                  CURCOL( npECB ) + i) &
                         ATTRIBUTE_ERASEABLE)
                     {
                        *(LPSTR)(CHARBUF( npECB ) +
                                 CURROW( npECB ) * MAXCOLS( npECB ) +
                                 CURCOL( npECB ) + i) = ' ' ;
                        *(LPSTR)(ATTRBUF( npECB ) +
                                 CURROW( npECB ) * MAXCOLS( npECB ) +
                                 CURCOL( npECB ) + i) = 0 ;
                     }
                  break ;

               case 1:
                  for (i = 0;
                       i < CURROW( npECB ) * MAXCOLS( npECB ) +
                           CURCOL( npECB ) + 1;
                       i++)
                     if (*(LPSTR)(ATTRBUF( npECB ) + i) & ATTRIBUTE_ERASEABLE)
                     {
                        *(LPSTR)(CHARBUF( npECB ) + i) = ' ' ;
                        *(LPSTR)(ATTRBUF( npECB ) + i) = 0 ;
                     }
                  break ;

               case 2:
                  for (i = 0; i < MAXROWS( npECB ) * MAXCOLS( npECB ); i++)
                     if (*(LPSTR)(ATTRBUF( npECB ) + i) & ATTRIBUTE_ERASEABLE)
                     {
                        *(LPSTR)(CHARBUF( npECB ) + i) = ' ' ;
                        *(LPSTR)(ATTRBUF( npECB ) + i) = 0 ;
                     }
                  break ;
            }
         }
         else
         {
            switch (PARAM( npECB, 0 ))
            {
               case 0:
                  _fmemset( (LPSTR) (CHARBUF( npECB ) +
                                     CURROW( npECB ) * MAXCOLS( npECB ) +
                                     CURCOL( npECB )),
                            ' ',
                            MAXROWS( npECB ) * MAXCOLS( npECB ) -
                            (CURROW( npECB ) * MAXCOLS( npECB ) +
                            CURCOL( npECB ) + 1) ) ;
                  _fmemset( (LPSTR) (ATTRBUF( npECB ) +
                                     CURROW( npECB ) * MAXCOLS( npECB ) +
                                     CURCOL( npECB )),
                            0,
                            MAXROWS( npECB ) * MAXCOLS( npECB ) -
                            (CURROW( npECB ) * MAXCOLS( npECB ) +
                            CURCOL( npECB ) + 1) ) ;
                  break ;
   
               case 1:
                  _fmemset( (LPSTR) CHARBUF( npECB ),
                            ' ',
                            CURROW( npECB ) * MAXCOLS( npECB ) +
                            CURCOL( npECB ) + 1) ;
                  _fmemset( (LPSTR) ATTRBUF( npECB ),
                            0,
                            CURROW( npECB ) * MAXCOLS( npECB ) +
                            CURCOL( npECB ) + 1) ;
                  break ;
   
               case 2:
                  _fmemset( (LPSTR) CHARBUF( npECB ), ' ',
                            MAXROWS( npECB ) * MAXCOLS( npECB ) ) ;
                  _fmemset( (LPSTR) ATTRBUF( npECB ), 0,
                            MAXROWS( npECB ) * MAXCOLS( npECB ) ) ;
                  break ;
            }
         }
         InvalidateRect( hWnd, NULL, FALSE ) ;
         break ;

      case 'K':
      {
         // Erase in Line (EL)

         RECT  rcErase ;

         if (SEQUENCEFLAG( npECB ) == SEQFLAG_QUOTE)
         {
            // Selective erase

            int  i ;

            switch (PARAM( npECB, 0 ))
            {
               case 0:
                  for (i = 0; i < MAXCOLS( npECB ) - CURCOL( npECB ); i++)
                     if (*(LPSTR)(ATTRBUF( npECB ) +
                                  CURROW( npECB ) * MAXCOLS( npECB ) +
                                  CURCOL( npECB ) + i) &
                         ATTRIBUTE_ERASEABLE)
                     {
                        *(LPSTR)(CHARBUF( npECB ) +
                                 CURROW( npECB ) * MAXCOLS( npECB ) +
                                 CURCOL( npECB ) + i) = ' ' ;
                        *(LPSTR)(ATTRBUF( npECB ) +
                                 CURROW( npECB ) * MAXCOLS( npECB ) +
                                 CURCOL( npECB ) + i) = 0 ;
                     }
                  rcErase.top = CURROW( npECB ) * YCHAR( npECB ) -
                                YOFFSET( npECB ) ;
                  rcErase.bottom = rcErase.top + YCHAR( npECB ) ;
                  rcErase.left = CURCOL( npECB ) * XCHAR( npECB ) -
                                 XOFFSET( npECB ) ;
                  rcErase.right = MAXCOLS( npECB ) * XCHAR( npECB ) -
                                  XOFFSET( npECB ) ;
                  break ;

               case 1:
                  for (i = 0; i < MAXCOLS( npECB ) - CURCOL( npECB ); i++)
                     if (*(LPSTR)(ATTRBUF( npECB ) +
                                  CURROW( npECB ) * MAXCOLS( npECB )) &
                         ATTRIBUTE_ERASEABLE)
                     {
                        *(LPSTR)(CHARBUF( npECB ) +
                                 CURROW( npECB ) * MAXCOLS( npECB ) + i) = ' ' ;
                        *(LPSTR)(ATTRBUF( npECB ) +
                                 CURROW( npECB ) * MAXCOLS( npECB ) + i) = 0 ;
                     }
                  rcErase.top = CURROW( npECB ) * YCHAR( npECB ) -
                                YOFFSET( npECB ) ;
                  rcErase.bottom = rcErase.top + YCHAR( npECB ) ;
                  rcErase.left = -XOFFSET( npECB ) ;
                  rcErase.right = CURCOL( npECB ) * XCHAR( npECB ) -
                                  XOFFSET( npECB ) ;
                  break ;

               case 2:
                  for (i = 0; i < MAXCOLS( npECB ); i++)
                     if (*(LPSTR)(ATTRBUF( npECB ) +
                                  CURROW( npECB ) * MAXCOLS( npECB ) + i) &
                         ATTRIBUTE_ERASEABLE)
                     {
                        *(LPSTR)(CHARBUF( npECB ) +
                                 CURROW( npECB ) * MAXCOLS( npECB ) + i) = ' ' ;
                        *(LPSTR)(ATTRBUF( npECB ) +
                                 CURROW( npECB ) * MAXCOLS( npECB ) + i) = 0 ;
                     }
                  rcErase.top =
                     CURROW( npECB ) * YCHAR( npECB ) - YOFFSET( npECB ) ;
                  rcErase.bottom =
                     rcErase.top + YCHAR( npECB ) ;
                  rcErase.left =
                     -XOFFSET( npECB ) ;
                  rcErase.right =
                     MAXCOLS( npECB ) * XCHAR( npECB ) - XOFFSET( npECB ) ;
                  break ;
            }
         }
         else
         {
            // Standard erase

            switch (PARAM( npECB, 0 ))
            {
               case 0:
                  _fmemset( (LPSTR) (CHARBUF( npECB ) +
                                     CURROW( npECB ) * MAXCOLS( npECB ) +
                                     CURCOL( npECB )),
                            ' ',
                            MAXCOLS( npECB ) - CURCOL( npECB ) ) ;
                  _fmemset( (LPSTR) (ATTRBUF( npECB ) +
                                     CURROW( npECB ) * MAXCOLS( npECB ) +
                                     CURCOL( npECB )),
                            0,
                            MAXCOLS( npECB ) - CURCOL( npECB ) ) ;
                  rcErase.top = CURROW( npECB ) * YCHAR( npECB ) -
                                YOFFSET( npECB ) ;
                  rcErase.bottom = rcErase.top + YCHAR( npECB ) ;
                  rcErase.left = CURCOL( npECB ) * XCHAR( npECB ) -
                                 XOFFSET( npECB ) ;
                  rcErase.right = MAXCOLS( npECB ) * XCHAR( npECB ) -
                                  XOFFSET( npECB ) ;
                  break ;
            
               case 1:
                  _fmemset( (LPSTR) (CHARBUF( npECB ) +
                                     CURROW( npECB ) * MAXCOLS( npECB )),
                            ' ',
                            MAXCOLS( npECB ) - CURCOL( npECB ) ) ;
                  _fmemset( (LPSTR) (ATTRBUF( npECB ) +
                                     CURROW( npECB ) * MAXCOLS( npECB )),
                            0,
                            MAXCOLS( npECB ) - CURCOL( npECB ) ) ;
                  rcErase.top =
                     CURROW( npECB ) * YCHAR( npECB ) - YOFFSET( npECB ) ;
                  rcErase.bottom =
                     rcErase.top + YCHAR( npECB ) ;
                  rcErase.left =
                     -XOFFSET( npECB ) ;
                  rcErase.right =
                     CURCOL( npECB ) * XCHAR( npECB ) - XOFFSET( npECB ) ;
                  break ;
   
               case 2:
                  _fmemset( (LPSTR) (CHARBUF( npECB ) +
                                     CURROW( npECB ) * MAXCOLS( npECB )),
                            ' ',
                            MAXCOLS( npECB ) ) ;
                  _fmemset( (LPSTR) (ATTRBUF( npECB ) +
                                     CURROW( npECB ) * MAXCOLS( npECB )),
                            0,
                            MAXCOLS( npECB ) ) ;
                  rcErase.top =
                     CURROW( npECB ) * YCHAR( npECB ) - YOFFSET( npECB ) ;
                  rcErase.bottom =
                     rcErase.top + YCHAR( npECB ) ;
                  rcErase.left =
                     -XOFFSET( npECB ) ;
                  rcErase.right =
                     MAXCOLS( npECB ) * XCHAR( npECB ) - XOFFSET( npECB ) ;
                  break ;
            }
         }
         InvalidateRect( hWnd, &rcErase, FALSE ) ;
      }
      break ;

      case 'L':
         // insert lines at cursor position only if in
         // the scrolling region (otherwise it's a NOP)

         if ((CURROW( npECB ) >= SCROLLREGIONTOP( npECB )) &&
             (CURROW( npECB ) <= SCROLLREGIONBOTTOM( npECB )))
            ScrollRegionDown( hWnd, npECB, CURROW( npECB ),
                              SCROLLREGIONBOTTOM( npECB ),
                              min( max( PARAM( npECB, 0 ), 1 ),
                                   SCROLLREGIONBOTTOM( npECB ) -
                                   CURROW( npECB ) + 1 ) ) ;
         break ;

      case 'M':
         // delete lines at cursor position only if in
         // the scrolling region (otherwise it's a NOP)

         if ((CURROW( npECB ) >= SCROLLREGIONTOP( npECB )) &&
             (CURROW( npECB ) <= SCROLLREGIONBOTTOM( npECB )))
            ScrollRegionUp( hWnd, npECB, CURROW( npECB ),
                            SCROLLREGIONBOTTOM( npECB ),
                            min( max( PARAM( npECB, 0 ), 1),
                                 SCROLLREGIONBOTTOM( npECB ) -
                                 CURROW( npECB ) + 1 ) ) ;
         break ;

      case 'P':
         // delete characters at current position

         ScrollLineLeft( hWnd, npECB, CURROW( npECB ), CURCOL( npECB ),
                         min( max( PARAM( npECB, 0 ), 1 ),
                              MAXCOLS( npECB ) - CURCOL( npECB ) ) ) ;
         break ;

      case '@':
         // insert characters at current position

         ScrollLineRight( hWnd, npECB, CURROW( npECB ), CURCOL( npECB ),
                          min( max( PARAM( npECB, 0 ), 1),
                               MAXCOLS( npECB ) - CURCOL( npECB ) ) ) ;
         break ;

      case 'X':
      {
         // Erase Character (ECH)

         RECT  rcErase ;

         _fmemset( (LPSTR) (CHARBUF( npECB ) +
                            CURROW( npECB ) * MAXCOLS( npECB ) +
                            CURCOL( npECB )),
                   ' ',
                   max( 1, min( PARAM( npECB, 0 ),
                   MAXCOLS( npECB ) - CURCOL( npECB ) ) ) ) ;
         _fmemset( (LPSTR) (ATTRBUF( npECB ) +
                            CURROW( npECB ) * MAXCOLS( npECB ) +
                            CURCOL( npECB )),
                   0,
                   max( 1, min( PARAM( npECB, 0 ),
                        MAXCOLS( npECB ) - CURCOL( npECB ) ) ) ) ;
         rcErase.top =
            CURROW( npECB ) * YCHAR( npECB ) - YOFFSET( npECB ) ;
         rcErase.bottom =
            rcErase.top + YCHAR( npECB ) ;
         rcErase.left =
            CURCOL( npECB ) * XCHAR( npECB ) - XOFFSET( npECB ) ;
         rcErase.right =
            CURCOL( npECB ) +
            max( 1, min( PARAM( npECB, 0), MAXCOLS( npECB ) - CURCOL( npECB ) ) ) *
            XCHAR( npECB ) - XOFFSET( npECB ) ;
         InvalidateRect( hWnd, &rcErase, FALSE ) ;
      }
      break ;

      case 'r':
         // set scrolling region

         SCROLLREGIONTOP( npECB ) =
            (PARAM( npECB, 0 ) > 0) ?
               (PARAM( npECB, 0 ) - 1) : 0 ;
         SCROLLREGIONBOTTOM( npECB ) =
            (PARAM( npECB, 1 ) > 0) ?
               (PARAM( npECB, 1) - 1) : (MAXROWS( npECB ) - 1) ;
         if (ORIGIN( npECB ))
            CURROW( npECB ) = SCROLLREGIONTOP( npECB ) ;
         else
            CURROW( npECB ) = 0 ;
         CURCOL( npECB ) = 0 ;
         break ;

      case 'p':
         switch (SEQUENCEFLAG( npECB ))
         {
            case SEQFLAG_BANG:
               SoftResetEmulator( npECB ) ;
               break ;

            case SEQFLAG_QUOTE:
               SoftResetEmulator( npECB ) ;
               switch (PARAM( npECB, 0 ))
               {
                  case 61:
                     COMPATIBILITYLEVEL( npECB ) = LEVEL_VT100 ;
                     break ;

                  case 62:
                     switch (PARAM( npECB, 1 ))
                     {
                        case 0:
                        case 2:
                           COMPATIBILITYLEVEL( npECB ) = LEVEL_VT2008BIT ;
                           break ;

                        case 1:
                           COMPATIBILITYLEVEL( npECB ) = LEVEL_VT2007BIT ;
                     }
                     break ;
               }
               DPF3( hWDB, "Compatibility level set to = %d\r\n",
                           COMPATIBILITYLEVEL( npECB ) ) ;
               break ;
         }
         break ;

      case 'g':
         // clear TAB stop settings
         switch (PARAM( npECB, 0 ))
         {
            case 0:
               ClearTabStop( npECB, CURCOL( npECB ) ) ;
               break ;

            case 3:
               ClearTabStops( npECB ) ;
         }
         break ;

      case 'h':
      case 'l':
      {
         // set various flags

         BOOL  fFlag ;

         fFlag = (bByte == 'h') ;

         if (SEQUENCEFLAG( npECB ) == SEQFLAG_QUESTION)
         {
            switch (PARAM( npECB, 0 ))
            {
               case DECANM:
                  // ANSI / VT52 mode
                  if (!fFlag)
                     COMPATIBILITYLEVEL( npECB ) = LEVEL_VT52 ;
                  DP3( hWDB, "ANSI / VT52 mode set" ) ;
                  break;

               case DECSCNM:
                  // Screen Mode

                  break ;

               case DECOM:
                  // Origin Mode

                  ORIGIN( npECB ) = fFlag ;
                  if (ORIGIN( npECB ))
                     CURROW( npECB ) = SCROLLREGIONTOP( npECB ) ;
                  else
                     CURROW( npECB ) = 0 ;
                  CURCOL( npECB ) = 0 ;
                  break ;

               case DECAWM:
                  // Autowrap Mode
                  AUTOWRAP( npECB ) = fFlag ;
                  break ;
            }
         }
         else
         {
            switch (PARAM( npECB, 0 ))
            {
               case 20:
                  // action of LF (VT and FF)
                  NEWLINE( npECB ) = fFlag ;
                  break ;
            }
         }
      }
      break ;

      case 'q':
         if (SEQUENCEFLAG( npECB ) == SEQFLAG_QUOTE)
         {
            switch (PARAM( npECB, 0 ))
            {
               case 0:
               case 1:
                  SELERASE( npECB ) = FALSE ;
                  break ;

               case 2:
                  SELERASE( npECB ) = TRUE ;
                  break ;
            }
         }
         break ;

      case 's':
         SAVEROW( npECB ) = CURROW( npECB ) ;
         SAVECOL( npECB ) = CURCOL( npECB ) ;
         break ;

      case 'u':
         CURROW( npECB ) = SAVEROW( npECB ) ;
         CURCOL( npECB ) = SAVECOL( npECB ) ;
         break ;

      case 'm':
      {
         int   i ;
         BOOL  fNegateAttribute = FALSE ;

         for  (i = 0; i < PARAMCOUNT( npECB ); i++)
         {
            switch (PARAM( npECB, i ))
            {
               case 0:
                  // cancel all attributes
                  ATTR( npECB ) =
                     ATTR( npECB ) &(~(BYTE) (ATTRIBUTE_BOLD |
                                              ATTRIBUTE_UNDERLINE |
                                              ATTRIBUTE_BLINKING |
                                              ATTRIBUTE_INVERSE)) ;
                  break ;

               case 1:
                  // bold
                  if (fNegateAttribute)
                     ATTR( npECB ) =
                        ATTR( npECB ) & (~(BYTE) ATTRIBUTE_BOLD) ;
                  else
                     ATTR( npECB ) =
                        ATTR( npECB ) | (BYTE) ATTRIBUTE_BOLD ;
                  break ;

               case 2:
                  // option off
                  fNegateAttribute = TRUE ;
                  break ;

               case 4:
                  // underline
                  if (fNegateAttribute)
                     ATTR( npECB ) =
                        ATTR( npECB ) & (~(BYTE) ATTRIBUTE_UNDERLINE) ;
                  else
                     ATTR( npECB ) =
                        ATTR( npECB ) | (BYTE) ATTRIBUTE_UNDERLINE ;
                  break ;

               case 5:
                  // blinking
                  if (fNegateAttribute)
                     ATTR( npECB ) =
                        ATTR( npECB ) & (~(BYTE) ATTRIBUTE_BLINKING) ;
                  else
                     ATTR( npECB ) =
                        ATTR( npECB ) | (BYTE) ATTRIBUTE_BLINKING ;
                  break ;

               case 7:
                  // inverse
                  if (fNegateAttribute)
                     ATTR( npECB ) =
                        ATTR( npECB ) & (~(BYTE) ATTRIBUTE_INVERSE) ;
                  else
                     ATTR( npECB ) =
                        ATTR( npECB ) | (BYTE) ATTRIBUTE_INVERSE ;
                  break ;
            }
         }
      }
      break ;

      case 'c':
         if (SEQUENCEFLAG( npECB ) != SEQFLAG_GREATER)
            SendControlSequence( GetParent( hWnd ), npECB, VT220_DEVATTRPRI ) ;
         else
            SendControlSequence( GetParent( hWnd ), npECB, VT220_DEVATTRSEC ) ;
      break ;
   }
   SEQUENCESTATE( npECB ) = SEQUENCE_NONE ;

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
      case '[':
         SEQUENCESTATE( npECB ) = SEQUENCE_CONTROL ;
         SEQUENCEFLAG( npECB ) = SEQFLAG_NONE ;
         ResetControlSeqParams( npECB ) ;
         return ;

      case '7':
         // save cursor state

         SAVESTATE( npECB ).nCurrentRow = CURROW( npECB ) ;
         SAVESTATE( npECB ).nCurrentCol = CURCOL( npECB ) ;
         SAVESTATE( npECB ).nDECGL = DECGL( npECB ) ;
         SAVESTATE( npECB ).nDECGR = DECGR( npECB ) ;
         SAVESTATE( npECB ).fAutoWrap = AUTOWRAP( npECB ) ;
         SAVESTATE( npECB ).fOrigin = ORIGIN( npECB ) ;
         SAVESTATE( npECB ).bAttr = ATTR( npECB ) ;
         SAVESTATE( npECB ).fSelErase = SELERASE( npECB ) ;
         CURSORSTATESAVED( npECB ) = TRUE ;
         break ;

      case '8':
         // restore cursor state

         if (CURSORSTATESAVED( npECB ))
         {
            CURROW( npECB ) = SAVESTATE( npECB ).nCurrentRow ;
            CURCOL( npECB ) = SAVESTATE( npECB ).nCurrentCol ;
            DECGL( npECB ) = SAVESTATE( npECB ).nDECGL ;
            DECGR( npECB ) = SAVESTATE( npECB ).nDECGR ;
            AUTOWRAP( npECB ) = SAVESTATE( npECB ).fAutoWrap ;
            ORIGIN( npECB ) = SAVESTATE( npECB ).fOrigin ;
            ATTR( npECB ) = SAVESTATE( npECB ).bAttr ;
            SELERASE( npECB ) = SAVESTATE( npECB ).fSelErase ;
            CURSORSTATESAVED( npECB ) = FALSE ;
         }
         else
         {
            CURCOL( npECB ) = 0 ;
            CURROW( npECB ) = 0 ;
            ORIGIN( npECB ) = FALSE ;
            ATTR( npECB ) = 0 ;
            DECGL( npECB ) = CHARSET_DEFAULT ;
            DECGR( npECB ) = CHARSET_DEFAULT ;
         }
         break ;

      case '(':
      case ')':
      case '*':
      case '+':
      {
         // character set selection
         SEQUENCESTATE( npECB ) = SEQUENCE_CHARSET ;
         SEQUENCEFLAG( npECB ) = (WORD) (bByte - '(') ;
         return ;
      }

      case '~':
         // invoke G1 character set into GL
         DECGL( npECB ) = 1 ;
         break ;

      case 'n':
         // invoke G2 character set into GL
         DECGL( npECB ) = 2 ;
         break ;

      case '}':
         // invoke G2 character set into GR
         DECGR( npECB ) = 2 ;
         break ;

      case 'o':
         // invoke G3 character set into GL
         DECGL( npECB ) = 3 ;
         break ;

      case '|':
         // invoke G3 character set into GR
         DECGR( npECB ) = 3 ;
         break ;

      case '=':
         KEYPADTYPE( npECB ) = KEYPAD_APPLICATION ;
         break ;

      case '>':
         KEYPADTYPE( npECB ) = KEYPAD_NUMERIC ;
         break ;

      case '#':
         SEQUENCESTATE( npECB ) = SEQUENCE_LINEATTR ;
         SEQUENCEFLAG( npECB ) = SEQFLAG_NONE ;
         return ;

      case 'H':
         SetTabStop( npECB, CURCOL( npECB ) ) ;
         break ;

      case 'E':
         // move to first position of next line
         CURCOL( npECB ) = 0 ;

         // fallthrough

      case 'D':
         // move line down and scroll if necessary

         if (CURROW( npECB ) > SCROLLREGIONBOTTOM( npECB ))
         {
            // out of scroll region
            if (CURROW( npECB )++ == MAXROWS( npECB ))
               CURROW( npECB )-- ;
         }
         else if (CURROW( npECB )++ == SCROLLREGIONBOTTOM( npECB ))
         {
            CURROW( npECB )-- ;
            ScrollRegionUp( hWnd, npECB, SCROLLREGIONTOP( npECB ),
                            SCROLLREGIONBOTTOM( npECB ), 1 ) ;
         }
         break ;

      case 'M':
         // move line up and scroll if necessary

         if (CURROW( npECB ) < SCROLLREGIONTOP( npECB ))
         {
            // out of scroll region
            if (CURROW( npECB )-- == 0)
               CURROW( npECB )++ ;
         }
         else if (CURROW( npECB )-- == SCROLLREGIONTOP( npECB ))
         {
            CURROW( npECB )++ ;
            ScrollRegionDown( hWnd, npECB, SCROLLREGIONTOP( npECB ),
                              SCROLLREGIONBOTTOM( npECB ), 1 ) ;
         }
         break ;

      case 'c':
         SoftResetEmulator( npECB ) ;
         break ;

      case 'N':
         // single shift G2 into GL
         SNGLSHFT( npECB ) = 2 ;
         break ;

      case 'O':
         // single shift G3 into GL
         SNGLSHFT( npECB ) = 3 ;
         break ;

      case 'Z':
         SendControlSequence( GetParent( hWnd ), npECB, VT220_DEVATTRPRI ) ;
         break ;

      case ' ':
         SEQUENCEFLAG( npECB ) = SEQFLAG_SPACE ;
         return ;

      case 'F':
      case 'G':
         if (SEQUENCEFLAG( npECB ) == SEQFLAG_SPACE &&
             COMPATIBILITYLEVEL( npECB ) != LEVEL_VT100)
            RESPONSELEVEL( npECB ) =
               (bByte == 'F') ? LEVEL_S7C1T : LEVEL_S8C1T ;
         break ;

      case '\\':
         COMPATIBILITYLEVEL( npECB ) = LEVEL_VT2008BIT ;
         break ;
   }
   SEQUENCESTATE( npECB ) = SEQUENCE_NONE ;

} /* end of EscapeSequence() */

/************************************************************************
 *  VOID NEAR LineAttribute( HWND hWnd, NPECB npECB, BYTE bByte )
 *
 *  Description:
 *     Handles VT220 line attribute settings.
 *     (e.g. DoubleWide, DoubleHeight )
 *
 *  Comments:
 *      5/18/91  baw  Pulled from WriteEmulatorByte()
 *
 ************************************************************************/

VOID NEAR LineAttribute( HWND hWnd, NPECB npECB, BYTE bByte )
{
   switch (bByte)
   {
      case '3':
         // set double width / double height (top)
         ExpandDoubleWide( hWnd, npECB, (BYTE) ~ATTRIBUTE_DBLSIZE,
                           (BYTE) ATTRIBUTE_DBLHEIGHTTOP ) ;
      break ;

      case '4':
         // set double width / double height (bottom)
         ExpandDoubleWide( hWnd, npECB, (BYTE) ~ATTRIBUTE_DBLSIZE,
                           (BYTE) ATTRIBUTE_DBLHEIGHTBTM ) ;
         break ;

      case '5':
         // reset line attributes
         ExpandDoubleWide( hWnd, npECB, (BYTE) ~ATTRIBUTE_DBLSIZE, 0 ) ;
         break ;

      case '6':
         // set double width
         ExpandDoubleWide( hWnd, npECB, (BYTE) ~ATTRIBUTE_DBLSIZE,
                           (BYTE) ATTRIBUTE_DBLWIDTH ) ;
         break ;

   }
   SEQUENCESTATE( npECB ) = SEQUENCE_NONE ;

} /* end of LineAttribute() */

/************************************************************************
 *  VOID NEAR SelectCharacterSet( HWND hWnd, NPECB npECB, BYTE bByte )
 *
 *  Description:
 *     Select character set based on bByte into SEQUENCEFLAG register.
 *
 *  Comments:
 *     5/18/91  baw  Pulled from WriteEmulatorByte()
 *
 ************************************************************************/

VOID NEAR SelectCharacterSet( HWND hWnd, NPECB npECB, BYTE bByte )
{
   switch (bByte)
   {
      case 'B':
         CHARACTERSET( npECB, SEQUENCEFLAG( npECB ) ) = CHARSET_ASCII ;
         break ;

      case '<':
         CHARACTERSET( npECB, SEQUENCEFLAG( npECB ) ) = CHARSET_DECSUPPL ;
         break ;

      case '0':
         CHARACTERSET( npECB, SEQUENCEFLAG( npECB ) ) = CHARSET_DECGRAPHIC ;
         DP3( hWDB, "DEC graphic set selected." ) ;
         break ;
  }
  SEQUENCESTATE( npECB ) = SEQUENCE_NONE ;

} /* end of SelectCharacterSet() */

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

   PARAMCOUNT( npECB ) = 0 ;
   _fmemset( (LPSTR) &CONTROLSEQ( npECB, 0 ), NULL, MAXLEN_CONTROLSEQ ) ;
   for (i = 0; i < MAXLEN_PARAMS; i++)
      PARAM( npECB, i ) = 0 ;

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
   int   nStartRow, nEndRow, nStartCol, nEndCol ;
   BOOL  fShowCursor ;
//   HDC   hDC ;
   RECT  rcClient, rcScroll, rcInvalid ;

   // Update the window before scrolling

   UpdateWindow( hWnd ) ;

   _fmemmove( (LPSTR) (CHARBUF( npECB ) +
                       nRegionTop * MAXCOLS( npECB )),
              (LPSTR) (CHARBUF( npECB ) +
                       (nRegionTop + nLines) * MAXCOLS( npECB )),
              (nRegionBottom - (nRegionTop + nLines) + 1) * MAXCOLS( npECB ) ) ;
   _fmemset( (LPSTR) (CHARBUF( npECB ) +
                      (nRegionBottom - nLines + 1) * MAXCOLS( npECB )),
             ' ', MAXCOLS( npECB ) * nLines ) ;
   _fmemmove( (LPSTR) (ATTRBUF( npECB ) +
                       nRegionTop * MAXCOLS( npECB )),
              (LPSTR) (ATTRBUF( npECB ) +
                       (nRegionTop + nLines) * MAXCOLS( npECB )),
              (nRegionBottom - (nRegionTop + nLines) + 1) * MAXCOLS( npECB ) ) ;
   _fmemset( (LPSTR) (ATTRBUF( npECB ) +
                      (nRegionBottom - nLines + 1) * MAXCOLS( npECB )),
             0, MAXCOLS( npECB ) * nLines ) ;

   // compute actual visible portion of screen to scroll

   GetClientRect( hWnd, &rcClient ) ;
   nStartRow =
      max( nRegionTop,
           (rcClient.top + YOFFSET( npECB )) / YCHAR( npECB ) ) ;
   nEndRow =
      min( nRegionBottom,
           (rcClient.bottom + YOFFSET( npECB )) / YCHAR( npECB )  - 1 ) ;
   nStartCol =
      max( 0,
           (rcClient.left + XOFFSET( npECB )) / XCHAR( npECB ) ) ;
   nEndCol =
      min( MAXCOLS( npECB ) - 1,
           (rcClient.right + XOFFSET( npECB )) / XCHAR( npECB ) - 1 ) ;

   // convert back to client coordinates

   rcScroll.top =
      nStartRow * YCHAR( npECB ) - YOFFSET( npECB ) ;
   rcScroll.bottom =
      nEndRow * YCHAR( npECB ) - YOFFSET( npECB ) + YCHAR( npECB ) ;
   rcScroll.left =
      nStartCol * XCHAR( npECB ) - XOFFSET( npECB ) ;
   rcScroll.right =
      nEndCol * XCHAR( npECB ) - XOFFSET( npECB ) + XCHAR( npECB ) ;

   // hide the caret ( if necessary )

   fShowCursor = HideEmulatorCursor( hWnd ) ;

   // scroll the rect

   ScrollDC( SCREENDC( npECB ), 0, -(nLines * YCHAR( npECB )),
             &rcScroll, &rcScroll, NULL, &rcInvalid ) ;

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
   int   nStartRow, nEndRow, nStartCol, nEndCol ;
   BOOL  fShowCursor ;
//   HDC   hDC ;
   RECT  rcClient, rcScroll, rcInvalid ;

   // Update the window before scrolling

   UpdateWindow( hWnd ) ;

   _fmemmove( (LPSTR) (CHARBUF( npECB ) +
                       (nRegionTop + nLines) * MAXCOLS( npECB )),
              (LPSTR) (CHARBUF( npECB ) +
                       nRegionTop * MAXCOLS( npECB )),
              (nRegionBottom - (nRegionTop + nLines) + 1) * MAXCOLS( npECB ) ) ;
   _fmemset( (LPSTR) (CHARBUF( npECB ) + nRegionTop * MAXCOLS( npECB )),
             ' ', MAXCOLS( npECB ) * nLines ) ;
   _fmemmove( (LPSTR) (ATTRBUF( npECB ) +
                       (nRegionTop + nLines) * MAXCOLS( npECB )),
              (LPSTR) (ATTRBUF( npECB ) +
                       nRegionTop * MAXCOLS( npECB )),
              (nRegionBottom - (nRegionTop + nLines) + 1) * MAXCOLS( npECB ) ) ;
   _fmemset( (LPSTR) (ATTRBUF( npECB ) + (nRegionTop) * MAXCOLS( npECB )),
             0, MAXCOLS( npECB ) * nLines ) ;

   // compute actual visible portion of screen to scroll

   GetClientRect( hWnd, &rcClient ) ;
   nStartRow =
      max( nRegionTop,
           (rcClient.top + YOFFSET( npECB )) / YCHAR( npECB ) ) ;
   nEndRow =
      min( nRegionBottom,
           (rcClient.bottom + YOFFSET( npECB )) / YCHAR( npECB ) - 1 ) ;
   nStartCol =
      max( 0,
           (rcClient.left + XOFFSET( npECB )) / XCHAR( npECB ) ) ;
   nEndCol =
      min( MAXCOLS( npECB ) - 1,
           (rcClient.right + XOFFSET( npECB )) / XCHAR( npECB ) - 1 ) ;

   // convert back to client coordinates

   rcScroll.top =
      nStartRow * YCHAR( npECB ) - YOFFSET( npECB ) ;
   rcScroll.bottom =
      nEndRow * YCHAR( npECB ) - YOFFSET( npECB ) + YCHAR( npECB ) ;
   rcScroll.left =
      nStartCol * XCHAR( npECB ) - XOFFSET( npECB ) ;
   rcScroll.right =
      nEndCol * XCHAR( npECB ) - XOFFSET( npECB ) + XCHAR( npECB ) ;

   // hide the caret ( if necessary )

   fShowCursor = HideEmulatorCursor( hWnd ) ;

   // scroll the rect

   ScrollDC( SCREENDC( npECB ), 0, nLines * YCHAR( npECB ),
             &rcScroll, &rcScroll, NULL, &rcInvalid ) ;

   // and show the cursor (if previously showing)

   if (fShowCursor)
      ShowEmulatorCursor( hWnd ) ;

   // invalidate new portion

   InvalidateRect( hWnd, &rcInvalid, FALSE ) ;

   // Force new portion of screen to clear

   UpdateWindow( hWnd ) ;

} /* end of ScrollRegionDown() */

/************************************************************************
 *  VOID ScrollLineLeft( HWND hWnd, NPECB npECB,
 *                       int nRow, int nCol, int nNumCols )
 *
 *  Description:
 *     Scrolls the character and attribute buffers to the left.
 *     The right-most nCols columns are blanked with spaces
 *     and the attributes are set to 0.
 *
 *  Comments:
 *      6/19/91  baw  Wrote this comment block.
 *
 ************************************************************************/

VOID ScrollLineLeft( HWND hWnd, NPECB npECB,
                     int nRow, int nCol, int nNumCols )
{
   int   nStartRow, nEndRow, nStartCol, nEndCol ;
   BOOL  fShowCursor ;
//   HDC   hDC ;
   RECT  rcClient, rcScroll, rcInvalid ;

   // Update the window before scrolling

   UpdateWindow( hWnd ) ;

   _fmemmove( (LPSTR) (CHARBUF( npECB ) +
                       nRow * MAXCOLS( npECB ) + nCol),
              (LPSTR) (CHARBUF( npECB ) +
                       nRow * MAXCOLS( npECB ) + nCol + nNumCols),
              MAXCOLS( npECB ) - (nCol + nNumCols) + 1 ) ;
   _fmemset( (LPSTR) (CHARBUF( npECB ) +
                      nRow * MAXCOLS( npECB ) +
                      MAXCOLS( npECB ) - (nCol + nNumCols)),
             ' ', nNumCols ) ;
   _fmemmove( (LPSTR) (ATTRBUF( npECB ) +
                       nRow * MAXCOLS( npECB ) + nCol),
              (LPSTR) (ATTRBUF( npECB ) +
                       nRow * MAXCOLS( npECB ) + nCol + nNumCols),
              MAXCOLS( npECB ) - (nCol + nNumCols) + 1 ) ;
   _fmemset( (LPSTR) (ATTRBUF( npECB ) +
                      nRow * MAXCOLS( npECB ) +
                      MAXCOLS( npECB ) - (nCol + nNumCols)),
             0, nNumCols ) ;

   // compute actual visible portion of screen to scroll

   GetClientRect( hWnd, &rcClient ) ;
   nStartRow =
      max( nRow,
           (rcClient.top + YOFFSET( npECB )) / YCHAR( npECB ) ) ;
   nEndRow =
      min( nRow,
           (rcClient.bottom + YOFFSET( npECB )) / YCHAR( npECB )  - 1 ) ;
   nStartCol =
      max( nCol,
           (rcClient.left + XOFFSET( npECB )) / XCHAR( npECB ) ) ;
   nEndCol =
      min( MAXCOLS( npECB ) - 1,
           (rcClient.right + XOFFSET( npECB )) / XCHAR( npECB ) - 1 ) ;

   // convert back to client coordinates

   rcScroll.top =
      nStartRow * YCHAR( npECB ) - YOFFSET( npECB ) ;
   rcScroll.bottom =
      nEndRow * YCHAR( npECB ) - YOFFSET( npECB ) + YCHAR( npECB ) ;
   rcScroll.left =
      nStartCol * XCHAR( npECB ) - XOFFSET( npECB ) ;
   rcScroll.right =
      nEndCol * XCHAR( npECB ) - XOFFSET( npECB ) + XCHAR( npECB ) ;

   // hide the caret ( if necessary )

   fShowCursor = HideEmulatorCursor( hWnd ) ;

   // scroll the rect

   ScrollDC( SCREENDC( npECB ), -(nNumCols * XCHAR( npECB )), 0,
             &rcScroll, &rcScroll, NULL, &rcInvalid ) ;

   // and show the cursor (if previously showing)

   if (fShowCursor)
      ShowEmulatorCursor( hWnd ) ;

   // invalidate new portion

   InvalidateRect( hWnd, &rcInvalid, FALSE ) ;

   // Force new portion of screen to clear

   UpdateWindow( hWnd ) ;

} /* end of ScrollLineLeft() */

/************************************************************************
 *  VOID ScrollLineRight( HWND hWnd, NPECB npECB,
 *                        int nRow, int nCol, int nNumCols )
 *
 *  Description:
 *     Scrolls the character and attribute buffers to the right.
 *     The right-most nCols columns are blanked with spaces
 *     and the attributes are set to 0.
 *
 *  Comments:
 *      6/19/91  baw  Wrote this comment block.
 *
 ************************************************************************/

VOID ScrollLineRight( HWND hWnd, NPECB npECB,
                      int nRow, int nCol, int nNumCols )
{
   int   nStartRow, nEndRow, nStartCol, nEndCol ;
   BOOL  fShowCursor ;
//   HDC   hDC ;
   RECT  rcClient, rcScroll, rcInvalid ;

   // Update the window before scrolling

   UpdateWindow( hWnd ) ;

   _fmemmove( (LPSTR) (CHARBUF( npECB ) +
                       nRow * MAXCOLS( npECB ) + nCol + nNumCols),
              (LPSTR) (CHARBUF( npECB ) +
                       nRow * MAXCOLS( npECB ) + nCol),
              MAXCOLS( npECB ) - (nCol + nNumCols) + 1 ) ;
   _fmemset( (LPSTR) (CHARBUF( npECB ) +
                      nRow * MAXCOLS( npECB ) + nCol),
             ' ', nNumCols ) ;
   _fmemmove( (LPSTR) (ATTRBUF( npECB ) +
                       nRow * MAXCOLS( npECB ) + nCol + nNumCols),
              (LPSTR) (ATTRBUF( npECB ) +
                       nRow * MAXCOLS( npECB ) + nCol),
              MAXCOLS( npECB ) - (nCol + nNumCols) + 1 ) ;
   _fmemset( (LPSTR) (ATTRBUF( npECB ) +
                      nRow * MAXCOLS( npECB ) + nCol),
             0, nNumCols ) ;

   // compute actual visible portion of screen to scroll

   GetClientRect( hWnd, &rcClient ) ;
   nStartRow =
      max( nRow,
           (rcClient.top + YOFFSET( npECB )) / YCHAR( npECB ) ) ;
   nEndRow =
      min( nRow,
           (rcClient.bottom + YOFFSET( npECB )) / YCHAR( npECB ) - 1 ) ;
   nStartCol =
      max( nCol,
           (rcClient.left + XOFFSET( npECB )) / XCHAR( npECB ) ) ;
   nEndCol =
      min( MAXCOLS( npECB ) - 1,
           (rcClient.right + XOFFSET( npECB )) / XCHAR( npECB ) - 1 ) ;

   // convert back to client coordinates

   rcScroll.top =
      nStartRow * YCHAR( npECB ) - YOFFSET( npECB ) ;
   rcScroll.bottom =
      nEndRow * YCHAR( npECB ) - YOFFSET( npECB ) + YCHAR( npECB ) ;
   rcScroll.left =
      nStartCol * XCHAR( npECB ) - XOFFSET( npECB ) ;
   rcScroll.right =
      nEndCol * XCHAR( npECB ) - XOFFSET( npECB ) + XCHAR( npECB ) ;

   // hide the caret ( if necessary )

   fShowCursor = HideEmulatorCursor( hWnd ) ;

   // scroll the rect

   ScrollDC( SCREENDC( npECB ), nNumCols * XCHAR( npECB ), 0,
             &rcScroll, &rcScroll, NULL, &rcInvalid ) ;

   // and show the cursor (if previously showing)

   if (fShowCursor)
      ShowEmulatorCursor( hWnd ) ;

   // invalidate new portion

   InvalidateRect( hWnd, &rcInvalid, FALSE ) ;

   // Force new portion of screen to clear

   UpdateWindow( hWnd ) ;

} /* end of ScrollLineRight() */

/************************************************************************
 *  VOID SendControlSequence( HWND hWnd, NPECB npECB, LPSTR lpSequence )
 *
 *  Description: 
 *     Sends a control sequence specified by lpSequence to hWnd.  The
 *     sequence prefix is added in this function.
 *
 *  Comments:
 *      5/17/91  baw  Wrote it.
 *
 ************************************************************************/

VOID SendControlSequence( HWND hWnd, NPECB npECB, LPSTR lpSequence )
{
   BYTE  uszBlock[ MAXLEN_FMTSTR ] ;

   switch (COMPATIBILITYLEVEL( npECB ))
   {
      case LEVEL_VT100:
      case LEVEL_VT2007BIT:
         // Must use 7 bit control sequence start
         uszBlock[0] = ASCII_ESC ;
         uszBlock[1] = '[' ;
         lstrcpy( &uszBlock[2], lpSequence ) ;
         break ;

      case LEVEL_VT2008BIT:
         uszBlock[0] = ASCII_CSI ;
         lstrcpy( &uszBlock[1], lpSequence ) ;
         break ;
   }
   SendMessage( hWnd, WM_CMD_WRITEBLK, (WORD) lstrlen( uszBlock ),
                (LONG) (LPSTR) uszBlock ) ;

} /* end of SendControlSequence() */

/************************************************************************
 *  VOID ExpandDoubleWide( HWND hWnd, NPECB npECB, BYTE bAttributeAnd,
 *                         BYTE bAttributeOr )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

VOID ExpandDoubleWide( HWND hWnd, NPECB npECB, BYTE bAttributeAnd,
                       BYTE bAttributeOr )
{
   int   nCount ;
   RECT  rcLine ;

   for (nCount = 0; nCount < MAXCOLS( npECB ) / 2; nCount++)
   {
      *(LPSTR) (ATTRBUF( npECB ) +
                CURROW( npECB ) * MAXCOLS( npECB ) + nCount) &= bAttributeAnd ;
      *(LPSTR) (ATTRBUF( npECB ) +
                CURROW( npECB ) * MAXCOLS( npECB ) + nCount) |= bAttributeOr ;
   }
   if (bAttributeOr & (BYTE) ATTRIBUTE_DBLSIZE)
   {
      if (CURCOL( npECB ) > MAXCOLS( npECB ) / 2)
         CURCOL( npECB ) = MAXCOLS( npECB ) / 2 ;
   }
   _fmemset( (LPSTR) (CHARBUF( npECB ) +
                      CURROW( npECB ) * MAXCOLS( npECB ) +
                      MAXCOLS( npECB ) / 2 + 1),
             ' ', MAXCOLS( npECB ) - (MAXCOLS( npECB ) / 2 + 1) ) ;
   _fmemset( (LPSTR) (ATTRBUF( npECB ) +
                      CURROW( npECB ) * MAXCOLS( npECB ) +
                      MAXCOLS( npECB ) / 2 + 1),
             0, MAXCOLS( npECB ) - (MAXCOLS( npECB ) / 2 + 1) ) ;
   rcLine.left = -XOFFSET( npECB ) ;
   rcLine.top =  (CURROW( npECB ) * YCHAR( npECB )) - YOFFSET( npECB ) ;
   rcLine.right = (MAXCOLS( npECB ) * XCHAR( npECB )) - XOFFSET( npECB ) ;
   rcLine.bottom = rcLine.top + YCHAR( npECB ) ;
   InvalidateRect( hWnd, &rcLine, FALSE ) ;

} /* end of ExpandDoubleWide() */

/************************************************************************
 *  BOOL TranslateEmulatorByte( NPECB npECB, LPBYTE lpByte )
 *
 *  Description:
 *     Translates to the appropriate character set.
 *
 *  Comments:
 *      6/ 1/91  baw  Wrote this comment block.
 *
 ************************************************************************/

BYTE abDECGraphicsSet[ 32 ] = {  32,   4, 177,  26,  23,  27,  25, 248,
                                241,  21,  18, 217, 191, 218, 192, 197,
                                196, 196, 196, 196, 196, 195, 180, 193,
                                194, 179, 243, 242, 227, 157, 156, 250 } ;

WORD TranslateEmulatorByte( NPECB npECB, BYTE bByte )
{
   if (bByte >= 0x5F && bByte <= 0x7E)
   {
      if (DECGL( npECB ) == CHARSET_DEFAULT)
         return ( MAKEWORD( 0x00, bByte ) ) ;

      switch (CHARACTERSET( npECB, DECGL( npECB ) ))
      {
         case CHARSET_DECGRAPHIC:
            return ( MAKEWORD( 0x01, abDECGraphicsSet[ bByte - 0x5F ] ) ) ;

         default:
            return ( MAKEWORD( 0x00, bByte ) ) ;
      }
   }
   return ( MAKEWORD( 0x00, bByte ) ) ;

} /* end of TranslateEmulatorByte() */

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
   BYTE     uszBlk[MAXLEN_FMTSTR] ;
   int      nLen ;

   nLen = wvsprintf( (LPSTR) uszBlk, (LPSTR) lpszFmt, (LPSTR) &lpszParams ) ;
   return ( WriteEmulatorBlock( hWnd, uszBlk, nLen ) ) ;

} /* end of WriteEmulatorFormat() */

/************************************************************************
 * End of File: write.c
 ************************************************************************/
