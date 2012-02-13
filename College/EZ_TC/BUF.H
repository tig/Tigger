/* buf.h               (c) KindelCo Software Systems, 1987

                        BUFFER - Buffer definitions (Bf)

Author:   Charles E. Kindel, Jr. (tigger)

Started:  July 22, 1987
Version:  1.00


DIRECTORY
void BfBeep (n)      Beep and send message to command line
     int n            Number of message.

long BfDel (n)   Delete n chars from buffer.  Returns num actually deleted.
    long n            Number of characters to delete.

int BfInit (fnp)     Set up a text buffer and ptrs.  Returns -1 if fail.
    char *fnp             Pointer to a file name.

int BfInsert (c)      Insert char into string.  Returns 0 if c not inserted.
    char c                character to be inserted

long BfLine (tp, n)  Calc distance in text to move n lines.  Returns distance.
    char *tp              Pointer to starting point in text.
    int n, m              Number of lines to move.

long BfMove (n) Move cursor n characters in text. Returns distance moved.
    long n            distance to move.

long BfWord (n)       Returns distance from cursor to nth word.
    long n                number of words to move.

*/

/* the maximum size of the buffer... */

#define BUFSIZE   60000


#define BFTOP     1      /* code for top of buffer message */
#define BFBOT     2      /* code for bottom of buffer message */
#define BFMOD     3      /* code for modified message */
#define BFUSG     4      /* code for "ez: usage" message */
#define BFNEW     5      /* code for new file message */
#define BFRFL     6      /* code for read fails message */
#define BFWFL     7      /* code for write fails message */
#define BFFUL     8      /* code for buffer full */

#define TOPMSG    "At TOP of buffer!"
#define BOTMSG    "At END of buffer!"
#define MODMSG    "Buffer modified.  LOSE CHANGES? (y or n)"
#define USGMSG    "Usage: ez filename"
#define NEWMSG    "NEW FILE!"
#define RFLMSG    "Read fails!"
#define WFLMSG    "Write fails!"
#define FULMSG    "Buffer full! (On read excess is truncated!)"
#define ERRMSG    "Unknown error!"


/* DATA DECLARATIONS */
extern char *Bfbp;               /* first char in text */
extern char *Bfsp;               /* char at top of screen */
extern char *Bfcur;              /* char at cursor */
extern char *Bfep;               /* last char in text */
extern char *Bflp;               /* last char in the entire buffer */
extern int Bfmsgflg;
extern char *BfFileNamep;
extern int BfNukedLines;
extern int Bfmodflg;

/* FUNCTION PROTOTYPES -------------------------------------------------------*/
#ifdef TURBOC
    void      BfBeep (int);
    void      BfDel (long);
    void       BfInit (char *);
    int       BfInsert (char);
    long      BfLine (char *, long);
    long      BfMove (long);
    long      BfWord (long);
    long      BfLineCount (void);
#else
    void      BfBeep ();
    void      BfDel ();
    void      BfInit ();
    int       BfInsert ();
    long      BfLine ();
    long      BfMove ();
    long      BfLineCount ();
    long      BfWordCount ();
#endif


/*==========================================================================*/
/* END OF  buf.h                  (C) KindelCo Software Systems, 1987       */
/*==========================================================================*/
