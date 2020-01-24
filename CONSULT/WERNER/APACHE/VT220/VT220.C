/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  vt220.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  VT220 Terminal Emulation
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.002  2/28/91 baw   Moved to self-contained DLL
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

// setup for global declarations (main module only)
#define GLOBALDEFS

#include "vt220.h"

/************************************************************************
 *  int FAR PASCAL GetEmulatorUseCount( VOID )
 *
 *  Description:
 *     Returns the use count.  The use count is the number of
 *     active sessions using this library.
 *
 *  Comments:
 *      7/ 4/91  baw  Added this comment block.
 *
 ************************************************************************/

int FAR PASCAL GetEmulatorUseCount( VOID )
{
   return ( nUseCount ) ;

} /* end of GetEmulatorUseCount() */

/************************************************************************
 *  HECB CreateECB( VOID )
 *
 *  Description:
 *     Creates an ECB.  The ECB must be initialized before use.
 *
 *  Comments:
 *      7/ 4/91  baw  Added this comment block.
 *
 ************************************************************************/

HECB CreateECB( VOID )
{
   return ( LocalAlloc( LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof( ECB ) ) ) ;

} /* end of CreateECB() */

/************************************************************************
 *  BOOL DestroyECB( HECB hECB )
 *
 *  Description:
 *     Destroys the given ECB.
 *
 *  Comments:
 *      7/ 4/91  baw  Added this comment block.
 *
 ************************************************************************/

BOOL DestroyECB( HECB hECB )
{
   return ( NULL == LocalFree( hECB ) ) ;

} /* end of DestroyECB() */

/************************************************************************
 *  BOOL InitECB( HECB hECB )
 *
 *  Description:
 *     Initializes the given ECB.  Returns FALSE if the ECB
 *     can not be locked.
 *
 *  Comments:
 *      7/ 4/91  baw  Added this comment block.
 *
 ************************************************************************/

BOOL InitECB( HECB hECB )
{
   NPECB  npECB ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;
   HardResetEmulator( npECB ) ;
   LocalUnlock( hECB ) ;

   return ( TRUE ) ;

} /* end of InitECB() */

/************************************************************************
 *  GLOBALHANDLE CreateScreenBuffer( int nRows, int nColumns,
 *                                   int nBytesPerChar )
 *
 *  Description:
 *     Creates a screen buffer based on the width, height and
 *     number of bytes per character.
 *
 *  Comments:
 *      7/ 4/91  baw  This routine is hardware dependant.  If this
 *                    buffer is > 64k, a HUGE pointer will be
 *                    necessary since __ahincr will be necessary!
 *
 ************************************************************************/

GLOBALHANDLE CreateScreenBuffer( int nRows, int nColumns,
                                 int nBytesPerChar )
{
   return ( GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT,
                         (DWORD) ((WORD) nRows *
                                  (WORD) nColumns *
                                  (WORD) nBytesPerChar) ) ) ;

} /* end of CreateScreenBuffer() */

/************************************************************************
 *  BOOL DestroyScreenBuffer( HECB hECB )
 *
 *  Description:
 *     Destroys the screen buffer associated with hECB.
 *
 *  Comments:
 *      7/ 4/91  baw  Added this comment.
 *
 ************************************************************************/

BOOL DestroyScreenBuffer( HECB hECB )
{
   BOOL   fRetVal ;
   NPECB  npECB ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   if (SCREENBUFFER( npECB ) != NULL)
      fRetVal = (NULL == GlobalFree( SCREENBUFFER( npECB ) )) ;
   else
      fRetVal = TRUE ;

   LocalUnlock( hECB ) ;
   return ( fRetVal ) ;

} /* end of DestroyScreenBuffer() */

/************************************************************************
 *  BOOL CreateEmulatorFonts( HWND hWnd, HECB hECB )
 *
 *  Description:
 *     Creates emulator fonts based on a given base font.
 *
 *  Comments:
 *      7/ 4/91  baw  Note: This currently relies on the fact that
 *                    GDI will stretch fonts (used for DoubleWidth
 *                    and DoubleHeight characters).
 *
 ************************************************************************/

