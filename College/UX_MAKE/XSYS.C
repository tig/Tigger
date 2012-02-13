
#include "include/xsys.h"
#include "include/make.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

/*  ------------------------------------------------------------------
    XSetTime - Set the modification time of a file.  Sets the modification
               time to the current time using the system routine utime.
               If the file does not exist, try to create it.
    ------------------------------------------------------------------
    RETURNS: 0 or -1 if the modification fails.
*/
int XSetTime(fp)
char *fp;
{
    long timebuf[2];

    timebuf[0] = timebuf[1] = time(0);

    if (utime(fp, timebuf) < 0)
        if (fopen(fp, "a") == (FILE *)0)
             return(-1);
    return(0);

} /* XSetTime */


long xmodtime (fp)
char *fp;
{    struct stat fstat;
     if (stat (fp, &fstat) >= 0)
        return (fstat.st_mtime);
     else
        return (0);
}


void xexec (cp)
char *cp;
{     short Xerror = 0;


      if ((cp != (char*) 0) && (*cp != NULL) && ((Xerror = system (cp)) < 0))
      {   printf ("Can't execute [%s]/n", cp);
          exit (0);
      }
      else
         if (Xerror > 0)
         {    fprintf (stderr, "***STOP!\n");
              exit (Xerror);
         }
      return;

}

