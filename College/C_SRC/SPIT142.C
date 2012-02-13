/*
             spit.c  (C) KindelCo Software Systems, 1987, 1988

                     SPIT - A print formatter for the PC

Author:   Charles E. Kindel, Jr. (tigger)

Started:  July 18, 1987
Version:  1.42

SYNOPIS
    spit [/ln /pn /w /cn /in /hs /fs /o<file> /s] <filename.ext>

       /ln  : where n is the number of lines to be printed on each page.
              Must be < (PageLen - 4).  Default is 56 lines.
       /pn  : where n is the page length in lines.  Default is 66 lines.
       /w   : sets compressed mode on IBM/EPSON dot matrix printers.  Sets the
              number of columns to 132.  Should come before the /c option on
              the command line.
       /cn  : where n is the number of columns.  Lines read in longer than
              n will be truncated.  The /i option reduces n by the number
              of columns to indent.  Lines longer than n will be truncated.
              Default for column is 80.  Use /w for 132 columns on IBM/EPSON
              dot matrix printers.
       /in  : where n is the number of columns to indent from the left margin.
              This option reduces the total number of columns specified with
              the /c and /w options.
       /hs  : where s is a string of characters representing the header to
              replace the default header which displays the filename and
              revision date.  Must be shorter than
              ((number of columns - indent) - 10) characters long.
       /fs  : where s is a string of characters representing the footer to
              replace the defalut which displays "printed on <date>".  Must
              be shorter than ((number of columns - indent) - 12) chars. long.
   /o<file> : Where <file> is an output filename.  Default is the standard
              print device (PRN).

DESCRIPTION
    "spit" spits a file with page breaks, headers, footers, page numbers,
    and indentation out on the printer.  It supports the IBM/EPSON dot matrix
    printers, and uses compressed and double strike characters.

    In a nutshell spit just echo's characters to the output device (the
    printer or a file).  But it counts lines and sends headers, footers,
    and page breaks.  If a line is longer than the number of columns
    specified (default is 80) that line is truncated.

    The headers and footers mentioned above are printed in double strike.
    The last page of the printout will have the string "end of file" in the
    footer.

LIMITATIONS
    The program will work on IBM/EPSON compatible dot matrix printers only.
    It is possible to change the #defines below to match your printer.

REVISIONS
    DATE           VER     NAME       CHANGE
    July 18, 1987  1.00    Kindel     Original
    July 20, 1987  1.20    Kindel     Fixed bugs, Incuded "revised on"
                                      Added several features.
    July 26, 1987  1.21    Kindel     Fixed footer on last page.
    Aug 31, 1987   1.40    Kindel     Added Formfeed support, fixed footers
                                      again.
    Oct 11, 1987   1.41    Kindel     Lines that were longer than MaxCols
                                      previously were thought to be truncated
                                      But were really being treated as new
                                      lines.  Fixed line counting to account
                                      for this.  Made messages go to STDOUT
                                      instead of STDERR.
    June 1, 1988   1.42    Kindel     Added color.

*/

/* -------- IMPORTS --------- */
#include <stdio.h>
#include <process.h>                   /* exit () */
#include <alloc.h>
#include <string.h>                    /* strtol (string to long) */
#include <dos.h>                       /* getdate */
#include <time.h>
#include <stat.h>
#include <conio.h>

/* -------- #defines --------- */

/* Printer specific definitions: */
#define ESC        0x1B                /* ASCII code for Escape is 0x1b  */
#define INIT       0x00                /* Initialize the Printer */
#define COMPON     0x0F                /* Epson code for compressed text is ESC,0x0e */
#define COMPOFF    0x12                /* Code for compressed off is ESC,0x12 */
#define FF         0x0C                /* Form Feed */
#define LF         0x0A                /* Line Feed */
#define CR         0x0D                /* Carraige Return */
#define BOLDON     0x45                /* Bold Print on */
#define BOLDOFF    0x46                /* Bold Print off */

/* Default definitions */
#define DEFLINES   56                  /* default number of printed lines */
#define DEFPAGE    66                  /* default page length */
#define DEFCOL     80                  /* default number of columns */
#define DEFINDENT  0                   /* default indentation */
#define DEFOUT     "PRN"               /* defalut output file name */

