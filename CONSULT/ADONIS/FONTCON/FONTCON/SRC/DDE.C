//*************************************************************
//  File name: dde.c
//
//  Description: 
//      DDE Communications module
//
//  History:    Date       Author     Comment
//               3/18/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"

#include "global.h"
#include "dde.h"

WORD    gwDDEError   = DDE_ERR_OK;
char    gszDEEError[120];
HCONV   ghDDEConv     = NULL;
DWORD   gdwDDEInst     = 0L;
FARPROC glpfnDDECallback = NULL;

#define CONN_FAIL(X) {gwDDEError = X; goto connect_abort;}


//*************************************************************
//
//  fsddeCallback
//
//  Purpose:
//      Callback for DDEML
//
//
//  Parameters:
//      UINT wType
//      UINT wFmt
//      HCONV hConv
//      HSZ hsz1
//      HSZ hsz2
//      HDDEDATA hData
//      DWORD dwData1
//      DWORD dwData2
//      
//
//  Return: (HDDEDATA CALLBACK)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/21/92   MSM        Created
//
//*************************************************************

HDDEDATA CALLBACK fsddeCallback(UINT wType, UINT wFmt, HCONV hConv, 
        HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1, DWORD dwData2)
{
   DP4( hWDB, "fsddeCallback" );

   switch (wType)
   {
      case XTYP_ERROR:
         DP4( hWDB, "XTYP_ERROR" ) ;
      return NULL ;

      case XTYP_ADVDATA:
         DP1( hWDB, "XTYP_ADVDATA" ) ;
      return NULL ;

      case XTYP_DISCONNECT:
         DP4( hWDB, "XTYP_DISCONNECT" ) ;
      return NULL ;

      default:
      return NULL ;
   }
   return NULL ;

} //*** fsddeCallback


//*************************************************************
//
//  fsddeConnect
//
//  Purpose:
//      Connects to the DDE Server
//
//
//  Parameters:
//      HWND  hWnd      - Window to use as parent
//      LPSTR lpApp     - app to launch (Can be NULL)
//      LPSTR lpService - Service to connect to
//      LPSTR lpTopic   - Topic to connect with
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/21/92   MSM        Created
//
//*************************************************************

BOOL WINAPI fsddeConnect(HWND hWnd, LPSTR lpApp, LPSTR lpService, LPSTR lpTopic)
{
    UINT uVal;
    HSZ  hszService, hszTopic;

    if (gdwDDEInst)
    {
        DP1( hWDB, "fsddeConnect() : Already connected!  Disconnecting!" );
        fsddeDisconnect() ;
    }

    if (!glpfnDDECallback)
        glpfnDDECallback = MakeProcInstance( (FARPROC)fsddeCallback, ghInst );

    uVal = DdeInitialize( &gdwDDEInst, (PFNCALLBACK)glpfnDDECallback,
                APPCMD_CLIENTONLY|APPCLASS_STANDARD, 0L ); 

    if (uVal != DMLERR_NO_ERROR)
    {
        DP1( hWDB, "DDEML Failed to initialize!" );
        gwDDEError = DDE_ERR_DML_INIT;
        return FALSE;
    }

    if (lpApp)
    {
      /*
       * Launch the server.  The server is a single instance app, so that
       * we can do this as many times as we want and we'll only
       * get one instance.
       *
       * The server will close when it receives the shutdown
       * message broadcast by us after we leave our
       * message loop in WinMain()
       */
        if ( WinExec( lpApp, SW_SHOWMINNOACTIVE ) < 32 )
        {
            DP1( hWDB, "WinExec failed!" );
            CONN_FAIL( DDE_ERR_WINEXEC );
        }
    }

    hszService = DdeCreateStringHandle( gdwDDEInst, lpService, 0 );
    hszTopic = DdeCreateStringHandle( gdwDDEInst, lpTopic, 0 );

    DP4( hWDB, "DdeConnect( %s, %s );", lpService, lpTopic );
    ghDDEConv = DdeConnect( gdwDDEInst, hszService, hszTopic, NULL );

    if (!ghDDEConv)
    {
        DP1( hWDB, "DDEML Failed to connect to server!" );
        CONN_FAIL( DDE_ERR_CONNECT_FAILED );
    }

    return TRUE ;    

connect_abort:
    DP1( hWDB, "fsddeConnect has failed to establich connection" );
    DdeUninitialize( gdwDDEInst );
    gdwDDEInst = 0L;
    return FALSE ;

} //*** fsddeConnect


//*************************************************************
//
//  fsddeDisconnect
//
//  Purpose:
//      Disconnects the conversation
//
//
//  Parameters:
//      
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/22/92   MSM        Created
//
//*************************************************************

BOOL WINAPI fsddeDisconnect()
{
    if (ghDDEConv)
    {
        DdeDisconnect( ghDDEConv );
        ghDDEConv = NULL;
    }

    if (gdwDDEInst)
    {
        DdeUninitialize( gdwDDEInst );
        gdwDDEInst = 0L;
    }

    return TRUE;

} //*** fsddeDisconnect


//*************************************************************
//
//  fsddeExecute
//
//  Purpose:
//      Executes a string across the DDE connection
//
//
//  Parameters:
//      LPSTR lpString
//      DWORD dwTimeOut - if 0L then a 1 second T/O is used    
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/22/92   MSM        Created
//
//*************************************************************

