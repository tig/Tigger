/************************************************************************
 *
 *     Project:  ZMODEM
 *
 *      Module:  zmsend.c
 *
 *     Remarks:
 *
 *    Handles all send side stuff for zmodem.
 *
 *   Revisions:  
 *    8/8/92   ckindel  Moved out of zmodem.c
 *
 ***********************************************************************/

#include "PRECOMP.H"   // precompiled header shit

#ifdef IHATEUNIX
   #include "winunix.h"
#endif

#include "izmodem.h"

VOID NEAR PASCAL SendBinData( NPZCB, int, LPSTR, int ) ;

VOID NEAR PASCAL ProcessZRINITHdr( NPZCB ) ;
VOID NEAR PASCAL ProcessZRPOS( NPZCB ) ;

BOOL NEAR PASCAL XmitZFILE( NPZCB ) ;
BOOL NEAR PASCAL XmitZDATA( NPZCB ) ;
VOID NEAR PASCAL XmitZEOF( NPZCB ) ;
VOID NEAR PASCAL XmitZRQINIT( NPZCB ) ;


/****************************************************************
 *  BOOL EXTERN HandleZM_SEND( NPZCB npZCB )
 *
 *  Description: 
 *
 *
 ****************************************************************/
BOOL EXTERN HandleZM_SEND( NPZCB npZCB, LPSTR lpFileList )
{
   /*
    * For the SEND and RECIEVE cases, we copy the buffer
    * passed to us into our own buffer FILELIST().
    * Then as we process files, we just copy then next entry
    * in the list into our FILENAME() member.
    *
    * The bitch is figuring out how long the buffer we've been
    * passed is, but hey..we're smart...we can figure it out.
    *
    */
   UINT  cbFileList ;
   LPSTR lpCur = lpFileList ;

   DP2( hWDB, "Zmodem : ZM_SEND" ) ;

   /*
      * If FILELIST() is non-NULL, then the caller did
      * ether a SEND or RECEIVE before without a KILL/INIT, thus
      * we must free the filelist buffer...
      */
   if (FILELIST( npZCB ))
   {
      GlobalFreePtr(  FILELIST( npZCB ) ) ;
      FILELIST( npZCB ) = NULL ;
      FILELISTNEXT( npZCB ) = NULL ;
   }

   if (lpFileList == ASCII_NUL || *lpFileList == ASCII_NUL)
   {
      /* It's an empty list which is illegal!
       */
      DP1( hWDB, "Zmodem : ZM_SEND with NULL filelist! " ) ;
      return FALSE ;
   }
   else
   {
      /* 
       * This little loop makes the assumption that the bozo that
       * called us *DID* actually give us a doubly-NULL-terminated
       * list...otherwise we're gonna go off the end of his
       * buffer fer sure!
       */
      cbFileList = 2 ;
      lpCur = lpFileList ;
      while(*lpCur || *(lpCur+1))
      {
         cbFileList++;
         lpCur++ ;
      }

      if (FILELIST( npZCB ) = GlobalAllocPtr( GHND, cbFileList ))
      {
         _fmemcpy( FILELIST( npZCB ), lpFileList, cbFileList ) ;
         FILELISTNEXT( npZCB ) = FILELIST( npZCB ) ;
      }
      else
      {
         // oh shit
         DP1( hWDB, "Cound not alloc space for filelist!" ) ;
         return FALSE ;
      }
   }

   DP1( hWDB, "\n***** ZMODEM SEND ******" ) ;

   DIRECTION( npZCB ) = DIR_SEND ;
   HFILE( npZCB ) = -1 ;
   STATUS( npZCB ) = ZMS_SENDING ;
   STATE( npZCB ) = SENT_INIT ;
   BLOCKSIZE( npZCB ) = 1024 ;
   LASTERROR( npZCB ) = 0 ;

   TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;

   XmitZRQINIT( npZCB ) ;
   ReadAnyHdrInit( npZCB ) ;

   return TRUE ;
}


