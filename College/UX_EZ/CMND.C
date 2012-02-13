/* cmnd.c

                 COMMAND - command loop (Cm)


Author:   Charles E. Kindel, jr. (tigger)

Started:  July 22, 1987
Version:  1.4


DESCRIPTION
    The command loop gets commands from the keyboard and executes them.  If
    it gets characters that are not commands it inserts them into the text.


REVISIONS
    Date            VERS       Name          Change
    July 22, 1987   1.00       Kindel        Original.
    July 25, 1987   1.2        Kindel        Move routines.
    July 28, 1987   1.30       Kindel        Delete routines.
    July 31, 1987   1.40       Kindel        Help, Goto line, minor fixes.

*/


/* IMPORTS -----------------------------------------------------------------*/
#include <string.h>                    /* strcat, strcpy, etc...            */
#include <stdio.h>                     /* printf                            */
#include <ctype.h>                     /* toupper, isspace                  */
#ifdef TURBOC
#include <stdlib.h>                    /* use TurboC's ltoa function        */
#endif
#include "ez.h"                        /* define void?                      */
#include "cmnd.h"
#include "term.h"                      /* terminal routines                 */
#include "disp.h"                      /* display routines.                 */
#include "move.h"                      /* insert/delete buffer routines.    */
#include "file.h"                      /* file access                       */
#include "buf.h"                       /* buffer management                 */
#include "input.h"                     /* input command table.              */


/* LOCAL SYMBOLIC DEFINITIONS ----------------------------------------------*/
#define VER        "2.50"
#define TRUE        1
#define FALSE       0
#define NEWLINE    '\n'
#define YES        'Y'                 /* for ABRT query                    */
#define WAIT        0                  /* wait for input in Xgetc           */


/* FUNCTION PROTOTYPES -----------------------------------------------------*/
#ifndef TURBOC                         /* Unix doesn't have this function   */
void Cmltoa ();                        /* convert long to string            */
#endif
int Cmatoi ();                         /* convert string to int.            */
void CmGoLine ();                      /* goto a line.                      */
void CmMsg (void);                     /* put the standard message.         */
int CmAbrtQuery (void);                /* ask if user wants to loose changes*/
void CmHelp (void);                    /* help screen */

