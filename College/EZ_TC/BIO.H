/* bio.h  - Assembly language support routines

MSDOS support routines for XSYS and TERM

REVISION HISTORY
	1-Jan-86	Original	tlw
 */

/* DEFINITIONS */

extern char bioattr;

void BIO21(void);			/* install error handler */
void BIOBEL(void);			/* ring the bell */
void BIODEOL(int);			/* erase char to end-of-line */
void BIODWN(int, int, int); /* scroll down n lines, start to end */
int  BIOKEY(int);			/* returns code from key press */
void BIOPOS(int, int);		/* set cursor row, column */
void BIOPUTC(char);			/* write a character to screen */
int  BIOPUTS(char *);		/* write string, returns cursor pos */
void BIOUP(int, int, int);	/* scroll up n lines, start to end */
/* bio.h */
