// simobj.cpp
//
// Implementation of the market simulation object.  This object
// simulates a real-time stock market data feed.
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
// October 29, 1993
//
// Internet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  October 29, 1993 cek Created
//
#include "precomp.h"
#include <time.h>
#include "xrtobj32.h"
#include "simobj.h"
#include "object.h"

LONG WINAPI ParseOffNumber( LPTSTR FAR *lplp, LPINT lpConv ) ;

CSimulation::CSimulation()
{
    m_pData = NULL ;

    for (UINT i = 0 ; i < MAX_CLIENTS ; i++)
    {
        m_rgClients[i] = NULL ;
    }

    Reset() ;
}

CSimulation::~CSimulation()
{
    if (m_pData != NULL)
        delete m_pData ;
}


// CSimulation::Reset()
// 
// Resets the simulation to it's start state
//
BOOL CSimulation::Reset()
{
    m_cUpdatesPerTick =  GetPrivateProfileInt( L"Simulation", L"UpdatesPerTick", 10, L"XRTOBJ32.INI" ) ;
    m_fBullMarket = TRUE ;

    if (m_pData != NULL)
        delete m_pData ;

    // [Database] secton of INI file contains entries like this:
    // MSFT=last,volume
    //
    TCHAR * lpbuf = new TCHAR[16000] ;  
    TCHAR szVal[80] ;
    if (GetPrivateProfileString( L"Quotes", NULL, L"", lpbuf, 16000, L"XRTOBJ32.INI" ))
    {
        int c ;
        LPTSTR pszKey ;
        for (pszKey = lpbuf, c = 0;
                *pszKey != TEXT('\0'); pszKey += lstrlen(pszKey) + 1) 
            c++ ;

        m_pData = (LPXRTSTOCKS)malloc( (c * sizeof(XRTSTOCK)) + sizeof(XRTSTOCKS) ) ;
        m_pData->cStocks = c ;
        
        LPTSTR lp ;
        int nConv ;
        for (pszKey = lpbuf, c = 0 ; *pszKey != TEXT('\0'); pszKey += lstrlen(pszKey) + 1) 
        {
            GetPrivateProfileString(L"Quotes", pszKey, L"XXX,0,0", szVal, sizeof(szVal)/ sizeof(TCHAR), L"XRTOBJ32.INI" ) ;
            lstrcpy( m_pData->rgStocks[c].szSymbol, pszKey ) ;
            lp = szVal ;
            m_pData->rgStocks[c].ulLast = ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
            m_pData->rgStocks[c].ulHigh = m_pData->rgStocks[c].ulLast ;
            m_pData->rgStocks[c].ulLow = m_pData->rgStocks[c].ulLast ;
            m_pData->rgStocks[c].ulVolume = ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
            m_pData->rgStocks[c].uiMembers = XSM_ALL ;
            m_pData->rgStocks[c].uiIndex = c ;
            c++ ;
        }            
    }
    else
    {
        m_pData = (LPXRTSTOCKS)malloc( sizeof(XRTSTOCKS) ) ;
        m_pData->cStocks = 0 ;
    }
    delete lpbuf ;

    
    // Reset the random number generator
    //
    srand( (unsigned)time( NULL ) );

    UpdateAllClients() ;

    return TRUE ;
}

// GetData
//
// This just returns a pointer to the simulation object.  
//
// CONSIDER:  This should allocate a new pointer, copy the
// data, and return a pointer to the copy.   Another member
// should be implemented (ReleaseData) which frees the
// data when the user is done.  
//
PXRTSTOCKS CSimulation::GetData()
{
    return m_pData ;
}