BOOL WINAPI fsddeExecute(LPSTR lpString, DWORD dwTimeOut)
{
    HDDEDATA hData;
    DWORD    dwRes;

    if (!dwTimeOut)
        dwTimeOut = 1000;

    if (!ghDDEConv)
    {
        gwDDEError = DDE_ERR_NO_CONNECTION;
        return FALSE;
    }

    hData = DdeClientTransaction( lpString, lstrlen(lpString)+1, ghDDEConv, 
                NULL, CF_TEXT, XTYP_EXECUTE, dwTimeOut, &dwRes );

    return (hData==(HDDEDATA)TRUE);

} //*** fsddeExecute


//*************************************************************
//
//  fsddePoke
//
//  Purpose:
//      Pokes a string across the DDE connection
//
//
//  Parameters:
//      LPSTR lpszItem
//      LPVOID lpData - pointer to data to be poked
//      DWORD cb - size of buffer to be poked
//      DWORD dwTimeOut - if 0L then a 1 second T/O is used    
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              4/5/92     CEK        Created
//
//*************************************************************

BOOL WINAPI fsddePoke( LPCSTR lpszItem, LPVOID lpData, DWORD cb, DWORD dwTimeOut )
{
    HDDEDATA hData;
    DWORD    dwRes;
    HSZ      hszItem ;

    if (!dwTimeOut)
        dwTimeOut = 1000;

    if (!ghDDEConv)
    {
        gwDDEError = DDE_ERR_NO_CONNECTION;
        return FALSE;
    }

    hszItem = DdeCreateStringHandle( gdwDDEInst, lpszItem, 0 ) ;

    hData = DdeClientTransaction( lpData, cb, ghDDEConv, 
                hszItem, CF_TEXT, XTYP_POKE, dwTimeOut, &dwRes );

    DdeFreeStringHandle( gdwDDEInst, hszItem ) ;

   if (hData != (HDDEDATA)TRUE)
   {
      DP1( hWDB, "fsddePoke( %s ) failed!!! ", (LPSTR)lpszItem ) ;
      return FALSE ;
   }
   else
       return TRUE;

} //*** fsddePoke


//*************************************************************
//
//  fsddeRequest
//
//  Purpose:
//      Requests a string across the DDE connection
//
//
//  Parameters:
//      LPSTR lpszItem
//      LPVOID lpData - pointer to data to be Request
//      DWORD cb - size of buffer to be Request
//      DWORD dwTimeOut - if 0L then a 1 second T/O is used    
//
//  Return: (UINT WINAPI)
//
//    Number of bytes actually received.
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              4/5/92     CEK        Created
//
//*************************************************************

DWORD WINAPI fsddeRequest( LPCSTR lpszItem, LPVOID lpData, DWORD cb, DWORD dwTimeOut )
{
    HDDEDATA hData;
    DWORD    dwRes;
    HSZ      hszItem ;
    LPVOID   lpDataRec ;
    DWORD    cbRec = 0 ;

    if (!dwTimeOut)
        dwTimeOut = 1000;

    if (!ghDDEConv)
    {
        gwDDEError = DDE_ERR_NO_CONNECTION;
        return 0 ;
    }

    hszItem = DdeCreateStringHandle( gdwDDEInst, lpszItem, 0 ) ;

    DASSERT( hWDB, hszItem ) ;

    hData = DdeClientTransaction( NULL, 0, ghDDEConv, 
                hszItem, CF_TEXT, XTYP_REQUEST, dwTimeOut, &dwRes );

    DdeFreeStringHandle( gdwDDEInst, hszItem ) ;

    if (hData == NULL)
    {
       DP1( hWDB, "Request failed!!! dwRes = %08lX", (DWORD)dwRes ) ;
       return 0 ;
    }

    if (lpDataRec = DdeAccessData( hData, &cbRec ))
    {
       /*
        * HACK!!!  If we ever really had to deal with HUGE data
        * we would want to replace this _fmemcpy() with
        * a huge memory copy
        */
       _fmemcpy( lpData, lpDataRec, LOWORD(min(cb, cbRec)) ) ;

       DdeUnaccessData( hData ) ;
    }
    else
    {
      DP1( hWDB, "DdeAccessData() in fsddeRequest failed!!!" ) ;
      return 0 ;
    }

    DP4( hWDB, "fsddeRequest( %s ) returned %d", (LPSTR)lpszItem, hData ) ;

    return min(cb, cbRec) ;

} //*** fsddeRequest


//*************************************************************
//
//  fsddeGetError
//
//  Purpose:
//      Returns the last error that happened
//
//
//  Parameters:
//      
//      
//
//  Return: (DWORD WINAPI)
//      LOWORD is my error code, HIWORD is DDEMLs last error code
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/21/92   MSM        Created
//
//*************************************************************

DWORD  WINAPI fsddeGetError()
{
    if (gdwDDEInst)
        return MAKELONG(gwDDEError, DdeGetLastError( gdwDDEInst ));
    else
        return MAKELONG(gwDDEError, 0 );

} //*** fsddeGetError

//*** EOF: dde.c
