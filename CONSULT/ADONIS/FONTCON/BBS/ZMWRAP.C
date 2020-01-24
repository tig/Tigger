/*
 * ZMWRAP.C
 *
 * This file 'wraps' the Zmodem implementation that is found
 * in ZMODEM.C.  That implementation is shared between Xenix
 * and Windows.
 *
 * Revisions:
 *    4/20/92  cek   Wrote it.
 *    5/11/92  cek   Wrote it some more after narf wrote it some too.
 */

/* size of the input buffer */

#define INBUFSIZE 1024

/* # of characters in a packet */

#define NCHARS 20

/* max allowable delay between chars (in 10ths of secs) */

#define IC_TIMER 5

/* # of seconds to wait before timing out */

#define TIMEOUT 15

#include "winunix.h"
#include "zmodem.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <termio.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>

/*
#include "stdcomm.h"
#include "comm.h"
*/
typedef WORD CRC ;
static int ghCom ;

UINT CALLBACK fnSendZmodem( HWND hwndZmodem, LPBYTE lp, UINT cb ) ;

/*************************************************************
 * ZmodemSendFiles()
 *
 * Sends the files pointed to by the lpszFiles parameter.
 *
 * The buffer pointed to by lpszFiles is a doubly-null terminated
 * list of filenames.  
 *
 * Returns TRUE if all files were sent ok
 *
 * Revisions:
 *   4/12/92  cek  Wrote this nifty comment block
 **************************************************************/
BOOL ZmodemSendFiles( int hCom, PSTR pszFiles )
{
   void*          pZmodem ;
   UINT           cb ;
   LPARAM         lParam1, lParam2 ;
   LRESULT        lr ;
   char           rgcInput[INBUFSIZE];
   struct termio  t;
   int            s;
   struct timeval tm;
   int            nOrigVMIN ;
   int            nOrigVTIME ;
   fd_set         ifd;

   ghCom = hCom ;   // used by fnSendZmodem

   printf( "ZmodemSendFiles( %d, %s ):\n", hCom, pszFiles ) ; 

   pZmodem = Zmodem( NULL, ZM_INIT, (LPARAM)fnSendZmodem, NULL ) ;

   if ((UINT)pZmodem == ZMS_ERROR)
   {
      printf( "   Zmodem( ZM_INIT ) returned faliure!\n" ) ;
      return FALSE ;
   }

   ioctl(hCom, TCGETA, &t);
   nOrigVMIN = t.c_cc[VMIN] ;
   nOrigVTIME = t.c_cc[VTIME] ;

   t.c_cc[VMIN] = NCHARS ;
   t.c_cc[VTIME] = IC_TIMER ;

   ioctl(hCom, TCSETA, &t);

   if (ZMS_OK != Zmodem( pZmodem, ZM_SEND, (LPARAM)pszFiles, NULL ))
   {
      printf( "   Zmodem( ZM_SEND ) failed!\n" ) ;
      Zmodem( pZmodem, ZM_KILL, 0L, 0L ) ;
      return FALSE ;
   }

   for(;;)
   {
      Zmodem( pZmodem, ZM_TIMER, 0L, 0L ) ;

      /* select is passe', mais nous n'avons pas de choix! */
      /* this just waits until input is ready or we time out */

      tm.tv_sec = TIMEOUT;
      tm.tv_usec = 0;

      FD_ZERO( &ifd ) ;
      FD_SET( hCom, &ifd ) ;

      if ((s = select(32, &ifd, NULL, NULL, &tm)) < 0)
      {
         ioctl(hCom, TCGETA, &t);
         t.c_cc[VMIN] = nOrigVMIN ;
         t.c_cc[VTIME] = nOrigVTIME;
         ioctl(hCom, TCSETA, &t);

         printf("Zmodem: serial error: %d\n", errno);
         return FALSE;
      }
      else if (s == 0)
      {
         ioctl(hCom, TCGETA, &t);
         t.c_cc[VMIN] = nOrigVMIN ;
         t.c_cc[VTIME] = nOrigVTIME;
         ioctl(hCom, TCSETA, &t);

         printf("Zmodem: select timeout\n");
         return FALSE;
      }

      if ((cb = read( hCom, rgcInput, INBUFSIZE )) < 0)
      {
         /* reset the driver params */

         ioctl(hCom, TCGETA, &t);
         t.c_cc[VMIN] = nOrigVMIN ;
         t.c_cc[VTIME] = nOrigVTIME;
         ioctl(hCom, TCSETA, &t);

         printf("Zmodem read failed!\n");
         return -1;
      }

      /* ship the data off to Zmodem */
      if (cb)
      {
         printf("Read %d chars...\n",cb);
         Zmodem(pZmodem, ZM_RXBLK, cb, rgcInput);
      }

      /*
       * Ask Zmodem() how much damage we did...
       */
      switch(lr = Zmodem( pZmodem, ZM_QUERYSTATUS, lParam1, lParam2 ))
      {
         case ZMS_SENDING:
            break ;

         case ZMS_RECEIVING:
         break ;

         case ZMS_FILECOMPLETED:
            printf( "Got ZMS_FILECOMPLETED!!!\n" ) ;
            Zmodem( pZmodem, ZM_ACK_FILECOMPLETED, lParam1, 0L ) ;
         break ;

         case ZMS_COMPLETED:
            /*
             * This status indicates that all of the files we're sending
             * went a-ok!
             */
            printf( "Got ZMS_COMPLETED!!!\n" ) ;

            Zmodem( pZmodem, ZM_ACK_COMPLETED, 0L, 0L ) ;
            Zmodem( pZmodem, ZM_KILL, 0L, 0L ) ;

            printf( "Zmodem completed.\n" ) ;

            ioctl(hCom, TCGETA, &t);
            t.c_cc[VMIN] = nOrigVMIN ;
            t.c_cc[VTIME] = nOrigVTIME;
            ioctl(hCom, TCSETA, &t);
            return TRUE ;
         break ;

         case ZMS_ERROR:
         case ZMS_LOCALCANCEL:
         case ZMS_TOOMANYERRORS:
         case ZMS_TIMEOUT:
         case ZMS_HOSTCANCEL:
         case ZMS_DISKERROR:
         case ZMS_DISKFULL:
         case ZMS_NAK:
         default:
            /*
             * This means the transfer was aborted (i.e. the
             * protocol detected an abort.  Treat this as an
             * error in communications.
             */

            Zmodem( pZmodem, ZM_KILL, 0L, 0L ) ;
   
            printf ("Zmodem returned an error code (%08X)\n", lr ) ;
            ioctl(hCom, TCGETA, &t);
            t.c_cc[VMIN] = nOrigVMIN ;
            t.c_cc[VTIME] = nOrigVTIME;
            ioctl(hCom, TCSETA, &t);
            return FALSE ;
         break ;
      }
   }

   /*
    * we'll never get here...
    */
   printf( "Zmodem Done!\n" ) ;
   return FALSE ;
 }


