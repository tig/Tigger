/* move.c       move.c    (c) KindelCo Software Systems, 1987

                    MOVE - buffer move routines (Mv)

Author:   Charles E. Kindel, Jr. (tigger)

Started:  July 22, 1987
Version:  1.00

DESCRIPTION
    Move the text up or down in the buffer.  For deleting and inserting
    text.

*/

/*==========================================================================*/
/*  move.c        Charles E. Kindel   (C) KindelCo Software Systems, 1987   */
/*==========================================================================*/

/* IMPORTS -----------------------------------------------------------------*/
#include "ez.h"

/* FUNCTIONS ---------------------------------------------------------------*/

/*==========================================================================*/
/*                                MvUp                                      */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*     Move n characters up in the buffer (for deleting) from the source    */
/*     pointer to the destination pointer.                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MvUp (sp, dp, n)
char *sp, *dp;
unsigned n;
{      while (n--)
           *dp++ = *sp++;
}


/*==========================================================================*/
/*                                MvDown                                    */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*     Move n characters down in the buffer (for inserting) from the source */
/*     pointer to the destination pointer.                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MvDown (sp, dp, n)
char *sp, *dp;
unsigned n;
{       sp += n - 1;
        dp += n - 1;
        while (n--)
        *dp-- = *sp--;
}



/*==========================================================================*/
/* END OF  move.c                 (C) KindelCo Software Systems, 1987       */
/*==========================================================================*/
