/*************************************************************************
**
**    OLE Automation Collection sample
**
**    item.cpp
**
**    CItem implementation
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
 * CItem::Create
 *
 * Purpose:
 *  Creates an instance of the Item object and initializes it.
 *
 * Parameters:
 *  bstr      Text of item that is to be created.
 *  ppitem    Returns Item object.
 *
 * Return Value:
 *  HRESULT
 *
 */
STDMETHODIMP 
CItem::Create(BSTR bstr, CItem FAR* FAR* ppitem ) 
{   
    HRESULT hr;
    CItem FAR* pitem = NULL;
     
    *ppitem = NULL;
    
    pitem = new CItem();
    if (pitem == NULL)
        goto error;
    
    pitem->m_cRef = 0;
    
    // Create an IDispatch interface for the item object using the standard IDispatch
    // interface inmplementation.
    hr = CreateDispatchInterface(pitem, pitem, &pitem->m_punkStdDispatch, IID_IItem); 
    if (FAILED(hr))
        goto error; 
    
    // Keep a private copy of the text of the item.    
    pitem->m_bstrText = SysAllocString(bstr);   
    if (pitem->m_bstrText == NULL)
    {
        hr = ResultFromScode(E_OUTOFMEMORY);
        goto error;
    }                   

    *ppitem = pitem;
    return NOERROR;
    
error: 
    if (pitem == NULL)
        return ResultFromScode(E_OUTOFMEMORY);
    if (pitem->m_punkStdDispatch)
        pitem->m_punkStdDispatch->Release();
    if (pitem->m_bstrText)
         SysFreeString(pitem->m_bstrText);  

    pitem->m_punkStdDispatch = NULL; 
    pitem->m_bstrText = NULL;   
    
    delete pitem;
    return hr;
}

/*
 * CItem::CItem
 *
 * Purpose:
 *  Constructor for CItem object. Initializes members to NULL.
 *
 */
CItem::CItem()
{
    m_punkStdDispatch = NULL;    
    m_bstrText = NULL;
}

/*
 * CItem::~CItem
 *
 * Purpose:
 *  Destructor for CItem object. 
 *
 */
CItem::~CItem()
{    
     if (m_punkStdDispatch) m_punkStdDispatch->Release();  
     if (m_bstrText) SysFreeString(m_bstrText);    
}

/*
 * CItem::QueryInterface, AddRef, Release
 *
 * Purpose:
 *  Implements IUnknown::QueryInterface, AddRef, Release
 *
 */
STDMETHODIMP
CItem::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{   
    // See CApp::QueryInterface for an explanation of why IID_DItem
    // is supported in this QueryInterface implemenatation.
    *ppv = NULL;
        
    if (iid == IID_IUnknown) 
        *ppv = this;   
    else if (iid == IID_IItem ) 
        *ppv = this;   
    else if (iid == IID_IDispatch || iid == IID_DItem )
        return m_punkStdDispatch->QueryInterface(IID_IDispatch, ppv);      
    else return ResultFromScode(E_NOINTERFACE); 

    AddRef();
    return NOERROR;    
}


STDMETHODIMP_(ULONG)
CItem::AddRef(void)
{  
    //char ach[50];
    //wsprintf(ach, "Ref = %ld, Item = %s\r\n", m_cRef+1, (LPSTR)m_bstrText); 
    //OutputDebugString(ach);
    
    return ++m_cRef;
}


STDMETHODIMP_(ULONG)
CItem::Release(void)
{   
    //char ach[50];
    //wsprintf(ach, "Ref = %ld, Item = %s\r\n", m_cRef-1, (LPSTR)m_bstrText); 
    //OutputDebugString(ach);
    
    if(--m_cRef == 0)
    {
        delete this;
        return 0;
    }
    return m_cRef;
}

/*
 * CItem::put_Text
 *
 * Purpose:
 *  Text property put function that sets the text of item.
 *
 */
STDMETHODIMP_(void)
CItem::put_Text(BSTR bstr)
{ 
    SysReAllocString(&m_bstrText, bstr);
}

/*
 * CItem::get_Text
 *
 * Purpose:
 *  Text property get function that gets the text of item.
 *
 */
STDMETHODIMP_(BSTR)
CItem::get_Text(void)
{  
   return SysAllocString(m_bstrText);  
}

