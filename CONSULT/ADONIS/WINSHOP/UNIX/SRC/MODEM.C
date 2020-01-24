/*************************************************************\
 ** FILE:  modem.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **  Modem access routines.
 **
 ** HISTORY:   
 **   Doug Kent  May 7, 1990 (created)
 **
\*************************************************************/
#include "stdinc.h"
#include "stdcomm.h"
#include "stdio.h"
#include "comm.h"
#include "modem.h"
#include "bbs.h"
#include <sys/types.h>
#include <fcntl.h>

#if 0
int 
Connect(char *whichtty, char *PhoneNum, WORD howlongtowait)
/** returns handle to port **/
{
    int nRetval=OK;
    BOOL bOpened=FALSE;
    HANDLE hCom;
	char buf[4];
    
    if ((hCom = open(whichtty,O_RDWR)) == -1)
        return -1;

    bOpened = TRUE;

	save_tty(hCom);

	if (set_tty_parms(hCom,1200,'n',8,1) != OK)
        END(FAIL)

    if (WriteModemCommand(hCom,"+~+~+") != OK)
        END(FAIL)

    /* this'll reset, hangup (if not already) **/
    if (WriteModemCommand(hCom,"~~~ATZ\r") != OK)
        END(FAIL)

    if (WriteModemCommand(hCom,"~ATE0V0Q0X4%C0S23=54\\Q3\r") != OK)
        END(FAIL)

    if (WriteModemCommand(hCom,"~ATDT") != OK)
        END(FAIL)

    {
	char PhoneNumber[60];
	sprintf(PhoneNumber,"%s\r",PhoneNum);
    if (WriteModemCommand(hCom,PhoneNumber) != OK)
        END(FAIL)
	}

    printf("Dialing...");
    do
    {
        switch (nRetval = WaitForConnection(hCom, howlongtowait))
        {
            case OK:
                printf("Connected");
				if (ReceiveBuf(hCom,buf,4,howlongtowait) != OK)
				{
					printf("failed initial handshake\n");
					END(FAIL)
				}
				if (set_tty_parms(hCom,1200,'e',7,1) != OK)
					END(FAIL)
            break;
            case COMM_BUSY:
            default:
                printf("Unable to Connect");
                END(FAIL)
            break;
        }
    }
    while (nRetval != OK);

    end:
    if ((nRetval != OK) AND bOpened)
    {
        HangUp(hCom);
    }

    return hCom;
}
#endif

int
WriteModemCommand(HANDLE hCom, char *szCommand)
{
    //printf("Writing modem command: %s\n",szCommand);

    for (;*szCommand; ++szCommand)
    {
        if (*szCommand == '~')
            Wait(500L); // wait for safety
        else
        {
            if (SendBuf(hCom,szCommand,1) != OK)
                return FAIL;
            //Wait(5L); // wait between characters
        }
    }
    return OK;
}


int
WaitForConnection(HANDLE hCom, WORD howlong)
{
    unsigned char buf[1];

	printf("waiting %d seconds for connection\n",howlong);
	while (1)
	{
		if (ReceiveBuf(hCom,buf,1,howlong) != OK)
			return FAIL;

		switch (buf[0])
		{
			default:
				 printf("got a %c instead\n",buf[0] & 0x7F);
				 flush(hCom,2);
				 return FAIL;
			break;
			case '1': 
				{
					if (read(hCom,buf,1))
					switch(buf[0])
					{
					 case '8':
						printf("Connect at 4800!\n");
						return OK;
					 break;
					 case '3':
						printf("Connect at 9600!\n");
						return OK;
					 break;
					 case '0':
						printf("Connect at 2400!\n");
						return OK;
					 break;
					 case '\r':
						printf("Connect at 300!\n");
						return OK;
					 break;
					}
				}
			break;

			case '5': 
				printf("Connect at 1200!\n");
				return OK;
			break;
		}
    }
	return FAIL;
}

#include <termio.h>
void FAR PASCAL
HangUp(HANDLE hCom, char *whichtty)
{
	char buf;
	BOOL bCloseIt = FALSE;

    if (hCom == 0)
	{
		HANDLE hTmp;
		bCloseIt = TRUE;
		if ((hTmp = open(whichtty,O_RDWR|O_NDELAY)) == -1)
		{
			printf("Error opening %s for hangup\n",savetty);
			return;
		}
		setraw(hTmp);
		set_tty_local(hTmp,TRUE);
		if ((hCom = open(whichtty,O_RDWR|O_NDELAY)) == -1)
		{
			printf("Error opening %s for hangup\n",savetty);
			return;
		}
		close(hTmp);
	}

	flush(hCom,2);
	set_tty_local(hCom,TRUE);
	WriteModemCommand(hCom,"+~+~+~~~ATH0S0=0~~~~~~\r");  // give time to hang up
	if (ReceiveBuf(hCom,&buf,1,7) != OK)
		//printf("Error closing %s\n",whichtty)
		;
	set_tty_baud(hCom,0);
	printf("modem hung up\n");
	//set_tty_local(hCom,FALSE);
	set_tty_xclude(hCom,FALSE);
	if (bCloseIt)
		close(hCom);
}

#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>
int
Wait(unsigned long mSEC)
{
	struct tms t;
	unsigned long starttime,curtime,times();
	#define MILLISEC_PER_CLOCKTICK (1000 / HZ) 
									   // last term for roundoff
	//printf("MILLI: %lx\n",MILLISEC_PER_CLOCKTICK);
	starttime = times(&t);
	//printf("starttime: %lx\n",starttime);
	do
	{
		curtime = times(&t);
		//printf("curtime: %lx\n",curtime);
	}
	while(((curtime - starttime) * MILLISEC_PER_CLOCKTICK) < mSEC);
}

