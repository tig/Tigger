/************************************************************************
 *
 *     Copyright (c) 1992-93 C.E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  ASYNCSRV
 *
 *      Module:  connect.c
 *
 *     Remarks:  This module contains the connect and disconnect
 *               dialog boxes and their respective finite
 *               state machines.
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "ASYNCSRV.h"
#include "connect.h"
#include "comm.h"
#include "fsm.h"
#include "bbs.h"
#include "online.h"
#include "settings.h"

//#define TEST

#ifdef TEST
#pragma message( "!!!! TEST !!!!!" ) 
#endif

#define TIMER_ID              42

extern BOOL gfIgnoreCarrierDetect ;

static UINT uiConnectDriverSpeed ;
static UINT uiPostAquirePause ;
static UINT uiModemDialPause ;
static UINT uiConnectWaitDialTimeout ;
 
/*
 * Function failure handling stuff.
 */
#define RIP_CITY( ui )  {uiIDerr=ui;goto Failure;}
static UINT    uiIDerr ;

static char gszModemResponse[MAX_MODEMRESP+1] ;
static PSTR gpszModemResponse ;
static UINT uiCurLED ;

#define MAX_HANGUPATTEMPTS    4
static UINT uiHangUpAttempts ;

#define MAX_REDIALATTEMPTS    3
static UINT uiMaxRedialAttempts ;
static UINT uiRedialAttempts ;

#define STATE_PROCESS         0x0000
#define STATE_INITFSM         0x0001
#define STATE_AQUIRE          0x0002
#define STATE_INITMODEM       0x0003
#define STATE_WAITINIT        0x0004
#define STATE_DIALMODEM       0x0005
#define STATE_WAITDIAL        0x0006
#define STATE_LOGIN           0x0007
#define STATE_DISCONNECT      0x0008
#define STATE_HANGUP          0x0009
#define STATE_WAITHANGUP      0x000a
#define STATE_ABORT           0x000b
#define STATE_TRYAGAIN        0x000c
#define STATE_EXITFSM         0x000d
#define STATE_PAUSE           0x000e


/*
 * Local functions
 */
BOOL  CALLBACK fnConnectDlg( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;
BOOL  CALLBACK fnDisConnectDlg( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;

UINT NEAR PASCAL CheckModemResponse( LPCONNECTFSM lpFSM, LPSTR lpszResponse ) ;
VOID  NEAR PASCAL SetConnectStatusMsg( HWND hDlg, LPSTR lpsz ) ;
LPSTR NEAR PASCAL SendModemString( LPFSM lpFSM, LPSTR lpsz ) ;
BOOL  NEAR PASCAL ConnectFSM( HWND hDlg, UINT uiFunction ) ;
BOOL  NEAR PASCAL DisConnectFSM( HWND hDlg, UINT uiFunction ) ;
VOID NEAR PASCAL ConnectError( UINT uiErr ) ;

/****************************************************************
 *  BOOL WINAPI DoConnect( VOID )
 *
 *  Description: 
 *
 *    Pops up the DoConnect modal dialog box.
 *
 *    This dialog box is centered over the client app which should
 *    have it's main window disabled before this dialog pops up.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI DoConnect( VOID )
{
   int ui ;
   DLGPROC  lpfn ;

   /*
    * If we're connected fail
    */
//   if (IsOnline())
//      return FALSE ;

   (FARPROC)lpfn = MakeProcInstance( (FARPROC)fnConnectDlg, hinstApp ) ;

   if (hwndClientApp == NULL)
   {
      hwndClientApp = hwndApp ;
   }

   ui = DialogBox( hinstApp, MAKEINTRESOURCE( DLG_CONNECT ),
                           hwndClientApp, lpfn ) ;

   FreeProcInstance( (FARPROC)lpfn ) ;

   /*
    * If connected start the Online finite state machine...
    *
    * A return of IDOK from the dialog box means that the
    * user did not abort.  It does NOT mean that we are
    * succesfully connected.  IDCANCEL means the
    * user hit the abort button.
    */
   if (ui == IDCANCEL)
   {
      #if 0
      if (IsWindow( hwndStat ))
         SetWindowText( hwndStat, "" ) ;
      #endif

      return FALSE ;
   }


   if (ui < IDS_ERR_DUMMY && IsOnline())
   {
      DP3( hWDB, "Online!  Starting OnlineDialog!" ) ;
      return StartOnlineDialog() ;
   }
   else
   {
      DP1( hWDB, "Could not connect" ) ;

      if (ui < IDS_ERR_DUMMY)
         ui = IDS_ERR_CONNECTFAILURE ;

      ConnectError( ui ) ;

      return FALSE ;
   }

}
/****************************************************************
 *  BOOL CALLBACK fnConnectDlg( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Connect Dialog box procedure
 *
 *  Comments:
 *
 ****************************************************************/
BOOL CALLBACK fnConnectDlg( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   UINT           i ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         /*
          * Setup all the controls
          */
         for (i = IDD_LED1 ; i <= IDD_LED10 ; i++)
            SendDlgItemMessage( hDlg, i, LED_SETCOLOR, 0, RGBLTGREEN ) ;

         uiCurLED = IDD_LED1 ;

         if (IsWindow( hwndClientApp ))
            DlgCenter( hDlg, hwndClientApp, 0 ) ;

         ConnectFSM( hDlg, FF_INITFSM ) ;
      }
      break ;

      case WM_TIMER:
         ConnectFSM( hDlg, FF_PROCESS ) ;

      break;

      case WM_COMMAND:
         switch ((UINT)wParam)
         {
            case IDCANCEL:
               ConnectFSM( hDlg, FF_USERABORT ) ;
            break ;

            default :
               return FALSE ;
         }
         break ;

      case WM_COMMNOTIFY:
      {
         ProcessCOMMNotification( wParam, lParam ) ;

         DASSERT( hWDB, gpszModemResponse ) ;
         while (GetInBYTE( gpszModemResponse ) != 0)
         {
            if (gpszModemResponse >= gszModemResponse + MAX_MODEMRESP)
            {
               gpszModemResponse = gszModemResponse ;
               gszModemResponse[0] = '\0' ;
            }
            else
            {
               gpszModemResponse++ ;
               *gpszModemResponse = '\0' ;
            }
         }
      }
      break ;

   }
   return FALSE ;
} /* fnConnectDlg()  */

