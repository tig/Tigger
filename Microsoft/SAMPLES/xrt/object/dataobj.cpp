// DATAOBJ.CPP
//
// Implementation of DataObject 
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

CDataObject::CDataObject( LPUNKNOWN punkOuter, LPFNDESTROYED pfnDestroy ) 
{
    m_cRef = 0 ;
    m_punkOuter = punkOuter ;
    m_pfnDestroy = pfnDestroy ;

    //NULL any contained interfaces initially.
    m_pIDataObject = NULL ;
    m_pIDataAdviseHolder = NULL ;

    // Initilize the FORMATETC we use for ::EnumFormatEtc
    m_cfeGet = 1 ;

    //These macros are in bookguid.h
    SETDefFormatEtc( m_rgfeGet[0], CF_TEXT, TYMED_HGLOBAL ) ;

    return ;
}


CDataObject::~CDataObject(void)
{
    if (NULL!=m_pIDataAdviseHolder)
        m_pIDataAdviseHolder->Release();

    if (NULL!=m_pIDataObject)
        delete m_pIDataObject;

    return;
}

// CDataObject::FInit
//
// Purpose:
//  Performs any intiailization of a CDataObject that's prone to failure
//  that we also use internally before exposing the object outside.
//
// Return Value:
//  BOOL            TRUE if the function is successful, FALSE otherwise.
//
BOOL CDataObject::Init(void)
{
    LPUNKNOWN       pIUnknown=(LPUNKNOWN)this;

    if (NULL != m_punkOuter)
        pIUnknown = m_punkOuter;

    //Allocate contained interfaces.
    m_pIDataObject = new CImpIDataObject( this, pIUnknown ) ;

    if (NULL==m_pIDataObject)
        return FALSE ;

    return TRUE;
}


// CDataObject::QueryInterface
// CDataObject::AddRef
// CDataObject::Release
//
// Purpose:
//  IUnknown members for CDataObject object.
//
STDMETHODIMP CDataObject::QueryInterface( REFIID riid, LPVOID FAR *ppv )
{
    *ppv = NULL ;

    if (IsEqualIID(riid, IID_IUnknown))
        *ppv=(LPVOID)this;

    if (IsEqualIID(riid, IID_IDataObject))
        *ppv=(LPVOID)m_pIDataObject;

    //AddRef any interface we'll return.
    if (NULL!=*ppv)
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }

    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) CDataObject::AddRef(void)
{
    return ++m_cRef;
}


STDMETHODIMP_(ULONG) CDataObject::Release(void)
{
    ULONG       cRefT;

    cRefT = --m_cRef;

    if (0 == m_cRef)
    {
        /*
         * Tell the housing that an object is going away so it can
         * shut down if appropriate.
         */
        if (NULL!=m_pfnDestroy)
            (*m_pfnDestroy)();

        delete this;
    }

    return cRefT;
}


/////////////////////////////////////////////////////////////////////////////
// CImpIDataObject Implementation
//
//
// CImpIDataObject::CImpIDataObject
// CImpIDataObject::~CImpIDataObject
//
// Parameters (Constructor):
//  pObj            LPVOID of the object we're in.
//  punkOuter       LPUNKNOWN to which we delegate.
//
CImpIDataObject::CImpIDataObject(LPVOID pObj, LPUNKNOWN punkOuter)
{
    m_cRef = 0 ;
    m_pObj = pObj ;
    m_punkOuter = punkOuter ;
    return ;
}

CImpIDataObject::~CImpIDataObject(void)
{
    return ;
}
           
// CImpIDataObject::QueryInterface
// CImpIDataObject::AddRef
// CImpIDataObject::Release
//
// Purpose:
//  IUnknown members for CImpIDataObject object.
//
STDMETHODIMP CImpIDataObject::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    return m_punkOuter->QueryInterface(riid, ppv);
}


STDMETHODIMP_(ULONG) CImpIDataObject::AddRef(void)
{
    ++m_cRef;
    return m_punkOuter->AddRef();
}

STDMETHODIMP_(ULONG) CImpIDataObject::Release(void)
{
    --m_cRef;
    return m_punkOuter->Release();
}


