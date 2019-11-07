// IOLEOBJ.CPP
//
// Template implementation of the IOleObject interface.
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

#include "ioleobj.h"

// CImpIOleObject::CImpIOleObject
// CImpIOleObject::~CImpIOleObject
//
// Parameters (Constructor):
//  pObj            LPVOID of the object we're in.
//  punkOuter       LPUNKNOWN to which we delegate.
//
CImpIOleObject::CImpIOleObject(LPCXRTDataObject pObj, LPUNKNOWN punkOuter)
{
    m_cRef=0 ;
    m_pObj=pObj ;
    m_punkOuter=punkOuter ;
    return ;
}

CImpIOleObject::~CImpIOleObject(void)
{
    return ;
}

// CImpIOleObject::QueryInterface
// CImpIOleObject::AddRef
// CImpIOleObject::Release
//
// Purpose:
//  IUnknown members for CImpIOleObject object.
//
STDMETHODIMP CImpIOleObject::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    return m_punkOuter->QueryInterface(riid, ppv) ;
}

STDMETHODIMP_(ULONG) CImpIOleObject::AddRef(void)
{
    ++m_cRef ;
    return m_punkOuter->AddRef() ;
}

STDMETHODIMP_(ULONG) CImpIOleObject::Release(void)
{
    --m_cRef ;
    return m_punkOuter->Release() ;
}


// CImpIOleObject::SetClientSite
//
// Purpose:
//  Provides the object with a pointer to the IOleClient site representing
//  the container in which this object resides.
//
// Parameters:
//  pIOleClientSite LPOLECLIENTSITE to the container's interface.
//
// Return Value:
//  HRESULT         NOERROR
//
STDMETHODIMP CImpIOleObject::SetClientSite(LPOLECLIENTSITE pClientSite)
{
    return NOERROR ;
}                    


// CImpIOleObject::GetClientSite
//
// Purpose:
//  Asks the object for the client site provided in SetClientSite.  If
//  you have not seen SetClientSite yet, return a NULL in ppIOleClientSite.
//
// Parameters:
//  ppIOleClientSite    LPOLECLIENTSITE FAR * in which to store the pointer.
//
// Return Value:
//  HRESULT         NOERROR
//
STDMETHODIMP CImpIOleObject::GetClientSite(LPOLECLIENTSITE FAR * ppIOleClientSite)
{
    return ResultFromScode(E_NOTIMPL) ;
}

        
// CImpIOleObject::SetHostNames
//
// Purpose:
//  Provides the object with names of the container application and the
//  object in the container to use in object user interface.
//
// Parameters:
//  pszApp          LPCSTR of the container application.
//  pszObj          LPCSTR of some name that is useful in window titles.
//
// Return Value:
//  HRESULT         NOERROR
//
STDMETHODIMP CImpIOleObject::SetHostNames(LPCSTR pszApp, LPCSTR pszObj)
{
    // Change the UI to reflect the embedded state
//    m_pObj->m_pXRTObjectView->m_fEmbedding = TRUE ;
//    m_pObj->m_pXRTObjectView->SetEmbeddingAppName( pszApp ) ;
//    m_pObj->m_pXRTObjectView->SetEmbeddingObjName( pszObj ) ;

//    m_pObj->m_pXRTObjectView->UpdateEmbeddingUI() ;

    return ResultFromScode(E_NOTIMPL) ;
}
        
        
// CImpIOleObject::Close
//
// Purpose:
//  Forces the object to close down its user interface and unload.
//
// Parameters:
//  dwSaveOption    DWORD describing the circumstances under which the
//                  object is being saved and closed.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::Close(DWORD dwSaveOption)
{
    // TODO:  Implement code to detect if we need to save,
    // and if we do do the appropriate thing.
    //
    
//    return m_pObj->m_pXRTObjectView->DestroyDataObject( NULL ) ;
    return ResultFromScode(E_NOTIMPL) ;
}          


