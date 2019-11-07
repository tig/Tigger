// object.cpp
//
// Implemenation of CStocksObject
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
// October 29, 1993
//
// Internet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  October 29, 1993 cek Created
//
// CStocksObject is derived from IUnknown and is a controlling
// unknown for an implementation of IDataObject which is implemented
// here as CImpIDataObject.
//
// The IDataObject object implemented here supports multiple 
// DAdvise clients via IDataAdviseHolder.
//
// Two clipboard formats are supported:  CF_TEXT and
// "CF_XRTDEMO".
//
// The semantics of IDataObject are slightly modified when this
// object is used in a DAdvise scenario:
//
//      Direct calls to IDataObject::GetData() result in
//      the entire data set being transferred.   For both
//      CF_TEXT and "CF_XRTDEMO" when a container calls
//      ::GetData() he gets the latest information about all
//      stocks in the database.
//
//      When a container is being notified of data changes
//      (through IAdviseSink::OnDataChange) and the container
//      has NOT used the ADVF_NODATA flag, the data that
//      accompanies the OnDataChange represents only the
//      items that have changed since the last OnDataChange.
//      (note:  this is only true for "CF_XRTDEMO"; CF_TEXT
//      always transfers everything.
//
//      This behavior is controlled by the m_fFullData
//      member.
//

#include "precomp.h"
#include "xrtobj32.h"
#include "object.h"
#include "simobj.h"
#include "ienumfe.h"
#include "iperfile.h"

#define FE_XRTDEMO      0
#define FE_XRTDEMO2     1
#define FE_TEXT         2

static FORMATETC g_rgfeGet[] =
{  
  //cfFormat,       ptd,    dwAspect,           lindex, tymed
    0,              NULL,   DVASPECT_CONTENT,   -1,     TYMED_HGLOBAL,
    0,              NULL,   DVASPECT_CONTENT,   -1,     TYMED_ISTREAM,
    CF_UNICODETEXT, NULL,   DVASPECT_CONTENT,   -1,     TYMED_HGLOBAL,
} ;
static ULONG g_cfeGet = sizeof( g_rgfeGet ) / sizeof( g_rgfeGet[0] ) ;

// Constructor
//
CStocksObject::CStocksObject( LPUNKNOWN punkOuter, PFNCREATED pfnCreated ) 
{
    m_uiSimObjID = 0 ;
    m_pStocks = NULL ;

    m_pfnCreated = pfnCreated ;
    
    m_cRef = 0 ;
    m_punkOuter = punkOuter ;

    //NULL any contained interfaces initially.
    m_pIDataObject = NULL ;
    m_pIPersistFile = NULL ;
    m_pIDataAdviseHolder = NULL ;
    m_fFullData = TRUE ;        // ::GetData uses to differentiate
                                // between an advise or direct GetData

    m_lpStorage = NULL ;
    // Initilize the FORMATETC we use for ::EnumFormatEtc
#ifdef REGISTER_FORMAT
    CLIPFORMAT cf = RegisterClipboardFormat( L"CF_XRTDEMO" ) ;
#else
    CLIPFORMAT cf = 0x8FFF ;
#endif
    SETDefFormatEtc( g_rgfeGet[FE_XRTDEMO], cf, g_rgfeGet[FE_XRTDEMO].tymed) ;
    SETDefFormatEtc( g_rgfeGet[FE_XRTDEMO2], cf, g_rgfeGet[FE_XRTDEMO2].tymed) ;

    // Tell the main application code that we've been
    // created
    //
    m_pfnCreated( this, TRUE ) ;

    return ;
}


CStocksObject::~CStocksObject(void)
{
    if (NULL!=m_pIDataAdviseHolder)
        m_pIDataAdviseHolder->Release();

    if (NULL!=m_pIDataObject)
        delete m_pIDataObject;

    if (NULL!=m_pIPersistFile)
        delete m_pIPersistFile ;

    g_pSimObj->UnRegisterClient( m_uiSimObjID ) ;

    // Tell the main application code that we've been
    // destroyed
    //
    m_pfnCreated( this, FALSE ) ;    

    if (m_pStocks != NULL)
        delete m_pStocks ;

    if (m_lpStorage != NULL)
        m_lpStorage->Release() ;

    return;
}

