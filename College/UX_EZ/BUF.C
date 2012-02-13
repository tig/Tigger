/* buf.c              (C) KindelCo Software Systems

                    BUFFER - buffer management for EZ (Bf)

Author:   Charles E. Kindel, Jr.

Started:  July 22, 1987
Version:  2.00

DISCRIPTION
    The buffer routines manage the buffer.  The delete or insert characters,
    move the cursor in the buffer.  And calculate the number of characters
    to move by lines or words.

REVISIONS
    DATE            VERS       NAME       CHANGE
    July 22, 1987   1.00       Kindel     Original, Bfinit, BfInsert
    July 23, 1987   1.30       Kindel     BfMove, BfLine, BfDel
    July 25, 1987   1.40       Kindel     BfWord, BfBeep.
    July 28, 1987   1.60       Kindel     Bugs in BfMove.
    July 29, 1987   1.70       Kindel     Bugs in BfLine, BfWord
    July 30, 1987   2.00       Kindel     Fixed minor bugs.
*/


/*==========================================================================*/
/*      Buf.c     Charles E. Kindel   (C) KindelCo Software Systems, 1987   */
/*==========================================================================*/

/* IMPORTS -----------------------------------------------------------------*/
#include "ez.h"
#include "buf.h"
#include "move.h"
#include "xsys.h"
#include "disp.h"
#include <ctype.h>

/* LOCAL SYMBOLIC DEFINITIONS ----------------------------------------------*/
#define NEWLINE   '\n'
#define TRUE      1
#define FALSE     0


/* DATA DEFINITIONS --------------------------------------------------------*/
/* all of these definitions are used in other routines.                     */
char BfBuffer [BUFSIZE];               /* the text buffer                   */
char *Bfbp;                            /* beginning of buffer               */
char *Bfsp;                            /* top of screen in buffer           */
char *Bfcur;                           /* current cursor pos.               */
char *Bfep;                            /* end of text in buffer             */
char *Bflp;                            /* end of physical buffer.           */
char *BfFileNamep;                     /* name of the file                  */
int Bfmsgflg = FALSE;                  /* is there a message on the cmndln  */
int BfNukedLines;                      /* how many lines have we deleted    */
                                       /*                (for disp routines */
int Bfmodflg = FALSE;                  /* has the buffer been modified?     */

/* FUNCIONS ----------------------------------------------------------------*/
/*==========================================================================*/
/*                         BfInit                                           */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Initializes the buffer.  Reads the file and sets the pointers.      */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      f_namep : pointer to a file name.                                   */
/*                                                                          */
/*  RETURNS                                                                 */
/*      Returns  -1 if it fails.                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void BfInit (f_namep)
char *f_namep;
{    long size = 0;

     Bfsp = Bfbp = Bfcur = BfBuffer;
     Bflp = BfBuffer + BUFSIZE;
     size = FlRead (f_namep, Bfbp, BUFSIZE-1);
     if (size == 1)
         BfBeep (BFNEW);
     if (size > BUFSIZE-1)
         BfBeep (BFFUL);
     Bfep = BfBuffer + size;
     return (0);
} /* End of Bfinit */


/*==========================================================================*/
/*                        BfInsert                                          */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*     Inserts one character into the buffer in front of the cursor.        */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*     char c : character to be inserted.                                   */
/*                                                                          */
/*  RETURNS                                                                 */
/*     returns a 0 if buffer is full.                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int BfInsert (c)
char c;
{      if (Bfep != Bflp)
       {    if (Bfcur ==  Bfep)
               MvDown (Bfcur, ++Bfep, 1);
            else
              MvDown (Bfcur, Bfcur + 1, (Bfep++) - Bfcur);
            *Bfcur++ = c;
            Bfmodflg = TRUE;
            return (1);
       }
       return (0);
} /* end of BfInsert */



/*==========================================================================*/
/*                              BfDel                                       */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Deletes n characters from the text buffer.                          */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      long n : number of characters to delete.                            */
/*                                                                          */
/*  RETURNS                                                                 */
/*      long : the number of characters actually deleted.                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void BfDel (n)
long n;
{    if (n != 0)
        if (n > 0)                           /* if we're going down         */
        {    if (Bfcur >= Bfep - 1)
             {    BfBeep (BFBOT);
                  return (0);
             }
             if ((Bfcur + n) >= (Bfep))
             {    n = (Bfep)  - Bfcur;
                  MvUp (Bfep - 1, Bfcur, n);
             }
             else
                 MvUp ((Bfcur + n), Bfcur, (Bfep + 1) - (Bfcur + n));
             Bfmodflg = TRUE;
             Bfep -= n;
             if (n == 0)
                BfBeep (BFBOT);
        }
        else                                  /* we're going up.             */
        {    if (Bfcur == Bfbp)
             {   BfBeep (BFTOP);
                 return (0);
             }
             if ((Bfcur + n) <= Bfbp)
             {    n =  (Bfbp - Bfcur);
                  if (n < 0)
                      MvUp (Bfcur, Bfbp, Bfep - Bfcur);
             }
             else
                  MvUp (Bfcur, (Bfcur + n), Bfep - Bfcur);
             Bfmodflg = TRUE;
             Bfcur += n;
             Bfep += n;
             if (n == 0)
                BfBeep (BFTOP);
        }
     return (n);
}  /* end of BfDel */




