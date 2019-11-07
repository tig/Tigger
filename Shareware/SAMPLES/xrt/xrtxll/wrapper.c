/***************************************************************************\

   PROGRAM     : wrapper.c

   PURPOSE     : This is not a full program but a module you can include
                 in your code.  It implements a standard DDEML callback
                 function that allows you to have most of your DDE table
                 driven.  The default callback function handles all basic
                 System Topic information based on the tables you give
                 to this app.

   LIMITATIONS : This only supports servers that:
                 have only one service name
                 have enumerable topics and items
                 do not change the topics or items they support over time.


   EXPORTED ROUTINES:

    InitializeDDE()
        Use this to initialize the callback function tables and the DDEML

    UninitializeDDE()
        Use this to cleanup this module and uninitialize the DDEML instance.

\***************************************************************************/

#include "precomp.h"
#include <string.h>
#include "wrapper.h"


VOID InitHszs( LPDDESERVICETBL psi);
UINT GetFormat(LPTSTR pszFormat);
VOID FreeHszs( LPDDESERVICETBL psi);
HDDEDATA EXPORT EXPENTRY fnWrapperCallback(UINT wType, UINT wFmt, HCONV hConv, HSZ hsz1,
        HSZ hsz2, HDDEDATA hData, DWORD dwData1, DWORD dwData2);
BOOL DoCallback( HSZ hszTopic, HSZ hszItem, UINT wFmt, UINT wType,
        HDDEDATA hDataIn, LPDDESERVICETBL psi, HDDEDATA *phDataRet);
HDDEDATA ReqItems(HDDEDATA hDataOut, LPDDETOPICTBL ptpc);
HDDEDATA AddReqFormat(HDDEDATA hDataOut, LPTSTR pszFmt);
HDDEDATA ReqFormats(HDDEDATA hDataOut, LPDDETOPICTBL ptpc);
HDDEDATA DoWildConnect( HSZ hszTopic);

static DWORD idInst;    

static LPDDESERVICETBL pasi;

TCHAR tab[] = TEXT("\t");

#define FOR_EACH_TOPIC(psvc, ptpc, i)  for (i = 0, ptpc=(psvc)->topic; i < (int)(psvc)->cTopics; i++, ptpc++)
#define FOR_EACH_ITEM(ptpc, pitm, i)   for (i = 0, pitm=(ptpc)->item;  i < (int)(ptpc)->cItems;  i++, pitm++)
#define FOR_EACH_FORMAT(pitm, pfmt, i) for (i = 0, pfmt=(pitm)->fmt;   i < (int)(pitm)->cFormats;i++, pfmt++)


/*     STANDARD PREDEFINED FORMATS     */

#define CSTDFMTS    9
struct {
    UINT wFmt;
    LPTSTR pszFmt;
} StdFmts[CSTDFMTS] = {
    {   CF_TEXT        ,  TEXT("TEXT")          } ,
    {   CF_BITMAP      ,  TEXT("BITMAP")        } ,
    {   CF_METAFILEPICT,  TEXT("METAFILEPICT")  } ,
    {   CF_SYLK        ,  TEXT("SYLK")          } ,
    {   CF_DIF         ,  TEXT("DIF")           } ,
    {   CF_TIFF        ,  TEXT("TIFF")          } ,
    {   CF_OEMTEXT     ,  TEXT("OEMTEXT")       } ,
    {   CF_DIB         ,  TEXT("DIB")           } ,
    {   CF_PALETTE     ,  TEXT("PALETTE")       } ,
};



HDDEDATA WINAPI SysReqTopics(HDDEDATA hDataOut);
HDDEDATA WINAPI SysReqSysItems(HDDEDATA hDataOut);
HDDEDATA WINAPI SysReqFormats(HDDEDATA hDataOut);

       /*      STANDARD SERVICE INFO TABLES        */

DDEFORMATTBL StdSvcSystopicTopicsFormats[] = {
    TEXT("TEXT"), 0, 0, NULL, SysReqTopics
};

