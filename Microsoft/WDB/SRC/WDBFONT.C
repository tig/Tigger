/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** wdbfont.c
 *
 *  DESCRIPTION: 
 *      The following code selects a font based on a point setting (not
 *      pixels).  It is cool since it helps avoid the wrath of the font
 *      randomizer in GDI.
 *
 *  HISTORY:
 *      1/02/92     cek     Ported to Win32.  You're welcome curtis!
 *      4/30/91     cjp     For WDB, this will default to SYSTEM_FONT
 *                          if the font face name could not be found.
 *      4/14/91     cjp     Fixed some stupid bugs...
 *      4/13/91     cjp     Charlie wrote this code--I just adapted it for
 *                          WDB.  Thanks Tigger!
 *
 ** cjp */


/* the includes required to build this file */
    #include <windows.h>
    #include "wdb.h"
    #include "wdbi.h"


/* private function prototypes */
    int FAR PASCAL fnEnumReally( LPLOGFONT, LPTEXTMETRIC, short, LPLOGFONT );


/** int FAR PASCAL fnEnumReally( lpLogFont, lpTextMetrics, nFontType, lpLF )
 *
 *  DESCRIPTION: 
 *      Call back for EnumFonts and wdbReallyCreateFont().
 *
 *  NOTES:
 *      Don't forget to export this if you need stuff in the DS.
 *
 ** cjp */

int FAR PASCAL fnEnumReally( LPLOGFONT      lpLogFont,
                             LPTEXTMETRIC   lpTextMetrics,
                             short          nFontType,
                             LPLOGFONT      lpLF )
{
   #ifdef WIN32
   UNREFERENCED_PARAMETER(lpTextMetrics) ;
   UNREFERENCED_PARAMETER(nFontType) ;
   #endif
    /* copy it */
    *lpLF = *lpLogFont;

    /* only need to be called once! */
    return ( FALSE );
} /* fnEnumReally() */


/** HFONT FAR PASCAL wdbReallyCreateFont( hDC, lpszFace, nPointSize, wFlags )
 *
 *  DESCRIPTION: 
 *      Creates a font just like CreateFont(), but attempts to avoid the
 *      font randomizer.  The strategy used is this: give the 'Font Mapper'
 *      so much information that it has NO CHOICE but it give us the bloody
 *      font we ASKED for.
 *      
 *  ARGUMENTS:
 *      HDC hDC         :   The DC to use.  If NULL, the DISPLAY is used.
 *
 *      LPSTR lpszFace  :   Face name of the font desired.
 *
 *      short nPointSize:   The point size of the font desired.
 *
 *      WORD wFlags     :   Font style flags.  RCF_ITALIC, RCF_UNDERLINE,
 *                          RCF_BOLD and/or RCF_STRIKEOUT.
 *
 *  RETURN (HFONT):
 *      The return value is a handle to the new font.  It is NULL if the
 *      function fails.
 *
 *  NOTES:
 *
 ** cjp */

HFONT FAR PASCAL wdbReallyCreateFont( HDC   hDC,
                                      LPSTR lpszFace,
                                      int nPointSize,
                                      UINT  wFlags )
{
    LOGFONT     lf;
    HDC         hCurDC;
#ifdef WE_ARE_NOT_IN_A_DLL
    FARPROC     lpfnEnumReally;
#endif

    /*  First use font enumeration get the facename specified.  If that
     *  fails, use a font based off SYSTEM_FONT.
     */
#ifdef WE_ARE_NOT_IN_A_DLL
    if ( !(lpfnEnumReally = MakeProcInstance( fnEnumReally, hInst )) )
    {
        OD( "WDB: MakeProcInstance failed\r\n" );
        return ( FALSE );
    }
#endif

    /* if the DC wasn't specified then use the display... */
    if ( !hDC )
    {
        if ( !(hCurDC = GetDC( NULL )) )
        {
            OD( "WDB: Could not GetDC( NULL )\r\n" );
#ifdef WE_ARE_NOT_IN_A_DLL
            FreeProcInstance( lpfnEnumReally );
#endif
            return ( FALSE );
        }
    }

    /* just dup it */
    else hCurDC = hDC;

    /* valid DC? */
    if ( hCurDC )
    {
        /*  Enumerate.  If lfFaceName is not lpszFace then the font was
         *  not found.
         */
        if ( lpszFace )
        {
#ifdef WE_ARE_NOT_IN_A_DLL
            EnumFonts( hCurDC, lpszFace, lpfnEnumReally, (LPSTR)&lf );
#else
            EnumFonts( hCurDC, lpszFace, (FARPROC)fnEnumReally, (LPSTR)&lf );
#endif
        }

        if ( !lpszFace || lstrcmpi( lf.lfFaceName, lpszFace) )
        {
            OD( "WDB: Going with the STOCK FONT\r\n" );

            GetObject( GetStockObject( SYSTEM_FONT ),
                            sizeof( LOGFONT ), (LPSTR)&lf );
        }

        else OD( "WDB: Going with the CHOSEN FONT\r\n" );

        /* negate the 'height' for 'POINT SIZE'... */
        lf.lfWidth = 0 ;
        lf.lfHeight = -MulDiv( nPointSize, GetDeviceCaps( hCurDC, LOGPIXELSX ), 72 );
        lf.lfWeight = (wFlags & RCF_BOLD) ? FW_BOLD : FW_NORMAL;
        lf.lfItalic = (BYTE)((wFlags & RCF_ITALIC) == RCF_ITALIC);
        lf.lfStrikeOut = (BYTE)((wFlags & RCF_STRIKEOUT) == RCF_STRIKEOUT);
        lf.lfUnderline = (BYTE)((wFlags & RCF_UNDERLINE) == RCF_UNDERLINE);
    }

#ifdef WE_ARE_NOT_IN_A_DLL
    FreeProcInstance( lpfnEnumReally );
#endif

    /* if we snarfed one, release of it */
    if ( hCurDC != hDC )
        ReleaseDC( NULL, hCurDC );

    /* hold your breath! ...this SHOULD succeed now */
    return ( CreateFontIndirect( &lf ) );
} /* wdbReallyCreateFont() */


/** EOF: wdbfont.c **/