/*==========================================================================*/
/*                           BfBeep                                         */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Beeps and sends a message to the command line.                      */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      int msgnum : the number of the message (defined in Buf.h)           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void BfBeep (msgnum)
int msgnum;
{     Xbel ();
      switch (msgnum)
      {    case BFTOP : DpMsg (TOPMSG); break;
           case BFBOT : DpMsg (BOTMSG); break;
           case BFMOD : DpMsg (MODMSG); break;
           case BFNEW : DpMsg (NEWMSG); break;
           case BFRFL : DpMsg (RFLMSG); break;
           case BFWFL : DpMsg (WFLMSG); break;
           case BFFUL : DpMsg (FULMSG); break;
           default    : DpMsg (ERRMSG); break;
      }
      Bfmsgflg = TRUE;
      return;
} /* end of BfBeep */



/*==========================================================================*/
/*                             BfWord                                       */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Finds the number of characters to move n words in the text.         */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      long n : number of words to move in the text.                       */
/*                                                                          */
/*  RETURNS                                                                 */
/*      Returns the number of characters to move n words in the text.       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long BfWord (n)
long n;
{    char *curp = Bfcur;
     long i = 0;
     BfNukedLines = 0;
     if (n > 0)                         /* UP */
     {   if (isspace (*curp))
         {   if (*curp == NEWLINE) BfNukedLines++;
             while ((curp < Bfep) && (isspace (*++curp)))
             {    i++;
                  if (*curp == NEWLINE) BfNukedLines++;
             }
             if (curp != Bfep)
                 i++;
         }
         while (n--)
         {   while ((curp < Bfep) && (!isspace (*++curp)))
                 i++;
             if (curp != Bfep)
                 i++;
         }
     }
     else                                /* DOWN */
     {   if ((curp > Bfbp) && (!isspace (*(curp-1))) && (!isspace (*curp)))
         {   while ((curp != Bfbp) && (!isspace (*--curp)))
                i--;
             curp++;
             n++;
         }
         while (n++)
         {   while ((curp != Bfbp) && (isspace (*--curp)))
             {  if (*curp == NEWLINE) BfNukedLines++;
                i--;
             }
             while ((curp != Bfbp) && (!isspace (*curp)))
             {  i--;
                curp--;
             }
             if (curp == Bfbp)
                return (--i);
         }
     }
     return (i);
} /* end of BfWord */



/*==========================================================================*/
/*                             BfMove                                       */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Moves the cursor pointer n characters in the text buffer.           */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      long n : the number of characters to move (+) is down (-) is up.    */
/*                                                                          */
/*  RETURNS                                                                 */
/*      returns the number of characters actually moved.                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long BfMove (n)
long n;
{    int i;

     if (n > 0)                   /* move up */
     {    for (i = 0; n > i; i++)
          {    if (Bfcur == Bfep)
               {   BfBeep (BFBOT);
                   return (i);
               }
               Bfcur++;
          }
     }
     else                         /* move down */
     {    for (i = 0; n < i; i--)
          {    if (Bfcur == Bfbp)
                   return (i);
               Bfcur--;
          }
     }
     return (i);
}  /* end of BfMove */



/*==========================================================================*/
/*                              BfLine                                      */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Calculates the number of characters to move n lines in the text.    */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      char *tp : pointer to the current character in the text.            */
/*      long n   : the number of lines to move (-) is up (+) is down.       */
/*                                                                          */
/*  RETURNS                                                                 */
/*      long : the number of characters to move.                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
long BfLine (tp, n)
char *tp;
long n;
{    int i = 0;
     if (n > 0)                  /* Lines down */
     {    while (n--)
          {     if (tp == Bfep)
                    return (i);
                while (*tp++ != NEWLINE)
                    i++;
                BfNukedLines++;
                if (tp <= Bfep)
                    i++;
          }
          return (i);
     }
     else                         /* Lines up */
     {    if (tp == Bfbp)
              return (i);
          for (; n <= 0; n++)
          {    while ((--tp != Bfbp) && (*tp != NEWLINE))
                   i--;
               i--;
               if (*tp == NEWLINE) BfNukedLines++;
               if (tp == Bfbp)
                   return (--i);
          }
          return (++i);
     }
} /* end of BfLine */

/*===========================================================================*/
/*                            BfLineCount                                    */
/*                                                                           */
/*  DISCRIPTION                                                              */
/*      Counts the number of lines in the file.                              */
/*                                                                           */
/*  RETURNS                                                                  */
/*      long : number of lines in buffer.                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
long BfLineCount ()
{     char *tempp = Bfbp;
      long NumLines = 0;

      while (tempp != Bfep)
      {   while (*tempp++ != NEWLINE)
              ;
          NumLines++;
      }
      return (NumLines);
}        

/*===========================================================================*/
/* end of buf.c               (C) 1987 KindelCo Software Systems             */
/*===========================================================================*/