DDEFORMATTBL StdSvcSystopicSysitemsFormats[] = {
    TEXT("TEXT"), 0, 0, NULL, SysReqSysItems
};

DDEFORMATTBL StdSvcSystopicFormatsFormats[] = {
    TEXT("TEXT"), 0, 0, NULL, SysReqFormats
};

#define ITPC_TOPICS     0
#define ITPC_SYSITEMS   1
#define ITPC_FORMATS    2
#define ITPC_ITEMLIST   3

#define ITPC_COUNT      4

DDEITEMTBL StdSvcSystopicItems[] = {
    { SZDDESYS_ITEM_TOPICS,   0, NULL, 1, 0, StdSvcSystopicTopicsFormats   },
    { SZDDESYS_ITEM_SYSITEMS, 0, NULL, 1, 0, StdSvcSystopicSysitemsFormats },
    { SZDDESYS_ITEM_FORMATS,  0, NULL, 1, 0, StdSvcSystopicFormatsFormats  },
    { SZDDE_ITEM_ITEMLIST,    0, NULL, 1, 0, StdSvcSystopicSysitemsFormats },
};

DDETOPICTBL StdSvc[] = {
    SZDDESYS_TOPIC, 0, ITPC_COUNT, 0, StdSvcSystopicItems
};

DDESERVICETBL SSI = {
    NULL, 0, 1, 0, StdSvc
};

/*********************************************************************/

UINT WINAPI InitializeDDE( LPDWORD pidInst, LPDDESERVICETBL AppSvcInfo,
                            DWORD dwFilterFlags, HANDLE hInst )
{   
    UINT uiRet ;
    
    uiRet = DdeInitialize( pidInst, fnWrapperCallback, dwFilterFlags, 0 ) ;
    
    if (uiRet != DMLERR_NO_ERROR)
    {
        return uiRet ;
    }
    
    idInst = *pidInst ;
    
    InitHszs( AppSvcInfo);
    InitHszs( &SSI);
    pasi = AppSvcInfo;
     
    DdeNameService( idInst, pasi->hszService, 0, DNS_REGISTER);
    return uiRet ;
}



VOID InitHszs( LPDDESERVICETBL psi)
{
    int iTopic, iItem, iFmt;
    LPDDETOPICTBL ptpc;
    LPDDEITEMTBL pitm;
    LPDDEFORMATTBL pfmt;

    if (psi->pszService) {
        psi->hszService = DdeCreateStringHandle(idInst, psi->pszService, 0);
    }
    FOR_EACH_TOPIC(psi, ptpc, iTopic) {
        ptpc->hszTopic = DdeCreateStringHandle(idInst, ptpc->pszTopic, 0);
        FOR_EACH_ITEM(ptpc, pitm, iItem) {
            pitm->hszItem = DdeCreateStringHandle(idInst, pitm->pszItem, 0);
            FOR_EACH_FORMAT(pitm, pfmt, iFmt) {
                pfmt->wFmt = GetFormat(pfmt->pszFormat);
            }
        }
    }
}


/*
 * This function allows apps to use standard CF_ formats.  The string
 * given may be in the StdFmts[] table.
 */

UINT GetFormat(LPTSTR pszFormat)
{
    int iFmt;

    for (iFmt = 0; iFmt < CSTDFMTS; iFmt++) {
        if (!lstrcmp(pszFormat, StdFmts[iFmt].pszFmt)) {
            return(StdFmts[iFmt].wFmt);
        }
    }
    return(RegisterClipboardFormat(pszFormat));
}



VOID WINAPI UninitializeDDE( )
{
    DdeNameService( idInst, pasi->hszService, 0, DNS_UNREGISTER);
    FreeHszs( pasi);
    FreeHszs( &SSI);
};

VOID WINAPI ReallyUninitializeDDE()
{
    #ifdef _DEBUG
    OutputDebugString( TEXT("Calling DdeUninitialize...\r\n") ) ;
    #endif
    DdeUninitialize( idInst );
}

