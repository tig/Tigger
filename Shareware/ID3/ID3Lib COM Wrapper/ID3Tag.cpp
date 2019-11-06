// ID3Tag.cpp : Implementation of CID3Tag
#include "stdafx.h"
#include "ID3COM.h"
#include "ID3Frames.h"
#include "ID3Tag.h"
#include "ID3Frame.h"
#include "ID3Field.h"
#include "ID3VersInfo.h"

/////////////////////////////////////////////////////////////////////////////
// CID3Tag

CID3Tag::CID3Tag()
{
	m_pFrames = NULL;
	m_pTag = new ID3_Tag();

	CID3Frames* pFrames=NULL;
	HRESULT hr = CID3Frames::_CreatorClass::CreateInstance(NULL, IID_IID3Frames, (void**)&pFrames);
	if (FAILED(hr))
	{
		AtlReportError(GetObjectCLSID(), L"Could not create ID3Frames object.", GUID_NULL, hr);
		return;
	}
	m_pFrames = pFrames;
	m_pFrames->m_pTag = m_pTag;

}

CID3Tag::~CID3Tag()
{
	delete m_pTag;
	m_pTag = NULL;

	delete m_pFrames;
	m_pFrames = NULL;
}


STDMETHODIMP CID3Tag::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IID3Tag
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


STDMETHODIMP CID3Tag::GetVersionInfo(/*[out,retval]*/ IID3VersInfo** ppReturnVal)
{
	if (ppReturnVal == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}

	*ppReturnVal = NULL;

	CID3VersInfo* pvers = NULL;
	HRESULT hr = CID3VersInfo::_CreatorClass::CreateInstance(NULL, IID_IID3VersInfo, (void**)ppReturnVal);
	if (FAILED(hr))
	{
		AtlReportError(GetObjectCLSID(), L"Could not create ID3VersInfo object.", GUID_NULL, hr);
	}

	return hr;
}

