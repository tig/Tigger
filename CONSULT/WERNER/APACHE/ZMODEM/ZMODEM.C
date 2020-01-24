//---------------------------------------------------------------------------
//
//  Module: zmodem.c
//
//  Purpose:
//     ZMODEM protocol for APACHE.  Adapted to Windows by
//     Bryan A. Woodruff based on original code from
//     Chuck Forsberg at Omen Technology Inc.
//
//  Description of functions:
//     Function descriptions are contained in function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"   // precompiled header shit

#include "zmodem.h"
#include "crctab.h"    // CRC tables

//------------------------------------------------------------------------
//  int FAR PASCAL GetZmodemUseCount( VOID )
//
//  Description:
//     Returns the usage count of this module.  This is the number
//     of active zmodem sessions.
//
//  Parameters:
//     None.
//
//  History:   Date       Author      Comment
//              3/10/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

int FAR PASCAL GetZmodemUseCount( VOID )
{
   return ( nUseCount ) ;

} // end of GetZmodemUseCount()

//------------------------------------------------------------------------
//  HWND FAR PASCAL ZmodemReceive( HWND hWnd, LPSTR lpFileName )
//
//  Description:
//     Initiates protocol receive of lpFileName.  Returns a handle
//     to the created dialog box.
//
//  Parameters:
//     HWND hWnd
//        handle to parent
//
//     LPSTR lpFileName
//        filename to save
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

HWND FAR PASCAL ZmodemReceive( HWND hWnd, LPSTR lpFileName )
{
   NPZCB     npZCB ;

   if (NULL == (npZCB = CreateZCB()))
   {
      DP1( hWDB, "CreateZCB failed!" ) ;
      SendAbortString( hWnd ) ;
      return( NULL ) ;
   }

   InitZmodem( npZCB ) ;

   HPARENTWND( npZCB ) = hWnd ;
   DIRECTION( npZCB ) = DIR_RECEIVE ;
   STATE( npZCB ) = RECV_INIT ;
   CURFILE( npZCB ) = 0 ;
   FILEBYTES( npZCB ) = 0 ;
   FILESIZE( npZCB ) = 0 ;
   TRANSMITTED( npZCB ) = 0 ;
   RECEIVED( npZCB ) = 0 ;
   TOTALFILES( npZCB ) = 0 ;
   BLOCKSIZE( npZCB ) = 1024 ;
   HFILE( npZCB ) = -1 ;

   if (lpFileName == NULL || *lpFileName == NULL)
      *FILENAME( npZCB ) = NULL ;
   else
      lstrcpy( FILENAME( npZCB ), lpFileName ) ;

   if (NULL != *FILENAME( npZCB ))
   {
      HFILE( npZCB ) = OpenXferFile( npZCB ) ;
      if (-1 == HFILE( npZCB ))
      {
         SendAbortString( hWnd ) ;
         DestroyZCB( npZCB ) ;
         return ( NULL ) ;
      }
   }

   // save starting time

   time( &TIMESTARTED( npZCB ) ) ;

   HDLG( npZCB ) =
      CreateDialogParam( hInstLib, MAKEINTRESOURCE( ZMODEMDLGBOX ),
                         hWnd, ZmodemDlgProc, MAKELONG( npZCB, 0 ) ) ;

   SetTimer( HDLG( npZCB ), TIMERID_ZMODEMFSM, TIMERINTERVAL_ZMODEMFSM,
             NULL ) ;

   // NEED: Check for window's status (e.g. is ICONIC?)

   ShowWindow( HDLG( npZCB ), SW_SHOW ) ;

   return ( HDLG( npZCB ) ) ;
                               
} // end of ZmodemReceive()

//------------------------------------------------------------------------
//  HWND FAR PASCAL ZmodemSend( HWND hWnd, LPSTR lpFileName )
//
//  Description:
//     Initiates protocol send of lpFileName.  Returns a handle
//     to the created dialog box.
//
//  Parameters:
//     HWND hWnd
//        handle to parent window
//
//     LPSTR lpFileName
//        filename to send
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

HWND FAR PASCAL ZmodemSend( HWND hWnd, LPSTR lpFileName )
{
   NPZCB       npZCB ;
   FILESTATUS  fsXferFile ;

   if (NULL == (npZCB = CreateZCB()))
   {
      DP1( hWDB, "CreateZCB failed!" ) ;
      SendAbortString( hWnd ) ;
      return( NULL ) ;
   }

   InitZmodem( npZCB ) ;

   HPARENTWND( npZCB ) = hWnd ;
   DIRECTION( npZCB ) = DIR_SEND ;
   STATE( npZCB ) = SENT_INIT ;
   CURFILE( npZCB ) = 0 ;
   FILEBYTES( npZCB ) = 0 ;
   TRANSMITTED( npZCB ) = 0 ;
   RECEIVED( npZCB ) = 0 ;
   TOTALFILES( npZCB ) = 0 ;
   HFILE( npZCB ) = -1 ;
   STREAMING( npZCB ) = FALSE ;
   USESTREAM( npZCB ) = FALSE ;

   if (lpFileName == NULL || *lpFileName == NULL)
   {
      SendAbortString( hWnd ) ;
      DestroyZCB( npZCB ) ;
      return ( NULL ) ;
   }

   lstrcpy( FILENAME( npZCB ), lpFileName ) ;

   if (NULL != *FILENAME( npZCB ))
   {
      HFILE( npZCB ) = OpenXferFile( npZCB ) ;
      if (-1 == HFILE( npZCB ))
      {
         SendAbortString( hWnd ) ;
         DestroyZCB( npZCB ) ;
         return ( NULL ) ;
      }
   }

   // get file information

   GetFileStatus( HFILE( npZCB ), &fsXferFile ) ;
   FILETIME( npZCB ) = fsXferFile.wTime ;
   FILEDATE( npZCB ) = fsXferFile.wDate ;
   FILESIZE( npZCB ) = fsXferFile.dwLength ;

   // save starting time

   time( &TIMESTARTED( npZCB ) ) ;

   HDLG( npZCB ) =
      CreateDialogParam( hInstLib, MAKEINTRESOURCE( ZMODEMDLGBOX ),
                         hWnd, ZmodemDlgProc, MAKELONG( npZCB, 0 ) ) ;

   SetTimer( HDLG( npZCB ), TIMERID_ZMODEMFSM, TIMERINTERVAL_ZMODEMFSM,
             NULL ) ;

   // NEED: Check for parent window's status (e.g. is ICONIC?)

   ShowWindow( HDLG( npZCB ), SW_SHOW ) ;

   return ( HDLG( npZCB ) ) ;
                               
} // end of ZmodemSend()

