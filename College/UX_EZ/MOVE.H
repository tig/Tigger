/* move.h               (c) KindelCo Software Systems, 1987

                   MOVE - Move text in the buffer definitions (Mv)

DIRECTORY
void MvUp (sp, dp, n)    Move n chars in text from sp to dp where dp < sp.
     char *sp            Ptr to source char in text
     char *dp            Ptr to destination char in text
     unsigned n          Number of chars to move

void MvDown (sp, dp, n)  Move n chars in the text from sp to dp where sp < dp.
     char *sp            Ptr to source char in text
     char *dp            Ptr to destination char in text
     unsigned n          Number of chars to move
*/



#ifdef TURBOC
   void MvUp (char *, char *, unsigned);
   void MvDown (char *, char *, unsigned);
#else
   void MvUp ();
   void MvUp ();
#endif


/*==========================================================================*/
/* END OF  move.h                 (C) KindelCo Software Systems, 1987       */
/*==========================================================================*/
