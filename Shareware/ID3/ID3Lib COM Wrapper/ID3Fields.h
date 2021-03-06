// ID3FIELDS.h: Definition of the CID3Fields class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ID3FIELDS_H__A58AD32F_829E_11D2_B7C7_00A0C9707B03__INCLUDED_)
#define AFX_ID3FIELDS_H__A58AD32F_829E_11D2_B7C7_00A0C9707B03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include "id3_tag.h"

/////////////////////////////////////////////////////////////////////////////
// CID3Fields

class ATL_NO_VTABLE CID3Fields : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IID3Fields, &IID_IID3Fields, &LIBID_ID3COM>, 
	public ISupportErrorInfo,
	public CComCoClass<CID3Fields,&CLSID_ID3Fields>
{
public:
	CID3Fields() : m_pFrame(NULL)
	{}
	
	ID3_Frame* m_pFrame;

	void ReportError(ID3_Error *e)
	{
		TCHAR szDesc[512];
		wsprintf(szDesc, _T("ID3Lib Error %08lX: %s, in file %s, line %l"), e->GetErrorID(), e->GetErrorDesc(), e->GetErrorFile(), e->GetErrorLine());
		AtlReportError(GetObjectCLSID(), szDesc, GUID_NULL, 0);
	}

//DECLARE_REGISTRY_RESOURCEID(IDR_ID3Fields)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CID3Fields)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IID3Fields)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CID3Fields) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IID3Fields

public:
	STDMETHOD(Item)(/* [in] */ VARIANT Item, /* [retval][out] */ IID3Field **Object);
	STDMETHOD(get_Count)(/* [retval][out] */ long *Count);
	STDMETHOD(_NewEnum)(/* [retval][out] */ IUnknown **ppEnum) ;
};

#endif // !defined(AFX_ID3FIELDS_H__A58AD32F_829E_11D2_B7C7_00A0C9707B03__INCLUDED_)