// CStocksObject::Init
//
// Purpose:
//  Performs any intiailization of a CStocksObject that's prone to failure
//  that we also use internally before exposing the object outside.
//
// Return Value:
//  BOOL            TRUE if the function is successful, FALSE otherwise.
//
BOOL CStocksObject::Init(void)
{
    LPUNKNOWN       pIUnknown=(LPUNKNOWN)this;

    if (NULL != m_punkOuter)
        pIUnknown = m_punkOuter;

    //Allocate contained interfaces.
    m_pIDataObject = new CImpIDataObject( this, pIUnknown ) ;
    if (NULL == m_pIDataObject)
        return FALSE ;

    m_pIPersistFile = new CImpIPersistFile( (LPVOID)this, pIUnknown ) ;
    if (NULL == m_pIPersistFile)
        return FALSE ;

    // Register ourselves with the data feed simulation
    //
    m_uiSimObjID = g_pSimObj->RegisterClient( (LPVOID)this ) ;

    // FORNOW:  Get entire data set from simulation.
    // In the future, when this object implements IDispatch,
    // the m_pStocks member will be filled out via requests on
    // IDispatch.
    //
    PXRTSTOCKS p = g_pSimObj->GetData() ;
    if (m_pStocks != NULL)
        delete m_pStocks ;
    m_pStocks = (PXRTSTOCKS)new BYTE[sizeof(XRTSTOCKS) + (sizeof(XRTSTOCK)*p->cStocks)] ;
    memcpy( m_pStocks, p, sizeof(XRTSTOCKS) + (sizeof(XRTSTOCK)*p->cStocks) ) ;
    
    return TRUE;
}

// CStocksObject::OnMarketChange()
//
// This member is called by the simulation object (CSimulation) when
// ever the 'market has changed'.   In other words, each time the
// simulation generates a new set of random stock changes it calls the
// OnMarketChange member of all objects which have registered with it.
//
void CStocksObject::OnMarketChange()
{
    PXRTSTOCKS pData ;
    pData = g_pSimObj->GetData() ;

    // See which stocks changed and update our own 
    // database
    //
    for (UINT i = 0 ; i < m_pStocks->cStocks ; i++)
    {
        if (pData->rgStocks[i].uiMembers)
            m_pStocks->rgStocks[i] = pData->rgStocks[i] ;
    }

    //#ifdef _DEBUG
    //OutputDebugString(TEXT("OnMarketChange\r\n")); 
    //#endif
}

// CStocksObject::OnUpdateTick()
//
// This member is called from the main window procedure on the
// WM_TIMER that indicates that it is time to send OnDataChange
// notifcations to IAdviseSinks.  
//
// IDataAdviseHolder is used to manage the sinks who have
// DAdvise()'d.
//
void CStocksObject::OnUpdateTick()
{
    // Set the FullData flag to indicate to the GetData implemenation
    // that it should only send changed data (if "CF_XRTDEMO" is the
    // clipboard format).
    //
    m_fFullData = FALSE ;

    if (m_pIDataAdviseHolder != NULL)
        m_pIDataAdviseHolder->SendOnDataChange( m_pIDataObject, 0L, 0L ) ;

    // Reset the 'changed' flags on each stock
    //
    for (UINT i = 0 ; i < m_pStocks->cStocks ; i++)
    {
        m_pStocks->rgStocks[i].uiMembers = 0 ;
    }

    // Do this so that normal calls to our GetData imp. will result
    // in all data being transferred rather than just that which
    // has changed.
    //
    m_fFullData = TRUE ;

    //#ifdef _DEBUG
    //OutputDebugString(TEXT("OnUpdateTick\r\n")); 
    //#endif

}

