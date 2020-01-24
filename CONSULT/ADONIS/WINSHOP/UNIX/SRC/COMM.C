#include "stdinc.h"
#include "stdcomm.h" 
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include "bbs.h"
#include "comm.h"
#include "xmodem.h"
#include "time.h"
#include "util.h"
#include <termio.h>

char *savetty;    /* moved from bbs.c */
static BOOL bParity=FALSE;
extern int errno;

struct termio savetbuf;
extern long inbytes;
extern long outbytes;

int
set_tty_baud(HANDLE hCom, WORD baud)
{
	struct termio ttybuf;

	if (ioctl(hCom,TCGETA,&ttybuf) == -1)
	    //syserr("ioctl");
	    printf("baud ioctl failed\n");

	switch(baud)
	{
		case 0:
			/* hangup */
			ttybuf.c_cflag = (ttybuf.c_cflag & ~CBAUD) | B0;
		break;
		case 300:
			ttybuf.c_cflag = (ttybuf.c_cflag & ~CBAUD) | B300;
		break; 
		case 1200:
			ttybuf.c_cflag = (ttybuf.c_cflag & ~CBAUD) | B1200;
		break;
		case 2400:
			ttybuf.c_cflag = (ttybuf.c_cflag & ~CBAUD) | B2400;
		break;
		case 4800:
			ttybuf.c_cflag = (ttybuf.c_cflag & ~CBAUD) | B4800;
		break;
		case 9600:
			ttybuf.c_cflag = (ttybuf.c_cflag & ~CBAUD) | B9600;
		break;
		case 19200:
			ttybuf.c_cflag = (ttybuf.c_cflag & ~CBAUD) | B19200;
		break;
		case 38400:
			ttybuf.c_cflag = (ttybuf.c_cflag & ~CBAUD) | B38400;
		break;
	}
    return set_tty(hCom,&ttybuf);
}

/* Set hardware flow control for terminal */
int
set_tty_flow(HANDLE hCom)
{
	struct termio ttybuf;

	if (ioctl(hCom,TCGETA,&ttybuf) == -1)
	    //syserr("ioctl");
	    printf("local ioctl failed\n");

    ttybuf.c_cflag = ttybuf.c_cflag | CTSFLOW;

    return set_tty(hCom,&ttybuf);
}

int
set_tty_local(HANDLE hCom, BOOL bOnOff)
{
	struct termio ttybuf;

	if (ioctl(hCom,TCGETA,&ttybuf) == -1)
	    //syserr("ioctl");
	    printf("local ioctl failed\n");

    ttybuf.c_cflag = bOnOff ? (CLOCAL | ttybuf.c_cflag) :
							  (ttybuf.c_cflag & ~CLOCAL);

    return set_tty(hCom,&ttybuf);
}


int
set_tty_xclude(HANDLE hCom, BOOL bOnOff)
{
	struct termio ttybuf;

	if (ioctl(hCom,TCGETA,&ttybuf) == -1)
	    //syserr("ioctl");
	    printf("xclude ioctl failed\n");

    ttybuf.c_lflag = bOnOff ? (XCLUDE | ttybuf.c_lflag) :
							  (ttybuf.c_lflag & ~XCLUDE);

    return set_tty(hCom,&ttybuf);
}



int
set_tty_parms(HANDLE hCom, 
			  short baud, 
			  char parity, 
			  char databits, 
			  char stopbits)
{
	struct termio ttybuf;

    //printf("set_tty_parms: %d %d %c %d %d\n",hCom,baud,parity,databits,stopbits);

	memset(&ttybuf,0,sizeof(struct termio));

	switch(parity)
	{
		case 'e':
			/* note we're assuming never need zeroes */
		    ttybuf.c_iflag = INPCK | PARMRK;
			ttybuf.c_cflag = PARENB ;
			bParity = TRUE;
		break;
		case 'o':
			/* note we're assuming never need zeroes */
			ttybuf.c_iflag = INPCK;
			ttybuf.c_cflag = PARENB | PARODD;
			bParity = TRUE;
		break;
		case 'n':
			ttybuf.c_iflag = 0;
			ttybuf.c_cflag = 0;
			bParity = FALSE;
		break;
	}

	switch(baud)
	{
		case 300:
			ttybuf.c_cflag |= B300; 
		break; 
		case 1200:
			ttybuf.c_cflag |= B1200;
		break;
		case 2400:
			ttybuf.c_cflag |= B2400;
		break;
		case 4800:
			ttybuf.c_cflag |= B4800;
		break;
		case 9600:
			ttybuf.c_cflag |= B9600;
		break;
		case 19200:
			ttybuf.c_cflag |= B19200;
		break;
		case 38400:
			ttybuf.c_cflag |= B38400;
		break;
	}

	switch(databits)
	{
		case 5:
			ttybuf.c_cflag |= CS5;
		break;
		case 6:
			ttybuf.c_cflag |= CS6;
		break;
		case 7:
			ttybuf.c_cflag |= CS7;
		break;
		case 8:
			ttybuf.c_cflag |= CS8;
		break;
	}

	switch(stopbits)
	{
		case 1:
		break;
		case 2:
			ttybuf.c_cflag |= CSTOPB;
		break;
	}
	ttybuf.c_cc[VMIN] = 1; /* MIN */
	ttybuf.c_cc[VTIME] = 0; /* TIME */
	ttybuf.c_cflag |= CREAD | HUPCL;
	set_tty(hCom,&ttybuf);
	return OK;
}

