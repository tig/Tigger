/*
             spit.c  (C) KindelCo Software Systems, 1987, 1988

                     SPIT - A print formatter for the PC

Author:   Charles E. Kindel, Jr. (tigger)

Started:  July 18, 1987
Version:  1.51

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
       /s   : Single sheet option.  Pauses before printing each page.
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
    Does not support color on 43line EGAs.  Until Borland supplies a CONIO
    library that supports 43line egas this program will be monochrome.

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
    June 1, 1988   1.50    Kindel     Set up for wildcard support.  As soon
                                      as TC is modified to automatically
                                      expand wildcards, the code I've added
                                      will handle multiple files.  For now, the
                                      user can type more than one file on the
                                      command line.
                                      Made the filename formatting in
                                      the header more robust.
                                      Commented out color until CONIO supports
                                      43 line EGA.
                                      Implemented single sheet option.
                                      Fixed problem with Compressed not
                                      turning off.
    June 2, 1988   1.51    Kindel     Added "expand wild" to the TC libraries
                                      thus this program will automatically
                                      expand wildcards.
												  Fixed problem with options: before, 
												  only lower case options were recongized
												  now it will recognize both upper and
												  lower case.

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
#define COMPOFF    0x12                /* Code for compressed off is 0x12 */
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
#define OPTSTRING  "l:L:p:P:wWc:C:i:I:o:O:sSeE"     /* option string for getopt (":" means that getopt expects an argument) */
#define LINE_OPT   'L'                 /* line number option */
#define PAGE_OPT   'P'                 /* page length option */
#define WIDE_OPT   'W'                 /* wide option (compressed 132 columns) */
#define COL_OPT    'C'                 /* column number option */
#define INDENT_OPT 'I'                 /* indent option */
#define HEAD_OPT   'H'                 /* header option */
#define FOOT_OPT   'F'                 /* footer option */
#define OUT_OPT    'O'                 /* output file option */
#define SINGLE_OPT 'S'                 /* single sheet option */
#define UNKNOWN_OPT'?'                 /* getopt returns this if it cant find the option */
#define FF_OPT     'E'                 /* output an extra formfeed at the end */
#define READ       "rt"                /* for fopen () */
#define WRITE      "wt"                /* for fopen () */
#define TRUE       1
#define FALSE      0
#define NOERR      1
#define ERROR      -1                  /* error to return on exit */
#define OUTFILEERR -2
#define INFILEERR  -3
#define CMNDERR    -4
#define OPTERR     -5


#define VERSION "1.51"



/* -------- DATA DEFINITONS ---------- */
long  NumLines,
      PageLen,
      NumCols,
      NumIndent;
char  header [81],
      footer [81];
short wideflg = FALSE,
      formflg = FALSE,
      singleflg = FALSE;


/* -------- FUNCTION PROTOTYPES --------- */
void sp_abort (short);
void sp_io (FILE *, FILE *);


