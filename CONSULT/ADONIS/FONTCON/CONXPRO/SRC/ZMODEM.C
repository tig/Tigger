//---------------------------------------------------------------------------
//
//  Module: zmodem.c
//
//  Purpose:
//     ZMODEM protocol - adapted to Windows by Bryan A. Woodruff
//     based on original code from Chuck Forsberg at Omen Technology Inc.
//
//     NOTE:  This source code can compile and run
//            cleanly under Xenix!  All you have to do is
//            #define IHATEUNIX and make sure the include file
//            'winunix.h' is to be found.  Obviously the
//            concept of window handles is not valid, but
//            we still use something similar to hold our
//            data...
//
//  Description of functions:
//     Function descriptions are contained in function headers.
//
//  Development Team:
//     Bryan A. Woodruff, Charlie Kindel
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//              4/19/92   CKindel     Fucked it up and made it unixable
//              5/10/92   BryanW      Added CRC-32, added proper
//                                    protocol termination code.
//              5/10/92   BryanW      Receive is spelled with ei...
//                                    I before E except after C. <grin>
//              5/10/92   BryanW      Verified date/time parsing.
//              5/10/92   BryanW      All CRC calculations are using
//                                    a common table look-up algorithm.
//
//              8/7/92    Ckindel     Fixed aborting stuff.  Now
//                                    aborts on both sides.  Also 
//                                    changed the meening of ZM_ABORT again.
//                                    It's now the same as ZM_LOCALCANCEL.
//              8/8/92    Ckindel     Pulled send/receive out into
//                                    separate files to minimize code
//                                    segment size.
//
// ****************************************
// **** Comments / Need To Implement: *****
// ****************************************
//
//    -  High-end features such as different size blocks, etc...
//       need to be implemented.
//
//    -  Need to change timeout interval.
//
//---------------------------------------------------------------------------
//
//    -  UNIX sz's file time information is based on GMT.  I don't
//       have the conversion necessary (it requires environment
//       variable settings), however, I don't have the inclination to
//       figure it out.  I think PDT is GMT + 7, UNIX sz sends 23:39
//       for a file created at 16:39.
//
//       NOTE!!! - This is REALLY screwy, some BBS/DOS implementations
//       are sending the file time in respect to local time.  PCPLUS
//       re-acts in the same manner.  ___UNIX sz must be fixed.___
//
//       N.B.:  I have converted this thing to use DOS 1980 formats...
//       PCPLUS uses this format, Chuck's piece of shit U(seless)NIX
//       version uses some half-assed scheme for date/time conversion.
//
//    -  There should probably be an option to enable CRC-32.  Right now,
//       ZRINIT sends CANFC32 - the host will determine if it'll use
//       it.
//
//    -  Streaming protocol is determined by CANFDX.  CANOVIO
//       determines if the client can perform overlapped I/O.
//
//    -  There are extra parameters send by UNIX sz for ZFILEHDR.
//       (4 parameters... the last extra parameter seems to be the
//       file size - perhaps these define the RLE options?)
//
//---------------------------------------------------------------------------

#include "PRECOMP.H"   // precompiled header shit

#ifdef IHATEUNIX
   #include "winunix.h"
   static UINT nUseCount ;
   static DWORD CRC32Table[256] ;
#endif

#include "crctab.h"    // CRC tables
#include "izmodem.h"


// function prototypes (private)

NPZCB NEAR PASCAL CreateZCB( VOID ) ;
VOID  NEAR PASCAL DestroyZCB( NPZCB ) ;

VOID NEAR PASCAL InitZmodem( NPZCB ) ;
VOID NEAR PASCAL CompleteZmodem( NPZCB ) ;
BOOL NEAR PASCAL ZmodemFSM( HWND ) ;

VOID NEAR PASCAL ResetReadBuffer( NPZCB ) ;
int  NEAR PASCAL ReadBufferChar( NPZCB ) ;

BOOL NEAR PASCAL VerifyBlock( LPSTR, int, BYTE ) ;
VOID NEAR PASCAL SendAbortString( HWND ) ;
BOOL NEAR PASCAL SetFileStatus( NPZCB, LPFILESTATUS ) ;

#ifdef IHATEUNIX

/*
 * This is global 'cause Xenix can't initialize arrays in functions
 */
static int    DaysInMonth[] = { 31, 28, 31, 30, 31, 30,
                                31, 31, 30, 31, 30, 31 } ;

#include <varargs.h>
#define DBG_LEVEL 1
                              
int _cdecl wdbPrint1( va_alist )
va_dcl
{
#if (DBG_LEVEL > 0)
   va_list args ;
   char    *fmt ;
   int     n ;

   va_start( args ) ;
   n = va_arg( args, n) ;
   fmt = va_arg( args, char * ) ;
   n = vprintf( fmt, args ) ;
   printf("\n") ;
   va_end( args ) ;
   return n ;
#endif
}
int _cdecl wdbPrint2( va_alist )
va_dcl
{
#if (DBG_LEVEL > 1)
   va_list args ;
   char    *fmt ;
   int     n ;

   va_start( args ) ;
   n = va_arg( args, n) ;
   fmt = va_arg( args, char * ) ;
   n = vprintf( fmt, args ) ;
   printf("\n") ;
   va_end( args ) ;
   return n ;
#endif
}
int _cdecl wdbPrint3( va_alist )
va_dcl
{
#if (DBG_LEVEL > 2)
   va_list args ;
   char    *fmt ;
   int     n ;

   va_start( args ) ;
   n = va_arg( args, n) ;
   fmt = va_arg( args, char * ) ;
   n = vprintf( fmt, args ) ;
   printf("\n") ;
   va_end( args ) ;
   return n ;
#endif
}
int _cdecl wdbPrint4( va_alist )
va_dcl
{
#if (DBG_LEVEL > 3)
   va_list args ;
   char    *fmt ;
   int     n ;

   va_start( args ) ;
   n = va_arg( args, n) ;
   fmt = va_arg( args, char * ) ;
   n = vprintf( fmt, args ) ;
   printf("\n") ;
   va_end( args ) ;
   return n ;
#endif
}

#endif

/****************************************************************
 *  LRESULT WINAPI Zmodem( )
 *
 *  Description:
 *
 *    This function controls the zmodem protocol.  It is the
 *    only function exported by the ZMODEM.DLL.
 *
 *  Parameters:
 *    HWND hwnd
 *       Identifies the window that owns the zmodem transfer.
 *       We store the zmodem data structures using window props on
 *       this window.
 *       There are several ZM_ commands that do not require a
 *       valid window handle, but most do.
 *
 *    UINT wCmd
 *       Specifies the ZM_ comamnd to be carried out.
 *
 *    LPARAM lParam1
 *       The meanint of lParam1 depends on wCmd.  In most cases where
 *       lParam1 is used, it specifies input data.
 *
 *    LPARAM lParam2
 *       The meaning of lParam2 depends on wCmd.  In most cases where
 *       lParam2 is used, it specifies output data.
 *
 *
 *  Returns:
 *    A polymorphic value whose meaning depends on wCmd.  In most
 *    cases, however, it is a WMS_ status code
 *
 ****************************************************************/
