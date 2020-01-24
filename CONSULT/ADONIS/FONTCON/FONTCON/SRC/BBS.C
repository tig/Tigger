/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  FontShopper
 *
 *      Module:  bbs.c
 *
 *     Remarks:  Handles transactions with ShopSrv (which is really
 *               just an indirection to the bbs).
 *
 *   Revisions:  
 *    3/18/92  cek   Basic design...started implementation.
 *    3/23/92  cek   I'm on a plane from NY to Seattle and I'm feeling
 *                   pretty good.  If some of the comments in this code
 *                   sound smart-ass, that's why.
 *    4/5/92   cek   Pulled out of shopsrv and put into fontshop.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include <lzexpand.h>
#include <dos.h>
#include "global.h"
#include "dde.h"
#include "newuser.h"

extern HCONV   ghDDEConv ;

/*
 * Function failure handling stuff.
 */
#define RIP_CITY( ui )  {uiIDerr=ui;goto Failure;}
static UINT    uiIDerr ;

static HWND    hwndActive ;

static char gszTempPath[144] ;

static char              szTemp[144] ;
static char              szTemp2[144] ;
static BOOL              fWhatsNewUpdated = FALSE ;

BOOL              gfDBUpdated = FALSE ;

BOOL NEAR PASCAL AskUserIfHeWantsToUpdate( HWND hwndParent ) ;
BOOL NEAR PASCAL IsFileInList( LPSTR lpLocalBuf, LPSTR lpFile ) ;

/****************************************************************
 *  BOOL WINAPI BBSConnect( VOID )
 *
 *  Description: 
 *
 *    Connects to the BBS, tells the bbs that we're font shopper
 *    and returns.
 *
 *    This func creates a dde session with shopsrv, thus if
 *    it returns non-zero you must call BBSDisConnect() some time
 *    in the future.
 *
 *    Returns 0 if we are not connected.
 *    Returns 1 if we are connected and the database was NOT updated.
 *    Returns 2 if we are connected and the database WAS updated.
 *
 ****************************************************************/
UINT WINAPI BBSConnect( HWND hwndDisable )
{
   static BOOL fExeVersionsChecked = FALSE ;
   static BOOL fDatabaseChecked = FALSE ;

   /*
    * Start the DDE conversation with shopsrv.
    */
   if (fsddeConnect( ghwndMain, GRCS(IDS_EXE_CONXSRV), GRCS(IDS_CONXSRV), 
                     GRCS(IDS_CONXSRV) ))
   {
      hwndActive = hwndDisable ;

      /*
       * Tell CONXSRV what our main window is
       */
      fsddePoke( "ClientHWND", (LPVOID)&hwndActive, sizeof( hwndActive ), 0 );

      /*
       * Disable FontShopper's windows
       */
      DP3( hWDB, "Disabling Window %04X", hwndActive ) ;
      EnableWindow( hwndActive, FALSE ) ;

      /*
       * Connect to the BBS.  CONXSRV uses the entries in
       * "SHOPPER.INI" for comm settings.
       */
      if (fsddeExecute( "[Connect()]", (DWORD)180000L ))
      {
         WORD     wExeType ;

         /*
          * If we connected ok
          */
#ifdef BETA
         wExeType = EXE_FONTSHOP_BETA ;
#else
         wExeType = EXE_FONTSHOP_REGULAR ;
#endif

         if (gfDemoDatabase == TRUE)
            wExeType |= 0x8000 ;

         if (SendBBSMsg(  MSG_SEND_EXETYPE, &wExeType, NULL ))
         {
            DP1( hWDB, "MSG_SEND_EXETYPE failed!" ) ;
            BBSDisConnect();
            return 0 ;
         }

         /* cek 7/17/92
          *
          * I really didn't want to do this here, but it is
          * necessary for tech support.  If someone is having trouble
          * connecting, it is nice to have the bbs print his
          * name out whenever he logs on...
          */
         SendBBSMsg( MSG_SET_CUST_INFO, &gCustInfo.CustAddress, NULL ) ;


         /* If we have not already checked to see if any
          * of the executables are out of date during this
          * run of FS do it now.
          */
         if (fExeVersionsChecked == FALSE || fDatabaseChecked == FALSE)
         {
            if (fExeVersionsChecked == FALSE)
            {
               if (SendBBSMsg( MSG_QUERY_EXEVERSIONS, NULL, grgBBSExeVer ))
               {
                  DP1( hWDB, "MSG_QUERY_EXEVERSIONS failed!" ) ;
                  BBSDisConnect();
                  return 0 ;
               }

               if (FALSE == UpdateExecutables( hwndDisable ))
               {
                  BBSDisConnect();
                  return 0 ;
               }
            
               fExeVersionsChecked = TRUE ;
            }

            /* Now on to updating the database....
             * UpdateDatabase returns TRUE if the check of the
             * database was successful.  It returns false if
             * something went wrong.  It sets gfDBUpdated to TRUE
             * if anything new was downloaded successfully.
             */
            fDatabaseChecked = UpdateDatabaseFiles( hwndDisable ) ;

            /* If critical exe's were donwloaded (FONTSHOP.EXE,
             * SHOPLIB.DLL, FCDB.DLL, FSMEMMAN.DLL) then we need
             * to shut down...WinExec'ing CONXUP in the process.
             *
             * Short cut the ReloadDatabase() call because if
             * both the database and exe's have been updated there
             * might be an incompatiblitiy between what's currently
             * running and the new databse files.
             */
            if (gfUpShop == TRUE)
            {
               gfDBUpdated = FALSE ;
               BBSDisConnect() ;

               PostMessage( ghwndMain, WM_CLOSE, 0, 0L ) ;

               /*
                * At the end of WinMain we WinExec CONXUP.EXE if
                * gfUpShop is set.
                */
               return 0 ;
            }

            /*
             */
            if (gfDBUpdated == TRUE)
            {
               SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_UPDATE, FALSE );
               return 2 ;
            }
            else if (fDatabaseChecked == FALSE)
            {
               return 0 ;
            }

            SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_UPDATE, FALSE );
         }
      }
      else
      {
         DP1( hWDB, "[Connect()] returned FALSE!" ) ;

         BBSDisConnect() ;

//         ErrorResBox( ghwndMain, NULL, MB_ICONINFORMATION,
//               IDS_APPNAME, IDS_ERR_NOCONNECT ) ;

         return FALSE ;
      }
   }
   else
   {
      DP1( hWDB, "fsddeConnect() returned FALSE!" ) ;

      ErrorResBox( ghwndMain, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_CONXSRV ) ;
      return FALSE ;
   }

   return 1 ;


} /* BBSConnect()  */

