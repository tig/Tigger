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
//  August 16, 1993   cek     First implementation.
//
// Implementation Notes:
//
//  CXRTDataObject is derived from IUnknown and is an aggregate containing
//  a IDataObject (implemented by CImpIDataObject) and an IDataAdviseHolder
//  (implemented by OLE2).
//
//  CXRTDataObject is wed to CXRTObjectView:  It has a member pointer
//  to the CXRTObjectView instance that created it, so that it can
//  tell it that it was destroyed.
//

#include "stdafx.h"
#include "xrt_obj.h"
#include "dataobj.h"           
#include "ienumfe.h"
#include "iperstor.h"
#include "iperfile.h"
#include "ioleobj.h"

#define FE_XRTSTOCKS    0
#define FE_TEXT         1
#define FE_METAPICT     2

static FORMATETC g_rgfeGet[] =
{  
  //cfFormat,       ptd,    dwAspect,           lindex, tymed
#ifdef USE_HGLOBAL  
    0,              NULL,   DVASPECT_CONTENT,   -1,     TYMED_HGLOBAL,
#else    
    0,              NULL,   DVASPECT_CONTENT,   -1,     TYMED_ISTREAM,
#endif
    CF_TEXT,        NULL,   DVASPECT_CONTENT,   -1,     TYMED_HGLOBAL,
//    CF_METAFILEPICT,NULL,   DVASPECT_CONTENT,   -1,     TYMED_MFPICT,
} ;

static ULONG g_cfeGet = sizeof( g_rgfeGet ) / sizeof( g_rgfeGet[0] ) ;

CXRTDataObject::CXRTDataObject( LPUNKNOWN punkOuter, CXRTObjectView* pBack ) 
{
    m_pXRTObjectView = pBack ;
    
    m_cRef = 0 ;
    m_punkOuter = punkOuter ;

    //NULL any contained interfaces initially.
    m_pIDataObject = NULL ;
    m_pIPersistStorage = NULL ;
    m_pIPersistFile = NULL ;
    m_pIOleObject = NULL ;
    m_pIDataAdviseHolder = NULL ;
    m_lpUpdateList = NULL ;

    // Initilize the FORMATETC we use for ::EnumFormatEtc
    
    CLIPFORMAT cf = RegisterClipboardFormat( "CF_XRTDEMO" ) ;
    SETDefFormatEtc( g_rgfeGet[FE_XRTSTOCKS], cf, g_rgfeGet[FE_XRTSTOCKS].tymed ) ;

    m_lpStorage = NULL ;

    return ;
}


CXRTDataObject::~CXRTDataObject(void)
{
    if (m_lpUpdateList != NULL)
    {
        GlobalFreePtr( m_lpUpdateList ) ;
        m_lpUpdateList = NULL ;
    }    

    if (NULL!=m_pIDataAdviseHolder)
        m_pIDataAdviseHolder->Release();
    
    if (NULL!=m_pIOleObject)
        delete m_pIOleObject ;

    if (NULL!=m_pIPersistStorage)
        delete m_pIPersistStorage ;

    if (NULL!=m_pIPersistFile)
        delete m_pIPersistFile ;

    if (NULL!=m_pIDataObject)
        delete m_pIDataObject;

    if (m_lpStorage != NULL)
        m_lpStorage->Release() ;
    
    return;
}

// CXRTDataObject::Init
//
// Purpose:
//  Performs any intiailization of a CXRTDataObject that's prone to failure
//  that we also use internally before exposing the object outside.
//
// Return Value:
//  BOOL            TRUE if the function is successful, FALSE otherwise.
//
BOOL CXRTDataObject::Init(void)
{
    LPUNKNOWN       pIUnknown=(LPUNKNOWN)this;

    if (NULL != m_punkOuter)
        pIUnknown = m_punkOuter;

    //Allocate contained interfaces.
    m_pIDataObject = new CImpIDataObject( this, pIUnknown ) ;
    if (NULL == m_pIDataObject)
        return FALSE ;

    m_pIPersistStorage = new CImpIPersistStorage( this, pIUnknown ) ;
    if (NULL == m_pIPersistStorage)
        return FALSE ;

    m_pIPersistFile = new CImpIPersistFile( this, pIUnknown ) ;
    if (NULL == m_pIPersistFile)
        return FALSE ;
        
    m_pIOleObject = new CImpIOleObject( this, pIUnknown ) ;
    if (NULL == m_pIOleObject)
        return FALSE ;
    
    return TRUE;
}

