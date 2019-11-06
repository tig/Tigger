// ID3Field.cpp : Implementation of CID3Field
#include "stdafx.h"
#include "ID3COM.h"
#include "ID3TextItems.h"
#include "ID3Field.h"
#include "id3_fix.h"
/////////////////////////////////////////////////////////////////////////////
// CID3Field


STDMETHODIMP CID3Field::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IID3Field
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CID3Field::get_Value(VARIANT * pVal)
{
	if (pVal == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid parameter.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	VariantInit(pVal);

	if (m_pField == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"m_pField is NULL", GUID_NULL, E_POINTER);
		return E_POINTER;
	}
	
	try
	{
		switch (ID3_Field_GetType(m_pField))
		{
			case ID3FTY_INTEGER:
			case ID3FTY_BITFIELD:
				{
					pVal->vt = VT_I4;
					pVal->ulVal = m_pField->Get();
				}
				break;

			case ID3FTY_BINARY:
				{
					ULONG n = m_pField->Size();

					pVal->vt = VT_ARRAY | VT_UI1;

					//Set up the bounds structure     
					SAFEARRAYBOUND  rgsabound[1];
					rgsabound[ 0 ].lLbound = 0;
					rgsabound[ 0 ].cElements = n;

					pVal->parray = SafeArrayCreate(VT_UI1,1,rgsabound);
				    if(pVal->parray == NULL)
					{
						AtlReportError(GetObjectCLSID(), L"Out of memory.", GUID_NULL, E_OUTOFMEMORY);
						return E_OUTOFMEMORY;
					}

#ifdef _DEBUG
					long LowerBounds, UpperBounds;
					SafeArrayGetLBound(pVal->parray, 1, &LowerBounds);
					SafeArrayGetUBound(pVal->parray, 1, &UpperBounds);

					ULONG cElements = UpperBounds - LowerBounds+1;
					_ASSERTE(cElements == n);
#endif

					// Reference pointer for accessing the SafeArray
					UCHAR * buff=NULL;
					SafeArrayAccessData (pVal->parray, (void**)&buff);
					m_pField->Get(buff, n);
					SafeArrayUnaccessData (pVal->parray);               
				}
				break;

			case ID3FTY_ASCIISTRING:
				/*
				{
					USES_CONVERSION;
					ULONG n = m_pField->Size()+1;
					char* sz = new char[n+1];
					pVal->vt = VT_BSTR;
					n = m_pField->Get(sz, n);
					pVal->bstrVal = SysAllocString(T2W(sz));
					delete[] sz;

				}
				break;
				*/
			case ID3FTY_UNICODESTRING:
				{
					ULONG n = m_pField->Size()+1;
					pVal->vt = VT_BSTR;
					pVal->bstrVal = SysAllocStringLen(NULL, n);
					n = m_pField->Get(pVal->bstrVal, n);
					if (n == 0)
						*pVal->bstrVal = L'\0';

				}
			break;
		}
		
	}
	catch (ID3_Error e)
	{
		VariantClear(pVal);
		ReportError(&e);
		return E_FAIL;
	}
	
	return S_OK;
}

