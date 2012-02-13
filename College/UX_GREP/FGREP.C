/*

              fgrep.c    (c) KindelCo Software Systems, 1987

Author:   Charles E. Kindel, Jr. (tigger)

Started: June 30, 1987
Version: 2.0

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

MESSAGES
   On an invalid command line the above synopsis is printed to stderr.
   If fgrep cannot open cannot open the key file specified with the -f
   option it prints this message to stderr:
      fgrep: Cannot open <keyfile>

DIAGNOSITICS
   Exit status is 0 if any matches are found, 1 if none, 2 for errors.

IMPLEMENTATION
   The string list is compiled into a trie structure augmented with
   failure links as described in "Efficient String Matching: an Aid
   to Bibliographic search", CACM 18, 6 (1975) by A. Aho and M.J.
   Corasick.  The approach is closely related to the deterministic
   finite automaton that would recognize the same string list.  The
   method takes linear time in the size of the string list and text
   file.  The program requires space proportional to the size of the
   string list.

FACILITY
   Coded on Unix 4.3 bsd.  Version also available for the IBM PC/XT
   using Borland International TurboC under MS-DOS.

LIMITATIONS
    Lines are limited to 256 characters; longer lines are truncated.
    Keywords in keyword file are limited to 256 characters; longer lines
    are truncated.
    A blank line in the keyword file will result in all input lines
    matching.


REVISIONS
    DATE       VERSION    NAME        CHANGE
    6/30/87     1.0       Kindel      Original: Get options, get keywords
    7/6/87      1.1       Kindel      fg_build_table, fg_get_keyword
    7/6/87      1.2       Kindel      fg_next, fg_goto
    7/7/87      1.3       Kindel      fixed fg_build_table
    7/8/87      1.4       Kindel      que routines.
    7/10/87     1.5       Kindel      fg_build_fails, fg_search
    7/12/87     2.0       Kindel      Final version.

*/
/**/
/*==========================================================================*/
/*    fgrep.c     Charles E. Kindel   (C) KindelCo Software Systems, 1987   */
/*==========================================================================*/


/* IMPORTS -----------------------------------------------------------------*/
#include <stdio.h>                     /* fprintf, fgets, etc...            */
#include <string.h>

/* LOCAL SYMBOLIC DEFINITIONS ----------------------------------------------*/
#define TRUE        1                  /* True/False testing                */
#define FALSE       0                  /* True/False testing                */
#define ERROR       2                  /* exit code on error                */
#define ZERO        0                  /* Zero!                             */
#define FILENUM     2                  /* argv index # 2 (file name)        */
#define READ        "r"                /* for fopen                         */
#define MAXLINE     256                /* maximum line length               */
#define END_OF_KEY  '*'                /* end of keyword flag (trie)        */
#define FAIL        NULL               /* goto has failed                   */
#define NEWLINE     '\n'               /* newline character                 */

/* MACROS ------------------------------------------------------------------*/
#define new (trie_type*) malloc (sizeof (trie_type)) /* allocate trie   */
#define empty_q frontq_p = NULL        /* empties the queue.                */

/* TYPEDEFS ----------------------------------------------------------------*/
typedef static trie
       {    char c;                    /* the character in the trie */
            char end;                  /* flag for end of keyword */
            trie_type* link_p;         /* the link pointer */
            trie_type* alt_p;          /* the alternate pointer */
            trie_type* fail_p;         /* the failure pointer */
       } trie_type;
static struct que                      /* structure for the queue */
       {      que_type *nextq_p;
              trie_type *trie_p;
       } que_type;

/* DATA DEFINITIONS --------------------------------------------------------*/
static trie_type *state_zero_p;
static char *key_file;                 /* the name of the key file          */
FILE *f_p, *fopen();                   /* file pointer                      */
static que_type *frontq_p;             /* pointer to the front of the q     */
static int idex;                       /* idex for argv                     */
static char **args;                    /* global argv                       */
static int match;                      /* were there any matches?           */