//------------------------------------------------------------------------
//  BOOL FAR PASCAL ZmodemDlgProc( HWND hDlg, UINT uMsg,
//                                 WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This is the ZMODEM dialog procedure.  Basically, a timer
//     is used to negotiate the state changes.  LOWORD( lParam )
//     on WM_INITDIALOG is _expected_ to contain the NPZCB.
//
//  Parameters:
//     Same as all standard dialog boxes.
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL FAR PASCAL ZmodemDlgProc( HWND hDlg, UINT uMsg,
                               WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_INITDIALOG:
      {
         NPZCB  npZCB ;

         npZCB = (NPZCB) LOWORD( lParam ) ;
         SET_PROP( hDlg, ATOM_NPZCB, (HANDLE) npZCB ) ;

         // NEED: center dialog box and perform shadow
         // control init

         // initiate transfer sequence

         // NEED: change title bar to reflect direction

         if (DIR_RECEIVE == DIRECTION( npZCB ))
         {
            TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;
            XmitZRINITHdr( npZCB ) ;
            ReadAnyHdrInit( npZCB ) ;
         }
         else
         {
            TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;
            XmitZRQINIT( npZCB ) ;
            ReadAnyHdrInit( npZCB ) ;
         }

         UpdateDialogBox( npZCB ) ;

      }
      return ( TRUE ) ;

      case WM_MEASUREITEM:
         return ( DlgMeasureItem( hDlg, (LPMEASUREITEMSTRUCT) lParam ) ) ;

      case WM_DRAWITEM:
         return( DlgDrawItem( hDlg, (LPDRAWITEMSTRUCT) lParam ) ) ;

      case WM_CMD_RXBLK:
      {
         NPZCB  npZCB ;
         WORD   wBlkLen, wMaxLen ;

         npZCB = (NPZCB) GET_PROP( hDlg, ATOM_NPZCB ) ;

         wBlkLen = wParam ;
         while (wBlkLen)
         {
            DPF3( hWDB, "INCOMINGCOUNT = %d. INCOMINGPOS = %d\r\n",
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
                         (LPSTR) lParam, wMaxLen ) ;
               ((LPSTR) lParam) += wMaxLen ;
               INCOMINGCOUNT( npZCB ) += wMaxLen ;
               if (INCOMINGCOUNT( npZCB ) == sizeof( INCOMING( npZCB ) ))
                  INCOMINGCOUNT( npZCB ) = 0 ;
               wBlkLen -= wMaxLen ;
            }
            ZmodemFSM( hDlg ) ;
         }
      }
      return ( TRUE ) ;

      case WM_COMMAND:
      {
         switch (wParam)
         {
            case IDCANCEL:
            {
               NPZCB  npZCB ;

               npZCB = (NPZCB) GET_PROP( hDlg, ATOM_NPZCB ) ;
               TerminateZmodem( npZCB, ERROR_LOCALCANCEL ) ;
            }
            break ;
         }
         break ;
      }
      return ( TRUE ) ;

      case WM_TIMER:
         return ( ZmodemFSM( hDlg ) ) ;
             
      case WM_DESTROY:
         // NEED: perform shadow control call

         REMOVE_PROP( hDlg, ATOM_NPZCB ) ;
         break ;
   }
   return ( FALSE ) ;

} // end of ZmodemDlgProc()

//------------------------------------------------------------------------
//  BOOL NEAR DlgMeasureItem( HWND hWnd, LPMEASUREITEMSTRUCT lpMIS )
//
//  Description:
//     Computes the size of the item and fills the structure
//     appropriately.
//
//  Parameters:
//     HWND hWnd
//        handle to window
//
//     LPMEASUREITEMSTRUCT lpMIS
//        pointer to MEASUREITEMSTRUCT
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR DlgMeasureItem( HWND hWnd, LPMEASUREITEMSTRUCT lpMIS )
{
   RECT  rcClient ;

   if (ODT_BUTTON == lpMIS -> CtlType)   // it better be!
   {
      GetClientRect( GetDlgItem( hWnd, lpMIS -> CtlID), &rcClient ) ;
      lpMIS -> itemWidth = rcClient.left - rcClient.right ;
      lpMIS -> itemHeight = rcClient.bottom - rcClient.top ;
   }
   return ( TRUE ) ;

} // end of DlgMeasureItem()

//------------------------------------------------------------------------
//  BOOL NEAR DlgDrawItem( HWND hWnd, LPDRAWITEMSTRUCT lpDIS )
//
//  Description:
//     Draws the item specified in lpDIS.
//
//  Parameters:
//     HWND hWnd
//        handle to window
//
//     LPDRAWITEMSTRUCT
//        pointer to DRAWITEMSTRUCT
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR DlgDrawItem( HWND hWnd, LPDRAWITEMSTRUCT lpDIS )
{
   char   szBuf[ MAXLEN_TEMPSTR ] ;
   DWORD  dwExtent ;
   RECT   rc1, rc2 ;
   WORD   wHeight, wPct, wWidth, wXPos ;

   if (ODT_BUTTON == lpDIS -> CtlType)   // it better be!
   {
      // determine if user is trying to set focus...
      // if so, set focus to IDCANCEL

      if (ODA_FOCUS == lpDIS -> itemAction)
      {
         if (ODS_FOCUS & lpDIS -> itemState)
            SetFocus( GetDlgItem( hWnd, IDCANCEL ) ) ;

         return ( TRUE ) ;
      }

      if (ODA_DRAWENTIRE == lpDIS -> itemAction)
      {
         rc1 = lpDIS -> rcItem ;
         rc2 = lpDIS -> rcItem ;
         wWidth = rc1.right - rc1.left ;
         wHeight = rc1.bottom - rc1.top ;
         GetWindowText( lpDIS -> hwndItem, szBuf, MAXLEN_TEMPSTR ) ;
         wPct = LOWORD( AtoL( szBuf ) ) ;
         wXPos = LOWORD( (DWORD) wWidth * (DWORD) wPct / 100L ) ;
         rc1.right = wXPos ;
         rc2.left = wXPos ;
         dwExtent = GetTextExtent( lpDIS -> hDC, szBuf, lstrlen( szBuf ) ) ;

         SetBkColor( lpDIS -> hDC, GetSysColor( COLOR_HIGHLIGHT ) ) ;
         SetTextColor( lpDIS -> hDC, GetSysColor( COLOR_HIGHLIGHTTEXT ) ) ;
         ExtTextOut( lpDIS -> hDC, (wWidth - LOWORD( dwExtent )) / 2,
                     (wHeight - HIWORD( dwExtent )) / 2,
                     ETO_OPAQUE | ETO_CLIPPED, &rc1, szBuf, lstrlen( szBuf ),
                     NULL ) ;

         SetBkColor( lpDIS -> hDC, GetSysColor( COLOR_WINDOW ) ) ;
         SetTextColor( lpDIS -> hDC, GetSysColor( COLOR_WINDOWTEXT ) ) ;
         ExtTextOut( lpDIS -> hDC, (wWidth - LOWORD( dwExtent )) / 2,
                     (wHeight - HIWORD( dwExtent )) / 2,
                     ETO_OPAQUE | ETO_CLIPPED, &rc2, szBuf, lstrlen( szBuf ),
                     NULL ) ;
      }
   }
   return ( TRUE ) ;

} // end of DlgDrawItem()

