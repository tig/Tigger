/************************************************************************
 *
 *     Project:  ZMODEM implementation that works on Windows or Xenix
 *
 *      Module:  winunix.h
 *
 *     Remarks:
 *
 *    This is a sick and wrong attempt at achiving code portabiltiy
 *    between Xenix and Windows.  Much of what follows is
 *    derived from WINDOWS.H WINDOWSX.H and mostly is required
 *    for the ZMODEM.DLL when being compiled on Xenix.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#ifndef _WINUNIX_H_
#define _WINUNIX_H_

#include <fcntl.h>      // c7
#include <sys/types.h>
// not defined in sys/types like it should be!
typedef struct utimbuf
{
	 time_t actime;
	 time_t modtime;
} utimbuf;

#include <sys/times.h>
#include <sys/stat.h>
#include <time.h>       // c7
//#include <io.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define VOID		    void

#define FAR           
#define NEAR		    
#define PASCAL		    
#define CDECL		    

#define WINAPI        
#define CALLBACK      

typedef int		      BOOL;
#define FALSE		    0
#define TRUE		    1

typedef unsigned char	    BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;

typedef unsigned int	    UINT;

typedef signed long	    LONG;

#define LOBYTE(w)	         ((BYTE)(w))
#define HIBYTE(w)          ((BYTE)(((UINT)(w) >> 8) & 0xFF))

#define LOWORD(l)           ((WORD)(DWORD)(l))
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))

#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))

/* Types use for passing & returning polymorphic values */
typedef UINT WPARAM;
typedef LONG LPARAM;
typedef LONG LRESULT;

#define MAKELPARAM(low, high)	((LPARAM)MAKELONG(low, high))
#define MAKELRESULT(low, high)	((LRESULT)MAKELONG(low, high))

/****** Common pointer types ************************************************/

#ifndef NULL
#define NULL		    0
#endif

typedef char NEAR*          PSTR;
typedef char NEAR*          NPSTR;

typedef char FAR*           LPSTR;
typedef const char FAR*     LPCSTR;

typedef BYTE NEAR*	    PBYTE;
typedef BYTE FAR*	    LPBYTE;

typedef int NEAR*	    PINT;
typedef int FAR*	    LPINT;

typedef WORD NEAR*          PWORD;
typedef WORD FAR*           LPWORD;

typedef long NEAR*	    PLONG;
typedef long FAR*	    LPLONG;

typedef DWORD NEAR*         PDWORD;
typedef DWORD FAR*          LPDWORD;

typedef void FAR*           LPVOID;

#define MAKELP(sel, off)    ((void FAR*)MAKELONG((off), (sel)))
#define SELECTOROF(lp)      HIWORD(lp)
#define OFFSETOF(lp)        LOWORD(lp)

#define FIELDOFFSET(type, field)    ((int)(&((type NEAR*)1)->field)-1)

/****** Common handle types *************************************************/

typedef const void NEAR*        HANDLE;
#define DECLARE_HANDLE(name)    struct name##__ { int unused; }; \
                                typedef const struct name##__ NEAR* name
#define DECLARE_HANDLE32(name)  struct name##__ { int unused; }; \
                                typedef const struct name##__ FAR* name
typedef HANDLE* 	    PHANDLE;
typedef HANDLE NEAR*	    SPHANDLE;
typedef HANDLE FAR*	    LPHANDLE;

typedef HANDLE		    HGLOBAL;
typedef HANDLE		    HLOCAL;

typedef HANDLE		    GLOBALHANDLE;
typedef HANDLE		    LOCALHANDLE;

typedef UINT                ATOM;

typedef void (CALLBACK*     FARPROC)(void);
typedef void (NEAR PASCAL*  NEARPROC)(void);

DECLARE_HANDLE(HSTR);

/****** typedefs, structures, and functions **************************/

DECLARE_HANDLE(HINSTANCE);
typedef HINSTANCE HMODULE;  /* HMODULEs can be used in place of HINSTANCEs */


typedef int HFILE;      /* Polymorphic with C runtime file handle type */

#define HFILE_ERROR ((HFILE)-1)

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

typedef void* HWND ;

/*
 * Misc stuff necessary to make zmodem.c compile correctly
 */

#define IsWindow(x)              (x)

#define _fmemcpy                 memcpy
#define _fmemset                 memset
#define _fstrncpy                strncpy

#define lstrcpy                  strcpy
#define lstrlen                  strlen

#define GlobalAllocPtr(t,n)      malloc(n)
#define GlobalFreePtr            free
#define LocalAlloc(t,n)          malloc(n)
#define LocalFree                free

#define _lopen                   open
#define _llseek                  lseek
#define _lclose                  close
#define _lwrite                  write
#define _lread                   read
#define _lcreat                  creat
#define _filelength              filelength
#define OF_READ                  O_RDONLY
#define OF_WRITE                 O_WRONLY
#define OF_CREATE                O_CREAT
#define OF_READWRITE             O_RDWR

struct tms tmsbuf ;
#define GetCurrentTime()         (DWORD)times(&tmsbuf)

#define wsscanf                  sscanf
#define wsprintf                 sprintf

#define SendMessage( hwnd, msg, w, p)
#define PostMessage( hwnd, msg, w, p)


#define hWDB   0

#ifdef DEBUG


    /* cheap and dirty debug macros */
    #define ODS(x)      printf(x)
    #define D(x)        {x;}

    #define DP      if (0) ((int (_cdecl *)(char *, ...)) 0)
    #define DP1     if (0) ((int (_cdecl *)(char *, ...)) 0)
    #define DP2     if (0) ((int (_cdecl *)(char *, ...)) 0)
    #define DP3     if (0) ((int (_cdecl *)(char *, ...)) 0)
    #define DP4     if (0) ((int (_cdecl *)(char *, ...)) 0)
    #define DP5     if (0) ((int (_cdecl *)(char *, ...)) 0)
#else

    #define ODS(a)
    #define D(x)

    #define DP      if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DP1     if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DP2     if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DP3     if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DP4     if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DP5     if (0) ((int (_cdecl *)(int, char *, ...)) 0)

#endif

#endif


/************************************************************************
 * End of File: winunix.h
 ***********************************************************************/

