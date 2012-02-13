/*        mail.c  - mail routines for OS/0 

These routines make up the OS/0 mail system.

*/

#include <stdlib.h>
#include "command.h"
#include "mail.h"

extern bEcho ;
extern Our_Root ;

/* int Madduser (int, ARGPTR) ----------------------------
 *
 *  This function allows a user on Terminal #0 to add a user
 *  to the "accounts" file.  
 *  If the user is already in the accounts file an error
 *  message will be printed.
 *
 *  The name of the user to be added is in Args->arg[1].
 *
 *  RETURNS TRUE if ok, FALSE otherwise
*/
int Madduser (int TermNum, ARGPTR Args) 
{
  char buf [80] ;
  int fAccounts, n ;
  char name [MAXUSERNAME], *p ;


  if (TermNum)       /* check to see if were on term */
    {
      sprintf (buf, "\x07Users can only be added to the system using Terminal 0.\n") ;
      Writeterm (buf, strlen(buf), TermNum) ;
      return FALSE ;
    }

  if (Args->num != 1)  /* make sure we have a username */
    {
      sprintf (buf, "\x07The usage for the adduser command is \"adduser <username>\".\n") ;
      Writeterm (buf, strlen(buf), TermNum) ;
      return FALSE ;
    }

  /* Open the accounts file.   */
  /* If it doesn't exist then create it. */
  if (-1 == (fAccounts = Uopen ("accounts", 1))) 
    {
      sprintf (buf, "Unable to open \"accounts\" file...creating it now.\n") ;
      Writeterm (buf, strlen (buf), TermNum) ;
      if (-1 == (fAccounts = Uopen ("accounts", 2)))
	{
	  sprintf (buf, "Error: Unable to create \"accounts\" file.\n") ;
	  Writeterm (buf, strlen (buf), TermNum) ;
	  return FALSE ;
	}
      Uclose (fAccounts) ;
      fAccounts = Uopen ("accounts", 1) ;
    }

  /* Go through accounts file looking for Args->arg[1] */
  /* if we find him then there is no need to add him is there */
  while (n = Uread (fAccounts, name, MAXUSERNAME))
    {
      if (n < MAXUSERNAME)
	  break ;

      *strchr (name, ' ') = '\0' ;
      if (!strcmp (name, Args->arg[1]))
	{
	  sprintf (buf, "\x07%s already has an account.\n", name) ;
	  Writeterm (buf, strlen (buf), TermNum) ;
	  Uclose (fAccounts) ;
	  return FALSE ;
	}
    }
  Uclose (fAccounts) ;

  /* at this point we know the guy isnt in the accounts file */
  /* simply open the file again and append his name to the end */
  /* (pad it with spaces to make it 128 characters) */
  if (-1 == (fAccounts = Uopen ("accounts", 3)))
    {
      sprintf (buf, "Error: Unable to open \"accounts\" file.\n") ;
      Writeterm (buf, strlen (buf), TermNum) ;
      return FALSE ;
    }

  strcpy (name, Args->arg[1]) ;
  for (p = name+strlen (name) ; p < name + MAXUSERNAME ; p++)
    *p = ' ' ;

  if (-1 == Uwrite (fAccounts, name, MAXUSERNAME))
    {
      sprintf (buf, "Error: Unable to write to \"accounts\" file.\n") ;
      Writeterm (buf, strlen (buf), TermNum) ;
      Uclose (fAccounts) ;
      return FALSE ;
    }

  sprintf (buf, "An account for %s has been set up.\n", Args->arg[1]) ;
  Writeterm (buf, strlen(buf), TermNum) ;
  Uclose (fAccounts) ;
  return TRUE ;
}  /*end of Madduser */