int
setraw(HANDLE hCom)
{
	struct termio tbuf;

	if (ioctl(hCom,TCGETA,&tbuf) == -1)
	    //syserr("ioctl");
	    printf("setraw ioctl failed\n");

        tbuf.c_iflag = 0;
		bParity=FALSE;

        tbuf.c_cflag = B2400 | CS8 | CREAD | HUPCL;

        tbuf.c_oflag = 0;

        tbuf.c_lflag = 0; //XCLUDE;

		tbuf.c_line = 0;

        tbuf.c_cc[VMIN] = 0; /* MIN */
		tbuf.c_cc[VTIME] = 0; /* TIME */
		set_tty(hCom,&tbuf); 
}


void 
save_tty(hCom)
{
	if (ioctl(hCom,TCGETA, &savetbuf) == -1)
       	//syserr("ioctl2");
	    printf("save ioctl failed\n");
}

void 
restore_tty(hCom)
{
	bParity=FALSE; // a bit of a presumption here!
	if (ioctl(hCom,TCSETAF, &savetbuf) == -1)
       	//syserr("ioctl2");
	    printf("restore ioctl failed\n");
}


int
set_tty(HANDLE hCom, struct termio *ttymode)
{
	FILE *fopen(),*fp;
    //printf("set_tty...\n");

	if (ioctl(hCom,TCSETAW, ttymode) == -1)
	{
       	//syserr("ioctl2");
	    printf("set_tty ioctl failed\n");
		return FAIL;
	}
	//LookIO(hCom,fp = fopen("lookio.out","a+"));
	//fclose(fp);
    return OK;
}

void
flush(HANDLE hCom,int whichQ)
/* 0 for input, 1 for output, 2 for both */
{

	if (ioctl(hCom,TCFLSH,whichQ) == -1)
	    //syserr("ioctl");
	    printf("flush failed\n");
}


#if 0

void
setwaitparms(HANDLE hCom, int chars, int time)
{
	struct termio tbuf;
	if (ioctl(hCom,TCGETA,&tbuf) == -1)
	    //syserr("ioctl");
	    printf("first ioctl failed\n");

    tbuf.c_cc[VMIN] = chars; /* MIN  chars to read before returning */
	tbuf.c_cc[VTIME] = time; /* TIME */

	if (ioctl(hCom,TCSETA,&tbuf) == -1)
	    //syserr("ioctl");
	    printf("first ioctl failed\n");
	
}

void 
SendXoff(HANDLE hCom)
{
//	SendByte(hCom,xoff);
}

void 
SendXon(HANDLE hCom)
{
//	SendByte(hCom,xon);
}

#endif



int
SendByte(HANDLE hCom,BYTE ByteToSend)
{
    if (write(hCom,&ByteToSend,1) != 1)
    {
		printf("SendByte:  hCom is dead.\n");
        return FAIL;
    }
	++outbytes;
    return OK;
}


int
SendBuf(HANDLE hCom, BYTE *buf, int iBufLen)
{
	int nNumWritten,nNumLeft;

	for (nNumLeft = iBufLen; 
		 nNumLeft; 
		 nNumLeft -= nNumWritten, buf += nNumWritten)
	{
		if ((nNumWritten = write(hCom, buf, nNumLeft)) == 0)
			{
				printf("SendBuf:  hCom is dead.\n");
				return FAIL;
            }
	}
	outbytes = outbytes + iBufLen;
    return OK;
}


int
SendCRC(HANDLE hCom, BYTE *buf, int iBufLen)
{
    CRC Crc;

    if (SendBuf(hCom,buf,iBufLen) != OK)
	   return FAIL;

    Crc = CalcCRC(buf,iBufLen);
    if (SendBuf(hCom,(BYTE *)&Crc,2) != OK)
	   return FAIL;

    return OK;
}

