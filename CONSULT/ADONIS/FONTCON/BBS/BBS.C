/*

 Revision History:
    9/9/92  ckindel Added lpszDatabase, which points to the directory
                    name of the database dir.  For ExeTypes with the high
                    bit (0x8000) set we point this to fs/demodata, otherwise
                    it points to fs/database.
	8/19/92 beng 	removed some debugging printfs for COMMIT_SUBSCRIPTION
					and OpenFile, also removed some ShowOrder()'s

	8/17/92 beng 	changed setting of orderstatus for COMMIT_SUBSCRIPTION
					added OpenIngres() call to msg MSG_GET_NEW_USER_ID for
					compatibility with WS v1.2

	7/30/92 beng 	commented out/changed some printfs for child exit
					status' and ShowOrder and ShowCust

	7/27/92 beng 	Added GetSystemStatus stuff, changed flow so that
					a database connection is not established until
					MSG_BEGIN_SESSION is received.  EOT is sent to the
					front end if Ingres is unavailable ( and the BBS 
					knows it before attempting connection ).

	7/22/92 beng 	Changed printing of modem result codes.

	7/20/92 beng 	Added subscription types to ShowOrder()

	7/20/92 beng 	Removed bWSSyncFix, bWSSaidHi shit

	7/15/92 beng 	Experimental stuff in Receive_Message():
						Send NAK after unequal messages,
						Hang up after 100 unequal msg's.

	7/15/92 cek 	New message MSG_SET_MEDIA_PREF and modified MSG_NEW_GET_NE
					W_USER_ID

	7/15/92 beng	Added comments, fixed diagnostic printfs to wrap better

	7/14/92 beng	Added comments, added a sleep() to openWSport

	6/25/92	beng	added device access check ( CheckAccess () )
					and file locking ( to prevent two bbs's from
					trying to use the same port )

					Added module fileutil.c to project.

	6/24/92	beng	added ModemInfo struct and modem config files

	6/24/92	beng	extended support for free download of font styles and faces

	6/22/92	beng	Added new Msg: MSG_GET_NEW_LIMITED_DEMO_USERID	

    6/19/92	beng	Rearranged order of MSG_COMMIT_RENEWAL, SUBSCRIPTION and
					NEWUSER.

					Cleaned up compiler warning msg's

   6/18/92  cek   Updated messages.h.  Added MODEM.INI and FCHELP.HLP to
				  executable list.

   6/17/92	beng	removed bAuthorized global, moved MSG_NEW_USERID_OK within
					switch statment, it was falling through to the wrong place!.

   6/17/92	beng	convert new ASSIGNED status value to WS_AUTH for front-end

   6/16/92	beng	added/modified printf's in openWSPort ()

   5/17/92  cek   CRC stuff. 

   5/10/92  cek   Major re-org.  Added tons of FS code and documented
                  messages big time.

   4/20/92  cek   Zmodem
   4/10/92  cek   Started adding FontShopper Support

 Notes:
   The messages that the bbs responds to are now documented
   in this file.  Before each message 'case' in the main
   loop below there is a documentation block that has the following
   format:

       * Description of message and any pertenent notes
       *
       * Class:      What message class does this message fall into
       * Flags:      Special message flags (see messages.h)
       * SendBuf:    The type (i.e size) of the buffer the shopper sends to
       *             the bbs.
       * ReceiveBuf: The type (i.e. size) of the buffer the bbs
       *             sends back to the shopper.

   If you add new messages, or modify existing ones, please
   document them carefully.

*/

#include "stdinc.h"
#include "stdcomm.h" 
#include "messages.h" 
#include <stdio.h>
#include "bbs.h"
#include "bbstruct.h"
#include "comm.h"
#include "xmodem.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termio.h>
#include <fcntl.h>
#include <prototypes.h>
#include "signal.h"
#include <string.h>
#include <id.h>
#include <tymnet.h>
#include <netcomm.h>
#include "strparse.h"
#include <setjmp.h>
#include "dirtree.h"
#include <ctype.h>
#include "modem.h"
#include <time.h>
#include <sys/dir.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fileutil.h>
#include <bbsstat.h>


/* (cek 4/10/92)
 * Define function prototypes for functions that are unix functions,
 * but do not appear to have an include file.
 */
int getpid() ;
int fork() ;
int setpgrp() ;
int open( char *path, int oflag ) ;
int stat( char *path, struct stat *buf ) ;
int fstat( int fildes, struct stat *buf ) ;


#ifndef ACCTH
#include  "acct.h"
#endif
#include  "acct_pro.h"

/*******************************************************************
 * Common directory structure and filenames
 *******************************************************************/

#define MODEM_CONFIG_PATH "/usr/bbs/master.bin/modems"

/*******************************************************************
 * ClipArt Shopper directory structure and filenames
 *******************************************************************/
#if 0
#define CLIPSDIR "/usr/bbs/master.bin/clips"
#else
#define CLIPSDIR "/shopper/clips"
#endif

/*******************************************************************
 * Font Shopper directory structure and filenames
 *******************************************************************/

/* The EXEDIR contains all of the latest execuatables in LZExpand()
 * compressed form.  The filenames are of the form ABCDEFGH.XXX where
 * XXX is a 3 digit DECIMAL number that identifies the file version.
 *
 * This directory also contains the database.lst file which is generated
 * in CalcFSDatabaseCRC().
 */
#define FONTSHOPEXEDIR     "/shopper/fs"
#define FSDBLIST_FNAME     "/shopper/fs/database.lst"

/* The DBDIR contains all fontshopper "local" database files.  The
 * root of DBDIR contains the database files (extensions are HEX
 * values that identify versions...only latest versions can be in
 * the dir!).
 * 
 * For each font publisher in the database, there is
 * a subdirectory of the DBDIR whose name is a 3 digit HEX value
 * that represents the 12 bit publsiher ID (low 12 bits of Order.pubID).
 *
 * The publisher sub-dirs contain all of the .TTF files for that
 * publisher.  This is done because many publishers have TTF files
 * with the same name (i.e. SYMBOL.TTF).
 */
#define FONTSHOPDBDIR      "/shopper/fs/database"

/* The DEMODBDIR contains the "demo" database.  Not TTF files
 * are stored here, but al FSPUB, FS, and FF files for the
 * demo database are.
 */
#define FONTSHOPDEMODBDIR   "/shopper/fs/demodata"

/*
 * External variables
 */
extern int errno;

/* (cek 4/10/92)
 * Define function prototypes for functions our own local functions.
 */
int   openWSPort( void ) ;
void  ExitParent(int CondCode) ;
void  ExitChild(int CondCode) ;
int   Receive_Message(HANDLE hCom, MSGTYPE *msgP) ;
void  ShowCust(ADDRESS_STRUCT *CustInfo) ;
void  ShowShip(Shipcost_Ary CostMatrix);
void ShowOrder(ORDER_STRUCT *pCurOrder) ;

int   OpenIngres ( int * nStatus ) ;
int   GetPaddedBuf(HANDLE hCom, WORD nNumChars, char *szBuf,BOOL upperize);
int   GetFileName(HANDLE hCom, WORD nNumChars, char *szFileName);
int   GetExt(char *szFileName);
int   OldGetExt(char *szFileName,char *dir);
BOOL  GetThatMoola(ORDER_STRUCT *pOrder, CCReq *pCCReq, BOOL bCheckLocalCredit);
void  ReceiveThatOrder(ORDER_STRUCT *pOrder);
WORD  CheckPassWord ( DWORD userID, char* szCheckPassWord) ;
static void Cleanup(int CondCode);

BOOL GetFSExeVersions( char *rgszFSExe[], WORD rgwFSExeID[], int n ) ;
DWORD CalcCRC32( unsigned char * lpBuf, int n ) ;
DWORD CalcFSDataBaseCRC() ;


/* (cek 4/20/92)
 * Prototypes for functions in other files.  No need for
 * an include file...
 */
BOOL ZmodemSendFiles( int iPort, PSTR pszFiles ) ; // zmwrap.c


/********************************************************************
 * Exported global variables.
 *
 * Note:  Are we sure these are exported?
 ********************************************************************/
//HANDLE cc_server;
DWORD    lCurUserID=0L,
         lSessionID=0L;

long     inbytes = 0;     /* To track data traffic */
long     outbytes = 0;

//to calc the LRC sum (see util.c in cash) for all database filenames to
// compare against the local database
DWORD    dwDataFileSum = 0;
WORD     wDataFileCount = 0;


/********************************************************************
 * Variables local to this module
 ********************************************************************/

/* (cek 5/10/92)
 * Font shopper uses a different method for calculating if the local
 * db is out of date then CS 1.2.  It uses a 16 bit CRC on a sorted
 * list of all the filenames.  This is much simpler than the "LRC"
 * method.
 */
static DWORD    dwFSDataBaseCRC ;

/*
 * Define the number of updatable executables in the product:
 *
 *    FONTSHOP.EXE
 *    UPSHOP.EXE
 *    SHOPSRV.EXE
 *    SHOPLIB.DLL
 *    FSDB.DLL
 *    FSMEMMAN.DLL
 *    ZMODEM.DLL  (or whatever it's called).
 *    MODEM.INI
 *    FCHELP.HLP
 */
#define EXE_FONTSHOP       0
#define EXE_UPSHOP         1
#define EXE_SHOPSRV        2
#define EXE_SHOPLIB        3
#define EXE_FSDB           4
#define EXE_FSMEMMAN       5
#define EXE_ZMODEM         6
#define EXE_MODEMINI       7
#define EXE_HELPFILE       8

static char *rgszFSExe[] =
      {
         "FONTCON.\0\0\0\0",
         "CONXUP.\0\0\0\0",
         "CONXSRV.\0\0\0\0",
         "CONXLIB.\0\0\0\0",
         "FCDB.\0\0\0\0",
         "CONXMEM.\0\0\0\0",
         "CONXPRO.\0\0\0\0",
		 "MODEM.\0\0\0\0",
		 "FCHELP.\0\0\0\0"
      } ;

#define NUM_FS_EXECUTABLES   (sizeof(rgszFSExe)/sizeof(rgszFSExe[0]))

static WORD rgwFSExeID[NUM_FS_EXECUTABLES] ;

/*
 * Local global variables (static)
 */
//This BOOL determines whether we ship a renewal package or not
static BOOL bRenewPkg = FALSE;
//This BOOL is set when child process is terminated because of INGRES failure
static BOOL bIngresAbort = FALSE;
static BOOL bIngresOpen  = FALSE ;

#define BADSIG (int (*)())-1
static void       HandleSig(int sig);
static int        nTimeout;
static void       SetSigHandler(void);
static HANDLE     GlobalhCom=(HANDLE)NULL,
                  hOrigCom=(HANDLE)NULL;

// which port to read from
#define WS_PORT GlobalhCom

static char       logfile[30];   /* For redirecting stderr and stdout */
static FILE*      plogfile;

static char       szLockFile[30];   /* For locking */
static int      nLockFile;

// Stores infomation about the current modem
MODEM_INFO_STRU ModemInfo ;


// NULL Password or 8 char szID 
#define NULL_ID_STR "\0\0\0\0\0\0\0\0"


/* (cek 4/18/92)
 * Added an extra byte to the following buffers 'cause ZmodemSendFiles
 * requires a doubly-null-terminated list of filenames
 */
static BYTE       RequestedFileName[STD_FILENAME_SIZE+2];
static BYTE       UptodateFileName[STD_FILENAME_SIZE+2];
static HANDLE     CurFilePtr = -1;
static ORDER_STRUCT CurOrder;
static CCReq      CurCCReq;
static ADDRESS_STRUCT CurCustInfo;
static char       CurPassword[PW_SIZE+1],CurUserID[PW_SIZE+1];
static WORD       WinVer,ExeType;
static int        parentPID,childPID=0;
static FILENODE*  FileTree=NULL;
static char*      p;
static time_t     clock;

/* (cek 5/6/92)
 * New message MSG_QUERY_ORDERID.  When an order is committed dwOrderID
 * is set.
 */
static DWORD      dwOrderID ;

/* (cek 5/6/92)
 * These flags are used throughout the code here to determine who
 * we're talking to.  if fShopSrv is TRUE then we are talking to a
 * ShopSrv based shopper product, otherwise it's ClipArt shopper 1.2.
 */
static BOOL       fShopSrv = FALSE ;
static BOOL       fFontShopper = FALSE ;

/* (cek 7/15/92)
 * New message (MSG_SET_MEDIA_PREF) because MSG_NEW_GET_NEW_USER_ID
 * requires that font con send two variables, and ConxSrv can only
 * send one item per message.
 */
static WORD gwMediaPreference = 0 ;