/* FUNCTION PROTOTYPES -----------------------------------------------------*/
#define void                           /* define void as nothing            */
    static fg_build_table (void);
    static char *fg_get_keyword (void);
    static fg_build_fail_states (void);        /* descriptions for functions*/
    static enque (void);                       /* are below with the        */
    static trie_type *deque (void);            /* routines themselves.      */
    static trie_type *fg_goto (void);
    static trie_type *fg_next (void);
    static fg_search (void);




/* FUNCTIONS ---------------------------------------------------------------*/

/*==========================================================================*/
/*                          main program                                    */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*       Gets command options (-f), exits if bad command line, trys to open */
/*       keyfile if -f is specified.  Exits if it fails.  Calls a routine   */
/*       to build the trie table.  Calls a routine to connect the fail      */
/*       states.  Calls a routine to get the input and search it for        */
/*       matches.                                                           */
/*                                                                          */
/*  INPUTS                                                                  */
/*       argv (pointer to the arguments) and argc (number of arguments on   */
/*       command line..                                                     */
/*                                                                          */
/*  CALLS                                                                   */
/*       getopt: get the command line options and arguments.                */
/*       fg_build_table: builds the trie structure.                         */
/*       fg_build_fail_states: figures out the fail states.                 */
/*       fg_search: reads the input file and finds matches.                 */
/*                                                                          */
/*  RETURN VALUE                                                            */
/*       Returns a 0 on matches found, 1 on no matches, and 2 on error      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static int main (argc, argv)
int argc;
char *argv[];
{  char c;                             /* command option                    */

   args = argv;                        /* make argv global                  */
   idex = 1;                           /* point idex to the first key       */
   key_file = NULL;                    /* if no keyfile this is the flag    */

   /* get all command line options                                          */
   while ((c = getopt (argc, argv, "f")) != EOF)
      switch (c)
      {    case 'f' : key_file = argv[FILENUM];  /* point to *argv[2]       */
                      break;
           default  : fprintf (stderr, "Usage: fgrep [-f keyfile]\n");
                      exit (ERROR);
                      break;
      }

   if (key_file != NULL)               /* if theres a keyfile              */
      if ((f_p = fopen (key_file, READ)) == NULL) /* trie and open file    */
       {    fprintf (stderr, "fgrep: can't open %s\n", key_file);
            exit (ERROR);
       }

   fg_build_table ();          /* build table from comm.line or file       */
   fg_build_fail_states ();    /* find fail states in trie struct          */
   fg_search ();               /* read input and find matches              */
   return (match);             /* return 0:matches found, 1:no matchesfound*/
                               /* 2:errors                                 */
}




/*==========================================================================*/
/*                          fg_build_table  routine                         */
/*                                                                          */
/* DESCRIPTION                                                              */
/*      Builds the trie structure using the keyfile of the command line.    */
/*                                                                          */
/* ARGUMENTS                                                                */
/*      There are no arguments for this function.                           */
/*                                                                          */
/* CALLS     fg_get_keyword: gets a keyword from keyfile or command line    */
/*                                                                          */
/* RETURN VALUE                                                             */
/*      It does not return a value                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static fg_build_table (void)
{    trie_type *current_p;             /* current trie                      */
     trie_type *new_trie_p;            /* a new trie                        */
     trie_type *next_p;                /* the next trie                     */
     char *keyword, c;
     int i;                            /* keyword char idex                 */

     state_zero_p = new;               /* malloc a zero state               */
     state_zero_p->fail_p = state_zero_p;  /* init state zero               */
     state_zero_p->link_p = NULL;
     state_zero_p->alt_p = NULL;
     state_zero_p->c = NULL;
     state_zero_p->end = NULL;

     while ((keyword = fg_get_keyword ()) != NULL) /* is there a keyword?   */
     {    current_p = state_zero_p;    /* point to origin                   */
          i = 0;                       /* keyword char idex                 */
          while ((c = keyword [i++]) != NULL) /* while theres more chars    */
          {    if (current_p->link_p == NULL) /* if link is not there       */
               {    new_trie_p = new;         /* make a new trie            */
                    new_trie_p->fail_p = state_zero_p;
                    new_trie_p->link_p = NULL;
                    new_trie_p->alt_p = NULL;
                    new_trie_p->end = NULL;
                    new_trie_p->c = c;
                    current_p->link_p = new_trie_p; /* point to new trie   */
                    current_p = new_trie_p;
               }
               else                      /* then theres a link             */
               {    next_p = current_p->link_p; /* point to linked trie    */
                    while ((next_p->c != c) && (next_p->alt_p != NULL))
                        next_p = next_p->alt_p;
                        /* go down alts till char matches or alt is null   */


                    if (next_p->c == c)   /* if char matches move on       */
                         current_p = next_p;
                    else                 /* else build an alt              */
                    {    new_trie_p = new;
                         new_trie_p->fail_p = state_zero_p;
                         new_trie_p->link_p = NULL;
                         new_trie_p->alt_p = NULL;
                         new_trie_p->c = c;
                         new_trie_p->end = NULL;
                         next_p->alt_p = new_trie_p;
                         current_p = new_trie_p;
                    }
               }
          }
          current_p->end = END_OF_KEY;    /* end of keyword!               */
     }
     return;
}



