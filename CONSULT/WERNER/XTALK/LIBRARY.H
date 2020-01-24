/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Library Include File (LIBRARY.H)                                *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

extern long cdecl atol(char *);
extern int  cdecl atoi(char *);
extern char * cdecl ltoa(long, char *, int);
extern char * cdecl itoa(int, char *, int);
//extern int  cdecl eof(int);
//extern int  cdecl chdir(char *);
//extern int  cdecl rename(char *, char *);

extern USHORT XlatError(USHORT);
extern USHORT StrRightTrim(char *psz);
extern void ChangeMenuMsg(WORD, int);
extern void ChangeMenuMsg1(WORD, int, char *);
extern void ChangeMenuText(WORD, PSTR);
extern void LibDestore(BYTE *, BYTE *, int);
extern void LibEnstore(BYTE *, BYTE *, int);
extern int  WhichRadioButton(HWND, WORD, WORD, WORD);
extern void GetDlgItemData(HWND, WORD, char *, int);
extern BOOL GetDlgItemPath(HWND, WORD, VAR *, SHORT);
extern void SetDlgItemData(HWND, WORD, char *);
extern void SetDlgItemLength(HWND, WORD, int);
extern void SetDlgItemPath(HWND, WORD, char *);
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
extern char * StripName(char *);
extern void SetScrollLock(HWND, int);
extern int    SetFilePath(char *);
extern USHORT SetPathFile(char *);
extern int    SetPath(char *);
extern void SetDlgItemLong(HWND, int, unsigned long, int);
extern void SpawnApp(char *, char *);

extern void   LibFilterString(BYTE *, BYTE);
extern void   CreatePath(char *, short, char *);
extern short  SetFullPath(int);
extern void   SeekToEndOfFile(HFILE hf);
extern USHORT LibGetLine(PSZ, USHORT, HFILE, PUSHORT, PUSHORT, PBYTE, USHORT);
extern void   LibDelay(USHORT);

//#define  BUFSIZ  512
#define  EOF	 (-1)
//#define  WINOPEN 20

//#define  SEEK_CUR 1
//#define  SEEK_END 2
//#define  SEEK_SET 0

//typedef struct
//  {
//  char *ptr;
//  int   cnt;
//  char *base;
//  int   hmem;
//  int   file;
//  char  flag;
//  } FILE;

//extern FILE WinFiles[WINOPEN];

/*
#define  _IOREAD    0x01
#define  _IOWRT     0x02
#define  _IOAPPEND  0x04

#define  _IOEOF     0x10
#define  _IOERR     0x20
#define  _IORW	    0x40			// or 0x80??? PJL (dca00045)
*/

//#define  getc(f)    (--(f)->cnt >= 0 ? 0xff & *(f)->ptr++ : wfilbuf(f))
//#define  putc(c,f)  (--(f)->cnt >= 0 ? 0xff & (*(f)->ptr++ = (char)(c)) : \
//	       wflsbuf((c),(f)))

//#define  feof(f)     ((f)->flag & _IOEOF)
//#define  ferror(f)   ((f)->flag & _IOERR)
//#define  fileno(f)   ((f)->file)


/* function prototypes */

extern BOOL IsPathAvailable(short nVarNo);
extern USHORT IsDriveReady(char *szPath);
extern BOOL   ValidateDiskDrive(char *szPath);
extern BOOL   ValidatePath(char *szPath);

//int	 wfilbuf(FILE *);
//int	 wflsbuf(int, FILE *);
//int	 fclose(FILE *);
//FILE * fopen(char *, char *);

//void	 clearerr(FILE *);
//int	 fcloseall(void);
//FILE * fdopen(int, char *);
//int	 fflush(FILE *);
//int	 fgetc(FILE *);
//int	 fgetchar(void);
//int	 fgetpos(FILE *, long *);
//char * fgets(char *, int, FILE *);
//int	 flushall(void);
//int	 fputc(int, FILE *);
//int	 fputchar(int);
//int	 fputs(const char *, FILE *);
//int	 fread(void *, int, int, FILE *);
//FILE * freopen(const char *, const char *, FILE *);
//int	 fsetpos(FILE *, const long *);
//int	 fseek(FILE *, long, int);
//long	 ftell(FILE *);
//int	 fwrite(void *, int, int, FILE *);
//char * gets(char *);
//int	 getw(FILE *);
//int	 puts(const char *);
//int	 putw(int, FILE *);
//int	 remove(const char *);
//void	 rewind(FILE *);
//int	 ungetc(int, FILE *);
//long	 filelength(const int);

//char * tempnam(char *, char *);
//FILE * tmpfile(void);
//char * tmpnam(char *);
//int	 rmtmp(void);

//int  cdecl open(char *, int, ...);
//int  cdecl close(int);
//int  cdecl read(int, char *, unsigned int);
//int  cdecl write(int, char *, unsigned int);
//long cdecl lseek(int, long, int);
//long cdecl tell(int);

/*
int    rename(const char *, const char *);
void   perror(const char *);
void   setbuf(FILE *, char *);
int    setvbuf(FILE *, char *, int, size_t);
int    fprintf(FILE *, const char *, ...);
int    fscanf(FILE *, const char *, ...);
int    printf(const char *, ...);
int    scanf(const char *, ...);
int    sscanf(const char *, const char *, ...);
int    sprintf(char *, const char *, ...);
int _CDECL vsprintf(char *, const char *, va_list);
int _CDECL vprintf(const char *, va_list);
int _CDECL vfprintf(FILE *, const char *, va_list);
*/

int cdecl sprintf(char *, const char *, ...);