// CImpIDataObject::GetData
//
// Purpose:
//  Retrieves data described by a specific FormatEtc into a StgMedium
//  allocated by this function.  Used like GetClipboardData.
//
// Parameters:
//  pFE             LPFORMATETC describing the desired data.
//  pSTM            LPSTGMEDIUM in which to return the data.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIDataObject::GetData(LPFORMATETC pFE, LPSTGMEDIUM pSTM)
{
    LPCDATAOBJECT   pObj=(LPCDATAOBJECT)m_pObj;
    UINT            cf=pFE->cfFormat;

    /*
     * This function is just cycling through each format you support
     * and finding a match with the requested one, rendering that
     * data, then returning NOERROR, otherwise returning either
     * DATA_E_FORMATETC or STG_E_MEDIUMFULL on error.
     */

    //Check the aspects we support.
    if (!(DVASPECT_CONTENT & pFE->dwAspect))
        return ResultFromScode(DATA_E_FORMATETC);

    switch (cf)
    {

        case CF_TEXT:
            if (!(TYMED_HGLOBAL & pFE->tymed))
                break;

            return pObj->RenderText(pSTM);

        case CF_METAFILEPICT:
//            if (!(TYMED_MFPICT & pFE->tymed))
//                break;
//
//            return pObj->RenderMetafilePict(pSTM);

        case CF_BITMAP:
//            if (!(TYMED_GDI & pFE->tymed))
//                break;
//
//            return pObj->RenderBitmap(pSTM);

        default:
            break;
    }

    return ResultFromScode(DATA_E_FORMATETC);
}


// CImpIDataObject::GetDataHere
//
// Purpose:
//  Renders the specific FormatEtc into caller-allocated medium
//  provided in pSTM.
//
// Parameters:
//  pFE             LPFORMATETC describing the desired data.
//  pSTM            LPSTGMEDIUM providing the medium into which
//                  wer render the data.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIDataObject::GetDataHere(LPFORMATETC pFE, LPSTGMEDIUM pSTM)
{
    //We don't implement this now.
    return ResultFromScode(E_NOTIMPL);
}


// CImpIDataObject::QueryGetData
//
// Purpose:
//  Tests if a call to ::GetData with this FormatEtc will provide
//  any rendering; used like IsClipboardFormatAvailable.
//
// Parameters:
//  pFE             LPFORMATETC describing the desired data.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIDataObject::QueryGetData(LPFORMATETC pFE)
{
    LPCDATAOBJECT   pObj=(LPCDATAOBJECT)m_pObj;
    UINT            cf=pFE->cfFormat;
    BOOL            fRet=FALSE;

    /*
     * This function is just cycling through each format you support
     * and finding a match with the requested one, returning NOERROR
     * if you do have it, S_FALSE if you don't.
     */

    //Check the aspects we support.
    if (!(DVASPECT_CONTENT & pFE->dwAspect))
        return ResultFromScode(S_FALSE);

    switch (cf)
    {
        case CF_TEXT:
            fRet=(BOOL)(pFE->tymed & TYMED_HGLOBAL);
            break;

        case CF_METAFILEPICT:
//            fRet=(BOOL)(pFE->tymed & TYMED_MFPICT);
//            break;

        case CF_BITMAP:
//            fRet=(BOOL)(pFE->tymed & TYMED_GDI);
//            break;

        default:
            fRet=FALSE;
            break;
    }

    return fRet ? NOERROR : ResultFromScode(S_FALSE);
}


// CImpIDataObject::GetCanonicalFormatEtc
//
// Purpose:
//  Provides the caller with an equivalent FormatEtc to the one
//  provided when different FormatEtcs will produce exactly the
//  same renderings.
//
// Parameters:
//  pFEIn            LPFORMATETC of the first description.
//  pFEOut           LPFORMATETC of the equal description.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIDataObject::GetCanonicalFormatEtc(LPFORMATETC pFEIn
    , LPFORMATETC pFEOut)
{
    /*
     *  1.  If you support an equivalent of pFEIn, return it in pFEOut.
     *  2.  Return NOERROR if you filled pFEOut with anything, otherwise
     *      return DATA_S_SAMEFORMATETC.  If you say that all renderings
     *      are identical, return DATA_S_SAMEFORMATETC.
     */

    return ResultFromScode(DATA_S_SAMEFORMATETC);
}
  
  
// CImpIDataObject::SetData
//
// Purpose:
//  Places data described by a FormatEtc and living in a StgMedium
//  into the object.  The object may be responsible to clean up the
//  StgMedium before exiting.
//
// Parameters:
//  pFE             LPFORMATETC describing the data to set.
//  pSTM            LPSTGMEDIUM containing the data.
//  fRelease        BOOL indicating if this function is responsible for
//                  freeing the data.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIDataObject::SetData(LPFORMATETC pFE, STGMEDIUM FAR *pSTM
    , BOOL fRelease)
{
    //We don't handle SetDatas here.
    return ResultFromScode(DATA_E_FORMATETC);
}


