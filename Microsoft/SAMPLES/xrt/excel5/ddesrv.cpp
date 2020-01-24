// ddesrv.cpp
//
// Excel has no documented way of being notified that cell data has changed
// beyond what is provided by the =SERVICE|TOPIC!ITEM synax.
//
// But when the XRTXLL gets an IAdviseSink::OnDataChange notification, it
// needs some way of telling XL that the data has changed.   We do that
// by having a =XRTXLL|DATA!ID entry on the sheet somewhere
// and all cells containing =XRTQuote() refer to this cell as the first
// parameter to XRTQuote().
//

#include "precomp.h"
#include "wrapper.h"
#include "fn.h"
#include "ddesrv.h"
                   
#include "debug.h"
ASSERTDATA

// prototype for functions we pass to wrapper.c functions
// none are required for but they are implented anyway to
// make wrapper.c happy.
// 
HDDEDATA WINAPI RequestDDEData( HDDEDATA hData ) ;

DDEFORMATTBL DDEFormats[] =
{
    { "TEXT",   CF_TEXT,    0, NULL, RequestDDEData },
    { "Dummy1",   CF_TEXT,    0, NULL, RequestDDEData },
    { "Dummy2",   CF_TEXT,    0, NULL, RequestDDEData }
} ;

DDEITEMTBL DDEItems[] = 
{
    { "ID", 0, NULL, sizeof(DDEFormats)/sizeof(DDEFormats[0]), 0, DDEFormats }
} ;

DDETOPICTBL DDETopics[] =
{
    { "Data", NULL, sizeof(DDEItems)/sizeof(DDEItems[0]), 0, DDEItems }
} ;

DDESERVICETBL ServiceInfo[] =
{
    { "XRTXLL", NULL, sizeof(DDETopics)/sizeof(DDETopics[0]), 0, DDETopics }
} ;

DWORD   g_idInst = 0L ; 
BOOL    g_fServerCreated = FALSE ;

BOOL WINAPI SetupDDEServer() 
{                                 
    if (g_fServerCreated == TRUE)
        return TRUE ;
        
    UINT uiErr = InitializeDDE( &g_idInst, ServiceInfo, 
                    CBF_FAIL_EXECUTES | 
                    CBF_FAIL_POKES | 
                    CBF_SKIP_ALLNOTIFICATIONS, 
                    g_hinst ) ;
                    
    if (uiErr != DMLERR_NO_ERROR)   
    {
        char sz[128] ;
        wsprintf( sz, "InitializeDDE Failed!  %#04x", uiErr ) ;
        AssertSz(0, sz ) ;
        g_fServerCreated = FALSE ;
    }                    
    else
    {         
        #ifdef _DEBUG
        char sz[128] ;
        wsprintf( sz, "InitializeDDE Succeeded.  g_idInst == %#08lx\r\n", (DWORD)g_idInst ) ;
        OutputDebugString( sz ) ;
        #endif

        g_fServerCreated = TRUE ;
    }
    
    return g_fServerCreated ;
}

BOOL WINAPI KillDDEServer() 
{
    if (g_fServerCreated == FALSE)
    {
        #ifdef _DEBUG
        OutputDebugString( "KillDDEServer called when server was never created!\r\n" ) ;
        #endif
        return FALSE ;
    }
        
    #ifdef _DEBUG
    OutputDebugString( "Uninitializing DDE server....\r\n" ) ;
    #endif
    
    UninitializeDDE( ) ;
    
    #ifdef _DEBUG
    OutputDebugString( "Uninitialized DDE server.\r\n" ) ;
    #endif
    
    g_idInst = 0 ;
    g_fServerCreated = FALSE ;
    return TRUE ;
}

void WINAPI PostDDEServer() 
{
    if (g_fServerCreated == FALSE)
    {
        #ifdef _DEBUG
        OutputDebugString( "PostDDEServer when g_fServerCreated == FALSE \r\n" ) ;
        #endif
        return ;
    }
        
    if (DdePostAdvise( g_idInst, ServiceInfo->topic[0].hszTopic,
                        ServiceInfo->topic[0].item[0].hszItem ) == 0)
    {
        #ifdef _DEBUG
        char sz[128] ;
        wsprintf( sz, "DdePostAdvise failed:  %#04x\r\n", DdeGetLastError( g_idInst ) ) ;                        
        OutputDebugString( sz) ;
        #endif
    }
}   
   
HDDEDATA WINAPI RequestDDEData( HDDEDATA hData ) 
{   
    char sz[128] ;

    if (g_fServerCreated == FALSE)
        return NULL ;

    #ifdef _DEBUG
    wsprintf( sz, "RequestDDEData %lu\r\n", g_dwUpdateCount ) ;
    OutputDebugString( sz ) ;
    #endif
               
    wsprintf( sz, "%lu", g_dwUpdateCount ) ;
    
    return DdeAddData( hData, (LPBYTE)sz, lstrlen( sz ) + 1, 0L ) ;
}   
