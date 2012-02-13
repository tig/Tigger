/* hash.c

                hash.c  (c) KindelCo Software Systems, 1988

          "hash" a program to study the efficiency of hash sorting.


Author:  Charles E. Kindel, Jr.
         S24498449

Version:	2.0

DESCRIPTION
     This program uses a chained implementation of a hash table to
     study the efficiency of hash tables under the operation of (1)
     inserting a key into the table and (2) searching for a key in the
     table.

     The abstraction, called a "dictionary", will have the following
     basic primitives.  Let D be a dictionary:

        insert (k, D)    Inserts a key k into the dictionary D if it is
                         not already there; does nothing if it is already
                         there.
        find (k, D)      Searches for the key k in dictionary D; returns true
                         if successful, otherwise returns false.  Does not
                         alter contents of D.
        makenull (D)     Initializes dictionary D with no entries.


INPUT
     The input to this program is a file consisting of a sequence of key
     strings, one per line, followed by a line containing only the
     separator *, followed by another sequence of keys strings, one per line,
     followed by a line containing only the terminator #.  Key strings
     consist of upper and lower case alphabetic characters only, with
     no white space or non-alpha characters.

     The keys strings before the * line are to be inserted into the table.
     Each of these lines is thus interpreted as a command to insert the key.
     When the line * is encountered, a representation of the table should
     be printed, showing what keys are store where in the table.

     The keys after the * line are to be searched for in the table.  Each
     of these lines is thus interpreted as a command to find the key.  Each
     such search is either successful or unsuccessful; the program accumulates
     statistices on these searches so that when the # line is encountered,
     it prints three bar graphs:


MESSAGES
     All input is guaranteed valid, there should be no reason for any
     error messages.

IMPLEMENTATION
     The hash table is implemented using a 61 element hash table of pointers
     to a singly linked list.  This method is called chaining. The hash
     function used is h(key) = (key mod 61). Thus the hash table is indexed
     from 0 to 60.

     To keep statistics on the efficiency of the hash function, there are
     two "tools".  The first consists of a "hash" and a "num" field in each
     hash table element.  The "hash" field contains the number of times
     h(key) = the index of that table element.  This data is used by graph1
     to print the first graph.  The "num" field holds the number of keys
     are chained off of that hash table entry, this is used to calculate
     the load density.

     The second "tool" is a structure called "probes"  this structure contains
     two arrays.  The first of which is called "success".  The array indicies
     of "success" indicate a number of probes (I have set the maximum number
     of probes a key can achieve to 30) that a key takes to be successfully
     searched for.  The values of each array element is the number of keys
     that took the indicies number of probes.  For example, if the 2nd
     element of "success" is equal to 12, then there are 12 keys that took
     2 probes to be successfully searched for.

     The second array is called "unsuccess".  It is identical to "success"
     except that it holds the data on unsuccessful searches.

FACILITY
     MS-DOS:  Borland International TurboC 1.5.

     VAX VMS: Digital Equipment Corp. VAX C.

REVISIONS
         DATE           VERSION     CHANGE
      25 Apr 1988         1.0       First version
      27 Apr 1988         1.1       Hashing works. No graphs.
      28 Apr 1988         1.2       Graph1 implemented, works.
      29 Apr 1988         1.3       Graph2 and Graph3.
       1 May 1988         1.4       Final Touches
		 2 May 1988		     2.0       Ported to VAX/VMS.

*/

/* include files */
#include <stdio.h>
#include <ctype.h>
#include <string.h>


/* Constants */
#define    void
#define    TRUE       1
#define    FALSE      0
#define    TABLESIZE  61       /* the number of elements in the hash table */
#define    KEYSIZE    15       /* the max length of any key                */
#define    PROBESIZE  30       /* Assume that there will be no more than   */
                               /* 30 probes per key.                       */

/* Type Declarations */
typedef int hash_val;

typedef char key_type [KEYSIZE]; /* string to hold a key */

