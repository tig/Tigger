#if !defined(AFX_METAXML_H__258B6FB9_1B9B_11D2_B7AF_00A0C9707B03__INCLUDED_)
#define AFX_METAXML_H__258B6FB9_1B9B_11D2_B7AF_00A0C9707B03__INCLUDED_

// METAXML.H - Header file for your Internet Server
//    MetaXML Extension

#include "resource.h"

class CMetaXMLExtension : public CHttpServer
{
public:
	CMetaXMLExtension();
	~CMetaXMLExtension();

// Overrides
	// ClassWizard generated virtual function overrides
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_VIRTUAL(CMetaXMLExtension)
	public:
	virtual BOOL GetExtensionVersion(HSE_VERSION_INFO* pVer);
	virtual LPCTSTR GetTitle() const;
	//}}AFX_VIRTUAL

	// TODO: Add handlers for your commands here.
	// For example:

	void Default(CHttpServerContext* pCtxt);

	DECLARE_PARSE_MAP()

	//{{AFX_MSG(CMetaXMLExtension)
	//}}AFX_MSG
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METAXML_H__258B6FB9_1B9B_11D2_B7AF_00A0C9707B03__INCLUDED)
