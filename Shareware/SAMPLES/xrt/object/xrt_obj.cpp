// XRT_OBJ.CPP
//
// Implementation XRT_OBJ IDataObject sample application.  This file
// defines the classes for the main window and UI.
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
// July 15, 1993
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  July 15, 1993   cek     First implementation.
//
// Implementation Notes:
//
//

#include "dataobj.h"
#include "xrt_obj.h"
#include "mainwnd.h"

CXRTObjectApp NEAR theApp;

BOOL CXRTObjectApp::InitInstance()
{
    SetDialogBkColor();

    if (m_pMainWnd = new CMainWnd())
    {               
        if (m_nCmdShow != SW_MINIMIZE)
            m_nCmdShow = SW_SHOW ;
        m_pMainWnd->ShowWindow(m_nCmdShow);
        m_pMainWnd->UpdateWindow();
    
        m_pIClassFactory = NULL ;
        m_dwRegCO = 0 ;
        m_cObj = 0 ;
            
        if (FAILED( CoInitialize( NULL )))
        {
            TRACE( "CoInitialize failed" ) ;
            m_pMainWnd->PostMessage( WM_CLOSE ) ;
            return FALSE ;
        }
    
        m_pIClassFactory = (LPCLASSFACTORY)new CImpIClassFactory ;
         
        if (m_pIClassFactory == NULL)
        {
            TRACE( "new CImpClassFactory failed!" ) ;
            m_pMainWnd->PostMessage( WM_CLOSE ) ;
            return FALSE ;
        }
        
        m_pIClassFactory->AddRef() ;
        
        HRESULT hr ;
        hr = CoRegisterClassObject( CLSID_XRTDataObject, 
                                    (LPUNKNOWN)m_pIClassFactory,
                                    CLSCTX_LOCAL_SERVER,
                                    REGCLS_MULTIPLEUSE, &m_dwRegCO ) ;
                                    
        if (FAILED( hr ))
        {   
            TRACE( "CoRegisterClassObject failed" ) ;
            m_pMainWnd->PostMessage( WM_CLOSE ) ;
            return FALSE ; 
        }
        
        return TRUE ;
    }
    else
        return FALSE ;
    
}
 
int CXRTObjectApp::ExitInstance()
{
    if (m_dwRegCO != 0)
        CoRevokeClassObject( m_dwRegCO ) ;
    
    if (m_pIClassFactory != NULL)
        m_pIClassFactory->Release() ;

    return 1 ;        
} 


void ObjectCreated( void )
{
    theApp.m_cObj++ ;
}

void ObjectDestroyed( void )
{
    theApp.m_cObj-- ;
    
    if ( theApp.m_cObj == 0 && theApp.m_cLock == 0 && IsWindow( theApp.m_pMainWnd->m_hWnd ) )
        theApp.m_pMainWnd->PostMessage( WM_CLOSE ) ;
}

void CXRTObjectApp::LockServer( void )
{
    m_cLock++ ;
}

void CXRTObjectApp::UnlockServer( void )
{
    m_cLock-- ;
    
    if ( m_cObj == 0 && m_cLock == 0 && IsWindow( m_pMainWnd->m_hWnd ))
        m_pMainWnd->PostMessage( WM_CLOSE ) ;
}




// CImpIClassFactory::CImpIClassFactory
// CImpIClassFactory::~CImpIClassFactory
//
//
CImpIClassFactory::CImpIClassFactory(void)
{
    m_cRef=0L;
    return;
}

CImpIClassFactory::~CImpIClassFactory(void)
{
    return ;
}

// CImpIClassFactory::QueryInterface
// CImpIClassFactory::AddRef
// CImpIClassFactory::Release
//
// Purpose:
//  IUnknown members for CImpIClassFactory object.
//
STDMETHODIMP CImpIClassFactory::QueryInterface( REFIID riid, LPVOID FAR *ppv )
{
    *ppv = NULL ;

    //Any interface on this object is the object pointer.
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
        *ppv = (LPVOID)this;

    // If we actually assign an interface to ppv we need to AddRef it
    // since we're returning a new pointer.
    //
    if (NULL != *ppv)
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }
    
    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) CImpIClassFactory::AddRef(void)
{
    return ++m_cRef;
}


STDMETHODIMP_(ULONG) CImpIClassFactory::Release(void)
{
    ULONG           cRefT;

    cRefT=--m_cRef;

    //Free the object if the reference and lock counts are zero.
    //
    if (0L==m_cRef)
        delete this;

    return cRefT;
}


// CImpIClassFactory::CreateInstance
//
// Purpose:
//  Instantiates an object supported by this class factory.  That
//  object must at least support IUnknown.
//
//  Derived classes should override this function.
//
// Parameters:
//  punkOuter       LPUNKNOWN to the controlling IUnknown if we are
//                  being used in an aggregation.
//  riid            REFIID identifying the interface the caller desires
//                  to have for the new object.
//  ppvObj          LPVOID FAR * in which to store the desired interface
//                  pointer for the new object.
//
// Return Value:
//  HRESULT         NOERROR if successful, otherwise contains E_NOINTERFACE
//                  if we cannot support the requested interface.
//
STDMETHODIMP CImpIClassFactory::CreateInstance(LPUNKNOWN punkOuter,
                                    REFIID riid, LPVOID FAR *ppvObj)
{
    LPCDATAOBJECT   pObj ;
    HRESULT         hr ;
    
    *ppvObj = NULL ;
    
    hr = ResultFromScode(E_OUTOFMEMORY) ;
    
    // Verify that if there is a controlling unknown it's asking for
    // IUnknown
    if (NULL != punkOuter && !IsEqualIID( riid, IID_IUnknown ))
        return ResultFromScode( E_NOINTERFACE ) ;
        
    // Create the object, telling it to call ObjectDestroyed
    // when it get's destroyed
    pObj = new CDataObject( punkOuter, ObjectDestroyed ) ;
    
    if (pObj == NULL)
        return hr ;
        
    if (pObj->Init())
        hr = pObj->QueryInterface( riid, ppvObj ) ;
        
    ObjectCreated() ;
    
    if (FAILED(hr))
    {
        delete pObj ;
        ObjectDestroyed() ;  // could shutdown app
    }
    
    return hr ;
    
}
    

// CImpIClassFactory::LockServer
//
// Purpose:
//  Increments or decrements the lock count of the serving IClassFactory
//  object.  When the lock count is zero and the reference count is zero
//  we get rid of this object.
//
//  DLL objects should override this to affect their DLL ref count.
//
// Parameters:
//  fLock           BOOL specifying whether to increment or decrement the
//                  lock count.
//
// Return Value:
//  HRESULT         NOERROR always.
//
STDMETHODIMP CImpIClassFactory::LockServer(BOOL fLock)
{
    if (fLock)
        theApp.LockServer() ;
    else
        theApp.UnlockServer() ;     // could shutdown app
    
    return NOERROR;
}
