/*************************************************************************
**
**    OLE Automation Collection sample
**
**    collect.cpp
**
**    CCollection implementation
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
 * CCollection::Create
 *
 * Purpose:
 *  Creates an instance of a collection object and initializes it.
 *
 * Parameters:
 *  lMaxSize   Maximum number of items that can added to collection.
 *  lLBound    Lower bound of index of collection. 
 *  ppcollection    Returns collection object.
 *
 * Return Value:
 *  HRESULT
 *
 */
STDMETHODIMP 
CCollection::Create(ULONG lMaxSize, long lLBound, CCollection FAR* FAR* ppcollection ) 
{   
    HRESULT hr;
    CCollection FAR* pcollection = NULL;
    SAFEARRAYBOUND sabound[1];
    extern HWND g_hwnd;
    extern HINSTANCE g_hinst;
     
    *ppcollection = NULL;
    
    // Create new collection
    pcollection = new CCollection();
    if (pcollection == NULL)
        goto error;
    
    pcollection->m_cRef = 0;
    pcollection->m_cMax = lMaxSize;
    pcollection->m_cElements = 0; 
    pcollection->m_lLBound = lLBound;
    
    // Load type information for the collection from type library. CreateStdDispatch
    // is not used to create a standard IDispatch implemenation because this
    // collection implementation raises exceptions on errors.    
    hr = LoadTypeInfo(&pcollection->m_ptinfo, IID_ICollection);
    if (FAILED(hr))
        goto error;
    
    // Create a safe array which is used to implement the collection.            
    sabound[0].cElements = lMaxSize;
    sabound[0].lLbound = lLBound;    
    pcollection->m_psa = SafeArrayCreate(VT_VARIANT, 1, sabound);
    if (pcollection->m_psa == NULL)
    {
        hr = ResultFromScode(E_OUTOFMEMORY);
        goto error;
    }     
    
    // Create list control to display the text of items in collection.   
    pcollection->m_hwndList = CreateWindow("listbox", "",
                 WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL,
                 10, 10, 300, 100,
                 g_hwnd, IDC_COLLECTIONLIST, g_hinst, NULL);
    if (!pcollection->m_hwndList)
    {
        hr =  ResultFromScode(E_OUTOFMEMORY);
        goto error;
    }             

    *ppcollection = pcollection;
    return NOERROR;
    
error: 
    if (pcollection == NULL)
        return ResultFromScode(E_OUTOFMEMORY);
    if (pcollection->m_ptinfo)
        pcollection->m_ptinfo->Release();                            
    if (pcollection->m_psa) 
        SafeArrayDestroy(pcollection->m_psa); 
    if (pcollection->m_hwndList)
        DestroyWindow(pcollection->m_hwndList);   
    pcollection->m_psa = NULL;
    pcollection->m_ptinfo = NULL; 
    pcollection->m_hwndList = NULL;   
    
    delete pcollection;
    return hr;
}

/*
 * CCollection::CCollection
 *
 * Purpose:
 *  Constructor for CCollection object. Initializes members to NULL.
 *
 */
CCollection::CCollection()
{    
    m_psa = NULL;
    m_ptinfo = NULL; 
    m_hwndList = NULL; 
}

/*
 * CCollection::~CCollection
 *
 * Purpose:
 *  Destructor for CCollection object. 
 *
 */
CCollection::~CCollection()
{    
     if (m_ptinfo) m_ptinfo->Release();                   
     if (m_psa) SafeArrayDestroy(m_psa);
     if (m_hwndList) DestroyWindow(m_hwndList);
}

/*
 * CCollection::QueryInterface, AddRef, Release
 *
 * Purpose:
 *  Implements IUnknown::QueryInterface, AddRef, Release
 *
 */