/*==========================================================================*/
/*                             main                                         */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
main (int argc, char *argv[])
{    char   option;                    /* command line option */
     char   *infile,
            *outfile,
            *cur;

     FILE *fopen (), *in_f, *out_f;
     extern int getopt (int argc, char *argv[], char *optionS);
     extern int optind,                /* the argv index of the next argument */
                opterr;                /* set this to zero so that getopt dont print its own error */
     extern char *optarg;              /* points to the option argument */
     struct stat f_info;               /* structure for stat */
     long secs_now;

     NumLines = DEFLINES;
     PageLen = DEFPAGE;
     NumCols = DEFCOL;
     NumIndent = DEFINDENT;
     outfile = NULL;
     opterr = 0;
     out_f = stdout;

/*     textcolor (YELLOW);
     clrscr ();*/
     printf (" SPIT V%s", VERSION);
     printf (" -file printer and formatter- ");
     printf ("(c)1987,88 KindelCo Software Systems\n\n");

     if (argc == 1)                    /* if there is nothing on the command line then print docs */
         sp_abort (NOERR);

/*     textcolor (LIGHTMAGENTA);*/
     while ((option = getopt (argc, argv, OPTSTRING)) != EOF)
         switch (toupper (option))
         {  case LINE_OPT    : NumLines = strtol (optarg, NULL, 0); /* strtol converts a string to a long int. */
                               printf ("    spit: Option not implemented yet: /%c.\n", option);
                               sp_abort (CMNDERR);
                               break;
            case PAGE_OPT    : PageLen = strtol (optarg, NULL, 0);
                               printf ("    spit: Option not implemented yet: /%c.\n", option);
                               sp_abort (CMNDERR);
                               PageLen = strtol (optarg, NULL, 0);
                               break;
            case WIDE_OPT    : if (NumIndent == DEFINDENT)
                                  NumIndent = 8;
                               NumCols = 132;
                               wideflg = TRUE;
                               break;
            case OUT_OPT     : outfile = malloc (strlen (optarg));
                               outfile = optarg;
                               break;
            case INDENT_OPT  : NumIndent = strtol (optarg, NULL, 0);
                               break;
            case UNKNOWN_OPT : printf ("    spit: unknown command line option.\n");
                               sp_abort (CMNDERR);
                               break;
            case FF_OPT      : formflg = TRUE;
                               break;
            case SINGLE_OPT  : singleflg = TRUE;
                               break;
            default          : printf ("    spit: Option not implemented yet: /%c.\n", option);
                               sp_abort (CMNDERR);
                               break;
         }

   
     if (NumLines >= (PageLen - 8))
     {  printf ("    spit: The number of lines must not exceed the page length - 8.\n");
        sp_abort (OPTERR);
     }

     while (optind < argc)
     {    infile = (char*) malloc (strlen (argv[optind])+2);
          strcpy (infile, argv [optind++]);
          if ((in_f = fopen (infile, READ)) == NULL)
          {    printf ("     spit: can't open input file ");
               /*textcolor (WHITE);*/
               printf ("%s\n", infile);
               /*textcolor (LIGHTMAGENTA);*/
               sp_abort (INFILEERR);
          }
          if (outfile == NULL)
          {   outfile = malloc (4);
              strcpy (outfile, DEFOUT);
          }

          if ((out_f = fopen (outfile, WRITE)) == NULL)
          {   printf ("     spit: can't open output file ");
              /*textcolor (WHITE);*/
              printf ("%s\n", outfile);
              /*textcolor (LIGHTMAGENTA);*/
              sp_abort (OUTFILEERR);
          }

          printf ("Printing ");
          /*textcolor (WHITE);*/
          printf ("%s ", infile);
          /*textcolor (LIGHTMAGENTA);*/
          printf ("...");

          if (strlen (infile) > 19)
          {   strcpy (header, infile);
              cur = header;
              while (strlen (cur) > 16)
                 *(cur++) = '.';
              cur -= 3;
              strcpy (header, cur);
              strcat (header, " ");
          }
          else
          {   strcpy (header, infile);
              while (strlen (header) < 20)
                 strcat (header, " ");
          }

          strcat (header, "-  Revised ");
          stat (infile, &f_info);               /* get file date */
          strcat (header, ctime (&f_info.st_mtime));
          *strchr (header, '\n') = NULL;
          strcat (header, "  -");
          while (strlen (header) < 69)
             strcat (header, " ");
          strcat (header, "Page ");

          strcpy (footer, "                   -  Printed on \0");
          time (&secs_now);                    /* get time in seconds */
          strcat (footer, ctime (&secs_now));  /* make it into a string */
          *(strchr (footer, '\n')) = NULL;
          strcat (footer, "  -");
          sp_io (in_f, out_f);
          if (fclose (in_f))
            printf ("Error closing input file.");
          if (fclose (out_f))
            printf ("Error closing output file.");
          printf ("\n");
          free (infile);
     }
     printf ("\n");
     return;
}


/*==========================================================================*/
/*                             sp_io                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void sp_io (FILE *input_f, FILE *output_f)
{    char line [150];
     short LineCount = 0, PageCount = 0, i = 0;


     if (singleflg)
     {   printf ("\nInsert paper and press any key...");
         (void) getch ();
         printf ("%c                                 %c", CR,CR);
     }
     fprintf (output_f, "%c%c%c", ESC, INIT, COMPOFF);
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
             if (singleflg)
             {   printf ("Insert paper and press any key...");
                 (void) getch ();
                 printf ("%c                                 %c", CR,CR);
             }
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
                 if (singleflg)
                 {   printf ("Insert paper and press any key...");
                     (void) getch ();
                     printf ("%c                                 %c", CR,CR);
                 }
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

void sp_abort (short errcode)
{  char *help;

help= "  Usage - spit [options] <filename.ext>\n"
      "    Options:\n"
      "    /Pn  : Where n is the page length in lines.  Default is 66 lines.\n\n"
      "    /Ln  : Where n is the number of lines to be printed per page. \n"
      "           Default is 58 lines.\n\n"
      "    /W   : Sets compressed mode on IBM/EPSON dot matrix printers.  Sets the\n"
      "           number of columns to 132 and indents 8 characters.  Both\n"
      "           the columns and indentation can be altered with /cn and /in.\n\n"
      "    /Cn  : where n is the number of columns.  Default is 80 columns.\n\n"
      "    /In  : where n is the number of columns to indent from the left margin.\n\n"
      "    /S   : Single sheet option.  Pauses before printing each page.\n\n"
      " /O<fil> : Where <fil> is an output filename.  Default is the standard\n"
      "           print device (PRN).\n";

 /*textcolor (LIGHTGREEN);*/
 if (errcode != NOERR)
   printf ("\n");
 printf ("%s",help);
 exit (errcode);
 }

/*==========================================================================*/
/*                             End of spit - (c) KindelCo 1987              */
/*==========================================================================*/