/****************************************************************
 *  BOOL WINAPI IsBBSConnected( VOID )
 *
 *  Description: 
 *
 *    Returns TRUE if we are connected to the bbs.
 *
 ****************************************************************/
BOOL WINAPI IsBBSConnected( VOID )
{
   char     szBuf[80] ;

   szBuf[0] = '\0' ;

   DP3( hWDB, "IsBBSConnected()..." ) ;

   if (0 == fsddeRequest( "Status", szBuf, 80, 0 ))
      return FALSE ;

   DP3( hWDB, "Status = %s", (LPSTR)szBuf ) ;

   return (0 == lstrcmpi( "CONNECTED", szBuf )) ;

} /* IsBBSConnected()  */


/****************************************************************
 *  BOOL WINAPI BBSDisConnect( VOID )
 *
 *  Description: 
 *
 *    Logs off the bbs and shuts down the dde conversation.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI BBSDisConnect( VOID )
{
   BOOL  f ;

   fsddeExecute( "[DisConnect()]", (DWORD)60000L ) ;

   if (IsWindow( hwndActive ))
   {
      DP2( hWDB, "Enabling %04X", hwndActive ) ;
      EnableWindow( hwndActive, TRUE ) ;
   }
   else
   {
      DP1( hWDB, "Huh?" ) ;
   }

   f = fsddeDisconnect();


   if (gfDBUpdated == TRUE)
   {
      gfDBUpdated = FALSE ;
      ErrorResBox( ghwndMain, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_DATABASEUPDATED ) ;

      ReloadDatabase();
   }

   if (fWhatsNewUpdated)
   {
      fWhatsNewUpdated = FALSE ;
      SendMessage( ghwndMain, WM_COMMAND, IDM_INFO_WHATSNEW, 0L ) ;
   }

   return f ;

} /* BBSDisConnect()  */

/****************************************************************
 *  UINT FAR WINAPI SendBBSMsg( MSGTYPE bbsmsg, LPVOID lpSend, LPVOID lpReceive )
 *
 *    SendBBSMsg is a wrapper for ProcessSendBBSMsg() that
 *    makes invoking a specific message simple.  Only the message ID
 *    (bbsmsg), send buffer (lpSend), and receive buffer (lpReceive)
 *    are needed.
 *
 ****************************************************************/