void main(int argc, char *argv[])
{

   MSGTYPE  msg;
   BOOL     PP9600;
   char     sztemp[5];

   time(&clock);

   printf("\n");
   printf("***************************************************************\n");
   printf("\n" );
   printf("   Font Shopper and ClipArt Shopper Enabled BBS is starting!\n" );
   printf("       BBS compilation time: %s %s\n", __DATE__, __TIME__ ) ;
   printf("\n" );

   /* Redirect stdout (printf's) and stderr to a logfile */
   // parse szLockFile name too -beng
   if (sscanf(argv[1], "/dev/tty%s", sztemp) != 1)
      printf("\nUnable to redirect logfile\n");
   else
   {
      sprintf(logfile, "%s/bbslog.%s", LOG_DIR, sztemp);
      sprintf(szLockFile, "%s/bbslock.%s", LOG_DIR, sztemp);
      printf("   Logging to %s\n", logfile ) ;
      printf("   To view output use 'tail -f %s' or the 'BA' command.\n\n", logfile);
   }

   /********************************************************************
    * Initialize ClipArt Shopper specific data.
    ********************************************************************/

   printf("Initializing ClipArt Shopper BBS\n") ;
   /* construct directory binary search tree */
   //printf("Calling MakeFileTree\n");
   if ((FileTree = MakeFileTree("./")) == NULL)
   {
      printf("** BBS Startup Failed ** : Error constructing directory tree.\n");
      exit(1);
   }

   printf("ClipArt Shopper LRC sum for database is %ld, based on %d database files\n", 
          dwDataFileSum, wDataFileCount);


   /********************************************************************
    * Initialize FontShopper specific data.
    ********************************************************************/

   printf("\nInitializing Font Shopper BBS\n") ;
   /* This func fills rgwFSExeID by looking in the
    * FontShopper executable directory for each of the file prefixes
    * given in rgszFSExe[].  It also appends the file version to the
    * end of the filename (i.e. in rgszFSExe[EXE_FONTSHOP], "FONTSHOP."
    * will turn into "FONTSHOP.042").
    */
   printf("   FontShopper Executable File Versions:\n");
   if (!GetFSExeVersions( rgszFSExe, rgwFSExeID, NUM_FS_EXECUTABLES ))
   {
      printf("** BBS Startup Failed ** : Error Getting FS Exe Versions!\n") ;
      exit(1) ;
   }

   if (!(dwFSDataBaseCRC = CalcFSDataBaseCRC()))
   {
      printf("** BBS Startup Failed ** : Could not calculate FS DataBase CRC\n" ) ;
      exit(1) ;
   }

   printf("   FontShopper Database CRC32 is 0x%08X\n", dwFSDataBaseCRC ) ;

   /********************************************************************
    * Start BBS                                                          
    ********************************************************************/
   savetty = argv[1];

	//lock access to the our lock file
	nLockFile = OpenLockFile ( szLockFile ) ;
	if ( NOT nLockFile )
	{
		printf ( "Can't lock Lock File %s, exiting.\n" , szLockFile ) ;
		exit ( 1 ) ;	
	}

	//Check for access to savetty
	if ( FAIL == CheckAccess ( savetty, ( R_OK | W_OK | F_OK ) ) )
	{
		printf ( "Can't access %s, exiting.\n" , savetty ) ;
		exit ( 1 ) ;
	}

   printf("Starting bbs on %s, at %s\n", savetty, ctime(&clock)); 

   //get modem configuration
   {
		char szPath [ 120 ] ;
		char * cp = NULL;

		if ( argc > 2 )
		{
			sprintf ( szPath, "%s/%s", MODEM_CONFIG_PATH, argv [ 2 ] ) ;
			cp = szPath ;
		}

		if ( OK != GetModemInfo ( cp, & ModemInfo ) )
			exit ( 1 ) ;
	}

   parentPID = getpid();

   #if 0
   // for continuous database connection
   if (Acct_Initialize() == FAIL)
   {   
      printf("** BBS Startup Failed ** : Can't CONNECT to database, exiting\n");
      exit(1);
   }
   else
   {
      //printf("Connected to database OK\n");
   }
   #endif

   printf("\n");
   printf("***************************************************************\n");

   /* Setup Logfile output */
   plogfile = freopen(logfile, "w", stdout);
   freopen(logfile, "a+", stderr);

   /* set unbuffered output */
   setbuf(stdout,NULL);
   setbuf(stderr,NULL);

/******

Past this point all output to stdout and stderr is re-directed to the log file

*******/

   //Header for the log file:
   printf("\n");
   printf("***************************************************************\n");

   printf("Starting bbs on %s, at %s\n", savetty, ctime(&clock)); 

   printf("\n");
   printf("***************************************************************\n\n");

   while(1)
   {
      SetSigHandler();  // child may have captured one.  Signals will be 
                        // inherited by child

      /* The fork necessary to get setpgrp() to work properly.  
         setpgrp() sets the group id to the value of the process id.  
         Thus setpgrp() will only work once for a given process.  
         Each child, however, has a unique process id, and 
         thus each child's call to setpgrp() is effective. */
      switch(childPID = fork()) 
      {
         int statusp;

         case 0: // child
            //printf("child forked\n");
            if ( openWSPort ( ) != OK ) // only the child opens the port
               ExitChild ( -3 ) ;

			// init global stuff
            ExeType = EXE_REGULAR;
            WinVer = 0x300;
            inbytes = outbytes = 0;
			CurUserID [ 0 ] = '\0' ;
			lCurUserID = lSessionID = 0L ;
			CurPassword [ 0 ] = '\0' ;
			nTimeout = 120 ;
		    if (CurFilePtr != -1)
            {
				close(CurFilePtr);
                CurFilePtr = -1;
            }


		   //Got MSG_SAY_HI as part of openWSPort, we respond here - beng
		   if (SendByte(WS_PORT,ACK) != OK)
			  printf("Got HI, Error sending ACK\n");
		   else
			  printf("Said Hi to ClipArt/Font Shopper\n");

         break; // into message loop

         case -1:
            printf("bbs: can't create new child process\n");
			ExitParent(1); // exit bbs

         default: // parent receives process id of child that died
		  {
			 int tmp ;

#ifdef COMMENT_OUT_BENG
            if (wait(&statusp) == -1)   //Wait here for child to die
               perror("bbs");
#else

         //If wait returns -1 it is because the parent got a signal, since
         //we handle signals elsewhere ( HandleSignal () ) we can ignore 
         //them here.
         // 5/27/92    beng

         while ( ( tmp = wait(&statusp) ) == -1)   //Wait here for child to die
            ;
         //printf("wait returned %d\n", tmp ) ;
#endif

         //printf("child returned status: 0x%X\n",statusp);

            if (WIFSIGNALED(statusp)) // then child died ignominiously
            {
               printf("CHILD was killed!!\n");
               ExitParent(-4);
            }
            else if (WIFEXITED(statusp))
            {
               //printf("WEXITSTATUS(status)  = 0x%X\n",WEXITSTATUS(statusp));
               if (WEXITSTATUS(statusp) == 0xff)
               {
                  /* assume -1 is INGRES exit status */
                  printf("CHILD exited by INGRES?\n");
                  bIngresAbort = TRUE;
                  Cleanup(-4);
               }
               //else
                  //printf("CHILD exited normally.\n");
            }
            continue; // to fork again
		  }

      } // switch( fork )

      /* we get here only if child */

      /* Seed random number generator with process ID for rare
         instance of new user and Window Shopper not in stock */

      srand(getpid());
   
      /* (cek 4/10/92)
       * The following while loop is the main message reception loop
       */
      while (1)
      {
         if (Receive_Message(WS_PORT,&msg) != OK)
         {
            printf("Client timed out.\n");
            ExitChild(-2);
         }

         switch(msg)
         {
            /* CS and FS send this immediatly after connecting to the bbs.
             * They also send it when trying to re-establish the connection
             * after line noise or whatever.
             *
             * Class:      MSGCLASS_GOIDLE
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_SAY_HI:
			  if (SendByte(WS_PORT,ACK) != OK)
				 printf("Error sending ACK\n");
			  else
				 printf("Said Hi\n");
            break;

            /* Sent by the shopper when it's going to hang up.
             * Basically shopper's just being nice.
             *
             * Class:      MSGCLASS_GOIDLE
             * SendBuf:    NULL
             * ReceiveBuf: NULL
             */             
            case MSG_HANGING_UP:
               printf("Client hanging up\n");
               ExitChild(0);
            break;

            /* This should be the 2nd message we get from the shopper
             * (after MSG_SAY_HI).  This is where we determine what kind
             * of shopper we're talking to.
             *
             * Class:      MSGCLASS_WAITACK
             * SendBuf:    WORD
             * ReceiveBuf: NULL
             */
            case MSG_SEND_EXETYPE:
               if (ReceiveBuf(WS_PORT,(BYTE *)&ExeType,
                              sizeof(ExeType),STD_WAIT) != OK)
               {
                  if (SendByte(WS_PORT,NAK) != OK)
                     ;
                  printf("Error getting ExeType:   %x\n",ExeType);
               }
               else
               {
                  /* (cek 5/10/92)
                   * The Timeout values that are set here appear
                   * completely arbitrary.  Someday someone needs to
                   * figure out what the best numbers are...
                   */
                  printf("Received ExeType: %x ",ExeType);

                  if (ExeType & 0x8000)
                  {
                     ExeType &= ~0x8000 ;
                     lpszDatabase = FONTSHOPDEMODBDIR ;
                  }
                  else
                  {
                     lpszDatabase = FONTSHOPDBDIR ;
                  }

                  switch(ExeType)
                  {
                     case EXE_BETA:
                        if (SendByte(WS_PORT,NAK) != OK)
                           ;
                        printf("(ClipArt Shopper Beta)\n");
                        
                        fShopSrv = FALSE ;
                        nTimeout = 120; // 2 minutes
                     break;

                     case EXE_REGULAR:
                        if (SendByte(WS_PORT,ACK) != OK)
                           ;
                        printf("(ClipArt Shopper Regular)\n");

                        fShopSrv = FALSE ;
                        nTimeout = 600; // ten minutes
                     break;

                     case EXE_INTERNAL:
                        if (SendByte(WS_PORT,ACK) != OK)
                           ;
                        printf("(ClipArt Shopper Internal)\n");

                        fShopSrv = FALSE ;
                        nTimeout = 600; // ten minutes
                     break;

                     case EXE_FONTSHOP_BETA:
                        if (SendByte(WS_PORT,ACK) != OK)
                           ;
                        printf("(Font Shopper Beta)\n");

                        fShopSrv = TRUE ;
                        fFontShopper = TRUE ;
                        nTimeout = 120; // 2 minutes
                     break;

                     case EXE_FONTSHOP_REGULAR:
                        if (SendByte(WS_PORT,ACK) != OK)
                           ;
                        printf("(Font Shopper Regular)\n");

                        fShopSrv = TRUE ;
                        fFontShopper = TRUE ;
                        nTimeout = 600; // ten minutes
                     break;

                     case EXE_FONTSHOP_INTERNAL:
                        if (SendByte(WS_PORT,ACK) != OK)
                           ;
                        printf("(Font Shopper Internal)\n");

                        fShopSrv = TRUE ;
                        fFontShopper = TRUE ;
                        nTimeout = 600; // ten minutes
                     break;

                     default:
                        if (SendByte(WS_PORT,NAK) != OK)
                           ;
                        printf("(Unknown EXE Type!)\n");

                        nTimeout=600 ;
                     break ;

                  }
               }
            break;

            /* New message for ShopSrv based shoppers.  The shopper is
             * asking us for an array of WORDs, where each element
             * is the file version of the latest executable component.
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:      MSGFLAGS_RECEIVECRC
             * SendBuf:
             * ReceiveBuf: WORD rgwFSExeID[NUM_FS_EXECUBABLES] 
             */
            case MSG_QUERY_EXEVERSIONS:
               if (fFontShopper)
               {
                  if (SendCRC( WS_PORT, rgwFSExeID,sizeof(rgwFSExeID) ) != OK)
                     ;

               }
               else
               {
                  printf("MSG_QUERY_EXEVERSIONS recieved and we're not talking to FontShop!\n" );
               }
            break ;

            /* The shopper sends this to set the CUSTINFO structure
             * which is essentially the customer's address.
             *
             * Class:      MSGCLASS_SENDDATAWAITACK
             * Flags:      MSGFLAGS_SENDCRC
             * SendBuf:    CUSTINFO struct
             * ReceiveBuf:
             */
            case MSG_SET_CUST_INFO:
               if (ReceiveCRC(WS_PORT,(BYTE *)&CurCustInfo, 
                              sizeof(CurCustInfo)) != OK)
               {
                  printf("Current CustInfo not received\n");
                  if (SendByte(WS_PORT,NAK) != OK)
                     ;
               }
               else
               {
                  /*
                     At this point we don't yet know whether this is a
                     new user.
                     Nor do we know the userID or password.

                     The sequence is (roughly):
                     1) receive this customer info
                     2) receive userID or generate a new one
                     3) if new user: 
                        a)  check whether new userID is OK
                        b)  receive password
                        c)  try to capture credit
                        d)  commit new user to INGRES
                     else not a new user:
                        a) obtain existing password
                     4) record that user's session has begun
                  */

                  //printf("Current CustInfo received\n");

                  if (SendByte(WS_PORT,ACK) != OK)
					  printf("Send ACK failed\n" ) ;

                  ShowCust ( & CurCustInfo ) ;
               }
            break;

            /* ClipArt shopper 1.2 only
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:     
             * SendBuf:
             * ReceiveBuf: WORD wAccountStatus
             */
            case MSG_QUERY_NEW_USERID_OK:
               /* (cek 4/10/92)
                * This varible is not used anywhere else in this
                * file and neither FontShopper or ClipArtShopper send this
                * message.  It should be removed.
				*
				*	6/17/92 - beng CLIP SHOP does use this msg
				*			and I removed bAuthorize
                */
				 // FALL THROUGH!!!!!

            /* Shopper uses this to query the account status.  The following
             * status' are possible:
             *  WS_NOEXIST doesn't exist
             *  WS_AUTH    been paid for but has not logged on, anonymous user
             *  WS_ACTIVE  been paid for and can purchase
             *  WS_EXPIRED done gone
             *  WS_AVAIL   on shelf, waiting to be used
			 *	ASSIGNED	purchased through adonis, customer records are
			 *				setup (new, 6/17/92 - beng)
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:     
             * SendBuf:
             * ReceiveBuf: WORD wAccountStatus
             */
            case MSG_QUERY_USERID_OK:
            {
               WORD Active, IDType ;
				  int nStat, nRetval ;

					nRetval = OpenIngres ( & nStat ) ;
					if ( nRetval == FAIL )
					{
					  SendByte ( WS_PORT, 
								(BYTE) ( nStat == SYS_DB_OFFLINE ? 
											EOT : NAK ) ) ;
						break ;
					}
               if ( Acct_CustIsActiveUserID ( lCurUserID, & Active,
															NULL, & IDType ) != OK )
                  Active = -1;

               printf("Query UserID returning: %d\n",Active);

					 // 6/17/92 - beng:
					 //	Font and WS do not know about ASSIGNED, WS_AUTH means the
					 //	same thing to the front end:
					 if ( Active == ASSIGNED )
					 {
						 printf ( "Converting ASSIGNED to WS_AUTH\n" ) ;
						 Active = WS_AUTH ;
					 }

					 // 8/19/92 - beng
					 // We now check whether the userID is of the right type,
					 // i.e. a FontCon userID is being used with a FontCon exe.

					 // MOREWORK make a seoerate function, add more IDTYPEs and
					 // Exetype bools (like fFontShopper) when more products are
					 // added

					switch ( IDType )
					{
						case IDTYPE_FONT_SHOP_WIN :
						{
							if ( fFontShopper )
								break;
							else
							{
								printf("Font Shopper logging in with ClipCon userID!");
								Active = WS_NOEXIST ;
							}
					 	}

						case IDTYPE_CLIP_SHOP_WIN :
						{
							if ( NOT fFontShopper )
								break ;
							else
							{
								printf("Font Shopper logging in with FontCon userID!");
								Active = WS_NOEXIST ;
							}
					 	}
					}

               if (SendBuf(WS_PORT,(BYTE *)&Active,sizeof(Active)) != OK)
                  printf("Error sending user Status\n");
            }
            break;

            /* Shopper is asking wether or not we ship a
             * package for a renewal.
             *
             * !!! QUESTIONABLE for FS !!!
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * SendBuf:
             * ReceiveBuf: BOOL bRenewPkg
             */
            case MSG_QUERY_SHIP_RENEWAL:
            {
               if (SendBuf(WS_PORT,(BYTE *)&bRenewPkg,sizeof(BOOL)) != OK)
                  printf("Error replying query ship renewal\n");
            }
            break;

            /* For ClipArt this returns a 'LRC' of all the files in the
             * database.  For FontShop this returns a CRC of all the
             * database filenames concatinated into a sorted list.
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * SendBuf:
             * ReceiveBuf: DWORD dwDatabaseCRC
             */
            case MSG_QUERY_DATA_LRC:
            {
               if (fFontShopper)
               {
                  if (SendBuf(WS_PORT,(BYTE *)&dwFSDataBaseCRC, 
                     sizeof(DWORD)) != OK)
                  printf("Error replying data LRC\n");
               }
               else
               {
                  if (SendBuf(WS_PORT,(BYTE *)&dwDataFileSum, 
                     sizeof(DWORD)) != OK)
                  printf("Error replying data LRC\n");
               }
            }
            break;

            /* record with INGRES that our user's session has begun.
             * It ends (gracefully) when we receive a HANGING_UP message. 
             *
             * Class:      MSGCLASS_WAITACK
             * SendBuf:
             * ReceiveBuf:
             */
				case MSG_BEGIN_SESSION:
				{
					 // here is where we will now connect to the
					 // database, instead of doing it at connect time
					 // We will first check the status of the system and make sure
					 // the database is online before attempting a connection.
					 // - beng 7/27/92

					 int nStat, nRetval ;

					 if ( lSessionID )
					 {
						  SendByte ( WS_PORT, ACK ) ;
							break ;
					 }

					 nRetval = OpenIngres ( & nStat ) ;
					 if ( nRetval == FAIL )
					 {
						  SendByte ( WS_PORT, 
									(BYTE) ( nStat == SYS_DB_OFFLINE ? 
												EOT : NAK ) ) ;
							break ;
					 }
					 else
					 {
							if ( ( Acct_NewID ( & lSessionID, ACCT_ID_SESSION, 
													 NULL ) != OK ) 
									 OR 
								  ( Acct_CustBeginSession ( lCurUserID, lSessionID,
																	 ExeType, WinVer ) != OK ) )
								SendByte ( WS_PORT, NAK ) ;
							else
							{
								SendByte ( WS_PORT, ACK ) ;

								 printf ( "Opened user (0x%08lX)(%ld) session (%ld)\n",
											  lCurUserID, lCurUserID, lSessionID ) ;
							}
					}
					 break ;
				 }

            /* Tells us the user's ID
             *
             * Class:      MSGCLASS_SENDDATA_WAITACK
             * Flags:      MSGFLAGS_SENDCRC
             * SendBuf:    MAX_PASSWORD chars
             * ReceiveBuf:
             */
            case MSG_SET_USER_ID:
            {
               char szTmp[PW_SIZE+1];

			   printf ( "Recieved MSG_SET_USER_ID\n" ) ;

               /* receive userID */
               if (ReceiveCRC(WS_PORT,szTmp,PW_SIZE) != OK)
               {
                  if (SendByte(WS_PORT,NAK) != OK)
                     ;
               }
               else
               {
                  if (SendByte(WS_PORT,ACK) != OK)
                     ;
                  szTmp[PW_SIZE] = '\0';
                  strcpy(CurUserID,szTmp);
                  lCurUserID = strtol(szTmp,NULL,16);
               }
            }
            break;


            /* Get a new userID from INGRESS (don't record it as
             * used yet, however).  Send it to client.
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:      MSGFLAGS_RECEIVECRC
             * SendBuf: 
             * ReceiveBuf: PW_SIZE bytes
				 *
				 * NOTE: retained for compatibility with WS v1.2 only
             */
            case MSG_GET_NEW_USER_ID:
            {
               DWORD NewUserID = 0L ;
					int	nStat, nRetval ;

               /* get a new unused userID from INGRES (don't record
                  it as used yet, however).  Send it to Client. */

					printf ( "Got msg MSG_GET_NEW_USER_ID:\t" ) ; 

					 //WS v1.2 sends this msg before sending the BEGIN_SESSION msg
					 // so we need to make sure the DB connection is open
					 nRetval = OpenIngres ( & nStat ) ;
					 if ( nRetval == FAIL )
						 printf ( "error opening ingres connection\n" ) ;
					 else
					 {
						 if ( Acct_NewID ( & NewUserID, ACCT_ID_USER, NULL ) != OK )
						 {
							 printf("error getting new userid\n");
						 }
					 }

					 lCurUserID = NewUserID ;

					 sprintf ( CurUserID, "%08lx", NewUserID ) ;

					 if ( SendCRC ( WS_PORT, CurUserID, PW_SIZE ) != OK )
						 ;
				}
				break;


            /* Get a new Limited Demo userID from INGRES (don't record it as
             * used yet, however).  Send it to client.
			 *
             * When this msg is called we assume the userID has already been 
			 *	set, with either a zero or a bundle ID.
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:      MSGFLAGS_RECEIVECRC
             * SendBuf: 
             * ReceiveBuf: PW_SIZE bytes
             */
            case MSG_GET_NEW_LIMITED_DEMO_USER_ID:
            {
               DWORD NewUserID;
			   WORD wIDType ;

				if ( fFontShopper )
					wIDType	 = IDTYPE_FONT_SHOP_WIN;
				else
					wIDType	 = IDTYPE_CLIP_SHOP_WIN;

               /* get a new unused userID from INGRES (don't record
                  it as used yet, however).  Send it to Client. */

			   //NOTE: we send the curuserid as the subsource
			   //parameter, because for a new limited demo user the userid
			   // identifies the bundle or subsource
               if ( Acct_NewLimitedDemoUserID ( & NewUserID, wIDType, 
												lCurUserID )  != OK)
               {
                  /* BANG!  This will cause shopsrv/bbs to
                   * loose connection.  We should send an
                   * "" user ID here and have the shopper detect that.
                   */

				   if (SendCRC(WS_PORT, NULL_ID_STR, PW_SIZE) != OK)
					  ;

                  printf("error getting new limited demo userid");
                  break;
               }

               lCurUserID = NewUserID;

               sprintf(CurUserID,"%08lX", NewUserID);

               printf("New Limited Demo userID: %s\n",CurUserID);

               if (SendCRC(WS_PORT,CurUserID,PW_SIZE) != OK)
                  ;
            }
            break;

            /* Used by FS to change the user's password in INGRESS. I don't
             * remember how CS uses it.
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:
             * SendBuf:
             * ReceiveBuf: BYTE bResult
             */
            case MSG_COMMIT_PASSWD:
               printf("Setting Password in Customer table\n");

               if (Acct_CustSetPassword(lCurUserID, CurPassword) == OK)
               {
                  if (SendByte(WS_PORT,ACK) != OK)
                     ;
               }
               else
                  if (SendByte(WS_PORT,NAK) != OK)
                     ;
            break;

            /* Set's the users address in INGRESS.
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:
             * SendBuf:
             * ReceiveBuf: BYTE bResult
             */
            case MSG_COMMIT_CUST_INFO:
               if (Acct_CustSetAddress(lCurUserID, &CurCustInfo) == OK)
               {
                  if (SendByte(WS_PORT,ACK) != OK)
                     ;
               }
               else
                  if (SendByte(WS_PORT,NAK) != OK)
                     ;
            break;

            /* Shopper sends the password with this...
             *
             * Class:      MSGCLASS_SENDDATA_WAITACK
             * Flags:      MSGFLAGS_SENDPAD
             * SendBuf:    PW_SIZE bytes.
             * ReceiveBuf:
             */
            case MSG_SET_PASSWD:
               /* receive password */

               if (GetPaddedBuf(WS_PORT,PW_SIZE,
                                CurPassword,FALSE) != OK)
               {
                  printf("Error getting password:   %s\n",CurPassword);
                  if (SendByte(WS_PORT,NAK) != OK)
                     ;
               }
               else
               {
                  if (SendByte(WS_PORT,ACK) != OK)
                     ;
                  printf("password:   %s\n",CurPassword);
               }
            break;

            /* CS uses this to check the users password.  FS uses this
             * only if the user forget's his password (not imp.)
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:      MSGFLAGS_RECEIVEPAD  | MSGFLAGS_RECEIVECRC
             * SendBuf:    
             * ReceiveBuf: PW_SIZE bytes
             */
            case MSG_QUERY_PASSWD:
            {
               int len;
               char szTmp[PW_SIZE+1];

               /** put password obtained from INGRES into CurPassword **/
               if (Acct_CustGetPassword(lCurUserID,CurPassword) == OK)
               {
                  strcpy(szTmp,CurPassword);
                  len = strlen(szTmp);
                  memset(szTmp+len,PADCHAR,PW_SIZE-len);

                  if (SendCRC(WS_PORT,(BYTE *)szTmp,PW_SIZE) != OK)
                     printf("Error sending password\n");
                  else
                     printf("password = %s\n",CurPassword);
               }
               else
                  printf("Error obtaining password (got %s)\n",CurPassword);

            }
            break;

            /* CS and FS use this to check to see if a new user's
             * subscription is ok.  
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:
             * SendBuf:
             * ReceiveBuf: BYTE bResult (ACK or NAK)
             */
            case MSG_QUERY_NEWUSER_OK:
               // Fallthrough!!!

            /* CS and FS use this to check to see if a renewal
             * subscription is ok.  
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:
             * SendBuf:
             * ReceiveBuf: BYTE bResult (ACK or NAK)
             */
            case MSG_QUERY_RENEWAL_OK: 
            {
               /* purchasing subscription online */

               //last param is to check local credit or not, if it is not
               // a renewal we have no local history so don't check it
               GetThatMoola ( & CurOrder, & CurCCReq, 
								( BOOL ) ( msg == MSG_QUERY_RENEWAL_OK ) ) ;

               /* WS will send a COMMIT message if all else completes OK */
            }
            break;

            /* CS uses this to check if a lib order is ok.
             * FS uses this to check if a collection order is ok.
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:
             * SendBuf:
             * ReceiveBuf: BYTE bResult (ACK or NAK)
             */
            case MSG_QUERY_LIB_ORDER_OK:
               GetThatMoola(&CurOrder, &CurCCReq, TRUE);
               /* WS will send a COMMIT message if all else completes OK */
            break;


            /* CS uses this to check if a clipart  order is ok.
             * FS uses this to check if a font face/style order is ok
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:
             * SendBuf:
             * ReceiveBuf: BYTE bResult (ACK or NAK)
             */
            case MSG_QUERY_CLIP_DLD_OK:
               if (GetThatMoola(&CurOrder, &CurCCReq, TRUE))
               {
                  /* record submission of order for delivery.  Assume 
                     this is tantamount to beginning the clipart download. 
                     (Makes sense since we've got their money now) */

                  CurOrder.flags |= OF_SUBMITTED;
                  Acct_NormalOrder ( &CurOrder, &CurCCReq ) ;
               }
               /* WS will send a COMMIT message if download completes OK */

            break;

            /* FS uses this message for all it's ordering.
             *
             * Class:      MSGCLASS_SENDDATA_WAITACK
             * Flags:      MSGFLAGS_SENDCRC
             * SendBuf:    ORDERSTRUCT
             * ReceiveBuf:
             */
            case MSG_SET_ORDER:
               // fall through!!!

            /* FS and CS sends a ORDER struct for a new user here.
             *
             * Class:      MSGCLASS_SENDDATA_WAITACK
             * Flags:      MSGFLAGS_SENDCRC
             * SendBuf:    ORDERSTRUCT
             * ReceiveBuf:
             */
            case MSG_SET_NEW_USER_ORDER:
               ReceiveThatOrder(&CurOrder);
			   ShowOrder ( & CurOrder ) ;
               //if (msg == MSG_SET_NEW_USER_ORDER)
               //    WriteBuf("ORDER.TXT",&CurOrder,sizeof(ORDER_STRUCT));
            break;

            /* Commits a subscription order 
			 *
             * After CS/FS have determined that the new user order
             * was ok (QUERY_) it commits it with this guy.
			 *
			 *	COMMIT_SUBSCRIPTION and COMMIT_RENEWAL
			 *	do the same thing, making the final call to NormalOrder () and
			 *	falling through to COMMIT_NEWUSER.
			 *
			 *	COMMIT_RENEWAL is maintained for
			 *	backward compatibility. After the release of WS 1.3 
			 *	COMMIT_RENEWAL should be removed. - beng
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:
             * SendBuf:
             * ReceiveBuf: BYTE bAckOrNak
             */

            case MSG_COMMIT_RENEWAL:
            case MSG_COMMIT_SUBSCRIPTION:
            {
               //Assumes CurOrder is Null if no subscription order in process

			  printf("%s, LibID: %ld, price: %u, media: %d\n",
					  msg == MSG_COMMIT_RENEWAL ? 
					  "MSG_COMMIT_RENEWAL" : "MSG_COMMIT_SUBSCRIPTION",  
					  CurOrder.libID, CurOrder.priceEach, CurOrder.mediaType);

			  if ( ( CurOrder.libID == WS_LIBID_NEW OR 
					 CurOrder.libID == WS_LIBID_UPGRADE OR
					 CurOrder.libID == FS_LIBID_NEW OR
					 CurOrder.libID == FS_LIBID_UPGRADE ) OR 
					 bRenewPkg )
			  {
				//order needs shipping	
				 CurOrder.flags |= OF_COMMITED;   
			  }
			  else         
			  {
				//No package, order considered shipped
				 CurOrder.flags |= OF_SHIPPED;
			  }

			 // printf ( "order flags = 0x%X\n", CurOrder . flags ) ;

			  if (Acct_NormalOrder ( &CurOrder, &CurCCReq ) != OK)
			  {
				 SendByte(WS_PORT,NAK);
				 printf("error committing subscription\n");
				 break;
			  }
			}   
			// FALL THROUGH!!!!

            /* After CS/FS have determined that the new user order
             * was ok (QUERY_) it commits it with this guy.
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:
             * SendBuf:
             * ReceiveBuf: BYTE bAckOrNak
             */
            case MSG_COMMIT_NEWUSER:
			{
				printf ( 
					"COMMIT_NEWUSER, userID: %lX, password: %s, LibID: %ld\n",
					 lCurUserID, CurPassword, CurOrder.libID ) ;

               if (Acct_CustNewUser(lCurUserID, CurPassword, CurOrder.libID, 
                                    &CurCustInfo)==OK)
               {
                  SendByte(WS_PORT,ACK);
                  break;
               }

               SendByte(WS_PORT,NAK);
               printf("error committing new user\n");
            }
            break;


            /* Commits a clipart order for CS and a face/style order
             * for FS.
             *
             * For CS 1.2 this guy also sends the order ID.  For ShopSrv
             * based shoppers the QUERY_ORDERID message is used.
             *
             * Class:      MSGCLASS_WAITACK
             * Flags:   
             * SendBuf:
             * ReceiveBuf: BYTE bAckOrNak
             */
            case MSG_COMMIT_CLIP_DLD:
               // Fallthrough!!!

            /* Commits a library order for CS and a collection order
             * for FS.
             *
             * For CS 1.2 this guy also sends the order ID.  For ShopSrv
             * based shoppers the QUERY_ORDERID message is used.
             *
             * Class:      MSGCLASS_WAITACK
             * Flags:   
             * SendBuf:
             * ReceiveBuf: BYTE bAckOrNak
             */
            case MSG_COMMIT_LIB_ORDER:
               if (msg == MSG_COMMIT_CLIP_DLD) // download succeeded
               {
                  printf("%s downloaded OK\n",
						CurOrder.prodType == CLIP ? "clipart" : 
						CurOrder.prodType == FONT_STYLE ? "font style" : 
						CurOrder.prodType == FONT_FACE ? "font face" : "???" 
						);

                  /* record delivery of clipart */
                  CurOrder.flags |= OF_SHIPPED;
               }
               else   // all go on lib order
               {
                  /* record placement of order */
                  CurOrder.flags |= OF_COMMITED;

                  printf("%s order placed\n",
						CurOrder.prodType == LIB ? "clipart library" :  
						CurOrder.prodType == FONT_COLLECTION ? 
											"font collection" : "???" 
						);

				  //MOREWORK send some mail or something to the person who has
				  //to complete this order
               }   

               if (Acct_NormalOrder ( &CurOrder, &CurCCReq ) == OK)
               {
                  SendByte(WS_PORT,ACK);

                  if (fShopSrv)
                     dwOrderID = CurOrder.orderID ;
                  else
                     SendBuf ( WS_PORT, (BYTE *) & CurOrder.orderID, 
								sizeof ( CurOrder.orderID ) ) ;
                  break;
               }

               SendByte(WS_PORT,NAK);
               printf("error placing order\n");
            break;

            /* New message for ShopSrv.  Get's Order ID.
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * Flags:     
             * SendBuf:
             * ReceiveBuf: DWORD dwOrderID
             */
            case MSG_QUERY_ORDERID:
			   printf( "Sending dwOrderID = 0x%08X\n", dwOrderID ) ;
               if (SendBuf( WS_PORT, (BYTE *)&dwOrderID, sizeof( dwOrderID ) ) != OK)
                   printf("Error sending order ID\n") ;

            break ;

            /* Sets the current filename (file to be downloaded).
             *
             * Class:      MSGCLASS_SENDDATA_WAITACK
             * Flags:      MSGFLAGS_SENDCRC | MSGFLAGS_SENDPAD
             * SendBuf:    STD_FILENAME_SIZE chars
             * ReceiveBuf:
             */
            case MSG_SET_CUR_FILENAME:
               if (fFontShopper)
               {
                  if (GetPaddedBuf(WS_PORT,STD_FILENAME_SIZE,
                                    RequestedFileName,TRUE) != OK)
                  {
                     printf("Error getting RequestedFileName:   %s\n",
                             RequestedFileName);
                     if (SendByte(WS_PORT,NAK) != OK)
                        ;
                  }
                  else 
                  {
                     if (SendByte(WS_PORT,ACK) != OK)
                        ;

                     //printf ( "RequestedFileName = %s\n", RequestedFileName ) ;
                  }
               }
               else
               {
                  if (GetPaddedBuf(WS_PORT,STD_FILENAME_SIZE,
                                    RequestedFileName,TRUE) != OK)
                  {
                     if (SendByte(WS_PORT,NAK) != OK)
                        ;
                     printf("Error getting RequestedFileName:   %s\n",
							 RequestedFileName ) ;

					if (CurFilePtr != -1)
					{
						close(CurFilePtr);
						CurFilePtr = -1;
					}
                  }
                  else 
                  {
                     char TmpFileName[ 120 ] ;

                     if (SendByte(WS_PORT,ACK) != OK)
                        ;
                     printf("Received RequestedFileName: %s\n", 
                              RequestedFileName);

					if (CurFilePtr != -1)
					{
						close(CurFilePtr);
						CurFilePtr = -1;
					}

                     /* the idea here is that we try to open any file that
                        has the same basename as RequestedFileName.  */
                     strcpy(UptodateFileName,RequestedFileName);

                     if (GetExt(UptodateFileName) == OK)
                     {
                        if ((UptodateFileName[0] == '0') AND 
                           (UptodateFileName[1] == '1'))
                           sprintf(TmpFileName,"%s/%s", CLIPSDIR, UptodateFileName);
                        else
                           strcpy(TmpFileName,UptodateFileName);

                        if ((CurFilePtr = open(TmpFileName,O_RDONLY)) == -1)
                        {
                           printf("(1)Unable to open %s\n",TmpFileName);
                           break;
                        }
                        else
                        {
                           //printf("Opened %s\n",UptodateFileName);
                           break;
                        }
                     }
                     else
                     {
                        printf("(5)Unable to open %s\n",UptodateFileName);
						CurFilePtr = -1 ;
                        break;
                     }
                  }
               }
            break;

            /* CS 1.2 uses Xmodem to download the current file.
             * FS uses Zmodem.
             *
             * Class:      MSGCLASS_ZMODEMRECEIVE
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_RECEIVE_FILE:
               printf("Sending %s...\n",UptodateFileName);

               if (!fShopSrv)
               {
                  if (XMSend(WS_PORT,CurFilePtr) != OK)
                     printf("Error sending file %s\n",UptodateFileName);
               }
               else
               {
                  char szFile[255] ;

                  memcpy(szFile, '\0', 255);

                  if (fFontShopper)
                  {
                     /* The directory structure for fontshopper is
                      * /shopper/fs/database
                      * /shopper/fs/database/XXX
                      *
                      * Where XXX is a hex (upper case) value that
                      * represents the 12 bit publisher ID for this
                      * file.  Note that since fontshopper only uses
                      * MSG_RECEIVE_FILE for downloading TTF files this
                      * is ok.
                      */
                     sprintf( szFile, "%s/FSPUB%03X/%s",
                              lpszDatabase,
                              CurOrder.pubID,
                              RequestedFileName );
                  }
                  else
                     sprintf( szFile, "%s", RequestedFileName ) ;

                  if (ZmodemSendFiles( WS_PORT, szFile ))
                  {
                     printf ("Zmodem send OK!\n") ;
                  }
                  else
                     printf( "Zmodem send BAD!\n" ) ;

               }
            break;

            /* CS 1.2 uses this justlike MSG_RECEIVE_FILE but FS uses
             * this to download an executable.  For FS, executables
             * are identified by an ID that is sent by this message
             * telling the bbs what file to download.
			 * 
			 * The beta version of font con uses wExeID | 0x8000 to
			 * request the appropriate .SYM file.
             *
             * Class:      MSGCLASS_ZMODEMRECEIVE_ID
             * Flags:
             * SendBuf:    WORD wExeID
             * ReceiveBuf:
             */
            case MSG_DOWNLOAD_EXE:
               if (fShopSrv)
               {
                  WORD wExeID ;
                  char szFile[255] ;

                  if (ReceiveBuf(WS_PORT,(BYTE *)&wExeID,
                                 sizeof(wExeID),STD_WAIT) != OK)
                  {
                     printf("Error getting executable ID:   %d\n",wExeID);
                     break;
                  }

                  /* Executables are stored in the rgszFSExe array where
                   * the index is wExeID.  Someday we'll store CS file
                   * names in rgszWSExe
                   */
                  if ((wExeID >= 0 && wExeID < NUM_FS_EXECUTABLES) ||
					  (ExeType == EXE_FONTSHOP_BETA))
                  {
                     memcpy(szFile, '\0', 255 ) ;

                     if (fFontShopper)
                        sprintf( szFile, "%s/%s", FONTSHOPEXEDIR, rgszFSExe[wExeID & 0x7FFF] );
                     else
                        sprintf( szFile, "foo" );

					if (wExeID & 0x8000) // .SYM file
					{
						char * p = strchr(szFile, '.') ;

						*(p+1) = 'S' ;
						*(p+2) = 'Y' ;
						*(p+3) = 'M' ;
					}

                     if (ZmodemSendFiles( WS_PORT, szFile ))
                     {
                        printf ("Zmodem send OK!\n") ;
                     }
                     else
                        printf( "Zmodem send BAD!\n" ) ;
                  }
                  else
                  {
                     /* Send the Zmodem abort sequence here...
                      */
                     printf("Executable ID out of range!\n") ;
                  }
               }
               else
               {
                  /* This is for CS 1.2 */

                  printf("Sending %s...\n",UptodateFileName);
                  if (XMSend(WS_PORT,CurFilePtr) != OK)
                     printf("Error sending file %s\n",UptodateFileName);
               }
            break;

            /* Fontshopper message.  Downloads the current
             * database.lst file.
             *
             * Class:      MSGCLASS_ZMODEMRECEIVE
             * Flags:
             * SendBuf:    
             * ReceiveBuf:
             */
            case MSG_DOWNLOAD_FCDBLST:
            {
               char szBuf[256] ;

               // need two nulls at the end
               memset( szBuf, '\0', 256 );
               strcpy( szBuf, FSDBLIST_FNAME ) ;
               if (ZmodemSendFiles( WS_PORT, szBuf ))
               {
                  printf ("Zmodem send OK!\n") ;
               }
               else
                  printf( "Zmodem send BAD!\n" ) ;
            }
            break;

            /* Fontshopper message.  Downloads the current file as found
             * in the database sub dir
             *
             * Class:      MSGCLASS_ZMODEMRECEIVE
             * Flags:
             * SendBuf:    
             * ReceiveBuf:
             */
            case MSG_DOWNLOAD_FCDBFILE:
            {
               char szBuf[256] ;

               memset( szBuf, '\0', 256 );
               sprintf( szBuf, "%s/%s", lpszDatabase, RequestedFileName ) ;
               if (ZmodemSendFiles( WS_PORT, szBuf ))
               {
                  printf ("Zmodem send OK!") ;
               }
               else
                  printf( "Zmodem send BAD!" ) ;
            }
            break ;

			case MSG_SET_MEDIA_PREF:
			   if (ReceiveBuf(WS_PORT,(BYTE*)&gwMediaPreference,sizeof(WORD),STD_WAIT) != OK)
			   {
			   printf("Error getting media preference.\n");
			   SendByte(WS_PORT, NAK);
			   break ;
			   }
			   else
				printf ( "Got media pref of %d\n", gwMediaPreference ) ;

			  SendByte(WS_PORT,ACK);
 			break;

            /************************************************************
             * All messages from here down are specific to ClipArt
             * Shopper 1.2 only.  FontShopper and other ShopSrv based
             * shoppers do not use these messages.
             ************************************************************/

            /* This message is sent by ClipArt Shopper 
			 *	it replaces MSG_GET_NEW_USER_ID from WS 1.2
			 * Font Con uses this!
			 * in conjunction with MSG_SET_MEDIA_PREF
             *
             * Class:      
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_NEW_GET_NEW_USER_ID:
            {

               /*
                This version of get NEW USERID requires the subtype (libid)
                and media for the new userid and calls a Acct_NewUserID()
                instead of Acct_NewID()
               */

               DWORD NewUserID;
               DWORD dwLibID;
               WORD  wMedia;

               if (fShopSrv == TRUE)
				 wMedia = gwMediaPreference ;
			   else
			   {
                 if (ReceiveBuf(WS_PORT,(BYTE *)&wMedia,
                              sizeof(wMedia),STD_WAIT) != OK)
                 {
                    printf("Error getting wMedia:   %d\n", wMedia);
                    SendByte(WS_PORT,NAK);
                    break;
                 }
				 }

               if (ReceiveBuf(WS_PORT,(BYTE *)&dwLibID,
                              sizeof(dwLibID),STD_WAIT) != OK)
               {
                  printf("Error getting Subscription LibID:   %ld\n", dwLibID);
                  SendByte(WS_PORT,NAK);
                  break;
               }

               printf("Request for new UserID for media %d and LibID %ld\n",
               wMedia, dwLibID);

               /* get a new unused userID from INGRES (don't record
               it as used yet, however).  Send it to Client. */
               if(Acct_NewUserID(&NewUserID, wMedia, dwLibID)  != OK)
               {
                  printf("error getting new userid\n");

					//Shop Serve wants null string to indicate data error
					//NAK to indicate comm error
					if ( fShopSrv )
					{
					   if (SendCRC(WS_PORT, NULL_ID_STR, PW_SIZE) != OK)
						  ;
					}
					else
					{
					  SendByte(WS_PORT,NAK);
					}

                  break;
               }
               lCurUserID = NewUserID;

               sprintf(CurUserID, "%08lX", NewUserID);
               //printf("New userID: %s\n",CurUserID);

               if (SendCRC(WS_PORT, CurUserID, PW_SIZE) != OK)
                  ;
            }
            break;

            /* CS 1.2 uses this to determine the shipping costs for
             * the current order.  FS stores shipping info in it's
             * local database, thus does not use this msg.
             *
             * Class:      The Shopsrv arch. does not support this msg.
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_QUERY_SHIPPING_COSTS:
            #if 0
            {
               Shipcost_Ary CostMatrix = {};
            }
            #else
            {
               /** assumes that CurOrder is set.  Query to INGRES
               and send the info on to WS. */

               Shipcost_Ary CostMatrix;

               if (Acct_ShippingCost(&CurOrder,CostMatrix) != OK)
               {
                  printf("unable to obtain shipping costs\n");
                  if (SendByte(WS_PORT,NAK) != OK)
                     ;
                  break;
               }
               else
               {
                  //printf("shipping costs:\n");
                  //ShowShip(CostMatrix);
                  if (SendByte(WS_PORT,ACK) != OK)
                     ;

                  if (SendCRC(WS_PORT,(BYTE *)CostMatrix,
                              sizeof(CostMatrix)) != OK)
                     ;
               }
            }
            #endif
            break;

            /* Message used by CS 1.2 only.
             *
             * Class:     
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_LATEST_VERSION:
            {
               #ifdef TEST_SERVER
               char buf[9];
               memset(buf,0,sizeof(buf));
               if (send_to_server(cc_server, "Hi to cc",8) == FAIL)
                  perror("cc_server");

               if (recv_from_server(cc_server, buf,8) == FAIL)
                  perror("cc_server");

               printf("cc_server returns: %s\n",buf);
               #endif

               //printf("Query file up-to-date (%s) (%s): ",
               //UptodateFileName,RequestedFileName);

               if (strcmp(UptodateFileName,RequestedFileName))
               {
                  /* file is not up-to-date */
                  //printf("no\n");
                  if (SendByte(WS_PORT,NAK) != OK)
                     ;
               }
               else
               {
                  //printf("yes\n");
                  if (SendByte(WS_PORT,ACK) != OK)
                     ;
               }
            }
            break;

            /* ClipArt Shopper 1.2 only.
             *
             * Class:      MSGCLASS_WAITACK
             * SendBuf:    WORD
             * ReceiveBuf: NULL
             */
            case MSG_SEND_WINVER:
               if (ReceiveBuf(WS_PORT,(BYTE *)&WinVer,
                              sizeof(WinVer),STD_WAIT) != OK)
               {
                  printf("Error getting WinVer:   %x\n",WinVer);
                  if (SendByte(WS_PORT,NAK) != OK)
                     ;
               }
               else
               {
                  //printf("Received WinVer:  %x\n",WinVer);
                  if (SendByte(WS_PORT,ACK) != OK)
                     ;
               }
            break;

            /* ClipArt Shopper Only.
             *
             * Class:      MSGCLASS_WAITRECEIVE
             * SendBuf: 
             * ReceiveBuf: WORD wSubType
             */
            case MSG_QUERY_SUBTYPE:
            {
               WORD nSubType;

               if (Acct_CustSubType(lCurUserID,&nSubType) != OK)
                  nSubType = -1;

               printf("QUERY SubType returning %d\n", nSubType);

               if (SendBuf(WS_PORT,(BYTE *)&nSubType,sizeof(nSubType)) != OK)
                  printf("Error sending user subtype\n");
            }
            break;

            /* CS sends a ORDER struct for clip art here.
             *
             * Class:      MSGCLASS_SENDDATA_WAITACK
             * Flags:      MSGFLAGS_SENDCRC
             * SendBuf:    ORDERSTRUCT
             * ReceiveBuf:
             */
            case MSG_SET_CUR_CLIP_DLD:
               // Fallthrough!!!

            /* CS sends a ORDER struct for library art here.
             *
             * Class:      MSGCLASS_SENDDATA_WAITACK
             * Flags:      MSGFLAGS_SENDCRC
             * SendBuf:    ORDERSTRUCT
             * ReceiveBuf:
             */
            case MSG_SET_CUR_LIB_ORDER:
               ReceiveThatOrder(&CurOrder);
			   ShowOrder ( & CurOrder ) ;
            break ;

            /* Not used by FS
             *
             * Class:      
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_QUERY_DEST_FILENAME:
            {
               char szDestName[STD_FILENAME_SIZE+1];
               int len;

               //printf("Query dest filename:");
               if (CurFilePtr == -1)
               {
                  memset(szDestName,PADCHAR,STD_FILENAME_SIZE);
                  if (SendCRC(WS_PORT,(BYTE *)szDestName,
                              STD_FILENAME_SIZE) != OK)
                     printf("Error sending dest filename\n");
                  break;
               }

               lseek(CurFilePtr,3L,SEEK_SET);
               if (read(CurFilePtr,szDestName,STD_FILENAME_SIZE) 
                           < STD_FILENAME_SIZE)
               {
                  //printf("  Unable to obtain.\n") ;
                  ;
               }
               else
               {
                  szDestName[STD_FILENAME_SIZE] = '\0';
                  //printf("  %s.\n",szDestName);
               }

               /* pad */
               len = strlen(szDestName);
               memset(szDestName+len,PADCHAR,STD_FILENAME_SIZE-len);

               if (SendCRC(WS_PORT,(BYTE *)szDestName,STD_FILENAME_SIZE) != OK)
                  printf("Error sending dest filename\n");

               /* add name to order struct */
               //if (p = strchr(szDestName,PADCHAR))
               //*p = '\0';
               //strcpy(CurOrder.clipFileName, szDestName);
            }
            break;

            /* Version 1.0 of FS does not support checking for stock.
             * A future version may.  CS 1.2 does use this.
             *
             * Class:      
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_QUERY_INSTOCK:
            {
               WORD pstatus=ACK;

               /* Get product availability if library or Client */
               if (Acct_CheckProdAvail ( &CurOrder, (char *) & pstatus ) != OK)
                  pstatus = FAIL;

               switch(pstatus)
               {
                  case CTRLX:
                     printf("Product out of stock\n");
                     CurOrder.flags |= OF_BACK_ORDERED;
                  break;

                  case SOH:
                     printf("Product not available\n");
                  break;
               }

               if (SendBuf(WS_PORT,(BYTE *)&pstatus,sizeof(pstatus))!= OK)
                  printf("Error sending In Stock info\n");
            }
            break;

            /* Used by CS 1.2.  FS get's file size from Zmodem and also
             * stores it in it's local databasel.
             *
             * Class:      
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_QUERY_CUR_FILESIZE:
            {
               WORD size = 0 ;

               printf ( "Query blocks:" ) ;

               if (CurFilePtr == -1)
               {
                  if ( SendBuf ( WS_PORT, (BYTE *) & size,
                     sizeof (size) ) != OK )

                  printf ( "Error sending size\n" ) ;
                  break ;
               }

               lseek ( CurFilePtr, 3L + TOTALBLOCKNUM_INDEX, SEEK_SET ) ;

               if ( read ( CurFilePtr, (char *) & size, 
							sizeof (size) ) < sizeof (size) )
			   {
				  size = 0 ;
                  printf ( "  Unable to obtain\n" ) ;
			   }
               else
				{
                  printf ( "  %d\n", size - 1 ) ;
				   --size ;
				}

               if ( SendBuf ( WS_PORT, (BYTE *) & size, sizeof (size) ) != OK )
                  printf ( "Error sending size\n" ) ;

				break;
            }

            /* CS 1.2 uses this.  FS stores this info locally.
             *
             * Class:      
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_QUERY_SUBSCR_PRICES:
            {
               WORD prices[3];
               FILE *fp, *fopen();

               printf("Query window shopper prices:");

               if ((fp = fopen("WSINFO","rt")) == NULL)
                  printf("Unable to open WSINFO\n");
               else
               {
                  if (fscanf(fp,"%*d %d %d %d", 
                             &prices[0], &prices[1], &prices[2]) != 3)
                     printf("Can't parse price info from WSINFO\n");
                  else
                     printf("%d %d %d\n", prices[0], prices[1], prices[2]);

                  fclose(fp);

                  if (SendBuf(WS_PORT, (BYTE *)prices, sizeof(prices)) != OK)
                     printf("Error sending prices\n");
               }
            }
            break;

            /* CS 1.2 uses this.  FS stores this info locally.
             *
             * Class:      
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_QUERY_SUBSCRIPTION_PRICE:
            {
               DWORD price;
               FILE *fp,*fopen();

               //printf("Query window shopper price:");
               if ((fp = fopen("WSINFO","rt")) == NULL)
                  printf("Unable to open WSINFO\n");
               else
               {
                  fscanf(fp,"%*d %ld",&price);
                  fclose(fp);

                  if (SendBuf(WS_PORT,(BYTE *)&price,sizeof(price)) != OK)
                     printf("Error sending price\n");
               }
            }
            break;

            /* CS 1.2 uses this.  FS stores this info locally.
             *
             * Class:      
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_QUERY_RENEWAL_PRICE:
            {
               char ctrlStr[80];
               DWORD price;
               FILE *fp,*fopen();

               //printf("Query window shopper price:");

               if ((fp = fopen("WSINFO","rt")) == NULL)
                  printf("Unable to open WSINFO\n");
               else
               {
                  fscanf(fp,"%*d %*ld %ld",&price);
                  fclose(fp);

                  if (SendBuf(WS_PORT,(BYTE *)&price,sizeof(price)) != OK)
                     printf("Error sending price\n");
               }
            }
            break;

            /* CS 1.2 uses this.  FS stores this info locally.
             *
             * Class:      
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_QUERY_UNCOMPRESSED_FILESIZE:
            {
               DWORD size;
               //printf("Query uncompressed filesize:");
               if (CurFilePtr == -1)
               {
                  size = 0L;

                  if (SendBuf(WS_PORT,(BYTE *)&size,sizeof(size)) != OK)
                     printf("Error sending size\n");

                  break;
               }

               lseek(CurFilePtr,3L + UNCOMPR_INDEX,SEEK_SET);

               if (read(CurFilePtr, (char*) &size,sizeof(size)) < sizeof(size))
                  //printf("  Unable to obtain.\n")
                  ;
               else
                  //printf("  %ld.\n",size)
                  ;

               if (SendBuf(WS_PORT,(BYTE *)&size,sizeof(size)) != OK)
                  printf("Error sending size\n");
            }
            break;

            /* CS 1.2 uses this.  FS stores uses MSG_QUERY_EXE_VERSIONS
             * instead.
             *
             * Class:      
             * Flags:
             * SendBuf:
             * ReceiveBuf:
             */
            case MSG_LATEST_EXE:
            {
               WORD Version,CurVersion;
               FILE *fp,*fopen();

               if (ReceiveBuf(WS_PORT,(BYTE *)&Version,
                              sizeof(Version),STD_WAIT) != OK)
               {
                  printf("Error getting Version:   %d\n",Version);
                  break;
               }
               else
                  printf("Received Version:  %d\n",Version);

               #if 0
               1/22/91
                  Have to exclude this for now: The older versions of WS (previous to 1.10) query
                  the EXE version before sending the WinVer, therefore WinVer is a random number,
                  users with old shoppers will probably not get new shoppers.  We have to take
                  this out and, unfortunatly, screw the Win21 user again.

               1/18/91 
                  For Win21 users we will ignore their shopper version so that
                  we don't have to maintain Win21 versions of shopper
                  we do this by telling versions of shopper running under 
                  Win21 that they are update irregardless. If the customer 
                  updates to Win30 they will be updated as always  
               if (WinVer < 0x300)
               {
                  printf("Win21 user, ignoring Shopper Version\n");
                  if (SendByte(WS_PORT,ACK) != OK)
                     ;
                  break;
               }
               #endif

               if ((fp = fopen("WSINFO","rt")) == NULL)
                  printf("Unable to open WSINFO\n");
               else
               {
                  fscanf(fp,"%d",&CurVersion);
                  fclose(fp);
                  if (Version < CurVersion)
                  {
                     /* exe is not up-to-date */
                     printf("out-of-date executable\n");
                     if (SendByte(WS_PORT,NAK) != OK)
                        ;
                  }
                  else
                     if (SendByte(WS_PORT,ACK) != OK)
                        ;
               }
            }
            break;

            default:
               printf("Received unknown message 0x%X\n",msg);
            break;
         }
      }
   }

} /* main() */


