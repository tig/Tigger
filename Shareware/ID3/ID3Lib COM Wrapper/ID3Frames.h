// ID3Frames.h: Definition of the CID3Frames class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ID3FRAMES_H__A58AD32E_829E_11D2_B7C7_00A0C9707B03__INCLUDED_)
#define AFX_ID3FRAMES_H__A58AD32E_829E_11D2_B7C7_00A0C9707B03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include "id3_tag.h"

/////////////////////////////////////////////////////////////////////////////
// CID3Frames

class ATL_NO_VTABLE CID3Frames : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IID3Frames, &IID_IID3Frames, &LIBID_ID3COM>, 
	public ISupportErrorInfo,
	public CComCoClass<CID3Frames,&CLSID_ID3Frames>
{
public:
	ID3_Tag* m_pTag;
	CID3Frames() {}
	void ReportError(ID3_Error *e)
	{
		TCHAR szDesc[512];
		wsprintf(szDesc, _T("ID3Lib Error %08lX: %s, in file %s, line %l"), e->GetErrorID(), e->GetErrorDesc(), e->GetErrorFile(), e->GetErrorLine());
		AtlReportError(GetObjectCLSID(), szDesc, GUID_NULL, 0);
	}

//DECLARE_REGISTRY_RESOURCEID(IDR_ID3Frames)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CID3Frames)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IID3Frames)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CID3Frames) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IID3Frames

public:
	STDMETHOD(Item)(/* [in] */ VARIANT Item,  /* [in, optional]*/ VARIANT FrameID,/*  [in, optional]*/ VARIANT Data, /* [retval][out] */ IID3Frame **Object);
	STDMETHOD(get_Count)(/* [retval][out] */ long *Count);
	STDMETHOD(Add)(/* [in] */ IID3Frame *frame);
	STDMETHOD(Remove)(/* [in] */ IID3Frame* pFrame);
	STDMETHOD(_NewEnum)(/* [retval][out] */ IUnknown **ppEnum) ;
};

#endif // !defined(AFX_ID3FRAMES_H__A58AD32E_829E_11D2_B7C7_00A0C9707B03__INCLUDED_)
