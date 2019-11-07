// IENUMFE.H
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
#ifndef _IENUMFE_H_
#define _IENUMFE_H_
/*
 * IEnumFORMATETC object that is created from IIENUMFEect::EnumFormatEtc.
 * This object lives on its own, that is, QueryInterface only knowns
 * IUnknown and IEnumFormatEtc, nothing more.  We still use an outer
 * unknown for reference counting, because as long as this enumerator
 * lives, the data object should live, thereby keeping the application up.
 */

class __far CImpEnumFORMATETC : public IEnumFORMATETC
{
private:
    ULONG           m_cRef;         //Object reference count.
    LPUNKNOWN       m_punkRef;      //IUnknown for reference counting.
    ULONG           m_iCur;         //Current element.
    ULONG           m_cfe;          //Number of FORMATETCs in us
    LPFORMATETC     m_prgfe;        //Source of FORMATETCs

public:
    CImpEnumFORMATETC(LPUNKNOWN, ULONG, LPFORMATETC);
    ~CImpEnumFORMATETC(void);

    //IUnknown members that delegate to m_punkOuter.
    STDMETHODIMP         QueryInterface(REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    //IEnumFORMATETC members
    STDMETHODIMP Next(ULONG, LPFORMATETC, ULONG FAR *);
    STDMETHODIMP Skip(ULONG);
    STDMETHODIMP Reset(void);
    STDMETHODIMP Clone(IEnumFORMATETC FAR * FAR *);
} ;

typedef CImpEnumFORMATETC FAR * LPIMPENUMFORMATETC ;

#endif // _IENUMFE_H_