/****************************************************************
 *  int Receive_Message(HANDLE hCom, MSGTYPE *msgP)
 *
 *  Description: 
 *
 *    This function receives the two byte message from the comm
 *    port.  If the two bytes of the message do not match, this
 *    thing just trys again until a Timeout occurs.
 *
 *  Comments:
 *    I think it might be possible to be more robust about this.
 *    If we get unequal messages wouldn't it be nice to
 *    tell the client?
 *
 ****************************************************************/
int 
Receive_Message(HANDLE hCom, MSGTYPE *msgP)
{
   MSGTYPE msg2;
   int nBadCount = 0 ;

   while(1)
   {
      /* wait nTimeout seconds before timing out */
      if (ReceiveBuf(hCom, (BYTE*) msgP, sizeof(MSGTYPE), 
					 (WORD) nTimeout) != OK)
      {
         printf("Timed out waiting to recieve 1st msg, timeout = %d\n",
				nTimeout ) ;
         return FAIL;
      }

	  //we don't wait around (long) for this one
      if (ReceiveBuf(hCom, (BYTE*) & msg2, sizeof(MSGTYPE), 1) != OK)
         continue;

      /* redundancy for error checking */
      if (*msgP == msg2)
         return OK;
      else
	  {
         printf("Recieved unequal msg's: M1: 0x%X, M2: 0x%X\n", *msgP, msg2);

		 //EXPERIMENTAL!!!!!
		 if (SendByte(WS_PORT,NAK) != OK)
			;

		if ( ++nBadCount > 100 ) //????????? 
			return FAIL ;
	  }
   }

} /* Receive_Message()  */