/* int Mlogin (int, char*, ARGPTR) -------------------------------
 *
 * This routine is called with the name of the user to be logged in
 * stored in Args->arg[1].  
 * We assume that no user is logged in before calling this function.
 * In other words, check before calling.
 *
 *  The name of the user is returned in the string user.
 *  If there were any errors logging the user in this
 *  routine returns FALSE.
 *
*/
int Mlogin (int TermNum, char *user, ARGPTR Args) 
{
  char buf [80] ;
  int fp, fAccounts, n ;
  char name [MAXUSERNAME] ;

  if (Args->num != 1)
    {
      sprintf (buf, "\x07The usage for the login command is \"login <username>\".\n") ;
      Writeterm (buf, strlen(buf), TermNum) ;
      return FALSE ;
    }

  if (-1 == (fAccounts = Uopen ("accounts", 1)))
    {
      sprintf (buf, "Error: Unable to open \"accounts\" file.\n") ;
      Writeterm (buf, strlen (buf), TermNum) ;
      return FALSE;
    }
  
  /* Search for user in accounts file */
  while (n = Uread (fAccounts, name, MAXUSERNAME))
    {
      if (n < MAXUSERNAME)
	  break ;

      *strchr (name, ' ') = '\0' ;
      if (!strcmp (name, Args->arg[1]))
	{
	  sprintf (buf, "\"%s\" logged in at %d\n", name, Get_Time_of_Day()) ;
	  Writeterm (buf, strlen (buf), TermNum) ;
	  Uclose (fAccounts) ;
	  if (-1 != (fp = Uopen (Args->arg[1], 1)))
	    {
	      sprintf (buf, "You have mail\n") ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	      Uclose (fp) ;
	    }
	  /* if we find him then return TRUE */
	  strcpy (user, Args->arg[1]) ;
	  return TRUE ;
	}
    }

  sprintf (buf, "\x07\"%s\" is not allowed on this system.\n", Args->arg[1]) ;
  Writeterm (buf, strlen(buf), TermNum) ;
  Uclose (fAccounts) ;
  return FALSE ;
} /* end of Mlogin */


/* int Mlogout (int, char *, ARGPTR) ---------------------------
 *
 * Log a user out.  Delay Args->arg[1] seconds.
 *
*/
int Mlogout (int TermNum, char *user, ARGPTR Args) 
{
  char buf[80] ;
  int t ;

  t = atoi (Args->arg[1]) ;

  sprintf (buf, "\"%s\" logged out at %d\n", user, Get_Time_of_Day ()) ;
  Writeterm (buf, strlen(buf), TermNum) ;

  if (t)
    {
      sprintf (buf, "This terminal will remain inactive for %d seconds...\n", t) ;
      Writeterm (buf, strlen(buf), TermNum) ;
      Delay (t) ;
    }

  return TRUE ;
} /* end of Mlogout */


