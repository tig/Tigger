// IPERFILE.H
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
//  Stolen directly out of KraigB's Chapter 6 code.
//
#ifndef _IPERFILE_H_
#define _IPERFILE_H_


class FAR CImpIPersistFile : public IPersistFile
{
protected:
    ULONG               m_cRef ;      //Interface reference count.
    LPVOID              m_pObj ;      //Back pointer to the object.
    LPUNKNOWN           m_punkOuter ; //Controlling unknown for delegation.

public:
    CImpIPersistFile(LPVOID, LPUNKNOWN) ;
    ~CImpIPersistFile(void) ;

    STDMETHODIMP QueryInterface(REFIID, LPVOID FAR *) ;
    STDMETHODIMP_(ULONG) AddRef(void) ;
    STDMETHODIMP_(ULONG) Release(void) ;

    STDMETHODIMP GetClassID(LPCLSID) ;

    STDMETHODIMP IsDirty(void) ;
    STDMETHODIMP Load(LPCTSTR, DWORD) ;
    STDMETHODIMP Save(LPCTSTR, BOOL) ;
    STDMETHODIMP SaveCompleted(LPCTSTR) ;
    STDMETHODIMP GetCurFile(LPTSTR FAR *);
} ;

typedef CImpIPersistFile * LPCIMPIPERSISTFILE ;


#endif  //_IPERFILE_H_