/****************************************************************
 *  int GetPaddedBuf(HANDLE hCom, WORD nNumChars, char *szBuf, BOOL upperize)
 *
 *  Description: 
 *
 *    Some messages that come across the wire have 'padded'
 *    buffers associated with them.  This function takes one of those
 *    buffers and 'un-pads' it.  The pad char is '~'.
 *
 *  Comments:
 *    I can't figure out why the padding was needed in the first place.
 *
 ****************************************************************/
int GetPaddedBuf(HANDLE hCom, WORD nNumChars, char *szBuf, BOOL upperize)
{
    char *p;

    /* get buf */
    if (ReceiveCRC(hCom,szBuf,nNumChars) != OK)
    {
        printf("Error receiving padded buffer\n");
        return FAIL;
    }
    else
    {
        szBuf[nNumChars] ='\0';
        if (p = strchr(szBuf,PADCHAR))
            *p ='\0';
        if (upperize)
            for(p = szBuf; *p; ++p)
                *p = toupper(*p);
        return OK;
    }

} /* GetPaddedBuf()  */

int GetExt(char *szFileName)
{
   extern    BOOL IsRightFormat(char *szFileName);
   char *p;

   // remove extension if there is one
    if (p = strchr(szFileName,'.'))
        *p = '\0';

    if ((szFileName[0] == '0') AND (szFileName[1] == '1'))
   // look in clips directory (clips separated for speed on others)
      return OldGetExt(szFileName, CLIPSDIR);
    else
   {
      if (GetExtFromTree(FileTree,szFileName) != OK)
      {
         if (!IsRightFormat(szFileName))
            return OldGetExt(szFileName,"./");
         return(FAIL);
      }
      return OK;
   }
}


