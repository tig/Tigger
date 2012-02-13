 /*   file.c

                   file.c    (c) KindelCo Software Systems, 1987

                           FILE - file routines for EZ (Fl)


Author:   Charles E. Kindel, Jr. (tigger)

Started:  July 22, 1987
Version:  1.00

DESCRIPTION
    These routines read and write the buffer for EZ.  They use the open, read
    and write routines.


/* IMPORTS -----------------------------------------------------------------*/

#include <errno.h>

#ifdef TURBOC
#include <io.h>
#include <fcntl.h>

#else
#include <sys/types.h>
#include <sys/file.h>
#include <sys/uio.h>
#endif

#include "ez.h"
#include "file.h"
#include "buf.h"

extern int errno;

/* LOCAL SYMBOLIC DEFINITIONS ----------------------------------------------*/
#define READERR    1
#define NEWLINE    '\n'
#define PMODE      0644    /* RW for owner, R for group, */

/* TYPEDEFS ----------------------------------------------------------------*/

/* FUNCTIONS ---------------------------------------------------------------*/
/*==========================================================================*/
/*                            FlRead                                        */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Reads the entire buffer into the text buffer.                       */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      char *namep : pointer to the file name to be read.                  */
/*      char *tp    : pointer to the top of the buffer.                     */
/*      unsigned sz : the maximum size of the buffer.                       */
/*                                                                          */
/*  RETURNS                                                                 */
/*      the number of characters actually read.                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
unsigned FlRead (namep, tp, sz)
char *namep, *tp;
long sz;
{     int fd;
      long  n_read = 0;

      if ((fd = open (namep, O_RDONLY)) == -1)
      {   if (errno == ENOENT)
          {    *(tp) = NEWLINE;
               return (1);
          }
          else
          {    printf ("ez : Read fails!\n");
               exit (READERR);
          }
      }
      else
      {    n_read = read (fd, tp, sz);
           close (fd);
      }
      *(tp + n_read) = NEWLINE;
      return (n_read+1);
}


/*==========================================================================*/
/*                            FlWrite                                       */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*      Writes the entire buffer to a file.                                 */
/*                                                                          */
/*  ARGUMENTS                                                               */
/*      char *namep : pointer to the file name to write.                    */
/*      char *tp    : pointer to the top of the buffer.                     */
/*      unsigned sz : the number of characters to write.                    */
/*                                                                          */
/*  RETURNS                                                                 */
/*      the number of characters actually written.                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
unsigned FlWrite (namep, tp, sz)
char *namep, *tp;
long sz;
{    int fd;
     long n_written = 0;

     if ((fd = open (namep, O_WRONLY | O_TRUNC | O_CREAT)) == -1)
         return (n_written);
     else
     {   n_written = write (fd, tp, sz-1);
         close (fd);
         return (n_written);
     }
}

/*==========================================================================*/
/*    END OF file.c                     (C) KindelCo Software Systems, 1987 */
/*==========================================================================*/