LRESULT WINAPI Zmodem( HWND hwnd, UINT wCmd, LPARAM lParam1, LPARAM lParam2 )
{
   NPZCB     npZCB = NULL ;
   int       i ;
#ifdef IHATEUNIX
   static   BOOL fCRC32TableBuilt = FALSE ;

   if (fCRC32TableBuilt == FALSE)
   {
      BuildCRC32Table( CRC32Table ) ;
      fCRC32TableBuilt = TRUE ;
   }

#else

   /*
    * Handle the wCmds that don't require hwnd to be valid
    */
   switch( wCmd )
   {
      case ZM_GETUSECOUNT:
      return MAKELRESULT( nUseCount, 0 ) ;
   }

   /*
    * All of the rest require a valid hwnd.
    */
   if (!hwnd || !IsWindow( hwnd ))
   {
      DP1( hWDB, "Zmodem() : Invalid hwnd : %04X", hwnd ) ;
      return ZMS_ERROR ;
   }

#endif

   npZCB = (NPZCB) GET_PROP( hwnd, ATOM_NPZCB ) ;
   
   switch(wCmd)
   {
      case ZM_INIT:
      {
         /*
          * lParam1 - (ZMODEMSENDPROC) or Message ID
          * lParam2 - Timer ID or NULL
          *
          * On unix we return 'hwnd'.
          */

         if (npZCB)
         {
            DP1( hWDB, "Zmodem : ZM_INIT sent and hwnd is already an active zmodem window: %04X", hwnd ) ;
            return ZMS_ERROR ;
         }   

         if (NULL == (npZCB = CreateZCB()))
         {
            DP1( hWDB, "Zmodem : CreateZCB failed!" ) ;
            return ZMS_ERROR ;
         }

         // increment use count

         nUseCount++ ;

         BLOCKCOUNT( npZCB ) = 0 ;
         TOTALERRORS( npZCB ) = 0 ;
         CONSECERRORS( npZCB ) = 0 ;
         INCOMINGCOUNT( npZCB ) = 0 ;
         INCOMINGPOS( npZCB ) = 0 ;
         CTLESC( npZCB ) = FALSE ;

         // NEED: get setting for ZMODEM timeout (e.g. TIGHT, NORMAL, LOOSE)

         TIMEOUTINTERVAL( npZCB ) = 10000 ;   // normal is 10 seconds

         // NEED: make sure connector is an 8-bit connection,
         // also set XON/XOFF protocol, and flush ports

         // set up attention string

         for (i = 0; i < 12; i++)
         {
            ATTN( npZCB )[ i ] = ASCII_CAN ;
            ATTN( npZCB )[ i+12 ] = ASCII_BS ;
         }
         ATTN( npZCB )[ 24 ] = ASCII_NUL ;

         HPARENTWND( npZCB ) = hwnd ;

         STATE( npZCB ) = 0 ;
         CURFILE( npZCB ) = 0 ;
         FILEBYTES( npZCB ) = 0 ;
         TRANSMITTED( npZCB ) = 0 ;
         RECEIVED( npZCB ) = 0 ;
         TOTALFILES( npZCB ) = 0 ;
         HFILE( npZCB ) = -1 ;
         STREAMING( npZCB ) = FALSE ;
         USESTREAM( npZCB ) = FALSE ;

         MAXERRORS( npZCB ) = 10 ;
         ALWAYSOVERWRITE( npZCB ) = FALSE;
         WANTSFCS32( npZCB ) = TRUE ;

         TIMERID( npZCB ) = 0 ;

         STATUS( npZCB ) = ZMS_IDLE ;

#ifndef IHATEUNIX
         /*
          * In the Windows version lParam2 is optionally a timerID
          */
         if (lParam2 != 0)
         {
            DP2( hWDB, "ZM_INIT : Setting a timer" ) ;

            TIMERID( npZCB ) = SetTimer( hwnd, LOWORD( lParam2 ),
                                         TIMERINTERVAL_ZMODEMFSM,
                                         NULL ) ;

            if (TIMERID( npZCB ) == 0)
            {
               DP1( hWDB, "Out of timers!!!!" ) ;

               DestroyZCB( npZCB ) ;
               return ZMS_ERROR ;
            }
         }

#endif

         /*
          * If the HIWORD of SENDPROC() is NULL then when
          * we go to call the proc, we'll actually do a
          * sendmessage().
          */
         SENDPROC( npZCB ) = (ZMODEMSENDPROC)lParam1 ;

         /*
          * Put a pointer to the ZCB in out window structure
          */
         DP2( hWDB, "Zmodem : ZM_INIT" ) ;
             
         SET_PROP( hwnd, ATOM_NPZCB, npZCB ) ;

#ifdef IHATEUNIX
         return (LRESULT)npZCB ;
#else
         return ZMS_OK ;
#endif

      }
      break ;

      case ZM_KILL:
         DP2( hWDB, "Zmodem : ZM_KILL" ) ;
         /*
          * This command completely and utterly kills the zmodem
          * instance. It nukes the memory and the window properties
          *
          * This command is NEVER sent by any of the code within
          * the zmodem implementation.  It MUST be sent by
          * the caller.
          */

         // NEED: terminate conversation with ZMODEM cooperative if
         // we haven't already done so. (we probably have).

         if (npZCB)
         {
            CloseXferFile( npZCB ) ;

            // NEED: if error occurred, enter ABORTING stage 

#ifndef IHATEUNIX
            if (TIMERID( npZCB ))
               KillTimer( HPARENTWND( npZCB ), IDTIMER_ZMODEM ) ;
#endif

            if (FILELIST( npZCB ))
               GlobalFreePtr(  FILELIST( npZCB ) ) ;

            DestroyZCB( npZCB ) ;

            REMOVE_PROP( hwnd, ATOM_NPZCB ) ;

            nUseCount-- ;
         }
      break ;

      case ZM_ABORT:
         // same as ZM_LOCALCANCEL
         DP2( hWDB, "Zmodem : ZM_ABORT" ) ;
         if (!npZCB)
         {
            DP1( hWDB, "Zmodem : ZM_ABORT with NULL pointer" ) ;
            return ZMS_ERROR ;
         }

         ABORTING( npZCB ) = TRUE ;
         TerminateZmodem( npZCB, ERROR_LOCALCANCEL ) ;

      break ;

      case ZM_LOCALCANCEL:
         DP2( hWDB, "Zmodem : ZM_LOCALCANCEL" ) ;
         if (!npZCB)
         {
            DP1( hWDB, "Zmodem : ZM_LOCALCANCEL with NULL pointer" ) ;
            return ZMS_ERROR ;
         }

         TerminateZmodem( npZCB, ERROR_LOCALCANCEL ) ;
      break ;

      case ZM_QUERYSTATUS:
         if (npZCB == NULL)
            return ZMS_IDLE ;

         if (ABORTING( npZCB ))
         {
            if (STATUS( npZCB ) == ZMS_ABORTED)
               return ZMS_ABORTED ;
            else
               return ZMS_ABORTING ;
         }

         /* NEED! return indicator of current file in
          * lParam1.
          */
         return STATUS( npZCB ) ;

      break ;

      case ZM_SEND:
         /*
          * lParam1 points to a doubly-null-terminated list
          * of filenames.
          */
         if (!HandleZM_SEND( npZCB, (LPSTR)lParam1 ))
            return ZMS_ERROR ;
      break ;


      case ZM_RECEIVE:
         /*
          * lParam1 points to a doubly-null-terminated list
          * of filenames.
          *
          * lParam2 points to a directory to be used
          * as the download dir if a pathname is not specified
          * for a file in the list at lParam1
          */
         if (!HandleZM_RECEIVE( npZCB, (LPSTR)lParam1, (LPSTR)lParam2 ))
            return ZMS_ERROR ;
      break ;

      case ZM_ACK_FILECOMPLETED:
         DP2( hWDB, "Zmodem : ZM_ACK_FILECOMPLETED" ) ;

         if (DIRECTION( npZCB ) == DIR_RECEIVE)
            STATUS( npZCB ) = ZMS_RECEIVING ;
         else
            STATUS( npZCB ) = ZMS_SENDING ;
      break ;

      case ZM_ACK_COMPLETED:
         DP2( hWDB, "Zmodem : ZM_ACK_COMPLETED" ) ;

         /*
          * The status doesn't get reset back to ZMS_IDLE
          * until ZM_KILL
          */
         if (DIRECTION( npZCB ) == DIR_RECEIVE)
            STATUS( npZCB ) = ZMS_RECEIVING ;
         else
            STATUS( npZCB ) = ZMS_SENDING ;
      break ;

      case ZM_TIMER:
         if (!npZCB)
         {
            DP1( hWDB, "Zmodem : Bad ZCB pointer!" ) ;
            return ZMS_ERROR ;
         }
         
         /*
          * Drive the Zmodem Finite State Machine...
          */
         if (!ZmodemFSM( hwnd ))
         {
            DP1( hWDB, "Zmodem : ZM_TIMER ZmodemFSM failed!" ) ;

            return ZMS_ERROR ;
         }

      break ;

      case ZM_RXBLK:
      {
         /*
          * lParam1 - (UINT)cbRx 
          * lParam2 - (LPBYTE)lpRxBuf ;
          */

         int wBlkLen, wMaxLen ;

         if ( !npZCB )
         {
            /*
             * QUESTION:  Is this really an error?
             */
            DP1( hWDB, "Zmodem : ZM_RXBLK with no ZCB setup!" ) ;
            return ZMS_ERROR ;
         }

         wBlkLen = LOWORD( lParam1 ) ;
         while (wBlkLen)
         {
            DP5( hWDB, "INCOMINGCOUNT = %d. INCOMINGPOS = %d",
                 INCOMINGCOUNT( npZCB ), INCOMINGPOS( npZCB ) ) ;

            // check for wrap-around

            if (INCOMINGCOUNT( npZCB ) < INCOMINGPOS( npZCB ))
               wMaxLen = (INCOMINGPOS( npZCB ) - INCOMINGCOUNT( npZCB )) - 1 ;
            else
               wMaxLen =
                  sizeof( INCOMING( npZCB ) ) - INCOMINGCOUNT( npZCB ) ;

            wMaxLen = min( wBlkLen, wMaxLen ) ;
            if (wMaxLen != 0)
            {
               time( &TIMEREAD( npZCB ) ) ;
               RECEIVED( npZCB ) += wMaxLen ;
               _fmemcpy( &INCOMING( npZCB )[ INCOMINGCOUNT( npZCB ) ],
                         (LPSTR) lParam2, wMaxLen ) ;
               ((LPSTR) lParam2) += wMaxLen ;
               INCOMINGCOUNT( npZCB ) += wMaxLen ;
               if (INCOMINGCOUNT( npZCB ) == sizeof( INCOMING( npZCB ) ))
                  INCOMINGCOUNT( npZCB ) = 0 ;
               wBlkLen -= wMaxLen ;
            }

            ZmodemFSM( hwnd ) ;
         }

      }
      break ;

      case ZM_GET_BLOCKCOUNT:
         return MAKELRESULT( BLOCKCOUNT( npZCB ), 0 ) ;

      case ZM_GET_BLOCKSIZE:
         return MAKELRESULT( BLOCKSIZE( npZCB ), 0 ) ;

      case ZM_GET_CANCOUNT:
         return MAKELRESULT( CANCOUNT( npZCB ), 0 ) ;

      case ZM_GET_CHARCOUNT:
         return MAKELRESULT( CHARCOUNT( npZCB ), 0 ) ;

      case ZM_GET_CONSECERRORS:
         return MAKELRESULT( CONSECERRORS( npZCB ), 0 ) ;

      case ZM_GET_CRCTYPE:
         return MAKELRESULT( CRCTYPE( npZCB ), 0 ) ;

      case ZM_GET_CURFILE:
         return MAKELRESULT( CURFILE( npZCB ), 0 ) ;

      case ZM_GET_FILEBYTES:
         return (LRESULT)FILEBYTES( npZCB ) ;

      case ZM_GET_FILEDATE:
         return MAKELRESULT( FILEDATE( npZCB ), 0 ) ;

      case ZM_GET_FILENAME:
         return (LRESULT)(LPSTR)FILENAME( npZCB ) ;

      case ZM_GET_FILESIZE:
         return FILESIZE( npZCB ) ;

      case ZM_GET_FILETIME:
         return MAKELRESULT( FILETIME( npZCB ), 0 ) ;

      case ZM_GET_INCOMINGCOUNT:
         return MAKELRESULT( INCOMINGCOUNT( npZCB ), 0 ) ;

      case ZM_GET_INCOMINGPOS:
         return MAKELRESULT( INCOMINGPOS( npZCB ), 0 ) ;

      case ZM_GET_LASTERROR:
         return MAKELRESULT( LASTERROR( npZCB ), 0 ) ;

      case ZM_GET_LENGTH:
         return MAKELRESULT( LENGTH( npZCB ), 0 ) ;

      case ZM_GET_MAXERRORS:
         return MAKELRESULT( MAXERRORS( npZCB ), 0 ) ;

      case ZM_GET_OUTCOUNT:
         return MAKELRESULT( OUTCOUNT( npZCB ), 0 ) ;

      case ZM_GET_POSITION:
         return (LRESULT)POSITION( npZCB ) ;

      case ZM_GET_RECEIVED:
         return (LRESULT)RECEIVED( npZCB ) ;

      case ZM_GET_TIMEREAD:
         return (LRESULT)TIMEREAD( npZCB ) ;

      case ZM_GET_TIMESTARTED:
         return (LRESULT)TIMESTARTED( npZCB ) ;

      case ZM_GET_TOTALBLOCKS:
         return MAKELRESULT( TOTALBLOCKS( npZCB ), 0 ) ;

      case ZM_GET_TOTALERRORS:
         return MAKELRESULT( TOTALERRORS( npZCB ), 0 ) ;

      case ZM_GET_TOTALFILES:
         return MAKELRESULT( TOTALFILES( npZCB ), 0 ) ;

      case ZM_GET_TRANSMITTED:
         return (LRESULT)TRANSMITTED( npZCB ) ;

      default:
      {
         DP1( hWDB, "Zmodem() : Invalid wCmd : %04X", wCmd ) ;
         return ZMS_ERROR ;
      }
   }

   return ZMS_OK ;

} /* Zmodem()  */