/****************************************************************
 *  BOOL NEAR PASCAL ConnectFSM( HWND hDlg, UINT uiFunction )
 *
 *  Description: 
 *
 *    This function is the finite state machine manager for the
 *    connect command.
 *
 *    uiFunction is a forced function:
 *
 *    FF_INITFSM      Initializes the FSM to STATE_INITFSM
 *
 *    FF_USERABORT    If the user indicates that he wants to
 *                      abort this connect attempt.
 *
 *    FF_PROCESS      Process the current state.  This is not
 *                      really a function.  FF_PROCESS is defined
 *                      as 0.
 *
 *
 *    The following states are defined:
 *
 *    State Label       Descripition
 *
 *    STATE_PAUSE       Pause for uiPauseTime milliseconds.
 *
 *    STATE_INITFSM     INIT the finite state machine.
 *
 *    STATE_AQUIRE      Aquire comm port.
 *
 *    STATE_INITMODEM   Initialize modem.
 *
 *    STATE_WAITINIT    Waiting for modem init response
 *
 *    STATE_DIALMODEM   Dial
 *
 *    STATE_WAITDIAL    Waiting for modem dial response message.
 *
 *    STATE_LOGIN       Logging in
 *
 *    STATE_DISCONNECT  Disconnecting (De-aquire) comm port.
 *
 *    STATE_HANGUP      Hanging up modem.
 *
 *    STATE_WAITHANGUP  Waiting for the hangup response message.
 *
 *    STATE_ABORT       Error message box is up.
 *
 *    STATE_TRYAGAIN    "Line was busy, do you want to try again?"
 *
 *    STATE_EXITFSM     Exit dialog box.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL ConnectFSM( HWND hDlg, UINT uiFunction )
{
   LPCONNECTFSM   lpFSM = NULL ;
   static fUIup = FALSE ;

   if (uiFunction != FF_INITFSM)
   {
      lpFSM = (LPCONNECTFSM)GET_FSM( hDlg ) ;

      /*
       * If we ever have this case, we are *really* in trouble.  No
       * need to tell the user about it.
       */
      if (!lpFSM)
         return FALSE ;

   }


   /* Handle each forced function.  The 'default' case (also FF_PROCESS)
    * in this
    * switch statement is where the actual states are handled.
    */
   switch (uiFunction)
   {
      case FF_INITFSM:
         DP3( hWDB, "ConnectFSM( FF_INITFSM )" ) ;
         SetConnectStatusMsg( hDlg, rglpsz[IDS_INITIALIZING] ) ;

         /* Attempt to read conxtion.ini for timing options
          */
         uiConnectDriverSpeed = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                               rglpsz[IDS_INI_CONNECTDRIVERSPEED],
                                               CONNECT_DRIVE_MSEC,
                                               rglpsz[IDS_INI_FILENAME] ) ;

         uiPostAquirePause = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                               rglpsz[IDS_INI_POSTAQUIREPAUSE],
                                               POSTAQUIREPAUSE_MSEC,
                                               rglpsz[IDS_INI_FILENAME] ) ;

         uiModemDialPause = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                               rglpsz[IDS_INI_MODEMDIALPAUSE],
                                               MODEMDIALPAUSE_MSEC,
                                               rglpsz[IDS_INI_FILENAME] ) ;

         uiConnectWaitDialTimeout = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                               rglpsz[IDS_INI_CONNECTWAITDIALTIMEOUT],
                                               CONNECT_WAITDIAL_SEC,
                                               rglpsz[IDS_INI_FILENAME] ) ;

         uiMaxRedialAttempts = MAX_REDIALATTEMPTS ;

            /*GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                               rglpsz[IDS_INI_MAXREDIALATTEMPTS],
                                               MAX_REDIALATTEMPTS,
                                               rglpsz[IDS_INI_FILENAME] ) ;*/

         uiRedialAttempts = 1 ;

         if (!SetTimer( hDlg, TIMER_ID, uiConnectDriverSpeed, NULL ))
         {
            ErrorResBox( hDlg, NULL, MB_OK | MB_ICONSTOP,
                   IDS_ERR_COMMUNICATIONS, IDS_ERR_TIMER ) ;

            return FALSE ;
         }   

         if (lpFSM = GlobalAllocPtr( GHND, sizeof( CONNECTFSM )))
         {
            /*
             * Put our far pointer into the window props of the dlg.
             */
            CREATE_FSM( hDlg, lpFSM ) ;

            /* Initialize the FSM structure
             *
             */
            lpFSM->uiNextState = STATE_PROCESS ;
            ResetPause( &lpFSM->Pause ) ;
            ResetTimeout( &lpFSM->Timeout ) ;

            /* Start with [Comm Settings] in the clients INI file
             */
            if (!ReadCommSettings( &lpFSM->CommSettings, rglpsz[IDS_INI_FILENAME] ))
            {
               DP1( hWDB, "ReadCommSettings failed!" ) ;

               ConnectError( IDS_ERR_COMMSETTINGS ) ;
               RIP_CITY( 0 ) ;
            }

            /* Get MODEM.INI for CommSettings.szModemType
             */
            if (!ReadModemSettings( lpFSM ))
            {
               DP1( hWDB, "ReadModemSettings failed!" ) ;

               ConnectError( IDS_ERR_MODEMSETTINGS ) ;
               RIP_CITY( 0 ) ;
            }

         }
         else
         {
            DP1( hWDB, "Could not allocate FSM structure" ) ;

            ErrorResBox( hDlg, NULL, MB_OK | MB_ICONSTOP,
                         IDS_APPTITLE, IDS_ERR_MEMORYALLOC,
                         (LPSTR)__FILE__, __LINE__ ) ;

            RIP_CITY( 0 ) ;
         }

         /* Make sure we do the right thing with carrier detect...
          *
          * fIgnoreCarrierDetect is exported from COMM.C
          */
         gfIgnoreCarrierDetect = lpFSM->fIgnoreCD ;


         /* Assume success
          */
         lpFSM->uiExitCode = IDOK ;

         lpFSM->uiState = STATE_AQUIRE ;

      break ;

      case FF_USERABORT:
         if (lpFSM->uiExitCode != IDCANCEL)
         {
            SetConnectStatusMsg( hDlg, rglpsz[IDS_ABORTING] ) ;
            DP4( hWDB, "ConnectFSM( FF_USERABORT )" ) ;
            lpFSM->uiExitCode = IDCANCEL ;
            lpFSM->uiState = STATE_ABORT ;
         }
      break ;

      case FF_PROCESS:
      default:
      {
         switch( lpFSM->uiState )
         {
            case STATE_PROCESS:
               DP4( hWDB, "STATE_PROCESS" ) ;
            break ;

            case STATE_PAUSE:
               SetConnectStatusMsg( hDlg, NULL ) ;
               DP4( hWDB, "STATE_PAUSE" ) ;
               if (IsPauseDone( &lpFSM->Pause ))
               {
                  /* Pause is complete.  Party on dude.
                   */
                  lpFSM->uiState = lpFSM->uiNextState ;
                  ResetPause( &lpFSM->Pause ) ;
               }
            break ;

            case STATE_INITFSM:
               SetConnectStatusMsg( hDlg, NULL ) ;
               DP4( hWDB, "STATE_INITFSM" ) ;
               /*
                * All we need to do here is set the next state.
                * or maybe...
                */
               lpFSM->uiState = STATE_AQUIRE ;
            break ;

            case STATE_AQUIRE:
               DP4( hWDB, "STATE_AQUIRE" ) ;
               SetConnectStatusMsg( hDlg, rglpsz[IDS_AQUIRING] ) ;

               /*
                * Use our CommSettings to aquire a port.
                */
               SetCommPort( lpFSM->CommSettings.szCommDevice ) ;
               SetBaudRate( lpFSM->CommSettings.dwBaudRate ) ;

               if (!OpenConnection())
               {
                  SetConnectStatusMsg( hDlg, "Could not aquire communications port!" ) ;

                  DP1( hWDB, "Could not establish COMM connection!" ) ;
                  lpFSM->uiExitCode = IDS_ERR_COMMPORT ;
                  lpFSM->uiLastState = lpFSM->uiState ;
                  lpFSM->uiState = STATE_ABORT ;
                  break ;
               }

               gpszModemResponse = gszModemResponse ;
               gszModemResponse[0] = '\0' ;
               SetCommNotificationWnd( hDlg ) ;

               lpFSM->uiLastState = lpFSM->uiState ;

#ifdef TEST
               lpFSM->uiState = STATE_LOGIN ;
#else

               lpFSM->uiState = STATE_PAUSE ;
               lpFSM->uiNextState = STATE_INITMODEM ;
               StartPause( &lpFSM->Pause, uiPostAquirePause ) ;
#endif
            break ;

            case STATE_TRYAGAIN:
               SetConnectStatusMsg( hDlg, NULL ) ;
               DP4( hWDB, "STATE_TRYAGAIN" ) ;

               if (uiRedialAttempts < uiMaxRedialAttempts)
               {
                  lpFSM->uiNextState = STATE_DIALMODEM ;
                  lpFSM->uiLastState = STATE_TRYAGAIN ;
                  lpFSM->uiState = STATE_PAUSE ;
                  StartPause( &lpFSM->Pause, uiModemDialPause * 4 ) ;
                  uiRedialAttempts++ ;
               }
               else
               {
                  if (fUIup == FALSE)
                  {
                     fUIup = TRUE ;
                     if (IDYES == ErrorResBox( hDlg, hinstApp,
                                          MB_YESNO | MB_ICONQUESTION,
                                          IDS_ERR_COMMUNICATIONS,
                                          IDS_ERR_BUSY_TRYAGAIN ))
                     {
                        lpFSM->uiNextState = STATE_DIALMODEM ;
                        lpFSM->uiLastState = STATE_TRYAGAIN ;
                        lpFSM->uiState = STATE_PAUSE ;
                        StartPause( &lpFSM->Pause, uiModemDialPause * 4 ) ;
                        uiRedialAttempts = 1 ;
                     }
                     else
                     {
                        lpFSM->uiLastState = lpFSM->uiState ;
                        lpFSM->uiState = STATE_DISCONNECT ;
                        lpFSM->uiExitCode = IDCANCEL ;
                     }

                     fUIup = FALSE ;
                  }
                  break ;
               }
            break ;


            case STATE_INITMODEM:
               DP4( hWDB, "STATE_INITMODEM" ) ;
               /*
                * This state can be entered many times during an
                * attempt.  The string pointed to by the lpszInit
                * member contains the entire init sequence.  But
                * this init sequence can be made up of many init
                * commands.  Each command is separated by a ^M or a
                * set of '~'.
                *
                * Each time we come into this state lpszInitCur
                * points to the beginning of the current command (i.e.
                * the one that needs to be sent).
                */
               SetConnectStatusMsg( hDlg, rglpsz[IDS_INITMODEM] ) ;
               if (lpFSM->lpszInitCur && *lpFSM->lpszInitCur)
               {
                  DWORD dwPause ;

                  for ( dwPause = 0 ;
                        *lpFSM->lpszInitCur == '~' ; lpFSM->lpszInitCur++)
                     dwPause += uiModemDialPause ;

                  if (dwPause)
                  {
                     lpFSM->uiLastState = STATE_INITMODEM ;
                     lpFSM->uiNextState = STATE_INITMODEM ;
                     lpFSM->uiState = STATE_PAUSE ;
                     StartPause( &lpFSM->Pause, dwPause ) ;
                  }
                  else
                  {
                     gpszModemResponse = gszModemResponse ;
                     gszModemResponse[0] = '\0' ;

                     /* Clear the incomming buffer.
                      */
                     FlushInComm() ;

                     lpFSM->lpszInitCur = SendModemString( (LPFSM)lpFSM, lpFSM->lpszInitCur ) ;

                     if ((lpFSM->lpszInitCur > lpFSM->szInit) &&
                         *(lpFSM->lpszInitCur - 1) == ASCII_CR)
                     {
                        /*
                        * After sending the command to the modem
                        * we just 'wait' until we get the response
                        * we wanted or we get a timeout
                        */
                        lpFSM->uiLastState = STATE_INITMODEM ;
                        lpFSM->uiState = STATE_WAITINIT ;

                        StartTimeout( &lpFSM->Timeout, 10) ;
                     }
                  }
               }
               else
               {
                  /*
                   * No more init string.  
                   */
                  lpFSM->uiNextState = STATE_DIALMODEM ;
                  lpFSM->uiLastState = STATE_INITMODEM ;
                  lpFSM->uiState = STATE_PAUSE ;
                  StartPause( &lpFSM->Pause, uiModemDialPause * 1 ) ;
               }
            break ;

            case STATE_WAITINIT:
            SetConnectStatusMsg( hDlg, NULL ) ;
            DP4( hWDB, "STATE_WAITINIT" ) ;
            {
               /*
                * We're waiting for the modem to respond to one of our
                * init commands.  It's a lot like a pause.
                */
               if (IsTimeoutOver( &lpFSM->Timeout ))
               {
                  /* We've timed out!  Bummer.
                   */                    
                  DP1( hWDB, "Timeout in STATE_WAITINIT!!!" ) ;
                  lpFSM->uiExitCode = IDS_ERR_INITMODEM ;
                  lpFSM->uiLastState = lpFSM->uiState ;
                  lpFSM->uiState = STATE_ABORT ;
                  ResetTimeout( &lpFSM->Timeout ) ;
                  break ;
               }

               /*
                * Check the modem response
                */
               switch( CheckModemResponse( lpFSM, gszModemResponse ))
               {
                  case MR_OK:
                     lpFSM->uiLastState = STATE_WAITINIT ;
                     lpFSM->uiState = STATE_INITMODEM ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;

                  case MR_ERROR:
                     DP3( hWDB, "Modem response was MR_ERROR! (%s)", (LPSTR)gszModemResponse ) ;
                     lpFSM->uiExitCode = IDS_ERR_MODEMERR ;
                     lpFSM->uiLastState = lpFSM->uiState ;
                     lpFSM->uiState = STATE_ABORT ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;
               }
            }
            break ;

            case STATE_DIALMODEM:
               DP4( hWDB, "STATE_DIALMODEM" ) ;
               SetConnectStatusMsg( hDlg, rglpsz[IDS_DIALINGMODEM] ) ;

               gpszModemResponse = gszModemResponse ;
               gszModemResponse[0] = '\0' ;

               SendModemString( (LPFSM)lpFSM, lpFSM->szDial ) ;

               /*
                * After sending the command to the modem
                * we just 'wait' until we get the response
                * we wanted or we get a timeout
                */
               lpFSM->uiLastState = STATE_DIALMODEM ;
               lpFSM->uiState = STATE_WAITDIAL ;

               StartTimeout( &lpFSM->Timeout, uiConnectWaitDialTimeout ) ;
            break ;

            case STATE_WAITDIAL:
               DP4( hWDB, "STATE_WAITDIAL" ) ;
            {
               if (IsTimeoutOver( &lpFSM->Timeout ))
               {
                  /* We've timed out!  Bummer.
                   */                    
                  DP1( hWDB, "Timeout in STATE_WAITDIAL!!!" ) ;
                  SetConnectStatusMsg( hDlg, rglpsz[IDS_DIALTIMEDOUT] ) ;
                  ResetTimeout( &lpFSM->Timeout ) ;

                  /* Just make sure we're not connected! */
                  if (!lpFSM->fIgnoreCD && IsOnline())
                  {
                     FlushInComm() ;
                     FlushOutComm() ;

                     uiHangUpAttempts=1 ;
                     lpFSM->lpszHangUpCur = lpFSM->szHangUp ;
                     lpFSM->uiNextState = STATE_HANGUP ;
                     lpFSM->uiState = STATE_PAUSE ;
                     StartPause( &lpFSM->Pause, uiModemDialPause ) ;
                  }
                  else
                  {
                     lpFSM->uiLastState = lpFSM->uiState ;
                     lpFSM->uiState = STATE_ABORT ;
                  }
                  break ;
               }

               SetConnectStatusMsg( hDlg, rglpsz[IDS_WAITINGFORANSWER] ) ;

               /*
                * Check the modem response
                */
               switch( CheckModemResponse( lpFSM, gszModemResponse ))
               {
                  case MR_CONNECT:
                     DP2( hWDB, "MR_CONNECT" ) ;
                     lpFSM->uiState = STATE_LOGIN ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;

                  case MR_CONNECT300:
                     DP2( hWDB, "MR_CONNECT300" ) ;
                     SetBaudRate( 300 ) ;
                     lpFSM->uiState = STATE_LOGIN ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;

                  case MR_CONNECT1200:
                     DP2( hWDB, "MR_CONNECT1200" ) ;
                     SetBaudRate( 1200 ) ;
                     lpFSM->uiState = STATE_LOGIN ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;
                  
                  case MR_CONNECT2400:
                     DP2( hWDB, "MR_CONNECT2400" ) ;
                     SetBaudRate( 2400 ) ;
                     lpFSM->uiState = STATE_LOGIN ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;

                  case MR_CONNECT4800:
                     DP2( hWDB, "MR_CONNECT4800" ) ;
                     SetBaudRate( 4800 ) ;
                     lpFSM->uiState = STATE_LOGIN ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;

                  case MR_CONNECT9600:
                     DP2( hWDB, "MR_CONNECT9600" ) ;
                     SetBaudRate( 9600 ) ;
                     lpFSM->uiState = STATE_LOGIN ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;

                  case MR_BUSY:
                     SetConnectStatusMsg( hDlg, rglpsz[IDS_BUSY] ) ;
                     lpFSM->uiState = STATE_TRYAGAIN ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;

                  case MR_NODIALTONE:
                     SetConnectStatusMsg( hDlg, rglpsz[IDS_NODIALTONE] ) ;
                     lpFSM->uiExitCode = IDS_ERR_NODIALTONE ;
                     lpFSM->uiLastState = lpFSM->uiState ;
                     lpFSM->uiState = STATE_ABORT ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;

                  case MR_NOCARRIER:
                     SetConnectStatusMsg( hDlg, rglpsz[IDS_MODEMERR] ) ;
                     lpFSM->uiExitCode = IDS_ERR_NOCARRIER ;
                     lpFSM->uiLastState = lpFSM->uiState ;
                     lpFSM->uiState = STATE_ABORT ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;

                  case MR_ERROR:
                     SetConnectStatusMsg( hDlg, rglpsz[IDS_MODEMERR] ) ;
                     lpFSM->uiExitCode = IDS_ERR_MODEMERR ;
                     lpFSM->uiLastState = lpFSM->uiState ;
                     lpFSM->uiState = STATE_ABORT ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;
               }
            }
            break ;

            case STATE_LOGIN:
               DP4( hWDB, "STATE_LOGIN" ) ;

               SetConnectStatusMsg( hDlg, rglpsz[IDS_LOGIN] ) ;

               lpFSM->uiState = STATE_EXITFSM ;
            break ;

            case STATE_DISCONNECT:
               DP4( hWDB, "STATE_DISCONNECT" ) ;
               SetConnectStatusMsg( hDlg, rglpsz[IDS_RELEASE] ) ;

               if (lpFSM->uiLastState != STATE_DISCONNECT)
               {
                  WriteCommBlock( "\x0D\x0D", 2 ) ;

                  lpFSM->uiLastState = STATE_DISCONNECT ;
                  lpFSM->uiNextState = STATE_DISCONNECT ;
               }
               else
               {
                  CloseConnection() ;
                  lpFSM->uiState = STATE_EXITFSM ;
               }
            break ;

            case STATE_HANGUP:
               DP4( hWDB, "STATE_HANGUP" ) ;
               SetConnectStatusMsg( hDlg, rglpsz[IDS_HANGUP] ) ;
               /*
                * Each time we come into this state lpszHangUpCur
                * points to the beginning of the current command (i.e.
                * the one that needs to be sent).
                */
               if ((uiHangUpAttempts < MAX_HANGUPATTEMPTS) &&
                   (IsOnline() && lpFSM->lpszHangUpCur && *lpFSM->lpszHangUpCur))
               {
                  DWORD dwPause ;

                  for ( dwPause = 0 ;
                        *lpFSM->lpszHangUpCur == '~' ; lpFSM->lpszHangUpCur++)
                     dwPause += uiModemDialPause ;

                  if (dwPause)
                  {
                     lpFSM->uiNextState = STATE_HANGUP ;
                     lpFSM->uiState = STATE_PAUSE ;
                     StartPause( &lpFSM->Pause, dwPause ) ;
                  }
                  else
                  {
                     gpszModemResponse = gszModemResponse ;
                     gszModemResponse[0] = '\0' ;

                     lpFSM->lpszHangUpCur = SendModemString( (LPFSM)lpFSM, lpFSM->lpszHangUpCur ) ;
                     if ((lpFSM->lpszHangUpCur > lpFSM->szHangUp) &&
                         *(lpFSM->lpszHangUpCur - 1) == ASCII_CR)
                     {
                        StartTimeout( &lpFSM->Timeout, 5) ;
                        lpFSM->uiState = STATE_WAITHANGUP ;
                     }
                  }
               }
               else
               {
                  /* Do we still detect carrier?
                   */
                  if ((uiHangUpAttempts < MAX_HANGUPATTEMPTS) && IsOnline())
                  {
                     FlushInComm() ;
                     FlushOutComm() ;

                     uiHangUpAttempts++ ;
                     lpFSM->lpszHangUpCur = lpFSM->szHangUp ;
                     lpFSM->uiNextState = STATE_HANGUP ;
                     lpFSM->uiState = STATE_PAUSE ;
                     StartPause( &lpFSM->Pause, uiModemDialPause ) ;
                  }
                  else
                     lpFSM->uiState = STATE_DISCONNECT ;
               }
            break ;

            case STATE_WAITHANGUP:
               DP4( hWDB, "STATE_WAITHANGUP" ) ;
            {
               SetConnectStatusMsg( hDlg, NULL ) ;

               /*
                * We're waiting for the modem to respond to one of our
                * HangUp commands.  It's a lot like a pause.
                */
               if (IsTimeoutOver( &lpFSM->Timeout ))
               {
                  /* We've timed out!  Bummer.
                   */                    
                  DP1( hWDB, "Timeout in STATE_WAITHANGUP!!!" ) ;

                  ResetTimeout( &lpFSM->Timeout ) ;

                  if (IsOnline())
                  {
                     FlushInComm() ;
                     FlushOutComm() ;

                     uiHangUpAttempts++ ;
                     lpFSM->lpszHangUpCur = lpFSM->szHangUp ;
                     lpFSM->uiNextState = STATE_HANGUP ;
                     lpFSM->uiState = STATE_PAUSE ;
                     StartPause( &lpFSM->Pause, uiModemDialPause ) ;
                  }
                  else
                  {
                     lpFSM->uiLastState = lpFSM->uiState ;
                     lpFSM->uiState = STATE_DISCONNECT ;
                  }
                  break ;
               }

               /*
                * Check the modem response
                */
               switch( CheckModemResponse( lpFSM, gszModemResponse ))
               {
                  case MR_ERROR:
                     lpFSM->uiLastState = lpFSM->uiState ;
                     lpFSM->uiState = STATE_DISCONNECT ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;

                  default:
                  case MR_OK:
                     lpFSM->uiState = STATE_HANGUP ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;
               }
            }
            break ;

            case STATE_ABORT:
               DP4( hWDB, "STATE_ABORT" ) ;
               SetConnectStatusMsg( hDlg, NULL ) ;
               switch( lpFSM->uiLastState)
               {
                  case STATE_LOGIN:
                     uiHangUpAttempts = 1 ;
                     lpFSM->uiState = STATE_HANGUP ;
                  break ;

                  case STATE_AQUIRE:
                  case STATE_INITMODEM:
                  case STATE_WAITINIT:
                  case STATE_DIALMODEM:
                  case STATE_WAITDIAL:
                  default:
                     lpFSM->uiState = STATE_DISCONNECT ;
                  break ;

               }
            break ;

            case STATE_EXITFSM:
               SetConnectStatusMsg( hDlg, NULL ) ;
               SetCommNotificationWnd( hwndTTY ) ;

               DP4( hWDB, "STATE_EXITFSM" ) ;
               KillTimer( hDlg, TIMER_ID ) ;
               EndDialog( hDlg, lpFSM->uiExitCode ) ;
               GlobalFreePtr( lpFSM ) ;
               REMOVE_FSM( hDlg ) ;
            break ;

            default:
               DP1( hWDB, "Yikes!  Undefined state in ConnectFSM" ) ;
               DASSERTMSG( hWDB, 0, "Yikes!  Undefined state in ConnectFSM" ) ;
               return FALSE ;
            break ;
         }

      }
   }

   return TRUE ;

