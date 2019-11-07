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
    if(--m_cRef == 0)
    {
        delete this;
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
    extern CApp FAR* g_papp;
    extern HWND g_hwnd;
    extern HINSTANCE g_hinst;
    
    HRESULT hr;
    
    // This implementation does not allow aggregation
    if (punkOuter)
        return ResultFromScode(CLASS_E_NOAGGREGATION);   
   
    // Create an instance of the App Automation object. This
    //   is a REGCLS_SINGLEUSE class factory - which means that this
    //   instance of the application will create at most one instance of
    //   the App object.
    hr = CApp::Create(&g_papp);       
    if (FAILED(hr))
        return hr;             
    hr = g_papp->QueryInterface(riid, ppv);
    if (FAILED(hr)) 
    {
        delete g_papp;
        return hr;
    } 
    return NOERROR;
}

STDMETHODIMP
CAppCF::LockServer(BOOL fLock)
{
    // Not implemented
    return NOERROR;            
}


