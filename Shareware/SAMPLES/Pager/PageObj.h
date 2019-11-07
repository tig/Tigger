// PageObj.h : Declaration of the CPager


#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// Pager

class CPager : 
	public CComDualImpl<IPager, &IID_IPager, &LIBID_PagerLib>,
	public ISupportErrorInfo,
	public CComObjectRoot,
	public CComCoClass<CPager,&CLSID_Pager>
{
public:
	CPager() {}
BEGIN_COM_MAP(CPager)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IPager)
END_COM_MAP()

// Use DECLARE_REGISTRY_REOSOURCID to use script based registry code
// This requires that register.dll be installed
//DECLARE_REGISTRY_RESOURCEID(IDR_Pager)

// Use DECLARE_STATIC_REGISTRY_RESOURCE to use script based registry code
// This requires that atl\ponent\register\static\statreg.cpp and statreg.h 
// are included in the build
//DECLARE_STATIC_REGISTRY_RESOURCEID(IDR_Pager)

// User old fashioned registry code
DECLARE_REGISTRY(CPager, _T("Microsoft.Pager.1"), _T("Microsoft.Pager"), IDS_PAGER_DESC, THREADFLAGS_BOTH)

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IPager
public:
        virtual /* [helpstring] */ HRESULT __stdcall Page( 
            /* [in] */ BSTR PhoneNumber,
            /* [in] */ BSTR PIN,
            /* [in] */ BSTR Msg,
            /* [out][retval] */ VARIANT_BOOL __RPC_FAR *retval);

};