int
ReceiveCRC(HANDLE hCom, BYTE *buf, int iBufLen)
{
    CRC Crc;

    if (ReceiveBuf(hCom,buf,iBufLen,STD_WAIT) != OK)
	   return FAIL;

    if (ReceiveBuf(hCom,(BYTE *)&Crc,sizeof(CRC),STD_WAIT) != OK)
	   return FAIL;

    /* make CRC is correct */
    if (Crc == CalcCRC(buf,iBufLen))
    {
//	printf("CRC is OK\n");
        return OK;
    }
    else
    {
	printf("Bad bad CRC (%x)\n",Crc);
	return FAIL;
    }
}

int
ReceiveBuf(HANDLE hCom, BYTE *buf, int iBufLen, WORD TimeToWait)
{
    int n_read;
    BYTE cTemp[2];
    CRC Crc;
    WORD nNumLeft,nNumRead;
    BYTE *pBuf;
	struct timeval timeout;
	struct timeval *tvp;
	fd_set readfds;
	char tbuf[2];

    if (bParity)			/* Gotta read one char at a time */
		for (pBuf = buf, nNumLeft = iBufLen; 
			nNumLeft; 
			nNumLeft -= nNumRead, pBuf += nNumRead)
		{
			if (TimeToWait)
			{
				timeout.tv_sec = TimeToWait;
				timeout.tv_usec = 0L;
				tvp = &timeout;
				FD_ZERO(&readfds);
				FD_SET(hCom,&readfds);
			}
			else tvp = NULL;
			switch (select(32,&readfds,NULL,NULL,tvp))
			{
				case -1:
					perror("ReceiveBuf: select");
					return FAIL;
				case 0:
					fprintf(stderr,"ReceiveBuf: timeout\n");
					return TIMEOUT_ERR;
			}
			switch (nNumRead = read(hCom,pBuf,1))
			{
				case -1:
					perror("ReceiveBuf: read");
					return FAIL;
				case 0:
					fprintf(stderr,"ReceiveBuf: no bytes read\n");
					return FAIL;
			}

			if (*pBuf == 0377)		/* parity error */
			{
				switch (n_read = read(hCom,tbuf,2))
				{
					case -1:
						perror("ReceiveBuf: parity read");
						return FAIL;
					case 0:
						fprintf(stderr,"ReceiveBuf: no parity bytes read\n");
						return FAIL;
					default:
						if (n_read != 2)
							fprintf(stderr,
								"ReceiveBuf: parity read: %d of 2 bytes read\n",
									n_read);
						return FAIL;
				}
				continue;
			}
			*pBuf &= 0x7F;			/* strip parity bit */
		}
	else
		for (pBuf = buf, nNumLeft = iBufLen; 
			nNumLeft; 
			nNumLeft -= nNumRead, pBuf += nNumRead)
		{
			if (TimeToWait)
			{
				timeout.tv_sec = TimeToWait;
				timeout.tv_usec = 0L;
				tvp = &timeout;
				FD_ZERO(&readfds);
				FD_SET(hCom,&readfds);
			}
			else tvp = NULL;
			switch (select(32,&readfds,NULL,NULL,tvp))
			{
				case -1:
					perror("ReceiveBuf: select 2");
					return FAIL;
				case 0:
					fprintf(stderr,"ReceiveBuf: timeout 2\n");
					return TIMEOUT_ERR;
			}
			switch (nNumRead= read(hCom,pBuf,nNumLeft))
			{
				case -1:
					perror("ReceiveBuf: read 2");
					return FAIL;
				case 0:
					fprintf(stderr,"ReceiveBuf: no bytes read 2\n");
					return FAIL;
			}
		}
	inbytes = inbytes + iBufLen;
	return OK;
}