typedef struct element_rec       /* each link on a chain is an element */
        {      key_type key;
               struct element_rec *next;
        } element_type, *element_ptr;

typedef struct hashtable_rec         /* hash table.  each element has a "hash" which is the */
        {      element_ptr next;     /* number of times h(key) = the index of that element */
               int num;            /* next is the ptr to the chain. */
               int hash;
        } hashtable_type [TABLESIZE];


/* probe_rec contains two arrays.  each element of "success" contains the number of successful */
/* search keys that took the number of probes equal to its index                               */
/* each element of "unsuccess" contains the number of unsuccessful search keys that took that  */
/* elements index probes.                                                                      */
typedef struct probe_rec
        {      int success [PROBESIZE];  /* 30 Keys is reasonable */
               int unsuccess [PROBESIZE];
        } probe_type, *probe_ptr;


/* functions */
void        insert ();
element_ptr find ();
void        makenull ();
hash_val     hash ();
void        print_table ();
void        graph1 ();
void        graph2 ();
void        graph3 ();




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Main Program                                                            */
/*                                                                         */
/*   The main block simply calls makenull to zero out all data structures, */
/*   then enters two loops.  The first loop reads all lines in the input   */
/*   before the "*" and inserts each key into the hash table using insert. */
/*   Then it prints out the hash table.  The second loop reads the rest of */
/*   the input, up to the "#" and tries to find these keys in the hash     */
/*   table. Next, all three graphs are printed.                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/
main (void)
{   hashtable_type D;          /* hash table */
    key_type key;              /* working key */
    element_ptr s;             /* dummy ptr to returned by find */
    probe_type probes;         /* probe statistics structure */

    makenull (D, &probes);

    fscanf (stdin, "%s", &key);
    while (strcmp (key, "*"))
    {  insert (key, D);
       fscanf (stdin, "%s", &key);
    }
    print_table (D);

    fscanf (stdin, "%s", &key);
    while (strcmp (key, "#"))
    {  s = find (key, D, &probes);
       fscanf (stdin, "%s", &key);
    }

    graph1 (D);
    graph2 (&probes);
    graph3 (&probes);

    printf ("End of input, end of hash.c\n");

}
/* End of main */




/*-------------------------------------------------------------------------*/
/* makenull                                                                */
/*    Zeroes out all of the data structures.                               */
/*                                                                         */
/*    PARAMETERS                                                           */
/*        D - the hash table.                                              */
/*        probes - A data structure used to compile statistics on the hash */
/*                                                                         */
/*    RETURNS                                                              */
/*        nothing.                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
void makenull (D, probes)
hashtable_type D;
probe_ptr probes;
{   int i;

    for (i=0; i < TABLESIZE; i++)  /* initialize the hash table */
    {   D[i].next = NULL;
        D[i].hash = 0;
        D[i].num = 0;
    }

    for (i=0; i < PROBESIZE; i++)   /* intialize probes */
       probes->success [i] = probes->unsuccess[i] = 0;
}
/* end of makenull */




/*-------------------------------------------------------------------------*/
/* hash                                                                    */
/*    Uses division hashing.  Sums the ascii values of all the chars in    */
/*    the key, then takes this sum mod the tablesize.                      */
/*                                                                         */
/*    PARAMETERS                                                           */
/*        k - the key to be hashed.                                        */
/*                                                                         */
/*    RETURNS                                                              */
/*        An index into the hash table between 0 and TABLESIZE-1.          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
hash_val hash (k)
key_type k;
{   long sum;
    int i;

    i = 0;
    sum = 0;
    while ((!isspace (k[i])) && (k[i] != '\0'))
    {   sum = sum + (int) k[i];
        i++;
    }

    return (sum % TABLESIZE);
}
/* end of hash */