/* FUNCTIONS ---------------------------------------------------------------*/
/*==========================================================================*/
/*                                Cmnd                                      */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*     Gets commands from the input command converter and processes them.   */
/*     This is the main command loop for the program.                       */
/*                                                                          */
/*  RETURNS                                                                 */
/*     Nothing                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void Cmnd (void)
{    int CmCommand, i;

     Bfmsgflg = TRUE;
     if ((Bfep - Bfbp == 1) || (Bfep >= Bflp - 1)) /* if newfile or buffer full then dont clear message */
     {   Bfmsgflg = FALSE;
         DpPaint ();
     }
     else
        CmVers ();

     for (;;)
     {     BfNukedLines = 0;
           switch (CmCommand = InpCmnd ())
           {    case TAB    :   for (i=1; i < 4; i++)
                                {   BfInsert (' ');
                                    DpInsert (' ');
                                } break;

                /* query user to see if he really wants to discard changes */
                case ABRT   :   if (CmAbrtQuery ())
                                {   TmRst ();
                                    Xrst ();          /* reset serial io */
                                    exit (0);
                                }
                                Bfmsgflg = TRUE;
                                CmMsg ();
                                break;

                /* Delete to beginning of line */
                case DBOL   :   CmMsg ();
                                if (Bfcur > Bfbp)
                                {   if (*(Bfcur-1) == NEWLINE)
                                    {    BfDel (-1);
                                         BfNukedLines++;
                                    }
                                    BfDel (BfLine (Bfcur, 0));
                                    BfNukedLines--;
                                    if (Bfcur < Bfsp)
                                    {while ((Bfsp != Bfbp) && (*Bfsp != NEWLINE))
                                             Bfsp--;
                                        if (Bfsp != Bfbp)
                                             Bfsp++;
                                        TmSetcur (0,0);
                                        TmDeol ();
                                        DpOutRow (Bfsp);
                                        DpMove ();
                                    }
                                    else
                                        DpDel ();
                                }
                                else
                                    BfBeep (BFTOP);
                                break;

                /* Delete to beginning of word */
                case DBOW   :   CmMsg ();
                                BfDel (BfWord (-1));
                                if (Bfcur < Bfsp)
                                {while ((Bfsp != Bfbp) && (*Bfsp != NEWLINE))
                                        Bfsp--;
                                    if (Bfsp != Bfbp)
                                        Bfsp++;
                                    TmSetcur (0,0);
                                    TmDeol ();
                                    DpOutRow (Bfsp);
                                    DpMove ();
                                }
                                else
                                    DpDel ();
                                break;

                /* Delete character under cursor */
                case DCHR   :   CmMsg ();
                                if (*Bfcur == NEWLINE) BfNukedLines++;
                                BfDel (1);
                                DpDel ();
                                break;

                /* Delete previous character */
                case DELP   :   CmMsg ();
                                if ((Bfcur > Bfbp) && (*(Bfcur-1) == NEWLINE))
                                    BfNukedLines++;
                                BfDel (-1);
                                if (Bfcur < Bfsp)
                                {while ((Bfsp != Bfbp) && (*Bfsp != NEWLINE))
                                        Bfsp--;
                                    if (Bfsp != Bfbp)
                                        Bfsp++;
                                    TmSetcur (0,0);
                                    TmDeol ();
                                    DpOutRow (Bfsp);
                                    DpMove ();
                                }
                                else
                                    DpDel ();
                                break;

                /* Delete to end on line (including newline) */
                case DEOL   :   CmMsg ();
                                BfDel (BfLine (Bfcur, 1));
                                DpDel ();
                                break;

                /* Delete to end of word */
                case DEOW   :   CmMsg ();
                                BfDel (BfWord (1));
                                DpDel ();
                                break;

                /* Delete to beginning of word */
                case MBOB   :   CmMsg ();
                                if (Bfcur == Bfbp)
                                BfBeep (BFTOP);
                                else
                                {   Bfcur = Bfbp;
                                    Bfsp = Bfbp;
                                    DpPaint ();
                                }
                                break;

                /* Move to beginning of line */
                case MBOL   :   CmMsg ();
                                if (Bfcur > Bfbp)
                                {   if (*(Bfcur-1) == NEWLINE)
                                       Bfcur--;
                                    BfMove (BfLine (Bfcur, 0));
                                    DpMove ();
                                }
                                else
                                    BfBeep (BFTOP);
                                break;

                /* Move to beginning of word */
                case MBOW   :   CmMsg ();
                                if (Bfcur == Bfbp)
                                   BfBeep(BFTOP);
                                else
                                {  BfMove (BfWord (-1));
                                   DpMove ();
                                }
                                break;

                /* Move to End of buffer */
                case MEOB   :   CmMsg ();
                                if (Bfcur < Bfep)
                                {   Bfcur = Bfep;
                                    BfMove (BfLine (Bfcur, -(TMROWS/2)));
                                    Bfsp = Bfcur;
                                    BfMove (BfLine (Bfcur, (TMROWS/2)+1));
                                    TmSetcur (0,0);
                                    TmClr ();
                                    Bfmsgflg = TRUE;
                                    CmMsg ();
                                    DpPaint ();
                                }
                                else
                                    BfBeep (BFBOT);
                                break;

                /* Move to end of line */
                case MEOL   :   CmMsg ();
                                if (Bfcur == Bfep)
                                    BfBeep (BFBOT);
                                else
                                {   if (*Bfcur == NEWLINE)
                                        Bfcur++;
                                    BfMove (BfLine (Bfcur, 1)-1);
                                    DpMove ();
                                }
                                break;

                /* Move to end of word */
                case MEOW   :   CmMsg ();
                                if (Bfcur == Bfep)
                                    BfBeep (BFBOT);
                                else
                                {   BfMove (BfWord (1));
                                    DpMove ();
                                }
                                break;

                /* Move down one line (to beginning of next line) */
                case MVDN   :   CmMsg ();
                                if (Bfcur == Bfep)
                                    BfBeep (BFBOT);
                                else
                                {   BfMove (BfLine (Bfcur, 1));
                                    DpMove ();
                                }
                                break;

                /* Move left one character */
                case MVLF   :   CmMsg ();
                                if (Bfcur > Bfbp)
                                {   Bfcur--;
                                    DpMove ();
                                }
                                else
                                    BfBeep (BFTOP);
                                break;

                /* Move to next page */
                case MVNP   :   CmMsg ();
                                if (Bfcur == Bfep)
                                    BfBeep (BFBOT);
                                else
                                {   BfMove (BfLine (Bfcur, 22));
                                    DpMove ();
                                }
                                break;

                /* Move to previous page */
                case MVPP   :   CmMsg ();
                                if (Bfcur == Bfbp)
                                    BfBeep (BFTOP);
                                else
                                {   BfMove (BfLine (Bfcur, -22));
                                    DpMove ();
                                }
                                break;

                /* Move right one character */
                case MVRT   :   CmMsg ();
                                if (Bfcur < Bfep)
                                {   Bfcur++;
                                    DpMove ();
                                }
                                else
                                    BfBeep (BFBOT);
                                break;

                /* move up to beginning of previous line */
                case MVUP   :   CmMsg ();
                                if (Bfcur == Bfbp)
                                    BfBeep (BFTOP);
                                else
                                {   BfMove (BfLine (Bfcur, -1));
                                    DpMove ();
                                }
                                break;

                /* Quit without saving (prompts if buffer has been changed */
                case QUIT   :   if (Bfmodflg == TRUE)
                                    CmWrit ();
                                TmWndw (0, TMROWS -1);
                                TmSetcur (0,TMROWS);
                                Xrst ();
                                printf ("\n");
                                exit (0);
                                break;

                /* Repaint screen, put version message on command bar. */
                case VERS   :   CmVers (); break;

                /* Write the buffer to file */
                case WRIT   :   CmWrit (); break;

                /* Move To a line.  Asks user for a line number and goes to */
                /* that line in the text.                                   */
                case MTLN   :   CmGoLine ();
                                CmMsg ();
                                break;

                /* Display help screen */
                case HELP   :   CmHelp ();
                                break;

                /* insert the character */
                default     :   CmMsg ();
                                if (BfInsert (CmCommand) != 0)
                                     DpInsert (CmCommand);
                                else
                                     BfBeep (BFFUL);
                                break;
           } /* switch */
     } /* for */
} /* end of main () */