//------------------------------------------------------------------------
//  NPZCB NEAR PASCAL CreateZCB( VOID )
//
//  Description:
//     Creates a ZMODEM control block and returns
//     the near pointer to the ZCB.
//
//  Parameters:
//     None.
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

NPZCB NEAR PASCAL CreateZCB( VOID )
{
   return ( (NPZCB) LocalAlloc( LPTR, sizeof( ZCB ) ) ) ;

} // end of CreateZCB()

//------------------------------------------------------------------------
//  VOID NEAR PASCAL DestroyZCB( NPZCB npZCB )
//
//  Description:
//     Destroys (frees) the ZMODEM control block.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL DestroyZCB( NPZCB npZCB )
{
   LocalFree( npZCB ) ;

} // end of DestroyZCB()

//------------------------------------------------------------------------
//  HFILE EXTERN OpenXferFile( NPZCB npZCB )
//
//  Description:
//     Opens the xfer file and should create backups if specified.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it,
//
//------------------------------------------------------------------------

HFILE EXTERN OpenXferFile( NPZCB npZCB )
{
   HFILE     hFile ;
   // NEED:  Create backup files

   if (DIRECTION( npZCB ) == DIR_RECEIVE)
   {
      DP( hWDB, "Opening %s for write", (LPSTR) FILENAME( npZCB ) ) ;
      hFile = _lcreat( FILENAME( npZCB ), 0 ) ;
   }
   else
   {
      DP( hWDB, "Opening %s for read", (LPSTR) FILENAME( npZCB ) ) ;
      hFile = _lopen( FILENAME( npZCB ), OF_READ ) ;
   }

   return ( hFile ) ;

} // end of OpenXferFile()

//------------------------------------------------------------------------
//  VOID EXTERN CloseXferFile( NPZCB npZCB )
//
//  Description:
//     Closes the file specified in the NPZCB
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID EXTERN CloseXferFile( NPZCB npZCB )
{
   if (-1 == HFILE( npZCB ))
      return ;

   if (DIR_RECEIVE == DIRECTION( npZCB ))
   {
      FILESTATUS fsXferFile ;

      fsXferFile.wDate = FILEDATE( npZCB ) ;
      fsXferFile.wTime = FILETIME( npZCB ) ;
      SetFileStatus( npZCB, &fsXferFile ) ;
   }

   _lclose( HFILE( npZCB ) ) ;
   HFILE( npZCB ) = -1 ;

} // end of CloseXferFile()

//------------------------------------------------------------------------
//  BOOL EXTERN UpdateErrorCount( NPZCB npZCB )
//
//  Description:
//     Updates the error count and checks to see if we
//     have too many consecutive errors.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL EXTERN UpdateErrorCount( NPZCB npZCB )
{
   CONSECERRORS( npZCB )++ ;
   TOTALERRORS( npZCB )++ ;

   if (MAXERRORS( npZCB ) != 0 && CONSECERRORS( npZCB ) > MAXERRORS( npZCB ))
   {
      DP1( hWDB, "Zmodem : Too many errors.  Aborting transfer" ) ;
      TerminateZmodem( npZCB, ERROR_TOOMANYERRORS ) ;
      return ( FALSE ) ;
   }
   return ( TRUE ) ;

} // end of UpdateErrorCount()

//------------------------------------------------------------------------
//  VOID NEAR PASCAL CompleteZmodem( NPZCB npZCB )
//
//  Description:
//     Should perform any additional clean-up when aborting
//     a transfer.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL CompleteZmodem( NPZCB npZCB )
{

   DP1( hWDB, "Zmodem : CompleteZmodem" ) ;


} // end of CompleteZmodem()

//------------------------------------------------------------------------
//  VOID EXTERN TerminateZmodem( NPZCB npZCB, int nError )
//
//  Description:
//     Terminates the ZMODEM conversation... cleans up and
//     enters the ABORTING state if necessary.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//     int nError
//        error code
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//              5/10/92   BryanW      Added cancel notification
//                                    to host.
//
//------------------------------------------------------------------------

VOID EXTERN TerminateZmodem( NPZCB npZCB, int nError )
{
   int  i ;

   DP1( hWDB, "Zmodem : TerminateZmodem(%d)", nError ) ;

   // NEED: if error occurred, enter ABORTING stage and let
   // ZMODEM CompleteZmodem() finish termination

   LASTERROR( npZCB ) = nError ;

   CloseXferFile( npZCB ) ;

   if (nError)
   {
      ABORTING( npZCB ) = TRUE ;

      // if error occured,  clear incoming buffer and
      // send attention sequence

      ResetReadBuffer( npZCB ) ;
      ResetWriteBuffer( npZCB ) ;
      for (i = 0; i < lstrlen( ATTN( npZCB ) ); i++)
         WriteBufferChar( npZCB, ATTN( npZCB )[ i ] ) ;
      SendWriteBuffer( npZCB ) ;

      return ;
   }
   else
   {
      if (DIR_SEND == DIRECTION( npZCB ))
      {
         ResetWriteBuffer( npZCB ) ;
         WriteBufferChar( npZCB, 'O' ) ;
         WriteBufferChar( npZCB, 'O' ) ;
         SendWriteBuffer( npZCB ) ;
      }
   }

   // Otherwise, everything went A-OK... we're completing this round

   STATE( npZCB ) = 0 ;
   STATUS( npZCB ) = ZMS_COMPLETED ;

} // end of TerminateZmodem()

