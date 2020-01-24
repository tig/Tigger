#ifndef STDINCH
#define STDINCH

#ifdef UNIX
#define PASCAL
#define FAR
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef int HANDLE;
typedef short BOOL;
#endif
 
#define FALSE 0
#define TRUE 1
#define ON 1
#define OFF 0
#define OK 0
#define FAIL -1
#define FAIL_W_MSG -2
#define NOT !
#define AND &&
#define OR ||
 
#define END(x) {nRetval = x; goto end;}
#endif