//------------------------------------------------------------------------
//  BOOL NEAR UpdateDialogBox( NPZCB npZCB )
//
//  Description:
//     Updates the information in the ZMODEM transfer dialog box.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/10/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR UpdateDialogBox( NPZCB npZCB )
{
   char   szBuf[ MAXLEN_TEMPSTR ] ;
   HWND   hDlg ;
   DWORD  dwElapsedTime ;
   WORD   wCPS, wProgress ;

   hDlg = HDLG( npZCB ) ;

   // file name
   SetDlgItemText( hDlg, IDD_FILENAME, StripPath( FILENAME( npZCB ) ) ) ;

   // file size
   wsprintf( szBuf, "%lu", FILESIZE( npZCB ) ) ;
   SetDlgItemText( hDlg, IDD_FILESIZE, szBuf ) ;

   // total blocks

   wsprintf( szBuf, "%u", TOTALBLOCKS( npZCB ) ) ;
   SetDlgItemText( hDlg, IDD_TOTALBLOCKS, szBuf ) ;

   // block check

   // block count

   SetDlgItemInt( hDlg, IDD_BLOCKCOUNT, BLOCKCOUNT( npZCB ), FALSE ) ;

   // byte count

   wsprintf( szBuf, "%lu", FILEBYTES( npZCB ) ) ;
   SetDlgItemText( hDlg, IDD_BYTECOUNT, szBuf ) ;

   // total received

   wsprintf( szBuf, "%lu", RECEIVED( npZCB ) ) ;
   SetDlgItemText( hDlg, IDD_RECEIVED, szBuf ) ;

   // compute avg cps

   // NEED: Compute wrap-around midnight

   if (DIR_RECEIVE == DIRECTION( npZCB ))
   {
      dwElapsedTime = TIMEREAD( npZCB ) - TIMESTARTED( npZCB ) ;
      wCPS = LOWORD( RECEIVED( npZCB ) / dwElapsedTime ) ;
      wsprintf( szBuf, "%u", wCPS ) ;
      SetDlgItemText( hDlg, IDD_AVERAGECPS, szBuf) ;
   }

   // progress indicator

   if (FILESIZE( npZCB ) > 0)
      wProgress = LOWORD( 100L * FILEBYTES( npZCB) / FILESIZE( npZCB ) ) ;
   else
      wProgress = 0 ;
   wsprintf( szBuf, "%u%%", wProgress ) ;
   SetDlgItemText( hDlg, IDD_PROGRESS, szBuf ) ;

   // update immediately

   UpdateWindow( hDlg ) ;

   return ( TRUE ) ;

} // end of UpdateDialogBox()

//------------------------------------------------------------------------
//  NPZCB NEAR CreateZCB( VOID )
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

NPZCB NEAR CreateZCB( VOID )
{
   return ( (NPZCB) LocalAlloc( LPTR, sizeof( ZCB ) ) ) ;

} // end of CreateZCB()

//------------------------------------------------------------------------
//  VOID NEAR DestroyZCB( NPZCB npZCB )
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

VOID NEAR DestroyZCB( NPZCB npZCB )
{
   LocalFree( npZCB ) ;

} // end of DestroyZCB()

//------------------------------------------------------------------------
//  HFILE NEAR OpenXferFile( NPZCB npZCB )
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

HFILE NEAR OpenXferFile( NPZCB npZCB )
{
   HFILE     hFile ;
   OFSTRUCT  of ;

   // NEED:  Create backup files

   if (DIRECTION( npZCB ) == DIR_RECEIVE)
      hFile = OpenFile( FILENAME( npZCB ), &of, OF_CREATE ) ;
   else
      hFile = OpenFile( FILENAME( npZCB ), &of, OF_READ ) ;

   return ( hFile ) ;

} // end of OpenXferFile()

//------------------------------------------------------------------------
//  VOID NEAR CloseXferFile( NPZCB npZCB )
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

VOID NEAR CloseXferFile( NPZCB npZCB )
{
   if (-1 == HFILE( npZCB ))
      return ;

   // NEED: check if receiving file and set file date/time

   if (DIR_RECEIVE == DIRECTION( npZCB ))
   {
      FILESTATUS fsXferFile ;

      fsXferFile.wDate = FILEDATE( npZCB ) ;
      fsXferFile.wTime = FILETIME( npZCB ) ;
      SetFileStatus( HFILE( npZCB ), &fsXferFile ) ;
   }

   _lclose( HFILE( npZCB ) ) ;
   HFILE( npZCB ) = -1 ;

} // end of CloseXferFile()

//------------------------------------------------------------------------
//  BOOL NEAR UpdateErrorCount( NPZCB npZCB )
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

BOOL NEAR UpdateErrorCount( NPZCB npZCB )
{
   CONSECERRORS( npZCB )++ ;
   TOTALERRORS( npZCB )++ ;

   UpdateDialogBox( npZCB ) ;
   if (MAXERRORS( npZCB ) != 0 && CONSECERRORS( npZCB ) > MAXERRORS( npZCB ))
   {
      TerminateZmodem( npZCB, ERROR_TOOMANYERRORS ) ;
      return ( FALSE ) ;
   }
   return ( TRUE ) ;

} // end of UpdateErrorCount()

//------------------------------------------------------------------------
//  VOID NEAR InitZmodem( NPZCB npZCB )
//
//  Description:
//     Initializes the ZMODEM protocol information.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR InitZmodem( NPZCB npZCB )
{
   int  i ;

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
   ATTN( npZCB )[ 24 ] = NULL ;

} // end of InitZmodem()

//------------------------------------------------------------------------
//  VOID NEAR CompleteZmodem( NPZCB npZCB )
//
//  Description:
//
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR CompleteZmodem( NPZCB npZCB )
{
   if (HDLG( npZCB ))
   {
      KillTimer( HDLG( npZCB ), TIMERID_ZMODEMFSM ) ;
      DestroyWindow( HDLG( npZCB ) ) ;
      HDLG( npZCB ) = NULL ;
   }

   nUseCount-- ;

} // end of CompleteZmodem()

//------------------------------------------------------------------------
//  VOID NEAR TerminateZmodem( NPZCB npZCB, int nError )
//
//  Description:
//
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
//
//------------------------------------------------------------------------

VOID NEAR TerminateZmodem( NPZCB npZCB, int nError )
{
   // NEED: terminate conversation with ZMODEM cooperative

   CloseXferFile( npZCB ) ;

   // NEED: if error occurred, enter ABORTING stage and let
   // ZMODEM CompleteZmodem() finish termination

   if (HDLG( npZCB ))
   {
      KillTimer( HDLG( npZCB ), TIMERID_ZMODEMFSM ) ;
      DestroyWindow( HDLG( npZCB ) ) ;
      HDLG( npZCB ) = NULL ;
   }

   nUseCount-- ;

} // end of TerminateZmodem()

