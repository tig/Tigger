// ID3TextItems.cpp : Implementation of CID3LibCOMWrapperApp and DLL registration.

#include "stdafx.h"
#include "ID3COM.h"
#include "ID3TextItems.h"
#include "ID3Field.h"
#include "ID3_Fix.h"

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP CID3TextItems::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IID3TextItems,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CID3TextItems::Item(/* [in] */ VARIANT Item, /* [retval][out] */ BSTR* pbstrReturnVal)
{
	USES_CONVERSION;

	if (pbstrReturnVal== NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid pointer.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}
	*pbstrReturnVal = NULL;

	HRESULT hr ;
	hr = VariantChangeType(&Item, &Item, 0, VT_UI4);
	if (FAILED(hr))
	{
		AtlReportError(GetObjectCLSID(), L"Item is of an invalid type", GUID_NULL, hr);
		return DISP_E_BADVARTYPE;
	}

	try
	{
		OLECHAR sz[2048];
		ULONG cc = m_pField->Get(sz, 2048, Item.ulVal);
		*pbstrReturnVal = SysAllocStringLen(sz, cc);
		if (*pbstrReturnVal == NULL)
		{
			hr = E_OUTOFMEMORY;
			AtlReportError(GetObjectCLSID(), L"Out of memory.", GUID_NULL, hr);
		}
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		hr = E_FAIL;
	}

	return hr;
}

STDMETHODIMP CID3TextItems::get_Count(/* [retval][out] */ long *Count)
{
	try
	{
		if (ID3_Field_GetType(m_pField) == ID3FTY_ASCIISTRING || 
			ID3_Field_GetType(m_pField) == ID3FTY_UNICODESTRING)
			*Count = m_pField->GetNumTextItems();
		else
			*Count = 0;
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CID3TextItems::Add(/* [in] */ BSTR bstr)
{
	HRESULT hr = S_OK;

	try
	{
		if (ID3_Field_GetType(m_pField) == ID3FTY_ASCIISTRING || 
			ID3_Field_GetType(m_pField) == ID3FTY_UNICODESTRING)
			m_pField->Add(bstr);
		else
		{
			hr = E_FAIL;
			AtlReportError(GetObjectCLSID(), L"Only text fields support TextItems.", GUID_NULL, hr);
		}
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}

	return hr;
}

STDMETHODIMP CID3TextItems::_NewEnum(/* [retval][out] */ IUnknown **ppEnum) 
{
	HRESULT hr = S_OK;
	*ppEnum = NULL;

	if (!(ID3_Field_GetType(m_pField) == ID3FTY_ASCIISTRING || 
	ID3_Field_GetType(m_pField) == ID3FTY_UNICODESTRING))
	{
		hr = E_FAIL;
		AtlReportError(GetObjectCLSID(), L"Only text fields support TextItems.", GUID_NULL, hr);
	}

	ULONG n = 0;
	VARIANT* rgv = NULL;
	
	typedef CComEnum<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy<VARIANT> > CComEnumVARIANT;
	CComEnumVARIANT* pEnum = NULL;

	try
	{
		pEnum = new CComObject<CComEnumVARIANT>;
		_ASSERTE(pEnum);
		if (pEnum == NULL)
			throw E_OUTOFMEMORY;

		n = m_pField->GetNumTextItems();
		rgv = (VARIANT*)malloc(n * sizeof(VARIANT));
		_ASSERTE(rgv);
		if (rgv == NULL)
			throw E_OUTOFMEMORY;

		VARIANT v;
		VariantInit(&v);
		v.vt = VT_UI4;
		for (ULONG i = 0 ; i < n ; i++)
		{
			VariantInit(&rgv[i]);
			rgv[i].vt = VT_BSTR;
			v.ulVal = i+1;
			hr = Item(v, &(rgv[i].bstrVal));
			if (FAILED(hr))
				throw hr; 
			_ASSERTE(rgv[i].bstrVal);
		}

		hr = pEnum->Init(&rgv[0], &rgv[n], NULL, AtlFlagCopy);
		if (FAILED(hr))
			throw hr;

		hr = pEnum->QueryInterface(IID_IUnknown, (void**)ppEnum);
		if (FAILED(hr))
			throw hr;
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		hr = E_FAIL;
	}
	catch(HRESULT hres)
	{
		AtlReportError(GetObjectCLSID(), L"Cannot create enumerator.", GUID_NULL, hres);
		hr = hres;
	}

	if (FAILED(hr))
	{
		if (rgv)
		{
			if (pEnum)
				delete pEnum;
			for (ULONG i = 0 ; i < n ; i++)
				VariantInit(&rgv[i]);
			free(rgv);
		}
	}

	return hr;
}