/*==========================================================================*/
/*                           CmMsg                                          */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Tests if there is a message on the status line that needs to be     */
/*      updated.  Prints the normal status line.                            */
/*                                                                          */
/*  RETURNS                                                                 */
/*      Nothing                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void CmMsg (void)
{     char msg [TMCOLS];

      if (Bfmsgflg == TRUE)
      {   if (Bfmodflg == TRUE)
              strcpy (msg, "* File: ");
          else
              strcpy (msg, "  File: ");
          strcat (msg, BfFileNamep);
          strcat (msg, "     - EZ - KindelCo Software Systems");
          DpMsg (msg);
          Bfmsgflg = FALSE;
      }
}   /* End of CmMsg */


/*==========================================================================*/
/*                            CmWrit                                        */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Writes buffer to file.  Puts message on command line.               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void CmWrit (void)
{    char msg [TMCOLS], num [10];
     long numchars;

     numchars = Bfep - Bfbp;

#ifdef TURBOC
     ltoa (numchars, num, 10);
#else                                /* TurboC has it Unix doesn't */
     Cmltoa (num, numchars);
#endif
     strcpy (msg, num);
     strcat (msg, " characters written to ");
     strcat (msg, BfFileNamep);
     DpMsg (msg);
     if ((FlWrite (BfFileNamep, Bfbp, numchars)) != numchars-1)
        BfBeep (BFWFL);  /* write fails message */
     Bfmsgflg = TRUE;
     return;
} /* end of CmWrt */

/*==========================================================================*/
/*                            CmVers                                        */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Repaints the screen then puts the version number and copyright      */
/*      notice on the command line.                                         */
/*                                                                          */
/*  RETURNS                                                                 */
/*      Nothing                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void CmVers (void)
{    char msg [TMCOLS], num [10];

     strcpy (msg, "EZ - V");
     strcat (msg, VER);
     strcat (msg, " - KindelCo Software Systems (C)1987 | ");
#ifdef TURBOC
     ltoa (Bfep-Bfbp, num, 10);
#else
     Cmltoa (num, Bfep-Bfbp);
#endif
     strcat (msg, num);
     strcat (msg, " bytes, ");
#ifdef TURBOC
     ltoa (BfLineCount (), num, 10);
#else
     Cmltoa (num, BfLineCount ());
#endif
     strcat (msg, num);
     strcat (msg, " lines.");
     DpMsg (msg);
     DpPaint ();
     Bfmsgflg = TRUE;                  /* reset flag */
     return;
}
/* end of CmVers */

/*==========================================================================*/
/*                            CmAbrtQuery                                   */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Prompts user as to whether he wants to loose changes or not.        */
/*                                                                          */
/*  RETURNS                                                                 */
/*      Retrns int : True or false                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int CmAbrtQuery ()
{    char c;

     if (Bfmodflg == TRUE)
     {    DpMsg ("Loose changes made?!? (y or n):");
          c = toupper (Xgetc (WAIT));
          if (c == YES)
             return (TRUE);
          else
             return (FALSE);
     }
     return (TRUE);
} /* end of CmAbrtQuery */



