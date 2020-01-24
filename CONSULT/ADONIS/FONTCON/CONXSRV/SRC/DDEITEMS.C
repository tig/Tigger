/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CONXSRV
 *
 *      Module:  ddeitems.c
 *
 *     Remarks:  DDE Poke/Receive items are handled here.
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "CONXSRV.h"
#include "dde.h"
#include "ddeitems.h"


HDDEDATA CALLBACK fnClientHWND( LPXFERINFO lpXferInfo, UINT iFmt ) 
{
   HDDEDATA hData;

   switch (lpXferInfo->wType)
   {
      case XTYP_REQUEST:
      case XTYP_ADVREQ:
         hData = DdeCreateDataHandle( dwDDEID,
                                       (LPWORD)&hwndClientApp,
                                       sizeof(hwndClientApp),
                                       0,
                                       lpXferInfo->hszItem,
                                       lpXferInfo->wFmt,
                                       0 ) ;
      break ;

      case XTYP_ADVSTART:
      return (HDDEDATA)1 ;

      case XTYP_POKE:
      {
         HWND FAR * lpHWND ;

         lpHWND = (HWND FAR *)DdeAccessData( lpXferInfo->hData, NULL ) ;

         if (IsWindow( *lpHWND ))
            hwndClientApp = *lpHWND ;
         else
         {
            DdeUnaccessData( lpXferInfo->hData ) ;
            return (HDDEDATA)DDE_FNOTPROCESSED ;
         }

         DdeUnaccessData( lpXferInfo->hData ) ;
      }
      return (HDDEDATA)DDE_FACK ;

   }
   return 0 ;

} /* fnClientHWND()  */

/****************************************************************
 *  HDDEDATA CALLBACK fnSendData( LPXFERINFO lpXferInfo, UINT iFmt )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
HDDEDATA CALLBACK fnSendData( LPXFERINFO lpXferInfo, UINT iFmt )
{

   switch (lpXferInfo->wType)
   {
      case XTYP_REQUEST:
      case XTYP_ADVREQ:
      case XTYP_ADVSTART:
      return (HDDEDATA)1 ;

      case XTYP_POKE:
      {
         DWORD  i ;
         DWORD cb ;
         LPSTR lpb ;

         lpb = DdeAccessData( lpXferInfo->hData, &cb ) ;

         for ( i = 0 ; i < cb && lpb[i] != '\0' ; i++)
            ProcessTTYCharacter( hwndTTY, lpb[i] ) ;

         DdeUnaccessData( lpXferInfo->hData ) ;
      }
      return (HDDEDATA)DDE_FACK ;

   }
   return 0 ;
} /* fnSendData()  */

/****************************************************************
 *  HDDEDATA CALLBACK fnReceiveData( LPXFERINFO lpXferInfo, UINT iFmt )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
HDDEDATA CALLBACK fnReceiveData( LPXFERINFO lpXferInfo, UINT iFmt )
{
   switch (lpXferInfo->wType)
   {
      case XTYP_REQUEST:
      case XTYP_ADVREQ:
      case XTYP_ADVSTART:
      return (HDDEDATA)1 ;

      case XTYP_POKE:
      return (HDDEDATA)DDE_FNOTPROCESSED ;

   }
   return 0 ;

} /* fnReceiveData()  */

/****************************************************************
 *  HDDEDATA CALLBACK fnStatus( LPXFERINFO lpXferInfo, UINT iFmt )
 *
 *  Description: 
 *
 *    Returns status as it relates to DDE.
 *
 *  Comments:
 *
 ****************************************************************/
HDDEDATA CALLBACK fnSrvStatus( LPXFERINFO lpXferInfo, UINT iFmt )
{
   HDDEDATA hData;
   char     szBuf[256] ;

   switch (lpXferInfo->wType)
   {
      case XTYP_REQUEST:
      case XTYP_ADVREQ:
         if (IsOnline( ))
            LoadString( hinstApp, IDS_DDE_STATUS_CONNECTED, szBuf, 255 ) ;
         else
            LoadString( hinstApp, IDS_DDE_STATUS_READY, szBuf, 255 ) ;

         DP1( hWDB, "CONXSRV status is %s", (LPSTR)szBuf ) ;

         hData = DdeCreateDataHandle( dwDDEID, szBuf,
                                       lstrlen(szBuf)+ 1,
                                       0, lpXferInfo->hszItem,
                                       lpXferInfo->wFmt,
                                       0 ) ;
      return hData ;

      case XTYP_ADVSTART:
      return (HDDEDATA)1 ;
   }
   return 0 ;

} /* fnStatus()  */

