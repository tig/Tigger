 /* disp.h               (c) KindelCo Software Systems, 1987

                      DISPLAY - display routines for ez (Dp)


DIRECTORY
   void DpDel ()
n  : Delete characters from the display.

   void DpInit (void)
     Initialize the display parameters.

   void DpInsert (char c)
c  : Character to be inserted on screen.

   void DpMove (void)
     Move the cursor to a new position on the screen.

   void DpMsg (char *sp)
     Display a message on the command line.
sp : pointer to string to display.

   void DpPaint (void)
     Repaints the entire screen.

   void DpFindPos  (void)
     Find the cursor position

   void DpOutRow (Char*)
     output a row starting at char*

   char * DpFindNext (void)
     finds the pointer to the line of text immeadiately below the bottom
     of the screen.

*/

/* FUNCTION PROTOTYPES ------------------------------------------------------*/
#ifdef TURBOC
  void DpFindPos (void);
  void DpOutRow (char*);
  char *DpFindNext (void);
  void DpDel (void);
  void DpInit (void);
  void DpInsert (char);
  void DpMove (void);
  void DpMsg (char *);
  void DpPaint (void);
#else
  void DpFindPos ();
  void DpOutRow ();
  char *DpFindNext ();
  void DpDel ();
  void DpInit ();
  void DpInsert ();
  void DpMove ();
  void DpMsg ();
  void DpPaint ();
#endif


/*==========================================================================*/
/* END OF  disp.c                 (C) KindelCo Software Systems, 1987       */
/*==========================================================================*/
