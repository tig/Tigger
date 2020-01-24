/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  rect.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Fills rect struct based on the given x, y coordinates
 *
 *   Revisions:  
 *     01.00.000  6/ 1/91 baw   Wrote it.
 *
 ************************************************************************/

#include "precomp.h"    // precompiled header stuff

#include "ibmpc.h"

/************************************************************************
 *  VOID NEAR GetStdCharRect( NPECB npECB, LPRECT lpRect,
 *                            int nXPos, int nYPos )
 *
 *  Description: 
 *     Fills rectangle with client coordinates given x and y of the
 *     character screen position
 *
 *  Comments:
 *      6/ 1/91  baw  Decided that this was a necesary function.
 *                    The code was littered with references to this
 *                    type of rect computation.  Also, the calculations
 *                    were off by one causing the paint routine to add a
 *                    character in both directions!  What a performance hit!  This should
 *                    BTW:  I wrote this essay because CEK likes comments
 *                    better than code! 8-)
 *
 *                    Oh yeah, these shitty drivers for this Genoa card
 *                    seem to have very slow BitBlt() routines. ARRGGHHH!
 *
 ************************************************************************/

VOID NEAR GetStdCharRect( NPECB npECB, LPRECT lpRect, int nXPos, int nYPos )
{
   lpRect -> top = nYPos * YCHAR - YOFFSET ;
   lpRect -> bottom = lpRect -> top + YCHAR ;
   lpRect -> left = nXPos * XCHAR - XOFFSET ;
   lpRect -> right = lpRect -> left + XCHAR ;

} /* end of GetStdCharRect() */

/************************************************************************
 * End of File: rect.c
 ************************************************************************/