HDDEDATA CALLBACK fnMsgReceiveBuf( LPXFERINFO lpXferInfo, UINT iFmt ) 
{
   HDDEDATA hData;

   switch (lpXferInfo->wType)
   {
      case XTYP_REQUEST:
      case XTYP_ADVREQ:
         hData = DdeCreateDataHandle( dwDDEID,
                                       gMsg.lpReceive,
                                       gMsg.cbReceive,
                                       0,
                                       lpXferInfo->hszItem,
                                       lpXferInfo->wFmt,
                                       0 ) ;
         return hData ;
      break ;

      case XTYP_ADVSTART:
      case XTYP_POKE:
      {
         LPVOID lp ;
         DWORD  dwcb ;

         if (gMsg.lpReceive != NULL)
         {
            GlobalFreePtr( gMsg.lpReceive ) ;
            gMsg.lpReceive = NULL ;
         }

         lp = DdeAccessData( lpXferInfo->hData, &dwcb ) ;

         gMsg.lpReceive = GlobalAllocPtr( GHND, dwcb ) ;

         if (gMsg.lpReceive)
            _fmemcpy( gMsg.lpReceive, lp, LOWORD(dwcb) ) ;

         DdeUnaccessData( lpXferInfo->hData ) ;
      }
      return (HDDEDATA)DDE_FACK ;

   }
   return 0 ;

} /* fnMsgReceiveBuf()  */

HDDEDATA CALLBACK fnMsgReceiveSize( LPXFERINFO lpXferInfo, UINT iFmt ) 
{
   switch (lpXferInfo->wType)
   {
      case XTYP_REQUEST:
      case XTYP_ADVREQ:
      case XTYP_ADVSTART:
      return (HDDEDATA)1 ;

      case XTYP_POKE:
      {
         LPDWORD   lpDW ;

         lpDW = (LPDWORD)DdeAccessData( lpXferInfo->hData, NULL ) ;

         if (lpDW)
            gMsg.cbReceive = *lpDW ;

         DdeUnaccessData( lpXferInfo->hData ) ;

      }
      return (HDDEDATA)DDE_FACK ;

   }
   return 0 ;

} /* fnMsgReceiveSize()  */

HDDEDATA CALLBACK fnMsgSendBuf( LPXFERINFO lpXferInfo, UINT iFmt ) 
{
   HDDEDATA hData;

   switch (lpXferInfo->wType)
   {
      case XTYP_REQUEST:
      case XTYP_ADVREQ:
         hData = DdeCreateDataHandle( dwDDEID,
                                       gMsg.lpSend,
                                       gMsg.cbSend,
                                       0,
                                       lpXferInfo->hszItem,
                                       lpXferInfo->wFmt,
                                       0 ) ;
         return hData ;

      case XTYP_ADVSTART:
      return (HDDEDATA)1 ;

      case XTYP_POKE:
      {
         LPVOID lp ;
         DWORD  dwcb ;

         if (gMsg.lpSend != NULL)
         {
            GlobalFreePtr( gMsg.lpSend ) ;
            gMsg.lpSend = NULL ;
         }

         lp = DdeAccessData( lpXferInfo->hData, &dwcb ) ;

         gMsg.lpSend = GlobalAllocPtr( GHND, dwcb ) ;

         if (gMsg.lpSend)
            _fmemcpy( gMsg.lpSend, lp, LOWORD(dwcb) ) ;

         DdeUnaccessData( lpXferInfo->hData ) ;
      }
      return (HDDEDATA)DDE_FACK ;

   }
   return 0 ;
}


HDDEDATA CALLBACK fnMsgSendSize( LPXFERINFO lpXferInfo, UINT iFmt ) 
{

   switch (lpXferInfo->wType)
   {
      case XTYP_REQUEST:
      case XTYP_ADVREQ:
      case XTYP_ADVSTART:
      return (HDDEDATA)1 ;

      case XTYP_POKE:
      {
         LPDWORD   lpDW ;

         lpDW = (LPDWORD)DdeAccessData( lpXferInfo->hData, NULL ) ;

         if (lpDW)
            gMsg.cbSend = *lpDW ;

         DdeUnaccessData( lpXferInfo->hData ) ;
      }
      return (HDDEDATA)DDE_FACK ;

   }
   return 0 ;

} /* fnMsgSendSize()  */