UINT FAR WINAPI SendBBSMsg( MSGTYPE bbsmsg, LPVOID lpSend, LPVOID lpReceive )
{
   GENERICMSG     Msg ;

   if (ghDDEConv == NULL)
      return EXITCODE_GENERALFAILURE ;

   DP2( hWDB, "<<<< SendBBSMsg( '%s' == 0x%02X ) >>>>", (LPSTR)GRCS( IDS_MSG_NUL + (UINT)bbsmsg ), (UINT)bbsmsg ) ;

   Msg.lpReceive     = lpReceive ;     // Pointer to receive buffer
   Msg.cbReceive     = 0 ;             // Number of bytes in receive buffer

   Msg.lpSend        = lpSend ;        // Pointer to send buffer
   Msg.cbSend        = 0 ;             // number of bytes to be sent.

   Msg.wFlags        = 0x0000 ;

   Msg.SendMsg       = (BYTE)bbsmsg ;  // Current message

   switch( bbsmsg )
   {
      /*
       * These messages have no send or receive buffers.  They simply
       * tell the bbs to do something with data it already has.  If
       * the bbs got the message it will ACK.
       */
      case MSG_SAY_HI:
      case MSG_BEGIN_SESSION:
         #ifdef ORIG
         Msg.wFlags |= MSGCLASS_WAITACK ;
         #else
         Msg.cbReceive = sizeof( BYTE ) ;
         Msg.wFlags |= MSGCLASS_WAITRECEIVE ;
         #endif
      break ;

      case MSG_HANGING_UP:
         /*
          * The host will respond with '+++'.  Just
          * ignore it.
          */
         Msg.wFlags |= MSGCLASS_GOIDLE ;
      break ;

      /*
       * Wait for nothing messages.
       */
      case MSG_RECEIVE_FILE:
      case MSG_DOWNLOAD_FCDBLST:
      case MSG_DOWNLOAD_FCDBFILE:
         if (lpReceive)
            Msg.cbReceive = lstrlen( lpReceive ) + 1 ;

         Msg.wFlags |= MSGCLASS_CONXPRO_RECEIVE ;
      break ;

      case MSG_DOWNLOAD_EXE:
         if (lpReceive)
            Msg.cbReceive = lstrlen( lpReceive ) + 1 ;
         Msg.cbSend = sizeof( WORD ) ;
         Msg.wFlags |= MSGCLASS_CONXPRO_RECEIVE ;
      break ;

      /*
       * Wait for a BYTE messages.
       */
      case MSG_QUERY_RENEWAL_OK:
      case MSG_QUERY_NEWUSER_OK:
      case MSG_QUERY_LIB_ORDER_OK:
      case MSG_QUERY_CLIP_DLD_OK:
      case MSG_QUERY_SHIP_RENEWAL:
      case MSG_COMMIT_CUST_INFO:
      case MSG_COMMIT_SUBSCRIPTION:
      case MSG_COMMIT_NEWUSER:
      case MSG_COMMIT_RENEWAL:
      case MSG_COMMIT_PASSWD:
      case MSG_COMMIT_LIB_ORDER:   
      case MSG_COMMIT_CLIP_DLD:
         Msg.cbReceive = sizeof( BYTE ) ;

         Msg.wFlags |= MSGCLASS_WAITRECEIVE ;
      break ;

      /*
       * Wait for a WORD messages.  It's kinda silly that these
       * don't simply get a BYTE, but WS expects a WORD and
       * the BBS sends a WORD.  See the BYTE messages below...
       */
      case MSG_QUERY_USERID_OK:
      case MSG_QUERY_NEW_USERID_OK:
      case MSG_QUERY_INSTOCK:
      case MSG_QUERY_SUBTYPE:
         Msg.cbReceive = sizeof( WORD ) ;

         Msg.wFlags |= MSGCLASS_WAITRECEIVE ;
      break ;

      /*
       * Wait for a DWORD messages.
       */
      case MSG_QUERY_UNCOMPRESSED_FILESIZE:
      case MSG_QUERY_RENEWAL_PRICE:
      case MSG_QUERY_SUBSCRIPTION_PRICE:
      case MSG_QUERY_DATA_LRC:
      case MSG_QUERY_ORDERID:
         Msg.cbReceive = sizeof( DWORD ) ;

         Msg.wFlags |= MSGCLASS_WAITRECEIVE ;
      break ;

      case MSG_QUERY_CUR_FILESIZE:
         if (1) // fFontShopper)
            Msg.cbReceive = sizeof( DWORD ) ;
         else
            Msg.cbReceive = sizeof( WORD ) ;

         Msg.wFlags |= MSGCLASS_WAITRECEIVE ;
      break ;

      case MSG_QUERY_EXEVERSIONS:                  //new for FontShopper
         Msg.cbReceive = sizeof( WORD ) * NUM_EXECUTABLES ;

         Msg.wFlags |= MSGFLAG_RECEIVECRC ;
         Msg.wFlags |= MSGCLASS_WAITRECEIVE ;
      break ;

      /*
       * These messages send a single WORD sized buffer that is
       * not CRC checked.  The bbs will send an ACK if it got
       * the message.
       */
      case MSG_SEND_EXETYPE:
      case MSG_SEND_WINVER:
         Msg.cbSend = sizeof( WORD ) ;

         /*  Wait for ack after sending */
         Msg.wFlags |= MSGCLASS_SENDDATA_WAITACK ;
      break ;

      case MSG_LATEST_EXE:
      case MSG_LATEST_VERSION:
         Msg.cbReceive = sizeof( BYTE ) ;
         
         Msg.cbSend = sizeof( WORD ) ;

         /* MSG_LATEST_EXE expects to get ACK or NAK back in *lpReceive
          */
         Msg.wFlags |= MSGCLASS_SENDDATA_WAITRECEIVE ;
      break ;

      case MSG_SET_CUST_INFO:
         Msg.wFlags |= MSGFLAG_SENDCRC ;
         Msg.cbSend = sizeof( ADDRESS ) ;

         Msg.wFlags |= MSGCLASS_SENDDATA_WAITACK ;
      break ;

      case MSG_SET_USER_ID:
         Msg.wFlags |= MSGFLAG_SENDCRC ;
         Msg.cbSend = MAX_PASSWORD ;

         Msg.wFlags |= MSGCLASS_SENDDATA_WAITACK ;
      break ;

      case MSG_SET_MEDIA_PREF:
         Msg.cbSend = sizeof( WORD ) ; // wMedia

         Msg.wFlags |= MSGCLASS_SENDDATA_WAITACK ;
      break ;

      case MSG_NEW_GET_NEW_USER_ID:
         Msg.wFlags |= MSGFLAG_RECEIVECRC ;
         Msg.cbSend = sizeof( DWORD ) ; // dwLibID

         Msg.cbReceive = MAX_PASSWORD ;

         Msg.wFlags |= MSGCLASS_SENDDATA_WAITRECEIVE ;
         
      break ;

      case MSG_GET_NEW_USER_ID:
      case MSG_GET_NEW_LIMITED_DEMO_USER_ID:
         /* The user id's come back *NOT* padded.
          */
         Msg.wFlags |= MSGFLAG_RECEIVECRC ;
         Msg.cbReceive = MAX_PASSWORD ;

         Msg.wFlags |= MSGCLASS_WAITRECEIVE ;
      break ;

      case MSG_SET_PASSWD:
         /* Send the password across.  Set fSendPad to TRUE and
          * the FSM will automatically pad it.
          */
         Msg.wFlags |= MSGFLAG_SENDPAD ;
         Msg.wFlags |= MSGFLAG_SENDCRC ;
         Msg.cbSend = MAX_PASSWORD ;

         Msg.wFlags |= MSGCLASS_SENDDATA_WAITACK ;
      break ;

      case MSG_QUERY_PASSWD:
         /* The password comes back padded.  Set the fReceivePad bit
          * to have the FSM automatically make it a null terminated
          * string.
          */
         Msg.wFlags |= MSGFLAG_RECEIVECRC ;
         Msg.wFlags |= MSGFLAG_RECEIVEPAD ;
         Msg.cbReceive = MAX_PASSWORD ;

         Msg.wFlags |= MSGCLASS_WAITRECEIVE ;
      break ;

      case MSG_SET_CUR_FILENAME:
         Msg.wFlags |= MSGFLAG_SENDPAD ;
         Msg.wFlags |= MSGFLAG_SENDCRC ;
         Msg.cbSend = STD_FILENAME_SIZE ;    // 12
         
         Msg.wFlags |= MSGCLASS_SENDDATA_WAITACK ;
      break ;

      case MSG_SET_ORDER:
      case MSG_SET_NEW_USER_ORDER:
      case MSG_SET_CUR_LIB_ORDER:
      case MSG_SET_CUR_CLIP_DLD:
         Msg.wFlags |= MSGFLAG_SENDCRC ;
         Msg.cbSend = sizeof( ORDER ) ;    
         
         Msg.wFlags |= MSGCLASS_SENDDATA_WAITACK ;
      break ;

      //
      // The symantics of these messages changed for the CONXSRV
      // architecture.
      // 
      // They have been superseeded by combinations of newly defined 
      // messages 
      //
#if 0
      case MSG_QUERY_SHIPPING_COSTS:
         Msg.wFlags != 
      break ;
#endif

      case MSG_QUERY_DEST_FILENAME:
         Msg.wFlags |= MSGFLAG_RECEIVEPAD ;
         Msg.wFlags |= MSGFLAG_RECEIVECRC ;
         Msg.cbReceive = STD_FILENAME_SIZE ;

         Msg.wFlags |= MSGCLASS_WAITRECEIVE ;
      break ;

      default:
      #ifdef DEBUG
      {
         char szBuf[80] ;
         wsprintf(szBuf, "Invalid SendBBSMsg! bbsmsg = %04X", bbsmsg ) ;
         DASSERTMSG( hWDB, 0, szBuf ) ;
      }
      #endif
      break ;

   }

   #if 0
   DP1( hWDB, "Msg.SendMsg    = %02X", (UINT)Msg.SendMsg ) ;
   DP1( hWDB, "Msg.wFlags     = %04X", (UINT)Msg.wFlags ) ;
   DP1( hWDB, "Msg.lpSend     = %08lX", (LPVOID)Msg.lpSend ) ;
   DP1( hWDB, "Msg.cbSend     = %d", Msg.cbSend ) ;
   DP1( hWDB, "Msg.lpReceive  = %08lX", (LPVOID)Msg.lpReceive ) ;
   DP1( hWDB, "Msg.cbReceive  = %d", Msg.cbReceive ) ;
   #endif

   if ( fsddePoke( "MSG", (LPVOID)&Msg.SendMsg, sizeof( Msg.SendMsg ), 0 ) &&
        fsddePoke( "MSGFLAGS", (LPVOID)&Msg.wFlags, sizeof( Msg.wFlags ), 0 ) &&
        fsddePoke( "MSGSENDBUF", (LPVOID)Msg.lpSend, Msg.cbSend, 0 ) &&
        fsddePoke( "MSGSENDSIZE", (LPVOID)&Msg.cbSend, sizeof(Msg.cbSend), 0 ) &&
        fsddePoke( "MSGRECEIVEBUF", (LPVOID)Msg.lpReceive, Msg.cbReceive, 0 ) &&
        fsddePoke( "MSGRECEIVESIZE", (LPVOID)&Msg.cbReceive, sizeof(Msg.cbReceive), 0 ))
   {

      if (fsddeExecute( "[SendBBSMsg()]", 86400000L ))  // Wait 24 hours
      {
         if ((Msg.wFlags & 0xFF00) == MSGCLASS_CONXPRO_RECEIVE)
            return EXITCODE_OK ;

         if (Msg.cbReceive)
            fsddeRequest( "MSGReceiveBUF", (LPVOID)Msg.lpReceive, Msg.cbReceive, 0 );

         return EXITCODE_OK ;
      }
      else
      {
         if (IsWindow( hwndActive ))
         {
            DP2( hWDB, "Enabling %04X (SendBBSMsg)", hwndActive ) ;
            EnableWindow( hwndActive, TRUE ) ;
         }

         /*
          * This could be because of a disconnect, in which case,
          * the user already has been told about it
          */
         return EXITCODE_GENERALFAILURE ;
      }
   }
   else
   {
      if (IsWindow( hwndActive ))
      {
         DP2( hWDB, "Enabling %04X (SendBBSMsg)", hwndActive ) ;
         EnableWindow( hwndActive, TRUE ) ;
      }

      DP1( hWDB, "One of the pokes failed!" ) ;
      return EXITCODE_GENERALFAILURE ;
   }

} /* SendBBSMsg()  */