/*
 * Failure handler.  If we come down here it's because of some sort
 * of fatal error such as out of memory or unable to read
 * config.
 */
Failure:
   /*
    * I hate goto's too, but they are great for failure handling
    */

   KillTimer( hDlg, TIMER_ID ) ;

   if (lpFSM)
   {
      GlobalFreePtr( lpFSM ) ;

      REMOVE_FSM( hDlg ) ;
   }

   /*
    * Put up a nasty error message.
    */
   if (uiIDerr)
   {
      /*
       * Use IDS_ERRXXXX format
       */
      ErrorResBox( hDlg, NULL, MB_OK | MB_ICONSTOP,
                   IDS_ERR_COMMUNICATIONS, uiIDerr ) ;
   }
   
   EndDialog( hDlg, IDCANCEL ) ;

   return FALSE ;

} /* ConnectFSM()  */


/****************************************************************
 *  BOOL WINAPI DoDisConnect( VOID )
 *
 *  Description: 
 *
 *    Pops up the DoDisConnect modal dialog box.
 *
 *    This dialog box is centered over the client app which should
 *    have it's main window disabled before this dialog pops up.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI DoDisConnect( VOID )
{
   int ui ;
   static DLGPROC  lpfn = NULL ;

   /*
    * If we're not connected the just bail.
    */
   if (GetCommID() < 0)
   {
      return TRUE ;
   }

   /*
    * If the online dialog is up then we're probably still connected
    * and we need to 'logoff'
    */
   if (hdlgOnline && IsWindow( hdlgOnline ))
   {
      /*
       * Tell the bbs we're hanging up
       */
      if (IsOnline())
         SendBBSMsg(  MSG_HANGING_UP, NULL, NULL ) ;

      /*
       * Shut down the Online finite state machine since we
       * assume that the disconnect will always succed.
       */
      OnlineFSM( hdlgOnline, FF_ONLINE_EXITFSM ) ;

   }

   if (lpfn == NULL)
   {
      (FARPROC)lpfn = MakeProcInstance( (FARPROC)fnDisConnectDlg, hinstApp ) ;

      SetTTYBinary( FALSE ) ;
      ui = DialogBox( hinstApp, MAKEINTRESOURCE( DLG_DISCONNECT ),
                           hwndClientApp, lpfn ) ;

      DP3( hWDB, "Back from DialogBox call..." ) ;
      FreeProcInstance( (FARPROC)lpfn ) ;
      lpfn =  NULL ;
   }
   else
      ui = IDOK ;

   #if 0
   if (IsWindow( hwndStat ))
      SetWindowText( hwndStat, "" ) ;
   #endif

   DP3( hWDB, "Returning from DoDisConnect()" ) ;
   return (ui == IDOK) ;

} /* DoDisConnect()  */


