/*       command.c - command interpreter for OS/0

*/

#include <stdio.h>
#include <ctype.h>

#include "filesys.h"
#include "command.h"
#include "mail.h"

extern int CurrentTerm ;
int bEcho = TRUE ;
/* void Ucommand (void) -------------------------
 * 
 * OS/0 command processor.
 * 
 *   Before spawning/calling this process, set the 
 *   external variable CurrentTerm to the number
 *   of the terminal you want the command processor to
 *   run on.  Note that CurrentTerm is incremented by
 *   the command processor after it "uses" the value.
 *   This allows Ucommand to be spawned in a loop.
 *
 *   This command interpreter was designed to be both
 *   spawned and called.  It is suggested that terms
 *   1-3 be spawned, then term 1 be called directly.
 *   This way when the "bye" command is issued at a terminal
 *   greater than 0 the system will not shut down.
*/
void Ucommand (void)
{
  int TermNum = (CurrentTerm++) ;
  ARG Args ;
  char *Cmd ;
  char *user ;
  char *prompt ;
  char *buf ;
  int NumChars, CmdNum, i ;
  short bLoggedIn = FALSE ;

  Cmd = (char*) malloc (sizeof(char)*256) ;
  user = (char*) malloc (sizeof(char)*256) ;
  prompt = (char*) malloc (sizeof(char)*256) ;
  buf = (char*) malloc (sizeof(char)*256) ;

  tprint ("   Command processor %d active\n", TermNum) ;
  sprintf (buf, "\aOS/0 Mail - Copyright (c) 1990, Charles Kindel and Sandy Smith.\n") ;
  Writeterm (buf, strlen (buf), TermNum) ;
  sprintf (buf, "\tCommand processor %d active\n", TermNum) ;
  Writeterm (buf, strlen (buf), TermNum) ;
  sprintf (buf, "Available commands:\n") ;
  Writeterm (buf, strlen(buf), TermNum) ;
  for (i = 0 ; i < NCMDS ; i++)
    {
      sprintf (buf, "\t%s\n", cmdnames[i]) ;
      Writeterm (buf, strlen(buf), TermNum) ;
    }  
  
  strcpy (user, "OS/0") ;
  while (TRUE)
    {
      if (!bLoggedIn && TermNum)
	sprintf (prompt, "\x1b[1mType \"login <username>\" to get started:\x1b[0m ") ;
      else
	sprintf (prompt, "\x1b[1m[%s]\x1b[0m ", user) ;
      Writeterm (prompt, strlen(prompt), TermNum) ;
      NumChars = Readterm (Cmd, CMDSIZE, TermNum) ;

      CmdNum = CmdToNum (Cmd, &Args) ; 

      if (bEcho)
	{
	  /* echo cmd to terminal */
	  for (i = 0 ; i <= Args.num ; i++)
	    {
	      sprintf (buf, "%s ", Args.arg[i]) ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	    }
	  Writeterm ("\n", 1, TermNum) ;
	}

      switch (CmdNum)
	{
	case CECHO :
	  if (TermNum)
	    {
	      sprintf (buf, "\aYou must be logged in to use that command!\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	      break ;
	    }
	  if (Args.num)
	    {
	      if (!strcmp (Args.arg[1], "on"))
		bEcho = TRUE ;
	      else
		if (!strcmp (Args.arg[1], "off"))
		  bEcho = FALSE ;
		else
		  {
		    sprintf (buf, "The usage for the echo command is : \"echo [on|off]\"\n") ;
		    Writeterm (buf, strlen(buf), TermNum) ;
		    break ;
		  }
	    }
	  else
	    {
	      sprintf (buf, "\techo is %s\n", bEcho ? "on"  : "off") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	    }
	  break ;

	case CHELP :
	  sprintf (buf, "Available commands:\n") ;
	  Writeterm (buf, strlen(buf), TermNum) ;
	  for (i = 0 ; i < NCMDS ; i++)
	    {
	      sprintf (buf, "\t%s\n", cmdnames[i]) ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	    }
	  break ;

	case CLIST :
	  if (bLoggedIn || !TermNum)
	    Mlist (TermNum) ;
	  else
	    {
	      sprintf (buf, "\aYou must be logged in to use that command!\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	      break ;
	    }
	  break ;

	case CADDUSER:
	  Madduser (TermNum, &Args) ;
	  break ;

	case CLOGIN :
	  if (bLoggedIn)
	    {
	      Mlogout (TermNum, user, &Args) ;
	      strcpy (user, "OS/0") ;
	    }

	  if (Mlogin (TermNum, user, &Args))
	    bLoggedIn = TRUE ;
	  break ;

	case CLOGOUT :
	  if (!bLoggedIn)
	    {
	      sprintf (buf, "\aNo one is logged in!\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	      break ;
	    }

	  bLoggedIn = !Mlogout (TermNum, user, &Args) ;
	  strcpy (user, "OS/0") ;
	  break ;

	case CMAIL :
	  if (!bLoggedIn)
	    {
	      sprintf (buf, "\aYou must be logged in to use that command!\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	      break ;
	    }
	  MMail (TermNum, user, &Args) ;
	  break ;

	case CCMD :
	  if (!bLoggedIn && TermNum)
	    {
	      sprintf (buf, "\aYou must be logged in to use that command!\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	      break ;
	    }

	  if (Args.num != 1)
	    {
	      sprintf (buf, "\aThe usage for \"cmd\" is: cmd <time>\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	      break ;
	    }
	  sprintf (buf, "Executing a psuedo-command...\n") ;
          Writeterm (buf, strlen(buf), TermNum) ;
	  Delay (atoi(Args.arg[1]) / 10) ;
	  break ;

	case CDIR :
	  if (bLoggedIn || !TermNum)
	    Udir (TermNum) ;
	  else
	    {
	      sprintf (buf, "\aYou must be logged in to use that command!\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	    }
	  break ;

	case CTIME:
	  if (bLoggedIn || !TermNum)
	    {
	      sprintf (buf, "The current time is %d\n", Get_Time_of_Day()) ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	    }
	  else
	    {
	      sprintf (buf, "\aYou must be logged in to use that command!\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	    }
	  break ;

	case CDEL:
	  if (TermNum)       /* check to see if were on term 0 */
	    {
	      sprintf (buf, "\aFiles can be deleted from Terminal 0 only.\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	      break ;
	    }
	  for (i = 1 ; i <= Args.num ; i++)
	    {
 	      if (!(strcmp (Args.arg[i], ".")) || (-1 == Uremove (Args.arg[i])))
		{
		  sprintf (buf, "\t%s does not exist.\n", Args.arg[i]) ;
		  Writeterm (buf, strlen(buf), TermNum) ;
		}
	      sprintf (buf, "\t%s deleted.\n", Args.arg[i]) ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	    }
	  break ;

	case CFORMAT :
	  if (TermNum)       /* check to see if were on term 0 */
	    {
	      sprintf (buf, "\aThe disk can be formatted from Terminal 0 only.\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	      break ;
	    }
	  Uformat (INODESPACE) ;
	  break ;

	case CTYPE :
	  if (TermNum && !bLoggedIn)
	    {
	      sprintf (buf, "\aYou must be logged in to use this command!\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	      break ;
	    }
	  Ctype (TermNum, &Args) ;
	  break ;

	case CBYE :
	  if (TermNum)       /* check to see if were on term 0 */
	    {
	      sprintf (buf, "\aThe system can only be shut down from Terminal 0.\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	      break ;
	    }
	  sprintf (buf, "Shutting down the OS/0 file system!\n") ;
	  Writeterm (buf, strlen(buf), TermNum) ;
	  for (i = 1 ; i < 4 ; i++)
	    {
	      sprintf (buf, "\n\a\a\aNotice! The OS/0 file system is going down in 10 seconds!\n") ;
	      Writeterm (buf, strlen(buf), i) ;
	    }
	  Delay (10) ;

	  free (Cmd) ;
	  free (user) ;
	  free (prompt) ;
	  free (buf) ;
	  Terminate () ;
	  break ;

	  default :
	    if (strlen(Cmd))
	      {
		sprintf (buf, "\a\"%s\" is an invalid command!\n", Cmd) ;
		Writeterm (buf, strlen(buf), TermNum) ;
	      }
	    break ;
	}

    } 
} /* Ucommand */

/* int CmdToNum (char *, ARGPTR) --------------------------
 *
 *   Takes a string and converts it into arguments.
 *   The command goes in Args->arg[0] and all arguments go
 *   in Args->arg[i>0].
 *   The number of arguments is in Args->num.
*/
int CmdToNum (char Cmd [], ARGPTR Args) 
{
  int CmdNum = -1, i ;

  *strchr (Cmd, '\n') = ' ' ;
  Args->arg[0] = Cmd ;
  Args->num = 0 ;
  
      if (Args->arg[1] = strchr (Cmd, ' '))
	{
	  *(Args->arg[1]++) = '\0' ;
	  while (*Args->arg[1] == ' ')
	    Args->arg[1]++ ;
	  if ((-1 != (CmdNum = binsearch (Args->arg[0])))) 
	    {
	      i = 2 ;
	      while (i < MAXARGS && (Args->arg[i] = strchr (Args->arg[i-1], ' ')))
		{
		  *(Args->arg[i]++) = '\0' ;
		  while (*(Args->arg[i]) == ' ')
		    Args->arg[i]++ ;
		  i++ ;
		  if (++Args->num >= MAXARGS - 1) 
		    break ;
		}
	    }
	}
      else 
	{
	  Args->arg[0][strlen (Args->arg[0])-1] = '\0' ;
	  CmdNum = binsearch (Args->arg[0]) ;
	}

  return CmdNum ;
} /* CmdToArgs */


/* char *strchr (char *, char) ----------
 *
 *   Because some idiot forgot to include the strchr 
 *   function in the gnucc libraries I had to write it
 *   myself.  Arrrgg!
*/
char *strchr (char *str, char c)
{
  int i ;

  for (i = 0 ; i < strlen (str) ;i++)
      if (str[i] == c)
	return (str + i) ;

  return NULL ;
}
	
/* int binsearch (char*) ---------------
 * 
 *  Searches in the list of available commands
 *  for the command pointed to by word.  If it 
 *  finds it it returns an integer identifying it.
 *  Pretty cool, huh?
*/      
int binsearch (char *word)
{
  int cond, i ;
  int low, high, mid ;

  low = 0 ;
  high = NCMDS - 1 ;
  while (low <= high) 
    {
      mid = (low+high) / 2;
      if ((cond = strcmp (word, cmdnames[mid])) < 0)
	high = mid - 1 ;
      else
	if (cond > 0)
	  low = mid + 1 ;
	else
	  return mid ;
    }
  return -1 ;
} /* binsearch */

int Ctype (int TermNum, ARGPTR Args) 
{
  int i, n, fd ;
  char buf[256] ;

  if (Args->num == 0)
    {
      sprintf (buf, "\aThe usage for the type command is: \"type <filenames>\"\n") ;
      Writeterm (buf, strlen(buf), TermNum) ;
      return FALSE ;
    }

  for (i = 1 ; i <= Args->num ; i++)
    {
      if (!strcmp (Args->arg[i], "accounts") && TermNum)
	{
	  sprintf (buf, "\aThe account file can only be accessed from Terminal 0.\n") ;
	  Writeterm (buf, strlen(buf), TermNum) ;
	  continue ;
	}
      if (-1 == (fd = Uopen (Args->arg[i], 1)))
	{
	  sprintf (buf, "\a\"%s\" does not exist.\n", Args->arg[i]) ;
	  Writeterm (buf, strlen(buf), TermNum) ;
	  continue ;
	}
      sprintf (buf, "%s:\n", Args->arg[i]) ;
      Writeterm (buf, strlen(buf), TermNum) ;
      while (n = Uread (fd, buf, 255)) 
	{
	  buf[n] = '\0' ;
	  Writeterm (buf, strlen(buf), TermNum) ;
	}
      Uclose (fd) ;
      Writeterm ("\n", 1, TermNum) ;
    }
  return TRUE ;
} /* Ctype */
/* ----- end of command.c ------ */