// OnTick
//
// Each timer tick generate random stock data.
//
void CSimulation::OnTick( void ) 
{
    int change ;
    LPXRTSTOCK    lpQ ;
    int i ;

    if (m_pData == NULL || m_pData->cStocks <= 0)
        return ;

    // TODO:  Instead of using m_cUpdatesPerTick literally,
    // generate a random number from 1 to m_cUpdatesPerTick and
    // use that here.
    for ( UINT n = 0 ; n < m_cUpdatesPerTick ; n++ )
    {
        i = (int)((rand() / (double)RAND_MAX) * (double)m_pData->cStocks) ;

        lpQ = &m_pData->rgStocks[i] ;

        // Randomize ask and volume
        // 
        if (m_fBullMarket) // tend to go up
            change = (rand() < (RAND_MAX / 2)) ? 250 : -100 ;
        else
            change = (rand() < (RAND_MAX / 2)) ? -250 : 100 ;

        if ((LONG)lpQ->ulLast + change > (LONG)0)
            lpQ->ulLast += change ;

        lpQ->uiMembers |= XSM_LAST ;
        
        if (lpQ->ulLast > lpQ->ulHigh)
        {
            lpQ->ulHigh = lpQ->ulLast ;
            lpQ->uiMembers |= XSM_HIGH ;
        }    
        if (lpQ->ulLast < lpQ->ulLow)
        {
            lpQ->ulLow = lpQ->ulLast ;
            lpQ->uiMembers |= XSM_LOW ;
        }
            
        lpQ->ulVolume += (abs(change)/2) ;
        lpQ->uiMembers |= XSM_VOLUME ;
    }

    // Call the OnUpdateData members of all CStocksObject objects who
    // have registered with us.
    //
    UpdateAllClients( ) ;
    
    for ( n = 0 ; n < m_pData->cStocks ; n ++ )
        m_pData->rgStocks[n].uiMembers = 0 ;
}

// CSimulation::UpdateAllClients()
//
// Call the OnUpdateData members of all CStocksObject objects who
// have registered with us.
//
void CSimulation::UpdateAllClients()
{
    // Force repaint
    //
    InvalidateRect( g_hwndMain, NULL, TRUE ) ;
    UpdateWindow( g_hwndMain ) ;

    for (UINT i = 0 ; i < MAX_CLIENTS ; i++)
    {
        if (m_rgClients[i] != NULL)
            ((CStocksObject*)m_rgClients[i])->OnMarketChange() ;
    }
}

// CSimulation::RegisterClient
//
// CStocksObjects who wish to be notified when data is
// generated call this member to 'register'. 
//
UINT CSimulation::RegisterClient( LPVOID pObj )
{
    for (UINT i = 0 ; i < MAX_CLIENTS ; i++)
    {
        if (m_rgClients[i] == NULL)
        {
            m_rgClients[i] = pObj ;
            return i + 1 ;
        }
    }
    return 0 ;
}

// CSimulation::UnRegisterClient
//
// CStocksObjects who registered with RegisterClient
// call this member to 'unregister'. 
//
BOOL CSimulation::UnRegisterClient( UINT index )
{
    if (index > 0)
        m_rgClients[index-1] = NULL ;
    else
        return FALSE ;

    return TRUE ;
}


#include <ctype.h>
// Very useful API that parses a LONG out of a string, updating
// the string pointer before it returns.
// 
LONG WINAPI ParseOffNumber( LPTSTR FAR *lplp, LPINT lpConv )
{
    LPTSTR lp = *lplp;
    LONG  lValue=0;
    int   sign=1;

    while( isspace(*lp) )
        lp++;
    if( *lp==TEXT('-') )
    {               
        sign = -1;
        lp++;
    }
    if (*lp==TEXT('\0'))
    {
        *lpConv = FALSE;
        return 0L;
    }

    // Is it a decimal number
    if( lstrcmpi( lp, (LPTSTR)L"0x") )
    {
        #ifdef UNICODE
        while( iswdigit(*lp) )
        #else
        while( isdigit(*lp) )
        #endif
        {
            lValue *= 10;
            lValue += (*lp - TEXT('0'));
            lp++;
        }
    }    
    else
    {
        lp+=2;

        #ifdef UNICODE
        while( iswxdigit(*lp) )
        #else
        while( isxdigit(*lp) )
        #endif
        {
            lValue *= 16;
            #ifdef UNICODE
            if( iswdigit(*lp) )
            #else
            if( isdigit(*lp) )
            #endif
                lValue += (*lp - TEXT('0'));
            else
                lValue += ((TCHAR)CharUpper((LPTSTR)*lp) - TEXT('A') + 10);
            lp++;
        }
    }
    #ifdef UNICODE
    while( iswspace(*lp) )
    #else
    while( isspace(*lp) )
    #endif
        lp++;

    lValue *= (long)sign;

    if (*lp==TEXT(','))
    {
        lp++;
        #ifdef UNICODE
        while( iswspace(*lp) )
        #else
        while( isspace(*lp) )
        #endif
            lp++;
        *lplp = lp;
        if (lpConv)
            *lpConv = (int)TRUE;
    }
    else
    {
        *lplp = lp;
        if (lpConv)
            *lpConv = (int)(*lp==TEXT('\0'));
    }
    return lValue;

} //*** ParseOffNumber