/****************************************************************
 *  BOOL CALLBACK fnDisConnectDlg( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    DisConnect Dialog box procedure
 *
 *  Comments:
 *
 ****************************************************************/
BOOL CALLBACK fnDisConnectDlg( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   UINT           i ;

   switch (wMsg)
   {
      case WM_INITDIALOG:

         /*
          * Setup all the controls
          */
         for (i = IDD_LED1 ; i <= IDD_LED10 ; i++)
            SendDlgItemMessage( hDlg, i, LED_SETCOLOR, 0, RGBLTGREEN ) ;

         uiCurLED = IDD_LED1 ;

         if (IsWindow( hwndClientApp ))
            DlgCenter( hDlg, hwndClientApp, 0 ) ;

         DisConnectFSM( hDlg, FF_INITFSM ) ;

      break ;

      case WM_TIMER:
         DisConnectFSM( hDlg, FF_PROCESS ) ;
      break;

      case WM_COMMNOTIFY:
      {
         ProcessCOMMNotification( wParam, lParam ) ;
         
         while (GetInBYTE( gpszModemResponse ))
         {
            if (gpszModemResponse >= gszModemResponse + MAX_MODEMRESP)
            {
               gpszModemResponse = gszModemResponse ;
               gszModemResponse[0] = '\0' ;
            }
            else
            {
               gpszModemResponse++ ;
               *gpszModemResponse = '\0' ;
            }
         }
      }
      break ;

   }
   return FALSE ;
} /* fnDisConnectDlg()  */