BOOL CreateEmulatorFonts( HWND hWnd, HECB hECB )
{
   LOGFONT     lfANSI, lfOEM ;
   NPECB       npECB ;
   TEXTMETRIC  tm ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   //
   // Setup the OEM font
   //

   FontInitLogFont( (LPLOGFONT) &OEMFONT( npECB ) ) ;

   lstrcpy( OEMFONT( npECB ).lfFaceName, "TERMINAL" ) ;
   OEMFONT( npECB ).lfHeight = NORMALFONT( npECB ).lfHeight ;
   OEMFONT( npECB ).lfWidth = NORMALFONT( npECB ).lfWidth ;
   OEMFONT( npECB ).lfFGcolor = NORMALFONT( npECB ).lfFGcolor ;
   OEMFONT( npECB ).lfBGcolor = NORMALFONT( npECB ).lfBGcolor ;
   OEMFONT( npECB ).lfPitchAndFamily = FF_MODERN | FIXED_PITCH ;
   OEMFONT( npECB ).lfCharSet = OEM_CHARSET ;

   // FontSelectIndirect( NULL, (LPLOGFONT) &OEMFONT( npECB ) ) ;
   // FontSelectIndirect( NULL, (LPLOGFONT) &NORMALFONT( npECB ) ) ;

   // copy NORMAL font (base font) to workspace

   _fmemcpy( &lfANSI, &NORMALFONT( npECB ), sizeof( LOGFONT ) ) ;
   _fmemcpy( &lfOEM, &OEMFONT( npECB ), sizeof( LOGFONT ) ) ;

   EMULATORFONT( npECB, FONT_STANDARD ) = CreateFontIndirect( &lfANSI ) ;
   EMULATORFONT( npECB, FONT_STANDARDOEM ) = CreateFontIndirect( &lfOEM ) ;

   lfANSI.lfUnderline =      1 ;
   EMULATORFONT( npECB, FONT_UNDERLINE ) = CreateFontIndirect( &lfANSI ) ;
   lfOEM.lfUnderline  =      1 ;
   EMULATORFONT( npECB, FONT_UNDERLINEOEM ) = CreateFontIndirect( &lfOEM ) ;

   lfANSI.lfUnderline =      0 ;
   lfANSI.lfWidth *=         2 ;
   EMULATORFONT( npECB, FONT_DBLWIDTH ) = CreateFontIndirect( &lfANSI ) ;
   lfOEM.lfUnderline =       0 ;
   lfOEM.lfWidth *=          2 ;
   EMULATORFONT( npECB, FONT_DBLWIDTHOEM ) = CreateFontIndirect( &lfOEM ) ;

   lfANSI.lfUnderline =      1 ;
   EMULATORFONT( npECB, FONT_DBLWIDTHUL) = CreateFontIndirect( &lfANSI ) ;
   lfOEM.lfUnderline =       1 ;
   EMULATORFONT( npECB, FONT_DBLWIDTHULOEM ) = CreateFontIndirect( &lfOEM ) ;

   lfANSI.lfUnderline =      0 ;
   lfANSI.lfHeight *=        2 ;
   EMULATORFONT( npECB, FONT_DBLHEIGHT ) = CreateFontIndirect( &lfANSI ) ;
   lfOEM.lfUnderline =       0 ;
   lfOEM.lfHeight *=         2 ;
   EMULATORFONT( npECB, FONT_DBLHEIGHTOEM ) = CreateFontIndirect( &lfOEM ) ;

   lfANSI.lfUnderline =      1 ;
   EMULATORFONT( npECB, FONT_DBLHEIGHTUL ) = CreateFontIndirect( &lfANSI ) ;
   lfOEM.lfUnderline =       1 ;
   EMULATORFONT( npECB, FONT_DBLHEIGHTULOEM ) = CreateFontIndirect( &lfOEM ) ;

   // get text metrics

   DEFAULTFONT( npECB ) =
      SelectObject( SCREENDC( npECB ), EMULATORFONT( npECB, FONT_STANDARD ) ) ;
   GetTextMetrics( SCREENDC( npECB ), &tm ) ;
   XCHAR( npECB ) = tm.tmAveCharWidth ;
   YCHAR( npECB ) = tm.tmHeight + tm.tmExternalLeading ;

   LocalUnlock( hECB ) ;

   return ( TRUE ) ;

} /* end of CreateEmulatorFonts() */

