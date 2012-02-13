/* copyright (c) 1989, Charles E. Kindel, Jr. */

#include <stdio.h>

#ifdef TURBOC
int common (const char*, const char*, const char*) ;
int find (const char*, const char*) ;
#endif

int main ()
{
  int c ;
  char String1[80], String2[80], String3[80] ;

  printf ("String1=abc\nString2=abcd\nString3=xyzabcde\n common returns %d\n\n",
         common("abc", "abcd", "xyzabcde")) ;
  printf ("String1=abc\nString2=abcxyz\nString3=123abcpqrabcxyz\n common returns %d\n\n",
         common("abc", "abcxyz", "123abcpqrabcxyz")) ;
  printf ("String1=abcxyz\nString2=abc\nString3=123abcpqrabcxyz123\n common returns %d\n\n",
         common("abcxyz", "abc", "123abcpqrabcxyz123")) ;
  printf ("String1=\nString2=\nString3=\n common returns %d\n\n", common ("","","")) ;
  printf ("String1=\nString2=\nString3=abc\n common returns %d\n\n",
         common ("","","abc")) ;
  printf ("String1=abc\nString2=abcd\nString3=xyz123cde\n common returns %d\n\n",
         common("abc", "abcd", "xyzabcde")) ;

/* The following is the interactive code.  It is commented out. */
/*
  while (gets (String1))
  {
     gets(String2) ;
     gets(String3) ;
     c = common (String1, String2, String3) ;
     if (c != 0)
       printf ("%d\n", c) ;
  }
*/
  return (0) ;
}


int common(psString1, psString2, psString3)
const char *psString1, *psString2, *psString3 ;
{
  int f1, f2 ;

  f1 = find(psString1, psString3) ;
  f2 = find(psString2, psString3) ;
  /* keep scanning strings until we find a match or find that one does */
  /* not match at all.                                                 */
  while (f1 != f2 && f1 != 0 && f2 != 0)
  {
    if (f1 > f2)
      f2 = f2 + find (psString2, psString3 + f2) ;       /* We do some pointer math here */
    else
      f1 = f1 + find (psString1, psString3 + f1) ;
  }
  if (f1==f2)
    return (f1) ;
  else
    return (0) ;
}

/*   int find(*char, *char)

     This function returns the location of the first occurrence
     of string psString1 in string psString2.  Returns 0 if find fails.
*/
int find(psString1,psString2)
const char *psString1, *psString2 ;
{
   int len1, len2, i ;
   char *temp ;

   temp = "\0" ;

   if (strlen(psString1) > strlen(psString2))
     return(0) ;
   len1 = strlen(psString1) ;
   if (len1 == 0)
     return (1) ;
   len2 = strlen(psString2) ;
   for (i=0 ; i < len2 ; i++)
   {
     strncpy (temp, psString2+i, len1) ;
     temp[len1] = '\0' ;
     if (strcmp (psString1, temp) == 0)
       return(i+1) ;
   }
   return (0) ;
}