/****************************************************************
 *  BOOL WINAPI UpdateExecutables( HWND hwndParent  )
 *
 *  Description: 
 *
 *    Downloads system components if needed.  Assumes
 *    we are already connected.
 *
 *    This function sets the global variable gfUpShop to TRUE
 *    if it downloads a critical system component.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI UpdateExecutables( HWND hwndParent )
{
   WORD  wExeID ;
   char  szFile[144] ;
   PSTR  p ;
   char szTemp[144] ;
   OFSTRUCT          of ;
   UINT              fuEM ;

   GetTempFileName(0, "fs", 0, gszTempPath ) ;
   fuEM = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;
   OpenFile( gszTempPath, &of, OF_DELETE ) ;
   SetErrorMode( fuEM ) ;
   *(strrchr( gszTempPath, '\\' )+1) = '\0' ;


   /* If executables are out of date, download the ones
    * that are bad to the FONTSHOP dir.  The new files
    * have .123 extensions so they will not overwrite
    * the current exes.
    *
    * We can tell which exe's are out of date by comparing
    * the elements in grgOurExeVer[] (which was generated
    * in init.c) and grgBBSExeVer[] which was generated
    * in BBSConnect().
    *
    */

   /* Before downloading the components, nuke the contents
    * of the [UpShop] section in SHOPPER.INI.
    */
   WritePrivateProfileString( GRCS( IDS_CONXUP ),
                              NULL,
                              NULL,
                              GRCS( IDS_INIFILE ) ) ;


   /* As we download the appropriate components (after we
    * get them) tell CONXUP what to do by writing a command
    * to it's INI file.
    */

   for (wExeID = 0 ; wExeID < NUM_EXECUTABLES ; wExeID++)
   {
      DP2( hWDB, "Comparing Exe ID's : %03d (local) to %03d (remote)",
            grgOurExeVer[wExeID], grgBBSExeVer[wExeID] ) ;
         
      if (grgOurExeVer[wExeID] < grgBBSExeVer[wExeID])
      {
         if (!AskUserIfHeWantsToUpdate( hwndParent ))
            return FALSE ;

         if (SendBBSMsg(  MSG_DOWNLOAD_EXE, &wExeID, gszTempPath ))
         {
            DP1( hWDB, "Failed downloading Executable #%d", wExeID ) ;
            return FALSE ;
         }

         #ifdef BETA
         {
            WORD wSymID = wExeID | 0x8000 ;

            if (wExeID != EXE_MODEMINI &&
                wExeID != EXE_HELPFILE)
            {
               if (SendBBSMsg(  MSG_DOWNLOAD_EXE, &wSymID, gszTempPath ))
               {
                  DP1( hWDB, "Failed downloading Symbol File #%d", wSymID ) ;
                  return FALSE ;
               }
            }
         }
         #endif

         lstrcpy( szFile, GRCS( IDS_EXECUTABLES + wExeID ) ) ;
         p = strchr( szFile, '.' ) + 1 ;
         wsprintf( p, "%03d", grgBBSExeVer[wExeID] ) ;

         if (wExeID == EXE_CONXUP)
         {
            HFILE             hfSrcFile ;
            HFILE             hfDstFile ;

            /* These components do not require Upshop in order to
             * be updated.  In fact CONXUP would not beable to
             * update CONXUP.EXE...
             */

            /* Uncompress using Lzexpand() with the exe dir
             * being the target.
             */
            wsprintf( szTemp, "%s%s", (LPSTR)gszTempPath, (LPSTR)szFile ) ;
            DP2( hWDB, "Opening %s as source...", (LPSTR)szTemp ) ;
            if (hfSrcFile = LZOpenFile( szTemp, &of, OF_READ ))
            {
               wsprintf( szTemp, "%s%s", (LPSTR)gszExePath,
                                 GRCS( IDS_EXECUTABLES +  wExeID) ) ;

               DP2( hWDB, "Opening %s as destination...", (LPSTR)szTemp ) ;
               if (hfDstFile = LZOpenFile( szTemp, &of, OF_CREATE ))
               {
                  LZCopy(hfSrcFile, hfDstFile) ;
                  LZClose( hfDstFile ) ;
               }
   
               LZClose( hfSrcFile ) ;

            }

            /* Delete it from the temp */
            wsprintf( szTemp, "%s%s", (LPSTR)gszTempPath, (LPSTR)szFile ) ;
            fuEM = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;
            OpenFile( szTemp, &of, OF_DELETE ) ;
            SetErrorMode( fuEM ) ;

            #ifdef BETA
            {
               lstrcpy( szFile, GRCS( IDS_EXECUTABLES + wExeID ) ) ;
               p = strchr( szFile, '.' ) + 1 ;
               *p = '\0' ;
               strcat( p, "SYM" ) ;

               /* Uncompress using Lzexpand() with the exe dir
                * being the target.
                */
               wsprintf( szTemp, "%s%s", (LPSTR)gszTempPath, (LPSTR)szFile ) ;
               DP2( hWDB, "Opening %s as source...", (LPSTR)szTemp ) ;
               if (hfSrcFile = LZOpenFile( szTemp, &of, OF_READ ))
               {
                  wsprintf( szTemp, "%s%s", (LPSTR)gszExePath, (LPSTR)szFile );

                  DP2( hWDB, "Opening %s as destination...", (LPSTR)szTemp ) ;
                  if (hfDstFile = LZOpenFile( szTemp, &of, OF_CREATE ))
                  {
                     LZCopy(hfSrcFile, hfDstFile) ;
                     LZClose( hfDstFile ) ;
                  }
   
                  LZClose( hfSrcFile ) ;

               }

               /* Delete it from the temp */
               wsprintf( szTemp, "%s%s", (LPSTR)gszTempPath, (LPSTR)szFile ) ;
               fuEM = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;
               OpenFile( szTemp, &of, OF_DELETE ) ;
               SetErrorMode( fuEM ) ;
            }
            #endif

         }
         else
         {
            gfUpShop = TRUE ;

            wsprintf( szTemp, "%s%s", (LPSTR)gszTempPath, (LPSTR)szFile ) ;

            if (wExeID != EXE_HELPFILE)
            {
               wsprintf( szFile, "%s%s", (LPSTR)gszExePath,
                                       GRCS( IDS_EXECUTABLES + wExeID ) ) ;


               /*
                * Tell UpShop it needs to do something.  The format is:
                *
                *   Dest File = Source File
                */
               WritePrivateProfileString( GRCS( IDS_CONXUP ),
                                          szFile,
                                          szTemp,
                                          GRCS( IDS_INIFILE ) ) ;

               #ifdef BETA

               if (wExeID != EXE_MODEMINI)
               {
                  p = strchr( szTemp, '.' ) + 1 ;
                  *p = '\0' ;
                  strcat( p, "SYM" ) ;

                  wsprintf( szFile, "%s%s", (LPSTR)gszExePath,
                                          GRCS( IDS_EXECUTABLES + wExeID ) ) ;

                  p = strchr( szFile, '.' ) + 1 ;
                  *p = '\0' ;
                  strcat( p, "SYM" ) ;
                  WritePrivateProfileString( GRCS( IDS_CONXUP ),
                                             szFile,
                                             szTemp,
                                             GRCS( IDS_INIFILE ) ) ;
               }

               #endif

            }
            else
            {
               HFILE hFile ;
               OFSTRUCT of ;

               wsprintf( szFile, "%sFCHELP.HLP", (LPSTR)gszExePath ) ;
               WritePrivateProfileString( GRCS( IDS_CONXUP ),
                                          szFile,
                                          szTemp,
                                          GRCS( IDS_INIFILE ) ) ;


               wsprintf(szFile, "%sFCHELP.BIN", (LPSTR)gszExePath ) ;
               if (hFile = OpenFile( szFile, &of, OF_CREATE | OF_WRITE ))
               {
                  wsprintf( szFile, "; File Version %03d\n",
                     grgBBSExeVer[wExeID] ) ;
                  _lwrite( hFile, szFile, lstrlen( szFile ) ) ;
                  _lclose( hFile ) ;
               }
            }
         }

      }
   }

   /* Tell CONXUP who to run when he's done.
    * Use the /r switch to tell fontcon it's being restarted.
    */
   if (gfUpShop)
   {
      wsprintf( szFile, "%s%s /r", (LPSTR)gszExePath,
                     GRCS( IDS_EXE_FONTCON ) ) ;
      WritePrivateProfileString( GRCS( IDS_CONXUP ),
                              "Application",
                              szFile,
                              GRCS( IDS_INIFILE ) ) ;
   }

   return TRUE ;

} /* UpdateExecutables()  */