/****************************************************************
 *  BOOL NEAR PASCAL DisConnectFSM( HWND hDlg, UINT uiFunction )
 *
 *  Description: 
 *
 *    This function is the finite state machine manager for the
 *    disconnect command.
 *
 *    uiFunction is a forced function:
 *
 *    FF_INITFSM      Initializes the FSM to STATE_INITFSM
 *
 *    FF_PROCESS      Process the current state.  This is not
 *                      really a function.  FF_PROCESS is defined
 *                      as 0.
 *
 *
 *    The following states are defined:
 *
 *    State Label       Descripition
 *
 *    STATE_PAUSE       Pause for uiPauseTime milliseconds.
 *
 *    STATE_INITFSM     INIT the finite state machine.
 *
 *    STATE_DISCONNECT  Disconnecting (De-aquire) comm port.
 *
 *    STATE_HANGUP      Hanging up modem.
 *
 *    STATE_WAITHANGUP  Waiting for the hangup response message.
 *
 *    STATE_EXITFSM     Exit dialog box.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL DisConnectFSM( HWND hDlg, UINT uiFunction )
{
   LPCONNECTFSM   lpFSM = NULL ;

   if (uiFunction != FF_INITFSM)
   {
      lpFSM = (LPCONNECTFSM)GET_FSM( hDlg ) ;

      /*
       * If we ever have this case, we are *really* in trouble.  No
       * need to tell the user about it.
       */
      if (!lpFSM)
         return FALSE ;

   }


   /* Handle each forced function.  The 'default' case (also FF_PROCESS)
    * in this
    * switch statement is where the actual states are handled.
    */
   switch (uiFunction)
   {
      case FF_INITFSM:
         DP3( hWDB, "DisConnectFSM( FF_INITFSM )" ) ;

         SetConnectStatusMsg( hDlg, rglpsz[IDS_HANGUP] ) ;

         /*
          * Create a timer
          */

         if (!SetTimer( hDlg, TIMER_ID, uiConnectDriverSpeed, NULL ))
         {
            ErrorResBox( hDlg, NULL, MB_OK | MB_ICONSTOP,
                   IDS_ERR_COMMUNICATIONS, IDS_ERR_TIMER ) ;

            return FALSE ;
         }   


         if (lpFSM = GlobalAllocPtr( GHND, sizeof( CONNECTFSM )))
         {
            /*
             * Put our far pointer into the window props of the dlg.
             */
            CREATE_FSM( hDlg, lpFSM ) ;

            /* Initialize the FSM structure
             *
             */
            lpFSM->uiNextState = STATE_PROCESS ;
            ResetPause( &lpFSM->Pause ) ;
            ResetTimeout( &lpFSM->Timeout ) ;

            /* Start with [Comm Settings] in the clients INI file
             */
            if (!ReadCommSettings( &lpFSM->CommSettings, rglpsz[IDS_INI_FILENAME] ))
            {
               DP1( hWDB, "ReadCommSettings failed!" ) ;

               ConnectError( IDS_ERR_COMMSETTINGS ) ;

               RIP_CITY( 0 ) ;
            }

            /* Get MODEM.INI for CommSettings.szModemType
             */
            if (!ReadModemSettings( lpFSM ))
            {
               DP1( hWDB, "ReadModemSettings failed!" ) ;

               ConnectError( IDS_ERR_MODEMSETTINGS ) ;
               RIP_CITY( 0 ) ;
            }

         }
         else
         {
            DP1( hWDB, "Could not allocate FSM structure" ) ;

            ErrorResBox( hDlg, NULL, MB_OK | MB_ICONSTOP,
                         IDS_APPTITLE, IDS_ERR_MEMORYALLOC,
                         (LPSTR)__FILE__, (LPSTR)__LINE__ ) ;
            RIP_CITY( 0 ) ;
         }

         /* Make sure we do the right thing with carrier detect...
          *
          * gfIgnoreCarrierDetect is exported from COMM.C
          */
         gfIgnoreCarrierDetect = lpFSM->fIgnoreCD ;

         gpszModemResponse = gszModemResponse ;
         gszModemResponse[0] = '\0' ;
         SetCommNotificationWnd( hDlg ) ;

         /*
          * Assume success
          */
         lpFSM->uiExitCode = IDOK ;

         if (IsOnline())
         {
            uiHangUpAttempts = 1 ;
            lpFSM->uiState = STATE_HANGUP ;

            FlushInComm() ;
            FlushOutComm() ;
         }
         else
            lpFSM->uiState = STATE_DISCONNECT ;

      break ;

      case FF_PROCESS:
      default:
      {
         switch( lpFSM->uiState )
         {
            case STATE_PROCESS:
//               SetConnectStatusMsg( hDlg, NULL ) ;
               DP4( hWDB, "STATE_PROCESS" ) ;
            break ;

            case STATE_PAUSE:
               SetConnectStatusMsg( hDlg, NULL ) ;
               DP4( hWDB, "STATE_PAUSE" ) ;
               if (IsPauseDone( &lpFSM->Pause ))
               {
                  /* Pause is complete.  Party on dude.
                   */
                  lpFSM->uiState = lpFSM->uiNextState ;
                  ResetPause( &lpFSM->Pause ) ;
               }
            break ;

            case STATE_INITFSM:
               SetConnectStatusMsg( hDlg, NULL ) ;
               DP4( hWDB, "STATE_INITFSM" ) ;
               /*
                * All we need to do here is set the next state.
                * or maybe...
                */
               uiHangUpAttempts = 1 ;
               lpFSM->uiState = STATE_HANGUP ;
            break ;


            case STATE_DISCONNECT:
               DP4( hWDB, "STATE_DISCONNECT" ) ;
               SetConnectStatusMsg( hDlg, rglpsz[IDS_RELEASE] ) ;

               if (lpFSM->uiLastState != STATE_DISCONNECT)
               {
                  WriteCommBlock( "\x0D", 1 ) ;

                  lpFSM->uiLastState = STATE_DISCONNECT ;
                  lpFSM->uiNextState = STATE_DISCONNECT ;
               }
               else
               {
                  SetCommNotificationWnd( hwndTTY ) ;
                  CloseConnection() ;
                  lpFSM->uiState = STATE_EXITFSM ;
               }
            break ;

            case STATE_HANGUP:
               DP4( hWDB, "STATE_HANGUP" ) ;
               SetConnectStatusMsg( hDlg, rglpsz[IDS_HANGUP] ) ;
               /*
                * Each time we come into this state lpszHangUpCur
                * points to the beginning of the current command (i.e.
                * the one that needs to be sent).
                */
               if ((uiHangUpAttempts < MAX_HANGUPATTEMPTS) &&
                   (IsOnline() && lpFSM->lpszHangUpCur && *lpFSM->lpszHangUpCur))
               {
                  DWORD dwPause ;

                  for ( dwPause = 0 ;
                        *lpFSM->lpszHangUpCur == '~' ; lpFSM->lpszHangUpCur++)
                     dwPause += uiModemDialPause ;

                  if (dwPause)
                  {
                     lpFSM->uiNextState = STATE_HANGUP ;
                     lpFSM->uiState = STATE_PAUSE ;
                     StartPause( &lpFSM->Pause, dwPause ) ;
                  }
                  else
                  {
                     gpszModemResponse = gszModemResponse ;
                     gszModemResponse[0] = '\0' ;

                     lpFSM->lpszHangUpCur = SendModemString( (LPFSM)lpFSM, lpFSM->lpszHangUpCur ) ;
                     if ((lpFSM->lpszHangUpCur > lpFSM->szHangUp) &&
                         *(lpFSM->lpszHangUpCur - 1) == ASCII_CR)
                     {
                        StartTimeout( &lpFSM->Timeout, 5) ;
                        lpFSM->uiState = STATE_WAITHANGUP ;
                     }
                  }
               }
               else
               {
                  /* Do we still detect carrier?
                   */
                  if ((uiHangUpAttempts < MAX_HANGUPATTEMPTS) && IsOnline())
                  {
                     FlushInComm() ;
                     FlushOutComm() ;

                     uiHangUpAttempts++ ;
                     lpFSM->lpszHangUpCur = lpFSM->szHangUp ;
                     lpFSM->uiNextState = STATE_HANGUP ;
                     lpFSM->uiState = STATE_PAUSE ;
                     StartPause( &lpFSM->Pause, uiModemDialPause ) ;
                  }
                  else
                     lpFSM->uiState = STATE_DISCONNECT ;
               }
            break ;

            case STATE_WAITHANGUP:
               DP4( hWDB, "STATE_WAITHANGUP" ) ;
            {
               SetConnectStatusMsg( hDlg, NULL ) ;

               /*
                * We're waiting for the modem to respond to one of our
                * HangUp commands.  It's a lot like a pause.
                */
               if (IsTimeoutOver( &lpFSM->Timeout ))
               {
                  /* We've timed out!  Bummer.
                   */                    
                  DP1( hWDB, "Timeout in STATE_WAITHANGUP!!!" ) ;

                  ResetTimeout( &lpFSM->Timeout ) ;

                  if (IsOnline())
                  {
                     FlushInComm() ;
                     FlushOutComm() ;

                     uiHangUpAttempts++ ;
                     lpFSM->lpszHangUpCur = lpFSM->szHangUp ;
                     lpFSM->uiNextState = STATE_HANGUP ;
                     lpFSM->uiState = STATE_PAUSE ;
                     StartPause( &lpFSM->Pause, uiModemDialPause ) ;
                  }
                  else
                  {
                     lpFSM->uiLastState = lpFSM->uiState ;
                     lpFSM->uiState = STATE_DISCONNECT ;
                  }
                  break ;
               }

               /*
                * Check the modem response
                */
               switch( CheckModemResponse( lpFSM, gszModemResponse ))
               {
                  case MR_ERROR:
                     lpFSM->uiLastState = lpFSM->uiState ;
                     lpFSM->uiState = STATE_DISCONNECT ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;

                  default:
                  case MR_OK:
                     lpFSM->uiState = STATE_HANGUP ;
                     ResetTimeout( &lpFSM->Timeout ) ;
                  break ;
               }
            }
            break ;

            case STATE_EXITFSM:
               DP4( hWDB, "STATE_EXITFSM" ) ;
               SetConnectStatusMsg( hDlg, NULL ) ;

               DP3( hWDB, "Calling EndDialog()..." ) ;
               EndDialog( hDlg, lpFSM->uiExitCode ) ;
               KillTimer( hDlg, TIMER_ID ) ;
               GlobalFreePtr( lpFSM ) ;
               REMOVE_FSM( hDlg ) ;
               DP3( hWDB, "Returning from STATE_EXITFSM..." ) ;
               return FALSE ;
            break ;

            default:
               DP1( hWDB, "Yikes!  Undefined state in DisConnectFSM" ) ;
               DASSERTMSG( hWDB, 0, "Yikes!  Undefined state in DisConnectFSM" ) ;
               return FALSE ;
            break ;
         }

      }
   }

   return TRUE ;

