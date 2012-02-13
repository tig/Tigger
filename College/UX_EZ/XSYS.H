/*  xsys.h    T. L. Williams

             XSYS - System dependent definitions (X)

Integer types:
  C favors char and int (e.g., usual type conversions, etc.),
  so char and int are favored here, even for "small" unsigned
  variables. Further, not all compilers recognize unsigned char,
  unsigned short, or unsigned long (K & R, p 193).
  Not all compilers treat char as signed (e.g. DeSmet). Short and
  int may be 16 bits or more.

  Characters can be widened to unsigned using the macro shown
  below. (See Harbison and Steele, Chap. 6 for more detail)
  Integer type conversions do one of the following:
    1. Preserve the value.
    2. If same size, preserve the bit pattern.
    3. If widening a signed number, do sign extension first.
    4. If narrowing, truncate.

DIRECTORY
void Xbel()      Send BEL command to the terminal.
int  Xgetc(peek) Get a character from the keyboard, no char processing.
	 int peek    If 0, wait for input, else do not wait.
void Xinit()     Set input to raw mode.
void Xputc(c)    Send char to the terminal.
	 char c      Char to be displayed.
void Xputs(sp)   Send a string to the terminal.
	 char *sp    Address of string to be sent.
void Xrst()		 Reset serial i/o to previous mode.
 */
#define UNIX	1
#define YES     1
#define NO      0

#define Uns unsigned
#define char_uns(c)  (int)(unsigned char)(c)
#define MAXINT  (~0 >> 1)

#define min(x, y) (x) < (y) ? (x) : (y)
#define max(x, y) (x) > (y) ? (x) : (y)

#ifdef	vax11c		/* For VMS version */
#define void	int
#endif

#ifdef MSDOS	/* For MSDOS - small model */
#define LINT_ARGS  1	/* allow arguments in declarations */
#endif

#ifdef UNIX
#endif

#ifdef LINT_ARGS
void  Xbel(void);
int   Xgetc(int);
void  Xinit(void);
void  Xputc(char);
void  Xputs(char *);
void  Xrst(void);
#else
void Xbel();
void Xinit();
void Xputc();
void Xputs();
void Xrst();
#endif

/* xsys.h */