//------------------------------------------------------------------------
//  BOOL NEAR ZmodemFSM( HWND hDlg )
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

BOOL NEAR ZmodemFSM( HWND hDlg )
{
   NPZCB  npZCB ;

   if (!(npZCB = (NPZCB) GET_PROP( hDlg, ATOM_NPZCB )))
      return ( FALSE ) ;

   // NEED: check for disconnect

   if (ABORTING( npZCB ))
   {
      // NEED: hack to clear buffer
      // (e.g. Read until delay of ten uninterrupted cycles)

      CompleteZmodem( npZCB ) ;
      return ( FALSE ) ;
   }

   if (DIR_RECEIVE == DIRECTION( npZCB))
      return ( ProcessZmodemReceiveState( npZCB ) ) ;
   else
      return ( ProcessZmodemSendState( npZCB ) ) ;

} // end of ZmodemFSM()

//------------------------------------------------------------------------
//  BOOL NEAR ProcessZmodemReceiveState( NPZCB npZCB )
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

BOOL NEAR ProcessZmodemReceiveState( NPZCB npZCB )
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
         LASTERROR( npZCB ) = ERROR_GENERAL ;
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
            TerminateZmodem( npZCB, NULL ) ;
            break ;
      }
   }

   return ( TRUE ) ;

} // end of ProcessZmodemReceiveState()

//------------------------------------------------------------------------
//  BOOL NEAR ProcessZmodemSendState( NPZCB npZCB )
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

BOOL NEAR ProcessZmodemSendState( NPZCB npZCB )
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
         LASTERROR( npZCB ) = ERROR_GENERAL ;

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
                        UpdateDialogBox( npZCB ) ;

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
//  int NEAR ReadBufferChar( NPZCB npZCB )
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

int NEAR ReadBufferChar( NPZCB npZCB )
{
   BYTE  ch ;

   if (INCOMINGCOUNT( npZCB ) != INCOMINGPOS( npZCB ))
   {
      ch = INCOMING( npZCB )[ INCOMINGPOS( npZCB )++ ] ;
      if (INCOMINGPOS( npZCB ) == sizeof( INCOMING( npZCB ) ))
         INCOMINGPOS( npZCB ) = 0 ;

//      DPF3( hWDB, "Character read: %02x ", ch ) ;
      return ( (int) ch ) ;
   }
   else
      return ( -1 ) ;

} // end of ReadBufferChar()

//------------------------------------------------------------------------
//  VOID NEAR ReadAnyHdrInit( NPZCB npZCB )
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

VOID NEAR ReadAnyHdrInit( NPZCB npZCB )
{
   CHARCOUNT( npZCB ) = 0 ;
   CANCOUNT( npZCB ) = 0 ;
   ESCSEQ( npZCB ) = FALSE ;
   PHASE( npZCB ) = PHASE_SYNC ;
   HEADERNEXT( npZCB ) = TRUE ;
   TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;

} // end of ReadAnyHdrInit()

//------------------------------------------------------------------------
//  int NEAR ReadAnyHdr( NPZCB npZCB )
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

int NEAR ReadAnyHdr( NPZCB npZCB )
{
   register ch ;

//   DP3( hWDB, "ReadAnyHdr() - entry\r\n" ) ;

   while (-1 != (ch = ReadBufferChar( npZCB )))
   {
//      if (READERROR( npZCB ))
//         return ( ZMODEM_ERROR ) ;

      switch (PHASE( npZCB ))
      {
         case PHASE_SYNC:
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
                  if (CANCOUNT( npZCB ) > 5)
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
//            DP( hWDB, "PHASE_PAD" ) ;
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
//            DP3( hWDB, "PHASE_TYPE" ) ;
            switch (ch)
            {
               case ZBIN:
                  ESCSEQ( npZCB ) = FALSE ;
                  PHASE( npZCB ) = PHASE_BINARY ;
                  break ;

#ifdef NOT_USED
               case ZBIN32:
                  ESCSEQ( npZCB ) = FALSE ;
                  PHASE( npZCB ) = PHASE_BINARY32 ;
                  break ;
#endif

               case ZHEX:
                  ESCSEQ( npZCB ) = FALSE ;
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
//            DP3( hWDB, "PHASE_BINARY" ) ;
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
                        // NEED: check for buffer overflow

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
                     // NEED: check for buffer overflow

                     INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (char) 0x7F ;
                     ESCSEQ( npZCB ) = FALSE ;
                     break;

                  case ZRUB1:
                     // NEED: check for buffer overflow

                     INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (char) 0xFF ;
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
                           // NEED: check for buffer overflow

                           INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] =
                              (char) (ch ^ 0x40) ;
                           ESCSEQ( npZCB ) = FALSE ;
                        }
                        else
                           return ( ZMODEM_ERROR ) ;
                     }
                     break ;
               }
            }
            break ;

#ifdef NOT_USED
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
                        // NEED: check for buffer overflow

                        INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (char) ch ;
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
                     // NEED: check for buffer overflow

                     INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = 0x7F ;
                     ESCSEQ( npZCB ) = FALSE ;
                     break;

                  case ZRUB1:
                     // NEED: check for buffer overflow

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
                           // NEED: check for buffer overflow

                           INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] =
                              (char) (ch ^ 0x40) ;
                           ESCSEQ( npZCB ) = FALSE ;
                        }
                        else
                           return ( ZMODEM_ERROR ) ;
                     }
                     break ;
               }
            }
            break ;
#endif
         case PHASE_HEX:
//            DP3( hWDB, "PHASE_HEX" ) ;
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
                           (char) (ch << 4) ;
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

                        // NEED: check for buffer overflow

                        INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] += (char) ch ;
                        ESCSEQ( npZCB ) = FALSE ;
                     }
                     break ;
               }
            }
            break ;

         case PHASE_ZFIN:
//            DP3( hWDB, "PHASE_ZFIN" ) ;
            if (CHARCOUNT( npZCB )++ > 0)
               return ( ZMODEM_EOF ) ;
            break ;
      }
      if (CHARCOUNT( npZCB ) >= 7)
         break ;
   }

   if (CHARCOUNT( npZCB ) >= 7)
   {
      DP3( hWDB, "Verifying Block." ) ;
      if (!VerifyBlock( INDATA( npZCB ), 5 ))
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
      return ( ZMODEM_TIMEOUT ) ;

   return ( -1 ) ;

} // end of ReadAnyHdr()

//------------------------------------------------------------------------
//  VOID NEAR ReadBinDataInit( NPZCB npZCB )
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

