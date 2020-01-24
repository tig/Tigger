/* 
 Date: 3-23-1990              
 Doug Kent                    
 Creation Date:   3/23/90     
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

#ifndef ACCTH
#include  "acct.h"
#endif
#include  "acct_pro.h"

extern int errno;

// which port to read from
#define WS_PORT GlobalhCom

//which directory are the clipart files:
#if 0
#define CLIPSDIR "/usr/bbs/master.bin/clips"
#else
#define CLIPSDIR "/shopper/clips"
#endif

//This BOOL determines whether we ship a renewal package or not
static BOOL bRenewPkg = FALSE;
//This BOOL is set when child process is terminated because of INGRES failure
static BOOL bIngresAbort = FALSE;

int setnewuser(void);
int GetPaddedBuf(HANDLE hCom, WORD nNumChars, char *szBuf,BOOL upperize);
int GetFileName(HANDLE hCom, WORD nNumChars, char *szFileName);
int GetExt(char *szFileName);
BOOL GetThatMoola(ORDER_STRUCT *Order, CCReq *ccreq, BOOL bCheckLocalCredit);
void ReceiveThatOrder(ORDER_STRUCT *Order);
static void Cleanup(int CondCode);
WORD CheckPassWord ( DWORD userID, char* szCheckPassWord) ;

#define BADSIG (int (*)())-1
static void HandleSig(int sig);
static int nTimeout;
static int SetSigHandler(void);
static HANDLE GlobalhCom=(HANDLE)NULL,
              hOrigCom=(HANDLE)NULL;
/* char *savetty;  moved declaration to comm.c so I could compile-Ben*/

static char logfile[30];   /* For redirecting stderr and stdout */
static FILE* plogfile;

//HANDLE cc_server;
DWORD lCurUserID=0L,lSessionID=0L;

long inbytes = 0;     /* To track data traffic */
long outbytes = 0;

static    BYTE RequestedFileName[STD_FILENAME_SIZE+1];
static    BYTE UptodateFileName[STD_FILENAME_SIZE+1];
static    HANDLE CurFilePtr = -1;
static    ORDER_STRUCT CurOrder;
static    CCReq CurCCReq;
static    ADDRESS_STRUCT CurCustInfo;
static    char CurPassword[PW_SIZE+1],CurUserID[PW_SIZE+1];
static    WORD WinVer,ExeType;
static	  int parentPID,childPID=0;
static    FILENODE *FileTree=NULL;
static    char* p;
static    time_t clock;

//to calc the LRC sum (see util.c in cash) for all database filenames to
// compare against the local database
DWORD dwDataFileSum = 0;
WORD wDataFileCount = 0;