// CStocksObject::QueryInterface
// CStocksObject::AddRef
// CStocksObject::Release
//
// Purpose:
//  IUnknown members for CStocksObject object.
//
STDMETHODIMP CStocksObject::QueryInterface( REFIID riid, LPVOID FAR *ppv )
{
    *ppv = NULL ;

    if (IsEqualIID(riid, IID_IUnknown))
        *ppv=(LPVOID)this;

    if (IsEqualIID(riid, IID_IDataObject))
        *ppv=(LPVOID)m_pIDataObject;

    if (IsEqualIID(riid, IID_IPersistFile))
        *ppv=(LPVOID)m_pIPersistFile;

    // AddRef any interface we'll return.
    if (NULL!=*ppv)
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }

    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) CStocksObject::AddRef(void)
{
    return ++m_cRef;
}


STDMETHODIMP_(ULONG) CStocksObject::Release(void)
{
    ULONG   cRefT;

    cRefT = --m_cRef;

    if (0 == m_cRef)
    {   
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
CImpIDataObject::CImpIDataObject( CStocksObject* pObj, LPUNKNOWN punkOuter)
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
    UINT            cf = pFE->cfFormat;

    /*
     * This function is just cycling through each format you support
     * and finding a match with the requested one, rendering that
     * data, then returning NOERROR, otherwise returning either
     * DATA_E_FORMATETC or STG_E_MEDIUMFULL on error.
     */

    //Check the aspects we support.
    if (!(DVASPECT_CONTENT & pFE->dwAspect))
        return ResultFromScode(DATA_E_FORMATETC);

    if (cf == CF_UNICODETEXT && TYMED_HGLOBAL & pFE->tymed)
        return m_pObj->RenderCF_TEXT(pSTM);

    if (cf == g_rgfeGet[FE_XRTDEMO].cfFormat && 
        g_rgfeGet[FE_XRTDEMO].tymed & pFE->tymed)
        return m_pObj->RenderCF_XRTDEMO( pSTM, (TYMED)g_rgfeGet[FE_XRTDEMO].tymed) ;

    if (cf == g_rgfeGet[FE_XRTDEMO2].cfFormat && 
        g_rgfeGet[FE_XRTDEMO2].tymed & pFE->tymed)
        return m_pObj->RenderCF_XRTDEMO( pSTM, (TYMED)g_rgfeGet[FE_XRTDEMO2].tymed ) ;

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
        case CF_UNICODETEXT:
            fRet=(BOOL)(pFE->tymed & TYMED_HGLOBAL);
        break;

        default:
            if (cf == g_rgfeGet[FE_XRTDEMO].cfFormat && 
                    g_rgfeGet[FE_XRTDEMO].tymed & pFE->tymed)
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
    // We don't handle SetDatas yet.
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
STDMETHODIMP CImpIDataObject::DAdvise( LPFORMATETC pFE, DWORD dwFlags, LPADVISESINK pIAdviseSink, LPDWORD pdwConn )
{
    HRESULT         hr;

    #ifdef _DEBUG
    OutputDebugString( TEXT("CImpIDataObject::DAdvise\r\n") ) ;
    #endif

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

    if (NULL == m_pObj->m_pIDataAdviseHolder)
    {
        hr=CreateDataAdviseHolder(&m_pObj->m_pIDataAdviseHolder);

        if (FAILED(hr))
            return ResultFromScode(E_OUTOFMEMORY);
    }

    hr = m_pObj->m_pIDataAdviseHolder->Advise( (LPDATAOBJECT)this, pFE, 
                                            dwFlags, pIAdviseSink, pdwConn);

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
    HRESULT         hr;

    #ifdef _DEBUG
    OutputDebugString( TEXT("CImpIDataObject::DUndvise\r\n") ) ;
    #endif

    /*
     *  1.  If you have stored and advise holder from IDataObject::Advise
     *      then pass dwConn to it's Unadvise and return the HRESULT from
     *      that function.
     *  2.  If you have no advise holder, return E_FAIL;
     */

    if (NULL == m_pObj->m_pIDataAdviseHolder)
        return ResultFromScode(E_FAIL);

    hr = m_pObj->m_pIDataAdviseHolder->Unadvise(dwConn);

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
    HRESULT         hr;

    /*
     *  1.  If you have a stored IDataAdviseHolder, pass this request on
     *      to its EnumAdvise and return the HRESULT from that function.
     *  2.  Otherwise return E_FAIL.
     */

    if (NULL == m_pObj->m_pIDataAdviseHolder)
        return ResultFromScode(E_FAIL);

    hr = m_pObj->m_pIDataAdviseHolder->EnumAdvise(ppEnum);
    return hr;
}

// CStocksObject::RenderCF_TEXT
//
// Helper to ::GetData.
//
// Renders the data in CF_UNICODETEXT format to the STGMEDIUM.
//
// Always renders ALL data (ignores m_fFullData flag).
//
HRESULT CStocksObject::RenderCF_TEXT(LPSTGMEDIUM pSTM)
{
    HGLOBAL     hMem;
    LPTSTR       psz;
    
    //Get the size of data we're dealing with
    hMem=GlobalAlloc( GMEM_SHARE | GMEM_MOVEABLE, m_pStocks->cStocks * 64 );

    if (NULL==hMem)
        return ResultFromScode(STG_E_MEDIUMFULL);
                    
                    
    // for CF_UNICODETEXT we transfer the ENTIRE update list.  This is per the XRT spec.
    //                    
    psz=(LPTSTR)GlobalLock(hMem);

    LPXRTSTOCK lpQ ;
    for ( UINT n = 0 ; n < m_pStocks->cStocks ; n ++ )
    {
        lpQ = &m_pStocks->rgStocks[n] ;
        wsprintf( psz, L"%s\t%lu.%02lu\t%lu.%02lu\t%lu.%02lu\t%lu\r\n",     
                    (LPTSTR)lpQ->szSymbol,
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

// CStocksObject::RenderCF_XRTDEMO
//
// Helper to ::GetData.
//
// Renders the data in "CF_XRTDEMO" format to the STGMEDIUM.
//
// Uses the m_fFullData flag to determine whether all data
// should be rendered or just the data that has non-zero
// uiMembers.   m_fFullData will be FALSE whenever GetData
// was called by IDataAdviseHolder::SendOnDataChange (ie
// when we are notifying a client of a data change).
//
//
HRESULT CStocksObject::RenderCF_XRTDEMO(LPSTGMEDIUM pSTM, TYMED tymed)
{
    UINT nSize = 0 ;
    HGLOBAL hmem = NULL ;
    HRESULT hr = NOERROR ;

//    OutputDebugString( L"RenderCF_XRTDEMO\r\n" ) ;

    if (m_pStocks == NULL)
        return ResultFromScode(DATA_E_FORMATETC);

    UINT    i, c = 0 ;
    if (m_fFullData == TRUE)
        c = m_pStocks->cStocks ;
    else        
        for (i = 0 ; i < m_pStocks->cStocks ; i++)
        {
            if (m_pStocks->rgStocks[i].uiMembers)
                c++ ;
        }

    // For each item that has changed data (i.e. uiMembers is non-NULL)
    // append to a transfer buffer      
    //
    hmem = GlobalAlloc( GMEM_SHARE | GHND, sizeof(XRTSTOCKS) + (c*sizeof(XRTSTOCK)) );

    if (hmem==NULL)
        return ResultFromScode(STG_E_MEDIUMFULL);
        
    PXRTSTOCKS pS = (PXRTSTOCKS)GlobalLock( hmem ) ;
    PXRTSTOCK  p = &pS->rgStocks[0] ;
    pS->cStocks = 0 ;
    for (i = 0 ; i < m_pStocks->cStocks ; i++)
    {
        if (m_pStocks->rgStocks[i].uiMembers || m_fFullData == TRUE)
        {
            pS->rgStocks[pS->cStocks] = m_pStocks->rgStocks[i] ;
            pS->cStocks++ ;
        }
    }

    pSTM->tymed = tymed ;

    if (tymed == TYMED_HGLOBAL)
    {
        GlobalUnlock( hmem ) ;
        pSTM->hGlobal = hmem ;
    }
    else
    {

        #if 1
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

        hr = m_lpStorage->CreateStream( L"CONTENTS", 
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

    }

    return hr ;
}




