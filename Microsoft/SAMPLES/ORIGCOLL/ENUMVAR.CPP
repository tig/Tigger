/*************************************************************************
**
**    OLE Automation Collection sample
**
**    enumvar.cpp
**
**    CEnumVariant implementation
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
 * CEnumVariant::Create
 *
 * Purpose:
 *  Creates an instance of the IEnumVARIANT enumerator object and initializes it.
 *
 * Parameters:
 *  psa        Safe array containing items to be enumerated.
 *  cElements  Number of items to be enumerated. 
 *  ppenumvariant    Returns enumerator object.
 *
 * Return Value:
 *  HRESULT
 *
 */
STDMETHODIMP 
CEnumVariant::Create(SAFEARRAY FAR* psa, ULONG cElements, CEnumVariant FAR* FAR* ppenumvariant) 
{   
    HRESULT hr;
    CEnumVariant FAR* penumvariant = NULL;
    long lLBound;
                      
    *ppenumvariant = NULL;
    
    penumvariant = new CEnumVariant();
    if (penumvariant == NULL)
        goto error; 
        
    penumvariant->m_cRef = 0;
    
    // Copy elements into safe array that is used in enumerator implemenatation and 
    // initialize state of enumerator.
    hr = SafeArrayGetLBound(psa, 1, &lLBound);
    if (FAILED(hr))
        goto error;
    penumvariant->m_cElements = cElements;    
    penumvariant->m_lLBound = lLBound;
    penumvariant->m_lCurrent = lLBound;                  
    hr = SafeArrayCopy(psa, &penumvariant->m_psa);
    if (FAILED(hr))
       goto error;
    
    *ppenumvariant = penumvariant;
    return NOERROR;
    
error: 
    if (penumvariant == NULL)
        return ResultFromScode(E_OUTOFMEMORY);   
                              
    if (penumvariant->m_psa) 
        SafeArrayDestroy(penumvariant->m_psa);   
    penumvariant->m_psa = NULL;     
    delete penumvariant;
    return hr;
}

/*
 * CEnumVariant::CEnumVariant
 *
 * Purpose:
 *  Constructor for CEnumVariant object. Initializes members to NULL.
 *
 */
CEnumVariant::CEnumVariant()
{    
    m_psa = NULL;
}

/*
 * CEnumVariant::~CEnumVariant
 *
 * Purpose:
 *  Destructor for CEnumVariant object. 
 *
 */
CEnumVariant::~CEnumVariant()
{                   
    if (m_psa) SafeArrayDestroy(m_psa);
}

/*
 * CEnumVariant::QueryInterface, AddRef, Release
 *
 * Purpose:
 *  Implements IUnknown::QueryInterface, AddRef, Release
 *
 */
STDMETHODIMP
CEnumVariant::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{   
    *ppv = NULL;
        
    if (iid == IID_IUnknown || iid == IID_IEnumVARIANT) 
        *ppv = this;     
    else return ResultFromScode(E_NOINTERFACE); 

    AddRef();
    return NOERROR;    
}


STDMETHODIMP_(ULONG)
CEnumVariant::AddRef(void)
{   
    //char ach[50];
    //wsprintf(ach, "Ref = %ld, Enum\r\n", m_cRef+1); 
    //OutputDebugString(ach);   
    
    return ++m_cRef;  //REVIEW: Should AddRef Application Object
}


STDMETHODIMP_(ULONG)
CEnumVariant::Release(void)
{   
    //char ach[50];
    //wsprintf(ach, "Ref = %ld, Enum\r\n", m_cRef-1); 
    //OutputDebugString(ach);   
    
    if(--m_cRef == 0)
    {
        delete this;
        return 0;
    }
    return m_cRef;
}

/*
 * CEnumVariant::Next
 *
 * Purpose:
 *  Retrieves the next cElements elements. Implements IEnumVARIANT::Next. 
 *
 */
STDMETHODIMP
CEnumVariant::Next(ULONG cElements, VARIANT FAR* pvar, ULONG FAR* pcElementFetched)
{ 
    HRESULT hr;
    ULONG l;
    long l1;
    ULONG l2;
    
    if (pcElementFetched != NULL)
        *pcElementFetched = 0;
    
    for (l=0; l<cElements; l++)
        VariantInit(&pvar[l]);
        
    // Retrieve the next cElements elements.
    for (l1=m_lCurrent, l2=0; l1<(long)(m_lLBound+m_cElements) && l2<cElements; l1++, l2++)
    {
       hr = SafeArrayGetElement(m_psa, &l1, &pvar[l2]); 
       if (FAILED(hr))
           goto error; 
    }
    // Set count of elements retrieved
    if (pcElementFetched != NULL)
        *pcElementFetched = l2;
    m_lCurrent = l1;
    
    return  (l2 < cElements) ? ResultFromScode(S_FALSE) : NOERROR;

error:
    for (l=0; l<cElements; l++)
        VariantClear(&pvar[l]);
    return hr;    
}

/*
 * CEnumVariant::Skip
 *
 * Purpose:
 *  Skips the next cElements elements. Implements IEnumVARIANT::Skip. 
 *
 */
STDMETHODIMP
CEnumVariant::Skip(ULONG cElements)
{   
    m_lCurrent += cElements; 
    if (m_lCurrent > (long)(m_lLBound+m_cElements))
    {
        m_lCurrent =  m_lLBound+m_cElements;
        return ResultFromScode(S_FALSE);
    } 
    else return NOERROR;
}

/*
 * CEnumVariant::Reset
 *
 * Purpose:
 *  Resets the current element in the enumerator to the beginning. Implements IEnumVARIANT::Reset. 
 *
 */
STDMETHODIMP
CEnumVariant::Reset()
{ 
    m_lCurrent = m_lLBound;
    return NOERROR;
}

/*
 * CEnumVariant::Clone
 *
 * Purpose:
 *  Creates a copy of the current enumeration state. Implements IEnumVARIANT::Clone. 
 *
 */
STDMETHODIMP
CEnumVariant::Clone(IEnumVARIANT FAR* FAR* ppenum)
{
    CEnumVariant FAR* penum = NULL;
    HRESULT hr;
    
    *ppenum = NULL;
    
    hr = CEnumVariant::Create(m_psa, m_cElements, &penum);
    if (FAILED(hr))
        goto error;        
    penum->AddRef();
    penum->m_lCurrent = m_lCurrent; 
    
    *ppenum = penum;        
    return NOERROR;
     
error:
    if (penum)
        penum->Release();
    return hr;        
}