/*
 * Failure handler.  If we come down here it's because of some sort
 * of fatal error such as out of memory or unable to read
 * config.
 */
Failure:
   DP1( hWDB, "Failure in DisConnectFSM()" ) ;

   /*
    * I hate goto's too, but they are great for failure handling
    */

   KillTimer( hDlg, TIMER_ID ) ;

   if (lpFSM)
   {
      GlobalFreePtr( lpFSM ) ;

      REMOVE_FSM( hDlg ) ;
   }

   /*
    * Put up a nasty error message.
    */
   if (uiIDerr)
   {
      /*
       * Use IDS_ERRXXXX format
       */
      ErrorResBox( hDlg, NULL, MB_OK | MB_ICONSTOP,
                   IDS_ERR_COMMUNICATIONS, uiIDerr ) ;
   }

   EndDialog( hDlg, IDCANCEL ) ;

   return FALSE ;

} /* DisConnectFSM()  */

/****************************************************************
 *  LPSTR NEAR PASCAL SendModemString( LPFSM lpFSM, LPSTR lpsz )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
LPSTR NEAR PASCAL SendModemString( LPFSM lpFSM, LPSTR lpsz )
{
   LPSTR lpszNext = lpsz ;
   UINT  cb = 0 ;

   while(*lpszNext)
   {
      switch( *lpszNext )
      {
         case '~':
            /*
             * to the port!
             */
            WriteCommBlock( lpsz, cb ) ;

         return lpszNext ;

         case ASCII_CR:
            /*
             * to the port!
             */
            WriteCommBlock( lpsz, cb+1 ) ;

         return ++lpszNext ;
      }

      cb++ ;

      lpszNext++ ;
   }

   /*
    * Nothing on the end...to the port!
    */
   if (cb)
      WriteCommBlock( lpsz, cb ) ;

   return lpszNext ;

} /* SendModemString()  */