/****************************************************************
 *  BOOL WINAPI UpdateDatabaseFiles( HWND hwndParent  )
 *
 *  Description: 
 *
 *    Downloads database files as needed.  Returns TRUE if
 *    files were downloaded.
 *
 ****************************************************************/
BOOL WINAPI UpdateDatabaseFiles( HWND hwndParent )
{
   DWORD             dwServerCRC ;
   DWORD             dwLocalCRC ;
   LPSTR             lpBuf ;
   char              szFile[144] ;
   int               hFile ;
   int               nLen = 0 ;
   OFSTRUCT          of ;
   OFSTRUCT          ofSrc ;
   OFSTRUCT          ofDst ;
   HFILE             hfSrcFile ;
   HFILE             hfDstFile ;
   UINT              fuEM ;
   struct _find_t    ftFind ;
   UINT              nFilesUpdated = 0 ;
   LPSTR             lpLimitedDBList = NULL ;
   BOOL              fAsked=FALSE ;

   GetTempFileName(0, "fs", 0, gszTempPath ) ;
   fuEM = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;
   OpenFile( gszTempPath, &of, OF_DELETE ) ;
   SetErrorMode( fuEM ) ;
   *(strrchr( gszTempPath, '\\' )+1) = '\0' ;
   
   if (SendBBSMsg( MSG_QUERY_DATA_LRC, NULL, &dwServerCRC ))
   {
      DP1( hWDB, "Query Database CRC32 failed!" ) ;
      return FALSE ;
   }

   if (lpBuf = BuildDirList( gszDBPath, &nLen ))
   {
      dwLocalCRC = CalcCRC32( lpBuf, nLen ) ;

      DP2( hWDB, "Local database CRC is 0x%08lX, remote is 0x%08lX",
         dwLocalCRC, dwServerCRC ) ;

      if (dwServerCRC == dwLocalCRC)
      {
         /* No need to update any files!
          */
         DestroyDirList( lpBuf ) ;

         DP2( hWDB, "Database is up to date" ) ;
         gfDBChecked = TRUE ;

         return TRUE ;
      }
   }
   else
   {
      DP1( hWDB, "BuildDirList failed!" ) ;
      dwServerCRC = 0 ;
      nLen = 0 ;
   }

   DP2( hWDB, "Database needs updating..." ) ;

   /* Read in the list of files that are to be included.  These
    * can be found in the [FCDB Limit] section of CONXSRV.INI file.
    */
   if (lpLimitedDBList = GlobalAllocPtr( GHND, (1024 * 16) ))
   {
      GetPrivateProfileString( GRCS( IDS_INI_FCDB_LIMIT ), NULL,
                                  "", lpLimitedDBList,
                                  (1024 *16), GRCS( IDS_INIFILE ) ) ;
      if (*lpLimitedDBList == '\0')
      {
         GlobalFreePtr( lpLimitedDBList ) ;
         lpLimitedDBList = NULL ;
      }
   }

   wsprintf( szFile, "%sdatabase.lst", (LPSTR)gszTempPath ) ;
   fuEM = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;
   OpenFile( szFile, &of, OF_DELETE ) ;
   SetErrorMode( fuEM ) ;

   /*
    * Download the list of files from the bbs.  This is a file
    * named database.lst, and it' has one filename per line.
    */

   DP2( hWDB, "Downloading %s", (LPSTR)szFile ) ;

   if (SendBBSMsg( MSG_DOWNLOAD_FCDBLST, NULL, gszTempPath ))
   {
      DP1( hWDB, "MSG_DOWNLOAD_FCDBLIST failed!" ) ;
      DestroyDirList( lpBuf ) ;
      return FALSE ;
   }

   /* Open and read the file "database.lst", which is a
    * sorted list of all filenames.
    */
   if (0 < (hFile = OpenFile( szFile, &of, OF_READ )))
   {
      LPSTR lpBufHost ;
      LPSTR lpCurHost ;
      int   cbFile ;

      cbFile = LOWORD( _filelength( hFile ) ) ;

      if (!(lpBufHost = GlobalAllocPtr( GHND, cbFile + 1 )))
      {
         DP1( hWDB, "GlobalAllocPtr failed in UpdateDatabaseFiles!!!" ) ;
         DestroyDirList( lpBuf ) ;
         return FALSE ;
      }

      _lread( hFile, lpBufHost, cbFile ) ;
      _lclose( hFile ) ;

      /* Step through each line in DATABASE.LST, determining
       * which files need to be updated.
       */
      lpCurHost = lpBufHost ;

      while( *lpCurHost && wsscanf( lpCurHost, "%s", (LPSTR)szFile ) )
      {
         if (lpLimitedDBList)
         {
            char szFileName[16] ;

            lstrcpy( szFileName, szFile ) ;

            // strip off the extension
            *(strchr( szFileName, '.' )) = '\0' ;

            if (!IsFileInList( lpLimitedDBList, szFileName ))
            {
               DP2( hWDB, "  Skipping %s because it's not in the list.",
                  (LPSTR)szFileName ) ;

               lpCurHost += lstrlen( szFile ) + 1 ;
               if (*lpCurHost == '\n' || *lpCurHost == '\n')
                  lpCurHost++ ;

               continue ;
            }
         }

         /* Download this one?
          */
         if ((lpBuf == NULL) || !IsFileInList( lpBuf, szFile ))
         {
            if (fAsked == FALSE && !AskUserIfHeWantsToUpdate( hwndParent ))
            {
               if (lpLimitedDBList)
               {
                  GlobalFreePtr( lpLimitedDBList ) ;
                  lpLimitedDBList = NULL ;
               }

               if (lpBuf)
                  DestroyDirList( lpBuf ) ;

               return FALSE ;
            }

            fAsked = TRUE ;

            DP2( hWDB, "   Downloading %s", (LPSTR)szFile ) ;

            SendBBSMsg( MSG_SET_CUR_FILENAME, szFile, NULL ) ;

            /* All messages of the RECEIVE_CONXPRO class take a destination
             * file/directory as the lpszRec parameter (optionally).
             */

            /* Download the new db files into the TEMP directory.
             */
            if (SendBBSMsg( MSG_DOWNLOAD_FCDBFILE, NULL, gszTempPath ))
            {
               DP1( hWDB, "Transfer failed!" ) ;
               GlobalFreePtr( lpBufHost ) ;
               DestroyDirList( lpBuf ) ;

               if (lpLimitedDBList)
               {
                  GlobalFreePtr( lpLimitedDBList ) ;
                  lpLimitedDBList = NULL ;
               }

               return FALSE ;             //???
            }

            nFilesUpdated ++ ;

         }

         lpCurHost += lstrlen( szFile ) + 1 ;
         if (*lpCurHost == '\n' || *lpCurHost == '\n')
            lpCurHost++ ;
      }

      if (nFilesUpdated == 0)
         goto TheEnd ;

      /* Go through the host's list again, copying any files we down
       * loaded to the database dir.
       */
      lpCurHost = lpBufHost ;
      while( *lpCurHost && wsscanf( lpCurHost, "%s", (LPSTR)szFile ) )
      {
         if (lpLimitedDBList)
         {
            char szFileName[16] ;

            lstrcpy( szFileName, szFile ) ;

            // strip off the extension
            *(strchr( szFileName, '.' )) = '\0' ;

            if (!IsFileInList( lpLimitedDBList, szFileName ))
            {
               lpCurHost += lstrlen( szFile ) + 1 ;
               if (*lpCurHost == '\n' || *lpCurHost == '\n')
                  lpCurHost++ ;

               continue ;
            }
         }

         /* Download this one?
          */
         if ((lpBuf == NULL) || !IsFileInList( lpBuf, szFile ))
         {
            DP2( hWDB, "   Copying %s to database dir", (LPSTR)szFile ) ;

            /* We should look for any files with the filename and
             * delete them before downloading this one.
             */
            fuEM = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;

            lstrcpy( szTemp, szFile ) ;
            *(strchr(szTemp, '.')+1) = '*' ;
            *(strchr(szTemp, '.')+2) = '\0' ;

            if (lstrcmp( szTemp, "WHATSNEW.*" ) == 0)
               fWhatsNewUpdated = TRUE ;

            wsprintf( szTemp2, "%s%s", (LPSTR)gszDBPath, (LPSTR)szTemp ) ;
            if (0 == _dos_findfirst( szTemp2, _A_NORMAL, &ftFind ))
            {
               do
               {
                  wsprintf( szTemp, "%s%s", (LPSTR)gszDBPath, (LPSTR)ftFind.name ) ;
                  DP2( hWDB, "Deleting %s", (LPSTR)szTemp ) ;

                  OpenFile( szTemp, &of, OF_DELETE ) ;

               } while( 0 == _dos_findnext( &ftFind )) ;
            }

            SetErrorMode( fuEM ) ;

            /* Uncompress using Lzexpand() with the DATABASE dir
             * being the target.
             */
            wsprintf( szTemp, "%s%s", (LPSTR)gszTempPath, (LPSTR)szFile ) ;
            DP2( hWDB, "Opening %s as source...", (LPSTR)szTemp ) ;
            hfSrcFile = LZOpenFile( szTemp, &ofSrc, OF_READ ) ;

            DASSERT( hWDB, hfSrcFile ) ;

            wsprintf( szTemp, "%s%s", (LPSTR)gszDBPath, (LPSTR)szFile ) ;
            DP2( hWDB, "Opening %s as destination...", (LPSTR)szTemp ) ;
            hfDstFile = LZOpenFile( szTemp, &ofDst, OF_CREATE ) ;

            DASSERT( hWDB, hfDstFile ) ;

            LZCopy(hfSrcFile, hfDstFile) ;

            LZClose( hfDstFile ) ;

            LZClose( hfSrcFile ) ;
            
            /* Delete it from the temp */
            wsprintf( szTemp, "%s%s", (LPSTR)gszTempPath, (LPSTR)szFile ) ;
            fuEM = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;
            OpenFile( szTemp, &of, OF_DELETE ) ;
            SetErrorMode( fuEM ) ;
         }

         lpCurHost += lstrlen( szFile ) + 1 ;
         if (*lpCurHost == '\n' || *lpCurHost == '\n')
            lpCurHost++ ;

      }

      GlobalFreePtr( lpBufHost ) ;
   }
   else
   {

      DP1( hWDB,  "Could not open '%s'!", (LPSTR)szFile ) ;

      if (lpLimitedDBList)
      {
         GlobalFreePtr( lpLimitedDBList ) ;
         lpLimitedDBList = NULL ;
      }

      DestroyDirList( lpBuf ) ;
      return FALSE ;
   }

TheEnd:
   if (lpLimitedDBList)
   {
      GlobalFreePtr( lpLimitedDBList ) ;
      lpLimitedDBList = NULL ;
   }

   DestroyDirList( lpBuf ) ;

   if (nFilesUpdated)
      gfDBUpdated = TRUE ;

   gfDBChecked = TRUE ;

   return TRUE ;

} /* UpdateDatabaseFiles()  */


