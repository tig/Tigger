/*  xsys.c

    SYS - System I/O routines   UNIX VERSION  (X)

Author:  T. L. Williams
Ver:     1.01
Date:    10-Jul-87
 
The standard set of editor i/o routines are defined here.
This file contains ALL of the system dependent routines
with the exception of the assembly language routines and
the file i/o routines in file.h.
Xsys.h contains ALL of the system dependent definitions.

I/O to the terminal should be as direct as possible with as
little special processing as possible.

Limited to UNIX systems that support ioctl (NOT HP-UX)

REVISIONS:
	DATE       VER   NAME           CHANGE
	13-Feb-85  1.00  T. L. Williams Original
	10-Jul-87  1.01  tlw            Add Xputs
*/

/* IMPORTS */
#include <sys/ioctl.h>    		/* in /usr/include/sys */
#include <sys/file.h>			/* open tty */
#include "xsys.h"

/*  DEFINITIONS */
#define ESC  '\033'				/* Escape character */
#define BEL  '\07'

/* DATA */
static int fd;                  /* "file descriptor" for our tty */
struct sgttyb ob;			    /* holds tty characteristics */

/* FUNCTIONS */
/*	------------------------------------------------------------------
	Xbel - Ring the "bell"
	------------------------------------------------------------------
	RETURNS: Nothing
 */
void Xbel()
{
	char c = BEL;
	write(fd, &c, 1);			/* Ctrl-G */
}

/*	------------------------------------------------------------------
	Xgetc -  Get a character from the keyboard
	------------------------------------------------------------------
	Gets key input in raw mode. Key input is translated into key codes
	between 1 and 254. Codes below 128 are for ASCII codes. Most
	terminals do not send codes above 128 (but the PC does).

	Environment: UNIX (BSD 4.3)
	Limitations: Depends upon raw i/o
	RETURNS: keycode or 0 if no char available
*/
int Xgetc(peek)
	int peek;
{
	char key;				/* key info from serial port */
	int count;

	if (peek == 1)
	{	ioctl(fd, FIONREAD, &count);
		if (count <= 0)		/* no input available */
			return (0);
	}
    read(fd, &key, 1);
    return (int)(key & 0x7f);
}

/*	------------------------------------------------------------------
	Xinit - Setup the i/o for the ibm pc
	------------------------------------------------------------------
	RETURNS: nothing
 */
void Xinit()
{
	int flags;
    /* open our terminal */
    if ((fd = open("/dev/tty", O_RDWR)) == -1)
    {   perror("open of tty fails\n");
        return;
    }

    /* get current tty characteristics */
    ioctl(fd, (int)TIOCGETP, (char *)&ob);
/*  printf("ispd = %x, ospd = %x, erase = %x, kill = %x, flags = %x\n",
    ob.sg_ispeed, ob.sg_ospeed, ob.sg_erase, ob.sg_kill, ob.sg_flags); */
 
    /* change to raw mode */
	flags = ob.sg_flags;
    ob.sg_flags = RAW;           /* Set to RAW for NO processing */
    ioctl(fd, TIOCSETP, &ob);
	ob.sg_flags = flags;
}

/*	------------------------------------------------------------------
	Xputc - Send c to the terminal with no char processing!
	------------------------------------------------------------------
	No translation of control characters. Sent as received.
	RETURNS: nothing
 */
void Xputc(ch)
char ch;
{
	write(fd, &ch, 1);
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
	write(fd, bp, strlen(bp));
}

/*	------------------------------------------------------------------
	Xrst - Undo the I/O setup upon termination of edit
	------------------------------------------------------------------
 */
void Xrst()
{
    /* Here is the "standard" setup. Constants from ioctl.h 
    ob.sg_flags = ECHO | CRMOD | ANYP | TBDELAY | VTDELAY;
	*/
    ioctl(fd, TIOCSETP, &ob);

/*  printf("ispd = %x, ospd = %x, erase = %x, kill = %x, flags = %x\n",
     ob.sg_ispeed, ob.sg_ospeed, ob.sg_erase, ob.sg_kill, ob.sg_flags); */
}

/* xsys.c */