/************************************************************************
 *  BOOL DestroyEmulatorFonts( HWND hWnd, HECB hECB )
 *
 *  Description:
 *     Destroys the emulator fonts associated with the given
 *     emulator window.
 *
 *  Comments:
 *      7/ 4/91  baw  Added this comment block.
 *
 ************************************************************************/

BOOL DestroyEmulatorFonts( HWND hWnd, HECB hECB )
{
   NPECB  npECB ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   SelectObject( SCREENDC( npECB ), DEFAULTFONT( npECB ) ) ;
   DeleteObject( EMULATORFONT( npECB, FONT_STANDARD ) ) ;
   DeleteObject( EMULATORFONT( npECB, FONT_UNDERLINE ) ) ;
   DeleteObject( EMULATORFONT( npECB, FONT_DBLWIDTH ) ) ;
   DeleteObject( EMULATORFONT( npECB, FONT_DBLWIDTHUL ) ) ;
   DeleteObject( EMULATORFONT( npECB, FONT_DBLHEIGHT ) ) ;
   DeleteObject( EMULATORFONT( npECB, FONT_DBLHEIGHTUL ) ) ;
   DeleteObject( EMULATORFONT( npECB, FONT_STANDARDOEM ) ) ;
   DeleteObject( EMULATORFONT( npECB, FONT_UNDERLINEOEM ) ) ;
   DeleteObject( EMULATORFONT( npECB, FONT_DBLWIDTHOEM ) ) ;
   DeleteObject( EMULATORFONT( npECB, FONT_DBLWIDTHULOEM ) ) ;
   DeleteObject( EMULATORFONT( npECB, FONT_DBLHEIGHTOEM ) ) ;
   DeleteObject( EMULATORFONT( npECB, FONT_DBLHEIGHTULOEM ) ) ;

   LocalUnlock( hECB ) ;
   return ( TRUE ) ;

} /* end of DestroyEmulatorFonts() */

/************************************************************************
 *  HECB CreateEmulator( HWND hWnd )
 *
 *  Description:
 *     Creates an emulator control block and initializes the block.
 *
 *  Comments:
 *      7/ 4/91  baw  Added this comment block.
 *
 ************************************************************************/

HECB CreateEmulator( HWND hWnd )
{
   HECB   hECB ;
   NPECB  npECB ;

   if (NULL == (hECB = CreateECB()))
      return ( NULL ) ;
   InitECB( hECB ) ;
   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
   {
      DestroyECB( hECB ) ;
      return ( NULL ) ;
   }

   // Get and store screen DC

   SCREENDC( npECB ) = GetDC( hWnd ) ;

   if (!CreateEmulatorFonts( hWnd, hECB ))
   {
      ReleaseDC( hWnd, SCREENDC( npECB ) ) ;
      LocalUnlock( hECB ) ;
      DestroyECB( hECB ) ;
      return ( NULL ) ;
   }
   SCREENBUFFER( npECB ) = NULL ;
   if (NULL ==
       (SCREENBUFFER( npECB ) = CreateScreenBuffer( MAXROWS( npECB ),
                                                    MAXCOLS( npECB ),
                                                    VT220_BYTESPERCHAR )))
   {
      ReleaseDC( hWnd, SCREENDC( npECB ) ) ;
      LocalUnlock( hECB ) ;
      DestroyECB( hECB ) ;
      return ( NULL )  ;
   }
   if (NULL == (CHARBUF( npECB ) = (LPSTR) GlobalLock( SCREENBUFFER( npECB ) )))
   {
      ReleaseDC( hWnd, SCREENDC( npECB ) ) ;
      LocalUnlock( hECB ) ;
      DestroyECB( hECB ) ;
      return ( NULL );
   }
   _fmemset( (LPSTR) CHARBUF( npECB ), ' ',
             MAXROWS( npECB ) * MAXCOLS( npECB ) ) ;
   ATTRBUF( npECB ) = CHARBUF( npECB ) + MAXROWS( npECB ) * MAXCOLS( npECB ) ;
   _fmemset( (LPSTR) ATTRBUF( npECB ), 0,
             MAXROWS( npECB ) * MAXCOLS( npECB ) ) ;

   // bump use count
   nUseCount++ ;

   // create timer for blink

   SetTimer( hWnd, BLINK_ID, BLINK_RATE, NULL ) ;

   LocalUnlock( hECB ) ;

   return ( hECB ) ;

} /* end of CreateEmulator() */