STDMETHODIMP
CCollection::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{   
    // See CApp::QueryInterface for an explanation of why IID_DCollection
    // is supported in this QueryInterface implemenatation.
    *ppv = NULL;
        
    if (iid == IID_IUnknown) 
        *ppv = this;  
    else if (iid == IID_ICollection) 
        *ppv = this;    
    else if (iid == IID_IDispatch || iid == IID_DCollection)
        *ppv = this;   
    else return ResultFromScode(E_NOINTERFACE); 

    AddRef();
    return NOERROR;    
}


STDMETHODIMP_(ULONG)
CCollection::AddRef(void)
{   
    //char ach[50];
    //wsprintf(ach, "Ref = %ld, Collection\r\n", m_cRef+1); 
    //OutputDebugString(ach);   
    
    return ++m_cRef;
}


STDMETHODIMP_(ULONG)
CCollection::Release(void)
{   
    //char ach[50];
    //wsprintf(ach, "Ref = %ld, Collection\r\n", m_cRef-1); 
    //OutputDebugString(ach);   
    
    if(--m_cRef == 0)
    {
        delete this;
        return 0;
    }
    return m_cRef;
} 

/*
 * CCollection::GetTypeInfoCount
 *
 * Purpose:
 *  Implements IDispatch::GetTypeInfoCount.
 *
 */
STDMETHODIMP
CCollection::GetTypeInfoCount(UINT FAR* pctinfo)
{
    *pctinfo = 1;
    return NOERROR;
}

/*
 * CCollection::GetTypeInfo
 *
 * Purpose:
 *  Implements IDispatch::GetTypeInfo. 
 *
 */
STDMETHODIMP
CCollection::GetTypeInfo(
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo)
{    
    *pptinfo = NULL;
     
    if(itinfo != 0)
        return ResultFromScode(DISP_E_BADINDEX);
    
    m_ptinfo->AddRef(); 
    *pptinfo = m_ptinfo;
    
    return NOERROR;
}

/*
 * CCollection::GetIDsOfNames
 *
 * Purpose:
 *  Implements IDispatch::GetIDsOfNames.  The standard implementation, DispGetIDsOfNames,
 *  is used.
 *
 */
STDMETHODIMP 
CCollection::GetIDsOfNames(
      REFIID riid,
      char FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid)
{
    return DispGetIDsOfNames(m_ptinfo, rgszNames, cNames, rgdispid);
}

/*
 * CCollection::Invoke
 *
 * Purpose:
 *  Implements IDispatch::Invoke.  The standard implementation, DispInvoke,
 *  is used. Properties and methods exposed by the collection object will
 *  set m_bRaiseException to raise an exception. 
 *
 */
STDMETHODIMP
CCollection::Invoke(
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr)
{  
    HRESULT hr;
     
    if (NULL == pexcepinfo)
        return ResultFromScode(E_INVALIDARG); 

    m_bRaiseException = FALSE;
         
    hr = DispInvoke(
        this, m_ptinfo,
        dispidMember, wFlags, pdispparams,
        pvarResult, pexcepinfo, puArgErr);
    
    // Check if property accessor function or method called by DispInvoke wants to raise
    // an exception. If so, return DISP_E_EXCEPTION after filling out the EXCEPINFO 
    // structure. The scode and bstrDescription fields of EXCEPINFO are filled by the 
    // property or method.    
    if (m_bRaiseException)
    {
        pexcepinfo->scode = m_excepinfo.scode; 
        pexcepinfo->bstrDescription = m_excepinfo.bstrDescription;   
        
        pexcepinfo->bstrSource = SysAllocString("Collection Application"); 
        pexcepinfo->bstrHelpFile = NULL;
        pexcepinfo->pvReserved =  NULL;
        m_bRaiseException = FALSE;
        return ResultFromScode(DISP_E_EXCEPTION); 
    }
    else return hr;    
}


/*
 * CCollection::Add
 *
 * Purpose:
 *  Adds an item to the collection. Signals IDispatch::Invoke to raise exception
 *  on error.
 *
 * Parameters:
 *  pdisp    IDispatch of item to be added to collection.
 *
 */
