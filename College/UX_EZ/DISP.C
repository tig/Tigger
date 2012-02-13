 /* disp.c        (c) KindelCo Software Systems, 1987

                  DISPLAY - terminal display routines (Dp)


Author:   Charles E. Kindel, Jr. (tigger)

Started:  July 17, 1987
Version:  2.50

DESCRIPTION
    The display routines put characters on the display.  Scrolling, repaining
    etc.. are all done here.

REVISIONS
    DATE            VERS      NAME       CHANGE
    July 17, 1987   1.00      Kindel     original
    July 22, 1987   1.20      Kindel     Paint, init.
    July 28, 1987   2.00      Kindel     Major re write.
    July 29, 1987   2.10      Kindel     Fixed bugs. Added scrolling.
    July 30, 1987   2.30      Kindel     More bugs. Made code cleaner.
    July 31, 1987   2.50      Kindel     More bugs.
*/

/*==========================================================================*/
/*  disp.c        Charles E. Kindel   (C) KindelCo Software Systems, 1987   */
/*==========================================================================*/

/* IMPORTS -----------------------------------------------------------------*/
#include "ez.h"
#include "buf.h"
#include "disp.h"
#include "term.h"
#include <stdio.h>

/* LOCAL SYMBOLIC DEFINITIONS ----------------------------------------------*/
#define NEWLINE '\n'
#define FALSE   0
#define TRUE    1

/* DATA DEFINITIONS --------------------------------------------------------*/
long Px, Py;              /* current cursor positions */

/* FUNCTIONS ---------------------------------------------------------------*/
/*==========================================================================*/
/*                                DpInit                                    */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      Initializes the display window.                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void DpInit (void)
{       TmInit ();
        TmAttr (TMNORM);
        TmWndw (0, TMROWS -2);
        TmClr ();
        return;
}


/*==========================================================================*/
/*                                DpPaint                                   */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      Repaints the whole screen given the pointer to the top of the screen*/
/*      in the text.                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void DpPaint (void)
{       int Rows = 1;
        char *Curp = Bfsp;

        TmSetcur (0,0);
        TmDeol ();
        DpOutRow (Curp);
        while ((++Rows < TMROWS) && (Curp < Bfep))
        {    while (*Curp++ != NEWLINE);
             TmSetcur (0, Rows-1);
             TmDeol ();
             DpOutRow (Curp);
        }
        if (Curp == Bfep)
        {   TmSetcur (0, Rows);
            TmDeol ();
        }
        DpFindPos ();
        TmSetcur (Px, Py);
        return;
}


/*==========================================================================*/
/*                             DpMove                                       */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Given the x and y coordinates of where the cursor is to go on the   */
/*      screen this routine adjusts the display (scrolling if neccissary)   */
/*      to put the display cursor on the buffer cursor.                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void DpMove (void)
{       int Paintflg = FALSE;

        DpFindPos ();                 /* Find x and y coordinates           */

        /* if we have to move more than 1 screen then don't scroll, jump   */
        if ((Py > (TMROWS -1)+(TMROWS/2)) || (Py < (-1)-(TMROWS/2)))
             Paintflg = TRUE;

        /* if the y coordinate is greater than the bottom of the screen     */
        while (Py > TMROWS - 2)
        {    if (Paintflg == FALSE)
             {   TmSetcur (0, 0);
                 TmDelrow (1);
             }
             while ((*Bfsp != NEWLINE) && (Bfsp < Bfep))
                Bfsp++;
             if ((*Bfsp == NEWLINE) && (Bfsp < Bfep))
                Bfsp++;
             if (Paintflg == FALSE)
             {   TmSetcur (0, TMROWS - 2);
                 DpOutRow (DpFindNext ());
             }
             Py--;
        }

        /* While the y coordinate is above the top of the screen */
        while (Py < 0)
        {    if (Paintflg == FALSE)
             {   TmSetcur (0, 0);
                 TmInsrow (1);
             }
             if (Bfsp > Bfbp)
                Bfsp--;
             if ((Bfsp > Bfbp) && (*Bfsp == NEWLINE))
                Bfsp--;
             while ((*Bfsp != NEWLINE) && (Bfsp != Bfbp))
                Bfsp--;
             if ((*Bfsp == NEWLINE) && (Bfsp != Bfbp))
                Bfsp++;
             if (Paintflg == FALSE)
             {   TmSetcur (0, 0);
                 DpOutRow (Bfsp);
             }
             Py++;
        }
        if (Paintflg == TRUE)
           DpPaint ();
        else
           TmSetcur (Px, Py);
        return;
} /* end of DpMove */


