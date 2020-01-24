/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  ibmpc.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  IBMPC Terminal Emulation
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.002  2/28/91 baw   Moved to self-contained DLL
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

// setup for global declarations (main module only)
#define GLOBALDEFS

#include "ibmpc.h"

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

   if (SCREENBUFFER != NULL)
      fRetVal = (NULL == GlobalFree( SCREENBUFFER )) ;
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

   FontInitLogFont( (LPLOGFONT) &OEMFONT ) ;

   lstrcpy( OEMFONT.lfFaceName, "TERMINAL" ) ;
   OEMFONT.lfHeight = NORMALFONT.lfHeight ;
   OEMFONT.lfWidth = NORMALFONT.lfWidth ;
   OEMFONT.lfFGcolor = NORMALFONT.lfFGcolor ;
   OEMFONT.lfBGcolor = NORMALFONT.lfBGcolor ;
   OEMFONT.lfPitchAndFamily = FF_MODERN | FIXED_PITCH ;
   OEMFONT.lfCharSet = OEM_CHARSET ;

   // FontSelectIndirect( NULL, (LPLOGFONT) &OEMFONT ) ;
   // FontSelectIndirect( NULL, (LPLOGFONT) &NORMALFONT ) ;

   // copy NORMAL font (base font) to workspace

   _fmemcpy( &lfANSI, &NORMALFONT, sizeof( LOGFONT ) ) ;
   _fmemcpy( &lfOEM, &OEMFONT, sizeof( LOGFONT ) ) ;

   EMULATORFONT( FONT_STANDARD ) = CreateFontIndirect( &lfANSI ) ;
   EMULATORFONT( FONT_STANDARDOEM ) = CreateFontIndirect( &lfOEM ) ;

   lfANSI.lfUnderline =      1 ;
   EMULATORFONT( FONT_UNDERLINE ) = CreateFontIndirect( &lfANSI ) ;
   lfOEM.lfUnderline  =      1 ;
   EMULATORFONT( FONT_UNDERLINEOEM ) = CreateFontIndirect( &lfOEM ) ;

   // get text metrics

   DEFAULTFONT = SelectObject( SCREENDC, EMULATORFONT( FONT_STANDARD ) ) ;
   GetTextMetrics( SCREENDC, &tm ) ;
   XCHAR = tm.tmAveCharWidth ;
   YCHAR = tm.tmHeight + tm.tmExternalLeading ;

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

   SelectObject( SCREENDC, DEFAULTFONT ) ;
   DeleteObject( EMULATORFONT( FONT_STANDARD ) ) ;
   DeleteObject( EMULATORFONT( FONT_UNDERLINE ) ) ;
   DeleteObject( EMULATORFONT( FONT_STANDARDOEM ) ) ;
   DeleteObject( EMULATORFONT( FONT_UNDERLINEOEM ) ) ;

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
   int    i ;
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

   SCREENDC = GetDC( hWnd ) ;

   if (!CreateEmulatorFonts( hWnd, hECB ))
   {
      ReleaseDC( hWnd, SCREENDC ) ;
      LocalUnlock( hECB ) ;
      DestroyECB( hECB ) ;
      return ( NULL ) ;
   }
   SCREENBUFFER = NULL ;
   if (NULL == (SCREENBUFFER = CreateScreenBuffer( MAXROWS, MAXCOLS,
                                                   IBMPC_BYTESPERCHAR )))
   {
      ReleaseDC( hWnd, SCREENDC ) ;
      LocalUnlock( hECB ) ;
      DestroyECB( hECB ) ;
      return ( NULL )  ;
   }
   if (NULL == (CHARBUF = (LPSTR) GlobalLock( SCREENBUFFER )))
   {
      ReleaseDC( hWnd, SCREENDC ) ;
      LocalUnlock( hECB ) ;
      DestroyECB( hECB ) ;
      return ( NULL );
   }
   _fmemset( (LPSTR) CHARBUF, ' ', MAXROWS * MAXCOLS ) ;

   ATTRBUF = (LPWORD) (CHARBUF + MAXROWS * MAXCOLS) ;
   for (i = 0; i < MAXROWS * MAXCOLS; i++)
      *(LPWORD)(ATTRBUF + i) = ATTRIBUTE_DEFAULT ;

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

   CURROW =                 0 ;
   CURCOL =                 0 ;
   SAVEROW =                0 ;
   SAVECOL =                0 ;
   MAXCOLS =                IBMPC_COLUMNS ;
   MAXROWS =                IBMPC_ROWS ;
   ATTR =                   ATTRIBUTE_DEFAULT ;
   SEQUENCESTATE =          SEQUENCE_NONE ;
   LOCALECHO =              FALSE ;
   AUTOWRAP =               FALSE ;
   NEWLINE =                FALSE ;
   *ANSWERBACK =            NULL ;
   BLINKSTATE =             FALSE ;

   // setup default tab stops
   ClearTabStops( npECB ) ;
   for (i = 8; i < IBMPC_COLUMNS; i += 8)
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

   XSIZE =                  0 ;
   YSIZE =                  0 ;
   XOFFSET =                0 ;
   YOFFSET =                0 ;

   //
   // Setup the "Default" fonts...
   //

   FontInitLogFont( (LPLOGFONT) &NORMALFONT ) ;
   FontInitLogFont( (LPLOGFONT) &HIGHLIGHTFONT ) ;

   lstrcpy( NORMALFONT.lfFaceName, "COURIER" ) ;
   NORMALFONT.lfFGcolor = RGBLTGRAY ;
   NORMALFONT.lfBGcolor = RGBBLACK ;
   NORMALFONT.lfHeight  = 12 ;
   NORMALFONT.lfWidth   = 8 ;

   lstrcpy( HIGHLIGHTFONT.lfFaceName, "COURIER" ) ;
   HIGHLIGHTFONT.lfFGcolor = RGBWHITE ;
   HIGHLIGHTFONT.lfBGcolor = RGBBLACK ;
   HIGHLIGHTFONT.lfWidth = NORMALFONT.lfWidth ;
   HIGHLIGHTFONT.lfHeight = NORMALFONT.lfHeight ;

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
   _fmemset( TABSTOPS, 0, sizeof(TABSTOPS) ) ;

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
   TABSTOPS[nByte] |= (BYTE) (1 << nBit) ;

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
   TABSTOPS[nByte] &= (BYTE) ~(1 << nBit) ;

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
   return ( (TABSTOPS[nByte] & (BYTE) (1 << nBit)) != 0 );

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
   hECB = GETECB( hWnd ) ;

   if (NULL == (npECB = (NPECB) LocalLock( hECB )))
      return ( FALSE ) ;

   // destroy blink timer
   KillTimer( hWnd, BLINK_ID ) ;

   // Unlock screen buffer
   GlobalUnlock( SCREENBUFFER ) ;

   // destroy fonts
   DestroyEmulatorFonts( hWnd, hECB ) ;

   // Release stored DC
   ReleaseDC( hWnd, SCREENDC ) ;

   LocalUnlock( hECB ) ;

   // bump use count
   nUseCount-- ;

   DestroyScreenBuffer( hECB ) ;
   DestroyECB( hECB ) ;

   return ( TRUE ) ;

} /* end of DestroyEmulator() */

/************************************************************************
 * End of File: ibmpc.c
 ************************************************************************/

