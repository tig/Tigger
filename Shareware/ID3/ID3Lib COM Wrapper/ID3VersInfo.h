// ID3VersInfo.h: Definition of the CID3VersInfo class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ID3VERSINFO_H__A58AD324_829E_11D2_B7C7_00A0C9707B03__INCLUDED_)
#define AFX_ID3VERSINFO_H__A58AD324_829E_11D2_B7C7_00A0C9707B03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include "id3_tag.h"

/////////////////////////////////////////////////////////////////////////////
// CID3VersInfo

class CID3VersInfo : 
	public IDispatchImpl<IID3VersInfo, &IID_IID3VersInfo, &LIBID_ID3COM>, 
	public ISupportErrorInfo,
	public CComObjectRoot,
	public CComCoClass<CID3VersInfo,&CLSID_ID3VersInfo>
{
private:

public:

	CID3VersInfo() 
	{
	}

BEGIN_COM_MAP(CID3VersInfo)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IID3VersInfo)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CID3VersInfo) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

//DECLARE_REGISTRY_RESOURCEID(IDR_ID3VersInfo)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IID3VersInfo
	STDMETHOD(get_Name)(/* [retval][out] */ BSTR *ReturnVal);
	STDMETHOD(get_Version)(/* [retval][out] */ long *ReturnVal);
	STDMETHOD(get_Revision)(/* [retval][out] */ long *ReturnVal);

public:

};

#endif // !defined(AFX_ID3VERSINFO_H__A58AD324_829E_11D2_B7C7_00A0C9707B03__INCLUDED_)