// CImpIOleObject::SetMoniker
//
// Purpose:
//  Informs the object of its moniker or its container's moniker
//  depending on dwWhich.
//
// Parameters:
//  dwWhich         DWORD describing whether the moniker is the object's
//                  or the container's.
//  pmk             LPMONIKER with the name.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::SetMoniker(DWORD dwWhich, LPMONIKER pmk)
{
    return ResultFromScode(E_NOTIMPL) ;
}


// CImpIOleObject::GetMoniker
//
// Purpose:
//  Asks the object for a moniker that can later be used to reconnect
//  to it.
//
// Parameters:
//  dwAssign        DWORD determining how to assign the moniker to
//                  to the object.
//  dwWhich         DWORD describing which moniker the caller wants.
//  ppmk            LPMONIKER FAR * into which to store the moniker.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker
    , LPMONIKER FAR * ppmk)
{
    return ResultFromScode(E_NOTIMPL) ;
}


// CImpIOleObject::InitFromData
//
// Purpose:
//  Initilizes the object from the contents of a data object.
//
// Parameters:
//  pIDataObject    LPDATAOBJECT containing the data.
//  fCreation       BOOL indicating if this is part of a new creation.
//                  If FALSE, the container is trying to paste here.
//  dwReserved      DWORD reserved.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//

STDMETHODIMP CImpIOleObject::InitFromData(LPDATAOBJECT pIDataObject, BOOL fCreation
    , DWORD dwReserved)
{
    return ResultFromScode(E_NOTIMPL) ;
}
         

// CImpIOleObject::GetClipboardData
//
// Purpose:
//  Returns an IDataObject pointer to the caller representing what would
//  be on the clipboard if the server did an Edit/Copy using OleSetClipboard.
//
// Parameters:
//  dwReserved      DWORD reserved.
//  ppIDataObj      LPDATAOBJECT FAR * into which to store the pointer.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::GetClipboardData(DWORD dwReserved, LPDATAOBJECT FAR * ppIDataObj)
{
    return ResultFromScode(E_NOTIMPL) ;
}


// CImpIOleObject::DoVerb
//
// Purpose:
//  Executes an object-defined action.
//
// Parameters:
//  iVerb           LONG index of the verb to execute.
//  pMSG            LPMSG describing the event causing the activation.
//  pIOleClientSite LPOLECLIENTSITE to the site involved.
//  lIndex          LONG the piece on which execution is happening.
//  hWndParent      HWND of the window in which the object can play in-place.
//  pRectPos        LPRECT of the object in hWndParent where the object
//                  can play in-place if desired.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::DoVerb( LONG iVerb, LPMSG PMSG, LPOLECLIENTSITE pIOleClientSite,
                                     LONG lIndex, HWND hWndParent, LPCRECT pRectPos )
{
    
    return ResultFromScode(E_NOTIMPL) ;
}


// CImpIOleObject::EnumVerbs
//
// Purpose:
//  Creates an enumerator that knows the object's verbs.  If you need
//  to change the verb list dynamically, then you'll need to implement
//  this, otherwise you can return OLE_S_USEREG.
//
// Parameters:
//  ppEnum          LPENUMOLEVERB FAR// into which to return the enum.
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::EnumVerbs(LPENUMOLEVERB FAR * ppEnum)
{
    //Trivial implementation if you fill the regDB.
    return ResultFromScode(OLE_S_USEREG) ;
}


// CImpIOleObject::Update
//
// Purpose:
//  Insures that the object is up to date.  This is mostly used for
//  caching but you must make sure that you recursively call all
//  nested objects you contain as well.
//
// Parameters:
//  None
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::Update(void)
{
    return ResultFromScode(E_NOTIMPL) ;
}