HDDEDATA CALLBACK fnMsgFlags( LPXFERINFO lpXferInfo, UINT iFmt ) 
{
   HDDEDATA hData;

   switch (lpXferInfo->wType)
   {
      case XTYP_REQUEST:
      case XTYP_ADVREQ:
         hData = DdeCreateDataHandle( dwDDEID,
                                       &gMsg.SendMsg,
                                       sizeof(gMsg.SendMsg),
                                       0,
                                       lpXferInfo->hszItem,
                                       lpXferInfo->wFmt,
                                       0 ) ;
      break ;

      case XTYP_ADVSTART:
      return (HDDEDATA)1 ;

      case XTYP_POKE:
      {
         LPWORD   lpW ;

         lpW = (LPWORD)DdeAccessData( lpXferInfo->hData, NULL ) ;

         if (lpW)
            gMsg.wFlags = *lpW ;

         DdeUnaccessData( lpXferInfo->hData ) ;
      }
      return (HDDEDATA)DDE_FACK ;

   }
   return 0 ;

} /* fnMsgFlags()  */

HDDEDATA CALLBACK fnMsg( LPXFERINFO lpXferInfo, UINT iFmt ) 
{
   HDDEDATA hData;

   switch (lpXferInfo->wType)
   {
      case XTYP_REQUEST:
      case XTYP_ADVREQ:
         hData = DdeCreateDataHandle( dwDDEID,
                                       &gMsg.wFlags,
                                       sizeof(gMsg.wFlags),
                                       0,
                                       lpXferInfo->hszItem,
                                       lpXferInfo->wFmt,
                                       0 ) ;
      break ;

      case XTYP_ADVSTART:
      return (HDDEDATA)1 ;

      case XTYP_POKE:
      {
         LPBYTE   lpB ;

         lpB = (LPBYTE)DdeAccessData( lpXferInfo->hData, NULL ) ;

         if (lpB)
            gMsg.SendMsg = *lpB ;

         DdeUnaccessData( lpXferInfo->hData ) ;
      }
      return (HDDEDATA)DDE_FACK ;

   }
   return 0 ;

} /* fnMsg()  */


/***************************** Private Function ****************************\
* This passes out a standard tab-delimited list of topic names for this
* application.
*
* This support is required for other apps to be able to
* find out about us.  This kind of support should be in every DDE
* application.
*
\***************************************************************************/
HDDEDATA CALLBACK fnTopicListXfer( LPXFERINFO lpXferInfo, UINT iFmt)
{
   #if 0

   UINT cbAlloc ;
   LPSTR pszTopicList;

   HDDEDATA hData;
   
   DP4( hWDB, "fnTopicListXfer" ) ;

   if (lpXferInfo->wType == XTYP_ADVSTART)
      return (HDDEDATA)TRUE ;

   if (lpXferInfo->wType != XTYP_REQUEST && lpXferInfo->wType != XTYP_ADVREQ)
      return (HDDEDATA)0 ;

   /*
   * construct the list of topics we have
   */
   cbAlloc = lstrlen( rglpsz[IDS_APPNAME] ) + 1 ;

   // allocate a data handle big enough for the list.
   hData = DdeCreateDataHandle(dwDDEID, NULL, 0, cbAlloc, lpXferInfo->hszItem,
      lpXferInfo->wFmt, 0);
   pszTopicList = (LPSTR)DdeAccessData(hData, NULL);
   lstrcpy(pszTopicList, rglpsz[IDS_APPNAME]);

   DdeUnaccessData(hData);

   #endif
   return 0 ;
}




