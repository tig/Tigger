// ID3Frame.h : Declaration of the CID3Frame

#ifndef __ID3FRAME_H_
#define __ID3FRAME_H_

#include "resource.h"       // main symbols
#include "id3_tag.h"

/////////////////////////////////////////////////////////////////////////////
// CID3Frame
class CID3Frame : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CID3Frame, &CLSID_ID3Frame>,
	public ISupportErrorInfo,
	public IDispatchImpl<IID3Frame, &IID_IID3Frame, &LIBID_ID3COM>
{
public:
	ID3_Frame*	m_pFrame;

	CID3Frame() :
		m_pFrame(NULL)
	{
	}

	CID3Frame(ID3_Frame * pFrame) :
		m_pFrame(pFrame)
	{
	}

	~CID3Frame()
	{
		m_pFrame = NULL;
	}

	void ReportError(ID3_Error *e)
	{
		TCHAR szDesc[512];
		wsprintf(szDesc, _T("ID3Lib Error %08lX: %s, in file %s, line %l"), e->GetErrorID(), e->GetErrorDesc(), e->GetErrorFile(), e->GetErrorLine());
		AtlReportError(GetObjectCLSID(), szDesc, GUID_NULL, 0);
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ID3FRAME)
DECLARE_PROTECT_FINAL_CONSTRUCT()
DECLARE_NOT_AGGREGATABLE(CID3Frame)

BEGIN_COM_MAP(CID3Frame)
	COM_INTERFACE_ENTRY(IID3Frame)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IID3Frame
public:
	STDMETHOD(GetFieldByID)(/*[in]*/ID3LibFieldID fieldID, /*[out, retval]*/ IID3Field **ppField);
	void SetFrame(ID3_Frame* pID3_Frame);
	STDMETHOD(get_Fields)(/*[out, retval]*/ IID3Fields** ReturnVal);
	STDMETHOD(get_ID)(/*[out, retval]*/ BSTR* pbstr);
	STDMETHOD(put_ID)(/*[in]*/ BSTR ID);
	STDMETHOD(Clear)();
};

#endif //__ID3FRAME_H_
