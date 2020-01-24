/************************************************************************
 *
 *     Project:  ZMODEM
 *
 *      Module:  zmrecv.c
 *
 *     Remarks:
 *
 *    Handles all receive side stuff for zmodem.
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

BOOL NEAR PASCAL ProcessZFILEData( NPZCB ) ;
VOID NEAR PASCAL ProcessZDATAHdr( NPZCB ) ;
BOOL NEAR PASCAL ProcessZDATAData( NPZCB npZCB ) ;
VOID NEAR PASCAL ProcessZFILEHdr( NPZCB ) ;

VOID NEAR PASCAL ProcessZSINITHdr( NPZCB ) ;
VOID NEAR PASCAL ProcessZSINITData( NPZCB ) ;

VOID NEAR PASCAL XmitZRINITHdr( NPZCB ) ;
VOID NEAR PASCAL XmitZRPOS( NPZCB ) ;

VOID NEAR PASCAL ReadBinDataInit( NPZCB ) ;

/****************************************************************
 * BOOL EXTERN HandleZM_RECEIVE( NPZCB npZCB, LPSTR lpFileList, LPSTR lpszDir )
 *
 *  Description: 
 *
 *
 ****************************************************************/
BOOL EXTERN HandleZM_RECEIVE( NPZCB npZCB, LPSTR lpFileList, LPSTR lpszDir )
{
   /*
    * For the SEND and RECEIVE cases, we copy the buffer
    * passed to us into our own buffer FILELIST().
    * Then as we process files, we just march our FILENAME()
    * member to the beginning of the next filename in
    * FILELIST().
    *
    * The bitch is figuring out how long the buffer we've been
    * passed is, but hey..we're smart...we can figure it out.
    *
    */
   UINT  cbFileList ;
   LPSTR lpCur = lpFileList ;

   DP1( hWDB, "\n***** ZMODEM RECEIVE (npZCB == %04X) ******", npZCB ) ;

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
      /* It's an empty list
         */

      DP1( hWDB, "   No destination files specified" ) ;

      FILELIST( npZCB ) = FILELISTNEXT( npZCB ) = NULL ;
      *FILENAME( npZCB ) = NULL ;

   }
   else
   {
      DP1( hWDB, "   lpFileList points to %s", lpFileList ) ;

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

   if (lpszDir)
      lstrcpy( DESTDIRECTORY( npZCB ), lpszDir ) ;
   else
      *DESTDIRECTORY( npZCB ) = '\0' ;

   DIRECTION( npZCB ) = DIR_RECEIVE ;

   HFILE( npZCB ) = -1 ;

   STATUS( npZCB ) = ZMS_RECEIVING ;
   STATE( npZCB ) = RECV_INIT ;
   BLOCKSIZE( npZCB ) = 1024 ;
   LASTERROR( npZCB ) = 0 ;

   CHARCOUNT( npZCB ) = 0 ;
   CANCOUNT( npZCB ) = 0 ;
   ESCSEQ( npZCB ) = FALSE ;
   PHASE( npZCB ) = PHASE_SYNC ;
   TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;

   DP( hWDB, "Before sending ZRInitHdr, npZCB == %04X", npZCB ) ;

   XmitZRINITHdr( npZCB ) ;

   DP( hWDB, "After sending ZRInitHdr, npZCB == %04X", npZCB ) ;
      
   ReadAnyHdrInit( npZCB ) ;

   DP( hWDB, "After ReadAnyHdrInit, npZCB == %04X", npZCB  ) ;

   // CRC type is forced to CRC_16 until a ZBIN32 type is read
   // CRCTYPE( npZCB ) = CRC_16 ;

   return TRUE ;

} /* HandleZM_RECEIVE()  */

