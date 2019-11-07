// {DCB8988F-DCB9-11ce-9049-080036F12502}
DEFINE_GUID(IID_ISomeInterface, 
0xdcb8988f, 0xdcb9, 0x11ce, 0x90, 0x49, 0x8, 0x0, 0x36, 0xf1, 0x25, 0x2);

#ifndef __ISomeInterface__
#define __ISomeInterface__
DECLARE_INTERFACE_(ISomeInterface, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS) PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** ISomeInterface methods ***
    STDMETHOD(SomeMethod) (void) PURE;
};
#endif // __ISomeInterface__

// CSomeObject is an aggregatable object that implements
// IUnknown and ISomeInterface
class CSomeObject : public IUnknown
{
	private:
		DWORD 		m_cRef;			// Object reference count
		IUnknown*	m_pUnkOuter;		// Outer unknown, no AddRef

		// Nested class to implement the ISomeInterface interface
		class CImpSomeInterface : public ISomeInterface
		{
			friend class CSomeObject ;
			private:
				DWORD       	m_cRef;        		// Interface ref-count, for debugging
				IUnknown*	m_pUnkOuter;		// Outer unknown, for delegation
			public:
				CImpSomeInterface() { m_cRef = 0;	};
				~ CImpSomeInterface(void) {};

				// Rule 2: IUnknown members delegate to the outer unknown
				// Rule 3: IUnknown members do not control lifetime of object
				STDMETHODIMP     QueryInterface(REFIID riid, void** ppv)
				{	return m_pUnkOuter->QueryInterface(riid,ppv);  };

				STDMETHODIMP_(DWORD) AddRef(void)
				{ 	return m_pUnkOuter->AddRef();   };

				STDMETHODIMP_(DWORD) Release(void)
				{ 	return m_pUnkOuter->Release();   };

				// ISomeInterface members
				STDMETHODIMP SomeMethod(void) 
				{	return S_OK;	};
		} ;
		CImpSomeInterface m_ImpSomeInterface ;
	public:
		CSomeObject(IUnknown * pUnkOuter)
		{
			m_cRef=0;
			// Rule 6: No AddRef necessary if non-NULL as we're aggregated.
			m_pUnkOuter=pUnkOuter;
			m_ImpSomeInterface.m_pUnkOuter=pUnkOuter;	
		} ;
        ~CSomeObject(void) {} ;

		// Static member function for creating new instances (don’t use
		// new directly).  Protects against outer objects asking for interfaces 
		// other than IUnknown (Rule 5)
		static HRESULT Create(IUnknown* pUnkOuter, REFIID riid, void **ppv)
		{
			CSomeObject*		pObj;
			if (pUnkOuter != NULL && riid != IID_IUnknown)
				return CLASS_E_NOAGGREGATION;
			pObj = new CSomeObject(pUnkOuter);
			if (pObj == NULL)
				return E_OUTOFMEMORY;
			// Set up the right unknown for delegation (the non-aggregation case)
			if (pUnkOuter == NULL)
				pObj->m_pUnkOuter = (IUnknown*)pObj ;
			return pObj->QueryInterface(riid, (void**)ppv);
		}

		// Rule 1: Implicit IUnknown members, non-delegating
		// Rule 4: Implicit QueryInterface only controls inner object
		STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
		{
			*ppv=NULL;
			if (riid == IID_IUnknown)   
				*ppv=this;
			if (riid == IID_ISomeInterface)  
				*ppv=&m_ImpSomeInterface;
			if (NULL==*ppv)  
				return ResultFromScode(E_NOINTERFACE);
			((IUnknown*)*ppv)->AddRef();
			return NOERROR;
		} ;
		STDMETHODIMP_(DWORD) AddRef(void)
		{  	return ++m_cRef; };
		STDMETHODIMP_(DWORD) Release(void)
		{	
			if (--m_cRef != 0)
				return m_cRef;
			delete this;
			return 0;
		};
};