/*==========================================================================*/
/*                        fg_get_keyword routine                            */
/*                                                                          */
/* DESCRIPTION                                                              */
/*      Gets a keyword from the keyword file or from the command line.      */
/*                                                                          */
/* ARGUMENTS                                                                */
/*      There are no arguments for this function.                           */
/*                                                                          */
/* RETURNS                                                                  */
/*      pointer to a keyword.                                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static char *fg_get_keyword (void)
{  char *line;                         /* string pulled from file or command*/
                                       /* line.                             */

   line = (char*) malloc (MAXLINE);    /* allocate the string               */

   if (key_file != NULL)
   {    if (fgets (line, MAXLINE, f_p) == NULL) /* gets str.-incl newline   */
             return (NULL);            /* if EOF return NULL                */
        else
        {    *strchr (line, NEWLINE) = NULL; /* find the newline and        */
                                       /* replace it with a null            */
             return (line);            /* return keyword                    */
        }
   }
   else
   {    line = args [idex++];         /* get next argument                  */
        return (line);                 /* return key word                   */
   }
}



/*==========================================================================*/
/*                           fg_build_fail_states routine                   */
/*                                                                          */
/* DESCRIPTION                                                              */
/*      Builds the fail states for the trie.                                */
/*                                                                          */
/* ARGUMENTS                                                                */
/*      There are no arguments for this function.                           */
/*                                                                          */
/* CALLS                                                                    */
/*      fg_next: find the next state.                                       */
/*                                                                          */
/* RETURN VALUE                                                             */
/*      It does not return a value                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static void fg_build_fail_states ()
{     trie_type *s_p,                  /* current trie                      */
                  *temp_p;             /* temporary pointer to a trie       */
      char a;

      empty_q;                         /* empty the queue                   */
      if ((s_p = state_zero_p->link_p) != NULL) /* if there are more tries  */
      {    while (s_p->alt_p != NULL)  /* go down first level an enque them */
           {    enque (s_p);
                s_p = s_p->alt_p;
           }
           enque (s_p);

           while ((s_p = deque ()) != NULL)
           {    temp_p = s_p;
                if (temp_p->link_p != NULL)
                {    temp_p = temp_p->link_p;
                     enque (temp_p);
                     temp_p->fail_p = fg_next (s_p->fail_p, temp_p->c);
                     while ((temp_p->alt_p) != NULL)
                     {    temp_p = temp_p->alt_p;
                          enque (temp_p);
                          temp_p->fail_p = fg_next (s_p->fail_p, temp_p->c);
                          if (temp_p->fail_p->c == END_OF_KEY)
                              temp_p->c = END_OF_KEY;
                     }
                }
           }
     }
     return;
}



