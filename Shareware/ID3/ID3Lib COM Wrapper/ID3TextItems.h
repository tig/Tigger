// ID3TextItems.h: Definition of the CID3TextItems class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ID3TextItems_H__A58AD32E_829E_11D2_B7C7_00A0C9707B03__INCLUDED_)
#define AFX_ID3TextItems_H__A58AD32E_829E_11D2_B7C7_00A0C9707B03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include "id3_tag.h"

/////////////////////////////////////////////////////////////////////////////
// CID3TextItems

class ATL_NO_VTABLE CID3TextItems : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IID3TextItems, &IID_IID3TextItems, &LIBID_ID3COM>, 
	public ISupportErrorInfo,
	public CComCoClass<CID3TextItems,&CLSID_ID3TextItems>
{
public:
	ID3_Field* m_pField;
	CID3TextItems() : m_pField(NULL)
	{}

	void ReportError(ID3_Error *e)
	{
		TCHAR szDesc[512];
		wsprintf(szDesc, _T("ID3Lib Error %08lX: %s, in file %s, line %l"), e->GetErrorID(), e->GetErrorDesc(), e->GetErrorFile(), e->GetErrorLine());
		AtlReportError(GetObjectCLSID(), szDesc, GUID_NULL, 0);
	}

//DECLARE_REGISTRY_RESOURCEID(IDR_ID3TextItems)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CID3TextItems)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IID3TextItems)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CID3TextItems) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IID3TextItems

public:
	STDMETHOD(Item)(/* [in] */ VARIANT Item, /* [retval][out] */ BSTR* pbstrReturnVal);
	STDMETHOD(get_Count)(/* [retval][out] */ long *Count);
	STDMETHOD(Add)(/* [in] */ BSTR bstr);
	STDMETHOD(_NewEnum)(/* [retval][out] */ IUnknown **ppEnum) ;
};

#endif // !defined(AFX_ID3TextItems_H__A58AD32E_829E_11D2_B7C7_00A0C9707B03__INCLUDED_)