/*==========================================================================*/
/*                            CmGoLine                                      */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Prompts the user for a line number to go to.  Then it uses BfLine   */
/*      and DpMove to position the cursor on that line.  If the user        */
/*      attempts to go beyond the end of the buffer the cursor will be      */
/*      placed at the end of the buffer.                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void CmGoLine (void)
{    char c[6];
     int i = 0, x, y;

     x = TmGetcur () & 0xFF;           /* Save old cursor position          */
     y = TmGetcur () >> 8;

     DpMsg ("Go to what line: ");      /* ask what line to go to            */
     TmAttr (TMINVR);
     TmSetcur (18, TMROWS-1);
     while (!isspace (c[i] = Xgetc (WAIT))) /* get the number (no edit)     */
     {    Xputc (c[i]);
          i++;
     }
     c[i] = NULL;
     TmAttr (TMNORM);
     TmSetcur (x,y);                   /* go back into text.                */
     Bfcur = Bfbp;                     /* point to beginning of buffer      */
     BfMove (BfLine (Bfcur, Cmatoi (c)-1));  /* move c lines                  */
     DpMove ();
     Bfmsgflg = TRUE;
     CmMsg ();
     return;
} /* End of CmGoLine  */



/*==========================================================================*/
/*                              Cmatoi                                      */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Converts an ascii string into an integer (from K&R)                 */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      char s[] : string to be converted to integer.                       */
/*                                                                          */
/*  RETURNS                                                                 */
/*      and integer.                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int Cmatoi (s)
char s[];
{    int i, n = 0;
     for (i=0; isspace(s[i]); i++)
         ;  /* skip white space */
     for (i = 0; s[i] >= '0' && s[i] <= '9'; i++)
         n = 10 * n + s[i] - '0';
     return (n);
} /* end of Cmatoi */

#ifndef TURBOC
/*==========================================================================*/
/*                            Cmltoa                                        */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Converts a long number to a null terminated ascii string.           */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      char s[]: address of string.                                        */
/*      long n : long integer to be converted.                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void Cmltoa (s, n)
char s[];
long n;
{   long i, j, sign;
    char c;


    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
         s[i++] = (char)(n % 10 + '0');
       }
    while ((n /= 10) > 0);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    for (i = 0, j = strlen (s) -1; i <j; i++, j--)
    {
        c = s[i];
        s[i] = s [j];
        s[j] = c;
    }
    s[strlen (s)+1] = NULL;
}
#endif

/*==========================================================================*/
/*                                CmHelp                                    */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Clears screen and displays a simple help screen.  Press any key     */
/*      to go back to edit.                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/


void CmHelp ()
{
TmClr ();
TmSetcur (30, 0);
TmAttr (TMINVR);
TmPuts (" -EZ- Help Screen ");
TmAttr (TMNORM);
TmSetcur (3, 2);
TmPuts ("^A  -  Move to beginning of text   | ^B  -  Move to beginning of line");
TmSetcur (3, 3);
TmPuts ("^C  -  Quit, Save changes          | ^D  -  Delete character under cursor");
TmSetcur (3, 4);
TmPuts ("^E  -  Delete to end of word       | ^F  -  Move to end of line");
TmSetcur (3, 5);
TmPuts ("^G  -  Delete to end of line       | ^H  -  Move left one character");
TmSetcur (3, 6);
TmPuts ("^I (TAB) - Insert four spaces      | ^J  -  Move down one line");
TmSetcur (3, 7);
TmPuts ("^K  -  Move up one line            | ^L  -  Move Right one character");
TmSetcur (3, 8);
TmPuts ("^O  -  Move to beginning of word   | ^P  -  Move to end of word");
TmSetcur (3, 9);
TmPuts ("^U  -  Delete to beginning of line | ^V  -  Prints version, bytes used");
TmSetcur (3, 10);
TmPuts ("^W  -  Delete to beginning of word |        lines.  Refreshes screen");
TmSetcur (3, 11);
TmPuts ("^Y  -  Move to previous page       | ^Z  -  Move to end of buffer");
TmSetcur (3, 12);
TmPuts ("Del  -  Delete previous character  | Return - New line");
TmSetcur (3, 14);
TmPuts ("               ESC-C  -  Abort the edit, does not save buffer");
TmSetcur (3, 15);
TmPuts ("               ESC-W  -  Write text buffer");
TmSetcur (3, 16);
TmPuts ("               ESC-G  -  Move to a specified line");
TmSetcur (3, 17);
TmPuts ("               ESC-H  -  This help screen");
TmSetcur (3, 18); 
TmPuts ("               ESC-(any Ctrl-Char)  -  Inserts Ctrl-Char");
TmSetcur (10, 21);
TmPuts ("      Copyright (C) KindelCo Software Systems 1987");
TmSetcur (40, TMROWS-1);

    DpMsg ("Press any key to exit help.");
    Xgetc (WAIT);
    DpPaint ();
    Bfmsgflg = TRUE;
    CmMsg ();
    return;
}

/*==========================================================================*/
/*    END OF  cmnd.c             (C) KindelCo Software Systems, 1987        */
/*==========================================================================*/