/****************************************************************
 *  UINT CALLBACK fnSend( HWND hwndZmodem, LPBYTE lp, UINT cb )
 *
 *  Description: 
 *
 *    Callback routine for ZModem sends.
 *
 *  Comments:
 *
 ****************************************************************/
UINT CALLBACK fnSendZmodem( HWND hwndZmodem, LPBYTE lp, UINT cb )
{
   printf( "Sending Zmodem Data: %d bytes\n", cb ) ;
   if (SendBuf( ghCom, lp, cb ) != 0)
   {
     printf( "  SendBuf() failed!\n" ) ;
      return FALSE ;
   }

   return TRUE ;

} /* fnSendBuffer()  */

#ifdef TEST
void main()
{
   ZmodemSendFiles( "foo.c\0bar.c\0hello.world\0" ) ;

}
#endif


#if 0
int ReceiveBuf(HANDLE hCom, BYTE *buf, int iBufLen, WORD TimeToWait)
{
   int n_read;
   BYTE cTemp[2];
   CRC Crc;
   WORD nNumLeft,nNumRead;
   BYTE *pBuf;
   struct timeval timeout;
   fd_set readfds;
   char tbuf[2];

   for (pBuf = buf, nNumLeft = iBufLen; 
         nNumLeft; 
         nNumLeft -= nNumRead, pBuf += nNumRead)
   {
      timeout.tv_sec = TimeToWait;
      timeout.tv_usec = 0L;
      FD_ZERO(&readfds);
      FD_SET(hCom,&readfds);

      switch (select(32,&readfds,NULL,NULL,&timeout))
      {
         case -1:
            printf("select 2\n");
         return ;

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

#endif

/*
 * End of ZMWRAP.C
 */
