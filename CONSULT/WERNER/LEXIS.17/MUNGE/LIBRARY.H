/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*  Include: Library Include File (LIBRARY.H)                                 */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

typedef struct
  {
  int   inx;
  int	cnt;
  char  buffer[512];
  int	file;
  } FILEBLK;

  extern void SpawnApp(char *, char *);

  extern int  FileOpenRead(FILEBLK *, char *);
  extern int  FileOpenWrite(FILEBLK *, char *);
  extern int  FileReadLine(FILEBLK *, char *, int);
  extern int  FileWriteLine(FILEBLK *, char *, int);
  extern int  FileClose(FILEBLK *);


extern int  SetFilePath(char *);
extern void ChangeMenuMsg(WORD, int);
extern void ChangeMenuText(WORD, PSTR);
extern void LibDestore(BYTE *, BYTE *, int);
extern void LibEnstore(BYTE *, BYTE *, int);
extern int  WhichRadioButton(HWND, WORD, WORD, WORD);
extern void GetDlgItemData(HWND, WORD, char *, int);
extern void SetDlgItemData(HWND, WORD, char *);
extern void SetDlgItemLength(HWND, WORD, int);
extern int  lmemcmp(LPSTR, LPSTR, WORD);
extern void lmemcpy(LPSTR, LPSTR, WORD);
extern void lmemset(LPSTR, BYTE, WORD);
extern void LibBinToAsc(BYTE *, BYTE *, int);
extern void LibAscToBin(BYTE *, BYTE *);
extern int  LibStrToInt(char *, long *);
extern void GetProfileData(char *, char *, char *, char *, int);
extern void WriteProfileData(char *, char *, char *);
extern int  IsEnoughDiskSpace(int, long);
extern char * StripPath(char *);

#define  BUFSIZ  512
#define  EOF	 (-1)
#define  WINOPEN 20

#define  SEEK_CUR 1
#define  SEEK_END 2
#define  SEEK_SET 0

typedef struct
  {
  char *ptr;
  int	cnt;
  char *base;
  int	hmem;
  int	file;
  char	flag;
  } FILE;

extern FILE WinFiles[WINOPEN];

#define  _IOREAD    0x01
#define  _IOWRT     0x02
#define  _IOAPPEND  0x04

#define  _IOEOF     0x10
#define  _IOERR     0x20
#define  _IORW	    0x40


#define  getc(f)    (--(f)->cnt >= 0 ? 0xff & *(f)->ptr++ : wfilbuf(f))
#define  putc(c,f)  (--(f)->cnt >= 0 ? 0xff & (*(f)->ptr++ = (char)(c)) : \
	     wflsbuf((c),(f)))

#define  feof(f)     ((f)->flag & _IOEOF)
#define  ferror(f)   ((f)->flag & _IOERR)
#define  fileno(f)   ((f)->file)


/* function prototypes */

int    wfilbuf(FILE *);
int    wflsbuf(int, FILE *);
int    fclose(FILE *);
FILE * fopen(char *, char *);

void   clearerr(FILE *);
int    fcloseall(void);
FILE * fdopen(int, char *);
int    fflush(FILE *);
int    fgetc(FILE *);
int    fgetchar(void);
int    fgetpos(FILE *, long *);
char * fgets(char *, int, FILE *);
int    flushall(void);
int    fputc(int, FILE *);
int    fputchar(int);
int    fputs(const char *, FILE *);
int    fread(void *, int, int, FILE *);
FILE * freopen(const char *, const char *, FILE *);
int    fsetpos(FILE *, const long *);
int    fseek(FILE *, long, int);
long   ftell(FILE *);
int    fwrite(void *, int, int, FILE *);
char * gets(char *);
int    getw(FILE *);
int    puts(const char *);
int    putw(int, FILE *);
void   rewind(FILE *);
int    ungetc(int, FILE *);
int    unlink(const char *);
long   filelength(const int);

char * tempnam(char *, char *);
FILE * tmpfile(void);
char * tmpnam(char *);
int    rmtmp(void);

int  cdecl open(char *, int, ...);
int  cdecl close(int);
int  cdecl read(int, char *, unsigned int);
int  cdecl write(int, char *, unsigned int);
long cdecl lseek(int, long, int);
long cdecl tell(int);

/*
void   perror(const char *);
void   setbuf(FILE *, char *);
int    setvbuf(FILE *, char *, int, size_t);
int    fprintf(FILE *, const char *, ...);
int    fscanf(FILE *, const char *, ...);
int    printf(const char *, ...);
int    scanf(const char *, ...);
int    sscanf(const char *, const char *, ...);
int _CDECL vsprintf(char *, const char *, va_list);
int _CDECL vprintf(const char *, va_list);
int _CDECL vfprintf(FILE *, const char *, va_list);
*/

int _cdecl sprintf(char *, const char *, ...);
int _cdecl rename(const char *, const char *);
int _cdecl remove(const char *);
int _cdecl chdir(char *);

extern BOOL ValidateSpeed(char *ptr);
extern BOOL ValidateNumber(char *ptr);
extern BOOL ValidatePath(char *ptr);
extern BOOL ValidateFilename(char *ptr);