/****************************************************************
 *  BOOL NEAR PASCAL IsFileInList( LPSTR lpLocalBuf, LPSTR lpFile )
 *
 *  Description: 
 *
 *    Returns TRUE if the string pointed to by lpFile exists
 *    in the doubly null terminated buffer lpLocalBuf.
 *
 ****************************************************************/
BOOL NEAR PASCAL IsFileInList( LPSTR lpLocalBuf, LPSTR lpFile )
{
   while (*lpLocalBuf && lstrcmp( lpLocalBuf, lpFile ))
   {
      lpLocalBuf += lstrlen( lpLocalBuf ) + 1 ;
   }

   if (*lpLocalBuf)
   {
      return TRUE ;
   }

   return FALSE ;
} /* IsFileInList()  */

/****************************************************************
 *  BOOL NEAR PASCAL AskUserIfHeWantsToUpdate( HWND hwndParent )
 *
 *  Description: 
 *
 *    Only pop's up the UI once during a session.
 *
 ****************************************************************/
BOOL NEAR PASCAL AskUserIfHeWantsToUpdate( HWND hwndParent )
{
   static BOOL fUIPresented = FALSE ;

   if (fUIPresented == FALSE)
   {
      /* Hey mr. user, this may take a while...
       */
      switch(ErrorResBox( hwndParent, NULL, MB_ICONQUESTION | MB_YESNO,
                        IDS_APPNAME, IDS_ERR_UPDATESTUFF ))
      {
         case IDYES:
            fUIPresented = TRUE ;
            return TRUE ;
         break ;

         case IDNO:
            /* Ensure we get logged off! */
            BBSDisConnect() ;
            return FALSE ;
         break ;
      }
   }

} /* AskUserIfHeWantsToUpdate()  */