/************************************************************************
 *  VOID SoftResetEmulator( NPECB npECB )
 *
 *  Description: 
 *     Resets emulator settings ( SOFT RESET )
 *
 *  Comments:
 *      5/ 4/91  baw  Wrote this nifty comment block
 *
 ************************************************************************/

VOID SoftResetEmulator( NPECB npECB )
{
   int    i ;

   DP3( hWDB, "Performing soft terminal reset." ) ;

   CURROW( npECB ) =             0 ;
   CURCOL( npECB ) =             0 ;
   SAVEROW( npECB ) =            0 ;
   SAVECOL( npECB ) =            0 ;
   MAXCOLS( npECB ) =            VT220_COLUMNS_80 ;
   MAXROWS( npECB ) =            VT220_ROWS ;
   SCROLLREGIONTOP( npECB ) =    0 ;
   SCROLLREGIONBOTTOM( npECB ) = MAXROWS( npECB ) - 1 ;
   ATTR( npECB ) =               0 ;
   SEQUENCESTATE( npECB ) =      SEQUENCE_NONE ;
   SEQUENCEFLAG( npECB ) =       SEQFLAG_NONE ;
   DECGL( npECB ) =              CHARSET_DEFAULT ;
   DECGR( npECB ) =              CHARSET_DEFAULT ;
   SNGLSHFT( npECB ) =           CHARSET_DEFAULT ;
   KEYPADTYPE( npECB ) =         KEYPAD_APPLICATION ;
   LOCALECHO( npECB ) =          FALSE ;
   AUTOWRAP( npECB ) =           FALSE ;
   NEWLINE( npECB ) =            FALSE ;
   USENUMLOCK( npECB ) =         TRUE ;
   *ANSWERBACK( npECB ) =        NULL ;
   CURSORSTATESAVED( npECB ) =   FALSE ;
   BLINKSTATE( npECB ) =         FALSE ;
   COMPATIBILITYLEVEL( npECB ) = LEVEL_VT2008BIT ;
   RESPONSELEVEL( npECB ) =      LEVEL_S8C1T ;

   // setup character sets
   for (i = 0; i < MAXLEN_CHARSETS; i++)
      CHARACTERSET( npECB, i ) = CHARSET_ASCII ;

   // setup default tab stops
   ClearTabStops( npECB ) ;
   for (i = 8; i < VT220_COLUMNS_132; i += 8)
      SetTabStop( npECB, i ) ;

} /* end of SoftResetEmulator() */

/************************************************************************
 *  VOID HardResetEmulator( NPECB npECB )
 *
 *  Description: 
 *     Resets emulator settings ( HARD RESET )
 *
 *  Comments:
 *      5/ 4/91  baw  Wrote this nifty comment block
 *
 ************************************************************************/

VOID HardResetEmulator( NPECB npECB )
{
   DP3( hWDB, "Performing hard terminal reset." ) ;

   SoftResetEmulator( npECB ) ;

   XSIZE( npECB ) =   0 ;
   YSIZE( npECB ) =   0 ;
   XOFFSET( npECB ) = 0 ;
   YOFFSET( npECB ) = 0 ;

   //
   // Setup the "Default" fonts...
   //

   FontInitLogFont( (LPLOGFONT) &NORMALFONT( npECB ) ) ;
   FontInitLogFont( (LPLOGFONT) &HIGHLIGHTFONT( npECB ) ) ;

   lstrcpy( NORMALFONT( npECB ).lfFaceName, "COURIER" ) ;
   NORMALFONT( npECB ).lfFGcolor = RGBLTGRAY ;
   NORMALFONT( npECB ).lfBGcolor = RGBBLACK ;
   NORMALFONT( npECB ).lfHeight  = 12 ;
   NORMALFONT( npECB ).lfWidth   = 8 ;

   lstrcpy( HIGHLIGHTFONT( npECB ).lfFaceName, "COURIER" ) ;
   HIGHLIGHTFONT( npECB ).lfFGcolor = RGBWHITE ;
   HIGHLIGHTFONT( npECB ).lfBGcolor = RGBBLACK ;
   HIGHLIGHTFONT( npECB ).lfWidth = NORMALFONT( npECB ).lfWidth ;
   HIGHLIGHTFONT( npECB ).lfHeight = NORMALFONT( npECB ).lfHeight ;

} /* end of HardResetEmulator() */