//------------------------------------------------------------------------
//  BOOL NEAR PASCAL ZmodemFSM( HWND hDlg )
//
//  Description:
//     This is the "guts" of the ZMODEM transfer protocol -
//     a Finite State Machine that processes / responds to
//     state changes as each state is executed.
//
//  Parameters:
//     HWND hDlg
//        handle to the ZMODEM dialog box
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR PASCAL ZmodemFSM( HWND hDlg )
{
   NPZCB  npZCB ;

   if (!(npZCB = (NPZCB) GET_PROP( hDlg, ATOM_NPZCB )))
      return ( FALSE ) ;

   if (ABORTING( npZCB ))
   {
      // During the abortion state, we simply clear the buffer
      // delay until the host stops sending data... this allows
      // us to continue without processing garbage data.

      if (ReadBufferChar( npZCB ) != -1)
      {
         DP1( hWDB, "ABORTING: setting ABORTDELAY to 0" ) ;

         ABORTDELAY( npZCB ) = 0 ;
         while (ReadBufferChar( npZCB ) != -1)
            ;

         return ( TRUE ) ;
      }
      else
         if (ABORTDELAY( npZCB )++ > 10)
         {
            DP1( hWDB, "ABORTING: ABORTDELAY() > 10 !!!" ) ;

            STATUS( npZCB ) = ZMS_ABORTED ;

            CompleteZmodem( npZCB ) ;
            return ( FALSE ) ;
         }

      return TRUE ;
   }

   if (DIR_RECEIVE == DIRECTION( npZCB))
      return ( ProcessZmodemReceiveState( npZCB ) ) ;
   else
      return ( ProcessZmodemSendState( npZCB ) ) ;

} // end of ZmodemFSM()

//------------------------------------------------------------------------
//  VOID NEAR PASCAL ResetReadBuffer( NPZCB npZCB )
//
//  Description:
//     ZAPs the incoming data by resetting INCOMINGPOS( npZCB ).
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              5/10/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR PASCAL ResetReadBuffer( NPZCB npZCB )
{
   INCOMINGPOS( npZCB ) = INCOMINGCOUNT( npZCB ) ;

} // end of ClearReadBuffer()

//------------------------------------------------------------------------
//  int NEAR PASCAL ReadBufferChar( NPZCB npZCB )
//
//  Description:
//     Gets a character from the incoming buffer.  If none,
//     returns -1.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/10/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

int NEAR PASCAL ReadBufferChar( NPZCB npZCB )
{
   BYTE  ch ;

   if (INCOMINGCOUNT( npZCB ) != INCOMINGPOS( npZCB ))
   {
      ch = INCOMING( npZCB )[ INCOMINGPOS( npZCB )++ ] ;
      if (INCOMINGPOS( npZCB ) == sizeof( INCOMING( npZCB ) ))
         INCOMINGPOS( npZCB ) = 0 ;

      return ( (int) ch ) ;
   }
   else
      return ( -1 ) ;

} // end of ReadBufferChar()

//------------------------------------------------------------------------
//  VOID EXTERN ReadAnyHdrInit( NPZCB npZCB )
//
//  Description:
//     Initialize for a ReadAnyHdr sequence.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID EXTERN ReadAnyHdrInit( NPZCB npZCB )
{
   CHARCOUNT( npZCB ) = 0 ;
   CANCOUNT( npZCB ) = 0 ;
   ESCSEQ( npZCB ) = FALSE ;
   PHASE( npZCB ) = PHASE_SYNC ;
   HEADERNEXT( npZCB ) = TRUE ;
   TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;

   // CRC type is forced to CRC_16 until a ZBIN32 type is read
   // CRCTYPE( npZCB ) = CRC_16 ;

} // end of ReadAnyHdrInit()

//------------------------------------------------------------------------
//  int EXTERN ReadAnyHdr( NPZCB npZCB )
//
//  Description:
//     Reads header of _ANY_ type.
//
//  Parameters:
//     NPZCB npZCB
//        pointer ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

int EXTERN ReadAnyHdr( NPZCB npZCB )
{
   register ch ;

   DP3( hWDB, "ReadAnyHdr() - entry" ) ;

   while (-1 != (ch = ReadBufferChar( npZCB )))
   {
//      if (READERROR( npZCB ))
//         return ( ZMODEM_ERROR ) ;

      switch (PHASE( npZCB ))
      {
         case PHASE_SYNC:
            DP5( hWDB, "PHASE_SYNC" ) ;
            switch (ch)
            {
               case 'O':
                  if (RECV_ZFIN == STATE( npZCB ))
                     return ( ZMODEM_EOF ) ;
                  CANCOUNT( npZCB ) = 0 ;
                  break ;

               case ZPAD | 0x80:
               case ZPAD:
                  CANCOUNT( npZCB ) = 0 ;
                  PHASE( npZCB ) = PHASE_PAD ;
                  break ;

               case ZDLE:
                  if (CANCOUNT( npZCB )++ > 5)
                     return ( ZMODEM_CAN ) ;
                  break ;

               case ZCRCW:
                  if (CANCOUNT( npZCB ))
                     return ( ZMODEM_ERROR ) ;

               default:
                  CANCOUNT( npZCB ) = 0 ;
            }
            break ;

         case PHASE_PAD:
            DP5( hWDB, "PHASE_PAD" ) ;
            switch (ch)
            {
               case ZPAD:
                  break ;

               case ZDLE:
                  PHASE( npZCB ) = PHASE_TYPE ;
                  break ;

               default:
                  CANCOUNT( npZCB ) = 0 ;
                  PHASE( npZCB ) = PHASE_SYNC ;
            }
            break ;

         case PHASE_TYPE:
            DP5( hWDB, "PHASE_TYPE" ) ;
            switch (ch)
            {
               case ZBIN:
                  ESCSEQ( npZCB ) = FALSE ;
                  CRCTYPE( npZCB ) = CRC_16 ;
                  PHASE( npZCB ) = PHASE_BINARY ;
                  break ;

               case ZBIN32:
                  ESCSEQ( npZCB ) = FALSE ;
                  CRCTYPE( npZCB ) = CRC_32 ;
                  PHASE( npZCB ) = PHASE_BINARY32 ;
                  break ;

               case ZHEX:
                  ESCSEQ( npZCB ) = FALSE ;
                  CRCTYPE( npZCB ) = CRC_16 ;
                  PHASE( npZCB ) = PHASE_HEX ;
                  break ;

               case ZDLE:
                  CANCOUNT( npZCB ) = 1 ;
                  PHASE( npZCB ) = PHASE_SYNC ;
                  break ;

               default:
                  CANCOUNT( npZCB ) = 0 ;
                  PHASE( npZCB ) = PHASE_SYNC ;
            }
            break ;

         case PHASE_BINARY:
            DP5( hWDB, "PHASE_BINARY" ) ;
            if (!ESCSEQ( npZCB ))
            {
               switch (ch)
               {
                  case 0x11:
                  case 0x91:
                  case 0x13:
                  case 0x93:
                     break ;

                  case ZDLE:
                     CANCOUNT( npZCB ) = 1 ;
                     ESCSEQ( npZCB ) = TRUE ;
                     break ;

                  default:
                     if ((!CTLESC( npZCB )) || (ch & 0x60))
                     {
#pragma message( "NEED : check for buffer overflow" )

                        INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = ch ;
                     }
                     break ;
               }
            }
            else
            {
               switch (ch)
               {
                  case ZDLE:
                     if (CANCOUNT( npZCB )++ >= 4)
                        return ( ZMODEM_CAN ) ;
                     break ;

                  case ZCRCE:
                  case ZCRCG:
                  case ZCRCQ:
                  case ZCRCW:
                     return ( ZMODEM_ERROR ) ;
                     break ;

                  case ZRUB0:
#pragma message( "NEED : check for buffer overflow" )

                     INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (BYTE) 0x7F ;
                     ESCSEQ( npZCB ) = FALSE ;
                     break;

                  case ZRUB1:
#pragma message( "NEED : check for buffer overflow" )

                     INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (BYTE) 0xFF ;
                     ESCSEQ( npZCB ) = FALSE ;
                     break ;

                  case 0x11:
                  case 0x91:
                  case 0x13:
                  case 0x93:
                     break ;

                  default:
                     if ((!CTLESC( npZCB )) || (ch & 0x60))
                     {
                        if ((ch & 0x60) == 0x40)
                        {
#pragma message( "NEED : check for buffer overflow" )

                           INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] =
                              (BYTE) (ch ^ 0x40) ;
                           ESCSEQ( npZCB ) = FALSE ;
                        }
                        else
                           return ( ZMODEM_ERROR ) ;
                     }
                     break ;
               }
            }
            break ;

         case PHASE_BINARY32:
            if (!ESCSEQ( npZCB ))
            {
               switch (ch)
               {
                  case 0x11:
                  case 0x91:
                  case 0x13:
                  case 0x93:
                     break ;

                  case ZDLE:
                     CANCOUNT( npZCB ) = 1 ;
                     ESCSEQ( npZCB ) = TRUE ;
                     break ;

                  default:
                     if ((!CTLESC( npZCB )) || (ch & 0x60))
                     {
#pragma message( "NEED : check for buffer overflow" )

                        INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (BYTE) ch ;
                     }
                     break ;
               }
            }
            else
            {
               switch (ch)
               {
                  case ZDLE:
                     if (CANCOUNT( npZCB )++ >= 4)
                        return ( ZMODEM_CAN ) ;
                     break ;

                  case ZCRCE:
                  case ZCRCG:
                  case ZCRCQ:
                  case ZCRCW:
                     return ( ZMODEM_ERROR ) ;
                     break ;

                  case ZRUB0:
#pragma message( "NEED : check for buffer overflow" )

                     INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = 0x7F ;
                     ESCSEQ( npZCB ) = FALSE ;
                     break;

                  case ZRUB1:
#pragma message( "NEED : check for buffer overflow" )

                     INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = 0xFF ;
                     ESCSEQ( npZCB ) = FALSE ;
                     break ;

                  case 0x11:
                  case 0x91:
                  case 0x13:
                  case 0x93:
                     break ;

                  default:
                     if ((!CTLESC( npZCB )) || (ch & 0x60))
                     {
                        if ((ch & 0x60) == 0x40)
                        {
#pragma message( "NEED : check for buffer overflow" )

                           INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] =
                              (BYTE) (ch ^ 0x40) ;
                           ESCSEQ( npZCB ) = FALSE ;
                        }
                        else
                           return ( ZMODEM_ERROR ) ;
                     }
                     break ;
               }
            }
            break ;

         case PHASE_HEX:
            DP5( hWDB, "PHASE_HEX" ) ;
            ch &= 0x7F ;
            if (!ESCSEQ( npZCB ))
            {
               switch (ch)
               {
                  case 0x11:
                  case 0x13:
                     break ;

                  case ZDLE:
                     return ( ZMODEM_ERROR ) ;

                  default:
                     if (ch >= ' ')
                     {
                        if (ch >= '0' && ch <= '9')
                           ch -= '0' ;
                        else if (ch >= 'a' && ch <= 'f')
                           ch = ch - 'a' + 10 ;
                        else
                           return ( ZMODEM_ERROR ) ;
                        INDATA( npZCB )[ CHARCOUNT( npZCB ) ] =
                           (BYTE) (ch << 4) ;
                        ESCSEQ( npZCB ) = TRUE ;
                     }
                     break ;
               }
            }
            else
            {
               switch (ch)
               {
                  case 0x11:
                  case 0x13:
                     break ;

                  case ZDLE:
                     return ( ZMODEM_ERROR ) ;

                  default:
                     if (ch >= ' ')
                     {
                        if (ch >= '0' && ch <= '9')
                           ch -= '0' ;
                        else if (ch >= 'a' && ch <= 'f')
                           ch = ch - 'a' + 10 ;
                        else
                           return ( ZMODEM_ERROR ) ;

#pragma message( "NEED : check for buffer overflow" )

                        INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] += (BYTE) ch ;
                        ESCSEQ( npZCB ) = FALSE ;
                     }
                     break ;
               }
            }
            break ;

         case PHASE_ZFIN:
            DP5( hWDB, "PHASE_ZFIN" ) ;
            if (CHARCOUNT( npZCB )++ > 0)
               return ( ZMODEM_EOF ) ;
            break ;
      }

      if (CHARCOUNT( npZCB ) >= 5 + (CRC_32 == CRCTYPE( npZCB ) ? 4 : 2))
         break ;
   }

   if (CHARCOUNT( npZCB ) >= 5 + (CRC_32 == CRCTYPE( npZCB ) ? 4 : 2))
   {
      if (!VerifyBlock( INDATA( npZCB ), 5, CRCTYPE( npZCB ) ))
         return ( ZMODEM_ERROR ) ;

      POSITION( npZCB ) =
         INDATA( npZCB )[ 4 ] & 0x00FF ;
      POSITION( npZCB ) =
         (POSITION( npZCB ) << 8) | (INDATA( npZCB )[ 3 ] & 0x00FF) ;
      POSITION( npZCB ) =
         (POSITION( npZCB ) << 8) | (INDATA( npZCB )[ 2 ] & 0x00FF) ;
      POSITION( npZCB ) =
         (POSITION( npZCB ) << 8) | (INDATA( npZCB )[ 1 ] & 0x00FF) ;
      return ( ZMODEM_HDR ) ;
   }

   if (GetCurrentTime() > TIMEOUT( npZCB ))
   {
      DP1( hWDB, "ZMODEM_TIMEOUT in ReadAnyHdr()" ) ;
      return ( ZMODEM_TIMEOUT ) ;
   }

   return ( -1 ) ;

} // end of ReadAnyHdr()

