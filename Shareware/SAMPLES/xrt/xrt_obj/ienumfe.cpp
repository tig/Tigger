/*
 * IENUMFE.CPP
 * Data Object for Chapter 6
 *
 * Full implementation of the IEnumFORMATETC interface that can be
 * used generically for any FORMATETC enumerations given that the
 * caller passes a count and pointer to array of FORMATETCs for this
 * enumeration.
 *
 * Copyright (c)1993 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */

#include "stdafx.h"
#include "xrt_obj.h"
#include "ienumfe.h"


/*
 * CImpEnumFORMATETC::CImpEnumFORMATETC
 * CImpEnumFORMATETC::~CImpEnumFORMATETC
 *
 * Parameters (Constructor):
 *  punkRef         LPUNKNOWN to use for reference counting.
 *  cFE             ULONG number of FORMATETCs in pFE
 *  prgFE           LPFORMATETC to the array over which to enumerate.
 */

CImpEnumFORMATETC::CImpEnumFORMATETC( LPUNKNOWN punkRef, ULONG cFE, LPFORMATETC prgFE )
{
    UINT        i;

    m_cRef = 0 ;
    m_punkRef = punkRef ;

    m_iCur = 0 ;
    m_cfe = cFE ;

    m_prgfe = new _far FORMATETC[(UINT)cFE];

    if (NULL != m_prgfe)
    {
        for (i=0; i < cFE; i++)
            m_prgfe[i] = prgFE[i] ;
    }

    return;
}


CImpEnumFORMATETC::~CImpEnumFORMATETC(void)
{
    if (NULL != m_prgfe)
        delete [] m_prgfe ;

    return ;
}


/*
 * CImpEnumFORMATETC::QueryInterface
 * CImpEnumFORMATETC::AddRef
 * CImpEnumFORMATETC::Release
 *
 * Purpose:
 *  IUnknown members for CImpEnumFORMATETC object.  For QueryInterface
 *  we only return out own interfaces and not those of the data object.
 *  However, since enumerating formats only makes sense when the data
 *  object is around, we insure that it stays as long as we stay by
 *  calling an outer IUnknown for ::AddRef and ::Release.  But since we
 *  are not controlled by the lifetime of the outer object, we still keep
 *  our own reference count in order to free ourselves.
 */

STDMETHODIMP CImpEnumFORMATETC::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv=NULL;

    /*
     * Enumerators do not live on the same level as the data object, so
     * we only need to support out IUnknown and IEnumFORMATETC interfaces
     * here with no concern for aggregation.
     */
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IEnumFORMATETC))
        *ppv=(LPVOID)this;

    //AddRef any interface we'll return.
    if (NULL!=*ppv)
    {
        ((LPUNKNOWN)*ppv)->AddRef() ;
        return NOERROR;
    }

    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) CImpEnumFORMATETC::AddRef(void)
{
    ++m_cRef;
    m_punkRef->AddRef();
    return m_cRef;
}

STDMETHODIMP_(ULONG) CImpEnumFORMATETC::Release(void)
{
    ULONG       cRefT;

    cRefT=--m_cRef;

    m_punkRef->Release();

    if (0==m_cRef)
        delete this;

    return cRefT;
}


/*
 * CImpEnumFORMATETC::Next
 *
 * Purpose:
 *  Returns the next element in the enumeration.
 *
 * Parameters:
 *  cFE             ULONG number of FORMATETCs to return.
 *  pFE             LPFORMATETC in which to store the returned structures.
 *  pulFE           ULONG FAR * in which to return how many we enumerated.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, S_FALSE otherwise,
 */

STDMETHODIMP CImpEnumFORMATETC::Next(ULONG cFE, LPFORMATETC pFE, ULONG FAR * pulFE)
{
    ULONG               cReturn=0L;

    if (NULL==m_prgfe)
        return ResultFromScode(S_FALSE);

    if (NULL!=pulFE)
        *pulFE=0L;

    if (NULL==pFE || m_iCur >= m_cfe)
        return ResultFromScode(S_FALSE);

    while (m_iCur < m_cfe && cFE > 0)
    {
        *pFE = m_prgfe[m_iCur++];
        pFE-- ;
        cReturn++;
        cFE--;
    }

    if (NULL!=pulFE)
        *pulFE=(cReturn-cFE);

    return NOERROR;
}


/*
 * CImpEnumFORMATETC::Skip
 *
 * Purpose:
 *  Skips the next n elements in the enumeration.
 *
 * Parameters:
 *  cSkip           ULONG number of elements to skip.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, S_FALSE if we could not
 *                  skip the requested number.
 */

STDMETHODIMP CImpEnumFORMATETC::Skip(ULONG cSkip)
{
    if ((m_iCur+cSkip) >= m_cfe)
        return ResultFromScode(S_FALSE);

    m_iCur+=cSkip;
    return NOERROR;
}



/*
 * CImpEnumFORMATETC::Reset
 *
 * Purpose:
 *  Resets the current element index in the enumeration to zero.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         Always NOERROR
 */

STDMETHODIMP CImpEnumFORMATETC::Reset(void)
{
    m_iCur=0;
    return NOERROR;
}


/*
 * CImpEnumFORMATETC::Clone
 *
 * Purpose:
 *  Returns another IEnumFORMATETC with the same state as ourselves.
 *
 * Parameters:
 *  ppEnum          LPENUMFORMATETC FAR * in which to return the new object.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, error code otherwise.
 */

STDMETHODIMP CImpEnumFORMATETC::Clone(LPENUMFORMATETC FAR *ppEnum)
{
    LPIMPENUMFORMATETC    pNew;

    *ppEnum=NULL;

    //Create the clone
    pNew=new CImpEnumFORMATETC(m_punkRef, m_cfe, m_prgfe);

    if (NULL==pNew)
        return ResultFromScode(E_OUTOFMEMORY);

    pNew->AddRef();
    pNew->m_iCur=m_iCur;

    *ppEnum=pNew;
    return NOERROR;
}
