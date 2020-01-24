/************************************************************************
 *
 *     Project:  WDB.DLL
 *
 *      Module:  monoout.c
 *
 *     Remarks:  Yanked out of DBWin
 *
 *   Revisions:
 *    4/3/92   cek   Yanked it.
 *
 *
 ***********************************************************************/

#include <windows.h>
#include "wdb.h"

#define WDB

//--------------------------------------------------------------------
//
// Monochrome video output routine
//
// void wdbMonoOut(LPCSTR lpsz)
//
// Output a string to the monochrome display (if present).  Handles
// carriage return, line feed, tab, bell, and the ANSI clear-screen
// escape sequence.
//
// BOOL wdbMonoOpen(void);
// BOOL wdbMonoClose(void);
//
// BOOL wdbMonoQuery(void);
//
// Returns TRUE if a secondary monochrome display exists.
//
// WARNING - PLEASE NOTE
//
// These output routines work by directly addressing the monochrome adapter
// video memory.  If an application or debugger is also using this memory
// (such as CodeView), debug output may trash the screen.
//

typedef struct
{
    char ch;
    BYTE attr;
} CA;

// NOTE: In order to be able to check to see if a debugger or another
// app is using the monochrome display, these routines do not use the
// top line of the display.
//
#define CCOLMAX         80
/*** WDB.DLL Change:  We *do* use all 25 lines.  (DWWIN uses 24)***/
#ifdef WDB
#define CROWMAX         25
#else
#define CROWMAX         24
#endif

// Macro used to access absolute exports from KERNEL
//
#define EXPVAL(name)    ((WORD)(void NEAR*)&name)

// Monochrome video memory base selector -- absolute exported by KERNEL.
//
extern WORD PASCAL __B000h;

#define ESC             27
#define BELL            7

#define DEFATTR         0x07
#define CA_SPACE        ((DEFATTR << 8) | ' ')

// Compute far pointer to character at row, col.  Note that top line is not used.
//
#ifdef WDB
/*** WDB.DLL Change:  We *do* use all 25 lines.  (DWWIN uses (row)+1) ***/
#define PCA(row, col)   MAKELP(EXPVAL(__B000h), (((row) + 0) * CCOLMAX + (col)) * sizeof(CA))
#else
#define PCA(row, col)   MAKELP(EXPVAL(__B000h), (((row) + 1) * CCOLMAX + (col)) * sizeof(CA))
#endif

static void NEAR PASCAL CACopy(CA FAR* pcaDst, CA FAR* pcaSrc, WORD cca);
static void NEAR PASCAL CAFill(CA FAR* pcaDst, WORD cca, WORD ca);
static BOOL NEAR PASCAL wdbMonoInUse(void) ;

int rowCur = 0;
int colCur = 0;

BOOL fMonoOpen = FALSE;

BOOL WINAPI wdbMonoOpen( BOOL fClear )
{
   #ifndef WIN32
    if (fMonoOpen)
        return FALSE;

    fMonoOpen = wdbMonoQuery();

    if (fMonoOpen && !wdbMonoInUse() && fClear)
    {
      CAFill(PCA(0, 0), CCOLMAX * CROWMAX, CA_SPACE);
      colCur = 0;
      rowCur = 0;
    }

    return fMonoOpen;
   #else
   return FALSE ;
   #endif
}

BOOL WINAPI wdbMonoClose(void)
{
   #ifndef WIN32
    if (!fMonoOpen)
        return FALSE;

    fMonoOpen = FALSE;
    return TRUE;
   #else
   return FALSE ;
   #endif
}