VOID NEAR ReadBinDataInit( NPZCB npZCB )
{
   CHARCOUNT( npZCB ) = 0 ;
   CANCOUNT( npZCB ) = 0 ;
   ESCSEQ( npZCB ) = FALSE ;
   LENGTH( npZCB ) = 0 ;
   HEADERNEXT( npZCB ) = FALSE ;
   TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;

} // end of ReadBinDataInit()

//------------------------------------------------------------------------
//  int NEAR ReadBinData( NPZCB npZCB )
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

int NEAR ReadBinData( NPZCB npZCB )
{
   register ch ;

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
               // NEED: check for buffer overflow

               if ((!CTLESC( npZCB )) || (ch & 0x60))
                  INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (char) ch ;

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

               // NEED: check for buffer overflow

               INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (char) ch ;
               LENGTH( npZCB ) = CHARCOUNT( npZCB ) ;
               ESCSEQ( npZCB ) = FALSE ;
               break ;

            case ZRUB0:
               // NEED: check for buffer overflow

               INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (char) 0x7F ;
               ESCSEQ( npZCB ) = FALSE ;
               break ;

            case ZRUB1:
               // NEED: check for buffer overflow

               INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] = (char) 0xFF ;
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
                     // NEED: check for buffer overflow

                     INDATA( npZCB )[ CHARCOUNT( npZCB )++ ] =
                        (char) (ch ^ 0x40) ;
                     ESCSEQ( npZCB ) = FALSE ;
                  }
                  else
                     return ( ZMODEM_ERROR ) ;
               break ;
         }
      }

      if (LENGTH( npZCB ) && (CHARCOUNT( npZCB ) >= LENGTH( npZCB ) + 2))
      {
         if (!VerifyBlock( INDATA( npZCB ), LENGTH( npZCB ) ))
            return ( ZMODEM_ERROR ) ;
         return ( ZMODEM_DATA ) ;
      }
   }
   if (GetCurrentTime() > TIMEOUT( npZCB ))
      return ( ZMODEM_TIMEOUT ) ;

   return ( -1 ) ;

} // end of ReadBinData()

//------------------------------------------------------------------------
//  VOID NEAR SendBinHdr( NPZCB npZCB, int nType, LPSTR lpHdr )
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

VOID NEAR SendBinHdr( NPZCB npZCB, int nType, LPSTR lpHdr )
{
   int   i ;
   WORD  wCRC16 ;
   LONG  lCRC32 ;

   ResetWriteBuffer( npZCB ) ;

   if (nType == ZDATA)
      for (i = 0; i < NULLS( npZCB ); i++)
         WriteBufferData( npZCB, NULL ) ;

   WriteBufferChar( npZCB, ZPAD ) ;
   WriteBufferChar( npZCB, ZDLE ) ;

#ifdef NOT_USED
   if (TXFCS32( npZCB ))
   {
      WriteBufferChar( npZCB, ZBIN32 ) ;
      WriteBufferData( npZCB, nType ) ;
      lCRC32 = UPCRC32( nType, 0xFFFFFFFF ) ;

      for (i = 0; i < 4; i++)
      {
         WriteBufferData( npZCB, *lpHdr ) ;
         lCRC32 = UPDCRC32( (255 & *lpHdr++), lCRC32 ) ;
      }
      lCRC32 = ~lCRC32 ;
      for (i = 0; i < 4; i++)
      {
         WriteBufferData( npZCB, LOWORD( lCRC32 ) ) ;
         lCRC32 >>= 8 ;
      }
   }
   else
   {
#endif
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

#ifdef NOT_USED
   }
#endif

   SendWriteBuffer( npZCB ) ;

} // end of SendBinHdr()

//------------------------------------------------------------------------
//  VOID NEAR SendHexHdr( NPZCB npZCB, int nType, LPSTR lpHdr )
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

VOID NEAR SendHexHdr( NPZCB npZCB, int nType, LPSTR lpHdr )
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
//  VOID NEAR SendBinData( NPZCB npZCB, int nFrameEnd, LPSTR lpBuf, 
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

VOID NEAR SendBinData( NPZCB npZCB, int nFrameEnd, LPSTR lpBuf, int nLength )
{
   LONG  lCRC32 ;
   WORD  wCRC16 ;

   ResetWriteBuffer( npZCB ) ;

#ifdef NOT_USED
   if (TXFCS32( npZCB ))
   {
      lCRC32 = 0xFFFFFFFF ;
      for (; --nLength >= 0;)
      {
         WriteBufferData( npZCB, *lpBuf ) ;
         lCRC32 = UPDCRC32( (255 & *lpBuf++), lCRC32 ) ;
      }
      WriteBufferChar( npZCB, ZDLE ) ;
      WriteBufferChar( npZCB, nFrameEnd ) ;
      lCRC32 = UPDCRC32( nFrameEnd, lCRC32 ) ;

      lCRC32 = ~lCRC32 ;
      for (nLength = 4; --nLength >= 0;)
      {
         WriteBufferData( npZCB, LOWORD( lCRC32 ) ) ;
         lCRC32 >>= 8 ;
      }
   }
   else
   {
#endif
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
#ifdef NOT_USED
   }
#endif
   if (nFrameEnd == ZCRCW)
      WriteBufferChar( npZCB, ASCII_XON ) ;

   SendWriteBuffer( npZCB ) ;

} // end of SendBinData()

//------------------------------------------------------------------------
//  VOID NEAR ResetWriteBuffer( NPZCB npZCB )
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

VOID NEAR ResetWriteBuffer( NPZCB npZCB )
{
   OUTCOUNT( npZCB ) = 0 ;

} // end of ResetWriteBuffer()

//------------------------------------------------------------------------
//  VOID NEAR WriteBufferChar( NPZCB npZCB, int ch )
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

VOID NEAR WriteBufferChar( NPZCB npZCB, int ch )
{
   OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = (char) LOBYTE( ch ) ;

} // end of WriteBufferChar()

//------------------------------------------------------------------------
//  VOID NEAR WriteBufferData( NPZCB npZCB, int ch )
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

