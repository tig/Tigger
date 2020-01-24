/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CONXSRV
 *
 *      Module:  ddecmds.c
 *
 *     Remarks:  DDE Execute commands are processed here.
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "CONXSRV.h"
#include "connect.h"
#include "dde.h"
#include "ddecmds.h"
#include "bbs.h"

/****************************************************************
 *  HDDEDATA CALLBACK fnCMDConnect( HSZ hszTopic, LPSTR lpszCmd, UINT wFlags )
 *                       
 *  Description: 
 *
 *       [Connect("userid","password")]
 *
 ****************************************************************/
HDDEDATA CALLBACK fnCMDConnect( HSZ hszTopic, LPSTR lpszCmd, UINT wFlags )
{
   DP3( hWDB, "DDE Execute: Connect( %s )", (LPSTR)lpszCmd ) ;

   if (!lpszCmd)
   {
      DP1( hWDB, "lpszCmd is NULL in fnCMDConnect" ) ;
      wsprintf( szLastDdeErr, "Invalid Login Information." ) ;
      return (HDDEDATA)DDE_FNOTPROCESSED ;
   }

   /*
    * Put up the connect status dialog box.  This implies
    * that the client has disabled all input.  The dialog box
    * will be popped up so that it is centered over the client.
    */
   if (DoConnect())
      return (HDDEDATA)DDE_FACK ;
   else
      return (HDDEDATA)DDE_FNOTPROCESSED ;

} /* fnCMDCOnnect()  */

/****************************************************************
 *  HDDEDATA CALLBACK fnCMDDisConnect( HSZ hszTopic, LPSTR lpszCmd, UINT wFlags )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
HDDEDATA CALLBACK fnCMDDisConnect( HSZ hszTopic, LPSTR lpszCmd, UINT wFlags )
{
   DP3( hWDB, "DDE Execute: DisConnect( %s )", (LPSTR)lpszCmd ) ;

   /*
    * Pop up a dialog that tells the user what we're up to..
    */
   DoDisConnect() ;

   return (HDDEDATA)DDE_FACK ;

} /* fnCMDDisConnect()  */


/****************************************************************
 *  HDDEDATA CALLBACK fnCMDSendMsg( HSZ hszTopic,  LPSTR lpszCmd,   UINT wFlags )
 *
 *  Description: 
 *
 *    This command causes the message in the gMsg structure to
 *    be sent.
 *
 *  Comments:
 *
 ****************************************************************/
HDDEDATA CALLBACK fnCMDSendMsg( HSZ hszTopic,  LPSTR lpszCmd,   UINT wFlags )
{
   UINT uiExitCode ;

   DP3( hWDB, "DDE Execute: SendMsg( %s )", (LPSTR)lpszCmd ) ;

   if (!IsOnline())
   {
      DP1( hWDB, "fnCmdSendMsg : Not online!" ) ;
      return (HDDEDATA)DDE_FNOTPROCESSED ;
   }

   if (!(uiExitCode = ProcessSendBBSMsg( &gMsg )))
   {
      DP3( hWDB, "ProcessSendBBSMsg() returned %d", uiExitCode ) ;
      return (HDDEDATA)DDE_FACK ;
   }
   else
   {
      DP1( hWDB, "fnCMDSendMsg : Failed!" ) ;
      return (HDDEDATA)DDE_FNOTPROCESSED ;
   }

} /* fnCMDSendMsg()  */


/************************************************************************
 * End of File: ddecmds.c
 ***********************************************************************/