/*-------------------------------------------------------------------------*/
/* insert                                                                  */
/*    inserts a key into the hash table.  If the key is already in the     */
/*    table this function does nothing.                                    */
/*                                                                         */
/*    PARAMETERS                                                           */
/*        k - the key to be inserted in the hash table.                    */
/*        D - the hash table.                                              */
/*                                                                         */
/*    RETURNS                                                              */
/*        nothing.                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
void insert (k, D)
key_type k;
hashtable_type D;
{   hash_val h;                      /* var to hold value returned by hash */
    element_ptr cur,
                new_element;         /* new element to be added */

    h = hash (k);

    new_element = (element_ptr) malloc (sizeof (element_type));
    new_element->next = NULL;
    strcpy (new_element->key, k);

    if (D[h].next == NULL)
      D[h].next = new_element;
    else
    {   cur = D[h].next;                  /* Go down the chain looking for a match or the end */
        while ((cur->next != NULL) && (strcmp (cur->key,k) != 0))
             cur = cur->next;

        if (strcmp (cur->key,k) != 0)     /* if the key is not found, add it to the end */
          cur->next = new_element;        /* otherwise do nothing.                      */
        else
          return;                         /* dont incrment D[h].num */
    }

    D[h].num += 1;    /* increment the number of elements in this chain */

    return;
}
/* end of insert */




/*-------------------------------------------------------------------------*/
/* find                                                                    */
/*    searches for and returns and element if found in the hash table.     */
/*                                                                         */
/*                                                                         */
/*    PARAMETERS                                                           */
/*       k - key to be searched for.                                       */
/*       D - the hash table.                                               */
/*       probes - A data structure used to keep statistics on the hash     */
/*                function.                                                */
/*                                                                         */
/*    RETURNS                                                              */
/*       a pointer to the element if search was successful, else returns   */
/*       null.                                                             */
/*                                                                         */
/*-------------------------------------------------------------------------*/
element_ptr find (k, D, probes)
key_type k;
hashtable_type D;
probe_ptr probes;
{   element_ptr cur;
    int h;
    int p = 1;                     /* number of probes for this key */

    h = hash (k);

    D[h].hash += 1;                /* increment the number of times this element */
                                   /* has been hashed to.                        */
    cur = D[h].next;

    if (cur == NULL)               /* if there is no chain then return null */
       return (NULL);
    else
    {   while ((strcmp (cur->key, k) != 0) && (cur != NULL))  /* scan down chain until we find key */
        {  p++;                                               /* or we reach the end */
           cur = cur->next;
        }

        /* if search was successful then increment the number of keys that have */
        /* p probes.                                                            */
        if (cur != NULL)
           probes->success[p] += 1;
        else
           probes->unsuccess[p] += 1;
        return (cur);
    }
}
/* end of find */




/*-------------------------------------------------------------------------*/
/* print_table                                                             */
/*    Prints the contents of the hash table, including all chains.         */
/*                                                                         */
/*    PARAMETERS                                                           */
/*        D - the hash table.                                              */
/*                                                                         */
/*    RETURNS                                                              */
/*        nothing.                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
void print_table (D)
hashtable_type D;
{    int i;
     element_ptr cur;

     printf ("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
     printf ("   Output from Charles Kindel's HASH.C, program #3 for CSc 342.\n\n");
     printf ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
     printf ("\n Hash table after all keys have been inserted:\n\n");
     for (i = 0; i <= TABLESIZE-1; i++)
     {   printf ("[%d]->", i);
         cur = D[i].next;
         while (cur != NULL)
         {   printf ("%s->", &(cur->key));
             cur = cur->next;
         }
         printf ("\n");
     }
     printf ("\n");
}
/* end of print_table */



