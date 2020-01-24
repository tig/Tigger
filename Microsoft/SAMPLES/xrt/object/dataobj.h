// DATAOBJ.H
//
// XRT Data Object
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

#ifndef _DATAOBJ_H_
#define _DATAOBJ_H_

#include "stdafx.h"
#include "resource.h"
#include <afxwin.h>   
#include <ole2.h>
#include <oleguid.h>
#include "clsid.h"

//Type for an object-destroyed callback
typedef void (__far __cdecl *LPFNDESTROYED)(void);

#define SETFormatEtc(fe, cf, asp, td, med, li)   \
    {\
    (fe).cfFormat=cf;\
    (fe).dwAspect=asp;\
    (fe).ptd=td;\
    (fe).tymed=med;\
    (fe).lindex=li;\
    };

#define SETDefFormatEtc(fe, cf, med)   \
    {\
    (fe).cfFormat=cf;\
    (fe).dwAspect=DVASPECT_CONTENT;\
    (fe).ptd=NULL;\
    (fe).tymed=med;\
    (fe).lindex=-1;\
    };

// The DataObject object is implemented in its own class with its own
// IUnknown to support aggregation.  It contains one CImpIDataObject
// object that we use to implement the externally exposed interfaces.
//

class CDataObject : public IUnknown
{
    //Make any contained interfaces your friends so they can get at you
    friend class CImpIDataObject;

    protected:
        ULONG               m_cRef;         //Object reference count.
        LPUNKNOWN           m_punkOuter;    //Cont. Unk. for aggregation
        LPFNDESTROYED       m_pfnDestroy;   //Function to call on closure.

        // Contained interface implemetation
        LPDATAOBJECT        m_pIDataObject;

        // Other interfaces used, implemented elsewhere
        LPDATAADVISEHOLDER  m_pIDataAdviseHolder;

        // for IDataObject::EnumFormatEtc
        ULONG               m_cfeGet;
        FORMATETC           m_rgfeGet[1];

    protected:
        //Functions for use from IDataObject::GetData
        HRESULT     RenderText(LPSTGMEDIUM);

    public:
        CDataObject( LPUNKNOWN, LPFNDESTROYED ) ;
        ~CDataObject( void ) ;

        BOOL Init(void ) ;

        //Non-delegating object IUnknown
        STDMETHODIMP         QueryInterface(REFIID, LPVOID FAR *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);
};

typedef CDataObject FAR * LPCDATAOBJECT;



/*
 * Interface implementations for the CDataObject object.
 */

class __far CImpIDataObject : public IDataObject
    {
    private:
        ULONG           m_cRef;         //Interface reference count.
        LPVOID          m_pObj;         //Back pointer to the object.
        LPUNKNOWN       m_punkOuter;    //Controlling unknown for delegation

    public:
        CImpIDataObject(LPVOID, LPUNKNOWN);
        ~CImpIDataObject(void);

        //IUnknown members that delegate to m_punkOuter.
        STDMETHODIMP         QueryInterface(REFIID, LPVOID FAR *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IDataObject members
        STDMETHODIMP GetData(LPFORMATETC, LPSTGMEDIUM);
        STDMETHODIMP GetDataHere(LPFORMATETC, LPSTGMEDIUM);
        STDMETHODIMP QueryGetData(LPFORMATETC);
        STDMETHODIMP GetCanonicalFormatEtc(LPFORMATETC, LPFORMATETC);
        STDMETHODIMP SetData(LPFORMATETC, STGMEDIUM FAR *, BOOL);
        STDMETHODIMP EnumFormatEtc(DWORD, LPENUMFORMATETC FAR *);
        STDMETHODIMP DAdvise(FORMATETC FAR *, DWORD,  LPADVISESINK, DWORD FAR *);
        STDMETHODIMP DUnadvise(DWORD);
        STDMETHODIMP EnumDAdvise(LPENUMSTATDATA FAR *);
    };



/*
 * IEnumFORMATETC object that is created from IDataObject::EnumFormatEtc.
 * This object lives on its own, that is, QueryInterface only knowns
 * IUnknown and IEnumFormatEtc, nothing more.  We still use an outer
 * unknown for reference counting, because as long as this enumerator
 * lives, the data object should live, thereby keeping the application up.
 */

class __far CEnumFormatEtc : public IEnumFORMATETC
    {
    private:
        ULONG           m_cRef;         //Object reference count.
        LPUNKNOWN       m_punkRef;      //IUnknown for reference counting.
        ULONG           m_iCur;         //Current element.
        ULONG           m_cfe;          //Number of FORMATETCs in us
        LPFORMATETC     m_prgfe;        //Source of FORMATETCs

    public:
        CEnumFormatEtc(LPUNKNOWN, ULONG, LPFORMATETC);
        ~CEnumFormatEtc(void);

        //IUnknown members that delegate to m_punkOuter.
        STDMETHODIMP         QueryInterface(REFIID, LPVOID FAR *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IEnumFORMATETC members
        STDMETHODIMP Next(ULONG, LPFORMATETC, ULONG FAR *);
        STDMETHODIMP Skip(ULONG);
        STDMETHODIMP Reset(void);
        STDMETHODIMP Clone(IEnumFORMATETC FAR * FAR *);
    };


typedef CEnumFormatEtc FAR * LPCEnumFormatEtc;



#endif //_DATAOBJ_H_
