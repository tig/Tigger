/*  xsys.c

    SYS - System I/O routines   UNIX VERSION  (X)

Author:  T. L. Williams
Ver:     1.01
Date:    12-Jul-87
 
The standard set of editor i/o routines are defined here.
This file contains ALL of the system dependent routines with the
exception of the assembly language routines and the file i/o routines
in file.h.
Xsys.h contains ALL of the system dependent definitions.

I/O to the terminal should be as direct as possible with as little
special processing as possible.

INPUT KEYCODES
Normal ASCII codes are returned for regular, shifted, and control
chars except as noted below. The extended code is the "scan code"
returned by the BIOS keyboard input routine. xgetc returns the Keycode
which is mostly just the extended code + 120 (all in decimal)

       Scan Key         Scan Key        Scan Key         Scan Key
 Key   Code Code   Key  Code Code  Key  Code Code   Key  Code Code

Ctrl-@    3 133  |<---    15 135  Alt-Q   16  136  Alt-W    17 137
Alt-E    18 138  Alt-R    19 139  Alt-T   20  140  Alt-Y    21 141
Alt-U    22 142  Alt-I    23 143  Alt-O   24  144  Alt-P    25 145
Alt-A    30 150  Alt-S    31 151  Alt-D   32  152  Alt-F    33 153
Alt-G    34 154  Alt-H    35 155  Alt-J   36  156  Alt-K    37 157
Alt-L    38 158  Alt-Z    44 164  Alt-X   45  165  Alt-C    46 166
Alt-V    47 167  Alt-B    48 168  Alt-N   49  169  Alt-M    50 170
F1       59 179  F2       60 180  F3      61  181  F4       62 182
F5       63 183  F6       64 184  F7      65  185  F8       66 186
F9       67 187  F10      68 188  Home    71  191  ^|       72 192
PgUp     73 193  <-       75 195  ->      77  197  End      79 199
v|       80 200  PgDn     81 201  Ins     82  202  Del      83 203
Sh-F1    84 204  Sh-F2    85 205  Sh-F3   86  206  Sh-F4    87 207
Sh-F5    88 208  Sh-F6    89 209  Sh-F7   90  210  Sh-F8    91 211
Sh-F9    92 212  Sh-F10   93 213  Ctl-F1  94  214  Ctl-F2   95 215
Ctl-F3   96 216  Ctl-F4   97 217  Ctl-F5  98  218  Ctl-F6   99 219
Ctl-F7  100 220  Ctl-F8  101 221  Ctl-F9  102 222  Ctl-F10 103 223
Alt-F1  104 224  Alt-F2  105 225  Alt-F3  106 226  Alt-F4  107 227
Alt-F5  108 228  Alt-F6  109 229  Alt-F7  110 230  Alt-F8  111 231
Alt-F9  112 232  Alt-F10 113 233  Ctl-Prt 114 234  Ctl <-  115 235
Ctl ->  116 236  Ctl-End 117 237  Ct-PgDn 118 238  Ct-Home 119 239
Alt-1   120 240  Alt-2   121 241  Alt-3   122 242  Alt-4   123 243
Alt-5   124 244  Alt-6   125 245  Alt-7   126 246  Alt-8   127 247
Alt-9   128 248  Alt-10  129 249  Alt '-' 130 250  Alt =   131 251
Ct-PgUp 132 252

REVISIONS:
	DATE       VER   NAME           CHANGE
	13-Feb-85  1.00  T. L. Williams Original
	12-Jul-87  1.01  tlw            Add Xputs
*/

/* IMPORTS */
#define LINTARGS 1
#define LINT_ARGS 1
#define MSDOS  1

#include <io.h>
#include "bio.h"
#include "xsys.h"

/*  DEFINITIONS */
#define ESC  '\033'				/* Escape character */

/* Starting function key code */
#define FCNKEY	  128

/* DATA */

/* FUNCTIONS */
/*	------------------------------------------------------------------
	Xbel - Ring the "bell"
	------------------------------------------------------------------
	RETURNS: Nothing
 */
void Xbel()
{
	BIOBEL();
}

/*	------------------------------------------------------------------
	Xgetc -  Get a character from the keyboard
	------------------------------------------------------------------
	Gets key input in raw mode. Key input is translated into key codes
	between 1 and 254. Codes below 128 are for ASCII codes. Codes for
	the function keys and the ALT-X keys are above 128.

	The PC BIOS call does not return a code for all combinations of key
	presses, so don't expect miracles.

	Environment: IBM-PC
	Limitations: Depends upon PC BIOS. Depends upon codes from standard
				 84 key keyboard.
	RETURNS: keycode or 0 if no char available
*/
int Xgetc(peek)
	int peek;
{
	int keyin;				/* key info from BIOS call */
	int ascii, code;

	/* get key info from MS-DOS. Quit if no key in peek mode */
	if (peek && BIOKEY(peek) == 0)
		/* no key available */
		return 0;
	else
		keyin = BIOKEY(0);

	/* ascii code in low byte, extended code in upper byte */
	ascii = keyin & 0xff;
	code  = keyin >> 8 & 0xff;

	/* Mostly, return non-zero ascii code as key code, except
	   translate Backspace into DEL.
	 */
	if (ascii > 0)
	{	/* Backspace */
		if (code == 14)
		{	if (ascii == '\b')
				ascii = '\177';  /* DEL */
			else
				ascii = '\177';  /* DEL in any case */
		}
		/* Tab key is code 15, return key is 28 */
	}
	/* else, just use the extended function code + FCNKEY */
	else
	{	/* translate "null" to extended function code (133) */
		if (code == 3)
			ascii = FCNKEY + '\05';
		/* add FCNKEY - 8 to extended codes (135 - 252) */
		else
			ascii = (code - 8) + FCNKEY;
	}
	return ascii;
}

/*	------------------------------------------------------------------
	Xinit - Setup the i/o for the ibm pc
	------------------------------------------------------------------
	RETURNS: nothing
 */
void Xinit()
{
}

/*	------------------------------------------------------------------
	Xputc - Send c to the terminal with no char processing!
	------------------------------------------------------------------
	No translation of control characters. Sent as received. Non-Ascii
	codes yields special symbols as defined by IBM rom.
	Limitations: Depends upon PC BIOS
	RETURNS: nothing
 */
void Xputc(ch)
char ch;
{
			BIOPUTC(ch);
}

/*	------------------------------------------------------------------
	Xputs - display string
	------------------------------------------------------------------
	Does not send the null character on the end.
	RETURNS: nothing
 */
void Xputs(bp)
char *bp;						/* Address of the string */
{
	while (*bp != '\0')
		BIOPUTC(*bp++);
}

/*	------------------------------------------------------------------
	Xrst - Undo the I/O setup upon termination of edit
	------------------------------------------------------------------
 */
void Xrst()
{
	/* nothing for now */
}
/* xsys.c */