VOID FreeHszs( LPDDESERVICETBL psi)
{
    int iTopic, iItem;
    LPDDETOPICTBL ptpc;
    LPDDEITEMTBL pitm;

    DdeFreeStringHandle(idInst, psi->hszService);
    FOR_EACH_TOPIC(psi, ptpc, iTopic) {
        DdeFreeStringHandle(idInst, ptpc->hszTopic);
        FOR_EACH_ITEM(ptpc, pitm, iItem) {
            DdeFreeStringHandle(idInst, pitm->hszItem);
        }
    }
}



HDDEDATA EXPORT EXPENTRY fnWrapperCallback( UINT wType, UINT wFmt, HCONV hConv, HSZ hsz1, HSZ hsz2,
                                              HDDEDATA hData, DWORD dwData1, DWORD dwData2 )
{   
    switch (wType) 
    {
        case XTYP_WILDCONNECT:
            if (!hsz2 || !DdeCmpStringHandles(hsz2, pasi->hszService)) 
            {
                return(DoWildConnect( hsz1));
            }
        break;   
        
    #ifdef _DEBUG
        case XTYP_ADVSTART:
            OutputDebugString( TEXT("XTYP_ADVSTART\r\n") ) ;
            goto doit ;            
        break ;
        case XTYP_CONNECT:
            OutputDebugString( TEXT("XTYP_CONNECT\r\n") ) ;
            goto doit ;            
        break ;
        case XTYP_EXECUTE:
            OutputDebugString( TEXT("XTYP_EXECUTE\r\n") ) ;
            goto doit ;            
        break ;
        case XTYP_REQUEST:
            OutputDebugString( TEXT("XTYP_REQUEST\r\n") ) ;
            goto doit ;            
        break ;
        case XTYP_ADVREQ:
            OutputDebugString( TEXT("XTYP_ADVREQ\r\n") ) ;
            goto doit ;            
        break ;
        case XTYP_ADVDATA:
            OutputDebugString( TEXT("XTYP_ADVDATA\r\n") ) ;
            goto doit ;            
        break ;
        
        case XTYP_POKE:
            OutputDebugString( TEXT("XTYP_POKE\r\n") ) ;
doit:        
    #else
        case XTYP_ADVSTART:
        case XTYP_CONNECT:
        case XTYP_EXECUTE:
        case XTYP_REQUEST:
        case XTYP_ADVREQ:
        case XTYP_ADVDATA:
        case XTYP_POKE:
    #endif //_DEBUG
        {
            static HDDEDATA hDataRet1;
            static HDDEDATA hDataRet2;
        
            if(DoCallback(hsz1, hsz2, wFmt, wType, hData,
                    (LPDDESERVICETBL)&SSI, &hDataRet1))
                return(hDataRet1);
    
            if (DoCallback(hsz1, hsz2, wFmt, wType, hData,
                    (LPDDESERVICETBL)pasi, &hDataRet2))
                return(hDataRet2);
        }
            /* Fall Through */
        default:
        break ;
    }
    return 0 ;
}