//------------------------------------------------------------------------
//  int EXTERN ReadBinData( NPZCB npZCB )
//
//  Description:
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------
int EXTERN ReadBinData( NPZCB npZCB )
{
   register  ch ;
   int       nCRCSize = (CRC_16 == CRCTYPE( npZCB )) ? 2 : 4 ;

   while (-1 != (ch = ReadBufferChar( npZCB )))
   {
      if (CHARCOUNT( npZCB ) > 1080)
         return ( ZMODEM_ERROR ) ;
//      if (READERROR( npZCB ))
//         return ( ZMODEM_ERROR ) ;

      if (!ESCSEQ( npZCB ))
      {
         switch (ch)
         {
            case 0x11:
            case 0x91:
            case 0x13:
            case 0x93:
               break ;

            case ZDLE:
               CANCOUNT( npZCB ) = 1 ;
               ESCSEQ( npZCB ) = TRUE ;
               break ;

            default:
#pragma message( "NEED : check for buffer overflow" )

               if ((!CTLESC( npZCB )) || (ch & 0x60))
                  INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (BYTE) ch ;

               break ;
         }
      }
      else
      {
         switch (ch)
         {
            case ZDLE:
               if (CANCOUNT( npZCB )++ >= 4)
                  return ( ZMODEM_CAN ) ;
               break ;

            case ZCRCE:
            case ZCRCG:
            case ZCRCQ:
            case ZCRCW:
               if (LENGTH( npZCB ))
                  return ( ZMODEM_ERROR ) ;

#pragma message( "NEED : check for buffer overflow" )

               INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (BYTE) ch ;
               LENGTH( npZCB ) = CHARCOUNT( npZCB ) ;
               ESCSEQ( npZCB ) = FALSE ;
               break ;

            case ZRUB0:
#pragma message( "NEED : check for buffer overflow" )

               INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (BYTE) 0x7F ;
               ESCSEQ( npZCB ) = FALSE ;
               break ;

            case ZRUB1:
#pragma message( "NEED : check for buffer overflow" )

               INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (BYTE) 0xFF ;
               ESCSEQ( npZCB ) = FALSE ;
               break ;

            case 0x11:
            case 0x91:
            case 0x13:
            case 0x93:
               break ;

            default:
               if ((!CTLESC( npZCB )) || (ch & 0x60))
                  if ((ch & 0x60) == 0x40)
                  {
#pragma message( "NEED : check for buffer overflow" )

                     INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] =
                        (BYTE) (ch ^ 0x40) ;
                     ESCSEQ( npZCB ) = FALSE ;
                  }
                  else
                     return ( ZMODEM_ERROR ) ;
               break ;
         }
      }

      if (LENGTH( npZCB ) &&
          (CHARCOUNT( npZCB ) >= LENGTH( npZCB ) + nCRCSize))
      {
         if (!VerifyBlock( INDATA( npZCB ), LENGTH( npZCB ),
                           CRCTYPE( npZCB ) ))
            return ( ZMODEM_ERROR ) ;
         return ( ZMODEM_DATA ) ;
      }
   }
   if (GetCurrentTime() > TIMEOUT( npZCB ))
      return ( ZMODEM_TIMEOUT ) ;

   return ( -1 ) ;

} // end of ReadBinData()