/*==========================================================================*/
/*                                DpInsert                                  */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      Inserts a character onto the screen at the current cursor postion   */
/*      If it is a newline it scrolls the screen.                           */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      char c : character to be inserted                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void DpInsert (c)
char c;
{   int x, y;

    x = TmGetcur () & 0xFF;   /* lowbits */
    y = TmGetcur () >> 8;     /* highbits */


    if (c == NEWLINE)
    {    TmDeol ();
         if (y == TMROWS - 2)
         {   TmSetcur (0,0);
             TmDelrow (1);
             TmSetcur (0,y);
             while (*Bfsp++ != NEWLINE);
         }
         else
         {   TmSetcur (0, ++y);
             TmInsrow (1);
         }
         DpOutRow (Bfcur);
         TmSetcur (0, y);
    }
    else
    {    TmDeol ();
         DpOutRow (Bfcur - 1);
         TmSetcur (x+1, y);
    }
    return;
} /* end of DpInsert */



/*==========================================================================*/
/*                           DpDel                                          */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Deletes n characters from the display.  If a routine deleted        */
/*      Newlines from the text, then scroll the screen acordingly           */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      int n : number of characters to delete (+) del forward, (-) del     */
/*              backward.                                                   */
/*                                                                          */
/*  RETURNS                                                                 */
/*      Nothing                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void DpDel (void)
{   DpFindPos ();
    DpMove ();
    TmDeol ();
    DpOutRow (Bfcur);
    DpMove ();
    while (BfNukedLines-- > 0)
   {   if (Py < TMROWS-2)
       {    TmSetcur (0,Py+1);
            TmDelrow (1);
            TmSetcur (0,TMROWS-2);
            DpOutRow (DpFindNext ());
       }
       TmSetcur (Px,Py);
    }
    BfNukedLines = 0;
    return;
} /* end of DpDel */




/*==========================================================================*/
/*                                DpMsg                                     */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      Puts a message at to bottom of the screen                           */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      char *Msgp : pointer to the string to display.                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void DpMsg (Msgp)
char *Msgp;
{       char Blank [2];
        int i,
        x,
        y;

        Blank [0]= ' '; Blank [1] = NULL;
        x = TmGetcur () & 0xFF;   /* lowbits */
        y = TmGetcur () >> 8;     /* highbits */
        TmSetcur (0, TMROWS);
        TmAttr (TMINVR);
        TmPuts (Blank);
        TmPuts (Msgp);
        i = 1;
        while ((strlen (Msgp) + (i++)) < TMCOLS)
           TmPuts (Blank);
        TmAttr (TMNORM);
        TmSetcur (x, y);
        return;
} /* end of DpMsg */


/*==========================================================================*/
/*                           DpFindPos                                      */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Finds the cursor position for the display based on where it finds   */
/*      the buffer cursor.  If the buffer cursor is above the top of the    */
/*      display then Py will be less than 0.  If the buffer cursor is below */
/*      the bottom of the screen then Py will be greater than TMROWS-2.     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void DpFindPos (void)
{       char *Posp;
        Px = 0;
        Py = 0;

        Posp = Bfsp;
        if (Posp <= Bfcur)
            while ((Posp != Bfcur) && (Posp < Bfep))
            {   while ((*Posp != NEWLINE) && (Posp != Bfcur))
                {   Px++;
                    Posp++;
                }
                if (Posp == Bfcur)
                    return;
                Py++;
                Px = 0;
                Posp++;
            }
        else
        {   if ((Posp > Bfbp+1) && (*(Posp-2) == NEWLINE))
                Py--;
            if (*Posp == NEWLINE)
                Py++;
            while ((Posp != Bfcur) && (Posp > Bfbp))
            {   while ((*Posp != NEWLINE) && (Posp != Bfcur))
                    Posp--;
                if (Posp == Bfcur)
                    return;
                Py--;
                Posp--;
            }
        }
        return;
} /* end of DpFindPos */


/*==========================================================================*/
/*                           DpOutRow                                       */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Outputs a row or the rest of a row on the screen starting at the    */
/*      position in the buffer at "Posp".                                   */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      char *Posp : pointer to the character to start displaying from.     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void DpOutRow (Posp)
char *Posp;
{        char  *Outp, *Endp;

         Outp = Endp = (char*) malloc (TMCOLS + 1);

         while ((*Posp != NEWLINE) && (Posp < Bfep))
         {     *Endp++ = *Posp;
               Posp++;
         }
         *Endp = NULL;
         TmPuts (Outp);
         return;
} /* end of DpOutRow */


/*==========================================================================*/
/*           fuck               DpFindNext                                      */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Finds the first character of the row immeadiately below the bottom  */
/*      of the screen (under the message bar).                              */
/*                                                                          */
/*  RETURNS                                                                 */
/*      the pointer to the first character of the next row.                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
char *DpFindNext ()
{    int Rows=0;
     char *Curp = Bfsp;

        while ((++Rows < TMROWS-1) && (Curp < Bfep))
            while (*Curp++ != NEWLINE);
        return (Curp);
} /* End of DpFIndNext */


/*==========================================================================*/
/* END OF  disp.c                 (C) KindelCo Software Systems, 1987       */
/*==========================================================================*/
