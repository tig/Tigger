/* mail.h */

#define MAXUSERNAME 16
#define SUBJECTLEN 80 
#define MSGSIZE  8192
#define LINESIZE 80
#define TRUE 1
#define FALSE 0

int Madduser (int TermNum, ARGPTR Args) ;
int Mlogin (int TermNum, char *user, ARGPTR Args) ;
int Mlogout (int TermNum, char *user, ARGPTR Args) ;
int MMail (int TermNum, char *user, ARGPTR Args) ;
void Mlist (int TermNum) ;