/* int MMail (int, char *, ARGPTR) -------------------------------
 *
 * Check/Send mail.
 * 
 * if Args->num == 0 then the user simply wants to check his mail
 * otherwise Args->arg[1..num] contains the names of users who 
 * are to receive mail from the user.
 *
 * Mail is stored in a disk file with the name of the user.
 * Each message is pre-formatted and simply appended to the
 * end of the file.
*/
int MMail (int TermNum, char *user, ARGPTR Args) 
{
  int fp, n, i ;
  char buf [80] ;
  char *whoto ;
  char *sb ;
  char *msg, *line ;

  msg = (char*) malloc (sizeof(char) *MSGSIZE) ;
  sb  = (char*) malloc (sizeof(char) *SUBJECTLEN) ;
  whoto = (char*) malloc (sizeof (char) * MAXUSERNAME*MAXARGS) ;

  if (-1 == (fp = Uopen (user, 1)))
    {
      sprintf (buf, "No mail\n") ;
      Writeterm (buf, strlen(buf), TermNum) ;
    }
  else
    {
      sprintf (buf, "You have mail!\n") ;
      Writeterm (buf, strlen(buf), TermNum) ;
      while (n = Uread (fp, msg, MSGSIZE))
	{
	  if (n == -1)
	    {
	      sprintf (buf, "Error: Cannot read from \"%s\"'s mail box.\n", user) ;
	      Writeterm (buf, strlen(buf), TermNum) ;
	      Uclose (fp) ;
	      free (msg) ;
	      free (sb) ;
	      free (whoto) ;
	      return FALSE ;
	    }
	  Writeterm (msg, n, TermNum) ;
	}
      sprintf (buf, "\nNo more messages.\n") ;
      Writeterm (buf, strlen(buf), TermNum) ;
      Uclose (fp) ;
      Uremove (user) ;
    }

    /* This is where mail is sent. */
    /* first we get a list of all the users the message is to */
    /* then we get a subject from the user.
    /* then we let the user compose the message */
  if (Args->num)
    {
      strcpy (whoto, "") ;
      for (i = 1 ; i <= Args->num ; i++) 
	sprintf (whoto, "%s%s,", whoto, Args->arg[i]) ;
      *(whoto + strlen(whoto) - 1) = '\0' ;  /* nuke last comma */
      sprintf (buf, "Subject: ") ;
      Writeterm (buf, strlen(buf), TermNum) ;
      for (i = 0 ; i < SUBJECTLEN ; i++)
	sb[i] = '\0' ;
      Readterm (sb, SUBJECTLEN, TermNum) ;
      sprintf (buf, "Enter your message below.\n") ;
      Writeterm (buf, strlen(buf), TermNum) ;
      sprintf (buf, "  Terminate message by typing a '.' in column 1 of a line.\n") ;
      Writeterm (buf, strlen(buf), TermNum) ;
      for (i = 0 ; i < MSGSIZE ; i++)
	msg[i] = '\0' ;
      sprintf (msg, "------------------------------\n") ;
      sprintf (msg, "%s     To: %s\n   From: %s\n Posted: %d\nSubject: %s\n", 
	             msg, whoto, user, Get_Time_of_Day(), sb) ;
      Writeterm (msg, strlen(msg), TermNum) ;
      line = msg ;
      while ((strlen (msg) < MSGSIZE) && (line[0] != '.'))
	{
	  line = msg + strlen(msg) ;
	  Writeterm (">", 1, TermNum) ;
	  n = Readterm (line, LINESIZE, TermNum) ;
	  if (bEcho) 
	    Writeterm (line, n, TermNum) ; 
	}
      if (line[0] == '.') 
	line[0] = ' ' ;
      sprintf (buf, "\nSending %d byte message to\n", strlen(msg)+1) ;
      Writeterm (buf, strlen(buf), TermNum) ;
      for (i = 1 ; i <= Args->num ; i++) 
	{
	  sprintf (buf, "\t\t%s...\n", Args->arg[i]) ;
	  Writeterm (buf, strlen(buf), TermNum) ;
	  if (-1 == (fp = Uopen(Args->arg[i], 3)))
	    {
	      if (-1 == (fp = Uopen (Args->arg[i], 2)))
		{
		  sprintf (buf, "Error: Cannot open \"%s\"'s mail box\n", Args->arg[i]) ;
		  Writeterm (buf, strlen (buf), TermNum) ;
		  free (msg) ;
		  free (sb) ;
		  free (whoto) ;
		  return FALSE ;
		}
	    }

	  if (-1 == Uwrite (fp, msg, strlen(msg)+1))
	    {
	      sprintf (buf, "Error: Could not write to \"%s\"'s mail box\n", Args->arg[i]) ;
	      Writeterm (buf, strlen (buf), TermNum) ;
	      Uclose (fp) ;
	      free (msg) ;
	      free (sb) ;
	      free (whoto) ;
	      return FALSE ;
	    }

	  Uclose (fp) ;
	} /* for */
    }
  free (msg) ;
  free (sb) ;
  free (whoto) ;
  return TRUE ;
      
} /* MMail */


/* void Mlist (int) ----------------------------
 *
 *   Simply lists the names of all users who can access the
 *   system.
 *
*/
void Mlist (int TermNum)
{
  int fAccounts, n ;
  char buf [80] ;
  char name [MAXUSERNAME] ;

  if (-1 == (fAccounts = Uopen ("accounts", 1)))
    {
      sprintf (buf, "Error: Unable to open \"accounts\" file.\n") ;
      Writeterm (buf, strlen (buf), TermNum) ;
      return ;
    }

  sprintf (buf, "List of users:\n") ;
  Writeterm (buf, strlen(buf), TermNum) ;

  while (n = Uread (fAccounts, name, MAXUSERNAME))
    {
      if (n < MAXUSERNAME)
	  break ;

      *strchr (name, ' ') = '\0' ;
      sprintf (buf, "\t%s\n", name) ;
      Writeterm (buf, strlen(buf), TermNum) ;
    }

  Uclose (fAccounts) ;
} /* MMlist */


/* -------------- end of mail.c --------------- */