//------------------------------------------------------------------------
//  BOOL EXTERN ProcessZmodemSendState( NPZCB npZCB )
//
//  Description:
//     Processes the next state of the ZMODEM send
//     finite state machine.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL EXTERN ProcessZmodemSendState( NPZCB npZCB )
{
   register  i ;

   if (HEADERNEXT( npZCB ))
      i = ReadAnyHdr( npZCB ) ;
   else
      i = ReadBinData( npZCB ) ;

   if (i == -1)
   {
      if (STREAMING( npZCB ))
      {
         DP3( hWDB, "STREAMING: Sent ZDATA" ) ;
         XmitZDATA( npZCB ) ;
      }
   }
   else
   {
      if (i == ZMODEM_TIMEOUT)
         LASTERROR( npZCB ) = ERROR_TIMEOUT ;
      else
         LASTERROR( npZCB ) = 0 ;

      switch (STATE( npZCB ))
      {
         case SENT_INIT:
            DP3( hWDB, "SENT_INIT" ) ;
            switch (i)
            {
               case ZMODEM_HDR:
                  DP3( hWDB, "ZMODEM_HDR" ) ;
                  switch (*INDATA( npZCB ))
                  {
                     case ZRINIT:
                        DP3( hWDB, "ZRINIT" ) ;
                        ProcessZRINITHdr( npZCB ) ;
                        if (XmitZFILE( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;

                     case ZNAK:
                        DP3( hWDB, "ZNAK" ) ;
                        if (ProcessZNAK( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;

                     default:
                        DP3( hWDB, "default" ) ;
                        ReadAnyHdrInit( npZCB ) ;
                  }
                  break ;

               case ZMODEM_CAN:
                  DP3( hWDB, "ZMODEM_CAN" ) ;
                  TerminateZmodem( npZCB, ERROR_HOSTCANCEL ) ;
                  break ;

               default:
                  DP3( hWDB, "default" ) ;
                  LASTERROR( npZCB ) = ERROR_SENT_INIT ;
                  if (UpdateErrorCount( npZCB ))
                  {
                     XmitZRQINIT( npZCB ) ;
                     ReadAnyHdrInit( npZCB ) ;
                  }
                  break ;
            }
            break ;

         case SENT_FILE:
            DP3( hWDB, "SENT_FILE" ) ;
            switch (i)
            {
               case ZMODEM_HDR:
                  DP3( hWDB, "ZMODEM_HDR" ) ;
                  switch (*INDATA( npZCB ))
                  {
                     case ZRPOS:
                        DP3( hWDB, "ZRPOS" ) ;
                        ProcessZRPOS( npZCB ) ;
                        if (XmitZDATA( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;

                     case ZACK:
                        DP3( hWDB, "ZACK" ) ;
                        XmitZDATA( npZCB ) ;
                        break ;

                     case ZNAK:
                        DP3( hWDB, "ZNAK" ) ;
                        if (ProcessZNAK( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;

                     case ZSKIP:
                        DP3( hWDB, "ZSKIP" ) ;
                        FILEBYTES( npZCB ) = FILESIZE( npZCB ) ;

                        XmitZEOF( npZCB ) ;
                        ReadAnyHdrInit( npZCB ) ;
                        break ;

                     default:
                        DP3( hWDB, "default" ) ;
                        ReadAnyHdrInit( npZCB ) ;
                        break ;
                  }
                  break ;

               case ZMODEM_CAN:
                  DP3( hWDB, "ZMODEM_CAN" ) ;
                  TerminateZmodem( npZCB, ERROR_HOSTCANCEL ) ;
                  break ;

               default:
                  DP3( hWDB, "default" ) ;
                  LASTERROR( npZCB ) = ERROR_SENT_FILE ;
                  if (UpdateErrorCount( npZCB ))
                  {
                     XmitZNAK( npZCB ) ;
                     ReadAnyHdrInit( npZCB ) ;
                  }
            }
            break;

         case SENT_DATA:
            DP3( hWDB, "SENT_DATA" ) ;
            switch (i)
            {
               case ZMODEM_HDR:
                  DP3( hWDB, "ZMODEM_HDR" ) ;
                  switch (*INDATA( npZCB ))
                  {
                     case ZRPOS:
                        DP3( hWDB, "ZRPOS" ) ;
                        ProcessZRPOS( npZCB ) ;
                        if (XmitZDATA( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;

                     case ZACK:
                        DP3( hWDB, "ZACK" ) ;
                        if (XmitZDATA( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;

                     case ZNAK:
                        DP3( hWDB, "ZNAK" ) ;
                        ReadAnyHdrInit( npZCB ) ;
                        break ;

                     default:
                        DP3( hWDB, "default" ) ;
                        ReadAnyHdrInit( npZCB ) ;
                  }
                  break ;

               case ZMODEM_CAN:
                  DP3( hWDB, "ZMODEM_CAN" ) ;
                  TerminateZmodem( npZCB, ERROR_HOSTCANCEL ) ;
                  break ;

               default:
                  DP3( hWDB, "default" ) ;
                  LASTERROR( npZCB ) = ERROR_SENT_DATA ;
                  if (UpdateErrorCount( npZCB ))
                  {
                     XmitZNAK( npZCB ) ;
                     ReadAnyHdrInit( npZCB ) ;
                  }
            }
            break ;

         case SENT_ZFIN:
            DP3( hWDB, "SENT_ZFIN" ) ;
            switch (i)
            {
               case ZMODEM_HDR:
                  switch (*INDATA( npZCB ))
                  {
                     case ZFIN:
                        TerminateZmodem( npZCB, 0 ) ;
                        break ;

                     case ZRINIT:
                        ProcessZRINITHdr( npZCB ) ;
                        if (XmitZFILE( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;

                     case ZACK:
                        XmitZFIN( npZCB ) ;
                        ReadAnyHdrInit( npZCB ) ;
                        break ;

                     default:
                        ReadAnyHdrInit( npZCB ) ;
                  }
                  break ;

               case ZMODEM_CAN:
                  TerminateZmodem( npZCB, ERROR_HOSTCANCEL ) ;
                  break ;

               default:
                  LASTERROR( npZCB ) = ERROR_SENT_ZFIN ;
                  if (UpdateErrorCount( npZCB ))
                  {
                     XmitZNAK( npZCB ) ;
                     ReadAnyHdrInit( npZCB ) ;
                  }
            }
            break ;

         default:
            return ( FALSE ) ;
      }

   }
   return ( TRUE ) ;

} // end of ProcessZmodemSendState()


//------------------------------------------------------------------------
//  VOID NEAR PASCAL SendBinData( NPZCB npZCB, int nFrameEnd, LPSTR lpBuf, 
//                         int nLength )
//
//  Description:
//     Send binary data packet of nLength, with ending ZDLE +
//     nFrameEnd sequence.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//     int nFrameEnd
//        frame's ending character (DLE prefixed)
//        
//     LPSTR lpBuf
//        pointer to buffer
//
//     int nLength
//        buffer length
//
//  History:   Date       Author      Comment
//              3/ 7/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL SendBinData( NPZCB npZCB, int nFrameEnd, LPSTR lpBuf, int nLength )
{
   DWORD dwCRC32 ;
   WORD  wCRC16 ;

   ResetWriteBuffer( npZCB ) ;

   if (TXFCS32( npZCB ))
   {
      dwCRC32 = 0xFFFFFFFFL ;
      for (; --nLength >= 0;)
      {
         WriteBufferData( npZCB, *lpBuf ) ;
         dwCRC32 = UPDCRC32( (255 & *lpBuf++), dwCRC32 ) ;
      }
      WriteBufferChar( npZCB, ZDLE ) ;
      WriteBufferChar( npZCB, nFrameEnd ) ;
      dwCRC32 = UPDCRC32( nFrameEnd, dwCRC32 ) ;
      dwCRC32 ^= 0xFFFFFFFFL ;
      for (nLength = 4; --nLength >= 0;)
      {
         WriteBufferData( npZCB, LOWORD( dwCRC32 ) ) ;
         dwCRC32 >>= 8 ;
      }
   }
   else
   {
      wCRC16 = 0 ;
      for (; --nLength >= 0;)
      {
         WriteBufferData( npZCB, *lpBuf ) ;
         wCRC16 = UPDCRC16( (255 & *lpBuf++), wCRC16 ) ;
      }
      WriteBufferChar( npZCB, ZDLE ) ;
      WriteBufferChar( npZCB, nFrameEnd ) ;
      wCRC16 = UPDCRC16( nFrameEnd, wCRC16 ) ;
      wCRC16 = UPDCRC16( 0, UPDCRC16( 0, wCRC16 ) ) ;
      WriteBufferData( npZCB, wCRC16 >> 8 ) ;
      WriteBufferData( npZCB, wCRC16 ) ;
   }
   if (nFrameEnd == ZCRCW)
      WriteBufferChar( npZCB, ASCII_XON ) ;

   SendWriteBuffer( npZCB ) ;

} // end of SendBinData()


//------------------------------------------------------------------------
//  VOID NEAR PASCAL ProcessZRINITHdr( NPZCB npZCB )
//
//  Description:
//     Process an incoming ZRINIT header.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL ProcessZRINITHdr( NPZCB npZCB )
{
   BYTE bRxFlags ;

   bRxFlags = INDATA( npZCB )[ 4 ] ;
   if (bRxFlags & CANFDX)
      USESTREAM( npZCB ) = TRUE ;
   else
      USESTREAM( npZCB ) = FALSE ;

   if (WANTSFCS32( npZCB ) && (bRxFlags & CANFC32))
      TXFCS32( npZCB ) = TRUE ;

   BLOCKSIZE( npZCB ) = INDATA( npZCB )[ 1 ] * 256 + INDATA( npZCB )[ 2 ] ;

   USESTREAM( npZCB ) = (BLOCKSIZE( npZCB ) == 0 && USESTREAM( npZCB ) ) ;

   if (BLOCKSIZE( npZCB ) < 32 || BLOCKSIZE( npZCB ) > 1024)
      BLOCKSIZE( npZCB ) = 1024 ;

   DP3( hWDB, "BLOCKSIZE = %d", BLOCKSIZE( npZCB ) ) ;

} // end of ProcessZRINITHdr()

//------------------------------------------------------------------------
//  VOID NEAR PASCAL ProcessZRPOS( NPZCB npZCB )
//
//  Description:
//     Process the ZRPOS data packet.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL ProcessZRPOS( NPZCB npZCB )
{
   FILEBYTES( npZCB ) = POSITION( npZCB ) ;
   _llseek( HFILE( npZCB ), POSITION( npZCB ), SEEK_SET ) ;
   SENDHEADER( npZCB ) = TRUE ;

} // end of ProcessZRPOS()


//------------------------------------------------------------------------
//  BOOL NEAR PASCAL XmitZFILE( NPZCB npZCB )
//
//  Description:
//     Transmit a ZFILE header packet.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//              5/18/92   BryanW      Had to change this shit code.
//                                    It now supports multiple file sends,
//                                    if HFILE() is -1, open the next
//                                    file in the list and prepare the
//                                    header... if the list is NULL,
//                                    we're done.
//
//------------------------------------------------------------------------

BOOL NEAR PASCAL XmitZFILE( NPZCB npZCB )
{
   int   i ;
   char  szBuf[ 256 ] ;

   BLOCKCOUNT( npZCB ) = 0 ;
   FILEBYTES( npZCB ) = 0 ;

#pragma message( "NEED: notify owner of OpenXferFile failure?" )

   while (*FILELISTNEXT( npZCB ) && (-1 == HFILE( npZCB )))
   {
      lstrcpy( FILENAME( npZCB ), FILELISTNEXT( npZCB ) ) ;
      if (*FILENAME( npZCB ))
      {
         HFILE( npZCB ) = OpenXferFile( npZCB ) ;
         if (FILELIST( npZCB ))
            FILELISTNEXT( npZCB ) =
               FILELISTNEXT( npZCB ) + lstrlen( FILENAME( npZCB ) ) + 1 ;

#if 0
         if (HFILE( npZCB ) == -1)
         {
            /*
             * Abort big time
             */
            TerminateZmodem( npZCB, ERROR_DISKERROR ) ;

            return FALSE ;
         }
#endif
      }
   }

   if (-1 == HFILE( npZCB ))
   {
      // we're done at this point

      szBuf[ 0 ] = 0 ;
      szBuf[ 1 ] = 0 ;
      szBuf[ 2 ] = 0 ;
      szBuf[ 3 ] = 0 ;
      SendHexHdr( npZCB, ZFIN, szBuf ) ;
      STATE( npZCB ) = SENT_ZFIN ;

   }
   else
   {
      FILESTATUS  fsXferFile ;

      // get file information

      GetFileStatus( npZCB, &fsXferFile ) ;
      FILETIME( npZCB ) = fsXferFile.wTime ;
      FILEDATE( npZCB ) = fsXferFile.wDate ;
      FILESIZE( npZCB ) = fsXferFile.dwLength ;

      // save starting time

      time( &TIMESTARTED( npZCB ) ) ;

      szBuf[ ZF0 ] = ZCBIN | ZCRESUM ;
      szBuf[ ZF1 ] = 0 ;
      szBuf[ ZF2 ] = 0 ;
      szBuf[ ZF3 ] = 0 ;
      SendBinHdr( npZCB, ZFILE, szBuf ) ;

      _fmemset( szBuf, ASCII_NUL, 256 ) ;
      lstrcpy( szBuf, StripPath( FILENAME( npZCB ) ) ) ;
      i = lstrlen( szBuf ) + 1 ;
      DateTimeToBuf( npZCB, &szBuf[ i ] ) ;
      i += lstrlen( &szBuf[ i ] ) ;

      CURFILE( npZCB )++ ;
      SendBinData( npZCB, ZCRCW, szBuf, i ) ;
      STATE( npZCB ) = SENT_FILE ;
   }

   return ( TRUE ) ;

} // end of XmitZFILE()


//------------------------------------------------------------------------
//  BOOL NEAR PASCAL XmitZDATA( NPZCB npZCB )
//
//  Description:
//     Transmit a ZDATA packet.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR PASCAL XmitZDATA( NPZCB npZCB )
{
   int   nBytesRead ;
   char  szBuf[ 1100 ] ;

   nBytesRead = _lread( HFILE( npZCB ), szBuf, BLOCKSIZE( npZCB ) ) ;
   if (-1 == nBytesRead)
   {
      TerminateZmodem( npZCB, ERROR_DISKERROR ) ;
      return ( FALSE ) ;
   }
   else if (nBytesRead)
   {
      if (SENDHEADER( npZCB ))
      {
         char  szHdr[ 8 ] ;

         DWORDToBuf( szHdr, FILEBYTES( npZCB ) ) ;
         SendBinHdr( npZCB, ZDATA, szHdr ) ;
         SENDHEADER( npZCB ) = FALSE ;
      }
      if (USESTREAM( npZCB ) &&
          FILEBYTES( npZCB ) + nBytesRead < FILESIZE( npZCB ))
      {
         SendBinData( npZCB, ZCRCG, szBuf, nBytesRead ) ;
         STREAMING( npZCB ) = TRUE ;
      }
      else
      {
         SendBinData( npZCB, ZCRCW, szBuf, nBytesRead ) ;
         STREAMING( npZCB ) = FALSE ;
      }
      FILEBYTES( npZCB ) += (DWORD) nBytesRead ;
      BLOCKCOUNT( npZCB )++ ;
      STATE( npZCB ) = SENT_DATA ;
   }
   else
      XmitZEOF( npZCB ) ;

   return ( TRUE ) ;

} // end of XmitZDATA()


//------------------------------------------------------------------------
//  VOID NEAR PASCAL XmitZEOF( NPZCB npZCB )
//
//  Description:
//     Transmit a ZEOF packet.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL XmitZEOF( NPZCB npZCB )
{
   char  szBuf[ 8 ] ;

   // close the file

   CloseXferFile( npZCB ) ;

   // send ZEOF

   DWORDToBuf( szBuf, FILEBYTES( npZCB ) ) ;
   SendHexHdr( npZCB, ZEOF, szBuf ) ;

   STATE( npZCB ) = SENT_ZFIN ;

} // end of XmitZEOF()


//------------------------------------------------------------------------
//  VOID NEAR PASCAL XmitZRQINIT( NPZCB npZCB )
//
//  Description:
//     Trasmits a ZRQINIT header.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL XmitZRQINIT( NPZCB npZCB )
{
   char  szBuf[ 8 ] ;

   szBuf[ 0 ] = 0 ;
   szBuf[ 1 ] = 0 ;
   szBuf[ 2 ] = 0 ;
   szBuf[ 3 ] = 0 ;
   SendHexHdr( npZCB, ZRQINIT, szBuf ) ;

} // end of XmitZRQINIT()

/************************************************************************
 * End of File: zmsend.c
 ***********************************************************************/