// ::DataHasChanged
//
// This is called by the View that owns the instance of the object to tell
// us that the main data store has changed.  
//
// We update our instanced data from the main data store, and if there were
// any changes that affected us, we do a OnDataChange to each of our
// clients by using IDataAdviseHolder
//
void CXRTDataObject::DataHasChanged( BOOL fPrime )
{
    // Update our instance list of data
    CXRTData*   pData = m_pXRTObjectView->GetDocument() ;
    LPXRTSTOCKS  lpDocQuotes = pData->m_lpQuotes ;
    UINT    n = sizeof(XRTSTOCKS)+(lpDocQuotes->cStocks * sizeof(XRTSTOCK)) ;
    
    // If we're priming the pump then free any list we have already,
    // and copy the master list over completely.   (note that 
    // we do not implement the XRT Programmmability interface (IDispatch) yet,
    // and thus use the entire master list).
    //
    if (fPrime)
    {
        if (m_lpUpdateList != NULL)
        {
            GlobalFreePtr( m_lpUpdateList ) ;
            m_lpUpdateList = NULL ;
        }    
        
        //Get the size of data we're dealing with and allocate our

        m_lpUpdateList = (LPXRTSTOCKS)GlobalAllocPtr( GHND, n ) ;
    
        if (NULL==m_lpUpdateList)
            return ;
    
        // For now we just transfer everything     
        _fmemcpy( m_lpUpdateList, lpDocQuotes, n ) ;

        // reset changed data flags
        //
        for ( n = 0 ; n < m_lpUpdateList->cStocks ; n ++ )
        {
            m_lpUpdateList->rgStocks[n].uiMembers = XSM_ALL ;
        }
        m_fGotData = FALSE ;
    }
    else
    {
        UINT ui = 0 ;
        
        // For now we just transfer everything     
        _fmemcpy( m_lpUpdateList, lpDocQuotes, n ) ;
        
        // Our client may not have asked for any data yet.
        // if so then we must keep the 'all changed' bits set until
        // he does.
        if (m_fGotData == FALSE)
        {
            for ( UINT i = 0 ; i < m_lpUpdateList->cStocks ;i ++ )
                m_lpUpdateList->rgStocks[i].uiMembers = XSM_ALL ;
            ui = XSM_ALL ;
        }
        else
            for ( UINT i = 0 ; i < m_lpUpdateList->cStocks ;i ++ )
                ui |= m_lpUpdateList->rgStocks[i].uiMembers ;
            
        if (ui == 0)
            return ;    // nothing has changed.
    }    
    
    if (m_pIDataAdviseHolder != NULL)
        m_pIDataAdviseHolder->SendOnDataChange( m_pIDataObject, 0, m_advf ) ;
}

BOOL CXRTDataObject::IsDirty() 
{
    return FALSE ;
}                 

// CXRTDataObject::QueryInterface
// CXRTDataObject::AddRef
// CXRTDataObject::Release
//
// Purpose:
//  IUnknown members for CXRTDataObject object.
//
STDMETHODIMP CXRTDataObject::QueryInterface( REFIID riid, LPVOID FAR *ppv )
{
    *ppv = NULL ;

    if (IsEqualIID(riid, IID_IUnknown))
        *ppv=(LPVOID)this;

    if (IsEqualIID(riid, IID_IDataObject))
        *ppv=(LPVOID)m_pIDataObject;

    if (IsEqualIID(riid, IID_IPersist) || IsEqualIID(riid, IID_IPersistStorage))
        *ppv = (LPVOID)m_pIPersistStorage ;

    if (IsEqualIID(riid, IID_IPersistFile))
        *ppv = (LPVOID)m_pIPersistFile ;

    if (IsEqualIID(riid, IID_IOleObject))
        *ppv = (LPVOID)m_pIOleObject ;
        
    //AddRef any interface we'll return.
    if (NULL!=*ppv)
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }

    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) CXRTDataObject::AddRef(void)
{
    return ++m_cRef;
}