BOOL DoCallback(
HSZ hszTopic,
HSZ hszItem,
UINT wFmt,
UINT wType,
HDDEDATA hDataIn,
LPDDESERVICETBL psi,
HDDEDATA *phDataRet)
{
    int iTopic, iItem, iFmt;
    LPDDEFORMATTBL pfmt;
    LPDDEITEMTBL pitm;
    LPDDETOPICTBL ptpc;

    FOR_EACH_TOPIC(psi, ptpc, iTopic) {
        if (DdeCmpStringHandles(ptpc->hszTopic, hszTopic))
            continue;

        if (wType == XTYP_CONNECT) {
            *phDataRet = (HDDEDATA)TRUE;
            return(TRUE);
        }

        FOR_EACH_ITEM(ptpc, pitm, iItem) {
            if (DdeCmpStringHandles(pitm->hszItem, hszItem))
                continue;

            if (wType == XTYP_EXECUTE) {
                if (pitm->lpfnExecute) {
                    if ((*pitm->lpfnExecute)(hDataIn))
                        *phDataRet = (HDDEDATA)DDE_FACK;
                } else {
                    *phDataRet = (HDDEDATA)DDE_FNOTPROCESSED;
                }
                return(TRUE);
            }

            FOR_EACH_FORMAT(pitm, pfmt, iFmt) {
                if (pfmt->wFmt != wFmt)
                    continue;

                switch (wType) {
                case XTYP_ADVSTART:
                    *phDataRet = (HDDEDATA)TRUE;
                    break;

                case XTYP_ADVDATA:
                case XTYP_POKE:
                    if (pfmt->lpfnPoke) {
                        if ((*pfmt->lpfnPoke)(hDataIn)) {
                            *phDataRet = (HDDEDATA)DDE_FACK;
                            DdePostAdvise(idInst, hszTopic, hszItem);
                        }
                    } else {
                        *phDataRet = (HDDEDATA)DDE_FNOTPROCESSED;
                    }
                    break;

                case XTYP_REQUEST:
                case XTYP_ADVREQ:
                    if (pfmt->lpfnRequest) {
                        HDDEDATA hDataOut;

                        hDataOut = DdeCreateDataHandle(idInst, NULL, 0, 0, pitm->hszItem,
                                pfmt->wFmt, 0);
                        *phDataRet = (HDDEDATA)(*pfmt->lpfnRequest)(hDataOut);
                        if (!*phDataRet) {
                            DdeFreeDataHandle(hDataOut);
                        }
                    } else {
                        *phDataRet = 0;
                    }
                    break;
                }
                return(TRUE);
            }
        }

        /* item not found in tables */

        if (wFmt == CF_TEXT && (wType == XTYP_REQUEST || wType == XTYP_ADVREQ)) {
            /*
             * If formats item was requested and not found in the tables,
             * return a list of formats supported under this topic.
             */
            if (!DdeCmpStringHandles(hszItem, SSI.topic[0].item[ITPC_FORMATS].hszItem)) {
                *phDataRet = DdeCreateDataHandle(idInst, NULL, 0, 0, hszItem, wFmt, 0);
                *phDataRet = ReqFormats(*phDataRet, ptpc);
                return(TRUE);
            }
            /*
             * If sysitems or topicitemlist item was requested and not found,
             * return a list of items supported under this topic.
             */
            if (!DdeCmpStringHandles(hszItem, SSI.topic[0].item[ITPC_SYSITEMS].hszItem) ||
                !DdeCmpStringHandles(hszItem, SSI.topic[0].item[ITPC_ITEMLIST].hszItem)) {
                *phDataRet = ReqItems(DdeCreateDataHandle(idInst, NULL, 0, 0, hszItem, wFmt, 0), ptpc);
                return(TRUE);
            }
        }
    }

    /* no topics fit */

    return(FALSE);
}


/*
 * These are Request routines for supporting the system topic.
 * Their behavior depends on the table contents.
 */

HDDEDATA WINAPI SysReqTopics(
HDDEDATA hDataOut)         // data handle to add output data to.
{
    int iTopic, cb, cbOff;
    LPDDETOPICTBL ptpc;

    /*
     * This code assumes SSI only contains the system topic.
     */

    cbOff = 0;
    FOR_EACH_TOPIC(pasi, ptpc, iTopic) {
        if (!DdeCmpStringHandles(ptpc->hszTopic, SSI.topic[0].hszTopic)) 
        {
            continue;       // don't add systopic twice.
        }
        cb = lstrlen(ptpc->pszTopic) * sizeof(TCHAR);
        hDataOut = DdeAddData(hDataOut, (LPBYTE)ptpc->pszTopic, (DWORD)cb, (DWORD)cbOff);
        cbOff += cb;
        hDataOut = DdeAddData(hDataOut, (LPBYTE)&tab, (DWORD)sizeof(TCHAR), (DWORD)cbOff);
        cbOff += sizeof(TCHAR);
    }

    hDataOut = DdeAddData(hDataOut, (LPBYTE)SSI.topic[0].pszTopic,
            (DWORD)(lstrlen(SSI.topic[0].pszTopic) + 1)*sizeof(TCHAR), (DWORD)cbOff );
    return(hDataOut);
}