main(int argc, char *argv[])
{

    MSGTYPE msg;
	BOOL PP9600;
	BOOL bAuthorized; // depends on kind of newuser
	BOOL bWSSaidHi, // kludge fix for 1.0 sync bug,
		 bWSSyncFix;// so we'll only respond to SAY_HI once on startup

	char sztemp[5];

	time(&clock);

	/* Redirect stdout (printf's) and stderr to a logfile */
	if (sscanf(argv[1], "/dev/tty%s", sztemp) != 1)
		printf("Unable to redirect logfile\n");
	else
	{
		sprintf(logfile, "%s/bbslog.%s", LOG_DIR, sztemp);
		printf("Logging to %s,\nto view output use 'tail -f %s'\n", 
				logfile, logfile);
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
    PP9600 = (argv[2][0] == '1');
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
	SetSigHandler(); // child may have captured one.  Signals will be 
				 // inherited by child

	bWSSyncFix = TRUE;
	bWSSaidHi = FALSE;

	switch(childPID = fork()) 
	/* The fork necessary to get setpgrp() to work properly.  
       setpgrp() sets the group id to the value of the process id.  
	   Thus setpgrp() will only work once for a given process.  
	   Each child, however, has a unique process id, and 
	   thus each child's call to setpgrp() is effective. */
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
					printf("Said Hi to Window Shopper\n");
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
	}

    /* we get here only if child */

	/* Seed random number generator with process ID for rare
	   instance of new user and Window Shopper not in stock */

	srand(getpid());

	while (1)
	{
        if (Receive_Message(WS_PORT,&msg) != OK)
        {
			printf("Window Shopper timed out.\n");
			ExitChild(-2);
        }

        switch(msg)
        {
            case MSG_HANGING_UP:
                printf("Window Shopper hanging up\n");
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
                    printf("Received ExeType:  %x\n",ExeType);
					switch(ExeType)
					{
						case EXE_BETA:
						default:
							nTimeout = 120; // 2 minutes
							if (SendByte(WS_PORT,NAK) != OK)
								;
						break;

						case EXE_REGULAR:
							nTimeout = 600; // ten minutes
							if (SendByte(WS_PORT,ACK) != OK)
								;
						break;

						case EXE_INTERNAL:
							nTimeout = 600; // ten minutes
							if (SendByte(WS_PORT,ACK) != OK)
								;
						break;
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
				if (SendBuf(WS_PORT,(BYTE *)&dwDataFileSum, 
										sizeof(DWORD)) != OK)
					printf("Error replying data LRC\n");
			}
			break;

            case MSG_BEGIN_SESSION:
                /* record with INGRES that our user's session has begun.
                   It ends (gracefully) when we receive a HANGING_UP message. */
                if (Acct_NewID(&lSessionID,ACCT_ID_SESSION, NULL) != OK)
                    SendByte(WS_PORT,NAK);
                else if (Acct_CustBeginSession(lCurUserID,lSessionID, \
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
            { /*
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
                   it as used yet, however).  Send it to Window Shopper. */
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
                   it as used yet, however).  Send it to Window Shopper. */
                //printf("getting New userID\n");
                if(Acct_NewID(&NewUserID,ACCT_ID_USER, NULL)  != OK)
                {
                    printf("error getting new userid");
                    break;
                }
                lCurUserID = NewUserID;
                sprintf(CurUserID,"%08lx",NewUserID);
                //printf("New userID: %s\n",CurUserID);
                if (SendCRC(WS_PORT,CurUserID,PW_SIZE) != OK)
					;
			}
            break;

            case MSG_COMMIT_PASSWD:
                /* send password to INGRES */
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
				break;
            }

            case MSG_QUERY_NEWUSER_OK: 
            case MSG_QUERY_RENEWAL_OK: 
			{	/* purchasing subscription online */

				//last param is to check local credit or not, if it is not
				// a renewal we have no local history so don't check it
                GetThatMoola(&CurOrder,&CurCCReq,(msg == MSG_QUERY_RENEWAL_OK));

				/* WS will send a COMMIT message if all else completes OK */
				break;
			}

            case MSG_QUERY_LIB_ORDER_OK:
                GetThatMoola(&CurOrder, &CurCCReq, TRUE);
				/* WS will send a COMMIT message if all else completes OK */
			break;

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

            case MSG_SET_CUR_CLIP_DLD:
            case MSG_SET_NEW_USER_ORDER:
            case MSG_SET_CUR_LIB_ORDER:
                ReceiveThatOrder(&CurOrder);
				//if (msg == MSG_SET_NEW_USER_ORDER)
		            //WriteBuf("ORDER.TXT",&CurOrder,sizeof(ORDER_STRUCT));
            break;

			//COMMIT_RENEWAL needed for backward compatibility with WS v1.2 r38
            case MSG_COMMIT_NEWUSER:
            case MSG_COMMIT_RENEWAL:
			{
				DWORD dwLibID = CurOrder.libID;
				//Assumes CurOrder is Null if no subscription order in process
			
				if (msg == MSG_COMMIT_RENEWAL)
				{
					printf("COMMIT_RENEWAL, LibID: %ld, price: %u, media: %d\n",
							dwLibID, CurOrder.priceEach, CurOrder.mediaType);
					if ((dwLibID == WS_LIBID_NEW OR dwLibID == WS_LIBID_UPGRADE)
						OR bRenewPkg)
						CurOrder.flags |= OF_COMMITED;	//order needs shipping
					else			//No package, order considered shipped
						CurOrder.flags |= OF_SHIPPED;

					if (Acct_NormalOrder ( &CurOrder, &CurCCReq ) != OK)
					{
						SendByte(WS_PORT,NAK);
						printf("error committing new user\n");
						break;
					}
				}	

				if (Acct_CustNewUser(lCurUserID, CurPassword, dwLibID, 
										&CurCustInfo)==OK)
				{
					SendByte(WS_PORT,ACK);
					break;
				}

				SendByte(WS_PORT,NAK);
				printf("error committing new user\n");
				break;
			}

            case MSG_COMMIT_SUBSCRIPTION:
				if ((CurOrder.libID == WS_LIBID_NEW) OR 
					(CurOrder.libID == WS_LIBID_UPGRADE) OR 
					bRenewPkg)
					CurOrder.flags |= OF_COMMITED;	//order needs shipping
				else			//No package, order considered shipped
					CurOrder.flags |= OF_SHIPPED;

				if (Acct_NormalOrder ( &CurOrder, &CurCCReq ) != OK)
				{
					SendByte(WS_PORT,NAK);
					printf("error committing new user\n");
					break;
				}

				SendByte(WS_PORT,ACK);
            break;

            case MSG_COMMIT_CLIP_DLD:
            case MSG_COMMIT_LIB_ORDER:
				if (msg == MSG_COMMIT_CLIP_DLD) // download succeeded
				{
					printf("Clipart downloaded OK\n");
					/* record delivery of clipart */
					CurOrder.flags |= OF_SHIPPED;
				}
				else   // all go on lib order
				{
					/* record placement of order */
					CurOrder.flags |= OF_COMMITED;
					printf("Library order placed\n");
				}	

				if (Acct_NormalOrder ( &CurOrder, &CurCCReq ) == OK)
				{
					SendByte(WS_PORT,ACK);
					SendBuf(WS_PORT,&CurOrder.orderID,sizeof(CurOrder.orderID));
					break;
				}

				SendByte(WS_PORT,NAK);
				printf("error placing order\n");
            break;

            case MSG_SAY_HI:
				if (!bWSSaidHi)
				{
					if (SendByte(WS_PORT,ACK) != OK)
						printf("Error sending ACK\n");
					else
					{
						printf("Said Hi\n");
						if (bWSSyncFix)
							bWSSaidHi = TRUE;
					}
				 }
            break;

            case MSG_SET_CUR_FILENAME: 
                if (CurFilePtr != -1)
				{
                    close(CurFilePtr);
					CurFilePtr = -1;
				}

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
					char TmpFileName[STD_FILENAME_SIZE+10+1];
					if (SendByte(WS_PORT,ACK) != OK)
						;
                    //printf("Received RequestedFileName: %s\n", 
						//RequestedFileName);
					/* the idea here is that we try to open any file that
					   has the same basename as RequestedFileName.  */
				    strcpy(UptodateFileName,RequestedFileName);
                    if (GetExt(UptodateFileName) == OK)
                    {
						if ((UptodateFileName[0] == '0') AND 
							(UptodateFileName[1] == '1'))
							 sprintf(TmpFileName,"%s/%s", CLIPSDIR,
									  UptodateFileName);
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
						break;
					}
                }
            break;

            case MSG_RECEIVE_FILE:
            case MSG_DOWNLOAD_EXE:
                printf("Sending %s...\n",UptodateFileName);
                if (XMSend(WS_PORT,CurFilePtr) != OK)
                    printf("Error sending file %s\n",UptodateFileName);
                break;


            case MSG_LATEST_VERSION:
                {
    #ifdef TEST_SERVER
                char buf[9];
                memset(buf,0,sizeof(buf));
                if (send_to_server(cc_server,
                                   "Hi to cc",8) == FAIL)
                    perror("cc_server");
                if (recv_from_server(cc_server,
                                    buf,8) == FAIL)
                    perror("cc_server");
                    printf("cc_server returns: %s\n",buf);
    #endif
                }

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
                break;

            case MSG_QUERY_DEST_FILENAME:
            {
                char szDestName[STD_FILENAME_SIZE+1];
                int len;

                //printf("Query dest filename:");
				if (CurFilePtr == -1)
				{
					memset(szDestName,PADCHAR,STD_FILENAME_SIZE);
					if (SendCRC(WS_PORT,(BYTE *)szDestName,STD_FILENAME_SIZE) 
						   != OK)
						printf("Error sending dest filename\n");
					break;
				}

                lseek(CurFilePtr,3L,SEEK_SET);
                if (read(CurFilePtr,szDestName,STD_FILENAME_SIZE) 
                                   < STD_FILENAME_SIZE)
                    //printf("  Unable to obtain.\n")
					;
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

            case MSG_QUERY_INSTOCK:
				{
					 WORD pstatus=ACK;

					/* Get product availability if library or Window Shopper */
					if (Acct_CheckProdAvail ( &CurOrder, &pstatus ) != OK)
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

					case MSG_QUERY_CUR_FILESIZE:
					{
						 WORD size;
						 //printf("Query blocks:");
						 if (CurFilePtr == -1)
						 {
							 size = 0;
							 if (SendBuf(WS_PORT,(BYTE *)&size,
									  sizeof(size)) != OK)
								  printf("Error sending size\n");
							 break;
						 }

						 lseek(CurFilePtr,3L + TOTALBLOCKNUM_INDEX,SEEK_SET);
						 if (read(CurFilePtr,&size,sizeof(size)) < sizeof(size))
							  //printf("  Unable to obtain.\n")
						;
						 else
							  //printf("  %d.\n",size-1)
						;
						 --size;
						 if (SendBuf(WS_PORT,(BYTE *)&size,sizeof(size)) != OK)
							  printf("Error sending size\n");
					}
					break;

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
                if (read(CurFilePtr,&size,sizeof(size)) < sizeof(size))
                    //printf("  Unable to obtain.\n")
					;
                else
                    //printf("  %ld.\n",size)
					;
                if (SendBuf(WS_PORT,(BYTE *)&size,sizeof(size)) != OK)
                    printf("Error sending size\n");
            }
            break;

            case MSG_LATEST_EXE:
            {
                WORD Version,CurVersion;
                FILE *fp,*fopen();

				bWSSyncFix = FALSE;
				bWSSaidHi = FALSE;

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

				/*	1/18/91 
					For Win21 users we will ignore their shopper version so that
					we don't have to maintain Win21 versions of shopper
					we do this by telling versions of shopper running under 
					Win21 that they are update irregardless. If the customer 
					updates to Win30 they will be updated as always  */
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
}

int
Receive_Message(HANDLE hCom, MSGTYPE *msgP)
{
    MSGTYPE msg2;

    while(1)
    {
        /* wait nTimeout seconds before timing out */
        if (ReceiveBuf(hCom,msgP,sizeof(MSGTYPE),nTimeout) != OK)
		{
			printf("Timed out waiting to recieve 1st msg\n");
			return FAIL;
		}

        if (ReceiveBuf(hCom,&msg2,sizeof(MSGTYPE),0) != OK)
           continue;

        /* redundancy for error checking */
        if (*msgP == msg2)
            return OK;
		else
			printf("Recieved unequal msg's: M1: 0x%X, M2: 0x%X\n", *msgP, msg2);
    }
}

int
GetPaddedBuf(HANDLE hCom, WORD nNumChars, char *szBuf, BOOL upperize)
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
            for(p=szBuf; *p; ++p)
                *p = toupper(*p);
        return OK;
    }
}

#include <sys/dir.h>
int
GetExt(char *szFileName)
{
	extern 	BOOL IsRightFormat(char *szFileName);
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
		while ((nread = read(fd, &dlink, sizeof(struct direct))) == 
								   (sizeof(struct direct)))
		{
			if (dlink.d_ino)
				if (dname = strstr(dlink.d_name,szFileName))
				{
					strcpy(szFileName,dlink.d_name);
					close(fd);
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
		ExitParent(sig);
}

static int
SetSigHandler(void)
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

openWSPort(BOOL bHiSpeed)
{
    int nRetval = OK;
    char buf;
	//use high DTE speed for pp9600 cause its buffered
    int tmpbaud = (bHiSpeed ? 9600 : 2400);
	char *psz, szResultBuf[255];
	BOOL bSaidHi, bTerminate = FALSE;

	if (GlobalhCom)
	{
		HangUp(GlobalhCom,savetty);
		close(GlobalhCom);
		GlobalhCom = NULL;
	}

	if (hOrigCom)
	{
		restore_tty(hOrigCom);
		close(hOrigCom);
	}

	if ((hOrigCom = open(savetty,O_RDWR|O_NDELAY)) < 0)
	{
		printf("(3)Error opening %s\n",savetty);
		END(FAIL)
	}
	save_tty(hOrigCom); // for restore_tty()

	/* in case these need resetting: */
	setraw(hOrigCom);
	set_tty_baud(hOrigCom,tmpbaud);
	set_tty_local(hOrigCom,TRUE);

	/* this'll guarantee return values below */
	WriteModemCommand(hOrigCom,"+~+~+~~~ATE0V0Q0S0=1X4\r~~~");

	/* clear return values */
	flush(hOrigCom,2);

	// MNP modem, kill compression: -????? - beng
	// \Q3  or K3 sets CTS and RTS hardware flow control (factory default)
	// 9600 modem, kill compression:
	if (bHiSpeed)
	{
		FILE*	fpInit9600;
		char	szInit9600[255];
		int		nInitLen;

		if ((fpInit9600 = fopen("/usr/bbs/master.bin/init9600", "r")) == NULL)
		{
			printf("Can't open Modem init file\n");
			END(FAIL)
		}
		if (fgets(szInit9600, 255, fpInit9600) == NULL)
		{
			printf("Can't read Modem init string\n");
			END(FAIL)
		}
		if (fclose(fpInit9600) != 0)
		{
			printf("Can't close Modem init file\n");
			END(FAIL)
		}

		if ((nInitLen = strlen(szInit9600)) == 255)
		{
			printf("9600 modem init string exceeds buffer\n");
			END(FAIL)
		}
		//modem wants carridge return instead of linefeed
		szInit9600[nInitLen-1] = '\r';	
		WriteModemCommand(hOrigCom, szInit9600);
		/* clear return values */
        //flush(hOrigCom,2);
		printf("Modem initialized: %s\n", szInit9600);

#if 0
		printf("PP9600 responded:\n");
		while (nRetval = read(hOrigCom,&buf,1))
		{
			if (buf != '\r')
				printf("%c", buf);
		}
		printf("\n");
#endif
	}

#if 0
	printf("Current modem profile:\n");
	WriteModemCommand(hOrigCom,"+~+~+~~~AT&V\r~~~~~~~~~");
	while (nRetval = read(hOrigCom,&buf,1))
	{
		if (buf != '\r')
			printf("%c", buf);
	}
	printf("\n");
#endif

	/* clear return values */
	flush(hOrigCom,2);

	/* so open will block: */
	set_tty_local(hOrigCom,FALSE);

#if 0
	if (bHiSpeed)	//Set hardware flow control
	{
		printf("Setting flow control\n");
		set_tty_flow(hOrigCom);
	}
#endif

	setpgrp(); // this causes the next opened tty to issue a
			   // SIGHUP when the device closes (the next opened
			   // device becomes the 'controlling' tty.

	printf("Waiting at the phone...\n");

	//This open call blocks until the carrier detect flag is raised
	if((GlobalhCom = open(savetty,O_RDWR)) < 0)
	{
		printf("Open %s failed\n", savetty);
		END(FAIL)
	}

	time(&clock);
	printf("Connected at %s\n", ctime(&clock));

	set_tty_local(WS_PORT,TRUE);

/*********** E S T A B L I S H   C O N N E C T I O N  ********************
	Read from the modem (WS_PORT) until the SAY_HI msg is recieved, everything
	preceding it is result codes from the modem or negotiation garbage.
	TimeToWait = 120 secs cause 9600 feature negotiation can take a while.
	Result codes from the modem are terminated with a CR.
	We fill up the result buffer until a CR is recieved, than we 
	try and interperate it and set the appropriate flags.
	We keep doing this until we get two consecutive SAY_HI msg's from Window
	Shopper or we fill up the buffer or timeout waiting for the next character.
******************************************************************************/
	bSaidHi = FALSE;
	tmpbaud = 0;
	while (1)
	{
		if (tmpbaud)	//tmpbaud got set via result code
		{
			set_tty_baud(WS_PORT, tmpbaud);
			set_tty_local(WS_PORT,FALSE);
			set_tty_xclude(WS_PORT,TRUE);
		}

		psz = szResultBuf;
		bTerminate = FALSE;
		while (psz-szResultBuf < sizeof(szResultBuf)) 
		{
			if (ReceiveBuf(WS_PORT, psz, 1, 120) != OK)
			{
				printf("Timed out reading from %s\n", WS_PORT);
				END (FAIL)
			}
			if (*psz == '\r')
			{
				bTerminate = TRUE;
				break;
			}
			else if (*psz == MSG_SAY_HI)
			{
				if (bSaidHi)
					END(OK)
				else
					bSaidHi = TRUE;
			}
			else
			{
				bSaidHi = FALSE;
				psz++;
			}
		}
		*psz = '\0';	//trim \r

		if (NOT (szResultBuf[0] AND bTerminate))
		{
			psz = szResultBuf;
			printf("Empty result buf or no terminate char:\n");
			while (*psz)
			{
				if (isalnum(*psz))
					printf("%c", *psz);
				else
					printf("(#%d)", *psz);
				++psz;
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
			printf("DING-A-LING-A-LING!!!!\n");
			continue;
		} else	
		if (strcmp(szResultBuf, RC_CONNECT_300) == 0)
		{
			printf("Connect at 300 %s\n", bHiSpeed ? "(DTE to DCE)" : "");
			tmpbaud = 300;
			continue;
		} else	
		if (strcmp(szResultBuf, RC_CONNECT_1200) == 0)
		{
			printf("Connect at 1200 %s\n", bHiSpeed ? "(DTE to DCE)" : "");
			tmpbaud = 1200;
			continue;
		} else	
		if (strcmp(szResultBuf, RC_CONNECT_2400) == 0)
		{
			printf("Connect at 2400 %s\n", bHiSpeed ? "(DTE to DCE)" : "");
			tmpbaud = 2400;
			continue;
		} else	
		if (strcmp(szResultBuf, RC_CONNECT_4800) == 0)
		{
			printf("Connect at 4800 %s\n", bHiSpeed ? "(DTE to DCE)" : "");
			tmpbaud = 4800;
			continue;
		} else	
		if (strcmp(szResultBuf, RC_CONNECT_9600) == 0)
		{
			printf("Connect at 9600 %s\n", bHiSpeed ? "(DTE to DCE)" : "");
			tmpbaud = 9600;
			continue;
		} else	
		if (strcmp(szResultBuf, RC_CONNECT_19200) == 0)
		{
			printf("Connect at 19200 %s\n", bHiSpeed ? "(DTE to DCE)" : "");
			tmpbaud = 19200;
			continue;
		} else	
		if (strcmp(szResultBuf, RC_CONNECT_38400) == 0)
		{
			printf("Connect at 38400 %s\n", bHiSpeed ? "(DTE to DCE)" : "");
			tmpbaud = 38400;
			continue;
		} else	
		if (strcmp(szResultBuf, RC_CARRIER_300) == 0)
		{
			printf("Carrier at 300!\n");
			tmpbaud = 300;
			continue;
		} else	
		if (strcmp(szResultBuf, RC_CARRIER_1200) == 0)
		{
			printf("Carrier at 1200!\n");
			tmpbaud = 1200;
			continue;
		} else	
		if (strcmp(szResultBuf, RC_CARRIER_2400) == 0)
		{
			printf("Carrier at 2400!\n");
			tmpbaud = 2400;
			continue;
		} else	
		if (strcmp(szResultBuf, RC_CARRIER_4800) == 0)
		{
			printf("Carrier at 4800!\n");
			tmpbaud = 4800;
			continue;
		} else	
		if (strcmp(szResultBuf, RC_CARRIER_9600) == 0)
		{
			printf("Carrier at 9600!\n");
			tmpbaud = 9600;
			continue;
		} else	
		if (strcmp(szResultBuf, RC_COMPRESSION_MNP5) == 0)
		{
			printf("MNP/5 Data Compression enabled!\n");
			continue;
		} else	
		if (strcmp(szResultBuf, RC_COMPRESSION_V42BIS) == 0)
		{
			printf("V.42bis Data Compression enabled!\n");
			continue;
		} else	
		if (strcmp(szResultBuf, RC_COMPRESSION_NONE) == 0)
		{
			printf("Data Compression disabled!\n");
			continue;
		} else	
		if (strcmp(szResultBuf, RC_PROTOCOL_NONE) == 0)
		{
			printf("Modem in standard async mode\n");
			continue;
		} else	
		if (strcmp(szResultBuf, RC_PROTOCOL_V42LAPM) == 0)
		{
			printf("Modem in V.42 LAPM Error Control mode\n");
			continue;
		} else	
		if (strcmp(szResultBuf, RC_PROTOCOL_MNP) == 0)
		{
			printf("Modem in MNP Error Control mode\n");
			continue;
		} 
		else	//BIG TROUBLE!!
		{
			char* cp = szResultBuf;
			printf("Got non-standard result code:\n");
			while (*cp)
			{
				if (NOT isalnum(*cp))
					printf("#%d ", *cp);
				else
					printf("%c", *cp);
				cp++;
			}
			printf("\n");
			continue;	//got garbage, keep reading
		}
	}

    end:
	setblock(GlobalhCom,OFF);
    if (nRetval == FAIL)
    {
        printf("(4)Error opening %s\n",savetty);
		if (bHiSpeed)
		{
			printf("Trying to decipher hangup reason:\n");
			WriteModemCommand(WS_PORT,"+~+~+~~~AT&S86?\r~~~");
			while (nRetval = read(WS_PORT,&buf,1))
			{
				printf("Recieved (#%d)\n", buf);
			}
		}
		else
			flush(WS_PORT,2);
        set_tty_local(WS_PORT,FALSE);
    }
    else
    {
        //flush(WS_PORT,2);

		//if (NOT bHiSpeed)
			//set_tty_baud(WS_PORT, tmpbaud);

        //set_tty_local(WS_PORT,FALSE);
        //set_tty_xclude(WS_PORT,TRUE);

		nRetval = setnewuser(); 
    }
	return nRetval;
}

ShowCust(ADDRESS_STRUCT *CustInfo)
{
    printf("CustInfo:\n");
    printf("   %s\n",CustInfo->name);
    printf("   %s\n",CustInfo->company);
    printf("   %s\n",CustInfo->street);
    printf("   %s\n",CustInfo->street2);
    printf("   %s, ",CustInfo->city);
    printf("   %s  ",CustInfo->state);
    printf("   %s\n",CustInfo->postal_code);
    printf("   %s  \n",CustInfo->province);
    printf("   %d: ",CustInfo->whichCountry);
    printf("   %s\n",CustInfo->country);
    printf("   (%s)",CustInfo->phone.area);
    printf("   %s - ",CustInfo->phone.number);
    printf("   %s\n",CustInfo->phone.ext);
}
ShowOrder(ORDER_STRUCT *CurOrder)
{
    printf("CurOrder:\n");
    //printf("   %s\n",CurOrder->libName);
    printf("   %ld\n",CurOrder->libID);
    //printf("   %s\n",CurOrder->clipDesc);
    printf("   %lx\n",CurOrder->clipID);
    //printf("   %s\n",CurOrder->vendorName);
    printf("   %lx\n",CurOrder->vendorID);
    printf("   %s\n",CurOrder->ShipAddress.name);
    printf("   %s\n",CurOrder->ShipAddress.street);
    printf("   %s\n",CurOrder->ShipAddress.street2);
    printf("   %s\n",CurOrder->ShipAddress.city);
    printf("   %s\n",CurOrder->ShipAddress.state);
    printf("   %s\n",CurOrder->ShipAddress.province);
    printf("   %s\n",CurOrder->ShipAddress.country);
    printf("   %s\n",CurOrder->ShipAddress.postal_code);
    printf("   %s\n",CurOrder->ccName);
    printf("   %s\n",CurOrder->ccNum);
    printf("   %s\n",CurOrder->ccExpDate);
    printf("   %d\n",CurOrder->ccType);
    printf("   %d\n",CurOrder->Quantity);
    printf("   %u\n",(WORD)CurOrder->priceTotal);
    printf("   %u\n",(WORD)CurOrder->priceEach);
    printf("   %d\n",CurOrder->mediaType);
    printf("   %d\n",CurOrder->CourierType);
    printf("   %lx\n",CurOrder->flags);
}

BOOL
GetThatMoola(ORDER_STRUCT *Order, CCReq *ccreq, BOOL bCheckLocalCredit)
/* returns whether credit captured */
{
    int nRetval = EOT;
	BYTE pstatus;				/* Product availability */

    strcpy(ccreq->AcctNum,Order->ccNum);
    strcpy(ccreq->ExpDate,Order->ccExpDate);
	ccreq->CCType = Order->ccType;
    ccreq->Amount = (float)Order->priceTotal / 100.0;
    ccreq->MsgCode = SALE;
    ccreq->Force = FORCE;
    ccreq->ReSend = NO_RESEND;
    ccreq->Result = 0;
    ccreq->Route = LOCAL;

    /* set up order */
	if (Acct_NewID(&Order->orderID,ACCT_ID_ORDER, NULL) != OK)
    {
		printf("didn't even get an order id!\n");
        /* note at this point we haven't created an order record */
	    if (SendByte(WS_PORT,(BYTE)EOT) != OK)
		    ;
	    return(FALSE);
	}

    /* create a new order record */
    Order->userID = lCurUserID;
	Order->sessionID = lSessionID;
	Order->flags |= OF_INITIALIZE;
	if (Acct_NormalOrder ( Order, ccreq ) != OK)
	{
		printf("didn't get an order record\n");
        /* note at this point we haven't created an order record */
	    if (SendByte(WS_PORT,(BYTE)EOT) != OK)
		    ;
	    return(FALSE);
	}

    //printf("CCReq fields from GetThatMoola pre CaptureCredit:\n");
	//PrintCCReq(ccreq);

    if (Order->priceTotal == 0L)
		if (Order->prodType == CLIP)
		{
			printf("Free ClipArt\n");
		    ccreq->Result = nCAPTURE;
			END(ACK)
		}
		else // no price???
		{
			printf("Error: no price on product!!!\n");
		    ccreq->Result = nAMOUNT_ERR;
			END(EOT)
		}

	if (Order->ccType == CORP)
	{
	    if (Acct_CorpAcctApproval(Order->userID, 
			Order->ccNum, 
			Order->priceTotal, 
			&ccreq->Result) != OK)
		{
			printf("didn't get corporate money\n");
			END(EOT)
		}
	}
	else
	{
        int Blocked;

        if (ExpDateCheck(ccreq) != OK)
		{
			printf("credit date check failed\n");
			ccreq->Result = -1;
			END(EOT)
		}
		else if (ccreq->Result == nDATE_ERR)
		{
			printf("credit expired locally\n");
			END(NAK)
		}

        /* see whether their credit is blocked */
	    if (bCheckLocalCredit)
		{
			if (Acct_CustCredit(lCurUserID,&Blocked) != OK)
			{
				printf("credit blocked failed\n");
				ccreq->Result = -1;
				END(EOT)
			}

			if (Blocked >= 2)
			{
				printf("credit denied locally\n");
				ccreq->Result = -2; 
				END(NAK)
			}
			else if (Blocked < 0)
			{
				printf("unable to check credit locally\n");
				ccreq->Result = -1;
				END(EOT)
			}
		} // else not blocked ....

#if 0
		if (CaptureCredit(ccreq, ExeType) != OK)
		{
			printf("didn't capture credit\n");
			END(EOT)
		}
#else
		ccreq->Result = PUNT;
#endif
	}
		switch (ccreq->Result)
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

	end:
	if (SendByte(WS_PORT,(BYTE)nRetval) != OK)
		;

#if 0
    if (Port)
	/* hang up credit card line now that we've responded to WS. */
	{
		HangUp(Port,NULL);
		//restore_tty(Port);
		close(Port);
		Port = NULL;
	}
#endif

	/* record result of transaction */
	Order->flags |= OF_CC_RESULT;
	Acct_NormalOrder ( Order, ccreq ) ;

	return(nRetval == ACK ? TRUE : FALSE);
}

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
			printf("Unable to close user (x%lX)(%ld) session (%ld)\n",
					lCurUserID,lCurUserID,lSessionID);
		else
			printf("Closed user (x%lX)(%ld) session (%ld)\n",
					lCurUserID,lCurUserID,lSessionID);
		Acct_Finalize();
		lSessionID = 0L;
	}
}

void
ReceiveThatOrder(ORDER_STRUCT *Order)
{
	if (ReceiveCRC(WS_PORT,(BYTE *)Order,sizeof(ORDER_STRUCT)) != OK)
	{
	    printf("Current order not received (%x)\n",sizeof(ORDER_STRUCT));
		// ShowOrder(Order);
	    if (SendByte(WS_PORT,NAK) != OK)
					;
	}
	else
	{
	    //printf("Current order received\n");
	    if (SendByte(WS_PORT,ACK) != OK)
			printf("Error ACK'ing Order\n");
	    //ShowOrder(Order);
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


ExitChild(int CondCode)
{
    printf("Exiting child...\n");

    Cleanup(CondCode);

	printf("Bye from child\n");
	fflush(stdout);
	exit(CondCode);
}

ExitParent(int CondCode)
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
 
    /* in case child was gone already */
	if (NOT bIngresAbort)
		CloseUserSession(CondCode);
	bIngresAbort = FALSE;

	printf("Total outgoing traffic: %ld bytes\n", outbytes); 
	printf("Total incoming traffic: %ld bytes\n", inbytes); 

    if (childPID) // then we're the parent, and GlobalhCom doesn't work
	{
		HangUp(NULL,savetty); // force hangup
		if (GlobalhCom)
		{
			close(GlobalhCom);
			GlobalhCom = NULL;
		}
		//Give modem a chance to drop the line
		sleep(5);
	}
	else // we're the child
	{
		if (GlobalhCom)
		{
			HangUp(GlobalhCom,savetty);
			close(GlobalhCom);
			GlobalhCom = NULL;
		}
	}

	if (hOrigCom)
	{
		//so won't answer phone...
		set_tty_local(hOrigCom,TRUE);
		//see hangup() WriteModemCommand(hOrigCom,"ATS0=0\r"); 
		restore_tty(hOrigCom);
		close(hOrigCom);
		hOrigCom=NULL;
	}

	/* from credit card: */
	if (Port)
	{
		HangUp(Port,NULL);
		//restore_tty(Port);
		close(Port);
		Port = NULL;
	}

#ifdef TEST_SERVER
	disconnect_from_server(cc_server);
#endif
}

ShowShip(Shipcost_Ary CostMatrix)
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
//	Get password via Acct_CustGetPassword. Check against password parameter.
//	Return OK if they match, FAIL if they don't.
//
// HISTORY:
//	3/13/91 - beng
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
