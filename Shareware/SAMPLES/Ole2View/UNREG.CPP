/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

LONG RecursiveRegDeleteKey(HKEY hParentKey, LPTSTR szKeyName);
void UnregisterInterfaces(ITypeLib* pTypeLib);

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Load type library before unregistering it.
	ITypeLib* pTypeLib = NULL;
	if (FAILED(LoadRegTypeLib(_tlid, _wVerMajor, _wVerMinor, 0, &pTypeLib)))
		pTypeLib = NULL;

	if (!AfxOleUnregisterTypeLib(_tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	// If type library was unregistered successfully,
	// also unregister interfaces.
	if (pTypeLib != NULL)
	{
		ITypeLib* pDummy = NULL;
		if (FAILED(LoadRegTypeLib(_tlid, _wVerMajor, _wVerMinor, 0, &pDummy)))
			UnregisterInterfaces(pTypeLib);
		else
			pDummy->Release();

		pTypeLib->Release();
	}

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}

#include <winreg.h>
#define ERROR_BADKEY_WIN16 2 // needed when running on Win32s

LONG RecursiveRegDeleteKey(HKEY hParentKey, LPTSTR szKeyName)
{
	DWORD   dwIndex = 0L;
	TCHAR   szSubKeyName[256];
	HKEY    hCurrentKey;
	DWORD   dwResult;

	if ((dwResult = RegOpenKey(hParentKey, szKeyName, &hCurrentKey)) ==
		ERROR_SUCCESS)
	{
		// Remove all subkeys of the key to delete
		while ((dwResult = RegEnumKey(hCurrentKey, 0, szSubKeyName, 255)) ==
			ERROR_SUCCESS)
		{
			if ((dwResult = RecursiveRegDeleteKey(hCurrentKey,
				szSubKeyName)) != ERROR_SUCCESS)
				break;
		}

		// If all went well, we should now be able to delete the requested key
		if ((dwResult == ERROR_NO_MORE_ITEMS) || (dwResult == ERROR_BADKEY) ||
			(dwResult == ERROR_BADKEY_WIN16))
		{
			dwResult = RegDeleteKey(hParentKey, szKeyName);
		}
	}

	RegCloseKey(hCurrentKey);
	return dwResult;
}

void UnregisterInterfaces(ITypeLib* pTypeLib)
{
	TCHAR szKey[128];
	_tcscpy(szKey, _T("Interface\\"));
	LPTSTR pszGuid = szKey + _tcslen(szKey);

	int cTypeInfo = pTypeLib->GetTypeInfoCount();

	for (int i = 0; i < cTypeInfo; i++)
	{
		TYPEKIND tk;
		if (SUCCEEDED(pTypeLib->GetTypeInfoType(i, &tk)) &&
			(tk == TKIND_DISPATCH || tk == TKIND_INTERFACE))
		{
			ITypeInfo* pTypeInfo = NULL;
			if (SUCCEEDED(pTypeLib->GetTypeInfo(i, &pTypeInfo)))
			{
				TYPEATTR* pTypeAttr;
				if (SUCCEEDED(pTypeInfo->GetTypeAttr(&pTypeAttr)))
				{
					StringFromGUID2(pTypeAttr->guid, pszGuid, 39);
					RecursiveRegDeleteKey(HKEY_CLASSES_ROOT, szKey);
					pTypeInfo->ReleaseTypeAttr(pTypeAttr);
				}

				pTypeInfo->Release();
			}
		}
	}
}
