// METAXML.CPP - Implementation file for your Internet Server
//    MetaXML Extension

#include "stdafx.h"
#include "MetaXML.h"

#define MyDEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

MyDEFINE_GUID(CLSID_MSAdminBase, 0xa9e69610, 0xb80d, 0x11d0, 0xb9, 0xb9, 0x0, 0xa0, 0xc9, 0x22, 0xe7, 0x50);
MyDEFINE_GUID(IID_IMSAdminBase, 0x70b51430, 0xb6ca, 0x11d0, 0xb9, 0xb9, 0x0, 0xa0, 0xc9, 0x22, 0xe7, 0x50);


///////////////////////////////////////////////////////////////////////
// command-parsing map

BEGIN_PARSE_MAP(CMetaXMLExtension, CHttpServer)
	// TODO: insert your ON_PARSE_COMMAND() and 
	// ON_PARSE_COMMAND_PARAMS() here to hook up your commands.
	// For example:

	ON_PARSE_COMMAND(Default, CMetaXMLExtension, ITS_EMPTY)
	DEFAULT_PARSE_COMMAND(Default, CMetaXMLExtension)
END_PARSE_MAP(CMetaXMLExtension)


///////////////////////////////////////////////////////////////////////
// The one and only CMetaXMLExtension object

CMetaXMLExtension theExtension;


///////////////////////////////////////////////////////////////////////
// CMetaXMLExtension implementation

CMetaXMLExtension::CMetaXMLExtension()
{
}

CMetaXMLExtension::~CMetaXMLExtension()
{
}

BOOL CMetaXMLExtension::GetExtensionVersion(HSE_VERSION_INFO* pVer)
{
	// Call default implementation for initialization
	CHttpServer::GetExtensionVersion(pVer);

	// Load description string
	TCHAR sz[HSE_MAX_EXT_DLL_NAME_LEN+1];
	ISAPIVERIFY(::LoadString(AfxGetResourceHandle(),
			IDS_SERVER, sz, HSE_MAX_EXT_DLL_NAME_LEN));
	_tcscpy(pVer->lpszExtensionDesc, sz);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// CMetaXMLExtension command handlers

WCHAR szMDName[METADATA_MAX_NAME_LEN+1];
TCHAR szBuf[METADATA_MAX_NAME_LEN+16];

HRESULT DumpMetabase(CHttpServerContext* pCtxt, IMSAdminBase *pAdmin, METADATA_HANDLE hRoot)
{
	WCHAR szMDName[METADATA_MAX_NAME_LEN+2];
	TCHAR szBuf[METADATA_MAX_NAME_LEN+2];
	HRESULT hr = S_OK;
	DWORD n = 0;
	DWORD len = 0;

	while (hr == S_OK)
	{
		hr = pAdmin->EnumKeys(hRoot, L"", szMDName, n);
		if (FAILED(hr))
		{
			if (HRESULT_CODE(hr) != ERROR_NO_MORE_ITEMS)
			{
				*pCtxt << _T("EnumKeys failed ") << HRtoString(hr);
				switch (HRESULT_CODE(hr))
				{
				case ERROR_ACCESS_DENIED:
					*pCtxt << _T("ERROR_ACCESS_DENIED");
					break;
				case ERROR_INVALID_PARAMETER:
					*pCtxt << _T("ERROR_INVALID_PARAMETER");
					break;
				case ERROR_PATH_NOT_FOUND:
					*pCtxt << _T("ERROR_PATH_NOT_FOUND");
					break;
				}
			}
		}
		else
		{
			METADATA_HANDLE h;
			wsprintf(szBuf, "/%S", szMDName);
			*pCtxt << _T("<div style=\"margin-left:20pt\">\r\n") << szBuf << _T("<br>\r\n");

			if (FAILED(hr = pAdmin->OpenKey(hRoot, szMDName, METADATA_PERMISSION_READ, 20, &h)))
			{
				*pCtxt << _T("Failed to open key") << szBuf << HRtoString(hr);
				return hr;
			}
 
			// Recurse
			if (FAILED(hr = DumpMetabase(pCtxt, pAdmin, h)))
			{
				*pCtxt << _T("Failed to dump key") << szBuf << HRtoString(hr);
				return hr;
			}
			*pCtxt <<  _T("</div>\r\n");
			n++;
		}
	}
	return S_OK;
}


void CMetaXMLExtension::Default(CHttpServerContext* pCtxt)
{
	StartContent(pCtxt);
	WriteTitle(pCtxt);

	TCHAR sz[HSE_MAX_EXT_DLL_NAME_LEN+1];
	ISAPIVERIFY(::LoadString(AfxGetResourceHandle(),
			IDS_SERVER, sz, HSE_MAX_EXT_DLL_NAME_LEN));

	*pCtxt << _T("<h1>");
	*pCtxt << sz;
	*pCtxt << _T("</h1>\r\n");

	// Get a ref to the metabase CLSID_MSAdminBase
	IMSAdminBase *pAdmin = NULL;
	HRESULT hr = CoCreateInstance(CLSID_MSAdminBase, NULL, CLSCTX_SERVER, IID_IMSAdminBase, (void**)&pAdmin);
	if (FAILED(hr))
	{
		*pCtxt << _T("Failed to create an instance of CLSID_MSAdminBase ") << HRtoString(hr);
		EndContent(pCtxt);
		return;
	}
	
	*pCtxt << _T("<h2>Enumerating Metabase</h2>");
	if (FAILED(hr = DumpMetabase(pCtxt, pAdmin, METADATA_MASTER_ROOT_HANDLE)))
	{
		*pCtxt << _T("Failed to dump metabase") << HRtoString(hr);
	}

	if (pAdmin)
	{
		pAdmin->Release();
		pAdmin = NULL;
	}

	EndContent(pCtxt);
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CMetaXMLExtension, CHttpServer)
	//{{AFX_MSG_MAP(CMetaXMLExtension)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0



///////////////////////////////////////////////////////////////////////
// If your extension will not use MFC, you'll need this code to make
// sure the extension objects can find the resource handle for the
// module.  If you convert your extension to not be dependent on MFC,
// remove the comments arounn the following AfxGetResourceHandle()
// and DllMain() functions, as well as the g_hInstance global.

/****

static HINSTANCE g_hInstance;

HINSTANCE AFXISAPI AfxGetResourceHandle()
{
	return g_hInstance;
}

BOOL WINAPI DllMain(HINSTANCE hInst, ULONG ulReason,
					LPVOID lpReserved)
{
	if (ulReason == DLL_PROCESS_ATTACH)
	{
		g_hInstance = hInst;
	}

	return TRUE;
}

****/

LPCTSTR CMetaXMLExtension::GetTitle() const
{
	return _T("XML Metabase Test ISAPI");
}
