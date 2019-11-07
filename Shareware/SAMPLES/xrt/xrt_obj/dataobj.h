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

//Type for an object-destroyed callback

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

class CXRTObjectView ;

class CXRTDataObject : public IUnknown
{
    //Make any contained interfaces your friends so they can get at you
    friend class CImpIDataObject;
    friend class CImpIPersistStorage;

    protected:
        CXRTObjectView*     m_pXRTObjectView ;  // back pointer to View      
        
        LPXRTSTOCKS         m_lpUpdateList ;    // Our local copy of the data
        
        ULONG               m_cRef;         //Object reference count.
        LPUNKNOWN           m_punkOuter;    //Cont. Unk. for aggregation

        LPSTORAGE           m_lpStorage ;
   
        // Contained interface implemetation for IDataObject
        // (CImpIDataObject)
        LPDATAOBJECT        m_pIDataObject;
        DWORD               m_advf ;
        BOOL                m_fGotData ;

        // Contained interface implemetation for IPersistStorage
        // (CImpIPersistStorage)
        LPPERSISTSTORAGE        m_pIPersistStorage;
                                     
        LPPERSISTFILE           m_pIPersistFile ;
                                             
        // Contained interface implemetation for IOleObject
        // (CImpIOleObject)
        LPOLEOBJECT             m_pIOleObject;
                
        // Other interfaces used, implemented elsewhere
        LPDATAADVISEHOLDER      m_pIDataAdviseHolder;

    protected:
        //Functions for use from IDataObject::GetData
        HRESULT     RenderCF_TEXT(LPSTGMEDIUM);
        HRESULT     RenderCF_METAFILEPICT(LPSTGMEDIUM);
        HRESULT     RenderCF_XRTSTOCKS(LPSTGMEDIUM);

    public:
        CXRTDataObject( LPUNKNOWN, CXRTObjectView* ) ;
        ~CXRTDataObject( void ) ;

        BOOL Init(void ) ;
        void DataHasChanged( BOOL fPrime ) ;
        
        BOOL IsDirty() ;

        //Non-delegating object IUnknown
        STDMETHODIMP         QueryInterface(REFIID, LPVOID FAR *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);
};

typedef CXRTDataObject FAR * LPCXRTDataObject;



// IDataObject interface implementation for CXRTDataObject
//
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




#endif //_DATAOBJ_H_