/* Other definitions */
#define OPTSTRING  "l:p:wc:i:o:se"  /* option string for getopt (":" means that getopt expects an argument) */
#define LINE_OPT   'l'                 /* line number option */
#define PAGE_OPT   'p'                 /* page length option */
#define WIDE_OPT   'w'                 /* wide option (compressed 132 columns) */
#define COL_OPT    'c'                 /* column number option */
#define INDENT_OPT 'i'                 /* indent option */
#define HEAD_OPT   'h'                 /* header option */
#define FOOT_OPT   'f'                 /* footer option */
#define OUT_OPT    'o'                 /* output file option */
#define SINGLE_OPT 's'                 /* single sheet option */
#define UNKNOWN_OPT'?'                 /* getopt returns this if it cant find the option */
#define FF_OPT     'e'                 /* output an extra formfeed at the end */
#define ERROR      2                   /* error to return on exit */
#define READ       "r"                 /* for fopen () */
#define WRITE      "w"                 /* for fopen () */
#define TRUE       1
#define FALSE      0


#define VERSION "2.42"



/* -------- DATA DEFINITONS ---------- */
long  NumLines,
      PageLen,
      NumCols,
      NumIndent;
char  header [80],
      footer [80];
short wideflg = 0,
      formflg = 0;




/* -------- FUNCTION PROTOTYPES --------- */
void sp_abort (void);
void sp_io (FILE *, FILE *);


/*==========================================================================*/
/*                             main                                         */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
main (int argc, char *argv[])
{    char   option;                    /* command line option */
     char   *infile,
            *outfile;
     FILE *fopen (), *in_f, *out_f;
     extern int getopt (int argc, char *argv[], char *optionS);
     extern int optind,                /* the argv index of the next argument */
                opterr;                /* set this to zero so that getopt dont print its own error */
     extern char *optarg;              /* points to the option argument */
     struct stat f_info;               /* structure for stat */
     long secs_now;

     directvideo = FALSE;              /* we don't want direct video! */

     NumLines = DEFLINES;
     PageLen = DEFPAGE;
     NumCols = DEFCOL;
     NumIndent = DEFINDENT;
     outfile = NULL;
     opterr = 0;
     out_f = stdout;

     textcolor (YELLOW);
     cprintf (" SPIT V%s", VERSION);
     cprintf (" -file printer and formatter- ");
     cprintf ("(c)1987,88 KindelCo Software Systems\n\n");

     if (argc == 1)                    /* if there is nothing on the command line then print docs */
         sp_abort ();

     textcolor (LIGHTMAGENTA);
     while ((option = getopt (argc, argv, OPTSTRING)) != EOF)
         switch (option)
         {  case LINE_OPT    : NumLines = strtol (optarg, NULL, 0); /* strtol converts a string to a long int. */
                               break;
            case WIDE_OPT    : NumCols = 132;
                               NumIndent = 8;
                               wideflg++;
                               break;
            case OUT_OPT     : outfile = malloc (strlen (optarg));
                               outfile = optarg;
                               break;
            case INDENT_OPT  : NumIndent = strtol (optarg, NULL, 0);
                               break;
            case UNKNOWN_OPT : cprintf ("    spit: bad command line option...\n", option);
                               sp_abort ();
                               break;
            case FF_OPT      : formflg++;
                               break;
            default          : cprintf ("    spit: Option not implemented yet.\n");
                               sp_abort ();
                               break;
         }

     infile = malloc (256);

     while (optind < argc)                /* get each input file */
     {    strcpy (infile, argv [optind]);
          if ((in_f = fopen (infile, READ)) == NULL)
          {    cprintf ("     spit: can't open input file ");
               textcolor (WHITE);
               cprintf ("%s\n", infile);
               textcolor (LIGHTMAGENTA);
               sp_abort ();
          }
          optind--;
          if (outfile == NULL)
          {   outfile = malloc (4);
              strcpy (outfile, DEFOUT);
          }

          if ((out_f = fopen (outfile, WRITE)) == NULL)
          {   cprintf ("     spit: can't open output file ");
              textcolor (WHITE);
              cprintf ("%s\n", outfile);
              textcolor (LIGHTMAGENTA);
              sp_abort ();
          }

          strcpy (header, infile);
          while (strlen (header) < 20)
             strcat (header, " ");
          strcat (header, "-  Revised ");
          stat (infile, &f_info);               /* get file date */
          strcat (header, ctime (&f_info.st_mtime));
          *strchr (header, '\n') = NULL;
          strcat (header, "  -");
          while (strlen (header) < 69)
             strcat (header, " ");
          strcat (header, "Page ");

          while (strlen (footer) < 20)
             strcat (footer, " ");
          strcat (footer, "-  Printed on ");
          time (&secs_now);                    /* get time in seconds */
          strcat (footer, ctime (&secs_now));  /* make it into a string */
          *(strchr (footer, '\n')) = NULL;
          strcat (footer, "  -");
          sp_io (in_f, out_f);
     }
     cprintf ("\n");
     return;
}


