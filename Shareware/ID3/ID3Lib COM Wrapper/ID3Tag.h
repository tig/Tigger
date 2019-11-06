// ID3Tag.h : Declaration of the CID3Tag

#ifndef __ID3TAG_H_
#define __ID3TAG_H_

#include "resource.h"       // main symbols
#include "id3_tag.h"

/////////////////////////////////////////////////////////////////////////////
// CID3Tag
class ATL_NO_VTABLE CID3Tag : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CID3Tag, &CLSID_ID3Tag>,
	public ISupportErrorInfo,
	public IDispatchImpl<IID3Tag, &IID_IID3Tag, &LIBID_ID3COM>
{
private:
	ID3_Tag* m_pTag;
	CID3Frames* m_pFrames;

	HRESULT GetTextFrame(WCHAR* wszFrameID, BSTR *pVal);
	HRESULT PutTextFrame(WCHAR* wszFrameID, BSTR newVal);

public:

	CID3Tag();
	~CID3Tag();

	void ReportError(ID3_Error *e)
	{
		TCHAR szDesc[512];
		wsprintf(szDesc, _T("ID3Lib Error %08lX: %s, in file %s, line %l"), e->GetErrorID(), e->GetErrorDesc(), e->GetErrorFile(), e->GetErrorLine());
		AtlReportError(GetObjectCLSID(), szDesc, GUID_NULL, 0);
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ID3TAG)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CID3Tag)
	COM_INTERFACE_ENTRY(IID3Tag)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ID3Tag
public:
	STDMETHOD(get_Comment)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Comment)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Genre)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Genre)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Year)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Year)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Artist)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Artist)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Album)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Album)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Title)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Title)(/*[in]*/ BSTR newVal);
	STDMETHOD(GetVersionInfo)(/*[out,retval]*/ IID3VersInfo** ReturnVal);
	STDMETHOD(GetNumBytes)(/*[in]*/ VARIANT Header, /*[out,retval]*/ long* ReturnVal);
	STDMETHOD(Strip)(/*[in, defaultvalue(False)]*/ VARIANT_BOOL v1Also);
	STDMETHOD(Update)();
	STDMETHOD(Attach)(/*[in]*/ BSTR bstrFilename, /*[in, defaultvalue(-1)]*/ VARIANT_BOOL Create, /*[out, retval]*/ long *ulVal);
	STDMETHOD(Parse)(/*[in]*/ VARIANT Header, /*[inl]*/ VARIANT Buffer);
	STDMETHOD(get_Frames)(/*[out, retval]*/ IID3Frames** ReturnVal);
	STDMETHOD(put_Padding)(/*[in]*/VARIANT_BOOL pad);
	STDMETHOD(get_Padding)(/*[out, retval*/VARIANT_BOOL* pad);
	STDMETHOD(put_Compression)(/*[in]*/VARIANT_BOOL comp);
	STDMETHOD(get_Compression)(/*[out, retval]*/VARIANT_BOOL* comp);
	STDMETHOD(put_ExtendedHeader)(/*[in]*/VARIANT_BOOL ext);
	STDMETHOD(get_ExtendedHeader)(/*[out, retval]*/VARIANT_BOOL* ext);
	STDMETHOD(put_Unsync)(/*[in]*/ VARIANT_BOOL UnSync);
	STDMETHOD(get_Unsync)(/*[in]*/ VARIANT_BOOL *UnSync);
	STDMETHOD(get_HasChanged)(/*[out, retval*/VARIANT_BOOL* changed);
	STDMETHOD(Clear)();
	STDMETHOD(get_Size)(/*[out, retval]*/ long* plsize);
	STDMETHOD(Render)(/*[out, retval]*/ VARIANT* pbuffer);


};

#endif //__ID3TAG_H_