/****************************************************************
 *  VOID NEAR PASCAL SetConnectStatusMsg( LPSTR lpsz )
 *
 *  Description: 
 *
 *    For whatever dialog box is up there will always be a
 *    IDD_STATUS text control. 
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL SetConnectStatusMsg( HWND hDlg, LPSTR lpsz )
{
   if (IsWindow( hDlg ))
   {
      char  szBuf[256] ;

      if (lpsz)
      {
         GetDlgItemText( hDlg, IDD_STATUS, szBuf, 255 ) ;
         if (0 != lstrcmpi( szBuf, lpsz ))
            SetDlgItemText( hDlg, IDD_STATUS, lpsz ) ;

         #if 0
         if (IsWindow( hwndStat ))
         {
            GetWindowText( hwndStat, szBuf, 255 ) ;
            if (0 != lstrcmpi( szBuf, lpsz ))
               SetWindowText( hwndStat, lpsz ) ;
         }
         #endif
      }

      SendDlgItemMessage( hDlg, uiCurLED, LED_SETCOLOR, 0, RGBLTGREEN ) ;
      UpdateWindow( GetDlgItem( hDlg, uiCurLED ) ) ;

      if (++uiCurLED > IDD_LED10)
         uiCurLED = IDD_LED1 ;

      SendDlgItemMessage( hDlg, uiCurLED, LED_SETCOLOR, 0, RGBLTRED ) ;
      UpdateWindow( GetDlgItem( hDlg, uiCurLED ) ) ;

   }


} /* SetConnectStatusMsg()  */

