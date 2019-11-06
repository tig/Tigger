// ID3VersInfo.cpp : Implementation of CID3LibCOMWrapperApp and DLL registration.

#include "stdafx.h"
#include "ID3COM.h"
#include "ID3VersInfo.h"

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP CID3VersInfo::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IID3VersInfo,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CID3VersInfo::get_Name(/* [retval][out] */ BSTR *pbstrReturnVal)
{
	*pbstrReturnVal = SysAllocString(L"ID3Lib");

	return S_OK;
}
        
STDMETHODIMP CID3VersInfo::get_Version(/* [retval][out] */ long *plReturnVal)
{
	*plReturnVal = ID3LIB_VER;
	return S_OK;
}
        
STDMETHODIMP CID3VersInfo::get_Revision(/* [retval][out] */ long *plReturnVal)
{
	*plReturnVal = ID3LIB_REV;
	return S_OK;
}

