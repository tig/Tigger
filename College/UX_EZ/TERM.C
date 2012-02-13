/* term.c

	  TERM  Terminal i/o routines for ANSI terminals (Tm)

Name:    T. L. Williams
Version: 1.02
Date:    9-Jul-87

PURPOSE
	The cursor control and delete routines for ANSI standard
	terminals are defined here.
	All of the terminal dependent definitions are found in term.h
	These 2 files contains ALL of the terminal dependent info.

	These are the ONLY routines that need to be changed to port
	the editor to terminals that do not have the ANSI commands
	needed in this version of term.

DEPENDENCIES
	Needs an ANSI terminal, System serial i/o routines

LIMITATIONS
	Does not support weird terminals.

NOTES
	The system dependent portion of ez is found in Xsys.c and Xsys.h.

TERMINALS

Input from the terminal keyboard:
	The terminal must be capable of sending the full ascii set,
	including control characters and possibly some escape sequences
	for special keys.

Output to the terminal:
	Only printing characters and some special control sequences are
	sent to the terminal (also a ^G for a bell).
	In particular, the terminal will never see
	linefeed, carriage return, tabs, etc. This is because some
	terminals do strange and crazy things on these characters (like
	scroll or wraparound). It would be best if the terminal never
	scrolls the display when it receives a character (even in the
	corners).

Cursor control:
	A window is defined as a set of lines on the screen that are
	used to display the text. Any insert, delete or move operations
	within this window should not effect the remainder of the display.
	The terminal must also be able to set the cursor to any position
	within a window (with no scrolling). The position is to be
	specified relative to the upper left corner of the window (0, 0)

Edit features:
	The terminal must be able to delete from the cursor to the end
	of the line. It also must be able to insert empty lines (moving
	the rest of the lines within the window down), and delete lines
	(moving the lines within the window up).
	The display outside of this window is unchanged.

Screen attributes:
	The terminal must be able to display characters in inverse video
	which is used for displaying special characters.
	No other capabilities are needed.

	The following utilities assume that the terminal conforms to
	the ANSI 3.4 standard for the commands noted.

REVISIONS
	DATE	   VERSION  NAME          CHANGE
	13-Feb-85  1.00     T.L. Williams Original
	21-Jul-86  1.01		tlw			  Add wrap off. Fix window in tmrst.
	 9-Jul-87  1.02     tlw           Add TmPuts. Fix cursor reset bug.
 */

/* IMPORTS */
#ifdef MSDOS
#include <stdio.h>			/* sprintf */
#include <string.h>			/* strcat, strcpy */
#endif
#include "xsys.h" 			/* serial i/o fcns */
#include "term.h"			/* terminal definitions */

/* DEFINITIONS */
#define TMOUT  400			/* Place to hold a row of output */

/* DATA */
int TmWndwsz;		/* Rows in window (typically 23 or 10) */

static int TmRow, TmCol;	/* Cursor position */
static int TmBot;			/* Bottom row of window */
static char TmOut[TMOUT];

/* FUNCTIONS */
/*	--------------------------------------------------------
	TmAttr - Set terminal attributes
	--------------------------------------------------------
	TMNORM and TMINVR set attributes. Others add a
	characteristic to existing attribute.
	RETURNS: nothing
 */
void TmAttr(attr)
	int attr;				/* Attribute code (see term.h) */
{
	switch (attr)
	{
	case TMNORM:			/* ANSI:  ESC [ 0 m  */
		Xputs("\033[0m");
		break;
	case TMINVR:			/* ANSI:  ESC [ 7 m  */
		Xputs("\033[7m");
		break;
	case TMBOLD:
		/* not done */
		break;
	case TMUNDL:
		/* not done */
		break;
	case TMBLNK:		/* blinking */
		/* not done */
	default:
		break;
	}
}

/*	--------------------------------------------------------
	TmClr - Clear the screen and home the cursor
	--------------------------------------------------------
	ANSI:	 ESC [ 2 J
	RETURNS: nothing
 */
void TmClr()
{
	Xputs("\033[2J");
	TmRow = TmCol = 0;
}

/*	--------------------------------------------------------
	TmDeol - Delete from the cursor to the end of the line.
	--------------------------------------------------------
	Cursor does not move.
	ANSI: ESC [ K
	RETURNS: nothing
 */
void TmDeol()
{
	Xputs("\033[K");
}

/*	--------------------------------------------------------
	TmDelrow  - Delete rows and move text up
	--------------------------------------------------------
	Move the display up by deleting the line that the cursor
	is on and n-1 subsequent lines. Move the rest of the
	display up leaving n blank lines at the bottom.
	The cursor is moved to the beg. of the line that it is on.

	ANSI:        ESC [ nn M
		   		 where nn is 1 or 2 decimal chars.
	LIMITATIONS: Cursor must be in current window.
	RETURNS:     nothing
*/
void TmDelrow(n)
int n;					/* Num of rows to be deleted */
{
	if (n > TmBot - TmRow + 1)
		n = TmBot - TmRow + 1;

	sprintf(TmOut, "\033[%dM", n);
	Xputs(TmOut);
	TmCol = 0;
}
/*	--------------------------------------------------------
	TmGetcur   Returns the current cursor position
	--------------------------------------------------------
	The cursor position is returned with the column in the
	lower 8 bits and the row in the upper 8 bits.
 */