int
OldGetExt(char *szFileName,char *dir)
{
    struct directx
    {
        ino_t d_ino;
        char d_name[DIRSIZ+1];
    } dlink;
    int fd, nread;
    char *dname;

      if ((fd = open(dir,0)) == -1)
      {
         printf("(2)Error opening %s\n",dir);
         return FAIL;
      }

      dlink.d_name[DIRSIZ] = '\0';
	  //printf ( "Reading clipart directory...\n" ) ;
      while ((nread = read(fd,  (char*) &dlink, sizeof(struct direct))) == 
                           (sizeof(struct direct)))
      {
         if (dlink.d_ino)
            if (dname = strstr(dlink.d_name,szFileName))
            {
               strcpy(szFileName,dlink.d_name);
               close(fd);
			   //printf( "found %s\n", szFileName ) ;
               return OK;
            }
         dlink.d_name[DIRSIZ] = '\0';
      }
      close(fd);
      return FAIL;
}

static void
HandleSig(int sig)
{
    extern HANDLE Port;

   printf("%s got signal: ", childPID ? "PARENT" : "CHILD");

    switch(sig)
    {
        case SIGHUP:
         printf("HangUp\n");
         /* for some reason doesn't go to default, goes into infinate loop */
         if (signal(SIGHUP,SIG_IGN) == BADSIG)
            printf("Bad SIGHUP signal call\n");
         break;
        case SIGINT:
            printf("Interrupt\n");
        break;
        case SIGQUIT:
            printf("Quit\n");
        break;
        case SIGTERM:
            printf("Terminate\n");
        break;
    }

   if (childPID == 0) // then child
   {
      if (sig == SIGHUP)
         printf("WS hung up without saying goodbye\n");
      ExitChild(sig);
   }
    else /* killing bbs */
#ifdef COMMENT_OUT_BENG
      ExitParent(sig);
#else
   // parent should ignore the Hangup signal, which occurs if the shell 
   // which started the BBS is exited
   // when these other signals are recieved and what we should really do 
   // about them remains unknown
   // 5/27/92 - beng
   {
      if (sig != SIGHUP)
         ExitParent(sig);
      else
         printf ( "Ignoring parent hangup, (still working)\n" ) ;
   }
#endif
}