HDDEDATA WINAPI SysReqSysItems(
HDDEDATA hDataOut)
{
    return(ReqItems(hDataOut, &SSI.topic[ITPC_SYSITEMS]));
}


/*
 * Given a topic table, this function returns a tab delimited list of
 * items supported under that topic.
 */
HDDEDATA ReqItems(
HDDEDATA hDataOut,
LPDDETOPICTBL ptpc)
{
    int cb, iItem, cbOff = 0;
    LPDDEITEMTBL pitm;

    /*
     * return a list of all the items within this topic
     */
    FOR_EACH_ITEM(ptpc, pitm, iItem) 
    {
        cb = lstrlen(pitm->pszItem) * sizeof(TCHAR);
        hDataOut = DdeAddData(hDataOut, (LPBYTE)pitm->pszItem, (DWORD)cb, (DWORD)cbOff);
        cbOff += cb;
        hDataOut = DdeAddData(hDataOut, (LPBYTE)&tab, (DWORD)sizeof(TCHAR), (DWORD)cbOff);
        cbOff += sizeof(TCHAR);
    }


    /*
     * if this is for the System Topic, add to the list our default items.
     */

    if (!DdeCmpStringHandles(ptpc->hszTopic, SSI.topic[0].hszTopic)) 
    {
        ptpc = &SSI.topic[0];
        FOR_EACH_ITEM(ptpc, pitm, iItem) 
        {
            cb = lstrlen(pitm->pszItem) * sizeof(TCHAR) ;
            hDataOut = DdeAddData(hDataOut, (LPBYTE)pitm->pszItem, (DWORD)cb, (DWORD)cbOff);
            cbOff += cb;
            hDataOut = DdeAddData(hDataOut, (LPBYTE)&tab, (DWORD)sizeof(TCHAR), (DWORD)cbOff);
            cbOff += sizeof(TCHAR);
        }
    }
    else 
    {
        /*
         * Add the standard TopicListItems and SysItem items.
         */
        cb = lstrlen(SSI.topic[0].item[ITPC_SYSITEMS].pszItem) * sizeof(TCHAR);
        hDataOut = DdeAddData(hDataOut,
            (LPBYTE)SSI.topic[0].item[ITPC_SYSITEMS].pszItem, (DWORD)cb, (DWORD)cbOff);
        cbOff += cb;
        hDataOut = DdeAddData(hDataOut, (LPBYTE)&tab, (DWORD)sizeof(TCHAR), (DWORD)cbOff);
        cbOff += sizeof(TCHAR);

        cb = lstrlen(SSI.topic[0].item[ITPC_ITEMLIST].pszItem) * sizeof(TCHAR);
        hDataOut = DdeAddData(hDataOut,
            (LPBYTE)SSI.topic[0].item[ITPC_ITEMLIST].pszItem, (DWORD)cb, (DWORD)cbOff);
        cbOff += cb;
        hDataOut = DdeAddData(hDataOut, (LPBYTE)&tab, (DWORD)sizeof(TCHAR), (DWORD)cbOff);
        cbOff += sizeof(TCHAR);

        cb = lstrlen(SSI.topic[0].item[ITPC_FORMATS].pszItem) * sizeof(TCHAR);
        hDataOut = DdeAddData(hDataOut,
            (LPBYTE)SSI.topic[0].item[ITPC_FORMATS].pszItem, (DWORD)cb, (DWORD)cbOff);
        cbOff += cb;
        hDataOut = DdeAddData(hDataOut, (LPBYTE)&tab, (DWORD)sizeof(TCHAR), (DWORD)cbOff);
        cbOff += sizeof(TCHAR);

    }

    cbOff -= sizeof(TCHAR) ;
    hDataOut = DdeAddData(hDataOut, (LPBYTE)TEXT("\0"), (DWORD)sizeof(TCHAR), (DWORD)cbOff);

    return(hDataOut);
}