int
ReceiveStr(HANDLE hCom, BYTE *buf, int iBufLen, char *str, WORD TimeToWait)
/* receive string matching str or return FAIL. */
{
    int n_read, i;
    BYTE cTemp[2];
    CRC Crc;
    WORD nNumLeft;
    BYTE *pBuf,*cBuf=str;
	char AllBuf[80];
	char Temp[80];
	struct timeval timeout;
	struct timeval *tvp;
	fd_set readfds;
	char tbuf[2];

	pBuf = buf; 
	nNumLeft = iBufLen; 
	i = 0;
	errno = 0;

	while (nNumLeft) 
	{
		if (TimeToWait)
		{
			timeout.tv_sec = TimeToWait;
			timeout.tv_usec = 0L;
			tvp = &timeout;
			FD_ZERO(&readfds);
			FD_SET(hCom,&readfds);
		}
		else tvp = NULL;
		switch (select(32,&readfds,NULL,NULL,tvp))
		{
			case -1:
				perror("ReceiveStr: select");
				return FAIL;
			case 0:
				fprintf(stderr,"ReceiveStr: timeout\n");
				return TIMEOUT_ERR;
		}
		switch (read(hCom,pBuf,1))
		{
			case -1:
				perror("ReceiveStr: read");
				return FAIL;
			case 0:
				fprintf(stderr,"ReceiveStr: no bytes read\n");
				return FAIL;
		}
		if (bParity)
		{
			if (*pBuf == 0377)		/* parity error */
			{
				switch (n_read = read(hCom,tbuf,2))
				{
					case -1:
						perror("ReceiveStr: parity read");
						return FAIL;
					case 0:
						fprintf(stderr,"ReceiveStr: no parity bytes read\n");
						return FAIL;
					default:
						if (n_read != 2)
							fprintf(stderr,
								"ReceiveStr: parity read: %d of 2 bytes read\n",
									n_read);
						return FAIL;
				}
			    //printf("ReceiveStr:  parity error, data = %c (%x)\n",
				  				 //tbuf[1],tbuf[1]);
			//	return PARITY_ERR;
				continue;
			}
			*pBuf &= 0x7F; // strip parity bit	
		}

		 AllBuf[i++] = *pBuf;

         if (*pBuf == *cBuf)
		 {
			 if (nNumLeft == 1)
				return OK; // all done
			 ++cBuf;
			 ++pBuf;
			 --nNumLeft;
		 }
		 else // start over
		 {
			 cBuf = str;
			 pBuf = buf;
			 nNumLeft = iBufLen;
		 }

	}

	printf("Failed in ReceiveStr, buffer = %s\n", UnCntrlStr(AllBuf,Temp,strlen(AllBuf)));
    return FAIL;
}

int
ReceiveVarBuf(HANDLE hCom, 
    BYTE *buf, 
	int iBufLen, 
	WORD TimeToWait, 
	BYTE TerminateChar,
	int *len)
	// can be NULL
{
	int nRetval=OK;
	WORD count = 0;
    BYTE *pBuf = buf,c;
	struct timeval timeout;
	struct timeval *tvp;
	fd_set readfds;

	do
	{
		if (TimeToWait)
		{
			timeout.tv_sec = TimeToWait;
			timeout.tv_usec = 0L;
			tvp = &timeout;
			FD_ZERO(&readfds);
			FD_SET(hCom,&readfds);
		}
		else tvp = NULL;
		switch (select(32,&readfds,NULL,NULL,tvp))
		{
			case -1:
				perror("ReceiveVarBuf: select");
				END(FAIL)
			case 0:
				fprintf(stderr,"ReceiveVarBuf: timeout\n");
				END(TIMEOUT_ERR)
		}
		switch (read(hCom,&c,1))
		{
			case -1:
				perror("ReceiveVarBuf: read");
				END(FAIL)
			case 0:
				fprintf(stderr,"ReceiveVarBuf: no bytes read\n");
				END(FAIL)
		}

		if ((bParity ? c : c & 0x7f) == TerminateChar)
        {
			//printf("ReceiveVarBuf: Got Terminate char\n");
			END(OK)
		}

		*pBuf++ =  bParity ? c : c & 0x7F;

		if (++count == iBufLen)
			END(FAIL)
	}
    while (1);

end:
	if (len)
		*len = count;
	return (nRetval);
}

time_t
Elapsed_Time(BOOL bReset)
{
    time_t curtime,timediff;
    static time_t lasttime;
	time_t time();

    /** time is in seconds **/

    if (bReset)
    {
        lasttime = time(NULL);
        return lasttime;
    }
    else
    {
        curtime = time(NULL);

        timediff = curtime - lasttime;
        return timediff;
    }
}

CRC
CalcCRC(BYTE *cCRCBuf,int dCRC_BUFFSIZE)
{
    CRC crc,i;    
    crc = 0;

//	 printf("bufsize %d\n",dCRC_BUFFSIZE);
    while(dCRC_BUFFSIZE--)
        {
         crc = crc^(CRC)*cCRCBuf++<<8;
         for(i = 0;i < 8;i++)
            {
             if(crc&0x08000) 
                {
                 crc = (crc<<1)^0x1021;
                }
             else
                {
                 crc = crc<<1;
                }
            }
        }
//		printf("CRC %x\n",crc);
        return(crc);
}

void
setblock(HANDLE hCom, BOOL onoff)
{
	int blockf,nonblockf;
	int flags;

	if ((flags = fcntl(hCom,F_GETFL,0)) == -1)
		perror("setblock");
	blockf = flags & ~O_NDELAY;
	nonblockf = flags | O_NDELAY;

	if (fcntl(hCom,F_SETFL, onoff ? blockf : nonblockf) == -1)
		perror("setblock2");
}