VOID NEAR WriteBufferData( NPZCB npZCB, int ch )
{
   switch (ch &= 0xFF)
   {
      case ZDLE:
         OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = ZDLE ;
         LASTCHAR( npZCB ) = (ch ^= 0x40) ;
         OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = (char) LOBYTE( ch ) ;
         break ;

      case 0x0D:
      case 0x8D:
         if ((!CTLESC( npZCB )) && ((LASTCHAR( npZCB ) & 0x7F) != '@'))
         {
            LASTCHAR( npZCB ) = ch ;
            OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = (char) LOBYTE( ch ) ;
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
         OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = (char) LOBYTE( ch ) ;
         break ;

      default:
         if ((CTLESC( npZCB )) && !(ch & 0x60))
         {
            OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = ZDLE ;
            ch ^= 0x40 ;
         }
         LASTCHAR( npZCB ) = ch ;
         OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = (char) LOBYTE( ch ) ;
   }

} // end of WriteBufferData()

//------------------------------------------------------------------------
//  VOID NEAR WriteBufferHexC( NPZCB npZCB, int ch )
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

VOID NEAR WriteBufferHexC( NPZCB npZCB, int ch )
{
   static char digits[] = "0123456789abcdef" ;

   OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = digits[ (ch >> 4) & 0x0f ] ;
   OUTDATA( npZCB )[ OUTCOUNT( npZCB )++ ] = digits[ ch & 0x0f ] ;

} // end of WriteBufferHexC()

//------------------------------------------------------------------------
//  VOID NEAR SendWriteBuffer( NPZCB npZCB )
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

VOID NEAR SendWriteBuffer( NPZCB npZCB )
{
   // send notification to parent

   SendMessage( HPARENTWND( npZCB ), WM_CMD_TXBLK,
                (WPARAM) OUTCOUNT( npZCB ),
                (LPARAM) (LPSTR) OUTDATA( npZCB ) ) ;

   TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;

} // end of SendWriteBuffer()

//------------------------------------------------------------------------
//  VOID NEAR XmitZRINITHdr( NPZCB npZCB )
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

VOID NEAR XmitZRINITHdr( NPZCB npZCB )
{
   char szBuf[ 8 ] ;

   szBuf[ 0 ] = 0 ;
   szBuf[ 1 ] = 0 ;
   szBuf[ 2 ] = 0 ;
   szBuf[ 3 ] = CANFDX | CANOVIO ;
   SendHexHdr( npZCB, ZRINIT, szBuf ) ;

} // end of XmitZRINITHdr()

//------------------------------------------------------------------------
//  VOID NEAR ProcessZSINITHdr( NPZCB npZCB )
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

VOID NEAR ProcessZSINITHdr( NPZCB npZCB )
{
} // end of ProcessZSINITHdr()

//------------------------------------------------------------------------
//  VOID NEAR ProcessZSINITData( NPZCB npZCB )
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

VOID NEAR ProcessZSINITData( NPZCB npZCB )
{
   XmitZACK( npZCB ) ;

   _fstrncpy( ATTN( npZCB ), &(INDATA( npZCB )[ 4 ]), 32 ) ;

   // MUST: Reset transfer indicators

   // ResetXferIndicators( FALSE ) ;

} // end of ProcessZSINITData()

//------------------------------------------------------------------------
//  VOID NEAR ProcessZFILEHdr( NPZCB npZCB )
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

VOID NEAR ProcessZFILEHdr( NPZCB npZCB )
{
} // end of ProcessZFILEHdr()

//------------------------------------------------------------------------
//  BOOL NEAR ProcessZFILEData( NPZCB npZCB )
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

BOOL NEAR ProcessZFILEData( NPZCB npZCB )
{
   char  szBuf[ 132 ] ;

   FILEBYTES( npZCB ) = 0 ;
   FILEDATE( npZCB ) = 0 ;
   FILETIME( npZCB ) = 0 ;

   if (-1 == HFILE( npZCB ))
   {
      char      szPath[ MAXLEN_PATH ] ;
      register  i ;
      DWORD     dwModTime ;
      OFSTRUCT  of ;
      WORD      wMaxLen ;

      // NEED: compute exact path for file transfer

      lstrcpy( StripPath( FILENAME( npZCB ) ), INDATA( npZCB ) ) ;
      dwModTime = 0 ;
      wsscanf( &INDATA( npZCB )[ lstrlen( INDATA( npZCB ) ) + 1 ],
               "%lu %lo", (DWORD FAR *) &FILESIZE( npZCB ),
               (DWORD FAR *) &dwModTime ) ;

      if (dwModTime > 0)
         DWORDToDateTime( npZCB, dwModTime ) ;

      DPF3( hWDB, "ProcessZFILEData: FILENAME = %s\r\n",
            (LPSTR) FILENAME( npZCB ) ) ;

      // check for possible file recovery mode

      if (-1 == (HFILE( npZCB ) = OpenFile( FILENAME( npZCB ), &of,
                                            OF_READ )))
      {
         if (-1 == (HFILE( npZCB ) = OpenXferFile( npZCB )))
         {
            TerminateZmodem( npZCB, ERROR_DISKERROR ) ;
            return ( FALSE ) ;
         }
      }
      else
      {
         FILESTATUS  fsXferFile ;

         GetFileStatus( HFILE( npZCB ), &fsXferFile ) ;
         _lclose( HFILE( npZCB ) ) ;

         // NEED: create a backup copy

         // check file date, time and size

         if ((fsXferFile.wDate == FILEDATE( npZCB )) &&
             (fsXferFile.wTime == FILETIME( npZCB )) &&
             (fsXferFile.dwLength <= FILESIZE( npZCB )))
         {
            // re-open for file recovery

            if (-1 == (HFILE( npZCB ) = OpenFile( FILENAME( npZCB ),
                                                  &of,
                                                  OF_READWRITE )))
            {
               TerminateZmodem( npZCB, ERROR_DISKERROR ) ;
               return ( FALSE ) ;
            }
            FILEBYTES( npZCB ) =
               _llseek( HFILE( npZCB ), fsXferFile.dwLength, SEEK_SET ) ;
         }
         else if (-1 == (HFILE( npZCB ) = OpenXferFile( npZCB )))
         {
            TerminateZmodem( npZCB, ERROR_DISKERROR ) ;
            return ( FALSE ) ;
         }
      }

      CURFILE( npZCB )++ ;
      BLOCKCOUNT( npZCB ) = 0 ;

      // NEED: What are XferIndicators ?

      // ResetXferIndicators( FALSE ) ;
   }

   // NEED: compute block size

   TOTALBLOCKS( npZCB ) = LOWORD( FILESIZE( npZCB ) / BLOCKSIZE( npZCB ) ) ;

   DWORDToBuf( szBuf, FILEBYTES( npZCB ) ) ;
   SendHexHdr( npZCB, ZRPOS, szBuf ) ;

   return ( TRUE ) ;

} // end of ProcessZFILEData()

//------------------------------------------------------------------------
//  VOID NEAR ProcessZDATAHdr( NPZCB npZCB )
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

VOID NEAR ProcessZDATAHdr( NPZCB npZCB )
{
   char   szBuf[ 8 ] ;

   if (POSITION( npZCB ) > FILEBYTES( npZCB ))
   {
      DPF3( hWDB, "Sending ZRPOS %lu\r\n", POSITION( npZCB ) ) ;

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
//  BOOL NEAR ProcessZDATAData( NPZCB npZCB )
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

BOOL NEAR ProcessZDATAData( NPZCB npZCB )
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
         DP3( hWDB, "Disk Error!" ) ;
         TerminateZmodem( npZCB, ERROR_DISKERROR ) ;
      }
      else if (nWritten != LENGTH( npZCB ) - 1)
      {
         DP3( hWDB, "Disk Full!" ) ;
         TerminateZmodem( npZCB, ERROR_DISKFULL ) ;
      }
      else
      {
         CONSECERRORS( npZCB ) = 0 ;

         // update stuff for user

         UpdateDialogBox( npZCB ) ;

         return ( TRUE ) ;
      }
   }
   return ( FALSE ) ;

} // end of ProcessZDATAData()

