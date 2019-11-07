/*************************************************************************
**
**    OLE Automation Collection sample
**
**    app.cpp
**
**    CApp implementation
**
**     Written by Microsoft Product Support Services, Windows Developer Support
**    (c) Copyright Microsoft Corp. 1993 All Rights Reserved
**
*************************************************************************/

#include <windows.h>
#include <ole2.h>    
#include <dispatch.h>       
#include "app.h"  

/*
 * CApp::Create
 *
 * Purpose:
 *  Creates an instance of the App Automation object and initializes it.
 *
 * Parameters:
 *  ppapp    Returns App automation object.
 *
 * Return Value:
 *  HRESULT
 *
 */
STDMETHODIMP 
CApp::Init( ) 
{   
    extern HINSTANCE g_hinst;
    HRESULT hr;
    CCollection FAR* pcollection = NULL;

    // Create our App window
    
    m_hwnd = CreateWindow("MainWndClass", "CApp Instance",
                       WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       350, 200,
                       NULL, NULL, g_hinst, NULL);
    if (!m_hwnd)
      return ResultFromScode( E_FAIL ) ;
       
    m_cRef = 0;
    
    // Create an IDispatch interface for the application object using the standard IDispatch
    // interface inmplementation.
    hr = CreateDispatchInterface(this, this, &m_punkStdDispatch, IID_IApp); 
    if (FAILED(hr))
        goto error;
    
    // Create an empty collection of size 10 and the lower bound of the index set to 1. The
    // collection code can handle any lower bound. Store the IDispatch of the collection.    
    hr = CCollection::Create(10, 1, &pcollection, m_hwnd);    
    if (FAILED(hr))
       goto error;      
    hr = pcollection->QueryInterface(IID_IDispatch, (void FAR* FAR*)&m_pdispCollection);
    if (FAILED(hr))   
    {
       delete pcollection;
       goto error; 
    }

    return NOERROR;
    
error:                        
    if (m_punkStdDispatch)
        m_punkStdDispatch->Release();        
    if (m_pdispCollection)
        m_pdispCollection->Release();
        
    m_punkStdDispatch = NULL;
    m_pdispCollection = NULL;

    return hr;
}

/*
 * CApp::CApp
 *
 * Purpose:
 *  Constructor for CApp object. Initializes members to NULL.
 *
 */
CApp::CApp()
{    
    m_punkStdDispatch = NULL;
    m_pdispCollection = NULL;
    m_hwnd = NULL ;
}

/*
 * CApp::~CApp
 *
 * Purpose:
 *  Destructor for CApp object. Closes the aplication.
 *
 */
CApp::~CApp()
{ 
             
    if (m_punkStdDispatch) m_punkStdDispatch->Release();
    if (m_pdispCollection) m_pdispCollection->Release();

    if (m_hwnd && IsWindow( m_hwnd ))
        DestroyWindow( m_hwnd ) ;     
}

/*
 * CApp::QueryInterface, AddRef, Release
 *
 * Purpose:
 *  Implements IUnknown::QueryInterface, AddRef, Release
 *
 */
STDMETHODIMP
CApp::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{   
    // This QI implementation handles IID_DApp for the following reason:
    // Some Automation controllers support/will support run-time type checking.
    // Such controllers may support a scripting language which supports the 
    // following syntax where a type name can be specified in the Dim statement:
    //   Dim AppObject As App
    //   Set AppObject = CreateObject("Collection.Application")
    // The Automation contoller will look up the UUID of the dispinterface called 
    // 'App' (which is IID_DApp) and will QI the object for this interface
    // to do type-checking.
    *ppv = NULL;
        
    if (iid == IID_IUnknown) 
        *ppv = this;  
    else if (iid == IID_IApp) 
        *ppv = this; 
    else if (iid == IID_IDispatch) 
        return m_punkStdDispatch->QueryInterface(IID_IDispatch, ppv);
    else if (iid == IID_DApp) 
        return m_punkStdDispatch->QueryInterface(IID_IDispatch, ppv);  
    else return ResultFromScode(E_NOINTERFACE); 

    AddRef();
    return NOERROR;    
}


STDMETHODIMP_(ULONG)
CApp::AddRef(void)
{   
    //char ach[50];
    //wsprintf(ach, "Ref = %ld, App\r\n", m_cRef+1); 
    //OutputDebugString(ach); 
    
    return ++m_cRef;
}


STDMETHODIMP_(ULONG)
CApp::Release(void)
{   
    //char ach[50];
    //wsprintf(ach, "Ref = %ld, App\r\n", m_cRef-1); 
    //OutputDebugString(ach);
    
    if(--m_cRef == 0)
    {
        g_pAppCF->AppObjectCreated( FALSE ) ;
        delete this;
        return 0;
    }
    return m_cRef;
}

/*
 * CApp::Collection
 *
 * Purpose:
 *  Implements the Collection method which returns the collection.  
 * 
 * Return Value:
 *  IDispatch of collection.
 *
 *
 */

STDMETHODIMP_(LPDISPATCH)
CApp::get_Collection(void)
{   
    m_pdispCollection->AddRef();
    return m_pdispCollection;
}  

/*
 * CApp::Collection
 *
 * Purpose:
 *  Implements the Collection method which returns the collection.    
 *
 * Parameters:
 *  bstr    Text of item to be created.
 *
 * Return Value:
 *  IDispatch of created item.
 *
 */
STDMETHODIMP_(LPDISPATCH)
CApp::NewItem(BSTR bstr)
{
    CItem FAR* pitem = NULL;
    HRESULT hr;   
    LPDISPATCH pdisp = NULL;
    
    // Create new item and QI for IDispatch   
    hr = CItem::Create(bstr, &pitem);
    if (FAILED(hr))
       goto error;       
    hr = pitem->QueryInterface(IID_IDispatch, (void FAR* FAR*)&pdisp); 
    if (FAILED(hr))
    {
        delete pitem;
        goto error; 
    } 
    return pdisp;
    
error: 
    if (pdisp) 
        pdisp->Release();
    // This failure does not raise an exception like the implementation of CCollection.
    // A similar approach to raise an exception can be used here.  
    return NULL;     
}


