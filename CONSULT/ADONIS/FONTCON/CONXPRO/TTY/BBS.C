/* 
 Date: 3-23-1990              
 Doug Kent                    
 Creation Date:   3/23/90

 Revision History:
   4/10/92  cek   Started adding FontShopper Support
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
#include <fcntl.h>
#include <sys/dir.h>

#ifndef ACCTH
#include  "acct.h"
#endif
#include  "acct_pro.h"


// which port to read from
#define WS_PORT GlobalhCom

//which directory are the clipart files:
#if 0
#define CLIPSDIR "/usr/bbs/master.bin/clips"
#else
#define CLIPSDIR "/shopper/clips"
#endif

/* (cek 4/10/92)
 * The following define where the various FontShopper related
 * files are
 */
#define FONTSHOP_TTFDIR "/shopper/fonts"


/*
 * External variables
 */
extern int errno;


/* (cek 4/10/92)
 * Define function prototypes for functions that are unix functions,
 * but do not appear to have an include file.
 */
int getpid() ;
int fork() ;
int setpgrp() ;


/* (cek 4/10/92)
 * Define function prototypes for functions our own local functions.
 */
int   openWSPort(BOOL bHiSpeed) ;
void  ExitParent(int CondCode) ;
void  ExitChild(int CondCode) ;
int   Receive_Message(HANDLE hCom, MSGTYPE *msgP) ;
void  ShowCust(ADDRESS_STRUCT *CustInfo) ;
void  ShowShip(Shipcost_Ary CostMatrix);
void ShowOrder(ORDER_STRUCT *pCurOrder) ;

int   setnewuser(void);
int   GetPaddedBuf(HANDLE hCom, WORD nNumChars, char *szBuf,BOOL upperize);
int   GetFileName(HANDLE hCom, WORD nNumChars, char *szFileName);
int   GetExt(char *szFileName);
int   OldGetExt(char *szFileName,char *dir);
BOOL  GetThatMoola(ORDER_STRUCT *pOrder, CCReq *pCCReq, BOOL bCheckLocalCredit);
void  ReceiveThatOrder(ORDER_STRUCT *pOrder);
WORD  CheckPassWord ( DWORD userID, char* szCheckPassWord) ;
static void Cleanup(int CondCode);


/*
 * Exported global variables.
 *
 * Note:  Are we sure these are exported?
 */
//HANDLE cc_server;
DWORD    lCurUserID=0L,
         lSessionID=0L;

long     inbytes = 0;     /* To track data traffic */
long     outbytes = 0;

//to calc the LRC sum (see util.c in cash) for all database filenames to
// compare against the local database
DWORD    dwDataFileSum = 0;
WORD     wDataFileCount = 0;

/*
 * Local global variables (static)
 */
//This BOOL determines whether we ship a renewal package or not
static BOOL bRenewPkg = FALSE;
//This BOOL is set when child process is terminated because of INGRES failure
static BOOL bIngresAbort = FALSE;

#define BADSIG (int (*)())-1
static void       HandleSig(int sig);
static int        nTimeout;
static void       SetSigHandler(void);
static HANDLE     GlobalhCom=(HANDLE)NULL,
                  hOrigCom=(HANDLE)NULL;

static char       logfile[30];   /* For redirecting stderr and stdout */
static FILE*      plogfile;

static BYTE       RequestedFileName[STD_FILENAME_SIZE+1];
static BYTE       UptodateFileName[STD_FILENAME_SIZE+1];
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

static BOOL       fFontShopper = FALSE ;
static CRC32      crc32FontShopDB ;         // crc32 of the fontshop db files

