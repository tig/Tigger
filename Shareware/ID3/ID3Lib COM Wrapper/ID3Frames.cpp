// ID3Frames.cpp : Implementation of CID3LibCOMWrapperApp and DLL registration.

#include "stdafx.h"
#include "ID3COM.h"
#include "ID3Frames.h"
#include "ID3Frame.h"
#include "id3_fix.h"

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP CID3Frames::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IID3Frames,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CID3Frames::Item(/* [in] */ VARIANT vItem,  /* [in, optional]*/ VARIANT FieldID,/*  [in, optional]*/ VARIANT Data, /* [retval][out] */ IID3Frame **ppReturnVal)
{
	if (ppReturnVal== NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid pointer.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}
	*ppReturnVal = NULL;

	HRESULT hr = S_OK;
	ID3_Frame* pID3_Frame = NULL;
	if (vItem.vt == VT_BSTR)
	{
		// 4 character ID3v2 identifier specified
		try
		{
			pID3_Frame = ID3_Tag_Find(m_pTag, vItem.bstrVal);
			if (pID3_Frame == NULL)
				ID3_THROW(ID3E_InvalidFrameID);
		}
		catch(ID3_Error e)
		{
			ReportError(&e);
			return E_FAIL;
		}
	}
	else
	{
		// collection index specified
		hr = VariantChangeType(&vItem, &vItem, 0, VT_UI4);
		if (FAILED(hr))
		{
			AtlReportError(GetObjectCLSID(), L"vItem is of an invalid type", GUID_NULL, hr);
			return DISP_E_BADVARTYPE;
		}

		try
		{
			pID3_Frame = m_pTag->GetFrameNum(vItem.ulVal-1);
			if (pID3_Frame == NULL)
				ID3_THROW(ID3E_InvalidFrameID);
		}
		catch(ID3_Error e)
		{
			ReportError(&e);
			return E_FAIL;
		}

	}

	// FieldID is optional. If it was specififed vt != VT_ERROR...
	if (!(FieldID.vt == VT_ERROR && FieldID.scode == DISP_E_PARAMNOTFOUND))
	{
		hr = VariantChangeType(&FieldID, &FieldID, 0, VT_UI4);
		if (FAILED(hr))
		{
			AtlReportError(GetObjectCLSID(), L"FieldID is of an invalid type", GUID_NULL, hr);
			return DISP_E_BADVARTYPE;
		}
	}

	// Data is optional. If it was specififed vt != VT_ERROR...
	if (!(Data.vt == VT_ERROR && Data.scode == DISP_E_PARAMNOTFOUND))
	{
		if (Data.vt != VT_BSTR)
		{
			hr = VariantChangeType(&Data, &Data, 0, VT_UI4);
			if (FAILED(hr))
			{
				AtlReportError(GetObjectCLSID(), L"Data is of an invalid type", GUID_NULL, hr);
				return DISP_E_BADVARTYPE;
			}
		}
	}

	try
	{
		if (pID3_Frame && FieldID.vt != VT_ERROR)
		{
			if (Data.vt == VT_BSTR)
				pID3_Frame = m_pTag->Find(pID3_Frame->GetID(), (ID3_FieldID)FieldID.ulVal, Data.bstrVal);
			else
				pID3_Frame = m_pTag->Find(pID3_Frame->GetID(), (ID3_FieldID)FieldID.ulVal, Data.ulVal);
			if (pID3_Frame == NULL)
				ID3_THROW(ID3E_InvalidFrameID);
		}
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}

	if (pID3_Frame != NULL)
	{
		CID3Frame* pFrame =NULL;
		CComObject<CID3Frame>::CreateInstance((CComObject<CID3Frame>**)&pFrame);
		pFrame->SetFrame(pID3_Frame);
		pFrame->QueryInterface(IID_IID3Frame, (void**)ppReturnVal) ;
	}

	return hr;
}

STDMETHODIMP CID3Frames::get_Count(/* [retval][out] */ long *Count)
{
	try
	{
		*Count = m_pTag->NumFrames();
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CID3Frames::Add(/* [in] */ IID3Frame *frame)
{
	try
	{
		m_pTag->AddFrame(((CComObject<CID3Frame>*)frame)->m_pFrame);
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CID3Frames::Remove(/* [in] */ IID3Frame* pFrame)
{
	try
	{
		m_pTag->RemoveFrame(((CComObject<CID3Frame>*)pFrame)->m_pFrame);
		((CComObject<CID3Frame>*)pFrame)->m_pFrame = NULL;
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}

	return S_OK;
}


STDMETHODIMP CID3Frames::_NewEnum(/* [retval][out] */ IUnknown **ppEnum) 
{
	HRESULT hr = S_OK;
	*ppEnum = NULL;
	
	ULONG n = 0;
	VARIANT* rgv = NULL;
	IID3Frame* pFrame = NULL;
	
	typedef CComEnum<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy<VARIANT> > CComEnumVARIANT;
	CComEnumVARIANT* pEnum = NULL;

	try
	{
		pEnum = new CComObject<CComEnumVARIANT>;
		_ASSERTE(pEnum);
		if (pEnum == NULL)
			throw E_OUTOFMEMORY;

		n = m_pTag->NumFrames();
		rgv = (VARIANT*)malloc(n * sizeof(VARIANT));
		_ASSERTE(rgv);
		if (rgv == NULL)
			throw E_OUTOFMEMORY;

		VARIANT v, vEmpty;
		VariantInit(&v);
		VariantInit(&vEmpty);
		vEmpty.vt = VT_ERROR;
		vEmpty.scode = DISP_E_PARAMNOTFOUND; // empty arg
		v.vt = VT_UI4;
		for (ULONG i = 0 ; i < n ; i++)
		{
			pFrame = NULL;
			v.ulVal = i+1;
			hr = Item(v, vEmpty, vEmpty, &pFrame);
			if (FAILED(hr))
				throw hr; 
			_ASSERTE(pFrame);
			
			VARIANT* vTemp = &rgv[i];
			VariantInit(vTemp);
			rgv[i].vt = VT_DISPATCH;
			hr = pFrame->QueryInterface(IID_IDispatch, (void**)&(rgv[i].pdispVal));
			if (FAILED(hr))
				throw hr;
			pFrame->Release();
			pFrame = NULL;
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
			if (pFrame)
				pFrame->Release();
			for (ULONG i = 0 ; i < n ; i++)
				VariantInit(&rgv[i]);
			free(rgv);
		}
	}

	return hr;
}
