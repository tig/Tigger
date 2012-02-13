 /* file.h               (c) KindelCo Software Systems, 1987

                       File - file access routines (Fl)


Author:  Charles E. Kindel

Started: July 22, 1987
Version: 1.00

DIRECTORY

unsigned FlRead (char *fp, char *tp, unsigned sz)

    Reads a file into the text buffer.
fp  : Pointer to the file name.
tp  : Pointer to the text buffer.
sz  : size of the buffer.
    Returns characters read.


unsigned FlWrite (char *fp, char *tp, unsigned sz)

    Writes text buffer to a file.
fp  : pointer to a file name.
tp  : pointer to the text buffer.
sz  : Number of characters to be written.
    Retunrs characters actually written.

*/



#ifdef TURBOC
     unsigned FlRead (char *, char *, long);
     unsigned FlWrite (char *, char *, long);
#else
     unsigned FlRead ();
     unsigned FlWrite ();
#endif


/*==========================================================================*/
/* END OF  file.c                 (C) KindelCo Software Systems, 1987       */
/*==========================================================================*/
