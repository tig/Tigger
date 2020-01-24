
#include "stdinc.h"
#include "stdcomm.h" 
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include "comm.h"
#include "xmodem.h"
#include "bbs.h"

int
XMSend(HANDLE hCom, HANDLE iFilePtr)
{
    int nBytesRead;
    int nNumberOfBlocks;
    BYTE c[2];
	XMBLOCK XMBlock;
	BOOL bGotResponse,bResend;

    /** wait for first nak **/
	// printf("downloading ...\n");
	//printf("waiting for NAK...\n");

    while(1)
	{
		if (ReceiveBuf(hCom,c,2,STD_WAIT) != OK)
		{
			printf("Timeout receiving ACK\n");
			return FAIL;
		}
		if ((c[0] == NAK) AND (c[1] == NAK))
		{
			//printf("Received NAK\n");
			break;
		}
		else
		{
			flush(hCom,2);
			printf("Received %x%x instead of NAK\n",c[0],c[1]);
			return FAIL;
		}
	}

    if (iFilePtr == -1)
	{
		SendByte(hCom,EOT);	
		SendByte(hCom,EOT);	
		return FAIL;
	}

    lseek(iFilePtr,0L,SEEK_SET);

    /** read header block (we're not going to send that one) **/
    if((nBytesRead = read(iFilePtr,&XMBlock,sizeof(XMBLOCK))) < sizeof(XMBLOCK))
    {
		printf("Error Reading File %d \n",nBytesRead);
        return(FAIL);
    }

    /** header contains number of blocks **/
	for (nNumberOfBlocks = (*(WORD *)&(XMBlock.Data[TOTALBLOCKNUM_INDEX]))-1;
	     nNumberOfBlocks--;
		 )
	{
        /** read one block **/
	   if((nBytesRead = read(iFilePtr,&XMBlock,sizeof(XMBLOCK))) 
		   != sizeof(XMBLOCK))
		{
		    printf("Error Reading File %d \n",nBytesRead);
            return(FAIL);
		}

        /** send block until receive a ACK **/
		bResend = TRUE;
		do // send the current block
		{
			//printf("Sending Block %d\n",nNumberOfBlocks);

			if (SendBuf(hCom, (BYTE *)&XMBlock, sizeof(XMBLOCK)) != OK)
				printf("Error writing block");

			bGotResponse = FALSE;
			do // wait for a valid response from WS
			{
				if (ReceiveBuf(hCom,c,2,300) != OK) // get two ACK/NAKs
					printf("Timeout receiving ACK\n");
					;
					 /* note that this can get in an infinite loop if
					 Window Shopper has disappeared for some reason. 
					 But I want to wait indefinitely in case the user
					 is grabbing the processor innocently computing
					 a spreadsheet or something.  -dug */
				switch(c[0])
				{
					case CTRLX:
					if (c[1] == CTRLX)
					{
						printf("XSend aborted by CRTL-X\n");
						flush(hCom,2);
						return FAIL;
					}
					break;

					case ACK:
						if (c[1] == ACK)
						{
							bGotResponse = TRUE;
							bResend = FALSE;
						}
					break;  // to next block

					case NAK:
						if (c[1] == NAK)
						{
							bGotResponse = TRUE;
							bResend = TRUE;
							//pause to allow cause of error to pass?
							wait(500L);
							printf("xmodem:  resending block\n");
						}
					break;
				}
				if (bGotResponse == FALSE)
				{
					flush(hCom,2); // flush garbage
				}
			}
			while (bGotResponse == FALSE);
		}
		while (bResend);
    }
    //printf("Sending EOT's...\n");
    SendByte(hCom,EOT);	
    SendByte(hCom,EOT);	
    return(OK);
}


#if 0
static int
XMReceive(char *szFileName)
{
    int fp= -1;
    BYTE cTemp[2]; /* a place to save the CRC's */
    BYTE *pBuf;
    XMBLOCK BlockBuf;
    BYTE BlockNum;
    int nRetval=OK,nRetry;
    HANDLE hCom;

//    AddFullPath(szFileName);

    if(SendByte(hCom,NAK,1) != OK)
        END(FAIL)

    if ((fp = open(szFileName,O_RDWR|O_CREAT)) == -1) 
        END(FAIL)

    BlockNum=1;
    nRetry=0;

    while(1)
    /** read blocks **/
    {
        int nNumRead,nNumLeft;

        Elapsed_Time(TRUE);

        /** read a block **/
        for (pBuf = (BYTE *)&BlockBuf, nNumLeft = sizeof(XMBLOCK); 
                nNumLeft; 
                nNumLeft -= nNumRead, pBuf += nNumRead)
        {
            while ((nNumRead= read(hCom,pBuf,nNumLeft)) <= 0) ;

            if (nNumRead == 2)
                if ((((char *)&BlockBuf)[0] == EOT) AND (((char *)&BlockBuf)[1] == EOT))
                    END(OK) // All Done
        }

        /** see if all done **/
        if (BlockBuf.SOH != soh)
                printf("Error receiving block\n");

        /** see if block number jibes **/
        else if ((BlockBuf.BlockNum == BlockNum) AND 
                 (BlockBuf.BlockNumCom != (~BlockNum)))
        {
            /* thanks to Intel we must swap the two crc bytes */
            cTemp[1]  = BlockBuf.CRC1;
            cTemp[0]  = BlockBuf.CRC2;

            /* make CRC is correct */
            if (* (WORD *)cTemp == CalcCRC((BYTE *)BlockBuf.Data,sizeof(BlockBuf.Data)))
            /* OK so write data to file */
            { 
                if(write(fp,BlockBuf.Data,sizeof(BlockBuf.Data)) != sizeof(BlockBuf.Data))
                    END(FAIL)

                if (SendBuf(hCom,szACK,sizeof(szACK)-1) != OK)
                    END(FAIL)

                ++BlockNum;
                nRetry=0;
                continue; // back up to top of read block loop
            }
        }

        /** if we get to here then we gotta retry **/
        if(nRetry == MAXRETRIES) // tried long enuf
        {
	        SendBuf(hCom,szCTRLX,sizeof(szCTRLX)-1);
            END(FAIL)
        }
        else
        {
            SendBuf(hCom,szNAK,sizeof(szNAK)-1);
            ++nRetry;
        }

    } /* end of read block loop */

    end:
    if (fp != -1)
        close(fp);

    if (nRetval == FAIL)
	    SendBuf(hCom,szCTRLX,strlen(szCTRLX));
        
    return (nRetval);
}
#endif