STDMETHODIMP CID3Field::put_Value(VARIANT Val)
{
	if (m_pField == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"m_pField is NULL", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	HRESULT hr = S_OK;

	try
	{
		switch (ID3_Field_GetType(m_pField))
		{
			case ID3FTY_INTEGER:
			case ID3FTY_BITFIELD:
				{
					HRESULT hr = VariantChangeType(&Val, &Val, 0, VT_UI4);
					if (FAILED(hr))
					{
						AtlReportError(GetObjectCLSID(), L"Could not convert value to an integer type.", GUID_NULL, hr);
						return hr;
					}
					m_pField->Set(Val.ulVal);
				}
				break;

			case ID3FTY_BINARY:
				{
					if (Val.vt = (VT_ARRAY | VT_UI1))
					{
						long Dims = SafeArrayGetDim(Val.parray);         
						long UpperBounds;
						long LowerBounds;         
						if (Dims == 1) 
						{
							SafeArrayGetLBound(Val.parray, 1, &LowerBounds);
							SafeArrayGetUBound(Val.parray, 1, &UpperBounds);

							long cElements = UpperBounds - LowerBounds+1;

							// Reference pointer for accessing the SafeArray
							UCHAR* buff;

							// Variable to store the data from the SafeArray
							UCHAR* abBinaryData = (UCHAR*)malloc(cElements);

							SafeArrayAccessData (Val.parray, (void**)&buff);
							for (int i = 0; i < cElements-1; i++) 
							{
								abBinaryData[i] = buff[i];
							}
							SafeArrayUnaccessData (Val.parray);               
							try
							{
								m_pField->Set(abBinaryData, cElements);
							}
							catch (ID3_Error e)
							{
								ReportError(&e);
								free(abBinaryData);
								return E_FAIL;
							}
							free(abBinaryData);
						}  
					}
					else
					{
						AtlReportError(GetObjectCLSID(), L"This field requires a binary value.", GUID_NULL, E_INVALIDARG);
						return E_INVALIDARG;
					}
				}
				break;

			case ID3FTY_ASCIISTRING:
				{
					USES_CONVERSION;
					hr = VariantChangeType(&Val, &Val, 0, VT_BSTR);
					if (FAILED(hr))
					{
						AtlReportError(GetObjectCLSID(), L"Could not convert value to an ASCII sting.", GUID_NULL, hr);
						return hr;
					}

					m_pField->Set(W2A(Val.bstrVal)); // char*
				}
				break;

			case ID3FTY_UNICODESTRING:
				{
					USES_CONVERSION;
					hr = VariantChangeType(&Val, &Val, 0, VT_BSTR);
					if (FAILED(hr))
					{
						AtlReportError(GetObjectCLSID(), L"Could not convert value to a Unicode sting.", GUID_NULL, hr);
						return hr;
					}

					m_pField->Set(Val.bstrVal); // wchar*
				}
			break;
		}

	}
	catch (ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CID3Field::get_Size(long * pVal)
{
	if (pVal == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid parameter.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	*pVal = NULL;

	if (m_pField == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"m_pField is NULL", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	try
	{
		*pVal = (long)m_pField->Size();
	}
	catch (ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}

		
	return S_OK;
}

STDMETHODIMP CID3Field::FromFile(BSTR bstrFilename)
{
	if (bstrFilename == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid parameter.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	if (m_pField == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"m_pField is NULL", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	CHAR szFile[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, bstrFilename, -1, szFile, MAX_PATH, NULL, NULL);

	try
	{
		m_pField->FromFile(szFile);
	}
	catch (ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}
	
	
	return S_OK;
}

STDMETHODIMP CID3Field::ToFile(BSTR bstrFilename)
{
	if (bstrFilename == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid parameter.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	if (m_pField == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"m_pField is NULL", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	CHAR szFile[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, bstrFilename, -1, szFile, MAX_PATH, NULL, NULL);

	try
	{
		m_pField->ToFile(szFile);
	}
	catch (ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CID3Field::Clear()
{
	if (m_pField == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"m_pField is NULL", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	try
	{
		m_pField->Clear();
	}
	catch (ID3_Error e)
	{
		ReportError(&e);
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CID3Field::get_ID(ID3LibFieldID *pVal)
{
	*pVal = (ID3LibFieldID)m_pField->name;
	return S_OK;
}

STDMETHODIMP CID3Field::get_TextItems(IID3TextItems **ppITextItems)
{
	if (ppITextItems== NULL)
	{
		AtlReportError(GetObjectCLSID(), L"Invalid parameter.", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	*ppITextItems = NULL;

	if (m_pField == NULL)
	{
		AtlReportError(GetObjectCLSID(), L"m_pField is NULL", GUID_NULL, E_POINTER);
		return E_POINTER;
	}

	CID3TextItems* pID3TextItems =NULL;
	CComObject<CID3TextItems>::CreateInstance((CComObject<CID3TextItems>**)&pID3TextItems);
	pID3TextItems->m_pField = m_pField;
	pID3TextItems->QueryInterface(IID_IID3TextItems, (void**)ppITextItems) ;

	return S_OK;
}
