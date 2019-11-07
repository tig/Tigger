/*
 * IPERSTOR.H
 *
 * Definitions of a template IPersistStorage interface implementation.
 *
 * Copyright (c)1993 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IPERSTOR_H_
#define _IPERSTOR_H_


class __far CImpIPersistStorage : public IPersistStorage
{
protected:
    ULONG               m_cRef;      //Interface reference count.
    LPCXRTDataObject    m_pObj;      //Back pointer to the object.
    LPUNKNOWN           m_punkOuter; //Controlling unknown for delegation.

public:
    CImpIPersistStorage(LPCXRTDataObject, LPUNKNOWN);
    ~CImpIPersistStorage(void);

    STDMETHODIMP QueryInterface(REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    STDMETHODIMP GetClassID(LPCLSID);

    STDMETHODIMP IsDirty(void);
    STDMETHODIMP InitNew(LPSTORAGE);
    STDMETHODIMP Load(LPSTORAGE);
    STDMETHODIMP Save(LPSTORAGE, BOOL);
    STDMETHODIMP SaveCompleted(LPSTORAGE);
    STDMETHODIMP HandsOffStorage(void);
} ;

typedef CImpIPersistStorage * LPCIMPIPERSISTSTORAGE;


#endif  //_IPERSTOR_H_