int TmGetcur()
{
	return ((unsigned)TmRow << 8 | TmCol);
}

/*	--------------------------------------------------------
	TmInit - Configure terminal for editing.
	--------------------------------------------------------
	Set up for ANSI terminal, clear the decks for action.
	ANSI: auto wrap off  ESC [ ? 7 l (ell)
	RETURNS: nothing
 */
void TmInit()
{
	Xputs("\033[?7l");
	TmClr();
	TmWndw(0, TMROWS-2);		/* leave one row for commands */
}

/*	--------------------------------------------------------
	TmInsrow - Insert blank lines and move text down
	--------------------------------------------------------
	Move the display down by inserting n blank lines starting
	at the same line as the cursor is on. Move the line of
	text that contained the cursor and all the subsequent
	lines down n lines.
	The cursor is moved to the beginning of the line it was on.
	ANSI:        ESC [ nn L
	LIMITATIONS: Cursor must be in current window
	RETURNS:     nothing
 */
void TmInsrow(n)
int n;					/* number of rows inserted */
{
	if (n > TmBot - TmRow + 1)
		n = TmBot - TmRow + 1;
	sprintf(TmOut, "\033[%dL", n);
	Xputs(TmOut);
	TmCol = 0;
}
/*	--------------------------------------------------------
	TmPuts - Send string to the terminal, move cursor
	--------------------------------------------------------
	Control characters are shown in inverse video. Tabs are
	expanded into spaces and newlines are not sent to the
	terminal (use Tmsetcur).
	RETURNS: nothing
*/
void TmPuts(sp)
register char *sp;				/* String to be displayed */
{
	register char *tp = TmOut;	/* -> output string */
	static int inverse = 0;		/* in inverse video mode */

	while (*sp != '\0' && TmCol < 79)
	{	/* Convert non-printing chars to display in inverse */
		if (*sp < ' ' || *sp >= '\177')
		{	if (!inverse)
strcpy(tp, "\033[7m"), tp += 4;
			inverse = 1;
			if (*sp < ' ')
				*tp++ = *sp + '@';
			else
				*tp++ = 'x';
		}
		else
		{	if (inverse)
strcpy(tp, "\033[0m"), tp += 4;
			inverse = 0;
			*tp++ = *sp;
		}
		TmCol++, sp++;
	}
	*tp = '\0';
	Xputs(TmOut);
}
/*	--------------------------------------------------------
	TmRst - restore the terminal to its original condition
	--------------------------------------------------------
	ANSI: ESC [ ? 7 h         Enable auto wrap
	RETURNS: nothing
*/
void TmRst()
{
	TmWndw(0, TMROWS-1);			/* Restore the window */
	TmSetcur(0, TMROWS-1);		/* Erase the last line */
	TmDeol();
	Xputs("\033[?7h");			/* Enable auto wrap */
}

/*	--------------------------------------------------------
	TmSetcur - Move the cursor within the current window
	--------------------------------------------------------
	Never any scrolling allowed!!
	x and y are measured from the top left of the window (0,0).
	ANSI:    ESC [ yy ; xx H
	         where xx represents x+1 as ASCII decimal chars
			 and yy represents y+1 as ASCII decimal chars.
	NOTE:	The cursor can be moved outside the current
			window.
	RETURNS: nothing
 */
void TmSetcur(x, y)
register int x, y;   /* screen position - top left is 0,0 */
{
	x = min(TMCOLS-1, x);
	y = min(TMROWS-1, y);

	/* Convert position to ascii and send seq */
	sprintf(TmOut, "\033[%d;%dH", y+1, x+1);
	Xputs(TmOut);
	TmCol = x;
	TmRow = y;
}

/*	--------------------------------------------------------
	TmWndw - Establish a new window
	--------------------------------------------------------
	Top is the position of the first row of the window and
	bot is the position of the last row of the window.
	Top must be less than bot. They vary from 0 to ROWS-1.
	Sets a scrolling region and moves the cursor to home.
	For example, a full screen window (24 rows) is set with
	TmWndw(0, 23).
	ANSI: ESC [ top ; bot r
	RETURNS: nothing
 */
void TmWndw(top, bot)
int top, bot;  /* top and bottom lines of the window */
{
	/* Convert top, bot to ASCII string */
	sprintf(TmOut, "\033[%d;%dr", top+1, bot+1);
	Xputs(TmOut);
	TmCol = TmRow = 0;
	TmBot = bot;
	TmWndwsz = bot - top + 1;		/* New window size */
}
/* term.c */