/****************************************************************
 *  UINT WINAPI CheckAccountStatus( HWND hwndParent )
 *
 *  Description: 
 *
 * Verify that this guy's account is active.  If it's not
 * then log off and provide him with a dialog that
 * allows him to renew.
 *
 * Assumes we're connected when it is called.
 *
 ****************************************************************/
UINT WINAPI CheckAccountStatus( HWND hwndParent )
{
   WORD wStatus ;
   WORD wSubType ;

   if (SendBBSMsg(  MSG_QUERY_USERID_OK, NULL, &wStatus ))
   {
      DP1( hWDB, "QUERY_USERID_OK failed!!!" ) ;
      return (UINT)-1 ;
   }

   switch( wStatus )
   {
      case WS_AUTH:                 // been paid for, but he's never
                                    // logged on before.
         DP2( hWDB, "Authorized account.  He's paid money, but never been on before" ) ;

      break ;

      case WS_ACTIVE:               // he's cool.  He can buy fonts.

         if (SendBBSMsg(  MSG_QUERY_SUBTYPE, NULL, &wSubType ))
         {
            DP1( hWDB, "QUERY_SUBTYPE failed!!!" ) ;
            return (UINT)-1 ;
         }

         switch( wSubType )
         {
            case 1:     // FREE_SUBSCRIBE:
            case 2:     // NORMAL:
            case 3:     // DEMO:    /* 60-day */
            case 4:     // SAMPLER: /* 90-day */
               DP2( hWDB, "Active account.  Let him buy fonts" ) ;
               gfExpiredUser = FALSE ;
               gfDemoUser = FALSE ;
               WritePrivateProfileWORD( GRCS( IDS_CONFIG ),
                                       GRCS( IDS_INI_STATUS ),
                                       42,
                                       GRCS( IDS_INIFILE ) ) ;
            break ;

            case 5:     // LIMITED_DEMO
               DP2( hWDB, "Active demo account.  He can have free fonts only!" ) ;
               gfDemoUser = TRUE ;
               WritePrivateProfileWORD( GRCS( IDS_CONFIG ),
                                       GRCS( IDS_INI_STATUS ),
                                       0,
                                       GRCS( IDS_INIFILE ) ) ;
            break ;
         }


      break ;

      case WS_EXPIRED:              // done gone.  Must renew.
         DP2( hWDB, "User's account has expired, trying to renew..." ) ;
         gfExpiredUser = TRUE ;
         WritePrivateProfileWORD( GRCS( IDS_CONFIG ),
                                 GRCS( IDS_INI_STATUS ),
                                 43,
                                 GRCS( IDS_INIFILE ) ) ;

         if (!RenewUser( hwndParent, &gCustInfo ))
         {
            BBSDisConnect() ;
            return (UINT)-1 ;
         }
         DP2( hWDB, "Expired user renewed successfully" ) ;
      break ;

      case WS_NOEXIST:              // doesn't exist
      case WS_AVAIL:                // on shelf, waiting to be used

         ErrorResBox( ghwndMain, NULL, MB_ICONEXCLAMATION,
                      IDS_APPNAME, IDS_ERR_QUERY_USERID ) ;
         BBSDisConnect() ;
      return (UINT)-1 ;

      default:
         DP1( hWDB, "QUERY_USERID_OK returned an invalid status! (%04X)", wStatus ) ;
         BBSDisConnect() ;


         ErrorResBox( ghwndMain, NULL, MB_ICONEXCLAMATION,
                      IDS_APPNAME, IDS_ERR_GENERAL_COMM ) ;

      return (UINT)-1 ;
   }


   return wStatus ;

} /* CheckAccountStatus()  */

