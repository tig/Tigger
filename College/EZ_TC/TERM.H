/* term.h    T. L. Williams

	TERM - Terminal definitions  (Tm)

	Terminal attributes, window sizes

DIRECTORY
void TmAttr(attr)   Set terminal attributes. Returns nothing.
	int attr        New attribute (see below)

void TmClr()        Clear screen and home cursor. Returns nothing.

void TmDeol()       Delete from cursor to end of the line.

void TmDelrow(n)	Delete n rows starting from cursor row.
	 int n          Number of rows to be deleted.

int  TmGetcur()     Returns the cursor. Returns col in upper 8 bits.

void TmInit()       Initialize terminal mode and window.

void TmInsrow(n)    Insert n blank rows starting from cursor row.
	 int n          Number of rows inserted.

void TmPuts(sp)     Send string to terminal. Update the cursor.

void TmRst()        Restore terminal modes.

void TmSetcur(x, y) Set cursor position.
	 int x, y       Cursor position (0,0) is upper left corner.

void TmWndw(t, b)   Establish a scrolling region.
     int t, b       Top, bottom row of window (numbered from 0).
*/
#define TMROWS   24
#define TMCOLS   80

/* terminal attributes */
#define TMNORM  0			/* normal video */
#define TMINVR  1			/* inverse video */
#define TMBOLD  2			/* bold */
#define TMUNDL  4			/* underline */
#define TMBLNK  8			/* blink */

extern int TmWndwsz;		/* rows in window */

void TmAttr (int);
void TmClr (void);
void TmDeol (void);
void TmDelrow (int);
int  TmGetcur (void);
void TmInit (void);
void TmInsrow (int);
void TmPuts (char *);
void TmRst (void);
void TmSetcur (int, int);
void TmWndw (int, int);
