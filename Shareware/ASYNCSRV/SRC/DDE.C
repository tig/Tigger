/************************************************************************
 *
 *     Copyright (c) 1992-93 C.E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  Shopper
 *
 *      Module:  dde.c
 *
 *     Remarks:  DDE Central.  This be where we do DDE.
 *
 *   Revisions:  
 *     00.00.000  7/23/91 cek   First Try
 *     01.00.000  2/13/92 cek   Moved to Shopper
 *
 *
 ***********************************************************************/

#ifndef NODDEML

#include "PRECOMP.H"
#include "ASYNCSRV.H"
#include "botstat.h"
#include "dde.h"

#include "ddeitems.h"     // has all items
#include "ddecmds.h"     // has all commands
#include "connect.h"
                         
/*
 * The following variables are global and are externed in DDE.H
 */
GENERICMSG  gMsg ;

DWORD       dwDDEID = 0L ;
HSZ         hszAppName ;
BOOL        fConnected ;
BOOL        fLoggedIn ;
char        szLastDdeErr[256] ;
FORMATINFO  aFormats[] =
            {
                { 0, "CF_TEXT" },   // only format currently supported
            };

#define CFORMATS     (sizeof(aFormats)/sizeof(aFormats[0]))


/********************************************
 *
 * Command Action Functions (EXECUTEs)
 *
 *    We also use this structure for EXECUTE commands.
 *
 *    When an EXECUTE is recieved it is in the form of:
 *
 *    [command1(param1,param2)][command2(param3,...
 *
 *    This follows WinWord, Excel, and progman conventions
 *    so I think it's kinda standard...
 *
 *    For EXECUTE commands the format of the function is a bit
 *    different.  Use this:
 *
 *      HDDEDATA CALLBACK fnCMD_command( LPSTR lpCmd, UINT wFlags ) ;
 *
 *
 *******************************************/

HDDEDATA CALLBACK fnCMDConnect   ( HSZ hszTopic,  LPSTR lpCmd,     UINT wFlags ) ;
HDDEDATA CALLBACK fnCMDDisConnect( HSZ hszTopic,  LPSTR lpszCmd,   UINT wFlags ) ;
HDDEDATA CALLBACK fnCMDSendMsg   ( HSZ hszTopic,  LPSTR lpszCmd,   UINT wFlags ) ;


CMDLIST CmdList[] =
         {
            /* hszItem, lpfnCallback,        pszItem           */
            {  0,       fnCMDConnect,     IDS_DDE_CMD_CONNECT    },
            {  0,       fnCMDDisConnect,  IDS_DDE_CMD_DISCONNECT   },
            {  0,       fnCMDSendMsg,     IDS_DDE_CMD_SENDMSG   },
         } ;
#define CCMDS   (sizeof(CmdList)/sizeof(CmdList[0]))

/*******************************************
 *
 * Action Functions
 *
 *    Action functions are functions that do something
 *    in response to a DDE message from DDEML.  To add
 *    a new Action function declare it below and add an
 *    entry to the ItemList structure (updating
 *    #define CITEMS also).
 *
 *    Action items take two parameters, a LPXFERINFO which
 *    is really just a pointer into the stack (HACK!)
 *    and iFmt which is not used in  currently.
 *
 *    Heck.  You can even add new topics too.  This might
 *    be handy to organize stuff.
 *
 *******************************************/