static void 
SetSigHandler ( void )
{
    if (signal(SIGHUP,HandleSig) == BADSIG)
        printf("Bad SIGHUP signal call\n");

    if (signal(SIGINT,SIG_IGN) == BADSIG) // ignore to disable del key
        printf("Bad SIGINT signal call\n");

    if (signal(SIGTERM,HandleSig) == BADSIG)
        printf("Bad SIGTERM signal call\n");

    if (signal(SIGQUIT,HandleSig) == BADSIG)
        printf("Bad SIGQUIT signal call\n");
}

int 
openWSPort ( void )
{
    int nRetval = OK;
    char buf;
   //use high DTE speed for pp9600 cause its buffered
    DWORD tmpbaud = ModemInfo . dwDTESpeed ;
	BOOL bHiSpeed = ( ModemInfo . dwDTESpeed > 2400 ) ; 
   char *psz, szResultBuf [ 2048 ] ;
   BOOL bSaidHi, bTerminate = FALSE;
	int count = 0 ;

   if (GlobalhCom)
   {
      HangUp(GlobalhCom,savetty, & ModemInfo );
      close(GlobalhCom);
      GlobalhCom = (HANDLE) NULL;
   }

   if (hOrigCom)
   {
      restore_tty(hOrigCom);
      close(hOrigCom);
   }

   if ( ( hOrigCom = open ( savetty, O_RDWR | O_NDELAY ) ) < 0 )
   {
      printf("(3)Error opening %s\n",savetty);
      END(FAIL)
   }

   save_tty(hOrigCom); // for restore_tty()

   /* in case these need resetting: */
   setraw(hOrigCom);
   set_tty_baud(hOrigCom, (int) tmpbaud);
   set_tty_local(hOrigCom,TRUE);

   /* initialize modem */
   WriteModemCommand(hOrigCom, ModemInfo . szModemInitString ) ;

	sleep ( 1 ) ;

   /* clear return values, make sure were not sending out any garbage either */
   flush(hOrigCom,2);

#if 0
   printf("Current modem profile:\n");
   WriteModemCommand(hOrigCom,"+~+~+~~~AT&V\r~~~~~");
   while (nRetval = read(hOrigCom,&buf,1))
   {
      if (buf != '\r')
         printf("%c", buf);
   }
   printf("\n");

   /* clear return values */
   flush(hOrigCom,2);
#endif

   /* so open will block: */
   set_tty_local(hOrigCom,FALSE);

   setpgrp(); // this causes the next opened tty to issue a
            // SIGHUP when the device closes (the next opened
            // device becomes the 'controlling' tty.

   printf("Waiting at the phone...\n");

   //This open call blocks until the carrier detect flag is raised
   if ( ( GlobalhCom = open ( savetty, O_RDWR ) ) < 0 )
   {
      printf("Open %s failed\n", savetty);
      END(FAIL)
   }

   time(&clock);
   printf("Connected at %s\n", ctime(&clock));

	//set_tty_local(WS_PORT,TRUE);

/*********** E S T A B L I S H   C O N N E C T I O N  ********************
   Read from the modem (WS_PORT) until the SAY_HI msg is recieved, everything
   preceding it is result codes from the modem or negotiation garbage.
   TimeToWait = 120 secs cause 9600 feature negotiation can take a while.
   Result codes from the modem are terminated with a CR.
   We fill up the result buffer until a CR is recieved, than we 
   try and interperate it and set the appropriate flags.
   We keep doing this until we get two consecutive SAY_HI msg's from Window
   Shopper or we fill up the buffer or Timeout waiting for the next character.
******************************************************************************/
   bSaidHi = FALSE;
   tmpbaud = 0L;
   while (1)
   {
      if (tmpbaud)   //tmpbaud got set via result code
      {
		  set_tty_baud(WS_PORT, (int) tmpbaud ) ;

		  set_tty_local(WS_PORT,FALSE);
		  set_tty_xclude(WS_PORT,TRUE);

		  tmpbaud = 0L ;
      }

      psz = szResultBuf;
      bTerminate = FALSE;

	  // read from port until buffer fills up, a entire result code is received
	  // or the front end has said HI
      while ( psz - szResultBuf < sizeof (szResultBuf) ) 
      {
         if (ReceiveBuf(WS_PORT, psz, 1, 60) != OK)
         {
            printf("Timed out reading from %s\n", savetty);
            END (FAIL)
         }
         if (*psz == '\r')	// end of modem result code, evaluate now
         {
            bTerminate = TRUE;
            break;
         }
         else if (*psz == MSG_SAY_HI)
         {
            if (bSaidHi)
			{
				printf ( "Got 2nd MSG_SAY_HI\n" ) ;
				END(OK)
			}
            else
			{
				bSaidHi = TRUE;
				printf ( "Got 1st MSG_SAY_HI\n" ) ;
			}
         }
         else
         {
            bSaidHi = FALSE;

            if (isalnum(*psz))
               printf("%c", *psz);
            else
               printf("0x%02X", (unsigned char) * psz);

			if (++count == 20 )
			{
				printf ( "\n" ) ;
				count = 0 ;
			}

            psz++;
         }
      }
      *psz = '\0';   //trim \r

      if ( NOT ( szResultBuf [ 0 ] AND bTerminate ) )
      {
         psz = szResultBuf;
         printf("Empty result buf or no terminate char:\n");

		count = 0 ;
         while (*psz)
         {
            if (isalnum(*psz))
               printf("%c", *psz);
            else
               printf("0x%02X", (unsigned char) * psz);
            ++psz;

				 if (++count == 20 )
				 {
					 printf ( "\n" ) ;
					 count = 0 ;
				 }
         }
         printf("\n");
         END (FAIL)
      }
         
      //Now interperate
      if (strcmp(szResultBuf, RC_OK) == 0)
      {
         printf("Modem sez OK\n");
         continue;
      } else   
      if (strcmp(szResultBuf, RC_ERROR) == 0)
      {
         printf("Modem sez ERROR\n");
         continue;
      } else   
      if (strcmp(szResultBuf, RC_RING) == 0)
      {
         printf ( "Result code: %s, RINNNGGGG!!!!\n",
					szResultBuf ) ;
         continue;
      } else   
      if (strcmp(szResultBuf, RC_CONNECT_300) == 0)
      {
         printf ( "Result code: %s, Connect at 300 %s\n", 
					szResultBuf,
					bHiSpeed ? "(DTE to DCE)" : "" ) ;

			// if bHighSpeed this result is the DTE-DCE speed, if not it is the 
			// actual carrier speed and we need to adjust to it
			if ( NOT bHiSpeed )
				 tmpbaud = 300L;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CONNECT_1200) == 0)
      {
         printf("Connect at 1200 %s\n", bHiSpeed ? "(DTE to DCE)" : "");

			// if bHighSpeed this result is the DTE-DCE speed, if not it is the 
			// actual carrier speed and we need to adjust to it
			if ( NOT bHiSpeed )
				 tmpbaud = 1200L;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CONNECT_2400) == 0)
      {
         printf ( "Result code: %s, Connect at 2400 %s\n", 
					szResultBuf,
					bHiSpeed ? "(DTE to DCE)" : "" ) ;

			// if bHighSpeed this result is the DTE-DCE speed, if not it is the 
			// actual carrier speed and we need to adjust to it
			if ( NOT bHiSpeed )
				 tmpbaud = 2400L;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CONNECT_4800) == 0)
      {
         printf ( "Result code: %s, Connect at 4800\n", szResultBuf ) ;
		 tmpbaud = 4800;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CONNECT_9600) == 0)
      {
         printf ( "Result code: %s, Connect at 9600\n", szResultBuf ) ;
		 tmpbaud = 9600;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CONNECT_14400) == 0)
      {
         printf ( "Result code: %s, Connect at 14400\n", szResultBuf ) ;
		 tmpbaud = 19200;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CONNECT_19200) == 0)
      {
         printf ( "Result code: %s, Connect at 19200 %s\n", szResultBuf ) ;
		 tmpbaud = 9600;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CONNECT_38400) == 0)
      {
         printf("Result code: %s, Connect at 38400\n", szResultBuf ) ;
		 tmpbaud = 38400;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CARRIER_300) == 0)
      {
         printf ( "Result code: %s, Carrier at 300\n", szResultBuf ) ;
         tmpbaud = 300;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CARRIER_1200) == 0)
      {
         printf ( "Result code: %s, Carrier at 1200\n", szResultBuf ) ;
         tmpbaud = 1200;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CARRIER_2400) == 0)
      {
         printf ( "Result code: %s, Carrier at 2400\n", szResultBuf ) ;
         tmpbaud = 2400;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CARRIER_4800) == 0)
      {
         printf ( "Result code: %s, Carrier at 4800\n", szResultBuf ) ;
         tmpbaud = 4800;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CARRIER_9600) == 0)
      {
         printf ( "Result code: %s, Carrier at 9600\n", szResultBuf ) ;
         tmpbaud = 9600;

         continue;
      } else   
      if (strcmp(szResultBuf, RC_CARRIER_14400) == 0)
      {
         printf ( "Result code: %s, Carrier at 14400\n", szResultBuf ) ;
         tmpbaud = 19200;	// UNIX doesn't support 14400, we depend on 
									// buffering and flow control

         continue;
      } else   
      if (strcmp(szResultBuf, RC_COMPRESSION_MNP5) == 0)
      {
         printf ( "Result code: %s, MNP/5 Data Compression enabled!\n",
					szResultBuf ) ;
         continue;
      } else   
      if (strcmp(szResultBuf, RC_COMPRESSION_V42BIS) == 0)
      {
         printf("Result code: %s, V.42bis Data Compression enabled!\n",
					szResultBuf ) ;
         continue;
      } else   
      if (strcmp(szResultBuf, RC_COMPRESSION_NONE) == 0)
      {
         printf("Result code: %s, Data Compression disabled!\n",
					szResultBuf ) ;
         continue;
      } else   
      if (strcmp(szResultBuf, RC_PROTOCOL_NONE) == 0)
      {
         printf("Result code: %s, standard async mode\n",
					szResultBuf ) ;
         continue;
      } else   
      if (strcmp(szResultBuf, RC_PROTOCOL_V42LAPM) == 0)
      {
         printf("Result code: %s, Modem in V.42 LAPM Error Control mode\n",
					szResultBuf ) ;
         continue;
      } else   
      if (strcmp(szResultBuf, RC_PROTOCOL_MNP) == 0)
      {
         printf("Result code: %s, Modem in MNP Error Control mode\n",
					szResultBuf ) ;
         continue;
      } 
      else   //BIG TROUBLE!!
      {
         char* cp = szResultBuf;
         printf("Got non-standard result code:\n");

		 count = 0 ;
         while ( * cp )
         {
            if (NOT isalnum(*cp))
               printf("0x%02X", (unsigned char) * cp ) ;
            else
               printf("%c", *cp);

            cp++;

			 if ( ++count == 20 )
			 {
				 count = 0 ;
				 printf ( "\n" ) ;
			 }
         }
         printf("\n");
         continue;   //got garbage, keep reading
      }
   }

	 end:

	 setblock(GlobalhCom,OFF);
	 
    if ( nRetval != OK )
    {
		  printf("(4)Error opening %s\n",savetty);

		  flush ( WS_PORT, 2 ) ;

		  if ( bHiSpeed )
		  {
				//MOREWORK this query command should be part of ModemInfo and
				//canned responses should be parsed from the modem's response
				// need a general function for reading/displaying chars read
				// from port
				printf("Asking modem for hangup reason (look up return value in modem manual):\n");
				WriteModemCommand(WS_PORT, "+~+~+~~~AT&S86?\r~~~");
				while (nRetval = read(WS_PORT,&buf,1))
				{
					int count = 0 ;

					 if ( isalnum ( buf ) )
						 printf ( "%c", buf ) ;
					 else
					 {
						  if ( ( buf == '\r' ) OR ( buf == '\n' ) )
								printf ( "\n" ) ;
						  else
								printf ( "0x%02X", (unsigned char) buf ) ;
					 }

					 if ( ++count == 20 )
					 {
						  count = 0 ;
						  printf ( "\n" ) ;
					 }
				}
				printf("\n");
		  }
		  set_tty_local ( WS_PORT, FALSE ) ;
		  nRetval = FAIL ;		//set this back
	 }

	 return nRetval;
}

