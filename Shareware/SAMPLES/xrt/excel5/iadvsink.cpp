/*
 * IADVSINK.CPP
 *
 * Implementation of the IAdviseSink interface for the Data User, Chapter 6
 *
 * Copyright (c)1993 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */

#include "precomp.h"
#include "..\\wosaxrt.h"
#include "fn.h"
#include "iadvsink.h"

/*
 * CImpIAdviseSink::CImpIAdviseSink
 * CImpIAdviseSink::~CImpIAdviseSink
 *
 * Parameters (Constructor):
 *  pAV             LPAPPVARS to the application
 *
 */

CImpIAdviseSink::CImpIAdviseSink( void )
{
    return;
}

CImpIAdviseSink::~CImpIAdviseSink(void)
{
    return;
}

/*
 * CImpIAdviseSink::QueryInterface
 * CImpIAdviseSink::AddRef
 * CImpIAdviseSink::Release
 *
 * Purpose:
 *  IUnknown members for CImpIAdviseSink object.
 */

STDMETHODIMP CImpIAdviseSink::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv=NULL;

    //Any interface on this object is the object pointer.
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IAdviseSink))
        *ppv=(LPVOID)this;

    /*
     * If we actually assign an interface to ppv we need to AddRef it
     * since we're returning a new pointer.
     */
    if (NULL!=*ppv)
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }

    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) CImpIAdviseSink::AddRef(void)
{
    return ++m_cRef;
}


STDMETHODIMP_(ULONG) CImpIAdviseSink::Release(void)
{
    ULONG   cRefT;

    cRefT=--m_cRef;

    if (0==cRefT)
        delete this;

    return cRefT;
}




/*
 * IAdviseSink::OnDataChange
 *
 * Purpose:
 *  Notifes the advise sink that data changed in a data object.  On
 *  this message you may request a new data rendering and update your
 *  displays as necessary.  Any data sent to this function is owned
 *  by the caller, not by this advise sink!
 *
 *  All Advise Sink methods should be considered asynchronous and therefore
 *  we should attempt no synchronous calls from within them to an EXE
 *  object.  If we do, we'll get RPC_E_CALLREJECTED as shown below.
 *
 * Parameters:
 *  pFE             LPFORMATETC describing format that changed
 *  pSTM            LPSTGMEDIUM providing the medium in which the data
 *                  is provided.
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnDataChange(LPFORMATETC pFE, LPSTGMEDIUM pSTM)
{
    if (!(DVASPECT_CONTENT & pFE->dwAspect))
        return;
    
    GotData( pFE, pSTM ) ;

    GlobalUnlock( pSTM->hGlobal ) ;
}
  
/*
 * IAdviseSink::OnViewChange
 *
 * Purpose:
 *  Notifes the advise sink that presentation data changed in the data
 *  object to which we're connected providing the right time to update
 *  displays using such presentations.
 *
 * Parameters:
 *  dwAspect        DWORD indicating which aspect has changed.
 *  lindex          LONG indicating the piece that changed.
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnViewChange(DWORD dwAspect, LONG lindex)
    {
    return;
    }





/*
 * IAdviseSink::OnRename
 *
 * Purpose:
 *  Informs the advise sink that an IOleObject has been renamed, primarily
 *  when its linked.
 *
 * Parameters:
 *  pmk             LPMONIKER providing the new name of the object
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnRename(LPMONIKER pmk)
    {
    return;
    }






/*
 * IAdviseSink::OnSave
 *
 * Purpose:
 *  Informs the advise sink that the OLE object has been saved
 *  persistently.  The primary purpose of this is for containers that
 *  want to make optimizations for objects that are not in a saved
 *  state, so on this you have to disable such optimizations.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnSave(void)
    {
    return;
    }





/*
 * IAdviseSink::OnClose
 *
 * Purpose:
 *  Informs the advise sink that the OLE object has closed and is
 *  no longer bound in any way.  On this you typically change state
 *  variables and redraw shading, etc.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnClose(void)
    {
    return;
    }
