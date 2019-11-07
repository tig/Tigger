// PageObj.cpp : Implementation of CPagerApp and DLL registration.

#include "stdafx.h"
#include "Pager.h"
#include "PageObj.h"

/////////////////////////////////////////////////////////////////////////////
//
STDMETHODIMP CPager::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IPager,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


// IPager implementation
//
HRESULT __stdcall CPager::Page( 
            /* [in] */ BSTR bstrPhoneNumber,
            /* [in] */ BSTR bstrPIN,
            /* [in] */ BSTR bstrMsg,
            /* [out][retval] */ VARIANT_BOOL __RPC_FAR *retval)
{
    HRESULT hr = S_OK;
    
    // Set retval to true (assume success) (-1 == True, 0 == False)
    *retval = -1;

    TCHAR sz[1024];
    // TODO: Replace this code with the code that dials
    wsprintf(sz, _T("Dialing %S"), bstrPhoneNumber);
    MessageBox(HWND_DESKTOP, sz, _T("Microsoft Pager"), MB_OK);

    // TODO: Replace this code with the code that enters the PIN
    wsprintf(sz, _T("Sending PIN: %S"), bstrPIN);
    MessageBox(HWND_DESKTOP, sz, _T("Microsoft Pager"), MB_OK);

    // TODO: Replace this code with the code that enters the message
    wsprintf(sz, _T("Sending Message: \"%S\""), bstrMsg);
    MessageBox(HWND_DESKTOP, sz, _T("Microsoft Pager"), MB_OK);

    // if there was an error set *retval = 0 and return

    return hr;
}
