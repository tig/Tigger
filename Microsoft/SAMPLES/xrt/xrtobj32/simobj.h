// simobj.h
//
//
#ifndef _SIMOBJ_H_
#define _SIMOBJ_H_

#define MAX_CLIENTS     16

class CSimulation
{
public:
    CSimulation() ;
    ~CSimulation() ;

    BOOL    Reset( void ) ;     // resets to startup state
    void    OnTick( void ) ;    // move to next state
    void    UpdateAllClients( void ) ;
    UINT    RegisterClient( LPVOID pObj ) ;
    BOOL    UnRegisterClient( UINT index ) ;
    PXRTSTOCKS  GetData() ;

private:
    UINT        m_cUpdatesPerTick ; // number of items to change per tick
    BOOL        m_fBullMarket ;     // if TRUE market tends to go up
    PXRTSTOCKS  m_pData ;           // our database

    // client info
    LPVOID      m_rgClients[MAX_CLIENTS] ;

} ;

#endif // _SIMOBJ_H_


