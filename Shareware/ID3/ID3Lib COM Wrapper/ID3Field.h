// ID3Field.h : Declaration of the CID3Field

#ifndef __ID3FIELD_H_
#define __ID3FIELD_H_

#include "resource.h"       // main symbols
#include "id3_tag.h"

/////////////////////////////////////////////////////////////////////////////
// CID3Field
class ATL_NO_VTABLE CID3Field : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CID3Field, &CLSID_ID3Field>,
	public ISupportErrorInfo,
	public IDispatchImpl<IID3Field, &IID_IID3Field, &LIBID_ID3COM>
{
public:
	ID3_Field* m_pField;

	CID3Field() :
		m_pField(NULL)
	{
	}

	void ReportError(ID3_Error *e)
	{
		TCHAR szDesc[512];
		wsprintf(szDesc, _T("ID3Lib Error %08lX: %s, in file %s, line %l"), e->GetErrorID(), e->GetErrorDesc(), e->GetErrorFile(), e->GetErrorLine());
		AtlReportError(GetObjectCLSID(), szDesc, GUID_NULL, 0);
	}

//DECLARE_REGISTRY_RESOURCEID(IDR_ID3FIELD)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CID3Field)
	COM_INTERFACE_ENTRY(IID3Field)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IID3Field
public:
	STDMETHOD(get_TextItems)(/*[out, retval]*/ IID3TextItems** ppITextItems);
	STDMETHOD(get_ID)(/*[out, retval]*/ ID3LibFieldID *pVal);
	// IID3Field
	STDMETHOD(get_Value)(/*[out,retval]*/VARIANT * pVal);
	STDMETHOD(put_Value)(/*[in]*/VARIANT pVal);
	STDMETHOD(get_Size)(/*[out,retval]*/long * pVal);
	STDMETHOD(FromFile)(/*[in]*/BSTR Filename);
	STDMETHOD(ToFile)(/*[in]*/BSTR Filename);
	STDMETHOD(Clear)();
};

#endif //__ID3FIELD_H_