//------------------------------------------------------------------------
//  VOID EXTERN SendBinHdr( NPZCB npZCB, int nType, LPSTR lpHdr )
//
//  Description:
//     Sends ZMODEM binary header of nType.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//     int nType
//        header type
//
//     LPSTR lpHdr
//        pointer to header
//
//  History:   Date       Author      Comment
//              3/ 7/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID EXTERN SendBinHdr( NPZCB npZCB, int nType, LPSTR lpHdr )
{
   int   i ;
   DWORD dwCRC32 ;
   WORD  wCRC16 ;

   ResetWriteBuffer( npZCB ) ;

   if (nType == ZDATA)
      for (i = 0; i < NULLS( npZCB ); i++)
         WriteBufferData( npZCB, ASCII_NUL ) ;

   WriteBufferChar( npZCB, ZPAD ) ;
   WriteBufferChar( npZCB, ZDLE ) ;

   if (TXFCS32( npZCB ))
   {
      WriteBufferChar( npZCB, ZBIN32 ) ;
      WriteBufferData( npZCB, nType ) ;
      dwCRC32 = UPDCRC32( nType, 0xFFFFFFFFL ) ;
      for (i = 0; i < 4; i++)
      {
         WriteBufferData( npZCB, *lpHdr ) ;
         dwCRC32 = UPDCRC32( (255 & *lpHdr++), dwCRC32 ) ;
      }
      dwCRC32 ^= 0xFFFFFFFFL ;
      for (i = 0; i < 4; i++)
      {
         WriteBufferData( npZCB, LOWORD( dwCRC32 ) ) ;
         dwCRC32 >>= 8 ;
      }
   }
   else
   {
      WriteBufferChar( npZCB, ZBIN ) ;
      WriteBufferData( npZCB, nType ) ;
      wCRC16 = UPDCRC16( nType, 0 ) ;
      for (i = 0; i < 4; i++)
      {
         WriteBufferData( npZCB, *lpHdr ) ;
         wCRC16 = UPDCRC16( (255 & *lpHdr++), wCRC16 ) ;
      }
      wCRC16 = UPDCRC16( 0, UPDCRC16( 0, wCRC16 ) ) ;
      WriteBufferData( npZCB, wCRC16 >> 8 ) ;
      WriteBufferData( npZCB, wCRC16 ) ;
   }

   SendWriteBuffer( npZCB ) ;

} // end of SendBinHdr()

//------------------------------------------------------------------------
//  VOID EXTERN SendHexHdr( NPZCB npZCB, int nType, LPSTR lpHdr )
//
//  Description:
//     Sends a ZMODEM Hex header of nType.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//     int nType
//        header type
//
//     LPSTR lpHdr
//        pointer to header
//
//  History:   Date       Author      Comment
//              3/ 7/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID EXTERN SendHexHdr( NPZCB npZCB, int nType, LPSTR lpHdr )
{
   int   i ;
   WORD  wCRC16 ;

   ResetWriteBuffer( npZCB ) ;

   WriteBufferChar( npZCB, ZPAD ) ;
   WriteBufferChar( npZCB, ZPAD ) ;
   WriteBufferChar( npZCB, ZDLE ) ;
   WriteBufferChar( npZCB, ZHEX ) ;
   WriteBufferHexC( npZCB, nType ) ;

   wCRC16 = UPDCRC16( nType, 0 ) ;
   for (i = 0; i < 4; i++)
   {
      WriteBufferHexC( npZCB, *lpHdr ) ;
      wCRC16 = UPDCRC16( (255 & *lpHdr++), wCRC16 ) ;
   }
   wCRC16 = UPDCRC16( 0, UPDCRC16( 0, wCRC16 ) ) ;

   WriteBufferHexC( npZCB, wCRC16 >> 8 ) ;
   WriteBufferHexC( npZCB, wCRC16 ) ;

   WriteBufferChar( npZCB, ASCII_CR ) ;
   WriteBufferChar( npZCB, ASCII_LF ) ;

   // "uncork" the remote in case a fake XOFF has stopped data flow

   if (nType != ZFIN)
      WriteBufferChar( npZCB, ASCII_XON ) ;

   SendWriteBuffer( npZCB ) ;

} // end of SendHexHdr()

//------------------------------------------------------------------------
//  VOID EXTERN ResetWriteBuffer( NPZCB npZCB )
//
//  Description:
//     Resets the write buffer position.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID EXTERN ResetWriteBuffer( NPZCB npZCB )
{
   OUTCOUNT( npZCB ) = 0 ;

} // end of ResetWriteBuffer()

//------------------------------------------------------------------------
//  VOID EXTERN WriteBufferChar( NPZCB npZCB, int ch )
//
//  Description:
//     Puts a character into the write buffer.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//     int ch
//        character to store in write buffer
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID EXTERN WriteBufferChar( NPZCB npZCB, int ch )
{
   OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = LOBYTE( ch ) ;

} // end of WriteBufferChar()

//------------------------------------------------------------------------
//  VOID EXTERN WriteBufferData( NPZCB npZCB, int ch )
//
//  Description:
//     Puts a DLE encoded character into the write buffer.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//     int ch
//        character to encode and store in write buffer
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID EXTERN WriteBufferData( NPZCB npZCB, int ch )
{
   switch (ch &= 0xFF)
   {
      case ZDLE:
         OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = ZDLE ;
         LASTCHAR( npZCB ) = (ch ^= 0x40) ;
         OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = LOBYTE( ch ) ;
         break ;

      case 0x0D:
      case 0x8D:
         if ((!CTLESC( npZCB )) && ((LASTCHAR( npZCB ) & 0x7F) != '@'))
         {
            LASTCHAR( npZCB ) = ch ;
            OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = LOBYTE( ch ) ;
            break ;
         }

         // ... fall through ...

      case 0x10:
      case 0x11:
      case 0x13:
      case 0x90:
      case 0x91:
      case 0x93:
         OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = ZDLE ;
         LASTCHAR( npZCB ) = (ch ^= 0x40) ;
         OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = LOBYTE( ch ) ;
         break ;

      default:
         if ((CTLESC( npZCB )) && !(ch & 0x60))
         {
            OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = ZDLE ;
            ch ^= 0x40 ;
         }
         LASTCHAR( npZCB ) = ch ;
         OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = LOBYTE( ch ) ;
   }

} // end of WriteBufferData()

//------------------------------------------------------------------------
//  VOID EXTERN WriteBufferHexC( NPZCB npZCB, int ch )
//
//  Description:
//     Converts the character to a HEX value and stores in
//     the write buffer.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//     int ch
//        character to convert and store in write buffer
//
//  History:   Date       Author      Comment
//
//------------------------------------------------------------------------

VOID EXTERN WriteBufferHexC( NPZCB npZCB, int ch )
{
   static char digits[] = "0123456789abcdef" ;

   OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = digits[ (ch >> 4) & 0x0f ] ;
   OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = digits[ ch & 0x0f ] ;

} // end of WriteBufferHexC()

//------------------------------------------------------------------------
//  VOID EXTERN SendWriteBuffer( NPZCB npZCB )
//
//  Description:
//     Sends the output buffer to the communications device.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID EXTERN SendWriteBuffer( NPZCB npZCB )
{
   DP4( hWDB, "SendWriteBuffer, npZCB == %04X", npZCB ) ;

   // send notification to parent
#ifdef IHATEUNIX
   (*SENDPROC( npZCB ))( HPARENTWND( npZCB ), OUTDATA( npZCB ), OUTCOUNT( npZCB ) ) ;
#else

   if (HIWORD( SENDPROC( npZCB ) ) != 0)
   {
      DP1( hWDB, "SENDPROC( %u )", OUTCOUNT( npZCB ) ) ;
      (*SENDPROC( npZCB ))( HPARENTWND( npZCB ), OUTDATA( npZCB ), OUTCOUNT( npZCB ) ) ;
   }
   else
   {
      DP1( hWDB, "No SENDPROC using SendMessage... %u", OUTCOUNT( npZCB ) ) ;
      SendMessage( HPARENTWND( npZCB ), LOWORD( SENDPROC( npZCB ) ),
                   (WPARAM) OUTCOUNT( npZCB ),
                   (LPARAM) (LPSTR) OUTDATA( npZCB ) ) ;
   }
#endif

   TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;

} // end of SendWriteBuffer()

//------------------------------------------------------------------------
//  VOID EXTERN XmitZACK( NPZCB npZCB )
//
//  Description:
//     Transmit a ZACK packet.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID EXTERN XmitZACK( NPZCB npZCB )
{
   char  szBuf[ 8 ] ;

   DWORDToBuf( szBuf, FILEBYTES( npZCB ) ) ;
   SendHexHdr( npZCB, ZACK, szBuf ) ;

   CONSECERRORS( npZCB ) = 0 ;

} // end of XmitZACK()

//------------------------------------------------------------------------
//  VOID EXTERN XmitZNAK( NPZCB npZCB )
//
//  Description:
//     Transmit a ZNAK packet.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID EXTERN XmitZNAK( NPZCB npZCB )
{
   char szBuf[ 8 ] ;

   DWORDToBuf( szBuf, FILEBYTES( npZCB ) ) ;
   SendHexHdr( npZCB, ZNAK, szBuf ) ;
   UpdateErrorCount( npZCB ) ;

} // end of XmitZNAK()

//------------------------------------------------------------------------
//  BOOL EXTERN ProcessZNAK( NPZCB npZCB )
//
//  Description:
//     Process a ZNAK packet.   
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL EXTERN ProcessZNAK( NPZCB npZCB )
{
   if (!UpdateErrorCount( npZCB ))
      return ( FALSE ) ;

   // send notification to parent
#ifdef IHATEUNIX
   (*SENDPROC( npZCB ))( HPARENTWND( npZCB ), OUTDATA( npZCB ), OUTCOUNT( npZCB ) ) ;
#else
   if (HIWORD( SENDPROC( npZCB ) ) != 0)
   {
      DP1( hWDB, "SENDPROC( %u )", OUTCOUNT( npZCB ) ) ;
      (*SENDPROC( npZCB ))( HPARENTWND( npZCB ), OUTDATA( npZCB ), OUTCOUNT( npZCB ) ) ;
   }
   else
   {
      SendMessage( HPARENTWND( npZCB ), LOWORD( SENDPROC( npZCB ) ),
                   (WPARAM) OUTCOUNT( npZCB ),
                   (LPARAM)(LPSTR)OUTDATA( npZCB ) ) ;
   }
#endif
   CHARCOUNT( npZCB ) = 0 ;
   LASTERROR( npZCB ) = ERROR_NAK ;

   TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;

   return ( TRUE ) ;

} // end of ProcessZNAK()