STDMETHODIMP_(ULONG) CXRTDataObject::Release(void)
{
    ULONG       cRefT;

    cRefT = --m_cRef;

    if (0 == m_cRef)
    {   
        // We have to tell the window that created us that
        // we're being destroyed.
        // 
        m_pXRTObjectView->DestroyDataObject( NULL ) ;
        delete this ;
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
    LPCXRTDataObject    pObj = (LPCXRTDataObject)m_pObj;
    UINT                cf = pFE->cfFormat;

    /*
     * This function is just cycling through each format you support
     * and finding a match with the requested one, rendering that
     * data, then returning NOERROR, otherwise returning either
     * DATA_E_FORMATETC or STG_E_MEDIUMFULL on error.
     */

    //Check the aspects we support.
    if (!(DVASPECT_CONTENT & pFE->dwAspect))
        return ResultFromScode(DATA_E_FORMATETC);

    if (cf == CF_TEXT && TYMED_HGLOBAL & pFE->tymed)
        return pObj->RenderCF_TEXT(pSTM);

    if (cf == g_rgfeGet[FE_XRTSTOCKS].cfFormat && 
        g_rgfeGet[FE_XRTSTOCKS].tymed & pFE->tymed)
        return pObj->RenderCF_XRTSTOCKS(pSTM);

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
    if (pFE == NULL)
        return ResultFromScode(S_FALSE);
    
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

        default:
            if (cf == g_rgfeGet[FE_XRTSTOCKS].cfFormat && 
                g_rgfeGet[FE_XRTSTOCKS].tymed & pFE->tymed)
                fRet = TRUE ;
            else
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
STDMETHODIMP CImpIDataObject::EnumFormatEtc(DWORD dwDir, LPENUMFORMATETC FAR *ppEnum)
{
    /*
     * We only support ::GetData in this object so we return NULL for
     * DATADIR_SET.  Otherwise we instantiate one of our CImpEnumFORMATETC
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
            *ppEnum=(LPENUMFORMATETC)new CImpEnumFORMATETC( m_punkOuter, 
                                                            g_cfeGet, g_rgfeGet);
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
STDMETHODIMP CImpIDataObject::DAdvise(LPFORMATETC pFE, DWORD dwFlags, LPADVISESINK pIAdviseSink, LPDWORD pdwConn)
{
    LPCXRTDataObject    pObj=(LPCXRTDataObject)m_pObj;
    HRESULT             hr;

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

    pObj->m_advf = dwFlags ;
    hr=pObj->m_pIDataAdviseHolder->Advise( (LPDATAOBJECT)this, pFE, 
                                            dwFlags, pIAdviseSink, pdwConn);

//    if (dwFlags & ADVF_PRIMEFIRST)
//        pObj->m_pIDataAdviseHolder->SendOnDataChange( pObj->m_pIDataObject, 0, ADVF_NODATA ) ;

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
    LPCXRTDataObject   pObj=(LPCXRTDataObject)m_pObj;
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
    LPCXRTDataObject   pObj=(LPCXRTDataObject)m_pObj;
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

HRESULT CXRTDataObject::RenderCF_TEXT(LPSTGMEDIUM pSTM)
{
    HGLOBAL     hMem;
    LPSTR       psz;
    
    //Get the size of data we're dealing with
    hMem=GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, m_lpUpdateList->cStocks * 64);

    if (NULL==hMem)
        return ResultFromScode(STG_E_MEDIUMFULL);
                    
                    
    // for CF_TEXT we transfer the ENTIRE update list.  This is per the XRT spec.
    //                    
    psz=(LPSTR)GlobalLock(hMem);
    LPXRTSTOCK lpQ ;
    for ( UINT n = 0 ; n < m_lpUpdateList->cStocks ; n ++ )
    {
        lpQ = &m_lpUpdateList->rgStocks[n] ;
        wsprintf( psz, "%s\t%lu.%02lu\t%lu.%02lu\t%lu.%02lu\t%lu\r\n",     
                    (LPSTR)lpQ->szSymbol,
                    lpQ->ulHigh/1000L, lpQ->ulHigh%1000L,
                    lpQ->ulLow/1000L, lpQ->ulLow%1000L,
                    lpQ->ulLast/1000L, lpQ->ulLast%1000L,
                    lpQ->ulVolume
                     ) ;
        
        psz += lstrlen( psz ) ;        
    }

    GlobalUnlock(hMem);

    pSTM->hGlobal=hMem;
    pSTM->tymed=TYMED_HGLOBAL;
    return NOERROR;
}

HRESULT CXRTDataObject::RenderCF_METAFILEPICT(LPSTGMEDIUM pSTM)
{
    return ResultFromScode(DATA_E_FORMATETC) ;
}

HRESULT CXRTDataObject::RenderCF_XRTSTOCKS(LPSTGMEDIUM pSTM)
{
    UINT nSize = 0 ;
    HGLOBAL hmem = NULL ;
    HRESULT hr = NOERROR ;

    if (m_lpUpdateList == NULL)
        return ResultFromScode(DATA_E_FORMATETC);

    // For each item that has changed data (i.e. uiMembers is non-NULL)
    // append to a transfer buffer      
    //
    hmem = GlobalAlloc( GMEM_SHARE | GHND, nSize = sizeof(XRTSTOCKS) );

    if (hmem==NULL)
        return ResultFromScode(STG_E_MEDIUMFULL);
        
    LPXRTSTOCKS pS = (LPXRTSTOCKS)GlobalLock( hmem ) ;
    LPXRTSTOCK  p = &pS->rgStocks[0] ;
         
    pS->cStocks = 0 ;
    for (UINT i = 0 ; i < m_lpUpdateList->cStocks ; i++)
    {
        if (m_lpUpdateList->rgStocks[i].uiMembers)
        {
            // Note that in this sample we copy an entire XRTSTOCK structure even if
            // only one attribute of the item changed (i.e. only the Last price).
            //
            // When the CF_XRTMARKETDATA structure is defined by the specification
            // it will allow us to only copy an ID, attribute, and attribute type, minimizing
            // the amount of data transferred.
            //
            GlobalUnlock( hmem ) ;
            hmem = GlobalReAlloc( hmem, nSize += sizeof(XRTSTOCK), GMEM_SHARE | GHND ) ;
            pS = (LPXRTSTOCKS)GlobalLock( hmem ) ;
            
            _fmemcpy( p + pS->cStocks, &m_lpUpdateList->rgStocks[i], sizeof(XRTSTOCK) ) ;
            pS->cStocks++ ;
        }
    }
    
    #ifdef USE_HGLOBAL
    GlobalUnlock( hmem ) ;
    pSTM->hGlobal = hmem ;
    pSTM->tymed = TYMED_HGLOBAL;
    #else

    #if 0
    if (m_lpStorage == NULL)
    {
        hr = StgCreateDocfile( NULL,
                                STGM_TRANSACTED |
                                STGM_CREATE | 
                                STGM_READWRITE | 
                                STGM_SHARE_DENY_NONE | 
                                STGM_DELETEONRELEASE |
                                0, 
                                0, &m_lpStorage ) ;
        if (FAILED(hr))
        {
            m_lpStorage = NULL ;
            GlobalUnlock( hmem ) ;
            GlobalFree( hmem ) ;
            return hr;
        }
    }

    hr = m_lpStorage->CreateStream( "CONTENTS", 
                                    //STGM_TRANSACTED |
                                    //STGM_PRIORITY |
                                    STGM_CREATE |
                                    STGM_SHARE_EXCLUSIVE |
                                    STGM_WRITE |
                                    STGM_READ | 
                                    0, 
                                    0, 0, &pSTM->pstm ) ;

    if (SUCCEEDED(hr))
    {
        hr = pSTM->pstm->Write( pS, (pS->cStocks * sizeof(XRTSTOCK)) + sizeof(XRTSTOCKS), NULL ) ;
        if (SUCCEEDED(hr))
            pSTM->pstm->Commit(0) ;
    }

    GlobalUnlock( hmem ) ;
    GlobalFree( hmem ) ;

    #else
    GlobalUnlock( hmem ) ;
    hr = CreateStreamOnHGlobal( hmem, TRUE, &pSTM->pstm ) ;
    #endif

    pSTM->tymed = TYMED_ISTREAM;

    #endif

    m_fGotData = TRUE ;
    
    return hr;
}