HDDEDATA WINAPI SysReqFormats(
HDDEDATA hDataOut)
{
    int iTopic, iItem, iFmt;
    LPDDETOPICTBL ptpc;
    LPDDEITEMTBL pitm;
    LPDDEFORMATTBL pfmt;

    TCHAR  sz[] = TEXT("TEXT") ;
    hDataOut = DdeAddData(hDataOut, (LPBYTE)sz, lstrlen(sz)*sizeof(TCHAR), 0);
    FOR_EACH_TOPIC(pasi, ptpc, iTopic) 
    {
        FOR_EACH_ITEM(ptpc, pitm, iItem) 
        {
            FOR_EACH_FORMAT(pitm, pfmt, iFmt) 
            {
                hDataOut = AddReqFormat(hDataOut, pfmt->pszFormat);
            }
        }
    }
    return(hDataOut);
}



HDDEDATA AddReqFormat(
HDDEDATA hDataOut,
LPTSTR pszFmt)
{
    LPTSTR pszList;
    DWORD cbOff;

    pszList = (LPTSTR)DdeAccessData(hDataOut, NULL);
    #ifdef WIN32
    if (wcsstr(pszList, pszFmt) == NULL)
    #else
    if (_fstrstr(pszList, pszFmt) == NULL) 
    #endif
    {
        cbOff = lstrlen(pszList)*sizeof(TCHAR);
        DdeUnaccessData(hDataOut);
        hDataOut = DdeAddData(hDataOut, (LPBYTE)&tab, sizeof(TCHAR), cbOff);
        cbOff += sizeof(TCHAR) ;
        hDataOut = DdeAddData(hDataOut, (LPBYTE)pszFmt, (lstrlen(pszFmt) + 1)*sizeof(TCHAR), cbOff);
    } 
    else
    {
        DdeUnaccessData(hDataOut);
    }

    return(hDataOut);
}


HDDEDATA ReqFormats(
HDDEDATA hDataOut,
LPDDETOPICTBL ptpc)
{
    int iItem, iFmt;
    LPDDEITEMTBL pitm;
    LPDDEFORMATTBL pfmt;

    hDataOut = DdeAddData(hDataOut, (LPBYTE)TEXT(""), sizeof(TCHAR), 0) ;
    FOR_EACH_ITEM(ptpc, pitm, iItem) 
    {
        FOR_EACH_FORMAT(pitm, pfmt, iFmt) 
        {
            hDataOut = AddReqFormat(hDataOut, pfmt->pszFormat);
        }
    }
    return(hDataOut);
}



HDDEDATA DoWildConnect( HSZ hszTopic)
{
    LPDDETOPICTBL ptpc;
    HDDEDATA hData;
    PHSZPAIR pHszPair;
    int iTopic, cTopics = 2;

    if (!hszTopic) 
    {
        cTopics += pasi->cTopics;
    }

    hData = DdeCreateDataHandle(idInst, NULL, cTopics * sizeof(HSZPAIR), 0, (HSZ)NULL, 0, 0);
    pHszPair = (HSZPAIR FAR *)DdeAccessData(hData, NULL);
    pHszPair->hszSvc = pasi->hszService;
    pHszPair->hszTopic = SSI.topic[0].hszTopic;  // always support systopic.
    pHszPair++;
    ptpc = &pasi->topic[0];
    FOR_EACH_TOPIC(pasi, ptpc, iTopic) 
    {
        if (hszTopic && DdeCmpStringHandles(hszTopic, ptpc->hszTopic)) 
        {
            continue;
        }
        if (!DdeCmpStringHandles(ptpc->hszTopic, SSI.topic[0].hszTopic)) 
        {
            continue;       // don't enter systopic twice.
        }
        pHszPair->hszSvc = pasi->hszService;
        pHszPair->hszTopic = ptpc->hszTopic;
        pHszPair++;
    }
    pHszPair->hszSvc = 0;
    pHszPair->hszTopic = 0;
    DdeUnaccessData(hData);
    return(hData);
}