HDDEDATA CALLBACK fnTopicListXfer(     LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnItemListXfer(      LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnSysFormatsXfer(    LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnHelpXfer(          LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnSrvStatus(            LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnGetLastErr(        LPXFERINFO lpXferInfo, UINT iFmt ) ;

HDDEDATA CALLBACK fnClientHWND( LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnSendData( LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnReceiveData( LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnMsgSendBuf( LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnMsgSendSize( LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnMsgReceiveBuf( LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnMsgReceiveSize( LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnMsgFlags( LPXFERINFO lpXferInfo, UINT iFmt ) ;
HDDEDATA CALLBACK fnMsg( LPXFERINFO lpXferInfo, UINT iFmt ) ;

/* Fontshoppper
 */
ITEMLIST ItemList[] =
         {
            { 0, fnTopicListXfer,   IDS_DDE_ITEM_TOPICS           },
            { 0, fnItemListXfer,    IDS_DDE_ITEM_SYSITEMS         },
            { 0, fnSysFormatsXfer,  IDS_DDE_ITEM_FORMATS          },
            { 0, fnHelpXfer,        IDS_DDE_ITEM_HELP             },
            { 0, fnSrvStatus,       IDS_DDE_ITEM_STATUS           },
            { 0, fnGetLastErr,      IDS_DDE_ITEM_LASTERR          },
            { 0, fnClientHWND,      IDS_DDE_ITEM_CLIENTHWND       },
            { 0, fnSendData,        IDS_DDE_ITEM_SD               },
            { 0, fnReceiveData,     IDS_DDE_ITEM_RD               },
            { 0, fnMsgSendBuf,      IDS_DDE_ITEM_MSGSENDBUF       },
            { 0, fnMsgSendSize,     IDS_DDE_ITEM_MSGSENDSIZE      },
            { 0, fnMsgReceiveBuf,   IDS_DDE_ITEM_MSGRECEIVEBUF    },
            { 0, fnMsgReceiveSize,  IDS_DDE_ITEM_MSGRECEIVESIZE   },
            { 0, fnMsgFlags,        IDS_DDE_ITEM_MSGFLAGS         },
            { 0, fnMsg     ,        IDS_DDE_ITEM_MSG              },
         } ;


#define CITEMS   (sizeof(ItemList)/sizeof(ItemList[0]))

/*
 * The topic structures are filled in dynamically in ddeInitDDEServer()
 * below...
 */
TOPICLIST Topics[] =
         {
            /* hszTopic, lpItemList, cItems, lpCmdList, cCmds, wIDS */
            { 0, NULL, 0, NULL, 0,  0 },  // FontShop
         } ;

#define CTOPICS    (sizeof(Topics)/sizeof(Topics[0]))

/*
 * Local helper functions
 */
UINT NEAR PASCAL GetTopicIndex( HSZ hszTopic ) ;
UINT NEAR PASCAL GetItemIndex( UINT wTopic, HSZ hszItem ) ;
UINT NEAR PASCAL GetCmdIndex( UINT wTopic, PSTR pszCmd ) ;
HDDEDATA NEAR PASCAL DispatchCommands( HSZ hszTopic, HDDEDATA hData ) ;

/****************************************************************
 *  HDDEDATA EXPENTRY fnDdeCallback()
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
HDDEDATA EXPENTRY fnDdeCallback( UINT     wType,
                                 UINT     wFmt,
                                 HCONV    hConv,
                                 HSZ      hszTopic,
                                 HSZ      hszItem,
                                 HDDEDATA hData,
                                 DWORD    dwData1,
                                 DWORD    dwData2 ) 
{
   UINT     i, j ;
   HDDEDATA hddeData ;

   DP4( hWDB, "fnDdeCallback" ) ;

   switch (wType)
   {
      case XTYP_ERROR:
         DP4( hWDB, "XTYP_ERROR" ) ;
      goto MyDefault ;

      case XTYP_ADVDATA:
         DP4( hWDB, "XTYP_ADVDATA" ) ;
      goto MyDefault ;

      case XTYP_ADVREQ:
         DP4( hWDB, "XTYP_ADVREQ" ) ;
      goto MyDefault ;

      case XTYP_ADVSTART:
         DP4( hWDB, "XTYP_ADVSTART" ) ;
      goto MyDefault ;

      case XTYP_ADVSTOP:
         DP4( hWDB, "XTYP_ADVSTOP" ) ;
      goto MyDefault ;

      case XTYP_EXECUTE:
         DP4( hWDB, "XTYP_EXECUTE" ) ;
         //
         // What topic index corresponds to hszTopic?  We store
         // this index in i.
         //
         // For EXECUTEs we let any topic do them...
         //
         hddeData = DispatchCommands( hszTopic, hData ) ;

      DP4( hWDB, "  execute ok!" ) ;
      return hddeData ;

      case XTYP_CONNECT:
         DP4( hWDB, "XTYP_CONNECT" ) ;

         if (-1 != GetTopicIndex( hszTopic ))
         {
            if (fConnected)
            {
               //
               // We only support one client 
               //

               wsprintf( szLastDdeErr, "ASYNCSRV Topic Session already active." ) ;
               DP2( hWDB, "DDE _ERROR: %s", (LPSTR)szLastDdeErr ) ;
               return NULL ;
            }
            else
            {
               DP4( hWDB, "\tConnected Ok!") ;

               StatusSetStatBox( hwndStat, IDSTAT_DDE ,rglpsz[IDS_DDE_CONNECT] ) ;
               return (HDDEDATA)TRUE ;
            }
         }

         wsprintf( szLastDdeErr, "Topic is not supported" ) ;
         DP2( hWDB, "DDE _ERROR: %s", (LPSTR)szLastDdeErr ) ;
      break ;

      case XTYP_CONNECT_CONFIRM:
         DP4( hWDB, "XTYP_CONNECT_CONFIRM" ) ;
         fConnected++ ;

         DP4( hWDB, "\tfConnected now %d", fConnected ) ;
      return NULL ;

      case XTYP_POKE:
         DP4( hWDB, "XTYP_POKE" ) ;
      goto MyDefault ;

      case XTYP_REGISTER:
         DP4( hWDB, "XTYP_REGISTER" ) ;
      goto MyDefault ;

      case XTYP_REQUEST:
         DP4( hWDB, "XTYP_REQUEST" ) ;
      goto MyDefault ;

      case XTYP_DISCONNECT:
         DP4( hWDB, "XTYP_DISCONNECT" ) ;

         if (IsOnline())
         {
            /*
             * Can't shutdown without first disconnecting!
             */
            DP1( hWDB, "\tDisconnecting COMM session in DDE..." ) ;

            DoDisConnect() ;
         }

         StatusSetStatBox( hwndStat, IDSTAT_DDE ,rglpsz[IDS_DDE_NOTCONNECT] ) ;

         fConnected-- ;

         DP4( hWDB, "\tfConnected now %d", fConnected ) ;
      return NULL ;

      case XTYP_UNREGISTER:
         DP4( hWDB, "XTYP_UNREGISTER" ) ;
      goto MyDefault ;

      case XTYP_WILDCONNECT:
         DP4( hWDB, "XTYP_WILDCONNECT" ) ;
      goto MyDefault ;

      case XTYP_MASK:
         DP4( hWDB, "XTYP_MASK" ) ;
      goto MyDefault ;

      case XTYP_SHIFT:
         DP4( hWDB, "XTYP_SHIFT" ) ;
      goto MyDefault ;

MyDefault:
      default:
         DP4( hWDB, "  wType = 0x%04X" ) ;
         #if 0
         if (wFmt != CF_TEXT)
         {
            wsprintf( szLastDdeErr, "Format, %d, is not supported", wFmt ) ;
            DP2( hWDB, "DDE _ERROR: %s", (LPSTR)szLastDdeErr ) ;
            break ;
         }
         #endif

         //
         // What topic index corresponds to hszTopic?  We store
         // this index in i
         //
         if (-1 != (i = GetTopicIndex( hszTopic )))
         {
            if (-1 != (j = GetItemIndex( i, hszItem )))
            {
               //
               // Currently  only supports CF_TEXT as a format, but
               // someday we might want to support other formats.  To
               // allow for this the second parameter to any of the action
               // functions is the format
               //
               // Call the Action function for this topic and item.
               //
               hddeData = (*Topics[i].lpItemList[j].lpfnCallback)
                              ((LPXFERINFO)&dwData2, CF_TEXT ) ;

               //
               // depending on the class of the transaction we either
               // return a handle or NULL.
               //
               switch (wType & XCLASS_MASK)
               {
                  case XCLASS_DATA:
                     DP4( hWDB, "XCLASS_DATA: %d", hddeData ) ;
                     return hddeData ;
                  
                  case XCLASS_FLAGS:
                     DP4( hWDB, "XCLASS_FLAGS: %d", (hddeData ? DDE_FACK : DDE_FNOTPROCESSED ) ) ;
                     return (hddeData ? (HDDEDATA)DDE_FACK : (HDDEDATA)DDE_FNOTPROCESSED ) ;

                  // !!! why does XCLASS_BOOL always return TRUE?  
                  // DDEML sample does this too....
                  case XCLASS_BOOL:
                     DP4( hWDB, "XCLASS_BOOL: TRUE" ) ;
                     return (HDDEDATA)TRUE ;

                  case XCLASS_NOTIFICATION:
                     DP4( hWDB, "XCLASS_NOTIFICATION: NULL") ;
                     return NULL ;

                  default:
                     DP4( hWDB, "XCLASS_default: wType = %d", wType ) ;

               }
            }
         }
         //
         // If we ever get here we either did't have the topic
         // or we need to return NULL
         //
      return NULL ;

   }

   return NULL ;

} /* fnDdeCallback()  */


/**********************************************************************
 **********************************************************************
 **
 ** Helper functions
 **
 **********************************************************************
 *********************************************************************/

/****************************************************************
 *  BOOL NEAR PASCAL GetTopicIndex( HSZ hszTopic )
 *
 *  Description: 
 *
 *    Returns -1 if the topic is bad
 *
 *  Comments:
 *
 ****************************************************************/
UINT NEAR PASCAL GetTopicIndex( HSZ hszTopic )
{
   UINT  i ;

   for (i = 0 ; i < CTOPICS ; i++)
      if (DdeCmpStringHandles( hszTopic, Topics[i].hszTopic) == 0)
         return i ;

   return (UINT)-1 ;

} /* ValidTopic()  */

/****************************************************************
 *  UINT NEAR PASCAL GetItemIndex( UINT wTopic, HSZ hszItem )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
UINT NEAR PASCAL GetItemIndex( UINT wTopic, HSZ hszItem )
{
   UINT           i ;
   UINT           cItems    = Topics[wTopic].cItems ;
   LPITEMLIST     pItemList = Topics[wTopic].lpItemList ;

   for (i = 0 ; i < cItems ; i++)
   {
      if (DdeCmpStringHandles( pItemList[i].hszItem, hszItem) == 0)
         return i ;
   }

   return (UINT)-1 ;

} /* GetItemIndex()  */


/****************************************************************
 *  UINT NEAR PASCAL GetCmdIndex( UINT wTopic, PSTR pszCmd )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
UINT NEAR PASCAL GetCmdIndex( UINT wTopic, PSTR pszCmd )
{
   UINT           i ;
   char           szCmd[128] ;
   UINT           cCmds    = Topics[wTopic].cCmds ;
   LPCMDLIST      lpCmdList = Topics[wTopic].lpCmdList ;

   for (i = 0 ; i < cCmds ; i++)
   {
      DdeQueryString( dwDDEID, lpCmdList[i].hszCmd, szCmd, 127, 0 ) ;

      DP4( hWDB, "szCmd(%s) =?= pszCmd(%s)",
         (LPSTR)szCmd, (LPSTR)pszCmd ) ;

      if (lstrcmpi( szCmd, pszCmd ) == 0)
         return i ;
   }

   return (UINT)-1 ;

} /* GetCmdIndex()  */

/****************************************************************
 *  HDDEDATA NEAR PASCAL DispatchCommands( HSZ hszTopic, HDDEDATA hData )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ****************************************************************/
HDDEDATA NEAR PASCAL DispatchCommands( HSZ hszTopic, HDDEDATA hData )
{
   char     szExec[128] ;
   PSTR     pszCurCmd ;
   HDDEDATA hddeData ;
   UINT     i ;

   DdeGetData( hData, (LPSTR)szExec, 128, 0) ;
   szExec[128-1] = '\0' ;

   DP4( hWDB, "  szExec = %s", (LPSTR)szExec ) ;

   //
   // Start parsing the execute string for commands.
   // Just go from left to right looking for '['s.
   // strtok() works great for this...
   //
   if (pszCurCmd = strtok( szExec, "[" ))
   {
      char  szCmd[128] ;
      char  szCmdKey[128] ;
      PSTR  pszArg1, pszTemp ;
      short n ;
      UINT  j ;

      //
      // For each command do the action function.
      //
      do
      {
         DP4( hWDB, "  pszCurCmd = %s", (LPSTR)pszCurCmd ) ;
         if (pszTemp = strchr( pszCurCmd + 1, ']' ))
         {
            n = pszTemp - pszCurCmd ;
            strncpy( szCmd, pszCurCmd, n ) ;
            szCmd[n] = '\0' ;
         }
         else
         {
            wsprintf( szLastDdeErr, "Missing ']': %s", (LPSTR)pszCurCmd ) ;
            DP2( hWDB, "DDE ERROR: %s", (LPSTR)szLastDdeErr ) ;
            return DDE_FNOTPROCESSED ;
         }

         //
         // pszArg1 points to first char of the argument list
         //
         if (pszArg1 = strchr( szCmd, '(' ))
         {
            //
            // Get the command string itself into szCmdKey
            //
            n = pszArg1 - szCmd  ;
            strncpy( szCmdKey, szCmd, n ) ;
            szCmdKey[n] = '\0' ;
         }
         else
         {
            wsprintf( szLastDdeErr, "Missing '(': %s", (LPSTR)szCmd ) ;
            DP2( hWDB, "DDE ERROR: %s", (LPSTR)szLastDdeErr ) ;
            return DDE_FNOTPROCESSED ;
         }

         //
         // go past '(', and nuke trailing ')' with a '\0'
         //
         pszArg1++ ;
         if (pszTemp = strchr( pszArg1, ')' ))
         {
            *pszTemp = '\0' ;
         }
         else
         {
            wsprintf( szLastDdeErr, "Missing ')': %s", (LPSTR)szCmd ) ;
            DP2( hWDB, "DDE ERROR: %s", (LPSTR)szLastDdeErr ) ;
            return DDE_FNOTPROCESSED ;
         }


         //
         // Find the index of the item...
         //
         if (-1 != (i = GetTopicIndex( hszTopic )))
         {
            if (-1 != (j = GetCmdIndex( i, szCmdKey )))
            {
               DP4( hWDB, "CmdIndex = %d", j ) ;
               //
               // Call the Action function for this Command.
               //
               hddeData = (*Topics[i].lpCmdList[j].lpfnCallback)
                              (hszTopic, (LPSTR)pszArg1, 0 ) ;
            }
            else
            {
               wsprintf( szLastDdeErr, "Command, %s, not supported", (LPSTR)szCmdKey ) ;
               DP2( hWDB, "DDE ERROR: %s", (LPSTR)szLastDdeErr ) ;
               hddeData = DDE_FNOTPROCESSED ;
            }
         }
         else
         {
            wsprintf( szLastDdeErr, "Topic not supported!" ) ;
            DP2( hWDB, "DDE ERROR: %s", (LPSTR)szLastDdeErr ) ;
            hddeData = DDE_FNOTPROCESSED ;
         }


      } while ((hddeData == (HDDEDATA)DDE_FACK) &&
               (pszCurCmd = strtok( NULL, "[" ))) ;
   }

   return hddeData ;

} /* DispatchCommands()  */

/****************************************************************
 *  BOOL WINAPI ddeInitDDEServer( void )
 *
 *  Description: 
 *
 *    Initializee DDEML shit.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI ddeInitDDEServer( void )
{
   UINT     iItem ;
   UINT     iTopic ;
   char     szCmd[MAX_EXEC], szItem[MAX_ITEM] ;

   DP4( hWDB, "ddeInitDDEServer..." ) ;
   if (DdeInitialize( &dwDDEID,
                      (PFNCALLBACK)MakeProcInstance( (FARPROC)fnDdeCallback,
                                                     hinstApp ),
                      APPCLASS_STANDARD, NULL ))
   {
      DP1( hWDB, "DdeInitialize failed!" ) ;

      return FALSE ;
   }
   else
   {
      //
      // Register our service names..
      //
      hszAppName = DdeCreateStringHandle( dwDDEID,
                                          rglpsz[IDS_APPNAME],
                                          NULL ) ;
      DdeNameService( dwDDEID, hszAppName, NULL, DNS_REGISTER ) ;

      for (iTopic = 0 ; iTopic < CTOPICS ; iTopic++)
      {
         switch( iTopic )
         {
            case 0:
              Topics[0].lpItemList = ItemList ;
              Topics[0].cItems = CITEMS ;
              Topics[0].lpCmdList = CmdList ;
              Topics[0].cCmds = CCMDS ;
              Topics[0].wIDS = IDS_DDE_TOPIC_MAIN ;
            break ;

         }


         if (!LoadString( hinstApp, Topics[iTopic].wIDS, szItem, MAX_ITEM ))
         {
            DP1( hWDB, "LoadString failed (Topics[%d].%d)", iItem,
                        Topics[iTopic].wIDS ) ;
            return FALSE ;
         }
         else
            Topics[iTopic].hszTopic = DdeCreateStringHandle( dwDDEID,
                                           szItem, NULL ) ;

         for (iItem = 0 ; iItem < Topics[iTopic].cItems ; iItem++ )
         {
            if (!LoadString( hinstApp, Topics[iTopic].lpItemList[iItem].wIDS, szItem, MAX_ITEM ))
            {
               DP1( hWDB, "LoadString failed (ItemList[%d].%d)", iItem,
                           Topics[iTopic].lpItemList[iItem].wIDS ) ;
               return FALSE ;
            }
            else
               Topics[iTopic].lpItemList[iItem].hszItem = DdeCreateStringHandle( dwDDEID,
                                       szItem, NULL ) ;

            DASSERTMSG( hWDB, Topics[iTopic].lpItemList[iItem].hszItem, "Could not create DDE string handle" ) ;
         }

         for (iItem = 0 ; iItem < Topics[iTopic].cCmds ; iItem++)
         {
            if (!LoadString( hinstApp, Topics[iTopic].lpCmdList[iItem].wIDS, szCmd, MAX_EXEC ))
            {
               DP1( hWDB, "LoadString failed (CmdList[%d].%d)", iItem,
                           Topics[iTopic].lpCmdList[iItem].wIDS ) ;
               return FALSE ;
            }
            else
               Topics[iTopic].lpCmdList[iItem].hszCmd = DdeCreateStringHandle( dwDDEID, szCmd, NULL ) ;

            DASSERTMSG( hWDB, Topics[iTopic].lpCmdList[iItem].hszCmd, "Could not create DDE string handle" ) ;
         }
      }
   }
   return TRUE ;

} /* ddeInitDDEServer()  */

/****************************************************************
 *  BOOL WINAPI ddeUnInitDDEServer( void )
 *
 *  Description: 
 *
 *    Clean up DDE shit.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI ddeUnInitDDEServer( void )
{
   UINT iItem ;
   UINT iTopic ;

   DP3( hWDB, "ddeUnInitDDEServer..." ) ;

   if (dwDDEID)
   {
      // DDE: Un register and close...
      //
      DdeNameService( dwDDEID, NULL, NULL, DNS_UNREGISTER ) ;
      DdeFreeStringHandle( dwDDEID, hszAppName ) ;

      for (iTopic = 0 ; iTopic < CTOPICS ; iTopic++)
      {
         DdeFreeStringHandle( dwDDEID, Topics[iTopic].hszTopic ) ;
         for (iItem = 0 ; iItem < CITEMS ; iItem++ )
         {
            DdeFreeStringHandle( dwDDEID, Topics[iTopic].lpItemList[iItem].hszItem ) ;
         }

         for (iItem = 0 ; iItem < Topics[iTopic].cCmds ; iItem++)
         {
            DdeFreeStringHandle( dwDDEID, Topics[iTopic].lpCmdList[iItem].hszCmd ) ;
         }
      }
      DdeUninitialize( dwDDEID ) ;

      dwDDEID = 0 ;
      return TRUE ;
   }
   else
      return FALSE ;

   return TRUE ;
} /* ddeUnInitDDEServer()  */


/************************************************************************
 * End of File: dde.c
 ***********************************************************************/