/*==========================================================================*/
/*                             sp_io                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void sp_io (FILE *input_f, FILE *output_f)
{    char line [150];
     short LineCount = 0, PageCount = 0, i;

     fprintf (output_f, "%c%c", ESC, INIT);
     if (wideflg != 0) fprintf (output_f, "%c%c", ESC, COMPON);

     fprintf (output_f, "\n%c%c", ESC, BOLDON);
     fprintf (output_f, "  %s %d", header, ++PageCount);
     fprintf (output_f, "%c%c\n\n\n", ESC, BOLDOFF);


     while (fgets (line, (NumCols - NumIndent), input_f) != NULL)
     {
         if (LineCount == NumLines)
         {   LineCount = 0;
             fprintf (output_f, "\n\n\n");
             fprintf (output_f, "%c%c%s%c", ESC, BOLDON, footer, FF);
             fprintf (output_f, "\n %s %d", header, ++PageCount);
             fprintf (output_f, "%c%c\n\n\n", ESC, BOLDOFF);
         }

         if (NumIndent > 0)
            for (i = 1; i < NumIndent; i++)
               fprintf (output_f, " ");

         i = 0;
         while ((line[i] != '\n') && (line[i] != NULL) && (i <= (NumCols - NumIndent)))
         {   if (line[i] == FF)
             {   for (; LineCount < NumLines; LineCount++)
                    fprintf (output_f, "\n");
                 fprintf (output_f, "%c%c", ESC, BOLDON);
                 fprintf (output_f, "\n\n\n%s%c", footer, FF);
                 fprintf (output_f, "\n %s %d", header, ++PageCount);
                 fprintf (output_f, "%c%c\n\n\n", ESC, BOLDOFF);
                 LineCount = 0;
             }
             else
                 fputc (line[i], output_f);
             i++;
         }
         fputc ('\n', output_f);
         LineCount++;
     }
     for (; LineCount < NumLines; ++LineCount)
        fprintf (output_f, "\n");
     fprintf (output_f, "%c%c", ESC, BOLDON);
     fprintf (output_f, "\n\n\n%s%c%c", footer, ESC, BOLDOFF);
     fprintf (output_f, "%c", FF);

     fprintf (output_f, "%c",COMPOFF);

     if (formflg != 0)
        fprintf (output_f, "%c", FF);
     return;
}

/*==========================================================================*/
/*                     Abort                                                */
/*==========================================================================*/

void sp_abort (void)
{  char *help;

help= "  Usage -\n\n"
      "    spit [/ln /pn /w /cn /in /hs /fs /o<file> /s] <filename.ext>\n"
      "    /ln  : where n is the number of lines to be printed on each page. Must be\n"
      "           less than (PageLen - 4).  Default is 58 lines.\n"
      "    /pn  : where n is the page length in lines.  Default is 66 lines.\n"
      "    /w   : sets compressed mode on IBM/EPSON dot matrix printers.  Sets the\n"
      "           number of columns to 132.  Should come before the /c option on\n"
      "           the command line.\n"
      "    /cn  : where n is the number of columns.  Lines read in longer than\n"
      "           n will be truncated.  Lines longer than n will be truncated.\n"
      "           Default is 80 columns.\n"
      "    /in  : where n is the number of columns to indent from the left margin.\n"
      "           This option reduces the total number of columns specified with\n"
      "           the /c and /w options.\n"
   /* "    /hs  : where s is a string of characters representing the header to\n"
      "           replace the default header.  Must be shorter than ((number of\n"
      "           columns - indent) - 10) characters long.\n"
      "    /fs  : where s is a string of characters representing the footer to\n"
      "           replace the default.  Must be shorter than ((number of columns -\n"
      "           indent) - 12) chararcters long.\n" */
      " /o<fil> : Where <fil> is an output filename.  Default is the standard\n"
      "           print device (PRN).";

 textcolor (LIGHTGREEN);
 textbackground (BLACK);
 cprintf ("\n%s\n",help);
 exit (ERROR);
 }

/*==========================================================================*/
/*                             End of spit - (c) KindelCo 1987              */
/*==========================================================================*/