/************************************************************************
 *  VOID ClearTabStops( NPECB npECB )
 *
 *  Description:
 *     Clears all TAB stops.
 *
 *  Comments:
 *      5/ 5/91  baw  Wrote it.
 *
 ************************************************************************/

VOID ClearTabStops( NPECB npECB )
{
   _fmemset( TABSTOPS( npECB ), 0, sizeof( TABSTOPS( npECB )) ) ;

} /* end of ClearTabStops() */

/************************************************************************
 *  VOID SetTabStop( NPECB npECB, int nCol )
 *
 *  Description:
 *     Set tab stop at nCol.  nCol is ZERO based!
 *
 *  Comments:
 *      5/ 5/91  baw  Wrote it and wrote this nifty comment block.
 *
 ************************************************************************/

VOID SetTabStop( NPECB npECB, int nCol )
{
   int  nByte, nBit ;

   nByte = nCol / 8 ;
   nBit =  nCol % 8 ;
   TABSTOPS( npECB )[ nByte ] |= (BYTE) (1 << nBit) ;

} /* end of SetTabStop() */

/************************************************************************
 *  VOID ClearTabStop( NPECB npECB, int nCol )
 *
 *  Description: 
 *     Set tab stop at nCol.  nCol is ZERO based!
 *
 *  Comments:
 *      5/ 5/91  baw  Wrote it and wrote this nifty comment block.
 *
 ************************************************************************/

VOID ClearTabStop( NPECB npECB, int nCol )
{
   int  nByte, nBit ;

   nByte = nCol / 8 ;
   nBit =  nCol % 8 ;
   TABSTOPS( npECB )[ nByte ] &= (BYTE) ~(1 << nBit) ;

} /* end of ClearTabStop() */

/************************************************************************
 *  BOOL GetTabState( NPECB npECB, int nCol )
 *
 *  Description:
 *    returns TRUE if TAB stop is set at nCol, FALSE if not
 *
 *  Comments:
 *      5/ 5/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL GetTabState( NPECB npECB, int nCol )
{
   int  nByte, nBit ;

   nByte = nCol / 8 ;
   nBit =  nCol % 8 ;
   return ( (TABSTOPS( npECB )[ nByte ] & (BYTE) (1 << nBit)) != 0 );

} /* end of GetTabState() */

/************************************************************************
 *  BOOL DestroyEmulator( HECB hECB )
 *
 *  Description:
 *     Destroys the ECB and associated information.
 *
 *  Comments:
 *      7/ 4/91  baw  Added this comment block.
 *
 ************************************************************************/

BOOL DestroyEmulator( HWND hWnd )
{
   HECB   hECB ;
   NPECB  npECB ;

   // Get Emulator Control Block
   hECB = GETHECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   // Unlock screen buffer
   GlobalUnlock( SCREENBUFFER( npECB ) ) ;

   // destroy fonts
   DestroyEmulatorFonts( hWnd, hECB ) ;

   // Release stored DC
   ReleaseDC( hWnd, SCREENDC( npECB ) ) ;

   LocalUnlock( hECB ) ;

   // destroy blink timer
   KillTimer( hWnd, BLINK_ID ) ;

   // bump use count
   nUseCount-- ;

   DestroyScreenBuffer( hECB ) ;
   DestroyECB( hECB ) ;

   return ( TRUE ) ;

} /* end of DestroyEmulator() */

/************************************************************************
 * End of File: vt220.c
 ************************************************************************/

