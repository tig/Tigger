/*
              fgrep.c    (c) Charles E. Kindel, Jr., 1987


Author:   Charles E. Kindel, Jr. (tigger)

Started: June 30, 1987
Version: 1.3

SYNOPSIS
    fgrep [-f keyfile]
    fgrep keyword1 keyword2 keywordn
      -f Keywords are taken from keyfile (one keyword per line)

DESCRIPTION
    Fgrep searches the standard input for the keywords found in
    "keyfile" (-f option).  Without the -f option, it will
    search for the keywords listed on the command line.  If one
    or more keywords are found on an input line, the entire
    line is written to standard output.  This program makes no
    accomodation for characters such as $ * [ ^ | () and \ in
    keywords taken from the command line, although they should work
    from an input file.

EXAMPLES
    fgrep -f keys < *.c
    fgrep trie grep <fgrep.c >lines

IMPLEMENTATION
   The string list is compiled into a trie structure augmented with
   failure links as described in "Efficient String Matching: an Aid
   to Bibliographic search", CACM 18, 6 (1975) by A. Aho and M.J.
   Corasick.  The approach is closely related to the deterministic
   finite automaton that would recognize the same string list.  The
   method takes linear time in the size of the string list and text
   file.  The program requires space proportional to the size of the
   string list.

LIMITATIONS
    Lines are limited to 256 characters; longer lines are truncated.
    Keywords in keyword file are limited to 256 characters; longer lines
    are truncated.

DIAGNOSITICS
   Exit status is 0 if any matches are found, 1 if none, 2 for errors.

REVISIONS
    DATE       VERSION    NAME        CHANGE
    6/30/87     1.0       Kindel      Original: Get options, get keywords
    7/6/87      1.1       Kindel      fg_build_table, fg_get_keyword
    7/6/87      1.2       Kindel      fg_next, fg_goto
    7/7/87      1.3       Kindel      fixed fg_build_table

*/

/*==========================================================================*/
/*    fgrep.c     Charles E. Kindel   (C) KindelCo Software Systems, 1987   */
/*==========================================================================*/

#define TURBOC

/* IMPORTS */
#include <stdio.h>
#include <alloc.h>
#include <string.h>
#ifdef TURBOC
#include <process.h>
#endif

/* LOCAL SYMBOLIC DEFINITIONS */
#define TRUE        1                  /* True/False testing */
#define FALSE       0                  /* True/False testing */
#define ERROR       2                  /* exit code on error f*/
#define ZERO        0                  /* Zero! */
#define FILENUM     2                  /* argv index # 2 (file name) */
#define READ        "r"                /* for fopen */
#define MAXLINE     256                /* maximum line length */
#define END_OF_KEY  '*'                /* end of keyword flag (trie) */
#define FAIL        NULL               /* goto has failed */
#define NEWLINE     '\n'               /* newline */

/* DATA DEFINITIONS */
static char *key_file;                 /* the name of the key file */
     FILE *f_p, *fopen();              /* file pointer */
static struct trie
       {    char c;                    /* the character in the trie */
            char end;                  /* flag for end of keyword */
            struct trie* link_p;       /* the link pointer */
            struct trie* alt_p;        /* the alternate pointer */
            struct trie* fail_p;       /* the failure pointer */
       };
static struct trie *state_zero_p;
static int index;                      /* index for argv */
static char **args;                     /* global argv */

/* LOCAL DECLARATIONS */
#ifdef TURBOC
    fg_build_table (void);
    struct trie *new (void);
    char *fg_get_keyword (void);
    void fg_build_fail_states (void);
    struct trie *fg_goto (struct trie*, char);
    struct trie *fg_next (struct trie*, char);
#else
#define void
    static fg_build_table ();
    static struct trie *new ();
    static fg_get_keyword ();
    static fg_build_fail_states ();
    static trie *fg_goto ();
    static trie *fg_next ();
#endif

/* MACROS */
#define new (struct trie*) malloc (sizeof (struct trie)) /* allocate trie */

/* FUNCTIONS */

/*--------------------------------------------------------------------------*/
/*                          main program                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void main (int argc, char *argv[])
{    static char c;                    /* command option */

#ifdef TURBOC
    extern int getopt (int argc, char *argv[], char *optionS);
    extern int opterr, optind;