/****************************************************************
 *  VOID NEAR PASCAL ConnectError( UINT uiErr )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL ConnectError( UINT uiErr )
{
   char szInfo[256] ;

   if (!LoadString( hinstApp, IDS_ERR_TECHSUPPORT, szInfo, 255 ))
      szInfo[0] = '\0' ;

   ErrorResBox( hwndClientApp, hinstApp, MB_OK | MB_ICONSTOP,
                        IDS_ERR_COMMUNICATIONS,
                        uiErr, (LPSTR)szInfo ) ;

} /* ConnectError()  */

BOOL NEAR PASCAL CompareModemResponse( LPSTR lpsz, LPSTR lpszResponse ) ;

/****************************************************************
 *  UINT WINAPI CheckModemResponse( LPFSM lpFSM, LPSTR lpszResponse )
 *
 *  Description: 
 *
 *    Returns the ID of the result string as found in lpszResponse.
 *    For example if the we read "CONNECT 9600" into lpszResponse,
 *    and we called this function MR_CONNECT9600 would be returned.
 *
 *  Comments:
 *
 ****************************************************************/
UINT NEAR PASCAL CheckModemResponse( LPCONNECTFSM lpFSM, LPSTR lpszResponse )
{

   if (CompareModemResponse( lpFSM->szOk, lpszResponse ))
   {
      DP4( hWDB, "MR_OK" ) ;
      return MR_OK ;
   }

   if (CompareModemResponse( lpFSM->szBusy, lpszResponse ))
   {
      DP4( hWDB, "MR_BUSY" ) ;
      return MR_BUSY ;
   }

   if (CompareModemResponse( lpFSM->szNoDialtone, lpszResponse ))
   {
      DP4( hWDB, "MR_NODIALTONE" ) ;
      return MR_NODIALTONE ;
   }

   if (CompareModemResponse( lpFSM->szNoCarrier , lpszResponse ))
   {
      DP4( hWDB, "MR_NOCARRIER" ) ;
      return MR_NOCARRIER ;
   }

   if (CompareModemResponse( lpFSM->szError, lpszResponse ))
   {
      DP4( hWDB, "MR_ERROR" ) ;
      return MR_ERROR ;
   }

   if (CompareModemResponse( lpFSM->szConnect300, lpszResponse ))
   {
      DP4( hWDB, "MR_CONNECT300" ) ;
      return MR_CONNECT300 ;
   }

   if (CompareModemResponse( lpFSM->szConnect1200, lpszResponse ))
   {
      DP4( hWDB, "MR_CONNECT1200" ) ;
      return MR_CONNECT1200 ;
   }

   if (CompareModemResponse( lpFSM->szConnect2400, lpszResponse ))
   {
      DP4( hWDB, "MR_CONNECT2400" ) ;
      return MR_CONNECT2400 ;
   }

   if (CompareModemResponse( lpFSM->szConnect4800, lpszResponse ))
   {
      DP4( hWDB, "MR_CONNECT4800" ) ;
      return MR_CONNECT4800 ;
   }

   if (CompareModemResponse( lpFSM->szConnect9600, lpszResponse ))
   {
      DP4( hWDB, "MR_CONNECT9600" ) ;
      return MR_CONNECT9600 ;
   }

   /*
    * CompareModemResponse will return TRUE for "CONNECT", "CONNECT 14400"
    */
   if (CompareModemResponse( lpFSM->szConnect, lpszResponse ))
   {
      DP4( hWDB, "MR_CONNECT" ) ;
      return MR_CONNECT ;
   }

   return (UINT)-1 ;

} /* CheckModemResponse()  */

/****************************************************************
 *  BOOL NEAR PASCAL CompareModemResponse( LPSTR lpsz, LPSTR lpszResponse )
 *
 *  Description: 
 *
 *    Returns true if the string pointed to by lpsz is found
 *    within lpszResponse.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL CompareModemResponse( LPSTR lpsz, LPSTR lpszResponse )
{
   LPSTR p ;
   int  n ;

   if (lstrlen(lpszResponse) < (n=lstrlen(lpsz)))
      return FALSE ;

   for (p = lpszResponse ; *(p+n-1) ; p++)
      if (0 == _fstrnicmp( p, lpsz, n ))
         return TRUE ;

   return FALSE ;

} /* CompareModemResponse()  */

/************************************************************************
 * End of File: connect.c
 ***********************************************************************/