// CImpIOleObject::IsUpToDate
//
// Purpose:
//  Returns if the object is currently up to date, which involves
//  asking all contained object inside this object if they are up to
//  date as well.
//
// Parameters:
//  None
//
// Return Value:
//  HRESULT         NOERROR if successful, S_FALSE if dirty.
//
STDMETHODIMP CImpIOleObject::IsUpToDate(void)
{
    return NOERROR ;
}


// CImpIOleObject::GetUserClassID
//
// Purpose:
//  Used for linked objects, this returns the class ID of what end
//  users think they are editing.
//
// Parameters:
//  pClsID          LPCLSID in which to store the CLSID.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::GetUserClassID(LPCLSID pClsID)
{
    return ResultFromScode(E_NOTIMPL) ;
}


// CImpIOleObject::GetUserType
//
// Purpose:
//  Determines the user-presentable name of the object.
//
// Parameters:
//  dwForm          DWORD describing which form of the string is desired.
//  pszType         LPSTR FAR * into which to return the pointer to
//                  the type string.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::GetUserType(DWORD dwForm, LPSTR FAR * ppszType)
{
    return ResultFromScode(OLE_S_USEREG) ;
}


// CImpIOleObject::SetExtent
//
// Purpose:
//  Sets the size of the object in HIMETRIC units.
//
// Parameters:
//  dwAspect        DWORD of the aspect affected.
//  pszl            LPSIZEL containing the new size.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::SetExtent(DWORD dwAspect, LPSIZEL pszl)
{
    return ResultFromScode(E_NOTIMPL) ;
}


// CImpIOleObject::GetExtent
//
// Purpose:
//  Retrieves the size of the object in HIMETRIC units.
//
// Parameters:
//  dwAspect        DWORD of the aspect requested
//  pszl            LPSIZEL into which to store the size.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::GetExtent(DWORD dwAspect, LPSIZEL pszl)
{
    return ResultFromScode(E_NOTIMPL) ;
}
  

// CImpIOleObject::Advise
//
// Purpose:
//  Provides an IAdviseSink to the object for notifications.
//
// Parameters:
//  pIAdviseSink    LPADVISESINK to notify.
//  pdwConn         LPDWORD into which to store a connection key.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::Advise(LPADVISESINK pIAdviseSink, LPDWORD pdwConn)
{
    return ResultFromScode(E_NOTIMPL) ;
}


// CImpIOleObject::Unadvise
//
// Purpose:
//  Terminates a previous advise connection from ::Advise.
//
// Parameters:
//  dwConn          DWORD connection key from ::Advise.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::Unadvise(DWORD dwConn)
{
    return ResultFromScode(E_NOTIMPL) ;
}


// CImpIOleObject::EnumAdvise
//
// Purpose:
//  Creates and returns a enumeration of the advises on this object.
//
// Parameters:
//  ppEnum          LPENUMSTATDATA FAR * in which to return the
//                  enumerator.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::EnumAdvise(LPENUMSTATDATA FAR * ppEnum)
{
    return ResultFromScode(E_NOTIMPL) ;
}


// CImpIOleObject::GetMiscStatus
//
// Purpose:
//  Returns a set of miscellaneous status flags for the object.
//
// Parameters:
//  dwAspect        DWORD of the aspect in question.
//  pdwStatus       LPDWORD in which to store the flags.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::GetMiscStatus(DWORD dwAspect, LPDWORD pdwStatus)
{
    return ResultFromScode(OLE_S_USEREG) ;
}


// CImpIOleObject::SetColorScheme
//
// Purpose:
//  Provides the object with the color palette as recommended by the
//  container application that also knows the palettes of other objects.
//  The object here is not required to use these colors.
//
// Parameters:
//  pLP             LPLOGPALETTE providing the colors.
//
// Return Value:
//  HRESULT         NOERROR if successful, error code otherwise.
//
STDMETHODIMP CImpIOleObject::SetColorScheme(LPLOGPALETTE pLP)
{
    return ResultFromScode(E_NOTIMPL) ;
}

