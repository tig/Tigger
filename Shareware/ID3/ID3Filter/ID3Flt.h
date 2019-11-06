// ID3Flt.h : Declaration of the CID3Flt

#ifndef __ID3FLT_H_
#define __ID3FLT_H_

#include "resource.h"       // main symbols
const WCHAR SZCIREGKEY[] = L"system\\currentcontrolset\\control\\contentindex";

/////////////////////////////////////////////////////////////////////////////
// CID3Flt
class ATL_NO_VTABLE CID3Flt : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CID3Flt, &CLSID_ID3Filter>,
//	public IID3Filter,
	public IFilter,
//	public IPersist,
	public IPersistFile
{
public:
	CID3Flt();
	~CID3Flt();
 

// DECLARE_REGISTRY_RESOURCEID(IDR_ID3FLT)
static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
{
	HKEY hkey = NULL;
	ULONG cbData = 0;
	ULONG ulType = REG_MULTI_SZ;
	WCHAR* pData = NULL;
	WCHAR szModule[_MAX_PATH+1];
	memset(szModule, 0, (_MAX_PATH+1)*sizeof(WCHAR)); // so we get the terminating NULLs
	WCHAR *szModuleFileName = NULL;
	GetModuleFileName(_pModule->GetModuleInstance(), szModule, _MAX_PATH);
	ULONG cbModule = (lstrlen(szModule)+1)*sizeof(WCHAR);
	szModuleFileName = wcsrchr(szModule, L'\\')+1;

	try
	{
		if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, SZCIREGKEY, 0, KEY_QUERY_VALUE | KEY_WRITE, &hkey))
		{
#ifdef _DEBUG
			if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_LOCAL_MACHINE, SZCIREGKEY, 
									NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL))
				throw HRESULT_FROM_NT(GetLastError());
#else
			throw HRESULT_FROM_NT(GetLastError());
#endif
		}

		if (ERROR_SUCCESS != RegQueryValueEx(hkey, L"DllsToRegister", NULL, &ulType, NULL, &cbData) || cbData == 0)
		{
#ifdef _DEBUG
			ulType = REG_MULTI_SZ;
			cbData = 2;
#else
			throw HRESULT_FROM_NT(GetLastError());
#endif
		}
		_ASSERTE(ulType == REG_MULTI_SZ);

		// Allocate enough for the original plus our module filename (incase it's needed).
		pData = (WCHAR*)malloc(cbData+cbModule);
		memset(pData, 0, cbData+cbModule);
		_ASSERTE(pData);
#ifdef _DEBUG
		if (cbData > 2)
#endif
		if (ERROR_SUCCESS != RegQueryValueEx(hkey, L"DllsToRegister", NULL, &ulType, (BYTE*)pData, &cbData))
			throw HRESULT_FROM_NT(GetLastError());
		// First make sure we're not already in there. If we are, nuke us.
		WCHAR* szCur = pData;
		while (*szCur)
		{
			if (wcsstr(szCur, szModuleFileName))
			{
				// move everything else up
				ULONG cbBytes = ((lstrlen(szCur)+1)*sizeof(WCHAR));
				memcpy((char*)szCur, ((char*)szCur)+cbBytes, cbData+2 - cbBytes);
				cbData -= cbBytes;
				break;
			}

			while(*szCur) // go to terminating NULL
				szCur++;
			szCur++; // go past terminating NULL (if last one this will be NULL too)
		}

		if (bRegister)
		{
			// Append
			memcpy(((char*)pData)+cbData-2, szModule, cbModule+4);
			cbData += cbModule;
		}
		if (ERROR_SUCCESS != RegSetValueEx(hkey, L"DllsToRegister", 0, ulType, (BYTE*)pData, cbData))
			throw HRESULT_FROM_NT(GetLastError());

		RegCloseKey(hkey);
	}
	catch(HRESULT hr)
	{
		_ASSERTE(hr == S_OK);

		if (hkey != NULL)
			RegCloseKey(hkey);
		if (pData != NULL)
			free(pData);

		hr = hr;
	}

	HRESULT hr = _Module.UpdateRegistryFromResource(IDR_ID3FLT, bRegister);
	_ASSERTE(hr == S_OK);
	return hr;
}

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CID3Flt)
	//COM_INTERFACE_ENTRY(IID3Filter)
	COM_INTERFACE_ENTRY(IFilter)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistFile)
END_COM_MAP()

public:
	// IFilter
       STDMETHOD(Init)( 
            /* [in] */ ULONG grfFlags,
            /* [in] */ ULONG cAttributes,
            /* [size_is][in] */ const FULLPROPSPEC __RPC_FAR *aAttributes,
            /* [out] */ ULONG __RPC_FAR *pFlags);
        
        STDMETHOD(GetChunk)( 
            /* [out] */ STAT_CHUNK __RPC_FAR *pStat);
        
        STDMETHOD(GetText)( 
            /* [out][in] */ ULONG __RPC_FAR *pcwcBuffer,
            /* [size_is][out] */ WCHAR __RPC_FAR *awcBuffer);
        
        STDMETHOD(GetValue)( 
            /* [out] */ PROPVARIANT __RPC_FAR *__RPC_FAR *ppPropValue);
        
        STDMETHOD(BindRegion)( 
            /* [in] */ FILTERREGION origPos,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppunk);

			
	// IPersistFile
		STDMETHOD(GetClassID)(CLSID *pClassID);

		STDMETHOD(IsDirty)( void);
        
        STDMETHOD(Load)( 
            /* [in] */ LPCOLESTR pszFileName,
            /* [in] */ DWORD dwMode);
        
        STDMETHOD(Save)( 
            /* [unique][in] */ LPCOLESTR pszFileName,
            /* [in] */ BOOL fRemember);
        
        STDMETHOD(SaveCompleted)( 
            /* [unique][in] */ LPCOLESTR pszFileName);
        
        STDMETHOD(GetCurFile)( 
            /* [out] */ LPOLESTR __RPC_FAR *ppszFileName);
        

private:
	HRESULT ID3ErrToHR(ID3_Err e);

	WCHAR *			m_pwszFileName;       // File that has been loaded
	ID3_Tag			m_ID3Tag;		
    ULONG           m_cAttributes;        // Count of attributes
    CFullPropSpec**	m_rgpAttributes;     // Attributes to filter
    ULONG           m_ulChunkID;          // Current chunk id
	ID3_Field*		m_pField;			// Current field
 
};

#endif //__ID3FLT_H_