//------------------------------------------------------------------------
//  VOID EXTERN XmitZFIN( NPZCB npZCB )
//
//  Description:
//     Transmit a ZFIN packet.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID EXTERN XmitZFIN( NPZCB npZCB )
{
   char  szBuf[ 8 ] ;

   szBuf[ 0 ] = 0 ;
   szBuf[ 1 ] = 0 ;
   szBuf[ 2 ] = 0 ;
   szBuf[ 3 ] = 0 ;
   SendHexHdr( npZCB, ZFIN, szBuf ) ;

} // end of XmitZFIN()

//------------------------------------------------------------------------
//  VOID EXTERN DateTimeToBuf( NPZCB npZCB, LPSTR lpBuf )
//
//  Description:
//     Converts date and time (and file length) to long-octal
//     format and stuffs into string buffer.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//     LPSTR lpBuf
//        pointer to string buffer
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//              5/18/92   BryanW      Brain-dead wsprintf() doesn't
//                                    support octal!
//
//------------------------------------------------------------------------

VOID EXTERN DateTimeToBuf( NPZCB npZCB, LPSTR lpBuf )
{
   int    i, nMon, nDay, nYear, nHour, nMin, nSec ;
   DWORD  dwTotalDays, dwTotalSecs ;
   

#ifndef IHATEUNIX
   int    DaysInMonth[] = { 31, 28, 31, 30, 31, 30,
                            31, 31, 30, 31, 30, 31 } ;
#endif

   nDay = FILEDATE( npZCB ) & 0x001F ;
   nMon = (FILEDATE( npZCB ) >> 5) & 0x000F ;
   nYear = ((FILEDATE( npZCB ) >> 9) & 0x007F) + 10 ;
   nSec = (FILETIME( npZCB ) & 0x001F) * 2 ;
   nMin = (FILETIME( npZCB ) >> 5) & 0x003F ;
   nHour = (FILETIME( npZCB ) >> 11) & 0x001F ;

   DP3( hWDB, "Date & Time: %d/%d/%d %d:%d:%d",
        nMon, nDay, nYear + 70, nHour, nMin, nSec ) ;

   dwTotalDays = nYear * 365 + nYear / 4 ;

   // add days in months

   for (i = 1; i < nMon; i++)
      dwTotalDays += DaysInMonth[ i - 1 ] ;
   dwTotalDays += nDay ;

   dwTotalSecs = dwTotalDays * 86400L ;
   dwTotalSecs += (DWORD) nHour * 3600L ;
   dwTotalSecs += (nMin * 60) + nSec ;

   wsprintf( lpBuf, "%lu ", FILESIZE( npZCB ) ) ;
   DWORDToOctalStr( &lpBuf[ lstrlen( lpBuf ) ], dwTotalSecs ) ;

} // end of DateTimeToBuf()

//------------------------------------------------------------------------
//  VOID EXTERN DWORDToBuf( LPSTR lpBuf, DWORD dwValue )
//
//  Description:
//     Stuffs a LONG value into the character buffer.
//
//  Parameters:
//     NPSTR lpBuf
//        pointer to character buffer
//
//     DWORD dwValue
//        value to stuff into buffer
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID EXTERN DWORDToBuf( LPSTR lpBuf, DWORD dwValue )
{
   lpBuf[ 0 ] = (char) LOBYTE( LOWORD( dwValue ) ) ;
   lpBuf[ 1 ] = (char) HIBYTE( LOWORD( dwValue ) ) ;
   lpBuf[ 2 ] = (char) LOBYTE( HIWORD( dwValue ) ) ;
   lpBuf[ 3 ] = (char) HIBYTE( HIWORD( dwValue ) ) ;

} // end of DWORDToBuf()

//------------------------------------------------------------------------
//  VOID EXTERN DWORDToOctalStr( LPSTR lpBuf, DWORD dwValue )
//
//  Description:
//     Converts a DWORD to an octal string.  This is necessary
//     in this DLL since wsprintf() is brain-dead and %o is not
//     supported.
//
//  Parameters:
//     LPSTR lpBuf
//        used to store the resultant string
//
//     DWORD dwValue
//        value to convert
//
//  History:   Date       Author      Comment
//              5/18/92   BryanW      Wrote it.
//              5/18/92   BryanW      wsprintf() is the most worthless SPOS
//                                    function ever written!
//
//------------------------------------------------------------------------

VOID EXTERN DWORDToOctalStr( LPSTR lpBuf, DWORD dwValue )
{
   int    i ;
   char   szTemp[ 12 ] ;
   LPSTR  lpTemp = &szTemp[ 11 ] ;

   DP1( hWDB, "converting %lu...", dwValue ) ;

   *lpTemp-- = NULL ;
   for (i = 0; i < 10; i++)
   {
      *lpTemp-- = (char) LOBYTE( LOWORD( dwValue & (DWORD) 0x07 ) ) + '0' ;
      dwValue >>= 3 ;
   }
   *lpTemp = (char) LOBYTE( LOWORD(dwValue & (DWORD) 0x07 ) ) + '0' ;
   lstrcpy( lpBuf, lpTemp ) ;

   DP1( hWDB, "octal conversion: %s", lpBuf ) ;

} // end of DWORDToOctalStr()

//------------------------------------------------------------------------
//  VOID EXTERN DWORDToDateTime( NPZCB npZCB, DWORD dwDateTime )
//
//  Description:
//     Converts the date and time (ZMODEM format) to the DOS
//     file date and time format.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//     DWORD dwDateTime
//        date and time in ZMODEM format
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//              5/18/92   BryanW      Add two to year for round-off
//                                    errors. (2/4 = .5)
//
//------------------------------------------------------------------------

VOID EXTERN DWORDToDateTime( NPZCB npZCB, DWORD dwDateTime )
{
   int    i, nMon, nDay, nYear, nHour, nMin, nSec ;
   DWORD  dwTotalSecs, dwTotalDays ;

#ifndef IHATEUNIX
   int    DaysInMonth[] = { 31, 28, 31, 30, 31, 30,
                            31, 31, 30, 31, 30, 31 } ;
#endif

   dwTotalSecs = dwDateTime ;
   DP3( hWDB, "dwTotalSecs = %lu", dwTotalSecs ) ;

   // convert seconds to days (seconds in a day = 86400)

   dwTotalDays = dwTotalSecs / (DWORD) 86400 ;

   // compute year and day

   nYear = (int) (dwTotalDays / (DWORD) 365) ;
   nDay = (int) (dwTotalDays % (DWORD) 365) ;
   
   // NEED: also adjust for centuries

   // adjust for leap year... 

   nDay -= (nYear + 2) / 4 ;
   if (nDay < 0)
   {
      nDay += 365 ;
      nYear-- ;
   }

   // compute month and day

   nMon = 1 ;
   for (i = 0; i < 12; i++)
   {
      if ((nDay - DaysInMonth[ i ]) <= 0)
         break ;
      nDay -= DaysInMonth[ i ] ;
      nMon++ ;
   }

   // convert left over seconds to hour, min, sec

   dwTotalSecs %= (DWORD) 86400 ;
   nHour = (int) (dwTotalSecs / (DWORD) 3600) ;

   dwTotalSecs %= (DWORD) 3600 ;
   nMin = (int) (dwTotalSecs / (DWORD) 60) ;

   nSec = (int) (dwTotalSecs % (DWORD) 60) ;

   DP3( hWDB, "Date & Time: %d/%d/%d %d:%d:%d",
        nMon, nDay, nYear + 70, nHour, nMin, nSec ) ;

   // date and time to DOS (1980) format

   FILEDATE( npZCB ) = (WORD) (((nYear - 10) << 9) + (nMon << 5) + nDay) ;
   FILETIME( npZCB ) = (WORD) ((nHour << 11) + (nMin << 5) + nSec / 2) ;

} // end of DWORDToDateTime()