STDMETHODIMP_(void)   
CCollection::Add(LPDISPATCH pdisp)
{ 
    HRESULT hr;   
    VARIANT v;  
    LONG l;
    CItem FAR* pitem = NULL;  
    BSTR bstr;
    
    // Is the collection full?
    if (m_cElements == m_cMax)     
    {
       m_excepinfo.scode = DISP_E_OVERFLOW;   
       m_excepinfo.bstrDescription = SysAllocString("Collection is full. No more items can be added.");           
       m_bRaiseException = TRUE;
       return;  
    }
    
    // Add new item to safe array
    VariantInit(&v);
    V_VT(&v) = VT_DISPATCH;
    V_DISPATCH(&v) = pdisp; 
    l = m_lLBound+m_cElements;    
    hr = SafeArrayPutElement(m_psa, &l, &v);
    if (FAILED(hr))
        goto error;    
    m_cElements++;
    
    // Add text of item to list box. Note we can QI for IID_IItem without writing remoting code
    // because the process boundary is not crossed.
    hr = pdisp->QueryInterface(IID_IItem, (void FAR* FAR*)&pitem);
    if (FAILED(hr))
        goto error;  
    bstr = pitem->get_Text(); 
    SendMessage(m_hwndList, LB_ADDSTRING, 0, (LPARAM)(LPSTR)bstr);  
    SysFreeString(bstr);     
    pitem->Release();

    return;    
    
error:  
    if (pitem)
        pitem->Release(); 
    // Signal CCollection::Invoke to raise exception.
    m_excepinfo.scode = GetScode(hr);
    m_excepinfo.bstrDescription = SysAllocString("Item addition failed.");
    m_bRaiseException = TRUE;    
    return;   
}

/*
 * CCollection::get_Count
 *
 * Purpose:
 *  Returns number of items in collection.
 *
 */
STDMETHODIMP_(long)     
CCollection::get_Count(void)  
{
    return m_cElements;             
}

/*
 * CCollection::get_Item
 *
 * Purpose:
 *  Retrieves item from collection, given an index. Signals CCollection::Invoke to raise
 *  exception on error.
 *
 * Parameters:   
 *   vIndex   Index of item to be retrieved. This index can be an integer or a string.
 *
 * Returns
 *  IDispatch of item retrieved from collection.
 *
 */
STDMETHODIMP_(LPDISPATCH)
CCollection::get_Item(VARIANT vParam)
{ 
    VARIANT v, vIndex; 
    HRESULT hr;
    long lIndex;
    CItem FAR* pitem = NULL;  
    BSTR bstr;
   
    VariantInit(&v); 
    VariantInit(&vIndex);
    
    // If a byref parameter was passed, get the value after dereference. 
    hr = VariantCopyInd(&vIndex, &vParam);  
    if (FAILED(hr))
        goto error;
            
    // Get index of item to be retreived. 
    if (V_VT(&vIndex) == VT_I2)
        lIndex = V_I2(&vIndex);
    else if (V_VT(&vIndex) == VT_I4)
        lIndex = V_I4(&vIndex);
    else if (V_VT(&vIndex) == VT_BSTR)
    {    
         // If index is a string, search the safe array for the item
        for (lIndex = m_lLBound; lIndex < (long)(m_lLBound+m_cElements); lIndex++)
        {
            hr = SafeArrayGetElement(m_psa, &lIndex, &v);
            if (FAILED(hr))
                goto error;  
            hr = V_DISPATCH(&v)->QueryInterface(IID_IItem, (void FAR* FAR*)&pitem);
            if (FAILED(hr))
                goto error;  
            bstr = pitem->get_Text();            
            if (!lstrcmp(V_BSTR(&vIndex), bstr))  // If item is found
            {
                pitem->Release();
                SysFreeString(bstr); 
                VariantClear(&vIndex);
                return V_DISPATCH(&v) ; 
            }
            else
            {
                pitem->Release();
                SysFreeString(bstr);
                VariantClear(&v); // SafeArrayGetElement AddRefs returned element, so release it.  
            }
        }
        hr = ResultFromScode(DISP_E_BADINDEX);   // Item not found
        goto error;
    }
    else // Index is not string or integer 
    {  
        hr = ResultFromScode(DISP_E_TYPEMISMATCH);  
        goto error;
    }
    
    VariantClear(&vIndex);
    
    // Check if integer index is within range         
    if (lIndex < m_lLBound || lIndex >= (long)(m_lLBound+m_cElements)) 
    {
        hr = ResultFromScode(DISP_E_BADINDEX);
        goto error; 
    }   
    
    // Retrieve and return item. Note that SafeArrayGetElement AddRefs so an additional AddRef
    // is not required.
    hr = SafeArrayGetElement(m_psa, &lIndex, &v);
    if (FAILED(hr))
        goto error;  
    return V_DISPATCH(&v);
   
error:
    VariantClear(&v); 
    VariantClear(&vIndex); 
    if (pitem)
        pitem->Release(); 
    
    // Signal CCollection::Invoke to raise exception.    
    m_excepinfo.scode = GetScode(hr);
    switch (m_excepinfo.scode)
    {
        case DISP_E_BADINDEX:
            m_excepinfo.bstrDescription = SysAllocString("Invalid index specified for item retrieval.");
            break;            
        case DISP_E_TYPEMISMATCH:
            m_excepinfo.bstrDescription = SysAllocString("Index type for item retrieval must be integer or string.");    
            break;        
        default:
            m_excepinfo.bstrDescription = SysAllocString("Item retrieval failed.");    
            break; 
    }    
    m_bRaiseException = TRUE;    
    return NULL;   
}

