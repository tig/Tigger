// ID3Frame.cpp : Implementation of CID3Frame
#include "stdafx.h"
#include "ID3COM.h"
#include "ID3Frame.h"
#include "ID3Field.h"
#include "ID3Fields.h"
#include "ID3_Fix.h"

STDMETHODIMP CID3Frame::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IID3Frame
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CID3Frame::Clear()
{
	if (m_pFrame == NULL)
	{
		return S_OK;
	}

	try
	{
		// Hack: ID3Lib's Clear doesn't reset the fields. this gets around that.
		m_pFrame->SetID(m_pFrame->GetID());
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}
	return S_OK;
}


STDMETHODIMP CID3Frame::get_ID(BSTR *pbstr)
{
	if (pbstr == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid parameter.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	*pbstr = NULL;

	if (m_pFrame == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Frame is not initialized", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	try
	{
		USES_CONVERSION;
		*pbstr = SysAllocString(T2W(ID3_Frame_GetStringID(m_pFrame)));
		_ASSERTE(*pbstr);
		if (*pbstr == NULL)
		{
			AtlReportError(GetObjectCLSID(), L"Out of memory", GUID_NULL, E_POINTER);
			return E_OUTOFMEMORY;
		}

	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CID3Frame::put_ID(BSTR bstr)
{

	if (m_pFrame == NULL)
	{
		m_pFrame = new ID3_Frame();
	}

	if (bstr == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid paramter (ID is NULL)", GUID_NULL, E_INVALIDARG);
		return E_POINTER;
	}

	try
	{
		USES_CONVERSION;
		ID3_Frame_SetStringID(m_pFrame, W2T(bstr));
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}

	return S_OK;
}


STDMETHODIMP CID3Frame::get_Fields(IID3Fields **ppReturnVal)
{
	if (ppReturnVal== NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid parameter.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	*ppReturnVal = NULL;

	if (m_pFrame == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Frame has not been initalized", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	CID3Fields* pID3Fields =NULL;
	CComObject<CID3Fields>::CreateInstance((CComObject<CID3Fields>**)&pID3Fields);
	pID3Fields->m_pFrame = m_pFrame;
	pID3Fields->QueryInterface(IID_IID3Fields, (void**)ppReturnVal) ;

	return S_OK;
}

void CID3Frame::SetFrame(ID3_Frame *pID3_Frame)
{
	_ASSERTE(!m_pFrame);
	_ASSERTE(pID3_Frame);
	m_pFrame = pID3_Frame;

}

STDMETHODIMP CID3Frame::GetFieldByID(ID3LibFieldID fieldID, IID3Field **ppField)
{
	if (ppField== NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid pointer.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}
	*ppField = NULL;


	ID3_Field* pID3_Field = NULL;
	try
	{
		pID3_Field = &m_pFrame->Field((ID3_FieldID)fieldID);
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
		pField->QueryInterface(IID_IID3Field, (void**)ppField) ;
	}

	return S_OK;
}