/*==========================================================================*/
/*                        fg_goto routine                                   */
/*                                                                          */
/* DESCRIPTION                                                              */
/*      Finds the next state in the trie. Does not use the fail functions.  */
/*                                                                          */
/* ARGUEMENTS                                                               */
/*      state_p: the current state (pointer to a trie.)                     */
/*      in: a character (to look for).                                      */
/*                                                                          */
/* RETURNS                                                                  */
/*     Returns the next state given an input.                               */
/*     it returns FAIL if it can't find the next state.                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static trie_type *fg_goto (state_p, in)
trie_type *state_p;
char in;
{    trie_type *temp_p;                /* a temporary pointer               */

     if (state_p->link_p == NULL)      /* no where to look                  */
         return (FAIL);
     else
     {   if (state_p->link_p->c == in)
              return (state_p->link_p);
         else
         {    temp_p = state_p->link_p;
              while (temp_p->alt_p != NULL)
              {    if (temp_p->alt_p->c == in)
                       return (temp_p->alt_p);
                   else
                       temp_p = temp_p->alt_p;
              }
              if (temp_p->c == in)
                   return (temp_p);
              else
                   return (FAIL);
          }
     }
}



/*==========================================================================*/
/*                       fg_next routine (recursive)                        */
/*                                                                          */
/* DESCRIPTION                                                              */
/*      Finds the next state in the trie. Uses the fail functions.          */
/*                                                                          */
/* ARGUEMENTS                                                               */
/*      state_p: the current state (pointer to a trie.)                     */
/*      in: a character (to look for).                                      */
/*                                                                          */
/* CALLS                                                                    */
/*      fg_goto.                                                            */
/*                                                                          */
/* RETURNS                                                                  */
/*     Returns the next state given an input.                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static trie_type *fg_next (state_p, in)
trie_type *state_p;
char in;
{    trie_type *temp_p;
     if ((temp_p = fg_goto (state_p, in)) == FAIL)
     {    if (state_p != state_zero_p)
             state_p = fg_next (state_p->fail_p, in);
     }
     if (temp_p == NULL)        /* if fg_goto =FAIL then return statezero  */
         return (state_zero_p);
     else
         return (temp_p);
}




/*==========================================================================*/
/*                         fg_search routine                                */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*       Does the DFSA search on the input to see if an input line contains */
/*       any keywords.  If a keyword is found on a line then that line is   */
/*       printed, and we go on to the next line.                            */
/*                                                                          */
/* ARGUMENTS                                                                */
/*      There are no arguments for this function.                           */
/*                                                                          */
/* USES  fg_next.                                                           */
/*                                                                          */
/* RETURN VALUE                                                             */
/*      It does not return a value                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static void fg_search ()
{    trie_type *current_p;
     char line [MAXLINE];
     char c;
     int i;

     match = TRUE;                     /* match = 1 if no matches are found */
     current_p = state_zero_p;
     while (fgets (line, MAXLINE, stdin) != NULL)  /* get a line            */
     {    i = 0;
          while ((c = line [i++]) != NULL)         /* go through line       */
          {    current_p = fg_next (current_p, c); /* checking the states   */
               if (current_p->end == END_OF_KEY)
               {     match = TRUE; /* if matches are found the return a 0   */
                     fprintf (stdout, "%s", &line[0]);
                     break;
               }
          }
     }
}




/*==========================================================================*/
/*                            queue routines                                */
/*                                                                          */
/* DESCRIPTION                                                              */
/*   These routines manage a simple queue of pointers to tries.             */
/*   Each element in the que is a struct with a next and a trie pointer     */
/*   The queue is pointed to by only one pointer, frontq_p.                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static void enque (new_p)
trie_type *new_p;
{    que_type *endq_p, *tempq_p;
     if (frontq_p == NULL)
     {   endq_p = (que_type*) malloc (sizeof (que_type));
         frontq_p = endq_p;
     }
     else
     {    endq_p = (que_type*) malloc (sizeof (que_type));
          tempq_p = frontq_p;
          while ((tempq_p->nextq_p) != NULL)
              tempq_p = tempq_p->nextq_p;
          tempq_p->nextq_p = endq_p;
     }
     endq_p->trie_p = new_p;
     endq_p->nextq_p = NULL;
     return;
}

static trie_type *deque (void)
{    que_type *dequed_p;
     trie_type *temp_p;
     if (frontq_p == NULL)
          return (NULL);
     else
     {     temp_p = frontq_p->trie_p;
           dequed_p = frontq_p;
           frontq_p = frontq_p->nextq_p;
           free (dequed_p);
           return (temp_p);
     }
}
/*==========================================================================*/
/*    END OF fgrep.c    (C) KindelCo Software Systems, 1987                 */
/*==========================================================================*/