/*
 * CCollection::get_NewEnum
 *
 * Purpose:
 *  Returns an enumerator (IEnumVARIANT) for the items curently in the collection. 
 *  The NewEnum property is restricted and so is invisible to users of an 
 *  automation controller's scripting language. Automation controllers that support
 *  a 'For Each' statement to iterate through the elements of a collection will use
 *  the enumerator returned by NewEnum.
 *
 */
STDMETHODIMP_(LPUNKNOWN)
CCollection::get__NewEnum(void)
{
    CEnumVariant FAR* penum = NULL;;
    LPUNKNOWN punkEnumVariant = NULL;
    HRESULT hr;
    
    // Create new enumerator for items currently in collection and QI for IUnknown
    hr = CEnumVariant::Create(m_psa, m_cElements, &penum);
    if (FAILED(hr))
        goto error;        
    hr = penum->QueryInterface(IID_IUnknown, (VOID FAR* FAR*)&punkEnumVariant);    
    if (FAILED(hr)) 
    {
        delete penum;
        goto error;  
    }      
    return punkEnumVariant; 
    
error:
    if (punkEnumVariant)
        punkEnumVariant->Release();  
    // Signal CCollection::Invoke to raise exception.
    m_excepinfo.scode = GetScode(hr);
    m_excepinfo.bstrDescription = SysAllocString("Enumerator creation failed."); 
    m_bRaiseException = TRUE;    
    return NULL;              
}

/*
 * CCollection::Remove
 *
 * Purpose:
 *  Removes specified item from collection. Signals CCollection::Invoke to raise
 *  exception on error.
 *
 * Parameters:   
 *   vIndex   Index of item to be removed. This index can be an integer or a string.
 *
 */
