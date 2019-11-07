// object.h
//
// Header file for the implementation of the CStocksObject
// class.
//

#ifndef _OBJECT_H_
#define _OBJECT_H_

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

// The StocksObject object is implemented in its own class with its own
// IUnknown to support aggregation.  
//
class CStocksObject : public IUnknown
{
    // Make any contained interfaces our friend so they can get at us
    friend class CImpIDataObject;

    protected:
        PFNCREATED          m_pfnCreated ;  // Call when we're created/destroyed

        ULONG               m_cRef;         //Object reference count.
        LPUNKNOWN           m_punkOuter;    //Cont. Unk. for aggregation

        // Contained interface implemetation for IDataObject
        // (CImpIDataObject)
        LPDATAOBJECT        m_pIDataObject;

        LPPERSISTFILE       m_pIPersistFile ;

        // Other interfaces used, implemented elsewhere
        LPDATAADVISEHOLDER  m_pIDataAdviseHolder;
        BOOL                m_fFullData ;

        // Data stuff
        UINT        m_uiSimObjID ;          // For CSimulation::UnRegisterClient
        PXRTSTOCKS  m_pStocks ;
        LPSTORAGE   m_lpStorage ;


    protected:
        //Functions for use from IDataObject::GetData
        HRESULT     RenderCF_TEXT(LPSTGMEDIUM);
        HRESULT     RenderCF_XRTDEMO(LPSTGMEDIUM pSTM, TYMED tymed);

    public:
        CStocksObject( LPUNKNOWN, PFNCREATED ) ;
        ~CStocksObject( void ) ;

        BOOL Init( void ) ;
        void OnMarketChange() ;     // called when data changes on g_pSimObj
        void OnUpdateTick() ;       // called when it's time to notify the
                                    // adivse sink.

        //Non-delegating object IUnknown
        STDMETHODIMP         QueryInterface(REFIID, LPVOID FAR *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);
};

// IDataObject interface implementation for CStocksObject
//
class FAR CImpIDataObject : public IDataObject
{
private:
    ULONG           m_cRef;         //Interface reference count.
    CStocksObject*  m_pObj;         //Back pointer to the object.
    LPUNKNOWN       m_punkOuter;    //Controlling unknown for delegation

public:
    CImpIDataObject( CStocksObject*, LPUNKNOWN ) ;
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


#endif //_OBJECT_H_


