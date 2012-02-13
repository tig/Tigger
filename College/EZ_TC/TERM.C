/* term.c

	  TERM  Terminal i/o routines for ANSI terminals (Tm)

Name:    T. L. Williams
Version: 1.02
Date:    12-Jul-87

PURPOSE
	The cursor control and delete routines for ibm-pc terminals
	are defined here.
	All of the terminal dependent definitions are found in term.h
	These 2 files contains ALL of the terminal dependent info.

	These are the ONLY routines that need to be changed to port
	the editor to terminals that do not have the ANSI commands
	needed in this version of term.
	Term uses the assembly language screen functions found in
	bios.asm See the ibm-pc hardware technical manual, appendix C
	for details as well as the bios.asm listing.

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
	12-Jul-87  1.02     tlw           Add Tmputs.
 */

/* IMPORTS */
#define MSDOS 1
#define LINT_ARGS 1
#define LINTARGS 1

#include <stdio.h>			/* sprintf */
#include <string.h>			/* strcat, strcpy */
#include "bio.h" 			/* BIO routines */
#include "xsys.h" 			/* serial i/o fcns */
#include "term.h"			/* terminal definitions */

/* DEFINITIONS */
#define TMOUT   80			/* Place to hold a row of output */

/* DATA */
int TmWndwsz;				/* Rows in window (23 or 10) */

static int  TmRow, TmCol;	/* Cursor position */
static char TmOut[TMOUT];
static int  TmBot;			/* Save bottom row from window */

/* FUNCTIONS */

/*	------------------------------------------------------------------
	TmAttr - Set terminal attributes
	------------------------------------------------------------------
	TMNORM and TMINVR set attributes. Others add a
	characteristic to existing attribute.
	RETURNS: nothing
 */
void TmAttr(attr)
	int attr;				/* Attribute code (see term.h) */
{
	switch (attr)
	{
	case TMNORM:
		bioattr = (char)0x7;
		break;
	case TMINVR:
		bioattr = (char)0x70;
		break;
	case TMBOLD:
		bioattr |= (char)0x8;
		break;
	case TMUNDL:
		bioattr |= (char)0x40;
		break;
	case TMBLNK:
		bioattr |= (char)0x80;
	default:
		break;
	}
}

/*	------------------------------------------------------------------
	TmClr - Clear the screen and home the cursor
	------------------------------------------------------------------

	RETURNS: nothing
 */
void TmClr()
{
	BIODWN(0,0,24);
	TmSetcur(0,0);
}

/*	------------------------------------------------------------------
	TmDeol - Delete from the cursor to the end of the line.
	------------------------------------------------------------------
	Cursor does not move.

	RETURNS: nothing
 */
void TmDeol()
{
	BIODEOL(TMCOLS - TmCol);
}

/*	------------------------------------------------------------------
	TmDelrow  - Delete rows and move text up
	------------------------------------------------------------------
	Move the display up by deleting the line that the cursor
	is on and n-1 subsequent lines. Move the rest of the
	display up leaving n blank lines at the bottom.
	The cursor is moved to the beg. of the line that it is on.

	LIMITATIONS: Cursor must be in current window.
	RETURNS:     nothing
*/
void TmDelrow(n)
int n;					/* Num of rows to be deleted */
{
	if (n > TmBot - TmRow + 1)
		n = 0;			/* Delete entire window */

	TmSetcur(0, TmRow);
	if (TmRow == TmBot)		/* Avoid BIOS malfunction */
		BIODEOL(TMCOLS); 
	else
		BIOUP(n, TmRow, TmBot);
}
/*	------------------------------------------------------------------
	TmGetcur   Returns the current cursor position
	------------------------------------------------------------------
	The cursor position is returned with the column in the
	lower 8 bits and the row in the upper 8 bits.
 */
int TmGetcur()
{
	return ((unsigned)TmRow << 8 | TmCol);
}

/*	------------------------------------------------------------------
	TmInit - Configure terminal for editing.
	------------------------------------------------------------------
	Set up for ANSI terminal, clear the decks for action.
	RETURNS: nothing
 */
void TmInit()
{
	TmClr();
	TmWndw(0, TMROWS-2);		/* leave one row for commands */
}

/*	------------------------------------------------------------------
	TmInsrow - Insert blank lines and move text down
	------------------------------------------------------------------
	Move the display down by inserting n blank lines starting
	at the same line as the cursor is on. Move the line of
	text that contained the cursor and all the subsequent
	lines down n lines.
	The cursor is moved to the beginning of the line it was on.

	LIMITATIONS: Cursor must be in current window
	RETURNS:     nothing
 */
void TmInsrow(n)
int n;					/* number of rows inserted */
{
	/* if n is large, clear entire display */
	if (n > TmBot - TmRow + 1)
		n = 0;				/* Delete entire screen */

	TmSetcur(0, TmRow);
	if (TmRow == TmBot)		/* Avoid BIOS malfunction */
		BIODEOL(TMCOLS);
	else
		BIODWN(n, TmRow, TmBot);
}
/*	------------------------------------------------------------------
	TmPuts - Send string to the terminal, move cursor
	------------------------------------------------------------------
	Control characters are shown in inverse video. Tabs are
	expanded into spaces and newlines are not sent to the
	terminal (use Tmsetcur).
	RETURNS: nothing
*/
void TmPuts(sp)
register char *sp;				/* String to be displayed */
{
	register char *tp = TmOut;	/* -> output string */

	while (*sp != '\0' && TmCol <= TMCOLS-1)
	{	*tp++ = *sp++;
		TmCol++;
	}
	*tp = '\0';
	Xputs(TmOut);
}
/*	------------------------------------------------------------------
	TmRst - restore the terminal to its original condition
	------------------------------------------------------------------
	RETURNS: nothing
*/
void TmRst()
{
	TmSetcur(0, TMROWS-1);		/* Erase the last line */
	TmDeol();
}
/*	------------------------------------------------------------------
	TmSetcur - Move the cursor within the current window
	------------------------------------------------------------------
	Never any scrolling allowed!!
	x and y are measured from the top left of the window (0,0).
	NOTE:	The cursor can be moved outside the current	window.
	RETURNS: nothing
 */
void TmSetcur(x, y)
register int x, y;   /* screen position - top left is 0,0 */
{	x = min(TMCOLS-1, x);
	y = min(TMROWS-1, y);
	BIOPOS(x, y);
	TmCol = x;
	TmRow = y;
}

/*	------------------------------------------------------------------
	TmWndw - Establish a new window
	------------------------------------------------------------------
	Top is the position of the first row of the window and
	bot is the position of the last row of the window.
	Top must be less than bot. They vary from 0 to ROWS-1.
	Sets a scrolling region and moves the cursor to home.
	For example, a full screen window (24 rows) is set with
	TmWndw(0, 23).
	RETURNS: nothing
 */
void TmWndw(top, bot)
int top, bot;  /* top and bottom lines of the window */
{
	TmBot = bot;
	TmSetcur(0, 0);
	TmWndwsz = bot - top + 1;		/* New window size */
}
/* term.c */