STDMETHODIMP_(void)
CCollection::Remove(VARIANT vParam)
{
    VARIANT v, vIndex; 
    HRESULT hr;
    long l1, l2, lIndex;
    CItem FAR* pitem = NULL; 
    BSTR bstr;
    
    VariantInit(&v); 
    VariantInit(&vIndex);
    
    // If a byref parameter was passed, get the value after dereference. 
    hr = VariantCopyInd(&vIndex, &vParam);  
    if (FAILED(hr))
        goto error; 
        
    // Get index of item to be retreived.  
    if (V_VT(&vIndex) == VT_I2)
        lIndex = V_I2(&vIndex);
    else if (V_VT(&vIndex) == VT_I4)
        lIndex = V_I4(&vIndex);
    else if (V_VT(&vIndex) == VT_BSTR)
    {    
        // If index is a string, search the safe array for the item
        for (lIndex = m_lLBound; lIndex < (long)(m_lLBound+m_cElements); lIndex++)
        {
            hr = SafeArrayGetElement(m_psa, &lIndex, &v);
            if (FAILED(hr))
                goto error;  
            hr = V_DISPATCH(&v)->QueryInterface(IID_IItem, (void FAR* FAR*)&pitem);
            if (FAILED(hr))
                goto error;       
            VariantClear(&v);  // SafeArrayGetElement AddRefs returned element, so release it. 
            bstr = pitem->get_Text();     
            if (!lstrcmp(V_BSTR(&vIndex), bstr))   // If item is found
            {   
                SysFreeString(bstr);
                pitem->Release();
                break ;  
            }
            SysFreeString(bstr);
            pitem->Release();

        }
        if (lIndex >= (long)(m_lLBound+m_cElements))  // Item not found
        {
            hr = ResultFromScode(DISP_E_BADINDEX);
           goto error; 
        }
    }
    else // Index is not string or integer
    {  
        hr = ResultFromScode(DISP_E_TYPEMISMATCH); 
        goto error;
    }
    
    VariantClear(&vIndex); 
    
    // Check if integer index is within range 
    if (lIndex < m_lLBound || lIndex >= (long)(m_lLBound+m_cElements))
    {
        hr = ResultFromScode(DISP_E_BADINDEX);
        goto error; 
    }   
    
    // Move array elements, after the element to be removed, by one place to the left.    
    // SafeArrayPutElement will release the element that is being replaced. So 
    // moving elements one place to the left will remove the element at lindex.
    // Note that SafeArrayGetElement will AddRef the the item being retrieved.
    // SafeArrayPutElement will AddRef the element being put into the array (in addition
    // to releasing the element being replaced).
    for (l1=lIndex+1; l1<(long)(m_lLBound+m_cElements); l1++)
    {   
        // Move array element to the left.
        hr = SafeArrayGetElement(m_psa, &l1, &v);
        if (FAILED(hr))
            goto error; 
        l2 = l1-1;
        hr = SafeArrayPutElement(m_psa, &l2, &v);    
        if (FAILED(hr))
            goto error;  
        // SAGetElement and SAPutElement results in the the ref count of the
        // element moved from l1 to l2 to be incremented by 2. A copy of the moved 
        // element also resides in l1 and will be replaced by the next element
        // is that is moved. This replacement (by SAPutElement) will decrement the ref count
        // of the replaced element by 1. The following VariantClear will decrement the 
        // ref count by 1 so that the final ref count does not change as a consequence of
        // the element being moved.
        VariantClear(&v);
    } 
    
    // Decrement the ref count of the last element in the array.
    VariantInit(&v);
    l2 = l1-1;    
    hr = SafeArrayPutElement(m_psa, &l2, &v);
    if (FAILED(hr))
        goto error; 
    
    m_cElements--; 
    // Remove item from list box control 
    SendMessage(m_hwndList, LB_DELETESTRING, (WPARAM)(lIndex-m_lLBound), 0L);   
    return; 
       
error: 
    VariantClear(&v); 
    VariantClear(&vIndex);  
    if (pitem)
        pitem->Release(); 
         
    // Signal CCollection::Invoke to raise exception.    
    m_excepinfo.scode = GetScode(hr);
    switch (m_excepinfo.scode)
    {
        case DISP_E_BADINDEX:
            m_excepinfo.bstrDescription = SysAllocString("Invalid index specified for item removal.");
            break;            
        case DISP_E_TYPEMISMATCH:
            m_excepinfo.bstrDescription = SysAllocString("Index type for item removal must be integer or string.");    
            break;        
        default:
            m_excepinfo.bstrDescription = SysAllocString("Item removal failed.");    
            break; 
    }    
    m_bRaiseException = TRUE;   
    return;
}