//------------------------------------------------------------------------
//  VOID NEAR XmitZACK( NPZCB npZCB )
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

VOID NEAR XmitZACK( NPZCB npZCB )
{
   char  szBuf[ 8 ] ;

   DWORDToBuf( szBuf, FILEBYTES( npZCB ) ) ;
   SendHexHdr( npZCB, ZACK, szBuf ) ;

   CONSECERRORS( npZCB ) = 0 ;

} // end of XmitZACK()

//------------------------------------------------------------------------
//  VOID NEAR XmitZNAK( NPZCB npZCB )
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

VOID NEAR XmitZNAK( NPZCB npZCB )
{
   char szBuf[ 8 ] ;

   DWORDToBuf( szBuf, FILEBYTES( npZCB ) ) ;
   SendHexHdr( npZCB, ZNAK, szBuf ) ;
   UpdateErrorCount( npZCB ) ;

} // end of XmitZNAK()

//------------------------------------------------------------------------
//  VOID NEAR XmitZRPOS( NPZCB npZCB )
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

VOID NEAR XmitZRPOS( NPZCB npZCB )
{
   char szBuf[ 8 ] ;

   DWORDToBuf( szBuf, FILEBYTES( npZCB ) ) ;
   SendHexHdr( npZCB, ZRPOS, szBuf ) ;

} // end of XmitZRPOS()

//------------------------------------------------------------------------
//  BOOL NEAR ProcessZNAK( NPZCB npZCB )
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

BOOL NEAR ProcessZNAK( NPZCB npZCB )
{
   if (!UpdateErrorCount( npZCB ))
      return ( FALSE ) ;

   // send notification to parent

   SendMessage( HPARENTWND( npZCB ), WM_CMD_TXBLK,
                (WPARAM) OUTCOUNT( npZCB ),
                (LPARAM) (LPSTR) OUTDATA( npZCB ) ) ;

   CHARCOUNT( npZCB ) = 0 ;
   LASTERROR( npZCB ) = ERROR_NAK ;

   TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;

   return ( TRUE ) ;

} // end of ProcessZNAK()

//------------------------------------------------------------------------
//  VOID NEAR XmitZFIN( NPZCB npZCB )
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

VOID NEAR XmitZFIN( NPZCB npZCB )
{
   char  szBuf[ 8 ] ;

   szBuf[ 0 ] = 0 ;
   szBuf[ 1 ] = 0 ;
   szBuf[ 2 ] = 0 ;
   szBuf[ 3 ] = 0 ;
   SendHexHdr( npZCB, ZFIN, szBuf ) ;

} // end of XmitZFIN()

//------------------------------------------------------------------------
//  VOID NEAR XmitZRQINIT( NPZCB npZCB )
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

VOID NEAR XmitZRQINIT( NPZCB npZCB )
{
   char  szBuf[ 8 ] ;

   szBuf[ 0 ] = 0 ;
   szBuf[ 1 ] = 0 ;
   szBuf[ 2 ] = 0 ;
   szBuf[ 3 ] = 0 ;
   SendHexHdr( npZCB, ZRQINIT, szBuf ) ;

} // end of XmitZRQINIT()

//------------------------------------------------------------------------
//  VOID NEAR ProcessZRINITHdr( NPZCB npZCB )
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

VOID NEAR ProcessZRINITHdr( NPZCB npZCB )
{
   if (INDATA( npZCB )[ 4 ] & CANFDX)
      USESTREAM( npZCB ) = TRUE ;
   else
      USESTREAM( npZCB ) = FALSE ;

   BLOCKSIZE( npZCB ) = INDATA( npZCB )[ 1 ] * 256 + INDATA( npZCB )[ 2 ] ;
   USESTREAM( npZCB ) = (BLOCKSIZE( npZCB ) == 0 && USESTREAM( npZCB ) ) ;

   if (BLOCKSIZE( npZCB ) < 32 || BLOCKSIZE( npZCB ) > 1024)
      BLOCKSIZE( npZCB ) = 1024 ;

} // end of ProcessZRINITHdr()

//------------------------------------------------------------------------
//  VOID NEAR ProcessZRPOS( NPZCB npZCB )
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

VOID NEAR ProcessZRPOS( NPZCB npZCB )
{
   FILEBYTES( npZCB ) = POSITION( npZCB ) ;
   _llseek( HFILE( npZCB ), POSITION( npZCB ), SEEK_SET ) ;
   SENDHEADER( npZCB ) = TRUE ;

} // end of ProcessZRPOS()

//------------------------------------------------------------------------
//  BOOL NEAR XmitZFILE( NPZCB npZCB )
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
//
//------------------------------------------------------------------------

BOOL NEAR XmitZFILE( NPZCB npZCB )
{
   int   i ;
   char  szBuf[ 256 ] ;

   BLOCKCOUNT( npZCB ) = 0 ;
   FILEBYTES( npZCB ) = 0 ;

   if (-1 != HFILE( npZCB ))
   {
      szBuf[ ZF0 ] = ZCBIN | ZCRESUM ;
      szBuf[ ZF1 ] = 0 ;
      szBuf[ ZF2 ] = 0 ;
      szBuf[ ZF3 ] = 0 ;
      SendBinHdr( npZCB, ZFILE, szBuf ) ;

      _fmemset( szBuf, NULL, 256 ) ;
      lstrcpy( szBuf, StripPath( FILENAME( npZCB ) ) ) ;
      i = lstrlen( szBuf ) + 1 ;
      DateTimeToBuf( npZCB, &szBuf[ i ] ) ;
      i += lstrlen( &szBuf[ i ] ) ;
      CURFILE( npZCB )++ ;
      SendBinData( npZCB, ZCRCW, szBuf, i ) ;
      STATE( npZCB ) = SENT_FILE ;

      // ResetXferIndicators( FALSE ) ;
   }
   else
   {
      szBuf[ 0 ] = 0 ;
      szBuf[ 1 ] = 0 ;
      szBuf[ 2 ] = 0 ;
      szBuf[ 3 ] = 0 ;
      SendHexHdr( npZCB, ZFIN, szBuf ) ;
      STATE( npZCB ) = SENT_ZFIN ;
   }

   return ( TRUE ) ;

} // end of XmitZFILE()

//------------------------------------------------------------------------
//  BOOL NEAR XmitZDATA( NPZCB npZCB )
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