// CImpIDataObject::EnumFormatEtc
//
//Purpose:
//  Returns an IEnumFORMATETC object through which the caller can iterate
//  to learn about all the data formats this object can provide through
//  either ::GetData[Here] or ::SetData.
//
// Parameters:
//  dwDir           DWORD describing a data direction, either DATADIR_SET
//                  or DATADIR_GET.
//  ppEnum          LPENUMFORMATETC FAR * in which to return the pointer
//                  to the enumerator.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIDataObject::EnumFormatEtc(DWORD dwDir
    , LPENUMFORMATETC FAR *ppEnum)
{
    LPCDATAOBJECT   pObj=(LPCDATAOBJECT)m_pObj;

    /*
     * We only support ::GetData in this object so we return NULL for
     * DATADIR_SET.  Otherwise we instantiate one of our CEnumFormatEtc
     * objects which has all the appropriate functions; we only need to
     * tell it where our arrays live.
     *
     * The m_punkOuter passed to the enumerator allows the enumerator
     * to AddRef and Release our controlling unknown as it receives
     * reference counting calls.  By calling AddRef itself, it can
     * insure that the object that holds the data context will stick
     * around as long as the enumerator itself is around.  Otherwise
     * it could be enumerating through bogus data.
     */

    switch (dwDir)
        {
        case DATADIR_GET:
            *ppEnum=(LPENUMFORMATETC)new CEnumFormatEtc(m_punkOuter
                , pObj->m_cfeGet, pObj->m_rgfeGet);
             break;

        case DATADIR_SET:
            *ppEnum=NULL;
            break;

        default:
            *ppEnum=NULL;
            break;
        }

    if (NULL==*ppEnum)
        return ResultFromScode(E_FAIL);
    else
        (*ppEnum)->AddRef();

    return NOERROR;
}


// CImpIDataObject::DAdvise
//
// Purpose:
//  Provides the data object with an IAdviseSink object that we are
//  responsible to notify when the data changes.
//
// Parameters:
//  ppFE            LPFORMATETC
//  dwFlags         DWORD carrying flags indicating how the advise sink
//                  wants to be treated.
//  pIAdviseSink    LPADVISESINK to the object to notify on data changes.
//  pdwConn         LPDWORD into which we store a DWORD key identifying
//                  the advise connection.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIDataObject::DAdvise(LPFORMATETC pFE, DWORD dwFlags
    , LPADVISESINK pIAdviseSink, LPDWORD pdwConn)
{
    LPCDATAOBJECT   pObj=(LPCDATAOBJECT)m_pObj;
    HRESULT         hr;

    /*
     *  1.  If you've already created an advise holder for this object,
     *      skip to step 3.  If there are multiple clients connected the
     *      one advise holder takes care of it when we call it's Advise.
     *  2.  Create and save an advise holder interface using
     *      CreateDataAdviseHolder.  Return E_OUTOFMEMORY on failure.
     *  3.  Call the IDataAdviseHolder::Advise member passing to it the
     *      advise sink and pdwConn.
     *  4.  Return the HRESULT from step 3;
     */

    if (NULL==pObj->m_pIDataAdviseHolder)
        {
        hr=CreateDataAdviseHolder(&pObj->m_pIDataAdviseHolder);

        if (FAILED(hr))
            return ResultFromScode(E_OUTOFMEMORY);
        }

    hr=pObj->m_pIDataAdviseHolder->Advise((LPDATAOBJECT)this, pFE
        , dwFlags, pIAdviseSink, pdwConn);

    return hr;
}

// CImpIDataObject::DUnadvise
//
// Purpose:
//  Turns off advising previously set up with ::Advise.
//
// Parameters:
//  dwConn          DWORD connection key returned from ::Advise.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIDataObject::DUnadvise(DWORD dwConn)
{
    LPCDATAOBJECT   pObj=(LPCDATAOBJECT)m_pObj;
    HRESULT         hr;

    /*
     *  1.  If you have stored and advise holder from IDataObject::Advise
     *      then pass dwConn to it's Unadvise and return the HRESULT from
     *      that function.
     *  2.  If you have no advise holder, return E_FAIL;
     */

    if (NULL==pObj->m_pIDataAdviseHolder)
        return ResultFromScode(E_FAIL);

    hr=pObj->m_pIDataAdviseHolder->Unadvise(dwConn);

    return hr;
}


// CImpIDataObject::EnumDAdvise
//
// Purpose:
//  Returns an enumerator object through which the caller can find all
//  the agents currently receiving advises on this data object.
//
// Parameters:
//  ppEnum          LPENUMSTATDATA FAR * in which to return the enumerator.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIDataObject::EnumDAdvise(LPENUMSTATDATA FAR *ppEnum)
{
    LPCDATAOBJECT   pObj=(LPCDATAOBJECT)m_pObj;
    HRESULT         hr;

    /*
     *  1.  If you have a stored IDataAdviseHolder, pass this request on
     *      to its EnumAdvise and return the HRESULT from that function.
     *  2.  Otherwise return E_FAIL.
     */

    if (NULL==pObj->m_pIDataAdviseHolder)
        return ResultFromScode(E_FAIL);

    hr=pObj->m_pIDataAdviseHolder->EnumAdvise(ppEnum);
    return hr;
}