STDMETHODIMP CID3Tag::GetNumBytes(/*[in]*/ VARIANT vHeader, /*[out,retval]*/ long* pReturnVal)
{
	if (pReturnVal == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}

	*pReturnVal = 0;

	if (vHeader.vt = (VT_ARRAY | VT_UI1))
	{
		UCHAR* buff = NULL;
		HRESULT hr = SafeArrayAccessData (vHeader.parray, (void**)&buff);
		if (FAILED(hr))
		{
			AtlReportError(GetObjectCLSID(), L"Header argument is an invalid array", GUID_NULL, E_INVALIDARG);
			return E_INVALIDARG;
		}

		try
		{
			*pReturnVal = ID3_IsTagHeader(buff);
		}
		catch (ID3_Error e)
		{
			ReportError(&e);
			return E_FAIL;
		}
	
		SafeArrayUnaccessData(vHeader.parray);               
	}
	else
	{
		AtlReportError(GetObjectCLSID(), L"Header must be an array of bytes.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}

	return S_OK;
}


STDMETHODIMP CID3Tag::Clear()
{
	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	try
	{
		m_pTag->Clear();
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CID3Tag::get_HasChanged(VARIANT_BOOL *pVal)
{
	if (pVal == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}

	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	*pVal = m_pTag->HasChanged() ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CID3Tag::put_Unsync(VARIANT_BOOL UnSync)
{
	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	m_pTag->SetUnsync(UnSync == VARIANT_TRUE);

	return S_OK;
}

STDMETHODIMP CID3Tag::get_Unsync(VARIANT_BOOL* UnSync)
{
	if (UnSync== NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}

	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	//m_pTag->SetUnsync(UnSync == VARIANT_TRUE);
	*UnSync = VARIANT_TRUE;

	return E_NOTIMPL;
}


STDMETHODIMP CID3Tag::put_ExtendedHeader(VARIANT_BOOL ext)
{
	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	m_pTag->SetExtendedHeader(ext == VARIANT_TRUE);

	return S_OK;
}

STDMETHODIMP CID3Tag::get_ExtendedHeader(VARIANT_BOOL *ext)
{
	if (ext== NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}
	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	*ext = VARIANT_TRUE;

	return E_NOTIMPL;
}

STDMETHODIMP CID3Tag::put_Compression(VARIANT_BOOL comp)
{
	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	m_pTag->SetCompression(comp == VARIANT_TRUE);

	return S_OK;
}

STDMETHODIMP CID3Tag::get_Compression(VARIANT_BOOL *comp)
{
	if (comp== NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}

	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	*comp = VARIANT_TRUE;

	return E_NOTIMPL;
}

STDMETHODIMP CID3Tag::put_Padding(VARIANT_BOOL pad)
{
	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	m_pTag->SetPadding(pad == VARIANT_TRUE);

	return S_OK;
}

STDMETHODIMP CID3Tag::get_Padding(VARIANT_BOOL *pad)
{
	if (pad== NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}

	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	*pad = VARIANT_TRUE;

	return E_NOTIMPL;
}


STDMETHODIMP CID3Tag::get_Frames(IID3Frames **ppFrames)
{
	if (ppFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}
	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	m_pFrames->QueryInterface(IID_IID3Frames, (void**)ppFrames);

	return S_OK;
}

STDMETHODIMP CID3Tag::Parse(VARIANT vHeader, VARIANT vBuffer)
{
	ID3_Tag* pTag = NULL;
	HRESULT hr = S_OK;
	if (vHeader.vt = (VT_ARRAY | VT_UI1))
	{
		long lTagSize = 0;
		hr = GetNumBytes(vHeader, &lTagSize);
		if (FAILED(hr))
			return hr;
		if (lTagSize == -1)
		{
			AtlReportError(GetObjectCLSID(), L"Invalid ID3 Header", GUID_NULL, E_INVALIDARG);
			return E_INVALIDARG;
		}

		// Reference pointer for accessing the SafeArray
		UCHAR* header= NULL;
		hr = SafeArrayAccessData(vHeader.parray, (void**)&header);
		if (FAILED(hr))
		{
			AtlReportError(GetObjectCLSID(), L"Header argument is an invalid array", GUID_NULL, E_INVALIDARG);
			return E_INVALIDARG;
		}

		UCHAR* buffer = NULL;
		hr = SafeArrayAccessData(vBuffer.parray, (void**)&buffer);
		if (FAILED(hr))
		{
			SafeArrayUnaccessData(vBuffer.parray);               
			AtlReportError(GetObjectCLSID(), L"Buffer argument is an invalid array", GUID_NULL, E_INVALIDARG);
			return E_INVALIDARG;
		}

		try
		{
			pTag = new ID3_Tag();
			_ASSERTE(pTag);
			if (pTag == NULL)
				ID3_THROW(ID3E_NoMemory);

			pTag->Parse(header, buffer);
		}
		catch (ID3_Error e)
		{
			delete pTag;
			SafeArrayUnaccessData (vHeader.parray);               
			SafeArrayUnaccessData (vBuffer.parray);               
			ReportError(&e);
			return E_FAIL;
		}
	
		SafeArrayUnaccessData (vHeader.parray);               
		SafeArrayUnaccessData (vBuffer.parray);               
	}
	else
	{
		hr = E_INVALIDARG;
		AtlReportError(GetObjectCLSID(), L"Header must be an array of bytes.", GUID_NULL, hr);
	}

	if (SUCCEEDED(hr) && pTag)
	{
		if (m_pTag)
		{
			delete m_pTag;
			m_pTag = pTag;
		}

		if (m_pFrames)
		{
			delete m_pFrames;
			m_pFrames = NULL;

			CID3Frames* pFrames=NULL;
			hr = CID3Frames::_CreatorClass::CreateInstance(NULL, IID_IID3Frames, (void**)&pFrames);
			if (FAILED(hr))
			{
				AtlReportError(GetObjectCLSID(), L"Could not create ID3Frames object.", GUID_NULL, hr);
				return hr;
			}
			m_pFrames = pFrames;
			m_pFrames->m_pTag = m_pTag;
		}
	}

	return hr;
}

// TODO: Think about how to make it so this will barf if the file does not
// exist instead of creating a file...
STDMETHODIMP CID3Tag::Attach(BSTR bstrFilename, VARIANT_BOOL fCreate, long* pulVal)
{
	if (bstrFilename == NULL || pulVal == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}

	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	*pulVal = 0;

	CHAR szFile[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, bstrFilename, -1, szFile, MAX_PATH, NULL, NULL);
	// if fCreate == FALSE then we need to verify that the file exists
	//
	if (fCreate == VARIANT_FALSE)
	{
		HANDLE h = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);
		if (h == INVALID_HANDLE_VALUE)
		{
			HRESULT hr = HRESULT_FROM_NT(GetLastError());
			AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, hr);
			return hr;
	
		}
		else
			CloseHandle(h);
	}

	try
	{
		if (m_pTag)
		{
			delete m_pTag;
			m_pTag = new ID3_Tag();
		}

		if (m_pFrames)
		{
			delete m_pFrames;
			m_pFrames = NULL;

			CID3Frames* pFrames=NULL;
			HRESULT hr = CID3Frames::_CreatorClass::CreateInstance(NULL, IID_IID3Frames, (void**)&pFrames);
			if (FAILED(hr))
			{
				AtlReportError(GetObjectCLSID(), L"Could not create ID3Frames object.", GUID_NULL, hr);
				return hr;
			}
			m_pFrames = pFrames;
			m_pFrames->m_pTag = m_pTag;
		}

		*pulVal = m_pTag->Link(szFile);
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CID3Tag::Update()
{
	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	try
	{
		m_pTag->Update();
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CID3Tag::Strip(VARIANT_BOOL v1Also)
{
	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	try
	{
		m_pTag->Strip(v1Also == VARIANT_TRUE);
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CID3Tag::get_Size(long* plsize)
{
	if (plsize== NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}

	*plsize = 0;

	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid internal pointer.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}
	try
	{
		*plsize = (long)m_pTag->Size();
	}
	catch(ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CID3Tag::Render(VARIANT *pBuffer)
{
	HRESULT hr = S_OK;
	if (pBuffer == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid parameter.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	VariantInit(pBuffer);

	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid internal pointer.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	try
	{
		ULONG n = m_pTag->Size();

		pBuffer->vt = VT_ARRAY | VT_UI1;

		//Set up the bounds structure     
		SAFEARRAYBOUND  rgsabound[1];
		rgsabound[ 0 ].lLbound = 0;
		rgsabound[ 0 ].cElements = n;

		pBuffer->parray = SafeArrayCreate(VT_UI1,1,rgsabound);
		if(pBuffer->parray == NULL)
		{
			AtlReportError(GetObjectCLSID(), L"Out of memory.", GUID_NULL, E_OUTOFMEMORY);
			return E_OUTOFMEMORY;
		}

#ifdef _DEBUG
		long LowerBounds, UpperBounds;
		SafeArrayGetLBound(pBuffer->parray, 1, &LowerBounds);
		SafeArrayGetUBound(pBuffer->parray, 1, &UpperBounds);

		ULONG cElements = UpperBounds - LowerBounds+1;
		_ASSERTE(cElements == n);
#endif

		// Reference pointer for accessing the SafeArray
		UCHAR * buff=NULL;
		hr = SafeArrayAccessData (pBuffer->parray, (void**)&buff);
		if (FAILED(hr))
		{
			VariantInit(pBuffer);
			AtlReportError(GetObjectCLSID(), L"Out of memory.", GUID_NULL, E_OUTOFMEMORY);
			return E_OUTOFMEMORY;
		}
		m_pTag->Render(buff);
		SafeArrayUnaccessData (pBuffer->parray);               
	}
	catch (ID3_Error e)
	{
		VariantClear(pBuffer);
		ReportError(&e);
		return E_FAIL;
	}
	
	return hr;
}

HRESULT CID3Tag::GetTextFrame(WCHAR* wszFrameID, BSTR *pVal)
{
	if (pVal== NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}
	if (m_pTag == NULL || m_pFrames == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"CID3Tag: Unexpected NULL pointer", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	HRESULT hr = S_OK;
	*pVal = NULL;

	IID3Frames* pFrames = NULL;
	hr = m_pFrames->QueryInterface(IID_IID3Frames, (void**)&pFrames);
	if (SUCCEEDED(hr))
	{
		VARIANT v, vEmpty;
		VariantInit(&vEmpty);
		vEmpty.vt = VT_ERROR;
		vEmpty.scode = DISP_E_PARAMNOTFOUND; // empty optional parameter

		VariantInit(&v);
		v.vt = VT_BSTR;
		v.bstrVal = SysAllocString(wszFrameID);

		IID3Frame* pFrame = NULL;
		hr = pFrames->Item(v, vEmpty, vEmpty, &pFrame);
		VariantClear(&v);
		if (SUCCEEDED(hr))
		{
			IID3Field* pField = NULL;
			hr = pFrame->GetFieldByID(id3TEXT, &pField);
			if (SUCCEEDED(hr))
			{
				VARIANT vField;
				VariantInit(&vField);
				hr = pField->get_Value(&vField);
				if (SUCCEEDED(hr))
				{
					VariantChangeType(&vField, &vField, 0, VT_BSTR);
					*pVal = vField.bstrVal;
				}
				pField->Release();
			}
			pFrame->Release();
		}

		pFrames->Release();
	}
	else
	{
		AtlReportError(GetObjectCLSID(), L"QueryInterface error.", GUID_NULL, hr);
	}

	return hr;
}

HRESULT CID3Tag::PutTextFrame(WCHAR* wszFrameID, BSTR newVal)
{
	if (newVal == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid argument.", GUID_NULL, E_INVALIDARG);
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

	IID3Frames* pFrames = NULL;
	hr = m_pFrames->QueryInterface(IID_IID3Frames, (void**)&pFrames);
	if (SUCCEEDED(hr))
	{
		VARIANT v, vEmpty;
		VariantInit(&vEmpty);
		vEmpty.vt = VT_ERROR;
		vEmpty.scode = DISP_E_PARAMNOTFOUND; // empty optional parameter

		VariantInit(&v);
		v.vt = VT_BSTR;
		v.bstrVal = newVal;

		IID3Frame* pFrame = NULL;
		hr = pFrames->Item(v, vEmpty, vEmpty, &pFrame);
		VariantClear(&v);
		if (FAILED(hr))
		{
			// Need to create it then
			CID3Frame* p =NULL;
			CComObject<CID3Frame>::CreateInstance((CComObject<CID3Frame>**)&p);
			hr = p->QueryInterface(IID_IID3Frame, (void**)&pFrame) ;
			if (SUCCEEDED(hr))
			{
				hr = p->put_ID(wszFrameID);
				if (SUCCEEDED(hr))
				{
					hr = pFrames->Add(pFrame);
				}
			}
			else
			{
				AtlReportError(GetObjectCLSID(), L"QueryInterface error.", GUID_NULL, hr);
			}
		}


		if (SUCCEEDED(hr) && pFrame)
		{
			IID3Field* pField = NULL;
			hr = pFrame->GetFieldByID(id3TEXT, &pField);
			if (SUCCEEDED(hr))
			{
				VARIANT vField;
				VariantInit(&vField);
				vField.vt = VT_BSTR;
				vField.bstrVal = newVal;
				hr = pField->put_Value(vField);
				pField->Release();
			}

			hr = pFrame->GetFieldByID(id3LANGUAGE, &pField);
			if (SUCCEEDED(hr))
			{
				VARIANT vField;
				VariantInit(&vField);
				vField.vt = VT_BSTR;
				vField.bstrVal = SysAllocString(L"eng"); // TODO: use locale 
				hr = pField->put_Value(vField);
				pField->Release();
			}

			hr = pFrame->GetFieldByID(id3DESCRIPTION, &pField);
			if (SUCCEEDED(hr))
			{
				VARIANT vField;
				VariantInit(&vField);
				vField.vt = VT_BSTR;
				vField.bstrVal = SysAllocString(L""); // TODO: use locale 
				hr = pField->put_Value(vField);
				pField->Release();
			}

			pFrame->Release();
		}

		pFrames->Release();
	}
	else
	{
		AtlReportError(GetObjectCLSID(), L"QueryInterface error.", GUID_NULL, hr);
	}

	return hr;
}

STDMETHODIMP CID3Tag::get_Title(BSTR *pVal)
{
	return GetTextFrame(L"TIT1", pVal);
}

STDMETHODIMP CID3Tag::put_Title(BSTR newVal)
{
	return PutTextFrame(L"TIT1", newVal);
}

STDMETHODIMP CID3Tag::get_Album(BSTR *pVal)
{
	return GetTextFrame(L"TALB", pVal);
}

STDMETHODIMP CID3Tag::put_Album(BSTR newVal)
{
	return PutTextFrame(L"TALB", newVal);
}

STDMETHODIMP CID3Tag::get_Artist(BSTR *pVal)
{
	return GetTextFrame(L"TPE1", pVal);
}

STDMETHODIMP CID3Tag::put_Artist(BSTR newVal)
{
	return PutTextFrame(L"TPE1", newVal);
}

STDMETHODIMP CID3Tag::get_Year(BSTR *pVal)
{
	return GetTextFrame(L"TYER", pVal);
}

STDMETHODIMP CID3Tag::put_Year(BSTR newVal)
{
	return PutTextFrame(L"TYER", newVal);
}

STDMETHODIMP CID3Tag::get_Genre(BSTR *pVal)
{
	return GetTextFrame(L"TCON", pVal);
}

STDMETHODIMP CID3Tag::put_Genre(BSTR newVal)
{
	return PutTextFrame(L"TCON", newVal);
}

STDMETHODIMP CID3Tag::get_Comment(BSTR *pVal)
{
	return GetTextFrame(L"COMM", pVal);
}

STDMETHODIMP CID3Tag::put_Comment(BSTR newVal)
{
	return PutTextFrame(L"COMM", newVal);
}
