//
// IPERSTOR.CPP
//
// Template implementation of an IPersistStorage interface.
//
// Copyright (c)1993 Microsoft Corporation, All Rights Reserved
//
// Kraig Brockschmidt, Software Design Engineer
// Microsoft Systems Developer Relations
//
// Internet  :  kraigb@microsoft.com
// Compuserve:  >INTERNET:kraigb@microsoft.com
//

#include "stdafx.h"
#include "xrt_obj.h"
#include "dataobj.h"           
#include "iperstor.h"

// CImpIPersistStorage:CImpIPersistStorage
// CImpIPersistStorage::~CImpIPersistStorage
//
// Constructor Parameters:
//  pObj            LPVOID pointing to the object we live in.
//  punkOuter       LPUNKNOWN of the controlling unknown.
//
CImpIPersistStorage::CImpIPersistStorage(LPCXRTDataObject pObj, LPUNKNOWN punkOuter)
{
    m_cRef = 0 ;
    m_pObj = pObj ;
    m_punkOuter = punkOuter ;
    return ;
}


CImpIPersistStorage::~CImpIPersistStorage(void)
{
    return ;
}


// CImpIPersistStorage::QueryInterface
// CImpIPersistStorage::AddRef
// CImpIPersistStorage::Release
//
// Purpose:
//  Standard set of IUnknown members for this interface
//
STDMETHODIMP CImpIPersistStorage::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    return m_punkOuter->QueryInterface(riid, ppv) ;
}

STDMETHODIMP_(ULONG) CImpIPersistStorage::AddRef(void)
{
    ++m_cRef ;
    return m_punkOuter->AddRef() ;
}

STDMETHODIMP_(ULONG) CImpIPersistStorage::Release(void)
{
    --m_cRef ;
    return m_punkOuter->Release() ;
}


// CImpIPersistStorage::GetClassID
//
// Purpose:
//  Returns the CLSID of the object represented by this interface.
//
// Parameters:
//  pClsID          LPCLSID in which to store our CLSID.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIPersistStorage::GetClassID(LPCLSID pClsID)
{   
    *pClsID = CLSID_StocksRUsStockQuotes ;
    
    return NOERROR ;
}

// CImpIPersistStorage::IsDirty
//
// Purpose:
//  Tells the caller if we have made changes to this object since
//  it was loaded or initialized new.
//
// Parameters:
//  None
//
// Return Value:
//  HRESULT         Contains S_OK if we ARE dirty, S_FALSE if NOT dirty,
//                  that is, "Yes I AM dirty, or NO, I'm clean."
//
STDMETHODIMP CImpIPersistStorage::IsDirty(void)
{
    return ResultFromScode(m_pObj->IsDirty() ? S_OK : S_FALSE) ;
}

// CImpIPersistStorage::InitNew
//
// Purpose:
//  Provides the object with the IStorage they can hold on to while
//  they are running.  Here we can initialize the structure of the
//  storage and AddRef it for incremental access.  This function will
//  only be called once in the object's lifetime in lieu of ::Load.
//
// Parameters:
//  pIStorage       LPSTORAGE for the object.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIPersistStorage::InitNew(LPSTORAGE pIStorage)
{
    return NOERROR ;
}

// CImpIPersistStorage::Load
//
// Purpose:
//  Instructs the object to load itself from a previously saved IStorage
//  that was handled by ::Save in another object lifetime.  This function
//  will only be called once in the object's lifetime in lieu of ::InitNew.
//  The object may hold on to pIStorage here for incremental access.
//
// Parameters:
//  pIStorage       LPSTORAGE from which to load.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIPersistStorage::Load(LPSTORAGE pIStorage)
{
    return NOERROR ;
}

// CImpIPersistStorage::Save
//
// Purpose:
//  Saves the data for this object to an IStorage which may
//  or may not be the same as the one previously passed to
//  ::Load, indicated with fSameAsLoad.  After this call we may
//  not write into the storage again until ::SaveCompleted is called,
//  although we may still read.
//
// Parameters:
//  pIStorage       LPSTORAGE in which to save our data.
//  fSameAsLoad     BOOL indicating if this is the same pIStorage
//                  that was passed to ::Load.  If it was, then
//                  objects that built up a structure in that storage
//                  do not have to regenerate the entire thing.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIPersistStorage::Save(LPSTORAGE pIStorage, BOOL fSameAsLoad)
{
    return NOERROR ;
}

// CImpIPersistStorage::SaveCompleted
//
// Purpose:
//  Notifies the object that the storage in pIStorage has been completely
//  saved now.  This is called when the user of this object wants to
//  save us in a completely new storage, and if we normally hang on to
//  the storage we have to reinitialize ourselves here for this new one
//  that is now complete.
//
// Parameters:
//  pIStorage       LPSTORAGE of the new storage in which we now live.
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIPersistStorage::SaveCompleted(LPSTORAGE pIStorage)
{
    return NOERROR ;
}

// CImpIPersistStorage::HandsOffStorage
//
// Purpose:
//  Instructs the object that another agent is interested in having total
//  access to the storage we might be hanging on to from ::InitNew or
//  ::SaveCompleted.  In this case we must release our hold and await
//  another call to ::SaveCompleted before we have a hold again.  Therefore
//  we cannot read or write after this call until ::SaveCompleted.
//
//  Situations where this might happen arise in compound document scenarios
//  where this object might be in-place active but the application wants
//  to rename and commit the root storage.  Therefore we are asked to
//  close our hold, let the container party on the storage, then call us
//  again later to tell us the new storage we can hold.
//
// Parameters:
//  None
//
// Return Value:
//  HRESULT         NOERROR on success, error code otherwise.
//
STDMETHODIMP CImpIPersistStorage::HandsOffStorage(void)
{
    return NOERROR ;
}