/*-------------------------------------------------------------------------*/
/* graph1                                                                  */
/*-------------------------------------------------------------------------*/
void graph1 (D)
hashtable_type D;
{   int i,j,n,max = 0,sum = 0;

                                     /* figure out the table slot with the highest number*/
    for (i=0; i < TABLESIZE; i++)    /* of keys such that h(k)=i */
      if (max < D[i].hash)
         max = D[i].hash;

    printf ("\n==============================================================================\n");
    printf ("\n  Graph Number 1: Summarization of the hash function h(k).\n");
    printf ("       x-axis = table indicies\n");
    printf ("       y-axis = number of input keys, k, for which h(k)=i.\n\n");

    for (i=0; i < TABLESIZE; i++)
       sum += D[i].num;
    printf ("  Load Density = (%d/%d)", sum, TABLESIZE);
    printf ("\n\n");

    max += 2;                              /* put two blank spaces at top of graph */
    for (n = max; n > 0; n--)              /* print the graph. */
    {   printf ("%3.0i  |", n);
        for (i=0; i < TABLESIZE; i++)
          if (D[i].hash >= n)
             printf ("*");
          else
             printf (" ");
        printf ("\n");
    }
    printf ("      ");
    for (i=0; i < TABLESIZE; i++)
      printf ("-");
    printf ("\n      0         ");
    for (i=10; i < TABLESIZE; i+=10)
    {   printf ("%d", i);
        for (j=1; j < 9; j++)
          printf (" ");
    }
    printf ("\n\n");
    return;
}
/* end of graph1 */




/*-------------------------------------------------------------------------*/
/* graph2                                                                  */
/*-------------------------------------------------------------------------*/
void graph2 (probes)
probe_ptr probes;
{   int i,j,n,max;

    max = 0;                               /* figure out the table slot with the highest number */
    for (i=0; i < PROBESIZE; i++)          /* of keys per nubmer of probes. */
       if (max < probes->success [i])
         max = probes->success[i];


    printf ("\n------------------------------------------------------------------------------\n");
    printf ("\n  Graph Number 2: Summarization of the statistics of successful\n");
    printf ("                    searches.\n");
    printf ("       x-axis = total number of probes.\n");
    printf ("       y-axis = Count of the number of input keys that required\n");
    printf ("                that number of probes.\n\n\n");

    max += 2;                              /* put two blank spaces at top of graph */
    for (n = max; n > 0; n--)              /* print the graph. */
    {   printf ("%3.0i  |", n);
        for (i=0; i < PROBESIZE; i++)
          if (probes->success[i] >= n)
             printf ("*");
          else
             printf (" ");
        printf ("\n");
    }
    printf ("      ");
    for (i=0; i< PROBESIZE; i++)
      printf ("-");
    printf ("\n      0         ");
    for (i=10; i <= PROBESIZE; i+=10)
    {   printf ("%d", i);
        for (j=1; j < 9; j++)
          printf (" ");
    }
    printf ("\n\n");
    return;
}
/* end of graph2 */




/*-------------------------------------------------------------------------*/
/* graph3                                                                  */
/*-------------------------------------------------------------------------*/
void graph3 (probes)
probe_ptr probes;
{   int i,j,n,max;

    max = 0;                               /* figure out the table slot with the highest number */
    for (i=0; i < PROBESIZE; i++)          /* of keys per nubmer of probes. */
       if (max < probes->unsuccess [i])
         max = probes->unsuccess[i];


    printf ("\n------------------------------------------------------------------------------\n");
    printf ("\n  Graph Number 3: Summarization of the statistics of unsuccessful\n");
    printf ("                    searches.\n");
    printf ("       x-axis = total number of probes.\n");
    printf ("       y-axis = Count of the number of input keys that required\n");
    printf ("                that number of probes.\n\n\n");

    max += 2;                              /* put two blank spaces at top of graph */
    for (n = max; n > 0; n--)              /* print the graph. */
    {   printf ("%3.0i  |", n);
        for (i=0; i < PROBESIZE; i++)
          if (probes->unsuccess[i] >= n)
             printf ("*");
          else
             printf (" ");
        printf ("\n");
    }
    printf ("      ");
    for (i=0; i< PROBESIZE; i++)
      printf ("-");
    printf ("\n      0         ");
    for (i=10; i <= PROBESIZE; i+=10)
    {   printf ("%d", i);
        for (j=1; j < 9; j++)
          printf (" ");
    }
    printf ("\n\n");
    printf ("==============================================================================\n");

}
/* end of graph3 */

/* End of hash.c ==========================================================*/