BOOL NEAR XmitZDATA( NPZCB npZCB )
{
   int   i, nBytesRead ;
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

   UpdateDialogBox( npZCB ) ;

   return ( TRUE ) ;

} // end of XmitZDATA()

//------------------------------------------------------------------------
//  VOID NEAR XmitZEOF( NPZCB npZCB )
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

VOID NEAR XmitZEOF( NPZCB npZCB )
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
//  VOID NEAR DateTimeToBuf( NPZCB npZCB, LPSTR lpBuf )
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
//
//------------------------------------------------------------------------

VOID NEAR DateTimeToBuf( NPZCB npZCB, LPSTR lpBuf )
{
   int    i, nMon, nDay, nYear, nHour, nMin, nSec ;
   DWORD  dwTotalDays, dwTotalSecs ;

   int    DaysInMonth[] = { 31, 28, 31, 30, 31, 30,
                            31, 31, 30, 31, 30, 31 } ;

   nDay = FILEDATE( npZCB ) & 0x001F ;
   nMon = (FILEDATE( npZCB ) & 0x01E0) >> 4 ;
   nYear = (FILEDATE( npZCB ) & 0xFE00) >> 8  + 10 ;
   nSec = (FILETIME( npZCB ) & 0x001F) * 2 ;
   nMin = (FILETIME( npZCB ) & 0x007E0) >> 4 ;
   nHour = (FILETIME( npZCB ) & 0xF800) >> 10 ;

   dwTotalDays = nYear * 365 + (nYear / 4) ;

   // add months

   for (i = 0; i < nMon; i++)
      dwTotalDays += DaysInMonth[ i ] ;
   dwTotalDays += nDay ;

   dwTotalSecs = dwTotalDays * 86400 ;
   dwTotalSecs += (nHour * 3600) + (nMin * 60) + nSec ;

   wsprintf( lpBuf, "%lu %lo", FILESIZE( npZCB ), dwTotalSecs ) ;

} // end of DateTimeToBuf()

//------------------------------------------------------------------------
//  VOID NEAR DWORDToBuf( LPSTR lpBuf, DWORD dwValue )
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

VOID NEAR DWORDToBuf( LPSTR lpBuf, DWORD dwValue )
{
   lpBuf[ 0 ] = (char) LOBYTE( LOWORD( dwValue ) ) ;
   lpBuf[ 1 ] = (char) HIBYTE( LOWORD( dwValue ) ) ;
   lpBuf[ 2 ] = (char) LOBYTE( HIWORD( dwValue ) ) ;
   lpBuf[ 3 ] = (char) HIBYTE( HIWORD( dwValue ) ) ;

} // end of DWORDToBuf()

//------------------------------------------------------------------------
//  VOID NEAR DWORDToDateTime( NPZCB npZCB, DWORD dwDateTime )
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
//
//------------------------------------------------------------------------

VOID NEAR DWORDToDateTime( NPZCB npZCB, DWORD dwDateTime )
{
   int    i, nMon, nDay, nYear, nHour, nMin, nSec ;
   DWORD  dwTotalSecs, dwTotalDays ;

   int    DaysInMonth[] = { 31, 28, 31, 30, 31, 30,
                            31, 31, 30, 31, 30, 31 } ;

   dwTotalSecs = dwDateTime ;

   DPF3( hWDB, "dwTotalSecs = %lu\r\n", dwTotalSecs ) ;

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

   DPF3( hWDB, "Date & Time: %d/%d/%d %d:%d:%d\r\n",
         nMon, nDay, nYear + 70, nHour, nMin, nSec ) ;

   FILEDATE( npZCB ) = (WORD) (((nYear - 10) << 9) + (nMon << 5) + nDay) ;
   FILETIME( npZCB ) = (WORD) ((nHour << 11) + (nMin << 5) + nSec / 2) ;

} // end of DWORDToDateTime()

//------------------------------------------------------------------------
//  BOOL NEAR GetFileStatus( HFILE hFile, LPFILESTATUS lpFileStatus )
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

BOOL NEAR GetFileStatus( HFILE hFile, LPFILESTATUS lpFileStatus )
{
   BOOL  fRetVal = TRUE ;
   WORD  wDate, wTime ;

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
      lpFileStatus -> dwLength = (DWORD) _filelength( hFile ) ;
   }

   return ( fRetVal ) ;

} // end of GetFileStatus()

//------------------------------------------------------------------------
//  BOOL NEAR SetFileStatus( HFILE hFile, LPFILESTATUS lpFileStatus )
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

BOOL NEAR SetFileStatus( HFILE hFile, LPFILESTATUS lpFileStatus )
{
   BOOL  fRetVal = TRUE ;
   WORD  wDate, wTime ;

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

} // end of SetFileStatus()

//------------------------------------------------------------------------
//  LPSTR NEAR StripPath( LPSTR lpFileName )
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

LPSTR NEAR StripPath( LPSTR lpFileName )
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
//  BOOL NEAR VerifyBlock( LPSTR lpBuf, int nCount )
//
//  Description:
//     Verifies the CRC16 value of the given block.
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

BOOL NEAR VerifyBlock( LPSTR lpBuf, int nCount )
{
	int     i ;
   WORD    wCRC16 ;

	wCRC16 = 0;
	while (nCount-- > 0)
	{
		wCRC16 = wCRC16 ^ (WORD) (*(lpBuf++)) << 8 ;
		for (i = 0; i < 8; ++i)
		{
			if (wCRC16 & 0x8000)
				wCRC16 = wCRC16 << 1 ^ 0x1021;
			else
				wCRC16 = wCRC16 << 1;
		}
	}

	if ((WORD)(*(lpBuf++) & 0x00FF) != (WORD)(((wCRC16 & 0xFF00) >> 8) & 0x00FF))
		return FALSE;

	if (((WORD)(*lpBuf) & 0x00FF) != (WORD)(wCRC16 & 0x00FF))
		return FALSE;

	return TRUE;

} // end of VerifyBlock()

//------------------------------------------------------------------------
//  VOID NEAR SendAbortString( HWND hWnd )
//
//  Description:
//     Cancels a transfer by sending out the Attn sequence.
//
//  Parameters:
//     LPSCB lpSCB
//        pointer to session control block.
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR SendAbortString( HWND hWnd )
{
	char      szBuf[ MAXLEN_ATTNSTR ] ;
   register  i ;

   for (i = 0; i < 12; i++)
   {
      szBuf[ i ] = ASCII_CAN ;
      szBuf[ i + 12 ] = ASCII_BS ;
   }
   szBuf[ 24 ] = NULL ;

   // send notification to parent

   SendMessage( hWnd, WM_CMD_TXBLK, (WPARAM) 24, (LPARAM) (LPSTR) szBuf ) ;

} // end of SendAbortString()

//---------------------------------------------------------------------------
//  End of File: zmodem.c
//---------------------------------------------------------------------------

