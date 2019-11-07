/*************************************************************************
**
**    OLE Automation Collection sample
**
**    appcf.cpp
**
**    CAppCF (class factory) implementation
**
**     Written by Microsoft Product Support Services, Windows Developer Support
**    (c) Copyright Microsoft Corp. 1993 All Rights Reserved
**
*************************************************************************/
#include <windows.h>
#include <ole2.h>          
#include "app.h" 

CAppCF::CAppCF(void)
{    
    m_cRef = 0; 
    AddRef();
}

/*
 * CAppCF::QueryInterface, AddRef, Release
 *
 * Purpose:
 *  Implements IUnknown::QueryInterface, AddRef, Release
 *
 */
STDMETHODIMP
CAppCF::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{   
    *ppv = NULL;
    
    if (iid == IID_IUnknown || iid == IID_IClassFactory)
        *ppv = this;
    else 
        return ResultFromScode(E_NOINTERFACE); 

    AddRef();
    
    g_pAppCF = this ;
    
    return NOERROR;    
}


STDMETHODIMP_(ULONG)
CAppCF::AddRef(void)
{
    return ++m_cRef;
}


STDMETHODIMP_(ULONG)
CAppCF::Release(void)
{
    extern ULONG g_cObj ;
    
    if(--m_cRef == 0)
    {
        if (g_cObj == 0)    // only release the CF if there are no objects
        {
            g_pAppCF = NULL ;
            delete this;
        }    
        return 0;
    }
    return m_cRef;
}

/*
 * CAppCF::CreateInstance, LockServer
 *
 * Purpose:
 *  Implements IClassFactory::CreateInstance, LockServer
 *
 */
STDMETHODIMP
CAppCF::CreateInstance(IUnknown FAR* punkOuter,
                         REFIID riid,
                         void FAR* FAR* ppv)
{
    extern HINSTANCE g_hinst;
    
    HRESULT hr;
    
    // This implementation does not allow aggregation
    if (punkOuter)
        return ResultFromScode(CLASS_E_NOAGGREGATION);   
   
    // Create an instance of the App Automation object. This
    //   is a REGCLS_SINGLEUSE class factory - which means that this
    //   instance of the application will create at most one instance of
    //   the App object.
    CApp FAR* papp ;
    papp = new CApp();
    if (papp == NULL)
        return ResultFromScode( E_OUTOFMEMORY ) ;
    
    hr = papp->Init( ) ;       
    delete papp ;
    if (FAILED(hr))
        return hr;             

    hr = papp->QueryInterface(riid, ppv);
    if (FAILED(hr)) 
    {
        delete papp;
        return hr;
    } 

    AppObjectCreated( TRUE ) ;
    
    return NOERROR;
}

BOOL CAppCF::AppObjectCreated( BOOL fCreated )
{   
    extern ULONG g_cObj ;
    
    if (fCreated)
    {
        g_cObj++ ;
    }
    else
    {
        g_cObj-- ;
        if (g_cObj == 0)
        {
            // TODO:  Do something to make the DLL/EXE go away
        }
    }

    return g_cObj != 0 ;
}

STDMETHODIMP
CAppCF::LockServer(BOOL fLock)
{
    // Not implemented
    return NOERROR;            
}