//------------------------------------------------------------------------
//  BOOL EXTERN GetFileStatus( NPZCB npZCB, LPFILESTATUS lpFileStatus )
//
//  Description:
//     Calls DOS to get file status information (date, time & length).
//
//  Parameters:
//     HFILE hFile
//        DOS handle to file
//
//     LPFILESTATUS
//        pointer to file status structure
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL EXTERN GetFileStatus( NPZCB npZCB, LPFILESTATUS lpFileStatus )
{
#ifdef IHATEUNIX

   int    i, nMon, nDay, nYear, nHour, nMin, nSec ;
   DWORD  dwTotalSecs, dwTotalDays ;
   struct stat Stat ;

   fstat( HFILE( npZCB ), &Stat ) ;

   dwTotalSecs = Stat.st_mtime ;

   // convert seconds to days (seconds in a day = 86400)

   dwTotalDays = dwTotalSecs / (DWORD) 86400 ;

   // compute year and day

   nYear = (int) (dwTotalDays / (DWORD) 365) ;
   nDay = (int) (dwTotalDays % (DWORD) 365) ;
   
   // NEED: also adjust for centuries

   // adjust for leap year... 

   nDay -= (nYear / 4) ;
   if (nDay < 0)
   {
      nDay += 365 ;
      nYear-- ;
   }

   // compute month and day

   nMon = 1 ;
   for (i = 0; i < 12; i++)
   {
      if ((nDay - DaysInMonth[ i ]) <= 0)
         break ;
      nDay -= DaysInMonth[ i ] ;
      nMon++ ;
   }

   // convert left over seconds to hour, min, sec

   dwTotalSecs %= (DWORD) 86400 ;
   nHour = (int) (dwTotalSecs / (DWORD) 3600) ;

   dwTotalSecs %= (DWORD) 3600 ;
   nMin = (int) (dwTotalSecs / (DWORD) 60) ;

   nSec = (int) (dwTotalSecs % (DWORD) 60) ;

   lpFileStatus->wDate = (WORD)(((nYear - 10) << 9) + (nMon << 5) + nDay) ;
   lpFileStatus->wTime = (WORD)((nHour << 11) + (nMin << 5) + nSec / 2) ;
   lpFileStatus->dwLength = (DWORD)Stat.st_size ;

   return TRUE ;

#else
   BOOL  fRetVal = TRUE ;
   WORD  wDate, wTime ;
   HFILE hFile = HFILE(npZCB) ;


   _asm
   {
         mov   ax, 5700h         ; DOS get status
         mov   bx, hFile         ; file handle
         int   21h               ; call DOS
         jc    GFS_ERROR

         mov   wDate, dx         ; date is in dx
         mov   wTime, cx         ; time is in cx
         jmp   short GFS_EXIT

      GFS_ERROR:
         mov   fRetVal, FALSE ;

      GFS_EXIT:
   }

   if (fRetVal)
   {
      lpFileStatus -> wDate = wDate ;
      lpFileStatus -> wTime = wTime ;
      lpFileStatus -> dwLength = (DWORD) _filelength( HFILE(npZCB) ) ;
   }

   return ( fRetVal ) ;

#endif

} // end of GetFileStatus()

//------------------------------------------------------------------------
//  BOOL NEAR PASCAL SetFileStatus( NPZCB npZCB, LPFILESTATUS lpFileStatus )
//
//  Description:
//     Sets the file's date and time given the information
//     in lpFileStatus.
//
//  Parameters:
//     HFILE hFile
//        handle to file
//
//     LPFILESTATUS
//        pointer to FILESTATUS structure
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR PASCAL SetFileStatus( NPZCB npZCB, LPFILESTATUS lpFileStatus )
{

#ifdef IHATEUNIX

   int    i, nMon, nDay, nYear, nHour, nMin, nSec ;
   DWORD  dwTotalDays, dwTotalSecs ;
   struct utimbuf Times ;

   nDay = lpFileStatus->wDate & 0x001F ;
   nMon = (lpFileStatus->wDate & 0x01E0) >> 4 ;
   nYear = (lpFileStatus->wDate & 0xFE00) >> 8  + 10 ;
   nSec = (lpFileStatus->wTime & 0x001F) * 2 ;
   nMin = (lpFileStatus->wTime & 0x007E0) >> 4 ;
   nHour = (lpFileStatus->wTime & 0xF800) >> 10 ;

   dwTotalDays = nYear * 365 + (nYear / 4) ;

   // add months

   for (i = 0; i < nMon; i++)
      dwTotalDays += DaysInMonth[ i ] ;
   dwTotalDays += nDay ;

   dwTotalSecs = dwTotalDays * 86400 ;
   dwTotalSecs += (nHour * 3600) + (nMin * 60) + nSec ;

   Times.actime  = dwTotalSecs ;
   Times.modtime = dwTotalSecs ;

   utime( FILENAME( npZCB ), &Times ) ;

#else

   BOOL  fRetVal = TRUE ;
   WORD  wDate, wTime ;
   HFILE hFile = HFILE(npZCB) ;

   wDate = lpFileStatus -> wDate ;
   wTime = lpFileStatus -> wTime ;

   _asm
   {
         mov   ax, 5701h         ; DOS set status
         mov   bx, hFile         ; file handle
         mov   dx, wDate         ; date is in dx
         mov   cx, wTime         ; time is in cx
         int   21h               ; call DOS
         jc    SFS_ERROR
         jmp   short SFS_EXIT

      SFS_ERROR:
         mov   fRetVal, FALSE ;

      SFS_EXIT:
   }
   return ( fRetVal ) ;
#endif

} // end of SetFileStatus()

//------------------------------------------------------------------------
//  LPSTR EXTERN StripPath( LPSTR lpFileName )
//
//  Description:
//     Returns a pointer to the beginning of the actual file name.
//
//  Parameters:
//     LPSTR lpFileName
//        pointer to file name
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

LPSTR EXTERN StripPath( LPSTR lpFileName )
{
   LPSTR lpPtr ;

   for (lpPtr = lpFileName; *lpFileName; lpFileName++)
   {
      if (*lpFileName == '/' || *lpFileName == '\\' || *lpFileName == ':')
         lpPtr = lpFileName + 1 ;
   }
   return( lpPtr ) ;

} // end of StripPath()

//------------------------------------------------------------------------
//  BOOL NEAR PASCAL VerifyBlock( LPSTR lpBuf, int nCount, BYTE bCRCType )
//
//  Description:
//     Verifies the CRC16/CRC32 value of the given block.
//
//  Parameters:
//     LPSTR lpBuf
//        pointer to buffer
//
//     int nCount
//        size of block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR PASCAL VerifyBlock( LPSTR lpBuf, int nCount, BYTE bCRCType )
{
	int     i ;
   WORD    wCRC16 ;
   DWORD   dwCRC32 ;

   if (CRC_32 == bCRCType)
   {
      DP1( hWDB, "nCount = %d", nCount ) ;

      if (nCount == 5 || nCount == 20 || nCount == 19)
      {
         char szBuf[30] ;

         _fmemcpy( szBuf, lpBuf, nCount ) ;
         szBuf[nCount] = '\0' ;

         DP1( hWDB, "VerifyBlock (%d) : '%s'", nCount, (LPSTR)szBuf ) ;
      }

      dwCRC32 = 0xFFFFFFFFL ;
	   while (nCount-- > 0)
		   dwCRC32 = UPDCRC32( (255 & *(lpBuf++)), dwCRC32 ) ;
      dwCRC32 ^= 0xFFFFFFFFL ;
      DP3( hWDB, "CRC-32 calculated: %04x%04x",
           HIWORD( dwCRC32 ), LOWORD( dwCRC32 ) ) ;

      D(
         DWORD dwInCRC32 ;

         dwInCRC32 = 0 ;
         for (i = 0; i < 4; i++)
            dwInCRC32 |= ((DWORD) ((BYTE) lpBuf[ i ])) << (i * 8) ;
         DP3( hWDB, "Incoming CRC-32: %04x%04x",
              HIWORD( dwInCRC32 ), LOWORD( dwInCRC32 ) ) ;

      ) ;

      for (i = 0; i < 4; i++)
      {
         if (LOBYTE( LOWORD( dwCRC32 ) ) != (BYTE) *(lpBuf++))
            return ( FALSE ) ;
         dwCRC32 >>= 8 ;
      }
      return ( TRUE ) ;
   }
   else
   {
	   wCRC16 = 0;
	   while (nCount-- > 0)
		   wCRC16 = UPDCRC16( (255 & *(lpBuf++)), wCRC16 ) ;
      wCRC16 = UPDCRC16( 0, UPDCRC16( 0, wCRC16 ) ) ;
      DP3( hWDB, "CRC-16 calculated: %04x", wCRC16 ) ;

      D(
         WORD wInCRC16 ;

         wInCRC16 = 0 ;
         for (i = 0; i < 2; i++)
            wInCRC16 |= ((DWORD) ((BYTE) lpBuf[ i ])) << ((1 - i) * 8) ;
         DP3( hWDB, "Incoming CRC-16: %04x", wInCRC16 ) ;

      ) ;

      for (i = 0; i < 2; i++)
      {
         if (LOBYTE( wCRC16 ) != (BYTE) lpBuf[ 1 - i ])
            return ( FALSE ) ;
         wCRC16 >>= 8 ;
      }
      return ( TRUE ) ;
   }

} // end of VerifyBlock()


//---------------------------------------------------------------------------
//  End of File: zmodem.c
//---------------------------------------------------------------------------



