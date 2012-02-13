#include <stdio.h>
#include <dir.h>            /* The stuff for fnsplit and ffblk */

/* We use a far pointer for the source so that no matter what 
   memory model you use the DOS command line can be passed to 
   expwild.  Because the command line resides outside of Turbo 
   C's data segment, it must be passed this way.  We do not want 
   the routine wild() to have to deal with far pointers, however.  
   To avoid this problem we copy any file specification we are 
   about to pass to wild() into a local variable.  */


expwild (source, dest, maxsize)    /* called from setargv to */
                                   /* handle any wildcard */
                                   /* specifications in the */ 
                                   /* command line */

char far *source;       
char *dest;
int maxsize;

{ 
  int count = 1, savecount = 1, wildcount;
  char *dwrdptr,                /* destination string pointer */
       wildspec[128],           /* 128 is max command line size */
       *wildptr;                /* used to extract wildcard spec */
  char far *swrdptr;            /* Far pointer to match source */

  swrdptr = source;             /* Initialize the source and */
  dwrdptr = dest;               /* destination word pointers */

/* Move characters from the source to the destination until
   we hit the end of the null-terminated string. */
    
  while ((*dest = *source++) != '\0') 
  {

/* If the string is too big, we're done.  Exit the loop. */

    if (++count >= maxsize) break;

/* Possibly take some action based on what the character was */

    switch (*dest++)
    {
      case '*':
      case '?':

/* We just moved a wildcard character.  Move back in the source to 
   the start of the last word and copy into wildspec (our local 
   variable buffer for the call to wild) that word.  In this way 
   wild need not deal with a far pointer.  We copy until we hit a 
   space, tab or the end of the string.  Terminate the wildcard 
   specification string with a null.  Reset count to what it was 
   at the start of the word. */

        wildptr = wildspec;
        source = swrdptr;
        while ( *source != ' ' && *source != '\t' &&
                *source != '\0' ) *wildptr++ = *source++;
        *wildptr = '\0';
        count = savecount;

/* Call wild to expand the wild spec.  Put the answer where the
   last destination word pointer was.  Update the count and the
   destination pointer. */

        wildcount = wild (wildspec, dwrdptr, maxsize - count);
        count += wildcount;
        dest = dwrdptr + wildcount;
        break;

/* dest now contains any previous arguments plus this one that
   has been expanded by wild().  We're done with this case. */

      case ' ':
      case '\t':

/* We hit a word terminator.  We update the source and 
   destination word pointers to point to the next character.  
   We also save the count as of the start of this word, in case  
   we have to roll back to this position later. */
   
        swrdptr = source;
        dwrdptr = dest;
        savecount = count;
        break;

/* Don't do anything for any other character.  Just move it from 
   the source string to the destination string.  */

      default:
        break;

    }  /* switch */

  }  /* while loop */

  return (count);                 /* Return the length of the new */
                                  /* command line */
}  /* expwild */


int wild (wildspec, expand, maxsize)   /* general purpose routine */
                                       /* that accepts an input */
                                       /* wildcard spec wildspec */
                                       /* and puts the resulting */
                                       /* full file names, if any, */
                                       /* in expand */

char *wildspec, *expand;
int maxsize;

{
  struct ffblk ffblk;           /* Special file block */
  char drive[MAXDRIVE], dir[MAXDIR], name[MAXFILE], ext[MAXEXT];
                                /* these constants are in dir.h */
                                /* and reflect DOS size limits */
  char *bufptr;
  int size = 0;

  bufptr = expand;              /* Save our original pointer */

/* Split the file name into its fundamental components. We need 
   the drive and directory information because the findfirst and 
   findnext functions return only simple file names, and we want 
   to return to our caller complete, unambiguous file pathnames. */
    
  fnsplit (wildspec, drive, dir, name, ext);

/* Find the first file that matches the input wildcard specification.
   If none match, then copy the input string to the output output 
   string and return the length of the resulting string. Note that 
   another option is to return a null if the caller passes in a 
   wildcard spec that matches no file.  We elected to let the caller 
   handle the results in this case.  Both findfirst and findnext 
   require a special file information block structure of type 
   ffblk. */
    
  if (findfirst (wildspec, &ffblk, 0) != 0)
  {
    strncpy (expand, wildspec, maxsize);
    return (strlen (expand));
  } /* if no files match this wildcard specification */
 
  while (1)  /* loop until no more files match the wildcard spec */
  {

/* Build up the complete file name from the optional drive name
   and path name of the original wild file spec and the file name
   found by either findfirst or findnext.  maxsize-size is always 
   the total space remaining for the string.  */

    strncpy (bufptr, drive, maxsize - size);
    strncat (bufptr, dir, maxsize - size);
    strncat (bufptr, ffblk.ff_name, maxsize - size);

    size += strlen (bufptr);     /* Increase the string size to */
                                 /* reflect the added file name */

/* If the string is now too long, roll back to the previous 
   file name, deposit a null and return the string size.  bufptr 
   still points to the start of this new file name, so rolling 
   back is simple.  */

    if (size >= maxsize - 1)
    {
      *(bufptr - 1) = '\0';
      return (strlen (expand));
    } /* if the string is too long */

/* Update the pointer to move past the newly added file name and 
   put a space after that file name.  Because we compared size to 
   maxsize-1 above, we know there is room for the blank. */
        
    bufptr = expand + size++;
    *bufptr++ = ' ';

/* Get the next file.  If there are no more, back up and put
   a null over the space after the last name and return the size. */
        
    if (findnext (&ffblk) != 0)
    {
      *(bufptr - 1) = '\0';
      return (size);
    }  /* if there are no more files that match this spec */

  }  /* loop through all matching file names */
}  /* wild */
