// xrtobj32.h
//

#ifndef _XRTOBJ32_H_
#define _XRTOBJ32_H_

extern HINSTANCE   g_hInst ;
extern HWND        g_hwndMain ;
extern LPTSTR      g_pszCmdLine ;

// Class factory related globals
extern LPCLASSFACTORY  g_pIClassFactory ;
extern DWORD           g_cLock ;              // Lock count
extern DWORD           g_cObj ;               // Number of alive objects
extern DWORD           g_dwRegCO ;

// Simulation globals
class CSimulation ;
extern CSimulation*    g_pSimObj ;
extern UINT            g_uiTimeoutMarket ;
extern UINT            g_uiTimeoutUpdate ;

class CStocksObject ;
void WINAPI ObjectCreated( CStocksObject* pObj, BOOL fCreated )  ;
typedef void (CALLBACK* PFNCREATED)( CStocksObject*, BOOL ) ;


/////////////////////////////////////////////////////////////////////////////
// IClassFactory implementation class
//
class FAR CImpIClassFactory : public IClassFactory
{
protected:
    ULONG           m_cRef;         //Reference count on object

public:
    CImpIClassFactory(void);
    ~CImpIClassFactory(void);

    //IUnknown interface members
    STDMETHODIMP QueryInterface(REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    //IClassFactory members
    STDMETHODIMP CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
    STDMETHODIMP LockServer(BOOL);
};


#endif // _XRTOBJ32_H_