void ShowCust(ADDRESS_STRUCT *pCustInfo)
{
    printf("Customer Address:\n");
    printf("   %s, ",pCustInfo->name);
    printf("   %s\n",pCustInfo->company);
    //printf("   %s, ",pCustInfo->street);
    //printf("   %s\n",pCustInfo->street2);
    printf("   %s, ",pCustInfo->city);
    printf("   %s  ",pCustInfo->state);
    printf("   %s, ",pCustInfo->postal_code);
    printf("   %s  \n",pCustInfo->province);
    printf("   Country: %d = ",pCustInfo->whichCountry);
    printf("   %s\n",pCustInfo->country);
    printf("   Phone: (%s)",pCustInfo->phone.area);
    printf("   %s - ",pCustInfo->phone.number);
    printf("   %s\n",pCustInfo->phone.ext);
}

void ShowOrder(ORDER_STRUCT *pCurOrder)
{
    printf("CurOrder:\n");

	printf("   Prod type   = %s\n", 
		pCurOrder -> prodType == CLIP ? "Clipart" :
		pCurOrder -> prodType == LIB  ? "Clipart Library" :
		pCurOrder -> prodType == WS  ?    "ClipCon Subscription" :
		pCurOrder -> prodType == FSHOP  ? "FontCon Subscription" :
		pCurOrder -> prodType == FONT_FACE  ? "Font Face" :
		pCurOrder -> prodType == FONT_STYLE  ? "Font Style" :
		pCurOrder -> prodType == FONT_COLLECTION  ? "Font Collection" : 
				"Unknown") ;
				
    printf("   libID       = %ld\t",pCurOrder->libID);
    printf("clipID      = 0x%lx\t",pCurOrder->clipID);
    printf("vendorID    = 0x%lx\n",pCurOrder->vendorID);

    printf("   name        = %s\n",pCurOrder->ShipAddress.name);

    //printf("   street1     = %s\n",pCurOrder->ShipAddress.street);
  //  printf("   street2     = %s\n",pCurOrder->ShipAddress.street2);

    printf("   city        = %s\t",pCurOrder->ShipAddress.city);
    printf("state       = %s\n",pCurOrder->ShipAddress.state);

   // printf("   province    = %s\n",pCurOrder->ShipAddress.province);
    //printf("   country     = %s\n",pCurOrder->ShipAddress.country);
   // printf("   postal_code = %s\n",pCurOrder->ShipAddress.postal_code);

   printf("   phone       = (%s) %s - %s\n", 
				pCurOrder->ShipAddress.phone.area,
				pCurOrder->ShipAddress.phone.number,
				pCurOrder->ShipAddress.phone.ext ) ;

    //printf("   ccName      = %s\n",pCurOrder->ccName);
    //printf("   ccNum       = %s (%s)\n",pCurOrder->ccNum, pCurOrder->ccExpDate);
    //printf("   ccType      = %d\n",pCurOrder->ccType);
    //printf("   Quantity    = %d\n",pCurOrder->Quantity);

    printf("   priceTotal  = %u\t",(WORD)pCurOrder->priceTotal);
    printf("   priceEach   = %u\n",(WORD)pCurOrder->priceEach);

    printf("   mediaType   = %d\t",pCurOrder->mediaType);
    printf("CourierType = %d\n",pCurOrder->CourierType);

    printf("   flags       = 0x%lX\n",pCurOrder->flags);
}

/****************************************************************
 *  BOOL
 *  GetThatMoola(ORDER_STRUCT *pOrder, CCReq *pCCReq, BOOL bCheckLocalCredit)
 *
 *  Description: 
 *
 *       returns whether credit captured 
 *
 ****************************************************************/
BOOL 
GetThatMoola(ORDER_STRUCT *pOrder, CCReq *pCCReq, BOOL bCheckLocalCredit)
{
   int   nRetval = EOT;
   BYTE  pstatus;            /* Product availability */

   strcpy(pCCReq->AcctNum,pOrder->ccNum);
   strcpy(pCCReq->ExpDate,pOrder->ccExpDate);
   pCCReq->CCType  = pOrder->ccType;
   pCCReq->Amount  = (float)pOrder->priceTotal / 100.0;
   pCCReq->MsgCode = SALE;
   pCCReq->Force   = FORCE;
   pCCReq->ReSend  = NO_RESEND;
   pCCReq->Result  = 0;
   pCCReq->Route   = LOCAL;

   /* set up order */
   if (Acct_NewID(&pOrder->orderID,ACCT_ID_ORDER, NULL) != OK)
   {
      printf("didn't even get an order id!\n");

      /* note at this point we haven't created an order record */
      if (SendByte(WS_PORT,(BYTE)EOT) != OK)
         ;
      return(FALSE);
   }

   /* create a new order record */
   pOrder->userID = lCurUserID;
   pOrder->sessionID = lSessionID;
   pOrder->flags = OF_INITIALIZE;

   if (Acct_NormalOrder ( pOrder, pCCReq ) != OK)
   {
      printf("didn't get an order record\n");

      /* note at this point we haven't created an order record */
      if (SendByte(WS_PORT,(BYTE)EOT) != OK)
         ;
      return(FALSE);
   }

   //printf("CCReq fields from GetThatMoola pre CaptureCredit:\n");
   //PrintCCReq(pCCReq);

   if (pOrder->priceTotal == 0L)
	{
      if ( ( pOrder->prodType == CLIP ) OR
		   ( pOrder->prodType == FONT_STYLE ) OR
		   ( pOrder->prodType == FONT_FACE ) )
      {
         printf("Free Download\n");
         pCCReq->Result = nCAPTURE;

         END(ACK)
      }
		else if ( pOrder -> libID == FS_LIBID_LIMITED_DEMO )
		{
         printf("Free FontCon Demo\n");
         pCCReq->Result = nCAPTURE;

         END(ACK)
		}
      else // no price???
      {
         printf("Error: no price on product!!!\n");
         pCCReq->Result = nAMOUNT_ERR;

         END(EOT)
      }
	}
   if (pOrder->ccType == CORP)
   {
      if (Acct_CorpAcctApproval(pOrder -> userID, 
                                pOrder -> ccNum, 
                                pOrder -> priceTotal, 
                                (WORD *) & pCCReq -> Result ) != OK )
      {
         printf("didn't get corporate money\n");

         END(EOT)
      }
   }
   else
   {
      int Blocked;

      if (ExpDateCheck(pCCReq) != OK)
      {
         printf("credit date check failed\n");
         pCCReq->Result = -1;

         END(EOT)
      }
      else if (pCCReq->Result == nDATE_ERR)
      {
         printf("bad credit card date\n");

         END(NAK)
      }

      /* see whether their credit is blocked */
      if (bCheckLocalCredit)
      {
         if (Acct_CustCredit(lCurUserID,&Blocked) != OK)
         {
            printf("credit blocked failed\n");
            pCCReq->Result = -1;

            END(EOT)
         }

         if (Blocked >= 2)
         {
            printf("credit denied locally\n");
            pCCReq->Result = -2;

            END(NAK)
         }
         else if (Blocked < 0)
         {
            printf("unable to check credit locally\n");
            pCCReq->Result = -1;

            END(EOT)
         }
      } // else not blocked ....

      #if 0
      if (CaptureCredit(pCCReq, ExeType) != OK)
      {
         printf("didn't capture credit\n");

         END(EOT)
      }
      #else
      pCCReq->Result = PUNT;
      #endif
   }

   switch (pCCReq->Result)
   {
      case PUNT:
         printf("Punting on credit capture.\n");
         END(ACK)
      break;

      case nCAPTURE:
         printf("Captured or authorized.\n");
         END(ACK)
      break;

      case nDUPLICATE: /* means got dup but was unable to force */
      case nVOICE_AUTH:
      case nAUTH:
      default: /* unable to communicate for some reason */
         printf("No capture (unable).\n");
         END(EOT)
      break;

      case nDECLINED:
         printf("No capture (rejected).\n");
         END(NAK)
      break;
   }

/* (cek 4/10/92)
 * The 'END()' macro does a 'goto end:
 */

end:

   if (SendByte(WS_PORT,(BYTE)nRetval) != OK)
      ;

   #if 0
   /* hang up credit card line now that we've responded to WS. */
   if (Port)
   {
      HangUp(Port,NULL, & ModemInfo );
      //restore_tty(Port);
      close(Port);
      Port =  (HANDLE) NULL;
   }
   #endif

   /* record result of transaction */
   pOrder->flags |= OF_CC_RESULT;
   Acct_NormalOrder ( pOrder, pCCReq ) ;

   return (BOOL) ( nRetval == ACK ? TRUE : FALSE ) ;

} /* GetThatMoola()  */

