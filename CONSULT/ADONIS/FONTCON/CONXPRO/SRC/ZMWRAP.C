/*
 * ZMWRAP.C
 *
 * This file 'wraps' the Zmodem implementation that is found
 * in ZMODEM.C.  That implementation is shared between Xenix
 * and Windows.
 *
 * Revisions:
 *    4/20/92  cek   Wrote it.
 */

#define IHATEUNIX

#include "winunix.h"
#include "zmodem.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "comm.h"

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
BOOL ZmodemSendFiles( int iPort, PSTR pszFiles )
{
   LRESULT  lr ;
   void*    pZmodem ;
   UINT     cb ;
   LPARAM   lParam1, lParam2 ;

   ghCom = iPort ;   // used by fnSendZmodem

   printf( "ZmodemSendFiles:\n     " );


   pZmodem = Zmodem( NULL, ZM_INIT, fnSendZmodem, NULL ) ;

   if ((UINT)pZmodem == ZMS_ERROR)
   {
      /*  Bummer.
       */
      printf( "   Zmodem( ZM_INIT ) returned faliure!" ) ;
      return FALSE ;
   }

   if (ZMS_OK != Zmodem( pZmodem, ZM_SEND, pszFiles, NULL ))
   {
      /*  Bummer.
       */
      printf( "   Zmodem( ZM_RECEIVE ) failed!" ) ;

      Zmodem( pZmodem, ZM_KILL, 0L, 0L ) ;

      return FALSE ;
   }


   /*
    * In Windows we are timer driven.  Here we just sit in
    * a loop.
    */
   for (;;;)
   {
      /* Force Zmodem() to munch...
       */
      Zmodem( pZmodem, ZM_TIMER, 0L, 0L ) ;

      #ifdef OPTIMALSOLUTIONIFIKNEWUNIX
      /*
       * Is there data waiting?
       */
      if (cb = PeekCommData( iPort ))
      {
         PBYTE pBuf ;

         if (pBuf = malloc( cb ))
         {
            if (ReceiveBuf( iPort, pBuf, cb, STD_WAIT ) != OK)
            {
               printf( "RecieveBuf failed in Zmodem!!!" ) ;
               free( pBuf ) ;
               Zmodem( pZmodem, ZM_KILL, 0L, 0L ) ;
               return FALSE ;
            }

            Zmodem( pZmodem, ZM_RXBLK, cb, pBuf ) ;

            free( pBuf ) ;
         }
      }
      #else
      {
         BYTE  b ;

         /*
          * HACK!!!  I don't know unix well enough to figure out
          * how to "PeekComData()" to see if we have any data
          * waiting so I just read a byte at a time...hardly
          * optimal.
          *
          * Since we're doing a send there's not going to be
          * much incomming data...thank god!
          */

         /*
          * HACK #2!!! We ignore the return value from
          * ReceiveBuf and only wait 1 second.
          */
         if (ReceiveBuf( iPort, &b, 1, 1 ) == OK)
            Zmodem( pZmodem, ZM_RXBLK, 1, &b ) ;

      }

      #endif

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
            Zmodem( pZmodem, ZM_ACK_FILECOMPLETED, lParam1, 0L ) ;
         break ;

         case ZMS_COMPLETED:
            /*
             * This status indicates that all of the files we're sending
             * went a-ok!
             */
            Zmodem( pZmodem, ZM_ACK_COMPLETED, 0L, 0L ) ;
            Zmodem( pZmodem, ZM_KILL, 0L, 0L ) ;

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
   if (SendBuf( ghCom, lp, cb ) != OK)
      return FALSE ;

   return TRUE ;

} /* fnSendBuffer()  */

#if 0
UINT PeekCommData( int hCom )
{

}
#endif

#ifdef TEST
void main()
{
   ZmodemSendFiles( "foo.c\0bar.c\0hello.world\0" ) ;

}
#endif


/*
 * End of ZMWRAP.C
 */