#define CRC32_POLYNOMIAL 0xEDB88320L

//------------------------------------------------------------------------
//  void BuildCRC32Table( void )
//
//  Description:
//     Builds the CRC-32 table.
//
//  Parameters:
//     NONE
//
//  History:   Date       Author      Comment
//              5/10/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------


void BuildCRC32Table( DWORD CRC32Table[] )
{
   int           i, j ;
   DWORD         dwCRC32 ;

   for (i = 0; i  < 256; i++)
   {
      dwCRC32 = i ;
      for (j = 8; j > 0; j--)
      {
         if (dwCRC32 & 1L)
            dwCRC32 = (dwCRC32 >> 1) ^ CRC32_POLYNOMIAL ;
         else
            dwCRC32 >>= 1 ;
      }
      CRC32Table[ i ] = dwCRC32 ;
   }

} // end of BuildCRC32Table()


/****************************************************************
 *  DWORD WINAPI CalcCRC32( LPBYTE lpBuf, int n)
 *
 *  Description: 
 *
 *    Returns a Crc32 for the buffer passed in.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD WINAPI CalcCRC32( LPBYTE lpBuf, int cb )
{
    static DWORD rgCRC32[256] ;
    static BOOL fTableBuilt = FALSE ;
    int         i ;
    DWORD       CRC32 = 0xFFFFFFFFL ;

   if (fTableBuilt == FALSE)
   {
      BuildCRC32Table( rgCRC32 ) ;
      fTableBuilt = TRUE ;
   }

   for (i = 0 ; i < cb ; i++)
   {
      dwCRC32 = rgCRC32[((int)dwCRC32 ^ lpBuf[i]) & 255] ^ ((dwCRC32 >> 8) & 0x00FFFFFFL) ;
   }
   dwCRC32 = ~dwCRC32 ;

   return dwCRC32 ;

} /* CalcCRC32()  */



/************************************************************************
 * End of File: bbs.c
 ***********************************************************************/