int
OpenIngres ( int * nStatus )
{
	 int nRetval;

	 * nStatus = 0 ;

	 if ( bIngresOpen )
		  return OK ;
	
	 if ( GetSystemStatus ( SYS_DB, & nRetval ) != OK )
		 return FAIL ;

	 switch ( nRetval )
	 {
		  case SYS_DB_OFFLINE :		//Ingres is down for scheduled maintenence
				* nStatus = SYS_DB_OFFLINE ;
				return FAIL ;

		  case SYS_DB_ONLINE :
		  {
				printf("Connecting to database server...\n");

				if ( Acct_Initialize ( ) == FAIL )
				{   
					 printf ( "Can't CONNECT to database, exiting\n" ) ;
					 * nStatus = SYS_DB_ONLINE ;
					 return FAIL ;
				}
				else
				{
					 bIngresOpen = TRUE ;
					 * nStatus = SYS_DB_ONLINE ;
					 //printf("Connected to database OK\n");
					 return OK ;
				}
		  }

		  default :
				return FAIL ;
	 }
}

int
CloseUserSession ( int ConditionCode )
{
   if (lSessionID)
   {
      if ( Acct_CustEndSession ( lSessionID, lCurUserID, 
								(WORD) ConditionCode ) != OK )
         printf("Unable to close user (x%lX)(%ld) session (%ld)\n",
               lCurUserID, lCurUserID, lSessionID);
      else
         printf("Closed user (x%lX)(%ld) session (%ld)\n",
               lCurUserID, lCurUserID, lSessionID);

      Acct_Finalize ( ) ;
		bIngresOpen = FALSE ;
      lSessionID = 0L;
   }
   return 1 ;
}

void
ReceiveThatOrder(ORDER_STRUCT *pOrder)
{
   if (ReceiveCRC(WS_PORT,(BYTE *)pOrder,sizeof(ORDER_STRUCT)) != OK)
   {
       printf("Current order not received size: (0x%X)\n",sizeof(ORDER_STRUCT));
       //ShowOrder(pOrder);
       if (SendByte(WS_PORT,NAK) != OK)
               ;
   }
   else
   {
       //printf("Current order received\n");
       if (SendByte(WS_PORT,ACK) != OK)
         printf("Error ACK'ing Order\n");
       //ShowOrder(pOrder);
   }
}

#ifdef DEBUG
int
WriteBuf(char *szFileName, char *buf, int NumBytes)
{
    int fp= -1;
    int nRetval=OK;

    if ((fp = open(szFileName,O_WRONLY|O_CREAT|O_APPEND|O_SYNC,
                           S_IREAD|S_IWRITE)) == -1)
        END(FAIL)

    if (write(fp,buf,NumBytes) != NumBytes)
        END(FAIL)

    end:
    if (fp != -1)
        close(fp);
    return nRetval;
}
#endif


void ExitChild(int CondCode)
{
    printf("Exiting child...\n");

    Cleanup(CondCode);

   printf("Bye from child\n");
   fflush(stdout);
   exit(CondCode);
}

void ExitParent(int CondCode)
{
    printf("Exiting parent...\n");

   if (childPID)
   {
      /* kill the child process */
      printf("killing child...\n");
      if (kill(childPID,SIGTERM) == -1)
         /* child couldn't hangup... */
         Cleanup(CondCode);
   }
   else /* we *are* the child process */
      Cleanup(CondCode);

   if (FileTree)
      free(FileTree);

   printf("Bye from parent\n");
   fflush(stdout);
   exit(CondCode);
}

static void
Cleanup(int CondCode)
{
    printf("cleaning up...\n");

	FileUnlock ( nLockFile ) ; 
 
    /* in case child was gone already */
   if (NOT bIngresAbort)
      CloseUserSession(CondCode);
   bIngresAbort = FALSE;

   printf("Total outgoing traffic: %ld bytes\n", outbytes); 
   printf("Total incoming traffic: %ld bytes\n", inbytes); 

   if (childPID) // then we're the parent, and GlobalhCom doesn't work
   {
      HangUp ( NULL, savetty, & ModemInfo ); // force hangup
      if (GlobalhCom)
      {
         close(GlobalhCom);
         GlobalhCom = (HANDLE) NULL;
      }
   }
   else // we're the child
   {
      if (GlobalhCom)
      {
         HangUp ( GlobalhCom, savetty, & ModemInfo ) ;
         close(GlobalhCom);
         GlobalhCom = (HANDLE) NULL;
      }
   }

   if (hOrigCom)
   {
      //so won't answer phone...
      set_tty_local ( hOrigCom, TRUE ) ;
      //see hangup() WriteModemCommand(hOrigCom,"ATS0=0\r"); 
      restore_tty ( hOrigCom ) ;
      close(hOrigCom);
      hOrigCom = (HANDLE) NULL;
   }

#if 0
   /* from credit card: */
   if (Port)
   {
      HangUp(Port,NULL, & ModemInfo );
      //restore_tty(Port);
      close(Port);
      Port = NULL;
   }
#endif

#ifdef TEST_SERVER
   disconnect_from_server(cc_server);
#endif
}

void ShowShip(Shipcost_Ary CostMatrix)
{
   printf("sm\[0\]:.valid %d .cost %d\n",CostMatrix[0].valid,
                               CostMatrix[0].cost);
   printf("sm\[1\]:.valid %d .cost %d\n",CostMatrix[1].valid,
                               CostMatrix[1].cost);
   printf("sm\[2\]:.valid %d .cost %d\n",CostMatrix[2].valid,
                               CostMatrix[2].cost);
}


/* CheckPassword()
//
// DESCRIPTION:
//   Get password via Acct_CustGetPassword. Check against password parameter.
//   Return OK if they match, FAIL if they don't.
//
// HISTORY:
//   3/13/91 - beng
*/

WORD CheckPassWord ( DWORD userID, char* szCheckPassWord) 
{
   char* cp;
   char szPassWord[PW_SIZE+1];

   if (Acct_CustGetPassword(userID, szPassWord) != OK) 
      return FAIL;

   cp = szPassWord;
   while (*cp)
   {
      *cp = (char)toupper(*cp);
      cp++;
   }

   cp = szCheckPassWord;
   while (*cp)
   {
      *cp = (char)toupper(*cp);
      cp++;
   }

   if (strcmp(szPassWord, szCheckPassWord) == 0)
      return OK;
   return FAIL;
}


/*******************************************************************
 * Utility functions for FontShopper
 *******************************************************************/

/****************************************************************
 *  BOOL GetFSExeVersions( char rgszFSExe[][], WORD rgwFSExeID[], int n )
 *
 *  Description: 
 *
 *    Get's the list of executable versions and fixes up the
 *    file extensions in rgszFSExe[].
 *
 *  Comments:
 *
 ****************************************************************/
BOOL GetFSExeVersions( char *rgszFSExe[], WORD rgwFSExeID[], int n )
{
   int i ;
   DIR *pDir ;
   struct dirent *pDirEnt ;
   char *pExt ;

   printf( "    " ) ;
   for (i = 0 ; i < n ; i++)
   {
      /* Do the equiv of a DOS findfirst/findnext */

      printf( "%s", rgszFSExe[i] ) ;
      if (pDir = opendir( FONTSHOPEXEDIR ))
      {
Blah:
         while( (pDirEnt = readdir( pDir )) != NULL )
         {
            /* Assume upper case */

            if (strncmp( pDirEnt->d_name,
                         rgszFSExe[i],
                         strlen(rgszFSExe[i]) ) == 0)
               break ;
         }

         if (pDirEnt == NULL)
         {
            printf( "\nCould not get find file matching '%s'\n",
                     rgszFSExe[i] ) ;
            return FALSE ;
         }

         /* Parse off the extension */
         if (!(pExt = strchr( pDirEnt->d_name, '.' )))
         {
            printf( "\nCould not get file version for '%s'\n",
                     rgszFSExe[i] ) ;
            return FALSE ;
         }

         pExt++ ;

         if (!(rgwFSExeID[i] = atoi( pExt )))
         {
			goto Blah ;
         }

         printf( "%03d,", rgwFSExeID[i] ) ;

         strcat( rgszFSExe[i], pExt ) ;
         closedir( pDir ) ;
      }
      else
      {
         printf( "\ncall to opendir() failed!!!\n" ) ;
         return FALSE ;
      }
   }

   printf("\n");
   return TRUE ;

} /* GetFSExeVersions()  */

/****************************************************************
 *  DWORD CalcFSDataBaseCRC() ;
 *
 *  Description: 
 *
 *    Calculates a 32 bit CRC for all of the fontshopper database
 *    files.  It does a "DIR" of the database directory.
 *
 *  Comments:
 *    This function assumes that the readdir() api returns
 *    the filenames sorted.  All my testing shows that this is
 *    true, but it is not documented anywhere!
 *
 *    This function also generates the /shopper/fs/database.lst
 *    file that is uploaded to fontshopper if a discrepancy is
 *    detected.
 *
 ****************************************************************/
DWORD CalcFSDataBaseCRC( void ) 
{
   DWORD          dwCRC ;
   int            nLen = 0 ;
   DIR            *pDir ;
   struct dirent  *pDirEnt ;
   struct stat    Stat ;
   char *         pBuf = NULL ;
   int            n ;
   char           szFullPath[256] ;
   FILE           *pFile ;

   dwCRC = 0 ;

   if (!(pFile = fopen( FSDBLIST_FNAME, "w" )))
   {
      printf( "creat() failed in CalcFSDataBaseCRC!!!\n" ) ;
      return FALSE ;
   }

   if (!(pDir = opendir( FONTSHOPDBDIR )))
   {
      printf( "opendir() failed in CalcFSDataBaseCRC!!!\n" ) ;
      fclose( pFile ) ;
      return FALSE ;
   }

   while (pDirEnt = readdir( pDir ))
   {
      sprintf( szFullPath, "%s/%s", FONTSHOPDBDIR, pDirEnt->d_name ) ;
      stat( szFullPath, &Stat ) ;

      /* The st_mode member of the Stat structure tells us what kind
       * of file this is.  We want to ignore everything but
       * "normal" files.
       */
      if (Stat.st_mode & 0x8000)
      {
         //printf( "Adding '%s' to CRC list\n", pDirEnt->d_name ) ;

         fprintf( pFile, "%s\n", pDirEnt->d_name ) ;

         n = strlen( pDirEnt->d_name ) + 1 ;

         if (pBuf == NULL)
            pBuf = malloc( n + 2 ) ;
         else
            pBuf = realloc( pBuf, nLen + n + 2 ) ;

         if (pBuf == NULL)
         {
            printf( "Major malfunction in CalcDatabaseCRC!!!\n" ) ;
            closedir( pDir ) ;
            fclose( pFile ) ;
            return 0 ;
         }

         strcpy( pBuf + nLen, pDirEnt->d_name ) ;

         nLen += n ;
      }
   }

   closedir( pDir ) ;

   if (pBuf == NULL)
   {
      printf( "Error allocating pBuf!\n" ) ;
      fclose( pFile ) ;
      return 0 ;
   }

   *(pBuf + ++nLen) = '\0' ;

   /* Calc the 32 bit CRC for the buffer.
    */
   dwCRC = CalcCRC32( pBuf, nLen ) ;

   free( pBuf ) ;

   fclose( pFile ) ;

   return dwCRC ;

} /* CalcFSDataBaseCRC()  */

#define CRC32_POLYNOMIAL 0xEDB88320L

//------------------------------------------------------------------------
//  void BuildCRC32Table( void )
//
//  Description:
//     Builds the CRC-32 table.
//
//  Parameters:
//     NONE
//
//  History:   Date       Author      Comment
//              5/10/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------


void BuildCRC32Table( DWORD CRC32Table[] )
{
   int           i, j ;
   DWORD         dwCRC32 ;

   for (i = 0; i  < 256; i++)
   {
      dwCRC32 = i ;
      for (j = 8; j > 0; j--)
      {
         if (dwCRC32 & 1L)
            dwCRC32 = (dwCRC32 >> 1) ^ CRC32_POLYNOMIAL ;
         else
            dwCRC32 >>= 1 ;
      }
      CRC32Table[ i ] = dwCRC32 ;
   }

} // end of BuildCRC32Table()


/****************************************************************
 *  DWORD WINAPI CalcCRC32( LPBYTE lpBuf, int n)
 *
 *  Description: 
 *
 *    Returns a Crc32 for the buffer passed in.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD CalcCRC32( unsigned char * lpBuf, int cb )
{
   static DWORD rgCRC32[256] ;
   static BOOL  fTableBuilt = FALSE ;
   int   i ;
   DWORD  dwCRC32 = 0xFFFFFFFFL ;


   if (fTableBuilt == FALSE)
   {
      BuildCRC32Table( rgCRC32 ) ;
      fTableBuilt = TRUE ;
   }

   for (i = 0 ; i < cb ; i++)
   {
      dwCRC32 = rgCRC32[((int)dwCRC32 ^ lpBuf[i]) & 255] ^ ((dwCRC32 >> 8) & 0x00FFFFFFL) ;
   }
   dwCRC32 = ~dwCRC32 ;

   return dwCRC32 ;

} /* CalcCRC32()  */

