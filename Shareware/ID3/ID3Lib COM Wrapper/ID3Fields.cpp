// ID3Frames.cpp : Implementation of CID3LibCOMWrapperApp and DLL registration.

#include "stdafx.h"
#include "ID3COM.h"
#include "ID3Fields.h"
#include "ID3Field.h"
#include "id3_fix.h"


STDMETHODIMP CID3Fields::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IID3Fields,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CID3Fields::Item(/* [in] */ VARIANT vItem, /* [retval][out] */ IID3Field **ppReturnVal)
{
	if (ppReturnVal== NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid pointer.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}
	*ppReturnVal = NULL;

	HRESULT hr ;
	hr = VariantChangeType(&vItem, &vItem, 0, VT_UI4);
	if (FAILED(hr))
	{
		AtlReportError(GetObjectCLSID(), L"Item is of an invalid type", GUID_NULL, hr);
		return DISP_E_BADVARTYPE;
	}

	ID3_Field* pID3_Field = NULL;
	try
	{
		pID3_Field = ID3_Frame_GetFieldNum(m_pFrame, vItem.ulVal-1);
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}

	if (pID3_Field != NULL)
	{
		CID3Field* pField =NULL;
		CComObject<CID3Field>::CreateInstance((CComObject<CID3Field>**)&pField);
		pField->m_pField = pID3_Field;
		pField->QueryInterface(IID_IID3Field, (void**)ppReturnVal) ;
	}

	return hr;
}




STDMETHODIMP CID3Fields::get_Count(/* [retval][out] */ long *Count)
{
	try
	{
		*Count = ID3_Frame_GetNumFields(m_pFrame);
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}
	return S_OK;
}



STDMETHODIMP CID3Fields::_NewEnum(/* [retval][out] */ IUnknown **ppEnum) 
{
	HRESULT hr = S_OK;
	*ppEnum = NULL;

	ULONG n = 0;
	VARIANT* rgv = NULL;
	IID3Field* pField = NULL;
	
	typedef CComEnum<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy<VARIANT> > CComEnumVARIANT;
	CComEnumVARIANT* pEnum = NULL;

	try
	{
		pEnum = new CComObject<CComEnumVARIANT>;
		_ASSERTE(pEnum);
		if (pEnum == NULL)
			throw E_OUTOFMEMORY;

		get_Count((long*)&n);
		_ASSERTE(n);
		rgv = (VARIANT*)malloc(n * sizeof(VARIANT));
		_ASSERTE(rgv);
		if (rgv == NULL)
			throw E_OUTOFMEMORY;

		VARIANT v;
		VariantInit(&v);
		v.vt = VT_UI4;
		for (ULONG i = 0 ; i < n ; i++)
		{
			pField = NULL;
			v.ulVal = i+1;
			hr = Item(v, &pField);
			if (FAILED(hr))
				throw hr; 
			_ASSERTE(pField);
			
			VariantInit(&rgv[i]);
			rgv[i].vt = VT_DISPATCH;
			hr = pField->QueryInterface(IID_IDispatch, (void**)&(rgv[i].pdispVal));
			if (FAILED(hr))
				throw hr;
			pField->Release();
			pField = NULL;
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
			if (pField)
				pField->Release();
			for (ULONG i = 0 ; i < n ; i++)
				VariantInit(&rgv[i]);
			free(rgv);
		}
	}

	return hr;
}