/***************************** Private Function ****************************\
* This passes out a standard tab-delimited list of item names for the
* specified topic.
*
* This support is required for other apps to be able to
* find out about us.  This kind of support should be in every DDE
* application.
*
\***************************************************************************/
HDDEDATA CALLBACK fnItemListXfer( LPXFERINFO lpXferInfo, UINT iFmt )
{
   #if 0
   UINT     cbAlloc, i, iItem ;
   LPSTR    pszItemList;
   HDDEDATA hData;
   char     szItem[MAX_ITEM] ;

   if (lpXferInfo->wType == XTYP_ADVSTART)
      return (HDDEDATA)TRUE ;

   if (lpXferInfo->wType != XTYP_REQUEST &&
       lpXferInfo->wType != XTYP_ADVREQ)
     return 0 ;

   /*
   * construct the list of items we support for this topic - this supports
   * more than the minimum standard which would support SysItems only on
   * the system topic.
   */

   cbAlloc = 0;
   for (iItem = 0; iItem < CITEMS; iItem++)
   {
      DdeQueryString( dwDDEID, ItemList[iItem].hszItem, szItem, MAX_ITEM, 0 ) ;

      cbAlloc += lstrlen( szItem ) + 1; // 1 for tab
   }
   // allocate a data handle big enough for the list.
   hData = DdeCreateDataHandle(dwDDEID, NULL, 0, cbAlloc, lpXferInfo->hszItem,
                                 lpXferInfo->wFmt, 0);
   pszItemList = (LPSTR)DdeAccessData(hData, NULL);

   if (pszItemList)
   {
      for (i = 0; i < CITEMS; i++)
      {
         DdeQueryString( dwDDEID, ItemList[i].hszItem, szItem, MAX_ITEM, 0 ) ;
         lstrcpy(pszItemList, szItem);
         pszItemList += lstrlen( szItem ) ;
         *pszItemList++ = '\t';
      }

      *--pszItemList = '\0';
      DdeUnaccessData(hData);
      return hData ;
   }

   #endif

   return 0 ;

}


/***************************** Private Function ****************************\
* Gives out a 0 terminated array of dde format numbers supported by this app.
*
* This support is required for other apps to be able to
* find out about us.  This kind of support should be in every DDE
* application.
*
\***************************************************************************/
HDDEDATA CALLBACK fnSysFormatsXfer( LPXFERINFO lpXferInfo, UINT iFmt )
{
   #if 0
   int i, cb;
   LPSTR psz, pszT;
   HDDEDATA hData;

   if (lpXferInfo->wType == XTYP_ADVSTART)
      return (HDDEDATA)TRUE ;

   if (lpXferInfo->wType != XTYP_REQUEST &&
       lpXferInfo->wType != XTYP_ADVREQ) 
      return 0 ;

   for (i = 0, cb = 0; i < CFORMATS; i++) 
      cb += lstrlen(aFormats[i].sz) + 1;
   
   hData = DdeCreateDataHandle(dwDDEID, NULL, (DWORD)cb,
                               0L, lpXferInfo->hszItem, lpXferInfo->wFmt, 0);
   psz = pszT = DdeAccessData(hData, NULL);

   for (i = 0; i < CFORMATS; i++)
   {
      lstrcpy(pszT, aFormats[i].sz);
      pszT += lstrlen(pszT);
      *pszT++ = '\t';
   }
   *(--pszT) = '\0';

   DdeUnaccessData(hData);

   return hData ;
   #endif
   return 0 ;
}



HDDEDATA CALLBACK fnHelpXfer( LPXFERINFO lpXferInfo, UINT iFmt )
{
   HDDEDATA hData;

   switch (lpXferInfo->wType)
   {
      case XTYP_REQUEST:
      case XTYP_ADVREQ:
         hData = DdeCreateDataHandle( dwDDEID, rglpsz[IDS_APPTITLE],
                                      lstrlen(rglpsz[IDS_APPTITLE]) + 1,
                                      0, lpXferInfo->hszItem,
                                      lpXferInfo->wFmt,
                                      0 ) ;
      return hData ;

      case XTYP_ADVSTART:
      return (HDDEDATA)1 ;
   }
   return 0 ;
}

/****************************************************************
 *  HDDEDATA CALLBACK fnGetLastErr(  LPXFERINFO lpXferInfo, UINT iFmt ) ;
 *
 *  Description: 
 *
 *    Returns the last error message
 *
 *  Comments:
 *
 ****************************************************************/
HDDEDATA CALLBACK fnGetLastErr(        LPXFERINFO lpXferInfo, UINT iFmt ) 
{
   HDDEDATA hData;

   switch (lpXferInfo->wType)
   {
      case XTYP_REQUEST:
      case XTYP_ADVREQ:
         hData = DdeCreateDataHandle( dwDDEID, szLastDdeErr,
                                      lstrlen(szLastDdeErr) + 1,
                                      0, lpXferInfo->hszItem,
                                      lpXferInfo->wFmt,
                                      0 ) ;
      return hData ;

      case XTYP_ADVSTART:
      return (HDDEDATA)1 ;
   }
   return 0 ;

} /* fnGetLastErr()  */


/************************************************************************
 * End of File: ddeitems.c
 ***********************************************************************/