BOOL WINAPI wdbMonoOut(LPCSTR sz)
{
   #ifndef WIN32
    CA FAR* pca;
    char ch;

    // If monochrome isn't open, bail out.

    if (!fMonoOpen)
        return FALSE;

    // If the monochrome adapter is in use by another app,
    // don't output anything.
    //
    if (wdbMonoInUse())
        return FALSE;

    pca = PCA(rowCur, colCur);

    while (ch = *sz++)
    {
        switch (ch)
        {
        case '\b':
            if (colCur > 0)
            {
                colCur--;
                pca--;
                pca->ch = ' ';
                pca->attr = DEFATTR;
            }
            break;

        case BELL:
            MessageBeep(0);
            break;

        case '\t':
            pca    += 8 - colCur % 8;
            colCur += 8 - colCur % 8;
            break;

        case '\r':
            colCur = 0;
            pca = PCA(rowCur, colCur);
            break;

        default:
            pca->ch = ch;
            pca->attr = DEFATTR;
            pca++;
            colCur++;

            if (colCur < CCOLMAX)
                break;

            // fall through to handle LF

        case '\n':
            colCur = 0;
            rowCur++;

            if (rowCur >= CROWMAX)
            {
                CACopy(PCA(0, 0), PCA(1, 0), CCOLMAX * (CROWMAX - 1));
                CAFill(PCA(CROWMAX - 1, 0), CCOLMAX, CA_SPACE);
                rowCur = CROWMAX - 1;
            }

            pca = PCA(rowCur, colCur);
            break;

        case ESC:
            //
            // ANSI clear screen escape
            //
            if (sz[1] == '[' && sz[2] == '2' && sz[3] == 'J')
            {
                CAFill(PCA(0,0), CCOLMAX * CROWMAX, CA_SPACE);
                rowCur = colCur = 0;
                sz += 3;
            }
        }
    }
    return TRUE;
   #else
   return FALSE ;
   #endif
}

#pragma optimize("gle", off)

static void NEAR PASCAL CACopy(CA FAR* pcaDst, CA FAR* pcaSrc, WORD cca)
{
   #ifndef WIN32
    _asm {
        push    ds

        mov     cx,cca
        lds     si,pcaSrc
        les     di,pcaDst
        cld
        rep     movsw

        pop     ds
    }
   #endif
}

static void NEAR PASCAL CAFill(CA FAR* pca, WORD cca, WORD ca)
{
   #ifndef WIN32
    _asm {
        mov     ax,ca
        mov     cx,cca
        les     di,pca
        cld
        rep     stosw

    }
   #endif
}

BOOL WINAPI wdbMonoQuery(void)
{
   #ifndef WIN32
    BOOL fHasAdapter;

    // Query BIOS for display configuration.
    // Return TRUE if secondary display is a monochrome adapter.
    //
    _asm
    {
        mov     ax,1a00h
        int     10h
        xor     ax,ax
        cmp     bh,1                ; return TRUE if secondary adapter
        jnz     nomono              ; is a monochrome adapter
        inc     ax
    nomono:
        mov     fHasAdapter,ax
    }
    return fHasAdapter;
   #else
   return FALSE ;
   #endif
}

// BOOL MonoInUse(void)
//
// This routine checks to see if the monochrome adapter is in use
// by a debugger or another app.  This is done by checking to see
// if there are any characters on the top line of the display other
// than space.
//
// It's done in this way for speed, and to avoid calling any Kernel routines.
// In order to ensure that this trick works, MonoOut does not use the
// top line of the display.
//
/*** WDB.DLL Change:  We nop this call ***/
static BOOL NEAR PASCAL wdbMonoInUse(void)
{
   #ifndef WIN32

#ifdef WDB
   return FALSE ;
#else   
    BOOL fCVW = FALSE;

    _asm
    {
        push    si

        mov     ax,offset __B000h
        mov     es,ax
        cmp     word ptr es:[0],CA_SPACE
        jnz     InUse
        cmp     word ptr es:[4],CA_SPACE
        jnz     InUse
        cmp     word ptr es:[8],CA_SPACE
        jnz     InUse
        cmp     word ptr es:[16],CA_SPACE
        jz      NotInUse
InUse:
        inc     fCVW                ; CodeView is running: fCVW = TRUE
NotInUse:
    }
    return fCVW;
#endif

   #else
   return FALSE ;
   #endif

}

#pragma optimize("", on)
/************************************************************************
 * End of File: monoout.c
 ***********************************************************************/