#endif

   args = argv;                        /* make argv global */
   index = 1;                          /* point index to the first key */
                                       /* word on the command line */
   key_file = NULL;                    /* if no keyfile this is the flag */
   while ((c = getopt (argc, argv, "f")) != EOF)
      switch (c)
      {    case 'f' : key_file = argv[FILENUM];  /* point to *argv[2] */
                      break;
           default  : fprintf (stderr, "Usage: fgrep [-f keyfile]\n");
                      exit (ERROR);
                      break;
      }

   if (key_file != NULL)          /* if theres a keyfile */
      if ((f_p = fopen (key_file, READ)) == NULL)
       {    fprintf (stderr, "fgrep: can't open %s\n", key_file);
            exit (ERROR);
       }

   fg_build_table ();    /* build table from comm.line or file*/
  /* fg_build_fail_states ();         /* find fail states in trie struct */
  */
}

/*--------------------------------------------------------------------------*/
/*                          fg_build_table  routine                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static fg_build_table (void)

{    static struct trie *current_p;
     static struct trie *new_trie_p;
     static struct trie *next_p;
     static char *keyword, c;
     static int i;                     /* keyword char index */

     state_zero_p = new;
      /* allocate a structure */
     state_zero_p->fail_p = state_zero_p;  /* init state zero */
     state_zero_p->link_p = NULL;
     state_zero_p->alt_p = NULL;
     state_zero_p->c = NULL;
     state_zero_p->end = NULL;

     while ((keyword = fg_get_keyword ()) != NULL) /* is there a keyword? */
     {    fprintf (stderr, "keyword = %s\n", keyword);
          current_p = state_zero_p;    /* point to origin */
          i = 0;                       /* keyword char index */
          while ((c = keyword [i++]) != NULL) /* while theres more chars */
          {    if (current_p->link_p == NULL) /* if link is not there */
               {    new_trie_p = new;         /* make a new trie */
                    new_trie_p->fail_p = state_zero_p;
                    new_trie_p->link_p = NULL;
                    new_trie_p->alt_p = NULL;
                    new_trie_p->end = NULL;
                    new_trie_p->c = c;
                    current_p->link_p = new_trie_p; /* point to new trie */
                    current_p = new_trie_p;
               }
               else                      /* then theres a link */
               {    next_p = current_p->link_p; /* point to linked trie */
                    while ((next_p->c != c) && (next_p->alt_p != NULL))
                        next_p = next_p->alt_p;
                        /* go down alts till char matches or alt is null */

                    if (next_p->c == c)   /* if char matches move on */
                         current_p = next_p;
                    else                 /* else build an alt */
                    {    new_trie_p = new;
                         new_trie_p->fail_p = state_zero_p;
                         new_trie_p->link_p = NULL;
                         new_trie_p->alt_p = NULL;
                         new_trie_p->c = c;
                         new_trie_p->end = NULL;
                         next_p->alt_p = current_p = new_trie_p;
                    }
               }
          }
          current_p->end = END_OF_KEY;    /* end of keyword! */
     }
     return;
}

/*--------------------------------------------------------------------------*/
/*                        fg_get_keyword routine                            */
/*--------------------------------------------------------------------------*/
static char *fg_get_keyword (void)
{   char *line;

   if (key_file != NULL)
   {    if (fgets (line, MAXLINE, f_p) == NULL) /* gets str.-incl newline */
             return (NULL);            /* if EOF return NULL */
        else
        {    *strchr (line, NEWLINE) = NULL; /* find the newline and */
                                       /* replace it with a null */
             return (line);            /* return keyword */
        }
   }
   else
   {    line = args [index++];         /* get next argument */
        return (line);                 /* return key word */
   }
}

/*--------------------------------------------------------------------------*/
/*                           fg_build_fail_states routine                   */
/*--------------------------------------------------------------------------*/
static void fg_build_fail_states ()
{
}

/*--------------------------------------------------------------------------*/
/*                        fg_goto routine                                   */
/*--------------------------------------------------------------------------*/
static struct trie *fg_goto (struct trie *state_p, char in)
{    static struct trie *current_p;

     current_p = state_p->link_p;
     if (current_p == NULL)
         return (FAIL);
     while ((current_p->c != in) && (current_p->alt_p != NULL))
         current_p = current_p->alt_p;
     if (current_p->c == in)
         return (current_p);
     else
         return (FAIL);
}

/*--------------------------------------------------------------------------*/
/*                       fg_next routine                                    */
/*--------------------------------------------------------------------------*/
static struct trie *fg_next (struct trie *state_p, char in)
{    if (fg_goto (state_p, in) == FAIL)
          state_p = fg_next (state_p->fail_p, in);
     return (fg_goto (state_p, in));
}

/*==========================================================================*/
/*    END OF fgrep.c    (C) KindelCo Software Systems, 1987                 */
/*==========================================================================*/