//------------------------------------------------------------------------
//  BOOL EXTERN ProcessZmodemReceiveState( NPZCB npZCB )
//
//  Description:
//     Processes the next state of the ZMODEM receive
//     finite state machine.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------
BOOL EXTERN ProcessZmodemReceiveState( NPZCB npZCB )
{
   register  i ;

   if (HEADERNEXT( npZCB ))
      i = ReadAnyHdr( npZCB ) ;
   else
      i = ReadBinData( npZCB ) ;

   if (i != -1)
   {
      if (i == ZMODEM_TIMEOUT)
         LASTERROR( npZCB ) = ERROR_TIMEOUT ;
      else
         LASTERROR( npZCB ) = 0 ;
      switch (STATE( npZCB ))
      {
         case RECV_INIT:
            switch (i)
            {
               case ZMODEM_HDR:
                  switch (*INDATA( npZCB ))
                  {
                     case ZRQINIT:
                        XmitZRINITHdr( npZCB ) ;
                        ReadAnyHdrInit( npZCB ) ;
                        break ;

                     case ZSINIT:
                        ProcessZSINITHdr( npZCB ) ;
                        ReadBinDataInit( npZCB ) ;
                        break ;

                     case ZFILE:
                        ProcessZFILEHdr( npZCB ) ;
                        ReadBinDataInit( npZCB ) ;
                        STATE( npZCB ) = RECV_FILE ;
                        break ;

                     case ZNAK:
                        if (ProcessZNAK( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;

                     case ZFIN:
                        DP3( hWDB, "ZFIN" ) ;
                        XmitZFIN( npZCB ) ;
                        ReadAnyHdrInit( npZCB ) ;
                        STATE( npZCB ) = RECV_ZFIN ;
                        break ;

                     default:
                        ReadAnyHdrInit( npZCB ) ;
                        break ;
                  }
                  break ;

               case ZMODEM_CAN:
                  TerminateZmodem( npZCB, ERROR_HOSTCANCEL ) ;
                  break ;

               case ZMODEM_DATA:
                  ProcessZSINITData( npZCB ) ;
                  ReadAnyHdrInit( npZCB ) ;
                  break ;

               default:
                  LASTERROR( npZCB ) = ERROR_RECV_INIT ;
                  if (UpdateErrorCount( npZCB ))
                  {
                     XmitZRINITHdr( npZCB ) ;
                     ReadAnyHdrInit( npZCB ) ;
                  }
                  break ;
            }
            break ;

         case RECV_FILE:
            switch (i)
            {
               case ZMODEM_HDR:
                  switch (*INDATA( npZCB ))
                  {
                     case ZFILE:
                        ProcessZFILEHdr( npZCB ) ;
                        ReadBinDataInit( npZCB ) ;
                        break ;

                     case ZDATA:
                        ProcessZDATAHdr( npZCB ) ;
                        STATE( npZCB ) = RECV_DATA ;
                        break ;

                     case ZNAK:
                        if (ProcessZNAK( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;

                     case ZEOF:
                        CloseXferFile( npZCB ) ;
                        XmitZRINITHdr( npZCB ) ;
                        ReadAnyHdrInit( npZCB ) ;
                        STATE( npZCB ) = RECV_INIT ;

                        STATUS( npZCB ) = ZMS_FILECOMPLETED ;

                        break ;

                     default:
                        ReadAnyHdrInit( npZCB ) ;
                  }
                  break ;

               case ZMODEM_DATA:
                  switch (INDATA( npZCB )[ LENGTH( npZCB ) - 1 ])
                  {
                     case ZCRCE:
                        if (ProcessZFILEData( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;

                     case ZCRCG:
                        if (ProcessZFILEData( npZCB ))
                           ReadBinDataInit( npZCB ) ;
                        break ;

                     case ZCRCQ:
                        if (ProcessZFILEData( npZCB ))
                           ReadBinDataInit( npZCB ) ;
                        break ;

                     case ZCRCW:
                        if (ProcessZFILEData( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;
                  }
                  break ;

               case ZMODEM_CAN:
                  TerminateZmodem( npZCB, ERROR_HOSTCANCEL ) ;
                  break ;

               default:
                  LASTERROR( npZCB ) = ERROR_RECV_FILE ;
                  if (UpdateErrorCount( npZCB ))
                  {
                     XmitZNAK( npZCB ) ;
                     ReadAnyHdrInit( npZCB ) ;
                  }
                  break ;
            }
            break ;

         case RECV_DATA:
            switch (i)
            {
               case ZMODEM_HDR:
                  switch (*INDATA( npZCB ))
                  {
                     case ZDATA:
                        ProcessZDATAHdr( npZCB ) ;
                        break ;

                     case ZEOF:
                        CloseXferFile( npZCB ) ;
                        XmitZRINITHdr( npZCB ) ;
                        ReadAnyHdrInit( npZCB ) ;
                        STATE( npZCB ) = RECV_INIT ;
                        break ;

                     case ZNAK:
                        if (ProcessZNAK( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;

                     default:
                        ReadAnyHdrInit( npZCB ) ;
                        break ;
                  }
                  break ;

               case ZMODEM_DATA:
                  switch (INDATA( npZCB )[ LENGTH( npZCB ) - 1 ])
                  {
                     case ZCRCE:
                        if (ProcessZDATAData( npZCB ))
                           ReadAnyHdrInit( npZCB ) ;
                        break ;

                     case ZCRCG:
                        if (ProcessZDATAData( npZCB ))
                           ReadBinDataInit( npZCB ) ;
                        break ;

                     case ZCRCQ:
                        if (ProcessZDATAData( npZCB ))
                        {
                           XmitZACK( npZCB ) ;
                           ReadBinDataInit( npZCB ) ;
                        }
                        break ;

                     case ZCRCW:
                        if (ProcessZDATAData( npZCB ))
                        {
                           XmitZACK( npZCB ) ;
                           ReadAnyHdrInit( npZCB ) ;
                        }
                        break ;

                     default:
                        XmitZRPOS( npZCB ) ;
                        ReadAnyHdrInit( npZCB ) ;
                        break ;
                  }
                  break ;

               case ZMODEM_CAN:
                  TerminateZmodem( npZCB, ERROR_HOSTCANCEL ) ;
                  break ;

               default:
                  LASTERROR( npZCB ) = ERROR_RECV_DATA ;
                  if (UpdateErrorCount( npZCB ))
                  {
                     XmitZRPOS( npZCB ) ;
                     ReadAnyHdrInit( npZCB ) ;
                  }
                  break ;
            }
            break ;

         case RECV_ZFIN:
            DP3( hWDB, "RECV_FIN" ) ;

            STATUS( npZCB ) = ZMS_COMPLETED ;

            break ;
      }
   }

   return ( TRUE ) ;

} // end of ProcessZmodemReceiveState()


//------------------------------------------------------------------------
//  BOOL NEAR PASCAL ProcessZFILEData( NPZCB npZCB )
//
//  Description:
//     Process the ZFILE data packet.
//
//  Parameters:
//     NPZCB npZCB
//
//  History:   Date       Author      Comment
//              3/10/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR PASCAL ProcessZFILEData( NPZCB npZCB )
{
   char  szBuf[ 132 ] ;

   FILEBYTES( npZCB ) = 0 ;
   FILEDATE( npZCB ) = 0 ;
   FILETIME( npZCB ) = 0 ;

   DP4( hWDB, "ProcessZFILEData()" ) ;

   if (-1 == HFILE( npZCB ))
   {
      DWORD     dwModTime ;
      BOOL      fTriedHostsName = FALSE ;

      if (FILELIST( npZCB ))
      {
         lstrcpy( FILENAME( npZCB ), FILELISTNEXT( npZCB ) ) ;
         FILELISTNEXT( npZCB ) =
            FILELISTNEXT( npZCB ) + lstrlen( FILENAME( npZCB ) ) + 1 ;
      }
      

      // NEED ! How to deal with DESTDIRECTORY()???
      // 
      // One idea I had was to change dir before we tried
      // to open the files.  That way if the file had path info
      // it would override the chdir (and drive).  Whadda ya think?

OpenTheFile:

      /*
       * If FILENAME() actually has anything in it then we need to try
       * to open it and ignore what's in INDATA().
       *
       * We try OF_READ first 'cause we might want recovery.
       */
      DP1( hWDB, "Attempting to open '%s' for recovery...", (LPSTR)FILENAME( npZCB ) ) ;

      if ( (ALWAYSOVERWRITE( npZCB ) == FALSE) &&
           (-1 != (HFILE( npZCB ) =  _lopen( FILENAME( npZCB ), OF_READ ))))
      {
         FILESTATUS  fsXferFile ;

         dwModTime = 0 ;
         wsscanf( &INDATA( npZCB )[ lstrlen( INDATA( npZCB ) ) + 1 ],
                  "%lu %lo", (DWORD FAR *) &FILESIZE( npZCB ),
                  (DWORD FAR *) &dwModTime ) ;

         if (dwModTime > 0)
            DWORDToDateTime( npZCB, dwModTime ) ;

         DP1( hWDB, "ProcessZFILEData: ReOpened File = %s",
               (LPSTR) FILENAME( npZCB ) ) ;

         GetFileStatus( npZCB, &fsXferFile ) ;
         _lclose( HFILE( npZCB ) ) ;

         // NEED: create a backup copy

         // check file date, time and size

         if ((fsXferFile.wDate == FILEDATE( npZCB )) &&
             (fsXferFile.wTime == FILETIME( npZCB )) &&
             (fsXferFile.dwLength <= FILESIZE( npZCB )))
         {
            // re-open for file recovery

            if (-1 == (HFILE( npZCB ) = _lopen( FILENAME( npZCB ),
                                                OF_READWRITE )))
            {
               DP1( hWDB, "Re-open for recovery failed!" ) ;
               TerminateZmodem( npZCB, ERROR_DISKERROR ) ;

               return ( FALSE ) ;
            }

            FILEBYTES( npZCB ) =
               _llseek( HFILE( npZCB ), fsXferFile.dwLength, SEEK_SET ) ;
            DP1( hWDB, "File size = %d", FILEBYTES( npZCB ) ) ;
         }
         else if (-1 == (HFILE( npZCB ) = OpenXferFile( npZCB )))
         {
            TerminateZmodem( npZCB, ERROR_DISKERROR ) ;
            return ( FALSE ) ;
         }
      }
      else
      {

         DP1( hWDB, "    File (%s) did not open for recovery, trying create...",
            (LPSTR)FILENAME( npZCB ) ) ;


#pragma message( "Should really check for FILENAME==NULL" )

         /*
          * Try creating it.  If that doesn't work, then use the hosts
          * filename...
          */
         if (-1 == (HFILE( npZCB ) = OpenXferFile( npZCB )))
         {
            DP1( hWDB, "   Could not create %s", (LPSTR)FILENAME( npZCB ) ) ;

            if (fTriedHostsName == TRUE)
            {
               DP1( hWDB, "   Bailing out since we already tried %s", (LPSTR)FILENAME( npZCB ) ) ;
               /* bail 'cause we've already been here... */

               TerminateZmodem( npZCB, ERROR_DISKERROR ) ;
               return ( FALSE ) ;
            }

            fTriedHostsName = TRUE ;

            /*
             * We'll that didn't work.  So let's try the filename that
             * has been given to us by the host (in INDATA()).  FILENAME()
             * may have path data in it, and we'll preserve that
             */
            lstrcpy( StripPath( FILENAME( npZCB ) ), INDATA( npZCB ) ) ;

            goto OpenTheFile ;
         }

         dwModTime = 0 ;
         wsscanf( &INDATA( npZCB )[ lstrlen( INDATA( npZCB ) ) + 1 ],
                  "%lu %lo", (DWORD FAR *) &FILESIZE( npZCB ),
                  (DWORD FAR *) &dwModTime ) ;

         if (dwModTime > 0)
            DWORDToDateTime( npZCB, dwModTime ) ;

         DP3( hWDB, "ProcessZFILEData: Created File = %s",
               (LPSTR) FILENAME( npZCB ) ) ;


      }

      DP1( hWDB, "'%s' opened!", (LPSTR)FILENAME( npZCB ) ) ;

      CURFILE( npZCB )++ ;
      BLOCKCOUNT( npZCB ) = 0 ;

      // NEED: What are XferIndicators ?

      // ResetXferIndicators( FALSE ) ;
   }


   // save starting time
   time( &TIMESTARTED( npZCB ) ) ;
   LASTERROR( npZCB ) = 0 ;

   // NEED: compute block size

#pragma message( "TOTALBLOCKS should be adjusted for position" ) 

   TOTALBLOCKS( npZCB ) = LOWORD( FILESIZE( npZCB ) / BLOCKSIZE( npZCB ) ) ;

   DWORDToBuf( szBuf, FILEBYTES( npZCB ) ) ;
   SendHexHdr( npZCB, ZRPOS, szBuf ) ;

   return ( TRUE ) ;

} // end of ProcessZFILEData()

//------------------------------------------------------------------------
//  VOID NEAR PASCAL ProcessZDATAHdr( NPZCB npZCB )
//
//  Description:
//     Process the header of a ZDATA packet (file offset).
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL ProcessZDATAHdr( NPZCB npZCB )
{
   char   szBuf[ 8 ] ;

   if (POSITION( npZCB ) > FILEBYTES( npZCB ))
   {
      DP3( hWDB, "Sending ZRPOS %lu", POSITION( npZCB ) ) ;

      DWORDToBuf( szBuf, FILEBYTES( npZCB ) ) ;
      SendHexHdr( npZCB, ZRPOS, szBuf ) ;
      ReadAnyHdrInit( npZCB ) ;
   }
   else
   {
      DP3( hWDB, "Moving file position" ) ;
      if (-1 != HFILE( npZCB ))
         _llseek( HFILE( npZCB ), POSITION( npZCB ), SEEK_SET ) ;
      FILEBYTES( npZCB ) = POSITION( npZCB ) ;
      ReadBinDataInit( npZCB ) ;
   }

} // end of ProcessZDATAHdr()

//------------------------------------------------------------------------
//  BOOL NEAR PASCAL ProcessZDATAData( NPZCB npZCB )
//
//  Description:
//     Process a ZDATA packet (e.g. write data from a ZDATA
//     data packet to the file).
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR PASCAL ProcessZDATAData( NPZCB npZCB )
{
   int nWritten ;

   DP3( hWDB, "ProcessZDATAData" ) ;

   FILEBYTES( npZCB ) += LENGTH( npZCB ) - 1 ;
   BLOCKCOUNT( npZCB )++ ;
   if (-1 != HFILE( npZCB ))
   {
      nWritten =
         _lwrite( HFILE( npZCB ), INDATA( npZCB ), LENGTH( npZCB ) - 1 ) ;
      if (-1 == nWritten)
      {
         DP1( hWDB, "Disk Error!" ) ;
         TerminateZmodem( npZCB, ERROR_DISKERROR ) ;
      }
      else if (nWritten != LENGTH( npZCB ) - 1)
      {
         DP1( hWDB, "Disk Full!" ) ;
         TerminateZmodem( npZCB, ERROR_DISKFULL ) ;
      }
      else
      {
         CONSECERRORS( npZCB ) = 0 ;

         // update stuff for user

         return ( TRUE ) ;
      }
   }
   return ( FALSE ) ;

} // end of ProcessZDATAData()

//------------------------------------------------------------------------
//  VOID NEAR PASCAL ProcessZFILEHdr( NPZCB npZCB )
//
//  Description:
//     Nothing being processed in the header.  We assume the
//     following:
//
//        ZF0: (conversion options) - Binary transfer and
//                                    file recovery on
//
//        ZF1: (management options) - None (all zeros)
//
//        ZF2: (transport options) -  None (all zeros)
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL ProcessZFILEHdr( NPZCB npZCB )
{
} // end of ProcessZFILEHdr()


//------------------------------------------------------------------------
//  VOID NEAR PASCAL ProcessZSINITHdr( NPZCB npZCB )
//
//  Description:
//     Don't have to do anything here.  Sender sends capability
//     flags, but currently, they are all zero - none yet defined
//     in protocol.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL ProcessZSINITHdr( NPZCB npZCB )
{
} // end of ProcessZSINITHdr()

//------------------------------------------------------------------------
//  VOID NEAR PASCAL ProcessZSINITData( NPZCB npZCB )
//
//  Description:
//     Process an incoming ZSINIT packet.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL ProcessZSINITData( NPZCB npZCB )
{
   XmitZACK( npZCB ) ;

   _fstrncpy( ATTN( npZCB ), &(INDATA( npZCB )[ 4 ]), 32 ) ;

   // MUST: Reset transfer indicators

   // ResetXferIndicators( FALSE ) ;

} // end of ProcessZSINITData()

//------------------------------------------------------------------------
//  VOID NEAR PASCAL ReadBinDataInit( NPZCB npZCB )
//
//  Description:
//     Prepares the ZCB for binary data receive.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL ReadBinDataInit( NPZCB npZCB )
{
   CHARCOUNT( npZCB ) = 0 ;
   CANCOUNT( npZCB ) = 0 ;
   ESCSEQ( npZCB ) = FALSE ;
   LENGTH( npZCB ) = 0 ;
   HEADERNEXT( npZCB ) = FALSE ;
   TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;

} // end of ReadBinDataInit()

//------------------------------------------------------------------------
//  VOID NEAR PASCAL XmitZRINITHdr( NPZCB npZCB )
//
//  Description:
//     Transmit a ZRINIT packet.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL XmitZRINITHdr( NPZCB npZCB )
{
   char szBuf[ 8 ] ;

   szBuf[ 0 ] = 0 ;
   szBuf[ 1 ] = 0 ;
   szBuf[ 2 ] = 0 ;
   szBuf[ 3 ] = CANFDX | CANOVIO | CANFC32 ;

   SendHexHdr( npZCB, ZRINIT, szBuf ) ;

} // end of XmitZRINITHdr()

//------------------------------------------------------------------------
//  VOID NEAR PASCAL XmitZRPOS( NPZCB npZCB )
//
//  Description:
//     Transmit a ZRPOS packet.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL XmitZRPOS( NPZCB npZCB )
{
   char szBuf[ 8 ] ;

   DWORDToBuf( szBuf, FILEBYTES( npZCB ) ) ;
   SendHexHdr( npZCB, ZRPOS, szBuf ) ;

} // end of XmitZRPOS()

/************************************************************************
 * End of File: zmrecv.c
 ***********************************************************************/