void main(int argc, char *argv[])
{

   MSGTYPE  msg;
   BOOL     PP9600;
   BOOL     bAuthorized; // depends on kind of newuser
   BOOL     bWSSaidHi, // kludge fix for 1.0 sync bug,
            bWSSyncFix;// so we'll only respond to SAY_HI once on startup

   char     sztemp[5];

   time(&clock);

   printf("\n***************************************************************\n" );
   printf(  "*\n" );
   printf(  "*   Font Shopper and ClipArt Shopper Enabled BBS is starting!\n" );
   printf(  "*\n" );

   /* Redirect stdout (printf's) and stderr to a logfile */
   if (sscanf(argv[1], "/dev/tty%s", sztemp) != 1)
      printf("\nUnable to redirect logfile\n");
   else
   {
      sprintf(logfile, "%s/bbslog.%s", LOG_DIR, sztemp);
      printf(  "*  Logging to %s\n", logfile ) ;
      printf(  "*  To view output use 'tail -f %s' or the 'BA' command.\n", logfile);

      printf(  "*\n" );
      printf(  "***************************************************************\n\n" );

      plogfile = freopen(logfile, "w", stdout);
      freopen(logfile, "a+", stderr);
   }

   /* set unbuffered output */
   setbuf(stdout,NULL);
   setbuf(stderr,NULL);

   if(argc != 3)
   {
      printf("Usage: %s [tty device] [PP9600 ? 1 : 0]\n\
      (example: %s /dev/ttyi1A 1)\n",argv[0],argv[0]);
      exit(1);
   }

   savetty = argv[1];
   PP9600 = (BOOL)(argv[2][0] == '1');
   parentPID = getpid();

   printf("Starting bbs on %s, at %s\n", savetty, ctime(&clock)); 
   if (PP9600) printf("BBS configured for Practical Peripherals 9600SA\n");

   /* construct directory binary search tree */
   //printf("Calling MakeFileTree\n");
   if ((FileTree = MakeFileTree("./")) == NULL)
   {
      printf("Error constructing directory tree.\n");
      exit(1);
   }

   printf("LRC sum for database is %ld, based on %d database files\n", 
   dwDataFileSum, wDataFileCount);

   #if 0
   // for continuous database connection
   if (Acct_Initialize() == FAIL)
   {	
      printf("Can't CONNECT to database, exiting\n");
      exit(1);
   }
   else
   {
      //printf("Connected to database OK\n");
   }
   #endif

   while(1)
   {
      SetSigHandler();  // child may have captured one.  Signals will be 
                        // inherited by child

      bWSSyncFix = TRUE;
      bWSSaidHi = FALSE;

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
            if (openWSPort(PP9600) != OK) // only the child opens the port
               ExitChild(-3);

            ExeType = EXE_REGULAR;
            WinVer = 0x300;
            inbytes = outbytes = 0;
            //respond to MSG_SAID_HI
            if (!bWSSaidHi)
            {
               if (SendByte(WS_PORT,ACK) != OK)
                  printf("Got HI, Error sending ACK\n");
               else
               {
                  printf("Said Hi to ClipArt/Font Shopper\n");
                  if (bWSSyncFix)
                     bWSSaidHi = TRUE;
               }
            }
         break; // into message loop

         case -1:
            printf("bbs: can't create new child process\n");
         ExitParent(1); // exit bbs

         default: // parent receives process id of child that died
            if (wait(&statusp) == -1)	//Wait here for child to die
               perror("bbs");

            printf("child returned status: 0x%X\n",statusp);

            if (WIFSIGNALED(statusp)) // then child died ignominiously
            {
               printf("CHILD was killed!!\n");
               ExitParent(-4);
            }
            else if (WIFEXITED(statusp))
            {
               printf("WEXITSTATUS(status)  = 0x%X\n",WEXITSTATUS(statusp));
               if (WEXITSTATUS(statusp) == 0xff)
               {
                  /* assume -1 is INGRES exit status */
                  printf("CHILD exited by INGRES?\n");
                  bIngresAbort = TRUE;
                  Cleanup(-4);
               }
               else
                  printf("CHILD exited normally.\n");
            }
            continue; // to fork again

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
            case MSG_HANGING_UP:
               printf("Client hanging up\n");
               ExitChild(0);
            break;

            case MSG_SEND_EXETYPE:
               if (ReceiveBuf(WS_PORT,(BYTE *)&ExeType,
                              sizeof(ExeType),STD_WAIT) != OK)
               {
                  printf("Error getting ExeType:   %x\n",ExeType);
                  if (SendByte(WS_PORT,NAK) != OK)
                     ;
               }
               else
               {
                  printf("Received ExeType: %x ",ExeType);

                  switch(ExeType)
                  {
                     case EXE_BETA:
                        printf("(ClipArt Shopper Beta)\n");
                        
                        fFontShopper = FALSE ;
                        nTimeout = 120; // 2 minutes
                        if (SendByte(WS_PORT,NAK) != OK)
                           ;
                     break;

                     case EXE_REGULAR:
                        printf("(ClipArt Shopper Regular)\n");

                        fFontShopper = FALSE ;
                        nTimeout = 600; // ten minutes
                        if (SendByte(WS_PORT,ACK) != OK)
                           ;
                     break;

                     case EXE_INTERNAL:
                        printf("(ClipArt Shopper Internal)\n");

                        fFontShopper = FALSE ;
                        nTimeout = 600; // ten minutes
                        if (SendByte(WS_PORT,ACK) != OK)
                           ;
                     break;

                     case EXE_FONTSHOP_BETA:
                        printf("(Font Shopper Beta)\n");

                        fFontShopper = TRUE ;
                        nTimeout = 120; // 2 minutes
                        if (SendByte(WS_PORT,NAK) != OK)
                           ;
                     break;

                     case EXE_FONTSHOP_REGULAR:
                        printf("(Font Shopper Regular)\n");

                        fFontShopper = TRUE ;
                        nTimeout = 600; // ten minutes
                        if (SendByte(WS_PORT,ACK) != OK)
                           ;
                     break;

                     case EXE_FONTSHOP_INTERNAL:
                        printf("(Font Shopper Internal)\n");

                        fFontShopper = TRUE ;
                        nTimeout = 600; // ten minutes
                        if (SendByte(WS_PORT,ACK) != OK)
                           ;
                     break;

                     default:
                        printf("(Unknown EXE Type!)\n");

                        fFontShopper = TRUE ;
                        nTimeout = 120; // 2 minutes
                        if (SendByte(WS_PORT,NAK) != OK)
                           ;
                     break ;

                  }
               }
            break;

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

                  ShowCust(&CurCustInfo);
                  if (SendByte(WS_PORT,ACK) != OK)
                     ;
               }
            break;

            case MSG_QUERY_NEW_USERID_OK:
   			{
               /* (cek 4/10/92)
                * This varible is not used anywhere else in this
                * file and neither FontShopper or ClipArtShopper send this
                * message.  It should be removed.
                */
	   			bAuthorized=TRUE; // yes, fall through
		   	}
            case MSG_QUERY_USERID_OK:
            {
               WORD Active;

               if (Acct_CustIsActiveUserID(lCurUserID,&Active,NULL,NULL) != OK)
                  Active = -1;

               printf("Query UserID returning: %d\n",Active);

               if (SendBuf(WS_PORT,(BYTE *)&Active,sizeof(Active)) != OK)
                  printf("Error sending user Status\n");
            }
            break;

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

            // WS is asking wether or not we ship a package for a renewal
            case MSG_QUERY_SHIP_RENEWAL:
            {
               if (SendBuf(WS_PORT,(BYTE *)&bRenewPkg,sizeof(BOOL)) != OK)
                  printf("Error replying query ship renewal\n");
            }
		   	break;

            case MSG_QUERY_DATA_LRC:
            {
               if (fFontShopper)
               {
                  if (SendBuf( WS_PORT, (BYTE*)&crcFontShopDB, 
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

            case MSG_BEGIN_SESSION:
               /* record with INGRES that our user's session has begun.
                  It ends (gracefully) when we receive a HANGING_UP message. */

               if (Acct_NewID(&lSessionID,ACCT_ID_SESSION, NULL) != OK)
                  SendByte(WS_PORT,NAK);
               else if (Acct_CustBeginSession(lCurUserID,lSessionID,
                                              ExeType,WinVer) != OK)
                  SendByte(WS_PORT,NAK);
               else if (SendByte(WS_PORT,ACK) == OK)
               {
                  printf("Opened user (x%lx)(%ld) session (%ld)\n",
                  lCurUserID,lCurUserID,lSessionID);
                  break;
               }

               printf("Unable to open user (x%lx)(%ld) session (%ld)\n",
               lCurUserID,lCurUserID,lSessionID);
            break;

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

            case MSG_SET_USER_ID:
            {
               char szTmp[PW_SIZE+1];
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

                  //printf("UserID: %s (%lx)\n",CurUserID,lCurUserID);
               }
            }
            break;


            case MSG_NEW_GET_NEW_USER_ID:
            {
               /* (cek 4/10/92)
                * This message is not sent by ClipArt Shopper or FontShopper.
                * It should be removed.
                */

               /*
                This version of get NEW USERID requires the subtype (libid)
                and media for the new userid and calls a Acct_NewUserID()
                instead of Acct_NewID()
               */

               DWORD NewUserID;
               DWORD dwLibID;
               WORD  wMedia;

               if (ReceiveBuf(WS_PORT,(BYTE *)&wMedia,
                              sizeof(wMedia),STD_WAIT) != OK)
               {
                  printf("Error getting wMedia:   %d\n", wMedia);
                  SendByte(WS_PORT,NAK);
                  break;
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
                  printf("error getting new userid");
                  SendByte(WS_PORT,NAK);
                  break;
               }
               lCurUserID = NewUserID;

               sprintf(CurUserID, "%08lX", NewUserID);
               //printf("New userID: %s\n",CurUserID);

               if (SendCRC(WS_PORT, CurUserID, PW_SIZE) != OK)
                  ;
            }
            break;

            case MSG_GET_NEW_USER_ID:
            {
               DWORD NewUserID;

               /* get a new unused userID from INGRES (don't record
                  it as used yet, however).  Send it to Client. */

               //printf("getting New userID\n");

               if(Acct_NewID(&NewUetThatMoola()  */

int
setnewuser(void)
{
	int nRetval;

	printf("Connecting to database server...\n");
    if (Acct_Initialize() == FAIL)
	{	
		printf("Can't CONNECT to database, exiting\n");
		END(FAIL)
	}
	else
	{
		END(OK)
		//printf("Connected to database OK\n");
	}

	end:
	if (nRetval == OK)
	{
		CurUserID[0] = '\0';
		lCurUserID=lSessionID=0L;
		CurPassword[0] = '\0';
		nTimeout = 120;
	}
	return nRetval;
}

int
CloseUserSession(int ConditionCode)
{
	if (lSessionID)
	{
		if (Acct_CustEndSession(lSessionID, lCurUserID, ConditionCode) != OK)
			printf("Unable to close user (x%lX)(%ld) session (%ld)\n