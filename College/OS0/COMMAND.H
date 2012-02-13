/* command.h */

#define ECHO

#define CMDSIZE 80
#define MAXARGS 16

typedef struct arguments
{
  int num ;
  char *arg [MAXARGS] ;
} arguments, ARG, *ARGPTR ;

#define NCMDS 14

enum cmdnums {  CADDUSER = 0,
		CBYE,
		CCMD,
		CDEL,
		CDIR,
		CECHO,
		CFORMAT,
		CHELP,
		CLIST,
		CLOGIN ,
		CLOGOUT,
		CMAIL,
		CTIME,
		CTYPE,
		} ;

/* if commands are added to this list they must be */
/* added in alphabetic order.  Also, a corredponding */
/* enum value must be added to the cmdnum enumeration */
/* plust NCMDS must be changed... */
static char *cmdnames [] =
{
  "adduser",
  "bye",
  "cmd",
  "del",
  "dir",
  "echo",
  "format",
  "help",
  "list",
  "login",
  "logout",
  "mail",
  "time",
  "type"
} ;

int binsearch (char *word) ;
int CmdToNum (char * Cmd, ARGPTR Args) ;
char *strchr (char *str, char c) ;
int Ctype (int TermNum, ARGPTR Args) ;

